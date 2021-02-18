/*
 * solitaire.c - A simple solitaire card game for X Windows. 
 * Written by David W. Burleigh, April 1987, for the 
 * Workstation Sales Support Training.
 * 
 */
/*
 * Converted to handle B & W displays 
 * Dave Lemke, 
 * 7/13/87, 
 * UCDavis Computer Science
 * 
 * 
 * Args: 
 * hostname:display -r	
 * -r	flag to turn off black cards reversed in B & W 
 *	- makes them simpler to read
 */
/*
 * converted to X11, 7/16/87 - 8/6/87 Dave Lemke
 */
/*
 * Final conversion work, optimizing, cleanup:
 * Dave Lemke
 * lemke@sun.com
 * Wed Jul 20 21:52:00 PDT 1988
 */

/************************************************************
Copyright 1988 by Sun Microsystems, Inc. Mountain View, CA.

		All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Sun or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific prior written permission. Sun and M.I.T. 
make no representations about the suitability of this software for 
any purpose. It is provided "as is" without any express or implied warranty.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE. IN NO EVENT SHALL SUN BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE 
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.
************************************************************/

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/X10.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <strings.h>
#include "cards.bm"		/* Bitmaps for playing cards */
#include "draw.bm"		/* Bitmap for back of card deck */
#include "heart.bm"		/* Bitmaps for suits */
#include "club.bm"
#include "diamond.bm"
#include "spade.bm"
#include "cursor.bm"		/* Bitmap for cursor */
#include "restart.bm"		/* Bitmap for Restart button */
#include "exit.bm"		/* Bitmap for Exit button */

#include "gray3.bitmap"	/* bitmap for B & W background */

#define border_width 3
#define icon_width 30
#define icon_height 30
#define defwidth 360		/* Default dimensions for window */
#define defheight 500
#define minwidth 360		/* Minimum dimensions for window */
#define minheight 500
#define offset 25		/* y offset for stacking cards */
#define hearts 0
#define diamonds 1
#define clubs 2
#define spades 3
#define draw 52			/* Array index for draw window */
#define Ace 1
#define King 13

struct Card {
	Window          self;	/* window id */
	short           suit;	/* hearts,diamonds,clubs,spades */
	short           value;	/* Ace,2,3,4,5,6,7,8,9,10,Jack,Queen,King */
	short           x, y;	/* Origin coordinates */
	short           stack;	/* Indicates which stack the card is in */
	struct Card    *prev;	/* Points to previous stacked card */
	struct Card    *next;	/* Points to next stacked card */
	XImage          image;	/* windows image */
}               deck[52];
struct Card     card_window;

struct {			/* Locations for stacking card windows */
	short           x, y;
	short           count;
}               stack[12] = {
	59, 9, 52, 
	9, 79, 0, 
	59, 79, 0, 
	109, 79, 0,
	159, 79, 0, 
	209, 79, 0, 
	259, 79, 0, 
	309, 79, 0,
	129, 9, 0, 
	179, 9, 0, 
	229, 9, 0, 
	279, 9, 0
};

#define	CNULL	(struct Card *) 0

#define	TRUE	1
#define	FALSE	0

short           iscolor = False;
short           rflag = 42;

short           order[52][2];	/* Shuffling array */

XAssocTable    *Key;		/* Pointer to the association table for
				 * matching windows to data structures */

static XColor   foreground = {0L, 65535, 65535, 65535};
static XColor   bground = {0L, 0, 0, 0};

XColor          Red, Green, Black, White;
Window          Table, Restart, Exit;	/* Display window and button ids */
GC              gc, rgc, Redgc, out_gc;
XImage          res_image, exit_image;
Pixmap          background, draw_pixmap;
Pixmap		icon_pixmap;
Atom		protocol_atom, kill_atom;

Bool	have_card = False;
Bool	no_drag = False;

#define	Usage	{fprintf(stderr, "xsol: [-d display] [-r] [-nodrag]\n"); exit(0);}

Display        *dpy;
int             screen;

