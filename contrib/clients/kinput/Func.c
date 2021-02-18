/* $Id: Func.c,v 3.2 89/10/27 16:57:29 ishisone Rel $ */
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

#define STATE	(w->kinput.state)
#define JCBUF	(w->kinput.jcbuf)
#define CCBUF	(w->kinput.ccbuf)

/*
 * カーソル移動
 */

static void
move_top(w)
KanjiInputWidget w;
{
	(void)jcTop(JCBUF);
	ccContextClear(CCBUF);
	UpdateHint(w) = REDRAW;
}

static void
move_bottom(w)
KanjiInputWidget w;
{
	(void)jcBottom(JCBUF);
	ccContextClear(CCBUF);
	UpdateHint(w) = REDRAW;
}

static void
move_forward(w)
KanjiInputWidget w;
{
	warn(jcMove(JCBUF, 1, JC_FORWARD));
	ccContextClear(CCBUF);
	UpdateHint(w) = REDRAW;
}

static void
move_backward(w)
KanjiInputWidget w;
{
	warn(jcMove(JCBUF, 1, JC_BACKWARD));
	ccContextClear(CCBUF);
	UpdateHint(w) = REDRAW;
}

/*
 * 次候補/前候補
 */

static void
next_or_prev(w, small, type)
KanjiInputWidget w;
int small;
int type;
{
	warn(jcNext(JCBUF, small, type));
	ccContextClear(CCBUF);
	UpdateHint(w) = REDRAW;
}

static void
select_next(w)
KanjiInputWidget w;
{
	next_or_prev(w, 0, JC_NEXT);
}

static void
select_next_small(w)
KanjiInputWidget w;
{
	next_or_prev(w, 1, JC_NEXT);
}

static void
select_prev(w)
KanjiInputWidget w;
{
	next_or_prev(w, 0, JC_PREV);
}

static void
select_prev_small(w)
KanjiInputWidget w;
{
	next_or_prev(w, 1, JC_PREV);
}

/*
 * 確定
 */

static void
fix(w)
KanjiInputWidget w;
{
	jcConvBuf	*jcbuf = JCBUF;

	if (STATE == MODE_SELECTION_L || STATE == MODE_SELECTION_S)
		endSelection(w, 0);
	else if (STATE == MODE_SYMBOL)
		endSymbol(w, 0);

	if (jcFix(jcbuf) < 0) {
		beep(w);
		return;
	}
	ccContextClear(CCBUF);

	/* 確定の処理 */
	CopyDisplayBuf(w);
	XtCallCallbacks((Widget)w, XtNfixNotify, (caddr_t)w->kinput.string);

	/* バッファをクリアする */
	jcClear(jcbuf);

	UpdateHint(w) = REDRAW;
}

/*
 * クリア
 */

static void
clear_buffer(w)
KanjiInputWidget w;
{
	if (STATE == MODE_SELECTION_L || STATE == MODE_SELECTION_S)
		endSelection(w, 0);
	else if (STATE == MODE_SYMBOL)
		endSymbol(w, 0);

	warn(jcClear(JCBUF));
	ccContextClear(CCBUF);
	UpdateHint(w) = REDRAW;
}

/*
 * 変換/無変換
 */

static void
convert(w)
KanjiInputWidget w;
{
	jcConvBuf	*jcbuf = JCBUF;

	if (jcbuf->curClause == jcbuf->nClause) {
		(void)jcMove(jcbuf, 0, JC_BACKWARD);
		UpdateHint(w) = REDRAW;
	}

	if (jcIsConverted(jcbuf, jcbuf->curClause)) {
		beginSelection(w, 0);
		return;
	}

	warn(jcConvert(jcbuf, 0, 0, 1));
	ccContextClear(CCBUF);
	UpdateHint(w) = REDRAW;
}

static void
convert_small(w)
KanjiInputWidget w;
{
	jcConvBuf	*jcbuf = JCBUF;

	if (jcbuf->curClause == jcbuf->nClause) {
		(void)jcMove(jcbuf, 1, JC_BACKWARD);
		UpdateHint(w) = REDRAW;
	}

	if (jcIsConverted(jcbuf, jcbuf->curClause)) {
		beginSelection(w, 1);
		return;
	}

	warn(jcConvert(jcbuf, 1, 0, 1));
	ccContextClear(CCBUF);
	UpdateHint(w) = REDRAW;
}

