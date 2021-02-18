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
 * $XConsortium: proto2.c,v 6.2 88/10/11 18:10:39 jim Exp $ 
 * $Locker:  $ 
 */
#include "resource.h"

Bool X10ProcStorePixmap(client, req, rep, pixmap)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
	char	*pixmap;
{
	PixmapInfoptr	pp;
	int	format;

	if (req->func == 0)
		format = XYPixmap;
	else if (req->func == 1)
		format = ZPixmap;
	else {
		Xstatus = X10BadValue;
		return(False);
	}

	pp = StorePixmapOrBitmap(
			client,
			(unsigned int)req->param.s[1],		/* width */
			(unsigned int)req->param.s[0],		/* height */
			(unsigned int)ScreenDepth,
			format,					/* format */
			pixmap);
	if (pp) {
		rep->param.l[0] = pp->id;
		return (True);
	}
	Xstatus = X10BadAlloc;
	return (False);
}

Bool X10ProcStoreBitmap(client, req, rep, bitmap)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
	char	*bitmap;
{
	PixmapInfoptr	pp;

	pp = StorePixmapOrBitmap(
			client,
			(unsigned int)req->param.s[1],	/* width */
			(unsigned int)req->param.s[0],	/* height */
			(unsigned int)1,		/* depth */
			XYBitmap,			/* format */
			bitmap);
	if (pp) {
		rep->param.l[0] = pp->id;
		return (True);
	}
	Xstatus = X10BadAlloc;
	return (False);
}

Bool X10ProcStoreCursor(client, req, rep)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
{
	PixmapInfoptr pcursor, pmask;
	Display *dpy = clist[ client ]->dpy;
#define foreground	(list+0)
#define background	(list+1)
	XColor	list[2];
	long	forepix = X10toX11pixel((unsigned long)req->param.u[2]),
		backpix = X10toX11pixel((unsigned long)req->param.u[3]);

	pcursor = BitmapLookup(client, (XID)req->param.l[0]);
	if (pcursor == NULL) {
		Xstatus = X10BadBitmap;
		return(False);
	}

	if (req->param.l[2]) {
		pmask = BitmapLookup(client, (XID)req->param.l[2]);
		if (pmask == NULL) {
			Xstatus = X10BadBitmap;
			return(False);
		}
	}
	else
		pmask = NULL;

	/*
	 * Translate pixel value into rgb value.
	 */
	foreground->pixel = forepix;
	background->pixel = backpix;
	XQueryColors(dpy,
		ScreenColormap,
		list,
		2);

	rep->param.l[0] = XCreatePixmapCursor(dpy,
		pcursor->id,
		pmask ? pmask->id : 0,
		foreground, background,
		(unsigned int) req->param.s[6],
		(unsigned int) req->param.s[7]);
	return(True);
#undef foreground
#undef background
}

void X10ProcFreeCursor(client, req)
	int	client;
	X10Req	*req;
{
	XFreeCursor(clist[ client ]->dpy, (Cursor) req->param.l[0]);
}

void X10ProcFreeBitmap(client, req)
	int	client;
	X10Req	*req;
{
	PixmapInfoptr pp;

	pp = BitmapLookup(client, (XID) req->param.l[0]);
	if (pp == NULL)
		Xstatus = X10BadBitmap;
	else
		FreePixmapInfo(client, pp);
}

void X10ProcFreePixmap(client, req)
	int	client;
	X10Req	*req;
{
	PixmapInfoptr pp;

	pp = PixmapLookup(client, (Pixmap) req->param.l[0]);
	if (pp == NULL)
		Xstatus = X10BadPixmap;
	else
		FreePixmapInfo(client, pp);
}

void X10ProcLine(client, req)
	int	client;
	X10Req	*req;
{
	Clientptr	cp = clist[ client ];
	Display		*dpy = cp->dpy;
	XGCValues	gcv;
	int		x1 = req->param.s[0],
			y1 = req->param.s[1],
			x2 = req->param.s[2],
			y2 = req->param.s[3],
			xdelta = 0,
			ydelta = 0;
	unsigned long	forground = req->param.u[4];
	GC		gc;
	Drawable	d = req->windowId;
	GetWinInfoOrSetBadDrawable(client, d, wp, return);

	if (!WindowMapRequested(wp->parent))
		return;
	cp->pendingFlags |= GraphicsPending;

	gc = wp->gc;

	gcv.fill_style = FillSolid;
	gcv.function = X10toX11function(req->func);
	gcv.foreground = X10toX11pixel(forground);
	gcv.plane_mask = req->mask;
	gcv.line_style = LineSolid;
	/*
	 * XXX brush width and height are colapsed into line width.
	 * This could be solved by looking at the slope of the line
	 * and deciding:
	 *	vertical:	X11 line width is X10 brush width
	 *	horizontal:	X11 line width is X10 brush height
	 *	0<slope<infite:	X11 line width is some function of
	 *			   X10 brush width and height
	 */
	gcv.line_width = max(req->param.b[10], req->param.b[11]);
	if (gcv.line_width == 1)
		gcv.line_width = 0;

	XChangeGC(dpy, gc,
		GCFunction|GCForeground|GCLineStyle
		|GCPlaneMask|GCLineWidth|GCFillStyle,
		&gcv);
	CheckTransparentDrawableAndXY(dpy, wp, &d, &xdelta, &ydelta);
	if (wp->transparent) {
		x1 += xdelta;
		y1 += ydelta;
		x2 += xdelta;
		y2 += ydelta;
	}
	XDrawLine(dpy, d, gc, x1, y1, x2, y2);
}

