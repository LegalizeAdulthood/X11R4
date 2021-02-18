/* -*-C-*-
********************************************************************************
*
* File:         wc_FileSB.c
* RCS:          $Header: wc_FileSB.c,v 1.1 89/11/25 04:42:12 mayer Exp $
* Description:  XM_FILE_SELECTION_BOX_WIDGET_CLASS
* Author:       Niels Mayer, HPLabs
* Created:      Fri Oct 27 22:29:33 1989
* Modified:     Sat Nov 25 04:41:23 1989 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       Experimental (Do Not Distribute)
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
static char rcs_identity[] = "@(#)$Header: wc_FileSB.c,v 1.1 89/11/25 04:42:12 mayer Exp $";

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/FileSB.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_funtab.h"


/*****************************************************************************
 * (send XM_FILE_SELECTION_BOX_WIDGET_CLASS :new 
 *                           [:managed/:unmanaged]
 *                           [:dialog]
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
 * (send XM_FILE_SELECTION_BOX_WIDGET_CLASS :new ...)
 *   --> XmCreateFileSelectionBox();
 * (send XM_FILE_SELECTION_BOX_WIDGET_CLASS :new :dialog ...)
 *   --> XmCreateFileSelectionDialog();
 ****************************************************************************/
LVAL Xm_File_Selection_Box_Widget_Class_Method_ISNEW()
{
  extern ArgList Wres_Get_LispArglist(); /* from w_resources.c */
  extern void    Wres_Free_C_Arglist_Data(); /* from w_resources.c */
  extern LVAL k_managed, k_unmanaged, k_dialog;
  LVAL self, o_parent;
  char* name;
  Boolean managed_p, dialog_p;
  Widget widget_id, parent_widget_id;

  self = xlgaobject();		/* NOTE: xlobj.c:clnew() returns an OBJECT; if this method
				   returns successfully, it will return a WIDGETOBJ */

  /* get optional managed/unmanaged arg */
  if (moreargs() && ((*xlargv == k_managed) || (*xlargv == k_unmanaged)))
    managed_p = (nextarg() == k_managed);
  else
    managed_p = FALSE;		/* by default don't call XtManageChild() */

  /* get optional :dialog arg */
  if (moreargs() && (*xlargv == k_dialog)) {
    nextarg();
    dialog_p = TRUE;
  }
  else
    dialog_p = FALSE;		/* by default, we don't want a dialog widget */

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
    if (dialog_p)
      widget_id = XmCreateFileSelectionDialog(parent_widget_id, name, xt_arglist, xt_numargs);
    else
      widget_id = XmCreateFileSelectionBox(parent_widget_id, name, xt_arglist, xt_numargs);
    Wres_Free_C_Arglist_Data();
  }
  else 
    if (dialog_p)
      widget_id = XmCreateFileSelectionDialog(parent_widget_id, name, ARGLIST());
    else
      widget_id = XmCreateFileSelectionBox(parent_widget_id, name, ARGLIST());

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
 * how to get values from the XmFileSelectionBoxCallbackStruct.
 * Specifying one or more of the following symbols in the callback bindings 
 * list will bind that symbol's value in the lexical environment of the callback:
 * CALLBACK_WIDGET
 * CALLBACK_REASON
 * CALLBACK_XEVENT
 * CALLBACK_VALUE
 * CALLBACK_LENGTH
 * CALLBACK_MASK
 * CALLBACK_MASK_LENGTH
 ******************************************************************************/
LVAL Xm_File_Selection_Box_Widget_Class_Method_ADD_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */
  extern void Wcb_XmFileSelectionBoxCallbackStruct_Callbackproc(); /* w_callbacks.c */
  return(Wcb_Meta_Method_Add_Callback(Wcb_XmFileSelectionBoxCallbackStruct_Callbackproc, FALSE));
}


/******************************************************************************
 * Same as WIDGET_CLASS's :set_callback method except that this understands
 * how to get values from the XmFileSelectionBoxCallbackStruct.
 * Specifying one or more of the following symbols in the callback bindings 
 * list will bind that symbol's value in the lexical environment of the callback:
 * CALLBACK_WIDGET
 * CALLBACK_REASON
 * CALLBACK_XEVENT
 * CALLBACK_VALUE
 * CALLBACK_LENGTH
 * CALLBACK_MASK
 * CALLBACK_MASK_LENGTH
 ******************************************************************************/
LVAL Xm_File_Selection_Box_Widget_Class_Method_SET_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */
  extern void Wcb_XmFileSelectionBoxCallbackStruct_Callbackproc(); /* w_callbacks.c */
  return (Wcb_Meta_Method_Add_Callback(Wcb_XmFileSelectionBoxCallbackStruct_Callbackproc, TRUE));
}


