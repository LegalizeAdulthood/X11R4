/* $Id: Update.c,v 3.5 89/10/31 14:44:43 ishisone Exp $ */
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

#define REDRAWREST	1
#define REDRAWCLAUSE	2
#define OVERWRITEREST	3
#define OVERWRITECLAUSE	4
#define REVERSECLAUSE	5
#define STIPPLECLAUSE	6

#define REVERSE	1
#define ULINE	2
#define STIPPLE	3

#ifndef MAX
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#endif

#if __STDC__ == 1
static int CheckClause(KanjiInputWidget, int, int *);
static void DrawClause(KanjiInputWidget, int *, int *, int, int);
static void flushstring(KanjiInputWidget, int, int, wchar_t *, wchar_t *, int, int);
static void getPosition(KanjiInputWidget, int *, int *, wchar_t *, wchar_t *, int);
static void clearClause(KanjiInputWidget, int, int, int);
static void ReverseClause(KanjiInputWidget, int);
static void clearrest(KanjiInputWidget, int, int);
static void SetCursor(KanjiInputWidget, int, int, int);
static void EraseCursor(KanjiInputWidget);
static void DrawCursor(KanjiInputWidget);
static void CursorPosition(KanjiInputWidget);
#else
static int CheckClause();
static void DrawClause();
static void flushstring();
static void getPosition();
static void clearClause();
static void ReverseClause();
static void clearrest();
static void SetCursor();
static void EraseCursor();
static void DrawCursor();
static void CursorPosition();
#endif

static void
RedrawText(w)
KanjiInputWidget w;
{
	int		clnum;
	int		x, y;
	ClauseRec	*crec = w->kinput.clrec;

	/* $B$H$j$"$($:2hLL$r%/%j%"$9$k(J */
	XClearWindow(XtDisplay(w), XtWindow(w));

	x = w->kinput.leftmargin;
	y = w->kinput.topmargin + w->kinput.ascent;

	clnum = 0;

	while (clnum < w->kinput.nclause) {
		DrawClause(w, &x, &y, clnum++, 0);
	}
	crec[clnum].x = x;
	crec[clnum].y = y;

	UpdateRec(w);

	/* $B%+!<%=%k$N0LCV$r7W;;$9$k(J */
	CursorPosition(w);

	/* $B%+!<%=%k$r=q$/(J */
	DrawCursor(w);
}

