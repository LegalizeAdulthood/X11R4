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

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "ImageP.h"

#define offset(field)  XtOffset( ImageWidget, image.field )
static XtResource	resources[] = {
/* {name, call, type, size, offset, default_type, default_addr }, */
    {XtNforeground, XtCColor, XtRPixel, sizeof(Pixel),
	offset(foreground), XtRString, "XtDefaultForeground" },
    {XtNfont,  XtCFont, XtRFontStruct, sizeof(XFontStruct *),
	offset(font), XtRString, "XtDefaultFont"},
    {XtNarrign,  XtCArrign, XtRDimension, sizeof(Dimension),
	offset(arrign), XtRImmediate, (caddr_t)ARRIGN_PIXEL },
    {XtNbitmap, XtCPixmap, XtRPixmap, sizeof(Pixmap),
        offset(pixmap), XtRPixmap, (caddr_t)None },
    {XtNimagex, XtCWidth, XtRDimension, sizeof(Dimension),
        offset(image_x), XtRImmediate, (caddr_t)0 },
    {XtNimagey, XtCHeight, XtRDimension, sizeof(Dimension),
        offset(image_y), XtRImmediate, (caddr_t)0 },
    {XtNinternalWidth, XtCWidth, XtRDimension,  sizeof(Dimension),
        offset(internal_width), XtRImmediate, (caddr_t)4},
    {XtNinternalHeight, XtCHeight, XtRDimension, sizeof(Dimension),
        offset(internal_height), XtRImmediate, (caddr_t)2},
    {XtNcwidth, XtCWidth, XtRDimension,  sizeof(Dimension),
        offset(cwidth), XtRImmediate, (caddr_t)0},
    {XtNcheight, XtCHeight, XtRDimension, sizeof(Dimension),
        offset(cheight), XtRImmediate, (caddr_t)0},
    {XtNimagecx, XtCWidth, XtRDimension,  sizeof(Dimension),
        offset(image_cx), XtRImmediate, (caddr_t)0},
    {XtNimagecy, XtCHeight, XtRDimension, sizeof(Dimension),
        offset(image_cy), XtRImmediate, (caddr_t)0},
    {XtNstrings, XtCStrings, XtRPointer, sizeof(caddr_t),
        offset(strings), XtRPointer, NULL }
};
#undef offset

static void	Initialize();
static void	Destroy();
static void	Realize();
static void	SetTextWidthAndHeight();
static Boolean	SetValues();

