/*
 * Copyright 1989 Digital Equipment Corporation
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  Digital Equipment Corporation makes no representations
 * about the suitability of this software for any purpose.  It is
 * provided "as is" without express or implied warranty.
 *
 * DIGITAL EQUIPMENT CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL DIGITAL EQUIPMENT CORPORATION BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Terry Weissman
 *          weissman@decwrl.dec.com
 */

/* rocks.c - handle movement, etc. of the rocks. */

#include "roids.h"



#define NUMTYPES	3	/* How many types of rocks there are. */
#define MAXPOINTS	10	/* Maximum number of points in a rock. */

#define MAXROCKS	256	/* Maximum rocks we'll have at one time. */


static int numtocreate = 4;	/* How many rocks to create each round. */


#define DIAMONDDATA(size)	\
{0, size},			\
{size, 0},			\
{0, -size},			\
{-size, 0},





static int smallshape[][2] = {
    DIAMONDDATA(10)
    };
static int mediumshape[][2] = {
    DIAMONDDATA(15)
    };
static int largeshape[][2] = {
    DIAMONDDATA(20)
    };


static int ***shapes;


static int numpoints[NUMTYPES] = {XtNumber(largeshape), XtNumber(mediumshape),
				      XtNumber(smallshape)};

int radius[NUMTYPES];

extern double sx, sy;		/* Ship location. */

typedef struct _RockRec {
    double x, y;		/* Location. */
    double vx, vy;		/* Velocity. */
    int radius;			/* Radius of this rock. */
    int **shape;		/* Pointer to shape record. */
    int numpoints;		/* Number of points in shape record. */
} RockRec, *Rock;

RockRec rocks[MAXROCKS+2];

int numrocks = 0;		/* Number of rocks existing. */


#define RockNearBox(rock, minx, maxx, miny, maxy)	\
    (rint((rock)->x) - (rock)->radius <= (maxx) &&		\
     rint((rock)->y) - (rock)->radius <= (maxy) &&		\
     rint((rock)->x) + (rock)->radius >= (minx) &&		\
     rint((rock)->y) + (rock)->radius >= (miny))


#define RandomSpeed()	(maxrockspeed * (random() % 2000 - 1000) / 1000.0)

Boolean AreaForShipIsClear()
{
    int minx, maxx, miny, maxy, i;
    Rock rock;
    minx = sx - 5 * shipradius;
    maxx = sx + 5 * shipradius;
    miny = sy - 5 * shipradius;
    maxy = sy + 5 * shipradius;
    for (i=0, rock = rocks ; i<numrocks ; i++, rock++) {
	if (RockNearBox(rock, minx, maxx, miny, maxy))
	    return FALSE;
    }
    return TRUE;
}



static void ConfigureRock(rock, x, y, vx, vy, shape, numpoints, radius)
Rock rock;
double x, y, vx, vy;
int **shape;
int numpoints, radius;
{
    rock->x = x;
    rock->y = y;
    rock->vx = vx;
    rock->vy = vy;
    rock->shape = shape;
    rock->numpoints = numpoints;
    rock->radius = radius;
}



static void PaintRock(rock, gc)
Rock rock;
GC gc;
{
    register int x = rock->x;
    register int y = rock->y;
    register int i;
    for (i=0 ; i<rock->numpoints ; i++)
	AddLine(x + rock->shape[i][0], y + rock->shape[i][1],
		x + rock->shape[i+1][0], y + rock->shape[i+1][1],
		gc);
}



/*
 * Destroy rock #i.  Often, this means creating two rocks in its place.  The
 * point given is used to determine the direction of the destroying influence,
 * and thus which direction the two rocks should go.
 */

static void DestroyRock(i, x, y)
int i, x, y;
{
    int num, value;
    Rock rock = rocks + i;
    double oldx = rock->x;
    double oldy = rock->y;
    PaintRock(rock, backgc);
    value = 50;
    for (num = 0 ; num < NUMTYPES ; num++)
	if (rock->shape == shapes[num]) break;
	else value *= 2;
    if (num >= NUMTYPES)
	Punt("Couldn't find shape in DestroyRock!");
    score += value;
    PaintScore();
    numrocks--;
    rocks[i] = rocks[numrocks];
    num++;
    if (num < NUMTYPES) {
	for (i=numrocks ; i<MIN(MAXROCKS, numrocks+2) ; i++) {
	    ConfigureRock(rocks + i, oldx, oldy, RandomSpeed(), RandomSpeed(),
			  shapes[num], numpoints[num], radius[num]);
	}
	numrocks = MIN(MAXROCKS, numrocks + 2);
    }
}


	
Boolean LineHitsRock(ax, ay, bx, by)
int ax, ay, bx, by;
{
    register Rock rock;
    int i, j, x, y;
    register int minx, miny, maxx, maxy;
    int **shape;
    minx = MIN(ax, bx);
    maxx = MAX(ax, bx);
    miny = MIN(ay, by);
    maxy = MAX(ay, by);
    for (i=0, rock = rocks ; i<numrocks ; i++, rock++) {
	if (RockNearBox(rock, minx, maxx,  miny, maxy)) {
	    shape = rock->shape;
	    x = rock->x;
	    y = rock->y;
	    for (j=0 ; j<rock->numpoints ; j++) {
		if (CheckIntersects(ax, ay, bx, by,
				    shape[j][0] + x, shape[j][1] + y,
				    shape[j+1][0] + x, shape[j+1][1] + y)) {
		    DestroyRock(i, ax, ay);
		    return TRUE;
		}
	    }
	}
    }
    return FALSE;
}
	    
	

