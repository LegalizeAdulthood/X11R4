/*
 *	$XConsortium: screen.c,v 1.10 89/01/03 16:18:06 jim Exp $
 *	$Header: screen.c,v 1.1 89/05/30 14:53:53 kagotani Rel $
 */

#include <X11/copyright.h>

/*
 *	Copyright 1989 OMRON TATEISI ELECTRONICS CO.
 *
 *	Original Source: ./client/kterm/screen.c
 *
 *	Modified by OMRON TATEISI ELECTRONICS CO., to handle XJp-library.
 *
 *	Author:	Seiji Kuwari	OMRON TATEISI ELECTRONICS CO.
 *				kuwa@frf.omron.junet
 *				uunet!nff.ncl.omron.co.jp!kuwa
 */				

/*
 * Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior permission.
 *
 *
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/* screen.c */

#ifndef lint
static char rcs_id[] = "$XConsortium: screen.c,v 1.10 89/01/03 16:18:06 jim Exp $";
static char kterm_id[] = "$Header: screen.c,v 1.1 89/05/30 14:53:53 kagotani Rel $";
#endif	/* lint */

#ifdef FACOM
#include <sys/types.h>
#endif

#include <X11/Xlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "ptyx.h"
#include "error.h"


#ifdef EWS
#ifdef TIOCSSIZE
#undef TIOCSSIZE
#endif
#ifdef TIOCSWINSZ
#undef TIOCSWINSZ
#endif
#ifdef SIGWINCH
#undef SIGWINCH
#endif
#endif

extern char *calloc();
extern char *malloc();
extern char *realloc();
extern void Bcopy();
extern void free();

ScrnBuf Allocate (nrow, ncol)
/*
   allocates memory for a 2-dimensional array of chars and returns a pointer
   thereto
   each line is formed from a pair of char arrays.  The first (even) one is
   the actual character array and the second (odd) one is the attributes.
 */
register int nrow, ncol;
{
	register ScrnBuf base;

#ifdef	STATUSLINE	/* kato */
	nrow++;
#endif	STATUSLINE
	if ((base = (ScrnBuf) calloc ((unsigned)(nrow *= 2), sizeof (char *))) == 0)
		SysError (ERROR_SCALLOC);

	for (nrow--; nrow >= 0; nrow--)
		if ((base [nrow] = calloc ((unsigned)ncol, sizeof(char))) == 0)
			SysError (ERROR_SCALLOC2);

	return (base);
}

ScreenWrite (screen, str, flags, length)
/*
   Writes str into buf at row row and column col.  Characters are set to match
   flags.
 */
TScreen *screen;
char *str;
register unsigned flags;
register int length;		/* length of string */
{
	register char *att;
	register int avail  = screen->max_col - screen->cur_col + 1;
	register char *col;
#ifdef	SX9100
	register int length_sv;

	length_sv = length;
#endif	/* SX9100 */

	if (length > avail)
	    length = avail;
	if (length <= 0)
		return;

	col = screen->buf[avail = 2 * screen->cur_row] + screen->cur_col;
	att = screen->buf[avail + 1] + screen->cur_col;
	flags &= ATTRIBUTES;
	Bcopy(str, col, length);
#ifdef	KANJI	/* sano */
	if (flags & KCS) {
		while (length > 0) {
			*att++ = flags & ~KCS2;
			*att++ = flags & ~KCS1;
			length -= 2;
		}
#ifdef	SX9100
		if (*att & KCS2) {
			*(col + length_sv) = NULL;
			*att = NULL;
		}
#endif	/* SX9100 */
	} else
#endif	KANJI
#ifdef	SX9100
	{
		while (length-- > 0) {
			*att++ = flags;
		}
		if (*att & KCS2) {
			*(col + length_sv) = NULL;
			*att = NULL;
		}
	}
#else	/* SX9100 */
	while(length-- > 0)
		*att++ = flags;
#endif	/* SX9100 */
}

ScrnInsertLine (sb, last, where, n, size)
/*
   Inserts n blank lines at sb + where, treating last as a bottom margin.
   Size is the size of each entry in sb.
   Requires: 0 <= where < where + n <= last
   	     n <= MAX_ROWS
 */
