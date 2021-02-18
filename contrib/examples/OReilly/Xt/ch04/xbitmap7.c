/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xbitmap7.c
 */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#ifdef X11R3
#include <X11/Box.h>
#include <X11/Form.h>
#include <X11/VPaned.h>
#include <X11/Command.h>
#include <X11/Scroll.h>
#else /* R4 or later */
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Paned.h>	/* now V or H paned */
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Scrollbar.h>
#endif /* X11R3 */

#include "BitmapEdit.h"

#include <stdio.h>

#define DRAWN 1
#define UNDRAWN 0
#define MAXSIZE 300
#define BUTTONBOXWIDTH 300

GC draw_gc, undraw_gc;

Pixmap normal_bitmap, reverse_bitmap;

Widget bigBitmap, showNormalBitmap, showReverseBitmap, buttonbox, showNormalBitmap, showReverseBitmap;

Dimension pixmap_width_in_cells, pixmap_height_in_cells;
Dimension pixmap_width_in_pixels, pixmap_height_in_pixels;
Dimension window_width, window_height;

int cell_size;

static void cell_toggled();

/* ARGSUSED */
static void 
printout(widget)
Widget widget;
{
    int x, y;
    char *cell;
    cell = BitmapEditGetArrayString(widget);

    (void) putchar('\n');
    for (y = 0; y < pixmap_height_in_cells; y++) {
	for (x = 0; x < pixmap_width_in_cells; x++)
	    (void) putchar(cell[x + y * pixmap_width_in_cells] ? '1' : '0');
	(void) putchar('\n');
    }
    (void) putchar('\n');
}

static void
redraw_small_picture(w)
Widget w;
{
	Window window = XtWindow(w);
	Pixmap pixmap;

	if (w == showNormalBitmap)
		pixmap = normal_bitmap;
	else
		pixmap = reverse_bitmap;

        if (DefaultDepthOfScreen(XtScreen(w)) == 1)
 	    XCopyArea(XtDisplay(w), pixmap, window,
			DefaultGCOfScreen(XtScreen(w)), 0, 0, 
			pixmap_width_in_cells, pixmap_height_in_cells, 0, 0);
        else
 	    XCopyPlane(XtDisplay(w), pixmap, window,
     			DefaultGCOfScreen(XtScreen(w)), 0, 0, 
			pixmap_width_in_cells, pixmap_height_in_cells, 0, 0, 1);
}

main(argc, argv)
int argc;
char *argv[];
{
    Widget topLevel, vpane;

    static XrmOptionDescRec table[] = 
    {
        {"-pw",            "*pixmapWidthInCells",        XrmoptionSepArg, NULL},
        {"-pixmapwidth",   "*pixmapWidthInCells",        XrmoptionSepArg, NULL},
        {"-ph",            "*pixmapHeightInCells",       XrmoptionSepArg, NULL},
        {"-pixmapheight",  "*pixmapHeightInCells",       XrmoptionSepArg, NULL},
        {"-cellsize",      "*cellSizeInPixels",           XrmoptionSepArg, NULL},
    };
    
    topLevel = XtInitialize(argv[0], "XBitmap3", table, XtNumber(table), &argc, argv);

#ifdef X11R3
    vpane = XtCreateManagedWidget("vpane", vPanedWidgetClass, topLevel, NULL, 0);
#else /* R4 or later */
    vpane = XtCreateManagedWidget("vpane", panedWidgetClass, topLevel, NULL, 0);
#endif /* X11R3 */

    create_viewport(vpane);

    create_buttonbox(vpane);

    create_gcs_and_pixmaps(topLevel);

    XtRealizeWidget(topLevel);
    XtMainLoop();
}

