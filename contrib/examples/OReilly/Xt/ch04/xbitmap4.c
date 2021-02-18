/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xbitmap4.c
 */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#ifdef X11R3
#include <X11/VPaned.h>
#include <X11/Box.h>
#include <X11/Command.h>
#else /* R4 or later */
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#endif /* X11R3 */

#include <stdio.h>

/* 
 * The following could be placed in an "xbitmap.h" file.
 */
#define XtNdebug "debug"
#define XtCDebug "Debug"
#define XtNpixmapWidthInCells "pixmapWidthInCells"
#define XtCPixmapWidthInCells "PixmapWidthInCells"
#define XtNpixmapHeightInCells "pixmapHeightInCells"
#define XtCPixmapHeightInCells "PixmapHeightInCells"
#define XtNcellSizeInPixels "cellSizeInPixels"
#define XtCCellSizeInPixels "CellSizeInPixels"

#define DRAWN 1
#define UNDRAWN 0

#define DRAW 1
#define UNDRAW 0

#define MAXLINES  1000

#define MINBITMAPWIDTH  1
#define MAXBITMAPWIDTH  1000
#define MINCELLSIZE  4
#define MAXCELLSIZE  100

#define DEFAULTWIDTH 300
#define DEFAULTHEIGHT 300

#define SCROLLBARWIDTH 15

Pixmap big_picture;
GC draw_gc, undraw_gc; /* for drawing into the big_picture, 1-bit deep */
GC copy_gc;	/* for copying from pixmap into window, screen depth */
Widget bitmap;  /* this is the drawing surface */
char *cell;  /* this is the array for printing output and keeping track of cells drawn */
int cur_x, cur_y;
int pixmap_width_in_pixels, pixmap_height_in_pixels;

/* data structure for application resources */
typedef struct {
	Pixel copy_fg;
	Pixel copy_bg;
	int pixmap_width_in_cells;
	int pixmap_height_in_cells;
	int cell_size_in_pixels;
	Boolean debug;
} AppData, *AppDataPtr;

AppData app_data;

/* resource list */
static XtResource resources[] = {
	{
		XtNforeground,
		XtCForeground,
		XtRPixel,
		sizeof(Pixel),
		XtOffset(AppDataPtr, copy_fg),
		XtRString,
		XtDefaultForeground
	},
	{
		XtNbackground,
		XtCBackground,
		XtRPixel,
		sizeof(Pixel),
		XtOffset(AppDataPtr, copy_bg),
		XtRString,
		XtDefaultBackground
	},
	{
		XtNpixmapWidthInCells,
		XtCPixmapWidthInCells,
		XtRInt,
		sizeof(int),
		XtOffset(AppDataPtr, pixmap_width_in_cells),
		XtRImmediate,
		(caddr_t) 32,
	},
	{
		XtNpixmapHeightInCells,
		XtCPixmapHeightInCells,
		XtRInt,
		sizeof(int),
		XtOffset(AppDataPtr, pixmap_height_in_cells),
		XtRImmediate,
		(caddr_t) 32,
	},
	{
		XtNcellSizeInPixels,
		XtCCellSizeInPixels,
		XtRInt,
		sizeof(int),
		XtOffset(AppDataPtr, cell_size_in_pixels),
		XtRImmediate,
		(caddr_t) 30,
	},
	{
		XtNdebug,
		XtCDebug,
		XtRBoolean,
		sizeof(Boolean),
		XtOffset(AppDataPtr, debug),
		XtRImmediate,
		(caddr_t) FALSE,
	},
};

/* Command-line options table */
static XrmOptionDescRec options[] = {
	{"-pw",            "*pixmapWidthInCells",        XrmoptionSepArg, NULL},
	{"-pixmapwidth",   "*pixmapWidthInCells",        XrmoptionSepArg, NULL},
	{"-ph",            "*pixmapHeightInCells",       XrmoptionSepArg, NULL},
	{"-pixmapheight",  "*pixmapHeightInCells",       XrmoptionSepArg, NULL},
	{"-cellsize",      "*cellSizeInPixels",           XrmoptionSepArg, NULL},
	{"-fg",            "*foreground",           XrmoptionSepArg, NULL},
	{"-foreground",    "*foreground",           XrmoptionSepArg, NULL},
	{"-debug",    "*debug",           XrmoptionNoArg, "True"},
};

