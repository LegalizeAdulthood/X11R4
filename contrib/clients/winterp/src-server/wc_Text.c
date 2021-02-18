/* -*-C-*-
********************************************************************************
*
* File:         wc_Text.c
* RCS:          $Header: wc_Text.c,v 1.1 89/11/25 04:53:36 mayer Exp $
* Description:  XM_TEXT_WIDGET_CLASS
* Author:       Niels Mayer, HPLabs
* Created:      Sat Oct 28 04:50:44 1989
* Modified:     Sat Nov 25 04:53:25 1989 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: wc_Text.c,v 1.1 89/11/25 04:53:36 mayer Exp $";

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/Text.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_funtab.h"

/*****************************************************************************
 * (send XM_TEXT_WIDGET_CLASS :new 
 *                           [:managed/:unmanaged]
 *                           [:scrolled]
 *                           [<name>]
 *                           <parent> 
 *                           [:XMN_<arg1> <val1>]
 *                           [. . .             ]
 *                           [:XMN_<argN> <valN>])
 *
 * The optional keyword submessage :managed will cause a subsequent call
 * to XtManageChild(). If the submessage :unmanaged is present, or no
 * submessage, then XtManageChild() won't be called, and the resulting
 * widget will be returned unmanaged.
 *
 *  (send XM_TEXT_WIDGET_CLASS :new ...)
 *  --> XmCreateText();
 *  (send XM_TEXT_WIDGET_CLASS :new :scrolled ...)
 *  --> XmCreateScrolledText();
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_ISNEW()
{
  extern ArgList Wres_Get_LispArglist(); /* from w_resources.c */
  extern void    Wres_Free_C_Arglist_Data(); /* from w_resources.c */
  extern LVAL k_managed, k_unmanaged, k_scrolled;
  LVAL self, o_parent;
  char* name;
  Boolean managed_p, scrolled_p;
  Widget parent_widget_id, widget_id;

  self = xlgaobject();		/* NOTE: xlobj.c:clnew() returns an OBJECT; if this method
				   returns successfully, it will return a WIDGETOBJ */

  /* get optional managed/unmanaged arg */
  if (moreargs() && ((*xlargv == k_managed) || (*xlargv == k_unmanaged)))
    managed_p = (nextarg() == k_managed);
  else
    managed_p = FALSE;		/* by default don't call XtManageChild() */

  /* get optional :scrolled arg */
  if (moreargs() && (*xlargv == k_scrolled)) {
    nextarg();
    scrolled_p = TRUE;
  }
  else
    scrolled_p = FALSE;		/* by default, we don't want a scrolled editor */

  /* get optional <name> arg */
  if (moreargs() && (stringp(*xlargv)))
    name = (char*) getstring(nextarg());
  else
    name = "";			/* default name */

  /* get required <parent> widget-object arg */
  o_parent = xlga_widgetobj();

  if (!(parent_widget_id = get_widgetobj_widgetID(o_parent)))
    xlerror("<parent> widget object has been :destroy'd or hasn't been initialized by :isnew.", o_parent);

  /*
   * Store the widget object <self> in the XmNuserData resource on the
   * widget. This will allow us to retrieve the widget object from Xtoolkit
   * functions returning widget ID's without having to keep around a table
   * of widgetID-->widget-objects.
   */
   ARGLIST_RESET(); ARGLIST_ADD(XmNuserData, self); 

  if (moreargs()) {		/* if there are more arguments, */
    Cardinal xt_numargs;	/* then we have some extra widget resources to set */
    ArgList xt_arglist = Wres_Get_LispArglist(self, parent_widget_id, ARGLIST(), &xt_numargs);
    if (scrolled_p)
      widget_id = XmCreateScrolledText(parent_widget_id, name, xt_arglist, xt_numargs);
    else
      widget_id = XmCreateText(parent_widget_id, name, xt_arglist, xt_numargs);
    Wres_Free_C_Arglist_Data();
  }
  else 
    if (scrolled_p)
      widget_id = XmCreateScrolledText(parent_widget_id, name, ARGLIST());
    else
      widget_id = XmCreateText(parent_widget_id, name, ARGLIST());

  Wcls_Initialize_WIDGETOBJ(self, widget_id);

  if (managed_p)
    XtManageChild(widget_id);

