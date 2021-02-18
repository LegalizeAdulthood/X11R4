/*
* $Header: CheckItem.h,v 1.2 88/05/31 21:07:42 moraes Exp $
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
 * CheckItem.h - CheckItem button widget
 *
 * Author:      Robert P. Shen
 *              Malcolm Forbes
 *              Digital Equipment Corporation
 *              ESG Systems Engineering
 *
 * Date:        February 1, 1988
 */
#ifndef _XtCheckItem_h
#define _XtCheckItem_h

/***********************************************************************
 *
 * CheckItem Widget
 *
 ***********************************************************************/

/*#define XtNcallback		"callback"*/
#define XtNhighlightThickness   "hightlightThickness"
/*#define XtNtranslations		"translations"*/

#ifndef _XtCheckState
#define _XtCheckState
#define XtNcheckState "checkState"
#define XtCCheckState "CheckState"
#define XtRCheckState "CheckState"
typedef enum {
    XtitemChecked,
    XtitemUnchecked
} XtCheckState;
#endif  _XtCheckState

extern WidgetClass     checkitemWidgetClass;

typedef struct _CheckItemClassRec   *CheckItemWidgetClass;
typedef struct _CheckItemRec        *CheckItemWidget;


#endif _XtCheckItem_h
/* DON'T ADD STUFF AFTER THIS */
