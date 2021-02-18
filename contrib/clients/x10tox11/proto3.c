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
 * $XConsortium: proto3.c,v 6.4 89/12/07 20:03:12 kit Exp $ 
 * $Locker:  $ 
 */
#include "resource.h"
#include "X11/Xutil.h"

void X10ProcRaiseWindow(client, req)
	int	client;
	X10Req	*req;
{
	GetWinInfoOrSetBadWindow(client, req->windowId, wp, return);

	if (WindowMapped(wp))
		clist[ client ]->pendingFlags |= ChangesPending;
	XRaiseWindow(clist[ client ]->dpy, req->windowId);
	RaiseWindow(wp);
}

void X10ProcLowerWindow(client, req)
	int	client;
	X10Req	*req;
{
	GetWinInfoOrSetBadWindow(client, req->windowId, wp, return);

	if (WindowMapped(wp))
		clist[ client ]->pendingFlags |= ChangesPending;
	XLowerWindow(clist[ client ]->dpy, req->windowId);
	LowerWindow(wp);
}

void X10ProcCircWindow(client, req, up)
	int	client;
	X10Req	*req;
	int	up;
{
	GetWinInfoOrSetBadWindow(client, req->windowId, wp, return);

	if (WindowMapped(wp))
		clist[ client ]->pendingFlags |= ChangesPending;
	XCirculateSubwindows(clist[ client ]->dpy, req->windowId,
		up ? RaiseLowest : LowerHighest);
	if (up)
		RaiseLowestChild(wp);
	else
		LowerHighestChild(wp);
}

void X10ProcMoveWindow(client, req)
	int	client;
	X10Req	*req;
{
	GetWinInfoOrSetBadWindow(client, req->windowId, wp, return);

	if (WindowMapped(wp))
		clist[ client ]->pendingFlags |= ChangesPending;
	wp->x = req->param.s[0];
	wp->y = req->param.s[1];

	XMoveWindow(clist[ client ]->dpy, req->windowId,
		req->param.s[0], req->param.s[1]);
}

void X10ProcChangeWindow(client, req)
	int	client;
	X10Req	*req;
{
	Display		*dpy = clist[ client ]->dpy;
	GetWinInfoOrSetBadWindow(client, req->windowId, wp, return);

	if (WindowMapped(wp))
		clist[ client ]->pendingFlags |= ChangesPending;

	AddRequestSerial(&wp->configureReq, client, NextRequest(dpy));

	wp->width = req->param.s[1];
	wp->height = req->param.s[0];

	XResizeWindow(dpy,
		req->windowId,
		(unsigned int) req->param.s[1],
		(unsigned int) req->param.s[0]);
}

void X10ProcConfigureWindow(client, req)
	int	client;
	X10Req	*req;
{
	Display		*dpy = clist[ client ]->dpy;
	GetWinInfoOrSetBadWindow(client, req->windowId, wp, return);

	if (WindowMapped(wp))
		clist[ client ]->pendingFlags |= ChangesPending;

	AddRequestSerial(&wp->configureReq, client, NextRequest(dpy));

	wp->width = req->param.s[1];
	wp->height = req->param.s[0];
	wp->x = req->param.s[2];
	wp->y = req->param.s[3];

	XMoveResizeWindow(dpy,
		req->windowId,
		req->param.s[2],		/* x */
		req->param.s[3],		/* y */
		(unsigned int) req->param.s[1],	/* width */
		(unsigned int) req->param.s[0]);/* height */
}

Bool X10ProcQueryColor(client, req, rep)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
{
	Display		*dpy = clist[ client ]->dpy;
	XColor	pixel;

	/*
	 * Translate pixel value into rgb value.
	 */
	pixel.pixel = (unsigned long) req->param.u[0];

#ifdef notdef
	/*
	 * Just for the heck of it... and to keep old, stupid clients
	 * from breaking, we'll mask off the high order bits that don't
	 * count.
	 */
	pixel.pixel &= ((1<<ScreenDepth) - 1);
#endif

	XQueryColors(dpy,
		ScreenColormap,
		&pixel,
		1);

	rep->param.u[0] = pixel.red;
	rep->param.u[1] = pixel.green;
	rep->param.u[2] = pixel.blue;
	return (True);
}

