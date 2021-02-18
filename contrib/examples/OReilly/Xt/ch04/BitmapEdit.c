/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * BitmapEdit.c - bitmap editor widget.
 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <stdio.h>

#include "BitmapEdiP.h"

#define INTERNAL_WIDTH    2
#define INTERNAL_HEIGHT 4

#define DEFAULT_PIXMAP_WIDTH	32	/* in cells */
#define DEFAULT_PIXMAP_HEIGHT	32	/* in cells */

#define DEFAULT_CELL_SIZE	30	/* in pixels */

/* values for instance variable is_drawn */
#define DRAWN 1
#define UNDRAWN 0

/* modes for drawing */
#define DRAW 1
#define UNDRAW 0

#define MAXLINES 1000	/* max of horiz or vertical cells */
#define SCROLLBARWIDTH 15

#define offset(field) XtOffset(BitmapEditWidget, field)

static XtResource resources[] = {
     {
	XtNforeground, 
	XtCForeground, 
	XtRPixel, 
	sizeof(Pixel),
	offset(bitmapEdit.foreground), 
	XtRString, 
	XtDefaultForeground
     },
     {
	XtNcallback, 
	XtCCallback, 
	XtRCallback, 
	sizeof(caddr_t),
	offset(bitmapEdit.callback), 
	XtRCallback, 
	NULL
     },
     {
	XtNcellSizeInPixels, 
	XtCCellSizeInPixels, 
	XtRInt, sizeof(int),
	offset(bitmapEdit.cell_size_in_pixels), 
	XtRImmediate, 
	(caddr_t)DEFAULT_CELL_SIZE
     },
     {
	XtNpixmapWidthInCells, 
	XtCPixmapWidthInCells, 
	XtRDimension, 
	sizeof(Dimension),
	offset(bitmapEdit.pixmap_width_in_cells), 
	XtRImmediate, 
	(caddr_t)DEFAULT_PIXMAP_WIDTH
     },
     {
	XtNpixmapHeightInCells, 
	XtCPixmapHeightInCells, 
	XtRDimension, 
	sizeof(Dimension),
	offset(bitmapEdit.pixmap_height_in_cells), 
	XtRImmediate, 
	(caddr_t)DEFAULT_PIXMAP_HEIGHT
     },
     {
	XtNcurX, 
	XtCCurX, 
	XtRInt, 
	sizeof(int),
	offset(bitmapEdit.cur_x), 
	XtRImmediate, 
	(caddr_t) 0
     },
     {
	XtNcurY, 
	XtCCurY, 
	XtRInt, 
	sizeof(int),
	offset(bitmapEdit.cur_y), 
	XtRString, 
	(caddr_t) NULL
     },
     {
	XtNcellArray, 
	XtCCellArray, 
	XtRString, 
	sizeof(String),
	offset(bitmapEdit.cell), 
	XtRImmediate, 
	(caddr_t) 0
     },
};

/* Declaration of methods */

/* 
 * These don't work under SunOS 4.0.1 and perhaps others.
 * static XtInitProc Initialize();
 * static XtExposeProc Redisplay();
 * static XtWidgetProc Destroy();
 * static XtWidgetProc Resize();
 * static XtSetValuesFunc SetValues();
 */

static void Initialize();
static void Redisplay();
static void Destroy();
static void Resize();
static Boolean SetValues();
static XtGeometryResult QueryGeometry();

/* these Core methods not needed by BitmapEdit:
 *
 * static XtProc ClassInitialize();
 * static XtRealizeProc Realize();
 * static void ClassInitialize();
 * static void Realize();
 */

/* the following are private functions unique to BitmapEdit */
static void DrawPixmaps(), DoCell(), ChangeCellSize();

/* the following are actions of BitmapEdit */
static void DrawCell(), UndrawCell(), ToggleCell();

/* The following are public functions of BitmapEdit, declared extern
 * in the public include file: */
char *BitmapEditGetArrayString(); 

static char defaultTranslations[] =
	"<Btn1Down>:    DrawCell()              \n\
         <Btn2Down>:    UndrawCell()            \n\
         <Btn3Down>:    ToggleCell()            \n\
         <Btn1Motion>:  DrawCell()              \n\
         <Btn2Motion>:  UndrawCell()            \n\
         <Btn3Motion>:  ToggleCell()";