static void
UpdateText(w)
KanjiInputWidget w;
{
	KanjiInputPart	*kpart = &(w->kinput);
	jcConvBuf	*jcbuf = kpart->jcbuf;
	int	clnum;
	int	redraw;
	int	offset;
	int	x, y;

	/*
	 * $B%"%C%W%G!<%H$7$J$,$i(J clrec $B$r=q$-49$($F$$$/$N$G!"(J
	 * $B$^$:$O(J clrec $B$N%5%$%:$,==J,$+$rD4$Y!">.$5$+$C$?$i(J
	 * $BA}$d$7$F$*$/(J
	 */
	if (kpart->clrecsize <= jcbuf->nClause) {
		kpart->clrecsize = MAX(kpart->clrecsize, jcbuf->nClause + 1);
		kpart->clrec = (ClauseRec *)
		    XtRealloc(kpart->clrec,
			      sizeof(ClauseRec) * kpart->clrecsize);
	}

	/* $B%+!<%=%k$r>C$7$F$*$/(J */
	EraseCursor(w);

	x = kpart->leftmargin;
	y = kpart->topmargin + kpart->ascent;

	redraw = 0;
	clnum = 0;

	while (clnum < jcbuf->nClause && !redraw) {
		PRINT(("UpdateText: clnum=%d x,y = %d,%d\n", clnum, x, y));
		switch (CheckClause(w, clnum, &offset)) {
		case REDRAWREST:
			PRINT(("REDRAWREST: clnum=%d\n", clnum));
			clearClause(w, clnum, 0, 1);
			redraw = 1;
			DrawClause(w, &x, &y, clnum, 0);
			break;
		case REDRAWCLAUSE:
			PRINT(("REDRAWCLAUSE: clnum=%d\n", clnum));
			clearClause(w, clnum, 0, 0);
			DrawClause(w, &x, &y, clnum, 0);
			break;
		case OVERWRITEREST:
			PRINT(("OVERWRITEREST: clnum=%d offset=%d\n", clnum, offset));
			clearClause(w, clnum, offset, 1);
			redraw = 1;
			DrawClause(w, &x, &y, clnum, offset - 1);
			break;
		case OVERWRITECLAUSE:
			PRINT(("OVERWRITECLAUSE: clnum=%d offset=%d\n", clnum, offset));
			clearClause(w, clnum, offset, 0);
			DrawClause(w, &x, &y, clnum, offset - 1);
			break;
		case REVERSECLAUSE:
			PRINT(("REVERSECLAUSE: clnum=%d\n", clnum));
			ReverseClause(w, clnum);
			x = kpart->clrec[clnum + 1].x;
			y = kpart->clrec[clnum + 1].y;
			break;
		default:
			x = kpart->clrec[clnum + 1].x;
			y = kpart->clrec[clnum + 1].y;
		}
		clnum++;
	}

	while (clnum < jcbuf->nClause) {
		DrawClause(w, &x, &y, clnum++, 0);
	}

	clearrest(w, x, y);

	kpart->clrec[clnum].x = x;
	kpart->clrec[clnum].y = y;

	/* ClauseRec $B$r%"%C%W%G!<%H$9$k(J */
	UpdateRec(w);

	/* $B%+!<%=%k$N0LCV$r7W;;$9$k(J */
	CursorPosition(w);

	/* $B%+!<%=%k$r=q$/(J */
	DrawCursor(w);
}

static void
UpdateRec(w)
KanjiInputWidget w;
{
	jcConvBuf	*jcbuf = w->kinput.jcbuf;
	int		i;
	wchar_t		*wstr;
	ClauseRec	*crec = w->kinput.clrec;
	jcClause	*cinfo = jcbuf->clauseInfo;

	/* $B%G%#%9%W%l%$%P%C%U%!$r%3%T!<$9$k(J */
	CopyDisplayBuf(w);
	wstr = w->kinput.string;

	/* $BJ8@a>pJs(J(clrec)$B$r:n$k(J */
	/* $BJ8@a$N0LCV$N>pJs$O$9$G$K$O$$$C$F$$$k(J */
	for (i = 0; i <= jcbuf->nClause; i++) {
		crec[i].clause = wstr + (cinfo->dispp - jcbuf->displayBuf);
		crec[i].conv = cinfo->conv;
		cinfo++;
	}

	w->kinput.nclause = jcbuf->nClause;
	w->kinput.curclause = jcbuf->curClause;
	w->kinput.curlcstart = jcbuf->curLCStart;
	w->kinput.curlcend = jcbuf->curLCEnd;
	w->kinput.dotoffset = jcDotOffset(jcbuf);
}

