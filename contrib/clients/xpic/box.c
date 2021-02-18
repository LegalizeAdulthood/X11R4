#include <X11/Xlib.h>
#ifndef MIN
#define MIN(x,y)	(((x) < (y)) ? (x) : (y))
#endif
#ifndef ABS
#define ABS(x)		(((x) >= 0) ? (x) : -(x))
#endif

draw_box(dpy, w, gc, x1, y1, x2, y2)
Display *dpy;
Drawable w;
GC gc;
int x1, y1, x2, y2;
{
	int xx, yy, wid, ht;

	wid = ABS(x2 - x1);
	ht = ABS(y2 - y1);
	xx = MIN(x1, x2);
	yy = MIN(y1, y2);
	
	XDrawRectangle(dpy, w, gc, xx, yy, (unsigned) wid, (unsigned) ht);
}
