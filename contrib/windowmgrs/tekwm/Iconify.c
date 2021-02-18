#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: Iconify.c,v 5.19 89/11/22 15:10:54 glennw Exp $";
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
 * 001 -- R. Kittell, DEC Storage A/D May 20, 1986
 *  Add optional warp of mouse to the upper right corner on de-iconify,
 *  and to the icon center on iconify.
 * 002 -- Loretta Guarino Reid, DEC Ultrix Engineering Group,
 *  Western Software Lab. Port to X11.
 * 003 -- Jordan Hubbard, Ardent Computer.
 *  Many mods to cope with context manager, titled windows. Almost a total
 *  rewrite.
 */

#include "awm.h"
#include "X11/Xutil.h"

ResetIconPositionHint(awi, x, y)
AwmInfoPtr awi;
int x, y;
{
     XWMHints *wmhints;

    /* reset client's IconPosition hint for restart */
    if (!(wmhints = XGetWMHints(dpy, awi->client))) {
	wmhints = XAllocWMHints();
	wmhints->flags = 0;
    }
    wmhints->flags |= IconPositionHint;
    wmhints->icon_x = x;
    wmhints->icon_y = y;
    XSetWMHints(dpy, awi->client, wmhints);
    XFree(wmhints);
    Leave_void
}

