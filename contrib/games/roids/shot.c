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

/* shot.c - handle movement, etc. of the shots. */

#include "roids.h"

#define MAXSHOTS	20

typedef struct _ShotRec {
    double x, y;		/* Location of this shot. */
    double vx, vy;		/* Velocity of this shot. */
    int count;			/* Countdown of life remaining to this shot. */
} ShotRec, *Shot;

ShotRec shots[MAXSHOTS];

static int numshots;		/* Number of shots currently flying. */

static int shotcount;		/* Initial value for life countdown. */

static void PaintShot(shot, gc)
Shot shot;
GC gc;
{
    AddLine(rint(shot->x), rint(shot->y),
	    rint(shot->x - shot->vx), rint(shot->y - shot->vy), gc);
}


void PaintAllShots()
{
    int i;
    for (i=0 ; i<numshots ; i++)
	PaintShot(shots + i, shotgc);
}


static void DestroyShot(i)
int i;
{
    PaintShot(shots + i, backgc);
    numshots--;
    shots[i] = shots[numshots];
}


void MoveShots(closure, id)
Opaque closure;
XtIntervalId id;
{
    int i;
    double newx, newy;
    Shot shot;
    if (closure != (Opaque) MoveShots) return;
    if (numshots > 0)
        shottimerid = XtAddTimeOut(shotwait, MoveShots, (Opaque) MoveShots);
    else
	shottimerid = NULL;
    for (i=0 ; i<numshots ; i++) {
	shot = shots + i;
	newx = shot->x + shot->vx;
	newy = shot->y + shot->vy;
	if (LineHitsRock(rint(shot->x - shot->vx), rint(shot->y - shot->vy),
			 rint(newx), rint(newy))
	    || --(shot->count) == 0) {
	    DestroyShot(i);
	    i--;		/* Ensures we don't skip moving a shot. */
	} else {
	    PaintShot(shot, backgc);
	    shot->x = newx < 0 ? newx + gamewidth :
		(newx > gamewidth ? newx - gamewidth : newx);
	    shot->y = newy < 0 ? newy + gameheight :
		(newy > gameheight ? newy - gameheight : newy);
	    PaintShot(shot, shotgc);
	}
    }
}



void AddShot(x, y, vx, vy)
double x, y, vx, vy;
{
    Shot shot;
    if (numshots >= maxshots) return;
    shot = shots + numshots;
    numshots++;
    shot->x = x;
    shot->y = y;
    shot->vx = vx;
    shot->vy = vy;
    shot->count = shotcount;
    if (shottimerid == NULL)
        shottimerid = XtAddTimeOut(shotwait, MoveShots, (Opaque) MoveShots);
}



InitShot()
{
    shottimerid = NULL;
    shotcount = shotduration / shotwait;
    numshots = 0;
}