static int
CheckClause(w, clnum, offset)
KanjiInputWidget w;
int clnum;
int *offset;
{
	jcConvBuf	*jcbuf = w->kinput.jcbuf;
	jcClause	*cinfo = jcbuf->clauseInfo + clnum;
	ClauseRec	*crec = &(w->kinput.clrec[clnum]);
	wchar_t		*new, *newend, *old, *oldend;
	int		state;

#define OLDCONV	(1)
#define NEWCONV	(1<<1)
#define OLDCUR	(1<<2)
#define NEWCUR	(1<<3)
#define OLDLCUR	(1<<4)
#define NEWLCUR	(1<<5)
	*offset = 0;

	/* $B$^$:$OJ8@aHV9f$N%A%'%C%/(J */
	if (clnum >= w->kinput.nclause)
		return REDRAWREST;

	/* $BJ8@a$N>uBV$N%A%'%C%/(J */
	state = 0;
	if (crec->conv) {
		state |= OLDCONV;
		if (clnum == w->kinput.curclause) {
			state |= OLDCUR;
		} else if (w->kinput.curlcstart <= clnum &&
			   clnum < w->kinput.curlcend) {
			state |= OLDLCUR;
		}
	}
	if (cinfo->conv) {
		state |= NEWCONV;
		if (clnum == jcbuf->curClause) {
			state |= NEWCUR;
		} else if (jcbuf->curLCStart <= clnum &&
			   clnum < jcbuf->curLCEnd) {
			state |= NEWLCUR;
		}
	}

	/* $BJ8;zNs$,F1$8$+$I$&$+$N%A%'%C%/(J */
	new = cinfo->dispp;
	newend = (cinfo + 1)->dispp;
	old = crec->clause;
	oldend = (crec + 1)->clause;
	while (new < newend && old < oldend) {
		if (*new != *old)
			break;
		new++, old++;
	}

	if (new == newend && old == oldend) {
		/*
		 * $BJ8;zNs$OJQ2=$7$F$$$J$$(J
		 */
		switch (state) {
		case OLDCONV|NEWCONV|OLDCUR|NEWCUR:
		case OLDCONV|NEWCONV|OLDLCUR|NEWLCUR:
		case OLDCONV|NEWCONV:
		case 0:
			return NOTCHANGED;
		case OLDCONV|NEWCONV|OLDCUR:
		case OLDCONV|NEWCONV|NEWCUR:
			return REVERSECLAUSE;
		default:
			return REDRAWCLAUSE;
		}
	} else {
		int	newwidth, oldwidth;

		newwidth = KITextWidth(w->kinput.textgc, new,
				       (newend - new), 1);
		oldwidth = KITextWidth(w->kinput.textgc, old,
				       (oldend - old), 1);
		switch (state) {
		case OLDCONV|NEWCONV:
		case 0:
			*offset = new - cinfo->dispp;
			if (newwidth == oldwidth)
				return OVERWRITECLAUSE;
			else
				return OVERWRITEREST;
		default:
			if (newwidth == oldwidth)
				return REDRAWCLAUSE;
			else
				return REDRAWREST;
		}
	}
	/* NOTREACHED */
}

static void
DrawClause(w, xp, yp, clnum, offset)
KanjiInputWidget w;
int *xp;
int *yp;
int clnum;
int offset;
{
	int		x, y;
	int		sx, sy;
	int		width;
	int		xmax = w->core.width - w->kinput.rightmargin;
	XWSGC		wgc = w->kinput.textgc;
	jcConvBuf	*jcbuf = w->kinput.jcbuf;
	jcClause	*cinfo = jcbuf->clauseInfo + clnum;
	ClauseRec	*crec = &(w->kinput.clrec[clnum]);
	wchar_t		*startp, *endp, *dp;
	int		cltop = (offset == 0);
	int		top;
	int		lb;
	int		attr;

	if (offset < 0)
		offset = 0;

	startp = cinfo->dispp;
	endp = (cinfo + 1)->dispp;

	getPosition(w, xp, yp, startp, startp + offset, 1);

	startp += offset;
	if (startp >= endp)
		return;		/* nothing to draw */

	if (!cinfo->conv) {
		attr = ULINE;
	} else if (clnum == jcbuf->curClause) {
		attr = REVERSE;
	} else if (jcbuf->curLCStart <= clnum && clnum < jcbuf->curLCEnd) {
		attr = STIPPLE;
	} else {
		attr = 0;
	}

	sx = x = *xp;
	sy = y = *yp;
	dp = startp;
	top = lb = cltop;
	while (dp < endp) {
		x += (width = charwidth(wgc, *dp, lb));
		if (x > xmax) {
			flushstring(w, sx, sy, startp, dp, attr, cltop);
			cltop = 0;
			lb = 0;
			y += w->kinput.fontheight;
			x = w->kinput.leftmargin;
			startp = dp;
			sx = x;
			sy = y;
		} else {
			dp++;
			lb = 0;
			if (top) {
				crec->x = x - width;
				crec->y = y;
				top = 0;
			}
		}
	}

	flushstring(w, sx, sy, startp, endp, attr, cltop);

	*xp = x;
	*yp = y;
}

