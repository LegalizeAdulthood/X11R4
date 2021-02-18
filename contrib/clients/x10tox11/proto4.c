/*
 * Copyright 1987 by Tektronix Inc, Beaverton, Oregon.
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Tektronix not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 *
 *
 * TEKTRONIX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * TEKTRONIX BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/* 
 * $XConsortium: proto4.c,v 6.3 89/02/02 16:28:44 rws Exp $ 
 * $Locker:  $ 
 */
#include "resource.h"

void X10ProcGetFont(client, name, rep)
	int	client;
	char	*name;
	X10Rep	*rep;
{
	extern FontMap	x10fontMap[];
	static char	**fonts;
	static int	nfonts;
	Font	fontid;
	int	index;
	Display *dpy = clist[ client ]->dpy;

	/*
	 * We keep our own list of fonts here so that the OpenFont
	 * request is sure to succeed.
	 */
	if (fonts == NULL)
		fonts = X11FontNames(dpy, &nfonts);

	debug(X10_GetFont, ("font %s ", name));
	/*
	 * If this font is known by the server, use it.
	 */
	if (searchlist(fonts, nfonts, name) >= 0)
		;
	/*
	 * If this font with capitals removed is known, use it.
	 */
	else if ((index = searchlist(fonts, nfonts, ToLower(name))) >= 0)
		name = fonts[ index ];
		
	/*
	 * If the font maps to an x11 font, use it.
	 */
	else if ((index = searchFontMap(name)) >= 0)
		name = fonts[ x10fontMap[ index ].fontindex ];

	/*
	 * otherwise we always succeed
	 */
	else
		name = fonts[ x10fontMap[ FONT_FIXED ].fontindex ];

	debug(X10_GetFont, ("... mapped to %s", name));
	if ((fontid = XLoadFont(dpy, name)) == 0) {
		Xstatus = X10BadFont;
		return;
	}

	if (! AddFont(client, fontid)) {
		Xstatus = X10BadFont;
		XUnloadFont(dpy, fontid);
		return;
	}

	rep->param.l[0] = fontid;
}

void X10ProcFreeFont(client, req)
	int	client;
	X10Req	*req;
{
	FontInfoptr fp;

	fp = FontLookup(client, (Font)req->param.l[0]);
	if (fp == NULL)
		Xstatus = X10BadFont;
	else
		FreeFontInfo(client, fp);
}

Bool X10ProcQueryFont(client, req, rep)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
{
	XFontStruct	*fstruct;
	FontInfoptr	finfo;

	if ((finfo = FontLookup(client, (Font)req->param.l[0])) == NULL) {
		Xstatus = X10BadFont;
		return(False);
	}

	fstruct = finfo->info;
	rep->param.s[0] = finfo->ascent + finfo->descent;/* height */
	rep->param.s[1] = fstruct->max_bounds.rbearing	/* "average" width */
			- fstruct->min_bounds.lbearing;
	rep->param.s[2] = fstruct->min_char_or_byte2;	/* 1st character */
	rep->param.s[3] = fstruct->max_char_or_byte2;	/* last character */
	rep->param.s[4] = finfo->descent;		/* baseline */
	rep->param.s[5] = (fstruct->per_char == NULL); /* 1=fixed, 0=variable */
	return(True);
}

Bool X10ProcStringWidth(client, req, rep, string)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
	char	*string;
{
	FontInfoptr	finfo;

	if ((finfo = FontLookup(client, (Font)req->param.l[0])) == NULL)
		return(False);
	rep->param.s[0] = XTextWidth(finfo->info, string, req->param.s[2]);
	return(True);
}

Bool X10ProcFontWidths(client, req, rep, pwidths)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
	short	**pwidths;
{
	XFontStruct	*fstruct;
	FontInfoptr	finfo;
	int	first, last, nshorts, i, count;
	char	c;
	short		*widths;

	if ((finfo = FontLookup(client, (Font)req->param.l[0])) == NULL) {
		Xstatus = X10BadFont;
		return(False);
	}
	fstruct = finfo->info;
	first = fstruct->min_char_or_byte2;
	last = fstruct->max_char_or_byte2;
	nshorts = last - first + 1;
	count = nshorts << 1;
	widths = *pwidths = (short *)StaticAlloc( X10WordPad(count) );
	for (i = 0; i < nshorts; i++) {
		c = i + first;
		widths[ i ] = XTextWidth(finfo->info, &c, 1);
	}
	rep->param.l[0] = (long)count;
	return(True);
}

