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

#define XIDLENAME "XIdleExtension"

typedef struct {
    CARD8  reqType;	/* always XIdleReqCode */
    CARD8  pad1;	/* unused */
    CARD16 length B16;
} xGetIdleTimeReq;
#define sz_xGetIdleTimeReq 4

typedef struct {
    CARD8   type;	/* always X_Reply */
    CARD8   pad1;
    CARD16  sequenceNumber B16;
    CARD32  length B32;	/* always 0 */
    CARD32  time;
    CARD32  pad2 B32;
    CARD32  pad3 B32;
    CARD32  pad4 B32;
    CARD32  pad5 B32;
    CARD32  pad6 B32;
} xGetIdleTimeReply;


