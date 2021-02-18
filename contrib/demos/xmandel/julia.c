#include <stdio.h>
#include <X11/Intrinsic.h>

extern Widget drawform;
struct zoomd {
    struct zoomd *zp;
    double lx, ux;
    double ly, uy;
};
extern int depth;
extern struct zoomd *zoomp;

void do_julia (w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    double a,b;
    double wx, wy, oldwx, oldwy;
    double sx, x, y;
    double incrx, incry;
    long ix, iy;
    static long width = 0;
    static long mwidth = 0;
    static long iter = 64;
    static long bias = 16;

    static XImage *xip1 = NULL, *xip2 = NULL;
    static char *dp1 = NULL, *dp2 = NULL;
    char *malloc();
    char ab[80];
    Window draw_win;
    static Window s_win = NULL;
    XWindowAttributes draw_wattr;
    GC draw_gc;
    Screen *draw_Screen;
    Display *draw_d;
    Visual *draw_v;
    Window Root;
    int S;				/* Screen */
    static Bool firsttime = True;

    if (zoomp == NULL) return;

    draw_d = XtDisplay(drawform);
    draw_win = XtWindow(drawform);
    draw_Screen = XtScreen(drawform);
    draw_gc = draw_Screen->default_gc;
    draw_v = draw_Screen->root_visual;
    XGetWindowAttributes(draw_d, draw_win, &draw_wattr);
    if (firsttime) {
	XSetWindowAttributes attr;

	attr.backing_store = Always;
	XChangeWindowAttributes (draw_d, draw_win, CWBackingStore, &attr);
	firsttime = True;
    }

    XSelectInput(draw_d, draw_win, ButtonPressMask | ButtonReleaseMask);
    while (1) {
	int winx, winy;
	XEvent report;
	XNextEvent(draw_d, &report);
	if (report.type == ButtonRelease) {
	    a = ( (double)report.xbutton.x/ (double)draw_wattr.width);
	    b = ( (double)report.xbutton.y/ (double)draw_wattr.height);
	    a = (a * (zoomp->ux - zoomp->lx)) + zoomp->lx;
	    b = zoomp->uy - (b * (zoomp->uy - zoomp->ly));
	    printf("julia: a = %f, b = %f\n", a, b);
	    break;
	}
	continue;
    }
    S = DefaultScreen(draw_d);
    Root = RootWindow(draw_d, S);
    if (s_win == NULL) {
	s_win = XCreateSimpleWindow(draw_d, Root, 0, 0, 200, 200, 1, 1, 0);
	XMapWindow(draw_d, s_win);
	XSync(draw_d);
	XSelectInput(draw_d, s_win, ExposureMask);
	while (1) {
	    XEvent report;
	    XNextEvent(draw_d, &report);
	    if (report.type == Expose) break;
	}
    }
    sprintf(ab, "%f + %fi", a,b);
    XStoreName(draw_d, s_win, ab);
    XClearWindow(draw_d, s_win);

    XGetWindowAttributes(draw_d, s_win, &draw_wattr);
    if (width != draw_wattr.width) {
	width = draw_wattr.width;
	mwidth = (depth > 1)? width: (1 + width/8);
	iter = (depth > 1)? iter * 4: iter;
	/* XDestroyImage should free dp1 and dp2 as well */
	if (xip1) {XDestroyImage(xip1); XDestroyImage(xip2); }
	dp1 = malloc(mwidth);
	dp2 = malloc(mwidth);
	if (dp2 == NULL) {printf("malloc failed\n"); return; }

	xip1 = XCreateImage(draw_d, draw_v, depth, ZPixmap, 0, dp1, 
		width, 1, 8, mwidth);
	xip2 = XCreateImage(draw_d, draw_v, depth, ZPixmap, 0, dp2, 
		width, 1, 8, mwidth);
	xip1->byte_order = xip2->byte_order = MSBFirst;
	xip1->bitmap_bit_order = xip2->bitmap_bit_order = MSBFirst;
    }
    sx = x = -1.5; y = -1.5;
    incrx = -x/draw_wattr.width * 2;
    incry = -y/draw_wattr.height * 2;


for (iy = 0; iy < (1 + draw_wattr.height/2); iy++, y+= incry) {
    long i;
    x = sx;
    for (ix = 0; ix < draw_wattr.width; ix++, x+= incrx) {
        wx = x; wy = y;
        for (i = 0; i < iter; i++) {
                oldwx = wx;
                wx = wx * wx - wy * wy + a;
                wy = 2 * oldwx * wy + b;
                if (wx * wx + wy * wy > 4) break;
        }
        if (depth > 1) {		/* color */
	    *(dp1 + ix) = i + bias;
	    *(dp2 + width - ix - 1) = i + bias;
	} else {
	    if (i == iter) {		 /* could also test if (i & 2) */
		dp1[ix/8] |= 1 << (7 - ix&7);
		dp2[(width - ix - 1)/8] |= 1 << (8 - (width - ix)&7);
	    }
	}
    }

   XPutImage(draw_d, s_win, draw_gc, xip1, 0,0, 0, iy, width, 1);
if (iy != draw_wattr.height - iy)
   XPutImage(draw_d,s_win,draw_gc,xip2,0,0,0, draw_wattr.height - iy, width, 1);
   if (depth == 1) for (i = 0 ; i < mwidth; i++) *(dp1 + i) = *(dp2 + i) = 0;
}

}