main(argc, argv)
	int             argc;	/* Number of arguments  */
	char           *argv[];	/* Array of pointers to arguments          */
{
	register int    i;
	Pixmap          cursor_bitmap;
	struct Card    *from = (struct Card *) 0,
		       *to = (struct Card *) 0;	/* User-selected cards */
	char           *display = NULL;
	XImage          timage;
	char           *progname;
	XSizeHints	hints;

	progname = argv[0];
	for (i = 1; i < argc; i++) {
		if (index(argv[i], ':'))
			display = argv[i];
		else if (strncmp(argv[i], "-d", 2) == 0)	{
			if (argv[++i])
				display = argv[i];
			else Usage;
		}
		else if (strncmp(argv[i], "-r", 2) == 0)
			rflag = 1;
		else if (strncmp(argv[i], "+r", 2) == 0)
			rflag = 0;
		else if (strncmp(argv[i], "-n", 2) == 0)
			no_drag = True;
		else
			Usage;
	}

	if (!(dpy = XOpenDisplay(display))) {
		fprintf(stderr, "%s: Can't open display.\n", progname);
		exit(1);
	}
	screen = DefaultScreen(dpy);

	gc = XCreateGC(dpy, RootWindow(dpy, screen), 0, NULL);
	rgc = XCreateGC(dpy, RootWindow(dpy, screen), 0, NULL);
	Redgc = XCreateGC(dpy, RootWindow(dpy, screen), 0, NULL);
	out_gc = XCreateGC(dpy, RootWindow(dpy, screen), 0, NULL);

	XSetForeground(dpy, gc, BlackPixel(dpy, screen));
	XSetForeground(dpy, rgc, WhitePixel(dpy, screen));

	XSetBackground(dpy, gc, WhitePixel(dpy, screen));
	XSetBackground(dpy, rgc, BlackPixel(dpy, screen));

	XSetForeground(dpy, out_gc, BlackPixel(dpy, screen));
	XSetFunction(dpy, out_gc, GXxor);
	XSetSubwindowMode(dpy, out_gc, IncludeInferiors);

	if (DisplayCells(dpy, screen) > 2)
		iscolor = TRUE;
	if (iscolor) {
		XAllocNamedColor(dpy, DefaultColormap(dpy, screen),
				 "red", &Red, &Red);
		XAllocNamedColor(dpy, DefaultColormap(dpy, screen),
				 "green", &Green, &Green);
		XAllocNamedColor(dpy, DefaultColormap(dpy, screen),
				 "black", &Black, &Black);
		XAllocNamedColor(dpy, DefaultColormap(dpy, screen),
				 "white", &White, &White);
		XSetForeground(dpy, Redgc, Red.pixel);
		XSetBackground(dpy, Redgc, White.pixel);
		if (rflag == 42)
			rflag = 0;
	}
	if (!iscolor && rflag == 42)
		rflag = 1;
		
	/* Fill in parent window parameters */

	if (!iscolor) {
		background = MakePixmap(dpy, screen, RootWindow(dpy, screen),
			    gray3_bits, gray3_width, gray3_height, &timage);
	}
	/* Store pixmaps for tiling areas */

	draw_pixmap = MakePixmap(dpy, screen, RootWindow(dpy, screen),
			       draw_bits, draw_width, draw_height, &timage);

	icon_pixmap = XCreatePixmap(dpy, RootWindow(dpy, screen),
		32, 32, DefaultDepth(dpy, screen));

	{
	XImage	temp;
	GC	redgc = (iscolor) ? Redgc : gc;
	GC	icon_gc, redicon_gc;

	icon_gc = XCreateGC(dpy, icon_pixmap, 0, NULL);
	redicon_gc = XCreateGC(dpy, icon_pixmap, 0, NULL);
	if (iscolor)	{
	    XSetForeground(dpy, icon_gc, Green.pixel);
	    XFillRectangle(dpy, icon_pixmap, icon_gc, 0, 0, 32, 32);
	    XSetForeground(dpy, redicon_gc, Red.pixel);
	    XSetBackground(dpy, redicon_gc, Green.pixel);
	    XSetForeground(dpy, icon_gc, BlackPixel(dpy, screen));
	    XSetBackground(dpy, icon_gc, Green.pixel);
	} else	{
	    XSetForeground(dpy, icon_gc, WhitePixel(dpy, screen));
	    XSetBackground(dpy, icon_gc, WhitePixel(dpy, screen));
	    XSetBackground(dpy, redicon_gc, WhitePixel(dpy, screen));
	    XFillRectangle(dpy, icon_pixmap, icon_gc, 0, 0, 32, 32);
	    XSetForeground(dpy, icon_gc, BlackPixel(dpy, screen));
	    XSetForeground(dpy, redicon_gc, BlackPixel(dpy, screen));
	}

	MakeImage(spade_width, spade_height, spade_bits, &temp);
	XPutImage(dpy, icon_pixmap, icon_gc, &temp, 0, 0, 2, 2, 
		spade_width, spade_height);
	MakeImage(heart_width, heart_height, heart_bits, &temp);
	XPutImage(dpy, icon_pixmap, redicon_gc, &temp, 0, 0, 17, 2, 
		heart_width, heart_height);
	MakeImage(club_width, club_height, club_bits, &temp);
	XPutImage(dpy, icon_pixmap, icon_gc, &temp, 0, 0, 17, 17, 
		club_width, club_height);
	MakeImage(diamond_width, diamond_height, diamond_bits, &temp);
	XPutImage(dpy, icon_pixmap, redicon_gc, &temp, 0, 0, 2, 17, 
		diamond_width, diamond_height);
	XFreeGC(dpy, icon_gc);
	XFreeGC(dpy, redicon_gc);
	}

	/* Try to create the display window and icon window */

	Table = XCreateSimpleWindow(dpy, RootWindow(dpy, screen), 100, 100,
		 defwidth, defheight, border_width, BlackPixel(dpy, screen),
				    BlackPixel(dpy, screen));
	if (iscolor)
		XSetWindowBackground(dpy, Table, Green.pixel);
	else
		XSetWindowBackgroundPixmap(dpy, Table, background);

	protocol_atom = XInternAtom(dpy, "WM_PROTOCOLS", False);
	kill_atom = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(dpy, Table, &kill_atom, 1);

	if (!Table) {
		fprintf(stderr, "%s: can't create display window.\n", progname);
		exit(1);
	}
	
	hints.flags = PSize | PMinSize | PMaxSize;
	/* doesn't like to be resized */
	hints.width = hints.min_width = hints.max_width = defwidth;
	hints.height = hints.min_height = hints.max_height = defheight;

	XSetStandardProperties(dpy, Table, "Xsol", "Xsol", icon_pixmap,
		argv, argc, &hints);

	/* Create windows for Restart and Exit buttons */

	Restart = XCreateSimpleWindow(dpy, Table, 0, defheight - restart_height,
		restart_width, restart_height, 1, BlackPixel(dpy, screen), 
		WhitePixel(dpy, screen));
	MakeImage(restart_width, restart_height, restart_bits, &res_image);

	Exit = XCreateSimpleWindow(dpy, Table, defwidth - exit_width,
		defheight - exit_height, exit_width, exit_height,
		1, BlackPixel(dpy, screen), WhitePixel(dpy, screen));

	MakeImage(exit_width, exit_height, exit_bits, &exit_image);

	if (!(Restart && Exit)) {
		fprintf(stderr, "%s: can't create button subwindows.\n", 
			progname);
		exit(1);
	}

	/* Associate the cursor with the display window */
	cursor_bitmap = XCreateBitmapFromData(dpy, RootWindow(dpy, screen),
		cursor_bits, cursor_width, cursor_height);

	XDefineCursor(dpy, Table, XCreatePixmapCursor(dpy, cursor_bitmap,
		cursor_bitmap, &bground, &foreground, 0, 0));

	/* Initialize subwindow structures and create card subwindows */

	if (!InitCardWindows(Table)) {
		fprintf(stderr, "%s: Can't create subwindows.\n", progname);
		exit(1);
	}
	/* Tell the X-window server which events we're interested in */

	XSelectInput(dpy, card_window.self, ButtonPressMask);
	/* table sucks buttons so they don't slide through to parent
	 * when player slips off card */
	XSelectInput(dpy, Table, ExposureMask | ButtonPressMask |
		((no_drag) ? PointerMotionMask : 0));
	XSelectInput(dpy, Restart, ExposureMask | ButtonPressMask);
	XSelectInput(dpy, Exit, ExposureMask | ButtonPressMask);

	DealCards();		/* Shuffle cards and deal to playing field */

	XMapWindow(dpy, Table);
	XMapWindow(dpy, Restart);
	XMapWindow(dpy, Exit);
	XMapWindow(dpy, card_window.self);

	while (1) {		/* Process events */
		XEvent          event;
		int	oldx, oldy;
		int	mx, my;
		int	num_cards;
		struct Card	*t;

		XNextEvent(dpy, &event);

		switch (event.type) {

		case ClientMessage:
		    {
		    XClientMessageEvent	*ev = (XClientMessageEvent *)&event;

		    if (ev->message_type == protocol_atom && 
			ev->data.l[0] == kill_atom)
			exit(0);
		    }
		    break;
		case Expose:
			DisplayCard(event.xexpose.window);
			break;
		case ButtonPress:
		    if (!have_card)	{
			SelectCard(event.xbutton.window, &from);
			if (from)	{
				/* count cards */
				for (num_cards = 0, t = from; t->next; 
				    t = t->next, num_cards++)
					;
				oldx = oldy = -1;
				mx = event.xbutton.x;
				my = event.xbutton.y;
				}
			}
		    else	{
			if (from != CNULL)	/* only w/ no_drag */
				/* erase old outline */
				draw_outline(oldx, oldy, num_cards);
				have_card = False;
				MoveCard(event.xbutton.window, 
					&from, &to);
				}
			break;
		case MotionNotify:
			if (have_card)	{
				draw_outline(oldx, oldy, num_cards);
				oldx = ((XMotionEvent *) &(event))->x_root - mx;
				oldy = ((XMotionEvent *) &(event))->y_root - my;
				draw_outline(oldx, oldy, num_cards);
				}
			break;
		case ButtonRelease:	/* only w/o nodrag */
			if (from != CNULL)	{
				/* erase old outline */
				draw_outline(oldx, oldy, num_cards);
				have_card = False;
				MoveCard(event.xbutton.window, &from, &to);
				}
			break;
		}
	}
}

