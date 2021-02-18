/* -*-C-*-
********************************************************************************
*
* File:         wc_SelectioB.c
* RCS:          $Header: wc_SelectioB.c,v 1.1 89/11/25 04:52:00 mayer Exp $
* Description:  XM_SELECTION_BOX_WIDGET_CLASS
* Author:       Niels Mayer, HPLabs
* Created:      Sat Oct 28 04:41:20 1989
* Modified:     Sat Nov 25 04:51:48 1989 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: wc_SelectioB.c,v 1.1 89/11/25 04:52:00 mayer Exp $";

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/SelectioB.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_funtab.h"

static LVAL k_prompt_dialog;

/*****************************************************************************
 * (send XM_SELECTION_BOX_WIDGET_CLASS :new 
 *                           [:managed/:unmanaged]
 *                           [:dialog/:prompt_dialog]
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
 *   (send XM_SELECTION_BOX_WIDGET_CLASS :new ...)
 *   --> XmCreateSelectionBox();
 *   (send XM_SELECTION_BOX_WIDGET_CLASS :new :dialog ...)
 *   --> XmCreateSelectionDialog();
 *   (send XM_SELECTION_BOX_WIDGET_CLASS :new :prompt_dialog ...)
 *   --> XmCreatePromptDialog();
 ****************************************************************************/
LVAL Xm_Selection_Box_Widget_Class_Method_ISNEW()
{
  extern ArgList Wres_Get_LispArglist(); /* from w_resources.c */
  extern void    Wres_Free_C_Arglist_Data(); /* from w_resources.c */
  extern LVAL k_managed, k_unmanaged, k_dialog;
  LVAL self, o_parent;
  char* name;
  Boolean managed_p;
  LVAL sb_kind;
  Widget widget_id, parent_widget_id;

  self = xlgaobject();		/* NOTE: xlobj.c:clnew() returns an OBJECT; if this method
				   returns successfully, it will return a WIDGETOBJ */
  
  /* get optional managed/unmanaged arg */
  if (moreargs() && ((*xlargv == k_managed) || (*xlargv == k_unmanaged)))
    managed_p = (nextarg() == k_managed);
  else
    managed_p = FALSE;		/* by default don't call XtManageChild() */

  /* get optional :dialog/:prompt_dialog arg */
  if (moreargs() && ((*xlargv == k_dialog) || (*xlargv ==  k_prompt_dialog)))
    sb_kind = nextarg();
  else
    sb_kind = NIL;		/* default is XmCreateSelectionBox() */
  
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
    if (sb_kind == NIL)
      widget_id = XmCreateSelectionBox(parent_widget_id, name, xt_arglist, xt_numargs);
    else if (sb_kind == k_dialog)
      widget_id = XmCreateSelectionDialog(parent_widget_id, name, xt_arglist, xt_numargs);
    else if (sb_kind == k_prompt_dialog)
      widget_id = XmCreatePromptDialog(parent_widget_id, name, xt_arglist, xt_numargs);
    else
      xlfatal("Bug in Xm_Selection_Box_Widget_Class_Method_ISNEW()");
    Wres_Free_C_Arglist_Data();
  }
  else
    if (sb_kind == NIL)
      widget_id = XmCreateSelectionBox(parent_widget_id, name, ARGLIST());
    else if (sb_kind == k_dialog)
      widget_id = XmCreateSelectionDialog(parent_widget_id, name, ARGLIST());
    else if (sb_kind == k_prompt_dialog)
      widget_id = XmCreatePromptDialog(parent_widget_id, name, ARGLIST());
    else
      xlfatal("Bug in Xm_Selection_Box_Widget_Class_Method_ISNEW()");

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
 * how to get values from the XmSelectionBoxCallbackStruct.
 * Specifying one or more of the following symbols in the callback bindings 
 * list will bind that symbol's value in the lexical environment of the callback:
 * CALLBACK_WIDGET
 * CALLBACK_REASON
 * CALLBACK_XEVENT
 * CALLBACK_VALUE
 * CALLBACK_LENGTH
 ******************************************************************************/
LVAL Xm_Selection_Box_Widget_Class_Method_ADD_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */
  extern void Wcb_XmSelectionBoxCallbackStruct_Callbackproc(); /* w_callbacks.c */
  return(Wcb_Meta_Method_Add_Callback(Wcb_XmSelectionBoxCallbackStruct_Callbackproc, FALSE));
}


/******************************************************************************
 * Same as WIDGET_CLASS's :set_callback method except that this understands
 * how to get values from the XmSelectionBoxCallbackStruct.
 * Specifying one or more of the following symbols in the callback bindings 
 * list will bind that symbol's value in the lexical environment of the callback:
 * CALLBACK_WIDGET
 * CALLBACK_REASON
 * CALLBACK_XEVENT
 * CALLBACK_VALUE
 * CALLBACK_LENGTH
 ******************************************************************************/
LVAL Xm_Selection_Box_Widget_Class_Method_SET_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */
  extern void Wcb_XmSelectionBoxCallbackStruct_Callbackproc(); /* w_callbacks.c */
  return (Wcb_Meta_Method_Add_Callback(Wcb_XmSelectionBoxCallbackStruct_Callbackproc, TRUE));
}


