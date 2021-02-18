/*
 *	Original Source: ./lib/Xaw/Viewport.c
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */
#ifndef lint
static char Xrcsid[] = "$XConsortium: Viewport.c,v 1.28 88/10/14 15:31:37 swick Exp $";
#endif lint

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/************************************************************
Copyright 1989 by Information-Technology Promotion Agency Japan (IPA),
Mitsubishi Research Institute, Inc. (MRI), and Fuji Xerox Co., Ltd. (FX)

                        All Rights Reserved

Permission to use, copy, modify and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IPA, MRI or FX not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IPA, MRI or FX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL IPA, MRI or FX BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

********************************************************/

#include <X11/IntrinsicP.h>
#include <X11/XawMisc.h>
#include <X11/Scroll.h>
#include <X11/StringDefs.h>
#include <St/StViewMgrP.h>

static void ScrollUpDownProc(), ThumbProc();

#define IsChildWidget(child, view)  ((child) != (view)->stview_manager.clip && \
                           (child) != (view)->stview_manager.frame && \
                           (child) != (view)->stview_manager.horiz_bar && \
                           (child) != (view)->stview_manager.vert_bar) 

static Boolean defFalse = False;

#define offset(field) XtOffset(StViewManagerWidget,stview_manager.field)
static XtResource resources[] = {
    {StNhorizontalScrollBar, XtCBoolean, XtRBoolean, sizeof(Boolean),
	 offset(horizontal_scrollbar), XtRBoolean, (caddr_t)&defFalse },
    {StNverticalScrollBar, XtCBoolean, XtRBoolean, sizeof(Boolean),
	 offset(vertical_scrollbar), XtRBoolean, (caddr_t)&defFalse },
    {StNscrollBarOnTop, XtCBoolean, XtRBoolean, sizeof(Boolean),
	 offset(scrollbar_on_top), XtRBoolean, (caddr_t)&defFalse },
    {StNscrollBarOnRight, XtCBoolean, XtRBoolean, sizeof(Boolean),
	 offset(scrollbar_on_right), XtRBoolean, (caddr_t)&defFalse },
};
#undef offset

static void Initialize(), 
    Realize(), Resize(), ChangeManaged(), DoLayout();
static Boolean SetValues();
static XtGeometryResult GeometryManager(), PreferredGeometry();

#define superclass	(&compositeClassRec)
StViewManagerClassRec stViewManagerClassRec = {
  { /* core_class fields */
    /* superclass	  */	(WidgetClass) superclass,
    /* class_name	  */	"StViewManager",
    /* widget_size	  */	sizeof(StViewManagerRec),
    /* class_initialize	  */	NULL,
    /* class_part_init    */    NULL,
    /* class_inited	  */	FALSE,
    /* initialize	  */	Initialize,
    /* initialize_hook    */    NULL,
    /* realize		  */	Realize,
    /* actions		  */	NULL,
    /* num_actions	  */	0,
    /* resources	  */	resources,
    /* num_resources	  */	XtNumber(resources),
    /* xrm_class	  */	NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	TRUE,
    /* compress_enterleave*/    TRUE,
    /* visible_interest	  */	FALSE,
    /* destroy		  */	NULL,
    /* resize		  */	Resize,
    /* expose		  */	XtInheritExpose,
    /* set_values	  */	SetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */	NULL,
    /* accept_focus	  */	NULL,
    /* version            */    XtVersion,
    /* callback_private	  */	NULL,
    /* tm_table    	  */	NULL,
    /* query_geometry     */    PreferredGeometry,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension          */	NULL
  },
  { /* composite_class fields */
    /* geometry_manager	  */	GeometryManager,
    /* change_managed	  */	ChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,
    /* delete_child	  */	XtInheritDeleteChild,
    /* extension          */	NULL
  },
  { /* StViewManagerClass fields */
    /* empty		  */	0
  }
};


  /* for public consumption */
WidgetClass stViewManagerWidgetClass = (WidgetClass)&stViewManagerClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static void SetBar(w, top, length, total)
	Widget w;
	Position top;
	Dimension length, total;
{
	XtScrollBarSetThumb(w, (float) top / total, (float) length / total );
}

