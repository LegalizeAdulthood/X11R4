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
 * StMenuItem.c - SUIT Menu-Item Button widget
 *
 */

  /* The following are defined for the reader's convenience.  Any
     Xt..Field macro in this code just refers to some field in
     one of the substructures of the WidgetRec.  */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/XawMisc.h>
#include <St/StwP.h>
#include <St/StMenuItmP.h>
#include "StMenuItmI.h"


/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

static void Initialize();
static void Destroy();
static void Redisplay();
static void Realize();
static void Resize();
static Boolean SetValues();
static Boolean ChangeSensitive();
static Boolean ParentExpose();
static void Set();
static void Unset();
static void Notify();
static void Motion();


static char defaultTranslations[] =
    "Button1<EnterWindow>:	set()	 \n\
     <EnterWindow>:		set() 	 \n\
     <LeaveWindow>:		unset()	 \n\
     <Btn1Up>:			notify() \n\
     <Motion>:			motion()";

#define offset(field) XtOffset(StMenuItemWidget, field)
static XtResource resources[] = {

   {XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
      offset(core.border_width), XtRImmediate, (caddr_t) 0},
   {StNlineTop, XtCBorderWidth, XtRDimension, sizeof(Dimension),
      offset(stMenuItem.line_top), XtRImmediate, (caddr_t) 0},
   {StNlineBottom, XtCBorderWidth, XtRDimension, sizeof(Dimension),
      offset(stMenuItem.line_bottom), XtRImmediate, (caddr_t) 0},
   {StNlineRight, XtCBorderWidth, XtRDimension, sizeof(Dimension),
      offset(stMenuItem.line_right), XtRImmediate, (caddr_t) 0},
   {StNlineLeft, XtCBorderWidth, XtRDimension, sizeof(Dimension),
      offset(stMenuItem.line_left), XtRImmediate, (caddr_t) 0},
   {StNsubmenu, XtCCallback, XtRCallback, sizeof(caddr_t),
      offset(stMenuItem.submenu_callback), XtRCallback, (caddr_t)NULL},
   {StNarrow, XtCBoolean, XtRBoolean, sizeof(Boolean),
      offset(stMenuItem.arrow), XtRCallback, (caddr_t)FALSE},

};
#undef offset

static XtActionsRec actionsList[] = {

    {"set",		Set},
    {"notify",		Notify},
    {"unset",		Unset},
    {"motion",		Motion},
};

#define SuperClass ((StSimpleButtonWidgetClass)&stSimpleButtonClassRec)

StMenuItemClassRec stMenuItemClassRec = {
  {
    (WidgetClass) SuperClass,		/* superclass		  */	
    "StMenuItem",	      		/* class_name		  */
    sizeof(StMenuItemRec),		/* size			  */
    NULL,				/* class_initialize	  */
    NULL,				/* class_part_initialize  */
    FALSE,				/* class_inited		  */
    Initialize,				/* initialize		  */
    NULL,				/* initialize_hook	  */
    Realize,				/* realize		  */
    actionsList,			/* actions		  */
    XtNumber(actionsList),		/* num_actions		  */
    resources,				/* resources		  */
    XtNumber(resources),	       	/* resource_count	  */
    NULLQUARK,				/* xrm_class		  */
    FALSE,				/* compress_motion	  */
    TRUE,				/* compress_exposure	  */
    TRUE,				/* compress_enterleave    */
    FALSE,				/* visible_interest	  */
    Destroy,				/* destroy		  */
    Resize,				/* resize		  */
    Redisplay,				/* expose		  */
    SetValues,				/* set_values		  */
    NULL,				/* set_values_hook	  */
    XtInheritSetValuesAlmost,		/* set_values_almost	  */
    NULL,				/* get_values_hook	  */
    NULL,				/* accept_focus		  */
    XtVersion,				/* version		  */
    NULL,				/* callback_private	  */
    defaultTranslations,		/* tm_table		  */
    XtInheritQueryGeometry,		/* query_geometry	  */
    XtInheritDisplayAccelerator,	/* display_accelerator	  */
    NULL				/* extension		  */
  },  /* CoreClass fields initialization */
  {
    ChangeSensitive,			/* change_sensitive	  */
    ParentExpose,			/* parent_expose	  */
  },  /* StPrimitiveClass fields initialization */
  {
    0,					/* field not used	  */
  },  /* StLabelClass fields initialization */
  {
    0,					/* field not used	  */
  },  /* StSimpleSwitchClass fields initialization */
  {
    0,					/* field not used	  */
  },  /* StSimpleButtonClass fields initialization */
  {
    0,					/* field not used	  */
  },  /* StMenuItemClass fields initialization */
};

  /* for public consumption */
