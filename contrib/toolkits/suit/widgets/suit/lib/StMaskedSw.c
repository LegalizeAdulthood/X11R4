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
 * StMaskedSwitch.c - SUIT Masked Switch widget
 *
 */

  /* The following are defined for the reader's convenience.  Any
     Xt..Field macro in this code just refers to some field in
     one of the substructures of the WidgetRec.  */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/XawMisc.h>
#include <St/StwP.h>
#include <St/StMaskSwP.h>
#include "StMaskSwI.h"

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

#define offset(field) XtOffset(StMaskedSwitchWidget, field)
static XtResource resources[] = {

    {StNpixmapForSet, XtCPixmap, XtRPixmap, sizeof(Pixmap),
        offset(stMaskSw.pixmap_for_set), XtRPixmap, (caddr_t)None},
    {StNpixmapForUnset, XtCPixmap, XtRPixmap, sizeof(Pixmap),
        offset(stMaskSw.pixmap_for_unset), XtRPixmap, (caddr_t)None},
};
#undef offset

static void UpdatePixmaps();
static void UpdatePixmapsOffset();
static void ChangedPixmaps();
static void FreeDefaultPixmaps();

static void Initialize();
static void Realize();
static void Destroy();
static void Resize();
static void Redisplay();
static Boolean SetValues();
static Boolean ParentExpose();
static void Set();
static void Unset();
static void Notify();


static XtActionsRec actionsList[] = {

    {"set",		Set},
    {"notify",		Notify},
    {"unset",		Unset},
};

#define SuperClass ((StPictrialSwitchWidgetClass)&stPictrialSwitchClassRec)

StMaskedSwitchClassRec stMaskedSwitchClassRec = {
  {
    (WidgetClass) SuperClass,		/* superclass		  */	
    "StMaskedSwitch",			/* class_name		  */
    sizeof(StMaskedSwitchRec),		/* size			  */
    NULL,				/* class_initialize	  */
    NULL,				/* class_part_initialize  */
    FALSE,				/* class_inited		  */
    Initialize,				/* initialize		  */
    NULL,				/* initialize_hook	  */
    Realize,				/* realize		  */
    actionsList,	      		/* actions		  */
    XtNumber(actionsList),	       	/* num_actions		  */
    resources,				/* resources		  */
    XtNumber(resources),		/* resource_count	  */
    NULLQUARK,				/* xrm_class		  */
    FALSE,				/* compress_motion	  */
    TRUE,				/* compress_exposure	  */
    TRUE,				/* compress_enterleave    */
    FALSE,				/* visible_interest	  */
    Destroy,				/* destroy		  */
    Resize,		        	/* resize		  */
    Redisplay,				/* expose		  */
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
    XtInheritChangeSensitive,		/* change_sensitive	  */
    ParentExpose,		/* parent_expose	  */
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
  },  /* StMaskedSwitchClass fields initialization */
};

  /* for public consumption */
WidgetClass stMaskedSwitchWidgetClass = (WidgetClass)&stMaskedSwitchClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

/***** displaying *************/

static void DisplayLabel(w, drawable, gc, offset_x, offset_y, width, height)
	Widget w;
	Drawable drawable;
	GC gc;
	Position offset_x, offset_y;
	Dimension width, height;
{
	StMaskedSwitchWidget bw = (StMaskedSwitchWidget) w;
	StPoint point;
	XRectangle clipRect;

	point.x = (width - PriWlabelRegion.width) / 2 + offset_x;
	point.y = (height - PriWlabelRegion.height) / 2 + offset_y;

	clipRect.x = offset_x;
	clipRect.y = offset_y;
	clipRect.width = width;
	clipRect.height = height;

	if(PriWbitmap != None) {
	    StuDisplayBitmap(w, PriWbitmap, PriWlabelRegion.width,
		PriWlabelRegion.height, PriWlabelLen, drawable,
		gc, point, clipRect);
	    return;
	}

	if(PriWlabel != NULL)
	    StuDisplayString(w, PriWlabel, PriWfont, drawable, gc, point,
		clipRect);
}

static void DrawButton(w, drawable, point_x, point_y, buttonWidth, buttonHeight,
		       borderWidth, borderGC, insideGC)
	Widget w;
	Drawable drawable;
	Position point_x, point_y;
	Dimension buttonWidth, buttonHeight, borderWidth;
	GC borderGC, insideGC;
{
	int width = buttonWidth - borderWidth;
	int height = buttonHeight - borderWidth;
	unsigned int x = point_x + (borderWidth / 2);
	unsigned int y = point_y + (borderWidth / 2);
	Display *display = XtDisplay(w);

	XFillRectangle(display, drawable, insideGC, x, y, width, height);
	XDrawRectangle(display, drawable, borderGC, x, y, width, height);
}

