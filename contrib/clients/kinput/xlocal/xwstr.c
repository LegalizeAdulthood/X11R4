/*
 *	xwstr.c
 */

/*
 * Copyright (c) 1989  Software Research Associates, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Software Research
 * Associates makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * Author:  Makoto Ishisone, Software Research Associates, Inc., Japan
 *		ishisone@sra.co.jp
 */

/*
 * wide character string を用いてテキストを表示するためのライブラリ
 *
 * wide character の特長を生かし、最大同時に 4 種類のフォントを
 * 扱うことができる。
 *
 *	GSet	bit 表現		UJIS での割り当て
 *	----------------------------------------------
 *	G0	0xxxxxxx0xxxxxxx	ASCII
 *	G1	1xxxxxxx1xxxxxxx	漢字
 *	G2	0xxxxxxx1xxxxxxx	半角かな
 *	G3	1xxxxxxx0xxxxxxx	外字
 *
 * XWSGC XWSSetGCSet(GC gc0, GC gc1, GC gc2, GC gc3)
 *	指定された 4つの GC を組み合わせて XWSGC を作る。
 *	G0, G1, G2, G3 の文字に対してそれぞれ gc0, gc1, gc2, gc3 が
 *	使われる。引数 gc? には NULL を指定してもよい。その場合には
 *	対応するキャラクタセットの文字は書かれない。
 *	なお Xt の中で使用する時には、xtwstr.c の中の XtWSGetGCSet()
 *	を使う方がよいだろう。
 *
 * int XWSDrawString(Display *d, Drawable w, XWSGC gcset,
 *		      int x, int y, wchar_t *wstr, int len)
 * int XWSDrawImageString(Display *d, Drawable w, XWSGC gcset,
 *		   int x, int y, wchar_t *wstr, int len)
 *	それぞれ、Xlib の XDrawString() / XDrawImageString() に
 *	対応する関数。指定された位置に指定された GC セットを用いて
 *	テキストを書く。引数 len は文字列 wstr の文字数である。
 *	バイト数ではないことに注意。
 *	なお、XDrawString() などとは異なり、リターンバリューとして
 *	書いた文字の幅を返す。
 *
 * int XWSTextWidth(XWSGC gcset, wchar_t *wstr, int len)
 * void XWSTextExtents(XWSGC gcset, wchar_t *wstr, int len,
 *			int *ascent, int *descent, XCharStruct *overall)
 *	それぞれ、Xlib の XTextWidth() / XTextExtents() に対応する
 *	関数。指定された文字列の幅・大きさを返す。
 *
 * void XWSFontHeight(XWSGC gcset, wchar_t *wstr, int len,
 *			int *ascent, int *descent)
 *	引数 wstr で指定された文字に対応するフォントの ascent/descent の
 *	最大値を返す。wstr に NULL を指定すると、XWSGC にセットされた
 *	すべてのフォントの ascent/descent の最大値を返す。
 */

#ifndef lint
static char	*rcsid = "$Header: /private/usr/mmb/ishisone/job/x11/xlocal/RCS/xwstr.c,v 1.7 89/09/12 10:14:20 ishisone Rel $";
#endif

#include <X11/Xlib.h>
#include "WStr.h"
#include "XWStr.h"

#define G0MASK	0x0000
#define G1MASK	0x8080
#define G2MASK	0x0080
#define G3MASK	0x8000

#define IS2B(f)	(((f)->max_byte1 > 0) || ((f)->max_char_or_byte2 > 255))
#define MIN(a, b)	((a) > (b) ? (b) : (a))
#define MAX(a, b)	((a) > (b) ? (a) : (b))

#ifndef NULL
#define NULL	0
#endif

#if __STDC__ == 1
/* static function prototype */
static int flushstr(Display *, Drawable,  FontEnt *, int, int,
		    unsigned char *, unsigned char *);
static int wsdrawstring(Display *, Drawable, XWSGC, int, int, wchar_t *, int);
#endif

static int	(*drawstrfunc)();
static int	(*drawstr16func)();

XWSGC
XWSSetGCSet(gc0, gc1, gc2, gc3)
GC gc0;
GC gc1;
GC gc2;
GC gc3;
{
	XWSGC	gcset;
	int	i;

	gcset = (XWSGC)malloc(sizeof(XWSGCSet));
	if (gcset == NULL)
		return (XWSGC)NULL;

	gcset->fe[0].gc = gc0;
	gcset->fe[1].gc = gc1;
	gcset->fe[2].gc = gc2;
	gcset->fe[3].gc = gc3;

	for (i = 0; i < 4; i++) {
		if (gcset->fe[i].gc == NULL) {
			gcset->fe[i].font = NULL;
		} else {
			XFontStruct	*font;
			gcset->fe[i].font = font = XQueryFont(gcset->fe[i].gc);
			gcset->fe[i].flag = FONTQUERY;
			if (IS2B(font))
				gcset->fe[i].flag = TWOB;
		}
	}

	return gcset;
}