static void RedrawThumbs(w)
  StViewManagerWidget w;
{
	register Cardinal x = w->stview_manager.disp_x;
	register Cardinal y = w->stview_manager.disp_y;
	register Cardinal width = w->stview_manager.children_width;
	register Cardinal height = w->stview_manager.children_height;
	register Widget clip = w->stview_manager.clip;

	if (w->stview_manager.horiz_bar)
		SetBar( w->stview_manager.horiz_bar, -(x),
			clip->core.width, width );

	if (w->stview_manager.vert_bar)
		SetBar( w->stview_manager.vert_bar, -(y),
			clip->core.height, height );
}

static void MoveChild(w, x, y)
	StViewManagerWidget w;
	Position x, y;
{
	register Widget frame = w->stview_manager.frame;
	register Widget clip = w->stview_manager.clip;
	register Cardinal width = w->stview_manager.children_width;
	register Cardinal height = w->stview_manager.children_height;

	/* make sure we never move past right/bottom borders */
	if (-x + clip->core.width > width)
		x = -(width - clip->core.width);

	if (-y + clip->core.height > height)
		y = -(height - clip->core.height);

	/* make sure we never move past left/top borders */
	if (x >= 0) x = 0;
	if (y >= 0) y = 0;

	XtMoveWidget(frame, x, y);

	w->stview_manager.disp_x = x;
	w->stview_manager.disp_y = y;

	RedrawThumbs(w);
}

static void ScrollUpDownProc(widget, closure, call_data)
    Widget widget;
    caddr_t closure;
    caddr_t call_data;
{
    StViewManagerWidget w = (StViewManagerWidget)closure;
    register Widget frame = w->stview_manager.frame;
    int pix = (int)call_data;
    Position x, y;

    x = frame->core.x - ((widget == w->stview_manager.horiz_bar) ? pix : 0);
    y = frame->core.y - ((widget == w->stview_manager.vert_bar) ? pix : 0);
    MoveChild(w, x, y);
}


/* ARGSUSED */
static void ThumbProc(widget, closure, percent)
    Widget widget;
    caddr_t closure;
    float *percent;
{
    StViewManagerWidget w = (StViewManagerWidget)closure;
    register Widget frame = w->stview_manager.frame;
    Position x, y;

    if (widget == w->stview_manager.horiz_bar)
#ifdef macII				/* bug in the macII A/UX 1.0 cc */
	x = (int)(-*percent * frame->core.width);
#else /* else not macII */
	x = -(int)(*percent * frame->core.width);
#endif /* macII */
    else
	x = frame->core.x;

    if (widget == w->stview_manager.vert_bar)
#ifdef macII				/* bug in the macII A/UX 1.0 cc */
	y = (int)(-*percent * frame->core.height);
#else /* else not macII */
	y = -(int)(*percent * frame->core.height);
#endif /* macII */
    else
	y = frame->core.y;

    MoveChild(w, x, y);
}

static void CreateScrollbar(w, horizontal)
    StViewManagerWidget w;
    Boolean horizontal;
{
    Widget clip = w->stview_manager.clip;

    static Arg barArgs[] = {
	{XtNorientation, NULL},
	{XtNlength, NULL},
	{XtNmappedWhenManaged, False},
    };
    Widget bar;

    XtSetArg(barArgs[0], XtNorientation,
	      horizontal ? XtorientHorizontal : XtorientVertical );
    XtSetArg(barArgs[1], XtNlength,
	     horizontal ? clip->core.width : clip->core.height);

    bar = XtCreateWidget( (horizontal ? "horizontal" : "vertical"),
			  scrollbarWidgetClass, (Widget)w,
			  barArgs, XtNumber(barArgs) );
    XtAddCallback( bar, XtNscrollProc, ScrollUpDownProc, (caddr_t)w );
    XtAddCallback( bar, XtNjumpProc, ThumbProc, (caddr_t)w );

    if (horizontal) {
	Dimension bw = bar->core.border_width;
	w->stview_manager.horiz_bar = bar;
	XtResizeWidget( clip, clip->core.width,	/* %%% wrong, but... */
		        w->core.height - bar->core.height - bw,
		        clip->core.border_width );
    }
    else {
	Dimension bw = bar->core.border_width;
	w->stview_manager.vert_bar = bar;
	XtResizeWidget( clip, w->core.width - bar->core.width - bw, /* %%% */
		        bar->core.height, clip->core.border_width );
    }

    XtManageChild( bar );

    return;
}

