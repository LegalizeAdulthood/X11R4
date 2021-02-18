#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: Raise.c,v 5.14 89/11/22 15:12:20 glennw Exp $";
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
 * 001 -- R. Kittell, DEC Storage A/D May 19, 1986
 *  Optionally warp mouse to upper right corner of window after raise.
 * 002 -- Loretta Guarino Reid, DEC Ultrix Engineering Group
 *  Western Software Lab. Convert to X11.
 * 003 -- Jordan Hubbard, Ardent Computer
 *  Changed for awm.
 */

#include "awm.h"

Boolean Raise(window, mask, button, x, y)
Window window;                          /* Event window. */
int mask;                               /* Button/key mask. */
int button;           	                /* Button event detail. */
int x, y;                               /* Event mouse position. */

{
     XWindowAttributes winfo;			/* Window details for warp */
     AwmInfoPtr awi;
     int status;
#ifdef WARP_UPPER_RIGHT	       
     int vpoint;
#endif
     
     Entry("Raise")
     
     /*
      * If the window is not the root window, raise the window and return.
      */

     awi = GetAwmInfo(window);
     if (!awi)
	  Leave(FALSE)
     if (window != RootWindow(dpy, scr)) {
	 if (IsIcon(window, NULL)) {
	      XRaiseWindow(dpy, awi->icon);
	      if (awi->client_icon)
		  XRaiseWindow(dpy, awi->client_icon);
	      Leave(FALSE)
	  }
	  window = awi->frame ? awi->frame : awi->client;
	  XRaiseWindow(dpy, window);
	  /*
	   * Optionally warp the mouse to the UR corner (center) of the window.
	   */
	  if (WarpOnRaise) {
	       status = XGetWindowAttributes (dpy, window, &winfo);
	       if (status == FAILURE)
		    Leave(FALSE)

#ifdef WARP_UPPER_RIGHT	       
	       vpoint = 10;
	       if (awi->frame) {
		   if (awi->attrs & AT_TITLE)
		       vpoint += titlePositionExcess;
		   if (awi->attrs & AT_BORDER)
		       vpoint += borderPositionExcess;
	       }
	       status = XWarpPointer (dpy, None, window, 
				      0, 0, 0, 0,
				      winfo.width >= 7 ? winfo.width - 7 :
				      winfo.width / 2,
				      winfo.height >= vpoint ? vpoint :
				      winfo.height / 2);
#else
	       status = XWarpPointer (dpy, None, window, 
				      0, 0, 0, 0,
				      winfo.width / 2, winfo.height / 2);
#endif /* WARP_UPPER_RIGHT */
       if (status == FAILURE)
		    Leave(FALSE)
	  }
     }
     Leave(FALSE)
}

Boolean RaiseAndLower(window, mask, button, x, y)
Window window;                          /* Event window. */
int mask;                               /* Button/key mask. */
int button;           	                /* Button event detail. */
int x, y;                               /* Event mouse position. */

{
     AwmInfoPtr awi;
     /* next three variables are for XQueryWindow */
     Window junk;
     Window *kiddies;
     unsigned int nkids;
     int i;
     int win_x, win_y;
     int right, bottom, other_right, other_bottom;
     unsigned int width, height, border_width, depth_junk;
     XWindowAttributes attr;

     Entry("RaiseAndLower")
     
     /*
      * If the window is not the root window,
      *	if the window/icon is already fully visible, lower it, 
      * else if it is obscured by another window, raise it,
      * else lower the window/icon and return.
      *
      */

     awi = GetAwmInfo(window);
     if (!awi)
	 Leave(FALSE)
     if (window != RootWindow(dpy, scr)) {
	 if (!IsIcon(window, NULL)) {
	     window = awi->frame ? awi->frame : awi->client;
	 }
	 if (IsIcon(window, NULL) && 
	      (awi->icon_visibility == VisibilityUnobscured) && 
	      (awi->client_icon_visibility == VisibilityUnobscured)) {
	      XLowerWindow(dpy, awi->icon);
	      if (awi->client_icon)
		  XLowerWindow(dpy, awi->client_icon);
	 }
	 else if (!IsIcon(window, NULL) && 
	          (awi->visibility == VisibilityUnobscured))
	     XLowerWindow(dpy, window);
	 else {
	     /* for speed, catch case of window fully on-screen first - if
		on-screen, it was obscured by another window. */
	     /*	OK, OK, for icons, I really need to check overall geometry,
		but I'm tired.  User will just have to click on other win. */
	     XGetGeometry(dpy, window, &junk, &win_x, &win_y, &width, &height,
			 &border_width, &depth_junk);
	     right  = win_x + (border_width * 2) + width - 1;
	     bottom = win_y + (border_width * 2) + height - 1;
	     if ((win_x >= 0) && (win_y >= 0) && (bottom < DisplayHeight(dpy, scr)) &&
					 (right < DisplayWidth(dpy, scr))) {
		 Raise(window, mask, button, x, y);
		 Leave(FALSE)
	     }
             if (XQueryTree(dpy, DefaultRootWindow(dpy), &junk, &junk, &kiddies,
			    &nkids) && kiddies) {
		/* if no windows on screen, or this win is not in the list,
		   something is wierd */
		 for (i = nkids - 1; kiddies[i] != window; i--) {
		     XGetWindowAttributes(dpy, window, &attr);
		     other_right  = attr.x + (attr.border_width * 2) + attr.width - 1;
		     other_bottom = attr.y + (attr.border_width * 2) + attr.height- 1;
		     if ((((right >= attr.x) && (right <= other_right)) ||
		          ((win_x >= attr.x) && (win_x <= other_right))) &&
		         (((bottom >= attr.y) && (bottom <= other_bottom)) ||
		          ((win_y >= attr.y) && (win_y <= other_bottom))) &&
			 attr.class == InputOutput &&
			 attr.map_state == IsViewable) {
			 /* we have an obscuration */
			 Raise(window, mask, button, x, y);
			 XFree(kiddies);
			 Leave(FALSE)
		     }
		 }
		 XFree(kiddies);
	     }
	     XLowerWindow(dpy, window);
	 }
     }
     Leave(FALSE)
}
