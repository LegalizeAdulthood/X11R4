/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        SRaster.c
 **
 **   Project:     X Widgets (Part II)
 **
 **   Description: Code/Definitions for Static Raster Widget
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
 *****************************************************************************/


/*
 * Include files & Static Routine Definitions
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <X11/Xlib.h>
#include <X11/StringDefs.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>

#include <Xw/Xw.h>
#include <Xw/XwP.h>
#include <Xw/SRaster.h>
#include <Xw/SRasterP.h>


static void    ClassInitialize();
static Boolean SetValues();
static void SRasterDestroy();

static void Select();
static void Release();

extern void BuildInvGC();
extern void BuildNormGC();

/****************
 *
 * Translation Tables.
 *
 ****************/

static char defaultTranslations[] = 
    "<EnterWindow>:     enter()\n\
     <LeaveWindow>:     leave()\n\
     <Btn1Down>:        select()\n\
     <Btn1Up>:          release()\n\
     <KeyDown>Select:   select() \n\
     <KeyUp>Select:     release()";

static XtActionsRec actionsList[] =
{
  { "enter",   (XtActionProc) _XwPrimitiveEnter   },
  { "leave",   (XtActionProc) _XwPrimitiveLeave   },
  { "select",  (XtActionProc) Select  },
  { "release", (XtActionProc) Release },

};



/*************************************<->*************************************
 *
 *   Resource List.
 *
 *   Provides default resource settings for instances of this class.
 *   To get full set of default settings, examine resouce list of super
 *   classes of this class.
 *
 *
 *************************************<->***********************************/

static XtResource resources[] =
{

   {
     XtNsRimage, XtCSRimage, XtRImage, sizeof(XImage *),
     XtOffset (XwSRasterWidget, sraster.image),
     XtRImage, NULL
   },
   {
     XtNinvertOnSelect, XtCInvertOnSelect, XtRBoolean, sizeof(Boolean),
     XtOffset (XwSRasterWidget, sraster.invert),
     XtRString,"TRUE"
   },
   {
     XtNshowSelected, XtCShowSelected, XtRBoolean, sizeof(Boolean),
     XtOffset (XwSRasterWidget, sraster.showme),
     XtRString,"TRUE"
   },
   {
     XtNset, XtCSet, XtRBoolean, sizeof(Boolean),
     XtOffset (XwSRasterWidget, sraster.poked),
     XtRString,"FALSE"
   }
};



/*************************************<->*************************************
 *
 *   StaticRaster Class Structure
 *
 *************************************<->***********************************/

SRasterClassRec srasterClassRec =
{
   {
      (WidgetClass) &XwprimitiveClassRec,    /* superclass	     */	
      "StaticRaster",                   /* class_name	     */	
      sizeof(SRasterRec),               /* widget_size	     */	
      ClassInitialize,                  /* class_initialize  */    
      NULL,		                /* class_part_initialize  */    
      FALSE,                            /* class_inited      */	
      InitSRaster,                      /* initialize	     */	
      NULL,                             /* initialize hook   */
      Realize,                          /* realize	     */	
      actionsList,                      /* actions           */	
      XtNumber(actionsList),            /* num_actions	     */	
      resources,                        /* resources	     */	
      XtNumber(resources),              /* num_resources     */	
      NULLQUARK,                        /* xrm_class	     */	
      TRUE,                             /* compress_motion   */	
      TRUE,                             /* compress_exposure */
      TRUE,                             /* compress_enterleave */
      FALSE,                            /* visible_interest  */	
      SRasterDestroy,                   /* destroy           */	
      Resize,                           /* resize            */	
      Resize,                           /* expose            */	
      SetValues,                        /* set_values	         */	
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
   },

   {
       0,         /* Static Raster class part - unused */
   },
};

WidgetClass XwsrasterWidgetClass = (WidgetClass) &srasterClassRec;
WidgetClass XwstaticRasterWidgetClass = (WidgetClass) &srasterClassRec;



/************************************************************************
 *
 *  ClassInitialize
 *
 *   Description:
 *   -----------
 *    Set fields in primitive class part of our class record so that
 *    the traversal code can invoke our select/release procedures (note
 *    that for this class toggle is a empty proc).
 *
 *************************************<->***********************************/
static void ClassInitialize()
{
   srasterClassRec.primitive_class.select_proc = (XtWidgetProc) Select;
   srasterClassRec.primitive_class.release_proc = (XtWidgetProc) Release;
   srasterClassRec.primitive_class.toggle_proc = (XtWidgetProc) NULL;
}


