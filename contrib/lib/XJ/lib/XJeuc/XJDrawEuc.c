/*
 * 		    Copyright 1989 TOSHIBA CORPORATION
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of TOSHIBA Corporation not be used in 
 * advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission. TOSHIBA Corporation makes no 
 * representations about the suitability of this software for any purpose.  
 * It is provided "as is" without express or implied warranty.
 *
 * TOSHIBA DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING 
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL 
 * TOSHIBA BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR 
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
 * SOFTWARE.
 *
 * Author:  
 * 		Masaki Takahashi
 * 		Katsuhisa Yano
 *		Personal Computer Design Dept.
 *		TOSHIBA CORPORATION OME WORKS
 *		5-20-7 SORD BLD. MASAGO 
 *		CHIBA-CITY CHIBA 260 JAPAN
 *		(0472) 77-8670
 */

#ident	"@(#)XJDrawEuc.c"

#include "X11/Xlib.h"
#include "XJEuc.h"
#include "defs.h"
#include <stdio.h>

Bool		font_set[4];
unsigned	font_first[4][2];
unsigned	font_last[4][2];
#ifdef ANKFONT
unsigned	mask[4];
#endif
int		csetint[4];			/* CSWIDTH integer */	
static		first = 1;

extern char	*getenv();
extern unsigned	_checkCodeset();

/*----------------------------------------------------------------------
	XJeTextWidth()
	Computing Character String Sizes
----------------------------------------------------------------------*/

XJeTextWidth(font_struct, string, length)
register XJeFontStruct	*font_struct;	/* euc font structure		*/
unsigned char		*string;	/* character string		*/
int			length;		/* number of characters		*/
{
    char		buf[BUFSIZE];
    unsigned		code_set;	/* code set			*/
    register		count;		/* buffer count			*/
    register 		width = 0;	/* Stringth Wigth		*/
    int			len;

    _initFontStruct(font_struct);

    for ( ; length > 0 && (count = _separateText(string, buf, 
	    length < BUFSIZE ? length : BUFSIZE, &code_set)); 
	    string += count, length -= count) {
	len = count / _getCsetWidth(code_set & ~EXTFLAG);
	if (code_set & EXTFLAG) {
	    code_set &= ~EXTFLAG;
	    if (csetint[code_set] == 1) 
	    	width += XTextWidth(font_struct->font_ext[code_set], buf, len);
	    else
	    	width += XTextWidth16(font_struct->font_ext[code_set],
				(XChar2b *)buf, len);
	} else {
	    if (csetint[code_set] == 1) 
	    	width += XTextWidth(font_struct->font_std[code_set], buf, len);
	    else
	    	width += XTextWidth16(font_struct->font_std[code_set], 
				(XChar2b *)buf, len);
	}
    }

    return(width);

} /* End of XJeTextWidth() */

/*----------------------------------------------------------------------
	XJeTextExtents()
	Returning Logical Extents
----------------------------------------------------------------------*/

XJeTextExtents(font_struct, string, length, 
	dir_ret, ascent_ret, descent_ret, overall_ret)
