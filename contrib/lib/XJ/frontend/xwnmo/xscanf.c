/*
 *	Copyright 1989 OMRON TATEISI ELECTRONICS CO.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of OMRON not be used in 
 * advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission.  OMRON makes no representations 
 * about the suitability of this software for any purpose.  It is provided 
 * "as is" without express or implied warranty.
 *
 * OMRON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * OMRON BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE. 
 *
 *	Author:	Seiji Kuwari	OMRON TATEISI ELECTRONICS CO.
 *				kuwa@frf.omron.junet
 *				uunet!nff.ncl.omron.co.jp!kuwa
 */				

/*	Version 4.0
 */
#include "commonhd.h"
#ifdef	BSD42
#include <stdio.h>

int	pp = 0;
extern int _xdoscan();

isspace(c)
int	c;
{

	if (c == ' ' || c == '\t' || c == '\n')
		return (1);
	else
		return(0);
}

int
xscanf(str, format, s1, s2)
char *str;
char *format;
char *s1;
char *s2;
{
	pp = 0;
	return(_xdoscan(str, format, s1, s2));
}


#ifndef	WCHAR_T
typedef	unsigned short int wchar_t;
#define	WCHAR_T
#endif

extern int unGETC();


static
int
_xdoscan(str, format, s1, s2)
char *str;
register unsigned char *format;
char *s1, *s2;
{
	register int c;
	register unsigned short ch;
	int nmatch = 0, len, inchar, size;
	char *des;

	for( ; ; ) {
		if((c = *format++) == '\0')
			return(nmatch); /* end of format */
		if(isspace(c)) {
			while(isspace(inchar = GETC(str)))
				;
			if(unGETC(inchar, str) != EOF)
				continue;
			break;
		}
		ch = c & 0xff;
		if(ch != '%' || (ch = *format++) == '%') {
			inchar = Getwc(str);
			if ((inchar & 0xff80) == 0x0080)

				inchar |= 0x8e00 ;
			if((inchar & 0xffff) == ch)
				continue;
			if(unGetwc(inchar, str) != EOF)
				return(nmatch);
			break;
		}
		len = 100;

		if(ch == '\0' )
			return(EOF); /* unexpected end of format or malloc() == NULL */

		while(isspace(inchar = GETC(str)))
			;
		unGETC(inchar, str);


		if (ch == 's') {
		    if (nmatch == 0)
			des = s1;
		    else
			des = s2;
		    if((size = string(ch, len, str, des)) != 0)
			    nmatch++;
		}
		if (nmatch >= 2)
			break;
		if(size == 0)
			return(nmatch); /* failed to match input */
	}
	return(nmatch != 0 ? nmatch : EOF); /* end of input */
}

static int
string(type, len, str, s1)
register int type, len;
char *str;
char *s1;
{
	register int ch, bh;
	register char *ptr;

	ptr = s1;

	while((ch = Getwc(str)) != EOF) {
		bh = ch;
		if ((ch & 0xff80) == 0x0080)
			ch |= 0x8e00;
		if (type == 's' && !(ch & 0xff80) && isspace(ch)) {
			*s1 = 0;
			break;
		}
		if (ch & 0xff80) {
			*s1 = ch >> 8;
			s1++;
			if (--len <= 0) {
				unGETC(ch & 0xff, str);
				goto out;
			}
		}
		*s1 = ch & 0xff;
		s1++;

		if(--len <= 0) break;
	}
	if(ch == EOF || len > 0 && unGetwc(bh, str) == EOF)
		*s1 = NULL; /* end of input */
	if(ptr == s1)
		return(0); /* no match */
out:
	return(1); /* successful match */
}


GETC(p)
char	*p;
{
	int	c;
	c = *(p+pp);
	pp++;
	if (c == 0)
		return(EOF);
	return(c & 0xff);
}

unGETC(c, p)
int	c;
char	*p;
{
	if (c == EOF || pp <= 0)
		return (EOF);
	pp--;
	*(p + pp) = c;
	return (c);
}

static int
Getwc(p)
char *p;
{
	register unsigned int c;

	c = GETC(p);
	if (c == 0x8e) {
		 c = GETC(p);
		return(c);
	} else if (c >= 0xa1 && c <= 0xfe) {
		return(c<<8 | GETC(p));
	} else {
		return(c);
	}
}


#define SS2 0x8e

static int
unGetwc(c, str)
int	c;
char *str;
{
	if (c == EOF)
		return(EOF);
	if ((c & 0xff80) == 0x80) {
		unGETC(c & 0xff, str);
		unGETC(0x8e, str);
	} else if ((c & 0x8000) == 0x8000) {
		unGETC(c & 0xff, str);
		unGETC(c >> 8, str);
	} else {
		unGETC(c, str);
	}
	return(c);
}
#endif	BSD42
