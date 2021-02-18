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

/* $Header: XJp_change.c,v 2.100 89/06/01 08:42:10 rws Exp $ */

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include "XJplib.h"
#include "XJplibint.h"

int
XJp_change(request_id, valuemask, values)
XJp_id 		request_id;
unsigned long 	valuemask;
XJp_attributes 	*values;
{
	XEvent 		Message;
	int 		data_size;
	unsigned char 	_aux_data[128];
	int 		cnt;
	unsigned char 	changedata[MAXDATA];
	extern Window 	_KeepW[MAXWINDOW][2];
	extern Display 	*_KeepDisp;
	extern Window 	_FrontendWindowID;
	extern Atom 	_atomId;
	
#ifdef DEBUG
	fprintf(stderr,"Called XJp_change!!\n");
#endif
        if ((cnt = W_id_check(request_id)) == MAXWINDOW) {
		fprintf(stderr,"id is unmatchs\n");
		return (-1);
	}
	Message.xclient.type = ClientMessage;
	Message.xclient.data.l[0] = XJP_CHANGE;
	Message.xclient.data.l[1] = request_id;
	Message.xclient.window = _KeepW[cnt][WID];
	Message.xclient.format = C_FORMAT;

	data_size = Data_Set(changedata, (long)0, valuemask, values, 
						_KeepW[cnt][WID], _aux_data);
	XChangeProperty (_KeepDisp, _KeepW[cnt][WID] , _atomId, XA_STRING, 
			C_P_FORMAT, PropModeReplace, changedata, data_size);
	XSendEvent (_KeepDisp, _FrontendWindowID, False, NoEventMask, &Message);
	XFlush(_KeepDisp);
	return (0);
}