Bool X10ProcLookupColor(client, req, rep, name)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
	char	*name;
{
	Display		*dpy = clist[ client ]->dpy;
	XColor	def, scr;
	int	result, nameLength = req->param.s[0];
	char	*buffer;

	buffer = StaticAlloc(nameLength + 1);
	bcopy(name, buffer, nameLength);
	buffer[ nameLength ] = '\0';

	debug(X10_LookupColor, ("(%s) ", buffer));
	/*
	 * Translate pixel value into rgb value.
	 */
	result = XLookupColor(dpy,
		ScreenColormap,
		buffer, &def, &scr);
	if (result == 0) {
		Xstatus = X10BadColor;
		return(False);
	}
	rep->param.u[0] = def.red;
	rep->param.u[1] = def.green;
	rep->param.u[2] = def.blue;
	rep->param.u[3] = scr.red;
	rep->param.u[4] = scr.green;
	rep->param.u[5] = scr.blue;
	return (True);
}

Bool X10ProcGetColor(client, req, rep)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
{
	XColor	color;
	Display	*dpy = clist[ client ]->dpy;
	Status	status;

	color.red = req->param.u[0];
	color.green = req->param.u[1];
	color.blue = req->param.u[2];

	status = XAllocColor(dpy,
			     ScreenColormap,
			     &color);

	rep->param.u[0] = color.pixel;
	if (status == 0) {
		Xstatus = X10BadAlloc;
		return(False);
	}
	return(True);
}

Bool X10ProcGetColorCells(client, req, rep, x10pixels)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
	short	**x10pixels;
{
	Display		*dpy = clist[ client ]->dpy;
	unsigned int	ncolors = req->param.s[0],
			nplanes = req->param.s[1];
	unsigned long	*pixels,
			*planemasks,
			planemask;
	int		successful, i, class;
	Bool		contiguous = (req->func != 0);

	/*
	 * XXX For this X10 request to succeed, the x11 visual class of
	 * this machine must be PseudoColor or GrayScale, because that
	 * is the model that X10 supported.  DirectColor is close, because
	 * it, too, has an allocatable colormap, but the bits may not
	 * be contiguous.  We allow it to see if it will fail on someone's
	 * server...
	 */
	class = ScreenVisual->class;
	switch(class) {
	case PseudoColor:
	case GrayScale:
		break;
	case DirectColor:
		/*
		 * DirectColor won't return us exactly one bit for each of
		 * the rgb.  So it just won't work here.
		 */
		if (!contiguous)
			break;

		/* Fall thru... */
	case StaticGray:
	case StaticColor:
	case TrueColor:
		debug(X10_GetColorCells,
			("\nvisual class=%s... can't do a GetColorCells%s\n",
			VisualClassNames[ class ],
			class == DirectColor && contiguous ?
				" with \"contiguous\" true" : ""));
		Xstatus = X10BadAlloc;
		return(False);
	}

	/*
	 * this said, "Allocate all the cells".
	 */
	if (ncolors == 0)
		ncolors = ScreenColorCells;
	
	/*
	 * Allocate space for the returned X10 return value (the list
	 * of pixels, the X11 returned pixels and the X11 return masks.
	 */
	pixels = (unsigned long *)StaticAlloc(
		(int)(sizeof(long)*(ncolors + nplanes)
			+ sizeof(short)*ncolors));
	planemasks = pixels + ncolors;
	*x10pixels = (short *)(planemasks + nplanes);

	successful = XAllocColorCells(dpy, 
		ScreenColormap,
		contiguous,
		planemasks,
		nplanes,
		pixels,
		ncolors);
	if (!successful) {
		Xstatus = X10BadAlloc;
		return(False);
	}


	/*
	 * the masks we get back are exactly one bit apiece (PseudoColor
	 * and GrayScale), and by or'ing them all we get the ``planemask''
	 * that X10 knows.
	 */
	for (i=planemask=0; i<nplanes; i++)
		planemask |= planemasks[ i ];
	for (i=0; i<ncolors; i++)
		(*x10pixels)[i] = pixels[i];
	rep->param.u[0] = planemask;
	return(True);
}

void X10ProcFreeColors(client, req, x10pixels)
	int	client;
	X10Req	*req;
	short	*x10pixels;
{
	Display		*dpy = clist[ client ]->dpy;
	unsigned long	*pixels;
	int		npixels = req->param.s[0],
			i;

	pixels = (unsigned long *)StaticAlloc(sizeof(long)*npixels);
	for (i=0; i<npixels; i++)
		pixels[i] = (unsigned long)x10pixels[i];
	XFreeColors(dpy,
		ScreenColormap,
		pixels,
		npixels,
		(unsigned long) req->mask);
}

