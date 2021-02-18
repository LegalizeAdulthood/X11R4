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

/* $Header: XJp_error.c,v 2.100 89/06/01 08:42:10 rws Exp $ */

#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include "XJplib.h"
#include "XJplibint.h"

int XJp_errno;

char *XJp_Errmsg0[19] = {
		"Frontend normal end",			/* 0 */
		"Frontend abnormal end",		/* 1 */
		"Connection to jserver was disconnected",	/* 2 */
		"Bad window ID (XJp_open) at Frontend",		/* 3 */
		"Bad window size (XJp_open) at Frontend",	/* 4 */
		"Bad font name (XJp_open) at Frontend",		/* 5 */
		"Bad dispmode (XJp_open) at Frontend",		/* 6 */
		"Can't make conversion window (XJp_open) at Frontend",	/* 7 */
		"Dispmode isn't supported (XJp_open) at Frontend",	/* 8 */
		"Bad client (XJp_close) at Frontend",		/* 9 */
		"Bad client (XJp_begin) at Frontend",		/* 10 */
		"Bad window ID (XJp_begin) at Frontend",	/* 11 */
		"Bad client (XJp_end) at Frontend",		/* 12 */
		"Bad window ID (XJp_end) at Frontend",		/* 13 */
		"Bad client (XJp_visible) at Frontend",		/* 14 */
		"Bad client (XJp_invisible) at Frontend",	/* 15 */
		"Bad client (XJp_change) at Frontend",		/* 16 */
		"Bad window size (XJp_change) at Frontend",	/* 17 */
		"Can't change attributes (XJp_change) at Frontend" /* 18 */
};
char *XJp_Errmsg200[17] = {
		"Frontend doesn't exist",			/* 200 */
		"Bad request_ID in XJp libraly",		/* 201 */
		"Bad w_key in XJp libraly",			/* 202 */
		"Bad display in XJp_open()",			/* 203 */
		"Bad font name in XJp_open()",			/* 204 */
		"Bad dispmode in XJp_open()",			/* 205 */
		"Event type isn't ClientMessage in XJp_read()",	/* 206 */
		"Event doesn't come from frontend in XJp_read()", /* 207 */
		"Buffer size is too small in XJp_read()",	/* 208 */
		"Bad event data in XJp_read()",			/* 209 */
		"Event type isn't ClientMessage in XJp_string()", /* 210 */
		" Event doesn't come from frontend in XJp_string()", /* 211 */
		"Buffer size is too small in XJp_string()",	/* 212 */
		"Bad code in XJp_string()",			/* 213 */
		"Bad event data in XJp_string()",		/* 214 */
		"Event type isn't DestroyNotify in XJp_reset()",/* 215 */
		"Event doesn't come from conversion window in XJp_reset()"
								/* 216 */
};
char *XJp_Errmsg400[6] = {
		"",						/* 400 */
		"Can't create socket in XLookupKanjiString()",	/* 401 */
		"Can't connect to Frontend in XLookupKanjiString()", /* 402 */
		"Can't write to socket in XLookupKanjiString()", /* 403 */
		"Can't read from socket in XLookupKanjiString()", /* 404 */
		"Miscellaneous ERROR in XLookupKanjiString()"	/* 405 */
};


char *
XJp_error()
{
	int n = XJp_errno;

	if (n >= 0 && n < 19)
	    return((char *)XJp_Errmsg0[n]);
	else if (n >= 200 && n < 217)
	    return((char *)XJp_Errmsg200[n - 200]);
	else if (n > 400 && n < 406)
	    return((char *)XJp_Errmsg400[n - 400]);

	return((char *)NULL);
}
