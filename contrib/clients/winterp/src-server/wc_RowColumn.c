/* -*-C-*-
********************************************************************************
*
* File:         wc_RowColumn.c
* RCS:          $Header: wc_RowColumn.c,v 1.1 89/11/25 04:48:46 mayer Exp $
* Description:  XM_ROW_COLUMN_WIDGET_CLASS
* Author:       Niels Mayer, HPLabs
* Created:      Sat Oct 28 04:28:02 1989
* Modified:     Sat Nov 25 04:48:36 1989 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: wc_RowColumn.c,v 1.1 89/11/25 04:48:46 mayer Exp $";

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_funtab.h"

static LVAL k_radio_box;
static LVAL k_option_menu;
static LVAL k_menu_bar;
static LVAL k_popup_menu;
static LVAL k_pulldown_menu;


/*****************************************************************************
 * (send XM_ROW_COLUMN_WIDGET_CLASS :new 
 *                           [:managed/:unmanaged]
 *                           [:radio_box/:option_menu/:menu_bar/
 *                            :popup_menu,:pulldown_menu]
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
 *     (send XM_ROW_COLUMN_WIDGET_CLASS :new ...)
 *     -->  XmCreateRowColumn();
 *     (send XM_ROW_COLUMN_WIDGET_CLASS :new :radio_box ...)
 *     -->  XmCreateRadioBox();
 *     (send XM_ROW_COLUMN_WIDGET_CLASS :new :option_menu ...)
 *     --> XmCreateOptionMenu();
 *     (send XM_ROW_COLUMN_WIDGET_CLASS :new :menu_bar ...)
 *     --> XmCreateMenuBar();
 *     (send XM_ROW_COLUMN_WIDGET_CLASS :new :popup_menu ...)
 *     --> XmCreatePopupMenu();
 *     (send XM_ROW_COLUMN_WIDGET_CLASS :new :pulldown_menu ...)
 *     --> XmCreatePulldownMenu();
 ****************************************************************************/
LVAL Xm_Row_Column_Widget_Class_Method_ISNEW()
{
  extern ArgList Wres_Get_LispArglist(); /* from w_resources.c */
  extern void    Wres_Free_C_Arglist_Data(); /* from w_resources.c */
  extern LVAL k_managed, k_unmanaged;
  LVAL self, o_parent;
  char* name;
  Boolean managed_p;
  LVAL rc_kind;
  Widget widget_id, parent_widget_id;

  self = xlgaobject();		/* NOTE: xlobj.c:clnew() returns an OBJECT; if this method
				   returns successfully, it will return a WIDGETOBJ */
  
  /* get optional managed/unmanaged arg */
  if (moreargs() && ((*xlargv == k_managed) || (*xlargv == k_unmanaged)))
    managed_p = (nextarg() == k_managed);
  else
    managed_p = FALSE;		/* by default don't call XtManageChild() */

  /* get optional :radio_box/:option_menu... arg */
  if (moreargs() && ((*xlargv == k_radio_box)   ||
		     (*xlargv == k_option_menu) ||
		     (*xlargv == k_menu_bar)    ||
		     (*xlargv == k_popup_menu)  ||
		     (*xlargv == k_pulldown_menu)))
    rc_kind = nextarg();
  else
    rc_kind = NIL;		/* default is XmCreateRowColumn() */

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
    if (rc_kind == NIL)
      widget_id = XmCreateRowColumn(parent_widget_id, name, xt_arglist, xt_numargs);
    else if (rc_kind == k_radio_box)
      widget_id = XmCreateRadioBox(parent_widget_id, name, xt_arglist, xt_numargs);
    else if (rc_kind == k_option_menu)
      widget_id = XmCreateOptionMenu(parent_widget_id, name, xt_arglist, xt_numargs);
    else if (rc_kind == k_menu_bar)
      widget_id = XmCreateMenuBar(parent_widget_id, name, xt_arglist, xt_numargs);
    else if (rc_kind == k_popup_menu)
      widget_id = XmCreatePopupMenu(parent_widget_id, name, xt_arglist, xt_numargs);
    else if (rc_kind == k_pulldown_menu)
      widget_id = XmCreatePulldownMenu(parent_widget_id, name, xt_arglist, xt_numargs);	   
    else
      xlfatal("Bug in Xm_Row_Column_Widget_Class_Method_ISNEW()");
    Wres_Free_C_Arglist_Data();
  }
  else 
    if (rc_kind == NIL)
      widget_id = XmCreateRowColumn(parent_widget_id, name, ARGLIST());
    else if (rc_kind == k_radio_box)
      widget_id = XmCreateRadioBox(parent_widget_id, name, ARGLIST());
    else if (rc_kind == k_option_menu)
      widget_id = XmCreateOptionMenu(parent_widget_id, name, ARGLIST());
    else if (rc_kind == k_menu_bar)
      widget_id = XmCreateMenuBar(parent_widget_id, name, ARGLIST());
    else if (rc_kind == k_popup_menu)
      widget_id = XmCreatePopupMenu(parent_widget_id, name, ARGLIST());
    else if (rc_kind == k_pulldown_menu)
      widget_id = XmCreatePulldownMenu(parent_widget_id, name, ARGLIST());	   
    else
      xlfatal("Bug in Xm_Row_Column_Widget_Class_Method_ISNEW()");

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
 * how to get values from the XmRowColumnCallbackStruct.
 * Specifying one or more of the following symbols in the callback bindings 
 * list will bind that symbol's value in the lexical environment of the callback:
 * CALLBACK_WIDGET
 * CALLBACK_REASON
 * CALLBACK_XEVENT
 * CALLBACK_ENTRY_WIDGET
 * CALLBACK_ENTRY_REASON
 * CALLBACK_ENTRY_SET
 * CALLBACK_ENTRY_DATA
 ******************************************************************************/
