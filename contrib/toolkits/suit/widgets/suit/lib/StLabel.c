/*
 *      Original Source: ./lib/Xaw/Label.c
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */
#ifndef lint
static char Xrcsid[] = "$XConsortium: Label.c,v 1.62 88/10/21 08:15:13 swick Exp $";
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
 * StLabel.c - SUIT Label widget
 *
 */

  /* The following are defined for the reader's convenience.  Any
     Xt..Field macro in this code just refers to some field in
     one of the substructures of the WidgetRec.  */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/XawMisc.h>
#include <St/StwP.h>
#include <St/StLabelP.h>
#include "StLabelI.h"
 
/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

#define offset(field) XtOffset(StLabelWidget, field)
static XtResource resources[] = { 

    {StNlabel, XtCLabel, XtRString, sizeof(String),
	offset(stLabel.label), XtRString, NULL},
    {StNbitmap, XtCPixmap, XtRPixmap, sizeof(Pixmap),
	offset(stLabel.bitmap), XtRPixmap, (caddr_t)None},
    {StNjustify, XtCJustify, XtRJustify, sizeof(XtJustify),
	offset(stLabel.justify), XtRImmediate, (caddr_t)XtJustifyCenter},
    {StNdrawable, XtCPixmap, XtRPixmap, sizeof(Pixmap),
	offset(stLabel.drawable), XtRPixmap, (caddr_t)None},
    {StNmarginX, StCLabelMargin, XtRDimension, sizeof(Dimension),
	offset(stLabel.margin_x), XtRImmediate, (caddr_t)2},
    {StNmarginY, StCLabelMargin, XtRDimension, sizeof(Dimension),
	offset(stLabel.margin_y), XtRImmediate, (caddr_t)0},
};
#undef offset

static void UpdataLabelBox();
static void UpdateLabelRegion();
static void UpdateLabelPoint();
static void DisplayLabel();
static void EraseLabel();

static void Initialize();
static void Destroy();
static void Realize();
static void Redisplay();
static void Resize();
static Boolean SetValues();
static Boolean ChangeSensitive();
static Boolean ParentExpose();


#define SuperClass ((StPrimitiveWidgetClass)&stPrimitiveClassRec)

StLabelClassRec stLabelClassRec = {
  {
    (WidgetClass) SuperClass,		/* superclass		  */	
    "StLabel",				/* class_name		  */
    sizeof(StLabelRec),			/* size			  */
    NULL,				/* class_initialize	  */
    NULL,				/* class_part_initialize  */
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
    Resize,				/* resize		  */
    Redisplay,				/* expose		  */
    SetValues,				/* set_values		  */
    NULL,				/* set_values_hook	  */
    XtInheritSetValuesAlmost,		/* set_values_almost	  */
    NULL,				/* get_values_hook	  */
    NULL,				/* accept_focus		  */
    XtVersion,				/* version		  */
    NULL,				/* callback_private	  */
    NULL,				/* tm_table		  */
    NULL,	          		/* query_geometry	  */
    XtInheritDisplayAccelerator,	/* display_accelerator	  */
    NULL				/* extension		  */
  },  /* CoreClass fields initialization */
  {
    ChangeSensitive,			/* change_sensitive	  */
    ParentExpose,			/* parent_expose	  */
  },  /* StPrimitiveClass fields initialization */
  {
    NULL,				/* field not used	  */
  },  /* StLabelClass fields initialization */
};

  /* for public consumption */
WidgetClass stLabelWidgetClass = (WidgetClass) &stLabelClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

/***** displaying *************/

static void DisplayLabel(w)
	Widget w;
{
	StLabelWidget bw = (StLabelWidget) w;
	StPoint point;
	Drawable drawable;

	point.x = PriWlabelRegion.x;
	point.y = PriWlabelRegion.y;

	drawable = (PriWdrawable != None) ? PriWdrawable : XtWindow(w);

	if(PriWbitmap != None) {
	    StuDisplayBitmap(w, PriWbitmap, PriWlabelRegion.width,
		PriWlabelRegion.height, PriWlabelLen, drawable,
		PriWlabelGC, point, PriWlabelBox);
	    return;
	}

	if(PriWlabel != NULL) 
	    StuDisplayString(w, PriWlabel, PriWfont, drawable, PriWlabelGC, 
		point, PriWlabelBox);
}

static void EraseLabel(w)
	Widget w;
{
	StLabelWidget bw = (StLabelWidget) w;

	if(PriWdrawable == None) {
	    XClearArea(XtDisplay(w), XtWindow(w), PriWlabelRegion.x,
		PriWlabelRegion.y, PriWlabelRegion.width, 
		PriWlabelRegion.height, FALSE);
	} else {
	    /*XFillRectangle(XtDisplay(w), PriWdrawable, PriWinverseGC,
		PriWlabelRegion.x, PriWlabelRegion.y, PriWlabelRegion.width,
		PriWlabelRegion.height);*/
	}
}

/***** accessing **************/

static StPoint GetLabelPoint(w)
	Widget w;
{
	StLabelWidget bw = (StLabelWidget) w;
	StPoint point;

	switch(PriWjustify) {
	    case XtJustifyLeft :
		point.x = PriWlabelBox.x;
		break;
	    case XtJustifyRight :
		point.x = (PriWlabelBox.width - PriWlabelRegion.width)
				+ PriWlabelBox.x;
		break;
	    case XtJustifyCenter :
		point.x = (PriWlabelBox.width - PriWlabelRegion.width) / 2
				+ PriWlabelBox.x;
		break;
	    default :
		point.x = 0;
	}
    
	point.y = (PriWlabelBox.height - PriWlabelRegion.height) / 2
			+ PriWlabelBox.y;

	return point;
}

