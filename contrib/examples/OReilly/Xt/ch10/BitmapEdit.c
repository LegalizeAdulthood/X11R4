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

#include <X11/Xatom.h>

#ifdef X11R3
/* this include file was not provided in R3 */
#else /* R4 or later */
#include <X11/Xmu/Atoms.h>
#endif /* X11R3 */

#include "BitmapEdiP.h"

/* R3 did not provide the Atoms.h include file necessary for
 * using the Xmu Atom caching routines.  Therefore, the
 * following is needed in R3. */
#ifndef XA_TARGETS
typedef struct _AtomRec *AtomPtr;
extern AtomPtr _XA_TARGETS;
#define XA_TARGETS(d)           XmuInternAtom(d, _XA_TARGETS)
#endif /* XA_TARGETS */

#define INTERNAL_WIDTH	2
#define INTERNAL_HEIGHT 4

#define DEFAULT_PIXMAP_WIDTH	32	/* in cells */
#define DEFAULT_PIXMAP_HEIGHT	32	/* in cells */

#define DEFAULT_CELL_SIZE	30	/* in pixels */

/* values for instance variable is_drawn, and for char elements
 * in cell state array. */
#define DRAWN 'y'
#define UNDRAWN 'n'

/* modes for drawing */
#define DRAW 'y'
#define UNDRAW 'n'

#define MAXLINES 1000	/* max of horiz or vertical cells */
#define SCROLLBARWIDTH 15


#define offset(field) XtOffset(BitmapEditWidget, field)

static XtResource resources[] = {
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	offset(bitmapEdit.foreground), XtRString, XtDefaultForeground },
    {XtNselectionForeground, XtCSelectionForeground, XtRPixel, sizeof(Pixel),
	offset(bitmapEdit.selectionForeground), XtRString, "red" },
    {XtNcallback, XtCCallback, XtRCallback, sizeof(caddr_t),
	offset(bitmapEdit.callback), XtRCallback, NULL},
    {XtNcellSizeInPixels, XtCCellSizeInPixels, XtRInt, sizeof(int),
	offset(bitmapEdit.cell_size_in_pixels), XtRImmediate, (caddr_t)DEFAULT_CELL_SIZE },
    {XtNpixmapWidthInCells, XtCPixmapWidthInCells, XtRDimension, sizeof(Dimension),
	offset(bitmapEdit.pixmap_width_in_cells), XtRImmediate, (caddr_t)DEFAULT_PIXMAP_WIDTH },
    {XtNpixmapHeightInCells, XtCPixmapHeightInCells, XtRDimension, sizeof(Dimension),
	offset(bitmapEdit.pixmap_height_in_cells), XtRImmediate, (caddr_t)DEFAULT_PIXMAP_HEIGHT },
    {XtNcurX, XtCCurX, XtRInt, sizeof(int),
	offset(bitmapEdit.cur_x), XtRImmediate, (caddr_t) 0},
    {XtNcurY, XtCCurY, XtRInt, sizeof(int),
	offset(bitmapEdit.cur_y), XtRImmediate, (caddr_t) 0},
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

/* these Core methods not needed by BitmapEdit:
 *
 * static XtProc ClassInitialize();
 * static XtRealizeProc Realize();
 * static void ClassInitialize();
 * static void Realize();
 */

/* the following are private functions unique to BitmapEdit */
static void DrawPixmaps(), HighlightCell(), draw_box(), DrawCell();

/* private functions for selections */
static void lose_ownership_proc(), transfer_done_proc(), requestor_callback();
static Boolean convert_proc();

/* the following are actions of BitmapEdit */
static void DoCell(), UndoCell(), ToggleCell();
static void TopLeft(), BottomRight(), DragHighlight();
static void PasteSelection();

/* The following are public functions of BitmapEdit, declared extern
 * in the public include file: */
char *BitmapEditGetArrayString(); 

