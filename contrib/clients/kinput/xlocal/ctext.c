/*
 * ctext.c -- Compound Text <-> Japanese Wide Character String converter
 */

/******************************************************************************

$B!&;X<((J (designation)
		1byte			multi-byte
		94char      96char	94char       96char
	-------------------------------------------------------
	G0  :	ESC ( F	 |  -none-	ESC $ ( F  |  -none-
	G1  :	ESC ) F  |  ESC - F	ESC $ ) F  |  ESC $ - F

$B!&=*C<J8;z(J F
	1byte
	    94chars
		B	ASCII
		I	JIS KANA
		J	JIS-ROMAN
	    96chars
		A	8859/1 right half
		B	8859/2 right half
		C	8859/3 right half
		D	8859/4 right half
		F	8859/7 right half
		G	8859/6 right half
		H	8859/8 right half
		M	8859/9 (DIS) right half
	multi-byte
	    94chars ^ 2
		A	GB Hanzi
		B	JIS Kanji 1983
		C	KS Hangul/Hanja

-------------------------------------------------------------------------------
COMPOUND_TEXT $B$N;EMM(J (Comopund Text Encoding Version 1 -- MIT X Consortium Standard)
$B!&(JG0 G1 $B$N$_$r;HMQ$9$k!#(JG2 G3 $B$O;HMQ$7$J$$!#(J
$B!&(JG0 $B$,(J GL$B!"(JG1 $B$,(J GR $B$K8F$S=P$5$l$F$*$j!"$=$l$rJQ99$9$k$3$H$O$G$-$J$$!#(J
  $B$D$^$j!"(JLocking Shift $B$*$h$S(J Single Shift $B$O;HMQ$7$J$$!#(J
$B!&=i4|@_Dj$H$7$F(J ISO Latin-1 $B$,(J G0/G1 $B$K;X<($5$l$F$$$k!#(J
$B!&%^%k%A%P%$%H$NJ8;z$r(J G0 $B$K;X<($9$k$N$K!"(JESC-$-F $B$O;HMQ$7$J$$!#(J
  ESC-$-(-F $B$r;HMQ$9$k!#(J
$B!&;HMQ$G$-$k=*C<J8;z$O!">e$K=q$+$l$?DL$j!#(J
$B!&(JC0 $B$G;HMQ$G$-$kJ8;z$O!"(JNL TAB ESC $B$N$_$H$9$k!#(J
$B!&(JC1 $B$G;HMQ$G$-$kJ8;z$O(J CSI $B$N$_$H$9$k!#(J
$B!&%F%-%9%H$NIA2hJ}8~$N%7!<%1%s%9$,4^$^$l$k!#(J
	$B:8$+$i1&(J
	$B1&$+$i:8(J
	$B85$NJ}8~$KLa$k(J
******************************************************************************/

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

#ifndef lint
static char *rcsid = "$Header: /private/usr/mmb/ishisone/job/x11/xlocal/RCS/ctext.c,v 1.6 89/10/18 18:20:01 ishisone Rel $";
#endif

typedef unsigned short	wchar_t;

#define NULL	0

#define CS96	0x100	/* 96chars CS */
#define MBCS	0x200	/* Multibyte CS */

/* convJWStoCT -- Japanese Wide Character String -> COMPOUND_TEXT */
int
convJWStoCT(wstr, xstr, jisroman)
register wchar_t *wstr;
register unsigned char *xstr;
int jisroman;	/* true $B$J$i$P(J G0 $B$N%-%c%i%/%?%;%C%H$H$7$F(J JIS ROMAN $B$r!"(J
		 * false $B$J$i$P(J ASCII $B$r;HMQ$9$k(J
		 */
