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
 * $XConsortium: proto1.c,v 6.2 88/10/11 18:10:37 jim Exp $ 
 * $Locker:  $ 
 */
#include "resource.h"
#include "X11/Xatom.h"
#include "X11/Xutil.h"

void X10ProcSetUp(client, rep)
	int	client;
	X10Rep	*rep;
{
	Display *dpy = clist[ client ]->dpy;

	rep->param.l[0] = DefaultRootWindow(dpy);
	rep->param.s[2] = X10_PROTOCOL;
	rep->param.s[3] = XDEV_X10TRANSLATOR;
	rep->param.s[4] = ScreenDepth;
	rep->param.u[5] = ScreenColorCells;
	/*
	 * This is also used as a synchronization mechanism in
	 * X10... so we do the same here.
	 */
	SyncWithServer(dpy, False);
}

Bool X10ProcQueryWindow(client, req, rep)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
{
	XWindowAttributes	att;
	Clientptr cp = clist[ client ];
	Display *dpy = cp->dpy;
	unsigned long	x10mask, x11mask;
	Window	w = req->windowId;
	GetWinInfoOrSetBadWindow(client, w, wp, return(False));

	if (! XGetWindowAttributes(dpy, w, &att)) {
		Xstatus = X10BadWindow;
		return(False);
	}
	x11mask = att.your_event_mask;
	x10mask = X11toX10EventMask(x11mask);
	if (debugged(X10_QueryWindow))
		ShowEventMask(x10mask, x11mask);
	rep->param.l[4] = wp->mask        = x10mask;
	rep->param.s[0] = wp->height	  = att.height;
	rep->param.s[1] = wp->width	  = att.width;
	rep->param.s[2] = wp->x		  = att.x;
	rep->param.s[3] = wp->y		  = att.y;
	rep->param.s[4] = wp->borderWidth = att.border_width;

	switch(att.map_state) {
	case IsUnmapped:
		rep->param.b[10] = 0;	/* IsUnmapped */
		wp->mapped = False;
		break;
	case IsUnviewable:
		rep->param.b[10] = 2;	/* IsInvisible */
		wp->mapped = True;
		break;
	case IsViewable:
		rep->param.b[10] = 1;	/* IsMapped */
		wp->mapped = True;
		break;
	}

	switch(att.class) {
	case InputOutput:
		wp->transparent = False;
		if (wp->isIcon)
			rep->param.b[11] = 2;	/* IsIcon */
		else
			rep->param.b[11] = 1;	/* IsOpaque */
		rep->param.l[3] = wp->icon;
		break;
	case InputOnly:
		wp->transparent = True;
		rep->param.b[11] = 0;	/* IsTransparent */
		rep->param.l[3] = 0;
		break;
	}

	return(True);
}