/* 
 * Carefully check if the given rock has hit the ship.  If so, destroy the
 * rock (creating new rocks as necessary), and return TRUE.  This does not
 * touch the ship; that is left to the calling routine.
 */

static Boolean CheckForCrash(rock)
Rock rock;
{
    extern int px[], py[];
    int i, j, ax, ay, bx, by;
    int **shape = rock->shape;
    int x = rock->x;
    int y = rock->y;
    if (shipdestroyed) return FALSE;
    for (i=0 ; i < NPOINTS ; i++) {
	ax = px[i];
	ay = py[i];
	bx = px[(i+1) % NPOINTS];
	by = py[(i+1) % NPOINTS];
	for (j=0 ; j<rock->numpoints ; j++) {
	    if (CheckIntersects(ax, ay, bx, by,
				shape[j][0] + x, shape[j][1] + y,
				shape[j+1][0] + x, shape[j+1][1] + y)) {
		DestroyRock(rock - rocks, rint(sx), rint(sy));
		return TRUE;
	    }
	}
    }
    return FALSE;
}


Boolean CheckIfShipHitRocks()
{
    int i, minx, miny, maxx, maxy;
    Rock rock;
    minx = sx - shipradius;
    miny = sy - shipradius;
    maxx = sx + shipradius;
    maxy = sy + shipradius;
    for (i=0, rock = rocks ; i < numrocks ; i++, rock++) {
	if (RockNearBox(rock, minx, maxx, miny, maxy)) {
	    if (CheckForCrash(rock))
		return TRUE;
	}
    }
    return FALSE;
}



void PaintAllRocks()
{
    int i;
    for (i=0 ; i<numrocks ; i++)
	PaintRock(rocks + i, rockgc);
}

void MoveRocks(closure, id)
Opaque closure;
XtIntervalId id;
{
    register Rock rock;
    register int i;
    int minx, miny, maxx, maxy;
    if (closure != (Opaque) MoveRocks) return;
    rocktimerid = XtAddTimeOut(rockwait, MoveRocks, (Opaque) MoveRocks);
    if (numrocks == 0) {
	numrocks = MIN(numtocreate, MAXROCKS);
	numtocreate += 2;
	for (i=0 ; i<numrocks ; i++) {
	    ConfigureRock(rocks + i, 0.0, 0.0,
			  RandomSpeed(), RandomSpeed(),
			  shapes[0], numpoints[0], radius[0]);
	}
    }
    minx = sx - shipradius;
    miny = sy - shipradius;
    maxx = sx + shipradius;
    maxy = sy + shipradius;
    for (i=0 ; i<numrocks ; i++) {
	rock = rocks + i;
	PaintRock(rock, backgc);
	rock->x += rock->vx;
	rock->y += rock->vy;
	rock->x = (rock->x < 0) ? rock->x + gamewidth :
	    ((rock->x > gamewidth) ? rock->x - gamewidth : rock->x);
	rock->y = (rock->y < 0) ? rock->y + gameheight :
	    ((rock->y > gameheight)? rock->y - gameheight: rock->y);
	PaintRock(rock, rockgc);
	if (RockNearBox(rock, minx, maxx, miny, maxy)) {
	    if (CheckForCrash(rock))
		DestroyShip();
	}
    }
}
	
	

static void SetIt(shape, init, num)
int **shape;
int init[][2];
int num;
{
    int i, j;
    for (i=0 ; i<num+1 ; i++) {
	shape[i] = (int *) XtMalloc(sizeof(int) * 2);
	for (j=0 ; j<2 ; j++)
	    shape[i][j] = init[i%num][j];
    }
}

void InitRocks()
{
    int i, j, m;
    rocktimerid = NULL;
    shapes = (int ***) XtMalloc(sizeof(int **) * NUMTYPES);
    for (i=0 ; i<NUMTYPES ; i++) {
	shapes[i] = (int **) XtMalloc(sizeof(int *) * (numpoints[i] + 1));
    }
    SetIt(shapes[0], largeshape, numpoints[0]);
    SetIt(shapes[1], mediumshape, numpoints[1]);
    SetIt(shapes[2], smallshape, numpoints[2]);
    for (i=0 ; i<NUMTYPES ; i++) {
	m = -1;
	for (j=0 ; j<numpoints[i] ; j++) {
	    m = MAX(m, shapes[i][j][0]);
	    m = MAX(m, shapes[i][j][1]);
	}
	radius[i] = m;
    }
}
