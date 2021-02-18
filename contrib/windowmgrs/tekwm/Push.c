#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: Push.c,v 5.8 89/11/22 15:12:09 glennw Exp $";
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
    Western Software Lab. Convert to X11.
 */

#include "awm.h"

#define SHOVE_DOWN	1
#define SHOVE_UP		2
#define SHOVE_LEFT	3
#define SHOVE_RIGHT	4

extern Boolean ShoveAll();

Boolean ShoveDown(window, mask, button, x, y)
Window window;				/* Event window. */
/*ARGSUSED*/
int mask;				/* Button/key mask. */
/*ARGSUSED*/
int button;				/* Button event detail. */
/*ARGSUSED*/
int x, y;				/* Event mouse position. */
{
    Entry("ShoveDown")

    Leave(ShoveAll(window, SHOVE_DOWN))
}

Boolean ShoveUp(window, mask, button, x, y)
Window window;				/* Event window. */
/*ARGSUSED*/
int mask;				/* Button/key mask. */
/*ARGSUSED*/
int button;				/* Button event detail. */
/*ARGSUSED*/
int x, y;				/* Event mouse position. */
{
    Entry("ShoveUp")

    Leave(ShoveAll(window, SHOVE_UP))
}

Boolean ShoveLeft(window, mask, button, x, y)
Window window;				/* Event window. */
/*ARGSUSED*/
int mask;				/* Button/key mask. */
/*ARGSUSED*/
int button;				/* Button event detail. */
/*ARGSUSED*/
int x, y;				/* Event mouse position. */
{
    Entry("ShoveLeft")

    Leave(ShoveAll(window, SHOVE_LEFT))
}

Boolean ShoveRight(window, mask, button, x, y)
Window window;				/* Event window. */
/*ARGSUSED*/
int mask;				/* Button/key mask. */
/*ARGSUSED*/
int button;				/* Button event detail. */
/*ARGSUSED*/
int x, y;				/* Event mouse position. */
{
    Entry("ShoveRight")

    Leave(ShoveAll(window, SHOVE_RIGHT))
}

Boolean ShoveAll(w, direction)
Window w;
int direction;
{
    XWindowAttributes winfo;		/* Event window information. */
    XWindowAttributes rinfo;		/* Root window information. */
    XWindowAttributes cliIconInfo;
    int client_x;			/* Client icon window x position */
    int inwx, inwy;			/* Icon name window position */
    AwmInfoPtr awi;
    XWindowChanges wc;
    int deltax, deltay;			/* to compute ConfigureNotify */
    int xofs, yofs;			/* Movement offsets. */
    int x, y;				/* New window position. */

    Entry("ShoveAll")

    /*
     * Do not try to move the root window.
     */
    if (w == RootWindow(dpy, scr))
        Leave(FALSE)

    awi = GetAwmInfo(w);
    if (!awi)
        Leave(FALSE)

    /*
     * Gather info on the event window.
     */
    if (IsIcon(w, NULL))
	 /* work off name icon */
	w = awi->icon;

    status = XGetWindowAttributes(dpy, w, &winfo);
    if (status == FAILURE) Leave(FALSE)

    if (w == awi->icon) {
	 /* compute overall dimensions */
	 AdjustIconGeom(awi, &winfo, &cliIconInfo);
    }

    if (!Pushval && Push)
	 Pushval = DEF_PUSH;

    /*
     * Calculate the movement offsets.
     */
    switch(direction) {
    case SHOVE_DOWN:
        xofs = 0;
        yofs = Push ? (winfo.height / Pushval) : Pushval;
        break;
    case SHOVE_UP:
        xofs = 0;
        yofs = 0 - (Push ? (winfo.height / Pushval) : Pushval);
        break;
    case SHOVE_LEFT:
        xofs = 0 - (Push ? (winfo.width / Pushval) : Pushval);
        yofs = 0;
        break;
    case SHOVE_RIGHT:
        xofs = Push ? (winfo.width / Pushval) : Pushval;
        yofs = 0;
        break;
    }

    /*
     * Calculate the new window position.
     */
    x = winfo.x + xofs;
    y = winfo.y + yofs;

    /*
     * Normalize the new window coordinates so we don't
     * lose the window off the edge of the screen.
     */
    if (x < (0 - winfo.width + CURSOR_WIDTH - (winfo.border_width << 1)))
        x = 0 - winfo.width + CURSOR_WIDTH - (winfo.border_width << 1);
    if (y < (0 - winfo.height + CURSOR_HEIGHT - (winfo.border_width << 1)))
        y = 0 - winfo.height + CURSOR_HEIGHT - (winfo.border_width << 1);
    if (x > (ScreenWidth - CURSOR_WIDTH))
        x = ScreenWidth - CURSOR_WIDTH;
    if (y > (ScreenHeight - CURSOR_HEIGHT))
        y = ScreenHeight - CURSOR_HEIGHT;

    /*
     * Move the window into place.
     */
    if ((w == awi->icon) && (awi->client_icon)) {
	inwx = x;
	inwy = y + cliIconInfo.height + (cliIconInfo.border_width << 1);
	AdjustIconX(&client_x, &inwx, cliIconInfo.width, winfo.width);
	XMoveWindow(dpy, awi->client_icon, client_x, y);
	XMoveWindow(dpy, w, inwx, inwy);
    }
    else
	XMoveWindow(dpy, w, x, y);
    if (w == awi->icon)
	/* reset client's IconPosition hint for restart */
        ResetIconPositionHint(awi, x, y);
    else if (w == awi->frame) {
	/* must inform client of window move.  Report 
	effective position as if client-requested border
	was in effect. */
	FrameToClientBorder(awi, &deltax, &deltay);
	wc.x = x + deltax;
	wc.y = y + deltay;
	XGetWindowAttributes(dpy, awi->client, &winfo);
	/* XXX Should cache in awi */
	wc.width = winfo.width;
	wc.height = winfo.height;
	SendSyntheticConfigureNotify(awi, &wc);
    }
    Leave(FALSE)
}