#ifdef DEBUG_WINTERP_1
  Wcls_Print_WidgetObj_Info(self);
#endif
  return (self);
}

/******************************************************************************
 * Same as WIDGET_CLASS's :add_callback method except that this understands
 * how to get values from the XmTextVerifyCallbackStruct.
 * Specifying one or more of the following symbols in the callback bindings 
 * list will bind that symbol's value in the lexical environment of the callback:
 * CALLBACK_WIDGET
 * CALLBACK_REASON
 * CALLBACK_XEVENT
 * CALLBACK_CUR_INSERT
 * CALLBACK_NEW_INSERT
 * CALLBACK_START_POS
 * CALLBACK_END_POS
 *
 * NOTE: setting the calldata->text->(XmTextBlockRec) and
 * calldata->doit slots of the XmTextVerifyCallbackStruct is not supported
 * in this version of winterp. Most of this functionality can be achieved
 * through methods on the XM_TEXT_WIDGET_CLASS. (For details, see comments
 * in src-server/w_callbacks.c, procedure,
 * Wcb_Lexical_Bindings_For_XmTextVerifyCallbackStruct().
 ******************************************************************************/
LVAL Xm_Text_Widget_Class_Method_ADD_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */
  extern void Wcb_XmTextVerifyCallbackStruct_Callbackproc(); /* w_callbacks.c */
  return(Wcb_Meta_Method_Add_Callback(Wcb_XmTextVerifyCallbackStruct_Callbackproc, FALSE));
}


/******************************************************************************
 * Same as WIDGET_CLASS's :set_callback method except that this understands
 * how to get values from the XmTextVerifyCallbackStruct.
 * Specifying one or more of the following symbols in the callback bindings 
 * list will bind that symbol's value in the lexical environment of the callback:
 * CALLBACK_WIDGET
 * CALLBACK_REASON
 * CALLBACK_XEVENT
 * CALLBACK_CUR_INSERT
 * CALLBACK_NEW_INSERT
 * CALLBACK_START_POS
 * CALLBACK_END_POS
 *
 * NOTE: setting the calldata->text->(XmTextBlockRec) and
 * calldata->doit slots of the XmTextVerifyCallbackStruct is not supported
 * in this version of winterp. Most of this functionality can be achieved
 * through methods on the XM_TEXT_WIDGET_CLASS. (For details, see comments
 * in src-server/w_callbacks.c, procedure,
 * Wcb_Lexical_Bindings_For_XmTextVerifyCallbackStruct().
 ******************************************************************************/
LVAL Xm_Text_Widget_Class_Method_SET_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */
  extern void Wcb_XmTextVerifyCallbackStruct_Callbackproc(); /* w_callbacks.c */
  return (Wcb_Meta_Method_Add_Callback(Wcb_XmTextVerifyCallbackStruct_Callbackproc, TRUE));
}


