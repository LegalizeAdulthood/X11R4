/* -*-C-*-
********************************************************************************
*
* File:         w_callbacks.c
* RCS:          $Header: w_callbacks.c,v 1.2 89/11/25 04:20:55 mayer Exp $
* Description:  
* Author:       Niels Mayer, HPLabs
* Created:      Sat Aug 26 07:44:17 1989
* Modified:     Sat Nov 25 04:20:44 1989 (Niels Mayer) mayer@hplnpm
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
*
********************************************************************************
*/
static char rcs_identity[] = "@(#)$Header: w_callbacks.c,v 1.2 89/11/25 04:20:55 mayer Exp $";

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/List.h>
#include <Xm/Text.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"

static LVAL s_CALLBACK_WIDGET, s_CALLBACK_REASON, s_CALLBACK_XEVENT,
  s_CALLBACK_WINDOW, s_CALLBACK_ITEM, s_CALLBACK_ENTRY_WIDGET,
  s_CALLBACK_ENTRY_REASON, s_CALLBACK_ENTRY_SET, s_CALLBACK_ENTRY_DATA,
  s_CALLBACK_VALUE, s_CALLBACK_PIXEL, s_CALLBACK_SET, s_CALLBACK_ITEM_LENGTH,
  s_CALLBACK_ITEM_POSITION, s_CALLBACK_SELECTED_ITEMS,
  s_CALLBACK_SELECTED_ITEM_COUNT, s_CALLBACK_SELECTION_TYPE, s_CALLBACK_LENGTH,
  s_CALLBACK_MASK, s_CALLBACK_MASK_LENGTH, s_CALLBACK_CUR_INSERT,
  s_CALLBACK_NEW_INSERT, s_CALLBACK_START_POS, s_CALLBACK_END_POS;

static LVAL s_INITIAL, s_ADDITION, s_MODIFICATION;

static LVAL s_CR_NONE, s_CR_HELP, s_CR_VALUE_CHANGED, s_CR_INCREMENT,
  s_CR_DECREMENT, s_CR_PAGE_INCREMENT, s_CR_PAGE_DECREMENT, s_CR_TO_TOP,
  s_CR_TO_BOTTOM, s_CR_DRAG, s_CR_ACTIVATE, s_CR_ARM, s_CR_DISARM, s_CR_MAP,
  s_CR_UNMAP, s_CR_FOCUS, s_CR_LOSING_FOCUS, s_CR_MODIFYING_TEXT_VALUE,
  s_CR_MOVING_INSERT_CURSOR, s_CR_EXECUTE, s_CR_SINGLE_SELECT,
  s_CR_MULTIPLE_SELECT, s_CR_EXTENDED_SELECT, s_CR_BROWSE_SELECT,
  s_CR_DEFAULT_ACTION, s_CR_CLIPBOARD_DATA_REQUEST, s_CR_CLIPBOARD_DATA_DELETE,
  s_CR_CASCADING, s_CR_OK, s_CR_CANCEL, s_CR_APPLY, s_CR_NO_MATCH,
  s_CR_COMMAND_ENTERED, s_CR_COMMAND_CHANGED, s_CR_EXPOSE, s_CR_RESIZE, s_CR_INPUT;


/* 
 * To prevent garbage collection of callback-objects, we store the objects in
 * the hashtable w_savedobjs.c:v_savedobjs after hashing on the address of the
 * widget-object LVAL. Since callbacks occur on a per-widget basis, we use the
 * widget as the hash-key and then search through the hashbucket looking for
 * LVALs that are callback-objects, have the appropriate widget-object, and
 * have the right callback name.
 * 
 * Garbage collection of lisp-objects will occur to any object not referenced
 * as symbol values, inside lexical environments saved as functional closures,
 * as elements of the evaluation or argument stack. In the case of callbacks,
 * the callback closure, the callback's lexical environment, and the callback
 * widget are implicitly referenced inside Motif/Xtoolkit code. As long as the
 * widget on which these callbacks are placed still exists, we must prevent
 * callback-objects from being garbage collected. 
 * 
 * This is done by:
 *    1) storing them in v_savedobjs when the callback is added
 *       (note that v_savedobjs is a hashtable hashed by the LVAL's pointer,
 *        for callbacks, we store all callbacks on a particular widget in the
 *        hashbucket associated with hashing on that widget-object. this makes
 *        it easier to implement :set_callback. and :remove_all_callbacks.)
 *    2) removing them when the callback is removed
 *       (as in methods :set_callback :remove_all_callbacks and function remove_callback)
 *    3) removing them when the widget is destroyed.
 *       (requires a destroy callback to be added onto each widget)
 */


/******************************************************************************
 * 
 ******************************************************************************/
void Wcb_Meta_Callbackproc(widget, client_data, call_data, bind_call_data_values_proc)
     Widget  widget;
     caddr_t client_data;	/* XLTYPE_CALLBACKOBJ */
     caddr_t call_data;		/* a pointer to a structure that is accessed via (*bind_call_data_values_proc)() */
     void    (*bind_call_data_values_proc)(/* LVAL bindings_list; LVAL lexical_env; caddr_t call_data; LVAL o_widget */);
{
  extern LVAL          xlenv, xlfenv;
  LVAL                 oldenv, oldfenv, l_evalforms, s_name;
  CONTEXT              cntxt;
  LVAL                 c_callback = get_callback_closure((LVAL) client_data);

  /*
   * Most of this procedure looks alot like xleval.c:evfun(), which is what
   * the evaluator calls when a functional form is to be evaluated. The
   * main difference is that instead of calling xlabind() to bind the
   * formal parameter symbols of a function to their values in the new
   * lexical environment frame returned by xlframe(getenv(fun)), we look
   * at the formal args from the callback's closure and bind these to the
   * appropriate values.
   */
  
  /* protect some pointers */
  xlstkcheck(3);
  xlsave(oldenv);
  xlsave(oldfenv);
  xlsave(l_evalforms);
  
  /* create a new environment frame */
  oldenv = xlenv;
  oldfenv = xlfenv;
  xlenv = xlframe(getenv(c_callback));
  xlfenv = getfenv(c_callback);
  
  /* get the list of bindings and bind (locally) the symbols to values retrieved from call_data -- see also xlabind() */
  (*bind_call_data_values_proc)(getargs(c_callback), /* a list of symbols to which values from call_data struct are locally bound */
				xlenv, /* the lexical environment in which the bindings are made  */
				call_data, /* a pointer to a Xm*CallbackStruct (the structure used depends on the widget-class of the widget containing the callback */
				get_callback_widget((LVAL) client_data)); /* the WIDGETOBJ on which the callback occured. */
  
  /* setup the implicit block */
  if (s_name = getname(c_callback))
    xlbegin(&cntxt, CF_RETURN, s_name);
  
  /* execute the block */
  if (s_name && setjmp(cntxt.c_jmpbuf))
    { }
  else
    for (l_evalforms = getbody(c_callback); consp(l_evalforms); 
	 l_evalforms = cdr(l_evalforms))
      xleval(car(l_evalforms));
  
  /* finish the block context */
  if (s_name)
    xlend(&cntxt);
  
  /* restore the environment */
  xlenv = oldenv;
  xlfenv = oldfenv;
  
  /* restore the stack */
  xlpopn(3);
}

/******************************************************************************
 * Are we having fun yet?
 ******************************************************************************/
