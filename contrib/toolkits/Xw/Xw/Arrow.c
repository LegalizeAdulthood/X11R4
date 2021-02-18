/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        Arrow.c
 **
 **   Project:     X Widgets
 **
 **   Description: Contains code for arrow widget class: XwarrowWidgetClass
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
#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <Xw/Xw.h>
#include <Xw/XwP.h>
#include <Xw/ArrowP.h>
#include <Xw/Arrow.h>
#include <X11/StringDefs.h>
#include <X11/keysymdef.h>   


/*  Event procedures referenced in the actions list  */

static XtActionProc Select();
static XtActionProc Release();


/*  Default translation table and action list  */

static char defaultTranslations[] =
    "<Btn1Down>:        select()\n\
     <Btn1Up>:          release()\n\
     <KeyDown>Select:   select()\n\
     <KeyUp>Select:     release()\n\
     <EnterWindow>:     enter()\n\
     <LeaveWindow>:     leave()";

static XtActionsRec actionsList[] =
{
  { "select",   (XtActionProc) Select               },
  { "release",  (XtActionProc) Release              },
  { "enter",    (XtActionProc) _XwPrimitiveEnter    },
  { "leave",    (XtActionProc) _XwPrimitiveLeave    },
};


/*  Resource list for Arrow  */

static XtResource resources[] = 
{
   {
      XtNarrowDirection, XtCArrowDirection, XtRArrowDirection, sizeof (int),
      XtOffset(XwArrowWidget, arrow.direction), XtRString, "arrow_up"
   }
};


/*  Static routine definitions  */

static void      Initialize();
static void    Redisplay();
static void    Destroy();
static Boolean SetValues();
static void GetArrowGC();


/*  The Arrow class record definition  */

XwArrowClassRec XwarrowClassRec =
{
   {
      (WidgetClass) &XwprimitiveClassRec, /* superclass	         */	
      "Arrow",                            /* class_name	         */	
      sizeof(XwArrowRec),               /* widget_size	         */	
      NULL,                             /* class_initialize      */    
      NULL,                             /* class_part_initialize */
      FALSE,                            /* class_inited          */	
      (XtInitProc) Initialize,          /* initialize	         */	
      NULL,                             /* initialize_hook       */
      _XwRealize,                       /* realize	         */	
      actionsList,                      /* actions               */	
      XtNumber(actionsList),            /* num_actions    	 */	
      resources,                        /* resources	         */	
      XtNumber(resources),              /* num_resources         */	
      NULLQUARK,                        /* xrm_class	         */	
      TRUE,                             /* compress_motion       */	
      TRUE,                             /* compress_exposure     */	
      TRUE,                             /* compress_enterleave   */
      FALSE,                            /* visible_interest      */	
      (XtWidgetProc) Destroy,           /* destroy               */	
      NULL,                             /* resize                */
      (XtExposeProc) Redisplay,         /* expose                */	
      (XtSetValuesFunc) SetValues,      /* set_values	         */	
      NULL,                             /* set_values_hook       */
      XtInheritSetValuesAlmost,         /* set_values_almost     */
      NULL,                             /* get_values_hook       */
      NULL,                             /* accept_focus	         */	
      XtVersion,                        /* version               */
      NULL,                             /* callback private      */
      defaultTranslations,              /* tm_table              */
      NULL,                             /* query_geometry        */
      XtInheritDisplayAccelerator, NULL,
   },

   {
      NULL,         /* Primitive border_highlight   */
      NULL,         /* Primitive border_unhighlight */
      NULL,         /* Primitive select_proc        */
      NULL,         /* Primitive release_proc       */
      NULL,         /* Primitive toggle_proc        */
   }
};

WidgetClass XwarrowWidgetClass = (WidgetClass) &XwarrowClassRec;




/************************************************************************
 *
 *  Initialize
 *     The main widget instance initialization routine.
 *
 ************************************************************************/

static void Initialize (request, new)
XwArrowWidget request, new;

{

   /*
    *  Check the data put into the new widget from .Xdefaults
    *  or through the arg list.
    */

   if (new -> arrow.direction != XwARROW_UP   &&
       new -> arrow.direction != XwARROW_DOWN &&
       new -> arrow.direction != XwARROW_LEFT &&
       new -> arrow.direction != XwARROW_RIGHT)
   {
      XtWarning ("Arrow: The arrow direction is incorrect.");
      new -> arrow.direction = XwARROW_UP;
   }


   /*  Set up a geometry for the widget if it is currently 0.  */

   if (request -> core.width == 0) new -> core.width += 15;
   if (request -> core.height == 0) new -> core.height += 15;



   new->arrow.selected = False;


   /*  Get the drawing graphics contexts.  */

   GetArrowGC (new);
}




