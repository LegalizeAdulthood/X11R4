/*
* $Header: MenuP.h,v 1.2 88/05/31 21:07:52 moraes Exp $
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
 * MenuPrivate.h - Private definitions for Menu widget
 * 
 * Author:       Robert P. Shen
 *               Malcolm Forbes
 *               Digital Equipment Corporation
 *               ESG Systems Engineering
 *
 * Date:         February 1, 1988
 */

#ifndef _MenuP_h
#define _MenuP_h

#include "Menu.h"
/***********************************************************************
 *
 * Menu Widget Private Data
 *
 ***********************************************************************/

/* New fields for the Menu widget class record */
typedef struct {
     int mumble;   /* No new procedures */
} MenuClassPart;

/* Full class record declaration */
typedef struct _MenuClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    MenuClassPart	menu_class;
} MenuClassRec;

extern MenuClassRec menuClassRec;

/* New fields for the Menu widget record */
typedef struct {
    Dimension   x_margin, y_margin;
    XtOrientation orient;
    MenuItems	items;
    Pixel       menu_foreground_pixel, check_foreground_pixel;
} MenuPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _MenuRec {
    CorePart		core;
    CompositePart	composite;
    MenuPart		menu;
} MenuRec;
#endif _MenuP_h
/* DON'T ADD STUFF AFTER THIS #endif */














