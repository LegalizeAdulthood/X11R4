/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        Sash.c
 **
 **   Project:     X Widgets
 **
 **   Description: Sash widget used by VPaned window widget
 **
 *****************************************************************************
 **   
 **   Copyright (c) 1988 by Hewlett-Packard Company
 **   Copyright (c) 1987, 1988 by Digital Equipment Corporation, Maynard,
 **             Massachusetts, and the Massachusetts Institute of Technology,
 **             Cambridge, Massachusetts
 **   
 **   Permission to use, copy, modify, and distribute this software 
 **   and its documentation for any purpose and without fee is hereby 
 **   granted, provided that the above copyright notice appear in all 
 **   copies and that both that copyright notice and this permission 
 **   notice appear in supporting documentation, and that the names of 
 **   Hewlett-Packard, Digital or  M.I.T.  not be used in advertising or 
 **   publicity pertaining to distribution of the software without 
 **   written prior permission.
 **   
 **   DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 **   ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 **   DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 **   ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 **   WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 **   ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 **   SOFTWARE.
 **   
 *****************************************************************************
 *************************************<+>*************************************/

#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/cursorfont.h>
#include <Xw/XwP.h>
#include <Xw/Xw.h>
#include <Xw/SashP.h>

static void Redisplay();
static void ClassInitialize();
static void Realize();
static int defDim = 12;
static int defBorder = 0;


static char defTranslations[] =
    "<Btn1Down>:		SashAction(Start, UpperPane) \n\
     <Btn2Down>:		SashAction(Start, ThisBorderOnly) \n\
     <Btn3Down>:		SashAction(Start, LowerPane) \n\
     <Btn1Motion>:		SashAction(Move, Upper) \n\
     <Btn2Motion>:		SashAction(Move, ThisBorder) \n\
     <Btn3Motion>:		SashAction(Move, Lower) \n\
     Any<BtnUp>:		SashAction(Commit) \n\
     <EnterWindow>:          enter() \n\
     <LeaveWindow>:          leave()";



static XtResource resources[] = {
   {XtNhighlightStyle, XtCHighlightStyle, XtRHighlightStyle,
      sizeof(int), XtOffset(XwSashWidget, primitive.highlight_style),
      XtRString, "widget_defined"},
   {XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
      XtOffset(XwSashWidget, core.width), XtRInt, (caddr_t)&defDim},
   {XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
      XtOffset(XwSashWidget, core.height), XtRInt, (caddr_t)&defDim},
   {XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
      XtOffset(XwSashWidget, core.border_width), XtRInt, (caddr_t)&defBorder},
   {XtNcallback, XtCCallback, XtRCallback, sizeof(caddr_t), 
      XtOffset(XwSashWidget, sash.sash_action), XtRPointer, NULL},
};

void SashAction( /* Widget, XEvent*, String*, Cardinal */ );

static XtActionsRec actionsList[] =
{
  {"SashAction",      SashAction},
  {"enter", (XtActionProc) _XwPrimitiveEnter},
  {"leave", (XtActionProc) _XwPrimitiveLeave},
};


XwSashClassRec XwsashClassRec = {
   {
/* core class fields */
    /* superclass         */   (WidgetClass) &XwprimitiveClassRec,
    /* class name         */   "Sash",
    /* size               */   sizeof(XwSashRec),
    /* class initialize   */   ClassInitialize,
    /* class_part_init    */   NULL,
    /* class_inited       */   FALSE,
    /* initialize         */   NULL,
    /* initialize_hook    */   NULL,
    /* realize            */   Realize,
    /* actions            */   actionsList,
    /* num_actions        */   XtNumber(actionsList),
    /* resourses          */   resources,
    /* resource_count     */   XtNumber(resources),
    /* xrm_class          */   NULLQUARK,
    /* compress_motion    */   TRUE,
    /* compress_exposure  */   TRUE,
    /* compress_enter/lv  */   TRUE,
    /* visible_interest   */   TRUE,
    /* destroy            */   NULL,
    /* resize             */   NULL,
    /* expose             */   Redisplay,
    /* set_values         */   NULL,
    /* set_values_hook    */   NULL,
    /* set_values_almost  */   XtInheritSetValuesAlmost,
    /* get_values_hook    */   NULL,
    /* accept_focus       */   NULL,
    /* version            */   XtVersion,
    /* callback_private   */   NULL,
    /* tm_table           */   defTranslations,
    /* query_geometry     */   NULL,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
   }
};

WidgetClass XwsashWidgetClass = (WidgetClass) &XwsashClassRec;



/************************************************************************
 *
 *  ClassInitialize
 *  ===============
 *    Initialize the primitive part of class structure with 
 *    routines to do special highlight & unhighlight for Sash.
 *
 ************************************************************************/

void HighlightSash(sash)
 XwSashWidget sash;
{
  int x, y, width, height;
  
  x=0;
  y=x;

  
  XFillRectangle (XtDisplay ((Widget) sash), XtWindow ((Widget) sash),
                   sash->primitive.highlight_GC,
                   x,y, sash->core.width-(2*x), sash->core.height-(2*y));

}


void UnhighlightSash(sash)
 XwSashWidget sash;
{
  int x, y, width, height;
  
  x=0;
  y=x;

  XClearArea (XtDisplay ((Widget) sash), XtWindow ((Widget) sash),
                   x,y, sash->core.width-(2*x), sash->core.height-(2*y),
	           FALSE);

}


static void ClassInitialize()
{
   XwsashClassRec.primitive_class.select_proc = (XtWidgetProc) NULL;
   XwsashClassRec.primitive_class.release_proc = (XtWidgetProc) NULL;
   XwsashClassRec.primitive_class.toggle_proc = (XtWidgetProc) NULL;
   XwsashClassRec.primitive_class.border_highlight =
                  (XtWidgetProc) HighlightSash;
   XwsashClassRec.primitive_class.border_unhighlight = 
                  (XtWidgetProc) UnhighlightSash;
}




static void SashAction( widget, event, params, num_params )
    Widget widget;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    SashCallDataRec call_data;

    call_data.event = event;
    call_data.params = params;
    call_data.num_params = *num_params;

    XtCallCallbacks( widget, XtNcallback, (caddr_t)&call_data );
}


static Cursor SashCursor=NULL;

static void Realize(w, p_valueMask, attributes)
    register Widget w;
    Mask *p_valueMask;
    XSetWindowAttributes *attributes;
{
   Mask valueMask = *p_valueMask;

   if (SashCursor==NULL)
         SashCursor=XCreateFontCursor(XtDisplay(w), XC_crosshair);
    attributes->cursor = SashCursor;
    valueMask |= CWCursor;
    _XwRealize(w, p_valueMask, attributes);
    XDefineCursor(XtDisplay(w), XtWindow(w), SashCursor);
}





/*************************************<->*************************************
 *
 *  Redisplay (w, event)
 *
 *   Description:
 *   -----------
 *     Cause the widget, identified by w, to be redisplayed.
 *
 *
 *   Inputs:
 *   ------
 *     w = widget to be redisplayed;
 *     event = event structure identifying need for redisplay on this
 *             widget.
 * 
 *   Outputs:
 *   -------
 *
 *   Procedures Called
 *   -----------------
 *   DrawToggle()
 *   XDrawString()
 *************************************<->***********************************/

static void Redisplay(w, event)
    Widget w;
    XEvent *event;
{
   register XwSashWidget sash = (XwSashWidget) w;


    if (sash->primitive.highlighted)
       _XwHighlightBorder(w);
    else if (sash->primitive.display_highlighted)
       _XwUnhighlightBorder(w);
}

