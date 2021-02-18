/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) AdShineMark.c 5.1 89/12/14 Crucible";
#include <errno.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include "menu.h"
#include "olwm.h"
#include <olgx/olgx.h>

#define windowmark_width 16
#define windowmark_height 15

#define in_windowmark(x,y) \
	( (x) >= 0 && (y) >= 0 && \
	  (x) <= windowmark_width && (y) <= windowmark_height \
	)
	

/* events in the adornment window that are interesting */
#define SHINE_EVENT_MASK        (ButtonPressMask | ButtonReleaseMask | ExposureMask)

extern int	FocusFollowsMouse;
extern Cursor	MoveWinCursor;

extern Graphics_info *olgx_gismisc;

/* 
 * MakeShineMark-- create the Shine Mark window with the passed title bar window
 *	as parent.  Return WinInfo structure.
 */
WinInfo *
MakeShineMark(dpy, title, frame)
Display	*dpy;
Window	title;
WinInfo *frame;
{
	WinInfo			*winInfo;
	Window			newWin;
	unsigned long		valueMask = 
		CWEventMask | CWBorderPixel | CWBackPixel | CWWinGravity;
	XSetWindowAttributes	attributes;
	int			DrawShineMark(), EventShineMark();
	int			DestroyShineMark(), FocusShineMark();

	/* if this window is a property or help window we want to know when
	 * the cursor leaves the window so we can cancel warps
	 */
	if ((frame->winType == PROPWINDOW) || (frame->winType == HELPWINDOW))
		attributes.event_mask = SHINE_EVENT_MASK | LeaveWindowMask;
	else
		attributes.event_mask = SHINE_EVENT_MASK;

	attributes.border_pixel = borderColor;
	attributes.background_pixel = bg1Color;
	attributes.win_gravity = WestGravity;

	newWin = XCreateWindow(dpy, title,
			 SHINEOFF, 0,
			 windowmark_width, windowmark_height,
			 0,
			 DefaultDepth(dpy, DefaultScreen(dpy)),
			 CopyFromParent,
			 DefaultVisual(dpy, DefaultScreen(dpy)),
			 valueMask,
			 &attributes);
	XMapWindow(dpy, newWin);
	winInfo = WICreateInfo(newWin);
	winInfo->eventfunc = EventShineMark;
	winInfo->focusfunc = FocusShineMark;
	winInfo->frame = frame;
	winInfo->destroyfunc = DestroyShineMark;

	/* if this window is a property or help window then warps are to the 
	 * shine mark */
	if ((frame->winType == PROPWINDOW) || (frame->winType == HELPWINDOW))
		/* set this frame's warp window to us */
		frame->warpWin = newWin;

	return winInfo;
}



/*
 * DrawShineMark -- draw the shine mark in the title bar
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
static
DrawShineMark(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	olgx_draw_abbrev_button(olgx_gismisc, winInfo->self, 0, 0, OLGX_NORMAL);
}

/*
 * EventShineMark --  handle events on the shine mark window.  If pressed
 *	go iconic.
 */
static
EventShineMark(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinInfo	*winInfo;
{
	WMDecorations	*winDecors = GetDecors(winInfo->frame);
	XEvent		clientEvent;
	int		x, y;
	static Bool	active = False;

	switch (event->type)
	{
	case LeaveNotify:
		winInfo->frame->warpBack = False;
		break;

	case Expose:
		DrawShineMark(dpy, winInfo);
		break;

	case ButtonPress:
		olgx_draw_abbrev_button(olgx_gismisc, winInfo->self, 0, 0, 
			OLGX_INVOKED);
		switch (MouseButton(dpy, event))
		{
		case MB_ADJUST:
			return;

		case MB_MENU:
			if (WINDOWMENU(winInfo->frame))
				MenuShow(dpy, winInfo->frame, 
					WINDOWMENU(winInfo->frame), 
					event->xbutton.x_root, 
					event->xbutton.y_root);
			olgx_draw_abbrev_button(olgx_gismisc, winInfo->self,
						0, 0,
						OLGX_NORMAL | OLGX_ERASE);
			return;

		}
		XGrabPointer(dpy, winInfo->self,
		     False, 
		     (ButtonReleaseMask | ButtonPressMask | PointerMotionMask),
		     GrabModeAsync, GrabModeAsync,
		     None,
		     MoveWinCursor,
		     CurrentTime);
		active = True;
		break;

	case ButtonRelease:

		/* REMIND: why is this necessary?  The pointer should have
		 * been re-grabbed with GrabModeAsync, thus deactivating
	         * the grab upon the release of the button. */
		XUngrabPointer(dpy,event->xbutton.time);

		x = event->xbutton.x;
		y = event->xbutton.y;

		active = False;
		olgx_draw_abbrev_button(olgx_gismisc, winInfo->self, 0, 0, 
			OLGX_NORMAL | OLGX_ERASE);

		if (!in_windowmark(x,y))
		{
			return;
		}

		/* we want to do the default action of the menu for this
		 * window.  This will either be Iconify if the window is
		 * a base window, or dismiss if not */
		switch(winDecors->menu_type)
		{
			case MENU_FULL:
				Iconify(dpy, winInfo);
				break;

			case MENU_NONE:
			case MENU_LIMITED:
				/*
				 * Send a WM_DELETE_WINDOW event.
				 */
				clientEvent.xclient.type = ClientMessage;
				clientEvent.xclient.message_type = 
					AtomDeleteWindow;
				clientEvent.xclient.format = 32;
				clientEvent.xclient.display = dpy;
				clientEvent.xclient.window = 
					GetPane(winInfo->frame);

				/* send the event */
				XSendEvent(dpy, 
					   clientEvent.xclient.window, 
					   False,
					   NoEventMask, 
					   &clientEvent);
				break;
		}
		break;
	
	case MotionNotify:
		x = event->xmotion.x;
		y = event->xmotion.y;
		if ( active && !in_windowmark(x,y) ) {
		    olgx_draw_abbrev_button(olgx_gismisc, winInfo->self, 0, 0, 
			OLGX_NORMAL | OLGX_ERASE);
		    active = False;
		} else if ( !active && in_windowmark(x,y) ) {
		    olgx_draw_abbrev_button(olgx_gismisc, winInfo->self, 0, 0, 
			OLGX_INVOKED);
		    active = True;
		}
		break;

	default:
		if (PrintOrphans)
			DebugEvent(event, "ShineMark");
		break;
	}
}

/*
 * DestroyShineMark -- destroy the shine mark window and free any allocated
 *	data.
 */
static 
DestroyShineMark(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	XDestroyWindow(dpy, winInfo->self);
	WIDeleteInfo(winInfo->self);
}

/*
 * FocusShineMark --- reverse the focus of the shine mark
 */
/*ARGSUSED*/	/* setFocus arg will be used later */
static 
FocusShineMark(dpy, winInfo, setFocus)
Display	*dpy;
WinInfo	*winInfo;
Bool	setFocus;
{
 	DrawShineMark(dpy, winInfo); 
}
