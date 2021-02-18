#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: XKeysym.c,v 1.2 89/11/13 08:16:26 glennw Exp $";
#endif RCS_ID
#endif LINT

/* $XConsortium:$ */

/*
 * Copyright 1989 by Tektronix, Inc. Beaverton, Oregon,
 * and the Massachusetts Institute of Technology, Cambridge, Massachusetts.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Tektronix or M.I.T. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Tektronix and M.I.T. make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * TEKTRONIX AND M.I.T. DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL TEKTRONIX OR M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Glenn Widener, Tektronix, Inc.
 *          P.O. Box 1000
 *          Wilsonville, OR, 97070
 *          glennw@orca.wv.tek.com
 */

#include "Xlibint.h"

/* This routine finds the lowest-valued modifier bit pattern that points to
the specified column of the keycode-keysym table.  If no modifier bit
pattern selects the specified column, the routine returns zero.
If the keyboard modifier index property does not exist, the routine
returns Shift for column 1 and no modifiers for column 0. */

Status XColumnToModifier(dpy, column, modifier)
/*ARGSUSED*/	/* until we implement keyboard arch */
Display *dpy;
int column;		/* In: column of keycode-keysym table */
int *modifier;		/* Out: mask of modifier bits */
{
    /* if ( no index property) */
    switch (column) {
      case 0:
	*modifier = 0;
	return (1);
      case 1:
	*modifier = ShiftMask;
	return (1);
    }
    /* default: (keep lint happy) */
    return (0);
}

/* This routine is the same as XKeysymToKeycode, except it also returns the
column in which the keysym was found. */

KeyCode XKeysymToKeycodeAndColumn(dpy, ks, column)
    Display *dpy;
    KeySym ks;
    int *column;
{
    int         i;

/*     if (dpy->keysyms == NULL)
         Initialize(dpy); really should do this!! But we can't without
	    putting this routine in Xlib. So get the effect with a null
	  call to XKeycodeToKeysym: */
    (void) XKeycodeToKeysym(dpy, (KeyCode)NoSymbol, -1);

    for (i = dpy->min_keycode; i <= dpy->max_keycode; i++) {
	int         j;

	for (j = 0; j < dpy->keysyms_per_keycode; j++) {
	    int ind = (i - dpy->min_keycode) * dpy->keysyms_per_keycode + j;

	    if (ks == dpy->keysyms[ind]) {
		*column = j;
		return (i);
	    }
	}
    }
    return (0);
}