XJeFontStruct		*font_struct;	/* euc font structure		*/
unsigned char		*string;	/* character string		*/
int			length;		/* number of characters		*/
int			*dir_ret;	/* value of the direction	*/
int			*ascent_ret;	/* font ascent number		*/
int			*descent_ret;	/* font descent number		*/
register XCharStruct	*overall_ret;	/* overall size			*/
{
    unsigned char	buf[BUFSIZE];
    register		count;		/* buffer count			*/
    unsigned		code_set;	/* code_set			*/
    int			ascent;		/* tmp font ascent number	*/
    int			descent;	/* tmp font descent number	*/
    XCharStruct 	overall;	/* tmp char struct		*/
    int			len;

    _initFontStruct(font_struct);

    *dir_ret = *ascent_ret = *descent_ret = 0;

    overall_ret->lbearing = overall_ret->rbearing = 
	overall_ret->width = overall_ret->ascent = overall_ret->descent = 0;

    for ( ; length > 0 && (count = _separateText(string, buf, 
	    length < BUFSIZE ? length : BUFSIZE, &code_set)); 
	    string += count, length -= count) {
	len = count / _getCsetWidth(code_set & ~EXTFLAG);
	if (code_set & EXTFLAG) {
	    code_set &= ~EXTFLAG;
	    if (csetint[code_set] == 1) 
	    	XTextExtents(font_struct->font_ext[code_set],
				buf, len, dir_ret, &ascent, &descent, &overall);
	    else
	    	XTextExtents16(font_struct->font_ext[code_set], (XChar2b *)buf, 
				len, dir_ret, &ascent, &descent, &overall);
	} else {
	    if (csetint[code_set] == 1) 
	    	XTextExtents(font_struct->font_std[code_set], buf, len, 
				dir_ret, &ascent, &descent, &overall);
	    else
	    	XTextExtents16(font_struct->font_std[code_set], (XChar2b *)buf, 
				len, dir_ret, &ascent, &descent, &overall);
	}

	*ascent_ret = MAX(*ascent_ret, ascent);
	*descent_ret = MAX(*descent_ret, descent);
	overall_ret->lbearing = MIN(overall_ret->lbearing, overall.lbearing);
	overall_ret->rbearing = MAX(overall_ret->rbearing, overall.rbearing);
	overall_ret->width += overall.width;
	overall_ret->ascent = MAX(overall_ret->ascent, overall.ascent);
	overall_ret->descent = MAX(overall_ret->descent, overall.descent);
    }

    return(1);

} /* End of XJeTextExtents() */


/*----------------------------------------------------------------------
	XJeDrawString()
	Drawing Text Characters
----------------------------------------------------------------------*/

int
XJeDrawString(display, d, gc, font_struct, x, y, string, length)
Display			*display;	/* connection to the X server	*/
Drawable		d;		/* drawable			*/
GC			gc;		/* graphics context		*/
register XJeFontStruct	*font_struct;	/* euc font structure		*/
int			x;		/* x cordinate			*/
int			y;		/* y cordinate			*/
char			*string;	/* character string		*/
int			length;		/* number of characters		*/
{
    char		buf[BUFSIZE];
    int			count;		/* buffer count			*/
    unsigned		code_set;	/* code_set			*/
    Font		font_id;
    int			len;

    font_id = gc->values.font;

    _initFontStruct(font_struct);

    for ( ; length > 0 && (count = _separateText(string, buf, 
	    length < BUFSIZE ? length : BUFSIZE, &code_set)); 
	    string += count, length -= count) {
	len = count / _getCsetWidth(code_set & ~EXTFLAG);
	if (code_set & EXTFLAG) {
	    code_set &= ~EXTFLAG;
	    if (font_id != font_struct->font_ext[code_set]->fid) {
		font_id = font_struct->font_ext[code_set]->fid;
		XSetFont(display, gc, font_id);
	    }
	    if (csetint[code_set] == 1) {
		if (!XDrawString(display, d, gc, x, y, buf, len))
		    return (0);
	    	x += XTextWidth(font_struct->font_ext[code_set], buf, len);
	    } else {
		if (!XDrawString16(display, d, gc, x, y, (XChar2b *)buf, len))
		    return (0);
	    	x += XTextWidth16(font_struct->font_ext[code_set], 
			(XChar2b *)buf, len);
	    }
	} else {
	    if (font_id != font_struct->font_std[code_set]->fid) {
		font_id = font_struct->font_std[code_set]->fid;
		XSetFont(display, gc, font_id);
	    }
	    if (csetint[code_set] == 1) {
		if (!XDrawString(display, d, gc, x, y, buf, len))
		    return (0);
	    	x += XTextWidth(font_struct->font_std[code_set], buf, len);
	    } else {
		if (!XDrawString16(display, d, gc, x, y, (XChar2b *)buf, len))
		    return (0);
	    	x += XTextWidth16(font_struct->font_std[code_set], 
			(XChar2b *)buf, len);
	    }
	}
    }

    return(x);

} /* End of XJeDrawString() */


/*----------------------------------------------------------------------
	XJeDrawImageString()
	Drawing Image Text Characters
----------------------------------------------------------------------*/

