/* $Header: spline.c,v 1.3 89/07/02 15:59:09 moraes Exp $ */
/* 
 *  Computes a quadratic B-spline (third-order) - this is derived from
 *  the equations given in Rogers and Adams (Sec 5-8) , but for the
 *  specific 3rd order case. The inner loop has been tuned a lot,
 *  particularly the calculation of the three constants. The routine
 *  takes the vertex array v, and the number of vertices nverts, and
 *  returns the spline points in the vertex array flattened and the
 *  number of flattened vertices in nFlat. flatsize is the size of the
 *  buffer which is passed in - th ebuffer 'flattened' must be obtained
 *  by malloc() since the routine will expand it with realloc if there
 *  isn't enough space. That's what InitSpline is for
 */
/* NOTE: Splines are to be drawn CoordModeRelative */
#include <stdio.h>
#include <X11/Xlib.h>

extern void error();
extern char errstring[];

#define VERTEXBLOCK	512				/* 
									 *  The array of flattened vertices is
									 *  allocated space in chunks of
									 *  VERTEXBLOCK
			 						 */

#define DELTA (long) 100
/*
 *  rather than take t from 0 to in steps of DELTA where DELTA < 1, we
 *  take t from 0 to SCALE in steps of DELTA where DELTA is an integer
 */
#define SCALE (long) (10 * DELTA)

void FlattenSpline (v, nVerts, flattened, nFlat, flatSize) 
register XPoint *v; 
int nVerts;
XPoint *flattened[];
int *nFlat;
int *flatSize;
{
	register long t, k1, k2, k3, t_old; 
	register int i; 
	XPoint new;
	XPoint current;		/* Current point */
	register XPoint *tmp;
	extern char *realloc();

	*nFlat = (nVerts - 1) * SCALE / DELTA;
	while (*nFlat >= *flatSize) {
		*flatSize += VERTEXBLOCK;
		if ((*flattened = (XPoint *) realloc((char *) *flattened, 
		 (unsigned) ((*flatSize) * sizeof(XPoint)))) == 0) {
		 	(void) sprintf(errstring,
			 "No more memory for spline buffer - Need %d\n", *flatSize);
			message(errstring);
			return;
		}
	}
	v[0] = v[1];
	v[nVerts] = v[nVerts-1];
	current = v[0];
	tmp = *flattened;
	for (i = 0; i < nVerts-1; i++) {
		for (k3 = 0, t_old = -DELTA, t = 0; t < SCALE; t += DELTA) {
			k1 = k3 - t_old - DELTA + SCALE / 2;
			k2 = SCALE - k3 - k1;
			new.x = (k1*v[i].x + k2*v[i+1].x + k3*v[i+2].x + SCALE/2) / SCALE;
			new.y = (k1*v[i].y + k2*v[i+1].y + k3*v[i+2].y + SCALE/2) / SCALE;
			tmp->x = new.x - current.x;
			tmp->y = new.y - current.y;
			tmp++;
			k3 += DELTA * (DELTA / 2 + t) / SCALE;
			t_old = t;
			current = new;
		}
	}
	(*flattened)->x = v[0].x;
	(*flattened)->y = v[0].y;
}


/* 
 *  InitSpline justs malloc's the initial chunk of memory for flattened
 */
XPoint *InitSpline(flatSize)
int *flatSize;
{
	XPoint *flattened;
	extern char *calloc();
	
	*flatSize = VERTEXBLOCK;
	if ((flattened = (XPoint *) calloc((unsigned) *flatSize, sizeof(XPoint)))
	 == 0)
		return((XPoint *) 0);

	return(flattened);
}
	

