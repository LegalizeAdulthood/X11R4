/*
 *	$XConsortium: util.c,v 1.10 88/10/07 08:20:08 swick Exp $
 *	$Header: util.c,v 1.1 89/05/30 14:53:58 kagotani Rel $
 */

#include <X11/copyright.h>

/*
 *	Copyright 1989 OMRON TATEISI ELECTRONICS CO.
 *
 *	Original Source: ./client/kterm/util.c
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

/* util.c */

#ifndef lint
static char rcs_id[] = "$XConsortium: util.c,v 1.10 88/10/07 08:20:08 swick Exp $";
static char kterm_id[] = "$Header: util.c,v 1.1 89/05/30 14:53:58 kagotani Rel $";
#endif	/* lint */

#ifdef FACOM
#include <sys/types.h>
#endif

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <signal.h>
#include <setjmp.h>
typedef int *jmp_ptr;

#include "ptyx.h"
#include "data.h"
#include "error.h"
#ifdef	XJP
extern XJp_attributes	jp_att;
#endif	/* XJP */

/*
 * These routines are used for the jump scroll feature
 */
FlushScroll(screen)
register TScreen *screen;
{
	register int i;
	register int shift = -screen->topline;
	register int bot = screen->max_row - shift;
	register int refreshtop;
	register int refreshheight;
	register int scrolltop;
	register int scrollheight;

	if(screen->cursor_state)
		HideCursor();
	if(screen->scroll_amt > 0) {
		refreshheight = screen->refresh_amt;
		scrollheight = screen->bot_marg - screen->top_marg -
		 refreshheight + 1;
		if((refreshtop = screen->bot_marg - refreshheight + 1 + shift) >
		 (i = screen->max_row - screen->scroll_amt + 1))
			refreshtop = i;
		if(screen->scrollWidget && !screen->alternate
		 && screen->top_marg == 0) {
			scrolltop = 0;
			if((scrollheight += shift) > i)
				scrollheight = i;
			if((i = screen->bot_marg - bot) > 0 &&
			 (refreshheight -= i) < screen->scroll_amt)
				refreshheight = screen->scroll_amt;
			if((i = screen->savedlines) < screen->savelines) {
				if((i += screen->scroll_amt) >
				  screen->savelines)
					i = screen->savelines;
				screen->savedlines = i;
				ScrollBarDrawThumb(screen->scrollWidget);
			}
		} else {
			scrolltop = screen->top_marg + shift;
			if((i = bot - (screen->bot_marg - screen->refresh_amt +
			 screen->scroll_amt)) > 0) {
				if(bot < screen->bot_marg)
					refreshheight = screen->scroll_amt + i;
			} else {
				scrollheight += i;
				refreshheight = screen->scroll_amt;
				if((i = screen->top_marg + screen->scroll_amt -
				 1 - bot) > 0) {
					refreshtop += i;
					refreshheight -= i;
				}
			}
		}
	} else {
		refreshheight = -screen->refresh_amt;
		scrollheight = screen->bot_marg - screen->top_marg -
		 refreshheight + 1;
		refreshtop = screen->top_marg + shift;
		scrolltop = refreshtop + refreshheight;
		if((i = screen->bot_marg - bot) > 0)
			scrollheight -= i;
		if((i = screen->top_marg + refreshheight - 1 - bot) > 0)
			refreshheight -= i;
	}
	if(scrollheight > 0) {
		if (screen->multiscroll && scrollheight == 1 &&
		 screen->topline == 0 && screen->top_marg == 0 &&
		 screen->bot_marg == screen->max_row) {
			if (screen->incopy < 0 && screen->scrolls == 0)
				CopyWait (screen);
			screen->scrolls++;
		} else {
			if (screen->incopy)
				CopyWait (screen);
			screen->incopy = -1;
		}

		XCopyArea (
		    screen->display, 
		    TextWindow(screen),
		    TextWindow(screen),
		    screen->normalGC,
		    (int) screen->border + screen->scrollbar,
		    (int) (scrolltop + screen->scroll_amt) * FontHeight(screen)
			+ screen->border, 
		    (unsigned) Width(screen),
		    (unsigned) scrollheight * FontHeight(screen),
		    (int) screen->border + screen->scrollbar, 
		    (int) scrolltop*FontHeight(screen) + screen->border);
	}
	ScrollSelection(screen, -(screen->scroll_amt));
	screen->scroll_amt = 0;
	screen->refresh_amt = 0;
	if(refreshheight > 0) {
		XClearArea (
		    screen->display,
		    TextWindow(screen),
		    (int) screen->border + screen->scrollbar,
		    (int) refreshtop * FontHeight(screen) + screen->border,
		    (unsigned) Width(screen),
		    (unsigned) refreshheight * FontHeight(screen),
		    FALSE);
		ScrnRefresh(screen, refreshtop, 0, refreshheight,
		 screen->max_col + 1, False);
	}
}

