/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        Valuator.c
 **
 **   Project:     X Widgets
 **
 **   Description: Contains code for primitive widget class: Valuator
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
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <Xw/Xw.h>
#include <Xw/XwP.h>
#include <Xw/ValuatorP.h>
#include <Xw/Valuator.h>
#include <X11/StringDefs.h>
#include <X11/keysymdef.h>   


/*  Message set for valuator  */

static char * messages[] =
{
   "Valuator: The valuator min is greater than or equal",	/* 0 */
   "           to the valuator max.",				/* 1 */
   "Valuator: The valuator extent is less than 1 or greater",	/* 2 */
   "           than the valuator max - the valuator min.",	/* 3 */
   "Valuator: The valuator origin is less than the valuator",  /* 4 */
   "           min or greater than the valuator max minus the",/* 5 */
   "           valuator extent.",				/* 6 */
   "Valuator: Incorrect slide area tile.",			/* 7 */
   "Valuator: Incorrect orientation.",				/* 8 */
   "Valuator: Incorrect slide move type.",			/* 9 */
};


/*  Event procedures referenced in the actions list  */

static void Select();
static void Release();
static void Moved();
static void Left();
static void Right();
static void Up();
static void Down();


/*  Default translation table and action list  */

static char defaultTranslations[] =
    "<Btn1Down>:        select()\n\
     <Btn1Up>:          release()\n\
     Button1<PtrMoved>: moved()\n\
     <EnterWindow>:     enter()\n\
     <LeaveWindow>:     leave()\n\
     Ctrl<Key>Left:     left()\n\
     Ctrl<Key>Up:       up()\n\
     Ctrl<Key>Right:    right()\n\
     Ctrl<Key>Down:     down()";


static XtActionsRec actionsList[] =
{
  { "select",      (XtActionProc) Select            },
  { "release",     (XtActionProc) Release           },
  { "moved",       (XtActionProc) Moved             },
  { "left",        (XtActionProc) Left              },
  { "right",       (XtActionProc) Right             },
  { "up",          (XtActionProc) Up                },
  { "down",        (XtActionProc) Down              },
  { "enter",       (XtActionProc) _XwPrimitiveEnter },
  { "leave",       (XtActionProc) _XwPrimitiveLeave },
};


/*  Resource list for Valuator  */

static XtResource resources[] = 
{
   {
      XtNsliderMin, XtCSliderMin, XtRInt, sizeof (int),
      XtOffset(XwValuatorWidget, valuator.min), XtRString, "0"
   },

   {
      XtNsliderMax, XtCSliderMax, XtRInt, sizeof (int),
      XtOffset(XwValuatorWidget, valuator.max), XtRString, "100"
   },

   {
      XtNsliderOrigin, XtCSliderOrigin, XtRInt, sizeof (int),
      XtOffset(XwValuatorWidget, valuator.origin), XtRString, "0"
   },

   {
      XtNsliderExtent, XtCSliderExtent, XtRInt, sizeof (int),
      XtOffset(XwValuatorWidget, valuator.extent), XtRString, "10"
   },

   {
      XtNslideOrientation, XtCSlideOrientation,
      XtRSlideOrientation, sizeof (int),
      XtOffset(XwValuatorWidget, valuator.orientation), XtRString, "vertical"
   },

   {
      XtNsliderTile, XtCSliderTile, XtRTileType, sizeof (int),
      XtOffset(XwValuatorWidget, valuator.slider_tile),
      XtRString, "foreground"
   },

   {
      XtNsliderMoved, XtCCallback, XtRCallback, sizeof (caddr_t),
      XtOffset(XwValuatorWidget, valuator.slider_moved),
      XtRPointer, (caddr_t) NULL
   },

   {
      XtNsliderReleased, XtCCallback, XtRCallback, sizeof (caddr_t),
      XtOffset(XwValuatorWidget, valuator.slider_released),
      XtRPointer, (caddr_t) NULL
   },

   {
      XtNareaSelected, XtCCallback, XtRCallback, sizeof (caddr_t),
      XtOffset(XwValuatorWidget, valuator.area_selected),
      XtRPointer, (caddr_t) NULL
   }
};