/*************************************<->*************************************
 *
 *  InitSRaster
 *
 *   Description:
 *   -----------
 *     This is the static raster instance initialize procedure. 
 *     It sets up the appropriate graphic contexts and looks for a 0,0
 *     size. If the core size is 0,0, the dimensions are reset to the
 *     image height and width.
 *
 *   Inputs:
 *   ------
 *     request  = original instance record;
 *
 *     new      = instance record with modifications induced by
 *                other initialize routines, changes are made to this
 *                record;
 *
 * 
 *   Outputs:
 *   -------
 *
 *   Procedures Called
 *   -----------------
 *
 *************************************<->***********************************/

static void InitSRaster(request, new)
 XwSRasterWidget request, new;
{

    if (request->core.width == 0)
	if (request->sraster.image != NULL)
	    new->core.width += request->sraster.image->width;
	else
	    new->core.width += 10;

    if (request->core.height == 0 )
        if (request->sraster.image != NULL)
	    new->core.height += request->sraster.image->height;
	else
	    new->core.height += 10;
    BuildInvGC(new);
    BuildNormGC(new);
}


/*************************************<->*************************************
 *
 *  Realize
 *
 *   Description:
 *   -----------
 *     Creates the window for this static raster instance.  Sets bit gravity
 *     so that on resize static raster is repainted.
 *
 *   Inputs:
 *   ------
 *     w  =  widget to be realized.
 *
 *     valueMask = contains event mask for this window/widget.
 *
 *     attributes = window attributes for this window/widget.
 * 
 *   Outputs:
 *   -------
 *
 *   Procedures Called
 *   -----------------
 *   XCreateWindow()
 *************************************<->***********************************/

static void Realize(w, p_valueMask, attributes)
    register Widget w;
    Mask *p_valueMask;
    XSetWindowAttributes *attributes;
{

    Mask valueMask = *p_valueMask;

    valueMask |= CWBitGravity;
    attributes->bit_gravity = ForgetGravity;

    XtCreateWindow(w,InputOutput, (Visual *)CopyFromParent,
		   valueMask, attributes);
}


/****************************************************************
 *
 *  ShowSR
 *
 *   Description:
 *   -----------
 *     Redraw the static raster image. If the window we are trying to fit in
 *     is too small, the image is clipped. If it's too big, the image is
 *     centered within the window.
 *
 *   Inputs:
 *   ------
 *     w  =  widget to be drawn.
 *     event = XEvent structure
 *     gc = GC to use
 ****************************************************************/
static void ShowSR(w, event,gc)
    register XwSRasterWidget w;
    XEvent *event;
    GC	   gc;
{
    int HOffset,WOffset;
    int VisHeight,VisWidth;
    register ImageW,ImageH;

    if ((w->sraster.image == NULL) || (w->sraster.image->data == NULL))
    {
	XtWarning("StaticRaster: NULL Image or bitmap pointer");
	return;
    }
    VisWidth  = w->sraster.image->width;
    VisHeight = w->sraster.image->height;
    HOffset = WOffset = (int) w->primitive.highlight_thickness;
    ImageW = VisWidth + (WOffset << 1);
    ImageH = VisHeight + (HOffset << 1);

/****************
 *
 * If the width and height aren't exact matches, then clip or center as needed.
 *
 ****************/
    
    if (ImageW != w->core.width)
    {
	if (ImageW < w->core.width)             /* Too small - center it */
	    WOffset += (w->core.width - ImageW) >> 1;
	else                                    /* Clip it */
	    VisWidth = w->core.width - (WOffset << 1);
    }
    if (ImageH != w->core.height)
    {
	if (ImageH < w->core.height)             /* Too small - center it */
	    HOffset += (w->core.height - ImageH) >> 1;
	else                                    /* Clip it */
	    VisHeight = w->core.height - (HOffset << 1);
    }
    
    if (XtIsRealized(w)) {
    XPutImage(XtDisplay(w),XtWindow(w),gc,w->sraster.image,
	      0,0, WOffset, HOffset,VisWidth,VisHeight);
  /*
   * We don't want to lose the highlight on redisplay
   * do we?
   */

    if (w->primitive.highlighted)
      {
	 _XwHighlightBorder(w);
	 w->primitive.display_highlighted = TRUE;
      }
    else
      if (w->primitive.display_highlighted)
	 {
	    _XwUnhighlightBorder(w);
	    w->primitive.display_highlighted = FALSE;
	 }
    }
}


/*************************************<->*************************************
 *
 *  SRasterDestroy
 *
 *   Description:
 *   -----------
 *     Free up the GC's and stuff on destroy.
 *
 *************************************<->***********************************/
static void SRasterDestroy (srw)
  XwSRasterWidget srw;
{
   XtDestroyGC (srw->sraster.NormalGC);
   XtDestroyGC (srw->sraster.InverseGC);
}


