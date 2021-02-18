/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) AdPushPin.c 5.1 89/12/14 Crucible";
#include <errno.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include "menu.h"
#include "olwm.h"
#include <olgx/olgx.h>

#include "bitmaps/pushpin.h"

#define	PUSHPIN_IN	1L
#define	PUSHPIN_OUT	0L

/* Events in the adornment window that are interesting. */
#define PUSHPIN_EVENT_MASK        (Button1MotionMask | ButtonReleaseMask | \
				   ButtonPressMask | ExposureMask)

extern 	GC	DrawReverseGC;
extern 	GC	DrawNormalGC;
extern	int	FocusFollowsMouse;

extern Graphics_info *olgx_gismisc;

/* 
 * MakePushPin-- create the push-pin window with the passed title bar window
 *	as parent.  Return WinInfo structure.
 */
/*ARGSUSED*/	/* dpy, width args will be used later */
WinInfo *
MakePushPin(dpy, title, width, frame)
Display	*dpy;
Window	title;
int	width;
WinInfo	*frame;
{
	WinInfo			*winInfo;
	Window			newwin;
	unsigned long		valuemask = 
		CWEventMask | CWBorderPixel | CWBackPixel | CWWinGravity;
	XSetWindowAttributes	attributes;
	int			DrawPushPin();
	int			EventPushPin();
	int			DestroyPushPin();
	WMDecorations		*winDecors;

	attributes.event_mask = PUSHPIN_EVENT_MASK;
	attributes.border_pixel = borderColor;
	attributes.background_pixel = bg1Color;
	attributes.win_gravity = WestGravity;

	newwin = XCreateWindow(dpy, title,
			 PUSHOFF, 1,
			 pushpin_width, pushpin_height-1,
			 0,
			 DefaultDepth(dpy, DefaultScreen(dpy)),
			 CopyFromParent,
			 DefaultVisual(dpy, DefaultScreen(dpy)),
			 valuemask,
			 &attributes);

	XMapWindow(dpy, newwin);
	winInfo = WICreateInfo(newwin);
	winInfo->drawfunc = DrawPushPin;
	winInfo->frame = frame;
	winInfo->eventfunc = EventPushPin;
	winInfo->destroyfunc = DestroyPushPin;

	/* Determine initial state of push pin. */
	winDecors = GetDecors(frame);
	if (winDecors->pushpin_initial_state == PinIn)
		winInfo->priv.val = 1;
	else
		winInfo->priv.val = 0;

	/* Register the push-pin state. */
	XChangeProperty(dpy, GetPane(winInfo->frame), 
			AtomPushpinState, 
			XA_INTEGER, 32, 
			PropModeReplace, 
			(unsigned char *)&(winInfo->priv.val), 1);
	return winInfo;
}


/*
 * DrawPushPin -- draw the pushpin.  Pushpin state is in private data
 *	in WinInfo structure.
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
static
DrawPushPin(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	if (winInfo->priv.val == PUSHPIN_IN)
	{
		/* If the titlebar is in reverse video we need to
		 * draw the pushpin in reverse video also.
		 */
		if ((CheckFocus(winInfo->frame)) &&
		    (!FocusFollowsMouse))
		{
			olgx_draw_pushpin(olgx_gismisc, winInfo->self, 0, 0,
				OLGX_PUSHPIN_IN | OLGX_ERASE);
		}
		else
		{
			olgx_draw_pushpin(olgx_gismisc, winInfo->self, 0, 0,
				OLGX_PUSHPIN_IN | OLGX_ERASE);
		}
	}
	else
	{
		/* If the titlebar is in reverse video we need to
		 * draw the pushpin in reverse video also.
		 */
		if ((CheckFocus(winInfo->frame)) &&
		    (!FocusFollowsMouse))

		{
			olgx_draw_pushpin(olgx_gismisc, winInfo->self, 0, 0,
				OLGX_PUSHPIN_OUT | OLGX_ERASE);
		}
		else
		{
			olgx_draw_pushpin(olgx_gismisc, winInfo->self, 0, 0,
				OLGX_PUSHPIN_OUT | OLGX_ERASE);
		}
	}
}


/*
 * EventPushPin -- deal with an event in a push-pin window
 */
