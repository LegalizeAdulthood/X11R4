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
 * StButton.c - SUIT Button widget
 *
 */

  /* The following are defined for the reader's convenience.  Any
     Xt..Field macro in this code just refers to some field in
     one of the substructures of the WidgetRec.  */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/XawMisc.h>
#include <St/StwP.h>
#include <St/StButtonP.h>
#include "StButtonI.h"

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

static void Initialize();
static void Set();
static void Unset();
static void Notify();


static XtActionsRec actionsList[] = {

    {"set",		Set},
    {"notify",		Notify},
    {"unset",		Unset},
};

#define SuperClass ((StMaskedButtonWidgetClass)&stMaskedButtonClassRec)

StButtonClassRec stButtonClassRec = {
  {
    (WidgetClass) SuperClass,		/* superclass		  */	
    "StfButton",			/* class_name		  */
    sizeof(StButtonRec),		/* size			  */
    NULL,				/* class_initialize	  */
    NULL,				/* class_part_initialize  */
    FALSE,				/* class_inited		  */
    Initialize,				/* initialize		  */
    NULL,				/* initialize_hook	  */
    XtInheritRealize,			/* realize		  */
    actionsList,	      		/* actions		  */
    XtNumber(actionsList),	       	/* num_actions		  */
    NULL,				/* resources		  */
    0,	 				/* resource_count	  */
    NULLQUARK,				/* xrm_class		  */
    FALSE,				/* compress_motion	  */
    TRUE,				/* compress_exposure	  */
    TRUE,				/* compress_enterleave    */
    FALSE,				/* visible_interest	  */
    NULL,				/* destroy		  */
    NULL,		        	/* resize		  */
    XtInheritExpose,			/* expose		  */
    NULL,				/* set_values		  */
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
  },  /* StMaskedSwitchClass fields initialization */
  {
    0,					/* field not used	  */
  },  /* StMaskedButtonClass fields initialization */
  {
    0,					/* field not used	  */
  },  /* StButtonClass fields initialization */
};

  /* for public consumption */
WidgetClass stButtonWidgetClass = (WidgetClass)&stButtonClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static void DisplayLabel(w, drawable, gc, offset_x, offset_y, width, height)
	Widget w;
	Drawable drawable;
	GC gc;
	Position offset_x, offset_y;
	Dimension width, height;
{
	StButtonWidget bw = (StButtonWidget) w;
	StPoint point;
	XRectangle clipRect;

	point.x =  (width - PriWlabelRegion.width) / 2 + offset_x;
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
	int width = buttonWidth - (CuttingOfButton * 2) - borderWidth;
	int height = buttonHeight -(CuttingOfButton * 2) - borderWidth;
	XPoint points[9];
	Display *display = XtDisplay(w);

	points[0].x = point_x + (borderWidth / 2);
	points[0].y = CuttingOfButton + point_y + (borderWidth / 2);
	points[1].x = 0;
	points[1].y = height;
	points[2].x = CuttingOfButton;
	points[2].y = CuttingOfButton;
	points[3].x = width;
	points[3].y = 0;
	points[4].x = CuttingOfButton;
	points[4].y = -CuttingOfButton;
	points[5].x = 0;
	points[5].y = -height;
	points[6].x = -CuttingOfButton;
	points[6].y = -CuttingOfButton;
	points[7].x = -width;
	points[7].y = 0;
	points[8].x = -CuttingOfButton;
	points[8].y = CuttingOfButton;
	
	XFillPolygon(display, drawable, insideGC, points, 9, Complex,
	    CoordModePrevious); 
	XDrawLines(display, drawable, borderGC, points, 9, CoordModePrevious);
}

static void RedrawButtonImages(w)
	Widget w;
{
	StButtonWidget bw = (StButtonWidget)w;
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

static void CreateDefaultPixmaps(w)
	Widget w;
{
	StButtonWidget bw = (StButtonWidget) w;

	PriWpixmapForSet = XCreatePixmap(XtDisplay(w), XtWindow(w), PriWwidth, 
				PriWheight, PriWdepth);
	PriWpixmapForUnset = XCreatePixmap(XtDisplay(w), XtWindow(w), 
				PriWwidth, PriWheight, PriWdepth);
	RedrawButtonImages(w);
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
	StButtonWidget bw = (StButtonWidget) new;

	PriWdefaultPixmaps = CreateDefaultPixmaps;
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
	StButtonWidget bw = (StButtonWidget) w;

	if(!PriWboolContents) {
	    PriWset = TRUE;
	    PriWcontents = TRUE;
	    (*SuperClass->core_class.expose) (w, event, NULL);
	}
}

/* ARGSUSED */
static void Unset(w,event,params,num_params)
	Widget w;
	XEvent *event;
	String *params;		/* unused */
	Cardinal *num_params;	/* unused */
{
	StButtonWidget bw = (StButtonWidget) w;

	if(PriWboolContents) {
	    PriWset = FALSE;
	    PriWcontents = FALSE;
	    (*SuperClass->core_class.expose) (w, event, NULL);
	}
}

/* ARGSUSED */
static void Notify(w,event,params,num_params)
	Widget w;
	XEvent *event;
	String *params;		/* unused */
	Cardinal *num_params;	/* unused */
{
	StButtonWidget bw = (StButtonWidget) w;

	if(PriWset) {
	    Unset(w, event, params, num_params);
	    XtCallCallbacks(w, StNcallback, PriWboolContents);
	}
}
