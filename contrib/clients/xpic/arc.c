#ifndef FASTARCS

#include <X11/Xlib.h>
#include <math.h>

/*
 * The R3 miarc.c code, which is used to draw arcs in the X server is very
 * slow, on Suns and uVaxen at least. (It uses iterative solution of
 * transcendentals to try to be faithful to the protocol) Instead, we use a
 * fast approximation that computes upto MAXPOINTS points on an arc and draws
 * a lines between them. Two calls each to sin(), cos(), and some floating
 * point math (could probably be done in fixed point or integer if you don't
 * have a math chip). These arcs don't follow the X protocol precisely, but
 * look reasonable.
 */
/* Author: D. A. Cahlander, <dac@earth.cray.com> 89/02/21 */
/*
 *  Last Modified: Mark Moraes, <moraes@csri.toronto.edu> 89/03/23 to
 *  generalize for elliptical arc drawing as a plug compatible
 *  substitute for Xlib XDrawArc()/XFillArc()/XDrawArcs()/XFillArcs()
 */

/* Basic idea: By representing a ellipse as:

        x = a * cos(t)
        y = b * sin(t)

   t can be divided into a small number of lines and represent the arc to
   "display precision" with 15-20 lines.  (More than 20 if it was a large
   arc.)

        let dt be the delta angle
        dc = cos(dt)
        ds = sin(dt)

        x1     dc  -ds    x0
            =          x
        y1     ds   dc    y0

   or
        x(i+1) = dc*x(i) - ds*y(i)
        y(i+1) = ds*x(i) + dc*y(i)

   with the actual (x,y) being:

        x = xc + x(i)
        y = yc + y(i)
 */


#define MAXPOINTS 99

static drawarc(dpy, d, gc, x1, y1, width, height, angle1, angle2, fill)
Display *dpy;
Drawable d;
GC gc;
int x1, y1;
unsigned int width, height;
int angle1, angle2;
int fill;
{
	XPoint	points[MAXPOINTS + 2];
	int	npoints;
	int	i;
	double	xc, yc;
	double	ang;
#ifdef ELLIPSE_WITHIN_BBOX
	double	delta;
#endif /* ELLIPSE_WITHIN_BBOX */
	double	t, xt, yt, dc, ds;
	double	xr, yr;

#define DEG_180  (180*64)
#define DEG_360  (360*64)
#define DEG_720  (720*64)

	if (angle2 > DEG_360)
		angle2 = DEG_360;

	/*
	 * compute number of points needed for "good" display precision 
	 */
	npoints = M_PI * sqrt((double) (width + height) / 2.0);
	npoints = npoints < 8 ? 8 : npoints;
	npoints = (npoints * ((angle2 < 0) ? -angle2 : angle2) + DEG_720 - 1)
	 / DEG_360;
	npoints = npoints > MAXPOINTS ? MAXPOINTS : npoints;
	/* angle between polygon points */
	ang = angle2 * M_PI / (DEG_180 * (npoints - 1));
	dc = cos(ang);
	ds = - sin(ang);
	
	if (angle1 == 0) {
		xt = 1.0;
		yt = 0.0;
	} else {
		ang = angle1 * M_PI / DEG_180;	/* start angle vector */
		xt = cos(ang);
		yt = - sin(ang);
	}

#ifdef ELLIPSE_WITHIN_BBOX
	/*
	 * Warning - possibly non-portable code. Uses internal details of GC
	 * struct
	 */
	delta = gc->values.line_width / 2.0;
#endif /* ELLIPSE_WITHIN_BBOX */
	
	/* radius of arc */
	xr = width / 2.0;
	yr = height / 2.0;

	/* center of arc */
	xc = x1 + xr;
	yc = y1 + yr;

#ifdef ELLIPSE_WITHIN_BBOX
	xr = (xr > delta) ? xr - delta : delta;
	yr = (yr > delta) ? yr - delta : delta;
#endif /* ELLIPSE_WITHIN_BBOX */

	i = 0;
	
	/*
	 * Warning - possibly non-portable code. Uses internal details of GC
	 * struct
	 */
	if (fill && angle2 != DEG_360 && gc->values.arc_mode == ArcPieSlice) {
		points[i].x = xc;
		points[i].y = yc;
		i++;
	}
	
	for (; i < npoints; i ++) {	/* compute polygon points */
		points[i].x = xc + xr * xt + (xt >= 0. ? .5 : -.5);
		points[i].y = yc + yr * yt + (yt >= 0. ? .5 : -.5);
		t = dc * xt - ds * yt;		/* rotate vector */
		yt = ds * xt + dc * yt;
		xt = t;
	}
	
	if (fill)
		XFillPolygon(dpy, d, gc, points, npoints, Convex,
		 CoordModeOrigin);
	else
		XDrawLines(dpy, d, gc, points, npoints, CoordModeOrigin);
#undef DEG_180
#undef DEG_360
#undef DEG_720
}

XDrawArc(dpy, d, gc, x1, y1, width, height, angle1, angle2)
Display *dpy;
Drawable d;
GC gc;
int x1, y1;
unsigned int width, height;
int angle1, angle2;
{
	drawarc(dpy, d, gc, x1, y1, width, height, angle1, angle2, 0);
}

XDrawArcs(dpy, d, gc, arcs, n_arcs)
Display *dpy;
Drawable d;
GC gc;
XArc *arcs;
int n_arcs;
{
	register XArc *arcp;

	for(arcp = arcs; n_arcs > 0; n_arcs--)
		drawarc(dpy, d, gc, (int) (arcp->x), (int) (arcp->y),
		 (unsigned int) (arcp->width), (unsigned int) (arcp->height),
		 (int) (arcp->angle1), (int) (arcp->angle2), 0);
}

XFillArc(dpy, d, gc, x1, y1, width, height, angle1, angle2)
Display *dpy;
Drawable d;
GC gc;
int x1, y1;
unsigned int width, height;
int angle1, angle2;
{
	drawarc(dpy, d, gc, x1, y1, width, height, angle1, angle2, 1);
}

XFillArcs(dpy, d, gc, arcs, n_arcs)
Display *dpy;
Drawable d;
GC gc;
XArc *arcs;
int n_arcs;
{
	register XArc *arcp;

	for(arcp = arcs; n_arcs > 0; n_arcs--)
		drawarc(dpy, d, gc, (int) (arcp->x), (int) (arcp->y),
		 (unsigned int) (arcp->width), (unsigned int) (arcp->height),
		 (int) (arcp->angle1), (int) (arcp->angle2), 1);
}
#endif /* FASTARCS */
