/* -*-C-*-
********************************************************************************
*
* File:         w_txlations.c
* RCS:          $Header: w_txlations.c,v 1.1 89/11/25 04:30:47 mayer Exp $
* Description:  Interfaces to Xtoolkit Translation Management Facilities
* Author:       Niels Mayer, HPLabs
* Created:      Wed Nov 22 02:08:31 1989
* Modified:     Sat Nov 25 04:30:36 1989 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: w_txlations.c,v 1.1 89/11/25 04:30:47 mayer Exp $";

#include <stdio.h>
#include <X11/Intrinsic.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"

static LVAL s_ACTION_WIDGET, s_ACTION_XEVENT, s_LISP_ACTION_PROC;


/******************************************************************************
 * Actions in winterp are added through this single action proc which will call
 * the lisp evaluator upon invocation of this action.
 *
 * A translation/accelerator table using this proc may look like
 * "<EnterWindow>: Lisp(lisp-function arg1 arg2 arg3)" in which case the
 * form '(lisp-function arg1 arg2 arg3)' will be passed on to the evaluator.
 *
 * 
 * In the lexical scope of the action invocation, ACTION_WIDGET gets bound  to
 * the widget causing the action, and ACTION_XEVENT gets bound to the event.
 *
 * BUG: I haven't found a way of escaping characters passed on to the
 * translation/action table parsers to allow lisp strings or lips lists to
 * be passed on to the lisp evaluator that is called by this function. Using
 * the evaluator through an action proc is still useful, just don't try to pass
 * in very complex lisp forms.
 * 
 * NOTE: the action table for winterp is set in winterp.c.
 ******************************************************************************/
void Wtx_Winterp_Lisp_Action_Proc(widget, event, params, num_params)
     Widget widget;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  register char c;
  register LVAL new_elt, last_elt;
  int param_count = *num_params;
  char* par;			/* points to current parameter */
  LVAL sexp_stream, parsed_sexp, oldenv;
  CONTEXT cntxt;
  extern LVAL Wcls_WidgetID_To_WIDGETOBJ(); /* w_classes.c */
  extern LVAL xlenv;

  xlstkcheck(3);
  xlsave(sexp_stream);
  xlsave(parsed_sexp);
  xlsave(oldenv);

  sexp_stream = newustream();	/* note - stream obj has ptrs for head and tail*/
  last_elt = cons(cvchar('('), NIL); /* tack on an opening paren to beginning of form*/
  sethead(sexp_stream, last_elt); /* make it the head of the stream */
  
  /* for each character in each param, append it onto sexp_stream */
  while (param_count--) {	/* while there are more parameters */
    par = *params++;		/* get current parameter string and increment to next */
    while (c = *par++) {	/* go through each parameter char by char turning into a stream */
      new_elt = cons(cvchar(c), NIL);
      rplacd(last_elt, new_elt); /* add new elt to tail of list */
      last_elt = new_elt;
    }
    new_elt = cons(cvchar(' '), NIL); /* put spaces between each param */
    rplacd(last_elt, new_elt);
    last_elt = new_elt;
  }
  new_elt = cons(cvchar(')'), NIL); /* tack on a closing paren to end of form */  
  rplacd(last_elt, new_elt);
  settail(sexp_stream, new_elt); /* streams are cdr-coded -- give ptr to tail */

  /*
   * Most of the rest of this procedure looks alot like xleval.c:evfun(), which
   * is what the evaluator calls when a functional form is to be evaluated. The
   * main difference is that instead of calling xlabind() to bind the
   * formal parameter symbols of a function to their values in the new
   * lexical environment frame returned by xlframe(getenv(fun)), we 
   * just bind ACTION_WIDGET and ACTION_XEVENT.
   */

  /* create a new environment frame */
  oldenv = xlenv;
  xlenv = xlframe(xlenv);

  /* lexically bind ACTION_WIDGET and ACTION_EVENT to the widget/event that caused the callback */
  xlpbind(s_ACTION_WIDGET, (widget) ? Wcls_WidgetID_To_WIDGETOBJ(widget) : NIL, xlenv);
  xlpbind(s_ACTION_XEVENT, (event) ? cv_xevent(event) : NIL, xlenv);

  /* setup the implicit block */
  xlbegin(&cntxt, CF_RETURN, s_LISP_ACTION_PROC);
  if (setjmp(cntxt.c_jmpbuf))
    { }
  else
    if (xlread(sexp_stream, &parsed_sexp, FALSE)) /* if didn't hit EOF during read */
      xleval(parsed_sexp);	/* then evaluate it -- we ignore evaluation result */

  xlend(&cntxt);

  /* restore the environment frame */
  xlenv = oldenv;
	 
  xlpopn(3);
}


/******************************************************************************
 * (XT_PARSE_TRANSLATION_TABLE <string>)
 * ==> returns a node of type XT_TRANSLATIONS, which is a compiled form of the
 * <string> translation table given as argument.
 *
 * XtTranslations XtParseTranslationTable(source)
 *     String source;
 *
 * NOTE: the memory allocated by XtParseTranslationTable() isn't freed anywhere
 * yet -- I can't find  any documentation that says you're supposed to free
 * this. For now this may be a memory leak.
 ******************************************************************************/
LVAL Wtx_Prim_XT_PARSE_TRANSLATION_TABLE()
{
  XtTranslations txl;
  char* source = (char*) getstring(xlgastring());
  xllastarg();
  
  txl = XtParseTranslationTable(source);

  return ( (txl) ? cv_xttranslations(txl) : NIL );
}

