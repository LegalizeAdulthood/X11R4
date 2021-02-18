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

#ifndef _StwP_h
#define _StwP_h

#include <St/Stw.h>

#define UnspecifiedPixmap	2
#define min(x, y)		((x) < (y) ? (x) : (y))
#define max(x, y)		((x) > (y) ? (x) : (y))
#define restrict(x, min, max)	((x) > (max)    \
					? (max) \
					: ((x) < (min) ? (min) : (x)))
#define isSensitive(w)		((w)->core.sensitive \
					&& (w)->core.ancestor_sensitive)
#define StStrlen(s)             ((s) ? strlen(s) : 0)
#define StStrcopy(s)            (strcpy(XtMalloc(strlen((s)) + 1), (s)))
#define StStrfree(s)            (XtFree((String *)(s)))

/********************************
 *
 * rectangle utility
 *
 ********************************/

extern Boolean StuContainsRectangle();
	/* XRectangle  rect1 */
	/* XRectangle  rect2   : rect1 contain rect2 ? */
extern XRectangle StuMergeRectangles();
	/* XRectangle  rect1 */
	/* XRectangle  rect2   : merge rect1 with rect2 */
extern Boolean StuEquivalentRectangles();
	/* XRectangle  rect1 */
	/* XRectangle  rect2   : rect1 == rect2 ? */
extern Boolean StuContainsPoint();
	/* XRectangle  rect */
	/* StPoint     point   : rect contain point ? */
extern void StuFillRectangle();
	/* Widget        w */
	/* XRectangle    rect */
	/* GC 		 gc    : fill rect with gc */

/********************************
 *
 * display utility
 *
 ********************************/

extern void StuDisplayBitmap();
	/* Widget w */
	/* Pixmap bitmap */
	/* unsigned int width, height, depth */
	/* Drawable drawable */
	/* GC gc */
	/* StPoint point */
	/* XRectangle clipRect */
extern void StuDisplayString();
	/* Widget w */
	/* String string */
	/* XFontStruct *font */
	/* Drawable drawable */
	/* GC gc */
	/* StPoint point */
	/* XRectangle clipRect */

#endif _StwP_h