/************************************************************************
 *
 *  GetArrowGC
 *     Get the graphics context used for drawing the arrow.
 *
 ************************************************************************/

static void GetArrowGC (aw)
XwArrowWidget aw;

{
   XGCValues values;
   XtGCMask  valueMask;

   valueMask = GCForeground | GCBackground;

   values.foreground = aw -> primitive.foreground;
   values.background = aw -> core.background_pixel;

   aw -> arrow.arrow_GC = XtGetGC ((Widget) aw, valueMask, &values);
}








/************************************************************************
 *
 *  Redisplay
 *     General redisplay function called on exposure events.
 *
 ************************************************************************/

static void Redisplay (aw, event, region)
XwArrowWidget aw;
XEvent * event;
Region region;

{
   int width;
   int height;
   int border;

   Boolean evenFlag;

   int    longLines;
   static XSegment pt[500];
   XSegment toePts[2];
   int    lineCount;

   int minDim;
   register int i;
   short x1, x2, yTop, yBottom;
   short temp1, temp2;
   

   /*  Get the width and height with respect to the direction */
   
   border = aw -> primitive.highlight_thickness + 1;

   if (aw -> arrow.direction == XwARROW_UP ||
       aw -> arrow.direction == XwARROW_DOWN)
   {
      width = aw -> core.width - 2 * border;
      height = aw -> core.height - 2 * border;
     
   }
   else
   {
      height = aw -> core.width - 2 * border;
      width = aw -> core.height - 2 * border;
   }

   /*  Set a flag indicating whether the arrow has an even  */
   /*  number of lines or odd number of lines.              */

   if (width % 2) evenFlag = False;
   else evenFlag = True;

   /* Calculate the number of lines that extend to the end of the arrow */

   if (width / 3 > height / 3)  longLines = height / 3;
   else longLines = width / 3;


   if (longLines % 2 && evenFlag)
   {
      longLines--;
      if (longLines < 2) longLines = 2;
   }
	 
   if (!(longLines % 2) && !evenFlag)
   {
      longLines--;
      if (longLines < 1) longLines = 1;
   }
   /*  Fill a point array with the centering long line on odd sizes  */

   if (width < height) minDim = width;
   else minDim = height;

   lineCount = 0;
   if (!evenFlag)
   {
      pt[0].x1 = border;
      pt[0].y1 = pt[0].y2 = width / 2 + border;
      pt[0].x2 = border + height - 1;
      lineCount++;
      longLines--;
   }


   /*  Set up a loop to generate the segments.  */

   x1 = border;
   x2 = border + height - 1;
   yTop = width / 2 + border - 1;
   toePts[0].x2 = toePts[1].x2 = x2;


   if (!evenFlag)
   {
      yBottom = yTop + 2;
      x1++;
   }
   else yBottom = yTop + 1;

   while (yTop >= border - 1 && x1 <= x2)
   {
      if (lineCount >= 499) break;

      if (lineCount == longLines || lineCount == longLines + 1)
      {
         if (width < height) x2 = border + width / 2 - 1;
         else x2 = border + height / 2 - 1;
         if (!evenFlag) x2++;
         toePts[0].x1 = toePts[1].x1 = x2;
         toePts[0].y1 = toePts[0].y2 = yTop + 1;
         toePts[1].y1 = toePts[1].y2 = yBottom - 1;
      }

      pt[lineCount].x1 = pt[lineCount + 1].x1 = x1;
      pt[lineCount].x2 = pt[lineCount + 1].x2 = x2;
      pt[lineCount].y1 = pt[lineCount].y2 = yTop;
      pt[lineCount + 1].y1 = pt[lineCount + 1].y2 = yBottom;

      x1++;
      yTop--;
      yBottom++;
      lineCount += 2;
   }


   /*  Transform the "left" pointing arrow to the correct direction  */

   switch (aw -> arrow.direction)
   {

      case XwARROW_LEFT:
      break;

      case XwARROW_RIGHT:
      {
         for (i = 0; i < lineCount; i++)
         {
            pt[i].x1 = aw -> core.width - pt[i].x1 - 1;
            pt[i].x2 = aw -> core.width - pt[i].x2 - 1;
         }
         for (i = 0; i < 2; i++)
         {
            toePts[i].x1 = aw -> core.width - toePts[i].x1 - 1;
            toePts[i].x2 = aw -> core.width - toePts[i].x2 - 1;
         }
      }
      break;

      case XwARROW_UP:
      {
         for (i = 0; i < lineCount; i++)
         {
            temp1 = pt[i].x1;
            pt[i].y2 = pt[i].x2;
            pt[i].x1 = pt[i].x2 = aw -> core.width - pt[i].y1 - 1;
            pt[i].y1 = temp1;
         }
         for (i = 0; i < 2; i++)
         {
            temp1 = toePts[i].x1;
            toePts[i].y2 = toePts[i].x2;
            toePts[i].x1 = toePts[i].x2 = aw -> core.width - toePts[i].y1 - 1;
            toePts[i].y1 = temp1;
         }
      }
      break;

      case XwARROW_DOWN:
      {
         for (i = 0; i < lineCount; i++)
         {
            temp1 = pt[i].x1;
            temp2 = pt[i].x2;
            pt[i].x1 = pt[i].x2 = pt[i].y1;
            pt[i].y1 = aw -> core.height - temp1 - 1;
            pt[i].y2 = aw -> core.height - temp2 - 1;
         }
         for (i = 0; i < 2; i++)
         {
            temp1 = toePts[i].x1;
            temp2 = toePts[i].x2;
            toePts[i].x1 = toePts[i].x2 = toePts[i].y1;
            toePts[i].y1 = aw -> core.height - temp1 - 1;
            toePts[i].y2 = aw -> core.height - temp2 - 1;
         }
      }
      break;
   }


   /*  Draw the arrow  */

   temp1 = aw -> primitive.highlight_thickness;

   if (aw -> core.sensitive && aw -> arrow.selected)
   {
      XDrawSegments (XtDisplay ((Widget) aw), XtWindow ((Widget) aw),
   		     aw -> arrow.arrow_GC, pt, lineCount);
   }
   else
   {
      XClearArea(XtDisplay ((Widget) aw), XtWindow ((Widget) aw),
                 temp1, temp1, 
                 aw -> core.width - 2 * temp1,
                 aw -> core.height - 2 * temp1, FALSE);
      XDrawPoints (XtDisplay ((Widget) aw), XtWindow ((Widget) aw),
   		   aw -> arrow.arrow_GC, pt, lineCount * 2, CoordModeOrigin);

      XDrawSegments (XtDisplay ((Widget) aw), XtWindow ((Widget) aw),
   		     aw -> arrow.arrow_GC, toePts, 2);
   }
}







