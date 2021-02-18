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
 * $XConsortium: events.c,v 6.3 89/03/11 09:36:53 rws Exp $ 
 * $Locker:  $ 
 */
#include "resource.h"
#include "X11/keysym.h"
#include "lk201keys.h"

/*
 * Standard local macro for looking up an event window.
 */
#define	GetWindowOrReturn(client, win, wp)				\
	wp = WinLookup(client, win);					\
	if (wp == NULL)							\
		return (EventWindowMissing);

#define	GetSubWindowOrReturn(client, win, wp)				\
	wp = WinLookup(client, win);					\
	if (wp == NULL)							\
		return (EventSubWindowMissing);

#define	GetEventWindowOrReturn(client, win, wp, interestMask)		\
	wp = WinLookup(client, win);					\
	if (wp == NULL)							\
		return (EventWindowMissing);				\
	if ((wp->mask & (interestMask)) == 0)				\
		return (EventNotSelected);


CheckX11events(client, reqcode)
	int	client;
	int	reqcode;
{
	Display *dpy = clist[ client ]->dpy;

	if (QLength(dpy)) {
		debug(reqcode, ("...\n"));
		X11events( ConnectionNumber(dpy) );
		debug(reqcode, (" ..."));
	}
}

StaticFunction WinInfoptr LookupEventAncestor(wp, mask)
	WinInfoptr	wp;
	unsigned long	mask;
{
	WinInfoptr	parent;

	/*
	 * Figure out the which ancestor is interested in this event.
	 */
	for (parent = wp; parent; parent = parent->parent)
		if (parent->mask & mask)
			break;;
	return(parent);
}

StaticFunction GenerateNoExposeEvent(client, noexpev, rep)
	int	client;
	XNoExposeEvent	*noexpev;
	X10Rep	*rep;
{
	WinInfoptr	wp, parent;
	Window		eventWindow, subWindow;
	PixmapInfoptr	pp;

	/*
	 * It may be that this is a graphics exposure event on a pixmap,
	 * which is meaningless to x10.  It is better to traverse the
	 * memory here looking for a pixmap, than to risk a round trip
	 * that GetEventWindowOrReturn() may cause when QueryTree gets
	 * called.
	 */
	pp = PixmapLookup(client, (Pixmap)noexpev->drawable);
	if (pp)
		return (EventNotSelected);

	/*
	 * But even that may not be enough... that has been at least one
	 * version of A tek server that unconditionally generated
	 * NoExpose events after a CopyArea.  X10ProcBitmapBits put
	 * tickled this excessively.
	 */
	MarkWindowDeleted(noexpev->drawable);

	GetEventWindowOrReturn(client, noexpev->drawable, wp, X10ExposeCopy);

	parent = LookupEventAncestor(wp, (unsigned long)X10ExposeCopy);
	if (parent == NULL)
		return (EventNotSelected);
	if (parent == wp) {
		eventWindow = wp->id;
		subWindow = 0;
	} else {
		eventWindow = parent->id;
		subWindow = wp->id;
	}

	rep->code = X10ExposeCopy;
	rep->param.l[0] = eventWindow;
	rep->param.l[3] = subWindow;
	return (EventOK);
}

StaticFunction GenerateGraphicsExposeEvent(client, grexpev, rep)
	int	client;
	XGraphicsExposeEvent	*grexpev;
	X10Rep	*rep;
{
	WinInfoptr	wp, parent;
	Window		eventWindow, subWindow;

	GetEventWindowOrReturn(client, grexpev->drawable, wp, X10ExposeCopy);

	parent = LookupEventAncestor(wp, (unsigned long)X10ExposeCopy);
	if (parent == NULL)
		return (EventNotSelected);
	if (parent == wp) {
		eventWindow = wp->id;
		subWindow = 0;
	} else {
		eventWindow = parent->id;
		subWindow = wp->id;
	}

	rep->code = X10ExposeRegion;
	rep->param.l[0] = eventWindow;
	rep->param.s[3] = X10ExposeCopy;
	rep->param.s[4] = grexpev->width;
	rep->param.s[5] = grexpev->height;
	rep->param.l[3] = subWindow;
	rep->param.s[9] = grexpev->x;
	rep->param.s[8] = grexpev->y;

	/*
	 * At the end of a expose sequence send
	 * an ExposeCopy.
	 */
	if (grexpev->count == 0) {
		XEvent	pe;
		StaticFunction UnGetXEvent();

		pe.xnoexpose.type = NoExpose;
		pe.xnoexpose.display = grexpev->display;
		pe.xnoexpose.drawable = grexpev->drawable;
		pe.xnoexpose.major_code = grexpev->major_code;
		pe.xnoexpose.minor_code = grexpev->minor_code;
		UnGetXEvent(client, &pe);
	}
	return (EventOK);
}