/*  Non-transitory variables used for interactive sliding  */

static int initialX;
static int initialY;
static int separationX;
static int separationY;


/*  Static routine definitions  */

static void      Initialize();
static void    Redisplay();
static void    Resize();
static void    Destroy();
static Boolean SetValues();

static int CalcSliderVal();
static void CalcSliderRect();

static void GetSliderGC();
static void GetSliderFillGC();

static void GetSliderPixmap();
static void InitSliding();
static void MoveSlider();
static void DrawSlider();
static void RedrawSlider();


/*  The Valuator class record definition  */

XwValuatorClassRec XwvaluatorClassRec =
{
   {
      (WidgetClass) &XwprimitiveClassRec, /* superclass	         */	
      "Valuator",                       /* class_name	         */	
      sizeof(XwValuatorRec),            /* widget_size	         */	
      NULL,                             /* class_initialize      */    
      NULL,                             /* class_part_initialize */    
      FALSE,                            /* class_inited          */	
      (XtInitProc) Initialize,          /* initialize	         */	
      NULL,                             /* initialize_hook       */
      _XwRealize,                       /* realize	         */	
      actionsList,                      /* actions               */	
      XtNumber(actionsList),            /* num_actions	         */	
      resources,                        /* resources	         */	
      XtNumber(resources),              /* num_resources         */	
      NULLQUARK,                        /* xrm_class	         */	
      TRUE,                             /* compress_motion       */	
      TRUE,                             /* compress_exposure     */	
      TRUE,                             /* compress_enterleave   */
      FALSE,                            /* visible_interest      */	
      (XtWidgetProc) Destroy,           /* destroy               */	
      (XtWidgetProc) Resize,            /* resize                */	
      (XtExposeProc) Redisplay,         /* expose                */	
      (XtSetValuesFunc) SetValues,      /* set_values    	 */	
      NULL,                             /* set_values_hook       */
      XtInheritSetValuesAlmost,         /* set_values_almost     */
      NULL,                             /* get_values_hook       */
      NULL,                             /* accept_focus	         */	
      XtVersion,                        /* version               */
      NULL,                             /* callback private      */
      defaultTranslations,              /* tm_table              */
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
   }
};

WidgetClass XwvaluatorWidgetClass = (WidgetClass) &XwvaluatorClassRec;




/************************************************************************
 *
 *  Initialize
 *     The main widget instance initialization routine.
 *
 ************************************************************************/

static void Initialize (request, new)
XwValuatorWidget request, new;

{
   /*  Set the internally used variables.  */

   new -> valuator.sliding_on = FALSE;


    /*  Check the data put into the new widget from .Xdefaults  */
    /*  or through the arg list.                                */

   if (new -> valuator.min >= new -> valuator.max)
   {
      XtWarning (messages[0]);
      XtWarning (messages[1]);
      new -> valuator.min = 0;
      new -> valuator.max = 100;
   }

   if (new -> valuator.extent < 1 || 
       new -> valuator.extent > new -> valuator.max - new -> valuator.min)
   {
      XtWarning (messages[2]);
      XtWarning (messages[3]);
      new -> valuator.extent = 1;
   }

   if (new -> valuator.origin < new -> valuator.min ||
       new -> valuator.origin > new -> valuator.max - new -> valuator.extent)
   {
      XtWarning (messages[4]);
      XtWarning (messages[5]);
      XtWarning (messages[6]);
      new -> valuator.origin = new -> valuator.min;
   }

   if (new -> valuator.slider_tile < XwFOREGROUND ||
       new -> valuator.slider_tile > XwSLANT_LEFT)
   {
      XtWarning (messages[7]);
      new -> valuator.slider_tile = XwFOREGROUND;
   }

   if (new -> valuator.orientation != XwHORIZONTAL &&
       new -> valuator.orientation != XwVERTICAL)
   {
      XtWarning (messages[8]);
      new -> valuator.orientation = XwVERTICAL;
   }


   /*  Set up a geometry for the widget if it is currently 0.  */

   if (request -> core.width == 0)
   {
      if (new -> valuator.orientation == XwHORIZONTAL)
	 new -> core.width += 100;
      else
	 new -> core.width += 15;
   }
   if (request -> core.height == 0)
   {
      if (new -> valuator.orientation == XwHORIZONTAL)
	 new -> core.height += 15;
      else
	 new -> core.height += 100;
   }


   /*  Get the drawing graphics contexts.  */

   GetSliderFillGC (new);
   GetSliderGC (new);
   new -> valuator.pixmap = 0;
}