static void
flushstring(w, x, y, sp, ep, attr, top)
KanjiInputWidget w;
int x;
int y;
wchar_t *sp;
wchar_t *ep;
int attr;
int top;
{
	int	width;

	if (sp >= ep)
		return;

	width = KIDrawString(XtDisplay(w), XtWindow(w), w->kinput.textgc,
			     x, y, sp, ep - sp, top);

	if (attr == ULINE) {
		XDrawLine(XtDisplay(w), XtWindow(w), w->kinput.normgc,
			  x, y + 1, x + width - 1, y + 1);
	} else if (attr == REVERSE) {
		reverseArea(w, x, y - w->kinput.ascent,
			    width, w->kinput.fontheight);
	} else if (attr == STIPPLE) {
		reverseArea(w, x, y - w->kinput.ascent,
			    width, w->kinput.fontheight);
		stippleArea(w, x, y - w->kinput.ascent,
			    width, w->kinput.fontheight);
	}
}

static void
getPosition(w, xp, yp, sp, ep, lb)
KanjiInputWidget w;
int *xp;
int *yp;
wchar_t *sp;
wchar_t *ep;
int lb;
{
	int	x = *xp;
	int	xmax = w->core.width - w->kinput.rightmargin;
	XWSGC	wgc = w->kinput.textgc;

	while (sp < ep) {
		x += charwidth(wgc, *sp, lb);
		if (x > xmax) {
			*yp += w->kinput.fontheight;
			x = w->kinput.leftmargin;
			lb = 0;
		} else {
			lb = 0;
			sp++;
		}
	}
	*xp = x;
}

static void
clearClause(w, clnum, offset, toend)
KanjiInputWidget w;
int clnum;
int offset;
int toend;
{
	ClauseRec	*crec = &(w->kinput.clrec[clnum]);
	ClauseRec	*crec1;
	int		x, y;

	crec1 = toend ? &(w->kinput.clrec[w->kinput.nclause]) : crec + 1;

	x = crec->x;
	y = crec->y;

	offset--;
	if (offset > 0) {
		getPosition(w, &x, &y, crec->clause, crec->clause + offset, 1);
	}

	while (y < crec1->y) {
		XClearArea(XtDisplay(w), XtWindow(w), x, y - w->kinput.ascent,
			   w->core.width - x, w->kinput.fontheight, False);
		x = w->kinput.leftmargin;
		y += w->kinput.fontheight;
	}

	if (crec1->x > x) {
		XClearArea(XtDisplay(w), XtWindow(w), x, y - w->kinput.ascent,
			   crec1->x - x, w->kinput.fontheight, False);
	}
}

static void
ReverseClause(w, clnum)
KanjiInputWidget w;
int clnum;
{
	ClauseRec	*crec = &(w->kinput.clrec[clnum]);
	ClauseRec	*crec1 = crec + 1;
	int		xmax = w->core.width - w->kinput.rightmargin;
	XWSGC		wgc = w->kinput.textgc;
	int		x, y;
	int		lb;
	wchar_t		*ws = w->kinput.jcbuf->clauseInfo[clnum].dispp;
	GC		gc;

	x = crec->x;
	y = crec->y;

	gc = w->kinput.revgc;

	lb = 1;
	while (y < crec1->y) {
		int	xend = x;
		int	width;
		while (xend <= xmax) {
			xend += (width = charwidth(wgc, *ws++, lb));
			lb = 0;
		}
		xend -= width;
		ws--;
		lb = 0;
		
		XFillRectangle(XtDisplay(w), XtWindow(w), gc,
			  x, y - w->kinput.ascent,
			  xend - x, w->kinput.fontheight);
		x = w->kinput.leftmargin;
		y += w->kinput.fontheight;
	}

	XFillRectangle(XtDisplay(w), XtWindow(w), gc,
		       x, y - w->kinput.ascent,
		       crec1->x - x, w->kinput.fontheight);
}

