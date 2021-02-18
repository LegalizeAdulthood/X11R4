/***********************************************************

$Header: StateGet.c,v 3.0 89/11/20 09:25:36 mikew Exp $

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
#include "ICC.h"
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "xdefs.h"
#include "Top.h"
#include "PropUtils.h"
#include "ICCUtils.h"
#include "WinInfo.h"
#include "State.h"
#include "Atoms.h"

static int	PropertiesGet();

/*
 * Description: Retrieve the properties for all the top-level windows.
 * 
 * 
 */

static
Status
AllGetProperties(state, display)
State	       *state;		/* pointer to the state */
Display        *display;	/* pointer to the X server */
{
    WinInfo        *wi;
    WinInfo        *nextWi;

    for (wi = state->topList; wi != NULL; wi = nextWi) {
	nextWi = wi->next;
	if (PropertiesGet(wi, display) != SM_SUCCESS) {
	    StateRemoveWinInfo(state, wi);
	    wiFree(wi);
	}
    }
    return (SM_SUCCESS);
}

/*
 * Description: Send WM_SAVE_YOURSELF to all top-level windows who want it.
 * 
 */

static
Status
AllSaveYourself(state, display)
    State	   *state;	/* pointer to the state */
    Display        *display;	/* pointer to the display */
{
    Status	cc;
    Time	time;
    WinInfo    *wi;
    WinInfo    *nextWi;

    time = GetTimeStamp(display, XtWindow(topShell));
    for (wi = state->topList; wi != NULL; wi = nextWi) {
	nextWi = wi->next;
	/*
	 * If the client participates in the WM_SAVE_YOURSELF protocol, send
	 * him the appopriate messages and wait for the proper responses
	 */
	if (!CheckForProtocol(display, wi->rid, wmSaveYourselfAtom))
	  continue;
	state->waitForCount++;
	wi->saveYourselfState = Saving;
	if ((cc = SaveYourself(display, wi->rid, time)) != SM_SUCCESS)
	  return(cc);
    }
    return (SM_SUCCESS);
}

/*
 * Description: Wait For WM_COMMAND to be updated on all windows who received
 *		WM_SAVE_YOURSELF.
 * 
 * Side Effects: 
 */

static
Status 
AllWaitForCommand(state, display)
    StatePtr	    state;	/* pointer to the state */
    Display        *display;	/* pointer to the X server */
{
    WinInfo        *wi;

    while (state->waitForCount > 0) {
	XEvent	event;
	XPropertyEvent *pEvent = (XPropertyEvent *) & event;

	XtNextEvent(&event);
	if (event.type == PropertyNotify && pEvent->atom == XA_WM_COMMAND) {
	    wi = StateGetWinInfoByRid(state, pEvent->window);
	    if (wi && wi->saveYourselfState == Saving) {
		if (XGetCommand(display, wi->rid, &wi->argv, &wi->argc) != 1) {
		    StateRemoveWinInfo(state, wi);
		    wiFree(wi);
		}
		else
		  wi->saveYourselfState = Saved;
		state->waitForCount--;
	    }
	}
	XtDispatchEvent(&event);
	XSelectInput(display, wi->rid, NoEventMask);
    }
    return (SM_SUCCESS);
}

/*
 * Description: Gather up the current state
 * 
 * Side Effects: Can cause the clients to update WM_COMMAND 
 *
 */

int 
StateGet(state, display)
    State      *state;		/* pointer to the state */
    Display    *display;	/* pointer to the X server */
{
    /* Get a list of children of the root window. */
    if (GetTopLevelWindows(display, state))
	return (1);
    AllSaveYourself(state,display);
    AllGetProperties(state,display);
    AllWaitForCommand(state,display);
    return (0);
}


/*
 * Description:	Compute the geometry argument for a command based on its
 *		window attributes, parent attributes, and WM_NORMAL_HINTS.
 *
 */

static
void
ComputeGeometry(attr, parentAttr, wmNormalHints, x, y, height, width)
    XWindowAttributes	*attr;	/* window attributes*/
    XWindowAttributes	*parentAttr; /* parent window attributes */
    XSizeHints      	*wmNormalHints;	/* contents of WM_NORMAL_HINTS */
    int			*x;	/* returned x location */
    int			*y;	/* returned y location */
    int			*width;	/* returned width */
    int			*height; /* returned height */
{
    *width = attr->width;
    *height = attr->height;
    *x = parentAttr->x + parentAttr->border_width;
    *y = parentAttr->y + parentAttr->border_width;
    /*
     * The width in pixels is base_width + (width * width_inc).	
     * The height in pixels is base_height + (width * height_inc).
     * Xlib current doesn't know about Base Size.
     */
    if (wmNormalHints->flags & PBaseSize) {
	*width -= wmNormalHints->base_width;
	*height -= wmNormalHints->base_height;
    } else if (wmNormalHints->flags & PMinSize) {
	*width -= wmNormalHints->min_width;
	*height -= wmNormalHints->min_height;
    }
    if (wmNormalHints->flags & PResizeInc) {
	*width = *width / wmNormalHints->width_inc;
	*height = *height / wmNormalHints->height_inc;
    }
    return;
}

/*
 * Description: Get the properties of a top-level window.
 * 
 * Side Effects: Updates many properties in the WinInfo structure.
 */

static
Status
PropertiesGet(wi, display)
    Display        *display;	/* pointer to the X server */
    WinInfo        *wi;		/* pointer to window information */
{
    Pixmap         	iconPixmap;
    unsigned char      *fakeargv[1];
    XWindowAttributes	attr;
    unsigned char      *wmIconName = NULL;
    unsigned char      *wmName = NULL;
    XWindowAttributes	parentAttr;
    XSizeHints      	wmNormalHints;

    XGetWindowAttributes(display, wi->rid, &attr);
    XGetWindowAttributes(display, wi->parentRid, &parentAttr);
    if (XGetStandardProperties(display, wi->rid, &wmName, &wmIconName,
			       &iconPixmap, &wi->argc, &wi->argv,
		       &wmNormalHints, &wi->wmClientMachine) != 1)
	return (SM_FAILURE);
    ComputeGeometry(&attr, &parentAttr, &wmNormalHints, 
		    &wi->x, &wi->y, &wi->height, &wi->width);
    if (wmIconName) 
	XFree(wmIconName);
    /* If the guy doesn't have a WM_COMMAND property, we can't restart him. */
    if (!wi->argv != NULL || !wi->argv[0]) {
	if (!wmName)
	    return (SM_FAILURE);
	wi->argv = fakeargv;
	fakeargv[0] = wmName;
	wi->argc = 1;
    }
    if (wmName)
	XFree(wmName);
    (void) XGetClassHint(display, wi->rid, &wi->wmClass);
    if ( XGetWMState(display, wi->rid, &wi->wmState) != 1)
	return (SM_FAILURE);
    if (wi->wmState.state == WithdrawnState)
	return (SM_FAILURE);
    return (SM_SUCCESS);
}