Bool X10ProcCreateWindow(client, req, rep)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
{
	Visual		*visual;
	XSetWindowAttributes xswa;
	Clientptr	cp = clist[ client ];
	Display		*dpy = cp->dpy;
	u_long		valuemask;
	XID		win;
	int		depth;
	unsigned int	width = req->param.s[1],
			height = req->param.s[0],
			border_width,
			class = (req->code == X10_CreateTransparency)
				? InputOnly : InputOutput;
	WinInfoptr	newp;
	GetWinInfoOrSetBadWindow(client, req->windowId, wp, return(False));

	if (width == 0 || height == 0) {
		Xstatus = X10BadValue;
		return(False);
	}

	/*
	 * We roll our own window prompting.
	 */
	xswa.override_redirect = True;
	valuemask = CWOverrideRedirect;

	/*
	 * Be sure to select events here, because backing store may produce an
	 * exposure event when its created.... not that the X10 client will
	 * be selecting it at this point, but I don't want to say that I
	 * dropped it.
	 */
	xswa.event_mask = BasicEventMask;
	valuemask |= CWEventMask;

	if (class == InputOnly) {
		border_width = 0;
		depth = 0;
		visual = DefaultVisual(dpy, DefaultScreen(dpy));
	} else {
		/*
		 * X10 backgrounds are always either inheirited from the
		 * parent or an explicit pixmap.
		 */
		valuemask |= CWBackPixmap;
		if (req->param.l[3])
			xswa.background_pixmap = req->param.l[3];
		else
			xswa.background_pixmap = wp->tile;

		/*
		 * XXX X10 spec does not say what the border_pixmap
		 * should be of it is specified as 0 and the
		 * border width is > 0.  Assume CopyFromParent (default).
		 */
		if (req->param.l[2]) {
			valuemask |= CWBorderPixmap;
			xswa.border_pixmap = req->param.l[2];
		}

		border_width = req->func;
		depth = ScreenDepth;

		/*
		 * We explicitly provide the colormap, because we may have
		 * named a visual different than our ancestor's.
		 */
		xswa.colormap = ScreenColormap;
		valuemask |= CWColormap;
		visual = ScreenVisual;
	}

	win = XCreateWindow(dpy,
		req->windowId,				/* parent */
		req->param.s[2],			/* x */
		req->param.s[3],			/* y */
		width,					/* width */
		height,					/* height */
		border_width,				/* border width */
		depth,					/* depth */
		class,					/* class */
		visual,					/* visual */
		valuemask,				/* flags */
		&xswa);					/* attributes */
	rep->param.l[0] = win;
	/* no errors detectable! XXX */

	newp = AddWin(client, win, wp, class == InputOnly);
	if (newp == NULL) {
		XDestroyWindow(dpy, win);
		Xstatus = X10BadAlloc;
		return(False);
	}

	newp->x = req->param.s[2];
	newp->y = req->param.s[3];
	newp->height = height;
	newp->width = width;
	newp->tile = xswa.background_pixmap;
	newp->borderWidth = border_width;

	/*
	 * Only set hints on children of the root.
	 */
	if (wp->id == DefaultRootWindow(dpy)) {
		static char *title[2] = { "Unnamed X10 client", 0 };
		XSizeHints	sh;

		sh.x = req->param.s[2];
		sh.y = req->param.s[3];
		sh.height = height;
		sh.width = width;
		sh.min_height = 0;
		sh.height_inc = 1;
		sh.min_width = 0;
		sh.width_inc = 1;

		sh.flags = (USSize | USPosition | PPosition | PSize
				| PMinSize | PResizeInc);
		XSetStandardProperties (dpy, win, title[0], title[0], None,
			title, 1, &sh);
	}

	/*
	 * Tile mode of Transparent window is TileModeRelative.
	 */
	if (class == InputOnly)
		wp->tileModeRelative = True;

	return (True);
}

void X10ProcMapWindow(client, req)
	int	client;
	X10Req	*req;
{
	Clientptr	cp = clist[ client ];
	Display		*dpy = cp->dpy;
	Window		w = req->windowId;
	GetWinInfoOrSetBadWindow(client, w, wp, return);

	if (WindowMapped(wp->parent))
		clist[ client ]->pendingFlags |= ChangesPending;
	/*
	 * Make sure it is on top of the stack.  We do this separately
	 * because NextRequest(dpy) must return the serial of the
	 * MapWindow request.
	 */
	XRaiseWindow (dpy, w);

	/*
	 * Just before we map the window for the first time, check for
	 * X10 backing store for this window.  If is has some, then,
	 * this window MUST be marked with overrideRedirect.
	 */
	if (wp->neverMapped) {
	    Bool		dontNeedOverride = (!OverrideRedirect);
	    XSetWindowAttributes xswa;
	    Pixmap		pixmap;
	    PixmapInfoptr	pp;

	    if (dontNeedOverride) {
		pixmap = wp->savedPixmap;
		if (pixmap) {
		    pp = PixmapLookup(client, pixmap);
		    if (pp != NULL)
			dontNeedOverride = False;
		} else if (WindowHasBackingStore(client, wp))
		    dontNeedOverride = False;
	    }

	    if (dontNeedOverride) {
		xswa.override_redirect = False;
		XChangeWindowAttributes (dpy, w, CWOverrideRedirect, &xswa);
	    }
	    wp->neverMapped = False;
	}

	/*
	 * Note the serial number of the request so we can change
	 * the exposure into an ExposeWindow.
	 */
	AddRequestSerial(&wp->mapReq, client, NextRequest(dpy));
	XMapWindow(dpy, w);

	wp->mapRequested = True;
	ClearUnmappedTransparent(wp);
}

