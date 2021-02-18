/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xbitmap.c -- an example application
 */
#include <stdio.h>
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

Dimension pixmap_width_in_cells, pixmap_height_in_cells;

/*
 * The printout routine prints an array of 1s and 0s representing the
 * contents of the bitmap.  This data can be processed into any
 * desired form, including standard X11 bitmap file format.
 */
/* ARGSUSED */
static void 
printout(widget, client_data, call_data)
Widget widget;
caddr_t client_data, call_data;	/* unused */
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
	Widget topLevel, form, buttonbox, quit, output, bigBitmap;
	int i;

	Arg arg[5];
	extern exit();

	static XrmOptionDescRec table[] = {
		{"-pw",            "*pixmapWidthInCells",        XrmoptionSepArg, NULL},
		{"-pixmapwidth",   "*pixmapWidthInCells",        XrmoptionSepArg, NULL},
		{"-ph",            "*pixmapHeightInCells",       XrmoptionSepArg, NULL},
		{"-pixmapheight",  "*pixmapHeightInCells",       XrmoptionSepArg, NULL},
		{"-cellsize",      "*cellSizeInPixels",           XrmoptionSepArg, NULL},
	};
    
	topLevel = XtInitialize(argv[0], "XBitmap1", table, XtNumber(table), &argc, argv);

	form = XtCreateManagedWidget("form", formWidgetClass, topLevel, NULL, 0);

	buttonbox = XtCreateManagedWidget("buttonbox", boxWidgetClass, form, NULL, 0);

	output = XtCreateManagedWidget("output", commandWidgetClass, buttonbox, NULL, 0);

	XtAddCallback(output, XtNcallback, printout, NULL);

	quit = XtCreateManagedWidget("quit", commandWidgetClass, buttonbox, NULL, 0);

	XtAddCallback(quit, XtNcallback, exit, NULL);

	bigBitmap = XtCreateManagedWidget("bigBitmap", bitmapEditWidgetClass, form, NULL, 0);

	/* need the following values for the printout routine. */
	i = 0;
	XtSetArg(arg[i], XtNpixmapWidthInCells, &pixmap_width_in_cells);  i++;
	XtSetArg(arg[i], XtNpixmapHeightInCells, &pixmap_height_in_cells);  i++;
	XtGetValues(bigBitmap, arg, i);

	XtRealizeWidget(topLevel);
	XtMainLoop();
}
