#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: errHndlr.c,v 5.10 89/11/22 15:14:33 glennw Exp $";
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

#include <stdio.h>
#include "X11/Xlib.h"
#include "awm.h"  /* for Boolean typedef */

extern Boolean PrintProtoErrors;/* print info for errors received */

int errorStatus = False;

#ifdef DEBUG
void debug(s, x1, x2, x3, x4, x5, x6, x7, x8)
char *s;
long x1, x2, x3, x4, x5, x6, x7, x8;
{
	static FILE *con = 0;

	if (!con)
		con = fopen("/dev/console", "w");
	fprintf(con, s, x1, x2, x3, x4, x5, x6, x7, x8);
}
#endif

int ErrorHandler (dpy, event)
    Display *dpy;
    XErrorEvent *event;
{
    if (PrintProtoErrors) {
#ifdef DEBUG
    char *buffer[BUFSIZ];
    XGetErrorText(dpy, event->error_code, buffer, BUFSIZ);
    (void) debug("Hey!\n");
    (void) debug("X Error: %s\n", buffer);
    (void) debug("  Request Major code: %d\n", event->request_code);
    (void) debug("  Request Minor code: %d\n", event->minor_code);
    (void) debug("  ResourceId 0x%x\n", event->resourceid);
    (void) debug("  Error Serial #%d\n", event->serial);
    (void) debug("  Current Serial #%d\n", dpy->request);
#else
    fprintf(stderr, "tekwm: ");
    XsError(dpy, event);    /* gets error text from database */
#endif
    }
    errorStatus = True;
    return 0;
}

static char *eventname[] = {
	"zero",
	"one",
	"KeyPress",
	"KeyRelease",
	"ButtonPress",
	"ButtonRelease",
	"MotionNotify",
	"EnterNotify",
	"LeaveNotify",
	"FocusIn",
	"FocusOut",
	"KeymapNotify",
	"Expose",
	"GraphicsExpose",
	"NoExpose",
	"VisibilityNotify",
	"CreateNotify",
	"DestroyNotify",
	"UnmapNotify",
	"MapNotify",
	"MapRequest",
	"ReparentNotify",
	"ConfigureNotify",
	"ConfigureRequest",
	"GravityNotify",
	"ResizeRequest",
	"CirculateNotify",
	"CirculateRequest",
	"PropertyNotify",
	"SelectionClear",
	"SelectionRequest",
	"SelectionNotify",
	"ColormapNotify",
	"ClientMessage",
	"MappingNotify",
};

void print_event_info(s, ev)
char *s;
XEvent *ev;
{
	if (ev->type >= sizeof(eventname)/sizeof(eventname[1]))
	    fprintf(stderr, "%s: Wow! UNKNOWN EVENT type (%d) on event drawable %x\n", s, 
		ev->type, ev->xany.window);
	else    
	    if ((ev->type == MapRequest) || 
		(ev->type == CirculateRequest) || 
		(ev->type == CreateNotify) || 
		(ev->type == ConfigureRequest))
	        fprintf(stderr, "%s: EVENT %s, serial %d on\twindow %d(0x%x),\n\t\t\t\t\t\tparent %d(0x%x)\n", s, eventname[ev->type],
	        ev->xbutton.serial, ev->xmaprequest.window,
	        ev->xmaprequest.window, ev->xmaprequest.parent, 
	        ev->xmaprequest.parent);
	    else if ((ev->type == DestroyNotify) || 
		(ev->type == UnmapNotify) || 
		(ev->type == ConfigureNotify) || 
		(ev->type == GravityNotify) || 
		(ev->type == CirculateNotify) || 
		(ev->type == GravityNotify))
	        fprintf(stderr, "%s: EVENT %s, serial %d on\tevent window %d(0x%x),\n\t\t\t\t\t\twindow %d(0x%x)\n", s, eventname[ev->type],
	        ev->xbutton.serial, ev->xmap.event,
	        ev->xmap.event, ev->xmap.window, 
	        ev->xmap.window);
	    else if (ev->type == MapNotify)
	        fprintf(stderr, "%s: EVENT %s, serial %d on\t\tevent window %d(0x%x),\n\t\t\t\t\t\twindow %d(0x%x)\n", s, eventname[ev->type],
	        ev->xbutton.serial, ev->xmap.event,
	        ev->xmap.event, ev->xmap.window, 
	        ev->xmap.window);
	    else if (ev->type == ReparentNotify)
	        fprintf(stderr, "%s: EVENT %s, serial %d on\tevent window %d(0x%x,)\n\t\t\t\t\t\twindow %d(0x%x),\n\t\t\t\t\t\tparent %d(0x%x)\n", s, eventname[ev->type],
	        ev->xbutton.serial, ev->xreparent.event,
	        ev->xreparent.event, ev->xreparent.window,
	        ev->xreparent.window, ev->xreparent.parent, 
	        ev->xreparent.parent);
	    else
		fprintf(stderr, "%s: EVENT %s, serial %d on\tevent drawable %d(0x%x)\n", s, eventname[ev->type],
		ev->xbutton.serial, ev->xany.window, ev->xany.window);

	/* serial is in all, but xany has not been updated to include it */
}