void X10ProcMapSubwindows(client, req)
	int	client;
	X10Req	*req;
{
	Display	*dpy;
	Window	w = req->windowId;
	GetWinInfoOrSetBadWindow(client, w, wp, return);

	dpy = clist[ client ]->dpy;

	if (WindowMapped(wp))
		clist[ client ]->pendingFlags |= ChangesPending;
	XMapSubwindows(dpy, w);
	MarkMappedWinTree(wp, True, client, NextRequest(dpy));
}

void X10ProcUnmapWindow(client, req)
	int	client;
	X10Req	*req;
{
	Window	w = req->windowId;
	GetWinInfoOrSetBadWindow(client, w, wp, return);

	if (WindowMapped(wp))
		clist[ client ]->pendingFlags |= ChangesPending;
	wp->mapRequested = False;
	XUnmapWindow(clist[ client ]->dpy, w);
}

void X10ProcUnmapSubwindows(client, req)
	int	client;
	X10Req	*req;
{
	Window	w = req->windowId;
	GetWinInfoOrSetBadWindow(client, w, wp, return);

	if (WindowMapped(wp))
		clist[ client ]->pendingFlags |= ChangesPending;
	MarkMappedWinTree(wp, False, client, (unsigned long)0);
	XUnmapSubwindows(clist[ client ]->dpy, w);
}

void X10ProcUnmapTransparent(client, req)
	int	client;
	X10Req	*req;
{
	Clientptr	cp = clist[ client ];
	Display		*dpy = cp->dpy;
	Window		w = req->windowId;
	Bool		saveUnder;
	Bool		windowMapRequested;
	GetWinInfoOrSetBadWindow(client, w, wp, return);

	/*
	 * If this looks like a window that had a pixmap put on top of it,
	 * put the bits back after the unmap.
	 */
	saveUnder = (!wp->transparent && wp->savedPixmap);
	windowMapRequested = WindowMapRequested(wp);

	/*
	 * If it's not a saveUnder, then
	 * we craft a window that is precisely the same height and
	 * width as the window to be unmapped, with a border of 0 and
	 * background of none (default), so that the screen stays unchanged.
	 * When the window gets mapped again, we will unmap this window.
	 */
	if (! saveUnder && windowMapRequested) {
	    XSetWindowAttributes xswa;
	    u_long	valuemask;
	    int		x = wp->x,
			y = wp->y,
			width = wp->width + 2*wp->borderWidth,
			height = wp->height + 2*wp->borderWidth;
	    XWindowChanges changes;

	    xswa.colormap = ScreenColormap;
	    xswa.override_redirect = True;
	    valuemask = CWOverrideRedirect | CWColormap;

	    wp->unmapTransparent = XCreateWindow(dpy,
			    wp->parent->id,		/* parent */
			    x, y,
			    width, height,
			    0,				/* border width */
			    ScreenDepth,		/* depth */
			    InputOutput,		/* class */
			    ScreenVisual,		/* visual */
			    valuemask,			/* flags */
			    &xswa);			/* attributes */
	    changes.stack_mode = Above;
	    changes.sibling = wp->id;
	    XConfigureWindow(dpy, wp->unmapTransparent,
		CWSibling | CWStackMode, &changes);
	    XMapWindow(dpy, wp->unmapTransparent);
	    debug(X10_UnmapTransparent,
		("covering 0x%x created for 0x%x\n",
		    wp->unmapTransparent, wp->id));

	    WindowsUnmappedTransparent++;

	    /* Cheat! */
	    MarkWindowDeleted(wp->unmapTransparent);
	}

	/*
	 * We depend very heavily on the fact that the XUnmapWindow routine
	 * uses precisely one request.
	 */
	AddRequestSerial(&cp->unmapTransReq, client, NextRequest(dpy));
	XUnmapWindow(dpy, w);

	if (saveUnder && windowMapRequested) {
		PixmapInfoptr	pp = PixmapLookup(client, wp->savedPixmap);
		WinInfoptr	next;
		int		x = 0, y = 0;
		unsigned long	valuemask;
		XGCValues	gcv;
		Bool		drawThrough = wp->drawThrough;

		/*
		 * Find the root-relative x and y.
		 */
		for (next = wp; next; next = next->parent) {
			x += next->x;
			y += next->y;
		}

		/*
		 * Fix up the GC for the copy... Be sure SubwindowMode is set
		 */
		SetClipMode(client, wp, True);
		valuemask = GCFunction | GCPlaneMask | GCGraphicsExposures;
		gcv.function = GXcopy;
		gcv.plane_mask = ~0;
		gcv.graphics_exposures = False;
		XChangeGC(dpy, wp->gc, valuemask, &gcv);

		/*
		 * do the copy.
		 */
		XCopyArea(dpy,
			wp->savedPixmap,        /* source drawable */
			DefaultRootWindow(dpy),	/* destination drawable */
			wp->gc,
			0, 0,			/* source x,y */
			(unsigned int)wp->width, (unsigned int)wp->height,
			x, y);			/* dest x,y */

		/*
		 * Clean up.
		 */
		if (pp == NULL)
			XFreePixmap(dpy, wp->savedPixmap);
		wp->savedPixmap = 0;
		SetClipMode(client, wp, drawThrough);
	}

	wp->mapRequested = False;
}

