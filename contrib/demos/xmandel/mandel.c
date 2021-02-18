#include <stdio.h>
#include <X11/Intrinsic.h>

extern Widget drawform;
struct zoomd {
    struct zoomd *zp;
    double lx, ux;
    double ly, uy;
};
struct zoomd *zoomp = NULL, *zoompn;
char *malloc();

extern int depth;
long mwidth;
long iter = 64;
long bias = 16;					/* bias into color table */
long wwidth = 0;
long wheight = 0;
#define MIN(a,b)((a) < (b) ? (a) : (b))
#define MAX(a,b)((a) < (b) ? (b) : (a))

void do_mandel (w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    if (zoomp) {	/* reset to beginning by popping zps off stack */
	while (zoompn = zoomp->zp) {
	    free(zoomp);
	    zoomp = zoompn;
	}
    } else {			/* This is first time */
	zoomp = (struct zoomd *) malloc(sizeof (struct zoomd));
	zoomp->zp = NULL;	/* NULL means last in stack - don't pop */
	zoomp->lx = -2.0; zoomp->ux = 1.0;
	zoomp->ly = -1.5; zoomp->uy = 1.5;
    }
    mandel (w);
}

mandel (w)
    Widget w;
{
    double wx, wy, oldwx, oldwy;
    double x, y;
    double incrx, incry;
    long ix, iy;
    static Bool firsttime = True;

    static XImage *xip = NULL;
    static char *dp = NULL;
    Window draw_win;
    XWindowAttributes draw_wattr;
    GC draw_gc;
    Screen *draw_Screen;
    Display *draw_d;
    Visual *draw_v;

    draw_d = XtDisplay(drawform);
    draw_win = XtWindow(drawform);
    draw_Screen = XtScreen(drawform);
    XGetWindowAttributes(draw_d, draw_win, &draw_wattr);
    draw_gc = draw_Screen->default_gc;
    draw_v = draw_Screen->root_visual;

    if (firsttime) {
	XSetWindowAttributes attr;

	attr.backing_store = Always;
	XChangeWindowAttributes (draw_d, draw_win, CWBackingStore, &attr);
	firsttime = True;
    }

    if (wwidth != draw_wattr.width) {
	wwidth = draw_wattr.width;
	mwidth = (depth > 1)? wwidth: (1 + wwidth/8);
	iter = (depth > 1)? iter * 4: iter;
	wheight = draw_wattr.height;
	if (dp) free (dp);
	dp = malloc(mwidth);
	if (dp == NULL) {printf("malloc failed\n"); return; }
	if (xip) XDestroyImage(xip);
	xip = XCreateImage(draw_d, draw_v, depth, ZPixmap, 0, dp, 
		wwidth, 1, 8, mwidth);
	xip->byte_order = MSBFirst;
	xip->bitmap_bit_order = MSBFirst;
    }
    
    incrx = (zoomp->ux - zoomp->lx)/wwidth;
    incry = (zoomp->uy - zoomp->ly)/wheight;
    y = zoomp->uy;

    XClearWindow(draw_d, draw_win);

for (iy = 0; iy < draw_wattr.height; iy++, y-= incry) {
    long i;
    if (depth == 1) for (i = 0; i < mwidth; i++) *(dp + i) = 0;
    x = zoomp->lx;
    for (ix = 0; ix < wwidth; ix++, x+= incrx) {
        wx = x; wy = y;
        for (i = 0; i < iter; i++) {
                oldwx = wx;
                wx = wx * wx - wy * wy + x;
                wy = 2 * oldwx * wy + y;
                if (wx * wx + wy * wy > 4) break;
        }
        if (depth > 1) dp[ix] = i + bias;	/* use iterations as color */
	else if (i == iter) dp[ix/8] |= 1 << (7 - ix%8);
	/* could also test if (i & 2) */
	
    }
    XPutImage(draw_d, draw_win, draw_gc, xip, 0,0, 0, iy, wwidth, 1);
}
}

void do_zoom (w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    Window draw_win;
    XWindowAttributes draw_wattr;
    GC draw_gc;
    Screen *draw_Screen;
    Display *draw_d;
    Visual *draw_v;
    Window Root;
    int S;

    if (zoomp == NULL) return;

    draw_d = XtDisplay(drawform);
    draw_win = XtWindow(drawform);
    draw_Screen = XtScreen(drawform);
    draw_gc = draw_Screen->default_gc;
    S = DefaultScreen(draw_d);
    Root = RootWindow(draw_d, S);

    XSetForeground(draw_d, draw_gc, 255);
    XSetSubwindowMode(draw_d, draw_gc, IncludeInferiors);
    XSetFunction(draw_d, draw_gc, GXxor);

    XSelectInput(draw_d, draw_win, 
	ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    while (1) {
	static int rubberband = 1;
	XEvent report;
	XNextEvent(draw_d, &report);
	switch(report.type) {
	    int winx0,winy0,winx1,winy1,width,height;
	    int x0, y0, x1, y1;
	    double tw, th;
	    case ButtonPress:
		XGrabServer(draw_d);
		x0 = winx0 = report.xbutton.x;
		y0 = winy0 = report.xbutton.y;
		rubberband = 0;
		width = height = 0;
		break;
	    case ButtonRelease:
		winx1 = report.xbutton.x;
		winy1 = report.xbutton.y;
		x0 = MIN(winx0, winx1);
		x1 = MAX(winx0, winx1);
		y0 = MIN(winy0, winy1);
		y1 = MAX(winy0, winy1);
		width = x1 - x0; height = y1 - y0;
	XDrawRectangle(draw_d,draw_win, draw_gc, x0, y0, width, height);
		rubberband = 1;
		XSetFunction(draw_d, draw_gc, GXcopy);
		XUngrabServer(draw_d);
		XFlush(draw_d);
		tw = zoomp->ux - zoomp->lx;
		th = zoomp->uy - zoomp->ly;
		zoompn = (struct zoomd *) malloc(sizeof (struct zoomd));
		zoompn->zp = zoomp;		/* push onto stack */
		zoompn->ux = zoomp->lx + ((double)x1/(double)wwidth) * tw;
		zoompn->lx = zoomp->lx + ((double)x0/(double)wwidth) * tw;
		zoompn->ly = zoomp->uy - ((double)y1/(double)wheight) * th;
		zoompn->uy = zoomp->uy - ((double)y0/(double)wheight) * th;
		zoomp = zoompn;			/* zoomp is current pointer */
		printf("zoom: lx, ux, ly, uy = %f %f %f %f\n", 
			zoomp->lx, zoomp->ux, zoomp->ly, zoomp->uy);
		mandel (w);
		return;			/* call mandel with new width, height */
	    case MotionNotify:
		if (rubberband) break;
		while (XCheckTypedEvent(draw_d, MotionNotify, &report));
		winx1 = report.xbutton.x;
		winy1 = report.xbutton.y;
	XDrawRectangle(draw_d,draw_win, draw_gc, x0, y0, width, height);
		x0 = MIN(winx0, winx1);
		x1 = MAX(winx0, winx1);
		y0 = MIN(winy0, winy1);
		y1 = MAX(winy0, winy1);
		width = x1 - x0; height = y1 - y0;
	XDrawRectangle(draw_d,draw_win, draw_gc, x0, y0, width, height);
		break;
	    default:
		break;
	}
    }
}

void do_unzoom (w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    if (zoomp) {		/* selecting mooz before mandel invalid */
	if (zoompn = zoomp->zp) {	/* last on stack */
	    free(zoomp);
	    zoomp = zoompn;
	}
	mandel (w);
    }				/* consider calling resetmandel() */
}
