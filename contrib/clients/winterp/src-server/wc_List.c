/* -*-C-*-
********************************************************************************
*
* File:         wc_List.c
* RCS:          $Header: wc_List.c,v 1.1 89/11/25 04:45:14 mayer Exp $
* Description:  XM_LIST_WIDGET_CLASS
* Author:       Niels Mayer, HPLabs
* Created:      Sat Oct 28 03:28:28 1989
* Modified:     Sat Nov 25 04:44:52 1989 (Niels Mayer) mayer@hplnpm
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
static char rcs_identity[] = "@(#)$Header: wc_List.c,v 1.1 89/11/25 04:45:14 mayer Exp $";

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/List.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_funtab.h"


/******************************************************************************/
static int Get_ListPosition_Argument()
{
  extern long max_int;		/* from w_resources.c */
  LVAL lval_position;
  long position;

  lval_position = xlgafixnum();
  position = (long) getfixnum(lval_position);
  if (position < 0L)
    xlerror("List-Position argument must be a fixnum >= 0.", lval_position);
  if (position > max_int)
    xlerror("List-Position argument too large.", lval_position);
  
  return((int) position);
}


/*****************************************************************************
 * (send XM_LIST_WIDGET_CLASS :new
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
 *     (send XM_LIST_WIDGET_CLASS :new ...)
 *     --> XmCreateList();
 *     (send XM_LIST_WIDGET_CLASS :new :scrolled ...)
 *     --> XmCreateScrolledList();
 *          Note: extra convenience fn XmCreateScrolledList() puts the
 *	  list widget inside a scrolled window but returns the list widget.
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_ISNEW()
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
    scrolled_p = FALSE;		/* by default, we don't want a scroled list */

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
      widget_id = XmCreateScrolledList(parent_widget_id, name, xt_arglist, xt_numargs);
    else
      widget_id = XmCreateList(parent_widget_id, name, xt_arglist, xt_numargs);
    Wres_Free_C_Arglist_Data();
  }
  else 
    if (scrolled_p)
      widget_id = XmCreateScrolledList(parent_widget_id, name, ARGLIST());
    else
      widget_id = XmCreateList(parent_widget_id, name, ARGLIST());

  Wcls_Initialize_WIDGETOBJ(self, widget_id);

  if (managed_p)
    XtManageChild(widget_id);

#ifdef DEBUG_WINTERP_1
  Wcls_Print_WidgetObj_Info(self);
#endif
  return (self);
}


/*****************************************************************************
 * (send <listwidget> :ADD_ITEM <item> <position>)
 *	==> returns the new item, as an XmString.
 * This method adds <item> to <listwidget> at the given position.
 * <item> is a string or XmString. If <item> is given as a string, it
 *	is converted to an XmString and that XmString is returned.
 * <position> is an positive fixnum. Specifying 0 will add the item to the end
 *	of the list, 1 makes it the first item, 2 the second, etc.
 *
 * void XmListAddItem(w, item, pos)
 * Widget w;
 * XmString item;
 * int pos;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_ADD_ITEM()
{
  LVAL self, lval_item;
  int position;
  Widget widget_id;
  XmString str;
  extern XmString Get_String_or_XmString_Arg_Returning_XmString(); /* w_XmString.c */

  self = xlga_widgetobj();	/* get <listwidget> */
  str = Get_String_or_XmString_Arg_Returning_XmString(&lval_item); /* get <item> */
  position = Get_ListPosition_Argument(); /* get <position> */
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  XmListAddItem(widget_id, str, position);

  /* note: XmListAddItem() creates a copy of the XmString <item>, allowing us to
     free str whenever we want. If <item> was passed in as an XmString, the
     XmString will be freed via garbage collection whenever <item> is no longer
     referenced. If <item> was passed in as a normal string, then a new
     XmString object is created in this method -- it will be freed when it
     gets garbage collected.
     */
  return (lval_item);		/* return the XmString item. */
}


