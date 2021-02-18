#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: Cursors.c,v 5.5 89/11/22 15:10:04 glennw Exp $";
#endif /* RCS_ID */
#endif /* LINT */


/*
 * Copyright 1987, 1988 by Digital Equipment Corporation, Maynard,
 * Massachusetts, and the Massachusetts Institute of Technology, Cambridge,
 * Massachusetts.  Copyright 1987 by Jordan Hubbard.  Copyright 1987, 1988 by
 * Ardent Computer Corporation, Sunnyvale, Ca.  Copyright 1989 by Tektronix,
 * Inc., Beaverton, Oregon.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Ardent Computer Corporation, Jordan
 * Hubbard, Digital Equipment, Tektronix or M.I.T.  not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Ardent Computer Corporation,
 * Jordan Hubbard, Digital Equipment, Tektronix and M.I.T.  make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL EQUIPMENT, TEKTRONIX,
 * AND M.I.T.  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL
 * EQUIPMENT, TEKTRONIX, OR M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Glenn Widener, Tektronix, Inc.
 *          P.O. Box 1000
 *          Wilsonville, OR, 97070
 *          glennw@orca.wv.tek.com
 */

/*
 * MODIFICATION HISTORY
 *
 * 000 -- M. Gancarz, DEC Ultrix Engineering Group
 * 001 -- Loretta Guarino Reid, DEC Ultrix Engineering Group
 *        Convert to X11
 */

#ifndef lint
static char *sccsid = "@(#)StoreCursors.c	3.8	1/24/86";
#endif

#include "awm.h"
#include "X11/cursorfont.h"

/*
 * Store all the cursors into global variables.
 */
StoreCursors()
{
    Entry("StoreCursors")

    /*
     * Main awm cursor and movement cursor.
     */
    ArrowCrossCursor = XCreateFontCursor(dpy, XC_fleur);
    if (ArrowCrossCursor == FAILURE) {
	Error("StoreCursors -> Unable to store ArrowCrossCursor.");
    }

    /*
     * Text cursor used in icons.
     */
    TextCursor = XCreateFontCursor(dpy, XC_xterm);	
    if (TextCursor == FAILURE) {
	Error("StoreCursors -> Unable to store TextCursor.");
    }

    /*
     * Icon cursor used to iconify windows.
     */
    IconCursor = XCreateFontCursor(dpy, XC_icon);	
    if (IconCursor == FAILURE) {
	Error("StoreCursors -> Unable to store IconCursor.");
    }

    /*
     * Menu cursor.
     */
    MenuCursor = XCreateFontCursor(dpy, XC_sb_left_arrow);
    if (MenuCursor == FAILURE) {
	Error("StoreCursors -> Unable to store MenuCursor.");
    }

    /*
     * Left button main cursor.
     */
    LeftButtonCursor = XCreateFontCursor(dpy, XC_leftbutton);	
    if (LeftButtonCursor == FAILURE) {
	Error("StoreCursors -> Unable to store LeftButtonCursor.");
    }

    /*
     * Middle button main cursor.
     */
    MiddleButtonCursor = XCreateFontCursor(dpy, XC_middlebutton);	
    if (MiddleButtonCursor == FAILURE) {
	Error("StoreCursors -> Unable to store MiddleButtonCursor.");
    }

    /*
     * Right button main cursor.
     */
    RightButtonCursor = XCreateFontCursor(dpy, XC_rightbutton);	
    if (RightButtonCursor == FAILURE) {
	Error("StoreCursors -> Unable to store RightButtonCursor.");
    }

    /*
     * Targer cursor used to identify a window for an action.
     */
    TargetCursor = XCreateFontCursor(dpy, XC_circle);	
    if (TargetCursor == FAILURE) {
	Error("StoreCursors -> Unable to store TargetCursor.");
    }
    Leave(0)
}

