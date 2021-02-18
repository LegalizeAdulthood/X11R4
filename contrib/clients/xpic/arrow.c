/* $Header: arrow.c,v 1.3 89/07/02 15:57:57 moraes Exp $ */
#include <X11/Xlib.h>
#include <math.h>

#define PI 		3.14159265358979323846264338327950288419716939937511
#define ARROWSIZE	10.
#define ARROWANGLE	0.46

#define round(x)	((int) ((x) + 0.5))

static XPoint arrowVerts[3];

/*
 *  Draws an arrowhead on the end of the line going from x1, y1 to x2, y2.
 *  It calculates the direction, and draws two lines ARROWSIZE long at
 *  angles of ARROWANGLE to the line.
 */
Arrow (dpy, w, x1, y1, x2, y2, gc) 
Display *dpy;
Window w;
int x1, y1, x2, y2; 
GC gc;
{
	double dir, dx, dy;

	dx = (double) (x2 - x1);
	dy = (double) (y2 - y1);
	if (dx == 0. && dy == 0.) 
		return;
	dir = atan2(dy, dx) + PI;

	arrowVerts[0].x = x2 + round (ARROWSIZE * cos (dir - ARROWANGLE));
	arrowVerts[0].y = y2 + round (ARROWSIZE * sin (dir - ARROWANGLE));
	arrowVerts[1].x = x2;
	arrowVerts[1].y = y2;
	arrowVerts[2].x = x2 + round (ARROWSIZE * cos (dir + ARROWANGLE));
	arrowVerts[2].y = y2 + round (ARROWSIZE * sin (dir + ARROWANGLE));

	XDrawLines(dpy, w, gc, arrowVerts, 3, CoordModeOrigin);
}

