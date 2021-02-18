/* -*-C-*-
********************************************************************************
*
* File:         wc_Scale.c
* RCS:          $Header: wc_Scale.c,v 1.1 89/11/25 04:50:11 mayer Exp $
* Description:  XM_SCALE_WIDGET_CLASS
* Author:       Niels Mayer, HPLabs
* Created:      Sat Oct 28 04:32:59 1989
* Modified:     Sat Nov 25 04:49:58 1989 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: wc_Scale.c,v 1.1 89/11/25 04:50:11 mayer Exp $";


#include <stdio.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/Scale.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_funtab.h"

/******************************************************************************
 * Same as WIDGET_CLASS's :add_callback method except that this understands
 * how to get values from the XmScaleCallbackStruct.
 * Specifying one or more of the following symbols in the callback bindings 
 * list will bind that symbol's value in the lexical environment of the callback:
 * CALLBACK_WIDGET
 * CALLBACK_REASON
 * CALLBACK_XEVENT
 * CALLBACK_VALUE
 ******************************************************************************/
LVAL Xm_Scale_Widget_Class_Method_ADD_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */
  extern void Wcb_XmScaleCallbackStruct_Callbackproc(); /* w_callbacks.c */
  return(Wcb_Meta_Method_Add_Callback(Wcb_XmScaleCallbackStruct_Callbackproc, FALSE));
}


/******************************************************************************
 * Same as WIDGET_CLASS's :set_callback method except that this understands
 * how to get values from the XmScaleCallbackStruct.
 * Specifying one or more of the following symbols in the callback bindings 
 * list will bind that symbol's value in the lexical environment of the callback:
 * CALLBACK_WIDGET
 * CALLBACK_REASON
 * CALLBACK_XEVENT
 * CALLBACK_VALUE
 ******************************************************************************/
LVAL Xm_Scale_Widget_Class_Method_SET_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */
  extern void Wcb_XmScaleCallbackStruct_Callbackproc(); /* w_callbacks.c */
  return (Wcb_Meta_Method_Add_Callback(Wcb_XmScaleCallbackStruct_Callbackproc, TRUE));
}

/******************************************************************************
 * (send <scale_widget> :SET_VALUE <value>) --> rets <scale_widget>
 * 
 * void XmScaleSetValue (w, value)
 * Widget w;
 * int    value;
 ******************************************************************************/
LVAL Xm_Scale_Widget_Class_Method_SET_VALUE()
{
  LVAL self;
  Widget widget_id;
  int value;

  self = xlga_widgetobj();
  value = (int) getfixnum(xlgafixnum());
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  XmScaleSetValue(widget_id, value);
  return (self);
}

/******************************************************************************
 * (send <scale_widget> :GET_VALUE) --> returns slider value as a FIXNUM
 * 
 * void XmScaleGetValue (w, value)
 * Widget w;
 * int * value;
 ******************************************************************************/
LVAL Xm_Scale_Widget_Class_Method_GET_VALUE()
{
  LVAL self;
  Widget widget_id;
  int value;

  self = xlga_widgetobj();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  XmScaleGetValue(widget_id, &value);
  return (cvfixnum((FIXTYPE) value));
}

/******************************************************************************
 *
 ******************************************************************************/
Wc_Scale_Init()
{
  LVAL o_XM_SCALE_WIDGET_CLASS;
  extern LVAL Wcls_Create_Subclass_Of_WIDGET_CLASS(); /* w_classes.c */

  o_XM_SCALE_WIDGET_CLASS =
    Wcls_Create_Subclass_Of_WIDGET_CLASS("XM_SCALE_WIDGET_CLASS",
					 xmScaleWidgetClass);

  xladdmsg(o_XM_SCALE_WIDGET_CLASS, ":ADD_CALLBACK",
           FTAB_Xm_Scale_Widget_Class_Method_ADD_CALLBACK);

  xladdmsg(o_XM_SCALE_WIDGET_CLASS, ":SET_CALLBACK",
           FTAB_Xm_Scale_Widget_Class_Method_SET_CALLBACK);

  xladdmsg(o_XM_SCALE_WIDGET_CLASS, ":SET_VALUE",
	   FTAB_Xm_Scale_Widget_Class_Method_SET_VALUE);

  xladdmsg(o_XM_SCALE_WIDGET_CLASS, ":GET_VALUE",
	   FTAB_Xm_Scale_Widget_Class_Method_GET_VALUE);
}

