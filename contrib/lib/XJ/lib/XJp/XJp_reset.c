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

/* $Header: XJp_reset.c,v 2.100 89/06/01 08:42:10 rws Exp $ */

#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include "XJplib.h"
#include "XJplibint.h"
#include "XJp_errno.h"

int
XJp_reset(ev)
XDestroyWindowEvent *ev;
{
	extern Window 	_KeepW[MAXWINDOW][2];
	extern Display	*_KeepDisp;
	extern long	_event_mask[MAXWINDOW];
	int		cnt;
	extern int	XJp_errno;
	extern Mask_W	mask_w[MAXWINDOW];
	int		cntt;

#ifdef DEBUG
	fprintf(stderr,"XJp_reset:::\n");
	fprintf(stderr,"XJp_reset:: window is %X\n",ev->window);
#endif
	if (ev->type != DestroyNotify) {
		XJp_errno = XJP_L_RESET_NOT_DN_EV;
		return(-1);
	}
	cnt = W_id_check(ev->window);
	if (cnt > MAXWINDOW) {
#ifdef DEBUG
		fprintf(stderr,"XJp_reset:: id is not id!\n");
#endif
		XJp_errno = XJP_L_RESET_NOT_FROM_CW;
		return(-1);
	}
	for (cntt = 0; cntt < MAXWINDOW; cntt++) {
		if (mask_w[cntt].w && mask_w[cntt].pc == cnt && 
		    mask_w[cntt].mask_flg) {
			XSelectInput(_KeepDisp, mask_w[cntt].w, mask_w[cntt].event_mask);
			mask_w[cntt].w = 0L;
		}
	}
	_KeepW[cnt][0] = 0L;
	_KeepW[cnt][1] = 0L;
}
