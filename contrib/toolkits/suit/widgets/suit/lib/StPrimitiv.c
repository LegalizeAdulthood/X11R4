/*
 *      Original Source: ./lib/Xaw/Simple.c
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */
#ifndef lint
static char Xrcsid[] = "$XConsortium: Simple.c,v 1.16 88/09/27 16:44:32 swick Exp $";
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

/*
 * StPrimitive.c - SUIT StPrimitive widget
 *
 */

  /* The following are defined for the reader's convenience.  Any
     Xt..Field macro in this code just refers to some field in
     one of the substructures of the WidgetRec.  */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <stdio.h>
#include <ctype.h>
#include <X11/XawMisc.h>
#include <St/StwP.h>
#include <St/StPrimitiP.h>
#include "StPrimitiI.h"
 
/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

#define offset(field) XtOffset(StPrimitiveWidget, field)
static XtResource resources[] = { 

    {StNcallback, XtCCallback, XtRCallback, sizeof(caddr_t), 
	offset(stPrimitive.callbacks), XtRCallback, (caddr_t)NULL},
    {StNcallbackInterval, StCIntervalTime, XtRInt, sizeof(int),
	offset(stPrimitive.callback_interval), XtRImmediate, (caddr_t)0},
    {StNcursor, XtCCursor, XtRCursor, sizeof(Cursor),
	offset(stPrimitive.cursor), XtRString, (caddr_t)None},
    {StNcontents, StCContents, StRStContents, sizeof(StContents),
	offset(stPrimitive.contents), XtRImmediate, (caddr_t)0},
    {StNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
	offset(stPrimitive.font), XtRString, "XtDefaultFont"},
    {StNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	offset(stPrimitive.foreground), XtRString, "XtDefaultForeground"},
    {StNinsensitivePixmap, XtCPixmap, XtRPixmap, sizeof(Pixmap),
	offset(stPrimitive.insensitive_pixmap), XtRPixmap, (caddr_t)None},
    {StNmaxHeight, StCMaxSize, XtRDimension, sizeof(Dimension),
	offset(stPrimitive.max_height), XtRImmediate,(caddr_t)-1},
    {StNmaxWidth, StCMaxSize, XtRDimension, sizeof(Dimension),
	offset(stPrimitive.max_width), XtRImmediate, (caddr_t)-1},
    {StNminHeight, StCMinSize, XtRDimension, sizeof(Dimension),
	offset(stPrimitive.min_height), XtRImmediate, (caddr_t)0},
    {StNminWidth, StCMinSize, XtRDimension, sizeof(Dimension),
	offset(stPrimitive.min_width), XtRImmediate, (caddr_t)0},
    {StNtransparence, StCTransparence, XtRBoolean, sizeof(Boolean),
	offset(stPrimitive.transparence), XtRBoolean, FALSE},
};
#undef offset

static void ClassPartInitialize();
static void Initialize();
static void Destroy();
static void Realize();
static Boolean SetValues();
static XtGeometryResult QueryGeometry();
static Boolean ChangeSensitive();
static Boolean ParentExpose();


#define SuperClass   ((WidgetClass)&widgetClassRec)

StPrimitiveClassRec stPrimitiveClassRec = {
  {
    (WidgetClass) SuperClass,		/* superclass		  */	
    "StPrimitive",			/* class_name		  */
    sizeof(StPrimitiveRec),		/* size			  */
    NULL,				/* class_initialize	  */
    ClassPartInitialize,		/* class_part_initialize  */
    FALSE,				/* class_inited		  */
    Initialize,				/* initialize		  */
    NULL,				/* initialize_hook	  */
    Realize,				/* realize		  */
    NULL,				/* actions		  */
    0,					/* num_actions		  */
    resources,				/* resources		  */
    XtNumber(resources),		/* resource_count	  */
    NULLQUARK,				/* xrm_class		  */
    FALSE,				/* compress_motion	  */
    TRUE,				/* compress_exposure	  */
    TRUE,				/* compress_enterleave    */
    FALSE,				/* visible_interest	  */
    Destroy,	 			/* destroy		  */
    NULL,				/* resize		  */
    NULL,				/* expose		  */
    SetValues,				/* set_values		  */
    NULL,				/* set_values_hook	  */
    XtInheritSetValuesAlmost,		/* set_values_almost	  */
    NULL,				/* get_values_hook	  */
    NULL,				/* accept_focus		  */
    XtVersion,				/* version		  */
    NULL,				/* callback_private	  */
    NULL,				/* tm_table		  */
    QueryGeometry,			/* query_geometry	  */
    XtInheritDisplayAccelerator,	/* display_accelerator	  */
    NULL				/* extension		  */
  },  /* CoreClass fields initialization */
  {
    ChangeSensitive,			/* change_sensitive	  */
    ParentExpose,			/* parent_expose	  */
  },  /* StPrimitiveClass fields initialization */
};

  /* for public consumption */
