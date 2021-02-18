/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        MenuSep.c
 **
 **   Project:     X Widgets
 **
 **   Description: Contains code for menuSep widget class.
 **
 *****************************************************************************
 **   
 **   Copyright (c) 1988 by Hewlett-Packard Company
 **   Copyright (c) 1988 by the Massachusetts Institute of Technology
 **   
 **   Permission to use, copy, modify, and distribute this software 
 **   and its documentation for any purpose and without fee is hereby 
 **   granted, provided that the above copyright notice appear in all 
 **   copies and that both that copyright notice and this permission 
 **   notice appear in supporting documentation, and that the names of 
 **   Hewlett-Packard or  M.I.T.  not be used in advertising or publicity 
 **   pertaining to distribution of the software without specific, written 
 **   prior permission.
 **   
 *****************************************************************************
 *************************************<+>*************************************/


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <X11/StringDefs.h>
#include <X11/keysymdef.h>   
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <Xw/Xw.h>
#include <Xw/XwP.h>
#include <Xw/MenuBtn.h>
#include <Xw/MenuBtnP.h>
#include <Xw/MenuSep.h>
#include <Xw/MenuSepP.h>


/*  Resource list for MenuSep  */

static XtResource resources[] = 
{
   {
      XtNseparatorType, XtCSeparatorType, XtRSeparatorType, sizeof (int),
      XtOffset (XwMenuSepWidget, menuSep.separator_type),
      XtRString, "single_line"
   },
};


/*  Static routine definitions  */

static void            ClassInitialize();
static void      Initialize();
static void    Redisplay();
static void    Destroy();
static Boolean SetValues();

static void IdealWidth();
static void Unhighlight();
static void Highlight();
static void SetCascadeEnabled();
static void ClearCascadeEnabled();
static void EnterParentsWindow();
static void SetTraversalType();

static void GetMenuSepGC();

/*  The MenuSep class record definition  */

XwMenuSepClassRec XwmenuSepClassRec =
{
   {
      (WidgetClass) &XwmenubuttonClassRec, /* superclass	 */
      "MenuSep",                        /* class_name	         */	
      sizeof(XwMenuSepRec),             /* widget_size           */	
      NULL,                             /* class_initialize      */    
      NULL,                             /* class_part_initialize */
      FALSE,                            /* class_inited          */	
      (XtInitProc) Initialize,          /* initialize	         */	
      NULL,                             /* initialize_hook       */
      _XwRealize,                       /* realize	         */	
      NULL,                             /* actions               */	
      0,                                /* num_actions    	 */	
      resources,                        /* resources	         */	
      XtNumber (resources),             /* num_resources         */	
      NULLQUARK,                        /* xrm_class	         */	
      TRUE,                             /* compress_motion       */	
      TRUE,                             /* compress_exposure     */	
      TRUE,                             /* compress_enterleave   */
      FALSE,                            /* visible_interest      */	
      (XtWidgetProc) Destroy,           /* destroy               */	
      (XtWidgetProc) NULL,              /* resize                */	
      (XtExposeProc) Redisplay,         /* expose                */	
      (XtSetValuesFunc) SetValues,      /* set_values	         */	
      NULL,                             /* set_values_hook       */
      XtInheritSetValuesAlmost,         /* set_values_almost     */
      NULL,                             /* get_values_hook       */
      NULL,                             /* accept_focus	         */	
      XtVersion,                        /* version               */
      NULL,                             /* callback private      */
      NULL,                             /* tm_table              */
      NULL,                             /* query_geometry        */
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
   },

   {
      NULL,         /* Primitive border_highlight   */
      NULL,         /* Primitive border_unhighlight */
      NULL,         /* Primitive select_proc        */
      NULL,         /* Primitive release_proc       */
      NULL,         /* Primitive toggle_proc        */
      NULL,         /* Primitive translations       */
   },

   {
      0,	/* button class fields */
   },

   {
      (XtWidgetProc) IdealWidth,
      (XtWidgetProc) Unhighlight,
      (XtWidgetProc) Highlight,
      (XtWidgetProc) SetCascadeEnabled,
      (XtWidgetProc) ClearCascadeEnabled,
      (XtWidgetProc) EnterParentsWindow,
      (XtWidgetProc) NULL,
      (XtWidgetProc) NULL,
      (XtWidgetProc) SetTraversalType,
   }
};

WidgetClass XwmenuSepWidgetClass = (WidgetClass) &XwmenuSepClassRec;





/************************************************************************
 *
 *  Initialize
 *     The main widget instance initialization routine.
 *
 ************************************************************************/

static void Initialize (request, new)
XwMenuSepWidget request, new;

{
   new -> primitive.traversal_type = XwHIGHLIGHT_OFF;


   new -> primitive.highlight_thickness = 0;      

   if (request -> core.width == 0)
      new -> core.width = 2 * new -> primitive.highlight_thickness + 2;
   if (request -> core.height == 0)
   {
      new -> core.height = 2 * new -> primitive.highlight_thickness;

      if (new -> menuSep.separator_type == XwSINGLE_LINE ||
          new -> menuSep.separator_type == XwSINGLE_DASHED_LINE)
         new -> core.height += 3;
      else if (new -> menuSep.separator_type == XwDOUBLE_LINE ||
               new -> menuSep.separator_type == XwDOUBLE_DASHED_LINE)
         new -> core.height += 5;
      else
         if (new -> core.height == 0)
            new -> core.height = 1;
   }

   if (new -> menuSep.separator_type != XwNO_LINE            &&
       new -> menuSep.separator_type != XwSINGLE_LINE        &&
       new -> menuSep.separator_type != XwDOUBLE_LINE        &&
       new -> menuSep.separator_type != XwSINGLE_DASHED_LINE &&
       new -> menuSep.separator_type != XwDOUBLE_DASHED_LINE)
   {
      XtWarning ("MenuSep: Invalid separator type.");
      new -> menuSep.separator_type = XwSINGLE_LINE;
   }


   /*  Get the drawing graphics contexts.  */

   GetMenuSepGC (new);
}