static void DoLayout(vmw)
	StViewManagerWidget	vmw;
{
	Widget frame = vmw->stview_manager.frame;
	int num_mapped_children = 0;
	Dimension frame_width = frame->core.width;
	Dimension frame_height = frame->core.height;
	int  i;
 
	if ((vmw->stview_manager.children_width != frame->core.width) ||
	    (vmw->stview_manager.children_height != frame->core.height)) 
	{
		frame_width = vmw->stview_manager.children_width;
		frame_height = vmw->stview_manager.children_height;
		XtResizeWidget(frame, frame_width, frame_height, (Dimension)0 );
	}

	for (i = 0; i < vmw->composite.num_children; i++) 
	{
		Widget widget = vmw->composite.children[i];
		if (widget->core.managed && IsChildWidget(widget, vmw)) 
		{
			if (widget->core.mapped_when_managed) 
					num_mapped_children++;
		} 
	} 
	if (XtIsRealized((Widget)vmw)) {
		if (vmw->composite.num_children == num_mapped_children) {
			XMapSubwindows( XtDisplay((Widget)vmw), XtWindow((Widget)vmw) );
		}
		else {
			int i = num_mapped_children;
			register Widget *childP = vmw->composite.children;
			for (; i > 0; childP++) {
				if (XtIsManaged(*childP) &&
					(*childP)->core.mapped_when_managed) {
					XtMapWidget(*childP);
					i--;
				}
	    		}
		}
	}
}

static void GetNewGeometry(vmw, width, height, reply_width, reply_height)
    StViewManagerWidget	vmw;
    Dimension	width, height;			/* height not currently used */
    Dimension	*reply_width, *reply_height; 	/* bounding box */
{
    Cardinal  i;
    Dimension w, h;	/* Width and height needed for viewManager	*/
    Dimension bw, bh;	/* Width and height needed for current widget 	*/
    register Widget widget;	/* Current widget 			*/
    Mask valuemask;
    XSetWindowAttributes attributes;
    int num_mapped_children = 0;
 
    /* viewManager width and height */
    w = width;
    h = height;
  
    for (i = 0; i < vmw->composite.num_children; i++) 
    {
	widget = vmw->composite.children[i];
	if (widget->core.managed && IsChildWidget(widget, vmw)) 
	{
	    if (widget->core.mapped_when_managed) num_mapped_children++;
	    /* Compute widget width */
	    bw = widget->core.x + 
		 widget->core.width + 
		 2*widget->core.border_width;
	    bh = widget->core.y + 
		 widget->core.height + 
		 2*widget->core.border_width;

	    AssignMax(w, bw);
	    AssignMax(h, bh);
	} 
    } 

    *reply_width = Max(w, 1);
    *reply_height = Max(h, 1);
}

static XtGeometryResult TryNewLayout(widget)
	Widget widget;
{
	StViewManagerWidget w = (StViewManagerWidget)widget;
	Dimension replay_width, replay_height;
	XtGeometryResult result;

	GetNewGeometry(widget, widget->core.width, widget->core.height,
			 &replay_width, &replay_height);

	/* save base window size */
	w->stview_manager.children_width = replay_width;
	w->stview_manager.children_height = replay_height;

	/* get clip and widget window size */
	do {
		result = XtMakeResizeRequest((Widget)w, 
						replay_width,
						replay_height,
						&replay_width,
						&replay_height);
	} while (result == XtGeometryAlmost); 

	return result;
}

/****************************************************************
 *
 * Class Procedures
 *
 ****************************************************************/


