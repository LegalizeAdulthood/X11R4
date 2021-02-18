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
#include "X.h"
#include "Xproto.h"
#include "misc.h"
#include "extnsionst.h"
#include "dixstruct.h"
#include "xidle.h"

extern void (* ReplySwapVector[256]) ();

static int XIdleReqCode = 0;

/****************
 * XIdleExtensionInit
 *
 * Called from InitExtensions in main() or from QueryExtension() if the
 * extension is dynamically loaded.
 *
 * XIdle has no events or errors (other than the core errors)
 ****************/

void
XIdleExtensionInit()
{
    ExtensionEntry *extEntry, *AddExtension();
    int XIdle(), SProcSimpleReq();
    void  XIdleReset(), SXIdleReply();

    extEntry = AddExtension(XIDLENAME, 0, 0, XIdle, SProcSimpleReq, XIdleReset);
    if (extEntry) {
	XIdleReqCode = extEntry->base;
	(void) MakeAtom(XIDLENAME, strlen(XIDLENAME), TRUE);
	ReplySwapVector[XIdleReqCode] = SXIdleReply;
    }
    else {
	FatalError("XIdleExtensionInit: AddExtensions failed\n");
    }
}

int
XIdle(client)
    register ClientPtr client;
{
    xGetIdleTimeReply rep;

    REQUEST(xGetIdleTimeReq);
    REQUEST_SIZE_MATCH(xGetIdleTimeReq);

    rep.type = X_Reply;
    rep.sequenceNumber = client->sequence;
    rep.length = 0;
    rep.time = TimeSinceLastInputEvent();
    WriteReplyToClient(client, sizeof(xGetIdleTimeReply), &rep);
    return (Success);
}

void
XIdleReset()
{
}

void
SXIdleReply(pClient, size, pRep)
    ClientPtr		pClient;
    int			size;
    xGetIdleTimeReply	*pRep;
{
    register char n;
    swaps(&pRep->sequenceNumber, n);
    swapl(&pRep->time, n);
    (void)WriteToClient(pClient, size, (char *) pRep);
}