Bool X10ProcGrabButton(client, req)
	int	client;
	X10Req	*req;
{
	unsigned int	button;
	unsigned long	x10EventMask = req->param.l[1],
			x11EventMask,
			x10ButtonMask = (unsigned long)req->mask,
			x11ButtonMask;
	Cursor		cursor = req->param.l[0];
	Clientptr	cp = clist[ client ];
	Display		*dpy = cp->dpy;
	GetWinInfoOrSetBadWindow(client, req->windowId, wp, return(False));

	cp->pendingFlags |= ChangesPending;

	button = X10ButtonMaskToX11Button(x10ButtonMask);
	x11ButtonMask = X10ButtonMaskToX11KeyMask(x10ButtonMask);
	x11EventMask = X10toX11EventMask(x10EventMask);

	if (debugged(X10_GrabButton)) {
		ShowEventMask(x10EventMask, x11EventMask);
		ShowButtonMask(x10ButtonMask, x11ButtonMask);
	}

	if (button > Button3) {
		Xstatus = X10BadValue;
		return(False);
	}

	XGrabButton(dpy,
		button,				/* button */
		(unsigned int) x11ButtonMask,	/* modifiers */
		req->windowId,
		True,				/* owner events */
		(unsigned int) x11EventMask,	/* event mask */
		GrabModeAsync, GrabModeAsync,	/* pointer/keyboard mode */
		None,				/* confine-to window */
		cursor == 0 ? None : cursor);

	/* XXX are X11 pointer/button grabs exclusive? */
	wp->grabmask = wp->mask;
	wp->mask = x11ButtonMask;
	cp->grabwin = req->windowId;

	return(True);
}

/*ARGSUSED*/
void X10ProcUngrabButton(client, req)
	int	client;
	X10Req	*req;
{
	Clientptr	cp = clist[ client ];
	Display		*dpy = cp->dpy;
	WinInfoptr	wp;

	/* XXX are X11 pointer/button grabs exclusive? */
	wp = WinLookup(client, cp->grabwin);
	cp->grabwin = 0;
	if (wp == NULL)
		return;
	wp->mask = wp->grabmask;

	XUngrabButton(dpy,
		(unsigned int)AnyButton,
		(unsigned int)AnyModifier,
		cp->grabwin);
}

