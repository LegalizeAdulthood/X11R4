/***********************************************************

$Header: WinInfo.c,v 3.0 89/11/20 09:25:45 mikew Exp $

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

/*
 * Description: Allocated a new WinInfo structure
 * 
 * Outputs:	WinInfo * - The newly allocated WinInfo structure.
 * 
 */

WinInfo        *
wiAlloc()
{
    WinInfo        *winInfo;

    winInfo = (WinInfo *) XtMalloc(sizeof (*winInfo));
    if (winInfo == NULL)
	return(NULL);
    winInfo->saveYourselfState = DontSave;
    winInfo->wmProtocols = NULL;
    winInfo->numProtocols = 0;
    winInfo->win_gravity = 0;
    winInfo->prev = NULL;
    winInfo->next = NULL;
    return (winInfo);
}

/*
 * Description: Free an allocated WinInfo structure.
 * 
 */

Status
wiFree(winInfo)
    WinInfoPtr        winInfo;	/* pointer to the WinInfo structure to free */
{
    XtFree(winInfo);
    return (SM_SUCCESS);
}




