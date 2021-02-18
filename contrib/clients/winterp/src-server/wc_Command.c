/* -*-C-*-
********************************************************************************
*
* File:         wc_Command.c
* RCS:          $Header: wc_Command.c,v 1.1 89/11/25 04:36:01 mayer Exp $
* Description:  XM_COMMAND_WIDGET_CLASS
* Author:       Niels Mayer, HPLabs
* Created:      Fri Oct 27 21:57:58 1989
* Modified:     Sat Nov 25 04:35:53 1989 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: wc_Command.c,v 1.1 89/11/25 04:36:01 mayer Exp $";

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/Command.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_funtab.h"

/******************************************************************************
 * Same as WIDGET_CLASS's :add_callback method except that this understands
 * how to get values from the XmCommandCallbackStruct.
 * Specifying one or more of the following symbols in the callback bindings 
 * list will bind that symbol's value in the lexical environment of the callback:
 * CALLBACK_WIDGET
 * CALLBACK_REASON
 * CALLBACK_XEVENT
 * CALLBACK_VALUE
 * CALLBACK_LENGTH
 ******************************************************************************/
LVAL Xm_Command_Widget_Class_Method_ADD_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */
  extern void Wcb_XmCommandCallbackStruct_Callbackproc(); /* w_callbacks.c */
  return(Wcb_Meta_Method_Add_Callback(Wcb_XmCommandCallbackStruct_Callbackproc, FALSE));
}


/******************************************************************************
 * Same as WIDGET_CLASS's :set_callback method except that this understands
 * how to get values from the XmCommandCallbackStruct.
 * Specifying one or more of the following symbols in the callback bindings 
 * list will bind that symbol's value in the lexical environment of the callback:
 * CALLBACK_WIDGET
 * CALLBACK_REASON
 * CALLBACK_XEVENT
 * CALLBACK_VALUE
 * CALLBACK_LENGTH
 ******************************************************************************/
LVAL Xm_Command_Widget_Class_Method_SET_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */
  extern void Wcb_XmCommandCallbackStruct_Callbackproc(); /* w_callbacks.c */
  return (Wcb_Meta_Method_Add_Callback(Wcb_XmCommandCallbackStruct_Callbackproc, TRUE));
}


/******************************************************************************
 * (send <commandwidget> :get_child <symbol>)
 * This method returns a WIDGETOBJ corresonding to <symbol>
 * <symbol> can be :DIALOG_COMMAND_TEXT,
 *                 :DIALOG_HISTORY_LIST, or
 *                 :DIALOG_PROMPT_LABEL
 *
 * Widget XmCommandGetChild (widget, child)
 * Widget   widget;
 * unsigned char   child;
 ******************************************************************************/
LVAL Xm_Command_Widget_Class_Method_GET_CHILD()
{
  extern LVAL Wcls_WidgetID_To_WIDGETOBJ();
  extern LVAL s_XmDIALOG_COMMAND_TEXT, s_XmDIALOG_HISTORY_LIST, s_XmDIALOG_PROMPT_LABEL;
  LVAL self, lval_child;
  Widget widget_id;
  unsigned char child;

  self = xlga_widgetobj();
  lval_child = xlgasymbol();
  xllastarg();
  
  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  if (lval_child == s_XmDIALOG_COMMAND_TEXT)
    child = XmDIALOG_COMMAND_TEXT;
  else if (lval_child == s_XmDIALOG_HISTORY_LIST)
    child = XmDIALOG_HISTORY_LIST;
  else if (lval_child == s_XmDIALOG_PROMPT_LABEL)
    child = XmDIALOG_PROMPT_LABEL;
  else 
    xlerror("COMMAND_WIDGET_CLASS method :GET_CHILD -- unknown child type.", lval_child);

  return (Wcls_WidgetID_To_WIDGETOBJ(XmCommandGetChild(widget_id, child)));
}


/******************************************************************************
 * (send <commandwidget> :set_value <value>)
 * This sets the text in the widget's command area to the string or XmString
 * <value>. If a normal string is given, it will be converted to an XmString
 * and returned as the  method's result.
 *
 * void XmCommandSetValue (widget, value)
 * Widget   widget;
 * XmString value;
 ******************************************************************************/