int
XWSDrawString(d, w, gcset, x, y, wstr, len)
Display *d;
Drawable w;
XWSGC gcset;
int x;
int y;
wchar_t *wstr;
int len;
{
	extern int	XDrawString();
	extern int	XDrawString16();

	drawstrfunc = XDrawString;
	drawstr16func = XDrawString16;

	return wsdrawstring(d, w, gcset, x, y, wstr, len);
}

int
XWSDrawImageString(d, w, gcset, x, y, wstr, len)
Display *d;
Drawable w;
XWSGC gcset;
int x;
int y;
wchar_t *wstr;
int len;
{
	extern int	XDrawImageString();
	extern int	XDrawImageString16();

	drawstrfunc = XDrawImageString;
	drawstr16func = XDrawImageString16;

	return wsdrawstring(d, w, gcset, x, y, wstr, len);
}

void
XWSFontHeight(gcset, wstr, len, ascent, descent)
XWSGC gcset;
wchar_t *wstr;
int len;
int *ascent;
int *descent;
{
	FontEnt	*fep = &(gcset->fe[0]);
	int	i;
	int	asc = 0;
	int	dsc = 0;

	if (wstr) {
		while (len-- > 0) {
			i = GSET(*wstr++);
			if (fep[i].font) {
				if ((fep[i].font)->ascent > asc)
					asc = (fep[i].font)->ascent;
				if ((fep[i].font)->descent > dsc)
					dsc = (fep[i].font)->descent;
			}
		}
	} else {
		for (i = 0; i < 4; i++) {
			if (fep[i].font) {
				if ((fep[i].font)->ascent > asc)
					asc = (fep[i].font)->ascent;
				if ((fep[i].font)->descent > dsc)
					dsc = (fep[i].font)->descent;
			}
		}
	}
	*ascent = asc;
	*descent = dsc;
}

int
XWSTextWidth(gcset, wstr, len)
XWSGC gcset;
wchar_t *wstr;
int len;
{
#define bufsize	256
	unsigned char		buf[bufsize];
	unsigned char		*cp;
#if __STDC__ == 1
	const wchar_t		*wstr1 = wstr + len;
	const unsigned char	*cpend = buf + bufsize;
#else
	wchar_t			*wstr1 = wstr + len;
	unsigned char		*cpend = buf + bufsize;
#endif
	int			c;
	int			width = 0;
	int			gmask, gset;
	FontEnt			*fe;
	int			is2b;

	while (wstr < wstr1) {
		gmask = *wstr & 0x8080;

		switch (gmask) {
		case G0MASK:
			gset = 0;
			break;
		case G1MASK:
			gset = 1;
			break;
		case G2MASK:
			gset = 2;
			break;
		case G3MASK:
			gset = 3;
			break;
		}

		fe = &gcset->fe[gset];
		is2b = fe->flag & TWOB;
		cp = buf;

		if (fe->font == NULL) {
			while (wstr < wstr1 && (*wstr & 0x8080) == gmask)
				wstr++;
			continue;
		}

		while (wstr < wstr1 && ((c = *wstr) & 0x8080) == gmask) {
			if (cp >= cpend - 2) {
				/* flush */
				if (is2b)
					width += XTextWidth16(fe->font, buf,
							      (cp - buf) / 2);
				else
					width += XTextWidth(fe->font, buf,
							    (cp - buf));
				cp = buf;
			}
			if (is2b)
				*cp++ = (c >> 8) & 0x7f;
			*cp++ = c & 0x7f;
			wstr++;
		}

		if (cp == buf)
			continue;

		/* flush */
		if (is2b)
			width += XTextWidth16(fe->font, buf, (cp - buf) / 2);
		else
			width += XTextWidth(fe->font, buf, (cp - buf));
	}

	return width;
}

