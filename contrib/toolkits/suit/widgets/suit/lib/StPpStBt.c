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
 * StPopupSheetButton.c - SUIT Popup-Sheet Button widget
 *
 */

  /* The following are defined for the reader's convenience.  Any
     Xt..Field macro in this code just refers to some field in
     one of the substructures of the WidgetRec.  */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/XawMisc.h>
#include <St/StwP.h>
#include <St/StPpStBnP.h>
#include "StPpStBnI.h"

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

static void Initialize();
static void Destroy();
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

#define SuperClass ((StMaskedButtonWidgetClass)&stMaskedButtonClassRec)

StPopupSheetButtonClassRec stPopupSheetButtonClassRec = {
  {
    (WidgetClass) SuperClass,		/* superclass		  */	
    "StPopupSheetButton",		/* class_name		  */
    sizeof(StPopupSheetButtonRec),	/* size			  */
    NULL,				/* class_initialize	  */
    NULL,				/* class_part_initialize  */
    FALSE,				/* class_inited		  */
    Initialize,				/* initialize		  */
    NULL,				/* initialize_hook	  */
    XtInheritRealize,			/* realize		  */
    actionsList,	      		/* actions		  */
    XtNumber(actionsList),	       	/* num_actions		  */
    NULL,				/* resources		  */
    0,	                        	/* resource_count	  */
    NULLQUARK,				/* xrm_class		  */
    FALSE,				/* compress_motion	  */
    TRUE,				/* compress_exposure	  */
    TRUE,				/* compress_enterleave    */
    FALSE,				/* visible_interest	  */
    Destroy,				/* destroy		  */
    XtInheritResize,			/* resize		  */
    XtInheritExpose,	       		/* expose		  */
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
  },  /* StPictrialSwitchClass fields initialization */
  {
    0,					/* field not used	  */
  },  /* StMaskedSwitchClass fields initialization */
  {
    0,					/* field not used	  */
  },  /* StMaskedButtonClass fields initialization */
  {
    0,					/* field not used	  */
  },  /* StPopupSheetButtonClass fields initialization */
};

  /* for public consumption */
WidgetClass stPopupSheetButtonWidgetClass = 
				(WidgetClass)&stPopupSheetButtonClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

/***** private initialze *****/

static void InitializeMyGC(w)
	Widget w;
{
	StPopupSheetButtonWidget bw = (StPopupSheetButtonWidget) w;
	XGCValues values;
	Mask valueMask;

	values.foreground = PriWborderColor;
	values.background = PriWparent->core.background_pixel;
	values.fill_style = FillTiled;
	values.tile = PriWinsensitiveBorderPixmap;
	valueMask = GCForeground | GCBackground | GCFillStyle | GCTile;
	PriWshadowGC = XtGetGC(w, valueMask, &values);
}

static void ReleaseMyGC(w)
	Widget w;
{
	StPopupSheetButtonWidget bw = (StPopupSheetButtonWidget) w;

	XtReleaseGC(w, PriWshadowGC);
}

static void DisplayLabel(w, drawable, gc, offset_x, offset_y, width, height)
	Widget w;
	Drawable drawable;
	GC gc;
	Position offset_x, offset_y;
	Dimension width, height;
{
	StPopupSheetButtonWidget bw = (StPopupSheetButtonWidget) w;
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
	int width = buttonWidth - borderWidth;
	int height = buttonHeight - borderWidth;
	XPoint points[8];
	Display *display = XtDisplay(w);

	points[0].x = point_x + (borderWidth / 2) + width - CuttingOfButton;
	points[0].y = point_y + (borderWidth / 2) + height;
	points[1].x = -(width - CuttingOfButton);
	points[1].y = 0;
	points[2].x = 0;
	points[2].y = -height;
	points[3].x = width;
	points[3].y = 0;
	points[4].x = 0;
	points[4].y = height - CuttingOfButton;
	points[5].x = -CuttingOfButton;
	points[5].y = CuttingOfButton;
	points[6].x = 0;
	points[6].y = -CuttingOfButton;
	points[7].x = CuttingOfButton;
	points[7].y = 0;

	XFillPolygon(display, drawable, insideGC, points, 6, Complex,
	    CoordModePrevious);
	XDrawLines(display, drawable, borderGC, points, 6, CoordModePrevious);
}

static void RedrawButtonImages(w)
	Widget w;
{
	StPopupSheetButtonWidget bw = (StPopupSheetButtonWidget) w;
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

	XFillRectangle(XtDisplay(w), PriWpixmapForUnset, PriWbackgroundGC, 0, 0,
	    PriWwidth, PriWheight);
	DrawButton(w, PriWpixmapForUnset, dx, dy, buttonWidth, buttonHeight,
	    PriWborderWidth, PriWshadowGC, PriWshadowGC);
	DrawButton(w, PriWpixmapForUnset, 0, 0, buttonWidth, buttonHeight,
	    PriWborderWidth, PriWborderGC, PriWinsideGC);
	DisplayLabel(w, PriWpixmapForUnset, PriWlabelGC, 0, 0, buttonWidth, 
	    buttonHeight);
}

static void DefaultPixmaps(w)
	Widget w;
{
	StPopupSheetButtonWidget bw = (StPopupSheetButtonWidget) w;

	PriWpixmapForSet = XCreatePixmap(XtDisplay(w), XtWindow(w), PriWwidth,
				PriWheight, PriWdepth);
	PriWpixmapForUnset = XCreatePixmap(XtDisplay(w), XtWindow(w), PriWwidth,
				PriWheight, PriWdepth);
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
	StPopupSheetButtonWidget bw = (StPopupSheetButtonWidget) new;

	PriWdefaultPixmaps = DefaultPixmaps;
	InitializeMyGC(new);
}

/* ARGSUSED */
static void Destroy(w)
	Widget w;
{
	ReleaseMyGC(w);
}

/* ARGSUSED */
static Boolean SetValues (current, request, new)
	Widget current, request, new;
{
	StPopupSheetButtonWidget cw = (StPopupSheetButtonWidget) current;
	StPopupSheetButtonWidget bw = (StPopupSheetButtonWidget) new;
	Boolean redisplay = FALSE;

	if(cw->core.border_pixel != PriWborderColor
	|| cw->core.border_pixmap != PriWborderPixmap) {
	    ReleaseMyGC(bw);
	    InitializeMyGC(bw);
	    RedrawButtonImages(bw);

	    redisplay = TRUE;
	}

	return redisplay;
}

static Boolean ParentExpose(w)
	Widget w;
{
	(*SuperClass->stPrimitive_class.parent_expose) (w);
	ReleaseMyGC(w);
	InitializeMyGC(w);
	RedrawButtonImages(w);

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
	StPopupSheetButtonWidget bw = (StPopupSheetButtonWidget) w;

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
	StPopupSheetButtonWidget bw = (StPopupSheetButtonWidget) w;

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
	StPopupSheetButtonWidget bw = (StPopupSheetButtonWidget) w;

	if(PriWset) {
	    Unset(w, event, params, num_params);
	    XtCallCallbacks(w, StNcallback, (Boolean)PriWcontents);
	}
}