StaticFunction GenerateConfigureExposeEvent(client, configev, rep)
	int	client;
	XConfigureEvent	*configev;
	X10Rep	*rep;
{
	WinInfoptr	wp, parent;
	Window		eventWindow,
			subWindow = configev->window;

	GetSubWindowOrReturn(client, subWindow, wp);

	/*
	 * The x10 protocol sez that a ConfigureWindow request generates
	 * an exposure event... so if we didn't issue a request, generate
	 * an exposure event only if the width and height changes.
	 * Note that we never change our configuration if we requested
	 * it, because it has already been recorded.  Besides, this
	 * may be an event for a ConfigureWindow long past!!
	 */
	if (! CausedByRequest(&wp->configureReq, configev->serial)) {
		wp->x = configev->x;
		wp->y = configev->y;
		if (wp->width == configev->width
		 && wp->height == configev->height)
			return (EventNoChange);
		wp->width = configev->width;
		wp->height = configev->height;
	}

	/*
	 * Don't give an expose event if it hasn't been mapped yet or
	 * if it's transparent.
	 */
	if (wp->transparent || !WindowMapRequested(wp))
		return (EventSuppressed);

	parent = LookupEventAncestor(wp,
			(unsigned long) X10ExposeRegion | X10ExposeWindow);
	if (parent == NULL)
		return (EventNotSelected);
	if (parent == wp) {
		eventWindow = wp->id;
		subWindow = 0;
	} else {
		eventWindow = parent->id;
		subWindow = wp->id;
	}

	rep->code = X10ExposeWindow;
	rep->param.l[0] = eventWindow;
	rep->param.s[3] = 0; /* detail */
	rep->param.s[4] = wp->width;
	rep->param.s[5] = wp->height;
	rep->param.l[3] = subWindow;
	rep->param.s[9] = 0; /* x */
	rep->param.s[8] = 0; /* y */

	return (EventOK);
}

StaticFunction GenerateUnmapEvent(client, unmapev, rep)
	int	client;
	XUnmapEvent	*unmapev;
	X10Rep	*rep;
{
	WinInfoptr	wp;
	Window		eventWindow = unmapev->event,
			subWindow;

	GetWindowOrReturn(client, eventWindow, wp);

	if (unmapev->window == eventWindow) {
		subWindow = 0;
		wp->mapped = False;
	} 
	else {
		subWindow = unmapev->window;
		GetSubWindowOrReturn(client, subWindow, wp);

		wp->mapped = False;
		if (wp->mask & X10UnmapWindow)
			return (EventSelectedByChild);
	}

	/*
	 * Now that we've marked it unmapped, we may need to do no more.
	 */
	if ((wp->mask & X10UnmapWindow) == 0)
		return (EventNotSelected);

	if (CausedByRequest(&clist[ client ]->unmapTransReq, unmapev->serial)) {
		debug(X10_UnmapTransparent,
			("serial %d: caused by UnmapTransparent\n",
			unmapev->serial));
		return (EventSuppressed);
	}


	rep->code = X10UnmapWindow;
	rep->param.l[0] = eventWindow;
	rep->param.l[3] = subWindow;
	return (EventOK);
}

/*
 * the UnmapTransparent request will sometimes gobble up unmap events...
 * those events must still be processed.
 */
ProcessUnmapEvent(client, unmapev)
	int	client;
	XUnmapEvent	*unmapev;
{
	X10Rep	dummy;

	(void) GenerateUnmapEvent(client, unmapev, &dummy);
}

StaticFunction GenerateMapExposeEvent(client, mapev, rep)
	int	client;
	XMapEvent	*mapev;
	X10Rep	*rep;
{
	WinInfoptr	wp;
	Window		eventWindow = mapev->event;

	GetWindowOrReturn(client, eventWindow, wp);

	if (mapev->window == eventWindow)
		wp->mapped = True;
	else {
		GetSubWindowOrReturn(client, mapev->window, wp);
		wp->mapped = True;
	}

	/*
	 * If this wasn't caused by a map done by us, mark it so
	 * that it looks that way.
	 */
	if (! CausedByRequest(&wp->mapReq, mapev->serial))
		AddRequestSerial(&wp->mapReq, client, mapev->serial);

	return (EventHasNoX10Equiv);
}