/************************************************************************
 *
 *  GetSliderFillGC
 *     Get the graphics context used for drawing the slide area.
 *
 ************************************************************************/

static void GetSliderFillGC (vw)
XwValuatorWidget vw;

{
    XGCValues values;
    XtGCMask  valueMask;

    valueMask = GCForeground | GCBackground | GCFillStyle | GCTile;

    values.foreground = vw -> primitive.foreground;
    values.background = vw -> core.background_pixel;
    values.fill_style = FillTiled;
    values.tile = XwCreateTile (XtScreen ((Widget)(vw)),
                                values.foreground, values.background,
                                vw -> valuator.slider_tile);

    vw->valuator.slider_fill_GC = XtGetGC ((Widget) vw, valueMask, &values);
}



/************************************************************************
 *
 *  GetSliderGC
 *     Get the graphics context used for drawing the slider
 *
 ************************************************************************/

static void GetSliderGC (vw)
XwValuatorWidget vw;

{
    XGCValues values;
    XtGCMask  valueMask;

    valueMask = GCForeground | GCBackground | GCFillStyle | 
                GCTile | GCGraphicsExposures;

    values.foreground = vw -> primitive.foreground;
    values.background = vw -> core.background_pixel;
    values.fill_style = FillTiled;
    values.tile = XwCreateTile (XtScreen ((Widget)(vw)),
                                values.foreground, values.foreground, 
                                XwFOREGROUND);
    values.graphics_exposures = False;
    
    vw -> valuator.slider_GC = XtGetGC ((Widget) vw, valueMask, &values);
}




/************************************************************************
 *
 *  GetSliderPixmap
 *     Create the pixmap for the slider and render the slider.
 *
 ************************************************************************/

static void GetSliderPixmap (vw)
XwValuatorWidget vw;

{
   int sliderX, sliderY, sliderWidth, sliderHeight;

   CalcSliderRect (vw, &sliderX, &sliderY, &sliderWidth, &sliderHeight);

   vw -> valuator.pixmap = 
      XCreatePixmap (XtDisplay(vw), RootWindowOfScreen (XtScreen(vw)), 
                     sliderWidth, sliderHeight, vw -> core.depth);

   vw -> valuator.sliderX = sliderX;
   vw -> valuator.sliderY = sliderY;
   vw -> valuator.sliderWidth = sliderWidth;
   vw -> valuator.sliderHeight = sliderHeight;

   DrawSlider(vw);
}




/************************************************************************
 *
 *  CalcSliderRect
 *     Calculate the slider location and size in pixels so that
 *     it can be drawn.
 *
 ************************************************************************/

static void
CalcSliderRect (vw, sliderX, sliderY, sliderWidth, sliderHeight)
XwValuatorWidget vw;
int * sliderX;
int * sliderY;
int * sliderWidth;
int * sliderHeight;