void X10ProcDraw(client, req, vlist)
	int	client;
	X10Req	*req;
	X10Vertex *vlist;
{
	Clientptr 	cp = clist[ client ];
	Display 	*dpy = cp->dpy;
	unsigned long	valuemask;
	XGCValues	gcv;
	int		npoints = req->param.s[0],
			i,
			xdelta = 0,
			ydelta = 0;
	GC		gc;
	Drawable	d = req->windowId;
	GetWinInfoOrSetBadDrawable(client, d, wp, return);

	if (!WindowMapRequested(wp->parent))
		return;
	cp->pendingFlags |= GraphicsPending;

	gc = wp->gc;

	valuemask = GCFunction|GCForeground|GCBackground|GCLineStyle
		|GCPlaneMask|GCLineWidth|GCFillStyle;
	gcv.function = X10toX11function(req->func);
	gcv.foreground = X10toX11pixel((unsigned long)req->param.u[1]);
	gcv.background = X10toX11pixel((unsigned long)req->param.u[4]);
	gcv.fill_style = FillSolid;
	gcv.plane_mask = req->mask;
	/*
	 * XXX brush width and height are colapsed into line width.
	 * This could be solved by looking at the slope of the line
	 * and deciding:
	 *	vertical:	X11 line width is X10 brush width
	 *	horizontal:	X11 line width is X10 brush height
	 *	0<slope<infite:	X11 line width is some function of
	 *			   X10 brush width and height
	 */
	gcv.line_width = max(req->param.b[4], req->param.b[5]);
	if (gcv.line_width == 1)
		gcv.line_width = 0;
	switch(req->param.s[3]) {
	case 1:
	case 2:
	{
		char	dash_list[16];
		short	patstring = req->param.s[5];
		short	patlength = req->param.s[6];
		short	patmul = req->param.s[7];
		int	state, n;

		if (patmul <= 0 || patlength <= 0 || patlength > 16) {
			Xstatus = X10BadValue;
			return;
		}
		if (req->param.s[3] == 1)
			gcv.line_style = LineOnOffDash;
		else
			gcv.line_style = LineDoubleDash;

		
		bzero((char *)dash_list, 16);
		state = patstring & 1;
		for (i=0, n=0; i<patlength; i++) {
			if ((patstring & (1<<i)) != state)
				n++;
			dash_list[n] += patmul;
		}
		XSetDashes(dpy, gc, 0, dash_list, n);
		break;
	}
	case 0:
		gcv.line_style = LineSolid;
		break;
	}

	XChangeGC(dpy, gc, valuemask, &gcv);

	CheckTransparentDrawableAndXY(dpy, wp, &d, &xdelta, &ydelta);
	if (xdelta || ydelta) {
		AdjustNonRelativeX10Points(vlist, npoints, xdelta, ydelta);
		debug(X10_Draw, ("\n   delta xy = (%d,%d)", xdelta, ydelta));
	}
	if (debugged(X10_Draw))
		ShowX10Line(npoints, vlist);
	if (! XDraw(dpy, d, gc, vlist, npoints))
		Xstatus = X10BadValue;
}

void X10ProcDrawFilled(client, req, vlist)
	int	client;
	X10Req	*req;
	X10Vertex *vlist;
{
	PixmapInfoptr	pp;
	Display		*dpy = clist[ client ]->dpy;
	int		npoints = req->param.s[0],
			xdelta = 0,
			ydelta = 0;
	Pixmap		tilePixmap = req->param.l[1];
	unsigned long	valuemask;
	XGCValues	gcv;
	GC		gc;
	Drawable	d = req->windowId;
	GetWinInfoOrSetBadDrawable(client, d, wp, return);

	if (!WindowMapRequested(wp->parent))
		return;
	clist[ client ]->pendingFlags |= GraphicsPending;

	gc = wp->gc;
	
	valuemask = GCFunction
		   |GCBackground
		   |GCPlaneMask
		   |GCLineWidth
		   |GCFillStyle;

	if (tilePixmap) {
		pp = PixmapLookup(client, (Pixmap) req->param.l[1]);
		if (pp == NULL) {
			Xstatus = X10BadPixmap;
			return;
		}
		gcv.tile = pp->id;
		gcv.fill_style = FillTiled;
		valuemask |= GCTile;
	} else {
		gcv.fill_style = FillSolid;
		valuemask |= GCForeground;
		gcv.foreground = X10toX11pixel((unsigned long) req->param.u[1]);
	}

	gcv.function = X10toX11function(req->func);
	gcv.background = gcv.foreground;
	gcv.plane_mask = req->mask;

	/*
	 * XXX brush width and height are colapsed into line width.
	 * This could be solved by looking at the slope of the line
	 * and deciding:
	 *	vertical:	X11 line width is X10 brush width
	 *	horizontal:	X11 line width is X10 brush height
	 *	0<slope<infite:	X11 line width is some function of
	 *			   X10 brush width and height
	 */
	gcv.line_width = max(req->param.b[4], req->param.b[5]);

	XChangeGC(dpy, gc, valuemask, &gcv);
	if (tilePixmap)
		SetTileMode(dpy, wp, wp->tileModeRelative);

	CheckTransparentDrawableAndXY(dpy, wp, &d, &xdelta, &ydelta);
	if (xdelta || ydelta) {
		AdjustNonRelativeX10Points(vlist, npoints, xdelta, ydelta);
		debug(X10_DrawFilled,
			("\n   delta xy = (%d,%d)", xdelta, ydelta));
	}
	if (debugged(X10_DrawFilled))
		ShowX10Line(npoints, vlist);
	if (! XDrawFilled(dpy, d, gc, vlist, npoints))
		Xstatus = X10BadValue;
}