static LVAL Wcb_Get_Callback_Reason_Symbol(cb_reason)
     int cb_reason;
{
  switch (cb_reason) {
  case XmCR_NONE:
    return (s_CR_NONE);
  case XmCR_HELP:
    return (s_CR_HELP);
  case XmCR_VALUE_CHANGED:
    return (s_CR_VALUE_CHANGED);
  case XmCR_INCREMENT:
    return (s_CR_INCREMENT);
  case XmCR_DECREMENT:
    return (s_CR_DECREMENT);
  case XmCR_PAGE_INCREMENT:
    return (s_CR_PAGE_INCREMENT);
  case XmCR_PAGE_DECREMENT:
    return (s_CR_PAGE_DECREMENT);
  case XmCR_TO_TOP:
    return (s_CR_TO_TOP);
  case XmCR_TO_BOTTOM:
    return (s_CR_TO_BOTTOM);
  case XmCR_DRAG:
    return (s_CR_DRAG);
  case XmCR_ACTIVATE:
    return (s_CR_ACTIVATE);
  case XmCR_ARM:
    return (s_CR_ARM);
  case XmCR_DISARM:
    return (s_CR_DISARM);
  case XmCR_MAP:
    return (s_CR_MAP);
  case XmCR_UNMAP:
    return (s_CR_UNMAP);
  case XmCR_FOCUS:
    return (s_CR_FOCUS);
  case XmCR_LOSING_FOCUS:
    return (s_CR_LOSING_FOCUS);
  case XmCR_MODIFYING_TEXT_VALUE:
    return (s_CR_MODIFYING_TEXT_VALUE);
  case XmCR_MOVING_INSERT_CURSOR:
    return (s_CR_MOVING_INSERT_CURSOR);
  case XmCR_EXECUTE:
    return (s_CR_EXECUTE);
  case XmCR_SINGLE_SELECT:
    return (s_CR_SINGLE_SELECT);
  case XmCR_MULTIPLE_SELECT:
    return (s_CR_MULTIPLE_SELECT);
  case XmCR_EXTENDED_SELECT:
    return (s_CR_EXTENDED_SELECT);
  case XmCR_BROWSE_SELECT:
    return (s_CR_BROWSE_SELECT);
  case XmCR_DEFAULT_ACTION:
    return (s_CR_DEFAULT_ACTION);
  case XmCR_CLIPBOARD_DATA_REQUEST:
    return (s_CR_CLIPBOARD_DATA_REQUEST);
  case XmCR_CLIPBOARD_DATA_DELETE:
    return (s_CR_CLIPBOARD_DATA_DELETE);
  case XmCR_CASCADING:
    return (s_CR_CASCADING);
  case XmCR_OK:
    return (s_CR_OK);
  case XmCR_CANCEL:
    return (s_CR_CANCEL);
  case XmCR_APPLY:
    return (s_CR_APPLY);
  case XmCR_NO_MATCH:
    return (s_CR_NO_MATCH);
  case XmCR_COMMAND_ENTERED:
    return (s_CR_COMMAND_ENTERED);
  case XmCR_COMMAND_CHANGED:
    return (s_CR_COMMAND_CHANGED);
  case XmCR_EXPOSE:
    return (s_CR_EXPOSE);
  case XmCR_RESIZE:
    return (s_CR_RESIZE);
  case XmCR_INPUT:
    return (s_CR_INPUT);
  default:
    xlfail("Internal error in Wcb_Get_Callback_Reason_Symbol() -- unknown callback reason.");
  }
}


/******************************************************************************
 * typedef struct
 * {
 *     int     reason;
 *     XEvent  *event;
 * } XmAnyCallbackStruct;
 ******************************************************************************/