/* Wide Character string wstr $B$r(J COMPOUND_TEXT xstr $B$KJQ49$7!"(J
 * $BJQ498e$N%P%$%H?t$rJV$9(J($B:G8e$N(J null byte $B$O4^$^$J$$(J)$B!#$b$7(J xstr $B$,(J
 * NULL $B$J$i$PJQ49$O$;$:!"J8;z?t$N$_$rJV$9!#(J
 */
{
	register int	c;
	register int	g0, g1;
	register int	n = 0;
	int		g0cs;
	
	g0cs = jisroman ? 'J' : 'B';

	g0 = 'B';
	g1 = CS96|'A';
	
	/*
	 * G0, G1 $B$O<!$N$h$&$K;H$$J,$1$k(J
	 *  G0: ASCII / JIS-ROMAN
	 *  G1: $B4A;z(J / $B$+$J(J
	 */
	
	while (c = *wstr++) {
		switch (c & 0x8080) {
		case 0:		/* ASCII or C0 */
			if (c < ' ') {
				/* C0 */
				if (c == '\t' || c == '\n') {
					if (xstr) *xstr++ = c;
					n++;
				}
				break;
			}
			if (g0 != g0cs) {
				if (xstr) {
					*xstr++ = '\033';
					*xstr++ = '(';
					*xstr++ = g0cs;
				}
				n += 3;
				g0 = g0cs;
			}
			if (xstr) *xstr++ = c & 0x7f;
			n++;
			break;
		case 0x80:	/* $B$+$J(J */
			if (g1 != 'I') {
				if (xstr) {
					*xstr++ = '\033';
					*xstr++ = ')';
					*xstr++ = 'I';
				}
				n += 3;
				g1 = 'I';
			}
			if (xstr) *xstr++ = c & 0xff;
			n++;
			break;
		case 0x8080:	/* $B4A;z(J */
			if (g1 != (MBCS|'B')) {
				if (xstr) {
					*xstr++ = '\033';
					*xstr++ = '$';
					*xstr++ = ')';
					*xstr++ = 'B';
				}
				n += 4;
				g1 = MBCS|'B';
			}
			if (xstr) {
				*xstr++ = (c >> 8) & 0xff;
				*xstr++ = c & 0xff;
			}
			n += 2;
			break;
		default:
			/* $BL5;k$9$k(J */
			break;
		}
	}
	if (xstr) *xstr = '\0';
	return n;
}

static unsigned char *
getesc(str, len)
unsigned char *str;
int len;
{
	register int	c;

	/* $B%(%9%1!<%W%7!<%1%s%9$N!"%(%9%1!<%W$KB3$/(J
	 * $BCf4VJ8;z$H=*C<J8;z$rD4$Y$k(J
	 */
	/* $BCf4VJ8;z$O(J 02/00 $B$+$i(J 02/15 $B$^$G(J */
	while (len > 0) {
		c = *str;
		if (c < 0x20 || 0x2f < c)
			break;
		len--, str++;
	}
	/* $B=*C<J8;z$O(J 03/00 $B$+$i(J 07/14 $B$^$G(J */
	if (--len < 0 || (c = *str++) < 0x30 || 0x7e < c)
		return (unsigned char *)NULL;

	return str;
}

static unsigned char *
getcsi(str, len)
unsigned char *str;
int len;
{
	register int	c;

	/* CSI $B%7!<%1%s%9$N!"(JCSI $B$KB3$/(J
	 * $B%Q%i%a%?J8;z!&Cf4VJ8;z$H=*C<J8;z$rD4$Y$k(J
	 */
	/* $B%Q%i%a%?$O(J 03/00 $B$+$i(J 03/15 $B$^$G(J */
	while (len > 0) {
		c = *str;
		if (c < 0x30 || 0x3f < c)
			break;
		len--, str++;
	}
	/* $BCf4VJ8;z$O(J 02/00 $B$+$i(J 02/15 $B$^$G(J */
	while (len > 0) {
		c = *str;
		if (c < 0x20 || 0x2f < c)
			break;
		len--, str++;
	}
	/* $B=*C<J8;z$O(J 04/00 $B$+$i(J 07/14 $B$^$G(J */
	if (--len < 0 || (c = *str++) < 0x40 || 0x7e < c)
		return (unsigned char *)NULL;

	return str;
}