static void
unconvert(w)
KanjiInputWidget w;
{
	warn(jcUnconvert(JCBUF));
	ccContextClear(CCBUF);
	UpdateHint(w) = REDRAW;
}

/*
 * 文節の拡大/縮小
 */

static void
expand_or_shrink(w, shrink, small, conv)
KanjiInputWidget w;
int shrink;
int small;
int conv;
{
	if (STATE == MODE_SELECTION_L || STATE == MODE_SELECTION_S)
		endSelection(w, 0);
	else if (STATE == MODE_SYMBOL)
		endSymbol(w, 0);

	if (shrink) {
		warn(jcShrink(JCBUF, small, conv));
	} else {
		warn(jcExpand(JCBUF, small, conv));
	}
	ccContextClear(CCBUF);
	UpdateHint(w) = REDRAW;
}

static void
expand_clause(w)
KanjiInputWidget w;
{
	expand_or_shrink(w, 0, 0, 1);
}

static void
expand_clause_small(w)
KanjiInputWidget w;
{
	expand_or_shrink(w, 0, 1, 1);
}

static void
expand_clause2(w)
KanjiInputWidget w;
{
	expand_or_shrink(w, 0, 0, 0);
}

static void
expand_clause2_small(w)
KanjiInputWidget w;
{
	expand_or_shrink(w, 0, 1, 0);
}

static void
shrink_clause(w)
KanjiInputWidget w;
{
	expand_or_shrink(w, 1, 0, 1);
}

static void
shrink_clause_small(w)
KanjiInputWidget w;
{
	expand_or_shrink(w, 1, 1, 1);
}

static void
shrink_clause2(w)
KanjiInputWidget w;
{
	expand_or_shrink(w, 1, 0, 0);
}

static void
shrink_clause2_small(w)
KanjiInputWidget w;
{
	expand_or_shrink(w, 1, 1, 0);
}

/*
 * 候補選択
 */

static void
selection_top(w)
KanjiInputWidget w;
{
	moveSelection(w, SEL_TOP);
}

static void
selection_bottom(w)
KanjiInputWidget w;
{
	moveSelection(w, SEL_BOTTOM);
}

static void
selection_forward(w)
KanjiInputWidget w;
{
	moveSelection(w, SEL_RIGHT);
}

static void
selection_backward(w)
KanjiInputWidget w;
{
	moveSelection(w, SEL_LEFT);
}

static void
selection_next(w)
KanjiInputWidget w;
{
	moveSelection(w, SEL_NEXT);
}

static void
selection_prev(w)
KanjiInputWidget w;
{
	moveSelection(w, SEL_PREV);
}

static void
selection_select(w)
KanjiInputWidget w;
{
	int	small = STATE == MODE_SELECTION_S;

	endSelection(w, 0);
	(void)jcMove(JCBUF, small, JC_FORWARD);
	UpdateHint(w) = REDRAW;
}

/*
 * 記号入力
 */

static void
symbol_input(w)
KanjiInputWidget w;
{
	beginSymbol(w);
}

static void
symbol_top(w)
KanjiInputWidget w;
{
	moveSymbol(w, SEL_TOP);
}

static void
symbol_bottom(w)
KanjiInputWidget w;
{
	moveSymbol(w, SEL_BOTTOM);
}

static void
symbol_forward(w)
KanjiInputWidget w;
{
	moveSymbol(w, SEL_RIGHT);
}

static void
symbol_backward(w)
KanjiInputWidget w;
{
	moveSymbol(w, SEL_LEFT);
}

static void
symbol_next(w)
KanjiInputWidget w;
{
	moveSymbol(w, SEL_NEXT);
}

static void
symbol_prev(w)
KanjiInputWidget w;
{
	moveSymbol(w, SEL_PREV);
}

static void
symbol_select(w)
KanjiInputWidget w;
{
	endSymbol(w, 0);
	UpdateHint(w) = REDRAW;
}