Bool X10ProcCharWidths(client, req, rep, string, pwidths)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
	char	*string;
	short	**pwidths;
{
	FontInfoptr	finfo;
	long		nchars = req->param.l[2],
			count = nchars << 1,
			i;
	short		*widths;

	if (nchars < 0
	 || (finfo = FontLookup(client, (Font)req->param.l[0])) == NULL) {
		Xstatus = X10BadValue;
		return(False);
	}

	widths = *pwidths = (short *)StaticAlloc( (int)X10WordPad(count) );

	for (i=0; i<nchars; i++)
		widths[ i ] = XTextWidth(finfo->info, string+i, 1);
	rep->param.l[0] = count;
	return(True);
}

void X10ProcText(client, req, string, fillBackground)
	int	client;
	X10Req	*req;
	char	*string;
	Bool	fillBackground;
{
#define TITEM_INC	16
#define	TITEM_MULTIPLE(n)	(((n) & 0xf) == 0)

	typedef struct TextItemList {
		XTextItem	*list;
		int		itemCnt;
		int		allocCnt;
	} TextItemList;
	static TextItemList	items;
	XTextItem	*pitem;
	Clientptr 	cp = clist[ client ];
	Display		*dpy = cp->dpy;
	FontInfoptr	fp;
	PixmapInfoptr	pp;
	int		x, y,
			length = req->param.s[6],
			icPad = req->param.b[14],   /* inter-character pad */
			spacePad = req->param.b[15],/* padding for space */
			index,
			direction,
			junkAscent,
			junkDescent;
	XCharStruct	overall;
	unsigned long	valuemask;
	unsigned int	xExtent, yExtent;
	XGCValues	gcv;
	GC		gc;
	Drawable	d = req->windowId;
	GetWinInfoOrSetBadDrawable(client, d, wp, return);

	if (!WindowMapRequested(wp->parent))
		return;
	cp->pendingFlags |= GraphicsPending;

	gc = wp->gc;

	fp = FontLookup(client, (Font)req->param.l[1]);
	if (fp == NULL) {
		Xstatus = X10BadFont;
		return;
	}

	valuemask = GCFont|GCFunction|GCForeground|GCFillStyle|GCPlaneMask;
	gcv.font = fp->id;
	gcv.function = X10toX11function(req->func);
	gcv.foreground = X10toX11pixel((unsigned long)req->param.u[4]);
	if (fillBackground) {
		gcv.background = X10toX11pixel((unsigned long)req->param.u[5]);
		valuemask |= GCBackground;
	}
	gcv.plane_mask = req->mask;
	gcv.fill_style = FillSolid;
	/* we don't set the gc 'till later, because we may have more to do */

	x = req->param.s[0];
	y = req->param.s[1] + fp->ascent;
	CheckTransparentDrawableAndXY(dpy, wp, &d, &x, &y);

	/*
	 * assemble the text items for this request.
	 */
	if (items.list == NULL) {
		items.list = (XTextItem *)Xalloc(sizeof(XTextItem) * TITEM_INC);
		items.allocCnt = TITEM_INC;
	}
	if (icPad == 0 && spacePad == 0) {
		items.itemCnt = 1;
		pitem = &items.list[ 0 ];
		pitem->chars = string;
		pitem->delta = 0;
		pitem->nchars = length;
		pitem->font = None;
		if (fillBackground) {
			XTextExtents(fp->info, string, length, &direction,
				&junkAscent, &junkDescent, &overall);
			xExtent = overall.rbearing - overall.lbearing;
		}
	} else {
	    index = 0;
	    items.itemCnt = 0;
	    xExtent = 0;
	    while (index < length) {
		pitem = &items.list[ items.itemCnt ];
		pitem->chars = string + index;
		pitem->delta = 0;
		pitem->nchars = 0;
		pitem->font = None;
		/*
		 * If there are initial spaces, figure up the delta
		 * separately for this string.
		 */
		while (index < length && string[ index ] == ' ') {
		    pitem->nchars++;
		    pitem->delta += spacePad + icPad;
		    index++;
		    if (icPad)
			break;
		}

		/*
		 * Assemble the non-space characters.
		 * If there is an inter-character pad, and there is already
		 * a character in the item, then don't do this loop.
		 */
		if (pitem->nchars == 0 || icPad == 0) {
		    while (index < length
		     && (spacePad == 0 || string[ index ] != ' ')) {
			pitem->nchars++;
			pitem->delta += icPad;
			index++;
			if (icPad)
			    break;
		    }
		}
		if (pitem->nchars == 0)
		    Error("X10ProcText: logic error\n");

		/*
		 * We don't put inter-character pad on the first
		 * character.
		 */
		if (icPad && pitem == &items.list[ 0 ])
		    pitem->delta -= icPad;

		if (fillBackground) {
		    XTextExtents(fp->info, pitem->chars, pitem->nchars,
			    &direction, &junkAscent, &junkDescent, &overall);
		    xExtent += overall.rbearing - overall.lbearing;
		    xExtent += pitem->delta;
		}

		items.itemCnt++;
		if (index < length && TITEM_MULTIPLE(items.itemCnt)) {
		    items.allocCnt += TITEM_INC;
		    items.list = (XTextItem *)Xrealloc((char *)items.list,
			    sizeof(XTextItem) * items.allocCnt);
		}
	    }
	}

	if (icPad == 0 && spacePad == 0
	 && fillBackground && gcv.function == GXcopy) {
	    XChangeGC(dpy, gc, valuemask, &gcv);
	    XDrawImageString(dpy, d, gc, x, y, string, length);
	    return;
	}

	/*
	 * All of this goo is because the ImageText protocol request
	 * only implements the GXCopy function.
	 */
	if (fillBackground) {
	    /*
	     * This effectively sets the font in the pixmap gc.
	     */
	    items.list[ 0 ].font = fp->id;

	    yExtent = fp->ascent + fp->descent;
	    pp = GetTextPixmap(client, xExtent, yExtent);
	    gcv.graphics_exposures = False;
	    valuemask |= GCGraphicsExposures;
	    XChangeGC(dpy, gc, valuemask, &gcv);

	    /*
	     * Clear the background of the pixmap, and draw the text on
	     * top of it.  This could be done faster if the ImageText8
	     * protocol request accepted text items... as it is
	     * it only accepts strings, so we can't incorporate the
	     * inter-character spacing and the space pad.
	     */
	    XSetFunction(dpy, pp->gc, GXclear);
	    XSetForeground(dpy, pp->gc, (unsigned long)0);
	    XFillRectangle(dpy, pp->id, pp->gc,
		    0, 0,		/* x and y */
		    xExtent, yExtent);	/* width and height */
	    XSetFunction(dpy, pp->gc, GXcopy);
	    XSetForeground(dpy, pp->gc, (unsigned long)1);
	    XDrawText(dpy, pp->id, pp->gc,
		    -overall.lbearing, fp->ascent,	/* x and y */
		    items.list,
		    items.itemCnt);

	    /*
	     * now copy the resulting bitmap onto the window
	     * with the requested operation.
	     */
	    XCopyPlane(dpy, pp->id, d, gc,
		      0, 0,			/* src x,y */
		      xExtent, yExtent,		/* width and height */
		      x-overall.lbearing,	/* dest x */
		      y-fp->ascent,		/* dest y */
		      (unsigned long)1);	/* bit plane */
	} else {
	    XChangeGC(dpy, gc, valuemask, &gcv);
	    XDrawText(dpy, d, gc,
		    x, y,
		    items.list,
		    items.itemCnt);
	    return;
	}
#undef TITEM_INC
#undef TITEM_MULTIPLE
}

