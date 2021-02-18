/* -*-C-*-
********************************************************************************
*
* File:         wc_WIDGET.c
* RCS:          $Header: wc_WIDGET.c,v 1.1 89/11/25 04:57:06 mayer Exp $
* Description:  WIDGET_CLASS (the widget metaclass)
* Author:       Niels Mayer, HPLabs
* Created:      Tue Nov 21 00:55:07 1989
* Modified:     Sat Nov 25 04:54:44 1989 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: wc_WIDGET.c,v 1.1 89/11/25 04:57:06 mayer Exp $";

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_funtab.h"

LVAL o_WIDGET_CLASS;		/* note -- this is used in w_classes.c */
LVAL k_managed;
LVAL k_unmanaged;
LVAL k_dialog;
LVAL k_scrolled;

/*
 * Note -- various methods on WIDGET_CLASS are implemented in:
 * w_libXt.c -- interfaces to Xtoolkit intrinsics.
 * w_libXm.c -- interfaces to Motif toolkit.
 * w_evnthndlr.c -- interfaces to event handling routines
 * w_callbacks.c -- interfaces to Xtoolkit callback mechanism.
 */


/*****************************************************************************
 * (send <widget_instance> :add_callback <name>
 *       <call_data_binding_names_list>
 *       <code>)
 * returns: <callback_id_object>
 *
 * <name> is a resource keyword of type XmRCallback, eg,
 * :XMN_ACTIVATE_CALLBACK, :XMN_ARM_CALLBACK, :XMN_DISARM_CALLBACK.
 *
 * <call_data_binding_name_list> is a list of symbols that get
 * bound to data specific to the action on the callback widget. For 
 * the XmAnyCallbackStruct passed as call data here, the only valid symbol
 * names are CALLBACK_WIDGET CALLBACK_REASON CALLBACK_XEVENT.
 * 
 * <code> is a list of lisp expressions that are evaluated when the callback
 * occurs. During the callback, the lexical environment that existed for
 * the call to :add_callback will be used for value and functional bindings.
 * 
 * The returned <callback_id_object> may be passed into the 
 * function (remove_callback <callback_id_object>).
 ****************************************************************************/
LVAL Widget_Class_Method_ADD_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */
  extern void Wcb_XmAnyCallbackStruct_Callbackproc(); /* w_callbacks.c */
  return(Wcb_Meta_Method_Add_Callback(Wcb_XmAnyCallbackStruct_Callbackproc, FALSE));
}


/*****************************************************************************
 * (send <widget_instance> :set_callback <name>
 *       <call_data_binding_names_list>
 *       <code>)
 * returns: <callback_id_object>
 *
 * This does exactly what method :add_callback does, except that it ensures
 * that the callback currently being set is the only callback on <widget_instance>
 * for <name>.
 *
 * This method is best used for interactively changing callbacks on a widget.
 * It is slightly less efficient than method :add_callback because it must
 * search-for and remove other callbacks on <widget> with the same <name>.
 ****************************************************************************/
LVAL Widget_Class_Method_SET_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */
  extern void Wcb_XmAnyCallbackStruct_Callbackproc(); /* w_callbacks.c */
  return (Wcb_Meta_Method_Add_Callback(Wcb_XmAnyCallbackStruct_Callbackproc, TRUE));
}


/*****************************************************************************
 * (send <Widget_Class> :new [:managed/:unmanaged]
 *                           [<name>]
 *                           <parent> 
 *                           [:XMN_<arg1> <val1>]
 *                           [. . .             ]
 *                           [:XMN_<argN> <valN>])
 *
 * For the <widget_class> meta-class (the parent class of all widget-objects), 
 * create a new widget via XtCreateWidget().
 *
 * The optional keyword-argument :managed will cause a subsequent call to
 * XtManageChild(). If the submessage :unmanaged is present, or no submessage,
 * then XtManageChild() won't be called, and the resulting widget will be
 * returned unmanaged.
 ****************************************************************************/