static void
symbol_abort(w)
KanjiInputWidget w;
{
	endSymbol(w, 1);
}

/*
 * 文字種の変換
 *	半角 <-> 全角
 *	ひらがな <-> カタカナ
 */

static void
hankaku(w)
KanjiInputWidget w;
{
	wchar_t	han[256];
	wchar_t	*h;
	wchar_t	*ks, *ke;
	register int	c;
	jcConvBuf	*jcbuf = JCBUF;

	if (STATE == MODE_SELECTION_L || STATE == MODE_SELECTION_S)
		endSelection(w, 0);
	else if (STATE == MODE_SYMBOL)
		endSymbol(w, 0);

	if (jcbuf->curClause != jcbuf->nClause) {
		jcClause	*cinfo = jcbuf->clauseInfo;
		ks = cinfo[jcbuf->curLCStart].kanap;
		ke = cinfo[jcbuf->curLCEnd].kanap;
		h = han;
		while (ks < ke) {
			c = *ks++;
			if (0xa1a1 <= c && c <= 0xa1fe) {
				/* 記号 バイナリ・サーチする */
				register struct symzenhan	*hi, *lo, *m;
				register int	dif;

				lo = kigoutohan;
				hi = kigoutohan + KIGOUSIZE;

				while (lo <= hi) {
					m = lo + (hi - lo) / 2;
					dif = c - m->zen;
					if (dif < 0)
						hi = m - 1;
					else if (dif > 0)
						lo = m + 1;
					else 
						break;
				}
				if (lo > hi)
					*h++ = c;
				else
					*h++ = m->han;
			} else if (0xa3b0 <= c && c <= 0xa3b9) {
				*h++ = c - 0xa3b0 + '0';
			} else if (0xa3c1 <= c && c <= 0xa3da) {
				/* A-Z */
				*h++ = c - 0xa3c1 + 'A';
			} else if (0xa3e1 <= c && c <= 0xa3fa) {
				/* a-z */
				*h++ = c - 0xa3e1 + 'a';
			} else if (0xa4a1 <= c && c <= 0xa4f3) {
				/* ひらがな */
				c = hiratohan[c - 0xa4a1];
				*h++ = c & 0xff;
				if (c & 0xff00)
					*h++ = c >> 8;
			} else if (0xa5a1 <= c && c <= 0xa5f3) {
				/* かたかな */
				c = hiratohan[c - 0xa5a1];
				*h++ = c & 0xff;
				if (c & 0xff00)
					*h++ = c >> 8;
			} else
				*h++ = c;
		}
		*h = 0;	/* NULL terminate */
		warn(jcChangeClause(jcbuf, han));
	}
	ccContextClear(CCBUF);
	UpdateHint(w) = REDRAW;
}

static void
zenkaku(w)
KanjiInputWidget w;
{
	wchar_t	han[256];
	wchar_t	*ks, *ke;
	wchar_t	*h;
	register int	c;
	jcConvBuf	*jcbuf = JCBUF;

	if (STATE == MODE_SELECTION_L || STATE == MODE_SELECTION_S)
		endSelection(w, 0);
	else if (STATE == MODE_SYMBOL)
		endSymbol(w, 0);

	if (jcbuf->curClause != jcbuf->nClause) {
		jcClause	*cinfo = jcbuf->clauseInfo;
		ks = cinfo[jcbuf->curLCStart].kanap;
		ke = cinfo[jcbuf->curLCEnd].kanap;
		h = han;
		while (ks < ke) {
			c = *ks++;
			/* ちなみに"か゛"といったものを
			 * "が"にするような親切はしない
			 * だいたい、半角を使おうって方が
			 * 間違ってるよな
			 */
			if ((0x20 <= c && c <= 0x7e) ||
			    (0xa1 <= c && c <= 0xdf))
				*h++ = hantozen[c];
			else
				*h++ = c;
		}
		*h = 0;	/* NULL terminate */
		warn(jcChangeClause(jcbuf, han));
	}
	ccContextClear(CCBUF);
	UpdateHint(w) = REDRAW;
}