/****************************************************************
 *
 *  Event Routines.
 *
 *       Select - Call the callback when the left button goes down.
 *
 *       Release - Call the callback when the left button goes up.
 *
 ****************************************************************/


static void Select(w,event)
XwSRasterWidget w;
XEvent *event;
{
    w->sraster.poked = TRUE;
    if (w->sraster.invert)
	ShowSR(w,NULL,w->sraster.InverseGC);
	
    XtCallCallbacks(w,XtNselect,event);
}

static void Release(w,event)
XwSRasterWidget w;
XEvent *event;
{
    w->sraster.poked = FALSE;
    if (w->sraster.invert)
	ShowSR(w,NULL,w->sraster.NormalGC);

    XtCallCallbacks(w,XtNrelease,event);
}


/*************************************<->*************************************
 *
 *  SetValues(current, request, new, last)
 *
 *   Description:
 *   -----------
 *     This is the set values procedure for the static raster class.
 *
 *
 *   Inputs:
 *   ------
 *    current = original widget;
 *    request = copy of current (?);
 *    new = copy of request which reflects changes made to it by
 *          set values procedures of its superclasses;
 *    last = TRUE if this is the last set values procedure to be called.
 * 
 *   Outputs:
 *   -------
 *
 *   Procedures Called
 *   -----------------
 *     XwGeomCheck;
 *************************************<->***********************************/

static Boolean SetValues(current, request, new)
    Widget current, request, new;
{
    XwSRasterWidget cw = (XwSRasterWidget) current;
    XwSRasterWidget nw = (XwSRasterWidget) new;
    Boolean  flag = FALSE;    /* our return value */
    int ht; 

        ht = 0;

/* First call the library routine to check geometry deltas */
/*    flag = XwGeomCheck(cw,nw,last);*/
    
    if (cw->primitive.foreground != nw->primitive.foreground ||
	cw->core.background_pixel != nw->core.background_pixel)
	    flag = TRUE;

    if ((cw->sraster.image != nw->sraster.image ||
	cw->sraster.image->data != nw->sraster.image->data) &&
	nw->primitive.recompute_size)
    {
	    nw->core.width = nw->sraster.image->width + ht;
	    nw->core.height = nw->sraster.image->height + ht;
	    flag = TRUE;
    }
    if (cw->sraster.invert != nw->sraster.invert)
	    flag = TRUE;
    
    return( flag );
}


/*************************************<->*************************************
 *
 *  Resize
 *
 *   Description:
 *   -----------
 *   Inputs:
 *   ------
 *     w  = widget to be resized.
 * 
 *   Outputs:
 *   -------
 *
 *   Procedures Called
 *   -----------------
 *
 *************************************<->***********************************/


static void Resize(w)
    XwSRasterWidget w;
{
    ShowSR(w,NULL,w->sraster.NormalGC);
}


/*************************************<->*************************************
 *
 *  BuildNormGC(srw)
 *    XwSRasterWidget  srw;
 *
 *   Description:
 *   -----------
 *      Uses the widget specific foreground to generate the "normal"
 *      graphic context.  Note that this is a XToolkit sharable GC.
 *      Creates the needed GC and sets ptr. in instance record.
 *
 *   Inputs:
 *   ------
 *     srw = widget instance.
 * 
 *   Outputs:
 *   -------
 *
 *   Procedures Called
 *   -----------------
 *   XtGetGC
 *************************************<->***********************************/

static void BuildNormGC(srw)
    XwSRasterWidget srw;
{
    XGCValues	values;

    values.foreground	= srw->primitive.foreground;
    values.background   = srw->core.background_pixel;
    values.line_width   = 1;
    

    srw->sraster.NormalGC = XtGetGC((Widget)srw,
     	                                  (unsigned) GCForeground |
					  (unsigned) GCBackground,
       	                                    &values);
}

/*************************************<->*************************************
 *
 *  void BuildInvGC(srw)
 *          XwSRasterWidget  srw;
 *
 *   Description:
 *   -----------
 *
 *   Inputs:
 *   ------
 *     srw = widget instance.
 * 
 *   Outputs:
 *   -------
 *
 *   Procedures Called
 *   -----------------
 *   XwGetGC
 *************************************<->***********************************/

static void BuildInvGC(srw)
    XwSRasterWidget srw;
{
    XGCValues	values;
    values.foreground	= srw->core.background_pixel;
    values.background   = srw->primitive.foreground;

    srw->sraster.InverseGC = XtGetGC((Widget)srw,
     	                                  (unsigned) GCForeground |
					  (unsigned) GCBackground,
       	                                    &values);
}