AddToRefresh(screen)
register TScreen *screen;
{
	register int amount = screen->refresh_amt;
	register int row = screen->cur_row;

#ifdef	STATUSLINE	/* michael */
	if(amount == 0 || screen->instatus)
#else	STATUSLINE
	if(amount == 0)
#endif	STATUSLINE
		return(0);
	if(amount > 0) {
		register int bottom;

		if(row == (bottom = screen->bot_marg) - amount) {
			screen->refresh_amt++;
			return(1);
		}
		return(row >= bottom - amount + 1 && row <= bottom);
	} else {
		register int top;

		amount = -amount;
		if(row == (top = screen->top_marg) + amount) {
			screen->refresh_amt--;
			return(1);
		}
		return(row <= top + amount - 1 && row >= top);
	}
}

/* 
 * scrolls the screen by amount lines, erases bottom, doesn't alter 
 * cursor position (i.e. cursor moves down amount relative to text).
 * All done within the scrolling region, of course. 
 * requires: amount > 0
 */
Scroll(screen, amount)
register TScreen *screen;
register int amount;
{
	register int i = screen->bot_marg - screen->top_marg + 1;
	register int shift;
	register int bot;
	register int refreshtop;
	register int refreshheight;
	register int scrolltop;
	register int scrollheight;

	if(screen->cursor_state)
		HideCursor();
	if (amount > i)
		amount = i;
    if(screen->jumpscroll) {
	if(screen->scroll_amt > 0) {
		if(screen->refresh_amt + amount > i)
			FlushScroll(screen);
		screen->scroll_amt += amount;
		screen->refresh_amt += amount;
	} else {
		if(screen->scroll_amt < 0)
			FlushScroll(screen);
		screen->scroll_amt = amount;
		screen->refresh_amt = amount;
	}
	refreshheight = 0;
    } else {
	ScrollSelection(screen, -(amount));
	if (amount == i) {
		ClearScreen(screen);
		return;
	}
	shift = -screen->topline;
	bot = screen->max_row - shift;
	scrollheight = i - amount;
	refreshheight = amount;
	if((refreshtop = screen->bot_marg - refreshheight + 1 + shift) >
	 (i = screen->max_row - refreshheight + 1))
		refreshtop = i;
	if(screen->scrollWidget && !screen->alternate
	 && screen->top_marg == 0) {
		scrolltop = 0;
		if((scrollheight += shift) > i)
			scrollheight = i;
		if((i = screen->savedlines) < screen->savelines) {
			if((i += amount) > screen->savelines)
				i = screen->savelines;
			screen->savedlines = i;
			ScrollBarDrawThumb(screen->scrollWidget);
		}
	} else {
		scrolltop = screen->top_marg + shift;
		if((i = screen->bot_marg - bot) > 0) {
			scrollheight -= i;
			if((i = screen->top_marg + amount - 1 - bot) >= 0) {
				refreshtop += i;
				refreshheight -= i;
			}
		}
	}
	if(scrollheight > 0) {
		if (screen->multiscroll
		&& amount==1 && screen->topline == 0
		&& screen->top_marg==0
		&& screen->bot_marg==screen->max_row) {
			if (screen->incopy<0 && screen->scrolls==0)
				CopyWait(screen);
			screen->scrolls++;
		} else {
			if (screen->incopy)
				CopyWait(screen);
			screen->incopy = -1;
		}

		XCopyArea(
		    screen->display, 
		    TextWindow(screen),
		    TextWindow(screen),
		    screen->normalGC,
		    (int) screen->border + screen->scrollbar,
		    (int) (scrolltop+amount) * FontHeight(screen) + screen->border, 
		    (unsigned) Width(screen),
		    (unsigned) scrollheight * FontHeight(screen),
		    (int) screen->border + screen->scrollbar,
		    (int) scrolltop * FontHeight(screen) + screen->border);
	}
	if(refreshheight > 0) {
		XClearArea (
		   screen->display,
		   TextWindow(screen),
		   (int) screen->border + screen->scrollbar,
		   (int) refreshtop * FontHeight(screen) + screen->border,
		   (unsigned) Width(screen),
		   (unsigned) refreshheight * FontHeight(screen),
		   FALSE);
		if(refreshheight > shift)
			refreshheight = shift;
	}
    }
	if(screen->scrollWidget && !screen->alternate && screen->top_marg == 0)
		ScrnDeleteLine(screen->allbuf, screen->bot_marg +
		 screen->savelines, 0, amount, screen->max_col + 1);
	else
		ScrnDeleteLine(screen->buf, screen->bot_marg, screen->top_marg,
		 amount, screen->max_col + 1);
	if(refreshheight > 0)
		ScrnRefresh(screen, refreshtop, 0, refreshheight,
		 screen->max_col + 1, False);
}


