#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: MoveOpaque.c,v 5.11 89/11/22 15:11:46 glennw Exp $";
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
 * Western Software Lab. Convert to X11.
 * 002 -- Jordan Hubbard, Ardent Computer
 *  Changed to work with awm.
 */

#include "awm.h"

Boolean MoveOpaque(window, mask, button, x, y)
Window window;				/* Event window. */
/*ARGSUSED*/
int mask;				/* Button/key mask. */
int button;				/* Button event detail. */
int x, y;				/* Event mouse position. */
{
     int prev_x, prev_y;		/* Previous mouse location. */
     int rbound, dbound;		/* potential right/down boundries */
     int cur_x, cur_y;			/* Current mouse location. */
     int win_x, win_y;			/* Current window location. */
     int root_x;			/* Root window X location. */
     int root_y;			/* Root window Y location. */
     int ptrmask;			/* state of ptr when queried */
     XWindowAttributes window_info;	/* Event window information. */
     XWindowAttributes cliIconInfo;
     AwmInfoPtr awi;
     int client_x;			/* Client icon window x position */
     int icon_x;			/* icon name window x position */
     int inwy;				/* Icon name window y position */
     XWindowChanges wc;
     int deltax, deltay;		/* to compute ConfigureNotify */
     Window sub_window;			/* Query mouse event sub-window. */
     Window root;			/* Query mouse event root. */
     XEvent button_event;		/* Button event packet. */
     Entry("MoveOpaque")

     /*
      * Do not try to move the root window.
      */
     if (window == RootWindow(dpy, scr))
	  Leave(FALSE)
     awi = GetAwmInfo(window);
     if (!awi)
	 Leave(FALSE)

     if (IsIcon(window, NULL))
	 /* work off name icon */
	 window = awi->icon;
     else {
	 if (awi->attrs & AT_POSITION)
	     /* moves forbidden for this window */
	     Leave(FALSE)
     }
	    
     /*
      * Change the cursor.
      */
     grab_pointer();
     
     /*
      * Gather info on the event window.
      */
     status = XGetWindowAttributes(dpy, window, &window_info);
     if (status == FAILURE)
	  Leave(FALSE)

    if (window == awi->icon) {
	 /* compute overall dimensions */
	 AdjustIconGeom(awi, &window_info, &cliIconInfo);
    }

     /*
      * Initialize movement variables.
      */
     prev_x = cur_x = x;
     prev_y = cur_y = y;
     win_x = window_info.x;
     win_y = window_info.y;
     rbound = ScreenWidth - (window_info.width + window_info.border_width);
     dbound = ScreenHeight - (window_info.height + window_info.border_width);

     /*
      * Main loop.
      */
     while (TRUE) {
	  
	  /*
	   * Check to see if we have a change in mouse button status.
	   * This is how we get out of this "while" loop.
	   */
	  if (XCheckMaskEvent(dpy,
			      ButtonPressMask | ButtonReleaseMask,
			      &button_event)) {
	       
		if (EventDebug)
		    print_event_info("MoveOpaque", &button_event);

	       if ((button_event.type == ButtonPress) && (button == -1)) {
		    button = ((XButtonPressedEvent *)&button_event)->button;
		    /* legal Press due to preselection from menu; stash it */
		    continue;
	        }

	       /*
		* If the button event was something other than the
		* release of the original button pressed, then move the
		* window back to where it was originally.
		*/
	       if ((button_event.type != ButtonRelease) ||
		   (((XButtonReleasedEvent *)&button_event)->button
		    != button)) {
		    ResetCursor(button);
		    if ((window == awi->icon) && (awi->client_icon)) {
			/* UL position is overall bbox.  Center both windows on
			   overall bbox, top/bottom borders touch */
			inwy = window_info.y + cliIconInfo.height + 
			       (cliIconInfo.border_width << 1);
			icon_x = window_info.x;
			AdjustIconX(&client_x, &icon_x, 
				    (unsigned int) cliIconInfo.width, 
				    (unsigned int) window_info.width);
			XMoveWindow(dpy, awi->client_icon,
				    client_x, window_info.y);
			XMoveWindow(dpy, window, icon_x, inwy);
			/* make sure they move together */
			XFlush(dpy);
		    }
		    else
		       XMoveWindow(dpy, window, window_info.x, window_info.y);
		    win_x = window_info.x;
		    win_y = window_info.y;
		    ungrab_pointer();
		    /* no need to report move to client */
		    Leave(FALSE)
	       }
	       ungrab_pointer();
	       if (window == awi->icon)
		   /* reset client's IconPosition hint for restart */
		   ResetIconPositionHint(awi, win_x, win_y);
	       else if (window == awi->frame) {
		   /* must inform client of final window move.  Report effective
		   position as if client-requested border was in effect. */
		   FrameToClientBorder(awi, &deltax, &deltay);
		   wc.x = win_x + deltax;
		   wc.y = win_y + deltay;
		   XGetWindowAttributes(dpy, awi->client, &window_info);
		   /* XXX Should cache in awi */
		   wc.width = window_info.width;
		   wc.height = window_info.height;
		   SendSyntheticConfigureNotify(awi, &wc);
	       }
	       Leave(TRUE)
	  }
	  
	  /*
	   * Take care of all the little things that have changed; 
	   * i.e., move the window, if necessary.
	   */
	  XQueryPointer(dpy, RootWindow(dpy, scr), 
			&root, &sub_window, &root_x, &root_y, &cur_x, &cur_y, 
			&ptrmask);
	  if ((cur_x != prev_x) || (cur_y != prev_y)) {
	       win_x += (cur_x - prev_x);
	       win_y += (cur_y - prev_y);
#ifdef titan /* align to 5x4 */
	       win_x += (win_x % 5);
	       win_y += (win_y % 4);
#endif /* titan */
	       if (Wall) {
		    if (win_x < 0)
			 win_x = 0;
		    else if (win_x > rbound)
			 win_x = rbound;
		    if (win_y < 0)
			 win_y = 0;
		    else if (win_y > dbound)
			 win_y = dbound;
 	       }
	       if ((window == awi->icon) && (awi->client_icon)) {
		   /* UL position is overall bbox.  Center both windows on
		      overall bbox, top/bottom borders touch */
		    inwy = win_y + cliIconInfo.height + 
			   (cliIconInfo.border_width << 1);
		    icon_x = win_x;
		    AdjustIconX(&client_x, &icon_x, 
				(unsigned int) cliIconInfo.width, 
				(unsigned int) window_info.width);
		    XMoveWindow(dpy, awi->client_icon, client_x, win_y);
		    XMoveWindow(dpy, window, icon_x, inwy);
		    /* make sure they move together */
		    XFlush(dpy);
	       }
	       else
		    XMoveWindow(dpy, window, win_x, win_y);
	       prev_x = cur_x;
	       prev_y = cur_y;
	  }
     }
}