/* ARGSUSED */
static void
resize_scrollbars(w, event, orientation)
Widget w;
XEvent *event;
char *orientation[];
{
    Dimension width, height;
    int cur_x, cur_y;
    Arg args[5];
    int i;

    i = 0;
    XtSetArg(args[i], XtNheight, &height);  i++;
    XtSetArg(args[i], XtNwidth, &width);  i++;
    XtGetValues(w, args, i);

    i = 0;
    XtSetArg(args[i], XtNcurX, &cur_x);  i++;
    XtSetArg(args[i], XtNcurY, &cur_y);  i++;
    XtGetValues(bigBitmap, args, i);

    if (*orientation[0] == 'h')
#ifdef X11R3
        XtScrollBarSetThumb(w, (float)cur_x/(pixmap_width_in_pixels),
#else /* R4 or later */
        XawScrollBarSetThumb(w, (float)cur_x/(pixmap_width_in_pixels),
#endif /* X11R3 */
			       (float)width/(pixmap_width_in_pixels));
    else 
#ifdef X11R3
        XtScrollBarSetThumb(w, (float)cur_y/(pixmap_height_in_pixels),
#else /* R4 or later */
        XawScrollBarSetThumb(w, (float)cur_y/(pixmap_height_in_pixels),
#endif /* X11R3 */
			       (float)height/(pixmap_height_in_pixels));
}

create_gcs_and_pixmaps(w)
Widget w;
{
	XGCValues values;

	normal_bitmap = XCreatePixmap(XtDisplay(w), 
	    RootWindowOfScreen(XtScreen(w)),
	    pixmap_width_in_cells, pixmap_height_in_cells, 1);

	reverse_bitmap = XCreatePixmap(XtDisplay(w), 
	    RootWindowOfScreen(XtScreen(w)),
	    pixmap_width_in_cells, pixmap_height_in_cells, 1);

	values.foreground = 1;
	values.background = 0;
	draw_gc = XCreateGC(XtDisplay(w),  normal_bitmap,
	        GCForeground | GCBackground, &values);

	values.background = 1;
	values.foreground = 0;
	undraw_gc = XCreateGC(XtDisplay(w), normal_bitmap,
	        GCForeground | GCBackground, &values);

	XFillRectangle(XtDisplay(w), reverse_bitmap, draw_gc,
   	    	0, 0, pixmap_width_in_cells, pixmap_height_in_cells);
}

/* ARGSUSED */
static void
scroll_up_down(w, unused, pixels)
Widget w;
caddr_t unused;
int pixels;
{
    Dimension height;
    int cur_y;
    Arg args[5];
    int i;

    i = 0;
    XtSetArg(args[i], XtNheight, &height);  i++;
    XtGetValues(w,args, i);

    i = 0;
    XtSetArg(args[i], XtNcurY, &cur_y);  i++;
    XtGetValues(bigBitmap, args, i);

    /* When pixels is negative, right button pressed, move data down,
     * thumb moves up.  Otherwise, left button pressed, pixels 
     * positive, move data up,
     * thumb down. 
     */
    cur_y += pixels;

    if (cur_y < 0)
	cur_y = 0;
    else if (cur_y > pixmap_height_in_pixels - height )
	cur_y = pixmap_height_in_pixels - height;

    i = 0;
    XtSetArg(args[i], XtNcurY, cur_y);   i++;
    XtSetValues(bigBitmap, args, i);
    
#ifdef X11R3
    XtScrollBarSetThumb(w, (float)cur_y/pixmap_height_in_pixels,
#else /* R4 or later */
    XawScrollBarSetThumb(w, (float)cur_y/pixmap_height_in_pixels,
#endif /* X11R3 */
			   (float)height/pixmap_height_in_pixels);
}

/* ARGSUSED */
static void
scroll_left_right(w, unused, pixels)
Widget w;
caddr_t unused;
int pixels;
{
    Dimension width;
    int cur_x;
    Arg args[5];
    int i;

    i = 0;
    XtSetArg(args[i], XtNwidth, &width);  i++;
    XtGetValues(w, args, i);

    i = 0;
    XtSetArg(args[i], XtNcurX, &cur_x);  i++;
    XtGetValues(bigBitmap, args, i);

    cur_x += pixels;

    if (cur_x < 0)
	cur_x = 0;
    else if (cur_x > pixmap_width_in_pixels - width)
	cur_x = pixmap_width_in_pixels - width;

    i = 0;
    XtSetArg(args[i], XtNcurX, cur_x);   i++;
    XtSetValues(bigBitmap, args, i);

#ifdef X11R3
    XtScrollBarSetThumb(w, (float)cur_x/pixmap_width_in_pixels,
#else /* R4 or later */
    XawScrollBarSetThumb(w, (float)cur_x/pixmap_width_in_pixels,
#endif /* X11R3 */
			   (float) width/pixmap_width_in_pixels);
}

/* ARGSUSED */
static void
thumb_up_down(w, unused, percent)
Widget w;
caddr_t unused;
float percent;
{
    Dimension height;
    int cur_y;
    Arg args[5];
    int i;

    i = 0;
    XtSetArg(args[i], XtNheight, &height);  i++;
    XtGetValues(w, args, i);

    i = 0;
    XtSetArg(args[i], XtNcurY, &cur_y);  i++;
    XtGetValues(bigBitmap, args, i);

    if ((cur_y = (int)(pixmap_height_in_pixels * percent)) >=
		       pixmap_height_in_pixels - height)
	cur_y = pixmap_height_in_pixels - height;

    i = 0;
    XtSetArg(args[i], XtNcurY, cur_y);   i++;
    XtSetValues(bigBitmap, args, i);

#ifdef X11R3
    XtScrollBarSetThumb(w, (float)cur_y/pixmap_height_in_pixels,
#else /* R4 or later */
    XawScrollBarSetThumb(w, (float)cur_y/pixmap_height_in_pixels,
#endif /* X11R3 */
			   (float)height/pixmap_height_in_pixels);
}

/* ARGSUSED */
static void
thumb_left_right(w, unused, percent)
Widget w;
caddr_t unused;
float percent;
{
    Dimension width;
    int cur_x;
    Arg args[5];
    int i;

    i = 0;
    XtSetArg(args[i], XtNwidth, &width);  i++;
    XtGetValues(w, args, i);

    i = 0;
    XtSetArg(args[i], XtNcurX, &cur_x);  i++;
    XtGetValues(bigBitmap, args, i);

    if ((cur_x = (int)(pixmap_width_in_pixels * percent)) >=
		       pixmap_width_in_pixels - width)
	cur_x = pixmap_width_in_pixels - width;

    i = 0;
    XtSetArg(args[i], XtNcurX, cur_x);   i++;
    XtSetValues(bigBitmap, args, i);

#ifdef X11R3
    XtScrollBarSetThumb(w, (float)cur_x/pixmap_width_in_pixels,
#else /* R4 or later */
    XawScrollBarSetThumb(w, (float)cur_x/pixmap_width_in_pixels,
#endif /* X11R3 */
			   (float)width/pixmap_width_in_pixels);
}

/* ARGSUSED */
static void
cell_toggled(w, unused, info)
Widget w;
caddr_t unused;
caddr_t info;
{
    BitmapEditPointInfo *cur_info = (BitmapEditPointInfo *) info;

    XDrawPoint(XtDisplay(w), normal_bitmap, ((cur_info->mode == UNDRAWN) ? draw_gc : undraw_gc), cur_info->newx, cur_info->newy);
    XDrawPoint(XtDisplay(w), reverse_bitmap, ((cur_info->mode == UNDRAWN) ? undraw_gc : draw_gc), cur_info->newx, cur_info->newy); 

    redraw_small_picture(showNormalBitmap);
    redraw_small_picture(showReverseBitmap);
}

create_buttonbox(parent)
Widget parent;
{
    Widget output, quit;
    int i;
    Arg args[5];
    extern exit();
    XtActionsRec window_actions[2];
    String trans =
	"<Expose>:	redraw_small_picture()";
    /* 
     * Setup buttonbox with four children
     */
    i = 0;
    XtSetArg(args[i], XtNwidth, BUTTONBOXWIDTH);    i++;
    buttonbox = XtCreateManagedWidget("buttonbox", boxWidgetClass, parent, args, i);

    output = XtCreateManagedWidget("output", commandWidgetClass, buttonbox, NULL, 0);
    XtAddCallback(output, XtNcallback, printout, NULL);

    quit = XtCreateManagedWidget("quit", commandWidgetClass, buttonbox, NULL, 0);
    XtAddCallback(quit, XtNcallback, exit, NULL);

    i = 0;
    XtSetArg(args[i], XtNwidth,	pixmap_width_in_cells);    i++;
    XtSetArg(args[i], XtNheight, pixmap_height_in_cells);    i++;
    XtSetArg(args[i], XtNtranslations, XtParseTranslationTable(trans));    i++;
    showNormalBitmap = XtCreateManagedWidget("showNormalBitmap", widgetClass, buttonbox, args, i);

    i = 0;
    XtSetArg(args[i], XtNwidth,	pixmap_width_in_cells);    i++;
    XtSetArg(args[i], XtNheight, pixmap_height_in_cells);    i++;
    XtSetArg(args[i], XtNtranslations, XtParseTranslationTable(trans));    i++;
    showReverseBitmap = XtCreateManagedWidget("showReverseBitmap", widgetClass, buttonbox, args, i);

    /* exposure action for showNormalBitmap and showReverseBitmap */
    window_actions[0].proc = redraw_small_picture;
    window_actions[0].string = "redraw_small_picture";
    XtAddActions(window_actions, 2);
}

create_viewport(parent)
Widget parent;
{
    Widget scrollHoriz, scrollVert, drawingForm;
    Arg args[6];
    int i;
    static XtActionsRec window_actions[] = {
		{"resize_scrollbars", resize_scrollbars}
	};

    XtAddActions(window_actions, 1);

    drawingForm = XtCreateManagedWidget("drawingForm", formWidgetClass, parent, NULL, 0);

    bigBitmap = XtCreateManagedWidget("bigBitmap", bitmapEditWidgetClass, drawingForm, NULL, 0);

    XtAddCallback(bigBitmap, XtNcallback, cell_toggled, NULL);

    i = 0;
    XtSetArg(args[i], XtNpixmapHeightInCells, &pixmap_height_in_cells);  i++;
    XtSetArg(args[i], XtNpixmapWidthInCells, &pixmap_width_in_cells);  i++;
    XtSetArg(args[i], XtNcellSizeInPixels, &cell_size);  i++;
    XtGetValues(bigBitmap, args, i);

    pixmap_height_in_pixels = pixmap_height_in_cells * cell_size;
    pixmap_width_in_pixels = pixmap_width_in_cells * cell_size;

    window_width = ((pixmap_width_in_pixels > MAXSIZE) ? MAXSIZE : pixmap_width_in_pixels);
    window_height = ((pixmap_height_in_pixels > MAXSIZE) ? MAXSIZE : pixmap_height_in_pixels);

    i = 0;
    XtSetArg(args[i], XtNwidth, window_width);   i++;
    XtSetArg(args[i], XtNheight, window_height);   i++;
    XtSetValues(bigBitmap, args, i);

    i = 0;
    XtSetArg(args[i], XtNorientation, XtorientVertical);   i++;
    XtSetArg(args[i], XtNheight, window_height);   i++;
    XtSetArg(args[i], XtNwidth, 15);   i++;
    XtSetArg(args[i], XtNshown, window_height/pixmap_height_in_pixels);   i++;
    /* XtSetArg(args[i], XtNfromHoriz, bigBitmap);   i++; */

    scrollVert = XtCreateManagedWidget("scrollVert", scrollbarWidgetClass,
	drawingForm, args, i);

    XtAddCallback(scrollVert, XtNscrollProc, scroll_up_down, bigBitmap);
    XtAddCallback(scrollVert, XtNthumbProc, thumb_up_down, bigBitmap);

    XtOverrideTranslations(scrollVert,
		XtParseTranslationTable("<Configure>: resize_scrollbars(v)\n\
				<Expose>: resize_scrollbars(v)"));

    i = 0;
    XtSetArg(args[i], XtNorientation, XtorientHorizontal);   i++;
    XtSetArg(args[i], XtNwidth, window_width);   i++;
    XtSetArg(args[i], XtNheight, 15);   i++;
    XtSetArg(args[i], XtNshown, window_height/pixmap_height_in_pixels);   i++;
    /* XtSetArg(args[i], XtNfromVert, bigBitmap);   i++; */

    scrollHoriz = XtCreateManagedWidget("scrollHoriz", scrollbarWidgetClass,
	drawingForm, args, i);

    XtAddCallback(scrollHoriz, XtNscrollProc, scroll_left_right, bigBitmap);
    XtAddCallback(scrollHoriz, XtNthumbProc, thumb_left_right, bigBitmap);
    XtOverrideTranslations(scrollHoriz,
		XtParseTranslationTable("<Configure>: resize_scrollbars(h)\n\
				<Expose>: resize_scrollbars(h)"));
}

