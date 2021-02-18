/* -*-C-*-
********************************************************************************
*
* File:         wc_SHELL.c
* RCS:          $Header: wc_SHELL.c,v 1.1 89/11/25 04:49:31 mayer Exp $
* Description:  SHELL_WIDGET/POPUP_SHELL_WIDGET metaclass + class instances.
* Author:       Niels Mayer, HPLabs
* Created:      Fri Oct 27 22:03:22 1989
* Modified:     Sat Nov 25 04:49:23 1989 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: wc_SHELL.c,v 1.1 89/11/25 04:49:31 mayer Exp $";

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/MenuShell.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_funtab.h"

/*****************************************************************************
 * (send <Shell_Widget_Class> :new
 *                                 [<name>]
 *                                 [:XMN_<arg1> <val1>]
 *                                 [. . .             ]
 *                                 [:XMN_<argN> <valN>])
 *
 * For the <Shell_Widget_Class> metaclass,
 * create a new widget via XtCreateApplicationShell().
 ****************************************************************************/
LVAL Shell_Widget_Class_Method_ISNEW()
{
  extern ArgList Wres_Get_LispArglist(); /* from w_resources.c */
  extern void    Wres_Free_C_Arglist_Data(); /* from w_resources.c */
  extern WidgetClass Wcls_WIDGETCLASSOBJ_To_WidgetClassID(); /* w_classes.c */
  extern Widget toplevel_Wgt;	/* from winterp.c -- see "NOTE POTENTIAL BUG" comment below */
  LVAL self;
  char* name;
  WidgetClass widgetclass_id;
  Widget widget_id;

  self = xlgaobject();		/* NOTE: xlobj.c:clnew() returns an OBJECT; if this method
				   returns successfully, it will return a WIDGETOBJ */

  /* get optional <name> arg */
  if (moreargs() && (stringp(*xlargv)))
    name = (char*) getstring(nextarg());
  else
    name = "";			/* default name */

  if (!(widgetclass_id = Wcls_WIDGETCLASSOBJ_To_WidgetClassID(getclass(self))))
    xlerror("Expected a 'Class' object that is a subclass of 'WIDGET_CLASS'.", self);

  /*
   * Store the widget object <self> in the XmNuserData resource on the
   * widget. This will allow us to retrieve the widget object from Xtoolkit
   * functions returning widget ID's without having to keep around a table
   * of widgetID-->widget-objects.
   */
   ARGLIST_RESET(); ARGLIST_ADD(XmNuserData, self); 
  
  if (moreargs()) {		/* if there are more arguments, */
    Cardinal xt_numargs;	/* then we have some extra widget resources to set */
    ArgList xt_arglist = Wres_Get_LispArglist(self, toplevel_Wgt, ARGLIST(), &xt_numargs);

    /* NOTE POTENTIAL BUG: above, we pass toplevel_Wgt as the widget for
       Wres_Get_LispArglist(). toplevel_Wgt is created by XtInitialize() in
       winterp.c:main(). Wres_Get_LispArglist() may need to use that widget
       for particular kinds of argument conversions done through XtConvert().
       These resource converters reference things like the display/screen/
       foreground/background. For those cases, do not attempt to invoke
       XtConvert() by passing string-valued arguments to this method. Instead,
       create the shell widget, and then use :set_values to set the arguments
       requiring such conversions. This will really break bigtime if I ever
       implement multiple AppContexts for multiscreen usage of WINTERP */

    widget_id = XtCreateApplicationShell(name, widgetclass_id, xt_arglist, xt_numargs);
    Wres_Free_C_Arglist_Data();
  }
  else
    widget_id = XtCreateApplicationShell(name, widgetclass_id, ARGLIST());

  Wcls_Initialize_WIDGETOBJ(self, widget_id);
  
#ifdef DEBUG_WINTERP_1
  Wcls_Print_WidgetObj_Info(self);
#endif
  return (self);
}


/*****************************************************************************
 * (send <Popup_Shell_Widget_Class> :new
 *                                       [<name>]
 *                                       <parent> 
 *                                       [:XMN_<arg1> <val1>]
 *                                       [. . .             ]
 *                                       [:XMN_<argN> <valN>])
 *
 * For the <Popup_Shell_Widget_Class> meta-class,
 * create a new widget via XtCreatePopupShell().
 ****************************************************************************/