Bool X10ProcMakePixmap(client, req, rep)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
{
	Clientptr cp = clist[ client ];
	Display	*dpy = cp->dpy;
	PixmapInfoptr pp, ppbitmap;
	XGCValues	gcv;
	unsigned	width, height;

	if (req->param.l[0] == 0) {
		width = BestWidth;
		height = BestHeight;
		ppbitmap = NULL;
	} else {
		ppbitmap = BitmapLookup(client, (XID) req->param.l[0]);
		if (ppbitmap == NULL) {
			Xstatus = X10BadBitmap;
			return(False);
		}
		width = ppbitmap->width;
		height = ppbitmap->height;
	}

	pp = CreatePixmap(client, DefaultRootWindow(dpy),
		width, height,
		(unsigned int)ScreenDepth,
		ppbitmap);
	if (pp == 0) {
		Xstatus = X10BadAlloc;
		return(False);
	} 

	if (req->param.l[0] == 0) {
		gcv.foreground = X10toX11pixel((unsigned long)req->param.u[2]);
		gcv.fill_style = FillSolid;
		XChangeGC(cp->dpy, pp->gc, GCForeground|GCFillStyle, &gcv);
		XFillRectangle(cp->dpy, pp->id, pp->gc, 0, 0, width, height);
	} else {
		gcv.foreground = X10toX11pixel((unsigned long)req->param.u[2]);
		gcv.background = X10toX11pixel((unsigned long)req->param.u[3]);
		gcv.graphics_exposures = False;
		XChangeGC(cp->dpy,
			  pp->gc,
			  GCForeground|GCBackground|GCGraphicsExposures,
			  &gcv);
		/* we simply replicate the 1's and 0's from the bitmap */
		XCopyPlane(cp->dpy, ppbitmap->id, pp->id, pp->gc,
			0, 0,			/* src x,y */
			width, height,
			0, 0,			/* dest x,y */
			(unsigned long)0x1);	/* plane mask */
	}
	rep->param.l[0] = pp->id;

	return(True);
}

