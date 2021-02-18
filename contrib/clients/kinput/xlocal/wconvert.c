/*
 *	wconvert.c -- wide character string conversion library
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

/* $Header: /private/usr/mmb/ishisone/job/x11/xlocal/RCS/wconvert.c,v 1.5 89/09/12 10:14:04 ishisone Rel $ */

#include "WStr.h"

#ifdef USE_LATIN_1
convJWStoLatin1(ws, ls)
wchar_t		*ws;
unsigned char	*ls;
{
	int	n = 0;
	int	c;
	
	if (ls) {
		while (c = *ws++) {
			if ((c & 0x8080) == 0) {
				*ls++ = c & 0x7f;
				n++;
			}
		}
	} else {
		while (c = *ws++) {
			if ((c & 0x8080) == 0)
			    n++;
		}
	}
	return n;
}

convLatin1toJWS(ls, ws)
unsigned char	*ls;
wchar_t		*ws;
{
	int	n = 0;
	int	c;

	if (ws) {
		while (c = *ls++) {
			if (c < 0x80) {
				*ws++ = c;
				n++;
			} else if (c == 0xa5) {	/* yen mark */
				*ws++ = '\\';
				n++;
			} else if (c == 0xa6) {	/* pipe */
				*ws++ = '|';
				n++;
			}
		}
	} else {
		while (c = *ls++) {
			if (c < 0x80 || c == 0xa5 || c == 0xa6)
				n++;
		}
	}
	return n;
}
#endif

#ifdef USE_JIS
/* JIS */
#define ASCII	0
#define KANJI	1
#define KANA	2

convJWStoJIS(ws, js)
wchar_t		*ws;
unsigned char	*js;
{
	int	c;
	int	kanji = ASCII;
	int	n = 0;

	while (c = *ws++) {
		switch (c & 0x8080) {
		case 0:
			if (kanji != ASCII) {
				if (js) {
					*js++ = '\033';
					*js++ = '(';
					*js++ = 'J';
				}
				n += 3;
			}
			if (js)
				*js++ = c & 0x7f;
			n++;
			kanji = ASCII;
			break;
		case 0x80:
			if (kanji != KANA) {
				if (js) {
					*js++ = '\033';
					*js++ = '(';
					*js++ = 'I';
				}
				n += 3;
			}
			if (js)
				*js++ = c & 0x7f;
			n++;
			kanji = KANA;
			break;
		case 0x8080:
			if (kanji != KANJI) {
				if (js) {
					*js++ = '\033';
					*js++ = '$';
					*js++ = 'B';
				}
				n += 3;
			}
			if (js) {
				*js++ = (c >> 8) & 0x7f;
				*js++ = c & 0x7f;
			}
			n += 2;
			kanji = KANJI;
			break;
		}
	}
	if (kanji != ASCII) {
		if (js) {
			*js++ = '\033';
			*js++ = '(';
			*js++ = 'J';
		}
		n += 3;
	}
	if (js)
		*js = '\0';

	return n;
}

convJIStoJWS(js, ws)
unsigned char	*js;
wchar_t		*ws;
{
	int	c, c1;
	int	kanji = ASCII;
	int	n = 0;

	while (c = *js++) {
		if (c == '\033') {
			if (!strncmp(js, "$B", 2) ||
			    !strncmp(js, "$@", 2)) {
				kanji = KANJI;
				js += 2;
			} else if (!strncmp(js, "(J", 2) ||
				   !strncmp(js, "(B", 2)) {
				kanji = ASCII;
				js += 2;
			} else if (!strncmp(js, "(I", 2)) {
				kanji = KANA;
				js += 2;
			} else {
				if (ws)
					*ws++ = c;
				n++;
			}
		} else if (kanji == KANJI) {
			c1 = *js++;
			if (c1 == '\0')
				break;
			if (ws)
				*ws++ = (c << 8) | c1 | 0x8080;
			n++;
		} else {
			if (ws)
				*ws++ = c | ((kanji == KANA) ? 0x80 : 0);
			n++;
		}
	}
	if (ws)
		*ws = 0;

	return n;
}
#endif

