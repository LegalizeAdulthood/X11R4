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

/* $Header: XJp_read.c,v 2.100 89/06/01 08:42:10 rws Exp $ */

#include <stdio.h>
#include <X11/Xlib.h>
#include "XJplib.h"
#include "XJplibint.h"
#include "XJp_errno.h"

int
XJp_read(xEv, buffer, size, code_length, character_set)
XClientMessageEvent 	*xEv;
unsigned char 		*buffer;	/* return data code	*/
int 			size;		/* buffer size 		*/
int 			*code_length;	/* return 1 or 2	*/
char 			*character_set;	
{
	extern Window 	_KeepW[MAXWINDOW][2];
	extern Window 	_FrontendWindowID;
	extern Display 	*_KeepDisp;
	int 		string_len;
	int		data_length;
	int		ret;
	int		cn;
	extern int 	hextoint();
	extern Atom 	atom_server;
	extern int	XJp_errno;
	extern Mask_W	mask_w[MAXWINDOW];
	int		cntt;


	if (xEv->type != ClientMessage) {
#ifdef DEBUG
		fprintf(stderr,"XJp_read:: Type is not a ClientMessage\n");
#endif
		XJp_errno = XJP_L_READ_NOT_CL_EV;
		return(-1);
	}
	for (cntt = 0;; cntt++) {
		if (xEv->window == mask_w[cntt].w)
			break;
		else if (cntt == MAXWINDOW) {
			XJp_errno = XJP_L_READ_NOT_FROM_FE;
			return(-1);
		}

	}
	if (xEv->format == 32) { 
#ifdef DEBUG
		fprintf(stderr,"xEv->format is 32 !!\n");
#endif
		if (xEv->data.l[0] == XJP_ERROR) {
			XJp_errno = (int)xEv->data.l[1];
			return(-1);
		}
	}
	data_length = xEv->data.b[3];
	if (data_length > size) { 
#ifdef DEBUG
		fprintf(stderr,"XJp_read:: size is smalle\n");
#endif
		XJp_errno = XJP_L_READ_OVERFLLOW;
		return(-1);
	}
#ifdef DEBUG
	fprintf(stderr,"XJp_read:: type is =%X\n",xEv->data.b[0]);
#endif
	if (!strncmp(xEv->data.b, XJP_ascii, strlen(XJP_ascii))) {
		strcpy(character_set, XJP_ascii);
		*code_length = 1;
		bcopy(xEv->data.b + DATA, buffer, data_length);
		buffer[data_length] = NULL;
		return(data_length);
	} else if(!strncmp(xEv->data.b,XJP_romanX0201,strlen(XJP_romanX0201))) {
		strcpy(character_set, XJP_romanX0201);
		bcopy(xEv->data.b + DATA, buffer, data_length);
		*code_length = 1;
		buffer[data_length] = NULL;
                return(data_length);
	} else if(!strncmp(xEv->data.b, XJP_kanaX0201, strlen(XJP_kanaX0201))) {
                strcpy(character_set, XJP_kanaX0201);
                bcopy(xEv->data.b + DATA, buffer, data_length);
		buffer[data_length] = NULL;
		*code_length = 1;
                return(data_length);
	} else if(!strncmp(xEv->data.b,XJP_gaiji1byte,strlen(XJP_gaiji1byte))) {
                strcpy(character_set, XJP_gaiji1byte);
                bcopy(xEv->data.b + DATA, buffer, data_length);
		buffer[data_length] = NULL;
		*code_length = 1;
                return(data_length);
	} else if(!strncmp(xEv->data.b,XJP_kanjiX0208,strlen(XJP_kanjiX0208))) {
                strcpy(character_set, XJP_kanjiX0208);
                bcopy(xEv->data.b + DATA, buffer, data_length);
		buffer[data_length] = NULL;
		*code_length = 2;
                return(data_length/2);
	} else if(!strncmp(xEv->data.b,XJP_gaiji2byte,strlen(XJP_gaiji2byte))) {
                strcpy(character_set, XJP_gaiji2byte);
                bcopy(xEv->data.b + DATA, buffer, data_length);
		buffer[data_length] = NULL;
		*code_length = 2;
                return(data_length/2);
	} else {
        	XJp_errno = XJP_L_READ_BAD_DATA;
        	return(-1);
	}
}
