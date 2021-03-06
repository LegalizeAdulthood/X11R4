/*****************************************************************************
Copyright 1988, 1989 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************************/

#include "x11perf.h"
#include "bitmaps.h"

#define NUM_POINTS 3   /* 3 points to a triangle */
static XPoint *points;
static GC     pgc;

extern double sin();
extern double cos();
extern double sqrt();
#define PI  3.14159265357989

int InitTriangles(xp, p, reps)
    XParms  xp;
    Parms   p;
    int     reps;
{
    int     i, j, numPoints;
    int     rows;
    int     x, y;
    int     size, iradius;
    double  phi, radius, delta, phi2;
    XPoint  *curPoint;

    pgc = xp->fggc;

    size = p->special;
    phi = 0.0;
    radius = ((double) size) * sqrt(3.0)/2.0;
    iradius = (int) (radius + 0.5);
    delta = 2.0 * PI / ((double) NUM_POINTS);

    numPoints = (p->objects) * NUM_POINTS;  
    points = (XPoint *)malloc(numPoints * sizeof(XPoint));
    curPoint = points;
    x = iradius;
    y = iradius;
    rows = 0;

    for (i = 0; i != p->objects; i++) {
	for (j = 0; j != NUM_POINTS; j++) {
	    phi2 = phi + ((double) j) * delta;
	    curPoint->x = (int) ((double)x + (radius * cos(phi2)) + 0.5);
	    curPoint->y = (int) ((double)y + (radius * sin(phi2)) + 0.5);
	    curPoint++;
	}
	phi += delta/10.0;
	y += 2 * iradius;
	rows++;
	if (y + iradius > HEIGHT || rows == MAXROWS) {
	    rows = 0;
	    y = iradius;
	    x += 2 * iradius;
	    if (x + iradius > WIDTH) {
		x = iradius;
	    }
	}
    }

    SetFillStyle(xp, p);

    return reps;
}

void DoTriangles(xp, p, reps)
    XParms  xp;
    Parms   p;
    int     reps;
{
    int     i, j;
    XPoint  *curPoint;

    for (i = 0; i != reps; i++) {
        curPoint = points;
        for (j = 0; j != p->objects; j++) {
            XFillPolygon(xp->d, xp->w, pgc, curPoint, NUM_POINTS, Convex, 
			 CoordModeOrigin);
            curPoint += NUM_POINTS;
	}
        if (pgc == xp->bggc)
            pgc = xp->fggc;
        else
            pgc = xp->bggc;
    }
}

void EndTriangles(xp, p)
    XParms  xp;
    Parms   p;
{
    free(points);
}