/******************************************************************************
 * (send <selectionbox_widget> :get_child <childname>)
 * ==> returns the WIDGETOBJ correspomding to <childname> which can be one
 * of the following:
 * :DIALOG_LIST, :DIALOG_LIST_LABEL, :DIALOG_SELECTION_LABEL, :DIALOG_WORK_AREA
 * :DIALOG_TEXT, :DIALOG_SEPARATOR, :DIALOG_OK_BUTTON, :DIALOG_APPLY_BUTTON,
 * :DIALOG_CANCEL_BUTTON, :DIALOG_HELP_BUTTON, :DIALOG_DEFAULT_BUTTON
 *
 * Widget XmSelectionBoxGetChild (sb, which)
 *        Widget sb;
 *        int which;
 ******************************************************************************/
LVAL Xm_Selection_Box_Widget_Class_Method_GET_CHILD()
{
  LVAL self, lval_child;
  Widget widget_id;
  int child;
  extern LVAL Wcls_WidgetID_To_WIDGETOBJ();
  extern LVAL s_XmDIALOG_LIST, s_XmDIALOG_LIST_LABEL, s_XmDIALOG_SELECTION_LABEL,
  s_XmDIALOG_WORK_AREA, s_XmDIALOG_TEXT, s_XmDIALOG_SEPARATOR,
  s_XmDIALOG_OK_BUTTON, s_XmDIALOG_APPLY_BUTTON, s_XmDIALOG_CANCEL_BUTTON,
  s_XmDIALOG_HELP_BUTTON, s_XmDIALOG_DEFAULT_BUTTON; /* from w_resources.c */
  
  self = xlga_widgetobj();
  lval_child = xlgasymbol();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  if (lval_child == s_XmDIALOG_LIST)
    child = XmDIALOG_LIST;
  else if (lval_child == s_XmDIALOG_LIST_LABEL)
    child = XmDIALOG_LIST_LABEL;
  else if (lval_child == s_XmDIALOG_SELECTION_LABEL)
    child = XmDIALOG_SELECTION_LABEL;
  else if (lval_child == s_XmDIALOG_WORK_AREA)
    child = XmDIALOG_WORK_AREA;
  else if (lval_child == s_XmDIALOG_TEXT)
    child = XmDIALOG_TEXT;
  else if (lval_child == s_XmDIALOG_SEPARATOR)
    child = XmDIALOG_SEPARATOR;
  else if (lval_child == s_XmDIALOG_OK_BUTTON)
    child = XmDIALOG_OK_BUTTON;
  else if (lval_child == s_XmDIALOG_APPLY_BUTTON)
    child = XmDIALOG_APPLY_BUTTON;
  else if (lval_child == s_XmDIALOG_CANCEL_BUTTON)
    child = XmDIALOG_CANCEL_BUTTON; 
  else if (lval_child == s_XmDIALOG_HELP_BUTTON)
    child = XmDIALOG_HELP_BUTTON;
  else if (lval_child == s_XmDIALOG_DEFAULT_BUTTON)
    child =XmDIALOG_DEFAULT_BUTTON;
  else
    xlerror("Invalid selectionbox child symbol.", lval_child);
    
  return (Wcls_WidgetID_To_WIDGETOBJ(XmSelectionBoxGetChild(widget_id, child)));
}


/******************************************************************************
 *
 ******************************************************************************/
Wc_SelectioB_Init()
{
  LVAL o_XM_SELECTION_BOX_WIDGET_CLASS;
  extern LVAL Wcls_Create_Subclass_Of_WIDGET_CLASS(); /* w_classes.c */
  extern      xladdmsg();	/* from xlobj.c */

  k_prompt_dialog      = xlenter(":PROMPT_DIALOG");

  o_XM_SELECTION_BOX_WIDGET_CLASS =
    Wcls_Create_Subclass_Of_WIDGET_CLASS("XM_SELECTION_BOX_WIDGET_CLASS",
					 xmSelectionBoxWidgetClass);

  /* a special :isnew method on this class allows for the creation of this
     widget inside a popup shell if one of the following submessage keywords
     are given: 
     :dialog, :prompt_dialog */
  xladdmsg(o_XM_SELECTION_BOX_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Xm_Selection_Box_Widget_Class_Method_ISNEW);

  xladdmsg(o_XM_SELECTION_BOX_WIDGET_CLASS, ":ADD_CALLBACK",
           FTAB_Xm_Selection_Box_Widget_Class_Method_ADD_CALLBACK);

  xladdmsg(o_XM_SELECTION_BOX_WIDGET_CLASS, ":SET_CALLBACK",
           FTAB_Xm_Selection_Box_Widget_Class_Method_SET_CALLBACK);

  xladdmsg(o_XM_SELECTION_BOX_WIDGET_CLASS, ":GET_CHILD",
           FTAB_Xm_Selection_Box_Widget_Class_Method_GET_CHILD);
}