WidgetClass stPrimitiveWidgetClass = (WidgetClass) &stPrimitiveClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

/***** private utility ********/

static GC GetStandardGC(w, fg, bg)
	Widget w;
	Pixel fg, bg;
{
	StPrimitiveWidget bw = (StPrimitiveWidget) w;
	XGCValues values;
	Mask valueMask;
  
	values.foreground = fg;
	values.background = bg;
	values.font       = PriWfont->fid;
	values.line_width = PriWborderWidth;
	valueMask = GCForeground | GCBackground | GCFont | GCLineWidth;
    
	return XtGetGC(w, valueMask, &values);
}

static GC GetGrayGC(w, fg, bg)
	Widget w;
	Pixel fg, bg;
{
	StPrimitiveWidget bw = (StPrimitiveWidget) w;
	XGCValues values;
	Mask valueMask;

	values.foreground = fg;
	values.background = bg;
	values.font       = PriWfont->fid;
	values.line_width = PriWborderWidth;
	values.fill_style = FillTiled;
	values.tile       = PriWinsensitivePixmap;
	valueMask = GCForeground | GCBackground | GCFont | GCLineWidth
		  | GCTile | GCFillStyle;

	return XtGetGC(w, valueMask, &values);
}

/*****  private initialize ****/

static void InitializeMyGC(w, copyFromParent)
	Widget w;
	Boolean copyFromParent;
{
	StPrimitiveWidget bw = (StPrimitiveWidget) w;
	Pixel background;

	background = copyFromParent
			? PriWparent->core.background_pixel 
			: PriWbackground;
 
	PriWnormalGC = GetStandardGC(bw, PriWforeground, background);
	PriWinverseGC = GetStandardGC(bw, background, PriWforeground);
	PriWnormalGrayGC = GetGrayGC(bw, PriWforeground, background);
	PriWinverseGrayGC = GetGrayGC(bw, background, PriWforeground);
}

/***** private release ********/

static void ReleaseMyGC(w)
	Widget w;
{
	StPrimitiveWidget bw = (StPrimitiveWidget) w;

	XtReleaseGC(w, PriWnormalGC);
	XtReleaseGC(w, PriWinverseGC);
	XtReleaseGC(w, PriWnormalGrayGC);
	XtReleaseGC(w, PriWinverseGrayGC);
}

/***** default accessing ******/

static Pixmap DefaultInsensitivePixmap(w)
	Widget w;
{
	StPrimitiveWidget bw = (StPrimitiveWidget) w;

	return XmuCreateStippledPixmap(XtScreen(w), PriWforeground,
		PriWbackground, PriWdepth);
}

static Pixmap DefaultInsensitiveBorderPixmap(w)
	Widget w;
{
	StPrimitiveWidget bw = (StPrimitiveWidget) w;

	return XmuCreateStippledPixmap(XtScreen(w), PriWborderColor,
		XtScreen(w)->white_pixel, PriWdepth);
}

/***** changing ***************/

static void ChangeTransparence(w, mode)
	Widget w;
	Boolean mode;
{
	StPrimitiveWidget bw = (StPrimitiveWidget) w;
	Display *display = XtDisplay(w);
	Window window = XtWindow(w);

	if(XtIsRealized(w)) {
	    if(mode) {
		if(PriWparent->core.background_pixmap != UnspecifiedPixmap) {
		    XSetWindowBackgroundPixmap(display, window,
			PriWparent->core.background_pixmap);
		} else {
		    XSetWindowBackground(display, window,
			PriWparent->core.background_pixel);
		}
		XSetWindowBorderWidth(display, window, 0);
	    } else {
		if(PriWbackgroundPixmap != UnspecifiedPixmap) {
		    XSetWindowBackgroundPixmap(display, window, 
			PriWbackgroundPixmap);
		} else {
		    XSetWindowBackground(display, window, PriWbackground);
		}
		XSetWindowBorderWidth(display, window, PriWborderWidth);
	    }
	XClearWindow(XtDisplay(w), XtWindow(w));
	}
}