StaticFunction GenerateEnterLeaveEvent(client, cross, rep)
	int	client;
	XCrossingEvent	*cross;
	X10Rep	*rep;
{
	WinInfoptr		wp, parent;
	Window			eventWindow,
				subWindow = cross->window;

	LastKnownServerTime = cross->time;
	if (cross->mode != NotifyNormal)
		return (EventModeField);
	GetSubWindowOrReturn(client, subWindow, wp);

	parent = LookupEventAncestor(wp,
			(unsigned long) X10LeaveWindow | X10EnterWindow);
	if (parent == NULL)
		return (EventNotSelected);
	if (parent == wp) {
		eventWindow = wp->id;
		subWindow = 0;
	} else {
		eventWindow = parent->id;
		subWindow = wp->id;
	}

	rep->param.s[3] = X11toX10KeyButState((unsigned long) cross->state);
	if (cross->type == EnterNotify)
		rep->code = X10EnterWindow;
	else
		rep->code = X10LeaveWindow;
	switch (cross->detail) {
	case NotifyInferior:
		rep->param.s[3] |= 1;
		break;
	case NotifyVirtual:
	case NotifyNonlinearVirtual:
		rep->param.s[3] |= 2;
		break;
	case NotifyAncestor:
	case NotifyNonlinear:
		break;
	}

	rep->param.l[0] = eventWindow;
	rep->param.s[2] = cross->time / 10;
	rep->param.s[4] = cross->x;
	rep->param.s[5] = cross->y;
	rep->param.l[3] = subWindow;
	rep->param.l[4] = (cross->x_root << 16)
		| cross->y_root;
	return(EventOK);
}

StaticFunction GenerateExposeEvent(client, expev, rep)
	int	client;
	XExposeEvent	*expev;
	X10Rep	*rep;
{
	WinInfoptr		wp, parent;
	Window			eventWindow,
				subWindow = expev->window;
	long			code;
	int			mask;
	XRectangle		bounds;

	GetSubWindowOrReturn(client, subWindow, wp);

	parent = LookupEventAncestor(wp,
			(unsigned long) X10ExposeRegion | X10ExposeWindow);
	if (parent == NULL)
		return (EventNotSelected);
	if (parent == wp) {
		mask = wp->mask;
		eventWindow = wp->id;
		subWindow = 0;
	} else {
		mask = parent->mask;
		eventWindow = parent->id;
		subWindow = wp->id;
	}

	if (CausedByRequest(&clist[ client ]->unmapTransReq, expev->serial)) {
		debug(X10_UnmapTransparent,
			("serial %d: caused by UnmapTransparent\n",
			expev->serial));
		return (EventSuppressed);
	}

	/*
	 * Must be one of these, if we have gotten here...
	 * Discover which kind of event to generate
	 */
	if ((mask & (X10ExposeWindow | X10ExposeRegion)) == X10ExposeWindow)
		code = X10ExposeWindow;
	else if ((mask & X10ExposeWindow)
	    && wp->width == expev->width
	    && wp->height == expev->height
	    && wp->x == 0
	    && wp->y == 0)
		code = X10ExposeWindow;
	else if (CausedByRequest(&wp->mapReq, expev->serial))
		code = X10ExposeWindow;
	else
		code = X10ExposeRegion;

	if (code == X10ExposeWindow && expev->count)
		return(EventNotSelected);

	/*
	 * Set the bounds for these exposure events.
	 */
	if (code == X10ExposeWindow) {
		bounds.width = wp->width;
		bounds.height = wp->height;
		bounds.x = 0;
		bounds.y = 0;
	} else {
		bounds.width = expev->width;
		bounds.height = expev->height;
		bounds.x = expev->x;
		bounds.y = expev->y;
	}

	rep->code = code;
	rep->param.l[0] = eventWindow;
	rep->param.s[3] = 0;	/* always 0 */
	rep->param.l[3] = subWindow;
	rep->param.s[4] = bounds.width;
	rep->param.s[5] = bounds.height;
	rep->param.s[9] = bounds.x;
	rep->param.s[8] = bounds.y;
	return(EventOK);
}

