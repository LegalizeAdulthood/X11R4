#ifndef lint
static char rcsid[] = "$Header: /ai/nil/var/scr/moraes/xtroff/XtStuff/RCS/MenuItem.c,v 1.1 89/11/15 21:37:34 moraes Exp $";
#endif lint

/*
 * Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.
 * 
 *                         All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior permission.  
 * 
 * 
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/*
 * MenuItem.c - MenuItem button widget
 *
 * Author:      Robert P. Shen
 *              Malcolm Forbes
 *              Digital Equipment Corporation
 *              ESG Systems Engineering
 *
 * Date:        February 1, 1988
 */

#define XtStrlen(s)	((s) ? strlen(s) : 0)

  /* The following are defined for the reader's convenience.  Any
     Xt..Field macro in this code just refers to some field in
     one of the substructures of the WidgetRec.  */

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "x11.h"
#ifdef R4
# include <X11/Xaw/LabelP.h>
#else
# include <X11/LabelP.h>
#endif
#include "MenuItem.h"
#include "MenuItemP.h"
#include "MenuItemI.h"

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

static char defaultTranslations[] = 
    "<BtnDown>:		set() \n\
     <BtnUp>:	        notify() unhighlight() unset() \n\
     <EnterWindow>:	highlight() \n\
     <LeaveWindow>:	unhighlight()";

static XtResource resources[] = { 
   {XtNcallback, XtCCallback, XtRCallback, sizeof(caddr_t), 
      XtOffset(MenuItemWidget, menuitem.callbacks), XtRCallback,
      (caddr_t)NULL},
   {XtNhighlightThickness, XtCThickness, XtRInt, sizeof(Dimension),
      XtOffset(MenuItemWidget,menuitem.highlight_thickness), XtRString,"2"},
 };  

static XtActionsRec actionsList[] =
{
  {"set",		Set},
  {"notify",		Notify},
  {"highlight",		Highlight},
  {"unset",		Unset},
  {"unhighlight",	Unhighlight},
};

  /* ...ClassData must be initialized at compile time.  Must
     initialize all substructures.  (Actually, last two here
     need not be initialized since not used.)
  */
MenuItemClassRec menuitemClassRec = {
  {
    (WidgetClass) &labelClassRec,          /* superclass	*/    
    "MenuItem",                             /* class_name	*/
    sizeof(MenuItemRec),                    /* size		*/
    NULL,                                  /* class initialize  */
    NULL,				   /* class_part_init    */
    FALSE,                                 /* class_inited      */
    Initialize,                            /* initialize	*/
    NULL,				   /* initialize_hook   */	
    XtInheritRealize,                      /* realize		*/
    actionsList,                           /* actions		*/
    XtNumber(actionsList),                 /* num_actions	*/
    resources,                             /* resources	        */
    XtNumber(resources),                   /* resource_count	*/
    NULLQUARK,                             /* xrm_class	        */
    FALSE,                                 /* compress_motion	*/
    FALSE,                                 /* compress_exposure	*/
    FALSE,				   /* compress_enterleave*/
    FALSE,                                 /* visible_interest	*/
    Destroy,                               /* destroy		*/
    XtInheritResize,                       /* resize		*/
    Redisplay,                             /* expose		*/
    SetValues,                             /* set_values	*/
    NULL,				   /* set_values_hook   */
    NULL,				   /* set_values_almost */
    NULL,				   /* get_values_hook   */
    NULL,                                  /* accept_focus	*/
    XtVersion,				   /* version           */
    NULL,                                  /* callback_private	*/
    defaultTranslations,		   /* tm_table          */
    NULL,                                  /* query_geometry	*/
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },  /* CoreClass fields initialization */
  {
    0,                                     /* field not used    */
  },  /* LabelClass fields initialization */
  {
    0,                                     /* field not used    */
  },  /* MenuItemClass fields initialization */
};

  /* for public consumption */
WidgetClass menuitemWidgetClass = (WidgetClass) &menuitemClassRec;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static void Get_inverseGC(cbw)
    MenuItemWidget cbw;
{
    XGCValues	values;

    /* Set up a GC for inverse (set) state */

    values.foreground   = ComWforeground;
    values.font		= ComWfont->fid;
    values.fill_style   = FillSolid;

    ComWinverseGC = XtGetGC((Widget)cbw,
    	(unsigned) GCForeground | GCFont | GCFillStyle, &values);
}