/*****************************************************************************
 * (send <listwidget> :ADD_ITEM_UNSELECTED <item> <position>)
 *	==> returns <item> as an XmString.
 * This method adds <item> to <listwidget> at the given position and ensures
 * that <item> is not selected.
 * <item> is a string or XmString. If <item> is given as a string, it
 *	is converted to an XmString and that XmString is returned by the method.
 * <position> is an positive fixnum. Specifying 0 will add the item to the end
 *	of the list, 1 makes it the first item, 2 the second, etc.
 *
 * void XmListAddItemUnselected(w, item, pos)
 *     Widget w;
 *     XmString item;
 *     int	     pos;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_ADD_ITEM_UNSELECTED()
{
  LVAL self, lval_item;
  int position;
  Widget widget_id;
  XmString str;
  extern XmString Get_String_or_XmString_Arg_Returning_XmString(); /* w_XmString.c */

  self = xlga_widgetobj();	/* get <listwidget> */
  str = Get_String_or_XmString_Arg_Returning_XmString(&lval_item); /* get <item> */
  position = Get_ListPosition_Argument(); /* get <position> */
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  XmListAddItemUnselected(widget_id, str, position);

  /* note: XmListAddItemUnselected() creates a copy of the XmString <item>,
     allowing us to free str whenever we want. If <item> was passed in as an
     XmString, the XmString will be freed via garbage collection whenever <item>
     is no longer referenced. If <item> was passed in as a normal string, then a
     new XmString object is created in this method -- it will be freed when it
     gets garbage collected.  */
  return (lval_item);		/* return the XmString item. */
}


/*****************************************************************************
 * (send <listwidget> :DELETE_ITEM <item>)
 *	==> returns <item> as an XmString
 * This method deletes the specified <item> from <listwidget>, giving
 * an XtWarning if <item> doesn't exist.
 * <item> is a string or XmString. If <item> is given as a string, it
 *	is converted to an XmString and that XmString is returned by the method.
 *
 * void XmListDeleteItem(w, item)
 *     Widget w;
 *     XmString item;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_DELETE_ITEM()
{
  LVAL self, lval_item;
  Widget widget_id;
  XmString str;
  extern XmString Get_String_or_XmString_Arg_Returning_XmString(); /* w_XmString.c */

  self = xlga_widgetobj();	/* get <listwidget> */
  str = Get_String_or_XmString_Arg_Returning_XmString(&lval_item); /* get <item> */
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  XmListDeleteItem(widget_id, str);

  /* note: XmListDeleteItem() creates a copy of the XmString <item>,
     allowing us to free str whenever we want. If <item> was passed in as an
     XmString, the XmString will be freed via garbage collection whenever <item>
     is no longer referenced. If <item> was passed in as a normal string, then a
     new XmString object is created in this method -- it will be freed when it
     gets garbage collected.  */
  return (lval_item);		/* return the XmString item. */
}


/*****************************************************************************
 * (send <listwidget> :DELETE_POS <position>)
 *	==> returns <listwidget>.
 * This method deletes the item at <position> from <listwidget>. A position
 * of zero deletes the last item on the list. Gives an XtWarning if the 
 * item doesn't exist.
 * <position> is an positive fixnum. 
 *
 * void XmListDeletePos(w, pos)
 *     Widget w;
 *     int	pos;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_DELETE_POS()
{
  LVAL self;
  int position;
  Widget widget_id;

  self = xlga_widgetobj();	/* get <listwidget> */
  position = Get_ListPosition_Argument(); /* get <position> */
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  XmListDeletePos(widget_id, position);

  return (self);
}