register ScrnBuf sb;
int last;
register int where, n, size;
{
	register int i;
	char *save [2 * MAX_ROWS];


	/* save n lines at bottom */
	Bcopy ((char *) &sb [2 * (last -= n - 1)], (char *) save,
		2 * sizeof (char *) * n);
	
	/* clear contents of old rows */
	for (i = 2 * n - 1; i >= 0; i--)
		bzero ((char *) save [i], size);

	/*
	 * WARNING, overlapping copy operation.  Move down lines (pointers).
	 *
	 *   +----|---------|--------+
	 *
	 * is copied in the array to:
	 *
	 *   +--------|---------|----+
	 */
	Bcopy ((char *) &sb [2 * where], (char *) &sb [2 * (where + n)],
		2 * sizeof (char *) * (last - where));

	/* reuse storage for new lines at where */
	Bcopy ((char *)save, (char *) &sb[2 * where], 2 * sizeof(char *) * n);
}


ScrnDeleteLine (sb, last, where, n, size)
/*
   Deletes n lines at sb + where, treating last as a bottom margin.
   Size is the size of each entry in sb.
   Requires 0 <= where < where + n < = last
   	    n <= MAX_ROWS
 */
register ScrnBuf sb;
register int n, last, size;
int where;
{
	register int i;
	char *save [2 * MAX_ROWS];

	/* save n lines at where */
	Bcopy ((char *) &sb[2 * where], (char *)save, 2 * sizeof(char *) * n);

	/* clear contents of old rows */
	for (i = 2 * n - 1 ; i >= 0 ; i--)
		bzero ((char *) save [i], size);

	/* move up lines */
	Bcopy ((char *) &sb[2 * (where + n)], (char *) &sb[2 * where],
		2 * sizeof (char *) * ((last -= n - 1) - where));

	/* reuse storage for new bottom lines */
	Bcopy ((char *)save, (char *) &sb[2 * last],
		2 * sizeof(char *) * n);
}


ScrnInsertChar (sb, row, col, n, size)
/*
   Inserts n blanks in sb at row, col.  Size is the size of each row.
 */
ScrnBuf sb;
int row, size;
register int col, n;
{
	register int i, j;
	register char *ptr = sb [2 * row];
	register char *att = sb [2 * row + 1];

	for (i = size - 1; i >= col + n; i--) {
		ptr[i] = ptr[j = i - n];
		att[i] = att[j];
	}

	bzero (ptr + col, n);
	bzero (att + col, n);
}


ScrnDeleteChar (sb, row, col, n, size)
/*
   Deletes n characters in sb at row, col. Size is the size of each row.
 */
ScrnBuf sb;
register int row, size;
register int n, col;
{
	register char *ptr = sb[2 * row];
	register char *att = sb[2 * row + 1];
	register nbytes = (size - n - col);

	Bcopy (ptr + col + n, ptr + col, nbytes);
	Bcopy (att + col + n, att + col, nbytes);
	bzero (ptr + size - n, n);
	bzero (att + size - n, n);
}


ScrnRefresh (screen, toprow, leftcol, nrows, ncols, force)
/*
   Repaints the area enclosed by the parameters.
   Requires: (toprow, leftcol), (toprow + nrows, leftcol + ncols) are
   	     coordinates of characters in screen;
	     nrows and ncols positive.
 */