/******************************************************************************
 * (send <fileselboxwidget> :GET_CHILD <child_sym>)
 * This method returns a WIDGETOBJ corresponding to one of
 * <fileselboxwidget> children. <child_sym> can be one of:
 *	:DIALOG_APPLY_BUTTON
 *	:DIALOG_CANCEL_BUTTON
 *	:DIALOG_DEFAULT_BUTTON
 *	:DIALOG_FILTER_LABEL
 *	:DIALOG_FILTER_TEXT
 *	:DIALOG_HELP_BUTTON
 *	:DIALOG_LIST
 *	:DIALOG_LIST_LABEL
 *	:DIALOG_OK_BUTTON
 *	:DIALOG_SELECTION_LABEL
 *	:DIALOG_TEXT
 *
 *  Widget XmFileSelectionBoxGetChild (fs, which)
 *  Widget fs;	         -- SelectionBox widget 
 *  unsigned char which; --  which child
 ******************************************************************************/
LVAL Xm_File_Selection_Box_Widget_Class_Method_GET_CHILD()
{
  extern LVAL Wcls_WidgetID_To_WIDGETOBJ();
  extern LVAL s_XmDIALOG_FILTER_LABEL, s_XmDIALOG_FILTER_TEXT,
  s_XmDIALOG_LIST, s_XmDIALOG_LIST_LABEL, s_XmDIALOG_SELECTION_LABEL, 
  s_XmDIALOG_WORK_AREA, s_XmDIALOG_TEXT, s_XmDIALOG_SEPARATOR,
  s_XmDIALOG_OK_BUTTON, s_XmDIALOG_APPLY_BUTTON, s_XmDIALOG_CANCEL_BUTTON,
  s_XmDIALOG_HELP_BUTTON, s_XmDIALOG_DEFAULT_BUTTON;
  LVAL self, lval_child;
  Widget widget_id;
  unsigned char child;

  self = xlga_widgetobj();
  lval_child = xlgasymbol();
  xllastarg();
  
  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  if (lval_child == s_XmDIALOG_FILTER_LABEL)
    child = XmDIALOG_FILTER_LABEL;
  else if (lval_child == s_XmDIALOG_FILTER_TEXT)
    child = XmDIALOG_FILTER_TEXT;
  else if (lval_child == s_XmDIALOG_LIST)
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
    child = XmDIALOG_DEFAULT_BUTTON;
  else 
    xlerror("FILE_SELECTION_BOX_WIDGET_CLASS method :GET_CHILD -- unknown child type.", lval_child);

  return (Wcls_WidgetID_To_WIDGETOBJ(XmFileSelectionBoxGetChild(widget_id, child))); 
}


/******************************************************************************
 * (send <fileselboxwidget> :DO_SEARCH <dirmask>)
 * this method initiates a directory search. The search will be according to 
 * the string or XmString <dirmask>. If <dirmask> is NIL, then the search will
 * use the current directory mask. This method returns <dirmask> as an XmString.
 *
 * void XmFileSelectionDoSearch(fs, dirmask)
 * Widget fs;
 * XmString   dirmask;
 ******************************************************************************/
LVAL Xm_File_Selection_Box_Widget_Class_Method_DO_SEARCH()
{
  LVAL self, lval_dirmask;
  Widget widget_id;
  XmString dirmask;
  extern XmString Get_String_or_XmString_Arg_Returning_XmString(); /* w_XmString.c */

  self = xlga_widgetobj();
  if (moreargs() && (*xlargv))
    dirmask = Get_String_or_XmString_Arg_Returning_XmString(&lval_dirmask);
  else
    dirmask = NULL;
  xllastarg();
  
  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  XmFileSelectionDoSearch(widget_id, dirmask);
  
  return (lval_dirmask);
}

/******************************************************************************
 *
 ******************************************************************************/
Wc_FileSB_Init()
{
  LVAL o_XM_FILE_SELECTION_BOX_WIDGET_CLASS;
  extern LVAL Wcls_Create_Subclass_Of_WIDGET_CLASS(); /* w_classes.c */
  extern      xladdmsg();	/* from xlobj.c */

  o_XM_FILE_SELECTION_BOX_WIDGET_CLASS =
    Wcls_Create_Subclass_Of_WIDGET_CLASS("XM_FILE_SELECTION_BOX_WIDGET_CLASS",
					 xmFileSelectionBoxWidgetClass);

  /* a special :isnew method on this class allows for the creation of this
     widget in a popup dialog if the submessage keyword :dialog is given ... */
  xladdmsg(o_XM_FILE_SELECTION_BOX_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Xm_File_Selection_Box_Widget_Class_Method_ISNEW);

  xladdmsg(o_XM_FILE_SELECTION_BOX_WIDGET_CLASS, ":ADD_CALLBACK",
           FTAB_Xm_File_Selection_Box_Widget_Class_Method_ADD_CALLBACK);

  xladdmsg(o_XM_FILE_SELECTION_BOX_WIDGET_CLASS, ":SET_CALLBACK",
           FTAB_Xm_File_Selection_Box_Widget_Class_Method_SET_CALLBACK);

  xladdmsg(o_XM_FILE_SELECTION_BOX_WIDGET_CLASS, ":GET_CHILD",
           FTAB_Xm_File_Selection_Box_Widget_Class_Method_GET_CHILD);

  xladdmsg(o_XM_FILE_SELECTION_BOX_WIDGET_CLASS, ":DO_SEARCH ",
	   FTAB_Xm_File_Selection_Box_Widget_Class_Method_DO_SEARCH);
}