LVAL Widget_Class_Method_ISNEW()
{
  extern void Wcls_Initialize_WIDGETOBJ();
  extern WidgetClass Wcls_WIDGETCLASSOBJ_To_WidgetClassID();
  extern ArgList Wres_Get_LispArglist(); /* from w_resources.c */
  extern void    Wres_Free_C_Arglist_Data(); /* from w_resources.c */
  LVAL self, o_parent;
  char* name;
  Boolean managed_p;
  WidgetClass widgetclass_id;
  Widget widget_id, parent_widget_id;

  self = xlgaobject();		/* NOTE: xlobj.c:clnew() returns an OBJECT; if this method
				   returns successfully, it will return a WIDGETOBJ */

  /* get optional managed/unmanaged arg */
  if (moreargs() && ((*xlargv == k_managed) || (*xlargv == k_unmanaged)))
    managed_p = (nextarg() == k_managed);
  else
    managed_p = FALSE;		/* by default don't call XtManageChild() */

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
    widget_id = XtCreateWidget(name, widgetclass_id, parent_widget_id, xt_arglist, xt_numargs);
    Wres_Free_C_Arglist_Data();
  }
  else
    widget_id = XtCreateWidget(name, widgetclass_id, parent_widget_id, ARGLIST());

  Wcls_Initialize_WIDGETOBJ(self, widget_id);

  if (managed_p)
    XtManageChild(widget_id);

#ifdef DEBUG_WINTERP_1
  Wcls_Print_WidgetObj_Info(self);
#endif
  return (self);
}


/*****************************************************************************
 * Initialize the Xtoolkit interface. Note that this procedure does
 * equivelent kinds of setup as done by xloinit(). The following call 
 * sequence happens on startup:
 * [xlsymbols(), xloinit()] <-- initwks() <-- xlinit() <-- main()...
 * However, note that main() calls xlinit() only if xlirestore("xlisp.wks") 
 * fails. In that case, we get the calling sequence:
 * obsymbols() <-- xlsymbols() <-- xlirestore() <-- main(). We could patch
 * in the symbols initialized in this module by putting initialization 
 * routines in ossymbols(), but it's not clear that this would make sense:
 * restore will only be able to restore widget objects' data, but will not
 * be able to recreate widgets themselves.
 *
 * Thus, we call this procedure after xlinit() in main() and this reinitializes
 * the widget objects independent of restoring. Of course, any widget objects
 * that were saved/restored may now not have a valid class, since the 
 * pointers may have changed ...
 ****************************************************************************/
