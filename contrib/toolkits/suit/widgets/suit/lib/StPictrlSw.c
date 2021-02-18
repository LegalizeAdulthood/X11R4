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
 * StPictrialSwitch.c - SUIT Pictrial Switch Widget
 *
 */

  /* The following are defined for the reader's convenience.  Any
     Xt..Field macro in this code just refers to some field in
     one of the substructures of the WidgetRec.  */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/XawMisc.h>
#include <St/StwP.h>
#include <St/StPictrSwP.h>
#include "StPictrSwI.h"

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

#define offset(field) XtOffset(StPictrialSwitchWidget, field)
static XtResource resources[] = {

    {StNtransparence, StCTransparence, XtRBoolean, sizeof(Boolean),
        offset(stPrimitive.transparence), XtRImmediate, (caddr_t)True},

};

static void Initialize();
static void Destroy();
static Boolean SetValues();
static Boolean ChangeSensitive();
static Boolean ParentExpose();
static void Set();
static void Unset();
static void Notify();


static XtActionsRec actionsList[] = {

    {"set",		Set},
    {"notify",		Notify},
    {"unset",		Unset},
};

#define SuperClass ((StSimpleSwitchWidgetClass)&stSimpleSwitchClassRec)

StPictrialSwitchClassRec stPictrialSwitchClassRec = {
  {
    (WidgetClass) SuperClass,		/* superclass		  */	
    "StPictrialSwitch",	      		/* class_name		  */
    sizeof(StPictrialSwitchRec),	/* size			  */
    NULL,	          		/* class_initialize	  */
    NULL,				/* class_part_initialize  */
    FALSE,				/* class_inited		  */
    Initialize,				/* initialize		  */
    NULL,				/* initialize_hook	  */
    XtInheritRealize,			/* realize		  */
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
    XtInheritResize,			/* resize		  */
    NULL,	         		/* expose		  */
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
};

  /* for public consumption */
WidgetClass stPictrialSwitchWidgetClass = 
                             (WidgetClass)&stPictrialSwitchClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

/***** private initialize *****/

static void InitializeMyBorderGC(w)
	Widget w;
{
	StPictrialSwitchWidget bw = (StPictrialSwitchWidget) w;
	XGCValues values;
	Mask valueMask;

	values.foreground = PriWborderColor;
	values.background = PriWbackground;
	values.line_width = PriWborderWidth;
	valueMask = GCForeground | GCBackground | GCLineWidth;
	if(!isSensitive(w)) {
	    values.fill_style = FillTiled;
	    values.tile = PriWinsensitiveBorderPixmap;
	    valueMask |= GCFillStyle | GCTile;
	}
	PriWborderGC = XtGetGC(w, valueMask, &values);
}

static void InitializeMyBackgroundGC(w)
	Widget w;
{
	StPictrialSwitchWidget bw = (StPictrialSwitchWidget) w;
	XGCValues values;
	Mask valueMask;

	values.foreground = PriWparent->core.background_pixel;
	values.background = PriWparent->core.background_pixel;
	valueMask = GCForeground | GCBackground;
	if(PriWparent->core.background_pixmap != UnspecifiedPixmap) {
	    values.fill_style = FillOpaqueStippled;
	    values.stipple = PriWparent->core.background_pixmap;
	    valueMask |= GCFillStyle | GCStipple;
	}
	PriWbackgroundGC = XtGetGC(w, valueMask, &values);
}

static void InitializeMyInsideGC(w)
	Widget w;
{
	StPictrialSwitchWidget bw = (StPictrialSwitchWidget) w;
	XGCValues values;
	Mask valueMask;

	values.foreground = PriWbackground;
	values.background = PriWforeground;
	valueMask = GCForeground | GCBackground;
	if(PriWbackgroundPixmap != UnspecifiedPixmap) {
	    values.fill_style = FillOpaqueStippled;
	    values.stipple = PriWbackgroundPixmap;
	    valueMask |= GCFillStyle | GCStipple;
	}
	PriWinsideGC = XtGetGC(w, valueMask, &values);
}

static void InitializeMyGC(w)
	Widget w;
{
	InitializeMyBorderGC(w);
	InitializeMyBackgroundGC(w);
	InitializeMyInsideGC(w);
}

static void ReleaseMyGC(w)
	Widget w;
{
	StPictrialSwitchWidget bw = (StPictrialSwitchWidget) w;

	XtReleaseGC(w, PriWborderGC);
	XtReleaseGC(w, PriWbackgroundGC);
	XtReleaseGC(w, PriWinsideGC);
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
	StPictrialSwitchWidget cw = (StPictrialSwitchWidget) current;
	StPictrialSwitchWidget bw = (StPictrialSwitchWidget) new;
	Boolean redisplay = FALSE;

	if(PriWforeground != cw->stPrimitive.foreground
	|| PriWbackground != cw->core.background_pixel
	|| PriWbackgroundPixmap != cw->core.background_pixmap
	|| PriWborderWidth != cw->core.border_width
	|| PriWborderColor != cw->core.border_pixel) {
	    XtReleaseGC(bw, PriWborderGC);
	    XtReleaseGC(bw, PriWinsideGC);
	    InitializeMyBorderGC(bw);
	    InitializeMyInsideGC(bw);

	    redisplay = TRUE;
	}

	return redisplay;
}

static Boolean ChangeSensitive(w)
	Widget w;
{
	StPictrialSwitchWidget bw = (StPictrialSwitchWidget) w;

	(*SuperClass->stPrimitive_class.change_sensitive) (w);
	XtReleaseGC(w, PriWborderGC);
	InitializeMyBorderGC(w);

	return FALSE;
}

static Boolean ParentExpose(w)
	Widget w;
{
	StPictrialSwitchWidget bw = (StPictrialSwitchWidget) w;

	(*SuperClass->stPrimitive_class.parent_expose) (w);
	XtReleaseGC(w, PriWbackgroundGC);
	InitializeMyBackgroundGC(w);

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
	StPictrialSwitchWidget bw = (StPictrialSwitchWidget) w;

	PriWset = TRUE;
	PriWcontents = (Boolean)!PriWcontents;
}

/* ARGSUSED */
static void Unset(w,event,params,num_params)
	Widget w;
	XEvent *event;
	String *params;		/* unused */
	Cardinal *num_params;	/* unused */
{
	StPictrialSwitchWidget bw = (StPictrialSwitchWidget) w;

	PriWset = FALSE;
	PriWcontents = (Boolean)!PriWcontents;
}

/* ARGSUSED */
static void Notify(w,event,params,num_params)
	Widget w;
	XEvent *event;
	String *params;		/* unused */
	Cardinal *num_params;	/* unused */
{
	StPictrialSwitchWidget bw = (StPictrialSwitchWidget) w;

	if(PriWset) { 
	    XtCallCallbacks(w, StNcallback, PriWboolContents);
	}
}