StaticFunction TranslateEvent(client, ev, rep)
	int	client;
	XEvent	*ev;
	X10Rep	*rep;
{
	int	keycode;
	WinInfoptr	wp;
	int	event = EventOK;
	
	switch (ev->type) {
	case EnterNotify:
	case LeaveNotify:
	    event = GenerateEnterLeaveEvent(client, &ev->xcrossing, rep);
	    break;

	case KeyPress:
	case KeyRelease:
	case ButtonPress:
	case ButtonRelease:
	case MotionNotify:
		LastKnownServerTime = ev->xmotion.time;
		clist[ client ]->pendingFlags |= DeviceEvents;
		rep->param.l[0] = ev->xmotion.window;
		rep->param.s[2] = ev->xmotion.time / 10;
		rep->param.s[4] = ev->xmotion.x;
		rep->param.s[5] = ev->xmotion.y;
		rep->param.l[3] = ev->xmotion.subwindow;
		rep->param.l[4] = (ev->xmotion.x_root << 16)
			| ev->xmotion.y_root;

		switch (ev->type) {
		case MotionNotify:
			rep->param.s[3] =
				X11toX10KeyButState(
					(unsigned long) ev->xmotion.state);
			rep->code = X10MouseMoved;
			break;

		case KeyPress:
		case KeyRelease:
			rep->param.s[3] =
				X11toX10KeyButState(
					(unsigned long) ev->xkey.state);
			keycode = X11toX10Keycode(ev->xkey.display,
					(KeyCode)ev->xkey.keycode);
			rep->param.s[3] |= keycode;
			if (ev->type == KeyPress)
				rep->code = X10KeyPressed;
			else
				rep->code = X10KeyReleased;
			if (keycode == KEY_LOCK && LockToggleMode)
				event = EventLockToggleMode;
			break;

		case ButtonPress:
		case ButtonRelease:
			rep->param.s[3] =
				X11toX10KeyButState(
					(unsigned long) ev->xbutton.state);
			if (ev->type == ButtonRelease)
				rep->code = X10ButtonReleased;
			else
				rep->code = X10ButtonPressed;
			switch(ev->xbutton.button) {
			case Button1:
				rep->param.s[3] |= 2;
				break;
			case Button2:
				rep->param.s[3] |= 1;
				break;
			case Button3:
			case Button4:
			case Button5:
				break;
			}
			break;
		}

		break;

	case ClientMessage:
	case FocusIn:
	case FocusOut:
	case KeymapNotify:
	case VisibilityNotify:
	case CreateNotify:
	case CirculateRequest:
	case ConfigureRequest:
	case MapRequest:
	case ResizeRequest:
	case ColormapNotify:
		event = EventHasNoX10Equiv;
		break;

	case PropertyNotify:
		LastKnownServerTime = ev->xproperty.time;
		event = EventHasNoX10Equiv;
		break;

	case ConfigureNotify:
		event = GenerateConfigureExposeEvent(client,
				&ev->xconfigure, rep);
		break;

	case MapNotify:
		event = GenerateMapExposeEvent(client, &ev->xmap, rep);
		break;

	case NoExpose:
		event = GenerateNoExposeEvent(client, &ev->xnoexpose, rep);
		break;

	case Expose:
		event = GenerateExposeEvent(client, &ev->xexpose, rep);
		break;

	case GraphicsExpose:
		event = GenerateGraphicsExposeEvent(client,
			&ev->xgraphicsexpose, rep);
		break;

	case UnmapNotify:
		event = GenerateUnmapEvent(client,
			&ev->xunmap, rep);
		break;

	case DestroyNotify:
		event = EventHasNoX10Equiv;
		/*
		 * We only look it up when the event delivered goes
		 * to the window that was destroyed... that way we
		 * only look it up once.
		 */
		MarkWindowDeleted(ev->xdestroywindow.window);
		if (ev->xdestroywindow.event != ev->xdestroywindow.window)
			break;
		wp = WinLookup(client, ev->xdestroywindow.window);
		if (wp != NULL)
			FreeWinInfo(client, wp);
		break;
	case ReparentNotify:
		event = EventHasNoX10Equiv;
		Reparent(ev->xreparent.window, ev->xreparent.parent);
		break;
	case GravityNotify:
		/* This isn't interesting, 'cause we also get ConfigureNotify */
	case CirculateNotify:
		/* SubStructureRedirect is not selcted */
		event = EventHasNoX10Equiv;
		break;

	case SelectionClear:
		LastKnownServerTime = ev->xselectionclear.time;
		/* XXX should do something with our properties */
		event = EventHasNoX10Equiv;
		break;

	case SelectionNotify:
		LastKnownServerTime = ev->xselection.time;
		/* XXX should do something with our properties */
		event = EventHasNoX10Equiv;
		break;

	case SelectionRequest:
		LastKnownServerTime = ev->xselectionrequest.time;
		/* XXX should do something with our properties */
		event = EventHasNoX10Equiv;
		break;

	default:
		event = EventUnknown;
		break;
	}

	return(event);
}

StaticFunction UnGetXEvent(client, ev)
	int	client;
	XEvent	*ev;
{
	Clientptr	cp = clist[ client ];
	/*
	 * we really only need one for graphics exposures.
	 */
	if (cp->buffer.type != 0)
		Error("event buffer overflow\n");
	bcopy((char *)ev, (char *)&cp->buffer, sizeof(XEvent));
	if (cp->buffer.type == 0)
		Error("funny event type\n");
}

StaticFunction GetNextXEvent(client, ev, forceIt)
	int	client;
	XEvent	*ev;
	int	forceIt;
{
	Clientptr	cp = clist[ client ];
	Display 	*dpy;

	if (cp->buffer.type != 0) {
		bcopy((char *)&cp->buffer, (char *)ev, sizeof(XEvent));
		cp->buffer.type = 0;
		return (True);
	}

	dpy = cp->dpy;
	if (QLength(dpy) == 0) {
		if (!forceIt)
			return (False);
	}
	XNextEvent(dpy, ev);
	if (clist[ client ] == NULL)	/* we were shut down */
		return (False);
	return (True);
}

