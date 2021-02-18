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

#ifndef _Image_h
#define _Image_h

/****************************************************************
 *
 * Image widgets
 *
 ****************************************************************/

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 cursor		     Cursor		Cursor		None
 destroyCallback     Callback		Pointer		NULL
 height		     Height		Dimension	0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 sensitive	     Sensitive		Boolean		True
 width		     Width		Dimension	0
 x		     Position		Position	0
 y		     Position		Position	0
 callback            Callback           Pointer         NULL
 drawingColor1       Color              Pixel           XtDefaultForeground
 drawingColor2       Color              Pixel           XtDefaultForeground
 exposeCallback      Callback           Callback        NULL
 font                Font               XFontStruct*    XtDefaultFont

*/

/* define any special resource names here that are not in 
   <X11/StringDefs.h> */

#define XtNforeground		"foreground"
#define XtNarrign		"arrign"
#define XtNbitmap		"bitmap"
#define XtNimagex		"imagex"
#define XtNimagey		"imagey"
#define XtNcwidth		"cwidth"
#define XtNcheight		"cheight"
#define XtNimagecx		"imagecx"
#define XtNimagecy		"imagecy"
#define XtNstrings		"string"
#define XtNfont			"font"
#define XtNinternalWidth	"internalWidth"
#define XtNinternalHeight	"internalHeight"

#define	XtCBitmap		"Bitmap"
#define XtCStrings		"String"
#define XtCArrign		"Arrign"

#define	ARRIGN_PIXEL	0
#define	ARRIGN_CHAR	1

extern void	ImageRedisplay(/* no arg */);

/* declare specific ImageWidget class and instance datatypes */

typedef struct _ImageClassRec	*ImageWidgetClass;
typedef struct _ImageRec	*ImageWidget;

/* declare the class constant */

extern WidgetClass imageWidgetClass;

typedef struct { 
    int			x;
    int			y;
    char		*str;		
} ImageStr;

typedef struct _ImageString { 
    int			num;
    ImageStr		*str;
    struct _ImageString	*next;
} ImageString;

#endif  _Image_h