WidgetClass stMenuItemWidgetClass = (WidgetClass)&stMenuItemClassRec;

/****************************************************************
 *
 * Private Data
 *
 ****************************************************************/

/* Define shape of arrow */

#define arrow_shape_width 16
#define arrow_shape_height 12
static char arrow_shape_bits[] = {
   0x18, 0x00, 0x18, 0x08, 0x18, 0x18, 0x18, 0x38, 0x18, 0x78, 0xff, 0xff,
   0xff, 0xff, 0x18, 0x78, 0x18, 0x38, 0x18, 0x18, 0x18, 0x08, 0x18, 0x00};

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

/***** private initialize *****/

static void InitializeMyGC(w)
	Widget w;
{
	StMenuItemWidget bw = (StMenuItemWidget) w;
	XGCValues values;
	Mask valueMask;
	Display *display = XtDisplay(w);

	values.foreground = PriWforeground;
	values.background = PriWbackground;
	values.clip_mask = PriWarrowShape;
	values.tile = PriWinsensitivePixmap;
	valueMask = GCForeground | GCBackground |GCClipMask | GCTile;
	if(!isSensitive(w)) {
	    values.fill_style = FillTiled;
	    valueMask |= GCFillStyle;
	}
	PriWarrowGC = XCreateGC(display, XRootWindow(display, 
			XDefaultScreen(display)), valueMask, &values);

	values.foreground = PriWbackground;
	values.background = PriWforeground;
	PriWinverseArrowGC = XCreateGC(display, XRootWindow(display, 
			XDefaultScreen(display)), valueMask, &values);
}

static CreateArrowShape(w)
        Widget w;
{
	StMenuItemWidget bw = (StMenuItemWidget) w;
	Display *display = XtDisplay(w);

        PriWarrowShape = XCreateBitmapFromData(display, 
				XRootWindow(display, XDefaultScreen(display)),
                                arrow_shape_bits, arrow_shape_width,
                                arrow_shape_height);
}

/*****  private release *******/

static void FreeArrowShape(w)
        Widget w;
{
	StMenuItemWidget bw = (StMenuItemWidget) w;

        if(PriWarrowShape != None) {
                XFreePixmap(XtDisplay(w), PriWarrowShape);
                PriWarrowShape = None;
        }
}

static void ReleaseMyGC(w)
	Widget w;
{
	StMenuItemWidget bw = (StMenuItemWidget) w;

	XFreeGC(XtDisplay(w), PriWarrowGC);
	XFreeGC(XtDisplay(w), PriWinverseArrowGC);
}

/***** update *****************/

static void UpdateLabelPoint(w)
	Widget w;
{
	StMenuItemWidget bw = (StMenuItemWidget) w;

	switch(PriWjustify) {
	    case XtJustifyLeft :
		PriWlabelRegion.x = PriWhighlightBox.x + PriWmarginX;
		break;
	    case XtJustifyRight :
		PriWlabelRegion.x = 
		    (PriWhighlightBox.width - PriWlabelRegion.width)
		    + PriWhighlightBox.x - PriWmarginX;
		break;
	    case XtJustifyCenter :
		PriWlabelRegion.x = 
		    (PriWhighlightBox.width - PriWlabelRegion.width) / 2
		    + PriWhighlightBox.x;
                break;
	    default :
		PriWlabelRegion.x = 0;
	}

	PriWlabelRegion.y = (PriWhighlightBox.height - PriWlabelRegion.height) 
				/ 2 + PriWhighlightBox.y;
}

static void UpdateHighlightBox(w)
	Widget w;
{
	StMenuItemWidget bw = (StMenuItemWidget) w;

	PriWhighlightBox.x = PriWlineLeft;
	PriWhighlightBox.y = PriWlineTop;
	PriWhighlightBox.width =
			PriWwidth - (PriWlineLeft + PriWlineRight);
	PriWhighlightBox.height =
			PriWheight - (PriWlineTop + PriWlineBottom);

	UpdateLabelPoint(w);
}

