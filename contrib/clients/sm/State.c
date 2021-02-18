/***********************************************************

$Header: State.c,v 3.0 89/11/20 09:25:34 mikew Exp $

Copyright 1989 by Mike Wexler, Santa Clara, Ca.
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

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
#include "X11/Xlib.h"
#include "X11/Xutil.h"
#include "ICC.h"
#include "WinInfo.h"
#include "State.h"


/*
 * Description: Initialize a window list data structure
 * 
 */

void
StateInit(state)
StatePtr	state;	/* pointer to the state */
{
    state->topList = NULL;
    state->waitForCount = 0;
    state->hostInfo = NULL;
    return;
}
/*
 * Description: Cleanup the a State
 * 
 * Inputs:	state - a pointer a State
 * 
 */

/*ARGSUSED*/
void
StateCleanup(state)
StatePtr	state;	/* pointer to the state */
{
    return;
}

/* 
 * Description: Add a window information structure to the state. 
 *
 */

Status
StateAddWinInfo(state, winInfo)
    StatePtr	state;		/* pointer to the state */
    WinInfoPtr	winInfo;	/* pointer to window information structure */
{
    winInfo->prev = NULL;
    winInfo->next = state->topList;
    if (state->topList)
	state->topList->prev = winInfo;
    state->topList = winInfo;
    return(SM_SUCCESS);
}

Status
StateAddToHostList(state, winInfo)
    StatePtr	state;
    WinInfoPtr	winInfo;
{
    HostInfoPtr	hostInfo;

    for (hostInfo = state->hostInfo; hostInfo != NULL; 
	 hostInfo = hostInfo->next) {
	if (strcmp(hostInfo->hostName, winInfo->wmClientMachine) == 0)
	    break;
    }
    if (hostInfo == NULL) {
	hostInfo = (HostInfoPtr) XtMalloc(sizeof(*hostInfo));
	hostInfo->next = state->hostInfo;
	hostInfo->hostName = (unsigned char *)
	    XtMalloc(strlen(winInfo->wmClientMachine) + 1);
	strcpy(hostInfo->hostName, winInfo->wmClientMachine);
	hostInfo->numWinInfos = 1;
	hostInfo->winInfos = (WinInfoPtr *)
	    XtMalloc(sizeof(*hostInfo->winInfos) * 1);
	if (hostInfo->winInfos == NULL)
	    return(SM_FAILURE);
	state->hostInfo = hostInfo;
    } else {
	++hostInfo->numWinInfos;
	hostInfo->winInfos = (WinInfoPtr *) 
	    XtRealloc(hostInfo->winInfos,
		    sizeof(*hostInfo->winInfos) * hostInfo->numWinInfos);
	if (hostInfo->winInfos == NULL)
	    return(SM_FAILURE);
    }
    hostInfo->winInfos[hostInfo->numWinInfos - 1] = winInfo;
    return(SM_SUCCESS);
}

Status
StateRemoveWinInfo(state, winInfo)
    StatePtr	state;
    WinInfoPtr	winInfo;
{
    if (winInfo->prev != NULL)
	winInfo->prev->next = winInfo->next;
    else
	state->topList = winInfo->next;
    if (winInfo->next != NULL)
	winInfo->next->prev = winInfo->prev;
    return(SM_SUCCESS);
}

/*
 * Description: Get a WinInfo structure give the Resource ID for the window.
 * 
 * Outputs:	WinInfo * - the WinInfo structure that matches or NULL if
 * 			    there is no match.
 * 
 */

WinInfo        *
StateGetWinInfoByRid(state, rid)
    StatePtr	state;	/* a pointer to the state */
    Window      rid;	/* resource id of the window to find */
{
    WinInfo        *wi;

    for (wi = state->topList; wi != NULL; wi = wi->next)
	if (wi->rid == rid)
	    return (wi);
    return (NULL);
}