static XtActionsRec actions[] = {
        {"DrawCell", DrawCell},
        {"UndrawCell", UndrawCell},
        {"ToggleCell", ToggleCell},
};

/* definition in BitmapEdit.h */
static BitmapEditPointInfo info;

BitmapEditClassRec bitmapEditClassRec = {
    {
    /* core_class fields */
    /* superclass	  	 */ (WidgetClass) &widgetClassRec,
    /* class_name	  	 */ "BitmapEdit",
    /* widget_size	  	 */ sizeof(BitmapEditRec),
    /* class_initialize   	 */ NULL,
    /* class_part_initialize	 */ NULL,
    /* class_inited       	 */ FALSE,
    /* initialize	  	 */ Initialize,
    /* initialize_hook		 */ NULL,
    /* realize		  	 */ XtInheritRealize,
    /* actions		  	 */ actions,
    /* num_actions	  	 */ XtNumber(actions),
    /* resources	  	 */ resources,
    /* num_resources	  	 */ XtNumber(resources),
    /* xrm_class	  	 */ NULLQUARK,
    /* compress_motion	  	 */ TRUE,
    /* compress_exposure  	 */ TRUE,
    /* compress_enterleave	 */ TRUE,
    /* visible_interest	  	 */ FALSE,
    /* destroy		  	 */ Destroy,
    /* resize		  	 */ Resize,
    /* expose		  	 */ Redisplay,
    /* set_values	  	 */ SetValues,
    /* set_values_hook		 */ NULL,
    /* set_values_almost	 */ XtInheritSetValuesAlmost,
    /* get_values_hook		 */ NULL,
    /* accept_focus	 	 */ NULL,
    /* version			 */ XtVersion,
    /* callback_private   	 */ NULL,
    /* tm_table		   	 */ defaultTranslations,
    /* query_geometry		 */ QueryGeometry,
    /* display_accelerator       */ XtInheritDisplayAccelerator,
    /* extension                 */ NULL
    },
    {
    /* dummy_field               */ 0,
    },
};

WidgetClass bitmapEditWidgetClass = (WidgetClass) & bitmapEditClassRec;

static void
GetDrawGC(cw)
BitmapEditWidget cw;
{
    XGCValues values;
    XtGCMask mask = GCForeground | GCBackground | GCDashOffset | GCDashList | GCLineStyle;

    /* 
     * Setting foreground and background to 1 and 0 looks like a 
     * kludge but isn't.  This GC is used for drawing
     * into a pixmap of depth one.  Real colors are applied with a
     * separate GC when the pixmap is copied into the window.
     */
    values.foreground = 1;
    values.background = 0;
    values.dashes = 1;
    values.dash_offset = 0;
    values.line_style = LineOnOffDash;

    cw->bitmapEdit.draw_gc = XCreateGC(XtDisplay(cw), cw->bitmapEdit.big_picture, mask, &values);
}

static void
GetUndrawGC(cw)
BitmapEditWidget cw;
{
    XGCValues values;
    XtGCMask mask = GCForeground | GCBackground;

    /* this looks like a kludge but isn't.  This GC is used for drawing
     * into a pixmap of depth one.  Real colors are applied as the 
     * pixmap is copied into the window.
     */
    values.foreground = 0;
    values.background = 1;

    cw->bitmapEdit.undraw_gc = XCreateGC(XtDisplay(cw), cw->bitmapEdit.big_picture, mask, &values);
}

static void
GetCopyGC(cw)
BitmapEditWidget cw;
{
    XGCValues values;
    XtGCMask mask = GCForeground | GCBackground;

    values.foreground = cw->bitmapEdit.foreground;
    values.background = cw->core.background_pixel;

    cw->bitmapEdit.copy_gc = XtGetGC(cw, mask, &values);
}

