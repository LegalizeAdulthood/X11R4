/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) CheckMap.c 5.1 89/12/14 Crucible";
/*
 *      this module contains code to check properties of newly
 *	recognized windows and save them away.
 */

#include <errno.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "menu.h"
#include "olwm.h"

extern int	DefScreen;

extern void	*IntMalloc();
extern void	SetWMState();
extern Bool	GetWMState();

/* border width for reparented windows */
#define NORMAL_BORDERWIDTH	0

/* sanity checks for getting stuff out of hints */
#define IsCard16(x) ((x) == ((unsigned short)(x)))
#define IsInt16(x)  ((x) == ((short) (x)))

/*
 * CheckAndMapWin -- check to see if this window needs to be mapped
 *	and if so add the required adornments.
 *		dpy 		-- display pointer
 *		window 		-- client's window
 *		positioned 	-- whether the window is already positioned
 *		ourWindow	-- is this one of our menu windows
 */
WinInfo *
CheckAndMapWin(dpy, window, positioned, ourWindow)
Display		*dpy;
Window          window;
Bool		positioned;
Bool		ourWindow;
{
	FrameInfo	*frameInfo;
	int             winX, winY;
	unsigned int	winWidth, winHeight;
	Window          transient_for;
	Window          winRoot;
	unsigned int    borderWidth, depthReturn;
	WinInfo		*winInfo;
	WinInfo		*paneInfo;
	NewXSizeHints	*normalHints;
	WinInfo		*MakeFrame();
	long		mask;
	Bool		transient = False;
	Atom		actType;
	int		actFormat;
	unsigned long	nItems;
	unsigned long	remain;
	int		status;
	Bool		wmstate_exists = False;
	int		initstate;
	XConfigureEvent	ce;

	/* if it's a transient window, do not adorn it */
	if (XGetTransientForHint(dpy, window, &transient_for))
	{
#ifdef OBSOLETE
		XMapRaised(dpy, window);
		return (WinInfo *)0;
#endif /* OBSOLETE */
		transient = True;
	}

	/* or if this window already has a winInfo structure,
	 * just return the winInfo,
	 * or if this window is not a child of root, or doesn't have the
	 * WM_STATE property, then it will be ignored
	 */
	if (!ourWindow)
	{
		char	*dummy;
		Window	root, parent, *children, iconwin;
		int	nChild;
		

		if (winInfo = WIGetInfo(window))
			return winInfo;
	
		XQueryTree(dpy, window,
			   &root, &parent, &children, &nChild);

		wmstate_exists = GetWMState(dpy, window, &initstate, &iconwin);

		if ( parent != RootWindow(dpy, DefaultScreen(dpy)) )
		    return (WinInfo *)0;
	}

	/* where does the window think it is */
	XGetGeometry(dpy, window,
		     &winRoot,
		     &winX, &winY, &winWidth, &winHeight,
		     &borderWidth,
		     &depthReturn);

	/*
	 * Get the WM_NORMAL_HINTS property.  If it's short, then we have a
	 * pre-ICCCM client on our hands, so we'll have to fill in some of the
	 * missing values.
	 */
	status = XGetWindowProperty(dpy,
			  window,
			  XA_WM_NORMAL_HINTS,
			  0L,
			  LONG_LENGTH(NewXSizeHints),
			  False,
			  XA_WM_SIZE_HINTS,
			  &actType,
			  &actFormat,
			  &nItems,
			  &remain,
			  /* lint will complain about this cast */
			  (unsigned char **)&normalHints);

	if ( status == Success ) {
	    if ( actType != XA_WM_SIZE_HINTS ) {
		/*
		 * We got a property of the wrong type; punt.
		 */
		XFree(normalHints);
		normalHints = (NewXSizeHints *)0;
	    } else if ( nItems == LONG_LENGTH(NewXSizeHints)-3 ) {
		/*
		 * We got a short property.  Assume that this is a pre-X11R4
		 * client who's using the short version of the property.  Copy
		 * the data into a correctly-sized structure.  Then, depending
		 * on the flags set, ignore the window's real geometry and use
		 * the data in the hint (but only if it passes some sanity 
		 * checking).  The sanity checking is necessary because early 
		 * versions of XView write a short property, but rely on the 
		 * window manager to look at the window's geometry instead of 
		 * at the values in the hint.
		 */
		NewXSizeHints *temp;

		temp = (NewXSizeHints *)
		    IntMalloc((unsigned)sizeof(NewXSizeHints));
		bcopy(normalHints,temp,
		      sizeof(NewXSizeHints)-3*sizeof(long));
		XFree(normalHints);
		normalHints = temp;

		if ((normalHints->flags & (USPosition|PPosition))
		    && IsInt16(normalHints->pad.x)
		    && IsInt16(normalHints->pad.y)) {
		    winX = normalHints->pad.x;
		    winY = normalHints->pad.y;
		}
		if ((normalHints->flags & (USSize|PSize))
		    && IsCard16(normalHints->pad.width)
		    && IsCard16(normalHints->pad.height)) {
		    winWidth = normalHints->pad.width;
		    winHeight = normalHints->pad.height;
		}
	    } else if ( nItems != LONG_LENGTH(NewXSizeHints) ) {
		/* wrong length, punt */
		XFree(normalHints);
		normalHints = (NewXSizeHints *)0;
	    }
	}

	if (normalHints &&
	    (normalHints->flags & (USPosition|PPosition)))
	    positioned = True;

	if ( !positioned )
	{
	    /*
	     * The hints don't specify a position, so we choose a suitable
	     * one, taking into account the window size (to prevent the window
	     * from lapping off the screen).  REMIND: CalcPosition should 
	     * return the calculated X and Y in parameters instead of 
	     * returning a single value that is copied to both X and Y.
	     */
	    winX = winY = CalcPosition(dpy, &winWidth, &winHeight);
	}

	/* go ahead and make the frame with adornments */
	winInfo = MakeFrame(dpy, window, 
			winX, winY, winWidth, winHeight,
			ourWindow, normalHints, transient);

	/* Put the window in the save set so it doesn't go away */
	if (!ourWindow)
		XChangeSaveSet(dpy, window, SetModeInsert);

	/* get private data pointer */
	frameInfo = (FrameInfo *)(winInfo->priv.ptr);
	paneInfo = WIGetInfo(frameInfo->pane);

	if (!ourWindow)
	{
		/* on the pane we ask for:
		 * PropertyChangeMask -- for ICCCM functionality
		 * StructureNotifyMask -- to get the UnmapNotify event
		 * FocusChangeMask -- to set the titlebar appropriately
		 * ColormapChangeMask -- to track colormaps accurately
		 * in addition, if the pane has a special colormap
		 * EnterWindowMask -- to install colormaps appropriately
		 *
		 * Note that the wininfo structure for the pane is
		 * created in MakeFrame(), a design flaw that requires
		 * some consideration.
		 */
		if (WIGetInfo(window)->colormap)
			mask = (PropertyChangeMask | StructureNotifyMask |
				FocusChangeMask | ColormapChangeMask |
				EnterWindowMask);
		else
			mask = (PropertyChangeMask | StructureNotifyMask |
				FocusChangeMask | ColormapChangeMask);
		XSelectInput(dpy, window, mask);
	}

	/* change the border width if necessary */
	if (borderWidth != NORMAL_BORDERWIDTH)
	{
		XSetWindowBorderWidth(dpy, window, NORMAL_BORDERWIDTH);
	}

	/*
	 * Send a synthetic ConfigureNotify event to let the client know where
	 * we've moved the window.
	 */

	ce.type = ConfigureNotify;
	ce.serial = 0L;
	ce.event = frameInfo->pane;
	ce.window = frameInfo->pane;
	ce.x = winInfo->x + paneInfo->x - frameInfo->oldBorderWidth;
	ce.y = winInfo->y + paneInfo->y - frameInfo->oldBorderWidth;
	ce.width = paneInfo->width;
	ce.height = paneInfo->height;
	ce.border_width = frameInfo->oldBorderWidth;
	ce.above = None;
	ce.override_redirect = False;

	XSendEvent(dpy, frameInfo->pane, False, StructureNotifyMask,&ce);

	/*
	 * Check to see if we should map in iconic or normal state.  If
	 * the initial_state field is IconicState, and the window is a
	 * base window (!frameInfo->leader) or the window is a popup
	 * window but its base window is iconic, then map this window
	 * into iconic state.
	 */
	if (!wmstate_exists) {
	    if (frameInfo->wmHints && (frameInfo->wmHints->flags & StateHint)
		&& (frameInfo->wmHints->initial_state == IconicState))
		initstate = IconicState;
	    else
		initstate = NormalState;
	}
	if ( frameInfo->leader &&
	     frameInfo->leader->frame->winState != Iconic )
	    initstate = NormalState;

	switch ( initstate ) {
	case NormalState:
	    winInfo->frame->winState = Normal;
	    XMapWindow(dpy, winInfo->self);
	    break;
	case IconicState:
	    Iconify(dpy, winInfo);
	    break;
	}
	SetWMState(dpy, winInfo->frame);

	return winInfo;
}