/*****************************************************************************
 * (send <text_widget> :GET_STRING)
 *   --> string displayed by the text edit widget
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_GET_STRING()
{
  LVAL self, s_result;
  char* string;
  Widget widget_id;

  self = xlga_widgetobj();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  string = XmTextGetString(widget_id);
  s_result = cvstring(string);
  XtFree(string);

  return (s_result);
}


/*****************************************************************************
 * (send <text_widget> :GET_LAST_POSITION)
 * --> integer representing last position
 *
 * XmTextPosition XmTextGetLastPosition(widget)
 * Widget widget;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_GET_LAST_POSITION()
{
  LVAL self;
  Widget widget_id;

  self = xlga_widgetobj();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  return (cvfixnum((FIXTYPE) XmTextGetLastPosition(widget_id)));
}


/*****************************************************************************
 * (send <text_widget> :SET_STRING <string>)
 *
 * void XmTextSetString(w, value)
 * Widget   w;
 * char     *value;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_SET_STRING()
{
  LVAL self;
  char* string;
  Widget widget_id;

  self = xlga_widgetobj();
  string = (char*) getstring(xlgastring());
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  XmTextSetString(widget_id, string);
  return (self);
}


/*****************************************************************************
 * (send <text_widget> :replace <from-pos> <to-pos> <string>)
 *
 * void XmTextReplace(w, frompos, topos, value)
 * Widget w;
 * XmTextPosition frompos, topos;
 * char   *value;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_REPLACE()
{
  LVAL self, lval_frompos, lval_topos;
  long frompos, topos;
  char* string;
  Widget widget_id;

  self = xlga_widgetobj();
  lval_frompos = xlgafixnum();
  frompos = (long) getfixnum(lval_frompos);
  if (frompos < 0L)
    xlerror("XmTextPosition must be a positive FIXNUM.", lval_frompos);
  lval_topos = xlgafixnum();
  topos = (long) getfixnum(lval_topos);
  if (topos < 0L)
    xlerror("XmTextPosition must be a positive FIXNUM.", lval_topos);
  string = (char*) getstring(xlgastring());
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  XmTextReplace(widget_id, frompos, topos, string);
  return (self);
}


/*****************************************************************************
 * (send <text_widget> :get_editable)
 *      ====> returns T if editable, NIL otherwise.
 *
 * Boolean XmTextGetEditable(widget)
 * Widget widget;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_GET_EDITABLE()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;

  self = xlga_widgetobj();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  return (XmTextGetEditable(widget_id) ? true : NIL);
}


/*****************************************************************************
 * (send <text_widget> :set_editable <editableP>)
 *
 * void XmTextSetEditable(w, editable)
 * Widget w;
 * Boolean editable;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_SET_EDITABLE()
{
  LVAL self, lval_editablep;
  Widget widget_id;

  self = xlga_widgetobj();
  lval_editablep = xlgetarg();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  XmTextSetEditable(widget_id, (lval_editablep) ? TRUE : FALSE);
  return (self);
}


/*****************************************************************************
 * (send <text_widget> :GET_MAX_LENGTH)
 *       =====> FIXNUM, the maximum length of the Text widget.
 *
 * int XmTextGetMaxLength(widget)
 * Widget widget;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_GET_MAX_LENGTH()
{
  LVAL self, s_result;
  char* string;
  Widget widget_id;

  self = xlga_widgetobj();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  return (cvfixnum((FIXTYPE) XmTextGetMaxLength(widget_id)));
}


/*****************************************************************************
 * (send <text_widget> :SET_MAX_LENGTH <max_length>)
 *
 * void XmTextSetMaxLength(w, max_length)
 * Widget w;
 * int max_length;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_SET_MAX_LENGTH()
{
  extern long max_int;		/* from w_resources.c */
  LVAL self, lval_max_length;
  long max_length;
  Widget widget_id;

  self = xlga_widgetobj();
  lval_max_length = xlgafixnum();
  max_length = (long) getfixnum(lval_max_length);
  if (max_length < 0L)
    xlerror("Expected a positive FIXNUM.", lval_max_length);
  if (max_length > max_int)
    xlerror("FIXNUM too large", lval_max_length);
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  XmTextSetMaxLength(widget_id, (int) max_length);
  return (self);
}


/*****************************************************************************
 * (send <text_widget> :GET_SELECTION)
 *       ===> returns a string, the selected text.
 *
 * char *XmTextGetSelection(widget)
 * Widget widget;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_GET_SELECTION()
{
  LVAL self, s_result;
  char* string;
  Widget widget_id;

  self = xlga_widgetobj();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  string = XmTextGetSelection(widget_id);
  s_result = cvstring(string);
  XtFree(string);

  return (s_result);
}


/*****************************************************************************
 * (send <text_widget> :SET_SELECTION <firstpos> <lastpos> [<time>])
 * note, time argument is not present in this version...
 *
 * void XmTextSetSelection(widget, first, last, time)
 * Widget widget;
 * XmTextPosition first, last;
 * Time time;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_SET_SELECTION()
{
  LVAL self, lval_firstpos, lval_lastpos;
  long firstpos, lastpos;
  Widget widget_id;

  self = xlga_widgetobj();
  lval_firstpos = xlgafixnum();
  firstpos = (long) getfixnum(lval_firstpos);
  if (firstpos < 0L)
    xlerror("XmTextPosition must be a positive FIXNUM.", lval_firstpos);
  lval_lastpos = xlgafixnum();
  lastpos = (long) getfixnum(lval_lastpos);
  if (lastpos < 0L)
    xlerror("XmTextPosition must be a positive FIXNUM.", lval_lastpos);
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  XmTextSetSelection(widget_id, firstpos, lastpos, CurrentTime); /* BUG -- replace CurrentTime with something useful */
  return (self);
}


