/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) Select.c 5.1 89/12/14 Crucible";
#include <errno.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include "menu.h"
#include "olwm.h"

#define	SELECTSIZE	50
static	WinInfo	*SelectList[SELECTSIZE];
static	int	NextSelect = 0;

#define	FRAMELISTSIZE	50
static	WinInfo	*FrameList[FRAMELISTSIZE];
static	int	NextFrame = 0;

Time SelectionTime;

/*
 * IsSelected -- returns boolean telling whether or not winInfo is selected.
 *
 * Input:
 *
 *	winInfo	- window which might be on selection list.
 *
 * Output:
 *
 *	Returns True if the window winInfo is on the selection list.
 *	Returns False if it is not.
 *
 * Globals:
 *
 *	SelectList	- The list of selected windows.
 *	NextSelect	- Index of the next available entry in the
 *			  SelectList.
 */
Bool
IsSelected(winInfo)
WinInfo	*winInfo;
{
	register	WinInfo **lastEntry = &SelectList[NextSelect];
	register	WinInfo	**ptr;

	for(ptr = SelectList; ptr < lastEntry; ptr++)
	{
		if (*ptr == winInfo)
			return True;
	}
	return False;
}


/*
 * AddSelection -- add this winInfo to the selection list.  If adjust is
 *	True then we add it winInfo to the list if it is not present, or
 *	delete it if it is.  If adjust is False, we clear the current
 *	Select list and add the winInfo structure .
 *
 * This routine adds a window to the SelectList.  It adds the entry
 * to the last empty location before the last entry in the SelectList.
 * If there are no empty locations before the last entry, it puts the
 * new window after the last entry.  For example, if the SelectList
 * currently has two entries, one at 0, and one at 5, and NextSelect
 * is 6, the new entry would be added at location 4.  NextSelect is
 * really only used as an index of the last possible legal entry.
 *
 * This routine also tells the server it is the current XA_PRIMARY
 * selection owner.
 *
 * Input:
 *
 *	dpy	- Display on which window appears.
 *	winInfo	- The window we are adding to the SelectList.
 *
 * Output:
 *
 *	None.
 *
 * Globals:
 *
 *	SelectList	- The list of selected windows.
 *	NextSelect	- The index of the next available empty
 *			  space. 
 *			  Note:  It is better to think of this
 *			  as the index of the space after the
 *			  the end of the list.
 *	The current selection owner in the server is also adjusted.
 */
AddSelection(dpy, winInfo, timestamp)
Display	*dpy;
WinInfo	*winInfo;
Time timestamp;
{
	register	WinInfo **lastEntry = &SelectList[NextSelect];
	register	WinInfo	**ptr;
	register	WinInfo	**zeroPtr = (WinInfo **)0;
	int		listIsEmpty;

	/* Just to be sure we only select frames. */
	winInfo = winInfo->frame;	

	if (NextSelect == SELECTSIZE)
	{
		/* Our table is full, blow the user off (shouldn't happen). */
		/* ??? Should be error dialog */
		XBell(dpy, 100);
		return;
	}

	listIsEmpty = True;
	/* First look to see if window is already listed.  Keep track
	 * of zero entries.
	 */
	for(ptr = SelectList; ptr < lastEntry; ptr++)
	{
		if (*ptr == winInfo)
			return;
		else if (*ptr == (WinInfo *)0)
			zeroPtr = ptr;
		else
			/* There is a non-zero entry in the list. */
			listIsEmpty = False;
	}

	/* If we get here the window wasn't already in the list. */

	if (listIsEmpty)
		/* Tell the server that we are now the 
		 * current selection owner.
		 * We really only need to call this 
		 * if this is the first window
		 * in the SelectList.  Calling this sends the
		 * previous owner of the XA_PRIMARY selection 
		 * a SelectionClear event, unless the previous 
		 * owner was us already.
	 	 */
		XSetSelectionOwner(dpy, XA_PRIMARY, 
				   RootWindow(dpy, DefaultScreen(dpy)), 
				   timestamp);

	/* If there was an empty space in the SelectList
	 * put the new entry there.  Else put it at the
	 * end of the list.
	 */
	if (zeroPtr)
		*zeroPtr = winInfo;
	else
		/* No zeroed entries so add the window after the
		 * end of the list.
		 */
		SelectList[NextSelect++] = winInfo;

	/* Tell the window it is selected. */
	(*winInfo->selectfunc)(dpy, winInfo, True);
	SelectionTime = timestamp;
}