int
XJeDrawImageString(display, d, gc, font_struct, x, y, string, length)
Display			*display;	/* connection to the X server	*/
Drawable		d;		/* drawable			*/
GC			gc;		/* graphics context		*/
register XJeFontStruct	*font_struct;	/* euc font structure		*/
int			x;		/* x cordinate			*/
int			y;		/* y cordinate			*/
char			*string;	/* character string		*/
int			length;		/* number of characters		*/
{
    char		buf[BUFSIZE];
    int			count;		/* buffer count			*/
    unsigned		code_set;	/* code_set			*/
    XGCValues		values;
    XGCValues		old_values;
    int			dir;		/* value of the direction	*/
    int			ascent = 0;	/* font ascent number		*/
    int			descent;	/* font descent number		*/
    register XCharStruct overall;	/* char struct			*/
    Font		font_id;
    Bool		diff = 0;
    register		i;
    int			len;

    _initFontStruct(font_struct);

    for (i = 0; i < 4; i++) {
	if (font_set[i] & STANDARD)
	    if (ascent) {
	    	if (ascent != font_struct->font_std[i]->ascent) {
		    diff = 1;
		    break;
		}
	    } else
		ascent = font_struct->font_std[i]->ascent;

	if (font_set[i] & EXTENSION)
	    if (ascent) {
	    	if (ascent != font_struct->font_ext[i]->ascent) {
		    diff = 1;
		    break;
		}
	    } else
		ascent = font_struct->font_ext[i]->ascent;
    }

    if (diff) {
	old_values.function = gc->values.function;
	old_values.fill_style = gc->values.fill_style;
	old_values.foreground = gc->values.foreground;

	values.function = GXcopy;
	values.fill_style = FillSolid;
	values.foreground = gc->values.background;

	XChangeGC(display, gc,
	    GCFunction | GCFillStyle | GCForeground, &values);

	XJeTextExtents(font_struct, string, length,
	    &dir, &ascent, &descent, &overall);
	XFillRectangle(display, d, gc, x, y - ascent,
	    overall.width, ascent + descent);

	XChangeGC(display, gc, GCForeground, &old_values);

	if (!XJeDrawString(display, d, gc, font_struct, x, y, string, length)) {
	    XChangeGC(display, gc, GCFunction | GCFillStyle, &old_values);
	    return (0);
	} else {
	    XChangeGC(display, gc, GCFunction | GCFillStyle, &old_values);
	    return (1);
	}
    } else {
	font_id = gc->values.font;

	for ( ; length > 0 && (count = _separateText(string, buf, 
	    length < BUFSIZE ? length : BUFSIZE, &code_set)); 
	    string += count, length -= count) {
	    len = count / _getCsetWidth(code_set & ~EXTFLAG);
	    if (code_set & EXTFLAG) {
	        code_set &= ~EXTFLAG;
	        if (font_id != font_struct->font_ext[code_set]->fid) {
		    font_id = font_struct->font_ext[code_set]->fid;
		    XSetFont(display, gc, font_id);
	        }
	        if (csetint[code_set] == 1) {
		    if (!XDrawImageString(display, d, gc, x, y, buf, len))
			return (0);
	    	    x += XTextWidth(font_struct->font_ext[code_set], buf, len);
	        } else {
		    if (!XDrawImageString16(display, d, gc, x, y, 
			    (XChar2b *)buf, len))
			return (0);
	    	    x += XTextWidth16(font_struct->font_ext[code_set], 
			    (XChar2b *)buf, len);
	        }
	    } else {
	        if (font_id != font_struct->font_std[code_set]->fid) {
		    font_id = font_struct->font_std[code_set]->fid;
		    XSetFont(display, gc, font_id);
	        }
	        if (csetint[code_set] == 1) {
		    if (!XDrawImageString(display, d, gc, x, y, buf, len))
			return (0);
	    	    x += XTextWidth(font_struct->font_std[code_set], buf, len);
	        } else {
		    if (!XDrawImageString16(display, d, gc, x, y, 
			    (XChar2b *)buf, len))
			return (0);
	    	    x += XTextWidth16(font_struct->font_std[code_set], 
			    (XChar2b *)buf, len);
	        }
	    }
        }
    }

    return(x);

} /* End of XJeDrawImageString() */