InitCardWindows(Table)		/* Fill in subwindow structures */
	Window          Table;

{
	register int    i;
	XAssocTable    *XCreateAssocTable();
	XSetWindowAttributes watt;
	Pixmap	pmap;

	/* Initialize OpaqueFrame structures and shuffling array */

	watt.backing_store = Always;
	for (i = 0; i < 52; i++) {
		deck[i].x = stack[0].x;
		deck[i].y = stack[0].y;
		deck[i].stack = 0;
		deck[i].next = deck[i].prev = CNULL;
		order[i][0] = i;
		deck[i].self = XCreateSimpleWindow(dpy, Table, 
			deck[i].x, deck[i].y, card_width, card_height, 
			0, (Pixmap) 0, WhitePixel(dpy, screen));
		XSelectInput(dpy, deck[i].self,
			ExposureMask | ButtonPressMask |
			((no_drag) ? 0 :
			(ButtonReleaseMask | OwnerGrabButtonMask 
			| Button1MotionMask)));
		XChangeWindowAttributes(dpy, deck[i].self, 
			CWBackingStore, &watt);
		MakeImage(card_width, card_height, card_bits[i],
			&(deck[i].image));
	}
	card_window.x = card_window.y = 9;
	card_window.self = XCreateSimpleWindow(dpy, Table, 
		card_window.x, card_window.y, 
		(card_width - 4), (card_height - 4), 
		2, BlackPixel(dpy, 0), (Pixmap) 0);
	XSetWindowBackgroundPixmap(dpy, card_window.self, draw_pixmap);

	/* Make a table for associating window ids to Card structures */

	if (!(Key = XCreateAssocTable(16)))	/* 16-bucket hash table */
		return (0);

	for (i = 0; i < 52; i++) {

		/* Associate address of card with card's window id */
		XMakeAssoc(dpy, Key, deck[i].self, &deck[i]);
	}

	/* Initialize the cards to their suits and values */

	for (i = 0; i < 13; i++) {
		deck[i].suit = hearts;
		deck[i].value = i + 1;
	}
	for (; i < 26; i++) {
		deck[i].suit = diamonds;
		deck[i].value = i - 12;
	}
	for (; i < 39; i++) {
		deck[i].suit = clubs;
		deck[i].value = i - 25;
	}
	for (; i < 52; i++) {
		deck[i].suit = spades;
		deck[i].value = i - 38;
	}

	srandom((int) time(0));	/* Seed the random number generator with the
				 * system time */

	return (1);
}

