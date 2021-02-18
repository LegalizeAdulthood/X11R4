
/*
 * Name:	xv_canvas_x_draw.c
 *
 * Description:
 *
 *	Demonstrates the use of Xlib drawing functions inside an 
 *	XView canvas.
 *	The CANVAS_X_PAINT_WINDOW attribute is used to inform the
 *	library to call the repaint procedure with Xlib specific
 *	arguments.
 */

#include <stdio.h>
#include <xview/xview.h>
#include <xview/canvas.h>
#include <xview/xv_xrect.h>

static 	char stipple_bits[] = {0xAA, 0xAA, 0x55, 0x55};
GC 	gc;

/*
 * 		main()
 *	Create the XView windows specifying CANVAS_X_PAINT_WINDOW
 *	for the canvas window.
 *	Find a font and create a pixmap on the server for use
 *	in the repaint proc.
 *	Create the graphics context and set it using the font
 *	and pixmap just created.
 */
main(argc, argv)
    int	    argc;
    char    *argv[];
{
	Frame   	frame;
	Canvas  	canvas;
	Xv_Window 	canvas_pw;
	XFontStruct 	*font;
	Display		*display;
	XGCValues 	gc_val;
	XID		xid;
	void		canvas_repaint();

	/* Create windows */
	xv_init(XV_INIT_ARGS, argc, argv, 0);

	frame = xv_create(NULL,FRAME,
			  FRAME_LABEL,	"xv_canvas_x_draw",
			  XV_WIDTH, 	400,
			  XV_HEIGHT, 	300,
			  0);

	canvas = xv_create(frame,CANVAS,
			   CANVAS_REPAINT_PROC,	    canvas_repaint,
			   CANVAS_X_PAINT_WINDOW,   TRUE,
			   0);
	canvas_pw = canvas_paint_window(canvas);

 	/* Get display and xid */
	display = (Display *)xv_get(canvas_pw, XV_DISPLAY);
	xid = (XID)xv_get(canvas_pw, XV_XID);

	font = XLoadQueryFont(display, "fixed");
	if (!font) {
	    fprintf(stderr,"xv_canvas_x_draw: Couldn't load fixed font\n");
	    exit(1);
	}

	/* Create and initialize GC */
	gc_val.foreground = BlackPixel(display, DefaultScreen(display));
	gc_val.background = WhitePixel(display, DefaultScreen(display));
	gc_val.font = font->fid;
	gc_val.stipple = XCreateBitmapFromData(display, xid, stipple_bits, 16, 2);

	gc = XCreateGC(display, xid,
		       GCForeground|GCBackground|GCFont|GCStipple,
		       &gc_val);

	/* Start event loop */
	xv_main_loop(frame);
	return(0);
}

/*
 * 		canvas_repaint()
 *	Draws onto the canvas using Xlib drawing functions.
 *	Use the current clipping rectangle to 
 *	1. Restrict graphics output by setting the clip_mask
 *	   in the graphics context.
 *	2. Do "smart repainting" by only painting the objects 
 *	   that lie within the damaged region (not being done in
 *	   this example).
 */
void
canvas_repaint(canvas, pw, display, xid, xrects)
    Canvas	canvas;
    Xv_Window 	pw;
    Display	*display;
    Window	xid;
    Xv_xrectlist *xrects;
{
	static XPoint 	box[] = {{0,0}, {100,100}, {0,-100}, {-100,100}, {0,-100}};
	static XPoint 	boxes[] = {{0,0}, {50,0}, {50,0}, {-100,50}, {50,0}, {50,0},
			        	{-100,50}, {50,0}, {50,0}};
	XGCValues	gc_val;

	/* Set clip rects, if any */
	if (xrects) {
		XSetClipRectangles(display, gc, 0, 0, xrects->rect_array,
                		   xrects->count, Unsorted);
	} else {
		gc_val.clip_mask = None;
		XChangeGC(display, gc, GCClipMask, &gc_val);
	}

	XDrawString(display, xid, gc, 30, 20, "XFillRectangle", 14);
	XFillRectangle(display, xid, gc, 25, 25, 100, 100);
	XSetFunction(display, gc, GXinvert);
	XFillRectangle(display, xid, gc, 50, 50, 50, 50);
	XSetFunction(display, gc, GXcopy);

	XDrawString(display, xid, gc, 155, 20, "XFillRect - stipple", 19);
	XSetFillStyle(display, gc, FillStippled);
	XFillRectangle(display, xid, gc, 150, 25, 100, 100);
	XSetFillStyle(display, gc, FillSolid);

	XDrawString(display, xid, gc, 280, 20, "XDrawPoints", 11);
	boxes[0].x = 275; boxes[0].y = 25;
	XDrawPoints(display, xid, gc, boxes, 9, CoordModePrevious);

	XDrawString(display, xid, gc, 30, 145, "XDrawLine - solid", 17);
	XDrawLine(display, xid, gc, 25, 150, 125, 250);
	XDrawLine(display, xid, gc, 25, 250, 125, 150);

	XDrawString(display, xid, gc, 155, 145, "XDrawLine - dashed", 18);
	XSetLineAttributes(display, gc, 5, LineDoubleDash, CapButt, JoinMiter);
	XDrawLine(display, xid, gc, 150, 150, 250, 250);
	XDrawLine(display, xid, gc, 150, 250, 250, 150);
	XSetLineAttributes(display, gc, 0, LineSolid, CapButt, JoinMiter);

	XDrawString(display, xid, gc, 280, 145, "XDrawLines", 10);
	box[0].x = 275; box[0].y = 150;
	XDrawLines(display, xid, gc, box, 5, CoordModePrevious);
}