_separateText(string, buf, length, code_set)
unsigned char		*string;	/* character string		*/
unsigned char		*buf;		/* separate buffer		*/
register 		length;		/* number of characters		*/
unsigned		*code_set;	/* code_set			*/
{
    register unsigned char	*strptr;/* current string pointer	*/
    register unsigned char	*bufptr;/* current buffer pointer	*/
    unsigned			char_2b;
    int				width;

    if (first) {
	_initCodeset();
	first = 0;
    }

    strptr = string;
    bufptr = buf;

    /* compute string code_set						*/
    *code_set = _checkCodeset(strptr, &char_2b);
    width = _getCsetWidth(*code_set & ~EXTFLAG);
    if ((length -= width) < 0)
	return(0);
    strptr += width;
    if (csetint[*code_set & ~EXTFLAG] < 2) {
	*bufptr++ = char_2b;
	for (length -= width; length >= 0; strptr += width, length -= width) {
	    if (*code_set != _checkCodeset(strptr, &char_2b))
		break;
	    *bufptr++ = char_2b;
	}
    } else {
	((XChar2b *)bufptr)->byte1 = char_2b >> 8;
	((XChar2b *)bufptr)->byte2 = char_2b;
	((XChar2b *)bufptr)++;
	for (length -= width; length >= 0; strptr += width, length -= width) {
	    if (*code_set != _checkCodeset(strptr, &char_2b))
		break;
	    ((XChar2b *)bufptr)->byte1 = char_2b >> 8;
	    ((XChar2b *)bufptr)->byte2 = char_2b;
	    ((XChar2b *)bufptr)++;
	}
    }

    return (strptr - string);
}

unsigned
_checkCodeset(str, char_2b)
register unsigned char	*str;
register unsigned	*char_2b;
{
    unsigned char	ch;
    int			codeset;
    register		i;
#ifdef ANKFONT
    unsigned		cmask;
#else
#define cmask	0x7f
#endif

    if (first) {
	_initCodeset();
	first = 0;
    }

    if ((ch = *str) & 0x80) {
	switch (ch) {
	    case XJPSS2:
		codeset = CSET2;
		str++;
		break;
	    case XJPSS3:
		codeset = CSET3;
		str++;
		break;
	    default:
		codeset = CSET1;
		break;
	}

#ifdef ANKFONT
	cmask = mask[codeset];
#endif
	if (font_set[codeset] == NONE || !csetint[codeset])
	    goto error;
	if ((i = csetint[codeset]) == 1)
	    *char_2b = *str & cmask;
	else
	    for (*char_2b = 0; i > 0; i--) {
	        if (*str & 0x80)
		    *char_2b = (*char_2b << 8) | (*str++ & cmask);
	        else
		    goto error;
	    }
    } else {
error:
	codeset = 0;
	*char_2b = ch;
    }

    switch (font_set[codeset]) {
	case NONE:
	case STANDARD:
	    return(codeset);
	case ALL:
	    if (font_first[codeset][1] > *char_2b ||
		font_last[codeset][1] < *char_2b ||
		(font_first[codeset][0] < *char_2b && 
		 font_last[codeset][0] > *char_2b))
		return(codeset);
	case EXTENSION:
	    return(codeset | EXTFLAG);
    }
}

_initFontStruct(font_struct)
XJeFontStruct	*font_struct;
{
    register		i;

#ifdef ANKFONT
    if (first) {
	_initCodeset();
	first = 0;
    }
#endif

    for (i = 0; i < 4; i++) {
#ifdef ANKFONT
	mask[i] = 0x7f;
#endif
	if (font_struct->font_std[i]) {
	    font_set[i] = STANDARD;
	    font_first[i][0] = (font_struct->font_std[i]->min_byte1 >> 8) 
			       + font_struct->font_std[i]->min_char_or_byte2;
	    font_last[i][0] = (font_struct->font_std[i]->max_byte1 >> 8) 
			      + font_struct->font_std[i]->max_char_or_byte2;
#ifdef ANKFONT
	    if (csetint[i] == 1 && font_last[i][0] & 0x80)
		mask[i] = 0xff;
#endif
	} else
	    font_set[i] = NONE;

	if (font_struct->font_ext[i]) {
	    font_set[i] |= EXTENSION;
	    font_first[i][1] = (font_struct->font_ext[i]->min_byte1 >> 8) 
			       + font_struct->font_ext[i]->min_char_or_byte2;
	    font_last[i][1] = (font_struct->font_ext[i]->max_byte1 >> 8) 
			      + font_struct->font_ext[i]->max_char_or_byte2;
#ifdef ANKFONT
	    if (csetint[i] == 1 && font_last[i][1] & 0x80)
		mask[i] = 0xff;
#endif
	}
    }
}