ImageClassRec imageClassRec = {
  { /* CoreClass fields */
#define superclass              (&simpleClassRec)
    /* superclass		*/	(WidgetClass) superclass,
    /* class_name		*/	"Image",
    /* widget_size		*/	sizeof(ImageRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	NULL,
    /* num_actions		*/	0,
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	NULL,
    /* expose			*/	ImageRedisplay,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	NULL,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
    { /* image fields */
        0, /* empty */
    },
    {
	0, /* empty */
    }
};

WidgetClass imageWidgetClass = (WidgetClass)&imageClassRec;
/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

/*
 * Calculate width and height of displayed text in pixels
 */
static void
SetTextWidthAndHeight( w )
ImageWidget w;
{
    register XFontStruct        *fs = w->image.font;

    w->image.font_height = fs->max_bounds.ascent + fs->max_bounds.descent;
    w->image.font_width = XTextWidth( fs, "?", 1 );

    if (w->image.pixmap != None) {
	Window		root;
	int		x, y;
	unsigned int	width, height, bw, depth;

	if (XGetGeometry(XtDisplay(w), w->image.pixmap, &root, &x, &y,
		   &width, &height, &bw, &depth)) {
	    w->image.image_width = width;
	    w->image.image_height = height;
	    w->image.image_len = depth;
	}
    }
}

static void GetnormalGC(iw)
    ImageWidget iw;
{
    XGCValues   values;

    values.function = GXcopy;
    values.foreground   = iw->image.foreground;
    values.background   = iw->core.background_pixel;
    values.font         = iw->image.font->fid;

    iw->image.normal_GC = XtGetGC(
        (Widget)iw,
        (unsigned) GCFunction | GCForeground | GCBackground | GCFont,
        &values);
}

static void GetgrayGC(iw)
    ImageWidget iw;
{
    XGCValues   values;

    values.foreground = iw->image.foreground;
    values.background = iw->core.background_pixel;
    values.font       = iw->image.font->fid;
    values.fill_style = FillTiled;
    values.tile       = XmuCreateStippledPixmap(XtScreen((Widget)iw),
                                                iw->image.foreground,
                                                iw->core.background_pixel,
                                                iw->core.depth);

    iw->image.gray_GC = XtGetGC((Widget)iw,
                                (unsigned) GCForeground | GCBackground |
                                           GCFont | GCTile | GCFillStyle,
                                &values);
}

/* ARGSUSED */
static void
Initialize( request, new )
Widget	request, new;
{
    ImageWidget	w = (ImageWidget)new;
    XGCValues	myXGCV;

    GetnormalGC( w );
    GetgrayGC( w );

    SetTextWidthAndHeight( w );

    ArrignValues( w );
}

static void
Realize( gw, valueMask, attrs )
Widget			gw;
XtValueMask		*valueMask;
XSetWindowAttributes	*attrs;
{
    ImageWidget		w = (ImageWidget)gw;
    XGCValues		myXGCV;
    Window		root;
    int			x, y;
    unsigned int	width, height, bw, depth;

    (*superclass->core_class.realize) ( w, valueMask, attrs );

    if (XGetGeometry(XtDisplay(w), XtWindow(w),
			  &root, &x, &y, &width, &height, &bw, &depth)) {
	w->image.depth = depth;
    }

    w->image.backmap = XCreatePixmap( XtDisplay( w ), XtWindow( w ),
			      w->core.width, w->core.height,
			      w->image.depth );

    myXGCV.function = GXcopy;
    myXGCV.foreground = w->core.background_pixel;
    myXGCV.background = w->image.foreground;
    myXGCV.font = w->image.font->fid;
    w->image.erase_GC = XCreateGC( XtDisplay( w ), XtWindow( w ), 
	    GCFunction | GCForeground | GCBackground | GCFont, &myXGCV );
}

/*
 * Repaint the widget window
 */
void
ImageRedisplay( w ) /* ARGSUSED */
Widget	w;
{
    ImageWidget	iw = (ImageWidget) w;
    ImageString	*ptr;
    ImageStr	*sptr;
    int		wid = iw->image.font_width;
    int		hei = iw->image.font_height;
    int		i, num;

    /* ###
    XCopyArea( XtDisplay(iw), iw->image.backmap, iw->image.backmap,
	     iw->image.erase_GC, 0, 0, iw->core.width, iw->core.height, 0, 0 );
*/
    XFillRectangle( XtDisplay(iw), iw->image.backmap,
	     iw->image.erase_GC, 0, 0, iw->core.width, iw->core.height );

   if (iw->image.pixmap != None) {
	if (iw->image.image_len == 1) { /* depth */
	    if( iw->image.arrign == ARRIGN_PIXEL ) {
		XCopyPlane( XtDisplay(iw), iw->image.pixmap, iw->image.backmap,
			iw->image.normal_GC, 0, 0,
			iw->image.image_width, iw->image.image_height,
			iw->image.image_x + iw->image.internal_width,
			iw->image.image_y + iw->image.internal_height, 1L);
	    }
	    else {
		XCopyPlane( XtDisplay(iw), iw->image.pixmap, iw->image.backmap,
			iw->image.normal_GC, 0, 0,
			iw->image.image_width, iw->image.image_height,
    iw->image.image_cx*iw->image.font_width + iw->image.internal_width,
    iw->image.image_cy*iw->image.font_height + iw->image.internal_height, 1L);
	    }
	} else {
	    if( iw->image.arrign == ARRIGN_PIXEL ) {
		XCopyArea( XtDisplay(iw), iw->image.pixmap, iw->image.backmap,
			iw->image.normal_GC, 0, 0,
			iw->image.image_width, iw->image.image_height,
			iw->image.image_x + iw->image.internal_width,
			iw->image.image_y + iw->image.internal_height );
	    }
	    else {
		XCopyArea( XtDisplay(iw), iw->image.pixmap, iw->image.backmap,
			iw->image.normal_GC, 0, 0,
			iw->image.image_width, iw->image.image_height,
    iw->image.image_cx*iw->image.font_width + iw->image.internal_width,
    iw->image.image_cy*iw->image.font_height + iw->image.internal_height );
	    }
       }
    }

    for( ptr = iw->image.strings; ptr != (ImageString *)NULL;
						ptr = ptr->next ) {
	num = ptr->num; 
	for( i = 0, sptr = ptr->str; i < num; i ++, sptr ++ ) {
	    XDrawString( XtDisplay(iw), iw->image.backmap, iw->image.normal_GC,
		sptr->x * wid  + iw->image.internal_width,
		(sptr->y + 1)  * hei + iw->image.internal_height,
		sptr->str, strlen( sptr->str ) );
	}
    }
    XCopyArea( XtDisplay(iw), iw->image.backmap, XtWindow(iw),
	    iw->image.normal_GC, 0, 0, iw->core.width, iw->core.height, 0, 0 );
}

static void
Destroy( gw )
Widget  gw;
{
    ImageWidget w = (ImageWidget)gw;

    XtReleaseGC( w, w->image.normal_GC );
    XFreeGC( XtDisplay( w ), w->image.erase_GC );
    XFreePixmap( XtDisplay( w ), w->image.backmap );
}

/*
 * Set specified arguments into widget
 */

/* ARGSUSED */
static Boolean
SetValues(current, request, new)
Widget	current, request, new;
{
    ImageWidget curw = (ImageWidget) current;
    ImageWidget reqw = (ImageWidget) request;
    ImageWidget neww = (ImageWidget) new;

    if (   (curw->image.cwidth != neww->image.cwidth) 
	|| (curw->image.cheight != neww->image.cheight)
        || (curw->core.width != neww->core.width)
        || (curw->core.height != neww->core.height)
        || (curw->image.image_cx != neww->image.image_cx) 
	|| (curw->image.image_cy != neww->image.image_cy)
        || (curw->image.image_x != neww->image.image_x)
        || (curw->image.image_y != neww->image.image_y) ) {
	ArrignValues( neww );
    }

    return( False );
}

ArrignValues( w )
ImageWidget	w;
{
    if( w->image.arrign == ARRIGN_PIXEL ) {
	w->image.cwidth = (w->core.width - w->image.internal_width*2)
							/ w->image.font_width;
	w->image.cheight = (w->core.height - w->image.internal_height*2)
							/ w->image.font_height;
	w->image.image_cx =
		     w->image.image_x / w->image.font_width;
	w->image.image_cy =
		     w->image.image_y / w->image.font_height;
    }
    else {
	w->core.width = w->image.cwidth * w->image.font_width
						+ w->image.internal_width*2;
	w->core.height = w->image.cheight * w->image.font_height
						+ w->image.internal_height*2;
	w->image.image_x
		     = w->image.image_cx * w->image.font_width;
	w->image.image_y
		     = w->image.image_cy * w->image.font_height;
    }
}