/* convCTtoJWS -- COMPOUND_TEXT -> Japanese Wide Character String */
int
convCTtoJWS(xstr, len, wstr)
register unsigned char *xstr;
int len;
wchar_t *wstr;
/* COMPOUND_TEXT xstr $B$r(J Wide Character string wstr $B$KJQ49$7!"(J
 * $BJQ498e$NJ8;z?t$rJV$9(J($B:G8e$N(J null $BJ8;z$O4^$^$J$$(J)$B!#$b$7(J wstr $B$,(J
 * NULL $B$J$i$PJQ49$O$;$:!"J8;z?t$N$_$rJV$9!#(J
 */
{
	register int	c;
	int	nskip;
	int	n = 0;
	int	g0, g1, gs;
	unsigned char	*xstr1;

	/*
	 * Compound Text $BCf$K$O(J null octet $B$,4^$^$l$k2DG=@-$,$"$k(J
	 * $B$=$3$GJ8;zNs$ND9$5(J len $B$r0z?t$G;XDj$G$-$k$h$&$K$7$F$"$k$N$@$,!"(J
	 * 0 $B$"$k$$$OIi$N;~$K$O(J (null octet $B$O$J$$$b$N$H$7$F(J) strlen() $B$G(J
	 * $BD9$5$rD4$Y$k(J
	 */
	if (len <= 0) {
		len = strlen((char *)xstr);
	}

	/* $B=i4|>uBV$O!"(JISO 8859/1 $B$,(J G0/G1 $B$KF~$C$F$$$k(J */
	g0 = 'B';	/* ASCII -> G0 */
	g1 = CS96|'A';	/* Latin/1 right hand part -> G1 */

	while (len-- > 0) {
		switch (c = *xstr++) {
		case '\n':	/* NEWLINE */
		case '\t':	/* TAB */
			if (wstr) *wstr++ = c;
			n++;
			break;
		case 0x9b:	/* CSI */
			/*
			 * CSI $B$N0lHL7A$O(J
			 *	CSI {P} {I} F
			 * $B%Q%i%a%?(J P $B$O(J 03/00 $B$+$i(J 03/15$B!"(J
			 * $BCf4VJ8;z(J I $B$O(J 02/00 $B$+$i(J 02/15$B!"(J
			 * $B=*C<J8;z(J F $B$O(J 04/00 $B$+$i(J 07/14 $B$NHO0O(J
			 */
			/*
			 * $B8=:_Dj5A$5$l$F$$$k$N$O(J directionality $B$@$1$G!"(J
			 * $B$=$l$O(J
			 *	CSI-1-]		begin left-to-right text
			 *	CSI-2-]		begin right-to-left text
			 *	CSI-]		end of string
			 * $B$G$"$k(J
			 * $B$,$H$j$"$($::#$O$3$l$rL5;k$9$k$N$G!"(JCSI $B$N(J
			 * $B%7!<%1%s%9$O$9$Y$FL5;k!"$H$$$&$3$H$K$J$k(J
			 */
			xstr1 = getcsi(xstr, len);
			if (xstr1 == NULL)
				return -1;
			len -= xstr1 - xstr;
			xstr = xstr1;
			break;
		case '\033':	/* ESC */
			/*
			 * $B%(%9%1!<%W%7!<%1%s%9$N0lHL7A$O(J
			 *	ESC {I} F
			 * $BCf4VJ8;z(J I $B$O(J 02/00 $B$+$i(J 02/15 $B$G!"(J
			 * $B=*C<J8;z(J F $B$O(J 03/00 $B$+$i(J 07/14 $B$NHO0O(J
			 */
			/*
			 * $B8=:_Dj5A$5$l$F$$$k$N$O!"(J
			 *   $B%9%?%s%@!<%I%-%c%i%/%?%;%C%H(J
			 *	ESC-(-F
			 *	ESC-$-(-F
			 *	ESC-)-F
			 *	ESC---F
			 *	ESC-$-)-F
			 *   $B%N%s%9%?%s%@!<%I%-%c%i%/%?%;%C%H(J
			 *	ESC-%-/-[0123]
			 * $B%9%?%s%@!<%I$J%-%c%i%/%?%;%C%H$O@5$7$/2r<a(J
			 * $B$7$J$/$F$O$J$i$J$$$7!"%N%s%9%?%s%@!<%I$J$b$N$O(J
			 * $BL5;k$9$k$1$l$I$b%G!<%?$r%9%-%C%W$9$kI,MW$,$"$k(J
			 */
			xstr1 = getesc(xstr, len);
			if (xstr1 == NULL)
				return -1;
			len -= xstr1 - xstr;
			switch (xstr1 - xstr) {
			case 2:		/* ESC - I - F */
				switch (*xstr++) {
				case '(':	/* 94chars CS -> G0 */
					g0 = *xstr;
					break;
				case ')':	/* 94chars CS -> G1 */
					g1 = *xstr;
					break;
				case '-':	/* 96chars CS -> G1 */
					g1 = *xstr | CS96;
					break;
				default:	/* ignore */
					break;
				}
				break;
			case 3:		/* ESC - I - I - F */
				switch (*xstr++) {
				case '$':
					switch (*xstr++) {
					case '(':	/* 94chars MBCS -> G0 */
						g0 = *xstr | MBCS;
						break;
					case ')':	/* 94chars MBCS -> G1 */
						g1 = *xstr | MBCS;
						break;
					case '-':	/* 96chars MBCS -> G1 */
						g1 = *xstr | CS96 | MBCS;
						break;
					default:	/* ignore */
						break;
					}
					break;
				case '%':
					if (*xstr++ != '/') {
						/* unknown sequence */
						break;
					}
					/*
					 * $B%W%i%$%Y!<%H%(%s%3!<%G%#%s%0(J
					 * $B40A4$KL5;k$9$k(J
					 * $B$?$@$7$=$N$"$H$KB3$/%G!<%?$r(J
					 * $B%9%-%C%W$9$kI,MW$,$"$k(J
					 *	ESC-%-/-F-M-L
					 */
					len -= 2;
					if (len < 0)
						return -1;
					nskip = (*xstr1 & 0x7f) * 128 +
					    (*(xstr1 + 1) & 0x7f);
					if ((len -= nskip) < 0)
						return -1;
					xstr1 += nskip + 2;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
			xstr = xstr1;
			break;
		default:
			if (!(c & 0x60)) {
				/*
				 * NL/TAB/ESC/CSI $B0J30$N(J C0 or C1
				 * $B$3$l$OL@$i$+$K%(%i!<(J
				 */
				return -1;
			}
			gs = (c & 0x80) ? g1 : g0;
			c &= 0x7f;
			if (gs & MBCS) {
				switch (gs & 0x70) {
				case 0x70:	/* 4byte/char */
					if (--len < 0) return -1;
					c = (c << 8) | (*xstr++ & 0x7f);
				case 0x60:	/* 3byte/char */
					if (--len < 0) return -1;
					c = (c << 8) | (*xstr++ & 0x7f);
				case 0x50:	/* 2byte/char */
				case 0x40:	/* 2byte/char */
					if (--len < 0) return -1;
					c = (c << 8) | (*xstr++ & 0x7f);
					break;
				default:
					return -1;
				}
			}
			if (wstr) {
				switch (gs) {
				case 'B':
				case 'J':
					*wstr++ = c;
					n++;
					break;
				case 'I':
					*wstr++ = 0x80 | c;
					n++;
					break;
				case MBCS|'B':
					*wstr++ = 0x8080 | c;
					n++;
					break;
				}
			} else {
				switch (gs) {
				case 'B':
				case 'J':
				case 'I':
					n++;
					break;
				case MBCS|'B':
					n++;
					break;
				}
			}
			break;
		}
	}
	if (wstr) *wstr = 0;
	return n;
}
