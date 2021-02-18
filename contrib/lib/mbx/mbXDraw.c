
#ifndef lint
static char sccs[] = "@(#)mbXDraw.c	1.4";
#endif

/***********************************************************
Copyright International Business Machines Corporation 1989

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/*
 * Name: mbXDraw - Multibyte Draw functions for X
 *
 * Description:
 * 
 *  Contains mbXDrawString and mbXDrawImageString.      
 *  Both depend on mbXCommon.c which contains the main parsing 
 *  code. This program just supplies the MbXflush macros. 
 *
 *  The following mehtod can also be used to handle the horizontial 
 *  positioning when switching across CSid's (fonts).
 *
 *  x += XTextWidth16(fontlist[cs], mbXbuffer, (p-mbXbuffer)/sizeof(XChar2b))
 *
 */

#include <X11/Xlib.h>
#include <locale.h>
#include "mbX.h"

#define mbXflush16(cs) \
	if ( p != mbXbuffer ) { \
		XSetFont( dpy, gc, fontlist[cs]->fid ) ;\
		XDrawImageString16(dpy, d, gc, x, y, mbXbuffer,\
					(p-mbXbuffer)/sizeof(XChar2b));\
		x += fontlist[cs]->max_bounds.width \
				      * (p-mbXbuffer)/sizeof(XChar2b);\
		p = mbXbuffer;\
	}

mbXDrawImageString( dpy, d, gc, x, y, str, len, fontlist)
Display *dpy;
Drawable d;
GC gc;
int x, y;
caddr_t str;
int len;
XFontStruct **fontlist;
{
if ( _mbXlocale == NULL ) {
	mbXsetlocale(setlocale(LC_CTYPE, NULL));
}

if ( str == NULL || len == 0 ) return ; 

#include "mbXCommon.c"
}

#undef mbXflush16

#define mbXflush16(cs) \
	if ( p != mbXbuffer ) { \
		XSetFont( dpy, gc, fontlist[cs]->fid ) ;\
		XDrawString16(dpy, d, gc, x, y, mbXbuffer,\
					(p-mbXbuffer)/sizeof(XChar2b));\
		x += fontlist[cs]->max_bounds.width \
				      * (p-mbXbuffer)/sizeof(XChar2b);\
		p = mbXbuffer;\
	}


mbXDrawString( dpy, d, gc, x, y, str, len, fontlist)
Display *dpy;
Drawable d;
GC gc;
int x, y;
caddr_t str;
int len;
XFontStruct **fontlist;
{
if ( _mbXlocale == NULL ) {
	mbXsetlocale(setlocale(LC_CTYPE, NULL));
}

if ( str == NULL || len == 0 ) return ; 

#include "mbXCommon.c"
}
