/*
 *      Original Source: ./lib/Xaw/Command.c
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */
#ifndef lint
static char Xrcsid[] = "$XConsortium: Command.c,v 1.46 88/11/01 16:57:33 swick Exp $";
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
 * StCheckBoxSwitch.c - SUIT Check-Box Switch Widget
 *
 */

  /* The following are defined for the reader's convenience.  Any
     Xt..Field macro in this code just refers to some field in
     one of the substructures of the WidgetRec.  */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/XawMisc.h>
#include <St/StwP.h>
#include <St/StCkBxSwP.h>
#include "StCkBxSwI.h"

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

static void UpdateMarkerRegion();
static void UpdateMarkerOffset();

static void Initialize();
static void Destroy();
static void Realize();
static void Resize();
static void Redisplay();
static Boolean SetValues();
static Boolean ChangeSensitive();
static void Set();
static void Unset();
static void Notify();


static XtActionsRec actionsList[] = {

    {"set",		Set},
    {"notify",		Notify},
    {"unset",		Unset},
};

#define SuperClass ((StPictrialSwitchWidgetClass)&stPictrialSwitchClassRec)

StCheckBoxSwitchClassRec stCheckBoxSwitchClassRec = {
  {
    (WidgetClass) SuperClass,		/* superclass		  */	
    "StCheckBoxSwitch",	      		/* class_name		  */
    sizeof(StCheckBoxSwitchRec),	/* size			  */
    NULL,	          		/* class_initialize	  */
    NULL,				/* class_part_initialize  */
    FALSE,				/* class_inited		  */
    Initialize,				/* initialize		  */
    NULL,				/* initialize_hook	  */
    Realize,				/* realize		  */
    actionsList,			/* actions		  */
    XtNumber(actionsList),		/* num_actions		  */
    NULL,				/* resources		  */
    0,	 			      	/* resource_count	  */
    NULLQUARK,				/* xrm_class		  */
    FALSE,				/* compress_motion	  */
    TRUE,				/* compress_exposure	  */
    TRUE,				/* compress_enterleave    */
    FALSE,				/* visible_interest	  */
    Destroy,				/* destroy		  */
    Resize,				/* resize		  */
    Redisplay,	         		/* expose		  */
    SetValues,				/* set_values		  */
    NULL,				/* set_values_hook	  */
    XtInheritSetValuesAlmost,		/* set_values_almost	  */
    NULL,				/* get_values_hook	  */
    NULL,				/* accept_focus		  */
    XtVersion,				/* version		  */
    NULL,				/* callback_private	  */
    XtInheritTranslations,		/* tm_table		  */
    XtInheritQueryGeometry,		/* query_geometry	  */
    XtInheritDisplayAccelerator,	/* display_accelerator	  */
    NULL				/* extension		  */
  },  /* CoreClass fields initialization */
  {
    ChangeSensitive,			/* change_sensitive	  */
    XtInheritParentExpose,		/* parent_expose	  */
  },  /* StPrimitiveClass fields initialization */
  {
    0,					/* field not used	  */
  },  /* StLabelClass fields initialization */
  {
    0,					/* field not used	  */
  },  /* StSimpleSwitchClass fields initialization */
  {
    0,					/* field not used	  */
  },  /* StPictrialSwitchClass fields initialization */
  {
    0,					/* field not used	  */
  },  /* StCheckBoxSwitchClass fields initialization */
};

  /* for public consumption */
WidgetClass stCheckBoxSwitchWidgetClass = 
                             (WidgetClass)&stCheckBoxSwitchClassRec;

/****************************************************************
 *
 * Private Data
 *
 ****************************************************************/

/* Define shape of marker */

#define marker_bitmap_width	16
#define marker_bitmap_height	16
static char marker_bitmap_bits[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x30, 0x06, 0x18, 0x06, 0x0c,
    0x06, 0x06, 0x06, 0x03, 0x86, 0x01, 0xc6, 0x00, 0x66, 0x00, 0x36, 0x00,
    0x1e, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00};

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

/***** private initialize *****/

static void InitializeMyGC(w)
	Widget w;
{
	StCheckBoxSwitchWidget bw = (StCheckBoxSwitchWidget) w;
	XGCValues values;
	Mask valueMask;
	Display *display = XtDisplay(w);

	values.foreground = PriWforeground;
	values.background = PriWbackground;
	values.clip_mask = PriWmarkerShape;
	values.tile = PriWinsensitivePixmap;
	valueMask = GCForeground | GCBackground | GCClipMask | GCTile;
	if(!isSensitive(w)) {
	    values.fill_style = FillTiled;
	    valueMask |= GCFillStyle;
	}
	PriWmarkerGC = XCreateGC(display, XRootWindow(display,
			   XDefaultScreen(display)), valueMask, &values);

	values.foreground = PriWbackground;
	values.background = PriWforeground;
	values.clip_mask = PriWmarkerShape;
	valueMask = GCForeground | GCBackground | GCClipMask;
	if(PriWbackgroundPixmap != UnspecifiedPixmap) {
	    values.tile = PriWbackgroundPixmap;
	    values.fill_style = FillTiled;
	    valueMask |= GCFillStyle | GCTile;
	}
	PriWinverseMarkerGC = XCreateGC(display, XRootWindow(display,
				XDefaultScreen(display)), valueMask, &values);
}