/*
 * ReparentTree -- called at start up, this routine queries the window
 *	tree and reparents all the windows 
 */
ReparentTree(dpy)
Display	*dpy;
{
	int	ii;
	int	numChildren;
	Window	*children, root, parent;
	XWindowAttributes	winAttr;

	if (XQueryTree(dpy, RootWindow(dpy, DefScreen), &root, &parent,
				      &children, &numChildren)) 
	{
		for (ii = 0; ii < numChildren; ii++) 
		{
			XGetWindowAttributes(dpy, children[ii], &winAttr);
			if((winAttr.map_state != IsUnmapped) && 
				(winAttr.override_redirect != True))
			{
				CheckAndMapWin(dpy, children[ii], True, False);
			}
		}
	}
	if (children)
		XFree(children);
}

/*
 * CalcPosition -- calculate the next position to place a new window.  This
 *	function places all new windows on the diagonal and makes sure that
 *	there is enough room on the screen for the new window's size passed
 *	in w and h.
 */
static
CalcPosition(dpy, w, h)
Display		*dpy;
unsigned int	*w, *h;
{
	static 	int	nextPosition = 0;
	int		stepValue;
	int		retVal;

	/* if the height of the current window is too large ... */
	while (nextPosition + *h > DisplayHeight(dpy, DefaultScreen(dpy)))
	{
		/* if we are already at the upper left corner ... */
		if (nextPosition == 0)
		{
			/* .. then set the height to just fit */
			*h = (unsigned)DisplayHeight(dpy, DefaultScreen(dpy));
			break;
		}
		else
			/* .. try to fit window in upper-left */
			nextPosition = 0;
	}

	/* if the width of the current window is too large to fit... */
	while (nextPosition + *w > DisplayWidth(dpy, DefaultScreen(dpy)))
	{
		/* if we are already at the upper-left corner... */
		if (nextPosition == 0)
		{
			/* ... then set the width to just fit */
			*w = (unsigned)DisplayWidth(dpy, DefaultScreen(dpy));
			break;
		}
		else
			/* ... try to fit window in the upper-left */
			nextPosition = 0;
	}

	stepValue = 30;  /* This should really be based on HEADER_HEIGHT. */

	/* we will return the current position */
	retVal = nextPosition;

	/* calculate the next return value */
	nextPosition = (nextPosition + stepValue ) 
			% DisplayWidth(dpy, DefaultScreen(dpy));

	return retVal;
}