static void
clearrest(w, x, y)
KanjiInputWidget w;
int x;
int y;
{
	ClauseRec	*crec = &(w->kinput.clrec[w->kinput.nclause]);

	while (y < crec->y) {
		XClearArea(XtDisplay(w), XtWindow(w), x, y - w->kinput.ascent,
			   w->core.width - x, w->kinput.fontheight, False);
		x = w->kinput.leftmargin;
		y += w->kinput.fontheight;
	}

	if (crec->x > x) {
		XClearArea(XtDisplay(w), XtWindow(w), x, y - w->kinput.ascent,
			   crec->x - x, w->kinput.fontheight, False);
	}
}

static void
SetCursor(w, x, y, width)
KanjiInputWidget w;
int x;
int y;
int width;
{
	CursorRec	*crec = &(w->kinput.cursorrec);

PRINT(("SetCursor(): x,y,width = %d, %d, %d\n", x, y, width));
	crec->x = x;
	crec->y = y - w->kinput.ascent;
	crec->w = width;
	crec->h = w->kinput.fontheight;
}

static void
EraseCursor(w)
KanjiInputWidget w;
{
	CursorRec	*crec = &(w->kinput.cursorrec);

	if (!crec->visible)
		return;

	XFillRectangle(XtDisplay(w), XtWindow(w), w->kinput.revgc,
		       crec->x, crec->y, crec->w, crec->h);
	crec->visible = 0;
}

static void
DrawCursor(w)
KanjiInputWidget w;
{
	jcConvBuf	*jcbuf = w->kinput.jcbuf;
	CursorRec	*crec = &(w->kinput.cursorrec);

	/* $B%+%l%s%HJ8@a$,JQ49$5$l$F$$$l$P%+!<%=%k$O=q$+$J$$(J */
	if (jcIsConverted(jcbuf, jcbuf->curClause))
		return;

	XFillRectangle(XtDisplay(w), XtWindow(w), w->kinput.revgc,
		       crec->x, crec->y, crec->w, crec->h);
	crec->visible = 1;
}