{
   float userSize;
   float trueSize;
   float factor;
   float slideStart;
   float slideSize;

   int borderWidth = vw -> primitive.highlight_thickness + 1;

   userSize = vw -> valuator.max - vw -> valuator.min;

   if (vw -> valuator.orientation == XwHORIZONTAL)
      trueSize =  vw -> core.width - 2.0 * borderWidth;
   else
      trueSize = vw -> core.height - 2.0 * borderWidth;

   factor = trueSize / userSize;

   slideStart = (float) (vw ->valuator.origin - vw->valuator.min) *
                         factor + borderWidth;

   slideSize = (float) (vw -> valuator.extent) * factor;

   if (vw -> valuator.orientation == XwHORIZONTAL)
   {
      *sliderX = (int) (slideStart + 0.5);
      *sliderY = borderWidth;
      *sliderWidth = (int) (slideSize + 0.5);
      *sliderHeight = vw -> core.height - 2 * borderWidth;
   }
   else
   {
      *sliderX = borderWidth;
      *sliderY = (int) (slideStart + 0.5);
      *sliderWidth = vw -> core.width - 2 * borderWidth;
      *sliderHeight = (int) (slideSize + 0.5);
   }

   if (*sliderWidth < MIN_SLIDER_SIZE)
   {
      *sliderWidth = MIN_SLIDER_SIZE;
      if (vw -> valuator.orientation == XwHORIZONTAL &&
          *sliderX + *sliderWidth + 1> 
          vw -> core.width - vw -> primitive.highlight_thickness)
         *sliderX = vw->core.width - vw->primitive.highlight_thickness -
                    *sliderWidth - 1;
   }

   if (*sliderHeight < MIN_SLIDER_SIZE)
   {
      *sliderHeight = MIN_SLIDER_SIZE;
      if (vw -> valuator.orientation == XwVERTICAL &&
          *sliderY + *sliderHeight + 1 > 
          vw -> core.height - vw -> primitive.highlight_thickness)
         *sliderY = vw->core.height - vw->primitive.highlight_thickness -
                    *sliderHeight - 1;
   }
}




/************************************************************************
 *
 *  DrawSlider
 *     Drawing function for drawing the slider.
 *
 ************************************************************************/

static void DrawSlider (vw)
XwValuatorWidget vw;

{
   register int sliderWidth = vw -> valuator.sliderWidth;
   register int sliderHeight = vw -> valuator.sliderHeight;
   register Drawable slider = vw -> valuator.pixmap;


   XFillRectangle (XtDisplay ((Widget) vw), slider,
                   vw -> valuator.slider_fill_GC,
                   0, 0, sliderWidth, sliderHeight);
   
}





/************************************************************************
 *
 *  Redisplay
 *     General redisplay function called on exposure events.
 *
 ************************************************************************/

static void Redisplay (vw, event, region)
XwValuatorWidget vw;
XEvent *event;
Region region;

{
   if (vw -> valuator.pixmap == 0) GetSliderPixmap(vw);

   if (vw -> core.sensitive && vw -> core.ancestor_sensitive)
   {
      if (vw -> valuator.pixmap != 0)
         XCopyArea (XtDisplay ((Widget) vw),
                    vw -> valuator.pixmap, XtWindow ((Widget) vw),
                    vw -> valuator.slider_GC,
                    0, 0, vw->valuator.sliderWidth, vw->valuator.sliderHeight,
                    vw->valuator.sliderX, vw->valuator.sliderY);
   }

   else
   {
      int borderWidth = vw -> primitive.highlight_thickness;

      if (vw -> core.width - 2 * borderWidth > 0 &&
          vw -> core.height - 2 * borderWidth > 0)
         XClearArea (XtDisplay ((Widget) vw), XtWindow ((Widget) vw),
                     borderWidth, borderWidth,
                     vw -> core.width - 2 * borderWidth,
                     vw -> core.height - 2 * borderWidth, False);
   }
}





/************************************************************************
 *
 *  Resize
 *     Process resizes on the widget by destroying and recreating the
 *     slider pixmap.
 *
 ************************************************************************/

static void Resize (vw)
XwValuatorWidget vw;

{
   if (vw -> valuator.pixmap)
   {
      XFreePixmap (XtDisplay (vw), vw -> valuator.pixmap);
      vw -> valuator.pixmap = 0;
   }
   GetSliderPixmap (vw);
}




/************************************************************************
 *
 *  Destroy
 *	Clean up allocated resources when the widget is destroyed.
 *
 ************************************************************************/