static void ChangeCursor(w, cursor)
	Widget w;
	Cursor cursor;
{
	XDefineCursor(XtDisplay(w), XtWindow(w), cursor);
}

static void ChangeBorder(w)
	Widget w;
{
	StPrimitiveWidget bw = (StPrimitiveWidget) w;

	PriWinsensitiveBorderPixmap = DefaultInsensitiveBorderPixmap(w);
	ChangeSensitive(w);
}

/***** my event handler *******/

static void EventHandler(w, closure, event)
	Widget w;
	Widget closure;
	XEvent *event;
{
	StPrimitiveWidget bw = (StPrimitiveWidget) closure;

	switch (event->type) {
	    case Expose:
		if(event->xexpose.count == 0) {
	    	    StPrimitiveWidgetClass class =
			(StPrimitiveWidgetClass)bw->core.widget_class;
		    Boolean redisplay = FALSE;

		    if(class->stPrimitive_class.parent_expose != NULL)
			redisplay = 
			    (*(class->stPrimitive_class.parent_expose)) (bw);
		    if(class->core_class.expose != NULL && redisplay)
			(*(class->core_class.expose)) (bw, event, NULL);
		}
	}
}

/****************************************************************
 *
 * Class Procedures
 *
 ****************************************************************/

static void ClassPartInitialize(class)
	WidgetClass class;
{
	register StPrimitiveWidgetClass cw = (StPrimitiveWidgetClass) class;
	register StPrimitiveWidgetClass super = 
			((StPrimitiveWidgetClass)cw->core_class.superclass);

	if(cw->stPrimitive_class.change_sensitive == XtInheritChangeSensitive)
	    cw->stPrimitive_class.change_sensitive =
				super->stPrimitive_class.change_sensitive;

	if(cw->stPrimitive_class.parent_expose == XtInheritParentExpose)
	    cw->stPrimitive_class.parent_expose = 
				super->stPrimitive_class.parent_expose;
}

/* ARGSUSED */
static void Initialize(request, new)
	Widget request, new;
{
	StPrimitiveWidget bw = (StPrimitiveWidget) new;

	if(PriWinsensitivePixmap == None)
	    PriWinsensitivePixmap = DefaultInsensitivePixmap(new);
	PriWinsensitiveBorderPixmap = DefaultInsensitiveBorderPixmap(new);

	InitializeMyGC(new, PriWtransparence);

	PriWwidth = restrict(PriWwidth, PriWminWidth, PriWmaxWidth);
	PriWheight = restrict(PriWheight, PriWminHeight, PriWmaxHeight);

	/*XtAddEventHandler(XtParent(bw), (EventMask)ExposureMask, TRUE,
	    EventHandler, (caddr_t)bw);*/ /* not support */

}

/* ARGSUSED */
static void Destroy(w)
	Widget w;
{
	ReleaseMyGC(w);
}

/* ARGSUSED */
static void Realize(w, valueMask, attributes)
	Widget w;
	Mask *valueMask;
	XSetWindowAttributes *attributes;
{
	register StPrimitiveWidget bw = (StPrimitiveWidget) w;

	if(PriWcursor != None) {
	    attributes->cursor = PriWcursor;
	    *valueMask |= CWCursor;
	}

	if(! isSensitive(w)) {
	    attributes->border_pixmap = PriWinsensitiveBorderPixmap;
	    *valueMask |= CWBorderPixmap;
	    *valueMask &= ~CWBorderPixel;
	}

	if(PriWtransparence) {
	    if(PriWparent->core.background_pixmap != UnspecifiedPixmap) {
		attributes->background_pixmap = PriWparent->core.background_pixmap;
		*valueMask |= CWBackPixmap;
	    } else {
		attributes->background_pixel = PriWparent->core.background_pixel;
		*valueMask |= CWBackPixel;
	    }
	}

	(*SuperClass->core_class.realize) (w, valueMask, attributes);

	if(PriWtransparence)
	    XSetWindowBorderWidth(XtDisplay(w), XtWindow(w), 0);
}

