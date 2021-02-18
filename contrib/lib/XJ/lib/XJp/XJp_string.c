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

/* $Header: XJp_string.c,v 2.100 89/06/01 08:42:10 rws Exp $ */

#include <stdio.h>
#include <X11/Xlib.h>
#include "XJplib.h"
#include "XJplibint.h"
#include "XJp_errno.h"


int
XJp_string(xEv, buffer, size, code)
XClientMessageEvent 	*xEv;
unsigned char 		*buffer;	/* return code	*/
int 			size;		/* buffer size	*/
int 			code;		/* henkan code	*/
{
	XEvent 		event;
	int 		string_len;
	int 		code_length;
	char		character_set[4];
	unsigned char	r_buffer[33];
	int 		ret, ret_length;
	int		cn;
	unsigned char	*ptr;
	extern Display *_KeepDisp;
	extern Window	_FrontendWindowID;
	extern int 	XJp_errno;
	extern Window	_KeepW[MAXWINDOW][2];
	extern char    *_ascii[];
	unsigned char	*r_ptr;
	extern Mask_W	mask_w[MAXWINDOW];
	int		cntt;

	if (xEv->type != ClientMessage) {
		XJp_errno = XJP_L_STRING_NOT_CL_EV;
		return(-1);
	}
	for (cntt = 0;; cntt++) {
		if (xEv->window == mask_w[cntt].w) {
			cn = mask_w[cntt].pc;
			break;
		}
		else if (cntt == MAXWINDOW) {
			XJp_errno = XJP_L_STRING_NOT_FROM_FE;
			return(-1);
		}
	}
	if (size < 33) {
		XJp_errno = XJP_L_STRING_OVERFLLOW;
		return(-1);
	}
	if (xEv->format == 32) {
		if (xEv->data.l[0] == -1) {
			XJp_errno = (int)xEv->data.l[1];
			return (-1);
		}
	}
	ret = XJp_read(xEv, buffer, size, &code_length, character_set);

	if (ret < 0) {
#ifdef DEBUG
		fprintf(stderr,"buffer size is smalle !!\n");
#endif
		return (-1);
	}

	switch (code) {
	case XJP_JIS7:
	    strcpy(r_buffer, buffer);
	    ptr = buffer;
	    if (strcmp((char *)character_set, _ascii[cn])) {
		*ptr++ = _ESC;
		strcpy(ptr, character_set);
		ptr += strlen(character_set);
	    }
	    strcpy(ptr, r_buffer);
	    ptr += strlen(r_buffer);
	    if (strcmp((char *)character_set, _ascii[cn])) {
		*ptr++ = _ESC;
		strcpy(ptr, _ascii[cn]);
		ptr += strlen(character_set);
	    }
	    ret_length = ptr - buffer;
	    break;
	case XJP_JIS8:
	    strcpy(r_buffer, buffer);
	    ptr = buffer;
	    if (strcmp((char *)character_set, XJP_kanaX0201)
			&& strcmp(character_set, _ascii[cn])) {
		*ptr++ = _ESC;
		strcpy(ptr, character_set);
		ptr += strlen(character_set);
	    }
	    strcpy(ptr, r_buffer);
	    if (!strcmp((char *)character_set, XJP_kanaX0201)) {
		for (;*ptr; ptr++)
		    *ptr |= 0x80;
	    }
	    else
		ptr += strlen(r_buffer);
	    if (strcmp((char *)character_set, XJP_kanaX0201)
			&& strcmp(character_set, _ascii[cn])) {
		*ptr++ = _ESC;
		strcpy(ptr, _ascii[cn]);
		ptr += strlen(character_set);
	    }
	    ret_length = ptr - buffer;
	    break;
	case XJP_EUC:
	    if (!strcmp((char *)character_set ,XJP_kanjiX0208)) {
		code_conv(buffer, code, strlen(buffer));
		ret_length = code_length * ret;
	    }
	    else if (!strcmp((char *)character_set ,XJP_kanaX0201)) {
		strcpy(r_buffer, buffer);
		ptr = buffer;
		r_ptr = r_buffer;
		for (;*r_ptr; r_ptr++) {
		    *ptr++ = SS2;
		    *ptr++ = (*r_ptr | 0x80);
		}
		ret_length = ptr - buffer;
	    } else {
		ret_length = code_length * ret;
	    }
	    break;
	case XJP_SJIS:
	    if (!strcmp((char *)character_set ,XJP_kanjiX0208)) {
		code_conv(buffer, code, strlen(buffer));
	    }
	    else if (!strcmp((char *)character_set, XJP_kanaX0201)) {
		ptr = buffer;
		for (;*ptr; ptr++)
		    *ptr |= 0x80;
	    }
	    ret_length = code_length * ret;
	    break;
	defalut:
	    XJp_errno = XJP_L_STRING_BAD_CODE;
	    return (-1);
	}
	return (ret_length);
}