/*
 * Reverse scrolls the screen by amount lines, erases top, doesn't alter
 * cursor position (i.e. cursor moves up amount relative to text).
 * All done within the scrolling region, of course.
 * Requires: amount > 0
 */
RevScroll(screen, amount)
register TScreen *screen;
register int amount;
{
	register int i = screen->bot_marg - screen->top_marg + 1;
	register int shift;
	register int bot;
	register int refreshtop;
	register int refreshheight;
	register int scrolltop;
	register int scrollheight;

	if(screen->cursor_state)
		HideCursor();
	if (amount > i)
		amount = i;
    if(screen->jumpscroll) {
	if(screen->scroll_amt < 0) {
		if(-screen->refresh_amt + amount > i)
			FlushScroll(screen);
		screen->scroll_amt -= amount;
		screen->refresh_amt -= amount;
	} else {
		if(screen->scroll_amt > 0)
			FlushScroll(screen);
		screen->scroll_amt = -amount;
		screen->refresh_amt = -amount;
	}
    } else {
	shift = -screen->topline;
	bot = screen->max_row - shift;
	refreshheight = amount;
	scrollheight = screen->bot_marg - screen->top_marg -
	 refreshheight + 1;
	refreshtop = screen->top_marg + shift;
	scrolltop = refreshtop + refreshheight;
	if((i = screen->bot_marg - bot) > 0)
		scrollheight -= i;
	if((i = screen->top_marg + refreshheight - 1 - bot) > 0)
		refreshheight -= i;
	if(scrollheight > 0) {
		if (screen->multiscroll
		&& amount==1 && screen->topline == 0
		&& screen->top_marg==0
		&& screen->bot_marg==screen->max_row) {
			if (screen->incopy<0 && screen->scrolls==0)
				CopyWait(screen);
			screen->scrolls++;
		} else {
			if (screen->incopy)
				CopyWait(screen);
			screen->incopy = -1;
		}

		XCopyArea (
		    screen->display,
		    TextWindow(screen),
		    TextWindow(screen),
		    screen->normalGC,
		    (int) screen->border + screen->scrollbar, 
		    (int) (scrolltop-amount) * FontHeight(screen) + screen->border, 
		    (unsigned) Width(screen),
		    (unsigned) scrollheight * FontHeight(screen),
		    (int) screen->border + screen->scrollbar,
		    (int) scrolltop * FontHeight(screen) + screen->border);
	}
	if(refreshheight > 0)
		XClearArea (
		    screen->display,
		    TextWindow(screen),
		    (int) screen->border + screen->scrollbar,
		    (int) refreshtop * FontHeight(screen) + screen->border,
		    (unsigned) Width(screen),
		    (unsigned) refreshheight * FontHeight(screen),
		    FALSE);
    }
	ScrnInsertLine (screen->buf, screen->bot_marg, screen->top_marg,
			amount, screen->max_col + 1);
}

/*
 * If cursor not in scrolling region, returns.  Else,
 * inserts n blank lines at the cursor's position.  Lines above the
 * bottom margin are lost.
 */
InsertLine (screen, n)
register TScreen *screen;
register int n;
{
	register int i;
	register int shift;
	register int bot;
	register int refreshtop;
	register int refreshheight;
	register int scrolltop;
	register int scrollheight;

	if (screen->cur_row < screen->top_marg ||
	 screen->cur_row > screen->bot_marg)
		return;
	if(screen->cursor_state)
		HideCursor();
	screen->do_wrap = 0;
	if (n > (i = screen->bot_marg - screen->cur_row + 1))
		n = i;
    if(screen->jumpscroll) {
	if(screen->scroll_amt <= 0 &&
	 screen->cur_row <= -screen->refresh_amt) {
		if(-screen->refresh_amt + n > screen->max_row + 1)
			FlushScroll(screen);
		screen->scroll_amt -= n;
		screen->refresh_amt -= n;
	} else if(screen->scroll_amt)
		FlushScroll(screen);
    }
    if(!screen->scroll_amt) {
	shift = -screen->topline;
	bot = screen->max_row - shift;
	refreshheight = n;
	scrollheight = screen->bot_marg - screen->cur_row - refreshheight + 1;
	refreshtop = screen->cur_row + shift;
	scrolltop = refreshtop + refreshheight;
	if((i = screen->bot_marg - bot) > 0)
		scrollheight -= i;
	if((i = screen->cur_row + refreshheight - 1 - bot) > 0)
		refreshheight -= i;
	if(scrollheight > 0) {
		if (screen->incopy)
			CopyWait (screen);
		screen->incopy = -1;
		XCopyArea (
		    screen->display, 
		    TextWindow(screen),
		    TextWindow(screen),
		    screen->normalGC,
		    (int) screen->border + screen->scrollbar,
		    (int) (scrolltop - n) * FontHeight(screen) + screen->border, 
		    (unsigned) Width(screen),
		    (unsigned) scrollheight * FontHeight(screen),
		    (int) screen->border + screen->scrollbar,
		    (int) scrolltop * FontHeight(screen) + screen->border);
	}
	if(refreshheight > 0)
		XClearArea (
		    screen->display,
		    TextWindow(screen),
		    (int) screen->border + screen->scrollbar,
		    (int) refreshtop * FontHeight(screen) + screen->border,
		    (unsigned) Width(screen),
		    (unsigned) refreshheight * FontHeight(screen),
		    FALSE);
    }
	/* adjust screen->buf */
	ScrnInsertLine(screen->buf, screen->bot_marg, screen->cur_row, n,
			screen->max_col + 1);
}