Bool X10ProcGrabMouse(client, req)
	int	client;
	X10Req	*req;
{
	int		status,
			result = False;
	unsigned long	mask = req->param.l[1];
	Cursor		cursor = req->param.l[0];
	Clientptr	cp = clist[ client ];
	Display 	*dpy = cp->dpy;
	Window		w = req->windowId;
	GetWinInfoOrSetBadWindow(client, w, wp, return(False));

	if (! GrabMouseAllowed)
		return(True);

	/*
	 * Only these bits are significant in the X10 GrabMouse request.
	 */
	mask =    X10ButtonPressed
		| X10ButtonReleased
		| X10EnterWindow
		| X10LeaveWindow
		| X10MouseMoved
		| X10RightDownMotion
		| X10MiddleDownMotion
		| X10LeftDownMotion;
	mask = (req->param.l[1] & mask) | (wp->mask & ~mask);

	/*
	 * If the window or its parent is not mapped, then this would
	 * result in GrabNotViewable.  Fix that here.
	 */
	if (! WindowMapRequested(wp)) {
		GrabUnviewable = client;
		w = DefaultRootWindow(dpy);
	} else
		GrabUnviewable = 0;

	status = XGrabPointer(dpy,
			w,
			True,
			(unsigned int)(X10toX11EventMask(mask)
				| BasicEventMask),
			GrabModeAsync, GrabModeAsync,
			None,
			cursor == 0 ? None : cursor,
			CurrentTime);
	switch (status) {
	case AlreadyGrabbed:
		debug(X10_GrabMouse, ("AlreadyGrabbed "));
		Xstatus = X10BadGrab;
		break;
	case GrabNotViewable:
		debug(X10_GrabMouse, ("GrabNotViewable "));
		Xstatus = X10BadGrab;
		break;
	case GrabFrozen:
		debug(X10_GrabMouse, ("GrabFrozen "));
		Xstatus = X10BadGrab;
		break;
	case GrabInvalidTime:
		Error("Invalid time on GrabPointer()");
		break;
	case GrabSuccess:
		result = True;
		wp->grabmask = wp->mask;
		wp->mask = mask;
		cp->grabwin = req->windowId;
		break;
	default:
		Error("unknow GrabPointer status=%d", status);
		break;
	}
	return(result);
}

/*ARGSUSED*/
void X10ProcUngrabMouse(client, req)
	int	client;
	X10Req	*req;
{
	Clientptr	cp = clist[ client ];
	Display		*dpy = cp->dpy;
	WinInfoptr	wp;

	GrabUnviewable = 0;

	/* XXX are X11 pointer/button grabs exclusive? */
	if (cp->grabwin) {
	    wp = WinLookup(client, cp->grabwin);
	    cp->grabwin = 0;
	    if (wp == NULL)
		    return;
	    wp->mask = wp->grabmask;
	}

	XUngrabPointer(dpy, CurrentTime);
}

void X10ProcGrabServer(client)
	int	client;
{
	GrabServer(client);
}

void X10ProcUngrabServer(client)
	int	client;
{
	UngrabServer(client);
}

Bool X10ProcQueryMouse(client, req, rep)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
{
	Window		root, child;
	int		rootx, rooty, winx, winy;
	unsigned int	mask;
	Display		*dpy = clist[ client ]->dpy;
	Bool		result;

	result = XQueryPointer(dpy,
			req->windowId,
			&root,
			&child,
			&rootx,
			&rooty,
			&winx,
			&winy,
			&mask);

	if (! result) {
		Xstatus = X10BadWindow;
		return (False);
	}
	rep->param.l[0] = child;
	rep->param.s[2] = winx;
	rep->param.s[3] = winy;
	rep->param.s[4] = X11toX10KeyButState((unsigned long)mask);
	return (True);
}

void X10ProcStoreName(client, req, command)
	int	client;
	X10Req	*req;
	char	*command;
{
	char	name[ BUFSIZ ];
	int len = req->param.s[0];

	strcpy (name, "(X10) ");
	strncpy (name+6, command, len);
	name[len+6] = '\0';
	XChangeProperty(clist[ client ]->dpy, 
	    req->windowId, XA_WM_NAME, XA_STRING, 8, PropModeReplace,
	    (unsigned char *)name, len+6);
	XChangeProperty(clist[ client ]->dpy, 
	    req->windowId, XA_WM_COMMAND, XA_STRING, 8, PropModeReplace,
	    (unsigned char *)command, len);
}