static void
hiragana(w)
KanjiInputWidget w;
{
	if (STATE == MODE_SELECTION_L || STATE == MODE_SELECTION_S)
		endSelection(w, 0);
	else if (STATE == MODE_SYMBOL)
		endSymbol(w, 0);

	warn(jcKana(JCBUF, 0, JC_HIRAGANA));
	ccContextClear(CCBUF);
	UpdateHint(w) = REDRAW;
}

static void
katakana(w)
KanjiInputWidget w;
{
	if (STATE == MODE_SELECTION_L || STATE == MODE_SELECTION_S)
		endSelection(w, 0);
	else if (STATE == MODE_SYMBOL)
		endSymbol(w, 0);

	warn(jcKana(JCBUF, 0, JC_KATAKANA));
	ccContextClear(CCBUF);
	UpdateHint(w) = REDRAW;
}

/*
 * 文字削除
 */

static void
backspace(w)
KanjiInputWidget w;
{
	if (STATE == MODE_SELECTION_L || STATE == MODE_SELECTION_S) {
		int	small = STATE == MODE_SELECTION_S;
		endSelection(w, 0);
		(void)jcMove(JCBUF, small, JC_FORWARD);
	} else if (STATE == MODE_SYMBOL)
		endSymbol(w, 0);

	ccContextDelete(CCBUF);
	warn(jcDeleteChar(JCBUF, 1));
	UpdateHint(w) = REDRAW;
}

static void
delete(w)
KanjiInputWidget w;
{
	if (STATE == MODE_SELECTION_L || STATE == MODE_SELECTION_S)
		endSelection(w, 0);
	else if (STATE == MODE_SYMBOL)
		endSymbol(w, 0);

	warn(jcDeleteChar(JCBUF, 0));
	ccContextClear(CCBUF);
	UpdateHint(w) = REDRAW;
}

static void
carriageret(w)
KanjiInputWidget w;
{
	insChar('\r', (caddr_t)w);
	fix(w);
}

static void
fixwrite(w)
KanjiInputWidget w;
{
	fix(w);
}

static void
beep(w)
KanjiInputWidget w;
{
	XBell(XtDisplay(w), 50);
}

/* ARGSUSED */
static void
jiscode_begin(w)
KanjiInputWidget w;
{
	/* NOT IMPLEMENTED */
}

/* ARGSUSED */
static void
jiscode_end(w)
KanjiInputWidget w;
{
	/* NOT IMPLEMENTED */
}

/* ARGSUSED */
static void
inspect(w)
KanjiInputWidget w;
{
/*	NOT IMPLEMENTED
	if (doInspection() < 0)
		beep(w);
*/
}

static void
convend(w)
KanjiInputWidget w;
{
	if (STATE == MODE_SELECTION_L || STATE == MODE_SELECTION_S)
		endSelection(w, 0);
	else if (STATE == MODE_SYMBOL)
		endSymbol(w, 0);

	fix(w);
	XtCallCallbacks((Widget)w, XtNendNotify, NULL);
}

static void
insChar(c, cldata)
int c;
caddr_t cldata;
{
	KanjiInputWidget	w = (KanjiInputWidget)cldata;
	jcConvBuf		*jcbuf = JCBUF;

	if (STATE == MODE_SELECTION_L || STATE == MODE_SELECTION_S)
		endSelection(w, 0);
	else if (STATE == MODE_SYMBOL)
		endSymbol(w, 0);

	if (jcIsConverted(jcbuf, jcbuf->curClause)) {
		(void)jcBottom(jcbuf);
	}

	(void)jcInsertChar(jcbuf, c);
	UpdateHint(w) = REDRAW;
}

static void
delChar(cldata)
caddr_t cldata;
{
	KanjiInputWidget	w = (KanjiInputWidget)cldata;

	if (STATE == MODE_SELECTION_L || STATE == MODE_SELECTION_S ||
	    STATE == MODE_SYMBOL) {
		beep(w);
		return;
	}
	ccContextDelete(CCBUF);
	jcDeleteChar(JCBUF, 1);
	UpdateHint(w) = REDRAW;
}
