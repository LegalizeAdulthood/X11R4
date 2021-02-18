/*
 *      Original Source: ./lib/Xaw/SimpleP.h
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */
/*
* $XConsortium: SimpleP.h,v 1.7 88/09/06 16:42:24 jim Exp $
*/

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/************************************************************
Copyright 1989 by Information-Technology Promotion Agency Japan (IPA),
Mitsubishi Research Institute, Inc. (MRI), and Fuji Xerox Co., Ltd. (FX)

                        All Rights Reserved

Permission to use, copy, modify and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IPA, MRI or FX not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IPA, MRI or FX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL IPA, MRI or FX BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

********************************************************/

#ifndef _StPrimitiveP_h
#define _StPrimitiveP_h

#include <St/StPrimiti.h>
#include <X11/CoreP.h>

typedef struct _StPrimitiveClass {
    Boolean	(*change_sensitive)();
    Boolean	(*parent_expose)();
} StPrimitiveClassPart;

#define XtInheritChangeSensitive ((Boolean (*)())_XtInherit)
#define XtInheritParentExpose ((Boolean (*)())_XtInherit)

typedef struct _StPrimitiveClassRec {
    CoreClassPart		core_class;
    StPrimitiveClassPart	stPrimitive_class;
} StPrimitiveClassRec;

extern StPrimitiveClassRec stPrimitiveClassRec;

typedef struct {
    /* resources */
    Pixel		foreground;
    XFontStruct		*font;
    Cursor		cursor;
    StContents		contents;
    XtCallbackList	callbacks;
    int			callback_interval;
    Dimension		max_height;
    Dimension		max_width;
    Dimension		min_height;
    Dimension		min_width;
    Pixmap		insensitive_pixmap;
    Boolean		transparence;

    /* private state */
    GC		normal_GC;
    GC		inverse_GC;
    GC		normal_gray_GC;
    GC		inverse_gray_GC;
    Pixmap	insensitive_border_pixmap;
} StPrimitivePart;

typedef struct _StPrimitiveRec {
    CorePart		core;
    StPrimitivePart	stPrimitive;
} StPrimitiveRec;

#endif _StPrimitiveP_h