LVAL Popup_Shell_Widget_Class_Method_ISNEW()
{
  extern ArgList Wres_Get_LispArglist(); /* from w_resources.c */
  extern void    Wres_Free_C_Arglist_Data(); /* from w_resources.c */
  extern WidgetClass Wcls_WIDGETCLASSOBJ_To_WidgetClassID(); /* w_classes.c */
  LVAL self;
  LVAL o_parent;
  char* name;
  WidgetClass widgetclass_id;
  Widget parent_widget_id, widget_id;

  self = xlgaobject();		/* NOTE: xlobj.c:clnew() returns an OBJECT; if this method
				   returns successfully, it will return a WIDGETOBJ */

  /* get optional <name> arg */
  if (moreargs() && (stringp(*xlargv)))
    name = (char*) getstring(nextarg());
  else
    name = "";			/* default name */

  /* get required <parent> widget-object arg */
  o_parent = xlga_widgetobj();

  if (!(parent_widget_id = get_widgetobj_widgetID(o_parent)))
    xlerror("<parent> widget object has been :destroy'd or hasn't been initialized by :isnew.", o_parent);

  if (!(widgetclass_id = Wcls_WIDGETCLASSOBJ_To_WidgetClassID(getclass(self))))
    xlerror("Expected a 'Class' object that is a subclass of 'WIDGET_CLASS'.", self);

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
    widget_id = XtCreatePopupShell(name, widgetclass_id, parent_widget_id, xt_arglist, xt_numargs);
    Wres_Free_C_Arglist_Data();
  }
  else
    widget_id = XtCreatePopupShell(name, widgetclass_id, parent_widget_id, ARGLIST());

  Wcls_Initialize_WIDGETOBJ(self, widget_id);

#ifdef DEBUG_WINTERP_1
  Wcls_Print_WidgetObj_Info(self);
#endif
  return (self);
}


/*****************************************************************************
 * (send <Popup_Shell_Widget_Instance> :popup <grabkind>)
 * where <grabkind> is one of the following keyword symbols: 
 *                  :grab_none, :grab_nonexclusive, or  :grab_exclusive.
 ****************************************************************************/
static LVAL k_grab_none;	/* initialized in Wc_SHELL_Init() */
static LVAL k_grab_nonexclusive; /* initialized in Wc_SHELL_Init() */
static LVAL k_grab_exclusive;	/* initialized in Wc_SHELL_Init() */
LVAL Popup_Shell_Widget_Class_Method_POPUP()
{
  LVAL self, grabkind;
  Widget widget_id;

  self = xlga_widgetobj();
  grabkind = xlgasymbol();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  if (grabkind == k_grab_none)
    XtPopup(widget_id, XtGrabNone);
  else if (grabkind == k_grab_nonexclusive)
    XtPopup(widget_id, XtGrabNonexclusive);
  else if (grabkind == k_grab_exclusive)
    XtPopup(widget_id, XtGrabExclusive);
  else
    xlerror("Unknown <grabkind> symbol. (Expected :grab_none, :grab_nonexclusive, or  :grab_exclusive)", grabkind);

  return (self);
}


/*****************************************************************************
 * (send <Popup_Shell_Widget_Instance> :popdown)
 ****************************************************************************/
LVAL Popup_Shell_Widget_Class_Method_POPDOWN()
{
  LVAL self;
  Widget widget_id;
  self = xlga_widgetobj();
  xllastarg();
  
  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  XtPopdown(widget_id);
  return (self);
}


/*****************************************************************************
 * (send <Shell_Widget_Instance> :realize)
 ****************************************************************************/
LVAL Shell_Widget_Class_Method_REALIZE()
{
  LVAL self;
  Widget widget_id;
  self = xlga_widgetobj();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  XtRealizeWidget(widget_id);
  return (self);
}


/*****************************************************************************
 * (send <Shell_Widget_Instance> :unrealize)
 ****************************************************************************/
LVAL Shell_Widget_Class_Method_UNREALIZE()
{
  LVAL self;
  Widget widget_id;
  self = xlga_widgetobj();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  XtUnrealizeWidget(widget_id);
  return (self);
}


/******************************************************************************
 * (send <Shell_Widget_Instance> :IS_MOTIF_WM_RUNNING)
 * returns T if mwm is running, else NIL.
 *
 * extern Boolean XmIsMotifWMRunning(shell);
 *        Widget shell;
 *******************************************************************************/
LVAL Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;
  self = xlga_widgetobj();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  return (XmIsMotifWMRunning(widget_id) ? true : NIL);
}


/******************************************************************************
 *
 ******************************************************************************/