DealCards()
{
	register int    i, j, k;
	int             compare();

	for (i = 0; i < 52; i++)
		order[i][1] = random();	/* Fill the shuffling array with
					 * random numbers */

	qsort((char *) order, 52, 4, compare);	/* Shuffle by sorting the
						 * random numbers */

	/* Deal the cards to the field stacks */

	for (i = 1; i < 8; i++) {
		for (j = i; j < 8; j++) {
			/* Get index of next card in deck */
			k = order[--stack[0].count][0];	
			/* Move it to its new stack */
			XMoveWindow(dpy, deck[k].self,
				stack[j].x, stack[j].y);
			if (j == i) /* Make it visible if its on the top */
				XMapRaised(dpy, deck[k].self);	
			deck[k].x = stack[j].x;	/* Update locator fields */
			deck[k].y = stack[j].y;
			deck[k].stack = j;	/* Update stack number */
			++stack[j].count;	/* Update card count in stack */
		}
	}

	/* map later, so we don't give any hints about what's in each stack */
	for (i = 0; i < 52; i++)	{
		/* don't map cards in draw deck */
		if (deck[i].y != stack[0].y)
			XMapWindow(dpy, deck[i].self);
		}
	/* map first draw */
	XMapRaised(dpy, deck[order[--stack[0].count][0]].self);
}

