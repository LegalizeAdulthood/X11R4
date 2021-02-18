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

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xmu.h>
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <ctype.h>
#include <X11/Intrinsic.h>
#include "ptyx.h"
#include "data.h"

#ifdef KCLIENT

#ifndef WCHAR_T
#define WCHAR_T
typedef unsigned short	wchar_t;
#endif

extern void	_beginConversion();
extern int	convCTtoJWS();
extern int	convWStoEUC();
extern int	convWStoSJIS();
extern int	convWStoJIS();
extern int	convEUCtoWS();

static Atom	japanese_conversion_atom;	/* JAPANESE_CONVERSION */
static Atom	compound_text_atom;		/* COMPOUND_TEXT */
static int	atominitialized;

static void
inputString(w, selection, type, format, size, str, client_data)
Widget		w;
Atom		selection;
Atom		type;
int		format;
unsigned long	size;
unsigned char	*str;
caddr_t		client_data;
{
	unsigned char	lbuf[256 + 1];
	unsigned char	*lstr, *p, *lp;
	int		n;
	int		(*convfunc)();
	int		pty = ((XtermWidget)w)->screen.respond;

	if (str == NULL)
		return;

	if (type == compound_text_atom && format == 8) {
		wchar_t		wbuf[256 + 1];
		wchar_t		*ws;
		n = convCTtoJWS(str, size, NULL);
		ws = (n > 256) ? (wchar_t *)XtMalloc((n+1) * sizeof(wchar_t))
			       : wbuf;
		(void)convCTtoJWS(str, size, ws);
		if (term->flags & EUC_KANJI)
		    convfunc = convWStoEUC;
		else if (term->flags & SJIS_KANJI)
		    convfunc = convWStoSJIS;
		else
		    convfunc = convWStoJIS;

		n = (*convfunc)(ws, NULL);
		lstr = (n > 256) ? (unsigned char *)XtMalloc(n + 1) : lbuf;
		(void)(*convfunc)(ws, lstr);
		if (ws != wbuf) XtFree(ws);
	} else {
		/* ignore unknown type */
		XtFree(str);
		return;
	}

	/*
	 * Hack: (since Compound Text can't send carriage return)
	 * change each newline into carriage return.
	 */
	for (p = lp = lstr; *p; p++)	{
		if (*p != '\n' || *p != '\r')
			continue;
		*p = '\r';
		v_write(pty, lp, p - lp + 1);
	    lp = p + 1;
	}
	if (lp != p)
		v_write(pty, lp, p - lp);

	XtFree(str);
	if (lstr != lbuf) XtFree(lstr);
}

void
BeginConversion(w, event, params, nparams)
Widget	w;
XEvent	*event;
String	*params;
Cardinal	*nparams;
{
	Atom	catom;

	if (!atominitialized) {
		japanese_conversion_atom =
		    XInternAtom(XtDisplay(w), "JAPANESE_CONVERSION", False);
		compound_text_atom =
		    XInternAtom(XtDisplay(w), "COMPOUND_TEXT", False);
		atominitialized = 1;
	}

	if (*nparams != 1) {
		catom = japanese_conversion_atom;
	} else {
		XmuInternStrings(XtDisplay(w), params, 1, &catom);
	}

	_beginConversion(w, catom, compound_text_atom, inputString, NULL);
}
#endif	/* KCLIENT */