static void Destroy (vw)
XwValuatorWidget vw;

{
   XtDestroyGC (vw -> valuator.slider_GC);
   XtDestroyGC (vw -> valuator.slider_fill_GC);
   if (vw -> valuator.pixmap != 0)
      XFreePixmap (XtDisplay (vw), vw -> valuator.pixmap);
   XtRemoveAllCallbacks (vw, XtNsliderMoved, vw -> valuator.slider_moved);
   XtRemoveAllCallbacks (vw, XtNareaSelected, vw -> valuator.area_selected);
   XtRemoveAllCallbacks (vw, XtNareaSelected, vw -> valuator.slider_released);
}




/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/

static Boolean SetValues (current, request, new)
XwValuatorWidget current, request, new;

{
   Boolean returnFlag = FALSE;
    

    /*  Check the data put into the new widget from .Xdefaults  */
    /*  or through the arg list.                                */

   if (new -> valuator.min >= new -> valuator.max)
   {
      XtWarning (messages[0]);
      XtWarning (messages[1]);
      new -> valuator.min = current -> valuator.min;
      new -> valuator.max =  current -> valuator.max;
   }

   if (new -> valuator.extent < 1 || 
       new -> valuator.extent > new -> valuator.max - new -> valuator.min)
   {
      XtWarning (messages[2]);
      XtWarning (messages[3]);

      if (current -> valuator.extent <= new->valuator.max - new->valuator.min)
         new -> valuator.extent = current -> valuator.extent;
      else
         new -> valuator.extent = 1;
   }

   if (new -> valuator.origin < new -> valuator.min ||
       new -> valuator.origin > new -> valuator.max - new -> valuator.extent)
   {
      XtWarning (messages[4]);
      XtWarning (messages[5]);
      XtWarning (messages[6]);

      if (current -> valuator.origin < new -> valuator.min ||
          current->valuator.origin > new->valuator.max - new->valuator.extent)
         new -> valuator.origin = new -> valuator.min;
      else
         new -> valuator.origin = current -> valuator.origin;
   }

   if (new -> valuator.slider_tile < XwFOREGROUND ||
       new -> valuator.slider_tile > XwSLANT_LEFT)
   {
      XtWarning (messages[7]);
      new -> valuator.slider_tile = current -> valuator.slider_tile;
   }

   if (new -> valuator.orientation != XwHORIZONTAL &&
       new -> valuator.orientation != XwVERTICAL)
   {
      XtWarning (messages[8]);
      new -> valuator.orientation = current -> valuator.orientation;
   }


   /*  See if the GC's need to be regenerated and widget redrawn.  */

   if (new -> valuator.extent != current->valuator.extent            ||
       new -> valuator.min != current -> valuator.min                ||
       new -> valuator.max != current -> valuator.max                ||
       new -> valuator.slider_tile != current->valuator.slider_tile  ||
       new -> valuator.orientation != current->valuator.orientation)
   {
      returnFlag = TRUE;

      XtDestroyGC (new -> valuator.slider_fill_GC);
      XtDestroyGC (new -> valuator.slider_GC);

      if (new -> valuator.pixmap != 0)
      {
         XFreePixmap (XtDisplay (new), new -> valuator.pixmap);
         new -> valuator.pixmap = 0;
      }

      GetSliderFillGC (new);
      GetSliderGC (new);
   }


   /*  If the return flag is False "no redraw" and the slider  */
   /*  origin has changed, erase and redraw the slider.        */

   if (returnFlag == False && 
       new -> valuator.origin != current -> valuator.origin)
      RedrawSlider (new);

   if (!XtIsRealized (new))
      CalcSliderRect (new,
                      &(new -> valuator.sliderX),
                      &(new -> valuator.sliderY),
                      &(new -> valuator.sliderWidth),
                      &(new -> valuator.sliderHeight));

   return (returnFlag);
}




/************************************************************************
 *
 *  CalcSliderVal
 *     Calculate the slider val in application coordiates given
 *     the input x and y.
 *
 ************************************************************************/

