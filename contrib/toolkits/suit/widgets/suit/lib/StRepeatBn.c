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
 * StRepeatButton.c - SUIT Repeat Button widget
 *
 */

  /* The following are defined for the reader's convenience.  Any
     Xt..Field macro in this code just refers to some field in
     one of the substructures of the WidgetRec.  */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/XawMisc.h>
#include <St/StwP.h>
#include <St/StRepetBnP.h>
#include "StRepetBnI.h"


/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

static void Set();
static void Unset();
static void Notify();


static char defaultTranslations[] =
    "Button1<EnterWindow>:	set() notify()	\n\
     Button1<LeaveWindow>:	unset()		\n\
     <Btn1Down>:		set() notify()	\n\
     <Btn1Up>:			unset()";


static XtActionsRec actionsList[] = {

    {"set",		Set},
    {"notify",		Notify},
    {"unset",		Unset},
};

#define SuperClass ((StSimpleButtonWidgetClass)&stSimpleButtonClassRec)

StRepeatButtonClassRec stRepeatButtonClassRec = {
  {
    (WidgetClass) SuperClass,		/* superclass		  */	
    "StRepeatButton",	      		/* class_name		  */
    sizeof(StRepeatButtonRec),		/* size			  */
    NULL,				/* class_initialize	  */
    NULL,				/* class_part_initialize  */
    FALSE,				/* class_inited		  */
    NULL,				/* initialize		  */
    NULL,				/* initialize_hook	  */
    XtInheritRealize,			/* realize		  */
    actionsList,			/* actions		  */
    XtNumber(actionsList),		/* num_actions		  */
    NULL,				/* resources		  */
    0,	                        	/* resource_count	  */
    NULLQUARK,				/* xrm_class		  */
    FALSE,				/* compress_motion	  */
    TRUE,				/* compress_exposure	  */
    TRUE,				/* compress_enterleave    */
    FALSE,				/* visible_interest	  */
    NULL,				/* destroy		  */
    XtInheritResize,			/* resize		  */
    XtInheritExpose,			/* expose		  */
    NULL,				/* set_values		  */
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
  {
    0,					/* field not used	  */
  },  /* StSimpleButtonClass fields initialization */
  {
    0,					/* field not used	  */
  },  /* StRepeatButtonClass fields initialization */
};

  /* for public consumption */
WidgetClass stRepeatButtonWidgetClass = (WidgetClass)&stRepeatButtonClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static void CallCallbacks(w)
	Widget w;
{
	StRepeatButtonWidget bw = (StRepeatButtonWidget) w;

	XtCallCallbacks(w, StNcallback, NULL);
	if(PriWcallbackInterval)
	    PriWintervalID = XtAddTimeOut(PriWcallbackInterval, CallCallbacks, 
				(caddr_t)w);
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
	StRepeatButtonWidget bw = (StRepeatButtonWidget) w;

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
	StRepeatButtonWidget bw = (StRepeatButtonWidget) w;

	if(PriWboolContents) {
	   PriWset = FALSE;
	   PriWcontents = FALSE;
	   (*SuperClass->core_class.expose) (w, event, NULL);
	}
	if(PriWcallbackInterval)
	    XtRemoveTimeOut(PriWintervalID);
}

/* ARGSUSED */
static void Notify(w,event,params,num_params)
	Widget w;
	XEvent *event;
	String *params;		/* unused */
	Cardinal *num_params;	/* unused */
{
	StRepeatButtonWidget bw = (StRepeatButtonWidget) w;

	CallCallbacks(w);
}