X11events(serverfd)
	int	serverfd;
{
	Clientptr	cp;
	int		client,
			eventcnt = 0,
			qLength = 0,
			status;
	Display 	*dpy;
	XEvent		pe;
	X10Rep		*rep, *replies;

	client = (int)clist[ serverfd ];
	if (client < 0 || client >= FileDescriptors) {
		Error("X11events: server %d: client # %d?\n",
			serverfd, client);
		dpy = NULL;
	} else
		dpy = clist[ client ]->dpy;
	if (dpy == NULL)
		return;
	cp = clist[ client ];

	if (! GetNextXEvent(client, &pe, True))
		return;

	qLength = QLength(dpy) + 1;
	rep = replies = (X10Rep *)StaticAlloc(sizeof(X10Rep) * (++qLength));
	debug(XSequenceDebug, ("Events: Last seq #%d\n",
		dpy->last_request_read));
	debug(XIODebug, ("Process %d events\n", qLength));

	do {
		status = TranslateEvent(client, &pe, rep);
		if (client == PrimaryClient)
			status = EventBelongsToServer;
		ShowX11AndX10Event(client,
			&pe,
			status == EventOK ? rep : NULL,
			&status);

		if (status == EventOK) {
			if (++eventcnt == qLength)
				replies = (X10Rep *)
				    StaticAlloc(sizeof(X10Rep) * (++qLength));
			rep = replies + eventcnt;
		}
		cp->lastEventDispatched = pe.xany.serial;

	} while (GetNextXEvent(client, &pe, False));

	if (eventcnt)
		SendEventsToX10Client(client, replies, eventcnt);
}

unsigned long X10toX11EventMask(mask)
unsigned long	mask;
{
	unsigned long	X11mask = 0;

	if (X10KeyPressed & mask)	X11mask |= KeyPressMask;
	if (X10KeyReleased & mask)	X11mask |= KeyReleaseMask;
	if (X10ButtonPressed & mask)	X11mask |= ButtonPressMask;
	if (X10ButtonReleased & mask)	X11mask |= ButtonReleaseMask;
	if (X10EnterWindow & mask)	X11mask |= EnterWindowMask;
	if (X10LeaveWindow & mask)	X11mask |= LeaveWindowMask;
	if (X10MouseMoved & mask)	X11mask |= PointerMotionMask;
	if (X10RightDownMotion & mask)	X11mask |= Button3MotionMask;
	if (X10MiddleDownMotion & mask)	X11mask |= Button2MotionMask;
	if (X10LeftDownMotion & mask)	X11mask |= Button1MotionMask;
	if (X10ExposeWindow & mask)	X11mask |= ExposureMask;
	if (X10ExposeRegion & mask)	X11mask |= ExposureMask;
	if (X10ExposeCopy & mask)	X11mask |= 0; /* XXX done in gc */
	if (X10FocusChange & mask)	X11mask |= FocusChangeMask;
	if (X10UnmapWindow & mask)	X11mask |= StructureNotifyMask;

	X11mask |= OwnerGrabButtonMask; /* always add this */

	/*
	 * We need this for map and configureNotify events.
	 */
	X11mask |= (StructureNotifyMask | SubstructureNotifyMask);
	return (X11mask);
}

unsigned long X11toX10EventMask(mask)
	unsigned long	mask;
{
	unsigned long	X10mask = 0;

	/* XXX missing GraphicsExposure --> ExposeCopy */
	if (KeyPressMask & mask)		X10mask |= X10KeyPressed;
	if (KeyReleaseMask & mask)		X10mask |= X10KeyReleased;
	if (ButtonPressMask & mask)		X10mask |= X10ButtonPressed;
	if (ButtonReleaseMask & mask)		X10mask |= X10ButtonReleased;
	if (EnterWindowMask & mask)		X10mask |= X10EnterWindow;
	if (LeaveWindowMask & mask)		X10mask |= X10LeaveWindow;
	if (PointerMotionMask & mask)		X10mask |= X10MouseMoved;
	if (PointerMotionHintMask & mask)	X10mask |= X10MouseMoved;
	if (Button1MotionMask & mask)		X10mask |= X10LeftDownMotion;
	if (Button2MotionMask & mask)		X10mask |= X10MiddleDownMotion;
	if (Button3MotionMask & mask)		X10mask |= X10RightDownMotion;
	if (Button4MotionMask & mask)		/* X10mask |= 0 */;
	if (Button5MotionMask & mask)		/* X10mask |= 0 */;
	if (ButtonMotionMask & mask)		/* X10mask |= 0 */;
	if (KeymapStateMask & mask)		/* X10mask |= 0 */;
	if (ExposureMask & mask)		X10mask |= X10ExposeWindow;
	if (VisibilityChangeMask & mask)	/* X10mask |= 0 */;
	if (StructureNotifyMask & mask)		X10mask |= X10UnmapWindow;
	if (ResizeRedirectMask & mask)		/* X10mask |= 0 */;
	if (SubstructureNotifyMask & mask)	X10mask |= X10UnmapWindow;
	if (SubstructureRedirectMask & mask)	/* X10mask |= 0 */;
	if (FocusChangeMask & mask)		X10mask |= X10FocusChange;
	if (PropertyChangeMask & mask)		/* X10mask |= 0 */;
	if (ColormapChangeMask & mask)		/* X10mask |= 0 */;
	if (OwnerGrabButtonMask & mask)		/* X10mask |= 0 */;

	return (X10mask);
}

