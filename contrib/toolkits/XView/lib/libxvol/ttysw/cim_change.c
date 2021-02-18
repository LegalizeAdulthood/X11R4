#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)cim_change.c 20.10 89/08/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Character image manipulation (except size change) routines.
 */

#include <sys/types.h>
#include <pixrect/pixrect.h>
#include <xview_private/ttyansi.h>
#include <xview_private/charimage.h>
#include <xview_private/charscreen.h>

char            boldify;

extern char    *strcpy();

#define JF

ttysw_vpos(row, col)
    int             row, col;
{
    register char  *line = image[row];
    register char  *bold = screenmode[row];
    register int    i;

    while (length(line) <= col) {
	bold[length(line)] = MODE_CLEAR;
	i = line[-1]++;
	line[i] = ' ';
    }
    setlinelength(line, (length(line)));
}

ttysw_bold_mode()
{
    boldify |= MODE_BOLD;
}

ttysw_nobold_mode()
{
    boldify &= ~MODE_BOLD;
}

ttysw_underscore_mode()
{
    boldify |= MODE_UNDERSCORE;
}

ttysw_nounderscore_mode()
{
    boldify &= ~MODE_UNDERSCORE;
}

ttysw_inverse_mode()
{
    boldify |= MODE_INVERT;
}

ttysw_noinverse_mode()
{
    boldify &= ~MODE_INVERT;
}

ttysw_clear_mode()
{
    boldify = MODE_CLEAR;
}

ttysw_writePartialLine(s, curscolStart)
    char           *s;
    register int    curscolStart;
{
    register char  *sTmp;
    register char  *line = image[cursrow], *bold = screenmode[cursrow];
    register int    curscolTmp = curscolStart;

    /*
     * Fix line length if start is past end of line length. This shouldn't
     * happen but does.
     */
    if (length(line) < curscolStart)
	(void) ttysw_vpos(cursrow, curscolStart);
    /*
     * Stick characters in line.
     */
    for (sTmp = s; *sTmp != '\0'; sTmp++) {
	line[curscolTmp] = *sTmp;
	bold[curscolTmp] = boldify;
	curscolTmp++;
    }
    /*
     * Set new line length.
     */
    if (length(line) < curscolTmp)
	setlinelength(line, curscolTmp);
    /*
     * if (sTmp>(s+3)) printf("%d\n",sTmp-s);
     */
    /* Note: curscolTmp should equal curscol here */
    /*
     * if (curscolTmp!=curscol) printf("csurscolTmp=%d, curscol=%d\n",
     * curscolTmp,curscol);
     */
    (void) ttysw_pstring(s, boldify, curscolStart, cursrow, PIX_SRC);
}

#ifdef	USE_WRITE_CHAR
writechar(c)
    char            c;
{
    register char  *line = image[cursrow];
    char            unitstring[2];

    unitstring[0] = line[curscol] = c;
    unitstring[1] = 0;
    if (length(line) <= curscol)
	setlinelength(line, curscol + 1);
    /* Note: if revive this proc then null terminate line */
    (void) ttysw_pstring(unitstring, curscol, cursrow, PIX_SRC);
}

#endif	/* USE_WRITE_CHAR */

#ifdef JF
ttysw_cim_scroll(n)
    register int    n;
{
    register int    new;

#ifdef DEBUG_LINES
    printf(" ttysw_cim_scroll(%d)	\n", n);
#endif
    if (n > 0) {		/* text moves UP screen	 */
	(void) delete_lines(ttysw_top, n);
    } else {			/* (n<0)	text moves DOWN	screen	 */
	new = ttysw_bottom + n;
	(void) ttysw_roll(ttysw_top, new, ttysw_bottom);
	(void) ttysw_pcopyscreen(ttysw_top, ttysw_top - n, new);
	(void) ttysw_cim_clear(ttysw_top, ttysw_top - n);
    }
}

#else
ttysw_cim_scroll(toy, fromy)
    int             fromy, toy;
{

    if (toy < fromy)		/* scrolling up */
	(void) ttysw_roll(toy, ttysw_bottom, fromy);
    else
	ttysw_swapregions(fromy, toy, ttysw_bottom - toy);
    if (fromy > toy) {
	(void) ttysw_pcopyscreen(fromy, toy, ttysw_bottom - fromy);
	(void) ttysw_cim_clear(ttysw_bottom - (fromy - toy), ttysw_bottom);
	/* move text up */
    } else {
	(void) ttysw_pcopyscreen(fromy, toy, ttysw_bottom - toy);
	(void) ttysw_cim_clear(fromy, ttysw_bottom - (toy - fromy));	/* down */
    }
}