/* ARGSUSED */
static void Initialize(request, new)
	Widget request, new;
{
	StViewManagerWidget w = (StViewManagerWidget)new;
	Arg args[5];
	int ac;

	w->stview_manager.clip = w->stview_manager.horiz_bar 
		               = w->stview_manager.vert_bar
		               = w->stview_manager.frame
		               = (Widget)NULL;
	/*  Set up base window */
	w->stview_manager.disp_x = 0;
	w->stview_manager.disp_y = 0;
	w->stview_manager.children_width = w->core.width;
	w->stview_manager.children_height = w->core.height;

	ac = 0;
	XtSetArg(args[ac], XtNwidth, (XtArgVal)Max(w->core.width,1)),	ac++;
	XtSetArg(args[ac], XtNheight, (XtArgVal)Max(w->core.height,1)),	ac++;
	XtSetArg(args[ac], XtNborderWidth, (XtArgVal)0),		ac++;
	XtSetArg(args[ac],XtNbackground,(XtArgVal)w->core.background_pixel),ac++;
	w->stview_manager.clip =
		XtCreateManagedWidget( "clip", widgetClass, new, args, ac);

	w->stview_manager.frame =
		XtCreateManagedWidget( "frame", widgetClass, new, args, ac);

	if (w->stview_manager.horizontal_scrollbar) CreateScrollbar(w, True);
	if (w->stview_manager.vertical_scrollbar) CreateScrollbar(w, False);
}

static void Realize(widget, value_mask, attributes)
	Widget widget;
	XtValueMask *value_mask;
	XSetWindowAttributes *attributes;
{
	StViewManagerWidget w = (StViewManagerWidget)widget;
	register WidgetList childP = w->composite.children;
	register Cardinal num_children = w->composite.num_children;
		 Widget clip = w->stview_manager.clip;
	register Widget frame = w->stview_manager.frame;
	register Cardinal i;

	*value_mask |= CWBitGravity;
	attributes->bit_gravity = NorthWestGravity;
	(*superclass->core_class.realize)(widget, value_mask, attributes);

	(*w->core.widget_class->core_class.resize)(widget); /* turn on bars */

	XtRealizeWidget(clip);
	XtRealizeWidget(frame);
	for (i = 0; i < num_children; i++, childP++) 
	{
		Widget child = *childP;

		if (IsChildWidget(child, w)) 
		{
			XtRealizeWidget(child);
			XReparentWindow(XtDisplay(w), XtWindow(child), 
					XtWindow(frame),
					(Position)child->core.x, 
					(Position)child->core.y);
			XtMapWidget(child);
		}
	}
	/*
	XReparentWindow(XtDisplay(w), XtWindow(frame), XtWindow(clip),
					(Position)frame->core.x, 
					(Position)frame->core.y);
	*/
}

/* ARGSUSED */
static Boolean SetValues(current, request, new)
	Widget current, request, new;
{
	StViewManagerWidget oldw = (StViewManagerWidget)current;
	StViewManagerWidget w = (StViewManagerWidget)new;

	if (oldw->core.background_pixel != w->core.background_pixel) {
		Arg arg[1];

		/* Change clip & frame window's background */
		XtSetArg(arg[0], XtNbackground, 
			(XtArgVal)w->core.background_pixel);
		XtSetValues(w->stview_manager.clip, arg, 1); 
		XtSetValues(w->stview_manager.frame, arg, 1); 
	}
	/* take care of bars, & etc. */
	(*w->core.widget_class->core_class.resize)( new );
	return False;
}


static void ChangeManaged(widget)
	Widget widget;
{
	StViewManagerWidget w = (StViewManagerWidget)widget;
	Dimension replay_width, replay_height;
	XtGeometryResult sts;

	if (TryNewLayout(widget) == XtGeometryYes) 
	{
		DoLayout(widget);
		/* always want to resize, as we may no longer need bars */
		(*widget->core.widget_class->core_class.resize) (widget);
	}
}

