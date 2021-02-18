/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) RootWin.c 5.1 89/12/14 Crucible";
#include <errno.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "menu.h"
#include "olwm.h"

extern Menu *RootMenu;
extern int	FocusFollowsMouse;
extern Window	NoFocusWin;
extern Colormap	CurrColormap;
extern Window	CurrColormapWindow;
extern KeyCode	FrontKeyCode;
extern KeyCode	HelpKeyCode;
extern KeyCode	OpenKeyCode;
extern Time	SelectionTime;
  
extern void SelectionResponse();

/* This is a global variable because TraceBoundingBox changes its value
 * to a nonexistant button, (Button5), when it is entered.  There is a
 * bug (in the X11R3 server?) which can allow a MotionNotify event to come
 * through when the button is up even though we've done an XSelectInput
 * with the Button1MotionMask.  If this bug gets fixed, move this variable
 * inside rootEvent and make it static.  Also, TraceBoundingBox would not
 * need to touch it any more.
 */
int	lastButton;
static void rootKeystroke();
static void RedistributeKeystroke();

rootEvent(dpy, pEvent, winInfo)
	Display	*dpy;
	XEvent	*pEvent;
	WinInfo	*winInfo;
{
	XWindowChanges		winChange;
	WinInfo			*frameWinInfo = (WinInfo *)0;
	int			AddSelection();
	short			boxX, boxY, boxW, boxH;
	XSelectionEvent		se;
static	XEvent			buttonPressEvent;
	Time			timestamp;

#define MapEvent	(pEvent->xmap)
#define ConfEvent	(pEvent->xconfigurerequest)

	switch (pEvent->type)
	{
	case EnterNotify:
		InstallColormap(dpy,
			RootWindow(dpy,DefaultScreen(dpy)),
			DefaultColormap(dpy,DefaultScreen(dpy)));
	break;

	case ConfigureRequest:
		if ((winInfo = WIGetInfo(ConfEvent.window)) == (WinInfo *)0)
		{
			/* We don't know about this window
			 * convert the request into an XConfigureWindow
			 * call. We do not look at the hints to see if
			 * the resize is in the proper increments, but since
			 * the app is asking for the reconfigure this seems
			 * right.
			 */
			winChange.x = ConfEvent.x;
			winChange.y = ConfEvent.y;
			winChange.width = ConfEvent.width;
			winChange.height = ConfEvent.height;
			winChange.border_width = ConfEvent.border_width;
			winChange.stack_mode = ConfEvent.detail;
			winChange.sibling = ConfEvent.above;

			XConfigureWindow(dpy,
				 ConfEvent.window,
				 /* lint will warn: this is a long, not int */
				 ConfEvent.value_mask,
				 &winChange );
			break;
		}

		/* if it is the same size and shape, do nothing */
#ifdef NOT_NOW
		if ((winInfo->x == ConfEvent.x) &&
		    (winInfo->y == ConfEvent.y) &&
		    (winInfo->width == ConfEvent.width) &&
		    (winInfo->height == ConfEvent.height)) 
#endif /* NOT_NOW */

		/*
		 * REMIND
		 * This section should really be reworked.  If we receive
		 * a ConfigureRequest event on _any_ Withdrawn window, we 
		 * should honor it.  When and if the window is remapped, the 
		 * new state of the window should be propagated to the frame.  
		 * That is, if the window was resized while it was unmapped, 
		 * any frame associated with it should be resized accordingly 
		 * when the window is moved out of the Withdrawn state by the 
		 * client.
		 */
		if (!(ConfEvent.value_mask & (CWX | CWY | CWWidth | CWHeight)))
			break;

		/* calculate the new frame */
		if (ConfEvent.value_mask & CWX)
			winInfo->x = ConfEvent.x;
		if (ConfEvent.value_mask & CWY)
			winInfo->y = ConfEvent.y;
		if (ConfEvent.value_mask & (CWWidth | CWHeight))
			GetFramedSize(winInfo->frame, 
				ConfEvent.width, ConfEvent.height);

		/* configure the window and its frame */
		(*(winInfo->frame)->configfunc)(dpy,winInfo->frame);
		break;

	case MapRequest:
		CheckAndMapWin(dpy, MapEvent.window, False, False);
		break;
	
	case MotionNotify:
		/* If the user hasn't moved more than the threshold
		 * amount, break out of here.
		 */
		if ((ABS(pEvent->xmotion.x - buttonPressEvent.xbutton.x) < 
		     MOVETHRESH) &&
		    (ABS(pEvent->xmotion.y - buttonPressEvent.xbutton.y) < 
		     MOVETHRESH))
		    break;

		switch(lastButton)
		{
		case MB_SELECT:	
			ClearSelections(dpy);

			/* need to trace a bounding box */
			timestamp = TraceBoundingBox(dpy, &buttonPressEvent, 
				&boxX, &boxY, &boxW, &boxH);

			/* select all frames or icons in the box */
			while(frameWinInfo = EnumFrames(frameWinInfo))
			{
				FrameInBox(dpy, frameWinInfo, 
					AddSelection,
					boxX, boxY,
					boxW, boxH,
					timestamp);
			}
			break;

		case MB_ADJUST:	
			/* bounding box with toggles */
			timestamp = TraceBoundingBox(dpy, &buttonPressEvent, 
				&boxX, &boxY, &boxW, &boxH);

			while(frameWinInfo = EnumFrames(frameWinInfo))
			{
				/* toggle all frames in the box */
				FrameInBox(dpy, frameWinInfo, 
					ToggleSelection,
					boxX, boxY,
					boxW, boxH,
					timestamp);
			}
			break;
		}
		break;

	case ButtonRelease:
		/* This only happens if we did NOT get a motion notify
		 * after the last button press. 
		 */
		if (MouseButton(dpy, pEvent) == MB_SELECT)
		{
			/* Clear the selection list. */
			ClearSelections(dpy);

			/* Set the input focus to the root window. */
			XSetInputFocus(dpy, NoFocusWin, RevertToPointerRoot,
				       CurrentTime);

			/* Tell the server that we are now the current
			 * selection owner.  This might be necessary if
			 * the user had selected some text in an xterm
			 * and wants to unselect it.  S/he does this by
			 * clicking in the root window.
			 */
			 XSetSelectionOwner(dpy, XA_PRIMARY,
					    RootWindow(dpy, DefaultScreen(dpy)),
				            pEvent->xbutton.time);
			 SelectionTime = pEvent->xbutton.time;
		}
		break;
		
	case ButtonPress:
		buttonPressEvent = *pEvent;

		InstallColormap(dpy,
			RootWindow(dpy,DefaultScreen(dpy)),
			DefaultColormap(dpy,DefaultScreen(dpy)));

		/* set lastButton */
		if ((lastButton = MouseButton(dpy, pEvent)) == MB_MENU)
		{
			/* bring up menu for root */
			MenuShow(dpy, winInfo, RootMenu,
				 pEvent->xbutton.x_root,
				 pEvent->xbutton.y_root);
		}
		break;

	case SelectionNotify:
		/* We get this event from the owner of the selection
		 * if we had made a XConvertSelection call.  At the
		 * moment we never make a XConvertSelction call
		 * so we don't have to worry about a SelectionNotify
		 * event.  An XCovertSelection call is used to get
		 * the current selection.
		 */
		break;

	case SelectionRequest:
		SelectionResponse((XSelectionRequestEvent *)pEvent);
		break;

	case SelectionClear:
		/*
		 * Someone else has asked to be the selection owner.
		 * Clear our list of selected windows.
		 */
		ClearSelections(dpy);
		break;

 	case KeyPress:
 	case KeyRelease:
 	    rootKeystroke( dpy, pEvent );
 
 	    break;

	default:
		if (PrintOrphans)
			DebugEvent(pEvent, "RootWin");
		break;
	}
}