#endif

ttysw_insert_lines(where, n)
    register int    where, n;
{
    register int    new = where + n;

#ifdef DEBUG_LINES
    printf(" ttysw_insert_lines(%d,%d) ttysw_bottom=%d	\n", where, n, ttysw_bottom);
#endif
    if (new > ttysw_bottom)
	new = ttysw_bottom;
    (void) ttysw_roll(where, new, ttysw_bottom);
    (void) ttysw_pcopyscreen(where, new, ttysw_bottom - new);
    (void) ttysw_cim_clear(where, new);
}

delete_lines(where, n)
    register int    where, n;
{
    register int    new = where + n;

#ifdef DEBUG_LINES
    printf(" delete_lines(%d,%d)	\n", where, n);
#endif
    if (new > ttysw_bottom) {
	n -= new - ttysw_bottom;
	new = ttysw_bottom;
    }
    (void) ttysw_roll(where, ttysw_bottom - n, ttysw_bottom);
    (void) ttysw_pcopyscreen(new, where, ttysw_bottom - new);
    (void) ttysw_cim_clear(ttysw_bottom - n, ttysw_bottom);
}

ttysw_roll(first, mid, last)
    int             first, last, mid;
{

    /* printf("first=%d, mid=%d, last=%d\n", first, mid, last); */
    reverse(first, last);
    reverse(first, mid);
    reverse(mid, last);
}

static
reverse(a, b)
    int             a, b;
{

    b--;
    while (a < b)
	(void) ttysw_swap(a++, b--);
}

ttysw_swapregions(a, b, n)
    int             a, b, n;
{

    while (n--)
	(void) ttysw_swap(a++, b++);
}

ttysw_swap(a, b)
    int             a, b;
{
    char           *tmpline = image[a];
    char           *tmpbold = screenmode[a];

    image[a] = image[b];
    image[b] = tmpline;
    screenmode[a] = screenmode[b];
    screenmode[b] = tmpbold;
}

ttysw_cim_clear(a, b)
    int             a, b;
{
    register int    i;

    for (i = a; i < b; i++)
	setlinelength(image[i], 0);
    (void) ttysw_pclearscreen(a, b);
    if (a == ttysw_top && b == ttysw_bottom) {
	if (delaypainting)
	    (void) ttysw_pdisplayscreen(1);
	else
	    delaypainting = 1;
    }
}

ttysw_deleteChar(fromcol, tocol, row)
    int             fromcol, tocol, row;
{
    char           *line = image[row];
    char           *bold = screenmode[row];
    int             len = length(line);

    if (fromcol >= tocol)
	return;
    if (tocol < len) {
	/*
	 * There's a fragment left at the end
	 */
	int             gap = tocol - fromcol;
	{
	    register char  *a = line + fromcol;
	    register char  *b = line + tocol;
	    register char  *am = bold + fromcol;
	    register char  *bm = bold + tocol;
	    while (*a++ = *b++)
		*am++ = *bm++;
	}
	setlinelength(line, len - gap);
	(void) ttysw_pcopyline(fromcol, tocol, len - tocol, row);
	(void) ttysw_pclearline(len - gap, len, row);
    } else if (fromcol < len) {
	setlinelength(line, fromcol);
	(void) ttysw_pclearline(fromcol, len, row);
    }
}

ttysw_insertChar(fromcol, tocol, row)
    int             fromcol;
    register int    tocol;
    int             row;
{
    register char  *line = image[row];
    register char  *bold = screenmode[row];
    int             len = length(line);
    register int    i;
    int             delta, newlen, slug, rightextent;

    if (fromcol >= tocol || fromcol >= len)
	return;
    delta = tocol - fromcol;
    newlen = len + delta;
    if (newlen > ttysw_right)
	newlen = ttysw_right;
    if (tocol > ttysw_right)
	tocol = ttysw_right;
    for (i = newlen; i >= tocol; i--) {
	line[i] = line[i - delta];
	bold[i] = bold[i - delta];
    }
    for (i = fromcol; i < tocol; i++) {
	line[i] = ' ';
	bold[i] = MODE_CLEAR;
    }
    setlinelength(line, newlen);
    rightextent = len + (tocol - fromcol);
    slug = len - fromcol;
    if (rightextent > ttysw_right)
	slug -= rightextent - ttysw_right;
    (void) ttysw_pcopyline(tocol, fromcol, slug, row);
    (void) ttysw_pclearline(fromcol, tocol, row);
}
