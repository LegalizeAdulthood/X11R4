/* -*-C-*-
********************************************************************************
*
* File:         wc_ScrollBar.c
* RCS:          $Header: wc_ScrollBar.c,v 1.1 89/11/25 04:50:48 mayer Exp $
* Description:  XM_SCROLL_BAR_WIDGET_CLASS
* Author:       Niels Mayer, HPLabs
* Created:      Sat Oct 28 04:35:49 1989
* Modified:     Sat Nov 25 04:50:39 1989 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       X11r4 contrib tape release
*
* WINTERP 1.0 Copyright 1989 Hewlett-Packard Company (by Niels Mayer).
* XLISP version 2.1, Copyright (c) 1989, by David Betz.
*
* Permission to use, copy, modify, distribute, and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of Hewlett-Packard and David Betz not be
* used in advertising or publicity pertaining to distribution of the software
* without specific, written prior permission.  Hewlett-Packard and David Betz
* make no representations about the suitability of this software for any
* purpose. It is provided "as is" without express or implied warranty.
*
* HEWLETT-PACKARD AND DAVID BETZ DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
* SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
* IN NO EVENT SHALL HEWLETT-PACKARD NOR DAVID BETZ BE LIABLE FOR ANY SPECIAL,
* INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
* LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
* OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
*
* See ./winterp/COPYRIGHT for information on contacting the authors.
* 
* Please send modifications, improvements and bugfixes to mayer@hplabs.hp.com
* Post XLISP-specific questions/information to the newsgroup comp.lang.lisp.x
**
********************************************************************************
*/
static char rcs_identity[] = "@(#)$Header: wc_ScrollBar.c,v 1.1 89/11/25 04:50:48 mayer Exp $";

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/ScrollBar.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_funtab.h"

/******************************************************************************
 * Same as WIDGET_CLASS's :add_callback method except that this understands
 * how to get values from the XmScrollBarCallbackStruct.
 * Specifying one or more of the following symbols in the callback bindings 
 * list will bind that symbol's value in the lexical environment of the callback:
 * CALLBACK_WIDGET
 * CALLBACK_REASON
 * CALLBACK_XEVENT
 * CALLBACK_VALUE
 * CALLBACK_PIXEL
 ******************************************************************************/
LVAL Xm_Scroll_Bar_Widget_Class_Method_ADD_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */
  extern void Wcb_XmScrollBarCallbackStruct_Callbackproc(); /* w_callbacks.c */
  return(Wcb_Meta_Method_Add_Callback(Wcb_XmScrollBarCallbackStruct_Callbackproc, FALSE));
}


/******************************************************************************
 * Same as WIDGET_CLASS's :set_callback method except that this understands
 * how to get values from the XmScrollBarCallbackStruct.
 * Specifying one or more of the following symbols in the callback bindings 
 * list will bind that symbol's value in the lexical environment of the callback:
 * CALLBACK_WIDGET
 * CALLBACK_REASON
 * CALLBACK_XEVENT
 * CALLBACK_VALUE
 * CALLBACK_PIXEL
 ******************************************************************************/
LVAL Xm_Scroll_Bar_Widget_Class_Method_SET_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */
  extern void Wcb_XmScrollBarCallbackStruct_Callbackproc(); /* w_callbacks.c */
  return (Wcb_Meta_Method_Add_Callback(Wcb_XmScrollBarCallbackStruct_Callbackproc, TRUE));
}


/******************************************************************************
 * (send <scrollbar_widget> :GET_VALUE) 
 * ==> returns a list (<value> <slider_size> <increment> <page_increment>)
 * all of which are FIXNUMs representing  the values of resources
 * :XMN_VALUE, :XMN_SLIDER_SIZE, :XMN_INCREMENT, and :XMN_PAGE_INCREMENT
 * respectively.
 *
 * Note that I couldn't call this method :GET_VALUES because it would override
 * method :GET_VALUES on WIDGET_CLASS.
 *
 * void XmScrollBarGetValues (w, value, slider_size, increment, page_increment)
 *	Widget w;
 *	int *value;
 *	int *slider_size;
 *	int *increment;
 *	int *page_increment;
 ******************************************************************************/