compare(arg1, arg2)		/* Compare low-order word only, for use in
				 * qsort */
	long           *arg1, *arg2;

{
#if defined(vax) || defined(i386)
	return ((*arg2 & 0xffff0000) - (*arg1 & 0xffff0000));
#else
	return ((*arg2 & 0xffff) - (*arg1 & 0xffff));
#endif
}

DisplayCard(window)		/* Service ExposeWindow events */
	Window          window;

{
	if (window == Restart)
		XPutImage(dpy, Restart, gc, &res_image, 0, 0, 0, 0,
			  restart_width, restart_height);
	else if (window == Exit)
		XPutImage(dpy, Exit, gc, &exit_image, 0, 0, 0, 0, 
			exit_width, exit_height);
	else if (window == Table)
		;		/* noop (for now) */
	else {
		struct Card    *c;

		/* Find the card structure that corresponds to the window */

		c = (struct Card *) XLookUpAssoc(dpy, Key, window);

		/* Fill the card window with its bitmap */

		if (iscolor)
		    if (rflag)
			XPutImage(dpy, window, (c->suit < 2) ? Redgc : rgc, 
				&(c->image), 0, 0, 0, 0, 
				card_width, card_height);
		    else
			XPutImage(dpy, window, (c->suit < 2) ? Redgc : gc, 
				&(c->image), 0, 0, 0, 0, 
				card_width, card_height);
		else if (rflag)
			XPutImage(dpy, window, (c->suit < 2) ? gc : rgc, 
				&(c->image), 0, 0, 0, 0, 
				card_width, card_height);
		else
			XPutImage(dpy, window, gc, &(c->image), 
				0, 0, 0, 0, card_width, card_height);
	}
	return;
}

SelectCard(window, pfrom)	/* Service ButtonPressed events */
	Window          window;
	struct Card   **pfrom;

{
	if (window == Restart)	{
		NewGame();
		}
	else if (window == Table)
		*pfrom = (struct Card *) 0;
	else if (window == Exit)
		exit(0);
	else if (window == card_window.self) {	/* Draw next card, if any */
		if (stack[0].count)	{
			XMapRaised(dpy, deck[order[--stack[0].count][0]].self);
			if (stack[0].count == 0)	{
				/* pull draw stack card */
				XUnmapWindow(dpy, card_window.self);
				}
			}
		else
			/* should never happen */
			XBell(dpy, 50);
	} else {		/* Select an exposed card */
		register int    i;
		struct Card    *from, *to;

		from = *pfrom = (struct Card *) XLookUpAssoc(dpy, Key, window);

		if (from->stack > 7) {	/* Can't move a card from the home
					 * stacks */
			XBell(dpy, 50);
			*pfrom = NULL;
			return;
		} else {	/* Aces and Kings are moved automatically
				 * upon selection; others are moved at
				 * ButtonReleased event */

			if (from->value == Ace) {/* Move Ace to home area */
				/* Skip non-vacant stacks */
				for (i = 8; stack[i].count; i++)
					;

				/*
				 * Reset card location fields and stack
				 * counters
				 */

				if (from->stack)
					--stack[from->stack].count;
				++stack[i].count;
				from->stack = i;
				from->x = stack[i].x;
				from->y = stack[i].y;

				/*
				 * Move the card window, and signal that
				 * we've done it
				 */

				XRaiseWindow(dpy, window);
				XMoveWindow(dpy, window, from->x, from->y);
				*pfrom = NULL;
				return;
			} else if (from->value == King) {	
			/* Move King to the first vacant column */

				if (from->stack) {
					if (stack[from->stack].count == 1)
						return;
				}
				/* Look for a vacancy */
				for (i = 1; ((i < 8) && stack[i].count); i++)
					;
				if (i < 8) {

					/*
					 * Reset card location fields and
					 * stack counters
					 */

					if (from->stack)
						--stack[from->stack].count;
					++stack[i].count;
					from->stack = i;
					from->x = stack[i].x;
					from->y = stack[i].y;

					/*
					 * Move the card window, and any
					 * cards stacked upon it
					 */

					XRaiseWindow(dpy, window);
					XMoveWindow(dpy, window, 
						from->x, from->y);

					if (from->next != CNULL) {
						to = from;
						from = from->next;
						Restack(&from, &to);
					}
					*pfrom = NULL;
					return;
				}
			}
		else
			have_card = True;
		}
	}
}

