/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        ScrollBar.c
 **
 **   Project:     X Widgets
 **
 **   Description: Contains code for primitive widget class: ScrollBar
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
#include <Xw/Valuator.h>
#include <Xw/ValuatorP.h>
#include <Xw/Arrow.h>
#include <Xw/ArrowP.h>
#include <Xw/ScrollBar.h>
#include <Xw/ScrollBarP.h>
#include <X11/StringDefs.h>
#include <X11/keysymdef.h>   


/*  Resource list for ScrollBar  */

static XtResource resources[] = 
{
   {
     XtNhighlightColor, XtCForeground, XtRPixel, sizeof (Pixel),
     XtOffset (XwScrollBarWidget, scrollbar.highlight_color),
     XtRString, "Black"
   },

   {
     XtNhighlightTile, XtCHighlightTile, XtRTileType, sizeof (int),
     XtOffset (XwScrollBarWidget, scrollbar.highlight_tile),
     XtRString, "50_foreground"
   },

   {
     XtNhighlightStyle, XtCHighlightStyle, XtRHighlightStyle, sizeof (int),
     XtOffset (XwScrollBarWidget, scrollbar.highlight_style),
     XtRString, "pattern_border"
   },

   {
     XtNtraversalType, XtCTraversalType, XtRTraversalType, sizeof (int),
     XtOffset (XwScrollBarWidget, scrollbar.traversal_type),
     XtRString, "highlight_off"
   },

   {
      XtNsliderMin, XtCSliderMin, XtRInt, sizeof (int),
      XtOffset(XwScrollBarWidget, scrollbar.min),
      XtRString, "0"
   },

   {
      XtNsliderMax, XtCSliderMax, XtRInt, sizeof (int),
      XtOffset(XwScrollBarWidget, scrollbar.max),
      XtRString, "100"
   },

   {
      XtNsliderOrigin, XtCSliderOrigin, XtRInt, sizeof (int),
      XtOffset(XwScrollBarWidget, scrollbar.origin),
      XtRString, "0"
   },

   {
      XtNsliderExtent, XtCSliderExtent, XtRInt, sizeof (int),
      XtOffset(XwScrollBarWidget, scrollbar.extent),
      XtRString, "10"
   },

   {
      XtNslideOrientation, XtCSlideOrientation,
      XtRSlideOrientation, sizeof (int),
      XtOffset(XwScrollBarWidget, scrollbar.orientation),
      XtRString, "vertical"
   },

   {
      XtNinitialDelay, XtCInitialDelay, XtRInt, sizeof (int),
      XtOffset (XwScrollBarWidget, scrollbar.initial_delay),
      XtRString, "500"
   },

   {
      XtNrepeatRate, XtCRepeatRate, XtRInt, sizeof (int),
      XtOffset (XwScrollBarWidget, scrollbar.repeat_rate),
      XtRString, "100"
   },

   {
      XtNgranularity, XtCGranularity, XtRInt, sizeof (int),
      XtOffset (XwScrollBarWidget, scrollbar.granularity),
      XtRString, "2"
   },

   {  
      XtNsliderMoved, XtCCallback, XtRCallback, sizeof (caddr_t),
      XtOffset(XwScrollBarWidget, scrollbar.slider_moved),
      XtRPointer, (caddr_t) NULL
   },

   {  
      XtNsliderReleased, XtCCallback, XtRCallback, sizeof (caddr_t),
      XtOffset(XwScrollBarWidget, scrollbar.slider_released),
      XtRPointer, (caddr_t) NULL
   },

   {  
      XtNareaSelected, XtCCallback, XtRCallback, sizeof (caddr_t),
      XtOffset(XwScrollBarWidget, scrollbar.area_selected),
      XtRPointer, (caddr_t) NULL
   },
};


/*  Static routine definitions  */

static void      Initialize();
static void    Resize();
static void    Destroy();
static Boolean SetValues();

static XtGeometryResult GeometryManager();

static XtTimerCallbackProc XwTimerEvent();

static void XwFindChildGeoms();
static void XwGetGeomArgs();
static void XwCalcGeom();
static void XwSliderAreaCallback();
static void XwSliderMovedCallback();
static void XwSliderReleasedCallback();
static void XwArrowSelectCallback();
static void XwArrowReleaseCallback();
static void XwGetValuatorVals();
static Boolean XwIncrementValuator();
static Boolean XwDecrementValuator();
static void XwBackInValues();


/*  shared arg lists  */

static Arg arrow1Args[20];
static Arg arrow2Args[20];
static Arg valuatorArgs[24];



/*  The ScrollBar class record */