static void Get_inverseTextGC(cbw)
    MenuItemWidget cbw;
{
    XGCValues	values;

    /* Set up a GC for inverse (set) state */

    values.foreground   = ComWbackground; 	/* default is White */
    values.font		= ComWfont->fid;
    values.fill_style   = FillSolid;

    ComWinverseTextGC = XtGetGC((Widget)cbw,
    	(unsigned) GCForeground | GCFont | GCFillStyle, &values);
}

static void Get_highlightGC(cbw)
    MenuItemWidget cbw;
{
    XGCValues	values;
    
    /* Set up a GC for highlighted state.  It has a thicker
       line width for the highlight border */

    values.foreground   = ComWforeground;
    values.line_width   = ComWhighlightThickness;

    ComWhighlightGC = XtGetGC((Widget)cbw,
    	(unsigned) GCForeground | GCLineWidth, &values);
}

static void Get_eraseHighGC(cbw)
    MenuItemWidget cbw;
{
    XGCValues	values;
    
    /* Set up a GC for erasing a highlighted state.  It has the same thickness
       line width as for the highlight border, only the color is opposite */

    values.foreground   = ComWbackground;
    values.line_width   = ComWhighlightThickness;

    ComWeraseHighGC = XtGetGC((Widget)cbw,
    	(unsigned) GCForeground | GCLineWidth, &values);
}

/* ARGSUSED */
static void Initialize(request, new)
 Widget request, new;
{
    MenuItemWidget cbw = (MenuItemWidget) new;
    
    Get_inverseGC(cbw);
    Get_inverseTextGC(cbw);
    Get_highlightGC(cbw);
    Get_eraseHighGC(cbw);

      /* init flags for state */

    ComWset = FALSE;
    ComWhighlighted = FALSE;  
    ComWdisplayHighlighted = FALSE;
    ComWdisplaySet = FALSE;
    ComWjustify = XtJustifyLeft;
} 

/***************************
*
*  EVENT HANDLERS
*
***************************/

/* ARGSUSED */
static void Set (w,event)
     Widget w;
     XEvent *event;
{
  MenuItemWidget cbw = (MenuItemWidget)w;
  ComWset = TRUE;
  Redisplay(w, event);
}

/* ARGSUSED */
static void Unset (w,event)
     Widget w;
     XEvent *event;
{
  MenuItemWidget cbw = (MenuItemWidget)w;
  ComWset = FALSE;
  Redisplay(w, event);
}

/* ARGSUSED */
static void Highlight (w,event)
     Widget w;
     XEvent *event;
{
  MenuItemWidget cbw = (MenuItemWidget)w;
  ComWhighlighted = TRUE;
  Redisplay(w, event);
}

/* ARGSUSED */
static void Unhighlight(w,event)
     Widget w;
     XEvent *event;
{
  MenuItemWidget cbw = (MenuItemWidget)w;
  ComWhighlighted = FALSE;
  Redisplay(w, event);
}

/* ARGSUSED */
static void Notify(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  XtCallCallbacks(w, XtNcallback, NULL);
}
/*
 * Repaint the widget window
 */

/************************
*
*  REDISPLAY (DRAW)
*
************************/

/* ARGSUSED */