static int CalcSliderVal (vw, x, y)
XwValuatorWidget vw;
int   x;
int   y;

{
   int   borderWidth;
   float userSize;
   float trueSize;
   float factor;
   float slideStart;
   float slideSize;
   float referencePoint;
   float temp;

   borderWidth = vw -> primitive.highlight_thickness;

   if (vw -> valuator.orientation == XwHORIZONTAL)
   {
      referencePoint = (float) x - separationX;
      trueSize = vw -> core.width - 2 * borderWidth - 4;
   }
   else
   {
      referencePoint = (float) y - separationY;
      trueSize = vw -> core.height - 2 * borderWidth - 4;
   }
   
   temp = (referencePoint - vw->primitive.highlight_thickness - 1) / trueSize;

   userSize = vw -> valuator.max - vw -> valuator.min;
   temp = temp * userSize + vw -> valuator.min + 0.5;

   if ((int) temp < vw -> valuator.min)
      temp = vw -> valuator.min;
   else if ((int) temp > vw -> valuator.max - vw -> valuator.extent)
      temp = vw -> valuator.max - vw -> valuator.extent;

   return ((int) temp);
}




/************************************************************************
 *
 *  Select
 *     This function processes selections occuring on the valuator.
 *
 ************************************************************************/

static void Select (vw, buttonEvent)
XwValuatorWidget      vw;
XButtonPressedEvent * buttonEvent;

{
   int borderWidth = vw -> primitive.highlight_thickness;
   int selectionPos;

   int sliderX = vw -> valuator.sliderX;
   int sliderY = vw -> valuator.sliderY;
   int sliderWidth = vw -> valuator.sliderWidth;
   int sliderHeight = vw -> valuator.sliderHeight;
   
   if (vw -> core.width - 2 * borderWidth > 3 &&
       vw -> core.height - 2 * borderWidth > 3)
   {
      /*  Calculate whether the selection point is in the slider  */

      if (buttonEvent -> x >= sliderX                     &&
          buttonEvent -> x <= sliderX + sliderWidth - 1   &&
          buttonEvent -> y >= sliderY                     &&
          buttonEvent -> y <= sliderY + sliderHeight - 1)
      {
         separationX = buttonEvent -> x - sliderX;
         separationY = buttonEvent -> y - sliderY;
         initialX = sliderX;
         initialY = sliderY;
         vw -> valuator.sliding_on = True;
      }

      else
      {
         separationX = 0;
         separationY = 0;
         selectionPos = CalcSliderVal (vw, buttonEvent -> x, buttonEvent -> y);
         XtCallCallbacks (vw, XtNareaSelected, (caddr_t) selectionPos);
      }
   }
}




/************************************************************************
 *
 *  Release
 *     This function processes releases occuring on the valuator.
 *
 ************************************************************************/

static void Release (vw, event)
XwValuatorWidget vw;
XEvent * event;

{
   if (vw -> valuator.sliding_on == True)
   {
      vw -> valuator.sliding_on = False;
      Moved (vw, event);
      XtCallCallbacks(vw, XtNsliderReleased, (caddr_t) vw -> valuator.origin);
   }
}




/************************************************************************
 *
 *  Moved
 *     This function processes mouse moved events during interactive
 *     slider moves.
 *
 ************************************************************************/

static void Moved (vw, event)
XwValuatorWidget vw;
XEvent * event;

