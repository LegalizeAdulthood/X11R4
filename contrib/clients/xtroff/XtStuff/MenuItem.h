/*
* $Header: MenuItem.h,v 1.6 87/12/01 15:36:03 swick Exp $
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
 * MenuItem.h - MenuItem button widget
 *
 * Author:      Robert P. Shen
 *              Malcolm Forbes
 *              Digital Equipment Corporation
 *              ESG Systems Engineering
 *
 * Date:        February 1, 1988
 */
#ifndef _XtMenuItem_h
#define _XtMenuItem_h

/***********************************************************************
 *
 * MenuItem Widget
 *
 ***********************************************************************/

/*#define XtNcallback		"callback"*/
#define XtNhighlightThickness   "hightlightThickness"
/*#define XtNtranslations		"translations"*/

extern WidgetClass     menuitemWidgetClass;

typedef struct _MenuItemClassRec   *MenuItemWidgetClass;
typedef struct _MenuItemRec        *MenuItemWidget;


#endif _XtMenuItem_h
/* DON'T ADD STUFF AFTER THIS */