char *X10ProcFetchName(client, req, rep)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
{
	char		*name;
	int		result, format;
	long		bytesafter;
	Atom		type;
	unsigned long	nitems;

	result = XGetWindowProperty(clist[ client ]->dpy, 
	    req->windowId, XA_WM_COMMAND, (long)0, (long)BUFSIZ-1,
	    False, XA_STRING, &type, &format, &nitems, &bytesafter,
	    (unsigned char **)&name);

	if (result == Success) {
		rep->param.s[0] = nitems;
		return(name);
	} else {
		rep->param.s[0] = 0;
		Xstatus = X10BadValue;
		switch(result) {
		default:
			Notice("unknown XGetWindowProperty() return=%d",
				result);
			break;
		case BadMatch:
			break;
		case BadWindow:
			Xstatus = X10BadWindow;
			break;
		}
		return(NULL);
	}
}

void X10ProcSetResizeHint(client, req)
	int	client;
	X10Req	*req;
{
	XSizeHints	sh;
	Display *dpy = clist[ client ]->dpy;
	int	result;
	GetWinInfoOrSetBadWindow(client, req->windowId, wp, return);

	result = XGetNormalHints(dpy, req->windowId, &sh);
	if (result != Success)
		bzero((char *)&sh, sizeof(XSizeHints));

	/*
	 * Re-assert exactly what we want.
	 */
	sh.x = wp->x;
	sh.y = wp->y;
	sh.height = wp->height;
	sh.width = wp->width;

	sh.min_height = req->param.s[0];
	sh.height_inc = req->param.s[1];
	sh.min_width = req->param.s[2];
	sh.width_inc = req->param.s[3];
	sh.flags = (USSize | USPosition | PPosition | PSize
			| PMinSize | PResizeInc);
	(void) XSetNormalHints(dpy, req->windowId, &sh);
}

void X10ProcGetResizeHint(client, req, rep)
	int	client;
	X10Req	*req;
	X10Rep	*rep;
{
	XSizeHints	sh;
	Display *dpy = clist[ client ]->dpy;
	int	result;
	GetWinInfoOrSetBadWindow(client, req->windowId, wp, return);

	result = XGetNormalHints(dpy, req->windowId, &sh);
	if (result != Success) {
		sh.min_height = 0;
		sh.height_inc = 1;
		sh.min_width = 0;
		sh.width_inc = 1;
	}
	rep->param.s[0] = sh.min_height;
	rep->param.s[1] = sh.height_inc;
	rep->param.s[2] = sh.min_width;
	rep->param.s[3] = sh.width_inc;
}

Bool X10ProcSelectInput(client, req)
	int	client;
	X10Req	*req;
{
	unsigned long	x10mask = req->param.l[0];
	unsigned long	x11mask = X10toX11EventMask(x10mask);
	GetWinInfoOrSetBadWindow(client, req->windowId, wp, return);

	if (x10mask & X10ExposeRegion)
		x10mask |= X10ExposeWindow; /* protocol says so */
	if (debugged(X10_SelectInput))
		ShowEventMask(x10mask, x11mask);
	wp->mask = x10mask;

	XSelectInput(clist[ client ]->dpy,
			req->windowId,
			BasicEventMask | x11mask);
}

Bool X10ProcClear(client, req)
	int	client;
	X10Req	*req;
{
	Drawable	d = req->windowId;
	GetWinInfoOrSetBadWindow(client, d, wp, return);

	if (!WindowMapRequested(wp->parent))
		return;

	clist[ client ]->pendingFlags |= GraphicsPending;

	if (wp->transparent) {
		int	x, y;
		WinInfoptr	parent;
		/*
		 * Find a drawble that is suitable for graphic requests.
		 */
		parent = FindDrawableParent(wp, &x, &y);

		XClearArea(clist[ client ]->dpy,
				parent->id,
				x, y,			/* x and y of area */
				(unsigned int) wp->width,
				(unsigned int) wp->height,
				False);
	} else
		XClearWindow(clist[ client ]->dpy, req->windowId);
}

Bool X10ProcDefineCursor(client, req)
	int	client;
	X10Req	*req;
{
	XDefineCursor(clist[ client ]->dpy,
		req->windowId,
		(Cursor) req->param.l[0]);
}