/*
 * RemoveSelection -- de-select the window
 *
 * Purpose:
 *
 * 	This routine will remove an entry from the SelectList.  It
 * 	does NOT move all the entries in the list after the deleted
 * 	entry down.  Only if the deleted entry was the one at the
 * 	end of the SelectList will the end of the list move down.
 *
 * 	For example, if there are six entries in the list and the
 * 	first five were removed, the last entry will still be at
 * 	SelectList[5], and NextSelect will be 6.  If the entry at
 * 	5 were then deleted, NextSelect would be 5 even though
 * 	the list is completely empty.  So, The value of NextSelect
 *	is not intuitive.
 *
 * Input:
 *
 *	dpy	- Display on which window appears.
 *	winInfo	- The window we are removing from the SelectList.
 *
 * Output:
 *
 *	None.
 *
 * Globals:
 *
 *	SelectList	- The list of selected windows.
 *	NextSelect	- The index of the next available empty
 *			  space. 
 *			  Note:  It is better to think of this
 *			  as the index of the space after the
 *			  the end of the list.
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
RemoveSelection(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	register	WinInfo **lastEntry = &SelectList[NextSelect];
	register	WinInfo	**ptr;

	for(ptr = SelectList; ptr < lastEntry; ptr++)
	{
		if (*ptr == winInfo)
		{
			*ptr = 0;
			if (ptr == lastEntry - 1)
				NextSelect--;
			break;
		}
	}
}


/*
 * ToggleSelection -- if the winInfo is already selected, then de-select it.
 *	If not, then select it.
 *	Returns whether it is selected or not.
 */
Bool
ToggleSelection(dpy, winInfo, timestamp)
Display	*dpy;
WinInfo	*winInfo;
Time	timestamp;
{
	register	WinInfo **lastEntry = &SelectList[NextSelect];
	register	WinInfo	**ptr;

	/* If already present, we want to deselect. */
	for(ptr = SelectList; ptr < lastEntry; ptr++)
	{
		if (*ptr == winInfo)
		{
			*ptr = 0;

			/* Tell frame it is not selected. */
			(*winInfo->selectfunc)(dpy, winInfo, False);

			if (ptr == lastEntry - 1)
				NextSelect--;
			return False;
		}
	}

	AddSelection(dpy, winInfo, timestamp);
	return True;
}


/*
 * ClearSelections -- clear the selection list
 */
ClearSelections(dpy)
Display	*dpy;
{
	register	WinInfo **lastEntry = &SelectList[NextSelect];
	register	WinInfo	**ptr;
	register	int	(*functionPtr)();
	register	WinInfo	*winInfo;

	for(ptr = SelectList; ptr < lastEntry; ptr++)
	{
		if (*ptr)
		{
			/* Save the window's select function pointer
			 * and a pointer to the winInfo structure.
			 * We need to do this because we are going
			 * to remove the entry from the select list.
			 */
			functionPtr = (*ptr)->selectfunc;
			winInfo = *ptr;

			/* Remove the window from the select list. */
			*ptr = 0;

			/* Call the window's selection function. */
			(*functionPtr)(dpy, winInfo, False);
		}
	}

	NextSelect = 0;
}

/*
 * EnumSelections --- enumerate the selected winInfo structures.  Passed
 *	(WinInfo *)0 returns the first WinInfo in the list, then passed
 *	the last returned value, returns the next value in the list.  Returns
 *	(WinInfo *)0 when list is exhausted.
 */
WinInfo *
EnumSelections(winInfo)
WinInfo	*winInfo;
{
	static int	nextEntry = 0;

	if (winInfo == (WinInfo *)0)
		nextEntry = 0;

	while(nextEntry < NextSelect)
	{
		if (SelectList[nextEntry] == (WinInfo *)0)
			nextEntry++;
		else
			return SelectList[nextEntry++];
	}

	return (WinInfo *)0;
}

/*
 * AddFrame -- add this WinInfo structure to the list of active frames
 */