Wc_SHELL_Init()
{
  LVAL o_TOP_LEVEL_SHELL_WIDGET_CLASS;
  LVAL o_TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS;
  LVAL o_APPLICATION_SHELL_WIDGET_CLASS;
  LVAL o_APPLICATION_POPUP_SHELL_WIDGET_CLASS;
  LVAL o_OVERRIDE_SHELL_WIDGET_CLASS;
  LVAL o_OVERRIDE_POPUP_SHELL_WIDGET_CLASS;
  LVAL o_TRANSIENT_SHELL_WIDGET_CLASS;
  LVAL o_TRANSIENT_POPUP_SHELL_WIDGET_CLASS;
  LVAL o_XM_DIALOG_POPUP_SHELL_WIDGET_CLASS;
  LVAL o_XM_MENU_POPUP_SHELL_WIDGET_CLASS;
  extern LVAL Wcls_Create_Subclass_Of_WIDGET_CLASS(); /* w_classes.c */
  extern      xladdmsg();	/* from xlobj.c */

  k_grab_none          = xlenter(":GRAB_NONE");
  k_grab_nonexclusive  = xlenter(":GRAB_NONEXCLUSIVE");
  k_grab_exclusive     = xlenter(":GRAB_EXCLUSIVE");

  /*------------------- TOP_LEVEL_SHELL_WIDGET_CLASS -----------------------*/
  o_TOP_LEVEL_SHELL_WIDGET_CLASS = 
    Wcls_Create_Subclass_Of_WIDGET_CLASS("TOP_LEVEL_SHELL_WIDGET_CLASS",
					 topLevelShellWidgetClass);
  xladdmsg(o_TOP_LEVEL_SHELL_WIDGET_CLASS, ":IS_MOTIF_WM_RUNNING", 
	   FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING);
  xladdmsg(o_TOP_LEVEL_SHELL_WIDGET_CLASS, ":UNREALIZE", 
	   FTAB_Shell_Widget_Class_Method_UNREALIZE);
  xladdmsg(o_TOP_LEVEL_SHELL_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Shell_Widget_Class_Method_ISNEW);
  xladdmsg(o_TOP_LEVEL_SHELL_WIDGET_CLASS, ":REALIZE", 
	   FTAB_Shell_Widget_Class_Method_REALIZE);

  /*------------------- TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS ------------------*/
  o_TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS = 
    Wcls_Create_Subclass_Of_WIDGET_CLASS("TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS",
					 topLevelShellWidgetClass);

  xladdmsg(o_TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS, ":IS_MOTIF_WM_RUNNING", 
	   FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING);
  xladdmsg(o_TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS, ":UNREALIZE", 
	   FTAB_Shell_Widget_Class_Method_UNREALIZE);
  xladdmsg(o_TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS, ":POPUP",
	   FTAB_Popup_Shell_Widget_Class_Method_POPUP);
  xladdmsg(o_TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS, ":POPDOWN",
	   FTAB_Popup_Shell_Widget_Class_Method_POPDOWN);
  xladdmsg(o_TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Popup_Shell_Widget_Class_Method_ISNEW);
  xladdmsg(o_TOP_LEVEL_POPUP_SHELL_WIDGET_CLASS, ":REALIZE", 
	   FTAB_Shell_Widget_Class_Method_REALIZE);

  /*------------------- APPLICATION_SHELL_WIDGET_CLASS -----------------------*/
  o_APPLICATION_SHELL_WIDGET_CLASS = 
    Wcls_Create_Subclass_Of_WIDGET_CLASS("APPLICATION_SHELL_WIDGET_CLASS",
					 applicationShellWidgetClass);
  xladdmsg(o_APPLICATION_SHELL_WIDGET_CLASS, ":IS_MOTIF_WM_RUNNING", 
	   FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING);
  xladdmsg(o_APPLICATION_SHELL_WIDGET_CLASS, ":UNREALIZE", 
	   FTAB_Shell_Widget_Class_Method_UNREALIZE);
  xladdmsg(o_APPLICATION_SHELL_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Shell_Widget_Class_Method_ISNEW);
  xladdmsg(o_APPLICATION_SHELL_WIDGET_CLASS, ":REALIZE", 
	   FTAB_Shell_Widget_Class_Method_REALIZE);

  /*------------------- APPLICATION_POPUP_SHELL_WIDGET_CLASS -----------------------*/
  o_APPLICATION_POPUP_SHELL_WIDGET_CLASS = 
    Wcls_Create_Subclass_Of_WIDGET_CLASS("APPLICATION_POPUP_SHELL_WIDGET_CLASS",
					 applicationShellWidgetClass);
  xladdmsg(o_APPLICATION_POPUP_SHELL_WIDGET_CLASS, ":IS_MOTIF_WM_RUNNING", 
	   FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING);
  xladdmsg(o_APPLICATION_POPUP_SHELL_WIDGET_CLASS, ":UNREALIZE", 
	   FTAB_Shell_Widget_Class_Method_UNREALIZE);
  xladdmsg(o_APPLICATION_POPUP_SHELL_WIDGET_CLASS, ":POPUP",
	   FTAB_Popup_Shell_Widget_Class_Method_POPUP);
  xladdmsg(o_APPLICATION_POPUP_SHELL_WIDGET_CLASS, ":POPDOWN",
	   FTAB_Popup_Shell_Widget_Class_Method_POPDOWN);
  xladdmsg(o_APPLICATION_POPUP_SHELL_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Popup_Shell_Widget_Class_Method_ISNEW);
  xladdmsg(o_APPLICATION_POPUP_SHELL_WIDGET_CLASS, ":REALIZE", 
	   FTAB_Shell_Widget_Class_Method_REALIZE);

  /*------------------- OVERRIDE_SHELL_WIDGET_CLASS -----------------------*/
  o_OVERRIDE_SHELL_WIDGET_CLASS = 
    Wcls_Create_Subclass_Of_WIDGET_CLASS("OVERRIDE_SHELL_WIDGET_CLASS",
					 overrideShellWidgetClass);
  xladdmsg(o_OVERRIDE_SHELL_WIDGET_CLASS, ":IS_MOTIF_WM_RUNNING", 
	   FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING);
  xladdmsg(o_OVERRIDE_SHELL_WIDGET_CLASS, ":UNREALIZE", 
	   FTAB_Shell_Widget_Class_Method_UNREALIZE);
  xladdmsg(o_OVERRIDE_SHELL_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Shell_Widget_Class_Method_ISNEW);
  xladdmsg(o_OVERRIDE_SHELL_WIDGET_CLASS, ":REALIZE", 
	   FTAB_Shell_Widget_Class_Method_REALIZE);

  /*------------------- OVERRIDE_POPUP_SHELL_WIDGET_CLASS -----------------------*/
  o_OVERRIDE_POPUP_SHELL_WIDGET_CLASS = 
    Wcls_Create_Subclass_Of_WIDGET_CLASS("OVERRIDE_POPUP_SHELL_WIDGET_CLASS",
					 overrideShellWidgetClass);
  xladdmsg(o_OVERRIDE_POPUP_SHELL_WIDGET_CLASS, ":IS_MOTIF_WM_RUNNING", 
	   FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING);
  xladdmsg(o_OVERRIDE_POPUP_SHELL_WIDGET_CLASS, ":UNREALIZE", 
	   FTAB_Shell_Widget_Class_Method_UNREALIZE);
  xladdmsg(o_OVERRIDE_POPUP_SHELL_WIDGET_CLASS, ":POPUP",
	   FTAB_Popup_Shell_Widget_Class_Method_POPUP);
  xladdmsg(o_OVERRIDE_POPUP_SHELL_WIDGET_CLASS, ":POPDOWN",
	   FTAB_Popup_Shell_Widget_Class_Method_POPDOWN);
  xladdmsg(o_OVERRIDE_POPUP_SHELL_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Popup_Shell_Widget_Class_Method_ISNEW);
  xladdmsg(o_OVERRIDE_POPUP_SHELL_WIDGET_CLASS, ":REALIZE", 
	   FTAB_Shell_Widget_Class_Method_REALIZE);

  /*------------------- TRANSIENT_SHELL_WIDGET_CLASS -----------------------*/
  o_TRANSIENT_SHELL_WIDGET_CLASS = 
    Wcls_Create_Subclass_Of_WIDGET_CLASS("TRANSIENT_SHELL_WIDGET_CLASS",
					 transientShellWidgetClass);
  xladdmsg(o_TRANSIENT_SHELL_WIDGET_CLASS, ":IS_MOTIF_WM_RUNNING", 
	   FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING);
  xladdmsg(o_TRANSIENT_SHELL_WIDGET_CLASS, ":UNREALIZE", 
	   FTAB_Shell_Widget_Class_Method_UNREALIZE);
  xladdmsg(o_TRANSIENT_SHELL_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Shell_Widget_Class_Method_ISNEW);
  xladdmsg(o_TRANSIENT_SHELL_WIDGET_CLASS, ":REALIZE", 
	   FTAB_Shell_Widget_Class_Method_REALIZE);

  /*------------------- TRANSIENT_POPUP_SHELL_WIDGET_CLASS -----------------------*/
  o_TRANSIENT_POPUP_SHELL_WIDGET_CLASS = 
    Wcls_Create_Subclass_Of_WIDGET_CLASS("TRANSIENT_POPUP_SHELL_WIDGET_CLASS",
					 transientShellWidgetClass);
  xladdmsg(o_TRANSIENT_POPUP_SHELL_WIDGET_CLASS, ":IS_MOTIF_WM_RUNNING", 
	   FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING);
  xladdmsg(o_TRANSIENT_POPUP_SHELL_WIDGET_CLASS, ":UNREALIZE", 
	   FTAB_Shell_Widget_Class_Method_UNREALIZE);
  xladdmsg(o_TRANSIENT_POPUP_SHELL_WIDGET_CLASS, ":POPUP",
	   FTAB_Popup_Shell_Widget_Class_Method_POPUP);
  xladdmsg(o_TRANSIENT_POPUP_SHELL_WIDGET_CLASS, ":POPDOWN",
	   FTAB_Popup_Shell_Widget_Class_Method_POPDOWN);
  xladdmsg(o_TRANSIENT_POPUP_SHELL_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Popup_Shell_Widget_Class_Method_ISNEW);
  xladdmsg(o_TRANSIENT_POPUP_SHELL_WIDGET_CLASS, ":REALIZE", 
	   FTAB_Shell_Widget_Class_Method_REALIZE);

  /*-------------------- XM_DIALOG_POPUP_SHELL_WIDGET_CLASS ------------------------*/
  o_XM_DIALOG_POPUP_SHELL_WIDGET_CLASS =
    Wcls_Create_Subclass_Of_WIDGET_CLASS("XM_DIALOG_POPUP_SHELL_WIDGET_CLASS",
					 xmDialogShellWidgetClass);
  xladdmsg(o_XM_DIALOG_POPUP_SHELL_WIDGET_CLASS, ":IS_MOTIF_WM_RUNNING", 
	   FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING);
  xladdmsg(o_XM_DIALOG_POPUP_SHELL_WIDGET_CLASS, ":UNREALIZE", 
	   FTAB_Shell_Widget_Class_Method_UNREALIZE);
  xladdmsg(o_XM_DIALOG_POPUP_SHELL_WIDGET_CLASS, ":POPUP",
	   FTAB_Popup_Shell_Widget_Class_Method_POPUP);
  xladdmsg(o_XM_DIALOG_POPUP_SHELL_WIDGET_CLASS, ":POPDOWN",
	   FTAB_Popup_Shell_Widget_Class_Method_POPDOWN);
  xladdmsg(o_XM_DIALOG_POPUP_SHELL_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Popup_Shell_Widget_Class_Method_ISNEW);
  xladdmsg(o_XM_DIALOG_POPUP_SHELL_WIDGET_CLASS, ":REALIZE", 
	   FTAB_Shell_Widget_Class_Method_REALIZE);

  /*-------------------- XM_MENU_POPUP_SHELL_WIDGET_CLASS ------------------------*/
  o_XM_MENU_POPUP_SHELL_WIDGET_CLASS =
    Wcls_Create_Subclass_Of_WIDGET_CLASS("XM_MENU_POPUP_SHELL_WIDGET_CLASS",
					 xmMenuShellWidgetClass);
  xladdmsg(o_XM_MENU_POPUP_SHELL_WIDGET_CLASS, ":IS_MOTIF_WM_RUNNING", 
	   FTAB_Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING);
  xladdmsg(o_XM_MENU_POPUP_SHELL_WIDGET_CLASS, ":UNREALIZE", 
	   FTAB_Shell_Widget_Class_Method_UNREALIZE);
  xladdmsg(o_XM_MENU_POPUP_SHELL_WIDGET_CLASS, ":POPUP",
	   FTAB_Popup_Shell_Widget_Class_Method_POPUP);
  xladdmsg(o_XM_MENU_POPUP_SHELL_WIDGET_CLASS, ":POPDOWN",
	   FTAB_Popup_Shell_Widget_Class_Method_POPDOWN);
  xladdmsg(o_XM_MENU_POPUP_SHELL_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Popup_Shell_Widget_Class_Method_ISNEW);
  xladdmsg(o_XM_MENU_POPUP_SHELL_WIDGET_CLASS, ":REALIZE", 
	   FTAB_Shell_Widget_Class_Method_REALIZE);
}