static void CreateMarkerShape(w)
	Widget w;
{
	StCheckBoxSwitchWidget bw = (StCheckBoxSwitchWidget) w;
	Display *display = XtDisplay(w);

	PriWmarkerShape = XCreateBitmapFromData(display, XRootWindow(display,
				XDefaultScreen(display)), marker_bitmap_bits,
				marker_bitmap_width, marker_bitmap_height);
}

/***** private release ********/

static void ReleaseMyGC(w)
	Widget w;
{
	StCheckBoxSwitchWidget bw = (StCheckBoxSwitchWidget) w;

	if(PriWmarkerGC != NULL)
	    XFreeGC(XtDisplay(w), PriWmarkerGC);
	if(PriWinverseMarkerGC != NULL)
	    XFreeGC(XtDisplay(w), PriWinverseMarkerGC);
}

static void FreeMarkerShape(w)
	Widget w;
{
	StCheckBoxSwitchWidget bw = (StCheckBoxSwitchWidget) w;

	if(PriWmarkerShape != None) {
	    XFreePixmap(XtDisplay(w), PriWmarkerShape);
	    PriWmarkerShape = None;
	}
}

/***** updating ***************/

static void UpdateMarkerOffset(w)
	Widget w;
{
	StCheckBoxSwitchWidget bw = (StCheckBoxSwitchWidget) w;
	Display *display = XtDisplay(w);

	PriWmarkerRegion.x = (PriWmarkerBox.width - marker_bitmap_width) / 2
				+ PriWmarkerBox.x;
	PriWmarkerRegion.y = (PriWmarkerBox.height - marker_bitmap_height) / 2
				+ PriWmarkerBox.y;

	XSetClipOrigin(display, PriWmarkerGC, PriWmarkerRegion.x,
		PriWmarkerRegion.y);
	XSetClipOrigin(display, PriWinverseMarkerGC, PriWmarkerRegion.x,
		PriWmarkerRegion.y);
}

static void UpdateMarkerBoxOffset(w)
	Widget w;
{
	StCheckBoxSwitchWidget bw = (StCheckBoxSwitchWidget) w;

	PriWmarkerBox.x = (PriWwidth - PriWmarkerBox.width) / 2;
	PriWmarkerBox.y = (PriWheight - PriWmarkerBox.height) / 2;
}

static void UpdateMarkerRegion(w)
	Widget w;
{
	StCheckBoxSwitchWidget bw = (StCheckBoxSwitchWidget) w;

	PriWmarkerRegion.width = marker_bitmap_width;
	PriWmarkerRegion.height = marker_bitmap_height;
	UpdateMarkerOffset(w);
}

static void UpdateMarkerBox(w, width, height)
	Widget w;
	Dimension width, height;
{
	StCheckBoxSwitchWidget bw = (StCheckBoxSwitchWidget) w;

	PriWmarkerBox.width = width;
	PriWmarkerBox.height = height;
	UpdateMarkerBoxOffset(w);
}

/***** displaying *************/

static void DrawMarkerBox(w)
	Widget w;
{
	StCheckBoxSwitchWidget bw = (StCheckBoxSwitchWidget) w;
	int x, y;
	unsigned int width, height;

	if(PriWborderWidth == 0)
	    return;

	x = PriWmarkerBox.x + PriWborderWidth / 2;
	y = PriWmarkerBox.y + PriWborderWidth / 2;
	width = PriWmarkerBox.width - PriWborderWidth;
	height = PriWmarkerBox.height - PriWborderWidth;

	XDrawRectangle(XtDisplay(w), XtWindow(w), PriWborderGC, x, y,
	    width, height);
}

/***** marker *****************/

static void MarkerOn(w)
	Widget w;
{
	StCheckBoxSwitchWidget bw = (StCheckBoxSwitchWidget) w;

	StuFillRectangle(w, PriWmarkerRegion, PriWmarkerGC);
}

static void MarkerOff(w)
	Widget w;
{
	StCheckBoxSwitchWidget bw = (StCheckBoxSwitchWidget) w;

	if(PriWmarkerBox.width < PriWmarkerRegion.width
	|| PriWmarkerBox.height < PriWmarkerRegion.height) {
	    StuFillRectangle(w, PriWmarkerRegion, PriWbackgroundGC);
	    StuFillRectangle(w, PriWmarkerBox, PriWinsideGC);
	} else {
	    StuFillRectangle(w, PriWmarkerRegion, PriWinverseMarkerGC);
	}
	DrawMarkerBox(w);
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
	StCheckBoxSwitchWidget bw = (StCheckBoxSwitchWidget) new;

	CreateMarkerShape(bw);
	InitializeMyGC(bw);

	PriWmarkerBox.x = 0;
	PriWmarkerBox.y = 0;
	PriWmarkerBox.width = 0;
	PriWmarkerBox.height = 0;
}

