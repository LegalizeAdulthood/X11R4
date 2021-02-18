#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: Bindings.c,v 5.5 89/11/22 15:02:32 glennw Exp $";
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
 */

#ifndef lint
static char *sccsid = "@(#)Bindings.c	3.8	1/24/86";
#endif

/*
 * Bindings.c 	Default bindings for the X window manager 'awm'
 *
 * Note: Any changes to this file should also be added to the file
 * /usr/new/lib/X/awm/default.awmrc to keep users informed as to the bindings
 * contained herein.
 */

char *DefaultBindings[] = {
"menu=\"WindowOps\"{",
"New Window:!\"xterm&\"",
"RefreshScreen:f.refresh",
"Redraw:f.redraw",
"Move:f.move",
"Resize:f.resize",
"Lower:f.lower",
"Raise:f.raise",
"CircUp:f.circleup",
"CircDown:f.circledown",
"AutoIconify:f.iconify",
"LowerIconify:f.newiconify",
"NewIconify:f.newiconify",
"Focus:f.focus",
"Freeze:f.pause",
"UnFreeze:f.continue",
"Restart:f.restart",
"}",
"menu=\"Preferences\"{",
"Bell Loud:!\"xset b 7&\"",
"Bell Normal:!\"xset b 3&\"",
"Bell Off:!\"xset b off&\"",
"Click Loud:!\"xset c 8&\"",
"Click Soft:!\"xset c on&\"",
"Click Off:!\"xset c off&\"",
"Lock On:!\"xset l on&\"",
"Lock Off:!\"xset l off&\"",
"Mouse Fast:!\"xset m 4 2&\"",
"Mouse Normal:!\"xset m 2 5&\"",
"Mouse Slow:!\"xset m 1 1&\"",
"}",
"f.newiconify=m:w|i:delta l",
"f.raise=m:w|i:delta l",
"f.lower=m:w|i:l u",
"f.raise=m:w:m d",
"f.resize=m:w:delta m",
"f.raise=m:w|i:r d",
"f.move=m:w|i:delta r",
"f.circledown=m:r:l d",
"f.circleup=m:r:r d",
"f.circledown=m|s::l d",
"f.menu=:r:m d:\"WindowOps\"",
"f.menu=m|s::m d:\"WindowOps\"",
"f.menu=m|s::m d:\"Preferences\"",
"f.circleup=m|s::r d",
"f.iconify=m|c:w|i:l d",
"f.newiconify=m|l:w|i:l d",
"f.raise=m|l:w|i:l u",
"f.pushright=m|l:w|i:r d",
"f.pushleft=m|c:w|i:r d",
"f.pushup=m|l:w|i:m d",
"f.pushdown=m|c:w|i:m d",
0	/* Must NOT be removed. */
};