unsigned long X11toX10KeyButState(state)
	unsigned long	state;
{
	unsigned long	x10state = 0;

	if (state & LockMask)		x10state |= X10ShiftLockMask;
	if (state & ShiftMask)		x10state |= X10ShiftMask;
	if (state & ControlMask)	x10state |= X10ControlMask;
	if (state & Mod1Mask)		x10state |= X10MetaMask;
	if (state & Mod2Mask)		x10state |= 0;
	if (state & Mod3Mask)		x10state |= 0;
	if (state & Mod4Mask)		x10state |= 0;
	if (state & Mod5Mask)		x10state |= 0;
	if (state & Button1Mask)	x10state |= X10LeftMask;
	if (state & Button2Mask)	x10state |= X10MiddleMask;
	if (state & Button3Mask)	x10state |= X10RightMask;
	if (state & Button4Mask)	x10state |= 0;
	if (state & Button5Mask)	x10state |= 0;

	return(x10state);
}

unsigned long X10ButtonMaskToX11KeyMask(mask)
unsigned long	mask;
{
	unsigned long	x11mask = 0;

	if (mask & X10ShiftMask)	x11mask |= ShiftMask;
	if (mask & X10ShiftLockMask)	x11mask |= LockMask;
	if (mask & X10ControlMask)	x11mask |= ControlMask;
	if (mask & X10MetaMask)		x11mask |= Mod1Mask;

	return(x11mask);
}

/*
* Called for GrabButton and UngrabButton: mask must have exactly
* one of LeftMask, MiddleMask or RightMask set.
*/
unsigned long X10ButtonMaskToX11Button(mask)
unsigned long	mask;
{
	unsigned long	button;

	switch(mask & (X10LeftMask|X10MiddleMask|X10RightMask)) {
	case X10LeftMask:       
		button = Button1; 
		break;
	case X10MiddleMask:     
		button = Button2; 
		break;
	case X10RightMask:      
		button = Button3; 
		break;
	default:		
		button = -1; 
		break;
	}
	return (button);
}

#define MAP_LENGTH	0x10000
unsigned char X11KeysymToX10KeycodeMap[ MAP_LENGTH ];
/*
* We must translate an X11 keycode --> X11 keysym --> X10 keycode,
* which just happens to be an lk201 keycode.
 */