void X10ProcPixmapPut(client, req)
	int	client;
	X10Req	*req;
{
	XGCValues	gcv;
	Clientptr	cp = clist[ client ];
	Display		*dpy = cp->dpy;
	PixmapInfoptr	pp;
	Drawable	d = req->windowId;
	int		x = req->param.s[6],
			y = req->param.s[7],
			srcx = req->param.s[2],
			srcy = req->param.s[3];
	unsigned int	width = req->param.s[1],
			height = req->param.s[0];
	unsigned long	gcmask;
	GetWinInfoOrSetBadDrawable(client, d, wp, return);

	if (!WindowMapRequested(wp->parent))
		return;
	cp->pendingFlags |= GraphicsPending;

	pp = PixmapLookup(client, (Pixmap)req->param.l[2]);
	if (pp == NULL) {
		Xstatus = X10BadPixmap;
		return;
	}

	gcmask = GCFunction | GCPlaneMask | GCGraphicsExposures;
	gcv.graphics_exposures = False;
	gcv.function = X10toX11function(req->func);
	gcv.plane_mask = req->mask;
	XChangeGC(dpy, wp->gc, gcmask, &gcv);

	CheckTransparentDrawableAndXY(dpy, wp, &d, &x, &y);
	XCopyArea(dpy,
		pp->id,			/* source drawable */
		d,			/* destination drawable */
		wp->gc,
		srcx, srcy,
		width, height,
		x, y);			/* dest x, y */

	/*
	 * try to guess if this is a save-under operation.  In particular,
	 * we want to know if this is type #2 restore where
	 *	- pixmap save on root into p
	 *	- map w
	 *	- PixmapPut p onto w
	 *	- UnmapTransparent on w
	 */
	if (wp->width + (wp->borderWidth<<1) == pp->width
	 && wp->height + (wp->borderWidth<<1) == pp->height) {
		wp->savedPixmap = pp->id;
		pp->isBackingStore = True;
	}
}

char *X10ProcPixmapGet(client, req, rep)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
{
	Clientptr	cp = clist[ client ];
	Display 	*dpy = cp->dpy;
	unsigned int	width = req->param.s[1],
			height = req->param.s[0];
	int		x = req->param.s[2],
			y = req->param.s[3],
			format,
			x10length;
	XImage		*image;
	char		*data;
	Window		win = req->windowId;
	GetWinInfoOrSetBadWindow(client, win, wp, return (NULL));

	x10length = X10CheckPixmapSize((int)req->func,
				       (int)height,
				       (int)width);
	if (x10length == 0)
            return (NULL);
	rep->param.l[0] = (long)x10length;

	if (req->func == 0)
		format = XYPixmap;
	else if (req->func == 1)
		format = ZPixmap;
	else {
		Xstatus = X10BadValue;
		return (NULL);
	}

	CheckTransparentDrawableAndXY(dpy, wp, &win, &x, &y);
	/*
	 * XXX will it matter if d is InputOnly but width and height
	 * are too big after clipping?  Maybe not.
	 */
	image = XGetImage(dpy,
			win,		/* drawable */
			x, y,		/* source x, y */
			width, height,	/* width, height */
			(long)~0,		/* plane mask */
			format);
	data = StaticAlloc(x10length);
	X11toX10image(image, data, ClientByteOrder(client));
	XDestroyImage(image);
	return(data);
}