/*****************************************************************************
 * (send <text_widget> :CLEAR_SELECTION [<time>])
 *
 * void XmTextClearSelection(widget, time)
 * Widget widget;
 * Time time;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_CLEAR_SELECTION()
{
  LVAL self;
  Widget widget_id;

  self = xlga_widgetobj();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  XmTextClearSelection(widget_id, CurrentTime);

  return (self);
}

/*****************************************************************************
 * (send <text_widget> :GET_TOP_POSITION)
 *       ====> returns a FIXNUM representing top position of Text widget.
 *
 XmTextPosition XmTextGetTopPosition(widget)
 XmTextWidget widget;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_GET_TOP_POSITION()
{
  LVAL self;
  Widget widget_id;

  self = xlga_widgetobj();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  return (cvfixnum((FIXTYPE) XmTextGetTopPosition(widget_id)));
}


/*****************************************************************************
 * (send <text_widget> :SET_TOP_POSITION <top_position>)
 *
 * void XmTextSetTopPosition(widget, top_position)
 * XmTextWidget widget;
 * XmTextPosition top_position;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_SET_TOP_POSITION()
{
  LVAL self, lval_top_position;
  long top_position;
  Widget widget_id;

  self = xlga_widgetobj();
  lval_top_position = xlgafixnum();
  top_position = (long) getfixnum(lval_top_position);
  if (top_position < 0L)
    xlerror("XmTextPosition must be a positive FIXNUM.", lval_top_position);
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  XmTextSetTopPosition(widget_id, top_position);

  return (self);
}


/*****************************************************************************
 * (send <text_widget> :GET_INSERTION_POSITION)
 *
 * XmTextPosition XmTextGetInsertionPosition(widget)
 * XmTextWidget widget;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_GET_INSERTION_POSITION()
{
  LVAL self;
  Widget widget_id;

  self = xlga_widgetobj();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  return (cvfixnum((FIXTYPE) XmTextGetInsertionPosition(widget_id)));
}


/*****************************************************************************
 * (send <text_widget> :SET_INSERTION_POSITION <position>)
 *
 * void XmTextSetInsertionPosition(widget, position)
 * XmTextWidget widget;
 * XmTextPosition position;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_SET_INSERTION_POSITION()
{
  LVAL self, lval_insertion_position;
  long insertion_position;
  Widget widget_id;

  self = xlga_widgetobj();
  lval_insertion_position = xlgafixnum();
  insertion_position = (long) getfixnum(lval_insertion_position);
  if (insertion_position < 0L)
    xlerror("XmTextPosition must be a positive FIXNUM.", lval_insertion_position);
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  XmTextSetInsertionPosition(widget_id, insertion_position);

  return (self);
}


/*****************************************************************************
 * (send <text_widget> :GET_SELECTION_POSITION)
 *
 * Boolean XmTextGetSelectionPosition(widget, left, right)
 * XmTextWidget widget;
 * XmTextPosition *left, *right;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_GET_SELECTION_POSITION()
{
  LVAL self, lval_result, lval_left_pos, lval_right_pos;
  Widget widget_id;
  long left_pos, right_pos;

  /* protect some pointers */
  xlstkcheck(3);
  xlsave(lval_left_pos);
  xlsave(lval_right_pos);
  xlsave(lval_result);

  self = xlga_widgetobj();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  if (XmTextGetSelectionPosition(widget_id, &left_pos, &right_pos)) {
    lval_left_pos = cvfixnum((FIXTYPE) left_pos);
    lval_right_pos = cvfixnum((FIXTYPE) right_pos);
    lval_result = cons(lval_left_pos, lval_right_pos);
  }
  else 
    lval_result = NIL;

  /* restore the stack */
  xlpopn(3);

  return (lval_result);
}


