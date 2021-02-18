/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xbitmap3.c
 */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#ifdef X11R3
#include <X11/Form.h>
#include <X11/Box.h>
#include <X11/Command.h>
#else /* R4 or later */
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#endif /* X11R3 */

#include "BitmapEdit.h"

#include <stdio.h>

#define DRAWN 1
#define UNDRAWN 0

GC draw_gc, undraw_gc;
Pixmap normal_bitmap, reverse_bitmap;
Widget bigBitmap, showNormalBitmap, showReverseBitmap;
Dimension pixmap_width_in_cells, pixmap_height_in_cells;

static void cell_toggled();

String filename;	/* filename to read and write */

/* ARGSUSED */
static void 
printout(widget, client_data, call_data)
Widget widget;
caddr_t client_data, call_data;	/* unused */
{
    XWriteBitmapFile(XtDisplay(widget), filename, normal_bitmap,
	    pixmap_width_in_cells, pixmap_height_in_cells, 0, 0);
}

static void
redraw_small_picture(w)
Widget w;
{
	Pixmap pixmap;

	if (w == showNormalBitmap)
		pixmap = normal_bitmap;
	else
		pixmap = reverse_bitmap;

        if (DefaultDepthOfScreen(XtScreen(w)) == 1)
 	    XCopyArea(XtDisplay(w), pixmap, XtWindow(w),
			DefaultGCOfScreen(XtScreen(w)), 0, 0, 
			pixmap_width_in_cells, pixmap_height_in_cells, 0, 0);
        else
 	    XCopyPlane(XtDisplay(w), pixmap, XtWindow(w),
     			DefaultGCOfScreen(XtScreen(w)), 0, 0, 
			pixmap_width_in_cells, pixmap_height_in_cells, 0, 0, 1);
}

main(argc, argv)
int argc;
char *argv[];
{
    Widget topLevel, form, buttonbox, quit, output;
    Arg args[5];
    int i;
    extern exit();
    static XtActionsRec window_actions[] = {
	{"redraw_small_picture", redraw_small_picture}
    };

    String trans =
	"<Expose>:	redraw_small_picture()";

    static XrmOptionDescRec table[] = 
    {
        {"-pw",            "*pixmapWidthInCells",        XrmoptionSepArg, NULL},
        {"-pixmapwidth",   "*pixmapWidthInCells",        XrmoptionSepArg, NULL},
        {"-ph",            "*pixmapHeightInCells",       XrmoptionSepArg, NULL},
        {"-pixmapheight",  "*pixmapHeightInCells",       XrmoptionSepArg, NULL},
        {"-cellsize",      "*cellSizeInPixels",           XrmoptionSepArg, NULL},
    };
    
    topLevel = XtInitialize(argv[0], "XBitmap3", table, XtNumber(table), &argc, argv);

    if (argv[1] != NULL)
         filename = argv[1];
    else {
         fprintf(stderr, "xbitmap: must specify filename on command line\n");
         exit(1);
    }

    form = XtCreateManagedWidget("form", formWidgetClass, topLevel, NULL, 0);

    buttonbox = XtCreateManagedWidget("buttonbox", boxWidgetClass, form, NULL, 0);

    output = XtCreateManagedWidget("output", commandWidgetClass, buttonbox, NULL, 0);

    XtAddCallback(output, XtNcallback, printout, NULL);

    quit = XtCreateManagedWidget("quit", commandWidgetClass, buttonbox, NULL, 0);

    XtAddCallback(quit, XtNcallback, exit, NULL);

    XtAddActions(window_actions, XtNumber(window_actions));

    bigBitmap = XtCreateManagedWidget("bigBitmap", bitmapEditWidgetClass, 
	form, NULL, 0);

    XtAddCallback(bigBitmap, XtNcallback, cell_toggled, NULL);

    i = 0;
    XtSetArg(args[i], XtNpixmapHeightInCells, &pixmap_height_in_cells);   i++;
    XtSetArg(args[i], XtNpixmapWidthInCells, &pixmap_width_in_cells);   i++;
    XtGetValues(bigBitmap, args, i);

    set_up_things(topLevel);

    i = 0;
    XtSetArg(args[i], XtNwidth, pixmap_width_in_cells);    i++;
    XtSetArg(args[i], XtNheight, pixmap_height_in_cells);    i++;
    XtSetArg(args[i], XtNtranslations, XtParseTranslationTable(trans));    i++;
    showNormalBitmap = XtCreateManagedWidget("showNormalBitmap", 
	widgetClass, buttonbox, args, i);

    showReverseBitmap = XtCreateManagedWidget("showReverseBitmap", 
	widgetClass, buttonbox, args, i);

    XtRealizeWidget(topLevel);
    XtMainLoop();
}

set_up_things(w)
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
	/* note that normal_bitmap is used as the drawable because it
	 * is one bit deep.  The root window may not be one bit deep */
	draw_gc = XCreateGC(XtDisplay(w),  normal_bitmap,
	        GCForeground | GCBackground, &values);

	values.foreground = 0;
	values.background = 1;
	undraw_gc = XCreateGC(XtDisplay(w), normal_bitmap,
	        GCForeground | GCBackground, &values);

	XFillRectangle(XtDisplay(w), reverse_bitmap, draw_gc,
   	    	0, 0, pixmap_width_in_cells, pixmap_height_in_cells);
}

/* ARGSUSED */
static void
cell_toggled(w, unused, info)
Widget w;
caddr_t unused;	/* client_data */
caddr_t info;	/* call_data (from widget) */
{
    /* cast pointer to needed type: */
    BitmapEditPointInfo *cur_info = (BitmapEditPointInfo *) info;
    /* 
     * Note, BitmapEditPointInfo is defined in BitmapEdit.h 
     */

    XDrawPoint(XtDisplay(w), normal_bitmap, ((cur_info->mode == DRAWN) ? draw_gc : undraw_gc), cur_info->newx, cur_info->newy);
    XDrawPoint(XtDisplay(w), reverse_bitmap, ((cur_info->mode == DRAWN) ? undraw_gc : draw_gc), cur_info->newx, cur_info->newy); 

    redraw_small_picture(showNormalBitmap);
    redraw_small_picture(showReverseBitmap);
}
