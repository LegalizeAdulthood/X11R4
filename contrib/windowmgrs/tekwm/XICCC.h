/* $Header: XICCC.h,v 1.1 89/11/22 15:30:41 glennw Exp $ */
/*
 * Copyright 1989 by Tektronix, Inc. Beaverton, Oregon.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of Tektronix not be used in advertising or publicity
 * pertaining to distribution of the software without specific, written prior
 * permission.  Tektronix makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * TEKTRONIX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * TEKTRONIX BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Glenn Widener, Tektronix, Inc.
 *          P.O. Box 1000
 *          Wilsonville, OR, 97070
 *          glennw@orca.wv.tek.com
 */

#ifndef _XICCC_H_
#define _XICCC_H_

/* the following should be in Xlib, but Consortium politics being what they
   are... */

/* start defining these structures right! */
typedef struct {
	unsigned long state;
	Window icon;
} XWMState;

XWMState *XGetWMState();
XWMState *XAllocWMState();

#endif /* _XICCC_H_ */