static
EventPushPin(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinInfo	*winInfo;
{

	static long	pushpinStateAfterPress;  /* State of the pushpin 
						  * after the user pressed 
						  * the mouse button. */
	Bool		fInWindow;

	switch(event->type)
	{
	case ButtonPress:
		switch (MouseButton(dpy, event))
		{
		case MB_SELECT:
			locallyChangePushPinState(dpy, winInfo, 
				(winInfo->priv.val==PUSHPIN_IN)?
					PUSHPIN_OUT:PUSHPIN_IN);
			pushpinStateAfterPress = winInfo->priv.val;

			break;  /* Break out of case MB_SELECT */
		}
		break;

	case ButtonRelease:
		switch (MouseButton(dpy, event))
		{
		case MB_SELECT:
			changePushPinState(dpy, event, winInfo);
			break;  /* Break out of case MB_SELECT */
		}
		break;

	case Expose:
		DrawPushPin(dpy, winInfo);
		break;

	case MotionNotify:
#ifdef WRONG
		/* If the user pressed MB_SELECT to change the pushpin
		 * from in to out and is still holding the button down, 
		 * we want to leave the pushpin out.  So, break out if 
		 * that's the case.  If we didn't do this we would end
		 * up putting the pushpin back in if the user moved
		 * around in the pushpin window.
		 */
		if (pushpinStateAfterPress == PUSHPIN_OUT)
			break;
#endif

		/* When the user moves the cursor off the pushpin
		 * while s/he has the button down we should pull 
		 * the button out.  If the user moves back onto the
		 * pushpin put the pin back in.  So,
		 *
		 * if (cursor is off the pushpin) and (pin is in)
		 * 	            OR
		 *    (cursor is on the pushpin) and (pin is out)
		 * then
		 *	change the state of the pushpin.
		 */
		fInWindow = !((event->xmotion.x < 0) || 
		     (event->xmotion.y < 0) || 
		     (event->xmotion.x >= pushpin_width) ||
		     (event->xmotion.y >= pushpin_height));
		locallyChangePushPinState(dpy, winInfo, 
			fInWindow ?
				pushpinStateAfterPress :
				((pushpinStateAfterPress == PUSHPIN_IN)?
					PUSHPIN_OUT:PUSHPIN_IN));

		/* Maybe we should do this by getting LeaveNotify events
		 * and seeing if MB_SELECT is pressed.  If it is, then we
		 * know the user left the pushpin with the button pressed???
		 */
		break;

	default:
		if (PrintOrphans)
			DebugEvent(event, "PushPin"); 
		break;
	}
}


/*
 * DestroyPushPin -- destroy the push pin window and free any allocated
 *	data.
 */
static 
DestroyPushPin(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	XDestroyWindow(dpy, winInfo->self);
	WIDeleteInfo(winInfo->self);
}

locallyChangePushPinState(dpy,winInfo,newState)
Display *dpy;
WinInfo *winInfo;
long newState;
{
	if (winInfo->priv.val != newState)
	{
		winInfo->priv.val = newState;
		(*winInfo->drawfunc)(dpy, winInfo);
	}
}

/*ARGSUSED*/	/* event arg will be used later */
changePushPinState(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinInfo	*winInfo;
{
	XEvent		clientEvent;
	FrameInfo	*frameInfo;

	(*winInfo->drawfunc)(dpy, winInfo);

	/* Tell the client that the state of its push-pin
	 * has changed.
	 */
	XChangeProperty(dpy, GetPane(winInfo->frame), 
			AtomPushpinState, 
			XA_INTEGER, 32, 
			PropModeReplace, 
			(unsigned char *)&(winInfo->priv.val), 1);

	/* If the client has WM_DELETE_WINDOW in
	 * its WM_PROTOCOLS, then we should send 
	 * a WM_DELETE_WINDOW client message.
	 */
	frameInfo = (FrameInfo *)(winInfo->frame->priv.ptr);

	if ((frameInfo->protocols & DELETE_WINDOW) &&
	    (winInfo->priv.val == PUSHPIN_OUT))
	{
		clientEvent.xclient.type = ClientMessage;
		clientEvent.xclient.message_type = AtomProtocols;
		clientEvent.xclient.format = 32;
		clientEvent.xclient.display = dpy;
		clientEvent.xclient.window = GetPane(winInfo->frame);
		clientEvent.xclient.data.l[0] = AtomDeleteWindow;

		/* This should really be the time of *event. */
		clientEvent.xclient.data.l[1] = CurrentTime;

		/* Send the event. */
		XSendEvent(dpy,
			   clientEvent.xclient.window,
			   False,
			   NoEventMask,
			   &clientEvent);

	}  /* End if window has DELETE_WINDOW protocol */
	/* Else just unmap the window. */
	/* ???
	else
		XUnmapWindow(dpy, clientEvent.xclient.window);
	*/
}  /* End changePushPinState */