{
   XButtonPressedEvent * buttonEvent = (XButtonPressedEvent *) event;
   int newX, newY;
   int slideVal;

   if (vw -> valuator.sliding_on != True)
      return;

   if (vw -> valuator.orientation == XwHORIZONTAL)
   {
      newX = buttonEvent -> x - separationX;
      newY = vw -> valuator.sliderY;

      if (newX < vw -> primitive.highlight_thickness + 1)
         newX = vw -> primitive.highlight_thickness + 1;
      if (newX + vw -> valuator.sliderWidth > 
          vw -> core.width - vw -> primitive.highlight_thickness - 1)
         newX = vw -> core.width - vw -> primitive.highlight_thickness -
                vw -> valuator.sliderWidth - 1;

   }
   else
   {
      newX = vw -> valuator.sliderX;
      newY = buttonEvent -> y - separationY;

      if (newY < vw -> primitive.highlight_thickness + 1)
         newY = vw -> primitive.highlight_thickness + 1;
      if (newY + vw -> valuator.sliderHeight > 
          vw -> core.height - vw -> primitive.highlight_thickness - 1)
         newY = vw -> core.height - vw -> primitive.highlight_thickness -
                vw -> valuator.sliderHeight - 1;
   }


   if ((vw -> valuator.orientation == XwHORIZONTAL && newX != initialX) ||
       (vw -> valuator.orientation == XwVERTICAL   && newY != initialY))
   {
      slideVal = CalcSliderVal (vw, buttonEvent -> x, buttonEvent -> y);

      if (slideVal == vw -> valuator.origin)
         return;

      vw -> valuator.origin = slideVal;

      MoveSlider(vw, newX, newY);
      XtCallCallbacks (vw, XtNsliderMoved, (caddr_t) slideVal);
   
      initialX = newX;
      initialY = newY;
   }
}




/************************************************************************
 *
 *  MoveSlider
 *	Given x and y positions, move the slider and clear the area
 *	moved out of.
 *
 ************************************************************************/

static void MoveSlider (vw, currentX, currentY)
XwValuatorWidget vw;
int currentX, currentY;

{
   int oldX = vw -> valuator.sliderX;
   int oldY = vw -> valuator.sliderY;
   int width = vw -> valuator.sliderWidth;
   int height = vw -> valuator.sliderHeight;

   XSegment seg[2];


   if (vw -> valuator.orientation == XwHORIZONTAL)
   {
      vw -> valuator.sliderX = currentX;

      seg[0].y1 = seg[0].y2 = oldY + 2;
      seg[1].y1 = seg[1].y2 = oldY + height - 3;

      if (oldX < currentX)
      {
         seg[0].x1 = seg[1].x1 = oldX;
         seg[0].x2 = seg[1].x2 = oldX + currentX - oldX - 1;
      }
      else
      {
         seg[0].x1 = seg[1].x1 = currentX + width;
         seg[0].x2 = seg[1].x2 = seg[0].x1 + oldX - currentX - 1;
      }


      if (vw -> valuator.pixmap != 0)
      {
         XCopyArea (XtDisplay ((Widget) vw),
                    vw -> valuator.pixmap, XtWindow ((Widget) vw),
                    vw -> valuator.slider_GC,
                    0, 0, width, height, currentX, currentY);

         XClearArea (XtDisplay ((Widget) vw), XtWindow ((Widget) vw),
                     seg[0].x1, oldY, 
                     seg[0].x2 - seg[0].x1 + 1, height, False);
      }
   } 
   else
   {
      vw -> valuator.sliderY = currentY;

      seg[0].x1 = seg[0].x2 = oldX + 2;
      seg[1].x1 = seg[1].x2 = oldX + width - 3;

      if (oldY < currentY)
      {
         seg[0].y1 = seg[1].y1 = oldY;
         seg[0].y2 = seg[1].y2 = oldY + currentY - oldY - 1;
      }
      else
      {
         seg[0].y1 = seg[1].y1 = currentY + height;
         seg[0].y2 = seg[1].y2 = seg[0].y1 + oldY - currentY - 1;
      }

      if (vw -> valuator.pixmap != 0)
      {
         XCopyArea (XtDisplay ((Widget) vw),
                    vw -> valuator.pixmap, XtWindow ((Widget) vw),
                    vw -> valuator.slider_GC,
                    0, 0, width, height, currentX, currentY);

         XClearArea (XtDisplay ((Widget) vw), XtWindow ((Widget) vw),
                     oldX, seg[0].y1, 
                     width, seg[0].y2 - seg[0].y1 + 1, False);
      }
   }
}