void X10ProcPixmapBitsPut(client, req, data)
	int	client;
	X10Req	*req;
	char	*data;
{
	Clientptr	cp = clist[ client ];
	Display 	*dpy = cp->dpy;
	unsigned int	width = req->param.s[1],
			height = req->param.s[0];
	int		x = req->param.s[2],
			y = req->param.s[3];
	unsigned long	valuemask;
	int		format;
	XImage		*image;
	XGCValues	gcv;
	GC		gc;
	Drawable	d = req->windowId;
	GetWinInfoOrSetBadDrawable(client, d, wp, return);

	if (!WindowMapRequested(wp->parent))
		return;
	cp->pendingFlags |= GraphicsPending;

	if (req->param.s[4] == 0)
		format = XYPixmap;
	else if (req->param.s[4] == 1)
		format = ZPixmap;
	else {
		Xstatus = X10BadValue;
		return;
	}

	gc = wp->gc;
	valuemask = GCFunction|GCPlaneMask;

	CheckTransparentDrawableAndXY(dpy, wp, &d, &x, &y);

	if (req->param.l[3]) {
		PixmapInfoptr pp;

		/*
		 * XXX note that this overrides the clip rectangles set by
		 * CheckTransparentDrawableAndXY() if 'd' was a transparent
		 * window.  The fix is to merge this with the clip
		 * rectangles.
		 */
		pp = PixmapLookup(client, (Pixmap)req->param.l[0]);
		if (pp == NULL) {
			Xstatus = X10BadBitmap;
			return;
		}
		if (pp->width != width || pp->height != height) {
			Xstatus = X10BadMatch;
			return;
		}
		valuemask |= GCClipXOrigin|GCClipYOrigin|GCClipMask;
		gcv.clip_mask = req->param.l[3];
		gcv.clip_x_origin = x;
		gcv.clip_y_origin = y;
	}

	gcv.function = X10toX11function(req->func);
	gcv.plane_mask = req->mask;
	XChangeGC(dpy, gc, valuemask, &gcv);

	image = CreateX10Image (client, ScreenDepth,
				format, data, width, height);
	XPutImage(dpy,
		d,		/* drawable */
		gc,		/* gc */
		image,
		0, 0,		/* source x, y offset */
		x, y,		/* destination x, y */
		width, height); /* width, height */
	XDestroyImage(image);

	if (req->param.l[3]) {
		gcv.clip_mask = 0;
		XChangeGC(cp->dpy, gc, GCClipMask, &gcv);
	}
}