register TScreen *screen;
int toprow, leftcol, nrows, ncols;
Boolean force;			/* ... leading/trailing spaces */
{
	int y = toprow * FontHeight(screen) + screen->border +
		screen->fnt_norm->ascent;
	register int row;
	register int topline = screen->topline;
	int maxrow = toprow + nrows - 1;
	int scrollamt = screen->scroll_amt;
	int max = screen->max_row;
#ifdef	STATUSLINE	/* michael */
	int dostatus = 0, left, width;
#endif	STATUSLINE

#ifdef	STATUSLINE	/* michael */
	if (screen->statusline && maxrow == screen->max_row + 1) {
		dostatus++;
		maxrow--;
	}
#endif	STATUSLINE
	if(screen->cursor_col >= leftcol && screen->cursor_col <=
	 (leftcol + ncols - 1) && screen->cursor_row >= toprow + topline &&
	 screen->cursor_row <= maxrow + topline)
		screen->cursor_state = OFF;
#ifdef	STATUSLINE	/* michael */
	for ( ; ; ) {
#endif	STATUSLINE
	for (row = toprow; row <= maxrow; y += FontHeight(screen), row++) {
	   register char *chars;
	   register char *att;
	   register int col = leftcol;
	   int maxcol = leftcol + ncols - 1;
	   int lastind;
	   int flags;
	   int x, n;
	   GC gc;
	   Boolean hilite;	

	   if (row < screen->top_marg || row > screen->bot_marg)
		lastind = row;
	   else
		lastind = row - scrollamt;

	   if (lastind < 0 || lastind > max)
	   	continue;

	   chars = screen->buf [2 * (lastind + topline)];
	   att = screen->buf [2 * (lastind + topline) + 1];

	   if (row < screen->startHRow || row > screen->endHRow ||
	       (row == screen->startHRow && maxcol < screen->startHCol) ||
	       (row == screen->endHRow && col >= screen->endHCol))
	       {
	       /* row does not intersect selection; don't hilite */
	       if (!force) {
		   while (col <= maxcol && (att[col] & ~BOLD) == 0 &&
			  (chars[col] & ~040) == 0)
		       col++;

		   while (col <= maxcol && (att[maxcol] & ~BOLD) == 0 &&
			  (chars[maxcol] & ~040) == 0)
		       maxcol--;
	       }
	       hilite = False;
	   }
	   else {
	       /* row intersects selection; split into pieces of single type */
	       if (row == screen->startHRow && col < screen->startHCol) {
		   ScrnRefresh(screen, row, col, 1, screen->startHCol - col,
			       force);
		   col = screen->startHCol;
	       }
	       if (row == screen->endHRow && maxcol >= screen->endHCol) {
		   ScrnRefresh(screen, row, screen->endHCol, 1,
			       maxcol - screen->endHCol + 1, force);
		   maxcol = screen->endHCol - 1;
	       }
	       /* remaining piece should be hilited */
	       hilite = True;
	   }

#ifdef	KANJI	/* sano */
	   if ((att[col] & KCS2) && col != 0)
	       col--;
	   if (att[maxcol] & KCS1)
	       maxcol++;
#endif

	   if (col > maxcol) continue;

	   flags = att[col];

#ifdef	KANJI	/* sano */
	   if (flags & KCS)
#ifdef	STATUSLINE	/* michael */
		if ((!hilite && (flags & INVERSE) != 0
		    || hilite && (flags & INVERSE) == 0)
		    ^ (dostatus < 0 && screen->reversestatus)) {
#else	STATUSLINE
		if (!hilite && (flags & INVERSE) != 0
		    || hilite && (flags & INVERSE) == 0) {
#endif	STATUSLINE
			if (flags & BOLD)
				gc = screen->w_reverseboldGC;
			else
				gc = screen->w_reverseGC;
		} else {
			if (flags & BOLD)
				gc = screen->w_normalboldGC;
			else
				gc = screen->w_normalGC;
		}
	   else if (flags & KANACS)
#ifdef	STATUSLINE	/* michael */
		if ((!hilite && (flags & INVERSE) != 0
		    || hilite && (flags & INVERSE) == 0)
		    ^ (dostatus < 0 && screen->reversestatus)) {
#else	STATUSLINE
		if (!hilite && (flags & INVERSE) != 0
		    || hilite && (flags & INVERSE) == 0) {
#endif	STATUSLINE
			if (flags & BOLD)
				gc = screen->r_reverseboldGC;
			else
				gc = screen->r_reverseGC;
		} else {
			if (flags & BOLD)
				gc = screen->r_normalboldGC;
			else
				gc = screen->r_normalGC;
		}
	else
#endif	KANJI
#ifdef	STATUSLINE	/* michael */
		if ((!hilite && (flags & INVERSE) != 0
		    || hilite && (flags & INVERSE) == 0)
		    ^ (dostatus < 0 && screen->reversestatus))
#else	STATUSLINE
	   if ( (!hilite && (flags & INVERSE) != 0) ||
	        (hilite && (flags & INVERSE) == 0) )
#endif	STATUSLINE
	       if (flags & BOLD) gc = screen->reverseboldGC;
	       else gc = screen->reverseGC;
	   else
	       if (flags & BOLD) gc = screen->normalboldGC;
	       else gc = screen->normalGC;

	   x = CursorX(screen, col);
	   lastind = col;

	   for (; col <= maxcol; col++) {
		if (att[col] != flags) {
#ifdef	KANJI	/* sano */
		   if (flags & KCS) {
			XDrawImageString16(screen->display, TextWindow(screen),
				gc, x, y, (XChar2b *)&chars[lastind],
				(n = col - lastind) / 2);
			if ((flags & BOLD) && screen->w_enbolden)
			    XDrawString16(screen->display, TextWindow(screen),
				gc, x + 1, y, (XChar2b *)&chars[lastind], n/2);
		   } else if (flags & KANACS) {
		   	XDrawImageString(screen->display, TextWindow(screen),
				gc, x, y, &chars[lastind], n = col - lastind);
		   	if ((flags & BOLD) && screen->r_enbolden)
		 		XDrawString(screen->display, TextWindow(screen),
					 gc, x + 1, y, &chars[lastind], n);
		   } else {
#endif	KANJI
		   XDrawImageString(screen->display, TextWindow(screen), 
		        	gc, x, y, &chars[lastind], n = col - lastind);
		   if((flags & BOLD) && screen->enbolden)
		 	XDrawString(screen->display, TextWindow(screen), 
			 gc, x + 1, y, &chars[lastind], n);
#ifdef	KANJI	/* sano */
		   }
#endif	KANJI
		   if(flags & UNDERLINE) 
			XDrawLine(screen->display, TextWindow(screen), 
			 gc, x, y+1, x+n*FontWidth(screen), y+1);

		   x += (col - lastind) * FontWidth(screen);

		   lastind = col;

		   flags = att[col];

#ifdef	KANJI	/* sano */
		   if (flags & KCS)
#ifdef	STATUSLINE	/* michael */
			if ((!hilite && (flags & INVERSE) != 0
			    || hilite && (flags & INVERSE) == 0)
			    ^ (dostatus < 0 && screen->reversestatus)) {
#else	STATUSLINE
			if (!hilite && (flags & INVERSE) != 0
			    || hilite && (flags & INVERSE) == 0) {
#endif	STATUSLINE
			    if (flags & BOLD)
				gc = screen->w_reverseboldGC;
			    else
				gc = screen->w_reverseGC;
			} else {
			    if (flags & BOLD)
				gc = screen->w_normalboldGC;
			    else
				gc = screen->w_normalGC;
			}
		    else if (flags & KANACS)
#ifdef	STATUSLINE	/* michael */
			if ((!hilite && (flags & INVERSE) != 0
			    || hilite && (flags & INVERSE) == 0)
			    ^ (dostatus < 0 && screen->reversestatus)) {
#else	STATUSLINE
			if (!hilite && (flags & INVERSE) != 0
			    || hilite && (flags & INVERSE) == 0) {
#endif	STATUSLINE
			    if (flags & BOLD)
				gc = screen->r_reverseboldGC;
			    else
				gc = screen->r_reverseGC;
			} else {
			    if (flags & BOLD)
				gc = screen->r_normalboldGC;
			    else
				gc = screen->r_normalGC;
			}
		    else
#endif	KANJI
				
#ifdef	STATUSLINE	/* michael */
		    if ((!hilite && (flags & INVERSE) != 0
			|| hilite && (flags & INVERSE) == 0)
			^ (dostatus < 0 && screen->reversestatus))
#else	STATUSLINE
	   	    if (!hilite && (flags & INVERSE) != 0
		        || hilite && (flags & INVERSE) == 0)
#endif	STATUSLINE
	       		if (flags & BOLD) gc = screen->reverseboldGC;
	       		else gc = screen->reverseGC;
	  	    else 
	      		 if (flags & BOLD) gc = screen->normalboldGC;
	      		 else gc = screen->normalGC;
		}

		if(chars[col] == 0)
			chars[col] = ' ';
#ifdef	KANJI	/* sano */
		if (att[col] & KCS1)
			col++;
#endif	KANJI
	   }


#ifdef	KANJI	/* sano */
	   if (flags & KCS)
#ifdef	STATUSLINE	/* michael */
		if ((!hilite && (flags & INVERSE) != 0
		     || hilite && (flags & INVERSE) == 0)
		    ^ (dostatus < 0 && screen->reversestatus)) {
#else	STATUSLINE
		if (!hilite && (flags & INVERSE) != 0
		    || hilite && (flags & INVERSE) == 0) {
#endif	STATUSLINE
		    if (flags & BOLD)
			gc = screen->w_reverseboldGC;
		    else
			gc = screen->w_reverseGC;
		} else {
		    if (flags & BOLD)
			gc = screen->w_normalboldGC;
		    else
			gc = screen->w_normalGC;
		}
	   else if (flags & KANACS)
#ifdef	STATUSLINE	/* michael */
		if ((!hilite && (flags & INVERSE) != 0
		     || hilite && (flags & INVERSE) == 0)
		    ^ (dostatus < 0 && screen->reversestatus)) {
#else	STATUSLINE
		if (!hilite && (flags & INVERSE) != 0
		    || hilite && (flags & INVERSE) == 0) {
#endif	STATUSLINE
		    if (flags & BOLD)
			gc = screen->r_reverseboldGC;
		    else
			gc = screen->r_reverseGC;
		} else {
		    if (flags & BOLD)
			gc = screen->r_normalboldGC;
		    else
			gc = screen->r_normalGC;
		}
	   else
#endif	KANJI
		
#ifdef	STATUSLINE	/* michael */
	    if ((!hilite && (flags & INVERSE) != 0
		 || hilite && (flags & INVERSE) == 0)
		^ (dostatus < 0 && screen->reversestatus))
#else	STATUSLINE
	   if (!hilite && (flags & INVERSE) != 0
	       || hilite && (flags & INVERSE) == 0)
#endif	STATUSLINE
	       if (flags & BOLD) gc = screen->reverseboldGC;
	       else gc = screen->reverseGC;
	   else 
	       if (flags & BOLD) gc = screen->normalboldGC;
	       else gc = screen->normalGC;
#ifdef	KANJI	/* sano */
	   if (flags & KCS) {
		XDrawImageString16(screen->display, TextWindow(screen), gc,
			x, y, (XChar2b *)&chars[lastind],
			(n = col - lastind) / 2);
		if ((flags & BOLD) && screen->w_enbolden)
		    XDrawString16(screen->display, TextWindow(screen), gc,
			x + 1, y, (XChar2b *)&chars[lastind], n / 2);
	   } else if (flags & KANACS) {
	   	XDrawImageString(screen->display, TextWindow(screen), gc, 
	         	x, y, &chars[lastind], n = col - lastind);
	   	if((flags & BOLD) && screen->r_enbolden)
			XDrawString(screen->display, TextWindow(screen), gc,
			x + 1, y, &chars[lastind], n);
	   } else {
#endif	KANJI
	   XDrawImageString(screen->display, TextWindow(screen), gc, 
	         x, y, &chars[lastind], n = col - lastind);
	   if((flags & BOLD) && screen->enbolden)
		XDrawString(screen->display, TextWindow(screen), gc,
		x + 1, y, &chars[lastind], n);
#ifdef	KANJI	/* sano */
	   }
#endif	KANJI
	   if(flags & UNDERLINE) 
		XDrawLine(screen->display, TextWindow(screen), gc, 
		 x, y+1, x + n * FontWidth(screen), y+1);
	}
#ifdef	STATUSLINE	/* michael */
	if (dostatus <= 0)
		break;
	dostatus = -1;
	topline = 0;
	scrollamt = 0;
	toprow = maxrow = max = screen->max_row + 1;
	left = CursorX(screen, leftcol);
	width = ncols * FontWidth(screen);
	if (leftcol == 0) {
		left--;
		width++;
	}
	if (leftcol + ncols - 1 >= screen->max_col)
		width++;
	XFillRectangle(screen->display, TextWindow(screen),
		screen->reversestatus ? screen->normalGC : screen->reverseGC,
		left, y - screen->fnt_norm->max_bounds.ascent,
		width, screen->statusheight);
	if (!screen->reversestatus)
		StatusBox(screen);
	y++;
	}	/* end of for(;;) { */
#endif	STATUSLINE
}

ClearBufRows (screen, first, last)
/*
   Sets the rows first though last of the buffer of screen to spaces.
   Requires first <= last; first, last are rows of screen->buf.
 */
register TScreen *screen;
register int first, last;
{
	first *= 2;
	last = 2 * last + 1;
	while (first <= last)
		bzero (screen->buf [first++], (screen->max_col + 1));
}

ScreenResize (screen, width, height, flags)
/*
   Resizes screen:
   1. If new window would have fractional characters, sets window size so as to
      discard fractional characters and returns -1.
      Minimum screen size is 1 X 1.
      Note that this causes another ExposeWindow event.
   2. Enlarges screen->buf if necessary.  New space is appended to the bottom
      and to the right
   3. Reduces  screen->buf if necessary.  Old space is removed from the bottom
      and from the right
   4. Cursor is positioned as closely to its former position as possible
   5. Sets screen->max_row and screen->max_col to reflect new size
   6. Maintains the inner border (and clears the border on the screen).
   7. Clears origin mode and sets scrolling region to be entire screen.
   8. Returns 0
 */
register TScreen *screen;
int width, height;
unsigned *flags;
{
	int rows, cols;
	register int index;
	int savelines;
	register ScrnBuf sb = screen->allbuf;
	register ScrnBuf ab = screen->altbuf;
	register int x;
	int border = 2 * screen->border;
	int i, j, k;
#ifdef	STATUSLINE	/* michael */
	register char *s10, *s11;	/* keep status line */
#endif	STATUSLINE
#ifdef sun
#ifdef TIOCSSIZE
	struct ttysize ts;
#endif	/* TIOCSSIZE */
#else	/* sun */
#ifdef TIOCSWINSZ
	struct winsize ws;
#endif	/* TIOCSWINSZ */
#endif	/* sun */
	Window tw = TextWindow (screen);

	/* clear the right and bottom internal border because of NorthWest
	   gravity might have left junk on the right and bottom edges */
	XClearArea (screen->display, tw,
		    width - screen->border, 0,                /* right edge */
		    screen->border, height,           /* from top to bottom */
		    False);
	XClearArea (screen->display, tw, 
		    0, height - screen->border,	                  /* bottom */
		    width, screen->border,         /* all across the bottom */
		    False);

	/* round so that it is unlikely the screen will change size on  */
	/* small mouse movements.					*/
#ifdef	STATUSLINE	/* michael */
	rows = (height + FontHeight(screen) / 2 - border - screen->statusheight) /
#else	STATUSLINE
	rows = (height + FontHeight(screen) / 2 - border) /
#endif	STATUSLINE
	 FontHeight(screen);
	cols = (width + FontWidth(screen) / 2 - border - screen->scrollbar) /
	 FontWidth(screen);
	if (rows < 1) rows = 1;
	if (cols < 1) cols = 1;

	/* change buffers if the screen has changed size */
	if (screen->max_row != rows - 1 || screen->max_col != cols - 1) {
		if(screen->cursor_state)
			HideCursor();
		savelines = screen->scrollWidget ? screen->savelines : 0;
		j = screen->max_col + 1;
		i = cols - j;
		k = screen->max_row;
		if(rows < k)
			k = rows;
		if(ab) {
			/* resize current lines in alternate buf */
			for (index = x = 0; index <= k; x += 2, index++) {
				if ((ab[x] = realloc(ab[x], (unsigned) cols)) == NULL)
					SysError(ERROR_SREALLOC);
				if((ab[x + 1] = realloc(ab[x + 1], (unsigned) cols)) ==
				 NULL)
					SysError (ERROR_SREALLOC2);
				if (cols > j) {
					bzero (ab [x] + j, i);
					bzero (ab [x + 1] + j, i);
				}
			}
			/* discard excess bottom rows in alt buf */
			for (index = rows, x = 2 * k ; index <=
			 screen->max_row; x += 2, index++) {
			   free (ab [x]);
			   free (ab [x + 1]);
			}
		}
		/* resize current lines */
#ifdef	STATUSLINE	/* michael */
                k += savelines + 1;	/* includes status line */
#else	STATUSLINE
                k += savelines;
#endif	STATUSLINE
		for (index = x = 0; index <= k; x += 2, index++) {
			if ((sb[x] = realloc(sb[x], (unsigned) cols)) == NULL)
				SysError(ERROR_SREALLOC3);
			if((sb[x + 1] = realloc(sb[x + 1], (unsigned) cols)) == NULL)
				SysError (ERROR_SREALLOC4);
			if (cols > j) {
				bzero (sb [x] + j, i);
				bzero (sb [x + 1] + j, i);
			}
		}
#ifdef	STATUSLINE	/* michael */
		/* save status line */
		s10 = sb[i = 2 * (savelines + screen->max_row + 1)];
		s11 = sb[i + 1];
		/* discard excess bottom rows */
		for (index = rows, x = 2 * (k - 1); index <= screen->max_row;
#else	STATUSLINE
		/* discard excess bottom rows */
		for (index = rows, x = 2 * k; index <= screen->max_row;
#endif	STATUSLINE
		 x += 2, index++) {
		   free (sb [x]);
		   free (sb [x + 1]);
		}
		if(ab) {
		    if((ab = (ScrnBuf)realloc((char *)ab,
		     (unsigned) 2 * sizeof(char *) * rows)) == NULL)
			SysError (ERROR_RESIZE);
		    screen->altbuf = ab;
		}
#ifdef	STATUSLINE	/* michael */
		k = 2 * (rows + savelines + 1);	/* includes status line */
#else	STATUSLINE
		k = 2 * (rows + savelines);
#endif	STATUSLINE
		/* resize sb */
		if((sb = (ScrnBuf)realloc((char *) sb, (unsigned) k * sizeof(char *)))
		  == NULL)
			SysError (ERROR_RESIZE2);
		screen->allbuf = sb;
		screen->buf = &sb[2 * savelines];
	
		if(ab) {
			/* create additional bottom rows as required in alt */
			for (index = screen->max_row + 1, x = 2 * index ;
			 index < rows; x += 2, index++) {
			   if((ab[x] = calloc ((unsigned) cols, sizeof(char))) == NULL)
				SysError(ERROR_RESIZROW);
			   if((ab[x + 1] = calloc ((unsigned) cols, sizeof(char))) == NULL)
				SysError(ERROR_RESIZROW2);
			}
		}
		/* create additional bottom rows as required */
		for (index = screen->max_row + 1, x = 2 * (index + savelines) ;
		 index < rows; x += 2, index++) {
		   if((sb[x] = calloc ((unsigned) cols, sizeof(char))) == NULL)
			SysError(ERROR_RESIZROW3);
		   if((sb[x + 1] = calloc ((unsigned) cols, sizeof(char))) == NULL)
			SysError(ERROR_RESIZROW4);
		}

		screen->max_row = rows - 1;
		screen->max_col = cols - 1;
#ifdef	STATUSLINE	/* michael */
		sb[i = 2 * (savelines + screen->max_row + 1)] = s10;
		sb[i + 1] = s11;
#endif	STATUSLINE
	
		/* adjust scrolling region */
		screen->top_marg = 0;
		screen->bot_marg = screen->max_row;
		*flags &= ~ORIGIN;
	
#ifdef	STATUSLINE	/* michael */
		if (screen->instatus)
			screen->cur_row = screen->max_row + 1;
		else
#endif	STATUSLINE
		if (screen->cur_row > screen->max_row)
			screen->cur_row = screen->max_row;
		if (screen->cur_col > screen->max_col)
			screen->cur_col = screen->max_col;
	
#ifdef	STATUSLINE	/* michael */
		screen->fullVwin.height = height - border - screen->statusheight;
#else	STATUSLINE
		screen->fullVwin.height = height - border;
#endif	STATUSLINE
		screen->fullVwin.width = width - border - screen->scrollbar;

	} else if(FullHeight(screen) == height && FullWidth(screen) == width)
	 	return(0);	/* nothing has changed at all */

	if(screen->scrollWidget)
		ResizeScrollBar(screen->scrollWidget, -1, -1, height);
	
	screen->fullVwin.fullheight = height;
	screen->fullVwin.fullwidth = width;
	ResizeSelection (screen, rows, cols);
#ifdef sun
#ifdef TIOCSSIZE
	/* Set tty's idea of window size */
	ts.ts_lines = rows;
	ts.ts_cols = cols;
	ioctl (screen->respond, TIOCSSIZE, &ts);
#ifdef SIGWINCH
	if(screen->pid > 1) {
		int	pgrp;
		
		if (ioctl (screen->respond, TIOCGPGRP, &pgrp) != -1)
			killpg(pgrp, SIGWINCH);
	}
#endif	/* SIGWINCH */
#endif	/* TIOCSSIZE */
#else	/* sun */
#ifdef TIOCSWINSZ
	/* Set tty's idea of window size */
	ws.ws_row = rows;
	ws.ws_col = cols;
	ws.ws_xpixel = width;
	ws.ws_ypixel = height;
	ioctl (screen->respond, TIOCSWINSZ, (char *)&ws);
#ifdef SIGWINCH	/* change to SIGWINCH if this doesn't work for you */
	if(screen->pid > 1) {
		int	pgrp;
		
		if (ioctl (screen->respond, TIOCGPGRP, &pgrp) != -1)
			killpg(pgrp, SIGWINCH);
	}
#endif	/* SIGWINCH */
#endif	/* TIOCSWINSZ */
#endif	/* sun */
	return (0);
}