/*****************************************************************************
 * (send <listwidget> :SELECT_ITEM <item> [<notify_p>])
 *	==> returns <item> as an XmString
 * This method adds <item> to the selected items list and highlights it.
 * <item> is a string or XmString. If <item> is given as a string, it
 *	is converted to an XmString and that XmString is returned by the method.
 * <notify_p> is an optional boolean parameter. If non-NIL, it will issue a
 *	callback indicating an item has been selected.
 *
 * void XmListSelectItem(w, item, notify)
 *     Widget w;
 *     XmString item;
 *     Boolean notify;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_SELECT_ITEM()
{
  LVAL self, lval_item;
  Widget widget_id;
  XmString str;
  extern XmString Get_String_or_XmString_Arg_Returning_XmString(); /* w_XmString.c */
  Boolean notify_p;

  self = xlga_widgetobj();	/* get <listwidget> */
  str = Get_String_or_XmString_Arg_Returning_XmString(&lval_item); /* get <item> */
  if (moreargs())		/* get optional <notify_p> */
    notify_p = nextarg() ? TRUE : FALSE;
  else
    notify_p = FALSE;
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  XmListSelectItem(widget_id, str, notify_p);

  /* note: XmListSelectItem() creates a copy of the XmString <item>,
     allowing us to free str whenever we want. If <item> was passed in as an
     XmString, the XmString will be freed via garbage collection whenever <item>
     is no longer referenced. If <item> was passed in as a normal string, then a
     new XmString object is created in this method -- it will be freed when it
     gets garbage collected.  */
  return (lval_item);		/* return the XmString item. */
}


/*****************************************************************************
 * (send <listwidget> :SELECT_POS <position> [<notify_p>])
 *	==> returns <listwidget>.
 * This method adds the item corresponding to <position> to the selected
 * items list and highlights it.
 * <position> is an positive fixnum. 
 * <notify_p> is an optional boolean parameter. If non-NIL, it will issue a
 *	callback indicating an item has been selected.
 *
 * void XmListSelectPos(w, pos, notify)
 *     Widget w;
 *     int	pos;
 *     Boolean notify;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_SELECT_POS()
{
  LVAL self;
  int position;
  Widget widget_id;
  Boolean notify_p;

  self = xlga_widgetobj();	/* get <listwidget> */
  position = Get_ListPosition_Argument(); /* get <position> */
  if (moreargs())		/* get optional <notify_p> */
    notify_p = nextarg() ? TRUE : FALSE;
  else
    notify_p = FALSE;
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  XmListSelectPos(widget_id, position, notify_p);

  return (self);
}


/*****************************************************************************
 * (send <listwidget> :DESELECT_ITEM <item>)
 *	==> returns <item> as an XmString
 * This method removes the specified <item> from the selected list and
 * unhighlights it.
 * <item> is a string or XmString. If <item> is given as a string, it
 *	is converted to an XmString and that XmString is returned by the method.
 *
 * void XmListDeselectItem(w, item)
 *     Widget w;
 *     XmString item;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_DESELECT_ITEM()
{
  LVAL self, lval_item;
  Widget widget_id;
  XmString str;
  extern XmString Get_String_or_XmString_Arg_Returning_XmString(); /* w_XmString.c */

  self = xlga_widgetobj();	/* get <listwidget> */
  str = Get_String_or_XmString_Arg_Returning_XmString(&lval_item); /* get <item> */
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  XmListDeselectItem(widget_id, str);

  /* note: XmListDeselectItem() creates a copy of the XmString <item>,
     allowing us to free str whenever we want. If <item> was passed in as an
     XmString, the XmString will be freed via garbage collection whenever <item>
     is no longer referenced. If <item> was passed in as a normal string, then a
     new XmString object is created in this method -- it will be freed when it
     gets garbage collected.  */
  return (lval_item);		/* return the XmString item. */
}


/*****************************************************************************
 * (send <listwidget> :DESELECT_POS <position>)
 *	==> returns <listwidget>.
 * This method removes the item associated with <position> from the
 * selected list and unhighlights it.
 * <position> is an positive fixnum. 
 *
 * void XmListDeselectPos(w, pos)
 *     Widget w;
 *     int	pos;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_DESELECT_POS()
{
  LVAL self;
  int position;
  Widget widget_id;

  self = xlga_widgetobj();	/* get <listwidget> */
  position = Get_ListPosition_Argument(); /* get <position> */
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  XmListDeselectPos(widget_id, position);

  return (self);
}