/************************************************************************
 *
 *  GetMenuSepGC
 *     Get the graphics context used for drawing the menu separator.
 *
 ************************************************************************/

static void GetMenuSepGC (mw)
XwMenuSepWidget mw;

{
   XGCValues values;
   XtGCMask  valueMask;

   valueMask = GCForeground | GCBackground;

   values.foreground = mw -> primitive.foreground;
   values.background = mw -> core.background_pixel;

   if (mw -> menuSep.separator_type == XwSINGLE_DASHED_LINE ||
       mw -> menuSep.separator_type == XwDOUBLE_DASHED_LINE)
   {
      valueMask = valueMask | GCLineStyle;
      values.line_style = LineDoubleDash;
   }

   mw -> menuSep.separator_GC = XtGetGC ((Widget) mw, valueMask, &values);
}




/************************************************************************
 *
 *  Redisplay
 *     Invoke the application exposure callbacks.
 *
 ************************************************************************/

static void Redisplay (mw, event, region)
XwMenuSepWidget mw;
XEvent * event;
Region region;

{
   int lineCount;
   XSegment pts[2];
   int center = mw -> core.height / 2;

   switch (mw -> menuSep.separator_type)
   {

      case XwNO_LINE:
       break;

      case XwSINGLE_LINE:
      case XwSINGLE_DASHED_LINE:
         lineCount = 1;
         pts[0].y1 = pts[0].y2 = center;
         pts[0].x1 = mw -> primitive.highlight_thickness + 1;
         pts[0].x2 = mw -> core.width - 
                     mw -> primitive.highlight_thickness - 2;
      break;

      case XwDOUBLE_LINE:
      case XwDOUBLE_DASHED_LINE:
         lineCount = 2;
         pts[0].y1 = pts[0].y2 = center - 1;
         pts[1].y1 = pts[1].y2 = center + 1;
         pts[0].x1 = pts[1].x1 = mw -> primitive.highlight_thickness + 1;
         pts[0].x2 = pts[1].x2 = mw -> core.width -
                                 mw -> primitive.highlight_thickness - 2;
      break;
   }

 
   if (mw -> menuSep.separator_type != XwNO_LINE)
      XDrawSegments (XtDisplay ((Widget) mw), XtWindow ((Widget) mw),
                     mw -> menuSep.separator_GC, pts, lineCount);

}




/************************************************************************
 *
 *  Destroy
 *	Remove the callback lists.
 *
 ************************************************************************/

static void Destroy (mw)
XwMenuSepWidget mw;

{
   XtDestroyGC (mw -> menuSep.separator_GC);
}




/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/

static Boolean SetValues (current, request, new)
XwMenuSepWidget current, request, new;

{
   /*
    * We never allow our traversal flags to be changed during SetValues();
    * this is enforced by our superclass.
    */


   new -> primitive.highlight_thickness = 0;      

   /*  Verify that intervening classes have not changed my size  */

   if (request -> core.width != new -> core.width)
      new -> core.width = request -> core.width;

   if (request -> core.height != new -> core.height && 
       request -> core.height != 0)
      new -> core.height = request -> core.height + 
                           2 * new -> primitive.highlight_thickness;

   if (request -> core.height == 0 || new -> primitive.recompute_size)
   {
      new -> core.height = 2 * new -> primitive.highlight_thickness;

      if (new -> menuSep.separator_type == XwSINGLE_LINE ||
          new -> menuSep.separator_type == XwSINGLE_DASHED_LINE)
         new -> core.height += 3;
      else if (new -> menuSep.separator_type == XwDOUBLE_LINE ||
               new -> menuSep.separator_type == XwDOUBLE_DASHED_LINE)
         new -> core.height += 5;
      else
         if (new -> core.height == 0)
            new -> core.height = 1;

   }


   new -> primitive.traversal_type = XwHIGHLIGHT_OFF;

   if (new -> menuSep.separator_type != XwNO_LINE             &&
       new -> menuSep.separator_type != XwSINGLE_LINE         &&
       new -> menuSep.separator_type != XwDOUBLE_LINE         &&
       new -> menuSep.separator_type != XwSINGLE_DASHED_LINE  &&
       new -> menuSep.separator_type != XwDOUBLE_DASHED_LINE)
   {
      XtWarning ("MenuSep: Invalid separator type.");
      new -> menuSep.separator_type = current -> menuSep.separator_type;
   }

   if (new -> menuSep.separator_type != current -> menuSep.separator_type  ||
       new -> core.background_pixel != current -> core.background_pixel    ||
       new -> primitive.foreground != current -> primitive.foreground)
   {
      XtDestroyGC (new -> menuSep.separator_GC);
      GetMenuSepGC (new);
      return (True);
   }

   return (False);
}




/************************************************************************
 *
 *  IdealWidth
 *      Return the ideal width of a menu separator.
 *
 ************************************************************************/

static void IdealWidth (w, width)
Widget w;
Dimension * width;

{
   *width = 1;
}




/************************************************************************
 *
 *  Dummy routines -- can be removed later.
 *
 ************************************************************************/

static void Highlight (w)
Widget w;
{
}


static void Unhighlight (w)
Widget w;
{
}

static void SetCascadeEnabled (w)
Widget w;

{
}

static void ClearCascadeEnabled (w)
Widget w;

{
}

static void EnterParentsWindow (w)
Widget w;

{
}


static void SetTraversalType (w, highlight_mode)

   Widget w;
   int highlight_mode;

{
}



