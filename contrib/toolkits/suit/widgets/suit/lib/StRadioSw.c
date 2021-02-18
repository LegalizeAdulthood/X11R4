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
 * StRadioSwitch.c - StRadioSwitch widget
 *
 */

  /* The following are defined for the reader's convenience.  Any
     Xt..Field macro in this code just refers to some field in
     one of the substructures of the WidgetRec.  */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/XawMisc.h>
#include <St/StwP.h>
#include <St/StRadioSwP.h>
#include "StRadioSwI.h"

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

static void Initialize();
static void Destroy();
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

#define SuperClass ((StMaskedSwitchWidgetClass)&stMaskedSwitchClassRec)

StRadioSwitchClassRec stRadioSwitchClassRec = {
  {
    (WidgetClass) SuperClass,		/* superclass		  */	
    "StRadioSwitch",				/* class_name		  */
    sizeof(StRadioSwitchRec),		/* size			  */
    NULL,				/* class_initialize	  */
    NULL,				/* class_part_initialize  */
    FALSE,				/* class_inited		  */
    Initialize,				/* initialize		  */
    NULL,				/* initialize_hook	  */
    XtInheritRealize,			/* realize		  */
    actionsList,			/* actions		  */
    XtNumber(actionsList),		/* num_actions		  */
    NULL,				/* resources		  */
    0,					/* resource_count	  */
    NULLQUARK,				/* xrm_class		  */
    FALSE,				/* compress_motion	  */
    TRUE,				/* compress_exposure	  */
    TRUE,				/* compress_enterleave    */
    FALSE,				/* visible_interest	  */
    Destroy,				/* destroy		  */
    NULL,				/* resize		  */
    XtInheritExpose,			/* expose		  */
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
  },  /* StMaskedSwitchClass fields initialization */
  {
    0,					/* field not used	  */
  },  /* StRadioSwitchClass fields initialization */
};

  /* for public consumption */
WidgetClass stRadioSwitchWidgetClass = (WidgetClass) &stRadioSwitchClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

/***** private initialize *****/

static void InitializeMyGC(w)
	Widget w;
{
	StRadioSwitchWidget bw = (StRadioSwitchWidget) w;

	XGCValues values;
	Mask valueMask;

	values.foreground = PriWborderColor;
	values.background = PriWbackground;
	values.line_width = InnerLineWidth;
	valueMask = GCForeground | GCBackground | GCLineWidth;
	if(!isSensitive(w)) {
	    values.fill_style = FillTiled;
	    values.tile = PriWinsensitiveBorderPixmap;
	    valueMask |= GCFillStyle;
	    valueMask |= GCTile;
	}
	PriWinnerLineGC = XtGetGC(w, valueMask, &values);
}

static void ReleaseMyGC(w)
	Widget w;
{
	StRadioSwitchWidget bw = (StRadioSwitchWidget) w;

	XtReleaseGC(w, PriWinnerLineGC);
}

static void DrawCircle(w, drawable, diameter, offset_x, offset_y, borderWidth,
			borderGC, insideGC)
	Widget w;
	Dimension diameter;
	Drawable drawable;
	Position offset_x, offset_y;
	Dimension borderWidth;
	GC borderGC, insideGC;
{
	StRadioSwitchWidget bw = (StRadioSwitchWidget)w;
	int x, y, adjust;
	unsigned int width, height;
	int angle1, angle2;

	diameter = diameter / 2 * 2;
	adjust = diameter % 2;
	x = (PriWwidth - diameter) / 2 + adjust + offset_x;
	y = (PriWheight - diameter) / 2 + adjust + offset_y;
	width = height = diameter;
	angle1 = 0;
	angle2 = 360 * 64;

	XFillArc(XtDisplay(w), drawable, insideGC, x, y, width, height,
	    angle1, angle2);

	if(borderWidth != 0)
	    XDrawArc(XtDisplay(w), drawable, borderGC, x, y,
		width, height, angle1, angle2);
}