Boolean Iconify(window, mask, button, x, y)
Window window;                          /* Event window. */
int mask;                               /* Button/key mask. */
int button;                             /* Button event detail. */
int x, y;                               /* Event mouse position. */
{
     XWindowAttributes fromInfo;	/* info on "from" window */
     XWindowAttributes toInfo;		/* info on "to" window */
     XWindowAttributes tmpInfo, *InfoPtr;
     int num_vectors;			/* Number of vectors in zap buffer. */
     int mmask;				/* Mouse state */
     Window root;			/* Mouse root window. */
     Window from, to;			/* from -> to windows */
     XSegment zap[MAX_ZAP_VECTORS];	/* Zap effect vertex buffer. */
     Boolean dozapwarp;			/* local Zap */
     AwmInfoPtr awi;
     Boolean tossed_button_up = FALSE;
     Boolean from_wm;
     Boolean placed = TRUE;	/* switch to newiconify if not yet placed */

     Entry("Iconify")

     /*
      * Do not try to iconify the root window.
      */
     if (window == RootWindow(dpy, scr))
	  Leave(FALSE)
     /*
      * The original idea of zap and warp has one flaw. If a window wants
      * to be created iconic, it should just appear that way without any fuss.
      * CheckMap() and main call us with (win, 0, 0, 0, 0) when this is the
      * case, so we can special case this to turn off Zap and warp temporarily.
      * Since we don't want to mess with the global "Zap", we use
      * "dozapwarp" instead. 
      */
     dozapwarp = mask || button || x || y;
     /*
      * Clear the vector buffer.
      */
     if (dozapwarp && Zap)
	  bzero(zap, sizeof(zap));
     
     if (button == -1)
	 /* invoked from menu - button is not down, no release to absorb */
	 tossed_button_up = TRUE;

     /*
      * Figure out which direction we're going in (icon->window or vica-versa)
      */
     awi = GetAwmInfo(window);
     if (!awi)
          Leave(tossed_button_up)

     if (awi->state & ST_ICON) {
          from = awi->icon;
          to = (awi->frame) ? awi->frame : awi->client;
	  if (!(awi->state & ST_PLACED)) {
	       PlaceWindow(to, None, &from_wm);
	       /* after we map the window, Decorate works off fake 
	          NorthWest gravity */
	       /* If PlaceWindow got the position from the user, it absorbed
		  the button up, so no need to do so */
	       tossed_button_up = from_wm;
	  }
     }
     else if (awi->state & ST_WINDOW) {
          from = (awi->frame) ? awi->frame : awi->client;
          to = (awi->icon) ? awi->icon :  MakeIcon(window, TRUE, &placed);
	  if (!placed && dozapwarp) {

	      /* this also should not be done on startup */
	      NewIconify(window, mask, button, x, y);
	      /* NewIconify absorbs Button up */
	      Leave(TRUE)
	  }
     }
     else {
          printf("Iconify: Window %x has unknown state '%x'\n",
		 awi->client, awi->state);
	  Leave(tossed_button_up)
     }

     status = XGetWindowAttributes(dpy, from, &fromInfo);
     if (status == FAILURE)
	  Leave(tossed_button_up)
     status = XGetWindowAttributes(dpy, to, &toInfo);
     if (status == FAILURE)
	  Leave(tossed_button_up)

     if (awi->client_icon) {
	 /* compute overall dimensions */
	 if (to == awi->icon)
	     InfoPtr = &toInfo;
	 else
	     InfoPtr = &fromInfo;
	 AdjustIconGeom(awi, InfoPtr, &tmpInfo);
    }
	
     /*
      * Store the zap vector buffer.
      */
     if (dozapwarp && Zap) {
	  num_vectors =
	       StoreZap(zap,
			fromInfo.x - 1,
			fromInfo.y - 1,
			fromInfo.x + fromInfo.width +
			(fromInfo.border_width << 1),
			fromInfo.y + fromInfo.height +
			(fromInfo.border_width << 1),
			toInfo.x - 1,
			toInfo.y - 1,
			toInfo.x + toInfo.width +
			(toInfo.border_width << 1),
			toInfo.y + toInfo.height +
			(toInfo.border_width << 1));
     }
     if (awi->state & ST_ICON) {
	  if (!awi->frame)
	       XRemoveFromSaveSet(dpy, awi->client);
	  awi->state ^= ST_ICON;
	  awi->state |= ST_WINDOW;
     }
     else if (awi->state & ST_WINDOW) {
          XAddToSaveSet(dpy, awi->client);
	  awi->state ^= ST_WINDOW;
	  awi->state |= ST_ICON;
     }
     else {
	  (void) sprintf(error_msg, "Window state for window %x got munged!",
			 awi->client);
	  Warning(error_msg);
     }
     /*
      * Map the target.
      */
     if (to == awi->icon) {
	 /* don't raise icons (will be created on top) */
	 XMapWindow(dpy, to);
	 if (awi->client_icon)
	     XMapWindow(dpy, awi->client_icon);
	 /* reset client's IconPosition hint for restart */
	 ResetIconPositionHint(awi, toInfo.x, toInfo.y);
     }
     else {
	 XMapRaised(dpy, to);
     }
     /* 
      * Map the client window so client sees it.
      */
     if (to == awi->frame)
	 /* Raising reparented client is a noop. */
	 XMapWindow(dpy, awi->client);

     if (dozapwarp && Zap) {
	  /*
	   * Draw the zap lines.
	   */
	  DrawZap();
     }
     /*
      * Unmap the "from" window.
      */
     XUnmapWindow(dpy, from);
     if ((from == awi->icon) && awi->client_icon)
	 XUnmapWindow(dpy, awi->client_icon);
     /* 
      * Unmap the client window so client sees it
      */
     if (from == awi->frame)
	 XUnmapWindow(dpy, awi->client);
     UpdateWMState(awi);
     /*
      * Optionally warp the mouse to the window
      */

     if (WarpOnDeIconify && awi->state & ST_WINDOW && dozapwarp) {

#if defined(WARP_UPPER_CENTER)
          /* Warp to center of titlebar. If no titlebar then
           * warp to center top of window.
           */
	  int y = 1;
	  if (awi->frame) { /* compensate for title */
	       XWindowAttributes xwa;
	       XGetWindowAttributes(dpy, awi->title, &xwa);
	       y += xwa.height / 2;
	  }
	  status = XWarpPointer (dpy, None, to,
				 0, 0, 0, 0,
				 toInfo.width / 2,
				 y);

#elif defined(WARP_UPPER_RIGHT)
	  /* Warp to upper right corner of window. Stays in
           * client portion of window, out of titlebar.
           */
	  int y;
	  y = (toInfo.height >= 10) ? 10 : toInfo.height / 2;
	  if (awi->frame) { /* compensate for title */
	       XWindowAttributes xwa;
	       XGetWindowAttributes(dpy, awi->title, &xwa);
	       y += xwa.height + 2;
	  }
	  status = XWarpPointer (dpy, None, to,
				 0, 0, 0, 0,
				 toInfo.width >= 7 ?
				 toInfo.width - 7 : toInfo.width / 2,
				 y);
#else
	  /* Warp to center of window. */
	  status = XWarpPointer (dpy, None, to, 
				 0, 0, 0, 0,
				 toInfo.width / 2, toInfo.height / 2 );
#endif /* WARP_UPPER_CENTER */
     }

     if (WarpOnIconify && awi->state & ST_ICON && dozapwarp)
	  status = XWarpPointer (dpy, None, to, 
				 0, 0, 0, 0,
				 toInfo.width / 2, toInfo.height / 2);
     if (awi->state & ST_ICON)
	 XSync(dpy, FALSE);
         /* Must Sync up to insure that the client window UnmapNotifys we
	    generated are ignored in GetButton. */
/*     fprintf(stderr, "leaving Iconify: tossed_button_up = %d\n", tossed_button_up);*/
     Leave(tossed_button_up)
}