/***** updating ***************/

static void UpdateLabelPoint(w)
	Widget w;
{
	StLabelWidget bw = (StLabelWidget) w;
	StPoint newPoint;

	newPoint = GetLabelPoint(w);
	PriWlabelRegion.x = newPoint.x;
	PriWlabelRegion.y = newPoint.y;
}

static void UpdateLabelBox(w)
	Widget w;
{
	StLabelWidget bw = (StLabelWidget) w;
	XRectangle box;

	if(PriWdrawable != None) {
	    Window root;
	    int x, y;
	    unsigned int width, height, borderWidth, depth;

	    XGetGeometry(XtDisplay(w), PriWdrawable, &root, &x, &y,
		&width, &height, &borderWidth, &depth);
	    box.x = 0;
	    box.y = 0;
	    box.width = width;
	    box.height = height;
	} else {
	    Dimension dx = PriWmarginX * 2;
	    Dimension dy = PriWmarginY * 2;

	    box.x = PriWmarginX;
	    box.y = PriWmarginY;
	    box.width = max(dx, PriWwidth) - dx;
	    box.height = max(dy, PriWheight) - dy;
	}
	PriWlabelBox = box;
}

static void UpdateLabelRegion(w)
	Widget w;
{
	StLabelWidget bw = (StLabelWidget) w;
	StPoint newPoint;
	
	if(PriWbitmap != None) {
	    Window root;
	    int x, y;
	    unsigned int width, height, borderWidth, depth;

	    XGetGeometry(XtDisplay(w), PriWbitmap, &root, &x, &y,
		&width, &height, &borderWidth, &depth);
	    PriWlabelRegion.width = width;
	    PriWlabelRegion.height = height;
	    PriWlabelLen = depth;
	} else {
	    if(PriWlabel != NULL) {
		PriWlabelRegion.width = XTextWidth(PriWfont, PriWlabel,
		    (PriWlabelLen = (int)StStrlen(PriWlabel)));
	        PriWlabelRegion.height = PriWfont->max_bounds.ascent
		    + PriWfont->max_bounds.descent;
	    } else {
		PriWlabelRegion.width = 0;
		PriWlabelRegion.height = 0;
		PriWlabelLen = 0;
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
	StLabelWidget bw = (StLabelWidget) new;

	if(PriWlabel != NULL) {
	    PriWlabel = StStrcopy(PriWlabel);
	}
	UpdateLabelRegion(new);
	PriWlabelGC = isSensitive(new) ? PriWnormalGC : PriWnormalGrayGC;
}

/* ARGSUSED */
static void Destroy(w)
	Widget w;
{
	StLabelWidget bw = (StLabelWidget) w;

	if(PriWlabel != NULL) {
	    StStrfree(PriWlabel);
	}
}

/* ARGSUSED */
static void Realize(w, valueMask, attributes)
	register Widget w;
	Mask *valueMask;
	XSetWindowAttributes *attributes;
{
	(*SuperClass->core_class.realize) (w, valueMask, attributes);

	UpdateLabelBox(w);
	UpdateLabelPoint(w);
}

/* ARGSUSED */
static void Redisplay(w, event, region)
	Widget w;
	XEvent *event;	
	Region region;
{
	DisplayLabel(w);
}

static void Resize(w)
	Widget w;
{
	EraseLabel(w);
	UpdateLabelBox(w);
	UpdateLabelPoint(w);
	DisplayLabel(w);
}

/* ARGSUSED */
static Boolean SetValues (current, request, new)
	Widget current, request, new;
{
	StLabelWidget cw = (StLabelWidget) current;
	StLabelWidget bw = (StLabelWidget) new;
	Boolean redisplay = FALSE;
	Boolean need_change_point = FALSE;

	if(PriWlabel != cw->stLabel.label) {
	    PriWbitmap = None;
	    if(PriWlabel != NULL)
		PriWlabel = StStrcopy(PriWlabel);
	    need_change_point = TRUE;
	}
	    
	if(PriWbitmap != cw->stLabel.bitmap
	|| PriWfont->fid != cw->stPrimitive.font->fid
	|| need_change_point) {
	    UpdateLabelRegion(bw);
	    need_change_point = TRUE;
	}

	if(PriWwidth != cw->core.width 
	|| PriWheight != cw->core.height
	|| PriWdrawable != cw->stLabel.drawable
	|| PriWmarginX != cw->stLabel.margin_x
	|| PriWmarginY != cw->stLabel.margin_y) {
	    UpdateLabelBox(bw);
	    need_change_point = TRUE;
	}

	if(PriWjustify != cw->stLabel.justify
	|| need_change_point) {
	    UpdateLabelPoint(bw);
	    redisplay = TRUE;
	}

	return redisplay;
}

static Boolean ChangeSensitive(w)
	Widget w;
{
	StLabelWidget bw = (StLabelWidget) w;

	(*SuperClass->stPrimitive_class.change_sensitive) (w);
	PriWlabelGC = isSensitive(w) ? PriWnormalGC : PriWnormalGrayGC;

	return FALSE;
}

static Boolean ParentExpose(w)
	Widget w;
{
	(*SuperClass->stPrimitive_class.parent_expose) (w);

	return TRUE;
}