static void RedrawWidget(w)
	Widget w;
{
	StRadioSwitchWidget bw = (StRadioSwitchWidget) w;
	Dimension innerD, outterD, delta;

	if(PriWpixmapForSet == None || PriWpixmapForUnset == None) return;

	outterD = min(PriWwidth, PriWheight) - PriWborderWidth;
	innerD = min(PriWwidth, PriWheight) - InnerLineWidth;
	delta = (PriWborderWidth + EdgeWidth) * 2;
	innerD -= (innerD > delta + 1) ? delta : PriWborderWidth;

	XFillRectangle(XtDisplay(w), PriWpixmapForSet, PriWbackgroundGC,
	    0, 0, PriWwidth, PriWheight);
	DrawCircle(w, PriWpixmapForSet, outterD, 0, 0, PriWborderWidth, 
	    PriWborderGC, PriWinsideGC);
	DrawCircle(w, PriWpixmapForSet, innerD, 0, 0, InnerLineWidth, 
	    PriWinnerLineGC, (isSensitive(w) ? PriWnormalGC : PriWnormalGrayGC));

	XFillRectangle(XtDisplay(w), PriWpixmapForUnset, PriWbackgroundGC,
	    0, 0, PriWwidth, PriWheight);
	DrawCircle(w, PriWpixmapForUnset, outterD, 0, 0, PriWborderWidth,
	    PriWborderGC, PriWinsideGC);
	DrawCircle(w, PriWpixmapForUnset, innerD, 0, 0, InnerLineWidth, 
	    PriWinnerLineGC, PriWinnerLineGC);
	DrawCircle(w, PriWpixmapForUnset, innerD, -ThicknessOfButton, 
	    -ThicknessOfButton, InnerLineWidth,  PriWinnerLineGC, PriWinsideGC);
}

static void DefaultPixmaps(w)
	Widget w;
{
	StRadioSwitchWidget bw = (StRadioSwitchWidget) w;

	PriWpixmapForSet = XCreatePixmap(XtDisplay(w), XtWindow(w), PriWwidth,
	    PriWheight, PriWdepth);
	PriWpixmapForUnset = XCreatePixmap(XtDisplay(w), XtWindow(w), PriWwidth,
	    PriWheight, PriWdepth);
	RedrawWidget(w);
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
	StRadioSwitchWidget bw = (StRadioSwitchWidget) new;

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
	StRadioSwitchWidget cw = (StRadioSwitchWidget) current;
	StRadioSwitchWidget bw = (StRadioSwitchWidget) new;
	Boolean redisplay = FALSE;

	if(cw->stPrimitive.foreground != PriWforeground
	|| cw->core.background_pixel != PriWbackground
	|| cw->core.border_pixel != PriWborderColor
	|| cw->core.border_pixmap != PriWborderPixmap) {
	    ReleaseMyGC(bw);
	    InitializeMyGC(bw);
	    RedrawWidget(bw);

	    redisplay = TRUE;
	}

    return redisplay;
}

static Boolean ChangeSensitive(w)
	Widget w;
{
	(*SuperClass->stPrimitive_class.change_sensitive) (w);
	ReleaseMyGC(w);
	InitializeMyGC(w);
	RedrawWidget(w);

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
	String *params;             /* unused */
	Cardinal *num_params;       /* unused */
{
	StRadioSwitchWidget bw = (StRadioSwitchWidget) w;

	PriWset = TRUE;
	PriWcontents = (Boolean)!PriWcontents;
	(*SuperClass->core_class.expose) (w, event, NULL);
}

/* ARGSUSED */
static void Unset(w,event,params,num_params)
	Widget w;
	XEvent *event;
	String *params;             /* unused */
	Cardinal *num_params;       /* unused */
{
	StRadioSwitchWidget bw = (StRadioSwitchWidget) w;

	PriWset = FALSE;
	PriWcontents = (Boolean)!PriWcontents;
	(*SuperClass->core_class.expose) (w, event, NULL);
}

/* ARGSUSED */
static void Notify(w,event,params,num_params)
	Widget w;
	XEvent *event;
	String *params;             /* unused */
	Cardinal *num_params;       /* unused */
{
	StRadioSwitchWidget bw = (StRadioSwitchWidget)w;

	if(PriWset)
	    XtCallCallbacks(w, StNcallback, (Boolean)PriWcontents);
}