LVAL Xm_Scroll_Bar_Widget_Class_Method_GET_VALUE()
{
  LVAL self, result;
  Widget widget_id;
  int value, slider_size, increment, page_increment;
  
  self = xlga_widgetobj();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  XmScrollBarGetValues(widget_id, &value, &slider_size, &increment, &page_increment);

  xlsave1(result);		/* protect from gc */
  result = cons(cvfixnum((FIXTYPE) page_increment), NIL);
  result = cons(cvfixnum((FIXTYPE) increment), result);
  result = cons(cvfixnum((FIXTYPE) slider_size), result);
  result = cons(cvfixnum((FIXTYPE) value), result);
  xlpop();
  return (result);
}


/******************************************************************************
 * (send <scrollbar_widget> :SET_VALUE <value> <slider_size> 
 *                                     [[[<increment>] <page_increment>] <notify>])
 * ==> returns <scrollbar_widget>.
 * <value> is a fixnum specifying the slider position
 *	(same as setting the resource :XMN_VALUE)
 * <slider_size> is a fixnum specifying the size of the slider.
 *	(same as setting resource :XMN_SLIDER_SIZE)
 * <increment> is an optional fixnum specifying the amount of button increment
 *	and decrement. (same as setting resource :XMN_INCREMENT).
 * <page_increment> is an optional fixnum specifying the page increment size.
 *	(same as setting resource :XMN_INCREMENT).
 * <notify> is an optional Boolean, if NIL, the value changed callback will not
 * 	be activated, else it will.
 * 
 * Note that I couldn't call this method :SET_VALUES because it would override
 * method :SET_VALUES on WIDGET_CLASS.
 *
 * void XmScrollBarSetValues (w, value, slider_size, 
 *                            increment, page_increment, notify)
 *		Widget w;
 *		int value;
 *		int slider_size;
 *		int increment;
 *		int page_increment;
 *		Boolean notify;
 ******************************************************************************/
LVAL Xm_Scroll_Bar_Widget_Class_Method_SET_VALUE()
{
  LVAL self;
  Widget widget_id;
  int value, slider_size, increment, page_increment;
  Boolean notify;

  self = xlga_widgetobj();
  value = (int) getfixnum(xlgafixnum());
  slider_size = (int) getfixnum(xlgafixnum());
  if (moreargs()) {
    increment = (int) getfixnum(xlgafixnum());
    if (moreargs()) {
      page_increment = (int) getfixnum(xlgafixnum());
      if (moreargs())
	notify = (xlgetarg() == NIL) ? FALSE : TRUE;
      else
	notify = FALSE;
    }
    else 
      page_increment = notify = FALSE;
  }
  else 
    increment = page_increment = notify = FALSE;
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  XmScrollBarSetValues(widget_id, value, slider_size,
		       increment, page_increment, notify);

  return (self);
}


/******************************************************************************
 *
 ******************************************************************************/
Wc_ScrollBar_Init()
{
  LVAL o_XM_SCROLL_BAR_WIDGET_CLASS;
  extern LVAL Wcls_Create_Subclass_Of_WIDGET_CLASS(); /* w_classes.c */

  o_XM_SCROLL_BAR_WIDGET_CLASS =
    Wcls_Create_Subclass_Of_WIDGET_CLASS("XM_SCROLL_BAR_WIDGET_CLASS",
					 xmScrollBarWidgetClass);

  xladdmsg(o_XM_SCROLL_BAR_WIDGET_CLASS, ":ADD_CALLBACK",
           FTAB_Xm_Scroll_Bar_Widget_Class_Method_ADD_CALLBACK);

  xladdmsg(o_XM_SCROLL_BAR_WIDGET_CLASS, ":SET_CALLBACK",
           FTAB_Xm_Scroll_Bar_Widget_Class_Method_SET_CALLBACK);

  xladdmsg(o_XM_SCROLL_BAR_WIDGET_CLASS, ":SET_VALUE",
           FTAB_Xm_Scroll_Bar_Widget_Class_Method_SET_VALUE);

  xladdmsg(o_XM_SCROLL_BAR_WIDGET_CLASS, ":GET_VALUE",
           FTAB_Xm_Scroll_Bar_Widget_Class_Method_GET_VALUE);
}