static void Wcb_Lexical_Bindings_For_XmAnyCallbackStruct(bindings_list, lexical_env, call_data, o_widget)
     LVAL bindings_list;	/* a list of symbols to which values from XmAnyCallbackStruct are bound */
     LVAL lexical_env;		
     caddr_t call_data;		/* a pointer to XmAnyCallbackStruct */
     LVAL o_widget;		/* XLTYPE_WIDGETOBJ */
{
  XmAnyCallbackStruct* cd = (XmAnyCallbackStruct*) call_data;
  LVAL s_bindname;

  for ( ; consp(bindings_list); bindings_list = cdr(bindings_list)) {

    s_bindname = car(bindings_list);

    if (s_bindname == s_CALLBACK_WIDGET) {
      xlpbind(s_bindname, o_widget, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_REASON) {
      xlpbind(s_bindname, Wcb_Get_Callback_Reason_Symbol(cd->reason), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_XEVENT) {
      xlpbind(s_bindname, (cd->event) ? cv_xevent(cd->event) : NIL, lexical_env);
    }
    else {
      extern char temptext[];	/* from winterp.c */
      sprintf(temptext,
	      "Unknown binding name in XmAnyCallbackStruct callback evaluator. Valid symbols are [%s %s %s].",
	      (char*) getstring(getpname(s_CALLBACK_WIDGET)),
	      (char*) getstring(getpname(s_CALLBACK_REASON)),
	      (char*) getstring(getpname(s_CALLBACK_XEVENT)));
      xlerror(temptext, s_bindname);
    }
  }
}


/******************************************************************************
 * typedef struct
 * {
 *     int     reason;
 *     XEvent  *event;
 *     Window  window;
 * } XmDrawingAreaCallbackStruct;
 ******************************************************************************/
static void Wcb_Lexical_Bindings_For_XmDrawingAreaCallbackStruct(bindings_list, lexical_env, call_data, o_widget)
     LVAL bindings_list;	/* a list of symbols to which values from XmDrawingAreaCallbackStruct are bound */
     LVAL lexical_env;		
     caddr_t call_data;		/* a pointer to XmDrawingAreaCallbackStruct */
     LVAL o_widget;		/* XLTYPE_WIDGETOBJ */
{
  XmDrawingAreaCallbackStruct* cd = (XmDrawingAreaCallbackStruct*) call_data;
  LVAL s_bindname;

  for ( ; consp(bindings_list); bindings_list = cdr(bindings_list)) {

    s_bindname = car(bindings_list);

    if (s_bindname == s_CALLBACK_WIDGET) {
      xlpbind(s_bindname, o_widget, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_REASON) {
      xlpbind(s_bindname, Wcb_Get_Callback_Reason_Symbol(cd->reason), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_XEVENT) {
      xlpbind(s_bindname, (cd->event) ? cv_xevent(cd->event) : NIL, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_WINDOW) {
      xlpbind(s_bindname, (cd->window) ? cv_window(cd->window) : NIL, lexical_env);
    }
    else {
      extern char temptext[];	/* from winterp.c */
      sprintf(temptext,
	      "Unknown binding name in XmDrawingAreaCallbackStruct callback evaluator. Valid symbols are [%s %s %s %s].",
	      (char*) getstring(getpname(s_CALLBACK_WIDGET)),
	      (char*) getstring(getpname(s_CALLBACK_REASON)),
	      (char*) getstring(getpname(s_CALLBACK_XEVENT)),
	      (char*) getstring(getpname(s_CALLBACK_WINDOW)));
      xlerror(temptext, s_bindname);
    }
  }
}


/******************************************************************************
 * typedef struct
 * {
 *     int     reason;
 *     XEvent  *event;
 *     Window  window;
 * } XmDrawnButtonCallbackStruct;
 ******************************************************************************/
static void Wcb_Lexical_Bindings_For_XmDrawnButtonCallbackStruct(bindings_list, lexical_env, call_data, o_widget)
     LVAL bindings_list;	/* a list of symbols to which values from XmDrawnButtonCallbackStruct are bound */
     LVAL lexical_env;		
     caddr_t call_data;		/* a pointer to XmDrawnButtonCallbackStruct */
     LVAL o_widget;		/* XLTYPE_WIDGETOBJ */
{
  XmDrawnButtonCallbackStruct* cd = (XmDrawnButtonCallbackStruct*) call_data;
  LVAL s_bindname;

  for ( ; consp(bindings_list); bindings_list = cdr(bindings_list)) {

    s_bindname = car(bindings_list);

    if (s_bindname == s_CALLBACK_WIDGET) {
      xlpbind(s_bindname, o_widget, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_REASON) {
      xlpbind(s_bindname, Wcb_Get_Callback_Reason_Symbol(cd->reason), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_XEVENT) {
      xlpbind(s_bindname, (cd->event) ? cv_xevent(cd->event) : NIL, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_WINDOW) {
      xlpbind(s_bindname, (cd->window) ? cv_window(cd->window) : NIL, lexical_env);
    }
    else {
      extern char temptext[];	/* from winterp.c */
      sprintf(temptext,
	      "Unknown binding name in XmDrawnButtonCallbackStruct callback evaluator. Valid symbols are [%s %s %s %s].",
	      (char*) getstring(getpname(s_CALLBACK_WIDGET)),
	      (char*) getstring(getpname(s_CALLBACK_REASON)),
	      (char*) getstring(getpname(s_CALLBACK_XEVENT)),
	      (char*) getstring(getpname(s_CALLBACK_WINDOW)));
      xlerror(temptext, s_bindname);
    }
  }
}


/******************************************************************************
 * typedef struct
 * {
 *     int     reason;
 *     XEvent  *event;
 *     Widget  widget;
 *     char    *data;
 *     char    *callbackstruct;
 * } XmRowColumnCallbackStruct;
 *
 * Note: if rowcol widget has an XmNentryCallback set, then any 
 * activate action on any of it's entries will produce a callback
 * on the rowcol widget with
 *    cb.reason==XmCR_ACTIVATE,
 *    cb.event==cb.callbackstruct->event
 *    cb.widget==<entrywidget>
 *    cb.data==<entrywidget's client-data if a activate callback was spec'd for entry and it had client-data> 
 *    cb.callbackstruct==<entrywidget's call-data> (XmAnyCallbackStruct/XmTobbleButtonCallbackStruct)
 *
 * The weird part here is the cb.callbackstruct slot -- we don't know the class
 * of the entrywidget, so we can't know what kind of callbackstruct it got.
 * We can only be guaranteed that the activated-entry-widget's .reason 
 * fields are valid. If we have a toggle button, then we may want to 
 * access the .set slot of the XmToggleButtonCallbackStruct. (??)
 * For now, assume that people 
 ******************************************************************************/
static void Wcb_Lexical_Bindings_For_XmRowColumnCallbackStruct(bindings_list, lexical_env, call_data, o_widget)
     LVAL bindings_list;	/* a list of symbols to which values from XmRowColumnCallbackStruct are bound */
     LVAL lexical_env;		
     caddr_t call_data;		/* a pointer to XmRowColumnCallbackStruct */
     LVAL o_widget;		/* XLTYPE_WIDGETOBJ */
{
  extern LVAL true;
  extern LVAL Wcls_WidgetID_To_WIDGETOBJ();
  XmRowColumnCallbackStruct* cd = (XmRowColumnCallbackStruct*) call_data;
  LVAL s_bindname;

  for ( ; consp(bindings_list); bindings_list = cdr(bindings_list)) {

    s_bindname = car(bindings_list);

    if (s_bindname == s_CALLBACK_WIDGET) {
      xlpbind(s_bindname, o_widget, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_REASON) {
      xlpbind(s_bindname, Wcb_Get_Callback_Reason_Symbol(cd->reason), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_XEVENT) {
      xlpbind(s_bindname, (cd->event) ? cv_xevent(cd->event) : NIL, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_ENTRY_WIDGET) {
      xlpbind(s_bindname, (cd->widget) ? Wcls_WidgetID_To_WIDGETOBJ(cd->widget) : NIL, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_ENTRY_REASON) {
      xlpbind(s_bindname, (cd->callbackstruct) ? Wcb_Get_Callback_Reason_Symbol(((XmToggleButtonCallbackStruct*) cd->callbackstruct)->reason) : NIL, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_ENTRY_SET) { /* this'll give meaningless results if entry isn't a togglebutton  */
      xlpbind(s_bindname, (cd->callbackstruct) ? ((((XmToggleButtonCallbackStruct*) cd->callbackstruct)->set) ? true : NIL) : NIL, lexical_env);
    }
    /* ----- WARNING: Using this could cause coredump!! -- this should dereference to the CALLBACKOBJ set for the entry, else NIL ---- */
    else if (s_bindname == s_CALLBACK_ENTRY_DATA) { 
      xlpbind(s_bindname, (LVAL) cd->data, lexical_env);
    }
    /* ------------------------------------------------------------------------------------------------------------------------------- */
    else {
      extern char temptext[];	/* from winterp.c */
      sprintf(temptext,
	      "Unknown binding name in XmRowColumnCallbackStruct callback evaluator. Valid symbols are [%s %s %s %s %s %s %s].",
	      (char*) getstring(getpname(s_CALLBACK_WIDGET)),
	      (char*) getstring(getpname(s_CALLBACK_REASON)),
	      (char*) getstring(getpname(s_CALLBACK_XEVENT)),
	      (char*) getstring(getpname(s_CALLBACK_ENTRY_WIDGET)),
	      (char*) getstring(getpname(s_CALLBACK_ENTRY_REASON)),
	      (char*) getstring(getpname(s_CALLBACK_ENTRY_SET)),
	      (char*) getstring(getpname(s_CALLBACK_ENTRY_DATA)));
      xlerror(temptext, s_bindname);
    }
  }
}


/******************************************************************************
 * typedef struct
 * {
 *    int reason;
 *    XEvent * event;
 *    int value;                 -- new slider location value
 *    int pixel;                 -- this is a stupid name: contains the coord of the XmNto{Top,Bottom}Calback
 * } XmScrollBarCallbackStruct;
 ******************************************************************************/
static void Wcb_Lexical_Bindings_For_XmScrollBarCallbackStruct(bindings_list, lexical_env, call_data, o_widget)
     LVAL bindings_list;	/* a list of symbols to which values from XmScrollBarCallbackStruct are bound */
     LVAL lexical_env;		
     caddr_t call_data;		/* a pointer to XmScrollBarCallbackStruct */
     LVAL o_widget;		/* XLTYPE_WIDGETOBJ */
{
  XmScrollBarCallbackStruct* cd = (XmScrollBarCallbackStruct*) call_data;
  LVAL s_bindname;

  for ( ; consp(bindings_list); bindings_list = cdr(bindings_list)) {

    s_bindname = car(bindings_list);

    if (s_bindname == s_CALLBACK_WIDGET) {
      xlpbind(s_bindname, o_widget, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_REASON) {
      xlpbind(s_bindname, Wcb_Get_Callback_Reason_Symbol(cd->reason), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_XEVENT) {
      xlpbind(s_bindname, (cd->event) ? cv_xevent(cd->event) : NIL, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_VALUE) {
      xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->value), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_PIXEL) {
      xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->pixel), lexical_env);
    }
    else {
      extern char temptext[];	/* from winterp.c */
      sprintf(temptext,
	      "Unknown binding name in XmScrollBarCallbackStruct callback evaluator. Valid symbols are [%s %s %s %s %s].",
	      (char*) getstring(getpname(s_CALLBACK_WIDGET)),
	      (char*) getstring(getpname(s_CALLBACK_REASON)),
	      (char*) getstring(getpname(s_CALLBACK_XEVENT)),
	      (char*) getstring(getpname(s_CALLBACK_VALUE)),
	      (char*) getstring(getpname(s_CALLBACK_PIXEL)));
      xlerror(temptext, s_bindname);
    }
  }
}


/******************************************************************************
 * typedef struct
 * {
 *    int reason;
 *    XEvent * event;
 *    int set;
 * } XmToggleButtonCallbackStruct;
 ******************************************************************************/
static void Wcb_Lexical_Bindings_For_XmToggleButtonCallbackStruct(bindings_list, lexical_env, call_data, o_widget)
     LVAL bindings_list;	/* a list of symbols to which values from XmToggleButtonCallbackStruct are bound */
     LVAL lexical_env;		
     caddr_t call_data;		/* a pointer to XmToggleButtonCallbackStruct */
     LVAL o_widget;		/* XLTYPE_WIDGETOBJ */
{
  extern LVAL true;
  XmToggleButtonCallbackStruct* cd = (XmToggleButtonCallbackStruct*) call_data;
  LVAL s_bindname;

  for ( ; consp(bindings_list); bindings_list = cdr(bindings_list)) {

    s_bindname = car(bindings_list);

    if (s_bindname == s_CALLBACK_WIDGET) {
      xlpbind(s_bindname, o_widget, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_REASON) {
      xlpbind(s_bindname, Wcb_Get_Callback_Reason_Symbol(cd->reason), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_XEVENT) {
      xlpbind(s_bindname, (cd->event) ? cv_xevent(cd->event) : NIL, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_SET) {
      xlpbind(s_bindname, (cd->set) ? true : NIL, lexical_env);
    }
    else {
      extern char temptext[];	/* from winterp.c */
      sprintf(temptext,
	      "Unknown binding name in XmToggleButtonCallbackStruct callback evaluator. Valid symbols are [%s %s %s %s].",
	      (char*) getstring(getpname(s_CALLBACK_WIDGET)),
	      (char*) getstring(getpname(s_CALLBACK_REASON)),
	      (char*) getstring(getpname(s_CALLBACK_XEVENT)),
	      (char*) getstring(getpname(s_CALLBACK_SET)));
      xlerror(temptext, s_bindname);
    }
  }
}


/******************************************************************************
 * typedef struct
 * {
 *    int 	reason;
 *    XEvent    *event;
 *    XmString  item;
 *    int       item_length;
 *    int       item_position;
 *    XmString  *selected_items;
 *    int       selected_item_count;
 *    int       selection_type;
 * } XmListCallbackStruct;
 ******************************************************************************/
static void Wcb_Lexical_Bindings_For_XmListCallbackStruct(bindings_list, lexical_env, call_data, o_widget)
     LVAL bindings_list;	/* a list of symbols to which values from XmListCallbackStruct are bound */
     LVAL lexical_env;		
     caddr_t call_data;		/* a pointer to XmListCallbackStruct */
     LVAL o_widget;		/* XLTYPE_WIDGETOBJ */
{
  extern LVAL Wxms_XmStringTable_To_Lisp_Vector(); /* from w_XmString.c */
  XmListCallbackStruct* cd = (XmListCallbackStruct*) call_data;
  LVAL                  s_bindname;

  for ( ; consp(bindings_list); bindings_list = cdr(bindings_list)) {

    s_bindname = car(bindings_list);

    if (s_bindname == s_CALLBACK_WIDGET) {
      xlpbind(s_bindname, o_widget, lexical_env); /* add binding to lexical_env */
    }
    else if (s_bindname == s_CALLBACK_REASON) {
      xlpbind(s_bindname, Wcb_Get_Callback_Reason_Symbol(cd->reason), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_XEVENT) {
      xlpbind(s_bindname, (cd->event) ? cv_xevent(cd->event) : NIL, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_ITEM) {
      xlpbind(s_bindname, (cd->item) ? cv_xmstring(XmStringCopy(cd->item)) : NIL, lexical_env); /* the copied XmString may be dealloc'd later by gc */
    }
    else if (s_bindname == s_CALLBACK_ITEM_LENGTH) {
      xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->item_length), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_ITEM_POSITION) {
      xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->item_position), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_SELECTED_ITEMS) {
      xlpbind(s_bindname, Wxms_XmStringTable_To_Lisp_Vector(cd->selected_items, cd->selected_item_count), lexical_env);	/* the vector and it's constituent XmStrings may be freed later via gc */
    }
    else if (s_bindname == s_CALLBACK_SELECTED_ITEM_COUNT) {
      xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->selected_item_count), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_SELECTION_TYPE)
      switch (cd->selection_type) {
      case XmINITIAL:
	xlpbind(s_bindname, s_INITIAL, lexical_env);
	break;
      case XmADDITION:
	xlpbind(s_bindname, s_ADDITION, lexical_env);
	break;
      case XmMODIFICATION:
	xlpbind(s_bindname, s_MODIFICATION, lexical_env);
	break;
      default:
	xlfail("Internal error in Wcb_Lexical_Bindings_For_XmListCallbackStruct() -- unknown cb->selection_type.");
	break;
      }
    else {
      extern char temptext[];	/* from winterp.c */
      sprintf(temptext,
	      "Unknown binding name in XmListCallbackStruct callback evaluator. Valid symbols are [%s %s %s %s %s %s %s %s %s].",
	      (char*) getstring(getpname(s_CALLBACK_WIDGET)),
	      (char*) getstring(getpname(s_CALLBACK_REASON)),
	      (char*) getstring(getpname(s_CALLBACK_XEVENT)),
	      (char*) getstring(getpname(s_CALLBACK_ITEM)),
	      (char*) getstring(getpname(s_CALLBACK_ITEM_LENGTH)),
	      (char*) getstring(getpname(s_CALLBACK_ITEM_POSITION)),
	      (char*) getstring(getpname(s_CALLBACK_SELECTED_ITEMS)),
	      (char*) getstring(getpname(s_CALLBACK_SELECTED_ITEM_COUNT)),
	      (char*) getstring(getpname(s_CALLBACK_SELECTION_TYPE)));
      xlerror(temptext, s_bindname);
    }
  }
}

/******************************************************************************
 * typedef struct
 * {
 *     int reason;
 *     XEvent	*event;
 *     XmString	value;
 *     int	length;
 * } XmSelectionBoxCallbackStruct;
 ******************************************************************************/
static void Wcb_Lexical_Bindings_For_XmSelectionBoxCallbackStruct(bindings_list, lexical_env, call_data, o_widget)
     LVAL bindings_list;	/* a list of symbols to which values from XmSelectionBoxCallbackStruct are bound */
     LVAL lexical_env;		
     caddr_t call_data;		/* a pointer to XmSelectionBoxCallbackStruct */
     LVAL o_widget;		/* XLTYPE_WIDGETOBJ */
{
  XmSelectionBoxCallbackStruct* cd = (XmSelectionBoxCallbackStruct*) call_data;
  LVAL s_bindname;

  for ( ; consp(bindings_list); bindings_list = cdr(bindings_list)) {

    s_bindname = car(bindings_list);

    if (s_bindname == s_CALLBACK_WIDGET) {
      xlpbind(s_bindname, o_widget, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_REASON) {
      xlpbind(s_bindname, Wcb_Get_Callback_Reason_Symbol(cd->reason), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_XEVENT) {
      xlpbind(s_bindname, (cd->event) ? cv_xevent(cd->event) : NIL, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_VALUE) {
      xlpbind(s_bindname, (cd->value) ? cv_xmstring(XmStringCopy(cd->value)) : NIL, lexical_env); /* copy the XmString even though cd->value never looks like it gets freed -- it's inconsistant w/ other callback XmStrings.. maybe it'll get fixed eventually */
    }
    else if (s_bindname == s_CALLBACK_LENGTH) {
      xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->length), lexical_env);
    }
    else {
      extern char temptext[];	/* from winterp.c */
      sprintf(temptext,
	      "Unknown binding name in XmSelectionBoxCallbackStruct callback evaluator. Valid symbols are [%s %s %s %s %s].",
	      (char*) getstring(getpname(s_CALLBACK_WIDGET)),
	      (char*) getstring(getpname(s_CALLBACK_REASON)),
	      (char*) getstring(getpname(s_CALLBACK_XEVENT)),
	      (char*) getstring(getpname(s_CALLBACK_VALUE)),
	      (char*) getstring(getpname(s_CALLBACK_LENGTH)));
      xlerror(temptext, s_bindname);
    }
  }
}


/******************************************************************************
 * typedef struct
 * {
 *     int reason;
 *     XEvent	*event;
 *     XmString	value;
 *     int	length;
 * } XmCommandCallbackStruct;
 ******************************************************************************/
static void Wcb_Lexical_Bindings_For_XmCommandCallbackStruct(bindings_list, lexical_env, call_data, o_widget)
     LVAL bindings_list;	/* a list of symbols to which values from XmCommandCallbackStruct are bound */
     LVAL lexical_env;		
     caddr_t call_data;		/* a pointer to XmCommandCallbackStruct */
     LVAL o_widget;		/* XLTYPE_WIDGETOBJ */
{
  XmCommandCallbackStruct* cd = (XmCommandCallbackStruct*) call_data;
  LVAL s_bindname;

  for ( ; consp(bindings_list); bindings_list = cdr(bindings_list)) {

    s_bindname = car(bindings_list);

    if (s_bindname == s_CALLBACK_WIDGET) {
      xlpbind(s_bindname, o_widget, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_REASON) {
      xlpbind(s_bindname, Wcb_Get_Callback_Reason_Symbol(cd->reason), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_XEVENT) {
      xlpbind(s_bindname, (cd->event) ? cv_xevent(cd->event) : NIL, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_VALUE) {
      xlpbind(s_bindname, (cd->value) ? cv_xmstring(XmStringCopy(cd->value)) : NIL, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_LENGTH) {
      xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->length), lexical_env);
    }
    else {
      extern char temptext[];	/* from winterp.c */
      sprintf(temptext,
	      "Unknown binding name in XmCommandCallbackStruct callback evaluator. Valid symbols are [%s %s %s %s %s].",
	      (char*) getstring(getpname(s_CALLBACK_WIDGET)),
	      (char*) getstring(getpname(s_CALLBACK_REASON)),
	      (char*) getstring(getpname(s_CALLBACK_XEVENT)),
	      (char*) getstring(getpname(s_CALLBACK_VALUE)),
	      (char*) getstring(getpname(s_CALLBACK_LENGTH)));
      xlerror(temptext, s_bindname);
    }
  }
}


/******************************************************************************
 * typedef struct
 * {
 *     int reason;
 *     XEvent	*event;
 *     XmString	value;
 *     int		length;
 *     XmString	mask;
 *     int		mask_length;
 * } XmFileSelectionBoxCallbackStruct;
 ******************************************************************************/
static void Wcb_Lexical_Bindings_For_XmFileSelectionBoxCallbackStruct(bindings_list, lexical_env, call_data, o_widget)
     LVAL bindings_list;	/* a list of symbols to which values from XmFileSelectionBoxCallbackStruct are bound */
     LVAL lexical_env;		
     caddr_t call_data;		/* a pointer to XmFileSelectionBoxCallbackStruct */
     LVAL o_widget;		/* XLTYPE_WIDGETOBJ */
{
  XmFileSelectionBoxCallbackStruct* cd = (XmFileSelectionBoxCallbackStruct*) call_data;
  LVAL                 s_bindname;

  for ( ; consp(bindings_list); bindings_list = cdr(bindings_list)) {

    s_bindname = car(bindings_list);

    if (s_bindname == s_CALLBACK_WIDGET) {
      xlpbind(s_bindname, o_widget, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_REASON) {
      xlpbind(s_bindname, Wcb_Get_Callback_Reason_Symbol(cd->reason), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_XEVENT) {
      xlpbind(s_bindname, (cd->event) ? cv_xevent(cd->event) : NIL, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_VALUE) {
      xlpbind(s_bindname, (cd->value) ? cv_xmstring(XmStringCopy(cd->value)) : NIL, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_LENGTH) {
      xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->length), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_MASK) {
      xlpbind(s_bindname, (cd->mask) ? cv_xmstring(XmStringCopy(cd->mask)) : NIL, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_MASK_LENGTH) {
      xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->mask_length), lexical_env);
    }
    else {
      extern char temptext[];	/* from winterp.c */
      sprintf(temptext,
	      "Unknown binding name in XmFileSelectionBoxCallbackStruct callback evaluator. Valid symbols are [%s %s %s %s %s %s %s].",
	      (char*) getstring(getpname(s_CALLBACK_WIDGET)),
	      (char*) getstring(getpname(s_CALLBACK_REASON)),
	      (char*) getstring(getpname(s_CALLBACK_XEVENT)),
	      (char*) getstring(getpname(s_CALLBACK_VALUE)),
	      (char*) getstring(getpname(s_CALLBACK_LENGTH)),
	      (char*) getstring(getpname(s_CALLBACK_MASK)),
	      (char*) getstring(getpname(s_CALLBACK_MASK_LENGTH)));
      xlerror(temptext, s_bindname);
    }
  }
}


/******************************************************************************
 * typedef struct 
 * {
 *    int reason;
 *    XEvent * event;
 *    int value;
 * } XmScaleCallbackStruct;
 ******************************************************************************/
static void Wcb_Lexical_Bindings_For_XmScaleCallbackStruct(bindings_list, lexical_env, call_data, o_widget)
     LVAL bindings_list;	/* a list of symbols to which values from XmScaleCallbackStruct are bound */
     LVAL lexical_env;		
     caddr_t call_data;		/* a pointer to XmScaleCallbackStruct */
     LVAL o_widget;		/* XLTYPE_WIDGETOBJ */
{
  XmScaleCallbackStruct* cd = (XmScaleCallbackStruct*) call_data;
  LVAL s_bindname;

  for ( ; consp(bindings_list); bindings_list = cdr(bindings_list)) {

    s_bindname = car(bindings_list);

    if (s_bindname == s_CALLBACK_WIDGET) {
      xlpbind(s_bindname, o_widget, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_REASON) {
      xlpbind(s_bindname, Wcb_Get_Callback_Reason_Symbol(cd->reason), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_XEVENT) {
      xlpbind(s_bindname, (cd->event) ? cv_xevent(cd->event) : NIL, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_VALUE) {
      xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->value), lexical_env);
    }
    else {
      extern char temptext[];	/* from winterp.c */
      sprintf(temptext,
	      "Unknown binding name in XmScaleCallbackStruct callback evaluator. Valid symbols are [%s %s %s%s].",
	      (char*) getstring(getpname(s_CALLBACK_WIDGET)),
	      (char*) getstring(getpname(s_CALLBACK_REASON)),
	      (char*) getstring(getpname(s_CALLBACK_XEVENT)),
	      (char*) getstring(getpname(s_CALLBACK_VALUE)));
      xlerror(temptext, s_bindname);
    }
  }
}


/******************************************************************************
 * typedef struct {
 *     char *ptr;               -- Pointer to data.
 *     int length;              -- Number of bytes of data.
 *     XmTextFormat format;     -- Representation format.
 * } XmTextBlockRec, *XmTextBlock;
 * 
 * typedef struct
 * {
 *     int reason;
 *     XEvent  *event;
 *     Boolean doit;                             -- NOTE: not get/settable in this version of WINTERP
 *     XmTextPosition currInsert, newInsert;
 *     XmTextPosition startPos, endPos;
 *     XmTextBlock text;                         -- NOTE: not get/settable in this version of WINTERP
 * } XmTextVerifyCallbackStruct, *XmTextVerifyPtr;
 *
 * Note that the text callbacks are "weird" in that they expect values in
 * the callback structure to be set inside the callback proc to determine
 * what actions need to be taken after the callbackproc returns. In
 * particular, the XmTextVerifyCallbackStruct's 'doit' slot is always set to True,
 * and must be set to False if the callbackproc doesn't want the action to be
 * taken. The 'text' slot is set inside the  callbackproc to the information to
 * be inserted. I'm not going to support either of these operations in this 
 * version of winterp since I have better things to worry about. As far as I
 * can tell, all the capabilities supported by mucking with these slots in the
 * callback can be emulated by calling winterp methods on the text widget inside
 * the lisp callback.
 * 
 * Another inconsistency with the Text widget is that some callbacks on this widget
 * return XmAnyCallbackStruct's (XmNactivateCallback, XmNfocusCallback,
 * XmNvalueChangedCallback), whereas XmNlosingFocusCallback, XmNmodifyVerifyCallback,
 * and XmNmotionVerifyCallback return XmTextVerifyCallbackStruct. Since the "reason"
 * and "event" slots are both in the same position in the two structs, it should be
 * fine to access both those slots in either callbackstruct. Bogus data will get
 * returned if you're accessing slots like currInsert, newInsert,  startPos, endPos
 * for a callback returning XmAnyCallbackStruct.
 *
 * Making all this work correctly would require that a special hacked version
 * of :set_callback and :add_callback methods would have to be placed on the
 * text widget class. Maybe I'll do this later after I actually need to do
 * some fancier forms entry on the text widget. For now, it's good enough.
 ******************************************************************************/
static void Wcb_Lexical_Bindings_For_XmTextVerifyCallbackStruct(bindings_list, lexical_env, call_data, o_widget)
     LVAL bindings_list;	/* a list of symbols to which values from XmTextVerifyCallbackStruct are bound */
     LVAL lexical_env;		
     caddr_t call_data;		/* a pointer to XmTextVerifyCallbackStruct */
     LVAL o_widget;		/* XLTYPE_WIDGETOBJ */
{
  XmTextVerifyCallbackStruct* cd = (XmTextVerifyCallbackStruct*) call_data;
  LVAL                 s_bindname;

  for ( ; consp(bindings_list); bindings_list = cdr(bindings_list)) {

    s_bindname = car(bindings_list);

    if (s_bindname == s_CALLBACK_WIDGET) {
      xlpbind(s_bindname, o_widget, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_REASON) {
      xlpbind(s_bindname, Wcb_Get_Callback_Reason_Symbol(cd->reason), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_XEVENT) {
      xlpbind(s_bindname, (cd->event) ? cv_xevent(cd->event) : NIL, lexical_env);
    }
    else if (s_bindname == s_CALLBACK_CUR_INSERT) {
      xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->currInsert), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_NEW_INSERT) {
      xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->newInsert), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_START_POS) {
      xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->startPos), lexical_env);
    }
    else if (s_bindname == s_CALLBACK_END_POS) {
      xlpbind(s_bindname, cvfixnum((FIXTYPE) cd->endPos), lexical_env);
    }
    else {
      extern char temptext[];	/* from winterp.c */
      sprintf(temptext,
	      "Unknown binding name in XmTextVerifyCallbackStruct callback evaluator. Valid symbols are [%s %s %s %s %s %s %s].",
	      (char*) getstring(getpname(s_CALLBACK_WIDGET)),
	      (char*) getstring(getpname(s_CALLBACK_REASON)),
	      (char*) getstring(getpname(s_CALLBACK_XEVENT)),
	      (char*) getstring(getpname(s_CALLBACK_CUR_INSERT)),
	      (char*) getstring(getpname(s_CALLBACK_NEW_INSERT)),
	      (char*) getstring(getpname(s_CALLBACK_START_POS)),
	      (char*) getstring(getpname(s_CALLBACK_END_POS)));
      xlerror(temptext, s_bindname);
    }
  }
}


/******************************************************************************
 * This is called indirectly via XtAddCallback() for callbacks returning
 * an XmAnyCallbackStruct as call_data. (see w_classes.c).
 ******************************************************************************/
void Wcb_XmAnyCallbackStruct_Callbackproc(widget, client_data, call_data)
     Widget  widget;
     caddr_t client_data;
     caddr_t call_data;
{
  Wcb_Meta_Callbackproc(widget, client_data, call_data,
			Wcb_Lexical_Bindings_For_XmAnyCallbackStruct);
}


/******************************************************************************
 * This is called indirectly via XtAddCallback() for callbacks returning
 * an XmDrawingAreaCallbackStruct as call_data. (see wc_DrawingA.c).
 ******************************************************************************/
void Wcb_XmDrawingAreaCallbackStruct_Callbackproc(widget, client_data, call_data)
     Widget  widget;
     caddr_t client_data;
     caddr_t call_data;
{
  Wcb_Meta_Callbackproc(widget, client_data, call_data,
			Wcb_Lexical_Bindings_For_XmDrawingAreaCallbackStruct);
}


/******************************************************************************
 * This is called indirectly via XtAddCallback() for callbacks returning
 * an XmDrawnButtonCallbackStruct as call_data. (see wc_DrawnB.c).
 ******************************************************************************/
void Wcb_XmDrawnButtonCallbackStruct_Callbackproc(widget, client_data, call_data)
     Widget  widget;
     caddr_t client_data;
     caddr_t call_data;
{
  Wcb_Meta_Callbackproc(widget, client_data, call_data,
			Wcb_Lexical_Bindings_For_XmDrawnButtonCallbackStruct);
}


/******************************************************************************
 * This is called indirectly via XtAddCallback() for callbacks returning
 * an XmRowColumnCallbackStruct as call_data. (see wc_RowColumn.c).
 ******************************************************************************/
void Wcb_XmRowColumnCallbackStruct_Callbackproc(widget, client_data, call_data)
     Widget  widget;
     caddr_t client_data;
     caddr_t call_data;
{
  Wcb_Meta_Callbackproc(widget, client_data, call_data,
			Wcb_Lexical_Bindings_For_XmRowColumnCallbackStruct);
}


/******************************************************************************
 * This is called indirectly via XtAddCallback() for callbacks returning
 * an XmScrollBarCallbackStruct as call_data. (see wc_ScrollBar.c).
 ******************************************************************************/
void Wcb_XmScrollBarCallbackStruct_Callbackproc(widget, client_data, call_data)
     Widget  widget;
     caddr_t client_data;
     caddr_t call_data;
{
  Wcb_Meta_Callbackproc(widget, client_data, call_data,
			Wcb_Lexical_Bindings_For_XmScrollBarCallbackStruct);
}


/******************************************************************************
 * This is called indirectly via XtAddCallback() for callbacks returning
 * an XmToggleButtonCallbackStruct as call_data. (see wc_ToggleB.c).
 ******************************************************************************/
void Wcb_XmToggleButtonCallbackStruct_Callbackproc(widget, client_data, call_data)
     Widget  widget;
     caddr_t client_data;
     caddr_t call_data;
{
  Wcb_Meta_Callbackproc(widget, client_data, call_data,
			Wcb_Lexical_Bindings_For_XmToggleButtonCallbackStruct);
}


/*******************************************************************************
 * This is called indirectly via XtAddCallback() for callbacks returning
 * an XmListCallbackStruct as call_data. (see wc_List.c).
 ******************************************************************************/
void Wcb_XmListCallbackStruct_Callbackproc(widget, client_data, call_data)
     Widget  widget;
     caddr_t client_data;
     caddr_t call_data;
{
  Wcb_Meta_Callbackproc(widget, client_data, call_data,
			Wcb_Lexical_Bindings_For_XmListCallbackStruct);
}


/******************************************************************************
 * This is called indirectly via XtAddCallback() for callbacks returning
 * an XmSelectionBoxCallbackStruct as call_data. (see wc_SelectioB.c).
 ******************************************************************************/
void Wcb_XmSelectionBoxCallbackStruct_Callbackproc(widget, client_data, call_data)
     Widget  widget;
     caddr_t client_data;
     caddr_t call_data;
{
  Wcb_Meta_Callbackproc(widget, client_data, call_data,
			Wcb_Lexical_Bindings_For_XmSelectionBoxCallbackStruct);
}


/******************************************************************************
 * This is called indirectly via XtAddCallback() for callbacks returning
 * an XmCommandCallbackStruct as call_data. (see wc_Command.c).
 ******************************************************************************/
void Wcb_XmCommandCallbackStruct_Callbackproc(widget, client_data, call_data)
     Widget  widget;
     caddr_t client_data;
     caddr_t call_data;
{
  Wcb_Meta_Callbackproc(widget, client_data, call_data,
			Wcb_Lexical_Bindings_For_XmCommandCallbackStruct);
}


/******************************************************************************
 * This is called indirectly via XtAddCallback() for callbacks returning
 * an XmFileSelectionBoxCallbackStruct as call_data. (see wc_FileSB.c).
 ******************************************************************************/
void Wcb_XmFileSelectionBoxCallbackStruct_Callbackproc(widget, client_data, call_data)
     Widget  widget;
     caddr_t client_data;
     caddr_t call_data;
{
  Wcb_Meta_Callbackproc(widget, client_data, call_data,
			Wcb_Lexical_Bindings_For_XmFileSelectionBoxCallbackStruct);
}


/******************************************************************************
 * This is called indirectly via XtAddCallback() for callbacks returning
 * an XmScaleCallbackStruct as call_data. (see wc_Scale.c).
 ******************************************************************************/
void Wcb_XmScaleCallbackStruct_Callbackproc(widget, client_data, call_data)
     Widget  widget;
     caddr_t client_data;
     caddr_t call_data;
{
  Wcb_Meta_Callbackproc(widget, client_data, call_data,
			Wcb_Lexical_Bindings_For_XmScaleCallbackStruct);
}


/******************************************************************************
 * This is called indirectly via XtAddCallback() for callbacks returning
 * an XmTextVerifyCallbackStruct as call_data. (see wc_Text.c).
 ******************************************************************************/
void Wcb_XmTextVerifyCallbackStruct_Callbackproc(widget, client_data, call_data)
     Widget  widget;
     caddr_t client_data;
     caddr_t call_data;
{
  Wcb_Meta_Callbackproc(widget, client_data, call_data,
			Wcb_Lexical_Bindings_For_XmTextVerifyCallbackStruct);
}


/*******************************************************************************
 *
 ******************************************************************************/
LVAL Wcb_Meta_Method_Add_Callback(callback_proc, one_callback_per_name_p)
     XtCallbackProc callback_proc;
     Boolean        one_callback_per_name_p;
{
  extern char* Wres_Get_Name();	/* w_resources.c */
  extern LVAL  Wres_Get_Symbol(); /* w_resources.c */
  extern Boolean Wres_Is_Callback_P(); /* w_resources.c */
  extern LVAL s_lambda, xlenv, xlfenv;
  LVAL   o_self, l_fargs, l_code;
  LVAL   xtr_name, s_callback, callback_obj;
  Widget widget_id;
  char*  name;
  
  /* get <widget_instance> */
  o_self = xlga_widgetobj();

  if (!(widget_id = get_widgetobj_widgetID(o_self)))
    xlerror("widget object not properly initialized by :isnew.", o_self);
				    
  /* get <name> */
  xtr_name = xlgetarg();
  if (xtresource_p(xtr_name) && Wres_Is_Callback_P(xtr_name)) {
    name = Wres_Get_Name(xtr_name);
    s_callback = Wres_Get_Symbol(xtr_name);
  }
  else if (symbolp(xtr_name)) {	
    xtr_name = getvalue(xtr_name);
    if (xtresource_p(xtr_name) && Wres_Is_Callback_P(xtr_name)) {
      name = Wres_Get_Name(xtr_name);
      s_callback = Wres_Get_Symbol(xtr_name);
    }
    else
      xlerror("Invalid callback name symbol", xtr_name);
  }
  else
    xlerror("Invalid callback name argument", xtr_name);
      
  /* get <call_data_binding_names_list> -- args to be bound at call time.
     NOTE: may want to check that these args are valid for the particular widget
     class that this "meta-method" is used in. No biggie though -- they'll get
     caught at runtime, when the callback fires. */
  l_fargs = xlgalist();		

  /* get <code> */
  l_code = xlgalist();
  xllastarg();

  /* 
   * if this procedure is being called from the :set_callback method 
   * (indicated by one_callback_per_name_p = TRUE), 
   * then remove all callbacks on <widget_inst> matching <name>
   */
  if (one_callback_per_name_p) { 
    extern LVAL v_savedobjs;	
    int  i = Wso_Hash(o_self);
    LVAL l_hbucket = getelement(v_savedobjs, i); /* a list of objects, including all callback-objs on this widget */
    LVAL obj;
    LVAL l_prev = NIL;
    while (l_hbucket) {		/* while there are elements in the hashbucket */
      obj = car(l_hbucket);	/* obj points to cur elt */
      if (callbackobj_p(obj) && (get_callback_name(obj) == name) && (get_callback_widget(obj) == o_self)) {
	XtRemoveCallback(widget_id, name, get_callback_proc(obj), (caddr_t) obj);
	l_hbucket = cdr(l_hbucket); /* l_hbucket now points to next elt or NIL */
	if (!l_prev)
	  setelement(v_savedobjs, i, l_hbucket); /* remove first, head is now next elt */
	else
	  rplacd(l_prev, l_hbucket); /* remove cur, point previous to next */
      }
      else {
	l_prev = l_hbucket;
	l_hbucket = cdr(l_hbucket);
      }
    }
  }

  /* 
   * create the client_data to be sent to (*callback_proc)()
   * That procedure takes the client_data and extracts the widget-object,
   * and the closure, and uses these to execute the callback.
   */
  xlsave1(callback_obj);	/* protect some pointers */
  callback_obj = new_callbackobj();
  set_callback_widget(callback_obj, o_self);
  set_callback_name(callback_obj, name);
  set_callback_proc(callback_obj, callback_proc);
  set_callback_closure(callback_obj,
		       xlclose(s_callback, s_lambda, l_fargs, l_code, xlenv, xlfenv));
  
  XtAddCallback(widget_id, name, callback_proc, (caddr_t) callback_obj);

  /*
   * Enter the callback_obj in v_savedobjs, so that it gets marked.
   * This way, it won't be garbage collected while the callback is
   * active. :set_callback, xt_remove_callback, and :remove_all_callbacks
   * may remove and destroy the callback_obj created here. Destroying the
   * widget will result in the callbackobj getting garbage collected 
   * -- see Wcls_Widget_Destroy_Callback()
   */
  { 
    int  i = Wso_Hash(o_self);
    LVAL l_hbucket;
    extern LVAL v_savedobjs;
    
    xlsave1(l_hbucket);
    l_hbucket = cons(callback_obj, getelement(v_savedobjs, i));
    setelement(v_savedobjs, i, l_hbucket);
    xlpop();
  }

  /* resore the stack */
  xlpop(/*callback_obj*/);

  return (callback_obj);
}


/******************************************************************************
 * lisp: (SEND <widget> :REMOVE_ALL_CALLBACKS <name>)
 * returns T.
 *
 * <name> is a resource keyword of type XmRCallback, eg,
 * :XMN_ACTIVATE_CALLBACK, :XMN_ARM_CALLBACK, :XMN_DISARM_CALLBACK.
 *
 * This procedure removes all callbacks matching <name> for <widget>.
 ******************************************************************************/
LVAL Widget_Class_Method_REMOVE_ALL_CALLBACKS()
{
  extern char* Wres_Get_Name();	/* w_resources.c */
  extern Boolean Wres_Is_Callback_P(); /* w_resources.c */
  LVAL   o_self;
  LVAL   xtr_name;
  Widget widget_id;
  char*  name;
  
  /* get <widget_instance> */
  o_self = xlga_widgetobj();

  if (!(widget_id = get_widgetobj_widgetID(o_self)))
    xlerror("widget object not properly initialized by :isnew.", o_self);
				    
  /* get <name> */
  xtr_name = xlgetarg();
  if (xtresource_p(xtr_name) && Wres_Is_Callback_P(xtr_name)) {
    name = Wres_Get_Name(xtr_name);
  }
  else if (symbolp(xtr_name)) {	
    xtr_name = getvalue(xtr_name);
    if (xtresource_p(xtr_name) && Wres_Is_Callback_P(xtr_name)) {
      name = Wres_Get_Name(xtr_name);
    }
    else
      xlerror("Invalid callback name symbol", xtr_name);
  }
  else
    xlerror("Invalid callback name argument", xtr_name);
      
  xllastarg();

  /* remove from v_savedobjs all callback-objs on <widget_inst> matching <name> */
  {
    extern LVAL v_savedobjs;
    int  i = Wso_Hash(o_self);
    LVAL l_hbucket = getelement(v_savedobjs, i); /* a list of objects, including all callback-objs on this widget  */
    LVAL obj;
    LVAL l_prev = NIL;
    while (l_hbucket) {		/* while there are elements in the hashbucket */
      obj = car(l_hbucket);	/* obj points to cur elt */
      if (callbackobj_p(obj) && (get_callback_name(obj) == name) && (get_callback_widget(obj) == o_self)) {
	l_hbucket = cdr(l_hbucket); /* l_hbucket now points to next elt or NIL */
	if (!l_prev)
	  setelement(v_savedobjs, i, l_hbucket); /* remove first, head is now next elt */
	else
	  rplacd(l_prev, l_hbucket); /* remove cur, point previous to next */
      }
      else {
	l_prev = l_hbucket;
	l_hbucket = cdr(l_hbucket);
      }
    }
  }

  XtRemoveAllCallbacks(widget_id, name);

  return (o_self);
}


/******************************************************************************
 * lisp: (XT_REMOVE_CALLBACK <callback-obj>)
 * 
 * where <callback-obj> is the value returned by methods :set_callback or
 * :add_callback.
 ******************************************************************************/
LVAL Wcb_Prim_XtRemoveCallback()
{
  LVAL callback_obj;
  LVAL o_widget;
  Widget widget_id;
  extern LVAL true;

  callback_obj = xlga_callbackobj();
  xllastarg();

  /* check if this callback hasn't already been removed */
  if ((o_widget = get_callback_widget(callback_obj)) == NIL)
    xlerror("Callback associated with <callback-obj> has already been removed.", callback_obj);
  
  /* mark the callback_obj as being removed */
  set_callback_widget(callback_obj, NIL);

  if (!(widget_id = get_widgetobj_widgetID(o_widget)))
    xlerror("widget object not properly initialized by :isnew.", o_widget);
  
  XtRemoveCallback(widget_id,
		   get_callback_name(callback_obj),
		   get_callback_proc(callback_obj), /* note that there are difft callbackproc's for difft widgetclasses */
		   (caddr_t) callback_obj);

  /* remove <callback_obj> from v_savedobjs allowing it to be garbage collected */
  {
    extern LVAL v_savedobjs;
    int i = Wso_Hash(o_widget); /* note that we hash all callbacks on the same widget to the same hashbucket */
    LVAL l_hbucket = getelement(v_savedobjs, i);
    LVAL l_prev = NIL;

    while (l_hbucket && (car(l_hbucket) != callback_obj)) {
      l_prev = l_hbucket;
      l_hbucket = cdr(l_hbucket);
    }
    if (!l_hbucket)
      xlerror("Internal error in XtRemoveCallback -- couldn't remove <callback-obj> from v_savedobjs. Hash error?",
	      callback_obj);
    if (!l_prev)		/* first elt matched */
      setelement(v_savedobjs, i, cdr(l_hbucket));
    else
      rplacd(l_prev, cdr(l_hbucket));
  }
  
  return (true);
}

/******************************************************************************
 *
 ******************************************************************************/
Wcb_Init()
{

  s_CALLBACK_WIDGET = xlenter("CALLBACK_WIDGET");
  s_CALLBACK_REASON = xlenter("CALLBACK_REASON");
  s_CALLBACK_XEVENT = xlenter("CALLBACK_XEVENT");
  s_CALLBACK_WINDOW = xlenter("CALLBACK_WINDOW");
  s_CALLBACK_ITEM = xlenter("CALLBACK_ITEM");
  s_CALLBACK_ENTRY_WIDGET = xlenter("CALLBACK_ENTRY_WIDGET");
  s_CALLBACK_ENTRY_REASON = xlenter("CALLBACK_ENTRY_REASON");
  s_CALLBACK_ENTRY_SET = xlenter("CALLBACK_ENTRY_SET");
  s_CALLBACK_ENTRY_DATA = xlenter("CALLBACK_ENTRY_DATA");
  s_CALLBACK_VALUE = xlenter("CALLBACK_VALUE");
  s_CALLBACK_PIXEL = xlenter("CALLBACK_PIXEL");
  s_CALLBACK_SET = xlenter("CALLBACK_SET");
  s_CALLBACK_ITEM_LENGTH = xlenter("CALLBACK_ITEM_LENGTH");
  s_CALLBACK_ITEM_POSITION = xlenter("CALLBACK_ITEM_POSITION");
  s_CALLBACK_SELECTED_ITEMS = xlenter("CALLBACK_SELECTED_ITEMS");
  s_CALLBACK_SELECTED_ITEM_COUNT = xlenter("CALLBACK_SELECTED_ITEM_COUNT");
  s_CALLBACK_SELECTION_TYPE = xlenter("CALLBACK_SELECTION_TYPE");
  s_CALLBACK_LENGTH = xlenter("CALLBACK_LENGTH");
  s_CALLBACK_MASK = xlenter("CALLBACK_MASK");
  s_CALLBACK_MASK_LENGTH = xlenter("CALLBACK_MASK_LENGTH");
  s_CALLBACK_CUR_INSERT = xlenter("CALLBACK_CUR_INSERT");
  s_CALLBACK_NEW_INSERT = xlenter("CALLBACK_NEW_INSERT");
  s_CALLBACK_START_POS = xlenter("CALLBACK_START_POS");
  s_CALLBACK_END_POS = xlenter("CALLBACK_END_POS");

  s_INITIAL = xlenter("INITIAL");
  s_ADDITION = xlenter("ADDITION");
  s_MODIFICATION = xlenter("MODIFICATION");

  s_CR_NONE = xlenter("CR_NONE");
  s_CR_HELP = xlenter("CR_HELP");
  s_CR_VALUE_CHANGED = xlenter("CR_VALUE_CHANGED");
  s_CR_INCREMENT = xlenter("CR_INCREMENT");
  s_CR_DECREMENT = xlenter("CR_DECREMENT");
  s_CR_PAGE_INCREMENT = xlenter("CR_PAGE_INCREMENT");
  s_CR_PAGE_DECREMENT = xlenter("CR_PAGE_DECREMENT");
  s_CR_TO_TOP = xlenter("CR_TO_TOP");
  s_CR_TO_BOTTOM = xlenter("CR_TO_BOTTOM");
  s_CR_DRAG = xlenter("CR_DRAG");
  s_CR_ACTIVATE = xlenter("CR_ACTIVATE");
  s_CR_ARM = xlenter("CR_ARM");
  s_CR_DISARM = xlenter("CR_DISARM");
  s_CR_MAP = xlenter("CR_MAP");
  s_CR_UNMAP = xlenter("CR_UNMAP");
  s_CR_FOCUS = xlenter("CR_FOCUS");
  s_CR_LOSING_FOCUS = xlenter("CR_LOSING_FOCUS");
  s_CR_MODIFYING_TEXT_VALUE = xlenter("CR_MODIFYING_TEXT_VALUE");
  s_CR_MOVING_INSERT_CURSOR = xlenter("CR_MOVING_INSERT_CURSOR");
  s_CR_EXECUTE = xlenter("CR_EXECUTE");
  s_CR_SINGLE_SELECT = xlenter("CR_SINGLE_SELECT");
  s_CR_MULTIPLE_SELECT = xlenter("CR_MULTIPLE_SELECT");
  s_CR_EXTENDED_SELECT = xlenter("CR_EXTENDED_SELECT");
  s_CR_BROWSE_SELECT = xlenter("CR_BROWSE_SELECT");
  s_CR_DEFAULT_ACTION = xlenter("CR_DEFAULT_ACTION");
  s_CR_CLIPBOARD_DATA_REQUEST = xlenter("CR_CLIPBOARD_DATA_REQUEST");
  s_CR_CLIPBOARD_DATA_DELETE = xlenter("CR_CLIPBOARD_DATA_DELETE");
  s_CR_CASCADING = xlenter("CR_CASCADING");
  s_CR_OK = xlenter("CR_OK");
  s_CR_CANCEL = xlenter("CR_CANCEL");
  s_CR_APPLY = xlenter("CR_APPLY");
  s_CR_NO_MATCH = xlenter("CR_NO_MATCH");
  s_CR_COMMAND_ENTERED = xlenter("CR_COMMAND_ENTERED");
  s_CR_COMMAND_CHANGED = xlenter("CR_COMMAND_CHANGED");
  s_CR_EXPOSE = xlenter("CR_EXPOSE");
  s_CR_RESIZE = xlenter("CR_RESIZE");
  s_CR_INPUT = xlenter("CR_INPUT");
}