LVAL Xm_Row_Column_Widget_Class_Method_ADD_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */
  extern void Wcb_XmRowColumnCallbackStruct_Callbackproc(); /* w_callbacks.c */
  return(Wcb_Meta_Method_Add_Callback(Wcb_XmRowColumnCallbackStruct_Callbackproc, FALSE));
}


/******************************************************************************
 * Same as WIDGET_CLASS's :set_callback method except that this understands
 * how to get values from the XmRowColumnCallbackStruct.
 * Specifying one or more of the following symbols in the callback bindings 
 * list will bind that symbol's value in the lexical environment of the callback:
 * CALLBACK_WIDGET
 * CALLBACK_REASON
 * CALLBACK_XEVENT
 * CALLBACK_ENTRY_WIDGET
 * CALLBACK_ENTRY_REASON
 * CALLBACK_ENTRY_SET
 * CALLBACK_ENTRY_DATA
 ******************************************************************************/
LVAL Xm_Row_Column_Widget_Class_Method_SET_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */
  extern void Wcb_XmRowColumnCallbackStruct_Callbackproc(); /* w_callbacks.c */
  return (Wcb_Meta_Method_Add_Callback(Wcb_XmRowColumnCallbackStruct_Callbackproc, TRUE));
}


/******************************************************************************
 * (send <popup_menu> :MENU_POSITION <event>)
 *      ==> returns <popup_menu>
 * This method will position a popup menupane at the position specified by
 * event->x_root, event->y_root, where <event> is an XEvent. This
 * method should only be called on a row_column widget that was created via
 * (send XM_ROW_COLUMN_WIDGET_CLASS :new :popup_menu ...).
 *
 * Possible design change: should I make a POPUP_MENU_WIDGET_CLASS that
 * has this as one of it's methods, and don't provide this method on
 * other rowcolumn widgets?
 *
 * void XmMenuPosition (p, event)
 *     Widget  p;
 *     XButtonPressedEvent *event;
 ******************************************************************************/
LVAL Xm_Row_Column_Widget_Class_Method_MENU_POSITION()
{
  LVAL self;
  Widget widget_id;
  XEvent *event;

  self = xlga_widgetobj();
  event = get_xevent(xlga_xevent());
  xllastarg();
  
  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  XmMenuPosition(widget_id, event);
  
  return (self);
}


