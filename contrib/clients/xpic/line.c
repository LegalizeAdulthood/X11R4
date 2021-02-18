#include <X11/Xlib.h>

extern char *XtRealloc(), *XtMalloc();

static XPoint *pbuf = 0;
static int psize = 0;
#define P_INC	128

/*
 * If there are two consecutive identical points in the 'points' array, the X
 * server seems to stop drawing the line there. I think it is a bug. So we go
 * through the array deleting duplicate consecutive points.
 */
drawlines(dpy, d, gc, points, npoints, mode)
Display *dpy;
Drawable d;
GC gc;
XPoint *points;
int npoints;
int mode;
{
	int i, j;
	short lastx, lasty;
	
	if (npoints == 0)
		return;
	if (psize <= npoints) {
		psize = ((npoints + P_INC - 1) / P_INC) * P_INC;
		if (pbuf) {
			pbuf = (XPoint *) XtRealloc((char *) pbuf,
			 (unsigned) (psize * sizeof(XPoint)));
		} else {
			pbuf= (XPoint *) XtMalloc((unsigned) (psize * sizeof(XPoint)));
		}
	}
	pbuf[0] = points[0];
	lastx = pbuf[0].x;
	lasty = pbuf[0].y;
	for(i = j = 1; i < npoints; i++) {
		if ((mode == CoordModeOrigin &&
		 points[i].x == lastx && points[i].y == lasty) ||
		 (mode == CoordModePrevious &&
		 points[i].x == 0 && points[i].y == 0))
			continue;
		lastx = pbuf[j].x = points[i].x;
		lasty = pbuf[j].y = points[i].y;
		j++;
	}
	XDrawLines(dpy, d, gc, pbuf, j, mode);
}