void X10ProcBitmapBitsPut(client, req, data)
	int	client;
	X10Req	*req;
	char	*data;
{
	Clientptr	cp = clist[ client ];
	Display 	*dpy = cp->dpy;
	unsigned int	width = req->param.s[1],
			height = req->param.s[0];
	int		x = req->param.s[2],
			y = req->param.s[3],
			function = X10toX11function(req->func);
	unsigned long	foreground =
				X10toX11pixel((unsigned long) req->param.u[4]),
			background =
				X10toX11pixel((unsigned long) req->param.u[5]);
	unsigned long	gcmask;
	XGCValues	gcv;
	GC		gc;
	Pixmap		bitmapMask = req->param.l[3];
	Drawable	d = req->windowId;
	GetWinInfoOrSetBadDrawable(client, d, wp, return);

	if (!WindowMapRequested(wp->parent))
		return;
	cp->pendingFlags |= GraphicsPending;

	gc = wp->gc;
	CheckTransparentDrawableAndXY(dpy, wp, &d, &x, &y);

	/*
	 * If there is a bitmap mask, then do a transparent stipple
	 * for each of the forground and background.  Essentially:
	 *	dest = (src & mask) rop dest 
	 *	dest = ('src & mask) rop dest
	 * where rop is the graphic function in the request, but it is
	 * applied with a transparent stipple.  The gc forground color
	 * in the first operation is the forground in the request.  The gc
	 * forground in the second operation is the background in the
	 * request.
	 */
	if (bitmapMask) {
		PixmapInfoptr	pmask;
		PixmapInfoptr	psrc;
		Pixmap		notsrc;

		pmask = BitmapLookup(client, bitmapMask);
		if (pmask == NULL) {
			Xstatus = X10BadBitmap;
			return;
		}
		if (pmask->width != width || pmask->height != height) {
			Xstatus = X10BadMatch;
			return;
		}

		/*
		 * First off, put the bitmap into an off-screen pixmap.
		 */
		psrc = StorePixmapOrBitmap(
				client,
				width,
				height,
				1,			/* depth */
				XYBitmap,		/* format */
				data);
		if (psrc == NULL) {
			Xstatus = X10BadAlloc;
			return;
		}

		notsrc = XCreatePixmap(dpy, DefaultRootWindow(dpy),
			width, height, 1);

		/*
		 * notsrc = ~psrc
		 */
		gcv.function = GXcopyInverted;
		gcv.plane_mask = ~0;
		gcv.graphics_exposures = False;
		XChangeGC(dpy,
			  psrc->gc,
			  GCFunction | GCPlaneMask | GCGraphicsExposures,
			  &gcv);
		XCopyArea(dpy, psrc->id, notsrc, psrc->gc,
			0, 0, width, height, 0, 0);

		/*
		 * psrc = psrc & mask
		 */
		gcv.function = GXand;
		XChangeGC(dpy, psrc->gc, GCFunction, &gcv);
		XCopyArea(dpy, pmask->id, psrc->id, psrc->gc,
			0, 0, width, height, 0, 0);

		/*
		 * notsrc = notsrc & mask
		 */
		XCopyArea(dpy, pmask->id, notsrc, psrc->gc,
			0, 0, width, height, 0, 0);

		/*
		 * dest = (src & mask) rop dest 
		 * Now we are using the window gc!  This way if the
		 * clip rectangles have been set, we will get clipped.
		 */
		gcmask = GCStipple | GCFillStyle | GCFunction |
			GCForeground | GCPlaneMask | GCGraphicsExposures |
			GCTileStipXOrigin | GCTileStipYOrigin;
		gcv.graphics_exposures = False;
		gcv.plane_mask = req->mask;
		gcv.stipple = psrc->id;
		gcv.fill_style = FillStippled;
		gcv.function = function;
		gcv.foreground = foreground;
		gcv.ts_x_origin = x;
		gcv.ts_y_origin = y;
		XChangeGC(dpy, gc, gcmask, &gcv);
		XFillRectangle(dpy, d, gc, x, y, width, height);

		/*
		 * dest = ('src & mask) rop dest
		 */
		gcv.stipple = notsrc;
		gcv.foreground = background;
		XChangeGC(dpy, gc, GCStipple | GCForeground, &gcv);
		XFillRectangle(dpy, d, gc, x, y, width, height);

		XFreePixmap(dpy, notsrc);
		FreePixmapInfo(client, psrc);
	}

	/*
	 * if there is no mask, then do an XPutImage of the
	 * src onto the window.
	 */
	else {
		XImage		*image;

		image = CreateX10Image (client,
					1,		/* depth */
					XYBitmap,	/* format */
					data,
					width, height);

		gcmask = GCFunction | GCForeground | GCBackground | GCPlaneMask;
		gcv.function = function;
		gcv.foreground = foreground;
		gcv.background = background;
		gcv.plane_mask = req->mask;
		XChangeGC(dpy, gc, gcmask, &gcv);

		XPutImage(dpy,
			d,	/* drawable */
			gc,	/* gc */
			image,
			0, 0,	/* source x, y */
			x, y,	/* destination x, y offset */
			width, height); /* width, height */
		XFree(image);
	}
}

void X10ProcPixFill(client, req)
	int	client;
	X10Req	*req;
{
	PixmapInfoptr	pp;
	XGCValues	gcv;
	Clientptr	cp = clist[ client ];
	Display		*dpy = cp->dpy;
	unsigned int	width = req->param.s[1],
			height = req->param.s[0];
	int		x = req->param.s[2],
			y = req->param.s[3];
	GC		gc;
	unsigned long	stipplemask;
	Pixmap		bitmask = req->param.l[3];
	Drawable d = req->windowId;
	GetWinInfoOrSetBadDrawable(client, req->windowId, wp, return);

	if (!WindowMapRequested(wp->parent))
		return;
	cp->pendingFlags |= GraphicsPending;

	gc = wp->gc;
	if (bitmask) {
		pp = BitmapLookup(client, bitmask);
		if (pp == NULL) {
			Xstatus = X10BadBitmap;
			return;
		}
		if (pp->width != width || pp->height != height) {
			Xstatus = X10BadMatch;
			return;
		}
		gcv.stipple = pp->id;
		gcv.ts_x_origin = x;
		gcv.ts_y_origin = y;
		gcv.fill_style = FillStippled;
		stipplemask = GCStipple|GCFillStyle
			|GCTileStipXOrigin|GCTileStipYOrigin;
	} else {
		gcv.fill_style = FillSolid;
		stipplemask = GCFillStyle;
	}

	gcv.function = X10toX11function(req->func);
	gcv.plane_mask = req->mask;
	gcv.foreground = X10toX11pixel((unsigned long)req->param.u[4]);
	XChangeGC(dpy,
		gc,
		stipplemask | (GCFunction|GCForeground|GCPlaneMask),
		&gcv);

	CheckTransparentDrawableAndXY(dpy, wp, &d, &x, &y);

	/*
	 * Very special case for MathScribe.
	 */
	if (width == 1 && height == 1)
		XDrawPoint(dpy, d, gc, x, y);
	else
		XFillRectangle(dpy, d, gc, x, y, width, height);
}

