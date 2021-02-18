/*
* $Header: Menu.h,v 1.2 88/06/01 02:47:17 moraes Exp $
*/

/*
 * Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.
 * 
 *                         All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior permission.  
 * 
 * 
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/* Menu.h
 *   
 * Author:       Robert P. Shen
 *               Malcolm Forbes
 *               Digital Equipment Corporation
 *               ESG Systems Engineering
 *
 * Date:         February 1, 1988
 */
#ifndef _XtMenu_h
#define _XtMenu_h

/***********************************************************************
 *
 * Menu Widget (subclass of CompositeClass)
 *
 ***********************************************************************/

/* New fields */
#define XtNxMargin		"xMargin"
#define XtNyMargin		"yMargin"
#define XtNmenuForeground       "menuForeground"
#define XtNcheckForeground      "checkForeground"
#define XtNmenuItemsList	"menuItemsList"

	/*--------------------------------------------------------------*
	 *  the following bit masks only apply when setting up		*
	 *  a Menu Items List.  (This should be in a sub-widget ;-)	*
	 *--------------------------------------------------------------*/

#define XtitemUnselectable	0x04	/* 0=selectable, 1=not		*/
#define XtitemCheckable		0x02	/* 0=uncheckable, 1=checkable	*/
#define XtitemIsChecked		0x01	/* 0=unchecked, 1=checked	*/

/* Class record constants */

extern WidgetClass menuWidgetClass;

typedef struct _MenuClassRec *MenuWidgetClass;
typedef struct _MenuRec      *MenuWidget;

typedef struct MenuItemsRec {
	String		text;
	XtCallbackProc	proc;
	caddr_t		closure;
	Boolean		flags;
} *MenuItems, MenuItemsList;

#endif _XtMenu_h
/* DON'T ADD STUFF AFTER THIS #endif */