/* ARGSUSED */
static void Redisplay(w, event, region)
	Widget w;
	XEvent *event;
	Region region;
{
	StCheckBoxSwitchWidget bw = (StCheckBoxSwitchWidget) w;

	StuFillRectangle(w, PriWmarkerBox, PriWinsideGC);
	DrawMarkerBox(w);
	if(PriWboolContents)
	    MarkerOn(w);
}

/* ARGSUSED */
static void Realize(w, value_mask, attributes)
	register Widget w;
	Mask *value_mask;
	XSetWindowAttributes *attributes;
{
	StCheckBoxSwitchWidget bw = (StCheckBoxSwitchWidget) w;

	(*SuperClass->core_class.realize) (w, value_mask, attributes);

	UpdateMarkerBox(w, PriWwidth, PriWheight);
	UpdateMarkerRegion(w);
}

/* ARGSUSED */
static void Destroy(w)
	Widget w;
{
	ReleaseMyGC(w);
	FreeMarkerShape(w);
}

static void Resize(w)
	Widget w;
{
	if(XtIsRealized(w)) {
	    UpdateMarkerBoxOffset(w);
	    UpdateMarkerOffset(w);
	}
}

/* ARGSUSED */
static Boolean SetValues (current, request, new)
	Widget current, request, new;
{
	StCheckBoxSwitchWidget cw = (StCheckBoxSwitchWidget) current;
	StCheckBoxSwitchWidget bw = (StCheckBoxSwitchWidget) new;
	Boolean redisplay = FALSE;
	Boolean need_change_offset = FALSE;

	if(PriWforeground != cw->stPrimitive.foreground
	|| PriWbackground != cw->core.background_pixel
	|| PriWbackgroundPixmap != cw->core.background_pixmap) {
	    ReleaseMyGC(bw);
	    InitializeMyGC(bw);
	    need_change_offset = TRUE;
	}

	if(PriWwidth != cw->core.width) {
	    PriWmarkerBox.width = PriWwidth;
	    PriWwidth = max(PriWwidth, MinBoxWidth);
	    UpdateMarkerBoxOffset(bw);
	    need_change_offset = TRUE;
	}

	if(PriWheight != cw->core.height) {
	    PriWmarkerBox.height = PriWheight;
	    PriWheight = max(PriWheight, MinBoxHeight);
	    UpdateMarkerBoxOffset(bw);
	    need_change_offset = TRUE;
	}

	if(need_change_offset) {
	    UpdateMarkerOffset(bw);
	    redisplay = TRUE;
	}

	return redisplay;
}

static Boolean ChangeSensitive(w)
	Widget w;
{
	StCheckBoxSwitchWidget bw = (StCheckBoxSwitchWidget) w;

	(*SuperClass->stPrimitive_class.change_sensitive) (w);

	if(isSensitive(w))
	    XSetFillStyle(XtDisplay(w), PriWmarkerGC, FillSolid);
	else
	    XSetFillStyle(XtDisplay(w), PriWmarkerGC, FillTiled);

	return TRUE;
}

/****************************************************************
 *
 * Action Procedures
 *
 ****************************************************************/

/* ARGSUSED */
static void Set(w,event,params,num_params)
	Widget w;
	XEvent *event;
	String *params;		/* unused */
	Cardinal *num_params;	/* unused */
{
	StCheckBoxSwitchWidget bw = (StCheckBoxSwitchWidget) w;

	if(!PriWset) {
	    PriWcontents = !PriWboolContents;
	    if(PriWboolContents)
		MarkerOn(w);
	    else
		MarkerOff(w);
	    PriWset = TRUE;
	}
}

/* ARGSUSED */
static void Unset(w,event,params,num_params)
	Widget w;
	XEvent *event;
	String *params;		/* unused */
	Cardinal *num_params;	/* unused */
{
	StCheckBoxSwitchWidget bw = (StCheckBoxSwitchWidget) w;

	if(PriWset) {
	    PriWcontents = !PriWboolContents;
	    if(PriWboolContents)
		MarkerOn(w);
	    else
		MarkerOff(w);
	    PriWset = FALSE;
	}
}

/* ARGSUSED */
static void Notify(w,event,params,num_params)
	Widget w;
	XEvent *event;
	String *params;		/* unused */
	Cardinal *num_params;	/* unused */
{
	StCheckBoxSwitchWidget bw = (StCheckBoxSwitchWidget) w;

	if(PriWset) { 
	    PriWset = FALSE;
	    XtCallCallbacks(w, StNcallback, PriWboolContents);
	}
}
