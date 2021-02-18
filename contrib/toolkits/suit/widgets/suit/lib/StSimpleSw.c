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
 * StSimpleSwitch.c - SUIT Simple Switch Widget
 *
 */

  /* The following are defined for the reader's convenience.  Any
     Xt..Field macro in this code just refers to some field in
     one of the substructures of the WidgetRec.  */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/XawMisc.h>
#include <St/StwP.h>
#include <St/StSimplSwP.h>
#include "StSimplSwI.h"

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

static char defaultTranslations[] =
    "Button1<EnterWindow>:	set()		\n\
     Button1<LeaveWindow>:	unset()		\n\
     <Btn1Down>:		set()		\n\
     <Btn1Up>:			notify()";

#define offset(field) XtOffset(StSimpleSwitchWidget, field)
static XtResource resources[] = {

    {StNcontents, StCContents, StRStContents, sizeof(StContents),
	offset(stPrimitive.contents), XtRImmediate, (caddr_t)False},

};
#undef offset

static void Initialize();
static void Realize();
static void Redisplay();
static void Resize();
static Boolean SetValues();
static void Set();
static void Unset();
static void Notify();


static XtActionsRec actionsList[] = {

    {"set",		Set},
    {"notify",		Notify},
    {"unset",		Unset},
};

#define SuperClass ((StLabelWidgetClass)&stLabelClassRec)

StSimpleSwitchClassRec stSimpleSwitchClassRec = {
  {
    (WidgetClass) SuperClass,		/* superclass		  */	
    "StSimpleSwitch",	      		/* class_name		  */
    sizeof(StSimpleSwitchRec),		/* size			  */
    NULL,	          		/* class_initialize	  */
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
    NULL,				/* destroy		  */
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
    XtInheritChangeSensitive,		/* change_sensitive	  */
    XtInheritParentExpose,		/* parent_expose	  */
  },  /* StPrimitiveClass fields initialization */
  {
    0,					/* field not used	  */
  },  /* StLabelClass fields initialization */
  {
    0,					/* field not used	  */
  },  /* StSimpleSwitchClass fields initialization */
};

  /* for public consumption */
WidgetClass stSimpleSwitchWidgetClass = (WidgetClass)&stSimpleSwitchClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

/***** updating ***************/

static void UpdateHighlightBox(w)
	Widget w;
{
	StSimpleSwitchWidget bw = (StSimpleSwitchWidget) w;

	PriWhighlightBox.x = 0;
	PriWhighlightBox.y = 0;
	PriWhighlightBox.width = PriWwidth;
	PriWhighlightBox.height = PriWheight;
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
	StSimpleSwitchWidget bw = (StSimpleSwitchWidget) new;

	PriWset = FALSE;

	PriWhighlightBox.x = 0;
	PriWhighlightBox.y = 0;
	PriWhighlightBox.width = 0;
	PriWhighlightBox.height = 0;
}


/* ARGSUSED */
static void Realize(w, valueMask, attributes)
	register Widget w;
	Mask valueMask;
	XSetWindowAttributes *attributes;
{
	(*SuperClass->core_class.realize) (w, valueMask, attributes);

	UpdateHighlightBox(w);
}

/* ARGSUSED */
static void Redisplay(w, event, region)
	Widget w;
	XEvent *event;	
	Region region;
{
	StSimpleSwitchWidget bw = (StSimpleSwitchWidget) w;

	if(PriWboolContents) {
	    XFillRectangle(XtDisplay(w), XtWindow(w), PriWnormalGC,
		PriWhighlightBox.x, PriWhighlightBox.y,
		PriWhighlightBox.width, PriWhighlightBox.height);
	} else {
	    XFillRectangle(XtDisplay(w), XtWindow(w), PriWinverseGC,
		PriWhighlightBox.x, PriWhighlightBox.y,
		PriWhighlightBox.width, PriWhighlightBox.height);
	}

	PriWlabelGC = PriWboolContents 
		? (isSensitive(w) ? PriWinverseGC : PriWinverseGrayGC)
		: (isSensitive(w) ? PriWnormalGC : PriWnormalGrayGC);

	(*SuperClass->core_class.expose) (w, event, region);
}

static void Resize(w)
	Widget w;
{
	UpdateHighlightBox(w);
}

/* ARGSUSED */
static Boolean SetValues (current, request, new)
    Widget current, request, new;
{
	StSimpleSwitchWidget cw = (StSimpleSwitchWidget) current;
	StSimpleSwitchWidget bw = (StSimpleSwitchWidget) new;
	Boolean redisplay = FALSE;

	if(PriWwidth != cw->core.width
	|| PriWheight != cw->core.height) {
	    UpdateHighlightBox(bw);
	    redisplay = TRUE;
	}

	return redisplay
		|| (PriWboolContents != (Boolean)(cw->stPrimitive.contents));
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
	StSimpleSwitchWidget bw = (StSimpleSwitchWidget)w;

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
	StSimpleSwitchWidget bw = (StSimpleSwitchWidget)w;

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
	StSimpleSwitchWidget bw = (StSimpleSwitchWidget)w;

	if(PriWset) { 
	    XtCallCallbacks(w, StNcallback, PriWboolContents);
	}
}