/*****************************************************************************
 * (send <listwidget> :DESELECT_ALL_ITEMS)
 *	==> returns <listwidget>.
 * This method unhighlighs all selected items and removes all items from the
 * selected items list.
 *
 * void XmListDeselectAllItems(w)
 *     Widget w;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_DESELECT_ALL_ITEMS()
{
  LVAL self;
  Widget widget_id;
  self = xlga_widgetobj();	/* get <listwidget> */
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  XmListDeselectAllItems(widget_id);
  return (self);
}


/*****************************************************************************
 * (send <listwidget> :SET_POS <position>)
 *	==> returns <listwidget>.
 * This method makes the item associated with <position> be the first
 * visible element of the list.
 * <position> is an positive fixnum. 
 *
 * void XmListSetPos(w, pos)
 *     Widget w;
 *     int    pos;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_SET_POS()
{
  LVAL self;
  int position;
  Widget widget_id;

  self = xlga_widgetobj();	/* get <listwidget> */
  position = Get_ListPosition_Argument(); /* get <position> */
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  XmListSetPos(widget_id, position);

  return (self);
}


/*****************************************************************************
 * (send <listwidget> :SET_BOTTOM_POS <position>)
 *	==> returns <listwidget>.
 * This method makes the item associated with <position> be the last visible
 * position in the list.
 * <position> is an positive fixnum. 
 *
 * void XmListSetBottomPos(w, pos)
 *     Widget w;
 *     int    pos;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_SET_BOTTOM_POS()
{
  LVAL self;
  int position;
  Widget widget_id;

  self = xlga_widgetobj();	/* get <listwidget> */
  position = Get_ListPosition_Argument(); /* get <position> */
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  XmListSetBottomPos(widget_id, position);

  return (self);
}


/*****************************************************************************
 * (send <listwidget> :SET_ITEM <item>)
 *	==> returns <item> as an XmString
 * This method makes <item> be the first visible item in the list.
 * <item> is a string or XmString. If <item> is given as a string, it
 *	is converted to an XmString and that XmString is returned by the method.
 *
 * void XmListSetItem(w, item)
 *     Widget w;
 *     XmString item;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_SET_ITEM()
{
  LVAL self, lval_item;
  Widget widget_id;
  XmString str;
  extern XmString Get_String_or_XmString_Arg_Returning_XmString(); /* w_XmString.c */

  self = xlga_widgetobj();	/* get <listwidget> */
  str = Get_String_or_XmString_Arg_Returning_XmString(&lval_item); /* get <item> */
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  XmListSetItem(widget_id, str);

  /* note: XmListSetItem() creates a copy of the XmString <item>,
     allowing us to free str whenever we want. If <item> was passed in as an
     XmString, the XmString will be freed via garbage collection whenever <item>
     is no longer referenced. If <item> was passed in as a normal string, then a
     new XmString object is created in this method -- it will be freed when it
     gets garbage collected.  */
  return (lval_item);		/* return the XmString item. */
}


/*****************************************************************************
 * (send <listwidget> :SET_BOTTOM_ITEM <item>)
 *	==> returns <item> as an XmString
 * This method makes <item> be the last visible position in the list.
 * <item> is a string or XmString. If <item> is given as a string, it
 *	is converted to an XmString and that XmString is returned by the method.
 *
 * void XmListSetBottomItem(w, item)
 *     Widget w;
 *     XmString 	  item;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_SET_BOTTOM_ITEM()
{
  LVAL self, lval_item;
  Widget widget_id;
  XmString str;
  extern XmString Get_String_or_XmString_Arg_Returning_XmString(); /* w_XmString.c */

  self = xlga_widgetobj();	/* get <listwidget> */
  str = Get_String_or_XmString_Arg_Returning_XmString(&lval_item); /* get <item> */
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  XmListSetBottomItem(widget_id, str);

  /* note: XmListSetBottomItem() creates a copy of the XmString <item>,
     allowing us to free str whenever we want. If <item> was passed in as an
     XmString, the XmString will be freed via garbage collection whenever <item>
     is no longer referenced. If <item> was passed in as a normal string, then a
     new XmString object is created in this method -- it will be freed when it
     gets garbage collected.  */
  return (lval_item);		/* return the XmString item. */
}


