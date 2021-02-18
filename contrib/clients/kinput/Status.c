/* $Id: Status.c,v 3.2 89/10/27 17:00:02 ishisone Rel $ */
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
 */

/*
 * Selection
 */

static wchar_t	selectionprompt[4] = { 0xc1aa, 0xc2f2, '>', 0 }; /* 選択> */

#if __STDC__ == 1
static void newSelection(KanjiInputWidget, int);
static void inverseSelection(KanjiInputWidget, int);
static void clearSelection(KanjiInputWidget);
static void newSymbol(KanjiInputWidget, int);
static wchar_t nextSymbol(wchar_t, int);
#else
static void newSelection();
static void inverseSelection();
static void clearSelection();
static void newSymbol();
static wchar_t nextSymbol();
#endif

static void
beginSelection(w, small)
KanjiInputWidget w;
int small;
{
	int			ncand, curcand;

	if (w->kinput.state != 0) {
		beep();
		return;
	}
	if (jcCandidateInfo(JCBUF, small, &ncand, &curcand) < 0) {
		beep();
		return;
	}
	
	w->kinput.ncand = ncand;
	w->kinput.curcand = curcand;
	w->kinput.state = small ? MODE_SELECTION_S : MODE_SELECTION_L;
	newSelection(w, INC);
}

static void
moveSelection(w, dir)
KanjiInputWidget w;
int	dir;	/* SEL_LEFT/SEL_RIGHT */
{
	KanjiInputPart		*kpart = &(w->kinput);

	switch (dir) {
	case SEL_NEXT:
		kpart->curcand = kpart->selectionmax + 1;
		if (kpart->curcand >= kpart->ncand)
			kpart->curcand = 0;
		newSelection(w, INC);
		return;
	case SEL_PREV:
		kpart->curcand = kpart->selectionmin - 1;
		if (kpart->curcand < 0)
			kpart->curcand = kpart->ncand - 1;
		newSelection(w, DEC);
		return;
	case SEL_TOP:
		if (kpart->curcand == kpart->selectionmin)
			return;
		inverseSelection(w, 0);
		inverseSelection(w, kpart->curcand - kpart->selectionmin);
		kpart->curcand = kpart->selectionmin;
		break;
	case SEL_BOTTOM:
		if (kpart->curcand == kpart->selectionmax)
			return;
		inverseSelection(w, kpart->curcand - kpart->selectionmin);
		inverseSelection(w, kpart->selectionmax - kpart->selectionmin);
		kpart->curcand = kpart->selectionmax;
		break;
	case SEL_LEFT:
		if (kpart->curcand == 0) {
			kpart->curcand = kpart->ncand - 1;
			newSelection(w, DEC);
			return;
		}
		kpart->curcand--;
		if (kpart->curcand < kpart->selectionmin) {
			newSelection(w, DEC);
			return;
		}
		inverseSelection(w, kpart->curcand - kpart->selectionmin);
		inverseSelection(w, kpart->curcand - kpart->selectionmin + 1);
		break;
	case SEL_RIGHT:
		if (kpart->curcand == kpart->ncand - 1) {
			kpart->curcand = 0;
			newSelection(w, INC);
			return;
		}
		kpart->curcand++;
		if (kpart->curcand > kpart->selectionmax) {
			newSelection(w, INC);
			return;
		}
		inverseSelection(w, kpart->curcand - kpart->selectionmin);
		inverseSelection(w, kpart->curcand - kpart->selectionmin - 1);
		break;
	}
}

static void
endSelection(w, n)
KanjiInputWidget w;
int	n;
{
	KanjiInputPart		*kpart = &(w->kinput);

	if (n < 0 || n > kpart->dcand) {
		beep();
		return;
	}

	if (n == 0)
		n = kpart->curcand;
	else
		n += kpart->selectionmin - 1;

	(void)jcSelect(JCBUF, n);
	clearSelection(w);
	kpart->state = 0;
}

static void
RedrawCandidates(w)
KanjiInputWidget w;
{
	KanjiInputPart		*kpart = &(w->kinput);

	if (kpart->state != MODE_SELECTION_S ||
	    kpart->state != MODE_SELECTION_L)
		return;

	newSelection(w, RESIZE);
}