static void RedrawButtonImages(w)
	Widget w;
{
	StMaskedSwitchWidget bw = (StMaskedSwitchWidget) w;
	Dimension buttonWidth = PriWwidth - ThicknessOfButton;
	Dimension buttonHeight = PriWheight - ThicknessOfButton;
	Position dx = ThicknessOfButton;
	Position dy = ThicknessOfButton;

	XFillRectangle(XtDisplay(w), PriWpixmapForSet, PriWbackgroundGC, 0, 0,
	    PriWwidth, PriWheight);
	DrawButton(w, PriWpixmapForSet, dx, dy, buttonWidth, buttonHeight, 
	    PriWborderWidth, PriWborderGC, PriWinsideGC);
	DisplayLabel(w, PriWpixmapForSet, PriWlabelGC, dx, dx, buttonWidth, 
	    buttonHeight);

	XFillRectangle(XtDisplay(w), PriWpixmapForUnset, PriWbackgroundGC,
	    0, 0, PriWwidth, PriWheight); 
	DrawButton(w, PriWpixmapForUnset, dx, dy, buttonWidth, buttonHeight, 
	    PriWborderWidth, PriWborderGC, PriWborderGC);
	DrawButton(w, PriWpixmapForUnset, 0, 0, buttonWidth, buttonHeight, 
	    PriWborderWidth, PriWborderGC, PriWinsideGC);
	DisplayLabel(w, PriWpixmapForUnset, PriWlabelGC, 0, 0, buttonWidth, 
	    buttonHeight);
}

/***** private release  *******/

static void FreeDefaultPixmaps(w)
	Widget w;
{
	StMaskedSwitchWidget bw = (StMaskedSwitchWidget) w;

	XFreePixmap(XtDisplay(w), PriWpixmapForSet);
	XFreePixmap(XtDisplay(w), PriWpixmapForUnset);
	PriWpixmapsAreMine = FALSE;
}

/***** default accessing ******/

static void DefaultPixmaps(w)
	Widget w;
{
	StMaskedSwitchWidget bw = (StMaskedSwitchWidget) w;

	PriWpixmapForSet = XCreatePixmap(XtDisplay(w), XtWindow(w), PriWwidth, 
				PriWheight, PriWdepth);
	PriWpixmapForUnset = XCreatePixmap(XtDisplay(w), XtWindow(w), 
	    PriWwidth, PriWheight, PriWdepth);
	RedrawButtonImages(w);
}

static void CreateDefaultPixmaps(w)
	Widget w;
{
	StMaskedSwitchWidget bw = (StMaskedSwitchWidget) w;

	if(PriWdefaultPixmaps != NULL)
	    (*PriWdefaultPixmaps) (w);
	else
	    DefaultPixmaps(w);
	PriWpixmapsAreMine = TRUE;
}

/***** updating ***************/

static void UpdatePixmaps(w)
	Widget w;
{
	StMaskedSwitchWidget bw = (StMaskedSwitchWidget) w;

	if(PriWpixmapsAreMine) {
	    FreeDefaultPixmaps(w);
	    CreateDefaultPixmaps(w);
	    ChangedPixmaps(w);
	} else {
	    UpdatePixmapsOffset(w);
	}
}

static void UpdatePixmapsOffset(w)
	Widget w;
{
	StMaskedSwitchWidget bw = (StMaskedSwitchWidget) w;

	PriWsetPixmap.x = (PriWwidth - PriWsetPixmap.width) / 2;
	PriWsetPixmap.y = (PriWheight - PriWsetPixmap.height) / 2;
	PriWunsetPixmap.x = (PriWwidth - PriWunsetPixmap.width) / 2;
	PriWunsetPixmap.y = (PriWheight - PriWunsetPixmap.height) / 2;
}

/***** changing ***************/

static void ChangedPixmaps(w)
	Widget w;
{
	StMaskedSwitchWidget bw = (StMaskedSwitchWidget) w;
	Window root;
	unsigned int borderWidth;

	if(PriWpixmapForSet == None || PriWpixmapForUnset == None)
	    CreateDefaultPixmaps(w);
            
	XGetGeometry(XtDisplay(w), PriWpixmapForSet, &root, 
	    &PriWsetPixmap.x, &PriWsetPixmap.y,
	    &PriWsetPixmap.width, &PriWsetPixmap.height,
	    &borderWidth, &PriWsetPixmap.depth);

	XGetGeometry(XtDisplay(w), PriWpixmapForUnset, &root, 
	    &PriWunsetPixmap.x, &PriWunsetPixmap.y,
	    &PriWunsetPixmap.width, &PriWunsetPixmap.height,
	    &borderWidth, &PriWunsetPixmap.depth);

	UpdatePixmapsOffset(w);
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
	StMaskedSwitchWidget bw = (StMaskedSwitchWidget) new;

	PriWpixmapsAreMine = FALSE;
	PriWdefaultPixmaps = DefaultPixmaps;
}