static void
CursorPosition(w)
KanjiInputWidget w;
/* $B%+!<%=%k$NI=<(0LCV$r7W;;$9$k(J
 * $BJ8@a$N>pJs$O(J widget $BCf$N(J clrec $B$r;HMQ$7$F$$$k$N$G!"(J
 * $B$3$N>pJs$,99?7$5$l$?$"$H(J ($B6qBNE*$K$O(J UpdateRec() $B$,(J
 * $B8F$P$l$?$"$H(J) $B$K8F$P$J$1$l$P$J$i$J$$(J
 */
{
	KanjiInputPart	*kpart = &(w->kinput);
	ClauseRec	*clrec = &(kpart->clrec[kpart->curclause]);
	wchar_t		*dotp;
	int		x, y;
	int		dotoffset = kpart->dotoffset;
	int		cwidth;

	if (clrec->conv) {
		/* $B%+%l%s%HJ8@a$,JQ49$5$l$F$$$k;~$K$O(J
		 * $B%+!<%=%k$O=q$+$J$$(J
		 */
		return;
	}

	x = clrec->x;
	y = clrec->y;

	dotp = clrec->clause + dotoffset;

	if (kpart->curclause == kpart->nclause) {
		/* $B:G8e$NJ8@a$,JQ49>uBV$G!"%+!<%=%k$O<!$NJ8@a$NF,$K$"$k>l9g$K(J
		 * ($B<B$O(J jcCurClause == jcNClause $B$N$H$-(J) $B%+!<%=%k$r=q$/(J
		 * $B8=:_9T$K%+!<%=%k$,=q$1$J$$>l9g$K$O<!$N9T$K(J
		 * $B0\F0$7$J$1$l$P$J$i$J$$(J
		 */
		int	rest = w->core.width - kpart->rightmargin - x;
		if (kpart->curwidth > rest) {
			/* $B%+!<%=%k$,=q$1$J$$$N$G2~9T$9$k(J */
			x = kpart->leftmargin;
			y += kpart->fontheight;
		}
		SetCursor(w, x, y, kpart->curwidth);
		return;
	}

	if (dotp == (clrec + 1)->clause) {
		/* $B%I%C%H$,J8@a$N:G8e$K$"$k(J
		 * $B$3$N;~$K$O%+!<%=%k$O<!$NJ8@a$N@hF,$KI=<($5$l$k(J
		 * $B$?$@$7!"$=$NJ8@a$,:G8e$NJ8@a$@$C$?>l9g$K$O(J
		 * $B<!$NJ8@a$OB8:_$7$J$$$N$G%G%U%)%k%H$N%+!<%=%kI}$G(J
		 * $BI=<($9$k(J
		 */
		clrec++;
		x = clrec->x;
		y = clrec->y;
		if (kpart->curclause == kpart->nclause - 1) {
			int	rest = w->core.width - kpart->rightmargin - x;
			if (kpart->curwidth > rest) {
				/* $B%+!<%=%k$,=q$1$J$$$N$G2~9T$9$k(J */
				x = kpart->leftmargin;
				y += kpart->fontheight;
			}
			SetCursor(w, x, y, kpart->curwidth);
			return;
		}
		dotoffset = 0;
	}

	if (dotoffset > 0) {
		int	rest = w->core.width - kpart->rightmargin - x;
		int	offset = KITextWidth(kpart->textgc, clrec->clause,
					     kpart->dotoffset + 1, 1);
		if (offset > rest) {
			/* $BESCf$G2~9T$7$F$$$k$N$G(J
			 * $B0lJ8;z$:$D8+$F$$$/(J
			 */
			int	width = 0;
			wchar_t	*p = clrec->clause;
			int	top = 1;

			while (p <= dotp) {
				width = charwidth(kpart->textgc, *p++, top);
				top = 0;
				if (width > rest) {
					x = kpart->leftmargin;
					y += kpart->fontheight;
					rest = w->core.width -
					    kpart->rightmargin - x;
				}
				rest -= width;
				x += width;
			}
			x -= width;
		} else {
			x += KITextWidth(kpart->textgc, clrec->clause,
					 kpart->dotoffset, 1);
		}
	}

	/* $B%+!<%=%k$NI}$O$=$3$K$"$kJ8;z$NI}$K$9$k(J */
	cwidth = charwidth(w->kinput.textgc, *dotp, dotp == clrec->clause);
	PRINT(("cwidth=%d *dotp=%04x\n", cwidth, *dotp));
	SetCursor(w, x, y, cwidth);
}

static void
CopyDisplayBuf(w)
KanjiInputWidget w;
{
	KanjiInputPart	*kpart = &(w->kinput);
	jcConvBuf	*jcbuf = kpart->jcbuf;
	int		len = jcbuf->displayEnd - jcbuf->displayBuf;

	if (kpart->stringsize < len + 1) {
		kpart->string = (wchar_t *)XtRealloc(kpart->string,
						     sizeof(wchar_t) * (len + 1));
		kpart->stringsize = len + 1;
	}
	(void)bcopy((char *)jcbuf->displayBuf, (char *)kpart->string,
		    sizeof(wchar_t) * len);
	kpart->string[len] = 0;
}