static void
newSelection(w, dir)
KanjiInputWidget	w;
int	dir;	/* INC / DEC / RESIZE */
{
	KanjiInputPart		*kpart = &(w->kinput);
	int	candnum;
	int	dcand;
	int	inv;
	int	i;
	int	len;
	int	width;
	int	dispwidth;
	int	buflen = SELBUFSIZE;
	int	x;
	wchar_t	*bp = kpart->selstring;
	wchar_t	*csp;
	wchar_t	candstr[64];

	/* ステータスラインをクリア */
	clearSelection(w);

	/* '選択>' */
	width = KIDrawString(XtDisplay(w), XtWindow(kpart->statuswid),
			     kpart->textgc,
			     0, kpart->ascent, selectionprompt, 3, 1) + 8;

	candnum = kpart->curcand;
	if (dir == INC)
		kpart->selectionmin = kpart->curcand;
	else if (dir == DEC)
		kpart->selectionmax = kpart->curcand;
	else	/* dir == RESIZE */
		candnum = kpart->selectionmin;

	dcand = 0;
	x = width;
	dispwidth = StatusWidth(w) - x;
	for (i = 0; i < MAXSELECTION; i++) {
		if (candnum < 0 || candnum >= kpart->ncand)
			break;
		jcGetCandidate(JCBUF, candnum, candstr);
		len = wstrlen(candstr) + 1;	/* add null character */
		if (len > buflen)
			break;
		width = KITextWidth(kpart->textgc, candstr, len - 1, 1) + 8;
		/* 長さが長くても最初の一個は表示する */
		if (i > 0 && width > dispwidth)
			break;

		buflen -= len;
		dispwidth -= width;

		/* 候補をコピー */
		kpart->selrec[i].candidate = bp;
		csp = candstr;
		while (*bp++ = *csp++)
			;
		dcand++;
		if (dir == DEC)
			candnum--;
		else	/* dir == INC || dir == RESIZE */
			candnum++;
	}

	kpart->dcand = dcand;
	if (dir == INC) {
		kpart->selectionmax = kpart->selectionmin + dcand - 1;
		inv = 0;
	} else if (dir == DEC) {
		kpart->selectionmin = kpart->selectionmax - dcand + 1;
		inv = dcand - 1;
	} else	/* dir == RESIZE */ {
		kpart->selectionmax = kpart->selectionmin + dcand - 1;
		if (kpart->curcand > kpart->selectionmax)
			kpart->curcand = kpart->selectionmax;
		inv = kpart->curcand - kpart->selectionmin;
	}
	
	if (dir == DEC) {
		/* selectionvec[] を reverse */
		int	i, j;
		wchar_t	*tmp;

		for (i = 0, j = dcand - 1; i < j; i++, j--) {
			tmp = kpart->selrec[i].candidate;
			kpart->selrec[i].candidate = kpart->selrec[j].candidate;
			kpart->selrec[j].candidate = tmp;
		}
	}

	for (i = 0; i < dcand; i++) {
		width = KIDrawString(XtDisplay(w), XtWindow(kpart->statuswid),
				     kpart->textgc, x, kpart->ascent,
				     kpart->selrec[i].candidate,
				     wstrlen(kpart->selrec[i].candidate),
				     1);
		kpart->selrec[i].x = x;
		x += width + 8;
	}

	/* セレクトされている候補を反転させる */
	inverseSelection(w, inv);
}

static void
inverseSelection(w, n) 
KanjiInputWidget w;
int n;
{
	int	x = w->kinput.selrec[n].x;
	int	len = KITextWidth(w->kinput.textgc,
				  w->kinput.selrec[n].candidate,
				  wstrlen(w->kinput.selrec[n].candidate),
				  1);
	XFillRectangle(XtDisplay(w), XtWindow(w->kinput.statuswid), 
		       w->kinput.revgc, x, 0, len, w->kinput.fontheight);
}

static void
clearSelection(w)
KanjiInputWidget w;
{
	XClearWindow(XtDisplay(w), XtWindow(w->kinput.statuswid));
}


/*
 * Symbol
 */

#define SYMBOL_START	0xa1a1
#define SYMBOL_END	0xa8c1

static wchar_t	symbolprompt[4] = { 0xb5ad, 0xb9e6, '>', 0 }; /* 記号> */

static void
beginSymbol(w)
KanjiInputWidget w;
{
	if (w->kinput.state != 0) {
		beep();
		return;
	}
	w->kinput.state = MODE_SYMBOL;
	if (w->kinput.cursymbol == 0)
		w->kinput.cursymbol = SYMBOL_START;
	newSymbol(w, INC);
}