/************************************************************************
 *
 *  Destroy
 *	Clean up allocated resources when the widget is destroyed.
 *
 ************************************************************************/

static void Destroy (aw)
XwArrowWidget aw;

{
   XtDestroyGC (aw -> arrow.arrow_GC);
}




/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/

static Boolean SetValues (current, request, new)
XwArrowWidget current, request, new;

{
   Boolean returnFlag = FALSE;
    

    /*  Check the data put into the new widget.  */

   if (new -> arrow.direction != XwARROW_UP   &&
       new -> arrow.direction != XwARROW_DOWN &&
       new -> arrow.direction != XwARROW_LEFT &&
       new -> arrow.direction != XwARROW_RIGHT)
   {
      XtWarning ("Arrow: The arrow direction is incorrect.");
      new -> arrow.direction = current -> arrow.direction;
   }


   /*  See if the GC's need to be regenerated and widget redrawn.  */

   if (new -> arrow.direction != current-> arrow.direction               ||
       new -> primitive.highlight_thickness != 
       current -> primitive.highlight_thickness                          ||
       new -> primitive.foreground != current -> primitive.foreground    ||
       new -> core.background_pixel != current -> core.background_pixel  ||
       new -> primitive.foreground != current -> primitive.foreground)
   {
      returnFlag = TRUE;
      XtDestroyGC (new -> arrow.arrow_GC);
      GetArrowGC (new);
   }

   return (returnFlag);
}




/************************************************************************
 *
 *  Select
 *     This function processes selections occuring on the arrow.
 *
 ************************************************************************/

static XtActionProc Select (aw, buttonEvent)
XwArrowWidget aw;
XButtonPressedEvent * buttonEvent;

{
   aw -> arrow.selected = True;

   Redisplay (aw, NULL);

   XtCallCallbacks (aw, XtNselect, NULL);
}




/************************************************************************
 *
 *  Release
 *     This function processes releases occuring on the arrow.
 *
 ************************************************************************/

static XtActionProc Release (aw, event)
XwArrowWidget   aw;
XEvent * event;

{
   aw -> arrow.selected = False;

   Redisplay (aw, NULL);

   XtCallCallbacks (aw, XtNrelease, NULL);
}
