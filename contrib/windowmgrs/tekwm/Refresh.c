#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: Refresh.c,v 5.5 89/11/22 15:12:26 glennw Exp $";
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
 * 001 -- Loretta Guarino Reid, DEC Ultrix Engineering Group,
 *  Western Software Lab. Convert to X11.
 * 002 -- Jordan Hubbard, Ardent Computer
 *  Changes for awm.
 */

#include "awm.h"

Boolean Refresh(window, mask, button, x, y)
Window window;                          /* Event window. */
int mask;                               /* Button/key mask. */
int button;                           /* Button event detail. */
int x, y;                               /* Event mouse position. */
{
    Entry("Refresh")

#ifdef titan
    XTitanReset(dpy);
#endif /* titan */
    Leave( Redraw(RootWindow(dpy,scr), mask, button, x, y) )
}



Boolean Redraw(window, mask, button, x, y)
Window window;                          /* Event window. */
int mask;                               /* Button/key mask. */
int button;                             /* Button event detail. */
int x, y;                               /* Event mouse position. */
{
    XWindowAttributes winfo;		/* window info. */
    XSetWindowAttributes swa;		/* New window info */
    Window w;				/* Refresh window. */

    Entry("Redraw")

    /*
     * Get info on the target window.
     */
    status = XGetWindowAttributes(dpy, window, &winfo);
    if (status == FAILURE)
	 Error("Refresh -> Can't query target window.");
    /*
     * Create and map a window which covers the target window, then destroy it.
     */
    swa.override_redirect = TRUE;
    swa.background_pixel = 0;

    /*
     * What we have here is a failure to communicate. This window should
     * more properly be created as a subwindow of "window", but
     * when we do that, all events (Create, Map, Destroy) get reported
     * as coming from "window", not the newly created one. This is
     * very nasty when the DestroyNotify is received. For now, we'll
     * create it on the RootWindow and take the chance of generating an
     * expose on an overlapping window.
     */
    if ((w = XCreateWindow(dpy, RootWindow(dpy, scr), winfo.x, winfo.y, 
			   winfo.width, winfo.height, 0,
			   DefaultDepth(dpy, scr),
			   CopyFromParent,
			   DefaultVisual(dpy, scr),
			   (CWOverrideRedirect | CWBackPixel),
			   &swa)) == NULL)
	 Error("Refresh -> Can't create refresh window.");
    XMapWindow(dpy, w);
    XDestroyWindow(dpy, w);
    XFlush(dpy);
    Leave(FALSE)
}