/*****************************************************************************
 * (send <text_widget> :XY_TO_POS <x> <y>)
 *
 * XmTextPosition XmTextXYToPos(widget, x, y)
 * XmTextWidget widget;
 * Position x, y;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_XY_TO_POS()
{
  extern long min_int, max_int;	/* from w_resources.c */
  LVAL self, lval_x, lval_y;
  long x, y;
  Widget widget_id;

  self = xlga_widgetobj();

  lval_x = xlgafixnum();
  x = (long) getfixnum(lval_x);
  if (x < min_int)
    xlerror("value too small.", lval_x);
  if (x > max_int)
    xlerror("value too large.", lval_x);

  lval_y = xlgafixnum();
  y = (long) getfixnum(lval_y);
  if (y < min_int)
    xlerror("value too small.", lval_y);
  if (y > max_int)
    xlerror("value too large.", lval_y);

  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  return (cvfixnum((FIXTYPE) XmTextXYToPos(widget_id, (int) x, (int) y)));
}


/*****************************************************************************
 * (send <text_widget> :POS_TO_XY <position>)
 *
 * Boolean XmTextPosToXY(widget, position, x, y)
 * XmTextWidget widget;
 * XmTextPosition position;
 * Position *x, *y;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_POS_TO_XY()
{
  LVAL self, lval_result, lval_x, lval_y, lval_position;
  Widget widget_id;
  long position;
  int x, y;			/* note: <X11/Intrinsic.h> sez typedef int Position; */

  /* protect some pointers */
  xlstkcheck(3);
  xlsave(lval_x);
  xlsave(lval_y);
  xlsave(lval_result);

  self = xlga_widgetobj();
  lval_position = xlgafixnum();
  position = (long) getfixnum(lval_position);
  if (position < 0L)
    xlerror("XmTextPosition must be a positive FIXNUM.", lval_position);
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  if (XmTextPosToXY(widget_id, position, &x, &y)) {
    lval_x = cvfixnum((FIXTYPE) x);
    lval_y = cvfixnum((FIXTYPE) y);
    lval_result = cons(lval_x, lval_y);
  }
  else 
    lval_result = NIL;

  /* restore the stack */
  xlpopn(3);

  return (lval_result);
}

/******************************************************************************
 * XmTextSource XmTextGetSource(widget)
 * XmTextWidget widget;
 ******************************************************************************/

/******************************************************************************
 * void XmTextSetSource(widget, source, top_position, cursor_position)
 * XmTextWidget widget;
 * XmTextSource source;
 * XmTextPosition top_position;
 * XmTextPosition cursor_position;
 ******************************************************************************/


/*****************************************************************************
 * (send <text_widget> :SHOW_POSITION <position>)
 *
 * void XmTextShowPosition(widget, position)
 * XmTextWidget widget;
 * XmTextPosition position;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_SHOW_POSITION()
{
  LVAL self, lval_position;
  long position;
  Widget widget_id;

  self = xlga_widgetobj();
  lval_position = xlgafixnum();
  position = (long) getfixnum(lval_position);
  if (position < 0L)
    xlerror("XmTextPosition must be a positive FIXNUM.", lval_position);
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  XmTextShowPosition(widget_id, position);

  return (self);
}


/*****************************************************************************
 * (send <text_widget> :SCROLL <n>)
 *
 * void XmTextScroll(widget, n)
 * XmTextWidget widget;
 * int n; //--Positive moves text upward; negative, downward.//
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_SCROLL()
{
  LVAL self, lval_n;
  Widget widget_id;
  int n;

  self = xlga_widgetobj();
  lval_n = xlgafixnum();
  n = (long) getfixnum(lval_n);
  if (n < min_int)
    xlerror("value too small.", lval_n);
  if (n > max_int)
    xlerror("value too large.", lval_n);
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  XmTextScroll(widget_id, (int) n);

  return (self);
}


/*****************************************************************************
 * (send <text_widget> :DISABLE_REDISPLAY <loses-backingstore-p>)
 *
 * void XmTextDisableRedisplay(widget, losesbackingstore)
 * XmTextWidget widget;
 * Boolean losesbackingstore;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_DISABLE_REDISPLAY()
{
  LVAL self, lval_losesbackingstore_p;
  Widget widget_id;

  self = xlga_widgetobj();
  lval_losesbackingstore_p = xlgetarg();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  XmTextDisableRedisplay(widget_id, (lval_losesbackingstore_p) ? TRUE : FALSE);
  
  return (self);
}

/*****************************************************************************
 * (send <text_widget> :ENABLE_REDISPLAY)
 *
 * void XmTextEnableRedisplay(widget)
 * XmTextWidget widget;
 ****************************************************************************/