MoveCard(window, pfrom, pto)
	Window          window;
	struct Card   **pfrom, **pto;

{
	struct Card    *from, *to;	/* Locals to make syntax less funky */

	from = *pfrom;
	to = *pto = (struct Card *) XLookUpAssoc(dpy, Key, window);

	if (to == NULL)
		return;

	if (to->next == CNULL) {/* Can't move here unless it's top card */
		if (to->stack > 7) {	/* Try to move card to home area */
			if ((from->next == CNULL) && (to->suit == from->suit)
				&& (to->value == (from->value - 1))) {

				XRaiseWindow(dpy, from->self);
				XMoveWindow(dpy, from->self, to->x, to->y);
				/* Fix underlying card's forward pointer */
				if (from->prev != CNULL)
					(from->prev)->next = CNULL;	
				from->prev = to;	/* Fix back pointer */
				from->x = to->x;
				from->y = to->y;
				if (from->stack)
					--stack[from->stack].count;
				++stack[to->stack].count;
				from->stack = to->stack;

				return;
			}
		} else if (to->stack) {	/* Try to restack the card */
			if ((to->value == (from->value + 1)) &&
			    (((to->suit < 2) && (from->suit > 1)) ||
			     ((to->suit > 1) && (from->suit < 2)))) {

				to->next = from;
				/* Fix underlying card's forward pointer */
				if (from->prev != CNULL)
					(from->prev)->next = CNULL;	
				from->prev = to;	/* Fix back pointer */
				Restack(&from, &to);	/* Move this card and
							 * any stacked on top of
							 * it */
				return;
			}
		}
	}
	XBell(dpy, 50);		/* Illegal move */
	return;
}

Restack(pfrom, pto)		/* Recursive function to restack cards */
	struct Card   **pfrom, **pto;

{
	struct Card    *from, *to;

	from = *pfrom;
	to = *pto;
	from->x = to->x;
	from->y = to->y + offset;
	XMoveWindow(dpy, from->self, from->x, from->y);
	XRaiseWindow(dpy, from->self);
	if (from->stack)
		--stack[from->stack].count;
	++stack[to->stack].count;
	from->stack = to->stack;
	if (from->next != CNULL) {
		to = from;
		from = from->next;
		Restack(&from, &to);
	}
	return;
}

NewGame()
{				/* Re-initialize the card deck, shuffle and
				 * deal */
	register int    i;

	XUnmapSubwindows(dpy, Table);
	XMapRaised(dpy, card_window.self);
	XMapRaised(dpy, Restart);
	XMapRaised(dpy, Exit);

	/* Relocate all cards to the draw stack */

	for (i = 0; i < 52; i++) {
		deck[i].x = stack[0].x;
		deck[i].y = stack[0].y;
		deck[i].stack = 0;
		deck[i].prev = deck[i].next = CNULL;
		XMoveWindow(dpy, deck[i].self, stack[0].x, stack[0].y);
	}

	stack[0].count = 52;
	for (i = 1; i < 12; i++)
		stack[i].count = 0;

	DealCards();

	return;
}

draw_outline(x, y, n)
int	x, y, n;
{
	if ((x == -1) && (y == -1))
		return;
	XDrawRectangle(dpy, RootWindow(dpy, screen), out_gc, x, y, 
		card_width, card_height + n * 25);
}
