/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	January 1985.
 *	1st revision : Aug 1985.
 *
 *	%W%	%G%
*/
#include "fig.h"
#include "resources.h"
#include "paintop.h"

#define		round(a)	((int)((a) + .5))

/*	This routine plot two dimensional curve defined by a second degree
	polynomial of the form :
			    2    2
		f(x, y) = ax + by + g = 0

	(x0,y0) is the starting point as well as ending point of the curve.
	The curve will translate with the offset xoff and yoff.

	This algorithm is derived from the eight point algorithm in :
	"An Improved Algorithm for the generation of Nonparametric Curves"
	by Bernard W. Jordan, William J. Lennon and Barry D. Holm, IEEE
	Transaction on Computers Vol C-22, No. 12 December 1973.
*/

curve(xstart, ystart, xend, yend, direction, a, b, xoff, yoff, val)
int	xstart, ystart, xend, yend, a, b, xoff, yoff;
int	direction, val;
{
	int	dfx, dfy, dfxx, dfyy;
	int	falpha, fx, fy, fxy, absfx, absfy, absfxy;
	int	margin, test_succeed, x, y, deltax, deltay, inc, dec;
	
	if (a == 0 || b == 0) return;
	x    = xstart;
	y    = ystart;
	dfx  = 2 * a * xstart;
	dfy  = 2 * b * ystart;
	dfxx = 2 * a;
	dfyy = 2 * b;

	falpha = 0;
	if (direction) {
	    inc = 1;  dec = -1;
	    }
	else {
	    inc = -1;  dec = 1;
	    }
	if (xstart == xend && ystart == yend) {
	    test_succeed = margin = 1;
	    }
	else {
	    test_succeed = margin = 3; 
	    }

	do {
	    if (dfy < 0) deltax = inc; else deltax = dec;
	    if (dfx < 0) deltay = dec; else deltay = inc;
	    fx  = falpha + dfx * deltax + a;
	    fy  = falpha + dfy * deltay + b;
	    fxy = fx + fy - falpha;
	    absfx  = abs(fx); absfy  = abs(fy); absfxy = abs(fxy);

	    if ((absfxy <= absfx) && (absfxy <= absfy))
		falpha = fxy;
	    else if (absfy <= absfx) {
		deltax = 0; falpha = fy;
		}
	    else {
		deltay = 0; falpha = fx;
		}
	    x = x + deltax;  y = y + deltay;
	    pw_put(canvas_pixwin, xoff+x, yoff-y, val);
	    dfx = dfx + dfxx * deltax;
	    dfy = dfy + dfyy * deltay;
	    if (abs(x - xend) < margin && abs(y - yend) < margin) 
		test_succeed--;
	    } while (test_succeed);
	}