/* ARGSUSED */
static void
Initialize(request, new)
BitmapEditWidget request, new;
{
    new->bitmapEdit.cur_x = 0;
    new->bitmapEdit.cur_y = 0;

    /* 
     *  Check instance values set by resources that may be invalid. 
     */

    if ((new->bitmapEdit.pixmap_width_in_cells < 1) ||
		(new->bitmapEdit.pixmap_height_in_cells < 1))  {
	XtWarning("BitmapEdit: pixmapWidth and/or pixmapHeight is too small (using 10 x 10)."); 
    	new->bitmapEdit.pixmap_width_in_cells = 10;
    	new->bitmapEdit.pixmap_height_in_cells = 10;
    }

    if (new->bitmapEdit.cell_size_in_pixels < 5) {
	XtWarning("BitmapEdit: cellSize is too small (using 5)."); 
    	new->bitmapEdit.cell_size_in_pixels = 5;
    }

    if ((new->bitmapEdit.cur_x < 0) ||  (new->bitmapEdit.cur_y < 0)) {
	XtWarning("BitmapEdit: cur_x and cur_y must be non-negative (using 0, 0)."); 
    	new->bitmapEdit.cur_x = 0;
    	new->bitmapEdit.cur_y = 0;
    }

    if (new->bitmapEdit.cell == NULL)
        new->bitmapEdit.cell = XtCalloc(new->bitmapEdit.pixmap_width_in_cells * new->bitmapEdit.pixmap_height_in_cells, sizeof(char));

    new->bitmapEdit.pixmap_width_in_pixels = new->bitmapEdit.pixmap_width_in_cells * new->bitmapEdit.cell_size_in_pixels;

    new->bitmapEdit.pixmap_height_in_pixels = new->bitmapEdit.pixmap_height_in_cells * new->bitmapEdit.cell_size_in_pixels;

    if (new->core.width == 0)
	new->core.width = (new->bitmapEdit.pixmap_width_in_pixels > 300) ? 300 : (new->bitmapEdit.pixmap_width_in_pixels);
    if (new->core.height == 0)
	new->core.height = (new->bitmapEdit.pixmap_height_in_pixels > 300) ? 300 : (new->bitmapEdit.pixmap_height_in_pixels);

    CreateBigPixmap(new);

    GetDrawGC(new);
    GetUndrawGC(new);
    GetCopyGC(new);

    DrawIntoBigPixmap(new);
}

/* ARGSUSED */
static void
Redisplay(cw, event)
BitmapEditWidget cw;
XExposeEvent *event;
{
    register int x, y;
    unsigned int width, height;
    if (!XtIsRealized(cw))
	return;

    if (event) {  /* called from btn-event or expose */
	x = event->x;
        y = event->y; 
	width = event->width;
	height =  event->height;
    } 
    else {        /* called because complete redraw */
	x = 0;
        y = 0; 
	width = cw->bitmapEdit.pixmap_width_in_pixels;
	height = cw->bitmapEdit.pixmap_height_in_pixels;
    }

    if (DefaultDepthOfScreen(XtScreen(cw)) == 1)
	XCopyArea(XtDisplay(cw), cw->bitmapEdit.big_picture, XtWindow(cw),
	    cw->bitmapEdit.copy_gc, x + cw->bitmapEdit.cur_x, y + 
	    cw->bitmapEdit.cur_y, width, height, x, y);
    else
	XCopyPlane(XtDisplay(cw), cw->bitmapEdit.big_picture, XtWindow(cw),
	    cw->bitmapEdit.copy_gc, x + cw->bitmapEdit.cur_x, y + 
	    cw->bitmapEdit.cur_y, width, height, x, y, 1);

}