static void Resize(widget)
	Widget widget;
{
	StViewManagerWidget w = (StViewManagerWidget)widget;
	register Widget clip = w->stview_manager.clip;
	register Widget frame = w->stview_manager.frame;
	Boolean needshoriz, needsvert;
	int   lw, lh;
	int clip_width, clip_height;

	clip_width = w->core.width;
	clip_height = w->core.height;

	needshoriz = w->stview_manager.horizontal_scrollbar;
	needsvert = w->stview_manager.vertical_scrollbar;
/*
	if (! XtIsRealized(widget))
		needsvert = needshoriz = False;
*/
	if (needshoriz && !w->stview_manager.horiz_bar)
		CreateScrollbar(w, True);
	if (needsvert && !w->stview_manager.vert_bar)
		CreateScrollbar(w, False);
	if (needsvert) {
		clip_width = w->core.width -
			(w->stview_manager.vert_bar->core.width
			+ w->stview_manager.vert_bar->core.border_width);
	}
	else {
		clip_width = w->core.width;
	}
	if (needshoriz) {
		clip_height = w->core.height -
			(w->stview_manager.horiz_bar->core.height
			+ w->stview_manager.horiz_bar->core.border_width);
	}
	else {
		clip_height = w->core.height;
	}
	AssignMax( clip_width, 1 );
	AssignMax( clip_height, 1 );

	/*
	if (XtIsRealized(clip))
		XRaiseWindow( XtDisplay(clip), XtWindow(clip) );
	*/
	if (XtIsRealized(frame))
		XRaiseWindow( XtDisplay(frame), XtWindow(frame) );

	XtMoveWidget( clip,
		  needsvert ? (w->stview_manager.scrollbar_on_right ? 0 :
			       w->stview_manager.vert_bar->core.width +
			       w->stview_manager.vert_bar->core.border_width) : 0,
		  needshoriz ? (w->stview_manager.scrollbar_on_top ? 
				(w->stview_manager.horiz_bar->core.height +
			         w->stview_manager.horiz_bar->core.border_width)
					: 0) 
				: 0);
	XtResizeWidget( clip, (Dimension)clip_width,
		    (Dimension)clip_height, (Dimension)0 );
	
	if (w->stview_manager.horiz_bar) {
		register Widget bar = w->stview_manager.horiz_bar;
		if (!needshoriz) {
			XtDestroyWidget( bar );
			w->stview_manager.horiz_bar = (Widget)NULL;
		}
		else {
			register int bw = bar->core.border_width;
			XtResizeWidget( bar, clip_width, bar->core.height, bw );
			XtMoveWidget( bar,
				((needsvert && !w->stview_manager.scrollbar_on_right)
				? w->stview_manager.vert_bar->core.width : -bw),
				(w->stview_manager.scrollbar_on_top ? -bw
				: w->core.height - bar->core.height - bw) );
			XtSetMappedWhenManaged( bar, True );
		}
		if (XtIsRealized(bar)) {
			XRaiseWindow( XtDisplay(bar), XtWindow(bar) );
		}
	}

	if (w->stview_manager.vert_bar) {
		register Widget bar = w->stview_manager.vert_bar;
		if (!needsvert) {
			XtDestroyWidget( bar );
			w->stview_manager.vert_bar = (Widget)NULL;
		}
		else {
			register int bw = bar->core.border_width;
			XtResizeWidget( bar, bar->core.width, clip_height, bw );
			XtMoveWidget( bar,
				(w->stview_manager.scrollbar_on_right
				? w->core.width - bar->core.width - bw 
				: -bw),
				((needshoriz && w->stview_manager.scrollbar_on_top)
				? w->stview_manager.horiz_bar->core.height
				: -bw) );
			XtSetMappedWhenManaged( bar, True );
		}
		if (XtIsRealized(bar)) {
			XRaiseWindow( XtDisplay(bar), XtWindow(bar) );
		}
	}
	RedrawThumbs(w);
}



/* Semi-public routines */


static XtGeometryResult GeometryManager(child, request, reply)
    Widget child;
    XtWidgetGeometry *request, *reply;
{
	StViewManagerWidget w = (StViewManagerWidget)child->core.parent;
	Boolean rWidth = (Boolean)(request->request_mode & CWWidth);
	Boolean rHeight = (Boolean)(request->request_mode & CWHeight);
	XtGeometryResult result;

	result == XtGeometryYes;
	return result;
}

static XtGeometryResult PreferredGeometry(w, constraints, reply)
    Widget w;
    XtWidgetGeometry *constraints, *reply;
{
	return XtGeometryYes;
}
