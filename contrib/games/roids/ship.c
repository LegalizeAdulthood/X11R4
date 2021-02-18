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

/* ship.c - handle movement, etc. of the ship. */

#include "roids.h"
#include "explode.bit"

#define MAXDIR 256


static int shipx[MAXDIR][NPOINTS];
static int shipy[MAXDIR][NPOINTS];
static double velx[MAXDIR];
static double vely[MAXDIR];
int px[NPOINTS], py[NPOINTS];	/* What points describe the ships outline */
				/* now. */

static int orientation;
double sx, sy;			/* The ship's current location (where the */
				/* center of it is, in units of pixels. */
static double vx, vy;		/* The ship's current velocity (in units */
				/* of pixels per repaint.) */
static int ex, ey;		/* Where we're painting an explosion. */
static Boolean showingexplosion = FALSE;

static int dturn;
static int goalx = -1, goaly;
static Boolean thrust;


void InitShip()
{
    int i, d;
    double angle;
    shipdestroyed = TRUE;
    showingexplosion = FALSE;
    shiptimerid = NULL;
    orientation = 0;
    vx = vy = 0.0;
    dturn = 0;
    thrust = FALSE;
    d = shipradius;
    if (d > shipradius) d = shipradius;
    for (i=0 ; i < numdir ; i++) {
	angle = i * 2 * M_PI / (double) numdir;
	shipx[i][0] = (int) rint(d * cos(angle + M_PI_2));
	shipy[i][0] = (int) rint(-d * sin(angle + M_PI_2));
	shipx[i][1] = (int) rint(d * cos(angle + M_PI + M_PI_4));
	shipy[i][1] = (int) rint(-d * sin(angle + M_PI + M_PI_4));
	shipx[i][2] = (int) rint(d * cos(angle - M_PI_4));
	shipy[i][2] = (int) rint(-d * sin(angle - M_PI_4));
	velx[i] = cos(angle + M_PI_2);
	vely[i] = -sin(angle + M_PI_2);
    }
}



void PaintShip(gc)
GC gc;
{
    static double lx = -100.0, ly = -100.0;
    static int lorient = -1;
    int i, j, x, y;
    BeginLines();
    if (sx != lx || sy != ly || orientation != lorient) {
	lx = sx;
	ly = sy;
	lorient = orientation;
	for (i=0 ; i<NPOINTS ; i++) {
	    px[i] = sx + shipx[orientation][i];
	    py[i] = sy + shipy[orientation][i];
	}
    }
    for (i=0 ; i<NPOINTS ; i++) {
	j = (i + 1) % NPOINTS;
	AddLine(px[i], py[i], px[j], py[j], gc);
    }
    EndLines();
}


void DestroyShip()
{
    XImage ximage;
    PaintShip(backgc);
    shipdestroyed = TRUE;
    ex = sx - explode_width/2;
    ey = sy - explode_height/2;
    ximage.height = explode_height;
    ximage.width = explode_width;
    ximage.xoffset = 0;
    ximage.format = XYBitmap;
    ximage.data = (char *)explode_bits;
    ximage.byte_order = LSBFirst;
    ximage.bitmap_unit = 16; 
    ximage.bitmap_bit_order = LSBFirst;
    ximage.bitmap_pad = 16;
    ximage.bytes_per_line = (ximage.width+15)/16 * 2;
    ximage.depth = 1;
    XPutImage(dpy, gamewindow, shipgc, &ximage,
	      0, 0, ex, ey, explode_width, explode_height);
    showingexplosion = TRUE;
    if (shiptimerid) XtRemoveTimeOut(shiptimerid);
    shiptimerid = XtAddTimeOut(1000, MoveShip, (Opaque) MoveShip);
}



void MoveShip(closure, id)
Opaque closure;
XtIntervalId id;
{
    int i, j, newx, newy;
    if (closure != (Opaque) MoveShip) return;
    if (shipdestroyed) {
	if (showingexplosion) {
	    XClearArea(dpy, gamewindow, ex, ey, explode_width, explode_height,
		       FALSE);
	    showingexplosion = FALSE;
	    shiptimerid = XtAddTimeOut(2000, MoveShip, (Opaque) MoveShip);
	    return;
	}
	sx = gamewidth / 2.0;
	sy = gameheight / 2.0;
	if (!AreaForShipIsClear()) {
	    shiptimerid = XtAddTimeOut(100, MoveShip, (Opaque) MoveShip);
	    return;
	}
	if (shipsleft <= 0) Quit();
	shipsleft--;
	PaintScore();
	shipdestroyed = FALSE;
	vx = vy = 0.0;
    }
    shiptimerid = XtAddTimeOut(shipwait, MoveShip, (Opaque) MoveShip);
    PaintShip(backgc);
    if (goalx >= 0) dturn = FindGoalTurn();
    if (dturn) {
	orientation += dturn;
	if (orientation < 0) orientation = numdir - 1;
	else if (orientation >= numdir) orientation = 0;
    }
    if (thrust) {
	vx += velx[orientation] * accper;
	vy += vely[orientation] * accper;
	vx = (vx > maxv) ? maxv : ((vx < -maxv) ? -maxv : vx);
	vy = (vy > maxv) ? maxv : ((vy < -maxv) ? -maxv : vy);
    }
    sx += vx;
    sy += vy;
    sx = (sx < 0) ? sx + gamewidth : ((sx > gamewidth) ? sx - gamewidth : sx);
    sy = (sy < 0) ? sy + gameheight: ((sy > gameheight)? sy - gameheight: sy);

    PaintShip(shipgc);

    if (CheckIfShipHitRocks())
	DestroyShip();
}



void ThrustOn()
{
    thrust = TRUE;
}


void ThrustOff()
{
    thrust = FALSE;
}


void RotateLeft()
{
    dturn = 1;
    goalx = -1;
}


void RotateRight()
{
    dturn = -1;
    goaly = -1;
}


void RotateOff()
{
    dturn = 0;
}


int FindGoalTurn()
{
    double dx, dy;
    double theta;
    int dgoal;
    dx = goalx - sx;
    dy = goaly - sy;
    if (rint(dy) == 0 && rint(dy) == 0)
	return 0;
    theta = atan2(-dy, dx);
    theta += 3 * M_PI_2;
    dgoal = (theta * numdir) / (2 * M_PI);
    dgoal = dgoal % numdir;
    if (dgoal == orientation) return 0;
    else if (orientation < dgoal) {
	if (dgoal - orientation <= numdir / 2)
	    return 1;
	else return -1;
    } else {
	if (orientation - dgoal <= numdir / 2)
	    return -1;
	else return 1;
    }
}

void RotateToPoint(w, event, params, num_params)
Widget w;
XEvent *event;
char **params;
int *num_params;
{
    goalx = event->xbutton.x;
    goaly = event->xbutton.y;
}

void RotateMouseMoved(w, event, params, num_params)
Widget w;
XEvent *event;
char **params;
int *num_params;
{
    goalx = event->xbutton.x;
    goaly = event->xbutton.y;
}

    
void StopRotateToPoint()
{
    goalx = -1;
    dturn = 0;
}


void Fire()
{
    double dx, dy;
    if (!shipdestroyed) {
	dx = vx + velx[orientation] * shotacc;
	dy = vy + vely[orientation] * shotacc;
	dx = dx * shotwait / shipwait;
	dy = dy * shotwait / shipwait;
	AddShot(sx, sy, dx, dy);
    }
}
