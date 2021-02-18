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

/* $Header: XJp_open.c,v 2.100 89/06/01 08:42:10 rws Exp $ */

#include <stdio.h>
#include <sys/signal.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include "XJplibint.h"
#include "XJplib.h"
#include "XJp_errno.h"

Window 	 _KeepW[MAXWINDOW][2];
char   	*_ascii[MAXWINDOW];
Display *_KeepDisp;
int 	_count = 0;
Atom 	_atomId;
Atom 	_atom_server, _atom_client;
Window 	_FrontendWindowID;
int 	_time_flag = 0;
Mask_W	mask_w[MAXWINDOW];


XJp_id XJp_open(display, w, valuemask, values, dispmode, aux_data)
Display		*display;
Window		w;
unsigned long 	valuemask;
XJp_attributes	*values;
int		dispmode;
char 		*aux_data;
{
	XEvent 		Message;
	XEvent 		event;
	Atom 		type;
	int 		data_size, i;
	unsigned char 	changedata[MAXDATA];
	XJp_id 		HenkanWID;
	extern int 	XJp_errno;
	extern int	time_out();
	extern 	int	_time_flag;
	char		_aux_data[128], *ptr;

	if (display == (Display *)NULL) {
		XJp_errno = XJP_L_OPEN_BAD_DPY;
		return ((XJp_id)-1);
	}

	if ((_atom_server = XInternAtom (display, XJPFRONTEND,True))
							 == (Atom)NULL){ 
#ifdef DEBUG
		fprintf (stderr,"XJPlib: Can not find XJPFRONTEND atom\n");
#endif
		XJp_errno = XJP_L_NO_EXIST_FE;
		return ((XJp_id)-1);
	}
	_FrontendWindowID = XGetSelectionOwner (display, _atom_server);
	if (_FrontendWindowID == NULL) {
#ifdef DEBUG
		fprintf (stderr,"XJPlib: Can not find XJPFRONTEND\n");
#endif
		XJp_errno = XJP_L_NO_EXIST_FE;
		return ((XJp_id)-1);
	}
	_atomId = XInternAtom (display, XJPCLIENT, False);
	if (_atomId == (Atom)NULL) {
#ifdef DEBUG
		fprintf (stderr,"XJPlib: Can not find XJPCLIENT\n");
#endif
		return ((XJp_id)-1);
	}
#ifdef DEBUG
	fprintf(stderr,"XJp_open(): valuemask = %lX\n",valuemask);
	fprintf(stderr,"Call Data_Set!!\n");
#endif
	for (i=128,ptr=_aux_data; i; i--,ptr++)
		*ptr=0;
	if (aux_data)
		strncpy(_aux_data, aux_data, 128);
	data_size = Data_Set(changedata, dispmode,valuemask,values,w,_aux_data);
	if (data_size < 0) {
		return ((XJp_id)-1);
	}
#ifdef DEBUG
	{
	int i;
	fprintf(stderr,"\nXJPlib: DATA[");
	for (i=0;i<data_size;i++) {
		fprintf(stderr,"%X",*(changedata+i));
		if (((i+1) % 4) == 0)
			fprintf(stderr,":");
	}
	fprintf(stderr,"]\n");
	}
#endif
	XChangeProperty (display, w, _atomId, XA_STRING, C_P_FORMAT, 
				PropModeReplace, changedata, data_size);
	Message.xclient.window		= w;
	Message.xclient.type 		= ClientMessage;
	Message.xclient.data.l[0] 	= XJP_OPEN;
	Message.xclient.data.l[1] 	= w;
	Message.xclient.data.l[2] 	= _atomId;
	Message.xclient.format		= C_FORMAT;
#ifdef DEBUG
	fprintf(stderr,"XJp_open():: Call XSendEvent\n");
#endif
	XSendEvent (display, _FrontendWindowID, False, NoEventMask, &Message);
	XFlush(display);
	signal(SIGALRM, time_out);
	alarm(120);
	for (;_time_flag != 1;) {
		if ((XCheckTypedEvent(display, ClientMessage, &event)) == False)
			continue;
#ifdef DEBUG
		{
		int i;

		for (i = 0;i < 6;i++)
			fprintf(stderr,"[%X]",event.xclient.data.l[i]);
		fprintf(stderr,"\ntype = %d\n",event.type);
		}
#endif
		if (event.xclient.data.l[0] == -1) {
			XJp_errno = (int)event.xclient.data.l[1];
			return ((XJp_id)-1);
		} else if (event.xclient.data.l[0] != XJP_WINDOWID) {
#ifdef DEBUG
			fprintf(stderr,"XJPlib: not a XJPFRONTEND = %X\n"
					,event.xclient.data.l[0]);
#endif
			XPutBackEvent(display, &event);
			continue;
		}
		else {
#ifdef DEBUG
			fprintf(stderr,"XJP_open:: WID IS %X\n",event.xclient.data.l[1]);
#endif
			alarm(0);
			HenkanWID = (XJp_id)event.xclient.data.l[1];
			values->status_width = (long)event.xclient.data.l[2];
			values->status_height = (long)event.xclient.data.l[3];
			XSelectInput(display, HenkanWID, 
						StructureNotifyMask);
			_KeepW[_count][0] = (long)HenkanWID;
			_KeepW[_count][1] = w;
#ifdef DEBUG
			fprintf(stderr,"KeepW[%d][0] = %X, %X\n",_count, _KeepW[_count][0],_KeepW[_count][1]);
#endif
			_count++;
			_KeepDisp = display;
			return ((XJp_id)event.xclient.data.l[1]);
		}
	}
	_time_flag = 0;
	return ((XJp_id)-1);
}