/* ARGSUSED */
static Boolean
SetValues(current, request, new)
Widget current, request, new;
{
    BitmapEditWidget curcw = (BitmapEditWidget) current;
    BitmapEditWidget newcw = (BitmapEditWidget) new;
    Boolean do_redisplay = False;

    if (curcw->bitmapEdit.foreground != newcw->bitmapEdit.foreground) {
	XtReleaseGC(curcw, curcw->bitmapEdit.copy_gc);
	GetCopyGC(newcw);
	do_redisplay = True;
    }

    if ((curcw->bitmapEdit.cur_x != newcw->bitmapEdit.cur_x) || 
		(curcw->bitmapEdit.cur_y != newcw->bitmapEdit.cur_y)) {
	do_redisplay = True;
    }

    if (curcw->bitmapEdit.cell_size_in_pixels != newcw->bitmapEdit.cell_size_in_pixels) {
	ChangeCellSize(curcw, newcw->bitmapEdit.cell_size_in_pixels);
	do_redisplay = True;
    }

    if (curcw->bitmapEdit.pixmap_width_in_cells != 
		newcw->bitmapEdit.pixmap_width_in_cells)  {
        newcw->bitmapEdit.pixmap_width_in_cells = curcw->bitmapEdit.pixmap_width_in_cells;
	XtWarning("BitmapEdit: pixmap_width_in_cells cannot be set by XtSetValues.\n");
    }

    if (curcw->bitmapEdit.pixmap_height_in_cells != 
		newcw->bitmapEdit.pixmap_height_in_cells) {
        newcw->bitmapEdit.pixmap_height_in_cells = curcw->bitmapEdit.pixmap_height_in_cells;
	XtWarning("BitmapEdit: pixmap_height_in_cells cannot be set by XtSetValues.\n");
    }

    return do_redisplay;
}


static void
Destroy(cw)
BitmapEditWidget cw;
{
    if (cw->bitmapEdit.big_picture)
	XFreePixmap(XtDisplay(cw), cw->bitmapEdit.big_picture);

    if (cw->bitmapEdit.draw_gc)
	XFreeGC(XtDisplay(cw), cw->bitmapEdit.draw_gc);

    if (cw->bitmapEdit.undraw_gc)
	XFreeGC(XtDisplay(cw), cw->bitmapEdit.undraw_gc);

    if (cw->bitmapEdit.copy_gc)
	XFreeGC(XtDisplay(cw), cw->bitmapEdit.copy_gc);

    /* NOTE!  This should only free when the application didn't 
     * allocate it.  Need to add another.  */
    XtFree(cw->bitmapEdit.cell);
}

static void
DrawCell(w, event)
BitmapEditWidget w;
XEvent *event;
{
    DrawPixmaps(w->bitmapEdit.draw_gc, DRAW, w, event);
}

static void
UndrawCell(w, event)
BitmapEditWidget w;
XEvent *event;
{
    DrawPixmaps(w->bitmapEdit.undraw_gc, UNDRAW, w, event);
}

static void
ToggleCell(w, event)
BitmapEditWidget w;
XEvent *event;
{
    static int oldx = -1, oldy = -1;
    GC gc;
    int mode;
    int newx, newy;

    /* This is strictly correct, but doesn't
     * seem to be necessary */
    if (event->type == ButtonPress) {
        newx = (w->bitmapEdit.cur_x + ((XButtonEvent *)event)->x) / 
		w->bitmapEdit.cell_size_in_pixels;
        newy = (w->bitmapEdit.cur_y + ((XButtonEvent *)event)->y) / 
		w->bitmapEdit.cell_size_in_pixels;
    }
    else  {
        newx = (w->bitmapEdit.cur_x + ((XMotionEvent *)event)->x) / 
		w->bitmapEdit.cell_size_in_pixels;
        newy = (w->bitmapEdit.cur_y + ((XMotionEvent *)event)->y) / 
		w->bitmapEdit.cell_size_in_pixels;
    }


    if ((mode = w->bitmapEdit.cell[newx + newy * w->bitmapEdit.pixmap_width_in_cells]) == DRAWN) {
        gc = w->bitmapEdit.undraw_gc;
	mode = UNDRAW;
    }
    else {
        gc = w->bitmapEdit.draw_gc;
	mode = DRAW;
    }

    if (oldx != newx || oldy != newy) {
        oldx = newx;
        oldy = newy;
        DrawPixmaps(gc, mode, w, event);
    } 
}

