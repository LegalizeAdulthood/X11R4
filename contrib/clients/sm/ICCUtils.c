/***********************************************************

$Header: ICCUtils.c,v 3.0 89/11/20 09:24:43 mikew Exp $

Copyright 1989 by Mike Wexler, Santa Clara, Ca.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Mike Wexler or not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

MIKE WEXLER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#include "SM.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include "Atoms.h"
#include "ICC.h"
#include "ICCUtils.h"
#include "xdefs.h"
#include "WinInfo.h"
#include "State.h"

/*
 * Description: Given a window that is child of the RootWindow, find the
 * 		corresponding top level window.
 * 
 */

static
Window 
GetTopLevelWindow(display, window)
    Display        *display;	/* pointer to the X server */
    Window          window;	/* a window that is either a top-level window
				   or an ancestor of one. */
{
    Window          parent, root, *children;
    Atom            actualType;
    int             actualFormat;
    unsigned long   nitems;
    unsigned long   leftover;
    unsigned int    nchildren;
    WM_STATE        wmState;
    XWindowAttributes windowAttr;

    XGetWindowAttributes(display, window, &windowAttr);
    XGetWindowProperty(display, window, wmStateAtom, 0L, (long) BUFSIZ, False,
		       wmStateAtom, &actualType, &actualFormat, &nitems,
		       &leftover, (unsigned char **) &wmState);
    if (actualType == wmStateAtom && !windowAttr.override_redirect)
	return (window);
    else {
	XQueryTree(display, window, &root, &parent, &children, &nchildren);
	while (nchildren--) {
	    if (window = GetTopLevelWindow(display, *children++))
		return (window);
	}
	return ((Window) 0);
    }
}

/*
 * Description: This functions get a time stamp by update a property and then
 * 		extracting the time stamp for the property notify.
 * 
 * Outputs:	Time - the timestamp
 * 
 */

Time 
GetTimeStamp(display, window)
    Display        *display;	/* pointer to the X server */
    Window          window;	/* the window to put the property on */
{
    unsigned long  *data = NULL;
    XEvent          event;
    XPropertyEvent *pEvent = (XPropertyEvent *) & event;

    /* do a zero length append to WM_PROTOCOLS to get a time stamp. */
    XSelectInput(display, window, PropertyChangeMask);
    XChangeProperty(display, window, wmProtocolsAtom, XA_ATOM, 32,
	 	    PropModeAppend, data, 0);
    for (;;) {
	XtNextEvent(&event);
	if (event.type == PropertyNotify && pEvent->window == window &&
	    pEvent->atom == wmProtocolsAtom)
	    break;		/* got the append */
	XtDispatchEvent(&event);
    }
    XSelectInput(display, window, NoEventMask);
    return (pEvent->time);
}

/*
 * Description:	Check a window to see if it supports WM_SAVE_YOURSELF.
 * 
 * Outputs:	Boolean - True = protocol supported, False = not supported
 * 
 */

Boolean
CheckForProtocol(display, window, protocol)
Display        *display;	/* pointer to the X server */
Window          window;		/* the window to be checked */
Atom            protocol;	/* protocol to check for */
{
    int             i;
    Atom *protocols;
    unsigned long numProtocols;

    protocols = NULL;
    /* Get the WM_PROTOCOLS property on the clients top-level window. */
    if (XGetWMProtocols(display, window, &protocols, &numProtocols) != True)
	return (False);		/* If the client doesn't have a WM_PROTOCOLS
				 * property,	 */
    /* Look for protocols atom.  */
    if (protocols != NULL)
	    for (i = 0; i < numProtocols; ++i)
		if (protocols[i] == protocol)
		    return(True);
    return (False);		/* doesn't participate in WM_SAVE_YOURSELF */
}

/*
 * Description:	This function sends the SAVE_YOURSELF message to a window.
 * 
 * Side Effects: Causes the client to update its WM_COMMAND Property.
 *
 */

Status
SaveYourself(display, window, time)
    Display        *display;	/* pointer to the X server */
    Window          window;	/* window to send WM_SAVE_YOURSELF to */
    Time            time;	/* timestamp of event causing the save */
{
    Status		cc;
    XClientMessageEvent saveYourselfMessage;

    /* Construct the ClientMessage. */
    saveYourselfMessage.type = ClientMessage;
    saveYourselfMessage.window = window;
    saveYourselfMessage.message_type = wmProtocolsAtom;
    saveYourselfMessage.format = 32;
    saveYourselfMessage.data.l[0] = wmSaveYourselfAtom;
    saveYourselfMessage.data.l[1] = time;

    /* look for changes in WM_COMMAND property */
    XSelectInput(display, window, PropertyChangeMask);	

    if ((cc = XSendEvent(display, window, False, NoEventMask,
	 &saveYourselfMessage)) != 1) {
	Warning("SaveYourself", "XSendEvent failed to send client message.\n");
	return (cc);
    }
    return (SM_SUCCESS);
}


/*
 * Description: Get a list of all the top-level windows.
 * 
 */

Status
GetTopLevelWindows(display, state)
    Display        *display;	/* pointer to the X server */
    State          *state;	/* anchor for the list of windows */
{
    Window          rootWindow, *childList, parentWindow;
    int             childListLen;
    WinInfo        *wi;
    int             i;

    /* get a list of children of the root window */
    if (XQueryTree(display, DefaultRootWindow(display), &rootWindow, 
		   &parentWindow, &childList, &childListLen) != 1)
	Fatal("GetTopLevelWindows", "Can't query window tree");
    /* scan list */
    for (i = 0; i < childListLen; ++i, childList++) {
	Window          topLevelWindow;

	wi = wiAlloc();
	topLevelWindow = GetTopLevelWindow(display, *childList);
	if (!topLevelWindow)
	    topLevelWindow = (*childList);
	wi->rid = topLevelWindow;
	wi->parentRid = *childList;
	StateAddWinInfo(state, wi);
    }
    return (SM_SUCCESS);
}
