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

/* $Header: XJp_errno.h,v 2.100 89/06/01 08:42:10 rws Exp $ */

/* XJP frontend ERRORS */
/* Frontend itself */
#define XJP_F_FRONTEND_END 		0 /* Frontend nomal end */
#define XJP_F_FRONTEND_ERR_END		1 /* Frontend abnormal end */
#define XJP_F_JSERVER_DISCONNECT	2 /* Connection to jserver was disconnected */
/* XJp_open() */
#define XJP_F_OPEN_BAD_WID		3 /* Bad window ID */
#define XJP_F_OPEN_BAD_SZ		4 /* Bad window size */
#define XJP_F_OPEN_BAD_FN		5 /* Bad font name */
#define XJP_F_OPEN_BAD_DM		6 /* Bad dispmode */
#define XJP_F_OPEN_NOT_WIN		7 /* Can't make conversion window */
#define XJP_F_OPEN_NOT_SUPPORT		8 /* Dispmode isn't supported */
/* XJp_close() */
#define XJP_F_CLOSE_BAD_CL		9 /* Bad client */
/* XJp_begin() */
#define XJP_F_BEGIN_BAD_CL		10 /* Bad client */
#define XJP_F_BEGIN_BAD_WID		11 /* Bad window ID */
/* XJp_end() */
#define XJP_F_END_BAD_CL		12 /* Bad client */
#define XJP_F_END_BAD_WID		13 /* Bad window ID */
/* XJp_visible() */
#define XJP_F_VISIBLE_BAD_CL		14 /* Bad client */
/* XJp_invisible() */
#define XJP_F_INVISIBLE_BAD_CL		15 /* Bad client */
/* XJp_change() */
#define XJP_F_CHANGE_BAD_CL		16 /* Bad client */
#define XJP_F_CHANGE_BAD_SZ		17 /* Bad window size */
#define XJP_F_CHANGE_NOT_WIN		18 /* Can't change attributes */

/* XJP libraly */
/* common ERROR in XJP libraly */
#define XJP_L_NO_EXIST_FE		200 /* Frontend doesn't exist */
#define XJP_L_BAD_REQUEST_ID		201 /* Bad request_ID */
#define XJP_L_BAD_W_KEY			202 /* Bad w_key */
/* XJp_open() */
#define XJP_L_OPEN_BAD_DPY		203 /* Bad display */
#define XJP_L_OPEN_BAD_FN		204 /* Bad font name */
#define XJP_L_OPEN_BAD_DM		205 /* Bad dispmode */
/* XJp_read() */
#define XJP_L_READ_NOT_CL_EV		206 /* Event type isn't ClientMessage */
#define XJP_L_READ_NOT_FROM_FE		207 /* Event doesn't come from frontend */
#define XJP_L_READ_OVERFLLOW		208 /* Buffer size is too small */
#define XJP_L_READ_BAD_DATA		209 /* Bad event data */
/* XJp_string() */
#define XJP_L_STRING_NOT_CL_EV		210 /* Event type isn't ClientMessage */
#define XJP_L_STRING_NOT_FROM_FE	211 /* Event doesn't come from frontend */
#define XJP_L_STRING_OVERFLLOW		212 /* Buffer size is too small */
#define XJP_L_STRING_BAD_CODE		213 /* Bad code */
#define XJP_L_STRING_BAD_DATA		214 /* Bad event data */
/* XJp_reset() */
#define XJP_L_RESET_NOT_DN_EV		215 /* Event type isn't DestroyNotify */
#define XJP_L_RESET_NOT_FROM_CW		216 /* Event doesn't come from conversion window */

/* XLookupKanjiString() */
#define XJP_L_BAD_SOCK			401 /* Can't create socket */
#define XJP_L_NOT_CONN			402 /* Can't connect */
#define XJP_L_ERR_WRITE			403 /* Can't write to socket*/
#define XJP_L_ERR_INTR			404 /* Can't read from socket */
#define XJP_L_ERR_MISC			405 /* Miscellaneous ERROR */