static void
DrawPixmaps(gc, mode, w, event)
GC gc;
int mode;
BitmapEditWidget w;
XButtonEvent *event;
{
    int newx = (w->bitmapEdit.cur_x + event->x) / w->bitmapEdit.cell_size_in_pixels;
    int newy = (w->bitmapEdit.cur_y + event->y) / w->bitmapEdit.cell_size_in_pixels;
    XExposeEvent fake_event;

	/* if already done, return */
    if (w->bitmapEdit.cell[newx + newy * w->bitmapEdit.pixmap_width_in_cells] == mode)
        return;

	/* otherwise, draw or undraw */
    XFillRectangle(XtDisplay(w), w->bitmapEdit.big_picture, gc,
    		w->bitmapEdit.cell_size_in_pixels*newx + 2, 
		w->bitmapEdit.cell_size_in_pixels*newy + 2, 
		(unsigned int)w->bitmapEdit.cell_size_in_pixels - 3, 
		(unsigned int)w->bitmapEdit.cell_size_in_pixels - 3);

    w->bitmapEdit.cell[newx + newy * w->bitmapEdit.pixmap_width_in_cells] = mode;
    info.mode = mode;
    info.newx = newx;
    info.newy = newy;

    fake_event.x = w->bitmapEdit.cell_size_in_pixels * newx - w->bitmapEdit.cur_x;
    fake_event.y = w->bitmapEdit.cell_size_in_pixels * newy - w->bitmapEdit.cur_y;
    fake_event.width = w->bitmapEdit.cell_size_in_pixels;
    fake_event.height = w->bitmapEdit.cell_size_in_pixels;

    Redisplay(w, &fake_event);
    XtCallCallbacks(w, XtNcallback, &info);
}

CreateBigPixmap(cw)
BitmapEditWidget cw;
{
    /* always a 1 bit deep pixmap, regardless of screen depth */
    cw->bitmapEdit.big_picture = XCreatePixmap(XtDisplay(cw),
            RootWindow(XtDisplay(cw), DefaultScreen(XtDisplay(cw))),
            cw->bitmapEdit.pixmap_width_in_pixels + 2, cw->bitmapEdit.pixmap_height_in_pixels + 2, 1);
}

DrawIntoBigPixmap(cw)
BitmapEditWidget cw;
{
	int n_horiz_segments, n_vert_segments;
	XSegment segment[MAXLINES];
	register int x, y;

	n_horiz_segments = cw->bitmapEdit.pixmap_height_in_cells + 1;
	n_vert_segments = cw->bitmapEdit.pixmap_width_in_cells + 1;

	for (x = 0; x < n_horiz_segments; x++) {
		segment[x].x1 = 0;
		segment[x].x2 = cw->bitmapEdit.pixmap_width_in_pixels;
		segment[x].y1 = cw->bitmapEdit.cell_size_in_pixels * x;
		segment[x].y2 = cw->bitmapEdit.cell_size_in_pixels * x;
	}

	XDrawSegments(XtDisplay(cw), cw->bitmapEdit.big_picture, cw->bitmapEdit.draw_gc, segment, n_horiz_segments);

	for (y = 0; y < n_vert_segments; y++) {
		segment[y].x1 = y * cw->bitmapEdit.cell_size_in_pixels;
		segment[y].x2 = y * cw->bitmapEdit.cell_size_in_pixels;
		segment[y].y1 = 0;
		segment[y].y2 = cw->bitmapEdit.pixmap_height_in_pixels;
	}

	XDrawSegments(XtDisplay(cw), cw->bitmapEdit.big_picture, cw->bitmapEdit.draw_gc, segment, n_vert_segments);

    /* draw current cell array into pixmap */
    for (x = 0; x < cw->bitmapEdit.pixmap_width_in_cells; x++) {
        for (y = 0; y < cw->bitmapEdit.pixmap_height_in_cells; y++) {
            if (cw->bitmapEdit.cell[x + (y * cw->bitmapEdit.pixmap_width_in_cells)] == DRAWN)
                DoCell(cw, x, y, cw->bitmapEdit.draw_gc);
            else
                DoCell(cw, x, y, cw->bitmapEdit.undraw_gc);
        }
    }
}

/* A Public function, not static */
char *
BitmapEditGetArrayString(w)
BitmapEditWidget w;
{
    return (w->bitmapEdit.cell);
}