void X10ProcStoreColors(client, req, x10colors)
	int	client;
	X10Req	*req;
	X10ColorDef *x10colors;
{
	Display	*dpy = clist[ client ]->dpy;
	int	ncolors = req->param.s[0];
	int	i;
	XColor	*colors;

	colors = (XColor *)StaticAlloc(sizeof(XColor)*ncolors);
	for (i=0; i<ncolors; i++) {
		colors[i].pixel = (unsigned long)x10colors[i].pixel;
		colors[i].red = x10colors[i].red;
		colors[i].green = x10colors[i].green;
		colors[i].blue = x10colors[i].blue;
		colors[i].flags = DoRed|DoGreen|DoBlue;
	}
	XStoreColors(dpy, 
		ScreenColormap,
		colors,
		ncolors);
}

void X10ProcMouseControl(client, req)
	int	client;
	X10Req	*req;
{
	clist[ client ]->pendingFlags |= ChangesPending;

	XChangePointerControl(clist[ client ]->dpy,
		True, True, req->param.s[0], 1, req->param.s[1]);
}


void X10ProcFeepControl(client, req)
	int	client;
	X10Req	*req;
{
	XKeyboardControl	kc;

	clist[ client ]->pendingFlags |= ChangesPending;

	kc.bell_percent = (req->func * 100) / 7;
	XChangeKeyboardControl(clist[ client ]->dpy, KBBellPercent, &kc);
}

void X10ProcFeep(client, req)
	int	client;
	X10Req	*req;
{
	int volume = (int)((req->func * 100) / 7);

	clist[ client ]->pendingFlags |= ChangesPending;

	if (volume < 0) volume = 0;
	if (volume > 100) volume = 100;

	XBell(clist[ client ]->dpy, volume);
}

/*ARGSUSED*/
void X10ProcShiftLock(client, req)
	int	client;
	X10Req	*req;
{
	switch (req->func) {
	case 0:	/* LockUpDownMode */
		LockToggleMode = False;
		break;
	case 1:	/* LockToggleMode */
		LockToggleMode = True;
		break;
	default:
		Xstatus = X10BadValue;
		break;
	}
}

void X10ProcQueryShape(client, req, rep)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
{
	Display	*dpy = clist[ client ]->dpy;
	unsigned int	width, height, reqWidth, reqHeight;
	int		class;

	/*
	 * Values already checked for != 0
	 */
	reqHeight = req->param.s[0];
	reqWidth = req->param.s[1];

	switch (req->func) {
	case 0:
		class = CursorShape;
		break;
	case 1:
		class = TileShape;
		if (MinimumTileSize) {
			if (reqHeight < MinimumTileSize)
				reqHeight = MinimumTileSize;
			if (reqWidth < MinimumTileSize)
				reqWidth = MinimumTileSize;
		}
		/*
		 * The code used to call XQueryBestSize(), but in X11,
		 * this returns the most optimal size, whereas in X10,
		 * QueryShape returned to size supported on the display.
		 */
		width = reqWidth;
		height = reqHeight;
		goto done;
		break;
	case 2: /* brush shape */
		rep->param.s[0] = rep->param.s[0] = 1;
		return;
	}

	(void) XQueryBestSize(dpy,
		class,
		DefaultRootWindow(dpy),
		reqWidth, reqHeight,
		&width, &height);

done:
	rep->param.s[1] = width;
	rep->param.s[0] = height;
}

void X10ProcChangeBackground(client, req)
	int	client;
	X10Req	*req;
{
	Pixmap	p = req->param.l[0];
	Window	w = req->windowId;
	GetWinInfoOrSetBadWindow(client, w, wp, return);

	if (wp->transparent) {
		Xstatus = X10BadWindow;
		return;
	}

	if (WindowMapped(wp))
		clist[ client ]->pendingFlags |= ChangesPending;
	/*
	 * XXX Note that if the X10 TileMode is TileModeRelative, we
	 * loose that distiction here, because X11 does not support
	 * a window background of ParentRelative plus having a uniq
	 * pixmap.
	 */
	if (p == 0)
		p = ParentRelative;
	else {
		PixmapInfoptr	pp = PixmapLookup(client, p);

		if (pp == NULL) {
			Xstatus = X10BadPixmap;
			return;
		}
	}
	wp->tile = p;

	XSetWindowBackgroundPixmap(clist[ client ]->dpy, w, p);
}