/*
 * If cursor not in scrolling region, returns.  Else, deletes n lines
 * at the cursor's position, lines added at bottom margin are blank.
 */
DeleteLine(screen, n)
register TScreen *screen;
register int n;
{
	register int i;
	register int shift;
	register int bot;
	register int refreshtop;
	register int refreshheight;
	register int scrolltop;
	register int scrollheight;

	if (screen->cur_row < screen->top_marg ||
	 screen->cur_row > screen->bot_marg)
		return;
	if(screen->cursor_state)
		HideCursor();
	screen->do_wrap = 0;
	if (n > (i = screen->bot_marg - screen->cur_row + 1))
		n = i;
    if(screen->jumpscroll) {
	if(screen->scroll_amt >= 0 && screen->cur_row == screen->top_marg) {
		if(screen->refresh_amt + n > screen->max_row + 1)
			FlushScroll(screen);
		screen->scroll_amt += n;
		screen->refresh_amt += n;
	} else if(screen->scroll_amt)
		FlushScroll(screen);
    }
    if(!screen->scroll_amt) {

	shift = -screen->topline;
	bot = screen->max_row - shift;
	scrollheight = i - n;
	refreshheight = n;
	if((refreshtop = screen->bot_marg - refreshheight + 1 + shift) >
	 (i = screen->max_row - refreshheight + 1))
		refreshtop = i;
	if(screen->scrollWidget && !screen->alternate && screen->cur_row == 0) {
		scrolltop = 0;
		if((scrollheight += shift) > i)
			scrollheight = i;
		if((i = screen->savedlines) < screen->savelines) {
			if((i += n) > screen->savelines)
				i = screen->savelines;
			screen->savedlines = i;
			ScrollBarDrawThumb(screen->scrollWidget);
		}
	} else {
		scrolltop = screen->cur_row + shift;
		if((i = screen->bot_marg - bot) > 0) {
			scrollheight -= i;
			if((i = screen->cur_row + n - 1 - bot) >= 0) {
				refreshheight -= i;
			}
		}
	}
	if(scrollheight > 0) {
		if (screen->incopy)
			CopyWait(screen);
		screen->incopy = -1;

		XCopyArea (
		    screen->display,
		    TextWindow(screen),
		    TextWindow(screen),
		    screen->normalGC,
		    (int) screen->border + screen->scrollbar,
		    (int) (scrolltop + n) * FontHeight(screen) + screen->border, 
		    (unsigned) Width(screen),
		    (unsigned) scrollheight * FontHeight(screen),
		    (int) screen->border + screen->scrollbar,
		    (int) scrolltop * FontHeight(screen) + screen->border);
	}
	if(refreshheight > 0)
		XClearArea (
		    screen->display,
		    TextWindow(screen),
		    (int) screen->border + screen->scrollbar,
		    (int) refreshtop * FontHeight(screen) + screen->border,
		    (unsigned) Width(screen),
		    (unsigned) refreshheight * FontHeight(screen),
		    FALSE);
    }
	/* adjust screen->buf */
	if(screen->scrollWidget && !screen->alternate && screen->cur_row == 0)
		ScrnDeleteLine(screen->allbuf, screen->bot_marg +
		 screen->savelines, 0, n, screen->max_col + 1);
	else
		ScrnDeleteLine(screen->buf, screen->bot_marg, screen->cur_row,
		 n, screen->max_col + 1);
}

/*
 * Insert n blanks at the cursor's position, no wraparound
 */