AddFrame(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	register	WinInfo **lastEntry = &FrameList[NextFrame];
	register	WinInfo	**ptr;
	register	WinInfo	**zeroPtr = (WinInfo **)0;

	if (NextFrame == FRAMELISTSIZE)
	{
		/* Our table is full, blow the user off (shouldn't happen). */
		/* ??? Should be error dialog */
		XBell(dpy, 100);
		return;
	}

	/* First look to see if window is already listed.  Keep track
	 * of zero entries. */
	for(ptr = FrameList; ptr < lastEntry; ptr++)
	{
		if (*ptr == winInfo)
			return;
		else if (*ptr == (WinInfo *)0)
			zeroPtr = ptr;
	}

	/* Wasn't present, add to list. */
	if (zeroPtr)
		*zeroPtr = winInfo;
	else
		/* No zeroed entries. */
		FrameList[NextFrame++] = winInfo;
}

/*
 * RemoveFrame -- remove this WinInfo structure from the list of active
 *	frames.
 */
RemoveFrame(winInfo)
WinInfo	*winInfo;
{
	register	WinInfo **lastEntry = &FrameList[NextFrame];
	register	WinInfo	**ptr;

	for(ptr = FrameList; ptr < lastEntry; ptr++)
	{
		if (*ptr == winInfo)
		{
			*ptr = 0;
			if (ptr == lastEntry - 1)
				NextFrame--;
		}
	}
}

/*
 * EnumFrames --- enumerate the active frame winInfo structures.  Passed
 *	(WinInfo *)0 returns the first WinInfo in the list, then passed
 *	the last returned value, returns the next value in the list.  Returns
 *	(WinInfo *)0 when list is exhausted.
 */
WinInfo *
EnumFrames(winInfo)
WinInfo	*winInfo;
{
	static int	nextEntry = 0;

	if (winInfo == (WinInfo *)0)
		nextEntry = 0;

	while(nextEntry < NextFrame)
	{
		if (FrameList[nextEntry] == (WinInfo *)0)
			nextEntry++;
		else
			return FrameList[nextEntry++];
	}

	return (WinInfo *)0;
}


/*
 * SelectionResponse
 * Respond to a selection request in the manner specified by the ICCCM.
 * If we understand the target, write the appropriate property and send the 
 * event.  If we don't understand the target, just send the event.
 */
void
SelectionResponse(request)
    XSelectionRequestEvent *request;
{
    XSelectionEvent response;
    unsigned long data[SELECTSIZE];
    int format, nelements, i;
    Bool accepted = True;
    Atom type;

    response.type = SelectionNotify;
    response.serial = request->serial;
    response.requestor = request->requestor;
    response.selection = request->selection;
    response.time = request->time;

    if ( request->property == None ) {
	/*
	 * The requestor is obsolete, because the property field should always 
	 * contain the destination property.  Use the target atom to name this 
	 * property, per the suggestion in ICCCM section 2.2.
	 */
	request->property = request->target;
    }

    /*
     * REMIND: We don't support MULTIPLE yet.
     */
    if ( request->target == AtomTargets ) {
	data[0] = AtomTargets;
	data[1] = AtomTimestamp;
	data[2] = AtomListLength;
	data[3] = XA_DRAWABLE;
	data[4] = AtomLength;
	type = XA_ATOM;
	format = 32;
	nelements = 5;
    } else if ( request->target == AtomTimestamp ) {
	data[0] = SelectionTime;
	type = XA_INTEGER;
	format = 32;
	nelements = 1;
    } else if ( request->target == AtomListLength ) {
	data[0] = NextSelect;
	type = XA_INTEGER;
	format = 32;
	nelements = 1;
    } else if ( request->target == XA_DRAWABLE ) {
	for ( i=0; i<NextSelect; ++i )
	    data[i] = GetPane(SelectList[i]);
	type = XA_DRAWABLE;
	format = 32;
	nelements = NextSelect;
    } else if ( request->target == AtomLength ) {
	data[0] = NextSelect*sizeof(long);
	type = XA_INTEGER;
	format = 32;
	nelements = 1;
    } else {
	accepted = False;
    }

    if ( accepted ) {
	XChangeProperty(request->display,request->requestor,request->property,
			type, format, PropModeReplace, data, nelements);
	response.target = request->target;
	response.property = request->property;
    } else {
	response.target = None;
	response.property = None;
    }

    XSendEvent(request->display, request->requestor, False,
	       NoEventMask, &response);
}