void X10ProcStippleFill(client, req)
	int	client;
	X10Req	*req;
{
	PixmapInfoptr pp;
	XGCValues	gcv;
	Clientptr cp = clist[ client ];
	Display *dpy = cp->dpy;
	unsigned int	width = (unsigned int)req->param.s[1],
			height = (unsigned int)req->param.s[0];
	int		x = (int)req->param.s[2],
			y = (int)req->param.s[3];
	Drawable d = req->windowId;
	GC		gc;
	GetWinInfoOrSetBadDrawable(client, req->windowId, wp, return);

	if (!WindowMapRequested(wp->parent))
		return;
	cp->pendingFlags |= GraphicsPending;

	gc = wp->gc;
	pp = BitmapLookup(client, (XID)req->param.l[3]);
	if (pp == NULL) {
		Xstatus = X10BadBitmap;
		return;
	}
	gcv.stipple = pp->id;
	gcv.fill_style = FillStippled;
	gcv.function = X10toX11function(req->func);
	gcv.plane_mask = req->mask;
	gcv.foreground = X10toX11pixel((unsigned long) req->param.u[4]);
	XChangeGC(dpy,
		gc,
		GCStipple|GCFillStyle|GCFunction|GCForeground|GCPlaneMask,
		&gcv);
	SetTileMode(dpy, wp, wp->tileModeRelative);

	CheckTransparentDrawableAndXY(dpy, wp, &d, &x, &y);

	XFillRectangle(dpy, d, gc, x, y, width, height);
}

void X10ProcTileFill(client, req)
	int	client;
	X10Req	*req;
{
	PixmapInfoptr	pp;
	XGCValues	gcv;
	Clientptr	cp = clist[ client ];
	Display		*dpy = cp->dpy;
	unsigned int	width = req->param.s[1],
			height = req->param.s[0];
	unsigned long	valuemask;
	int		x = req->param.s[2],
			y = req->param.s[3];
	Drawable	d = req->windowId;
	GC		gc;
	GetWinInfoOrSetBadDrawable(client, req->windowId, wp, return);

	if (!WindowMapRequested(wp->parent))
		return;
	cp->pendingFlags |= GraphicsPending;

	CheckTransparentDrawableAndXY(dpy, wp, &d, &x, &y);

	gc = wp->gc;
	valuemask = GCFunction|GCForeground|GCPlaneMask|GCFillStyle|GCTile;
	if (req->param.l[3]) {
		/*
		 * XXX note that this overrides the clip rectangles set by
		 * CheckTransparentDrawableAndXY() if 'd' was a transparent
		 * window.  The fix is to merge this with the clip
		 * rectangles.
		 */
		pp = BitmapLookup(client, (XID)req->param.l[3]);
		if (pp == NULL) {
			Xstatus = X10BadBitmap;
			return;
		}
		if (pp->width != width || pp->height != height) {
			Xstatus = X10BadMatch;
			return;
		}
		valuemask |= GCClipXOrigin|GCClipYOrigin|GCClipMask;
		gcv.clip_mask = req->param.l[3];
		gcv.clip_x_origin = x;
		gcv.clip_y_origin = y;
	}

	pp = PixmapLookup(client, (Pixmap)req->param.l[2]);
	if (pp == NULL) {
		Xstatus = X10BadPixmap;
		return;
	}

	gcv.tile = pp->id;
	gcv.fill_style = FillTiled;
	gcv.function = X10toX11function(req->func);
	gcv.plane_mask = req->mask;
	XChangeGC(dpy, gc, valuemask, &gcv);
	SetTileMode(dpy, wp, wp->tileModeRelative);

	XFillRectangle(dpy, d, gc, x, y, width, height);

	if (req->param.l[3]) {
		gcv.clip_mask = 0;
		XChangeGC(cp->dpy, gc, GCClipMask, &gcv);
	}
}