/*****************************************************************************
 * (send <listwidget> :ITEM_EXISTS <item>)
 *	==> returns T if the item is in the specified list widget, else NIL.
 * <item> is a string or XmString. If <item> is given as a string, it
 *	is converted to an XmString.
 *
 * Boolean XmListItemExists(w, item)
 *     Widget w;
 *     XmString item;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_ITEM_EXISTS()
{
  extern LVAL true;
  Boolean result;
  LVAL self, lval_item;
  Widget widget_id;
  XmString str;

  self = xlga_widgetobj();	/* get <listwidget> */
  if (moreargs() && stringp(*xlargv)) {	/* get <item> */
    str = XmStringCreate((char*) getstring(nextarg()), XmSTRING_DEFAULT_CHARSET);
    lval_item = NIL;
  }
  else if (moreargs() && xmstring_p(*xlargv)) {
    lval_item = nextarg();
    str = get_xmstring(lval_item);
  }
  else
    xlerror("Bad List-Item type: expected an XmString or a string.", *xlargv);
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  result = XmListItemExists(widget_id, str);

  if (lval_item == NIL)		/* free the XmString only if we created it here ... note that we can't rely on gc to free the XmStrings since we can't return the XmString from this method */
    XmStringFree(str);

  return (result ? true : NIL);
}


/*****************************************************************************
 * (send <listwidget> :SET_HORIZ_POS <position>)
 *	==> returns <listwidget>.
 * This method scrolls the list horizontally such that the scrollbar reflects
 * <position> in the list.
 * <position> is an positive fixnum. 
 *
 * void XmListSetHorizPos(w, position)
 * Widget w;
 * int    position;
 ****************************************************************************/
LVAL Xm_List_Widget_Class_Method_SET_HORIZ_POS()
{
  LVAL self;
  int position;
  Widget widget_id;

  self = xlga_widgetobj();	/* get <listwidget> */
  position = Get_ListPosition_Argument(); /* get <position> */
  xllastarg();

  if (!(widget_id = get_widgetobj_widgetID(self)))
    xlerror("widget object has been :destroy'd or hasn't been initialized by :isnew.", self);
  
  XmListSetHorizPos(widget_id, position);

  return (self);
}


/******************************************************************************
 * Same as WIDGET_CLASS's :add_callback method except that this understands
 * how to get values from the XmListCallbackStruct.
 * Specifying one or more of the following symbols in the callback bindings 
 * list will bind that symbol's value in the lexical environment of the callback:
 * CALLBACK_WIDGET
 * CALLBACK_REASON
 * CALLBACK_XEVENT
 * CALLBACK_ITEM
 * CALLBACK_ITEM_LENGTH
 * CALLBACK_ITEM_POSITION
 * CALLBACK_SELECTED_ITEMS
 * CALLBACK_SELECTED_ITEM_COUNT
 * CALLBACK_SELECTION_TYPE
 ******************************************************************************/
LVAL Xm_List_Widget_Class_Method_ADD_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */
  extern void Wcb_XmListCallbackStruct_Callbackproc(); /* w_callbacks.c */
  return(Wcb_Meta_Method_Add_Callback(Wcb_XmListCallbackStruct_Callbackproc, FALSE));
}


