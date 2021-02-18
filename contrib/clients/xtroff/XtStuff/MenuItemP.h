/*
* $Header: MenuItemP.h,v 1.2 88/05/31 21:07:50 moraes Exp $
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
/* 
 * MenuItemP.h - Private definitions for MenuItem widget
 *
 * Author:      Robert P. Shen
 *              Malcolm Forbes
 *              Digital Equipment Corporation
 *              ESG Systems Engineering
 *
 * Date:        February 1, 1988
 */

#ifndef _XtMenuItemPrivate_h
#define _XtMenuItemPrivate_h

/***********************************************************************
 *
 * MenuItem Widget Private Data
 *
 ***********************************************************************/

/************************************
 *
 *  Class structure
 *
 ***********************************/


   /* New fields for the MenuItem widget class record */
typedef struct _MenuItemClass 
  {
    int makes_compiler_happy;  /* not used */
  } MenuItemClassPart;

   /* Full class record declaration */
typedef struct _MenuItemClassRec {
    CoreClassPart	core_class;
    LabelClassPart	label_class;
    MenuItemClassPart     menuitem_class;
} MenuItemClassRec;

extern MenuItemClassRec menuitemClassRec;

/***************************************
 *
 *  Instance (widget) structure 
 *
 **************************************/

    /* New fields for the MenuItem widget record */
typedef struct {
    Pixmap      gray_pixmap;
    GC          highlight_GC;
    GC          erase_highlight_GC;
    GC          inverse_GC;
    GC          inverse_text_GC;
    Dimension   highlight_thickness;
    XtCallbackList callbacks;
    Boolean     set;
    Boolean     highlighted;
    Boolean     display_set;
    Boolean     display_highlighted;
} MenuItemPart;


/*    XtEventsPtr eventTable;*/


   /* Full widget declaration */
typedef struct _MenuItemRec {
    CorePart         core;
    SimplePart	     simple;
    LabelPart	     label;
    MenuItemPart      menuitem;
} MenuItemRec;

#endif _XtMenuItemPrivate_h
/* DON'T ADD STUFF AFTER THIS #endif */