/*
 * XXX X10 pixmapsave could save any portion of the root window
 * including inferiors.  X11 explicitly prevents copyarea from
 * doing this.  The solution would be to
 *    - grab the server
 *    - querytree on the root
 *    - calculate obscured regions and what window is showing in that region
 *    - to a copy area for each... tough beans if the depth is different!
 */
Bool X10ProcPixmapSave(client, req, rep)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
{
	XGCValues	gcv;
	PixmapInfoptr	pp;
	WinInfoptr	next;
	Display		*dpy = clist[ client ]->dpy;
	Drawable	d = req->windowId;
	int		x = req->param.s[2],
			y = req->param.s[3];
	unsigned int	width = req->param.s[1],
			height = req->param.s[0];
	unsigned long	valuemask;
	GetWinInfoOrSetBadDrawable(client, d, wp, return (False));

	if (! wp->mapped) {
		Xstatus = X10BadWindow;
		return(False);
	}

	/*
	 * Normally, we would use CheckTransparentDrawableAndXY(), but
	 * PixmapSave explicitly copies the overlapping windows and
	 * subwindows.  Hence, we simply need to use the root, but with
	 * an adjusted x and y.
	 */
	for (next = wp; next; next = next->parent) {
		x += next->x;
		y += next->y;
	}
	d = DefaultRootWindow(dpy);
	pp = CreatePixmap(client,
		d,			/* drawable */
		width,			/* width */
		height,			/* height */
		(unsigned int) ScreenDepth,
		(PixmapInfoptr) NULL);
	if (pp == NULL) {
		Xstatus = X10BadAlloc;
		return(False);
	}

	valuemask = GCFunction|GCPlaneMask|GCGraphicsExposures;
	gcv.function = GXcopy;
	gcv.plane_mask = ~0;
	gcv.graphics_exposures = False;
	XChangeGC(dpy, pp->gc, valuemask, &gcv);
	XCopyArea(dpy, d, pp->id, pp->gc, x, y, width, height, 0, 0);

	rep->param.l[0] = pp->id;
	pp->x = x;
	pp->y = y;
	pp->isSavedPixmap = True;
	return(True);
}

Bool X10ProcCharBitmap(client, req, rep)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
{
	PixmapInfoptr	pp;
	Clientptr 	cp = clist[ client ];
	Display		*dpy = cp->dpy;
	char		glyph = req->param.s[2];
	XFontStruct	*fstruct;
	FontInfoptr	finfo;
	XCharStruct	extent;
	int		dir, junkAscent, junkDescent;

	if ((finfo = FontLookup(client, (Font) req->param.l[0])) == NULL) {
		Xstatus = X10BadFont;
		return(False);
	}
	fstruct = finfo->info;

	XTextExtents(fstruct,
		     &glyph,		/* string */
		     1,			/* number of chars */
		     &dir,		/* direction */
		     &junkAscent, &junkDescent,
		     &extent);		/* extent of the character */

	if (extent.ascent == 0
	 && extent.descent == 0
	 && extent.width == 0
	 && extent.lbearing == 0
	 && extent.rbearing == 0) {
		Xstatus = X10BadValue;
		return(False);
	}

	pp = CreatePixmap(client,
		DefaultRootWindow(dpy),				/* drawable */
		(unsigned int)fstruct->max_bounds.width,	/* width */
		(unsigned int)finfo->ascent + finfo->descent,	/* height */
		1,						/* depth */
		(PixmapInfoptr) NULL);			   /* bitmap pointer */
	if (pp == NULL) {
		Xstatus = X10BadAlloc;
		return(False);
	}

 	/*
	 * XXX We assume that x is 0.
	 * what if dir == 1?
	 */
	XDrawImageString(dpy,
			 pp->id,
			 cp->bitmapGC,
			 0,			/* x */
			 finfo->ascent,		/* y */
			 &glyph,
			 1);			/* number of glyphs */

	rep->param.l[0] = pp->id;
	return(True);
}
