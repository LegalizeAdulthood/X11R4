/*
 * Copyright 1989 Waseda University.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Waseda University not be used
 * in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.
 * Waseda University makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * WASEDA DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL WASEDA
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:	Nobuhiko Nakajima
 *		Waseda University
 *
 *	61-505, Sci&Eng department, Waseda University
 *	3-1-9 shinjyuku-ku ookubo, Tokyo, Japan	169
 *	(03) 232-0551
 *
 *	Junet	hiko@ohara.waseda.ac.jp
 */
/*
 * ImageP.h - Private definitions for Image widget
 *
 */

#ifndef _ImageP_h
#define _ImageP_h

/***********************************************************************
 *
 * Image Widget Private Data
 *
 ***********************************************************************/

#include <X11/CoreP.h>
#include <X11/SimpleP.h>

#include "Image.h"

/* New fields for the Label widget class record */
typedef struct _ImageClassPart{
  int empty;      /* No new fields needed */
} ImageClassPart;

/* Full class record declaration */
typedef struct _ImageClassRec {
    CoreClassPart	core_class;
    SimpleClassPart	simple_class;
    ImageClassPart	image_class;
} ImageClassRec;

extern ImageClassRec imageClassRec;

/* New fields for the Label widget record */
typedef struct _ImagePart{
  /* resources */
    Pixel	foreground;
    XFontStruct	*font;
    Dimension	arrign;
    Dimension   internal_width;
    Dimension   internal_height;
    Pixmap	pixmap;
    Dimension	image_x;
    Dimension	image_y;
    Dimension	image_cx;
    Dimension	image_cy;
    Dimension	image_width;
    Dimension	image_height;
    Dimension	image_len;
    Dimension	cwidth;
    Dimension	cheight;

    /* private state */
    GC		normal_GC;
    GC		gray_GC;
    Dimension	font_width;
    Dimension	font_height;
    ImageString	*strings;

    Pixmap	backmap;
    Dimension	depth;
    GC		erase_GC;
} ImagePart;

/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _ImageRec {
    CorePart	core;
    SimplePart	simple;
    ImagePart	image;
} ImageRec;

#endif  _ImageP_h
