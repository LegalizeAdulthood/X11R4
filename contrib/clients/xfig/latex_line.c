/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	January 1985.
 *	1st revision : Aug 1985.
 *
 *	%W%	%G%
*/
#include "math.h"

int  pgcd(a,b)
    int a, b;
/*
 *  compute greatest common divisor, assuming 0 < a <= b
 */
{
    b = b % a;
    return (b)? gcd(b, a): a;
}

int  gcd(a, b)
    int a, b;
/*
 *  compute greatest common divisor
 */
{
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    return (a <= b)? pgcd(a, b): pgcd(b, a);
}


int  lcm(a, b)
    int a, b;
/*
 *  compute least common multiple
 */
{
    return abs(a*b)/gcd(a,b);
}


double rad2deg = 57.295779513082320877;

struct angle_table {
    int    x, y;
    double angle;
};

struct angle_table line_angles[25] =
  { {0, 1, 90.0}, 
    {1, 0,  0.0},
    {1, 1, 45.0}, 
    {1, 2, 63.434948822922010648},
    {1, 3, 71.565051177077989351},
    {1, 4, 75.963756532073521417},
    {1, 5, 78.690067525979786913},
    {1, 6, 80.537677791974382609},
    {2, 1, 26.565051177077989351},
    {2, 3, 56.309932474020213086},
    {2, 5, 68.198590513648188229}, 
    {3, 1, 18.434948822922010648},
    {3, 2, 33.690067525979786913},
    {3, 4, 53.130102354155978703},
    {3, 5, 59.036243467926478582},
    {4, 1, 14.036243467926478588},
    {4, 3, 36.869897645844021297},
    {4, 5, 51.340191745909909396},
    {5, 1, 11.309932474020213086},
    {5, 2, 21.801409486351811770},
    {5, 3, 30.963756532073521417},
    {5, 4, 38.659808254090090604},
    {5, 6, 50.194428907734805993},
    {6, 1, 9.4623222080256173906},
    {6, 5, 39.805571092265194006}
  };

struct angle_table arrow_angles[13] =
  { {0, 1, 90.0}, 
    {1, 0,  0.0},
    {1, 1, 45.0}, 
    {1, 2, 63.434948822922010648},
    {1, 3, 71.565051177077989351},
    {1, 4, 75.963756532073521417},
    {2, 1, 26.565051177077989351},
    {2, 3, 56.309932474020213086},
    {3, 1, 18.434948822922010648},
    {3, 2, 33.690067525979786913},
    {3, 4, 53.130102354155978703},
    {4, 1, 14.036243467926478588},
    {4, 3, 36.869897645844021297},
  };

get_slope(dx, dy, sxp, syp, arrow)
    int  dx, dy, *sxp, *syp, arrow;
{
    double angle;
    int    i, s, max;
    double d, d1;
    struct angle_table *st;

    if (dx == 0) {
	*sxp = 0;
	*syp = (dy < 0)? -1: 1;
	return;
    }
    angle = atan((double)abs(dy) / (double)abs(dx)) * rad2deg;
    if (arrow) {
	st = arrow_angles;
	max = 13;
    } else {
	st = line_angles;
	max = 25;
    }
    s = 0;
    d = 9.9e9;
    for (i = 0; i < max; i++) {
	d1 = fabs(angle - st[i].angle);
	if (d1 < d) {
	    s = i;
	    d = d1;
	} 
    }
    *sxp = st[s].x;
    if (dx < 0) *sxp = -*sxp;
    *syp = st[s].y;
    if (dy < 0) *syp = -*syp;
}

latex_endpoint(x1, y1, x2, y2, xout, yout, arrow, magnet)
    int  x1, y1, x2, y2;
    int  *xout, *yout;
    int  arrow, magnet;
{
    int    dx, dy, sx, sy, ds, dsx, dsy;

    dx = x2-x1;
    dy = y2-y1;
    get_slope(dx, dy, &sx, &sy, arrow);
    if (abs(sx) >= abs(sy)) {
	ds = lcm(sx, magnet*gcd(sx,magnet));
	dsx = (2*abs(dx)/ds + 1)/2;
	dsx = (dx >= 0)? dsx*ds: -dsx*ds;
	*xout = x1 + dsx;
	*yout = y1 + dsx*sy/sx;
    } else {
	ds = lcm(sy, magnet*gcd(sy,magnet));
	dsy = (2*abs(dy)/ds + 1)/2;
	dsy = (dy >= 0)? dsy*ds: -dsy*ds;
	*yout = y1 + dsy;
	*xout = x1 + dsy*sx/sy;
    }
}