#ifdef USE_EUC
/* EUC */
convJWStoEUC(ws, es)
wchar_t		*ws;
unsigned char	*es;
{
	int	c;
	int	n = 0;

	while (c = *ws++) {
		switch (c & 0x8080) {
		case 0:
			if (es)
				*es++ = c & 0x7f;
			n++;
			break;
		case 0x80:
			if (es) {
				*es++ = 0x8e;	/* SS2 */
				*es++ = c & 0xff;
			}
			n += 2;
			break;
		case 0x8000:
			if (es) {
				*es++ = 0x8f;	/* SS3 */
				*es++ = (c >> 8) | 0x80;
				*es++ = (c & 0xff) | 0x80;
			}
			n += 3;
			break;
		case 0x8080:
			if (es) {
				*es++ = c >> 8;
				*es++ = c & 0xff;
			}
			n += 2;
			break;
		}
	}
	if (es)
		*es = '\0';

	return n;
}

convEUCtoJWS(es, ws)
unsigned char	*es;
wchar_t		*ws;
{
	int	c;
	int	n = 0;

	while (c = *es++) {
		if (c == 0x8e) {	/* SS2 */
			if (ws)
				*ws++ = *es | 0x80;
			es++;
			n++;
		} else if (c == 0x8f) {	/* SS3 */
			c = *es++;
			if (es)
				*ws++ = (c << 8) | (*es & 0x7f) | 0x8000;
			es++;
			n++;
		} else if (c & 0x80) {
			if (ws)
				*ws++ = (c << 8) | *es | 0x8080;
			es++;
			n++;
		} else {
			if (ws)
				*ws++ = c;
			n++;
		}
	}
	if (ws)
		*ws = 0;

	return n;
}
#endif

#ifdef USE_SJIS
/* SJIS */
convJWStoSJIS(ws, ss)
wchar_t		*ws;
unsigned char	*ss;
{
	int	c1, c2;
	int	n = 0;

	while (c1 = *ws++) {
		switch (c1 & 0x8080) {
		case 0:
		case 0x80:
			if (ss)
				*ss++ = c1 & 0xff;
			n++;
			break;
		case 0x8080:
			c2 = c1 & 0x7f;
			c1 = (c1 >> 8) & 0x7f;
			if (ss) {
				*ss++ = (c1 - 0x21) / 2 +
				    ((c1 <= 0x5e) ? 0x81 : 0xc1);
				if (c1 & 1)	/* odd */
				    *ss++ = c2 + ((c2 <= 0x5f) ? 0x1f : 0x20);
				else
				    *ss++ = c2 + 0x7e;
			}
			n += 2;
			break;
		}
	}
	if (ss)
		*ss = '\0';

	return n;
}

#define ISKANJI(c) \
	(((c) >= 0x81 && (c) <= 0x9f) || ((c) >= 0xe0 && (c) <= 0xfc))

convSJIStoJWS(ss, ws)
unsigned char *ss;
wchar_t *ws;
{
	int	c, c1;
	int	n = 0;

	while (c = *ss++) {
		if (ISKANJI(c)) {
			c1 = *ss++;
			c -= (c>=0xa0) ? 0xc1 : 0x81;
			if (ws) {
				if (c1 >= 0x9f) {
					*ws++ = ((c<<9) + 0x2200 + c1 - 0x7e) | 0x8080;
				} else {
					*ws++ = ((c<<9) + 0x2100 +
						 c1 - ((c1<=0x7e) ? 0x1f : 0x20)) | 0x8080;
				}
			}
			n++;
		} else {
			if (ws)
				*ws++ = c;
			n++;
		}
	}
	if (ws)
		*ws = 0;

	return n;
}
#endif