static void UpdateArrowOffset(w)
        Widget w;
{
	StMenuItemWidget bw = (StMenuItemWidget) w;
        Display *display = XtDisplay(w);

        PriWarrowRegion.x = max(((PriWhighlightBox.width 
				- PriWarrowRegion.width)
				+ PriWhighlightBox.x - PriWmarginX), 0);
        PriWarrowRegion.y = max(((PriWhighlightBox.height
				 - PriWarrowRegion.height) 
				/ 2 + PriWhighlightBox.y), 0);

	XSetClipOrigin(display, PriWarrowGC, PriWarrowRegion.x,
	    PriWarrowRegion.y);
	XSetClipOrigin(display, PriWinverseArrowGC, PriWarrowRegion.x,
	    PriWarrowRegion.y);
}

static void UpdateArrowRegion(w)
        Widget w;
{
	StMenuItemWidget bw = (StMenuItemWidget) w;

        PriWarrowRegion.width = arrow_shape_width;
        PriWarrowRegion.height = arrow_shape_height;
        UpdateArrowOffset(w);
}

static void UpdateBackground(w)
	Widget w;
{
	StMenuItemWidget bw = (StMenuItemWidget) w;
	
	if(XtIsRealized(w)) {
	    if(!isSensitive(w)) {
		XSetWindowBackgroundPixmap(XtDisplay(w), XtWindow(w), 
		    PriWinsensitiveBorderPixmap);
	    } else {
		if(PriWborderPixmap != UnspecifiedPixmap) {
		    XSetWindowBackgroundPixmap(XtDisplay(w), XtWindow(w), 
			PriWborderPixmap);
		} else {
		    XSetWindowBackground(XtDisplay(w), XtWindow(w), 
			PriWborderColor);
		}
	    }
	}
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
	StMenuItemWidget bw = (StMenuItemWidget) new;

	CreateArrowShape(bw);
	InitializeMyGC(bw);

	PriWarrowRegion.x = 0;
	PriWarrowRegion.y = 0;
	PriWarrowRegion.width = 0;
	PriWarrowRegion.height = 0;
}

/* ARGSUSED */
static void Redisplay(w, event, region)
        Widget w;
        XEvent *event;
        Region region;
{
	StMenuItemWidget bw = (StMenuItemWidget) w;
	GC gc;

	(*SuperClass->core_class.expose) (w, event, region);

	if(PriWarrow) {
		gc = PriWboolContents ? PriWinverseArrowGC : PriWarrowGC;
		XFillRectangle(XtDisplay(w), XtWindow(w), gc, PriWarrowRegion.x,
			PriWarrowRegion.y, PriWarrowRegion.width,
			PriWarrowRegion.height); 
	}
}

/* ARGSUSED */
static void Realize(w, valueMask, attributes)
        register Widget w;
        Mask *valueMask;
        XSetWindowAttributes *attributes;
{
	StMenuItemWidget bw = (StMenuItemWidget) w;

	if(!isSensitive(w)) {
		attributes->background_pixmap = PriWinsensitiveBorderPixmap;
		*valueMask |= CWBackPixmap;
		*valueMask &= ~CWBackPixel;
	} else {
	    if(PriWborderPixmap != UnspecifiedPixmap) {
		attributes->background_pixmap = PriWborderPixmap;
		*valueMask |= CWBackPixmap;
		*valueMask &= ~CWBackPixel;
	    } else {
		attributes->background_pixel = PriWborderColor;
		*valueMask |= CWBackPixel;
	    }
	}

        (*SuperClass->core_class.realize) (w, valueMask, attributes);

        UpdateHighlightBox(w);
	UpdateArrowRegion(w);

	ChangeSensitive(w);
}

/* ARGSUSED */
static void Destroy(w)
        Widget w;
{
	ReleaseMyGC(w);
        FreeArrowShape(w);
}

static void Resize(w)
        Widget w;
{
        UpdateHighlightBox(w);
	UpdateArrowOffset(w);
}

