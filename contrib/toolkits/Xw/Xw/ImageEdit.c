/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        ImageEdit.c
 **
 **   Project:     X Widgets
 **
 **   Description: Contains code for ImageEdit widget class.
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
#include <X11/Xutil.h>
#include <Xw/Xw.h>
#include <Xw/XwP.h>
#include <Xw/ImageEditP.h>
#include <Xw/ImageEdit.h>
#include <X11/StringDefs.h>
#include <X11/keysymdef.h>   


/*  Event procedures referenced in the actions list  */

static void Select();
static void Release();
static void Moved();


/*  Default translation table and action list  */

static char defaultTranslations[] =
    "<Btn1Down>:        select()\n\
     <Btn1Up>:          release()\n\
     Button1<PtrMoved>: moved()\n\
     <EnterWindow>:     enter()\n\
     <LeaveWindow>:     leave()";

static XtActionsRec actionsList[] =
{
  { "select",   (XtActionProc) Select               },
  { "release",  (XtActionProc) Release              },
  { "moved",    (XtActionProc) Moved                },
  { "enter",    (XtActionProc) _XwPrimitiveEnter    },
  { "leave",    (XtActionProc) _XwPrimitiveLeave    },
};


/*  Resource list for ImageEdit  */

static XtResource resources[] = 
{
   {
      XtNimage, XtCImage, XtRPointer, sizeof (caddr_t),
      XtOffset(XwImageEditWidget, imageEdit.image), XtRPointer, NULL
   },

   {
      XtNpixelScale, XtCPixelScale, XtRInt, sizeof (int),
      XtOffset(XwImageEditWidget, imageEdit.pixel_scale), XtRString, "6"
   },

   {
      XtNgridThickness, XtCGridThickness, XtRInt, sizeof (int),
      XtOffset(XwImageEditWidget, imageEdit.grid_thickness), XtRString, "1"
   },

   {
      XtNdrawColor, XtCBackground, XtRPixel, sizeof (Pixel),
      XtOffset(XwImageEditWidget, imageEdit.draw_color), XtRString, "Black"
   },

   {
      XtNeraseColor, XtCBackground, XtRPixel, sizeof (Pixel),
      XtOffset(XwImageEditWidget, imageEdit.erase_color), XtRString, "White"
   },

   {
      XtNeraseOn, XtCEraseOn, XtRBoolean, sizeof (Boolean),
      XtOffset(XwImageEditWidget, imageEdit.erase_on), XtRString, "True"
   },

   {
      XtNbackground, XtCBackground, XtRPixel, sizeof (Pixel),
      XtOffset(XwImageEditWidget, core.background_pixel), XtRString, "Black"
   }
};


/*  Static routine definitions  */

static void   Realize();
static void      Initialize();
static void    Destroy();
static void    Resize();
static void    Redisplay();
static Boolean SetValues();

static void GetImageEditGC();
static void GetImageEditEraseGC();

static void XwDrawPts();


/*  The ImageEdit class record definition  */