Wc_WIDGET_Init()
{
  extern LVAL xlclass();	/* from xlobj.c */
  extern      xladdivar();	/* from xlobj.c */
  extern      xladdmsg();	/* from xlobj.c */

  k_managed            = xlenter(":MANAGED");
  k_unmanaged          = xlenter(":UNMANAGED");
  k_dialog             = xlenter(":DIALOG"); /* used by various :isnew methods */
  k_scrolled           = xlenter(":SCROLLED"); /* used by various :isnew methods */

  /* 
   * create 'WIDGET_CLASS' by doing the (almost) equivalent of 
   * (setq Widget_Class (send Class :new 
   *                           '(widget_id)
   *                             object))
   * Creating a new 'Class' instance would end up calling clnew() to create
   * a new instance of 'Class', and then call clisnew() on that 'Class'
   * instance to set the appropriate slots for 'WIDGET_CLASS'. Thus we need to
   * create object 'WIDGET_CLASS', and with that object, set
   *    self.MESSAGES = (:SET_CALLBACK :GET_VALUES :SET_VALUES ...)
   *    self.IVARS = (WIDGET_ID SETVALUES_ALIST)
   *    self.CVARS = NIL
   *    self.CVALS = NIL
   *    self.SUPERCLASS = 'Object'
   *    self.IVARCNT = WIDGETOBJ_SIZE     ;; == length(self.IVARS)
   *    self.IVARTOTAL = WIDGETOBJ_SIZE   ;; == self.IVARCNT + length(superclass.IVARS)
   *
   * 'WIDGET_CLASS' contains methods corresponding to common operations on
   * 'Widget' in the Xtoolkit. The :isnew method in 'Widget_Class' is the
   * default means to create a new widget in this system -- this method will
   * look up the WidgetClass pointer from class(self).CVALS[0]. Note that 
   * 'WIDGET_CLASS' is a core class. Widget instances should be created
   * from classes inheriting from 'Widget_Class' The message :isnew will 
   * complain if you try to instantiate 'WIDGET_CLASS' since 
   * class(self).CVALS == NIL for that class only.
   *
   * Specific widget classes, corresponding to a instantiatable widget objects
   * in Motif, are created by doing, for example
   * (setq PUSHBUTTON_CLASS (send Class :new
   *                              '()                 ;; no new IVARS
   *                              '(WIDGET_CLASS_ID   ;; value==xmPushButtonWidgetClass
   *                                WIDGET_CLASS_SYM  ;; value==PUSHBUTTON_CLASS
   *                              Widget_Class)       ;; superclass
   * where WIDGET_CLASS_ID is associated with the WidgetClass pointer
   * designating the class of the widget for the Xtoolkit creation 
   * functions.
   */

  /*--------------- create 'Class' instance 'WIDGET_CLASS' -----------------*/
  /* Calling xlclass() creates the 'Class' instance called 'WIDGET_CLASS' and 
     sets IVARCNT, IVARTOTAL, and SUPERCLASS slots
     slots CVARS, CVALS are set to NIL */
  o_WIDGET_CLASS = xlclass("WIDGET_CLASS", WIDGETOBJ_SIZE); 

  /* set up the list of ivarnames in IVARS */
  /* xladdivar(o_WIDGET_CLASS, "SETVALUES_ALIST"); */ /* ivar number 1 */
  /* xladdivar(o_WIDGET_CLASS, "WIDGET_ID"); */ /* ivar number 0 */

  /* set up the list of methods in MESSAGES, and bind the associated FSUBR */

#ifdef 0			/* template for adding new methods */
  xladdmsg(o_WIDGET_CLASS, ":",
	   FTAB_Widget_Class_Method_); /*  */
#endif

  xladdmsg(o_WIDGET_CLASS, ":BUILD_EVENT_MASK",
	   FTAB_Widget_Class_Method_BUILD_EVENT_MASK); /* w_evnthndlr.c */

  xladdmsg(o_WIDGET_CLASS, ":OVERRIDE_TRANSLATIONS",
	   FTAB_Widget_Class_Method_OVERRIDE_TRANSLATIONS); /* w_txlations.c */

  xladdmsg(o_WIDGET_CLASS, ":AUGMENT_TRANSLATIONS",
	   FTAB_Widget_Class_Method_AUGMENT_TRANSLATIONS); /* w_txlations.c */

  xladdmsg(o_WIDGET_CLASS, ":UNINSTALL_TRANSLATIONS",
	   FTAB_Widget_Class_Method_UNINSTALL_TRANSLATIONS); /* w_txlations.c */

  xladdmsg(o_WIDGET_CLASS, ":INSTALL_ACCELERATORS",
	   FTAB_Widget_Class_Method_INSTALL_ACCELERATORS); /* w_txlations.c */

  xladdmsg(o_WIDGET_CLASS, ":INSTALL_ALL_ACCELERATORS",
	   FTAB_Widget_Class_Method_INSTALL_ALL_ACCELERATORS); /* w_txlations.c */

  xladdmsg(o_WIDGET_CLASS, ":REMOVE_ALL_CALLBACKS",
	   FTAB_Widget_Class_Method_REMOVE_ALL_CALLBACKS); /* w_callbacks.c */

  xladdmsg(o_WIDGET_CLASS, ":ADD_TAB_GROUP",
	   FTAB_Widget_Class_Method_ADD_TAB_GROUP); /* w_libXm.c */

  xladdmsg(o_WIDGET_CLASS, ":REMOVE_TAB_GROUP",
	   FTAB_Widget_Class_Method_REMOVE_TAB_GROUP); /* w_libXm.c */

  xladdmsg(o_WIDGET_CLASS, ":UPDATE_DISPLAY", /* w_libXm.c */
	   FTAB_Widget_Class_Method_UPDATE_DISPLAY);

  xladdmsg(o_WIDGET_CLASS, ":ADD_GRAB",
	   FTAB_Widget_Class_Method_ADD_GRAB); /* w_libXt.c */

  xladdmsg(o_WIDGET_CLASS, ":REMOVE_GRAB",
	   FTAB_Widget_Class_Method_REMOVE_GRAB); /* w_libXt.c */

   xladdmsg(o_WIDGET_CLASS, ":IS_COMPOSITE",
	   FTAB_Widget_Class_Method_IS_COMPOSITE); /* w_libXt.c */

  xladdmsg(o_WIDGET_CLASS, ":IS_CONSTRAINT",
	   FTAB_Widget_Class_Method_IS_CONSTRAINT); /* w_libXt.c */

  xladdmsg(o_WIDGET_CLASS, ":IS_SHELL",
	   FTAB_Widget_Class_Method_IS_SHELL); /* w_libXt.c */

  xladdmsg(o_WIDGET_CLASS, ":IS_PRIMITIVE",
	   FTAB_Widget_Class_Method_IS_PRIMITIVE); /* w_libXm.c */

  xladdmsg(o_WIDGET_CLASS, ":IS_GADGET",
	   FTAB_Widget_Class_Method_IS_GADGET); /* w_libXm.c */

  xladdmsg(o_WIDGET_CLASS, ":IS_MANAGER",
	   FTAB_Widget_Class_Method_IS_MANAGER); /* w_libXm.c */
 
  xladdmsg(o_WIDGET_CLASS, ":SET_SENSITIVE",
	   FTAB_Widget_Class_Method_SET_SENSITIVE); /* w_libXt.c */

  xladdmsg(o_WIDGET_CLASS, ":SET_MAPPED_WHEN_MANAGED",
	   FTAB_Widget_Class_Method_SET_MAPPED_WHEN_MANAGED); /* w_libXt.c */

  xladdmsg(o_WIDGET_CLASS, ":IS_MANAGED",
	   FTAB_Widget_Class_Method_IS_MANAGED); /* w_libXt.c */

  xladdmsg(o_WIDGET_CLASS, ":IS_REALIZED",
	   FTAB_Widget_Class_Method_IS_REALIZED); /* w_libXt.c */

  xladdmsg(o_WIDGET_CLASS, ":IS_SENSITIVE",
	   FTAB_Widget_Class_Method_IS_SENSITIVE); /* w_libXt.c */

  xladdmsg(o_WIDGET_CLASS, ":PARENT",
	   FTAB_Widget_Class_Method_PARENT); /* w_libXt.c */

  xladdmsg(o_WIDGET_CLASS, ":WINDOW",
	   FTAB_Widget_Class_Method_WINDOW); /* w_libXt.c */

  xladdmsg(o_WIDGET_CLASS, ":MAP",
	   FTAB_Widget_Class_Method_MAP); /* w_libXt.c */

  xladdmsg(o_WIDGET_CLASS, ":UNMAP",
	   FTAB_Widget_Class_Method_UNMAP); /* w_libXt.c */

  xladdmsg(o_WIDGET_CLASS, ":SET_EVENT_HANDLER",
	   FTAB_Widget_Class_Method_SET_EVENT_HANDLER);	/* w_evnthndlr.c */

  xladdmsg(o_WIDGET_CLASS, ":ADD_EVENT_HANDLER",
	   FTAB_Widget_Class_Method_ADD_EVENT_HANDLER);	/* w_evnthndlr.c */

  xladdmsg(o_WIDGET_CLASS, ":SET_CALLBACK",
	   FTAB_Widget_Class_Method_SET_CALLBACK);

  xladdmsg(o_WIDGET_CLASS, ":ADD_CALLBACK",
	   FTAB_Widget_Class_Method_ADD_CALLBACK);

  xladdmsg(o_WIDGET_CLASS, ":DESTROY", /* w_libXt.c */
	   FTAB_Widget_Class_Method_DESTROY);

  xladdmsg(o_WIDGET_CLASS, ":MANAGE", /* w_libXt.c */
	   FTAB_Widget_Class_Method_MANAGE);

  xladdmsg(o_WIDGET_CLASS, ":UNMANAGE",	/* w_libXt.c */
	   FTAB_Widget_Class_Method_UNMANAGE);

  xladdmsg(o_WIDGET_CLASS, ":GET_VALUES", /* w_libXt.c */
	   FTAB_Widget_Class_Method_GET_VALUES);	

  xladdmsg(o_WIDGET_CLASS, ":SET_VALUES", /* w_libXt.c */
	   FTAB_Widget_Class_Method_SET_VALUES);

  xladdmsg(o_WIDGET_CLASS, ":ISNEW",
	   FTAB_Widget_Class_Method_ISNEW);
}