InsertChar (screen, n)
register TScreen *screen;
register int n;
{
	register int width = n * FontWidth(screen), cx, cy;

	if(screen->cursor_state)
		HideCursor();
	screen->do_wrap = 0;
#ifdef	STATUSLINE	/* michael */
	if(screen->cur_row - screen->topline <= screen->max_row ||
	   screen->instatus) {
#else	STATUSLINE
	if(screen->cur_row - screen->topline <= screen->max_row) {
#endif	STATUSLINE
	    if(!AddToRefresh(screen)) {
		if(screen->scroll_amt)
			FlushScroll(screen);
	
#ifdef	KANJI	/* kagotani */
		BreakKanji(screen);
#endif	KANJI

		if (screen->incopy)
			CopyWait (screen);
		screen->incopy = -1;
	
		cx = CursorX (screen, screen->cur_col);
		cy = CursorY (screen, screen->cur_row);
		XCopyArea(
		    screen->display,
		    TextWindow(screen), TextWindow(screen),
		    screen->normalGC,
		    cx, cy,
		    (unsigned) Width(screen)
		        - (screen->cur_col + n) * FontWidth(screen),
		    (unsigned) FontHeight(screen), 
		    cx + width, cy);
		XFillRectangle(
		    screen->display,
		    TextWindow(screen), 
#ifdef	STATUSLINE	/* michael */
		    screen->instatus ? screen->normalGC :
#endif	STATUSLINE
		    screen->reverseGC,
		    cx, cy,
		    (unsigned) width, (unsigned) FontHeight(screen));
	    }
	}
	/* adjust screen->buf */
	ScrnInsertChar(screen->buf, screen->cur_row, screen->cur_col, n,
			screen->max_col + 1);
}

/*
 * Deletes n chars at the cursor's position, no wraparound.
 */
DeleteChar (screen, n)
register TScreen *screen;
register int	n;
{
	register int width, cx, cy;

	if(screen->cursor_state)
		HideCursor();
	screen->do_wrap = 0;
	if (n > (width = screen->max_col + 1 - screen->cur_col))
	  	n = width;
		
#ifdef	STATUSLINE	/* michael */
	if(screen->cur_row - screen->topline <= screen->max_row ||
	   screen->instatus) {
#else	STATUSLINE
	if(screen->cur_row - screen->topline <= screen->max_row) {
#endif	STATUSLINE
	    if(!AddToRefresh(screen)) {
		if(screen->scroll_amt)
			FlushScroll(screen);
	
		width = n * FontWidth(screen);
	
#ifdef	KANJI	/* kagotani */
		BreakKanji(screen);
		screen->cur_col += n;
		BreakKanji(screen);
		screen->cur_col -= n;
#endif	KANJI

		if (screen->incopy)
			CopyWait (screen);
		screen->incopy = -1;
	
		cx = CursorX (screen, screen->cur_col);
		cy = CursorY (screen, screen->cur_row);
		XCopyArea(screen->display,
		     TextWindow(screen), TextWindow(screen),
		     screen->normalGC, 
		     cx + width, cy,
		     Width(screen) - (screen->cur_col + n) * FontWidth(screen),
		     FontHeight(screen), 
		     cx, cy);
		XFillRectangle (screen->display, TextWindow(screen),
#ifdef	STATUSLINE	/* michael */
		     screen->instatus? screen->normalGC :
#endif	STATUSLINE
		     screen->reverseGC,
		     screen->border + screen->scrollbar + Width(screen) - width,
		     cy, width, FontHeight(screen));
	    }
	}
	/* adjust screen->buf */
	ScrnDeleteChar (screen->buf, screen->cur_row, screen->cur_col, n,
			screen->max_col + 1);

}

/*
 * Clear from cursor position to beginning of display, inclusive.
 */
ClearAbove (screen)
register TScreen *screen;
{
	register top, height;

	if(screen->cursor_state)
		HideCursor();
	if((top = -screen->topline) <= screen->max_row) {
		if(screen->scroll_amt)
			FlushScroll(screen);
		if((height = screen->cur_row + top) > screen->max_row)
			height = screen->max_row;
		if((height -= top) > 0)
			XClearArea(screen->display, TextWindow(screen),
			 screen->border + screen->scrollbar, top *
			 FontHeight(screen) + screen->border,
			 Width(screen), height * FontHeight(screen), FALSE);

		if(screen->cur_row - screen->topline <= screen->max_row)
			ClearLeft(screen);
	}
	ClearBufRows(screen, 0, screen->cur_row - 1);
}

/*
 * Clear from cursor position to end of display, inclusive.
 */
ClearBelow (screen)
register TScreen *screen;
{
	register top;

	ClearRight(screen);
	if((top = screen->cur_row - screen->topline) <= screen->max_row) {
		if(screen->scroll_amt)
			FlushScroll(screen);
		if(++top <= screen->max_row)
			XClearArea(screen->display, TextWindow(screen),
			 screen->border + screen->scrollbar, top *
			 FontHeight(screen) + screen->border,
			 Width(screen), (screen->max_row - top + 1) *
			 FontHeight(screen), FALSE);
	}
	ClearBufRows(screen, screen->cur_row + 1, screen->max_row);
}

/* 
 * Clear last part of cursor's line, inclusive.
 */
ClearRight (screen)
register TScreen *screen;
{
	if(screen->cursor_state)
		HideCursor();
	screen->do_wrap = 0;
#ifdef	STATUSLINE	/* michael */
	if(screen->cur_row - screen->topline <= screen->max_row ||
	   screen->instatus) {
#else	STATUSLINE
	if(screen->cur_row - screen->topline <= screen->max_row) {
#endif	STATUSLINE
	    if(!AddToRefresh(screen)) {
	if(screen->scroll_amt)
		FlushScroll(screen);
#ifdef	KANJI	/* kagotani */
		BreakKanji(screen);
#endif	KANJI
		XFillRectangle(screen->display, TextWindow(screen),
#ifdef	STATUSLINE	/* michael */
		screen->instatus ? screen->normalGC :
#endif	STATUSLINE
		  screen->reverseGC,
		 CursorX(screen, screen->cur_col),
		 CursorY(screen, screen->cur_row),
		 Width(screen) - screen->cur_col * FontWidth(screen),
		 FontHeight(screen));
	    }
	}
	bzero(screen->buf [2 * screen->cur_row] + screen->cur_col,
	       (screen->max_col - screen->cur_col + 1));
	bzero(screen->buf [2 * screen->cur_row + 1] + screen->cur_col,
	       (screen->max_col - screen->cur_col + 1));
}

/*
 * Clear first part of cursor's line, inclusive.
 */
ClearLeft (screen)
register TScreen *screen;
{
	if(screen->cursor_state)
		HideCursor();
	screen->do_wrap = 0;
#ifdef	STATUSLINE	/* michael */
	if(screen->cur_row - screen->topline <= screen->max_row ||
	   screen->instatus) {
#else	STATUSLINE
	if(screen->cur_row - screen->topline <= screen->max_row) {
#endif	STATUSLINE
	    if(!AddToRefresh(screen)) {
		if(screen->scroll_amt)
			FlushScroll(screen);
#ifdef	KANJI	/* kagotani */
		BreakKanji(screen);
#endif	KANJI
		XFillRectangle (screen->display, TextWindow(screen),
#ifdef	STATUSLINE	/* michael */
		     screen->instatus ? screen->normalGC :
#endif	STATUSLINE
		     screen->reverseGC,
		     screen->border + screen->scrollbar,
		      CursorY (screen, screen->cur_row),
		     (screen->cur_col + 1) * FontWidth(screen),
		     FontHeight(screen));
	    }
	}
	bzero (screen->buf [2 * screen->cur_row], (screen->cur_col + 1));
	bzero (screen->buf [2 * screen->cur_row + 1], (screen->cur_col + 1));
}

/* 
 * Erase the cursor's line.
 */
ClearLine(screen)
register TScreen *screen;
{
	if(screen->cursor_state)
		HideCursor();
	screen->do_wrap = 0;
#ifdef	STATUSLINE	/* michael */
	if(screen->cur_row - screen->topline <= screen->max_row ||
	   screen->instatus) {
#else	STATUSLINE
	if(screen->cur_row - screen->topline <= screen->max_row) {
#endif	STATUSLINE
	    if(!AddToRefresh(screen)) {
		if(screen->scroll_amt)
			FlushScroll(screen);
		XFillRectangle (screen->display, TextWindow(screen), 
#ifdef	STATUSLINE	/* michael & kato */
		     screen->instatus && screen->reversestatus ?
		     screen->normalGC :
#endif	STATUSLINE
		     screen->reverseGC,
		     screen->border + screen->scrollbar,
		      CursorY (screen, screen->cur_row),
		     Width(screen), FontHeight(screen));
	    }
	}
	bzero (screen->buf [2 * screen->cur_row], (screen->max_col + 1));
	bzero (screen->buf [2 * screen->cur_row + 1], (screen->max_col + 1));
}

ClearScreen(screen)
register TScreen *screen;
{
	register int top;

	if(screen->cursor_state)
		HideCursor();
	screen->do_wrap = 0;
	if((top = -screen->topline) <= screen->max_row) {
		if(screen->scroll_amt)
			FlushScroll(screen);
#ifdef	STATUSLINE	/* michael */
		if(top == 0 && !screen->statusline)
#else	STATUSLINE
		if(top == 0)
#endif	STATUSLINE
			XClearWindow(screen->display, TextWindow(screen));
		else
			XClearArea(screen->display, TextWindow(screen),
			 screen->border + screen->scrollbar, 
			 top * FontHeight(screen) + screen->border,	
		 	 Width(screen), (screen->max_row - top + 1) *
			 FontHeight(screen), FALSE);
	}
	ClearBufRows (screen, 0, screen->max_row);
}

CopyWait(screen)
register TScreen *screen;
{
	XEvent reply;
	XEvent *rep = &reply;

	while (1) {
		XWindowEvent (screen->display, VWindow(screen), 
		  ExposureMask, &reply);
		switch (reply.type) {
		case Expose:
			HandleExposure (screen, (XExposeEvent *) &reply);
			break;
		case NoExpose:
		case GraphicsExpose:
			if (screen->incopy <= 0) {
				screen->incopy = 1;
				if (screen->scrolls > 0)
					screen->scrolls--;
			}
			if (reply.type == GraphicsExpose)
				HandleExposure (screen, (XExposeEvent *) &reply);

			if ((reply.type == NoExpose) ||
			    ((XExposeEvent *)rep)->count == 0) {
			    if (screen->incopy <= 0 && screen->scrolls > 0)
				screen->scrolls--;
			    if (screen->scrolls == 0) {
				screen->incopy = 0;
				return;
			    }
			    screen->incopy = -1;
			}
			break;
		}
	}
}
/*
 * This routine handles exposure events
 */
HandleExposure (screen, reply)
register TScreen *screen;
register XExposeEvent *reply;
{
	register int toprow, leftcol, nrows, ncols;

	if((toprow = (reply->y - screen->border) /
	 FontHeight(screen)) < 0)
		toprow = 0;
	if((leftcol = (reply->x - screen->border - screen->scrollbar)
	 / FontWidth(screen)) < 0)
		leftcol = 0;
	nrows = (reply->y + reply->height - 1 - screen->border) / 
		FontHeight(screen) - toprow + 1;
	ncols =
	 (reply->x + reply->width - 1 - screen->border - screen->scrollbar) /
			FontWidth(screen) - leftcol + 1;
	toprow -= screen->scrolls;
	if (toprow < 0) {
		nrows += toprow;
		toprow = 0;
	}
	if (toprow + nrows - 1 > screen->max_row)
#ifdef	STATUSLINE	/* michael */
		nrows = screen->max_row - toprow + 1 + screen->statusline;
#else	STATUSLINE
		nrows = screen->max_row - toprow + 1;
#endif	STATUSLINE
	if (leftcol + ncols - 1 > screen->max_col)
		ncols = screen->max_col - leftcol + 1;

	if (nrows > 0 && ncols > 0) {
		ScrnRefresh (screen, toprow, leftcol, nrows, ncols, False);
		if (screen->cur_row >= toprow &&
		    screen->cur_row < toprow + nrows &&
		    screen->cur_col >= leftcol &&
		    screen->cur_col < leftcol + ncols)
			return (1);
	}
	return (0);
}

ReverseVideo (term)
	XtermWidget term;
{
	register TScreen *screen = &term->screen;
	register GC tmpGC;
	register int tmp;
	register Window tek = TWindow(screen);

	tmp = term->core.background_pixel;
	if(screen->cursorcolor == screen->foreground)
		screen->cursorcolor = tmp;
	if(screen->mousecolor == screen->foreground)
		screen->mousecolor = tmp;
	term->core.background_pixel = screen->foreground;
	screen->foreground = tmp;

	tmpGC = screen->normalGC;
	screen->normalGC = screen->reverseGC;
	screen->reverseGC = tmpGC;

	tmpGC = screen->normalboldGC;
	screen->normalboldGC = screen->reverseboldGC;
	screen->reverseboldGC = tmpGC;
/*
 * Bug fix by michael
 * 3 non null lines are inserted.
 */
	tmpGC = screen->cursorGC;
	screen->cursorGC = screen->reversecursorGC;
	screen->reversecursorGC = tmpGC;

#ifdef	KANJI	/* ariza & michael */
	tmpGC = screen->w_normalGC;
	screen->w_normalGC = screen->w_reverseGC;
	screen->w_reverseGC = tmpGC;

	tmpGC = screen->w_normalboldGC;
	screen->w_normalboldGC = screen->w_reverseboldGC;
	screen->w_reverseboldGC = tmpGC;

	tmpGC = screen->w_cursorGC;
	screen->w_cursorGC = screen->w_reversecursorGC;
	screen->w_reversecursorGC = tmpGC;
#ifdef	SX9100
	tmpGC = screen->r_normalGC;
	screen->r_normalGC = screen->r_reverseGC;
	screen->r_reverseGC = tmpGC;

	tmpGC = screen->r_normalboldGC;
	screen->r_normalboldGC = screen->r_reverseboldGC;
	screen->r_reverseboldGC = tmpGC;

	tmpGC = screen->r_cursorGC;
	screen->r_cursorGC = screen->r_reversecursorGC;
	screen->r_reversecursorGC = tmpGC;
#endif	/* SX9100 */
#endif	KANJI

	{
	    unsigned long fg, bg;
	    bg = term->core.background_pixel;
	    if (screen->mousecolor == term->core.background_pixel) {
		fg = screen->foreground;
	    } else {
		fg = screen->mousecolor;
	    }
	    
	    recolor_cursor (screen->pointer_cursor, fg, bg);
	    recolor_cursor (screen->arrow, fg, bg);

	}
	term->misc.re_verse = !term->misc.re_verse;

	XDefineCursor(screen->display, TextWindow(screen), screen->pointer_cursor);
	if(tek)
		XDefineCursor(screen->display, tek, screen->arrow);
#ifdef MODEMENU
	MenuNewCursor(screen->arrow);
	ReverseVideoAllMenus ();
#endif	/* MODEMENU */

	
	if (term) {
	    if (term->core.border_pixel == term->core.background_pixel) {
		term->core.border_pixel = screen->foreground;
		term->core.parent->core.border_pixel = screen->foreground;
		if (term->core.parent->core.window)
		  XSetWindowBorder (screen->display,
				    term->core.parent->core.window,
				    term->core.border_pixel);
	    }
	}

	if(screen->scrollWidget)
		ScrollBarReverseVideo(screen->scrollWidget);

	XSetWindowBackground(screen->display, TextWindow(screen), term->core.background_pixel);
#ifdef	XJP
	if (term->core.parent->core.window && screen->kmode != XJP_ROOT)
		XSetWindowBackground(screen->display,
				     term->core.parent->core.window,
				     term->core.background_pixel);
#endif	/* XJP */
	if(tek) {
	    TekReverseVideo(screen);
	}
	XClearWindow(screen->display, TextWindow(screen));
#ifdef	STATUSLINE	/* michael */
	ScrnRefresh (screen, 0, 0, screen->max_row + 1 + screen->statusline,
#else	STATUSLINE
	ScrnRefresh (screen, 0, 0, screen->max_row + 1,
#endif	STATUSLINE
	 screen->max_col + 1, False);
	if(screen->Tshow) {
	    XClearWindow(screen->display, tek);
	    TekExpose((XExposeEvent *) NULL);
	}
#ifdef	XJP
	if (screen->kconvert && screen->kmode != XJP_ROOT) {
		jp_att.fg = screen->w_normalGC->values.foreground;
		jp_att.bg = screen->w_reverseGC->values.foreground;
		XJp_change(screen->jpreq_id,XJP_FG|XJP_BG,&jp_att);
	}
#endif	/* XJP */
}


recolor_cursor (cursor, fg, bg)
    Cursor cursor;			/* X cursor ID to set */
    unsigned long fg, bg;		/* pixel indexes to look up */
{
    register TScreen *screen = &term->screen;
    register Display *dpy = screen->display;
    XColor colordefs[2];		/* 0 is foreground, 1 is background */

    colordefs[0].pixel = fg;
    colordefs[1].pixel = bg;
    XQueryColors (dpy, DefaultColormap (dpy, DefaultScreen (dpy)),
		  colordefs, 2);
    XRecolorCursor (dpy, cursor, colordefs, colordefs+1);
    return;
}

#ifdef	KANJI	/* kagotani */
/*
 * If the cursor points KCS2 character, replace this Kanji into two blanks.
 */
BreakKanji(screen)
register TScreen *screen;
{
	register char *col;
	register char *att;
	if (screen->cur_col >= 1 && screen->cur_col <= screen->max_col
	 && *(att = screen->buf[2*screen->cur_row+1]+screen->cur_col) & KCS2) {
		col = screen->buf[2*screen->cur_row] + screen->cur_col;
		XFillRectangle(screen->display, TextWindow(screen),
#ifdef	STATUSLINE
		screen->instatus ? screen->normalGC :
#endif	STATUSLINE
		  screen->reverseGC,
		 CursorX(screen, screen->cur_col - 1),
		 CursorY(screen, screen->cur_row),
		 2 * FontWidth(screen), FontHeight(screen));
		bzero(col - 1, 2);
		bzero(att - 1, 2);
	}
}
#endif	KANJI
