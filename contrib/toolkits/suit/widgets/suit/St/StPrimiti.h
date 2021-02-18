/*
 *      Original Source: ./lib/Xaw/Simple.h
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */
/*
* $XConsortium: Simple.h,v 1.6 88/10/23 14:31:50 swick Exp $
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

#ifndef _StPrimitive_h
#define _StPrimitive_h

/***********************************************************************
 *
 * SUIT Primitive Widget
 *
 ***********************************************************************/

#include <X11/Core.h>

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 backgroundPixmap    Pixmap		Pixmap		None
 borderColor         BorderColor        Pixel           XtDefaultForeground
 borderPixmap        Pixmap             Pixmap          None
 borderWidth         BorderWidth        Dimension       1
 callback	     Callback		Pointer		NULL
 callbackInterval    StCIntervalTime    int             0
 cursor		     Cursor		Cursor		None
 contents	     StCContents        StContents      0
 destroyCallback     Callback		Pointer		NULL
 font		     Font		XFontStruct*	XtDefaultFont
 foreground	     Foreground		Pixel		XtDefaultForeground
 height		     Height		Dimension	0
 insensitivePixmap   Pixmap             Pixmap          None
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 maxHeight           StCMaxSize         Dimension       65535
 maxWidth            StCMaxSize         Dimension       65535
 minHeight           StCMinSize         Dimension       0
 minWidth            StCMinSize         Dimension       0
 sensitive	     Sensitive		Boolean		True
 transparence        StCTransparence    Boolean         False
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0

*/

#define StNcallback		"stCallback"
#define StNcallbackInterval     "stCallbackInterval"
#define StNcursor               "stCursor"
#define StNcontents             "stContents"
#define StNfont			"stFont"
#define StNforeground 		"stForeground"
#define StNinsensitivePixmap    "stInsensitivePixmap"
#define StNmaxHeight            "stMaxHeight"
#define StNmaxWidth             "stMaxWidth"
#define StNminHeight            "stMinHeight"
#define StNminWidth             "stMinWidth"
#define StNtransparence         "stTransparence"

#define StCIntervalTime         "StIntervalTime"
#define StCContents             "StContents"
#define StCMaxSize              "StMaxsize"
#define StCMinSize              "StMinsize"
#define StCTransparence         "StTransParence"

#define StRStContents		"StContents"

extern WidgetClass	stPrimitiveWidgetClass;

typedef struct _StPrimitiveClassRec	*StPrimitiveWidgetClass;
typedef struct _StPrimitiveRec		*StPrimitiveWidget;

#endif _StPrimitive_h
/* DON'T ADD STUFF AFTER THIS */