/******************************************************************************
 * Same as WIDGET_CLASS's :set_callback method except that this understands
 * how to get values from the XmListCallbackStruct.
 * Specifying one or more of the following symbols in the callback bindings 
 * list will bind that symbol's value in the lexical environment of the callback:
 * CALLBACK_WIDGET
 * CALLBACK_REASON
 * CALLBACK_XEVENT
 * CALLBACK_ITEM
 * CALLBACK_ITEM_LENGTH
 * CALLBACK_ITEM_POSITION
 * CALLBACK_SELECTED_ITEMS
 * CALLBACK_SELECTED_ITEM_COUNT
 * CALLBACK_SELECTION_TYPE
 ******************************************************************************/
LVAL Xm_List_Widget_Class_Method_SET_CALLBACK()
{
  extern LVAL Wcb_Meta_Method_Add_Callback(); /* w_callbacks.c */
  extern void Wcb_XmListCallbackStruct_Callbackproc(); /* w_callbacks.c */
  return (Wcb_Meta_Method_Add_Callback(Wcb_XmListCallbackStruct_Callbackproc, TRUE));
}


/******************************************************************************
 *
 ******************************************************************************/
Wc_List_Init()
{
  LVAL o_XM_LIST_WIDGET_CLASS;
  extern LVAL Wcls_Create_Subclass_Of_WIDGET_CLASS(); /* w_classes.c */
  extern      xladdmsg();	/* from xlobj.c */

  o_XM_LIST_WIDGET_CLASS =
    Wcls_Create_Subclass_Of_WIDGET_CLASS("XM_LIST_WIDGET_CLASS",
					 xmListWidgetClass);

  /* a special :isnew method on this class allows for the creation of this
     widget inside a scrolled window if the submessage keyword :scrolled 
     is given ... */
  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":ISNEW", 
	   FTAB_Xm_List_Widget_Class_Method_ISNEW);
  
  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":ADD_ITEM", 
	   FTAB_Xm_List_Widget_Class_Method_ADD_ITEM);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":ADD_ITEM_UNSELECTED", 
	   FTAB_Xm_List_Widget_Class_Method_ADD_ITEM_UNSELECTED);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":DELETE_ITEM", 
	   FTAB_Xm_List_Widget_Class_Method_DELETE_ITEM);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":DELETE_POS", 
	   FTAB_Xm_List_Widget_Class_Method_DELETE_POS);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":SELECT_ITEM", 
	   FTAB_Xm_List_Widget_Class_Method_SELECT_ITEM);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":SELECT_POS", 
	   FTAB_Xm_List_Widget_Class_Method_SELECT_POS);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":DESELECT_ITEM", 
	   FTAB_Xm_List_Widget_Class_Method_DESELECT_ITEM);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":DESELECT_POS", 
	   FTAB_Xm_List_Widget_Class_Method_DESELECT_POS);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":DESELECT_ALL_ITEMS", 
	   FTAB_Xm_List_Widget_Class_Method_DESELECT_ALL_ITEMS);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":SET_POS", 
	   FTAB_Xm_List_Widget_Class_Method_SET_POS);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":SET_BOTTOM_POS", 
	   FTAB_Xm_List_Widget_Class_Method_SET_BOTTOM_POS);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":SET_ITEM", 
	   FTAB_Xm_List_Widget_Class_Method_SET_ITEM);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":SET_BOTTOM_ITEM", 
	   FTAB_Xm_List_Widget_Class_Method_SET_BOTTOM_ITEM);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":ITEM_EXISTS", 
	   FTAB_Xm_List_Widget_Class_Method_ITEM_EXISTS);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":SET_HORIZ_POS", 
	   FTAB_Xm_List_Widget_Class_Method_SET_HORIZ_POS);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":ADD_CALLBACK", 
	   FTAB_Xm_List_Widget_Class_Method_ADD_CALLBACK);

  xladdmsg(o_XM_LIST_WIDGET_CLASS, ":SET_CALLBACK", 
	   FTAB_Xm_List_Widget_Class_Method_SET_CALLBACK);
}
