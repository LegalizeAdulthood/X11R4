#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: Focus.c,v 5.10 89/11/22 15:10:09 glennw Exp $";
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
    Western Software Lab. Converted to X11.
 */

#ifndef lint
static char *sccsid = "@(#)Focus.c	3.8	1/24/86";
#endif

#include "awm.h"

Boolean Focus(window, mask, button, x, y)
Window window;				/* Event window. */
int mask;				/* Button/key mask. */
int button;				/* Button event detail. */
int x, y;				/* Event mouse position. */
{
     AwmInfoPtr awi;

     Entry("Focus");

     if (!(awi = GetAwmInfo(window)))
	 Leave(FALSE)
     if (IsIcon(window, NULL)) {
	 if (awi->client_icon)
	     /* focus can be set to (client) icon */
	     window = awi->client_icon;
	 else {
	     /* next guess - set focus to client window.
		Must deiconify client window to set focus to it.
		Calling Iconify with zeros causes it to not zap or warp */
	     Iconify(window, 0, 0, 0, 0);
	     window = awi->client;
	 }
     }
     else
	 /* focus must be set to actual client window */
	 window = awi->client;
     if (window == DefaultRootWindow(dpy)) {
	  FocusSetByUser = FALSE;
	  /* server apparently runs faster if PointerRoot */
	  XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
     }
     else {
	  FocusSetByUser = TRUE;
	  XSetInputFocus(dpy, window, RevertToPointerRoot, CurrentTime);
     }
     FocusSetByWM = FALSE;
     FocusWindow = window;
     Leave(FALSE)
}

Boolean UnFocus(window, mask, button, x, y)
Window window;				/* Event window. */
int mask;				/* Button/key mask. */
int button;				/* Button event detail. */
int x, y;				/* Event mouse position. */
{
     Entry("UnFocus");

     XSetInputFocus(dpy, PointerRoot, None, CurrentTime);
     FocusSetByUser = FALSE;
     FocusSetByWM = FALSE;
     FocusWindow = RootWindow(dpy, scr);
     Leave(FALSE)
}