XwScrollBarClassRec XwscrollbarClassRec =
{
   {
      (WidgetClass) &XwmanagerClassRec, /* superclass	         */	
      "ScrollBar",                       /* class_name	         */	
      sizeof(XwScrollBarRec),           /* widget_size	         */	
      NULL,                             /* class_initialize      */    
      NULL,                             /* class_part_initialize */    
      FALSE,                            /* class_inited          */	
      (XtInitProc) Initialize,          /* initialize	         */	
      NULL,                             /* initialize_hook       */
      _XwRealize,                       /* realize	         */	
      NULL,                             /* actions               */	
      0,                                /* num_actions	         */	
      resources,                        /* resources	         */	
      XtNumber(resources),              /* num_resources         */	
      NULLQUARK,                        /* xrm_class	         */	
      TRUE,                             /* compress_motion       */	
      TRUE,                             /* compress_exposure     */	
      TRUE,                             /* compress_enterleave   */
      FALSE,                            /* visible_interest      */	
      (XtWidgetProc) Destroy,           /* destroy               */	
      (XtWidgetProc) Resize,            /* resize                */	
      NULL,                             /* expose                */	
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

   {                                          /*  composite class     */
      (XtGeometryHandler) GeometryManager,    /*  geometry_manager    */    
      NULL,                                   /*  change_managed      */    
      XtInheritInsertChild,                   /*  insert_child        */
      XtInheritDeleteChild,                   /*  delete_child        */
      NULL,
   },

   {                 /*  constraint class  */
      NULL,          /*  constraint resource set     */
      0,             /*  num_resources               */
      NULL,          /*  size of the constraint data */
      NULL,          /*  contraint initilize proc    */
      NULL,          /*  contraint destroy proc      */
      NULL,          /*  contraint set values proc   */
      NULL,
   },

   {                          /*  manager class     */
      (XwTraversalProc) XtInheritTraversalProc  /*  traversal handler */    
   },

   {              /*  scrollbar class  */
      0           /*  mumble           */               
   }	
};


WidgetClass XwscrollbarWidgetClass = (WidgetClass) &XwscrollbarClassRec;
WidgetClass XwscrollBarWidgetClass = (WidgetClass) &XwscrollbarClassRec;



/************************************************************************
 *
 *  Initialize
 *     The main widget instance initialization routine.
 *
 ************************************************************************/

static void Initialize (request, new)
XwScrollBarWidget request, new;

{
   if (new -> scrollbar.orientation != XwHORIZONTAL &&
       new -> scrollbar.orientation != XwVERTICAL)
   {
      XtWarning ("Initialize: invalid scrollbar orientation.");
      new -> scrollbar.orientation = XwVERTICAL;
   }

   if (new -> scrollbar.granularity <= 0)
   {
      XtWarning ("ScrollBar: The scrolling granularity must be greater than 0.");
      new -> scrollbar.granularity = 2;
   }

   if (new -> scrollbar.initial_delay <= 0)
   {
      XtWarning ("ScrollBar: The scrolling initial delay must be greater than 0.");
      new -> scrollbar.initial_delay = 500;
   }

   if (new -> scrollbar.repeat_rate <= 0)
   {
      XtWarning ("ScrollBar: The scrolling repeat rate must be greater than 0.");
      new -> scrollbar.repeat_rate = 100;
   }


   /*  Set up a geometry for the widget if it is currently 0.  */

   if (request -> core.width == 0)
   {
      if (new -> scrollbar.orientation == XwHORIZONTAL)
         new -> core.width += 100 + new -> manager.highlight_thickness;
      else
         new -> core.width += 15 + new -> manager.highlight_thickness;
   }

   if (request -> core.height == 0)
   {
      if (new -> scrollbar.orientation == XwHORIZONTAL)
         new -> core.height += 15 + new -> manager.highlight_thickness;
      else
         new -> core.height += 100 + new -> manager.highlight_thickness;
   }


   /*
    *  Perform the generation of the data and creation of the internal
    *  widgets used by ScrollBar.
    */

   /*  Set up the arrow directions, and valuator orientation  */

   if (new -> scrollbar.orientation == XwHORIZONTAL)
   {
      XtSetArg (arrow1Args[5], XtNarrowDirection, (XtArgVal) XwARROW_LEFT);
      XtSetArg (arrow2Args[5], XtNarrowDirection, (XtArgVal) XwARROW_RIGHT);
   }
   else
   {
      XtSetArg (arrow1Args[5], XtNarrowDirection, (XtArgVal) XwARROW_UP);
      XtSetArg (arrow2Args[5], XtNarrowDirection, (XtArgVal) XwARROW_DOWN);
   }

   XtSetArg (valuatorArgs[5],XtNslideOrientation,new->scrollbar.orientation);
                              


   /*  Set up the "Primitive" args for widgets  */

   XtSetArg (arrow1Args[6],XtNforeground,(XtArgVal)new->manager.foreground);
   XtSetArg (arrow2Args[6],XtNforeground,(XtArgVal)new->manager.foreground);
   XtSetArg (valuatorArgs[6],XtNforeground,(XtArgVal)new->manager.foreground);

   XtSetArg (arrow1Args[7],XtNbackground,(XtArgVal)new->core.background_pixel);
   XtSetArg (arrow2Args[7],XtNbackground,(XtArgVal)new->core.background_pixel);
   XtSetArg (valuatorArgs[7],XtNbackground,(XtArgVal)new->core.background_pixel);

   XtSetArg (arrow1Args[8],XtNhighlightColor,(XtArgVal)new->scrollbar.highlight_color);
   XtSetArg (arrow2Args[8],XtNhighlightColor,(XtArgVal)new->scrollbar.highlight_color);
   XtSetArg (valuatorArgs[8],XtNhighlightColor,(XtArgVal)new->scrollbar.highlight_color);

   XtSetArg (arrow1Args[9],XtNhighlightTile,(XtArgVal)new->scrollbar.highlight_tile);
   XtSetArg (arrow2Args[9],XtNhighlightTile,(XtArgVal)new->scrollbar.highlight_tile);
   XtSetArg (valuatorArgs[9],XtNhighlightTile,(XtArgVal)new->scrollbar.highlight_tile);

   XtSetArg (arrow1Args[10],XtNhighlightStyle,(XtArgVal)new->scrollbar.highlight_style);
   XtSetArg (arrow2Args[10],XtNhighlightStyle,(XtArgVal)new->scrollbar.highlight_style);
   XtSetArg (valuatorArgs[10],XtNhighlightStyle,(XtArgVal)new->scrollbar.highlight_style);

   XtSetArg (arrow1Args[11],XtNhighlightThickness,
             (XtArgVal)new->manager.highlight_thickness);
   XtSetArg (arrow2Args[11],XtNhighlightThickness,
             (XtArgVal)new->manager.highlight_thickness);
   XtSetArg (valuatorArgs[11],XtNhighlightThickness,
             (XtArgVal)new->manager.highlight_thickness);
   new -> manager.highlight_thickness = 0;

   XtSetArg (arrow1Args[12],XtNtraversalType,(XtArgVal)new->scrollbar.traversal_type);
   XtSetArg (arrow2Args[12],XtNtraversalType,(XtArgVal)new->scrollbar.traversal_type);
   XtSetArg (valuatorArgs[12],XtNtraversalType,(XtArgVal)new->scrollbar.traversal_type);

   XtSetArg (arrow1Args[13], XtNbackgroundTile,
             (XtArgVal)new->manager.background_tile);
   XtSetArg (arrow2Args[13], XtNbackgroundTile,
             (XtArgVal)new->manager.background_tile);
   XtSetArg (valuatorArgs[13], XtNbackgroundTile,
             (XtArgVal)new->manager.background_tile);


   /*  Set the valuator constraints  */

   XtSetArg(valuatorArgs[14],XtNsliderMin,(XtArgVal)new->scrollbar.min);
   XtSetArg(valuatorArgs[15],XtNsliderMax,(XtArgVal)new->scrollbar.max);
   XtSetArg(valuatorArgs[16],XtNsliderOrigin,(XtArgVal)new->scrollbar.origin);
   XtSetArg(valuatorArgs[17],XtNsliderExtent,(XtArgVal)new->scrollbar.extent);


   /*  Get the geometries of the arrows and valuator  */

   XwFindChildGeoms (new);


   /*  Create the arrows and valuator using the args calculated  */

   new -> scrollbar.left_or_up_arrow = (XwArrowWidget)
       XtCreateManagedWidget ("Arrow1",XwarrowWidgetClass,new,arrow1Args, 14);

   new -> scrollbar.valuator = (XwValuatorWidget)
      XtCreateManagedWidget ("Valuator",
                             XwvaluatorWidgetClass, new, valuatorArgs, 18);

   new -> scrollbar.right_or_down_arrow = (XwArrowWidget) 
      XtCreateManagedWidget ("Arrow2",XwarrowWidgetClass,new,arrow2Args, 14);


   /*  Add the callbacks into the valuator and arrows.  */

   XtAddCallback (new -> scrollbar.valuator, XtNareaSelected,
                  (XtCallbackProc) XwSliderAreaCallback, NULL);
   XtAddCallback (new -> scrollbar.valuator, XtNsliderMoved,
                  (XtCallbackProc) XwSliderMovedCallback, NULL);
   XtAddCallback (new -> scrollbar.valuator, XtNsliderReleased,
                  (XtCallbackProc) XwSliderReleasedCallback, NULL);
   XtAddCallback (new -> scrollbar.left_or_up_arrow, XtNselect,
                  (XtCallbackProc) XwArrowSelectCallback, NULL);
   XtAddCallback (new -> scrollbar.left_or_up_arrow, XtNrelease,
                  (XtCallbackProc) XwArrowReleaseCallback, NULL);
   XtAddCallback (new -> scrollbar.right_or_down_arrow, XtNselect,
                  (XtCallbackProc) XwArrowSelectCallback, NULL);
   XtAddCallback (new -> scrollbar.right_or_down_arrow, XtNrelease,
                  (XtCallbackProc) XwArrowReleaseCallback, NULL);


   /*  Ensure that the scrollbar reflects the same data as the  */
   /*  arrows and valuator.                                     */

   XwBackInValues (new, new -> scrollbar.left_or_up_arrow);
   XwBackInValues (new, new -> scrollbar.valuator);

}





/************************************************************************
 *
 *  FindChildGeoms
 *	Calculate the geometrys of the child widgets of Scroll bar and
 *      place into the first five members of the static arg lists.
 *
 ************************************************************************/

static void XwFindChildGeoms (sbw)
XwScrollBarWidget sbw;

{
   int a1X, a2X, a1Y, a2Y;
   int vX, vY;
   int aWidth, vWidth;
   int aHeight, vHeight;


   if (sbw -> scrollbar.orientation == XwHORIZONTAL)
   {
      XwCalcGeom (sbw, sbw -> core.width, sbw -> core.height,
                  sbw -> manager.highlight_thickness,
                  &a1X, &a2X, &vX, &aWidth, &vWidth);
      XwGetGeomArgs (sbw, a1X, a2X, vX, aWidth, vWidth);
   }

   else
   {
      XwCalcGeom (sbw, sbw -> core.height, sbw -> core.width, 
                  sbw -> manager.highlight_thickness,
                  &a1Y, &a2Y, &vY, &aHeight, &vHeight);
      XwGetGeomArgs (sbw, a1Y, a2Y, vY, aHeight, vHeight);
   }
}




/************************************************************************
 *
 *  GetGeomArgs
 *	Fill out the geometry args within the static arg lists
 *      given the widget direction, and internal widget origins
 *      and widths.
 *
 ************************************************************************/


static void XwGetGeomArgs (sbw, a1, a2, v, aWidth, vWidth)
XwScrollBarWidget sbw;
int a1, a2, v;
int aWidth, vWidth;

{
   int width = sbw -> core.width;
   int height = sbw -> core.height;
   int border = sbw -> manager.highlight_thickness;
   int border2 = sbw -> manager.highlight_thickness * 2;

   if (sbw -> scrollbar.orientation == XwHORIZONTAL)
   {
      if (aWidth != -1)
      {

	 XtSetArg (arrow1Args[0], XtNx, (XtArgVal) a1);
	 XtSetArg (arrow1Args[1], XtNy, (XtArgVal) border);
	 XtSetArg (arrow1Args[2], XtNwidth, (XtArgVal) aWidth);
	 XtSetArg (arrow1Args[3], XtNheight, (XtArgVal) height - border2);

	 XtSetArg (arrow2Args[0], XtNx, (XtArgVal) a2);
	 XtSetArg (arrow2Args[1], XtNy, (XtArgVal) border);
	 XtSetArg (arrow2Args[2], XtNwidth, (XtArgVal) aWidth);
	 XtSetArg (arrow2Args[3], XtNheight, (XtArgVal) height - border2);
      }

      XtSetArg (valuatorArgs[0], XtNx, (XtArgVal) v);
      XtSetArg (valuatorArgs[1], XtNy, (XtArgVal) border);
      XtSetArg (valuatorArgs[2], XtNwidth, (XtArgVal) vWidth);
      XtSetArg (valuatorArgs[3], XtNheight, (XtArgVal) height - border2);
   }

   else
   {
      if (aWidth != -1)
      {
	 XtSetArg (arrow1Args[0], XtNx, (XtArgVal) border);
	 XtSetArg (arrow1Args[1], XtNy, (XtArgVal) a1);
	 XtSetArg (arrow1Args[2], XtNwidth, (XtArgVal) width - border2);
	 XtSetArg (arrow1Args[3], XtNheight, (XtArgVal) aWidth);

	 XtSetArg (arrow2Args[0], XtNx, (XtArgVal) border);
	 XtSetArg (arrow2Args[1], XtNy, (XtArgVal) a2);
	 XtSetArg (arrow2Args[2], XtNwidth, (XtArgVal) width - border2);
	 XtSetArg (arrow2Args[3], XtNheight, (XtArgVal) aWidth);
      }

      XtSetArg (valuatorArgs[0], XtNx, (XtArgVal) border);
      XtSetArg (valuatorArgs[1], XtNy, (XtArgVal) v);
      XtSetArg (valuatorArgs[2], XtNwidth, (XtArgVal) width - border2);
      XtSetArg (valuatorArgs[3], XtNheight, (XtArgVal) vWidth);
   }


   /*  Force internal border widths to 0.  */

   XtSetArg (arrow1Args[4], XtNborderWidth, (XtArgVal) 0);
   XtSetArg (arrow2Args[4], XtNborderWidth, (XtArgVal) 0);
   XtSetArg (valuatorArgs[4], XtNborderWidth, (XtArgVal) 0);

}




/************************************************************************
 *
 *  CalcGeom
 *	Calculate the geometry for the arrows and valuator contained
 *	within the scrollbar.
 *
 ************************************************************************/

static void XwCalcGeom (sbw, width, height, border,
                             a1X, a2X, vX, aWidth, vWidth)
XwScrollBarWidget sbw;
int width, height, border;
int * a1X, * a2X, * vX;
int * aWidth, * vWidth;

{
   int temp;

   
   /*  Put in "good" width values  */

   *aWidth = height - 2 * border;
   *vWidth = width - 2 * (height + border);


   /*  See if these values match the predefined good ratio of 3:1.  */

   while (*vWidth < *aWidth * 3)
   {
      *aWidth = *aWidth - 1;
      *vWidth = *vWidth + 2;

      if (*aWidth <= border)
      {
	 *aWidth = -1;
	 break;
      }
   }


   /*  Align the new data to correct sizes.  */

   if (*aWidth == -1)
   {
      *vX = border;
      *vWidth = width - 2 * border;
   }
   else
   {
      *a1X = border;
      *vX = *aWidth + border;
      *vWidth = width - *aWidth * 2 - border * 2;;
      *a2X = *aWidth + *vWidth + border;
   }
}




/************************************************************************
 *
 *  Resize
 *	Get the new child widget geometries and use SetValues
 *      to changes their geometries
 *
 ************************************************************************/
		
static void Resize (sbw)
XwScrollBarWidget sbw;

{
   XwFindChildGeoms (sbw);
   XtSetValues (sbw -> scrollbar.left_or_up_arrow, arrow1Args, 5);
   XtSetValues (sbw -> scrollbar.right_or_down_arrow, arrow2Args, 5);
   XtSetValues (sbw -> scrollbar.valuator, valuatorArgs, 5);
}




/************************************************************************
 *
 *  Destroy
 *	Clean up allocated resources when the widget is destroyed.
 *
 ************************************************************************/

static void Destroy (sbw)
XwScrollBarWidget sbw;

{
   XtRemoveAllCallbacks (sbw, XtNsliderMoved, sbw->scrollbar.slider_moved);
   XtRemoveAllCallbacks (sbw, XtNareaSelected, sbw->scrollbar.area_selected);
}




/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/

static Boolean SetValues (current, request, new)
XwScrollBarWidget current, request, new;

{
   int arrowArgCount = 0;
   int valuatorArgCount = 0;    


   /*  Check the data put into the new widget  */

   if (new -> scrollbar.granularity <= 0)
   {
      XtWarning ("ScrollBar: The scrolling granularity must be greater than 0.");
      new -> scrollbar.granularity = current -> scrollbar.granularity;
   }

   if (new -> scrollbar.initial_delay <= 0)
   {
      XtWarning ("ScrollBar: The scrolling initial delay must be greater than 0.");
      new -> scrollbar.initial_delay = current -> scrollbar.initial_delay;
   }

   if (new -> scrollbar.repeat_rate <= 0)
   {
      XtWarning ("ScrollBar: The scrolling repeat rate must be greater than 0.");
      new -> scrollbar.repeat_rate = current -> scrollbar.repeat_rate;
   }


   /*  Check for an invalid orientation  */

   if (new -> scrollbar.orientation != XwHORIZONTAL &&
       new -> scrollbar.orientation != XwVERTICAL)
   {
      XtWarning ("ScrollBar: Invalid scrollbar orientation.");
      new -> scrollbar.orientation = current -> scrollbar.orientation;
   }


   /*  Process a change in orienataion by building an arg list.  */

   if (new -> scrollbar.orientation != current -> scrollbar.orientation)
   {
      if (new -> scrollbar.orientation == XwHORIZONTAL)
      {
         XtSetArg (arrow1Args[arrowArgCount], 
                   XtNarrowDirection, (XtArgVal) XwARROW_LEFT);
         XtSetArg (arrow2Args[arrowArgCount], 
                   XtNarrowDirection, (XtArgVal) XwARROW_RIGHT);
         arrowArgCount++; 
      }
      else
      {
         XtSetArg (arrow1Args[arrowArgCount],
                   XtNarrowDirection, (XtArgVal) XwARROW_UP);
         XtSetArg (arrow2Args[arrowArgCount],
                   XtNarrowDirection, (XtArgVal) XwARROW_DOWN);
         arrowArgCount++;
      }

      XtSetArg (valuatorArgs[valuatorArgCount],
                XtNslideOrientation, new -> scrollbar.orientation);
      valuatorArgCount++;
   }


   /*  Continue building an arg list for attribute changes  */
   /*  that effect both the arrows and valuator.            */

   if (new -> manager.foreground != current -> manager.foreground)
   {
      XtSetArg (arrow1Args[arrowArgCount], XtNforeground,
                (XtArgVal) new -> manager.foreground);
      XtSetArg (arrow2Args[arrowArgCount], XtNforeground,
                (XtArgVal) new -> manager.foreground);
      XtSetArg (valuatorArgs[valuatorArgCount], XtNforeground,
                (XtArgVal) new -> manager.foreground);
      arrowArgCount++; 
      valuatorArgCount++;
   }

   if (new -> core.background_pixel != current -> core.background_pixel)
   {
      XtSetArg (arrow1Args[arrowArgCount], XtNbackground,
                (XtArgVal) new -> core.background_pixel);
      XtSetArg (arrow2Args[arrowArgCount], XtNbackground,
                (XtArgVal) new -> core.background_pixel);
      XtSetArg (valuatorArgs[valuatorArgCount], XtNbackground,
                (XtArgVal) new -> core.background_pixel);
      arrowArgCount++; 
      valuatorArgCount++;
   }

   if (new -> manager.background_tile != current -> manager.background_tile)
   {

      XtSetArg (arrow1Args[arrowArgCount], XtNbackgroundTile,
                (XtArgVal) new -> manager.background_tile);
      XtSetArg (arrow2Args[arrowArgCount], XtNbackgroundTile,
                (XtArgVal) new -> manager.background_tile);
      XtSetArg (valuatorArgs[valuatorArgCount], XtNbackgroundTile,
                (XtArgVal) new -> manager.background_tile);
      arrowArgCount++; 
      valuatorArgCount++;
   }

   if (new->scrollbar.highlight_color != current->scrollbar.highlight_color)
   {
      XtSetArg (arrow1Args[arrowArgCount], XtNhighlightColor,
                (XtArgVal) new -> scrollbar.highlight_color);
      XtSetArg (arrow2Args[arrowArgCount], XtNhighlightColor, 
                (XtArgVal) new -> scrollbar.highlight_color);
      XtSetArg (valuatorArgs[valuatorArgCount], XtNhighlightColor, 
                (XtArgVal) new -> scrollbar.highlight_color);
      arrowArgCount++; 
      valuatorArgCount++;
   }

   if (new->scrollbar.highlight_tile != current->scrollbar.highlight_tile)
   {
      XtSetArg (arrow1Args[arrowArgCount], XtNhighlightTile,
                (XtArgVal) new -> scrollbar.highlight_tile);
      XtSetArg (arrow2Args[arrowArgCount], XtNhighlightTile, 
                (XtArgVal) new -> scrollbar.highlight_tile);
      XtSetArg (valuatorArgs[valuatorArgCount], XtNhighlightTile, 
                (XtArgVal) new -> scrollbar.highlight_tile);
      arrowArgCount++; 
      valuatorArgCount++;
   }

   if (new->scrollbar.highlight_style != current->scrollbar.highlight_style)
   {
      XtSetArg (arrow1Args[arrowArgCount], XtNhighlightStyle,
                (XtArgVal) new -> scrollbar.highlight_style);
      XtSetArg (arrow2Args[arrowArgCount], XtNhighlightStyle, 
                (XtArgVal) new -> scrollbar.highlight_style);
      XtSetArg (valuatorArgs[valuatorArgCount], XtNhighlightStyle, 
                (XtArgVal) new -> scrollbar.highlight_style);
      arrowArgCount++; 
      valuatorArgCount++;
   }

   if (new -> manager.highlight_thickness !=
       current -> manager.highlight_thickness)
   {
         XtSetArg (arrow1Args[arrowArgCount], XtNhighlightThickness,
                   (XtArgVal) new -> manager.highlight_thickness);
         XtSetArg (arrow2Args[arrowArgCount], XtNhighlightThickness, 
                   (XtArgVal) new -> manager.highlight_thickness);
         XtSetArg (valuatorArgs[valuatorArgCount], XtNhighlightThickness, 
                   (XtArgVal) new -> manager.highlight_thickness);
         arrowArgCount++; 
         valuatorArgCount++;
         new -> manager.highlight_thickness = 0;
   }

   if (new->scrollbar.traversal_type != current->scrollbar.traversal_type)
   {
      XtSetArg (arrow1Args[arrowArgCount], XtNtraversalType,
                (XtArgVal) new -> scrollbar.traversal_type);
      XtSetArg (arrow2Args[arrowArgCount], XtNtraversalType, 
                (XtArgVal) new -> scrollbar.traversal_type);
      XtSetArg (valuatorArgs[valuatorArgCount], XtNtraversalType, 
                (XtArgVal) new -> scrollbar.traversal_type);
      arrowArgCount++; 
      valuatorArgCount++;
   }


   if (new -> scrollbar.min != current -> scrollbar.min)
   {
      XtSetArg (valuatorArgs[valuatorArgCount], XtNsliderMin, 
                (XtArgVal) new -> scrollbar.min);
      valuatorArgCount++;
   }

   if (new -> scrollbar.max != current -> scrollbar.max)
   {
      XtSetArg (valuatorArgs[valuatorArgCount], XtNsliderMax, 
                (XtArgVal) new -> scrollbar.max);
      valuatorArgCount++;
   }

   if (new -> scrollbar.origin != current -> scrollbar.origin)
   {
      XtSetArg (valuatorArgs[valuatorArgCount], XtNsliderOrigin, 
                (XtArgVal) new -> scrollbar.origin);
      valuatorArgCount++;
   }

   if (new -> scrollbar.extent != current -> scrollbar.extent)
   {
      XtSetArg (valuatorArgs[valuatorArgCount], XtNsliderExtent,
                (XtArgVal) new -> scrollbar.extent);
      valuatorArgCount++;
   }


   /*  If any arrow or valuator arg lists were built, perform  */
   /*  a SetValues on the children.                            */

   if (arrowArgCount)
   {
      XtSetValues (new -> scrollbar.left_or_up_arrow, 
                   arrow1Args, arrowArgCount);
      XtSetValues (new -> scrollbar.right_or_down_arrow, 
                   arrow2Args, arrowArgCount);
   }

   if (valuatorArgCount)
      XtSetValues (new->scrollbar.valuator, valuatorArgs, valuatorArgCount);

   /*  If the arrow or valuators had a SetValues done to them  */
   /*  ensure that the scrollbar reflects the same data.       */

   if (arrowArgCount)
      XwBackInValues (new, new -> scrollbar.left_or_up_arrow);

   if (valuatorArgCount)
      XwBackInValues (new, new -> scrollbar.valuator);


   /*  Always return false, the primitive perform their own redraw.  */

   return (False);
}




/************************************************************************
 *
 *  XwBackInValues
 *	Ensure that the values in the child widget is the same as the
 *      duplicated data in the scrollbar widget.
 *
 ************************************************************************/

static void XwBackInValues (sbw, child)
XwScrollBarWidget sbw;
Widget    child;

{
   Arg args[10];   


   /*  Set up the args for the primitive classes, and the set up  */
   /*  for either the arrow or valuator class.                    */
   /*  Once set up issue the GetValues to get the actual values.  */

   XtSetArg (args[0], XtNhighlightTile, &sbw->scrollbar.highlight_tile);
   XtSetArg (args[1], XtNhighlightStyle, &sbw->scrollbar.highlight_style);
   XtSetArg (args[2], XtNtraversalType, &sbw->scrollbar.traversal_type);

   if (XtIsSubclass (child, XwvaluatorWidgetClass))
   {
      XtSetArg (args[3], XtNsliderMin, &sbw->scrollbar.min);
      XtSetArg (args[4], XtNsliderMax, &sbw->scrollbar.max);
      XtSetArg (args[5], XtNsliderOrigin, &sbw->scrollbar.origin);
      XtSetArg (args[6], XtNsliderExtent, &sbw->scrollbar.extent);
      XtGetValues (child, args, 7);
   }   
   else
      XtGetValues (child, args, 3);
}




/************************************************************************
 *
 *  GeometryManager
 *	Accept all requests since Scrollbar initiates the requests with
 *      its Resize procedure.
 *
 ************************************************************************/

static XtGeometryResult GeometryManager (w, request, reply)
Widget w;
XtWidgetGeometry * request;
XtWidgetGeometry * reply;

{
   if (request -> request_mode & CWX && request -> request_mode & CWY)
      XtMoveWidget (w, request -> x, request -> y);
   else if (request -> request_mode & CWX)
       XtMoveWidget (w, request -> x, w -> core.y);
   else if (request -> request_mode & CWY)
      XtMoveWidget (w, w -> core.x, request -> y);

   if (request -> request_mode & CWWidth)
      w -> core.width = request -> width;

   if (request -> request_mode & CWHeight)
      w -> core.height = request -> height;

   if (request -> request_mode & CWBorderWidth)
      w -> core.border_width = request -> border_width;

   return (XtGeometryYes);
}




/************************************************************************
 *
 *   ScrollBar Callback functions
 *	The following set of functions are installed into the
 *	valuator and arrows when the scrollbar is created.  Their
 *	purpose is to give the scrollbar access to the callback
 *	processing.
 *
 ************************************************************************/


/*
 *  The first two function handle normal valuator to user supplied
 *  callback processing.
 */


/*
 *  Issue the callbacks with the scrollbar widget as the
 *  issuing widget, not the valuator.
 */

static void XwSliderAreaCallback (vw, closure, call_data)
XwValuatorWidget vw;
caddr_t          closure;
caddr_t          call_data;

{
   XtCallCallbacks (vw -> core.parent, XtNareaSelected, call_data);
}


/*
 *  Issue the callbacks with the scrollbar widget as the
 *  issuing widget, not the valuator.
 */

static void XwSliderMovedCallback (vw, closure, call_data)
XwValuatorWidget vw;
caddr_t          closure;
caddr_t          call_data;

{
   XwScrollBarWidget sb = (XwScrollBarWidget) vw -> core.parent;

   sb -> scrollbar.origin = (int) call_data;
   XtCallCallbacks (sb, XtNsliderMoved, call_data);
}



static void XwSliderReleasedCallback (vw, closure, call_data)
XwValuatorWidget vw;
caddr_t          closure;
caddr_t          call_data;

{
   XwScrollBarWidget sb = (XwScrollBarWidget) vw -> core.parent;

   sb -> scrollbar.origin = (int) call_data;
   XtCallCallbacks (sb, XtNsliderReleased, call_data);
}



/*
 *  The next two functions handle internal processing of callbacks
 *  generated on the arrows and their effect on the valuator.
 */

static void XwArrowSelectCallback (aw, closure, call_data)
XwArrowWidget aw;
caddr_t       closure;
caddr_t       call_data;

{
   XwScrollBarWidget sbw = (XwScrollBarWidget) aw -> core.parent;
   XwValuatorWidget vw;
   int min, max, origin, extent;
   Boolean flag;


   vw = sbw -> scrollbar.valuator;


   /*  Get the current origin, extent, min, and max of the */
   /*  slider in the valuator                              */

   XwGetValuatorVals (vw, &min, &max, &origin, &extent);


   /*  Increase or decrease the valuator slider origin according  */
   /*  to which arrow was selected.                               */

   if (aw == sbw -> scrollbar.left_or_up_arrow)
   {
     flag = XwDecrementValuator (vw, &origin, min);
     sbw -> scrollbar.origin_increasing = False;
   }
   else
   {
     flag = XwIncrementValuator (vw, &origin, max, extent);
     sbw -> scrollbar.origin_increasing = True;
   }


   /*  If the orgin was changed, set up the repeat timer and   */
   /*  invoke the application supplied slider moved callbacks  */

   if (flag)
   {
      sbw -> scrollbar.timerProcessing = True;
      sbw -> scrollbar.initialProcessing = True;

      XtCallCallbacks (sbw, XtNsliderMoved, (caddr_t) origin);      
      sbw -> scrollbar.timer =
         XtAddTimeOut ((unsigned long) sbw->scrollbar.initial_delay,
                       (XtTimerCallbackProc) XwTimerEvent, (caddr_t) sbw); 
   }
}



/*
 *  Remove the timer, if there is one.
 */

static void XwArrowReleaseCallback (aw, closure, call_data)
XwArrowWidget aw;
caddr_t       closure;
caddr_t       call_data;

{
   XwScrollBarWidget sbw = (XwScrollBarWidget) aw -> core.parent;   

   if (sbw -> scrollbar.timerProcessing)
      XtRemoveTimeOut (sbw -> scrollbar.timer);
}




/************************************************************************
 *
 *  XwTimerEvent
 *	This is an event processing function which handles timer
 *	event evoked because of arrow selection.
 *
 ************************************************************************/

static XtTimerCallbackProc XwTimerEvent (sbw, id)
XwScrollBarWidget sbw;
XtIntervalId * id;

{
   int origin, extent, min, max;
   Boolean flag;
   XwValuatorWidget vw;
   XwArrowWidget    aw;


   vw = sbw -> scrollbar.valuator;

   if (sbw -> scrollbar.initialProcessing)
   {
      sbw -> scrollbar.initialProcessing = False;
      XtRemoveTimeOut (sbw -> scrollbar.timer); 
   }


   /*  Get the current origin, extent, min, and max of the  */
   /*  slider in the valuator                               */

   XwGetValuatorVals (vw, &min, &max, &origin, &extent);


   /*  Increase or decrease the valuator slider origin according  */
   /*  to which arrow was selected.                               */

   if (sbw -> scrollbar.origin_increasing)
     flag = XwIncrementValuator (vw, &origin, max, extent);
   else
     flag = XwDecrementValuator (vw, &origin, min);


   /*  If the orgin was changed invoke the application supplied  */
   /*  slider moved callbacks                                    */

   if (flag)
      XtCallCallbacks (sbw, XtNsliderMoved, (caddr_t) origin);      


   /*  Add the timer  */

   sbw -> scrollbar.timer = 
      XtAddTimeOut ((unsigned long) sbw->scrollbar.repeat_rate,
                    (XtTimerCallbackProc) XwTimerEvent,  (caddr_t) sbw); 

}



/************************************************************************
 *
 *  XwGetValuatorVals
 *	Get the current origin and extent of the slider in the valuator
 *
 ************************************************************************/

static void XwGetValuatorVals (vw, min, max, origin, extent)
XwValuatorWidget vw;
int * min;
int * max;
int * origin;
int * extent;

{
   Arg args[4];

   XtSetArg (args[0], XtNsliderOrigin, (XtArgVal) origin);
   XtSetArg (args[1], XtNsliderExtent, (XtArgVal) extent);
   XtSetArg (args[2], XtNsliderMin, (XtArgVal) min);
   XtSetArg (args[3], XtNsliderMax, (XtArgVal) max);
   XtGetValues (vw, args, 4);
}




/************************************************************************
 *
 *  XwIncrementValuator
 *	See if the origin of the slider can be incremented
 *	If so, increment it by granularity and perform a
 *	SetValues on the valuator.  Then, to ensure that the
 *      actual valuator position is reflected within the scrollbar
 *      data, GetValues the origin and reset the scrollbar value.
 *
 ************************************************************************/

static Boolean XwIncrementValuator (vw, origin, max, extent)
XwValuatorWidget vw;
int * origin;
int max;
int extent;

{
   XwScrollBarWidget sbw = (XwScrollBarWidget) vw -> core.parent;
   Arg args[1];

   if (*origin < max - extent)
   {
      *origin += sbw -> scrollbar.granularity;

      if (*origin > max - extent) *origin = max - extent;

      XtSetArg (args[0], XtNsliderOrigin, (XtArgVal) *origin);
      XtSetValues (vw, args, 1);

      XtSetArg (args[0], XtNsliderOrigin, (XtArgVal) origin);
      XtGetValues (vw, args, 1);
      sbw -> scrollbar.origin = *origin;

      return (True);
   }

   return (False);
}




/************************************************************************
 *
 *  XwDecrementValuator
 *	See if the origin of the slider can be decremented
 *	If so, decrement it by granularity and perform a
 *	SetValues on the valuator.  Then, to ensure that the
 *      actual valuator position is reflected within the scrollbar
 *      data, GetValues the origin and reset the scrollbar value.
 *
 ************************************************************************/

static Boolean XwDecrementValuator (vw, origin, min)
XwValuatorWidget vw;
int * origin;
int min;

{
   XwScrollBarWidget sbw = (XwScrollBarWidget) vw -> core.parent;
   Arg args[1];

   if (*origin > min)
   {
      *origin -= sbw -> scrollbar.granularity;

      if (*origin < min) *origin = min;

      XtSetArg (args[0], XtNsliderOrigin, (XtArgVal) *origin);
      XtSetValues (vw, args, 1);

      XtSetArg (args[0], XtNsliderOrigin, (XtArgVal) origin);
      XtGetValues (vw, args, 1);
      sbw -> scrollbar.origin = *origin;

      return (True);
   }

   return (False);
}