/* callback function to print cell array to stdout */
/* ARGSUSED */
static void 
printout()
{
	int x, y;
	putchar('\n');
	for (y = 0; y < app_data.pixmap_height_in_cells; y++) {
		for (x = 0; x < app_data.pixmap_width_in_cells; x++)
			putchar(cell[x + y * app_data.pixmap_width_in_cells] ? '1' : '0');
		putchar('\n');
	}
	putchar('\n');

	/*
	 * It wouldn't be hard to write a function to convert from cell format
	 * to one accepted by XWriteBitmapFile, and the reverse to import such files.
	 * This is done in xbitmap3, where it is even easier.
	 */
}

static void RedrawPicture(), DrawCell(), UndrawCell(), ToggleCell(), DrawPixmaps();

static void Syntax(argc, argv)
int argc;
char * argv[];
{
    int i;
    static int errs = False;

    /* first argument is program name - skip that */
    for (i = 1; i < argc; i++) {
	if (!errs++) /* do first time through */
		fprintf(stderr, "xbitmap4: command line option not understood:\n");
	fprintf(stderr, "option: %s\n", argv[i]);
    }

    fprintf(stderr, "xbitmap understands all standard Xt command line options.\n\n");

    fprintf(stderr, "Additional options are as follows:\n\n");
    fprintf(stderr, "Option             Valid Range\n");
    fprintf(stderr, "-pw                2 to 1000\n");
    fprintf(stderr, "-pixmapwidth       2 to 1000\n");
    fprintf(stderr, "-ph                2 to 1000\n");
    fprintf(stderr, "-pixmapheight      2 to 1000\n");
    fprintf(stderr, "-cellsize          4 to 100\n");
    fprintf(stderr, "-fg                color name\n");
    fprintf(stderr, "-foreground        color name\n");
    fprintf(stderr, "-debug             no value necessary\n\n");
}