void X10ProcChangeBorder(client, req)
	int	client;
	X10Req	*req;
{
	Pixmap	p = req->param.l[0];
	PixmapInfoptr	pp = NULL;
	GetWinInfoOrSetBadWindow(client, req->windowId, wp, return);

	if (p)
		pp = PixmapLookup(client, p);
	if (pp == NULL) {
		Xstatus = X10BadPixmap;
		return;
	}
	if (WindowMapped(wp))
		clist[ client ]->pendingFlags |= ChangesPending;

	XSetWindowBorderPixmap(clist[ client ]->dpy, req->windowId, p);
}

void X10ProcTileMode(client, req)
	int	client;
	X10Req	*req;
{
	Display	*dpy = clist[ client ]->dpy;
	XSetWindowAttributes	swa;
	GetWinInfoOrSetBadDrawable(client, req->windowId, wp, return);

	if (WindowMapped(wp))
		clist[ client ]->pendingFlags |= ChangesPending;

	/*
	 * XXX In X10, there were four combinations of tiles and modes
	 * that were possible:
	 *	Tile		Mode			X11
	 *	----		----			---
	 *	use parent's	Absolute		can't
	 *	use parent's	Relative to parent	can
	 *	uniqe tile	Absolute		can
	 *	uniqe tile	Relative to parent	can't
	 * In X11, we really can't do a complete job for the window
	 * background, but we can for X10 StippleFill and DrawFilled
	 * requests
	 */
	switch (req->func) {
	case 0: /* TileModeAbsolute */
		wp->tileModeRelative = False;

		if (wp->tile == ParentRelative) {
			if (wp->parent->tile == ParentRelative) {
				Notice("Can't set tile mode to absolute!\n");
				break;
			} else
				wp->tile = wp->parent->tile;
		}
		swa.background_pixmap = wp->tile;
		XChangeWindowAttributes(dpy, req->windowId, CWBackPixmap, &swa);
		break;
		
	case 1: /* TileModeRelative */
		/*
		 * XXX For window-background tiles: We could look up
		 * the parent here
		 * and see if the tile was different than the childs...
		 * but then we would have to copy the tile into another
		 * tile changing the origin.  This is too much trouble for
		 * the value right now.  If the parent is the root, we can
		 * never be sure if we know what its tile is, because we can't
		 * query the tile.
		 * So for now, either the tile is already ParentRelative
		 * (so we do nothing), or the tile is unique (which we 
		 * can't handle, so we still do nothing.
		 */
		wp->tileModeRelative = True;
		break;
		
	default:
		Xstatus = X10BadValue;
		return;
	}
}

void X10ProcClipMode(client, req)
	int	client;
	X10Req	*req;
{
	GetWinInfoOrSetBadDrawable(client, req->windowId, wp, return);

	if (req->func == 0)
		SetClipMode(client, wp, False);
	else if (req->func == 1)
		SetClipMode(client, wp, True);
	else {
		Xstatus = X10BadValue;
		return;
	}
}

void X10ProcSetIconWindow(client, req)
	int	client;
	X10Req	*req;
{
	Display	*dpy = clist[ client ]->dpy;
	XWMHints hints, *phints;
	Window	icon = req->param.l[0];
	WinInfoptr	iconwp;
	GetWinInfoOrSetBadWindow(client, req->windowId, wp, return);

	if ((phints = XGetWMHints(dpy, wp->id)) == NULL) {
		phints = &hints;
		bzero((char *)phints, sizeof(XWMHints));
		phints->initial_state = NormalState;
		phints->flags = StateHint;
	}

	if (icon) {
		iconwp = WinLookup(client, icon);
		if (iconwp == NULL) {
			Xstatus = X10BadWindow;
			return;
		}
		if (iconwp->transparent || wp->transparent /* must be opaque */
		 || wp->parent != iconwp->parent	/* must be siblings */
		 || wp->isIcon || iconwp->isIcon) {	/* can't be icon */
			Xstatus = X10BadMatch;
			return;
		}
		iconwp->isIcon = True;
		iconwp->icon = wp->id;
		wp->icon = iconwp->id;
		phints->flags |= IconPositionHint;
		phints->icon_x = iconwp->x;
		phints->icon_y = iconwp->y;
	} else {
		if (wp->icon) {
			iconwp = WinLookup(client, wp->icon);
			if (iconwp) {
				iconwp->icon = 0;
				iconwp->isIcon = False;
			}
		}
		wp->icon = 0;
		phints->flags &= ~IconPositionHint;
		phints->icon_x = 0;
		phints->icon_y = 0;
	}
	
	phints->flags |= IconWindowHint;
	phints->icon_window = wp->icon;

	XSetWMHints(dpy, wp->id, phints);
}