static void
rootKeystroke(dpy,pEvent)
    Display		*dpy;
    XEvent		*pEvent;
{
    Window		child;
    WinInfo		*childInfo;
    Bool		samescreen;
    int			destX, destY;
    KeyCode		kc;

    samescreen = XTranslateCoordinates(dpy,
	DefaultRootWindow(dpy), DefaultRootWindow(dpy),
	pEvent->xkey.x, pEvent->xkey.y, &destX, &destY, &child );

    if ( !samescreen || child == None )
	return;

    kc = pEvent->xkey.keycode;
    if ( kc == FrontKeyCode && pEvent->type == KeyPress ) {
	XWindowChanges xwc;
	xwc.stack_mode = Opposite;
	XConfigureWindow( dpy, child, CWStackMode, &xwc );
    } else if ( kc == HelpKeyCode ) {
	RedistributeKeystroke( dpy, pEvent );
    } else if ( kc == OpenKeyCode && pEvent->type == KeyPress ) {
	childInfo = WIGetInfo(child);
	if ( childInfo ) {
	    if ( childInfo->winState == Iconic ) {
		DeIconify(dpy,childInfo->frame);
	    } else {
		/* check for base vs. popup window */
		if (GetLeader(childInfo) != NULL) {
		    /*
		     * REMIND: passing (Menu *)NULL is bogus.
		     * Also, should pass DA_THIS instead of 0.
		     */
		    DismissAction(dpy,childInfo,(Menu *)NULL,0,False);
		} else {
		    Iconify(dpy,childInfo);
		}
	    }
	}
    }
}


static void
RedistributeKeystroke( dpy, e )
    Display *dpy;
    XEvent *e;
{
    XKeyEvent *ke = (XKeyEvent *) e;
    Window childwin, dstwin, srcwin;
    int srcx, srcy, dstx, dsty;
    static Bool pressreceived = False;
    static XKeyEvent pressevent;

    if ( ke->type == KeyPress ) {
	if ( pressreceived == False ) {
	    srcwin = ke->window;
	    dstwin = ke->window;
	    srcx = ke->x;
	    srcy = ke->y;
	    while (1) {
		XTranslateCoordinates( dpy, srcwin, dstwin, srcx, srcy,
		    &dstx, &dsty, &childwin );
		if ( childwin == None )
		    break;
		srcx = dstx;
		srcy = dsty;
		srcwin = dstwin;
		dstwin = childwin;
	    }
	    pressevent = *ke;
	    pressevent.x = dstx;
	    pressevent.y = dsty;
	    pressevent.window = dstwin;
	    pressevent.subwindow = None;
	    pressreceived = True;
	}
    } else {
	if ( pressreceived == True ) {
#ifdef notdef
	    XUngrabKeyboard( dpy, pressevent.time );
#endif /* notdef */
	    XSendEvent( dpy, pressevent.window, True, 
		KeyPressMask, &pressevent );
	    XTranslateCoordinates( dpy, ke->window, pressevent.window,
		ke->x, ke->y, &dstx, &dsty, &childwin );
	    ke->x = dstx;
	    ke->y = dsty;
	    ke->window = pressevent.window;
	    ke->subwindow = None;
	    XSendEvent( dpy, pressevent.window, True, KeyPressMask, ke );
	}
	pressreceived = False;
    }
}