XwImageEditClassRec XwimageEditClassRec =
{
   {
      (WidgetClass) &XwprimitiveClassRec, /* superclass	         */	
      "ImageEdit",                      /* class_name	         */	
      sizeof(XwImageEditRec),           /* widget_size	         */	
      NULL,                             /* class_initialize      */    
      NULL,                             /* class_part_initialize */
      FALSE,                            /* class_inited          */	
      (XtInitProc) Initialize,          /* initialize	         */	
      NULL,                             /* initialize_hook       */
      (XtRealizeProc) Realize,          /* realize	         */	
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
      (XtWidgetProc) Resize,            /* resize                */	
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

WidgetClass XwimageEditWidgetClass = (WidgetClass) &XwimageEditClassRec;




/************************************************************************
 *
 *  Initialize
 *     The main widget instance initialization routine.
 *
 ************************************************************************/

static void Initialize (request, new)
XwImageEditWidget request, new;

{

   /*
    *  Check the data put into the new widget from .Xdefaults
    *  or through the arg list.
    */

   if (new -> imageEdit.pixel_scale < 1)
   {
      XtWarning ("ImageEdit: The imageEdit pixel scale is to small.");
      new -> imageEdit.pixel_scale = 6;
   }

   if (new -> imageEdit.grid_thickness < 0)
   {
      XtWarning ("ImageEdit: The imageEdit grid thickness is to small.");
      new -> imageEdit.grid_thickness = 1;
   }

   if (new -> imageEdit.image == NULL)
      XtWarning ("ImageEdit: No XImage provided");


   /*  Set up a geometry for the widget if it is currently 0.  */

   if (request -> core.width == 0 && new -> imageEdit.image != NULL)
      new -> core.width += new -> imageEdit.image -> width *
         (new -> imageEdit.pixel_scale + new -> imageEdit.grid_thickness) +
            new -> imageEdit.grid_thickness;

   if (request -> core.height == 0 && new -> imageEdit.image != NULL)
      new -> core.height += new -> imageEdit.image -> height *
         (new -> imageEdit.pixel_scale + new -> imageEdit.grid_thickness) +
            new -> imageEdit.grid_thickness;

   new -> imageEdit.old_width =  new -> core.width;
   new -> imageEdit.old_height = new -> core.height;
}




/************************************************************************
 *
 *  GetImageEditGC
 *     Get the graphics context used for drawing the imageEdit.
 *
 ************************************************************************/

static void GetImageEditGC (iw)
XwImageEditWidget iw;

{
   XRectangle rect[1];
   XGCValues values;
   XtGCMask  valueMask;


   valueMask = GCForeground | GCBackground | GCFillStyle;

   values.foreground = iw -> imageEdit.draw_color;
   values.background = iw -> core.background_pixel;
   values.fill_style = FillSolid;

   iw -> imageEdit.imageEdit_GC = 
      XCreateGC (XtDisplay (iw), XtWindow (iw), valueMask, &values);

   rect[0].x = 0;
   rect[0].y = 0;
   rect[0].width = iw->core.width - 2 * iw->primitive.highlight_thickness;
   rect[0].height = iw->core.height - 2 * iw->primitive.highlight_thickness;

   XSetClipRectangles (XtDisplay (iw), iw -> imageEdit.imageEdit_GC,
                       iw -> primitive.highlight_thickness,
                       iw -> primitive.highlight_thickness,
                       rect, 1, Unsorted);
}




/************************************************************************
 *
 *  GetImageEditEraseGC
 *     Get the graphics context used for drawing the imageEdit.
 *
 ************************************************************************/

static void GetImageEditEraseGC (iw)
XwImageEditWidget iw;

{
   XGCValues values;
   XtGCMask  valueMask;
   XRectangle rect[1];

   valueMask = GCForeground | GCBackground | GCFillStyle;

   values.foreground = iw -> imageEdit.erase_color;
   values.background = iw -> core.background_pixel;
   values.fill_style = FillSolid;

   iw -> imageEdit.imageEditErase_GC =
      XCreateGC (XtDisplay (iw), XtWindow (iw), valueMask, &values);

   rect[0].x = 0;
   rect[0].y = 0;
   rect[0].width = iw->core.width - 2 * iw->primitive.highlight_thickness;
   rect[0].height = iw->core.height - 2 * iw->primitive.highlight_thickness;

   XSetClipRectangles (XtDisplay (iw), iw -> imageEdit.imageEditErase_GC,
                       iw -> primitive.highlight_thickness,
                       iw -> primitive.highlight_thickness,
                       rect, 1, Unsorted);
}




/************************************************************************
 *
 *  Resize
 *     Recompute the clipping rectangles for the graphics contexts.
 *
 ************************************************************************/

static void Resize (iw)
XwImageEditWidget iw;

{
   int border;
   XRectangle rect[1];

   if (XtIsRealized (iw))
   {

      /*  Clear up any left over border drawing and reset the  */
      /*  saved width and height.                              */

      border = iw -> primitive.highlight_thickness;

      if (iw -> imageEdit.old_width < iw -> core.width)
         XClearArea (XtDisplay (iw), XtWindow (iw),
                     iw -> imageEdit.old_width - border, 0, 
                     border, iw -> imageEdit.old_height, True);

      if (iw -> imageEdit.old_height < iw -> core.height)
         XClearArea (XtDisplay (iw), XtWindow (iw),
                     0, iw -> imageEdit.old_height - border,
                     iw -> imageEdit.old_width, border, True);

      iw -> imageEdit.old_width = iw -> core.width;
      iw -> imageEdit.old_height = iw -> core.height;


      /*  Modify the drawing GC's to the correct clipping rectangles.  */

      rect[0].x = 0;
      rect[0].y = 0;
      rect[0].width = iw->core.width-2 * iw->primitive.highlight_thickness;
      rect[0].height = iw->core.height-2 * iw->primitive.highlight_thickness;

      XSetClipRectangles (XtDisplay (iw), iw -> imageEdit.imageEdit_GC,
                          iw -> primitive.highlight_thickness,
                          iw -> primitive.highlight_thickness,
                          rect, 1, Unsorted);

      XSetClipRectangles (XtDisplay (iw), iw -> imageEdit.imageEditErase_GC,
                          iw -> primitive.highlight_thickness,
                          iw -> primitive.highlight_thickness,
                          rect, 1, Unsorted);
   }
}




/************************************************************************
 *
 *  Redisplay
 *     General redisplay function called on exposure events.
 *
 ************************************************************************/

static void Redisplay (iw, event, region)
XwImageEditWidget iw;
XExposeEvent * event;
Region region;

{
   Display * display = XtDisplay (iw);
   Drawable window = XtWindow (iw);
   XImage * image = iw -> imageEdit.image;
   int pixelScale = iw -> imageEdit.pixel_scale;
   int gridThickness = iw -> imageEdit.grid_thickness;
   Pixel foreground = iw -> primitive.foreground;
   Pixel background = iw -> core.background_pixel;
   GC drawGC;
   XRectangle rect[1];
   unsigned long valueMask;
   XGCValues values;
   register int x;
   register int y;
   register int xPos;
   register int yPos;
   unsigned long pixel;


   /*  Draw the border highlight  */

   if (iw -> primitive.highlighted)
      _XwHighlightBorder (iw);
   else if (iw -> primitive.display_highlighted)
      _XwUnhighlightBorder (iw);


   if (iw -> imageEdit.image == NULL) 
	return;


   /*  Set up the redisplay GC and set its clipping rectangle properly.  */

   values.foreground = foreground;
   values.background = background;
   values.fill_style = FillSolid;
   valueMask = GCForeground | GCBackground | GCFillStyle;
   drawGC = XCreateGC (display, window, valueMask, &values);
   valueMask = GCForeground;

   rect[0].x = 0;
   rect[0].y = 0;
   rect[0].width = iw->core.width - 2 * iw -> primitive.highlight_thickness;
   rect[0].height = iw->core.height - 2 * iw -> primitive.highlight_thickness;

   XSetClipRectangles (XtDisplay (iw), drawGC,
                       iw -> primitive.highlight_thickness,
                       iw -> primitive.highlight_thickness,
                       rect, 1, Unsorted);

   XClipBox (region, rect);

#define SCALE_WI(v) \
   (((v) - gridThickness - iw->primitive.highlight_thickness) / \
    (gridThickness + pixelScale))

#define SCALE_IW(v) \
   ((v) * (gridThickness + pixelScale) + \
    (gridThickness + iw->primitive.highlight_thickness))

   {
       int xwMin = rect[0].x - 1;
       int xwMax = rect[0].x + rect[0].width + 1;
       int ywMin = rect[0].y - 1;
       int ywMax = rect[0].y + rect[0].height + 1;

       int xiMin = SCALE_WI(xwMin) - 1;
       int xiMax = SCALE_WI(xwMax) + 1;
       int yiMin = SCALE_WI(ywMin) - 1;
       int yiMax = SCALE_WI(ywMax) + 1;

       if (xiMin < 0) xiMin = 0;
       if (yiMin < 0) yiMin = 0;
       if (xiMax > image->width)  xiMax = image->width;
       if (yiMax > image->height) yiMax = image->height;

       yPos = SCALE_IW(yiMin);

       for (y = yiMin; y < yiMax; y++) {
	   xPos = SCALE_IW(xiMin);

	   for (x = xiMin; x < xiMax; x++) {
         pixel = XGetPixel (image, x, y);

	       if (pixel != background) {
		   if (pixel != foreground) {
               foreground = pixel;
               values.foreground = foreground;
   	       XChangeGC (display, drawGC, valueMask, &values);
            }

            if (XRectInRegion (region, xPos, yPos, 
                               pixelScale, pixelScale) != RectangleOut)
               XFillRectangle (display, window, drawGC,
                               xPos, yPos, pixelScale, pixelScale);

         }

         xPos += gridThickness + pixelScale;
      }

      yPos += gridThickness + pixelScale;
   }
   }

   XFreeGC (display, drawGC);
}




/************************************************************************
 *
 *  Realize
 *	Create the widget window and create the gc's.
 *
 ************************************************************************/

static void Realize (iw, valueMask, attributes)
XwImageEditWidget      iw;
XtValueMask          * valueMask;
XSetWindowAttributes * attributes;

{
   Mask newValueMask = *valueMask;
   newValueMask |=  CWBitGravity;
   attributes -> bit_gravity = NorthWestGravity;

   if (iw->primitive.cursor != None) newValueMask |= CWCursor;
   attributes->cursor = iw->primitive.cursor;

   XtCreateWindow (iw, InputOutput, (Visual *) CopyFromParent,
		   newValueMask, attributes);
   GetImageEditGC (iw);
   GetImageEditEraseGC (iw);
   _XwRegisterName(iw);
}




/************************************************************************
 *
 *  Destroy
 *	Clean up allocated resources when the widget is destroyed.
 *
 ************************************************************************/

static void Destroy (iw)
XwImageEditWidget iw;

{
   if (XtIsRealized (iw))
   {
      XFreeGC (XtDisplay (iw), iw -> imageEdit.imageEdit_GC);
      XFreeGC (XtDisplay (iw), iw -> imageEdit.imageEditErase_GC);
   }
}




/************************************************************************
 *
 *  SetValues
 *
 ************************************************************************/

static Boolean SetValues (current, request, new)
XwImageEditWidget current, request, new;

{
   Boolean returnFlag = FALSE;
    
   if (new -> imageEdit.pixel_scale < 1)
   {
      XtWarning ("ImageEdit: The imageEdit pixel scale is to small.");
      new -> imageEdit.pixel_scale = current -> imageEdit.pixel_scale;
   }

   if (new -> imageEdit.grid_thickness < 0)
   {
      XtWarning ("ImageEdit: The imageEdit grid thickness is to small.");
      new -> imageEdit.grid_thickness = current -> imageEdit.grid_thickness;
   }

   if (new -> imageEdit.image == NULL)
      XtWarning ("ImageEdit: No XImage provided.");


   /*  See if the GC's need to be regenerated and widget redrawn.  */

   if (new -> imageEdit.pixel_scale != current-> imageEdit.pixel_scale      ||
       new->imageEdit.grid_thickness != current->imageEdit.grid_thickness   ||
       new->primitive.highlight_thickness !=
          current->primitive.highlight_thickness                            ||
       new -> core.background_pixel != current -> core.background_pixel     ||
       new -> imageEdit.draw_color != current -> imageEdit.draw_color       ||
       new -> imageEdit.erase_color != current -> imageEdit.erase_color)
   {
      if (XtIsRealized (new))
      {
         returnFlag = TRUE;
         XFreeGC (XtDisplay (new), new -> imageEdit.imageEdit_GC);
         XFreeGC (XtDisplay (new), new -> imageEdit.imageEditErase_GC);
         GetImageEditGC (new);
         GetImageEditEraseGC (new);
      }
   }


   /*  Set up a geometry for the widget if 0 was requested.  */

   if (new -> primitive.recompute_size)
   {
      if (new -> imageEdit.image != NULL)
      {
         new -> core.width = new -> primitive.highlight_thickness * 2 +
            new -> imageEdit.image -> width *
            (new -> imageEdit.pixel_scale + new -> imageEdit.grid_thickness) +
             new -> imageEdit.grid_thickness;

         new -> core.height =  new -> primitive.highlight_thickness * 2 +
            new -> imageEdit.image -> height *
            (new -> imageEdit.pixel_scale + new -> imageEdit.grid_thickness) +
             new -> imageEdit.grid_thickness;
      }
   }

   return (returnFlag);
}




/************************************************************************
 *
 *  Select
 *     This function processes selections occuring on the imageEdit.
 *
 ************************************************************************/

static void Select (iw, buttonEvent)
XwImageEditWidget iw;
XButtonPressedEvent * buttonEvent;

{
   XwDrawPts (iw, buttonEvent -> x, buttonEvent -> y, False);
   XtCallCallbacks (iw, XtNselect, NULL);
}




/************************************************************************
 *
 *  Release
 *     This function processes releases occuring on the imageEdit.
 *
 ************************************************************************/

static void Release (iw, buttonEvent)
XwImageEditWidget iw;
XButtonReleasedEvent * buttonEvent;

{
   XtCallCallbacks (iw, XtNrelease, NULL);
}




/************************************************************************
 *
 *  Moved
 *     This function processes mouse moved events during interactive
 *     slider moves.
 *
 ************************************************************************/

static void Moved (iw, event)
XwImageEditWidget iw;
XButtonPressedEvent * event;

{
   XwDrawPts (iw, event -> x, event -> y, True);
}




/************************************************************************
 *
 *  XwDrawPts
 *     Calculate the points to be drawn and render them in both
 *     the widget and image.
 *
 ************************************************************************/

static void XwDrawPts (iw, x1, y1, usedPts)
XwImageEditWidget iw;
register int x1;
register int y1;
Boolean usedPts;

{
   register int x2;
   register int y2;
   int pixelScale = iw -> imageEdit.pixel_scale;
   int gridThickness = iw -> imageEdit.grid_thickness;
   GC drawGC;
   Pixel drawPixel;
   int imageWidth;
   int imageHeight;
   XImage * image = iw -> imageEdit.image;
   int highlightThickness = iw -> primitive.highlight_thickness;
   Display * display = XtDisplay (iw);
   Drawable window = XtWindow (iw);
   register int d;
   register int count;
   int xincr1, yincr1;
   int xincr2, yincr2;
   int a, b;
   int xPtCount;
   int yPtCount;

   if (iw -> imageEdit.image == NULL) return;

   imageWidth = iw -> imageEdit.image -> width;
   imageHeight = iw -> imageEdit.image -> height;

   /*  Transform the input points  */

   x1 = (x1 - highlightThickness) / (gridThickness + pixelScale);
   y1 = (y1 - highlightThickness) / (gridThickness + pixelScale);

   if (x1 >= imageWidth) x1 = imageWidth - 1;
   else if (x1 < 0) x1 = 0;
   if (y1 >= imageHeight) y1 = imageHeight - 1;
   else if (y1 < 0) y1 = 0;


   if (!usedPts)
   {
      iw -> imageEdit.pt_x = x2 = x1;
      iw -> imageEdit.pt_y = y2 = y1;


      /*  Set the drawing GC dependent on the selected pixel and the  */
      /*  erase_on setting.                                           */

      if (iw -> imageEdit.erase_on && 
           XGetPixel (image, x1, y1) == iw -> imageEdit.draw_color)
      {
         iw -> imageEdit.draw_GC = iw -> imageEdit.imageEditErase_GC;
         iw -> imageEdit.draw_Pixel = iw -> imageEdit.erase_color;

      }
      else
      {
         iw -> imageEdit.draw_GC = iw -> imageEdit.imageEdit_GC;
         iw -> imageEdit.draw_Pixel = iw -> imageEdit.draw_color;
      }
   }
   else
   {
      x2 = iw -> imageEdit.pt_x;
      y2 = iw -> imageEdit.pt_y;

      if (x1 == x2 && y1 == y2) return;

      iw -> imageEdit.pt_x = x1;
      iw -> imageEdit.pt_y = y1;
   }


   drawGC = iw -> imageEdit.draw_GC;
   drawPixel = iw -> imageEdit.draw_Pixel;


   /* Draw the first point */

   XPutPixel(image, x1, y1, drawPixel);
   XFillRectangle (display, window, drawGC, 
                   x1 * (pixelScale + gridThickness) + 
                         gridThickness + highlightThickness,
                   y1 * (pixelScale + gridThickness) +
                         gridThickness + highlightThickness,
                   pixelScale, pixelScale);


   /* Stop, if this is a single point line */

   if (x1 == x2 && y1 == y2) return;


   /*  Initialize the incrementing values used to generate  */
   /*  the points within the segment.                       */

   xPtCount = abs(x2 - x1);
   yPtCount = abs(y2 - y1);

   if (x2 > x1)
   {
      if (y2 > y1)
	 xincr2 = yincr2 = 1;
      else
      {
	 xincr2 = 1;
	 yincr2 = -1;
      }
   }
   else
   {
      if (y2 > y1)
      {
	 xincr2 = -1;
	 yincr2 = 1;
      }
      else
	 xincr2 = yincr2 = -1;
   }

   if (xPtCount > yPtCount)
   {
      count = xPtCount;
      xincr1 = xincr2;
      yincr1 = 0;
      b = yPtCount << 1;
      a = (xPtCount << 1) - b;
      d = b - xPtCount;
   }
   else
   {
      count = yPtCount;
      xincr1 = 0;
      yincr1 = yincr2;
      b = xPtCount << 1;
      a = (yPtCount << 1) - b;
      d = b - yPtCount;
   }

   do
   {
      if (d > 0)
      {
	 d -= a;
         x1 += xincr2;
         y1 += yincr2;
      }
      else
      {
	 d += b;
         x1 += xincr1;
         y1 += yincr1;
      }

      /* Set the specified pixel, and draw it */

      XPutPixel(image, x1, y1, drawPixel);
      XFillRectangle (display, window, drawGC, 
                      x1 * (pixelScale + gridThickness) + 
                            gridThickness + highlightThickness,
                      y1 * (pixelScale + gridThickness) +
                            gridThickness + highlightThickness,
                      pixelScale, pixelScale);
   }
   while (--count);
}

