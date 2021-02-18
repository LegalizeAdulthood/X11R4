/*
 * Copyright 1989 University of Wisconsin-Madison
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of Wisconsin-Madison not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  The University of
 * Wisconsin-Madison makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF WISCONSIN-MADISON DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF WISCONSIN-MADISON BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Tim Theisen             Department of Computer Sciences
 *          tim@cs.wisc.edu         University of Wisconsin-Madison
 *          uwvax!tim               1210 West Dayton Street
 *          (608)262-0438           Madison, WI   53706
 */

#define NEED_REPLIES
#include "Xlibint.h"
#include "xidle.h"

static int X_GetIdleTime = 0;

Time XGetIdleTime(dpy)
register Display *dpy;
{
    Time IdleTime;
    xGetIdleTimeReply rep;
    register xReq *req;
    int first_event, first_error;

    LockDisplay(dpy);
    if (!X_GetIdleTime) {
	XQueryExtension(dpy, XIDLENAME, &X_GetIdleTime,
			&first_event, &first_error);
    }
    if (X_GetIdleTime) {
	GetEmptyReq(GetIdleTime, req);
	if (_XReply(dpy, (xReply *) &rep, 0, xTrue)) IdleTime = rep.time;
    }

    UnlockDisplay(dpy);
    SyncHandle();
    return (IdleTime);
}