/******************************************************************************
 * (send <option_menu> :OPTION_LABEL_GADGET)
 *      ==> this method returns the WIDGETOBJ corresponding to the label
 *          gadget created in an <option_menu> instance. An <option_menu>
 *          rowcolumn instance can be created by doing
 *          (send XM_ROW_COLUMN_WIDGET_CLASS :new :option_menu ...)
 *
 * Widget XmOptionLabelGadget (m)
 *     Widget   m;
 ******************************************************************************/
LVAL Xm_Row_Column_Widget_Class_Method_OPTION_LABEL_GADGET()
{
  extern LVAL Wcls_WidgetID_To_WIDGETOBJ();
  LVAL self;
  Widget widget_id;

  self = xlga_widgetobj();
  xllastarg();
  
  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  return (Wcls_WidgetID_To_WIDGETOBJ(XmOptionLabelGadget(widget_id)));
}


/******************************************************************************
* (send <option_menu> :OPTION_BUTTON_GADGET)
 *      ==> this method returns the WIDGETOBJ corresponding to the cascade
 *          button gadget created in an <option_menu> instance. An
 *          <option_menu> rowcolumn instance can be created by doing
 *          (send XM_ROW_COLUMN_WIDGET_CLASS :new :option_menu ...)
 *
 * Widget XmOptionButtonGadget (m)
 *     Widget   m;
 ******************************************************************************/
LVAL Xm_Row_Column_Widget_Class_Method_OPTION_BUTTON_GADGET()
{
  extern LVAL Wcls_WidgetID_To_WIDGETOBJ();
  LVAL self;
  Widget widget_id;

  self = xlga_widgetobj();
  xllastarg();
  
  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  return (Wcls_WidgetID_To_WIDGETOBJ(XmOptionLabelGadget(widget_id)));
}


/******************************************************************************
 *
 ******************************************************************************/
Wc_RowColumn_Init()
{
  LVAL o_XM_ROW_COLUMN_WIDGET_CLASS;
  extern LVAL Wcls_Create_Subclass_Of_WIDGET_CLASS(); /* w_classes.c */
  extern      xladdmsg();	/* from xlobj.c */

  k_radio_box          = xlenter(":RADIO_BOX");
  k_option_menu        = xlenter(":OPTION_MENU");
  k_menu_bar           = xlenter(":MENU_BAR");
  k_popup_menu         = xlenter(":POPUP_MENU");
  k_pulldown_menu      = xlenter(":PULLDOWN_MENU");

  o_XM_ROW_COLUMN_WIDGET_CLASS =
    Wcls_Create_Subclass_Of_WIDGET_CLASS("XM_ROW_COLUMN_WIDGET_CLASS",
					 xmRowColumnWidgetClass);

  /* a special :isnew method on this class allows for the creation of this
     widget inside a popup shell if one of the following submessage keywords
     are given: 
     :radio_box, :option_menu, :menu_bar, :popup_menu, :pulldown_menu */
  xladdmsg(o_XM_ROW_COLUMN_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Xm_Row_Column_Widget_Class_Method_ISNEW);

  xladdmsg(o_XM_ROW_COLUMN_WIDGET_CLASS, ":ADD_CALLBACK",
           FTAB_Xm_Row_Column_Widget_Class_Method_ADD_CALLBACK);

  xladdmsg(o_XM_ROW_COLUMN_WIDGET_CLASS, ":SET_CALLBACK",
           FTAB_Xm_Row_Column_Widget_Class_Method_SET_CALLBACK);

  xladdmsg(o_XM_ROW_COLUMN_WIDGET_CLASS, ":MENU_POSITION",
           FTAB_Xm_Row_Column_Widget_Class_Method_MENU_POSITION);

  xladdmsg(o_XM_ROW_COLUMN_WIDGET_CLASS, ":OPTION_LABEL_GADGET",
           FTAB_Xm_Row_Column_Widget_Class_Method_OPTION_LABEL_GADGET);

  xladdmsg(o_XM_ROW_COLUMN_WIDGET_CLASS, ":OPTION_BUTTON_GADGET",
           FTAB_Xm_Row_Column_Widget_Class_Method_OPTION_BUTTON_GADGET);

}