LVAL Xm_Text_Widget_Class_Method_ENABLE_REDISPLAY()
{
  LVAL self;
  Widget widget_id;

  self = xlga_widgetobj();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  XmTextEnableRedisplay(widget_id);
  
  return (self);
}


/******************************************************************************
 *
 ******************************************************************************/
Wc_Text_Init()
{
  LVAL o_XM_TEXT_WIDGET_CLASS;
  extern LVAL Wcls_Create_Subclass_Of_WIDGET_CLASS(); /* w_classes.c */
  extern      xladdmsg();	/* from xlobj.c */

  o_XM_TEXT_WIDGET_CLASS =
    Wcls_Create_Subclass_Of_WIDGET_CLASS("XM_TEXT_WIDGET_CLASS",
					 xmTextWidgetClass);

  /* a special :isnew method on this class allows for the creation of this
     widget inside a scrolled window if the submessage keyword :scrolled 
     is given ... */
  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Xm_Text_Widget_Class_Method_ISNEW);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":ADD_CALLBACK",
           FTAB_Xm_Text_Widget_Class_Method_ADD_CALLBACK);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SET_CALLBACK",
           FTAB_Xm_Text_Widget_Class_Method_SET_CALLBACK);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":GET_STRING", 
	   FTAB_Xm_Text_Widget_Class_Method_GET_STRING);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":GET_LAST_POSITION",
	   FTAB_Xm_Text_Widget_Class_Method_GET_LAST_POSITION);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SET_STRING", 
	   FTAB_Xm_Text_Widget_Class_Method_SET_STRING);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":REPLACE",
	   FTAB_Xm_Text_Widget_Class_Method_REPLACE);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":GET_EDITABLE",
	   FTAB_Xm_Text_Widget_Class_Method_GET_EDITABLE);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SET_EDITABLE",
	   FTAB_Xm_Text_Widget_Class_Method_SET_EDITABLE);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":GET_MAX_LENGTH",
	   FTAB_Xm_Text_Widget_Class_Method_GET_MAX_LENGTH);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SET_MAX_LENGTH",
	   FTAB_Xm_Text_Widget_Class_Method_SET_MAX_LENGTH);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":GET_SELECTION",
	   FTAB_Xm_Text_Widget_Class_Method_GET_SELECTION);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SET_SELECTION",
	   FTAB_Xm_Text_Widget_Class_Method_SET_SELECTION);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":CLEAR_SELECTION",
	   FTAB_Xm_Text_Widget_Class_Method_CLEAR_SELECTION);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":GET_TOP_POSITION",
	   FTAB_Xm_Text_Widget_Class_Method_GET_TOP_POSITION);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SET_TOP_POSITION",
	   FTAB_Xm_Text_Widget_Class_Method_SET_TOP_POSITION);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":GET_INSERTION_POSITION",
	   FTAB_Xm_Text_Widget_Class_Method_GET_INSERTION_POSITION);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SET_INSERTION_POSITION",
	   FTAB_Xm_Text_Widget_Class_Method_SET_INSERTION_POSITION);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":GET_SELECTION_POSITION",
	   FTAB_Xm_Text_Widget_Class_Method_GET_SELECTION_POSITION);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":XY_TO_POS",
	   FTAB_Xm_Text_Widget_Class_Method_XY_TO_POS);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":POS_TO_XY",
	   FTAB_Xm_Text_Widget_Class_Method_POS_TO_XY);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SHOW_POSITION",
	   FTAB_Xm_Text_Widget_Class_Method_SHOW_POSITION);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":SCROLL",
	   FTAB_Xm_Text_Widget_Class_Method_SCROLL);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":DISABLE_REDISPLAY",
	   FTAB_Xm_Text_Widget_Class_Method_DISABLE_REDISPLAY);

  xladdmsg(o_XM_TEXT_WIDGET_CLASS, ":ENABLE_REDISPLAY",
	   FTAB_Xm_Text_Widget_Class_Method_ENABLE_REDISPLAY);
}