static void Redisplay(w, event)
	Widget w;
	XEvent *event;
    {
	MenuItemWidget cbw = (MenuItemWidget) w;
	XSetWindowAttributes window_attributes;
	Dimension  thick;

	/*-----------------------------------------------------------------*
	 *  Here's the NEW scoop:  If the button is normal, draw the text. *
	 *  If it's highlighted, then invert the interior area and draw    *
	 *  the text inverted too.  If it's set, then also draw a border   *
	 *  rectangle around the interior area.				   *
	 *-----------------------------------------------------------------*/

	/*-----------------------------------------------------------------*
	 *  Note that Redisplay must remember the state of its last	   *
	 *  state ????							   *
	 *  redrawing to avoid flicker.  If the state is the same,	   *
	 *  the window just needs to redraw (even on an expose).	   *
	 *-----------------------------------------------------------------*/

/*
	if ((!ComWhighlighted && ComWdisplayHighlighted) ||
						(!ComWset && ComWdisplaySet))
 */
	/*-----------------------------------------------------------------*
	 *  To avoid flicker, check first if the selection is still	   *
	 *  highlighted; if not, go ahead and clear the window, otherwise  *
	 *  clear only the border is it was set and now isn't, otherwise   *
	 *  if it highlighted or set, invert the interior box.  Then,	   *
	 *  add the border if it is set now.				   *
	 *-----------------------------------------------------------------*/

	thick = ComWhighlightThickness;

	if (!ComWhighlighted && ComWdisplayHighlighted)
	    XClearWindow(XtDisplay(w),XtWindow(w));
	else if (!ComWset && ComWdisplaySet)
	    XDrawRectangle (XtDisplay(w),XtWindow(w), ComWeraseHighGC,
				thick >> 1, thick >> 1,
				ComWwidth-thick, ComWheight-thick);
	if (ComWhighlighted)
	    {
		XFillRectangle (XtDisplay(w),XtWindow(w), ComWinverseGC,
			thick, thick, ComWwidth-2*thick, ComWheight-2*thick);
		if (ComWset)
		    XDrawRectangle (XtDisplay(w),XtWindow(w), ComWhighlightGC,
				thick >> 1, thick >> 1,
				ComWwidth-thick, ComWheight-thick);
	    }

	/*-----------------------------------------------------------------*
	 *    check whether border is taken out of size of rectangle or	   *
	 *    is outside of rectangle					   *
	 *-----------------------------------------------------------------*/


	/*-----------------------------------------------------------------*
	 *    draw the string:  there are three different "styles" for it, *
	 *    all in separate GCs					   *
	 *-----------------------------------------------------------------*/

	XDrawString (XtDisplay(w),XtWindow(w),
			(ComWhighlighted ?  ComWinverseTextGC :
			(ComWsensitive ? ComWnormalGC : ComWgrayGC)),
			ComWlabelX, ComWlabelY + ComWfont->max_bounds.ascent,
/*			ComWlabelX + LeftMarginPadding, ComWlabelY,	*/
			ComWlabel, (int) ComWlabelLen);

	ComWdisplayHighlighted	= ComWhighlighted;
	ComWdisplaySet		= ComWset;
    }


/* ARGSUSED */
static void Destroy(w)
    Widget w;
{
  /* must free GCs and pixmaps */
}


/*
 * Set specified arguments into widget
 */
/* ARGSUSED */
static Boolean SetValues (current, request, new, last)
    Widget current, request, new;
    Boolean last;
{
    MenuItemWidget cbw = (MenuItemWidget) current;
    MenuItemWidget newcbw = (MenuItemWidget) new;

     if (XtLField(newcbw,foreground) != ComWforeground)
       {
         XtDestroyGC(ComWinverseGC);
	 Get_inverseGC(newcbw);
         XtDestroyGC(ComWhighlightGC);
	 Get_highlightGC(newcbw);
         XtDestroyGC(ComWeraseHighGC);
	 Get_eraseHighGC(newcbw);
       }
    else 
      {
	if (XtCField(newcbw,background_pixel) != ComWbackground ||
	     XtLField(newcbw,font) != ComWfont) {
	     XtDestroyGC(ComWinverseTextGC);
	     Get_inverseTextGC(newcbw);
	     }
	if (XtCBField(newcbw,highlight_thickness) != ComWhighlightThickness) {
	    XtDestroyGC(ComWhighlightGC);
	    Get_highlightGC(newcbw);
	    XtDestroyGC(ComWeraseHighGC);
	    Get_eraseHighGC(newcbw);
	}
      }
     
    /*  NEED TO RESET PROC AND CLOSURE */

     /* ACTIONS */
    /* Change Label to remove ClearWindow and Redisplay */
    /* Change Label to change GCs if foreground, etc */

    return (XtCField(newcbw, sensitive) != ComWsensitive ||
	    XtCBField(newcbw, set) != ComWset ||
	    XtCBField(newcbw, highlighted) != ComWhighlighted);
}