LVAL Xm_Command_Widget_Class_Method_SET_VALUE()
{
  LVAL self, lval_value;
  Widget widget_id;
  XmString value;
  extern XmString Get_String_or_XmString_Arg_Returning_XmString(); /* w_XmString.c */

  self = xlga_widgetobj();
  value = Get_String_or_XmString_Arg_Returning_XmString(&lval_value);
  xllastarg();
  
  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  /* the lame part about all this is that the XmString value that we may
     have converted above will just be cvted back to a string below. */
  XmCommandSetValue(widget_id, value);
  
  return (lval_value);
}


/******************************************************************************
 * (send <commandwidget> :append_value <value>)
 * This appends the string or XmString <value> to the string in the command
 * area widget. If a normal string is given, it will be converted to an
 * XmString and returned as the method's result.
 * 
 * void XmCommandAppendValue (widget, value)
 * Widget widget;
 * XmString value;
 ******************************************************************************/
LVAL Xm_Command_Widget_Class_Method_APPEND_VALUE()
{
  LVAL self, lval_value;
  Widget widget_id;
  XmString value;
  extern XmString Get_String_or_XmString_Arg_Returning_XmString(); /* w_XmString.c */

  self = xlga_widgetobj();
  value = Get_String_or_XmString_Arg_Returning_XmString(&lval_value);
  xllastarg();
  
  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  /* the lame part about all this is that the XmString value that we may
     have converted above will just be cvted back to a string below */
  XmCommandAppendValue(widget_id, value);
  
  return (lval_value);
}


/******************************************************************************
 * (send <commandwidget> :error <error>)
 * This temporarily displays the string or XmString <error> in the history
 * area of the command widget, the display is cleared upon entry of the
 * next command. If a normal string is given, it will be converted to an
 * XmString and returned as the method's result.
 *
 * void XmCommandError (widget, error)
 * Widget widget;
 * XmString error;
 ******************************************************************************/
LVAL Xm_Command_Widget_Class_Method_ERROR()
{
  LVAL self, lval_value;
  Widget widget_id;
  XmString value;
  extern XmString Get_String_or_XmString_Arg_Returning_XmString(); /* w_XmString.c */

  self = xlga_widgetobj();
  value = Get_String_or_XmString_Arg_Returning_XmString(&lval_value);
  xllastarg();
  
  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  /* the lame part about all this is that the XmString value that we may
     have converted above will just be cvted back to a string below */
  XmCommandError(widget_id, value);
  
  return (lval_value);
}


/******************************************************************************
 *
 ******************************************************************************/
Wc_Command_Init()
{
  LVAL o_XM_COMMAND_WIDGET_CLASS;
  extern LVAL Wcls_Create_Subclass_Of_WIDGET_CLASS(); /* w_classes.c */

  /* NOTE -- POSSIBLE BUG:
     :new method on this ends up calling Widget_Class_Method_ISNEW()
     which simply does an XtCreateWidget()... however, the convenience
     function Command.c:XmCreateCommand() prepends the following arg to the
     creation arglist: 
         XtSetArg (argsNew[ac], XmNdialogType, XmDIALOG_COMMAND);
	 ???
	 */

  o_XM_COMMAND_WIDGET_CLASS =
    Wcls_Create_Subclass_Of_WIDGET_CLASS("XM_COMMAND_WIDGET_CLASS",
					 xmCommandWidgetClass);

  xladdmsg(o_XM_COMMAND_WIDGET_CLASS, ":ADD_CALLBACK",
           FTAB_Xm_Command_Widget_Class_Method_ADD_CALLBACK);

  xladdmsg(o_XM_COMMAND_WIDGET_CLASS, ":SET_CALLBACK",
           FTAB_Xm_Command_Widget_Class_Method_SET_CALLBACK);

  xladdmsg(o_XM_COMMAND_WIDGET_CLASS, ":GET_CHILD",
           FTAB_Xm_Command_Widget_Class_Method_GET_CHILD);

  xladdmsg(o_XM_COMMAND_WIDGET_CLASS, ":ERROR",
	   FTAB_Xm_Command_Widget_Class_Method_ERROR);

  xladdmsg(o_XM_COMMAND_WIDGET_CLASS, ":SET_VALUE",
	   FTAB_Xm_Command_Widget_Class_Method_SET_VALUE);

  xladdmsg(o_XM_COMMAND_WIDGET_CLASS, ":APPEND_VALUE",
	   FTAB_Xm_Command_Widget_Class_Method_APPEND_VALUE);
}
