/*
 * Copyright 1987 by Tektronix Inc, Beaverton, Oregon.
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Tektronix not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 *
 *
 * TEKTRONIX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * TEKTRONIX BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/* 
 * $XConsortium: sequence.c,v 6.2 88/10/11 18:10:48 jim Exp $ 
 * $Locker:  $ 
 */

/*
 * These routines maintain the x11 request sequence numbering of x10
 * requests.  A circular list (named 'list') is kept for each
 * "outstanding" x10 request.  An outstanding request is a request that we
 * have not received an x11 reply for (several X11 requests have no
 * reply).  An x10 request is no longer outstanding if an x11 reply has
 * been received whose sequence number is greater than or equal to the x10
 * request sequence number.
 */
#include "resource.h"
#include "X11/Xproto.h"
#include <stdio.h>

/*
 * Called Before reading any X11 replies/events.  If there has been a skip in
 * the sequence of events, then we must assume that those missing
 * received replies: Xlib does not call the "after" function for
 * requests that receive replies.
 * Then we can move the tail marker up to the head.
 */
StaticFunction void TruncateSequences(seq, lastRequest)
	SequenceListptr	seq;
	unsigned long	lastRequest;
{
	
	int		tail, head, last, index,
			width, leftWidth, rightWidth;
	unsigned long	request;
	unsigned long	*ptr;
	static unsigned long	previousRequest;

	/* search the list only when something has changed */
	if (lastRequest == previousRequest)
		return;
	previousRequest = lastRequest;

	/* Don't bother if there is nothing in the list */
	tail = seq->tail;
	head = seq->head;
	if (tail == head || seq->last == 0)
		return;

	/* dont't bother if nothing is going to change. */ 
	ptr = &seq->x11seq[ 0 ];
	if (lastRequest < ptr[ tail ])
		return;

	/* Often times, there will be many asynchronous requests followed
	 * by a synchronous request that will invalidate everything on this
	 * list.  Check there first.
	if (lastRequest >= ptr[ PrevSeqPtr(last, head, 1) ]) {
		seq->tail = seq->head;
		return;
	}
	 */

	last = seq->last;
	width = WidthOfSeqList(seq);
	if (width < 4) {
		index = tail;
		while (index != head) {
			request = ptr[ index ];
			if (lastRequest < request)
				break;
			index = NextSeqPtr(last, index, 1);
		}
	} else {
		leftWidth = width >> 1;
		rightWidth = width - leftWidth;
		index = NextSeqPtr(last, tail, leftWidth);
		/*
		 * Find the first request that has not been read...
		 * Throw away everything before that.
		 */
		for (;;) {
			request = ptr[ index ];
			if (lastRequest < request) {
				if (leftWidth <= 0)
					break;
				rightWidth = (leftWidth+1) >> 1;
				leftWidth -= rightWidth;
				index = PrevSeqPtr(last, index, rightWidth);
			} else {
				if (rightWidth <= 1) {
					index = NextSeqPtr(last, index, 1);
					break;
				}
				leftWidth = rightWidth >> 1;
				rightWidth -= leftWidth;
				index = NextSeqPtr(last, index, leftWidth);
			}
		}
	}
	seq->tail = index;
	debug(XSequenceDebug, (" chop @#%d\n", request));
}

/*
 * Put a request on the queue expanding if necessary.
 */
MarkStartOfRequest(client, req)
	int	client;
	X10Req	*req;
{
	Clientptr	cp = clist[ client ];
	SequenceListptr	seq = &cp->seq;
	X10Req		*save;
	int		next = NextSeqPtr(seq->last, seq->head, 1);
	static unsigned long previousRequest;
	unsigned long 	nextRequest = NextRequest(cp->dpy);

	/*
	 * Allocate space for a new circular list.
	 */
	TruncateSequences(seq, LastKnownRequestProcessed(cp->dpy) );
	if (seq->last == 0) {
		seq->req = (X10Req *) Xalloc(sizeof(X10Req) * SEQ_INCREMENT);
		seq->x11seq = (unsigned long *)
			Xalloc(sizeof(unsigned long) * SEQ_INCREMENT);
		seq->last = SEQ_INCREMENT;
		seq->head = seq->tail = 0;
	}

	/*
	 * Some X10 requests require zero X11 requests.  In that case,
	 * subsequent X10 requests should overwrite the previous cell.
	 */
	else if (nextRequest == previousRequest && seq->head != seq->tail) {
		next = seq->head;
		seq->head = PrevSeqPtr(seq->last, seq->head, 1);
	}

	/*
	 * Stretch the old one
	 */
	else if (next == seq->tail) {
		int		oldlast, tailToEnd, startToHead;
		X10Req		*newReq;
		unsigned long	*newx11seq;

		oldlast = seq->last;
		seq->last = (seq->last+1) * 16; /* grow by factor of 16 */
		newReq = (X10Req *) Xalloc(sizeof(X10Req) * seq->last);
		newx11seq = (unsigned long *)
			Xalloc(sizeof(unsigned long) * seq->last);
		debug(XInitializeDebug, ("sequence list now %d entries\n",
			seq->last));

		/*
		 * With + representing data and - representing no data...
		 */
		if (seq->tail < seq->head) {
			/*
			 * Our list looks like:
			 *	----++++++++++++++------ 
			 *          ^             ^     ^
			 *     tail ^        head ^     ^ last
			 */
			tailToEnd = seq->head - seq->tail;
			startToHead = 0;
		} else {
			/*
			 * Our list looks like:
			 *	++++--------------++++++ 
			 *          ^             ^     ^
			 *     head ^        tail ^     ^ last
			 */
			tailToEnd = oldlast - seq->tail;
			startToHead = seq->head;
		}

		if (startToHead) {
			bcopy((char *)seq->req,
				(char *)(newReq + tailToEnd),
				startToHead * sizeof(X10Req));
			bcopy((char *)seq->x11seq,
				(char *)(newx11seq + tailToEnd),
				startToHead * sizeof(unsigned long));
		}
		if (tailToEnd) {
			bcopy((char *)(seq->req + seq->tail),
				(char *)newReq,
				tailToEnd * sizeof(X10Req));
			bcopy((char *)(seq->x11seq + seq->tail),
				(char *)newx11seq,
				tailToEnd * sizeof(unsigned long));
		}

		free((char *)seq->req);
		free((char *)seq->x11seq);
		seq->req = newReq;
		seq->x11seq = newx11seq;
		seq->head = oldlast-1;
		seq->tail = 0;
		next = NextSeqPtr(seq->last, seq->head, 1);
	}


	/*
	 * Record the x11 sequence number and x10 request.
	 * Explicit copy is faster than bcopy.
	 */
	save = &seq->req[ seq->head ];
	save->code = req->code;
	save->func = req->func;
	save->mask = req->mask;
	save->windowId = req->windowId;
	save->param.l[0] = req->param.l[0];
	save->param.l[1] = req->param.l[1];
	save->param.l[2] = req->param.l[2];
	save->param.l[3] = req->param.l[3];

	seq->x11seq[ seq->head ] = NextRequest(cp->dpy);
	seq->head = next;
}
