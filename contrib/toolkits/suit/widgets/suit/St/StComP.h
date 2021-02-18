/*
 *	Original Source: ./lib/Xaw/CommandP.h
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */

/*
* $XConsortium: CommandP.h,v 1.21 88/09/27 11:19:53 swick Exp $
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

#ifndef _StComP_h
#define _StComP_h

#include <St/StCom.h>
#include <X11/LabelP.h>

/***********************************************************************
 *
 * Com Widget Private Data
 *
 ***********************************************************************/

/************************************
 *
 *  Class structure
 *
 ***********************************/


   /* New fields for the Com widget class record */
typedef struct _StComClass 
  {
    int makes_compiler_happy;  /* not used */
  } StComClassPart;

   /* Full class record declaration */
typedef struct _StComClassRec {
    CoreClassPart	core_class;
    SimpleClassPart	simple_class;
    LabelClassPart	label_class;
    StComClassPart   com_class;
} StComClassRec;

extern StComClassRec comClassRec;

/***************************************
 *
 *  Instance (widget) structure 
 *
 **************************************/

    /* New fields for the Com widget record */
typedef struct {
    /* resources */
    XtCallbackList callbacks;

    /* private state */
    GC          normal_GC;
    GC          gray_GC;
    GC          normal_line_GC;
    GC          gray_line_GC;
    Drawable    normal_shape;
    Drawable    wnormal_shape;
    Boolean     switchon;
    Boolean     set;
    Boolean     first;
    Dimension   line_width;
    Pixel       line_pixel;
    Pixmap      line_pixmap;
} StComPart;


/*    XtEventsPtr eventTable;*/


   /* Full widget declaration */
typedef struct _StComRec {
    CorePart         core;
    SimplePart	     simple;
    LabelPart	     label;
    StComPart     com;
} StComRec;

#endif _StComP_h


