/*
 *	Original Source: ./lib/Xaw/Viewport.h
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */

/*
 * $XConsortium: Viewport.h,v 1.7 88/10/23 14:43:56 swick Exp $
 * Public definitions for StPopupSheetView widget
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

#ifndef _StPopupSheetView_h
#define _StPopupSheetView_h

#include <X11/Xmu.h>

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 ancestorSensitive   Sensitive		Boolean		True
 background	     Background		Pixel		XtDefaultBackground
 backgroundPixmap    Pixmap     	Pixmap     	None
 borderColor	     BorderColor	Pixel		XtDefaultForeground
 borderPixmap        Pixmap     	Pixmap     	None
 borderWidth	     BorderWidth	Dimension	2
 depth		     Depth		int		0
 destroyCallback     Callback		Pointer		NULL
 height		     Height		Dimension	0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 sensitive	     Sensitive		Boolean		True
 translations	     Translations	Translations    NULL
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0
 insertPosition      InsertPosition     OrderProc	NULL
 bottomMargin	     StCMargin		Dimension 	0 
 topMargin	     StCMargin		Dimension 	0
 leftMargin	     StCMargin		Dimension  	0	
 rightMargin	     StCMargin		Dimension	0
 maxHeight	     StCMargin 		Dimension 	65535 
 maxWidth      	     StCMargin 		Dimension 	65535 
 minHeight	     StCMargin 		Dimension 	0
 minWidth	     StCMargin 		Dimension	0
 title		     StCtitle 		String 		NULL 
 titleBackground     Background 	Pixel 		XtDefaultBackground
 titleBackgroundPixmap Pixmap 		Pixmap 		None
 titleBorderColor    BorderColor 	Pixel 		XtDefaultForeground
 titleBorderPixmap 	Pixmap 		Pixmap		None
 titleBorderWidth    BorderWidth 	Dimension 	2 
 titleFont	     Font		XFontStruct * 	XtDefaultFont
 titleJustify 	     Justify 		Justify 	XtJustifyCenter 
 titleForeground     Foreground 	Pixel 		XtDefaultForeground
 windowable 	     Boolean 		Boolean 	FALSE

*/

/* fields added to Composit */
#define	StNbottomMargin		"bottomMargin"
#define	StNtopMargin		"topMargin"
#define	StNleftMargin		"leftMargin"
#define	StNrightMargin		"rightMargin"
#define	StNmaxHeight		"maxHeight"
#define	StNmaxWidth		"maxWidth"
#define	StNminHeight		"minHeight"
#define	StNminWidth		"minWidth"
#define	StNtitle		"title"
#define	StNfont			"stfont"
#define	StNtitleBackground	"titleBackground"
#define	StNtitleBackgroundPixmap	"titleBackgroundPixmap"
#define	StNtitleBorderColor	"titleBorderColor"
#define	StNtitleBorderPixmap	"titleBorderPixmap"
#define	StNtitleBorderWidth	"titleBorderWidth"
#define	StNtitleFont		"titleFont"
#define	StNtitleForeground	"titleForeground"
#define	StNtitleJustify		"titleJustify"
#define	StNwindowable		"windowable"
#define	StCMargin		"StMargin"
#define	StCWindowHeight		"StWindowHeight"
#define	StCWindowWidth		"StWindowWidth"
#define	StCwindowable		"Stwindowable"
#define	StCtitle		"Sttitle"

extern WidgetClass stPopupSheetViewWidgetClass;

typedef struct _StPopupSheetViewClassRec *StPopupSheetViewWidgetClass;
typedef struct _StPopupSheetViewRec	 *StPopupSheetViewWidget;

#endif _StPopupSheetView_h
