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

/* $Header: XJplibint.h,v 2.100 89/06/01 08:42:10 rws Exp $ */

#define XJP_OPEN	1L
#define XJP_CLOSE	2L
#define XJP_BEGIN	3L
#define XJP_END		4L
#define XJP_VISIBLE	5L
#define XJP_INVISIBLE	6L
#define XJP_CHANGE	7L
#define XJP_MOVE	8L
#define XJP_WINDOWID	1L
#define XJP_ERROR	-1L

/* ******** */
#define MAXWINDOW	100
#define HWID		0
#define WID		1
#define C_P_FORMAT	8
#define C_FORMAT	32
#define SHIFT		8

#define	DATA		4

#define BRORKENFRONTEND 1

#define XJPCLIENT	"XJPCLIENT"
#define XJPFRONTEND	"XJPFRONTEND"

#define SETDATA(s,d,c)\
{\
int i;\
for(i=0;i<c;i++)\
	*((d)+i) = *((s)+i);\
}

#define MAXDATA		1024
#define _TAB		0x09
#define _CR		0x0A
#define _ESC		0x1B

#ifndef	OLD	/* change by OMRON */
#define	SS2		0x8e

typedef struct	_Mask_W {
	Window		w;
	unsigned long	event_mask;
	int		mask_flg;
	int		pc;
} Mask_W;
#endif	OLD	/* change by OMRON */