static char defaultTranslations[] =
	"Shift<Btn1Down>:    TopLeft()          \n\
         Shift<Btn1Motion>:  DragHighlight()    \n\
         Shift<Btn1Up>:      BottomRight()      \n\
	 Shift<Btn2Down>:    PasteSelection()   \n\
	 ~Shift<Btn1Down>:    DoCell()          \n\
         ~Shift<Btn2Down>:    UndoCell()        \n\
         ~Shift<Btn3Down>:    ToggleCell()      \n\
         ~Shift<Btn1Motion>:  DoCell()          \n\
         ~Shift<Btn2Motion>:  UndoCell()        \n\
         ~Shift<Btn3Motion>:  ToggleCell()";

static XtActionsRec actions[] = {
        {"DoCell", DoCell},
        {"UndoCell", UndoCell},
        {"ToggleCell", ToggleCell},
        {"TopLeft", TopLeft},
        {"DragHighlight", DragHighlight},
        {"BottomRight", BottomRight},
        {"PasteSelection", PasteSelection},
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
    /* query_geometry		 */ NULL,
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
GetXorGC(cw)
BitmapEditWidget cw;
{
    XGCValues values;
    XtGCMask mask = GCForeground | GCFunction | GCLineWidth;

    values.line_width = 3;
    values.foreground = cw->bitmapEdit.selectionForeground;
    values.function = GXxor;

    cw->bitmapEdit.xor_gc = XtGetGC(cw, mask, &values);
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
    GetXorGC(new);

    DrawIntoBigPixmap(new);

    new->bitmapEdit.target_atom = XInternAtom(XtDisplay(new), "CELL_ARRAY", False);

    /* This makes Xmu initialize the necessary atoms.
     * Wierd, but true. */
    (void) XmuInternAtom( XtDisplay(new), XmuMakeAtom("NULL") );

    new->bitmapEdit.data = XtCalloc(
		new->bitmapEdit.pixmap_width_in_cells * 
		new->bitmapEdit.pixmap_height_in_cells, sizeof(char));
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

    if (event) {  /* called from btn-event */
	x = event->x;
        y = event->y; 
	width = event->width;
	height =  event->height;
    } 
    else {        /* called because of expose */
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
    if (cw->bitmapEdit.first_box)
        draw_box(cw, cw->bitmapEdit.select_end_x, cw->bitmapEdit.select_end_y); 
}

/* ARGSUSED */
static Boolean
SetValues(current, request, new)
Widget current, request, new;
{
    BitmapEditWidget curcw = (BitmapEditWidget) current;
    BitmapEditWidget newcw = (BitmapEditWidget) new;
    Boolean do_redisplay = False;

/*    
    if (curcw->core.sensitive != newcw->core.sensitive ||
	curcw->bitmapEdit.checked != newcw->bitmapEdit.checked ||
	curcw->bitmapEdit.fancy != newcw->bitmapEdit.fancy)
	do_redisplay = True;
*/

    if (curcw->bitmapEdit.foreground != newcw->bitmapEdit.foreground) {
	XtReleaseGC(curcw, curcw->bitmapEdit.copy_gc);
	GetCopyGC(newcw);
	do_redisplay = True;
    }

    if ((curcw->bitmapEdit.cur_x != newcw->bitmapEdit.cur_x) || 
		(curcw->bitmapEdit.cur_y != newcw->bitmapEdit.cur_y)) {
	do_redisplay = True;
    }

    if ((curcw->bitmapEdit.pixmap_width_in_cells != newcw->bitmapEdit.pixmap_width_in_cells) || 
		(curcw->bitmapEdit.pixmap_height_in_cells != newcw->bitmapEdit.pixmap_height_in_cells) || 
		(curcw->bitmapEdit.cell_size_in_pixels != newcw->bitmapEdit.cell_size_in_pixels))
	XtWarning("BitmapEdit widget: pixmap_width_in_cells, pixmap_heigt, and cell_size_in_pixels resources can only be set before widget is created\n");

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

    XtFree(cw->bitmapEdit.cell);
}

static void
DoCell(w, event)
BitmapEditWidget w;
XEvent *event;
{
    DrawPixmaps(w->bitmapEdit.draw_gc, DRAW, w, event);
}

static void
UndoCell(w, event)
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
    int newx, newy;

    /* This is strictly correct, but doesn't
     * seem to be necessary */
    if (event->type == ButtonPress) {
        newx = (w->bitmapEdit.cur_x + ((XButtonEvent *)event)->x) / w->bitmapEdit.cell_size_in_pixels;
        newy = (w->bitmapEdit.cur_y + ((XButtonEvent *)event)->y) / w->bitmapEdit.cell_size_in_pixels;
    }
    else  {
        newx = (w->bitmapEdit.cur_x + ((XMotionEvent *)event)->x) / w->bitmapEdit.cell_size_in_pixels;
        newy = (w->bitmapEdit.cur_y + ((XMotionEvent *)event)->y) / w->bitmapEdit.cell_size_in_pixels;
        if (oldx == newx && oldy == newy)
		return;
    }

    if (w->bitmapEdit.cell[newx + newy * w->bitmapEdit.pixmap_width_in_cells] == DRAWN)
    	DrawPixmaps(w->bitmapEdit.undraw_gc, UNDRAW, w, event);
    else
    	DrawPixmaps(w->bitmapEdit.draw_gc, DRAW, w, event);

    if (event->type == MotionNotify) {
	oldx = newx;
	oldy = newy;
    }
}

static void
DrawPixmaps(gc, mode, w, event)
GC gc;
char mode;
BitmapEditWidget w;
XButtonEvent *event;
{
    int newx = (w->bitmapEdit.cur_x + event->x) / w->bitmapEdit.cell_size_in_pixels;
    int newy = (w->bitmapEdit.cur_y + event->y) / w->bitmapEdit.cell_size_in_pixels;
    XExposeEvent fake_event;

	/* if already done, return */
    if (w->bitmapEdit.cell[newx + newy * w->bitmapEdit.pixmap_width_in_cells] != mode) {
    	DrawCell(w, newx, newy, gc); 
        w->bitmapEdit.cell[newx + newy * w->bitmapEdit.pixmap_width_in_cells] = mode;
    }

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

static void
DrawCell(w, x, y, gc)
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

CreateBigPixmap(cw)
BitmapEditWidget cw;
{
    /* always a 1 bit deep pixmap, regardless of screen depth */
    cw->bitmapEdit.big_picture = XCreatePixmap(XtDisplay(cw),
            RootWindow(XtDisplay(cw), DefaultScreen(XtDisplay(cw))),
            cw->bitmapEdit.pixmap_width_in_pixels, cw->bitmapEdit.pixmap_height_in_pixels, 1);
}

DrawIntoBigPixmap(cw)
BitmapEditWidget cw;
{
	int n_horiz_segments, n_vert_segments;
	XSegment segment[MAXLINES];
	register int x, y;

	n_horiz_segments = cw->bitmapEdit.pixmap_height_in_cells + 1;
	n_vert_segments = cw->bitmapEdit.pixmap_width_in_cells + 1;

	for (x = 0; x < n_horiz_segments; x += 1) {
		segment[x].x1 = 0;
		segment[x].x2 = cw->bitmapEdit.pixmap_width_in_pixels;
		segment[x].y1 = cw->bitmapEdit.cell_size_in_pixels * x;
		segment[x].y2 = cw->bitmapEdit.cell_size_in_pixels * x;
	}

	XDrawSegments(XtDisplay(cw), cw->bitmapEdit.big_picture, cw->bitmapEdit.draw_gc, segment, n_horiz_segments);

	for (y = 0; y < n_vert_segments; y += 1) {
		segment[y].x1 = y * cw->bitmapEdit.cell_size_in_pixels;
		segment[y].x2 = y * cw->bitmapEdit.cell_size_in_pixels;
		segment[y].y1 = 0;
		segment[y].y2 = cw->bitmapEdit.pixmap_height_in_pixels;
	}

	XDrawSegments(XtDisplay(cw), cw->bitmapEdit.big_picture, cw->bitmapEdit.draw_gc, segment, n_vert_segments);
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
    /*
     * This takes care of the case where the widget is resized larger
     * but there is no more pixmap to display in the bottom right corner.
     * in that case, we need to move cur_x and cur_y to make sure that
     * the pixmap fills as much of the window as possible, and fill 
     * the remainder of the window with gray, or draw a line around it.
     */
    if (cw->bitmapEdit.cur_x + cw->core.width > cw->bitmapEdit.pixmap_width_in_pixels)
	cw->bitmapEdit.cur_x = cw->bitmapEdit.pixmap_width_in_pixels - cw->core.width;
    if (cw->bitmapEdit.cur_y + cw->core.height > cw->bitmapEdit.pixmap_height_in_pixels)
	cw->bitmapEdit.cur_y = cw->bitmapEdit.pixmap_height_in_pixels - cw->core.height;

    /* Can't clear window is it hasn't been realized yet,
     * as would happen if -geometry option is specified. */
    if (XtIsRealized(cw))
    /* now force redraw by clearing window */
        XClearArea(XtDisplay(cw), XtWindow(cw), 0, 0, cw->bitmapEdit.pixmap_width_in_pixels, cw->bitmapEdit.pixmap_height_in_pixels, True);
}

static void
TopLeft(w, event)
BitmapEditWidget w;
XButtonEvent *event;
{
	w->bitmapEdit.first_box = False;

	w->bitmapEdit.select_start_x = (w->bitmapEdit.cur_x + event->x) / w->bitmapEdit.cell_size_in_pixels;
	w->bitmapEdit.select_start_y = (w->bitmapEdit.cur_y + event->y) / w->bitmapEdit.cell_size_in_pixels;

	/* clear old selection */
	Redisplay(w, NULL);
}

/* ARGSUSED */
static void
transfer_done_proc(w, selection, target)
Widget w;
Atom *selection;
Atom *target;
{
	/* 
	 * This widget keeps the selection ready for pasting additional
	 * times.  Having a transfer_done_proc indicates that the
	 * selection owner owns the storage allocated for
	 * converting the selection.  However, this widget allocates
	 * this storage only once in initialize, and keeps it until
	 * quitting.  Therefore, no \f(CWXtFree\fP call necessary here.
	 */
}

static void
BottomRight(w, event)
BitmapEditWidget w;
XButtonEvent *event;
{
	int temp;

	w->bitmapEdit.select_end_x = (w->bitmapEdit.cur_x + event->x) / w->bitmapEdit.cell_size_in_pixels;
	w->bitmapEdit.select_end_y = (w->bitmapEdit.cur_y + event->y) / w->bitmapEdit.cell_size_in_pixels;

	if ((w->bitmapEdit.select_end_x == w->bitmapEdit.select_start_x) && 
			(w->bitmapEdit.select_end_y == w->bitmapEdit.select_start_y))  {
		Redisplay(w, NULL);
		return;	/* no selection */
	}

	/* swap start and end if end is greater than start */
	if (w->bitmapEdit.select_end_x < w->bitmapEdit.select_start_x) {
		temp = w->bitmapEdit.select_end_x;
		w->bitmapEdit.select_end_x = w->bitmapEdit.select_start_x;
		w->bitmapEdit.select_start_x = temp;
	}

	if (w->bitmapEdit.select_end_y < w->bitmapEdit.select_start_y) {
		temp = w->bitmapEdit.select_end_y;
		w->bitmapEdit.select_end_y = w->bitmapEdit.select_start_y;
		w->bitmapEdit.select_start_y = temp;
	}

	if (XtOwnSelection(w, XA_PRIMARY, event->time, convert_proc, 
			lose_ownership_proc, transfer_done_proc) == False) {
		XtWarning("bitmapEdit: failed attempting to become selection owner; make a new selection.\n");
		/* Clear old selection, because lose_ownership_proc 
		 * isn't registered. */
		Redisplay(w, NULL); 
	}
}

static Boolean
convert_proc(w, selection, target, type_return, value_return, length_return, format_return)
BitmapEditWidget w;
Atom *selection;
Atom *target;
Atom *type_return;
caddr_t *value_return;
unsigned long *length_return;
int *format_return;
{
	int x, y;
	int width, height;

	/* handle all required atoms, and the one that we use */
	if (*target == XA_TARGETS(XtDisplay(w))) {
		/* TARGETS handling copied from xclipboard.c */
		Atom* targetP;
		Atom* std_targets;
		unsigned long std_length;
		XmuConvertStandardSelection(w, CurrentTime, selection, 
				target, type_return,
   				(caddr_t*)&std_targets, 
				&std_length, format_return);
		*value_return = XtMalloc(sizeof(Atom)*(std_length + 1));
		targetP = *(Atom**)value_return;
		*length_return = std_length + 1;
		*targetP++ = w->bitmapEdit.target_atom;
		bcopy((char*)std_targets, (char*)targetP, sizeof(Atom)*std_length);
		XtFree((char*)std_targets);
		*type_return = XA_ATOM;
		*format_return = sizeof(Atom) * 8;
		return(True);
	}
	/* Required by ICCCM, but I think Xt already does 
	 * what's necessary with this.
	else if (*target == w->bitmapEdit.MULTIPLE) {
		return(False);
	}
	*/
	else if (*target == w->bitmapEdit.target_atom) {
		char *data;

		width = w->bitmapEdit.select_end_x - w->bitmapEdit.select_start_x;
		height = w->bitmapEdit.select_end_y - w->bitmapEdit.select_start_y;

		/* 8 chars is enough for two 3-digit numbers and two delimiters */
		*length_return = ((width * height) + 8) * sizeof(char);
			
		data = XtMalloc(*length_return);

		sprintf(data, "%d@%d~", width, height);

		for (x = 0; x < width; x++) {
			for (y = 0; y < height; y++) {
				data[8 + x + (y * width)] = w->bitmapEdit.cell[(x + w->bitmapEdit.select_start_x) + ((y + w->bitmapEdit.select_start_y) * w->bitmapEdit.pixmap_width_in_cells)];
			}
		}

		*value_return = data;

		*type_return = w->bitmapEdit.target_atom;

		*format_return = 8;  /* number of bits in char */
		return(True);
	}
	else { 
 		if (XmuConvertStandardSelection(w, CurrentTime, selection, 
				target, type_return, value_return, 
				length_return, format_return))
    			return True;
		else {
			XtWarning("bitmapEdit: requestor is requesting",
					 "unsupported selection target type.\n");
			return(False);
		}
	}
}

/* ARGSUSED */
static void
lose_ownership_proc(w, selection)
BitmapEditWidget w;
Atom *selection;
{
	/* clear old selection */
	w->bitmapEdit.first_box = False;
	w->bitmapEdit.select_start_x = 0;
	w->bitmapEdit.select_start_y = 0;
	w->bitmapEdit.select_end_x = 0;
	w->bitmapEdit.select_end_y = 0;
	Redisplay(w, NULL);
}


static void
PasteSelection(w, event)
BitmapEditWidget w;
XButtonEvent *event;
{
	/* 
	 * Note: the actual pasting takes place in requestor_callback.
	 * This is a callback because the owner and requestor roles 
	 * may not be in the same widget.
	 */
	XtGetSelectionValue(w, XA_PRIMARY, w->bitmapEdit.target_atom, 
			requestor_callback, event, 
			event->time);
	/* Sony is confused...works better but not great with time = CurrentTime */
}

/* ARGSUSED */
static void
requestor_callback(w, event, selection, type, value, length, format)
BitmapEditWidget w;
XButtonEvent *event;	/* client_data, to pass press position */
Atom *selection;
Atom *type;
caddr_t value;
unsigned long *length;
int *format;
{
	if ((*type = 0 /* XT_CONVERT_FAIL */) || (*length == 0)) {
		XBell(XtDisplay(w), 100);
		XtWarning("bitmapEdit: no selection or selection timed out; try again\n");
	}
	else {
		int width, height;
		int x, y;
		int dst_offset_x, dst_offset_y;
		char *ptr;

		dst_offset_x = (w->bitmapEdit.cur_x + event->x) / w->bitmapEdit.cell_size_in_pixels;
		dst_offset_y = (w->bitmapEdit.cur_y + event->y) / w->bitmapEdit.cell_size_in_pixels;

		printf("dst offset is %d, %d\n", dst_offset_x, dst_offset_y);
		printf("time is %d\n", event->time);

		ptr = value;
		width = atoi(ptr);
		ptr = index(ptr, '@');
		ptr++;
		height = atoi(ptr);
		ptr = &value[8];


		for (x = 0; x < width; x++) {
			for (y = 0; y < height; y++) {
				/* range checking */
				if (((dst_offset_x + x) > 
					   w->bitmapEdit.pixmap_width_in_cells) 
					   || ((dst_offset_x + x) < 0))
					break;
				if (((dst_offset_y + y) > 
					   w->bitmapEdit.pixmap_height_in_cells) 
					   || ((dst_offset_y + y) < 0))
					break;
				w->bitmapEdit.cell[(dst_offset_x + x) 
					   + ((dst_offset_y + y) * 
					   w->bitmapEdit.pixmap_width_in_cells)] 
					   = ptr[x + (y * width)];
    				if (w->bitmapEdit.cell[(dst_offset_x + x) 
					   + ((dst_offset_y + y) * 
					   w->bitmapEdit.pixmap_width_in_cells)] 
					   == DRAWN)
        				
					DrawCell(w, dst_offset_x + x, 
						dst_offset_y + y, 
						w->bitmapEdit.draw_gc);
    				else
					DrawCell(w, dst_offset_x + x, 
						dst_offset_y + y, 
						w->bitmapEdit.undraw_gc);
        				
			}
		}
		/* Regardless of the presence of a 
		 * \f(CWtransfer_done_proc\fP in the owner, 
		 * the requestor must free the data passed by 
		 * Xt after using it. */
		XtFree(value);
		Redisplay(w, NULL);
	}
}

static void
DragHighlight(w, event)
BitmapEditWidget w;
XMotionEvent *event;
{
	static int last_drawn_x, last_drawn_y;
	int event_cell_x, event_cell_y;

	event_cell_x = w->bitmapEdit.cur_x + (event->x / w->bitmapEdit.cell_size_in_pixels);
	event_cell_y = w->bitmapEdit.cur_y + (event->y / w->bitmapEdit.cell_size_in_pixels);

	if ((event_cell_x == last_drawn_x) && (event_cell_y == last_drawn_y))
		return;

	if (w->bitmapEdit.first_box) {
		draw_box(w, last_drawn_x, last_drawn_y);  /* undraws */
		draw_box(w, event_cell_x, event_cell_y);
	}
	else {
		draw_box(w, event_cell_x, event_cell_y);
		w->bitmapEdit.first_box = True;
	}

	last_drawn_x = event_cell_x;
	last_drawn_y = event_cell_y;
}

static void
draw_box(w, x, y)
BitmapEditWidget w;
Position x, y;
{
	Position start_pos_x, start_pos_y;
	Dimension width, height;

	start_pos_x = w->bitmapEdit.cur_x + w->bitmapEdit.select_start_x;
	start_pos_y = w->bitmapEdit.cur_x + w->bitmapEdit.select_start_y;
	
	/* swap start and end if end is greater than start */
	if (x < start_pos_x) {
		width = start_pos_x - x;
		start_pos_x = x;
	}
	else {
		width = x - start_pos_x;
	}

	if (y < start_pos_y) {
		height = start_pos_y - y;
		start_pos_y = y;
	}
	else {
		height = y - start_pos_y;
	}

	XDrawRectangle(XtDisplay(w), XtWindow(w), w->bitmapEdit.xor_gc,
    		(start_pos_x * w->bitmapEdit.cell_size_in_pixels) - 1,
		(start_pos_y * w->bitmapEdit.cell_size_in_pixels) - 1, 
		(unsigned int) width * w->bitmapEdit.cell_size_in_pixels + 2,
		(unsigned int) height * w->bitmapEdit.cell_size_in_pixels + 2);
}
