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
 * wide character string $B$rMQ$$$F%F%-%9%H$rI=<($9$k$?$a$N%i%$%V%i%j(J
 *
 * wide character $B$NFCD9$r@8$+$7!":GBgF1;~$K(J 4 $B<oN`$N%U%)%s%H$r(J
 * $B07$&$3$H$,$G$-$k!#(J
 *
 *	GSet	bit $BI=8=(J		UJIS $B$G$N3d$jEv$F(J
 *	----------------------------------------------
 *	G0	0xxxxxxx0xxxxxxx	ASCII
 *	G1	1xxxxxxx1xxxxxxx	$B4A;z(J
 *	G2	0xxxxxxx1xxxxxxx	$BH>3Q$+$J(J
 *	G3	1xxxxxxx0xxxxxxx	$B30;z(J
 *
 * XWSGC XWSSetGCSet(GC gc0, GC gc1, GC gc2, GC gc3)
 *	$B;XDj$5$l$?(J 4$B$D$N(J GC $B$rAH$_9g$o$;$F(J XWSGC $B$r:n$k!#(J
 *	G0, G1, G2, G3 $B$NJ8;z$KBP$7$F$=$l$>$l(J gc0, gc1, gc2, gc3 $B$,(J
 *	$B;H$o$l$k!#0z?t(J gc? $B$K$O(J NULL $B$r;XDj$7$F$b$h$$!#$=$N>l9g$K$O(J
 *	$BBP1~$9$k%-%c%i%/%?%;%C%H$NJ8;z$O=q$+$l$J$$!#(J
 *	$B$J$*(J Xt $B$NCf$G;HMQ$9$k;~$K$O!"(Jxtwstr.c $B$NCf$N(J XtWSGetGCSet()
 *	$B$r;H$&J}$,$h$$$@$m$&!#(J
 *
 * int XWSDrawString(Display *d, Drawable w, XWSGC gcset,
 *		      int x, int y, wchar_t *wstr, int len)
 * int XWSDrawImageString(Display *d, Drawable w, XWSGC gcset,
 *		   int x, int y, wchar_t *wstr, int len)
 *	$B$=$l$>$l!"(JXlib $B$N(J XDrawString() / XDrawImageString() $B$K(J
 *	$BBP1~$9$k4X?t!#;XDj$5$l$?0LCV$K;XDj$5$l$?(J GC $B%;%C%H$rMQ$$$F(J
 *	$B%F%-%9%H$r=q$/!#0z?t(J len $B$OJ8;zNs(J wstr $B$NJ8;z?t$G$"$k!#(J
 *	$B%P%$%H?t$G$O$J$$$3$H$KCm0U!#(J
 *	$B$J$*!"(JXDrawString() $B$J$I$H$O0[$J$j!"%j%?!<%s%P%j%e!<$H$7$F(J
 *	$B=q$$$?J8;z$NI}$rJV$9!#(J
 *
 * int XWSTextWidth(XWSGC gcset, wchar_t *wstr, int len)
 * void XWSTextExtents(XWSGC gcset, wchar_t *wstr, int len,
 *			int *ascent, int *descent, XCharStruct *overall)
 *	$B$=$l$>$l!"(JXlib $B$N(J XTextWidth() / XTextExtents() $B$KBP1~$9$k(J
 *	$B4X?t!#;XDj$5$l$?J8;zNs$NI}!&Bg$-$5$rJV$9!#(J
 *
 * void XWSFontHeight(XWSGC gcset, wchar_t *wstr, int len,
 *			int *ascent, int *descent)
 *	$B0z?t(J wstr $B$G;XDj$5$l$?J8;z$KBP1~$9$k%U%)%s%H$N(J ascent/descent $B$N(J
 *	$B:GBgCM$rJV$9!#(Jwstr $B$K(J NULL $B$r;XDj$9$k$H!"(JXWSGC $B$K%;%C%H$5$l$?(J
 *	$B$9$Y$F$N%U%)%s%H$N(J ascent/descent $B$N:GBgCM$rJV$9!#(J
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