_getCsetWidth(code_set)
unsigned	code_set;
{
    register	ret;

    if (first) {
	_initCodeset();
	first = 0;
    }

    switch (code_set) {
	case CSET0:
	    return (1);
	case CSET1:
	    ret = csetint[CSET1];
	    break;
	case CSET2:
	    ret = csetint[CSET2] + 1;
	    break;
	case CSET3:
	    ret = csetint[CSET3] + 1;
	    break;
    }

    if (ret < 1)
	return (1);
    else
	return ret;
}

_checkLastCharByte(str)
register unsigned char	*str;
{
    register	i, width;

    if ((str[-1] & 0x80) == 0)				/* it's easy case */
	return(1);

    if (first) {
	_initCodeset();
	first = 0;
    }

    if (csetint[CSET2]) {
	width = csetint[CSET2] + 1;
	if (str[-width] == XJPSS2) {
	    for (i = -width + 1; i < 0; i++)
	    	if (!(str[i] & 0x80))
		    break;
	    if (i == 0)
		return(width);
	}
    }

    if (csetint[CSET3]) {
	width = csetint[CSET3] + 1;
	if (str[-width] == XJPSS3) {
	    for (i = -width + 1; i < 0; i++)
	    	if (!(str[i] & 0x80))
		    break;
	    if (i == 0)
		return(width);
	}
    }

    if (width = csetint[CSET1]) {
    	for (i = -width; i < 0; i++)
	    if (!(str[i] & 0x80))
		break;
	if (i == 0)
	    return(width);
    }

    return (1);
}

_initCodeset()
{
    register char	ch, *cswidth;
    register		i;

    csetint[0] = 1;
    csetint[1] = csetint[2] = csetint[3] = 0;

    if ((cswidth = getenv("CSWIDTH")) == NULL)
	return;

    for (i = 1; i < 4; i++) {
	if ((ch = *cswidth++) == NULL)
	    break;
	if ('0' <= ch && ch <= '9')
	    csetint[i] = ch - '0';
	while (*cswidth && *cswidth++ != ',')
	    ;
    }

    return;
}

XJeFontStruct *
XJeLoadQueryFont(display, fnametab)
Display			*display;
XJeFontNameStruct	*fnametab;
{
    XJeFontStruct		*xje_font_st;
    register XFontStruct	*font_st;
    char			*fname;
    register			i;
    int				ascent, descent;
    Bool			flag = False;

    if ((xje_font_st = (XJeFontStruct *) malloc(sizeof(XJeFontStruct))) == 0)
	return NULL;

    ascent = descent = 0;
    for (i = 0; i < 4; i++) {
	if (fname = fnametab->fontname_std[i]) {
	    font_st = XLoadQueryFont(display, fname);
	    if (font_st) {
		ascent = MAX(font_st->ascent, ascent);
		descent = MAX(font_st->descent, descent);
		flag = True;
	    }
	} else
	    font_st = NULL;
	xje_font_st->font_std[i] = font_st;

	if (fname = fnametab->fontname_ext[i]) {
	    font_st = XLoadQueryFont(display, fname);
	    if (font_st) {
		ascent = MAX(font_st->ascent, ascent);
		descent = MAX(font_st->descent, descent);
		flag = True;
	    }
	} else
	    font_st = NULL;
	xje_font_st->font_ext[i] = font_st;
    }

    if (!flag) {
    	free(xje_font_st);
	return NULL;
    }

    xje_font_st->ascent = ascent;
    xje_font_st->descent = descent;
	
    return xje_font_st;
}

