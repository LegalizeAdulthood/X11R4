/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	January 1985.
 *	1st revision : March 1988
 *
 *	%W%	%G%
*/
#include "fig.h"
#include "resources.h"
#include "paintop.h"

/****************************************************************************

	The following spline drawing routine is from 

	"An Algorithm for High-Speed Curve Generation" 
	by George Merrill Chaikin,
	Computer Graphics and Image Processing, 3, Academic Press, 
	1974, 346-349.

	and

	"On Chaikin's Algorithm" by R. F. Riesenfeld,
	Computer Graphics and Image Processing, 4, Academic Press, 
	1975, 304-310.

*****************************************************************************/

#define		round(x)	((int) (x + .5))
#define		half(z1, z2)	((z1+z2)/2.0)
#define		THRESHOLD	5

/* iterative version */
quadratic_spline(a1, b1, a2, b2, a3, b3, a4, b4, op)
float	a1, b1, a2, b2, a3, b3, a4, b4;
int	op;
{
	float	x1, y1, x2, y2, x3, y3, x4, y4;
	float	xmid, ymid;

	clear_stack();
	push(a1, b1, a2, b2, a3, b3, a4, b4);

	while(pop(&x1, &y1, &x2, &y2, &x3, &y3, &x4, &y4)) {
	    xmid = half(x2, x3);
	    ymid = half(y2, y3);
	    if (fabs(x1 - xmid) < THRESHOLD && fabs(y1 - ymid) < THRESHOLD) {
		pw_vector(canvas_pixwin, round(x1), round(y1), 
			round(xmid), round(ymid), op, 1);
		}
	    else {
		push(x1, y1, half(x1, x2), half(y1, y2),
			half(x2, xmid), half(y2, ymid), xmid, ymid);
		}

	    if (fabs(xmid - x4) < THRESHOLD && fabs(ymid - y4) < THRESHOLD) {
		pw_vector(canvas_pixwin, round(xmid), round(ymid), 
			round(x4), round(y4), op, 1);
		}
	    else {
		push(xmid, ymid, half(xmid, x3), half(ymid, y3),
			half(x3, x4), half(y3, y4), x4, y4);
		}
	    }
	}

bezier_spline(a0, b0, a1, b1, a2, b2, a3, b3, op)
float	a0, b0, a1, b1, a2, b2, a3, b3;
int	op;
{
	float	x0, y0, x1, y1, x2, y2, x3, y3;
	float	sx1, sy1, sx2, sy2, tx, ty, tx1, ty1, tx2, ty2, xmid, ymid;

	clear_stack();
	push(a0, b0, a1, b1, a2, b2, a3, b3);

	while(pop(&x0, &y0, &x1, &y1, &x2, &y2, &x3, &y3)) {
	    if (fabs(x0 - x3) < THRESHOLD && fabs(y0 - y3) < THRESHOLD) {
		pw_vector(canvas_pixwin, round(x0), round(y0), 
			round(x3), round(y3), op, 1);
		}
	    else {
		tx = half(x1, x2);	ty = half(y1, y2);
		sx1 = half(x0, x1);	sy1 = half(y0, y1);
		sx2 = half(sx1, tx);	sy2 = half(sy1, ty);
		tx2 = half(x2, x3);	ty2 = half(y2, y3);
		tx1 = half(tx2, tx);	ty1 = half(ty2, ty);
		xmid = half(sx2, tx1);	ymid = half(sy2, ty1);

		push(x0, y0, sx1, sy1, sx2, sy2, xmid, ymid);
		push(xmid, ymid, tx1, ty1, tx2, ty2, x3, y3);
		}
	    }
	}