InitKeycodeMap()
{
	X11KeysymToX10KeycodeMap[ XK_Escape ] = KEY_ESC;
	X11KeysymToX10KeycodeMap[ XK_BackSpace ] = KEY_BS;
	X11KeysymToX10KeycodeMap[ XK_Linefeed ] = KEY_LF;

	X11KeysymToX10KeycodeMap[ XK_F1 ] = KEY_F1;
	X11KeysymToX10KeycodeMap[ XK_F2 ] = KEY_F2;
	X11KeysymToX10KeycodeMap[ XK_F3 ] = KEY_F3;
	X11KeysymToX10KeycodeMap[ XK_F4 ] = KEY_F4;

	/* Yep: skewed by 1.  This matches X10. lk201's skipped f5 */
	X11KeysymToX10KeycodeMap[ XK_F5 ] = KEY_F6;
	X11KeysymToX10KeycodeMap[ XK_F6 ] = KEY_F7;
	X11KeysymToX10KeycodeMap[ XK_F7 ] = KEY_F8;
	X11KeysymToX10KeycodeMap[ XK_F8 ] = KEY_F9;

	X11KeysymToX10KeycodeMap[ XK_F9 ] = KEY_F9;
	X11KeysymToX10KeycodeMap[ XK_F10 ] = KEY_F10;
	X11KeysymToX10KeycodeMap[ XK_F11 ] = KEY_F11;
	X11KeysymToX10KeycodeMap[ XK_F12 ] = KEY_F12;
	X11KeysymToX10KeycodeMap[ XK_F13 ] = KEY_F13;
	X11KeysymToX10KeycodeMap[ XK_F14 ] = KEY_F14;
	X11KeysymToX10KeycodeMap[ XK_F15 ] = KEY_HELP;
	X11KeysymToX10KeycodeMap[ XK_Help ] = KEY_HELP;
	X11KeysymToX10KeycodeMap[ XK_F16 ] = KEY_MENU;
	X11KeysymToX10KeycodeMap[ XK_Menu ] = KEY_MENU;
	X11KeysymToX10KeycodeMap[ XK_F17 ] = KEY_F17;
	X11KeysymToX10KeycodeMap[ XK_F18 ] = KEY_F18;
	X11KeysymToX10KeycodeMap[ XK_F19 ] = KEY_F19;
	X11KeysymToX10KeycodeMap[ XK_F20 ] = KEY_F20;
	X11KeysymToX10KeycodeMap[ XK_Find ] = KEY_FIND;
	X11KeysymToX10KeycodeMap[ XK_Insert ] = KEY_INSERT_HERE;
	X11KeysymToX10KeycodeMap[ XK_Delete ] = KEY_REMOVE;
	X11KeysymToX10KeycodeMap[ XK_Select ] = KEY_SELECT;
	X11KeysymToX10KeycodeMap[ XK_Prior ] = KEY_PREV_SCREEN;
	X11KeysymToX10KeycodeMap[ XK_Next ] = KEY_NEXT_SCREEN;

	X11KeysymToX10KeycodeMap[ XK_KP_Space ] = KEY_SPACE;
	X11KeysymToX10KeycodeMap[ XK_KP_Tab ] = KEY_TAB;
	X11KeysymToX10KeycodeMap[ XK_KP_Decimal ] = KEY_KP_PERIOD;
	X11KeysymToX10KeycodeMap[ XK_KP_Enter ] = KEY_KP_ENTER;
	X11KeysymToX10KeycodeMap[ XK_KP_Equal ] = KEY_EQUAL;
	X11KeysymToX10KeycodeMap[ XK_KP_Add ] = KEY_EQUAL;
	X11KeysymToX10KeycodeMap[ XK_KP_Divide ] = KEY_SLASH;
	X11KeysymToX10KeycodeMap[ XK_KP_0 ] = KEY_KP_0;
	X11KeysymToX10KeycodeMap[ XK_KP_1 ] = KEY_KP_1;
	X11KeysymToX10KeycodeMap[ XK_KP_2 ] = KEY_KP_2;
	X11KeysymToX10KeycodeMap[ XK_KP_3 ] = KEY_KP_3;
	X11KeysymToX10KeycodeMap[ XK_KP_4 ] = KEY_KP_4;
	X11KeysymToX10KeycodeMap[ XK_KP_5 ] = KEY_KP_5;
	X11KeysymToX10KeycodeMap[ XK_KP_6 ] = KEY_KP_6;
	X11KeysymToX10KeycodeMap[ XK_KP_Separator ] = KEY_KP_COMMA;
	X11KeysymToX10KeycodeMap[ XK_KP_7 ] = KEY_KP_7;
	X11KeysymToX10KeycodeMap[ XK_KP_8 ] = KEY_KP_8;
	X11KeysymToX10KeycodeMap[ XK_KP_9 ] = KEY_KP_9;

	X11KeysymToX10KeycodeMap[ XK_KP_Subtract ] = KEY_KP_HYPHEN;
	X11KeysymToX10KeycodeMap[ XK_hyphen ] = KEY_HYPHEN;

	X11KeysymToX10KeycodeMap[ XK_KP_F1 ] = KEY_KP_PF1;
	X11KeysymToX10KeycodeMap[ XK_KP_F2 ] = KEY_KP_PF2;
	X11KeysymToX10KeycodeMap[ XK_KP_F3 ] = KEY_KP_PF3;
	X11KeysymToX10KeycodeMap[ XK_KP_F4 ] = KEY_KP_PF4;
	X11KeysymToX10KeycodeMap[ XK_Left ] = KEY_LEFT;
	X11KeysymToX10KeycodeMap[ XK_Right ] = KEY_RIGHT;
	X11KeysymToX10KeycodeMap[ XK_Down ] = KEY_DOWN;
	X11KeysymToX10KeycodeMap[ XK_Up ] = KEY_UP;
	X11KeysymToX10KeycodeMap[ XK_Shift_L ] = KEY_SHIFT;
	X11KeysymToX10KeycodeMap[ XK_Shift_R ] = KEY_SHIFT;
	X11KeysymToX10KeycodeMap[ XK_Control_L ] = KEY_CTRL;
	X11KeysymToX10KeycodeMap[ XK_Control_R ] = KEY_CTRL;
	X11KeysymToX10KeycodeMap[ XK_Caps_Lock ] = KEY_LOCK;
	X11KeysymToX10KeycodeMap[ XK_Shift_Lock ] = KEY_LOCK;
	X11KeysymToX10KeycodeMap[ XK_Multi_key ] = KEY_COMPOSE;
	X11KeysymToX10KeycodeMap[ XK_Meta_L ] = KEY_COMPOSE;
	X11KeysymToX10KeycodeMap[ XK_Meta_R ] = KEY_COMPOSE;
	X11KeysymToX10KeycodeMap[ XK_Delete ] = KEY_DELETE;
	X11KeysymToX10KeycodeMap[ XK_Return ] = KEY_RETURN;
	X11KeysymToX10KeycodeMap[ XK_Tab ] = KEY_TAB;
	X11KeysymToX10KeycodeMap[ XK_quoteleft ] = KEY_TILDE;
	X11KeysymToX10KeycodeMap[ XK_bar ] = KEY_TILDE;
	X11KeysymToX10KeycodeMap[ XK_1 ] = KEY_TR_1;                 
	X11KeysymToX10KeycodeMap[ XK_q ] = KEY_Q;
	X11KeysymToX10KeycodeMap[ XK_a ] = KEY_A;
	X11KeysymToX10KeycodeMap[ XK_z ] = KEY_Z;
	X11KeysymToX10KeycodeMap[ XK_2 ] = KEY_TR_2;
	X11KeysymToX10KeycodeMap[ XK_w ] = KEY_W;
	X11KeysymToX10KeycodeMap[ XK_s ] = KEY_S;
	X11KeysymToX10KeycodeMap[ XK_x ] = KEY_X;
	X11KeysymToX10KeycodeMap[ XK_less ] = KEY_LANGLE_RANGLE;
	X11KeysymToX10KeycodeMap[ XK_3 ] = KEY_TR_3;
	X11KeysymToX10KeycodeMap[ XK_e ] = KEY_E;
	X11KeysymToX10KeycodeMap[ XK_d ] = KEY_D;
	X11KeysymToX10KeycodeMap[ XK_c ] = KEY_C;
	X11KeysymToX10KeycodeMap[ XK_4 ] = KEY_TR_4;
	X11KeysymToX10KeycodeMap[ XK_r ] = KEY_R;
	X11KeysymToX10KeycodeMap[ XK_f ] = KEY_F;
	X11KeysymToX10KeycodeMap[ XK_v ] = KEY_V;
	X11KeysymToX10KeycodeMap[ XK_space ] = KEY_SPACE;
	X11KeysymToX10KeycodeMap[ XK_5 ] = KEY_TR_5;
	X11KeysymToX10KeycodeMap[ XK_t ] = KEY_T;
	X11KeysymToX10KeycodeMap[ XK_g ] = KEY_G;
	X11KeysymToX10KeycodeMap[ XK_b ] = KEY_B;
	X11KeysymToX10KeycodeMap[ XK_6 ] = KEY_TR_6;
	X11KeysymToX10KeycodeMap[ XK_y ] = KEY_Y;
	X11KeysymToX10KeycodeMap[ XK_h ] = KEY_H;
	X11KeysymToX10KeycodeMap[ XK_n ] = KEY_N;
	X11KeysymToX10KeycodeMap[ XK_7 ] = KEY_TR_7;
	X11KeysymToX10KeycodeMap[ XK_u ] = KEY_U;
	X11KeysymToX10KeycodeMap[ XK_j ] = KEY_J;
	X11KeysymToX10KeycodeMap[ XK_m ] = KEY_M;
	X11KeysymToX10KeycodeMap[ XK_8 ] = KEY_TR_8;
	X11KeysymToX10KeycodeMap[ XK_i ] = KEY_I;
	X11KeysymToX10KeycodeMap[ XK_k ] = KEY_K;
	X11KeysymToX10KeycodeMap[ XK_comma ] = KEY_COMMA;
	X11KeysymToX10KeycodeMap[ XK_9 ] = KEY_TR_9;
	X11KeysymToX10KeycodeMap[ XK_o ] = KEY_O;
	X11KeysymToX10KeycodeMap[ XK_l ] = KEY_L;
	X11KeysymToX10KeycodeMap[ XK_period ] = KEY_PERIOD;
	X11KeysymToX10KeycodeMap[ XK_0 ] = KEY_TR_0;
	X11KeysymToX10KeycodeMap[ XK_p ] = KEY_P;
	X11KeysymToX10KeycodeMap[ XK_semicolon ] = KEY_SEMICOLON;
	X11KeysymToX10KeycodeMap[ XK_slash ] = KEY_SLASH;   
	X11KeysymToX10KeycodeMap[ XK_equal ] = KEY_EQUAL;
	X11KeysymToX10KeycodeMap[ XK_bracketright ] = KEY_RBRACE;
	X11KeysymToX10KeycodeMap[ XK_backslash ] = KEY_BACKSLASH;
	X11KeysymToX10KeycodeMap[ XK_asciitilde ] = KEY_BACKSLASH;
	X11KeysymToX10KeycodeMap[ XK_minus ] = KEY_HYPHEN;
	X11KeysymToX10KeycodeMap[ XK_bracketleft ] = KEY_LBRACE;
	X11KeysymToX10KeycodeMap[ XK_quoteright ] = KEY_QUOTE;
}

X11toX10Keycode(dpy, x11keyode)
	Display	*dpy;
	KeyCode x11keyode;
{
	unsigned short 	keysym;

	/* always pick column 0 and let the x10 client interpret */
	keysym = (unsigned short) XKeycodeToKeysym(dpy, x11keyode, 0);
	return ( X11KeysymToX10KeycodeMap[ keysym ] );
}