/* ARGSUSED */
static void
Resize(cw)
BitmapEditWidget cw;
{
    /* resize does nothing unless new size is bigger than entire pixmap */
    if ((cw->core.width > cw->bitmapEdit.pixmap_width_in_pixels) &&
            (cw->core.height > cw->bitmapEdit.pixmap_height_in_pixels)) {
        /* 
         * Calculate the maximum cell size that will allow the
         * entire bitmap to be displayed.
         */
        Dimension w_temp_cell_size_in_pixels, h_temp_cell_size_in_pixels;
        Dimension new_cell_size_in_pixels;
    
        w_temp_cell_size_in_pixels = cw->core.width / cw->bitmapEdit.pixmap_width_in_cells;
        h_temp_cell_size_in_pixels = cw->core.height / cw->bitmapEdit.pixmap_height_in_cells;
    
        if (w_temp_cell_size_in_pixels < h_temp_cell_size_in_pixels)
            new_cell_size_in_pixels = w_temp_cell_size_in_pixels;
        else
            new_cell_size_in_pixels = h_temp_cell_size_in_pixels;
    
        /* if size change mandates a new pixmap, make one */
        if (new_cell_size_in_pixels != cw->bitmapEdit.cell_size_in_pixels)
            ChangeCellSize(cw, new_cell_size_in_pixels);
    }
}

static void
ChangeCellSize(cw, new_cell_size)
BitmapEditWidget cw;
int new_cell_size;
{
    int x, y;

    cw->bitmapEdit.cell_size_in_pixels = new_cell_size;

    /* recalculate variables based on cell size */
    cw->bitmapEdit.pixmap_width_in_pixels = cw->bitmapEdit.pixmap_width_in_cells * cw->bitmapEdit.cell_size_in_pixels;

    cw->bitmapEdit.pixmap_height_in_pixels = cw->bitmapEdit.pixmap_height_in_cells * cw->bitmapEdit.cell_size_in_pixels;
    
    /* destroy old and create new pixmap of correct size */
    XFreePixmap(XtDisplay(cw), cw->bitmapEdit.big_picture);
    CreateBigPixmap(cw);
    
    /* draw lines into new pixmap */
    DrawIntoBigPixmap(cw);
    
    /* draw current cell array into pixmap */
    for (x = 0; x < cw->bitmapEdit.pixmap_width_in_cells; x++) {
        for (y = 0; y < cw->bitmapEdit.pixmap_height_in_cells; y++) {
            if (cw->bitmapEdit.cell[x + (y * cw->bitmapEdit.pixmap_width_in_cells)] == DRAWN)
                DoCell(cw, x, y, cw->bitmapEdit.draw_gc);
            else
                DoCell(cw, x, y, cw->bitmapEdit.undraw_gc);
        }
    }
}

static void
DoCell(w, x, y, gc)
BitmapEditWidget w;
int x, y;
GC gc;
{
        /* otherwise, draw or undraw */
    XFillRectangle(XtDisplay(w), w->bitmapEdit.big_picture, gc,
                w->bitmapEdit.cell_size_in_pixels * x + 2,
                w->bitmapEdit.cell_size_in_pixels * y + 2,
                (unsigned int)w->bitmapEdit.cell_size_in_pixels - 3,
                (unsigned int)w->bitmapEdit.cell_size_in_pixels - 3);

}

static XtGeometryResult QueryGeometry(w, proposed, answer)
BitmapEditWidget w;
XtWidgetGeometry *proposed, *answer;
{
    answer->request_mode = CWWidth | CWHeight;

    /* initial width and height */
    answer->width = (w->bitmapEdit.pixmap_width_in_pixels > 300) 
		? 300 : w->bitmapEdit.pixmap_width_in_pixels;
    answer->height = (w->bitmapEdit.pixmap_height_in_pixels > 300) 
		? 300 : w->bitmapEdit.pixmap_height_in_pixels;

    if (  ((proposed->request_mode & (CWWidth | CWHeight))
                == (CWWidth | CWHeight)) &&
          proposed->width == answer->width &&
          proposed->height == answer->height)
        return XtGeometryYes;
    else if (answer->width == w->core.width &&
             answer->height == w->core.height)
        return XtGeometryNo;
    else
        return XtGeometryAlmost;
}