static void
moveSymbol(w, dir)
KanjiInputWidget w;
int	dir;	/* SEL_LEFT/SEL_RIGHT */
{
	wchar_t	inv1, inv2;
	wchar_t	c;
	int	x;
	int	width;
	KanjiInputPart		*kpart = &(w->kinput);

	switch (dir) {
	case SEL_NEXT:
		kpart->cursymbol = nextSymbol(kpart->symbolmax, INC);
		if (kpart->cursymbol > SYMBOL_END)
			kpart->cursymbol = SYMBOL_START;
		newSymbol(w, INC);
		return;
	case SEL_PREV:
		kpart->cursymbol = nextSymbol(kpart->symbolmin, DEC);
		if (kpart->cursymbol < SYMBOL_START)
			kpart->cursymbol = SYMBOL_END;
		newSymbol(w, DEC);
		return;
	case SEL_TOP:
		if (kpart->cursymbol == kpart->symbolmin)
			return;
		inv1 = kpart->symbolmin;
		inv2 = kpart->cursymbol;
		kpart->cursymbol = kpart->symbolmin;
		break;
	case SEL_BOTTOM:
		if (kpart->cursymbol == kpart->symbolmax)
			return;
		inv1 = kpart->cursymbol;
		inv2 = kpart->symbolmax;
		kpart->cursymbol = kpart->symbolmax;
		break;
	case SEL_LEFT:
		c = nextSymbol(kpart->cursymbol, DEC);
		if (c < SYMBOL_START) {
			kpart->cursymbol = SYMBOL_END;
			newSymbol(w, DEC);
			return;
		}
		inv1 = c;
		inv2 = kpart->cursymbol;
		kpart->cursymbol = c;
		if (c < kpart->symbolmin) {
			newSymbol(w, DEC);
			return;
		}
		break;
	case SEL_RIGHT:
		c = nextSymbol(kpart->cursymbol, INC);
		if (c > SYMBOL_END) {
			kpart->cursymbol = SYMBOL_START;
			newSymbol(w, INC);
			return;
		}
		inv1 = kpart->cursymbol;
		inv2 = c;
		kpart->cursymbol = c;
		if (c > kpart->symbolmax) {
			newSymbol(w, INC);
			return;
		}
		break;
	}

	x = KITextWidth(kpart->textgc, symbolprompt, 3, 1) + 8;

	for (c = kpart->symbolmin; c <= kpart->symbolmax; c = nextSymbol(c, INC)) {
		width = KITextWidth(kpart->textgc, &c, 1, 1);
		if (c == inv1 || c == inv2) {
			XFillRectangle(XtDisplay(w),
				       XtWindow(w->kinput.statuswid),
				       w->kinput.revgc,
				       x, 0, width, w->kinput.fontheight);
		}
		x += width + 8;
	}
}

static void
endSymbol(w, quit)
KanjiInputWidget w;
int	quit;
{
	KanjiInputPart		*kpart = &(w->kinput);

	kpart->state = 0;
	if (!quit) {
		ccContextAppend(kpart->ccbuf, kpart->cursymbol);
		insChar(kpart->cursymbol, (caddr_t)w);
	}
	clearSelection(w);
}

static void
RedrawSymbol(w)
KanjiInputWidget w;
{
	KanjiInputPart	*kpart = &(w->kinput);

	if (kpart->state != MODE_SYMBOL)
		return;
	newSymbol(w, RESIZE);
}

static void
newSymbol(w, dir)
KanjiInputWidget	w;
int	dir;	/* INC / DEC / RESIZE */
{
	KanjiInputPart		*kpart = &(w->kinput);
	wchar_t	c, nc;
	int	revx;
	int	revwidth;
	int	width;
	int	dispwidth;
	int	x;

	/* ステータスラインをクリア */
	clearSelection(w);

	/* '記号>' */
	width = KIDrawString(XtDisplay(w), XtWindow(kpart->statuswid),
			     kpart->textgc,
			     0, kpart->ascent, symbolprompt, 3, 1) + 8;

	c = kpart->cursymbol;
	if (dir == INC)
		kpart->symbolmin = kpart->cursymbol;
	else if (dir == DEC)
		kpart->symbolmax = kpart->cursymbol;
	else {	/* dir == RESIZE */
		c = kpart->symbolmin;
		dir = INC;
	}

	x = width;
	dispwidth = StatusWidth(w) - x;
	for (nc = c;;) {
		if (nc < SYMBOL_START || nc > SYMBOL_END)
			break;
		width = KITextWidth(kpart->textgc, &nc, 1, 1) + 8;
		if (width > dispwidth)
			break;
		c = nc;
		dispwidth -= width;

		nc = nextSymbol(nc, dir);
	}

	if (dir == INC)
		kpart->symbolmax = c;
	else
		kpart->symbolmin = c;
	
	if (kpart->cursymbol > kpart->symbolmax)
		kpart->cursymbol = kpart->symbolmax;

	for (c = kpart->symbolmin; c <= kpart->symbolmax; c = nextSymbol(c, INC)) {
		width = KIDrawString(XtDisplay(w), XtWindow(kpart->statuswid),
				     kpart->textgc, x, kpart->ascent,
				     &c, 1, 1);
		if (c == kpart->cursymbol) {
			revx = x;
			revwidth = width;
		}
		x += width + 8;
	}

	/* カレントシンボルを反転する */
	XFillRectangle(XtDisplay(w), XtWindow(w->kinput.statuswid),
		       w->kinput.revgc,
		       revx, 0, revwidth, w->kinput.fontheight);
}

#if __STDC__ == 1
static wchar_t
nextSymbol(wchar_t c, int dir)
#else
static wchar_t
nextSymbol(c, dir)
wchar_t	c;
int	dir;
#endif
{
#define NCROW	(0xfe - 0xa1 + 1)	/* number of character in a row */
	int	uh = c & 0xff00;
	int	lh = c & 0xff;

	if (dir == INC) {
		if (++lh > 0xfe) {
			uh += 0x100;
			lh = 0xa1;
		}
	} else {	/* DEC */
		if (--lh < 0xa1) {
			uh -= 0x100;
			lh = 0xfe;
		}
	}

	return (wchar_t)(uh | lh);
}