/* ARGSUSED */
static void Realize(w, valueMask, attributes)
	Widget w;
	Mask *valueMask;
	XSetWindowAttributes *attributes;
{
	register StMaskedSwitchWidget bw = (StMaskedSwitchWidget) w;

	(*SuperClass->core_class.realize) (w, valueMask, attributes);

	if(PriWpixmapForSet == None || PriWpixmapForUnset == None)
	    CreateDefaultPixmaps(bw);
	ChangedPixmaps(bw);
    
}

/* ARGSUSED */
static void Destroy(w)
	Widget w;
{
	StMaskedSwitchWidget bw = (StMaskedSwitchWidget) w;

	if(PriWpixmapsAreMine)
	    FreeDefaultPixmaps(w);
}

/* ARGSUSED */
static void Redisplay(w, event, region)
	Widget w;
	XEvent *event;	
	Region region;
{
	StMaskedSwitchWidget bw = (StMaskedSwitchWidget) w;
	int x, y;
	unsigned int width, height, depth;
	Pixmap pixmap;

	if(PriWboolContents) {
	    depth = PriWsetPixmap.depth;
	    width = PriWsetPixmap.width;
	    height = PriWsetPixmap.height;
	    x = PriWsetPixmap.x;
	    y = PriWsetPixmap.y;
	    pixmap = PriWpixmapForSet;
	} else {
	    depth = PriWunsetPixmap.depth;
	    width = PriWunsetPixmap.width;
	    height = PriWunsetPixmap.height;
	    x = PriWunsetPixmap.x;
	    y = PriWunsetPixmap.y;
	    pixmap = PriWpixmapForUnset;
	}

	if(depth == 1) {
	    XCopyPlane(XtDisplay(w), pixmap, XtWindow(w), PriWnormalGC,
		0, 0, width, height, x, y, 1L);
	} else {
	    XCopyArea(XtDisplay(w), pixmap, XtWindow(w), PriWnormalGC,
		0, 0, width, height, x, y);
	}
}

static void Resize(w)
	Widget w;
{
	UpdatePixmaps(w);
}

/* ARGSUSED */
static Boolean SetValues (current, request, new)
	Widget current, request, new;
{
	StMaskedSwitchWidget cw = (StMaskedSwitchWidget) current;
	StMaskedSwitchWidget bw = (StMaskedSwitchWidget) new;
	Boolean redisplay = FALSE;

	if(PriWpixmapForSet != cw->stMaskSw.pixmap_for_set
	|| PriWpixmapForUnset != cw->stMaskSw.pixmap_for_unset) {
	    if(PriWpixmapsAreMine)
		FreeDefaultPixmaps(cw);
	    ChangedPixmaps(bw);
	    redisplay = TRUE;
	}

	if ((PriWsensitive != cw->core.sensitive
	|| PriWlabel != cw->stLabel.label
	|| PriWbitmap != cw->stLabel.bitmap
	|| PriWfont->fid != cw->stPrimitive.font->fid
	|| PriWforeground != cw->stPrimitive.foreground
	|| PriWbackground != cw->core.background_pixel
	|| PriWbackgroundPixmap != cw->core.background_pixmap
	|| PriWborderWidth != cw->core.border_width
	|| PriWborderColor != cw->core.border_pixel
	|| PriWborderPixmap != cw->core.border_pixmap
	|| PriWwidth != cw->core.width
	|| PriWheight != cw->core.height)
	&& PriWpixmapsAreMine) {
	    UpdatePixmaps(bw);
	    redisplay = TRUE;
	}
    
	return (redisplay || PriWcontents != bw->stPrimitive.contents);
}

static Boolean ParentExpose(w)
	Widget w;
{
	(*SuperClass->stPrimitive_class.parent_expose) (w);
	UpdatePixmaps(w);

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
	StMaskedSwitchWidget bw = (StMaskedSwitchWidget) w;

	PriWset = TRUE;
	PriWcontents = (Boolean)!PriWcontents;

	Redisplay(w, event, NULL);
}

/* ARGSUSED */
static void Unset(w,event,params,num_params)
	Widget w;
	XEvent *event;
	String *params;		/* unused */
	Cardinal *num_params;	/* unused */
{
	StMaskedSwitchWidget bw = (StMaskedSwitchWidget) w;

	PriWset = FALSE;
	PriWcontents = (Boolean)!PriWcontents;

	Redisplay(w, event, NULL);
}

/* ARGSUSED */
static void Notify(w,event,params,num_params)
	Widget w;
	XEvent *event;
	String *params;		/* unused */
	Cardinal *num_params;	/* unused */
{
	StMaskedSwitchWidget bw = (StMaskedSwitchWidget) w;

	if(PriWset)
	    XtCallCallbacks(w, StNcallback, PriWboolContents);
}