Bool X10ProcInterpretLocator(client, x, y, req, rep)
	int	client, x, y;
	X10Req	*req;
	X10Rep	*rep;
{
	Display	*dpy = clist[ client ]->dpy;
	Window	child;
	int	rel_x, rel_y, result;

	result = XTranslateCoordinates(dpy,
		DefaultRootWindow(dpy),
		req->windowId,
		x, y,
		&rel_x, &rel_y,
		&child);
	if (! result)
		Xstatus = X10BadWindow;
	rep->param.l[0] = child;
	rep->param.s[2] = rel_x;
	rep->param.s[3] = rel_y;
	return(result);
}

void X10ProcWarpMouse(client, req)
	int	client;
	X10Req	*req;
{
	clist[ client ]->pendingFlags |= ChangesPending;

	XWarpPointer(clist[ client ]->dpy,
		(Window)req->param.l[1],	/* src window */
		(Window)req->windowId,		/* dest window */
		req->param.s[6],		/* src x */
		req->param.s[7],		/* src y */
		(unsigned int)req->param.s[5],	/* src width */
		(unsigned int)req->param.s[4],	/* src height */
		req->param.s[0],		/* dest x */
		req->param.s[1]);		/* dest y */
}

void X10ProcFocusKeyboard(client, req)
	int	client;
	X10Req	*req;
{
	if (FocusKeyboardAllowed) {
		clist[ client ]->pendingFlags |= ChangesPending;
		XSetInputFocus(clist[ client ]->dpy,
			req->windowId,
			(int)PointerRoot,
			CurrentTime);
	}
}

Bool X10ProcQueryTree(client, req, rep, nchildren, children)
	int		client;
	X10Req		*req;
	X10Rep		*rep;
	unsigned int	*nchildren;
	Window		**children;
{
	Window	root, parent, win = req->windowId;
	int	successful;
	GetWinInfoOrSetBadWindow(client, win, wp, return(False));

	successful = XQueryTree(clist[ client ]->dpy,
		win,
		&root,
		&parent,
		children,
		nchildren);
	if (! successful) {
		Xstatus = X10BadWindow;
		return(False);
	}
	UpdateWinTree(client, wp, *nchildren, *children);
	rep->param.l[0] = parent;
	rep->param.l[1] = (long) *nchildren;
	return(True);
}

void X10ProcCopyArea(client, req)
	int	client;
	X10Req	*req;
{
	Clientptr	cp = clist[ client ];
	GC		gc;
	Display		*dpy = cp->dpy;
	XGCValues	gcv;
	int		destx = req->param.s[6],
			desty = req->param.s[7],
			srcx = req->param.s[2],
			srcy = req->param.s[3];
	Drawable	d = req->windowId;
	GetWinInfoOrSetBadDrawable(client, d, wp, return);

	if (WindowMapped(wp))
		cp->pendingFlags |= ChangesPending; /* this generates events */

	gc = wp->gc;

	gcv.function = X10toX11function(req->func);
	gcv.plane_mask = req->mask;
	gcv.graphics_exposures = True;
	XChangeGC(dpy, gc, GCFunction|GCPlaneMask|GCGraphicsExposures, &gcv);

	CheckTransparentDrawableAndXY(dpy, wp, &d, &destx, &desty);
	if (wp->transparent) {
		srcx += destx - req->param.s[6];
		srcy += desty - req->param.s[7];
	}

	XCopyArea(dpy,
		d,				/* source drawable */
		d,				/* dest drawable */
		gc,				/* gc */
		srcx, srcy,
		(unsigned int) req->param.s[1],	/* width */
		(unsigned int) req->param.s[0],	/* height */
		destx, desty);
}