main(argc, argv)
int argc;
char *argv[];
{
    int i;
    Arg args[5];
    Widget topLevel, vpane, buttonbox, quit, output;
    extern exit();
    /* translation table for bitmap core widget */
    String trans =
	"<Expose>:	RedrawPicture()	        \n\
         <Btn1Down>:    DrawCell()              \n\
         <Btn2Down>:    UndrawCell()            \n\
         <Btn3Down>:    ToggleCell()            \n\
         <Btn1Motion>:  DrawCell()              \n\
         <Btn2Motion>:  UndrawCell()            \n\
         <Btn3Motion>:  ToggleCell()";

    static XtActionsRec window_actions[] = {
	{"RedrawPicture",	RedrawPicture},
        {"DrawCell", 	DrawCell},
        {"UndrawCell",	UndrawCell},
        {"ToggleCell",	ToggleCell},
    };

    topLevel = XtInitialize( argv[0], 
		"XBitmap4", 
		options, 
		XtNumber(options), 
		&argc, 
		argv);

    /* XtInitialize leaves program name in args */
    if (argc > 1)
	Syntax(argc, argv);

    XtGetApplicationResources(topLevel, 
		&app_data, 
		resources, 
		XtNumber(resources), 
		NULL, 
		0);

    /*
     * We must check the application resource values here.
     * Otherwise, user could supply out of range values and crash program.
     * Conversion routines do this automatically, so colors are already checked.
     */
    if ((app_data.pixmap_width_in_cells > MAXBITMAPWIDTH) || 
		(app_data.pixmap_width_in_cells < MINBITMAPWIDTH) ||
    		(app_data.pixmap_height_in_cells > MAXBITMAPWIDTH) || 
		(app_data.pixmap_height_in_cells < MINBITMAPWIDTH)) {
	fprintf(stderr, "xbitmap: error in resource settings:",
			"dimension must be between %d and %d cells\n", 
			MINBITMAPWIDTH, MAXBITMAPWIDTH);
	exit(1);
    }
    if ((app_data.cell_size_in_pixels < MINCELLSIZE) || 
    		(app_data.cell_size_in_pixels > MAXCELLSIZE)) {
	fprintf(stderr, "xbitmap: error in resource settings:",
			"cell size must be between %d and %d pixels\n", 
			MINCELLSIZE, MAXCELLSIZE);
	exit(1);
    }

    /* begin application code */

    set_up_things(topLevel);

    cell = XtCalloc(app_data.pixmap_width_in_cells * 
		app_data.pixmap_height_in_cells, sizeof(char));

    if (app_data.debug)
		fprintf(stderr, "xbitmap: pixmap dimensions are %d by %d\n", 
				app_data.pixmap_width_in_cells, 
				app_data.pixmap_height_in_cells);

    i = 0;
    XtSetArg(args[i], XtNwidth, (pixmap_width_in_pixels > 300) ? (300 + SCROLLBARWIDTH + 15) : (pixmap_width_in_pixels + SCROLLBARWIDTH + 15));   i++;

    /* following line is a kludge */
    XtSetArg(args[i], XtNheight, 15);   i++;

#ifdef X11R3
    vpane = XtCreateManagedWidget("vpane", vPanedWidgetClass, topLevel, args, i);
#else /* R4 or later */
    vpane = XtCreateManagedWidget("vpane", panedWidgetClass, topLevel, args, i);
#endif /* X11R3 */

    buttonbox = XtCreateManagedWidget("buttonbox", boxWidgetClass, vpane, NULL, 0);

    output = XtCreateManagedWidget("output", commandWidgetClass, buttonbox, NULL, 0);

    XtAddCallback(output, XtNcallback, printout, NULL);

    quit = XtCreateManagedWidget("quit", commandWidgetClass, buttonbox, NULL, 0);

    XtAddCallback(quit, XtNcallback, exit, NULL);

    i = 0;
    XtSetArg(args[i], XtNtranslations, XtParseTranslationTable(trans));  i++;
    XtSetArg(args[i], XtNwidth, (pixmap_width_in_pixels > 300) ? 300 : pixmap_width_in_pixels);  i++;
    XtSetArg(args[i], XtNheight, (pixmap_height_in_pixels > 300) ? 300 : pixmap_height_in_pixels);  i++;
    bitmap = XtCreateManagedWidget("bitmap", widgetClass, vpane, args, i);

    XtAddActions(window_actions, XtNumber(window_actions));

    XtRealizeWidget(topLevel);

    XtMainLoop();
}

set_up_things(w)
Widget w;
{
	XGCValues values;
	int x, y;
	XSegment segment[MAXLINES];
	int n_horiz_segments, n_vert_segments;

	pixmap_width_in_pixels = app_data.pixmap_width_in_cells * 
			app_data.cell_size_in_pixels;
	pixmap_height_in_pixels = app_data.pixmap_height_in_cells * 
			app_data.cell_size_in_pixels;

	big_picture = XCreatePixmap(XtDisplay(w),
	    RootWindowOfScreen(XtScreen(w)),
	    pixmap_width_in_pixels, pixmap_height_in_pixels, 1);

	values.foreground = 1;
	values.background = 0;
	values.dashes = 1;
	values.dash_offset = 0;
	values.line_style = LineOnOffDash;

	draw_gc = XCreateGC(XtDisplay(w), big_picture,
	    GCForeground | GCBackground | GCDashOffset | GCDashList | GCLineStyle, &values);

	values.foreground = 0;
	values.background = 1;
	undraw_gc = XCreateGC(XtDisplay(w), big_picture,
	    GCForeground | GCBackground | GCDashOffset | GCDashList | GCLineStyle, &values);

	values.foreground = app_data.copy_fg;
	values.background = app_data.copy_bg;
	copy_gc = XCreateGC(XtDisplay(w), RootWindowOfScreen(XtScreen(w)),
	    GCForeground | GCBackground, &values);

	/* draw permanent grid into pixmap */
	n_horiz_segments = app_data.pixmap_height_in_cells + 1;
	n_vert_segments = app_data.pixmap_width_in_cells + 1;

	for (x = 0; x < n_horiz_segments; x += 1) {
		segment[x].x1 = 0;
		segment[x].x2 = pixmap_width_in_pixels;
		segment[x].y1 = app_data.cell_size_in_pixels * x;
		segment[x].y2 = app_data.cell_size_in_pixels * x;
	}

	/* drawn only once into pixmap */
	XDrawSegments(XtDisplay(w), big_picture, draw_gc, segment, n_horiz_segments);

	for (y = 0; y < n_vert_segments; y += 1) {
		segment[y].x1 = y * app_data.cell_size_in_pixels;
		segment[y].x2 = y * app_data.cell_size_in_pixels;
		segment[y].y1 = 0;
		segment[y].y2 = pixmap_height_in_pixels;
	}

	/* drawn only once into pixmap */
	XDrawSegments(XtDisplay(w), big_picture, draw_gc, segment, n_vert_segments);
}

