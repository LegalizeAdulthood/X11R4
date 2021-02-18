/* -*-C-*-
********************************************************************************
*
* File:         w_libXm.c
* RCS:          $Header: w_libXm.c,v 1.1 89/11/25 04:26:55 mayer Exp $
* Description:  Random XLISP Primitives and Methods for the Motif toolkit.
* Author:       Niels Mayer, HPLabs
* Created:      Fri Nov 24 00:36:30 1989
* Modified:     Sat Nov 25 04:26:42 1989 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: w_libXm.c,v 1.1 89/11/25 04:26:55 mayer Exp $";

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"


/*****************************************************************************
 * (send <widget> :UPDATE_DISPLAY)
 *
 * (see XmUpdateDisplay(3X))
 * This function is useful for refreshing winterp's
 * Xwindows displays while inside a callback. Since the X display won't get
 * refreshed until you hit the XtNextEvent/XtDispatcEvent loop you cannot
 * normally change the display inside a callback. Thus, this function is
 * expecally useful for forcing a status message to be seen before a long
 * computation occurs within a callback, timeout, or workproc.
 ****************************************************************************/
LVAL Widget_Class_Method_UPDATE_DISPLAY()
{
  LVAL self;
  Widget widget_id;
  self = xlga_widgetobj();
  xllastarg();
  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  XmUpdateDisplay(widget_id);
  return (self);
}


/******************************************************************************
 * (send <widget> :ADD_TAB_GROUP)
 * ==> returns <widget>
 *
 * This method adds <widget> to the list of tab groups associated with a 
 * particular widget hierarchy.
 *
 * void XmAddTabGroup(tabGroup)
 *      Widget tabGroup;
 ******************************************************************************/
LVAL Widget_Class_Method_ADD_TAB_GROUP()
{
  LVAL self;
  Widget widget_id;

  self = xlga_widgetobj();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  XmAddTabGroup(widget_id);
  return (self);
}


/******************************************************************************
 * (send <widget> :REMOVE_TAB_GROUP)
 * ==> returns <widget>
 *
 * This method removes <widget> from the list of tab groups associated with a 
 * particular widget hierarchy.
 *
 * void XmRemoveTabGroup (tabGroup)
 *      Widget tabGroup;
 ******************************************************************************/
LVAL Widget_Class_Method_REMOVE_TAB_GROUP()
{
  LVAL self;
  Widget widget_id;

  self = xlga_widgetobj();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  XmRemoveTabGroup(widget_id);
  return (self);
}

/******************************************************************************
 * (send <widget> :IS_PRIMITIVE)
 * ==> returns T if <widget> is a motif primitive widget, else NIL.
 *
 * #define XmIsPrimitive(w)	XtIsSubclass(w, xmPrimitiveWidgetClass)
 ******************************************************************************/
LVAL Widget_Class_Method_IS_PRIMITIVE()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;

  self = xlga_widgetobj();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  return (XmIsPrimitive(widget_id) ? true : NIL);
}


/******************************************************************************
 * (send <widget> :IS_GADGET)
 * ==> returns T if <widget> is a motif gadget, else NIL.
 *
 * #define XmIsGadget(w)	XtIsSubclass(w, xmGadgetClass)
 ******************************************************************************/
LVAL Widget_Class_Method_IS_GADGET()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;

  self = xlga_widgetobj();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  return (XmIsGadget(widget_id) ? true : NIL);
}


/******************************************************************************
 * (send <widget> :IS_MANAGER)
 * ==> returns T if <widget> is a motif manager widget, else NIL.
 *
 * #define XmIsManager(w)	XtIsSubclass(w, xmManagerWidgetClass)
 ******************************************************************************/
LVAL Widget_Class_Method_IS_MANAGER()
{
  extern LVAL true;
  LVAL self;
  Widget widget_id;

  self = xlga_widgetobj();
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);

  return (XmIsManager(widget_id) ? true : NIL);
}