/* ARGSUSED */
static Boolean SetValues (current, request, new)
	Widget current, request, new;
{
	StMenuItemWidget cw = (StMenuItemWidget) current;
	StMenuItemWidget bw = (StMenuItemWidget) new;
	Boolean redisplay = FALSE;
	Boolean need_change_offset = FALSE;

	if(PriWbackground != cw->core.background_pixel
	|| PriWbackgroundPixmap != cw->core.background_pixmap
	|| PriWborderWidth != cw->core.border_width
	|| PriWborderColor != cw->core.border_pixel
	|| PriWborderPixmap != cw->core.border_pixmap) {
	    UpdateBackground(new);
	    redisplay = TRUE;
	}

	if(PriWbackground != cw->core.background_pixel
	|| PriWforeground != cw->stPrimitive.foreground) {
	    ReleaseMyGC(bw);
	    InitializeMyGC(bw);
	    need_change_offset = TRUE;
	}

	if(PriWwidth != cw->core.width
	|| PriWheight != cw->core.height
	|| PriWlineTop != cw->stMenuItem.line_top
	|| PriWlineLeft != cw->stMenuItem.line_left
	|| PriWlineRight != cw->stMenuItem.line_right) {
	    UpdateHighlightBox(bw);
	    need_change_offset = TRUE;
	}

	if(PriWjustify != cw->stLabel.justify
	|| PriWmarginX != cw->stLabel.margin_x
	|| PriWmarginY != cw->stLabel.margin_y
	|| PriWfont->fid != cw->stPrimitive.font->fid
	|| PriWlabel != cw->stLabel.label
	|| PriWbitmap != cw->stLabel.bitmap) {
	    UpdateLabelPoint(bw);
	    need_change_offset = TRUE;
	}

	if(PriWarrow != cw->stMenuItem.arrow
	|| need_change_offset) {
	    UpdateArrowOffset(bw);
	    redisplay = TRUE;
	}

	return redisplay;
}

static Boolean ChangeSensitive(w)
	Widget w;
{
	StMenuItemWidget bw = (StMenuItemWidget) w;
	Display *display = XtDisplay(w);

	(*SuperClass->stPrimitive_class.change_sensitive) (w);

	if(isSensitive(w)) {
	    XSetFillStyle(display, PriWarrowGC, FillSolid);
	    XSetFillStyle(display, PriWinverseArrowGC, FillSolid);
	} else {
	    XSetFillStyle(display, PriWarrowGC, FillTiled);
	    XSetFillStyle(display, PriWinverseArrowGC, FillTiled);
	}	
	UpdateBackground(w);
	
	return TRUE;
}

static Boolean ParentExpose(w)
	Widget w;
{
	(*SuperClass->stPrimitive_class.parent_expose) (w);

	UpdateBackground(w);
	XClearWindow(XtDisplay(w), XtWindow(w));

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
	StMenuItemWidget bw = (StMenuItemWidget) w;

	if(!PriWset) {
	    PriWset = TRUE;
	    PriWcontents = TRUE;
	    Redisplay(w, event, NULL);
	}
}

/* ARGSUSED */
static void Unset(w,event,params,num_params)
	Widget w;
	XEvent *event;
	String *params;		/* unused */
	Cardinal *num_params;	/* unused */
{
	StMenuItemWidget bw = (StMenuItemWidget) w;

	if(PriWset) {
	    PriWset = FALSE;
	    PriWcontents = FALSE;
	    Redisplay(w, event, NULL);
	}
}

/* ARGSUSED */
static void Notify(w,event,params,num_params)
	Widget w;
	XEvent *event;
	String *params;		/* unused */
	Cardinal *num_params;	/* unused */
{
	StMenuItemWidget bw = (StMenuItemWidget) w;

	if(PriWset) { 
	    PriWcontents = FALSE;
	    XtCallCallbacks(w, StNcallback, NULL);
	}
}

/* ARGSUSED */
static void Motion(w,event,params,num_params)
	Widget w;
	XEvent *event;
	String *params;         /* unused */
	Cardinal *num_params;   /* unused */
{
	StMenuItemWidget bw = (StMenuItemWidget)w;

	if(PriWset) {
	   XtCallCallbacks(w, StNsubmenu, event);
	}
}

