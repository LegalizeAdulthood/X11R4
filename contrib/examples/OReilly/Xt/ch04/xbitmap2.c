/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xbitmap2.c
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

Widget bigBitmap, buttonbox;

Dimension pixmap_width_in_cells, pixmap_height_in_cells;
Dimension pixmap_width_in_pixels, pixmap_height_in_pixels;
Dimension window_width, window_height;

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
    
    topLevel = XtInitialize(argv[0], "XBitmap2", table, XtNumber(table), &argc, argv);

#ifdef X11R3
    vpane = XtCreateManagedWidget("vpane", vPanedWidgetClass, topLevel, NULL, 0);
#else /* R4 or later */
    vpane = XtCreateManagedWidget("vpane", panedWidgetClass, topLevel, NULL, 0);
#endif /* X11R3 */

    create_buttonbox(vpane);

    create_viewport(vpane);

    XtRealizeWidget(topLevel);
    XtMainLoop();
}

static void
resize_thumbs(w, event, orientation)
Widget w;
XEvent *event;
String orientation[];	/* String *params */
/* Cardinal *num_params;    not used */
{
    Dimension width, height;
    int cur_x, cur_y;
    Arg args[5];
    int i;

    i = 0;
    XtSetArg(args[i], XtNheight, &height);  i++;
    XtSetArg(args[i], XtNwidth, &width);  i++;
    XtSetArg(args[i], XtNcurX, &cur_x);  i++;
    XtSetArg(args[i], XtNcurY, &cur_y);  i++;
    XtGetValues(bigBitmap, args, i);

    if (*orientation[0] == 'h')
#ifdef X11R3
        XtScrollBarSetThumb(w,
#else /* R4 or later */
        XawScrollBarSetThumb(w,
#endif /* X11R3 */
				(float)cur_x/pixmap_width_in_pixels,
			        (float)width/pixmap_width_in_pixels);
    else 
#ifdef X11R3
        XtScrollBarSetThumb(w,
#else /* R4 or later */
        XawScrollBarSetThumb(w,
#endif /* X11R3 */
				(float)cur_y/pixmap_height_in_pixels,
			        (float)height/pixmap_height_in_pixels);
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
    XtSetArg(args[i], XtNcurY, &cur_y);  i++;
    XtGetValues(bigBitmap, args, i);

    /* When pixels is negative, right button pressed, move data down,
     * thumb moves up.  Otherwise, left button pressed, pixels 
     * positive, move data up, thumb down. 
     */
    cur_y += pixels;

    /* limit panning to size of bitmap */
    if (cur_y < 0)
	cur_y = 0;
    else if (cur_y > pixmap_height_in_pixels - height )
	cur_y = pixmap_height_in_pixels - height;

    i = 0;
    XtSetArg(args[i], XtNcurY, cur_y);   i++;
    XtSetValues(bigBitmap, args, i);
    
#ifdef X11R3
    XtScrollBarSetThumb(w, 
#else /* R4 or later */
    XawScrollBarSetThumb(w,
#endif /* X11R3 */
			(float)cur_y/pixmap_height_in_pixels,
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
    XtSetArg(args[i], XtNcurX, &cur_x);  i++;
    XtGetValues(bigBitmap, args, i);

    cur_x += pixels;

    /* limit panning to size of bitmap */
    if (cur_x < 0)
	cur_x = 0;
    else if (cur_x > pixmap_width_in_pixels - width)
	cur_x = pixmap_width_in_pixels - width;

    i = 0;
    XtSetArg(args[i], XtNcurX, cur_x);   i++;
    XtSetValues(bigBitmap, args, i);

#ifdef X11R3
    XtScrollBarSetThumb(w,
#else /* R4 or later */
    XawScrollBarSetThumb(w,
#endif /* X11R3 */
			(float)cur_x/pixmap_width_in_pixels,
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
    XtSetArg(args[i], XtNcurY, &cur_y);  i++;
    XtGetValues(bigBitmap, args, i);

    if ((cur_y = (int)(pixmap_height_in_pixels * percent)) >=
		       pixmap_height_in_pixels - height)
	cur_y = pixmap_height_in_pixels - height;

    i = 0;
    XtSetArg(args[i], XtNcurY, cur_y);   i++;
    XtSetValues(bigBitmap, args, i);

#ifdef X11R3
    XtScrollBarSetThumb(w,
#else /* R4 or later */
    XawScrollBarSetThumb(w,
#endif /* X11R3 */
				(float)cur_y/pixmap_height_in_pixels,
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
    XtSetArg(args[i], XtNcurX, &cur_x);  i++;
    XtGetValues(bigBitmap, args, i);

    if ((cur_x = (int)(pixmap_width_in_pixels * percent)) >=
		       pixmap_width_in_pixels - width)
	cur_x = pixmap_width_in_pixels - width;

    i = 0;
    XtSetArg(args[i], XtNcurX, cur_x);   i++;
    XtSetValues(bigBitmap, args, i);

#ifdef X11R3
    XtScrollBarSetThumb(w,
#else /* R4 or later */
    XawScrollBarSetThumb(w,
#endif /* X11R3 */
			(float)cur_x/pixmap_width_in_pixels,
			(float)width/pixmap_width_in_pixels);
}

create_buttonbox(parent)
Widget parent;
{
    Widget output, quit;
    int i;
    Arg args[5];
    extern exit();

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

}

create_viewport(parent)
Widget parent;
{
    Widget scrollHoriz, scrollVert, drawingForm;
    Arg args[6];
    int i;
    int cell_size;
    static XtActionsRec window_actions[] = {
		{"resize_thumbs", resize_thumbs}
	};

    XtAddActions(window_actions, 1);

    drawingForm = XtCreateManagedWidget("drawingForm", formWidgetClass, parent, NULL, 0);

    bigBitmap = XtCreateManagedWidget("bigBitmap", bitmapEditWidgetClass, drawingForm, NULL, 0);

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
    XtSetArg(args[i], XtNlength, window_height);   i++;
    scrollVert = XtCreateManagedWidget("scrollVert", scrollbarWidgetClass,
	drawingForm, args, i);

    XtAddCallback(scrollVert, XtNscrollProc, scroll_up_down, bigBitmap);
    XtAddCallback(scrollVert, XtNthumbProc, thumb_up_down, bigBitmap);

    XtOverrideTranslations(scrollVert,
		XtParseTranslationTable("<Configure>: resize_thumbs(v)"));

    i = 0;
    XtSetArg(args[i], XtNorientation, XtorientHorizontal);   i++;
    XtSetArg(args[i], XtNlength, window_width);   i++;
    scrollHoriz = XtCreateManagedWidget("scrollHoriz", scrollbarWidgetClass,
	drawingForm, args, i);

    XtAddCallback(scrollHoriz, XtNscrollProc, scroll_left_right, bigBitmap);
    XtAddCallback(scrollHoriz, XtNthumbProc, thumb_left_right, bigBitmap);
    XtOverrideTranslations(scrollHoriz,
		XtParseTranslationTable("<Configure>: resize_thumbs(h)"));
}