void
XWSTextExtents(gcset, wstr, len, ascent, descent, overall)
XWSGC gcset;
wchar_t *wstr;
int len;
int *ascent;
int *descent;
XCharStruct *overall;
{
#define bufsize	256
	unsigned char		buf[bufsize];
	unsigned char		*cp;
#if __STDC__ == 1
	const wchar_t		*wstr1 = wstr + len;
	const unsigned char	*cpend = buf + bufsize;
#else
	wchar_t			*wstr1 = wstr + len;
	unsigned char		*cpend = buf + bufsize;
#endif
	int			c;
	int			gmask, gset;
	FontEnt			*fe;
	int			is2b;
	int			dir, as, ds;
	XCharStruct		oa;

	*ascent = *descent = 0;
	(void)bzero(overall, sizeof(XCharStruct));

	while (wstr < wstr1) {
		gmask = *wstr & 0x8080;

		switch (gmask) {
		case G0MASK:
			gset = 0;
			break;
		case G1MASK:
			gset = 1;
			break;
		case G2MASK:
			gset = 2;
			break;
		case G3MASK:
			gset = 3;
			break;
		}

		fe = &gcset->fe[gset];
		is2b = fe->flag & TWOB;
		cp = buf;

		if (fe->font == NULL) {
			while (wstr < wstr1 && (*wstr & 0x8080) == gmask)
				wstr++;
			continue;
		}

		while (wstr < wstr1 && ((c = *wstr) & 0x8080) == gmask) {
			if (cp >= cpend - 2) {
				/* flush */
				if (is2b)
					XTextExtents16(fe->font, buf,
						       (cp - buf) / 2,
						       &dir, &as, &ds, &oa);
				else
					XTextExtents(fe->font, buf, cp - buf,
						     &dir, &as, &ds, &oa);
				cp = buf;
				*ascent = MAX(*ascent, as);
				*descent = MAX(*descent, ds);
				overall->lbearing = MIN(overall->lbearing,
							overall->width + oa.lbearing);
				overall->rbearing = MAX(overall->rbearing,
							overall->width + oa.rbearing);
				overall->width += oa.width;
				overall->ascent = MAX(overall->ascent, oa.ascent);
				overall->descent = MAX(overall->descent, oa.descent);
			}
			if (is2b)
				*cp++ = (c >> 8) & 0x7f;
			*cp++ = c & 0x7f;
			wstr++;
		}

		if (cp == buf)
			continue;

		/* flush */
		if (is2b)
			XTextExtents16(fe->font, buf, (cp - buf) / 2,
				     &dir, &as, &ds, &oa);
		else
			XTextExtents(fe->font, buf, (cp - buf),
				   &dir, &as, &ds, &oa);
		*ascent = MAX(*ascent, as);
		*descent = MAX(*descent, ds);
		overall->lbearing = MIN(overall->lbearing,
					overall->width + oa.lbearing);
		overall->rbearing = MAX(overall->rbearing,
					overall->width + oa.rbearing);
		overall->width += oa.width;
		overall->ascent = MAX(overall->ascent, oa.ascent);
		overall->descent = MAX(overall->descent, oa.descent);
	}
}


/*
 * private functions
 */

static int
wsdrawstring(d, w, gcset, x, y, wstr, len)
Display *d;
Drawable w;
XWSGC gcset;
int x;
int y;
wchar_t *wstr;
int len;
{
#define bufsize	256
	unsigned char		buf[bufsize];
	unsigned char		*cp;
#if __STDC__ == 1
	const wchar_t		*wstr1 = wstr + len;
	const unsigned char	*cpend = buf + bufsize;
#else
	wchar_t			*wstr1 = wstr + len;
	unsigned char		*cpend = buf + bufsize;
#endif
	int			c;
	int			sx = x;
	int			gmask, gset;
	FontEnt			*fe;
	int			is2b;

	while (wstr < wstr1) {
		gmask = *wstr & 0x8080;

		switch (gmask) {
		case G0MASK:
			gset = 0;
			break;
		case G1MASK:
			gset = 1;
			break;
		case G2MASK:
			gset = 2;
			break;
		case G3MASK:
			gset = 3;
			break;
		}

		fe = &gcset->fe[gset];
		is2b = fe->flag & TWOB;
		cp = buf;

		if (fe->gc == NULL) {
			while (wstr < wstr1 && (*wstr & 0x8080) == gmask)
				wstr++;
			continue;
		}
		while (wstr < wstr1 && ((c = *wstr) & 0x8080) == gmask) {
			if (cp >= cpend - 2) {
				/* flush */
				x += flushstr(d, w, fe, x, y, buf, cp);
				cp = buf;
			}
			if (is2b)
				*cp++ = (c >> 8) & 0x7f;
			*cp++ = c & 0x7f;
			wstr++;
		}
		/* flush */
		x += flushstr(d, w, fe, x, y, buf, cp);
		cp = buf;
	}

	return x - sx;
}

static int
flushstr(d, w, fe, x, y, cp0, cp1)
Display *d;
Drawable w;
FontEnt *fe;
int x;
int y;
unsigned char *cp0;
unsigned char *cp1;
{
	if (cp0 >= cp1 || fe->gc == NULL)
		return 0;

	if (fe->flag & TWOB) {
		(*drawstr16func)(d, w, fe->gc, x, y, cp0, (cp1 - cp0) / 2);
		return XTextWidth16(fe->font, cp0, (cp1 - cp0) / 2);
	} else {
		(*drawstrfunc)(d, w, fe->gc, x, y, cp0, cp1 - cp0);
		return XTextWidth(fe->font, cp0, cp1 - cp0);
	}
	/* NOTREACHED */
}