/******************************************************************************
 * (XT_PARSE_ACCELERATOR_TABLE <string>)
 * ==> returns a node of type XT_ACCELERATORS, which is a compiled form of the
 * <string> accelerator table given as argument.
 *
 * XtAccelerators XtParseAcceleratorTable (source)
 *     String   source;
 *
 * NOTE: the memory allocated by XtParseAcceleratorTable() isn't freed anywhere
 * yet -- I can't find  any documentation that says you're supposed to free
 * this. For now this may be a memory leak.
 ******************************************************************************/
LVAL Wtx_Prim_XT_PARSE_ACCELERATOR_TABLE()
{
  XtAccelerators axl;
  char* source = (char*) getstring(xlgastring());
  xllastarg();
  
  axl = XtParseAcceleratorTable(source);

  return ( (axl) ? cv_xtaccelerators(axl) : NIL );
}


/******************************************************************************
 * (send <widget> :OVERRIDE_TRANSLATIONS <translations>)
 * ==> returns <widget>
 * 
 * This method destructively merges the new <translations> into <widget>'s
 * existing translations -- event sequences in <translations> that already exist
 * a previous translation will override.
 *
 * <translations> can be a string, in which case it
 * is compiled into a translation table. Otherwise, we expect a node of
 * type XT_TRANSLATIONS.
 *
 * void XtOverrideTranslations(widget, new)
 *     Widget widget;
 *     XtTranslations new;
 ******************************************************************************/
LVAL Widget_Class_Method_OVERRIDE_TRANSLATIONS()
{
  LVAL self, lval_txl;
  Widget widget_id;
  XtTranslations txl;

  self = xlga_widgetobj();
  lval_txl = xlgetarg();
  xllastarg();
  
  if (stringp(lval_txl))
    txl = XtParseTranslationTable((char*) getstring(lval_txl));
  else if (xttranslations_p(lval_txl))
    txl = get_xttranslations(lval_txl);
  else
    xlerror("Bad argument type -- expected a string or XT_TRANSLATIONS.", lval_txl);

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  XtOverrideTranslations(widget_id, txl);
  return (self);
}

/******************************************************************************
 * (send <widget> :AUGMENT_TRANSLATIONS <translations>) 
 * ==> returns <widget>.
 *
 * This method nondestructively merges the new <translations> into <widget>'s
 * existing translations -- event sequences in <translations> that already exist
 * a previous translation will be ignored.
 *
 * <translations> can be a string, in which case it
 * is compiled into a translation table. Otherwise, we expect a node of
 * type XT_TRANSLATIONS.
 *
 * void XtAugmentTranslations(widget, new)
 *     Widget widget;
 *     XtTranslations new;
 ******************************************************************************/
LVAL Widget_Class_Method_AUGMENT_TRANSLATIONS()
{
  LVAL self, lval_txl;
  Widget widget_id;
  XtTranslations txl;

  self = xlga_widgetobj();
  lval_txl = xlgetarg();
  xllastarg();
  
  if (stringp(lval_txl))
    txl = XtParseTranslationTable((char*) getstring(lval_txl));
  else if (xttranslations_p(lval_txl))
    txl = get_xttranslations(lval_txl);
  else
    xlerror("Bad argument type -- expected a string or XT_TRANSLATIONS.", lval_txl);

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  XtAugmentTranslations(widget_id, txl);
  return (self);
}

/******************************************************************************
 * (send <widget> :UNINSTALL_TRANSLATIONS)
 * ==>  returns <widget>
 *
 * This method removes all translations from <widget>.
 *
 * void XtUninstallTranslations(widget)
 *     Widget widget;
 ******************************************************************************/
LVAL Widget_Class_Method_UNINSTALL_TRANSLATIONS()
{
  LVAL self;
  Widget widget_id;

  self = xlga_widgetobj();
  xllastarg();
  
  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  XtUninstallTranslations(widget_id);
  return (self);
}

/******************************************************************************
 * (send <destination> :INSTALL_ACCELERATORS <source>)
 * ==>  returns <destination>
 * 
 * This method installs the accelerators from widget <source> onto widget
 * <destination> by augmenting the destination translations with the source
 * accelerators.
 * 
 * void XtInstallAccelerators(destination,source)
 *     Widget destination,source;
 ******************************************************************************/
LVAL Widget_Class_Method_INSTALL_ACCELERATORS()
{
  LVAL self, lval_src;
  Widget widget_id, src;

  self = xlga_widgetobj();
  lval_src = xlga_widgetobj();
  xllastarg();
  
  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  if (!(src = get_widgetobj_widgetID(lval_src)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", lval_src);
  
  XtInstallAccelerators(widget_id, src);
  return (self);
}


/******************************************************************************
 * (send <destination> :INSTALL_ALL_ACCELERATORS <source>)
 * ==>  returns <destination>
 * 
 * This method installs the accelerators from widget <source> and all it's 
 * children onto widget <destination> by augmenting the destination translations
 * with the source accelerators.
 *
 * void XtInstallAllAccelerators(destination,source)
 *    Widget destination,source;
 ******************************************************************************/
LVAL Widget_Class_Method_INSTALL_ALL_ACCELERATORS()
{
  LVAL self, lval_src;
  Widget widget_id, src;

  self = xlga_widgetobj();
  lval_src = xlga_widgetobj();
  xllastarg();
  
  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  if (!(src = get_widgetobj_widgetID(lval_src)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", lval_src);
  
  XtInstallAllAccelerators(widget_id, src);
  return (self);
}


/******************************************************************************
 *
 ******************************************************************************/
Wtx_Init()
{
  s_ACTION_WIDGET = xlenter("ACTION_WIDGET");
  s_ACTION_XEVENT = xlenter("ACTION_XEVENT");
  s_LISP_ACTION_PROC = xlenter("LISP_ACTION_PROC");
}
