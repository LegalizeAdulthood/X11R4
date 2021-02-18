
#ifndef lint
static char sccs[] = "@(#)mbXText.c	1.2";
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
 * Name: mbXText - obtains font metrics for multibyte strings
 *
 * Description:
 *
 *  Contains mbXTextWidth and mbXTextExtents
 *  Both depend on mbXCommon.c which contains the main parsing
 *  code. This program just supplies the MbXflush macros.
 */

#include <X11/Xlib.h>
#include <locale.h>
#include "mbX.h"

#define mbXflush16(cs) \
	if ( p != mbXbuffer ) { \
		x += XTextWidth16(fonts[cs], mbXbuffer, (p-mbXbuffer)/sizeof(XChar2b));\
		p = mbXbuffer;\
	}

int mbXTextWidth( fonts, str, len)
XFontStruct **fonts;
caddr_t str;
int len;
{
int x;

if ( _mbXlocale == NULL ) {
	mbXsetlocale(setlocale(LC_CTYPE, NULL));
}

if ( str == NULL || len == 0 ) return ;

#include "mbXCommon.c"
return (x) ; 
}

#undef mbXflush16

#define mbXflush16(cs) \
	if ( p != mbXbuffer ) { \
		XTextExtents16(fonts[cs], mbXbuffer,\
			(p-mbXbuffer)/sizeof(XChar2b),\
			direction, &tmp_ascent, &tmp_descent, &tmp);\
		overall->ascent   = max(overall->ascent, tmp.ascent);\
		overall->descent  = max(overall->descent, tmp.descent);\
		overall->width    = max(overall->width, tmp.width);\
		overall->lbearing = min(overall->lbearing, tmp.lbearing);\
		overall->rbearing = max(overall->rbearing, tmp.rbearing);\
		*ascent = overall->ascent;\
		*descent = overall->descent;\
		p = mbXbuffer;\
	}


mbXTextExtents( fonts, str, len, direction, ascent, descent, overall)
XFontStruct **fonts;
caddr_t str;
int len;
int *direction;		/* RETURN */
int *ascent, *descent;	/* RETURN */
XCharStruct *overall;	/* RETURN */
{
int x;
int tmp_ascent, tmp_descent;
XCharStruct tmp;
overall->ascent   = 0;
overall->descent  = 0;
overall->width    = 0;
overall->lbearing = 0;
overall->rbearing = 0;

*ascent = 0;
*descent = 0;
if ( _mbXlocale == NULL ) {
	mbXsetlocale(setlocale(LC_CTYPE, NULL));
}

if ( str == NULL || len == 0 ) return ;

#include "mbXCommon.c"
}