/* ARGSUSED */
static Boolean SetValues (current, request, new)
	Widget current, request, new;
{
	StPrimitiveWidget cw = (StPrimitiveWidget) current;
	StPrimitiveWidget rw = (StPrimitiveWidget) request;
	StPrimitiveWidget bw = (StPrimitiveWidget) new;
	Boolean redisplay = FALSE;
	Boolean need_rest_width = FALSE;
	Boolean need_rest_height = FALSE;
	Boolean need_change_gc = FALSE;

	if(PriWmaxWidth != cw->stPrimitive.max_width
	|| PriWminWidth != cw->stPrimitive.min_width) {
	    PriWmaxWidth = max(PriWminWidth, PriWmaxWidth);
	    need_rest_width = TRUE;
	}
		    
	if(PriWmaxHeight != cw->stPrimitive.max_height
	|| PriWminHeight != cw->stPrimitive.min_height) {
	    PriWmaxHeight = max(PriWminHeight, PriWmaxHeight);
	    need_rest_height = TRUE;
	}
		    
	if(PriWwidth != cw->core.width || need_rest_width) {
	    PriWwidth = restrict(PriWwidth, PriWminWidth, PriWmaxWidth); 
	    redisplay = TRUE;
	}

	if(PriWheight != cw->core.height || need_rest_height) {
	    PriWheight = restrict(PriWheight, PriWminHeight, PriWmaxHeight);
	    redisplay = TRUE;
	}

	if(PriWcursor != cw->stPrimitive.cursor && XtIsRealized(bw)) {
	    ChangeCursor(bw, PriWcursor);
	}

	if(PriWborderColor != cw->core.border_pixel) {
	    ChangeBorder(bw);
	    need_change_gc = TRUE;
	}

	if(PriWtransparence != cw->stPrimitive.transparence
	|| PriWbackground != cw->core.background_pixel
	|| PriWbackgroundPixmap != cw->core.background_pixmap
	|| PriWborderWidth != cw->core.border_width) {
	    ChangeTransparence(bw, PriWtransparence);
	    need_change_gc = TRUE;
	}

	if(PriWforeground != cw->stPrimitive.foreground
	|| PriWfont->fid != cw->stPrimitive.font->fid
	|| PriWinsensitivePixmap != cw->stPrimitive.insensitive_pixmap
	|| need_change_gc) {
	    ReleaseMyGC(bw);
	    InitializeMyGC(bw, PriWtransparence);
	    redisplay = TRUE;
	}

	if(PriWsensitive != cw->core.sensitive
	|| PriWancestorSensitive != cw->core.ancestor_sensitive) {
	    StPrimitiveWidgetClass class =
		(StPrimitiveWidgetClass)bw->core.widget_class;
	    if(class->stPrimitive_class.change_sensitive != NULL)
	        (*(class->stPrimitive_class.change_sensitive)) (bw);
	    redisplay = TRUE;
	}

	return redisplay;
}

static XtGeometryResult QueryGeometry(w, intended, preferred)
	Widget w;
	XtWidgetGeometry *intended, *preferred;
{
	StPrimitiveWidget bw = (StPrimitiveWidget) w;

	preferred->request_mode = CWWidth | CWHeight;
	preferred->width = 
	    restrict(intended->width, PriWminWidth, PriWmaxWidth);
	preferred->height = 
	    restrict(intended->height, PriWminHeight, PriWmaxHeight);

	if(preferred->width == PriWwidth
	&& preferred->height == PriWheight)
	    return XtGeometryNo;

	if(intended->width == preferred->width
	&& intended->height == preferred->height
	&& (intended->request_mode & (CWWidth | CWHeight)) 
		== (CWWidth | CWHeight))
	    return XtGeometryYes;

	return XtGeometryAlmost;

}

static Boolean ChangeSensitive(w)
	Widget w;
{
	register StPrimitiveWidget bw = (StPrimitiveWidget) w;
	Display *display = XtDisplay(w);
	Window window = XtWindow(w);

	if(XtIsRealized(w)) {
	    if(isSensitive(w)) {
		if(PriWborderPixmap != UnspecifiedPixmap) {
		    XSetWindowBorderPixmap(display, window, PriWborderPixmap);
		} else {
		    XSetWindowBorder(display, window, PriWborderColor);
		}
	    } else {
		XSetWindowBorderPixmap(display, window,
		PriWinsensitiveBorderPixmap);
	    }
	}
    
	return FALSE;
}

static Boolean ParentExpose(w)
	Widget w;
{
	StPrimitiveWidget bw = (StPrimitiveWidget) w;

	ChangeTransparence(bw, PriWtransparence);
	ReleaseMyGC(bw);
	InitializeMyGC(bw);

	return FALSE;
}
