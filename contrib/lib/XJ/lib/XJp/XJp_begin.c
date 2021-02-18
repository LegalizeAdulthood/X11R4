/*
 * Copyright 1989 FUJITSU LIMITED
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of FUJITSU not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  FUJITSU makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * FUJITSU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL FUJITSU
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Masahiko Narita, FUJITSU LIMITED
 * 	   Yoshitaka Kakuta, FUJITSU LIMITED
 * 	   Shuuichi Akagawa, FUJITSU LIMITED
 *         kakuta@sysrap.ks.fujitsu.co.jp
 */

/* $Header: XJp_begin.c,v 2.100 89/06/01 08:42:10 rws Exp $ */

#include <stdio.h>
#include <X11/Xlib.h>
#include "XJplib.h"
#include "XJplibint.h"

unsigned long _event_mask[MAXWINDOW];

int
XJp_begin(request_id, w_key)
XJp_id 	request_id;
Window 	w_key;
{
	XEvent 			Message;
	extern Window   	_KeepW[MAXWINDOW][2];
	extern Display 		*_KeepDisp;
	extern Window 		_FrontendWindowID;
	XWindowAttributes 	attributes_return;
	int 			cnt;
	unsigned long		dummy_mask;
	extern Mask_W		mask_w[MAXWINDOW];
	int			cntt;
	
#ifdef DEBUG
	fprintf(stderr,"Called XJp_begin:: reqid=%X, w_key=%X\n",request_id, w_key);
#endif
	if ((cnt = W_id_check(request_id)) == MAXWINDOW) {
#ifdef DEBUG
		fprintf(stderr,"not a WINDOW\n");
#endif
		return(-1);
	}
#ifdef DEBUG
	fprintf(stderr,"XJp_begin:: Call XGetWindowAttributes \n");
#endif
	XGetWindowAttributes(_KeepDisp, w_key, &attributes_return);
	_event_mask[cnt] = attributes_return.your_event_mask;
	dummy_mask = _event_mask[cnt];
	dummy_mask &= ~KeyPressMask;
	dummy_mask &= ~KeyReleaseMask;
#ifdef DEBUG
	fprintf(stderr,"XJp_begin:: eventMask = %X\n",_event_mask[cnt]);
	fprintf(stderr,"XJp_begin:: dummy_mask is %X\n",dummy_mask);
#endif
	XSelectInput(_KeepDisp, w_key, dummy_mask);
	XFlush(_KeepDisp);
#ifdef DEBUG
	fprintf(stderr,"XJp_begin:: set event struct\n");
#endif
	Message.xclient.type = 	ClientMessage;
	Message.xclient.window = (Window)_KeepW[cnt][WID];
	Message.xclient.data.l[0] = (long)XJP_BEGIN;
	Message.xclient.data.l[1] = (long)request_id;
	Message.xclient.data.l[2] = (long)w_key;
	Message.xclient.format = C_FORMAT;
	
#ifdef DEBUG
	fprintf(stderr,"XJp_begin:: Call XSentEvent:: \n");
	fprintf(stderr,"KeepDisp = %X\n",_KeepDisp);
#endif
	XSendEvent (_KeepDisp, _FrontendWindowID, False, NoEventMask, &Message);
	XFlush(_KeepDisp);
#ifdef DEBUG
	XGetWindowAttributes(_KeepDisp, w_key, &attributes_return);
	fprintf(stderr,"XJp_begin:: after mask is %X\n",attributes_return.your_event_mask);
#endif
	for (cntt = 0; cntt < MAXWINDOW; cntt++) {
 		if (mask_w[cntt].w == w_key && mask_w[cntt].mask_flg != 1) {
			mask_w[cntt].event_mask = attributes_return.your_event_mask;
			mask_w[cntt].mask_flg = 1;
			return(0);
		}
	}
	for (cntt = 0; cntt < MAXWINDOW; cntt++) {
		if (mask_w[cntt].w == 0) {
			mask_w[cntt].w = w_key;
			mask_w[cntt].event_mask = attributes_return.your_event_mask;
			mask_w[cntt].mask_flg = 1;
			mask_w[cntt].pc = cnt;
			return(0);		/* changed by TOSHIBA */
		}
	}

#ifdef OLD	/* changed by TOSHIBA */
	return(0);
#else
	fprintf(stderr,"XJp_begin: mask_w overflow\n");
	return(-1);
#endif
}