void X10ProcDestroyWindow(client, req)
	int	client;
	X10Req	*req;
{
	Clientptr cp = clist[ client ];
	Display *dpy = cp->dpy;
	GetWinInfoOrSetBadWindow(client, req->windowId, wp, return);

	if (WindowMapped(wp))
		cp->pendingFlags |= ChangesPending;
	/*
	 * Note that we cannot free the window info yet because other
	 * events comming in may refer to this window.  Hence, we
	 * must be able to look it up.  As soon as we receive the
	 * DestroyNotify, we can clean up.
	 */

	XDestroyWindow(dpy, req->windowId);
}

void X10ProcDestroySubwindows(client, req)
	int	client;
	X10Req	*req;
{
	Clientptr cp = clist[ client ];
	GetWinInfoOrSetBadWindow(client, req->windowId, wp, return);

	if (WindowMapped(wp))
		cp->pendingFlags |= ChangesPending;

	/* see comment above in X10ProcDestroyWindow */
	XDestroySubwindows(cp->dpy, req->windowId);
}

void X10ProcKeyClick(client, req)
	int	client;
	X10Req	*req;
{
	XKeyboardControl	kc;

	kc.key_click_percent = (req->func * 100) / 8;
	XChangeKeyboardControl(clist[ client ]->dpy, KBKeyClickPercent, &kc);
}

void X10ProcAutoRepeat(client, req)
	int	client;
	X10Req	*req;
{
	XKeyboardControl	kc;

	kc.auto_repeat_mode = req->func;
	XChangeKeyboardControl(clist[ client ]->dpy, KBAutoRepeatMode, &kc);
}

void X10ProcScreenSaver(client, req)
	int	client;
	X10Req	*req;
{
	XSetScreenSaver(clist[ client ]->dpy,
		req->param.s[0], /* timeout */
		req->param.s[0], /* interval */
		req->func == 0 ? PreferBlanking : DontPreferBlanking,
		DefaultExposures);
}

void X10ProcStoreBytes(client, req, string)
	int	client;
	X10Req	*req;
	char	*string;
{
	int	length = req->param.s[0];
	int	which = req->func;

	if (which < 0 || which > 7) {
		Xstatus = X10BadValue;
		return;
	}
	XStoreBuffer(clist[ client ]->dpy, string, length, which);
}

void X10ProcAppendBytes(client, req, string)
	int	client;
	X10Req	*req;
	char	*string;
{
	int	length = req->param.s[0],
		currentLength,
		newLength;
	int	which = req->func;
	char	*currentBuffer;
	Display	*dpy = clist[ client ]->dpy;

	if (which < 0 || which > 7) {
		Xstatus = X10BadValue;
		return;
	}
	
	currentBuffer = XFetchBuffer(dpy, &currentLength, which);
	newLength = length + currentLength;
	if (currentBuffer)
		currentBuffer = Xrealloc(currentBuffer, newLength + 1);
	else
		currentBuffer = Xalloc(newLength + 1);
	strncpy(currentBuffer + currentLength, string, length);

	XStoreBytes(dpy, currentBuffer, length);
	free(currentBuffer);
}

int X10ProcFetchBytes(client, req, string)
	int	client;
	X10Req	*req;
	char	**string;
{
	int	currentLength,
		which = req->func;
	char	*currentBuffer;

	if (which < 0 || which > 7) {
		Xstatus = X10BadValue;
		return(0);
	}
	
	currentBuffer = XFetchBuffer(clist[ client ]->dpy,
				     &currentLength, which);

	*string = StaticAlloc(currentLength+1);
	if (currentBuffer) {
		bcopy(currentBuffer, *string, currentLength);
		free(currentBuffer);
	}
	return(currentLength);
}

void X10ProcRotateCuts(client, req)
	int	client;
	X10Req	*req;
{
	int	count = req->func;

	if (count < 0 || count > 7) {
		Xstatus = X10BadValue;
		return;
	}
	
	XRotateBuffers(clist[ client ]->dpy, count);
}
