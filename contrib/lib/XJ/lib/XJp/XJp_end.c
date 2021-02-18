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

/* $Header: XJp_end.c,v 2.100 89/06/01 08:42:10 rws Exp $ */

#include <stdio.h>
#include <X11/Xlib.h>
#include "XJplib.h"
#include "XJplibint.h"

int
XJp_end(request_id, w_key)
XJp_id request_id;
Window w_key;
{
	XEvent 		Message;
	extern Display 	*_KeepDisp;
	extern Window 	_FrontendWindowID;
	extern unsigned long 	_event_mask[MAXWINDOW];
	extern Window	_KeepW[MAXWINDOW][2];
	int		cnt;
	extern Mask_W	mask_w[MAXWINDOW];
	int		cntt;
	
#ifdef DEBUG
	fprintf(stderr,"Call XJp_end!!\n");
#endif
	if ((cnt = W_id_check(request_id)) == MAXWINDOW) {
		return(-1);
	}
	for (cntt = 0; cntt < MAXWINDOW; cntt++) {
		if (mask_w[cntt].w == w_key) {
			XSelectInput(_KeepDisp, w_key, mask_w[cntt].event_mask);
			mask_w[cntt].mask_flg = 0;
		}
	}
#ifdef DEBUG
	fprintf(stderr,"XJp_end:: event_mask is %X\n",_event_mask[cnt]);
#endif
	XFlush(_KeepDisp);

	Message.xclient.type = ClientMessage;
	Message.xclient.data.l[0] = XJP_END;
	Message.xclient.data.l[1] = request_id;
	Message.xclient.window = (Window)_KeepW[cnt][WID];
	Message.xclient.data.l[2] = w_key;
	Message.xclient.format = C_FORMAT;
	XSendEvent (_KeepDisp, _FrontendWindowID, False, NoEventMask, &Message);
	XFlush(_KeepDisp);

	return(0);
}
