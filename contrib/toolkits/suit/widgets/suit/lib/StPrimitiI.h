/*
 *      Original Source: ./lib/Xaw/CommandI.h
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */
/*
* $XConsortium: CommandI.h,v 1.15 88/10/18 12:36:05 swick Exp $
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

/*
 *
 *  A few definitions to make StPrimitive.c easier to read.
 *   
 */

  /* Yes, this is gross, but the code will be easier to read.
     Trust me.  */
#define PriWparent			bw->core.parent
#define PriWx				bw->core.x
#define PriWy				bw->core.y
#define PriWdepth			bw->core.depth
#define PriWwidth			bw->core.width
#define PriWheight			bw->core.height
#define PriWborderWidth			bw->core.border_width
#define PriWborderColor			bw->core.border_pixel
#define PriWborderPixmap		bw->core.border_pixmap
#define PriWbackground			bw->core.background_pixel
#define PriWbackgroundPixmap		bw->core.background_pixmap
#define PriWsensitive			bw->core.sensitive 
#define PriWancestorSensitive		bw->core.ancestor_sensitive

#define PriWforeground			bw->stPrimitive.foreground
#define PriWfont			bw->stPrimitive.font
#define PriWcursor			bw->stPrimitive.cursor
#define PriWcontents			bw->stPrimitive.contents
#define PriWcallbackList		bw->stPrimitive.callback_list
#define PriWcallback			bw->stPrimitive.callbacks
#define PriWclosure			bw->stPrimitive.closure
#define PriWcallbackInterval		bw->stPrimitive.callback_interval
#define PriWmaxHeight			bw->stPrimitive.max_height
#define PriWmaxWidth			bw->stPrimitive.max_width
#define PriWminHeight			bw->stPrimitive.min_height
#define PriWminWidth			bw->stPrimitive.min_width
#define PriWinsensitivePixmap		bw->stPrimitive.insensitive_pixmap
#define PriWtransparence		bw->stPrimitive.transparence
#define PriWnormalGC			bw->stPrimitive.normal_GC
#define PriWinverseGC			bw->stPrimitive.inverse_GC
#define PriWnormalGrayGC		bw->stPrimitive.normal_gray_GC
#define PriWinverseGrayGC		bw->stPrimitive.inverse_gray_GC
#define PriWinsensitiveBorderPixmap	bw->stPrimitive.insensitive_border_pixmap