/* Action */
static void
RedrawPicture(w, event)
Widget w;
XExposeEvent *event;
{
    register int x, y;
    unsigned int width, height;

    if (event) {	/* drawing because of expose or button press */
	x = event->x;
        y = event->y; 
	width = event->width;
	height =  event->height;
    } 
    else {	/* drawing because of scrolling */
	x = 0;
        y = 0; 
	width =  10000;  /* always the whole window! */
	height =  10000;
    }

    if (DefaultDepthOfScreen(XtScreen(w)) == 1)
	XCopyArea(XtDisplay(w), big_picture, XtWindow(w),
		copy_gc, x + cur_x, 
		y + cur_y, width, height, x, y);
    else
	XCopyPlane(XtDisplay(w), big_picture, XtWindow(w),
		copy_gc, x + cur_x, 
		y + cur_y, width, height, x, y, 1);
}

/* Action */
static void
DrawCell(w, event)
Widget w;
XButtonEvent *event;
{
    DrawPixmaps(draw_gc, DRAW, w, event);
}

/* Action */
static void
UndrawCell(w, event)
Widget w;
XButtonEvent *event;
{
    DrawPixmaps(undraw_gc, UNDRAW, w, event);
}

/* Action */
static void
ToggleCell(w, event)
Widget w;
XButtonEvent *event;
{
    static int oldx = -1, oldy = -1;
    GC gc;
    int mode;
    int newx = (cur_x + event->x) / app_data.cell_size_in_pixels;
    int newy = (cur_y + event->y) / app_data.cell_size_in_pixels;

    if ((mode = cell[newx + newy * app_data.pixmap_width_in_cells]) == DRAWN) {
        gc = undraw_gc;
	mode = UNDRAW;
    }
    else {
        gc = draw_gc;
	mode = DRAW;
    }

    if (oldx != newx || oldy != newy) {
        oldx = newx;
        oldy = newy;
        DrawPixmaps(gc, mode, w, event);
    } 
}

/* Private Function */
static void
DrawPixmaps(gc, mode, w, event)
GC gc;
int mode;
Widget w;
XButtonEvent *event;
{
    int newx = (cur_x + event->x) / app_data.cell_size_in_pixels;
    int newy = (cur_y + event->y) / app_data.cell_size_in_pixels;
    XExposeEvent fake_event;

	/* if already done, return */
    if (cell[newx + newy * app_data.pixmap_width_in_cells] == mode) 
        return;

    XFillRectangle(XtDisplay(w), big_picture, gc,
      	app_data.cell_size_in_pixels*newx + 2, app_data.cell_size_in_pixels*newy + 2, 
	(unsigned int)app_data.cell_size_in_pixels - 3, (unsigned int)app_data.cell_size_in_pixels - 3);

    cell[newx + newy * app_data.pixmap_width_in_cells] = mode;

    fake_event.x = app_data.cell_size_in_pixels * newx - cur_x;
    fake_event.y = app_data.cell_size_in_pixels * newy - cur_y;
    fake_event.width = app_data.cell_size_in_pixels;
    fake_event.height = app_data.cell_size_in_pixels;

    RedrawPicture(bitmap, &fake_event);
}