/************************************************************************
 *
 *  Left
 *     This function processes left keyboard actions to handle
 *     keyboard input for the valuator.
 *
 ************************************************************************/

static void Left (vw, event)
XwValuatorWidget vw;
XEvent * event;

{
   /*  If the slider origin can be decreased, decrement it,     */
   /*  redraw the slider, and call the slider moved callbacks.  */

   if (vw -> valuator.orientation == XwHORIZONTAL &&
       vw -> valuator.origin > vw -> valuator.min)
   {
      vw -> valuator.origin--;
      RedrawSlider (vw);
      XtCallCallbacks (vw, XtNsliderMoved, (caddr_t) vw -> valuator.origin);
   }
}




/************************************************************************
 *
 *  Right
 *     This function processes right keyboard actions to handle
 *     keyboard input for the valuator.
 *
 ************************************************************************/

static void Right (vw, event)
XwValuatorWidget vw;
XEvent * event;

{
   /*  If the slider origin can be increased, increment it,     */
   /*  redraw the slider, and call the slider moved callbacks.  */

   if (vw -> valuator.orientation == XwHORIZONTAL &&
       vw -> valuator.origin < vw -> valuator.max - vw -> valuator.extent)
   {
      vw -> valuator.origin++;
      RedrawSlider (vw);
      XtCallCallbacks (vw, XtNsliderMoved, (caddr_t) vw -> valuator.origin);
   }
}



/************************************************************************
 *
 *  Up
 *     This function processes up keyboard actions to handle
 *     keyboard input for the valuator.
 *
 ************************************************************************/

static void Up (vw, event)
XwValuatorWidget vw;
XEvent * event;

{
   /*  If the slider origin can be decreased, decrement it,     */
   /*  redraw the slider, and call the slider moved callbacks.  */

   if (vw -> valuator.orientation == XwVERTICAL &&
       vw -> valuator.origin > vw -> valuator.min)
   {
      vw -> valuator.origin--;
      RedrawSlider (vw);
      XtCallCallbacks (vw, XtNsliderMoved, (caddr_t) vw -> valuator.origin);
   }
}




/************************************************************************
 *
 *  Down
 *     This function processes down keyboard actions to handle
 *     keyboard input for the valuator.
 *
 ************************************************************************/

static void Down (vw, event)
XwValuatorWidget vw;
XEvent * event;

{
   /*  If the slider origin can be increased, increment it,     */
   /*  redraw the slider, and call the slider moved callbacks.  */

   if (vw -> valuator.orientation == XwVERTICAL &&
       vw -> valuator.origin < vw -> valuator.max - vw -> valuator.extent)
   {
      vw -> valuator.origin++;
      RedrawSlider (vw);
      XtCallCallbacks (vw, XtNsliderMoved, (caddr_t) vw -> valuator.origin);
   }
}




/************************************************************************
 *
 *  RedrawSlider
 *	Clear, calculate, and redraw the slider.
 *
 ************************************************************************/

static void RedrawSlider (vw)
XwValuatorWidget vw;

{
   if (XtIsRealized (vw))
   {
      if (vw -> valuator.pixmap != 0)
         XClearArea (XtDisplay ((Widget) vw), XtWindow ((Widget) vw),
                     vw -> valuator.sliderX, vw -> valuator.sliderY,
                     vw -> valuator.sliderWidth, vw -> valuator.sliderHeight,
                     False);

      CalcSliderRect (vw, &(vw -> valuator.sliderX), 
                          &(vw -> valuator.sliderY),
                          &(vw -> valuator.sliderWidth),
                          &(vw -> valuator.sliderHeight));

      if (vw -> valuator.pixmap != 0)
         XCopyArea (XtDisplay ((Widget) vw),
                    vw -> valuator.pixmap, XtWindow ((Widget) vw),
                    vw -> valuator.slider_GC,
                    0, 0, vw->valuator.sliderWidth, 
                          vw->valuator.sliderHeight,
                    vw->valuator.sliderX, vw->valuator.sliderY);
   }
}
