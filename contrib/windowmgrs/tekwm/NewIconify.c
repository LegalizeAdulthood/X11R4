#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: NewIconify.c,v 5.18 89/11/22 15:11:53 glennw Exp $";
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
 *  Western Software Lab. Port to X11.
 */

#include "awm.h"

Boolean NewIconify(window, mask, button, x, y)
Window window;                          /* Event window. */
int mask;                               /* Button/key mask. */
int button;                             /* Button event detail. */
int x, y;                               /* Event mouse position. */
{
     XWindowAttributes fromInfo;	/* from window info. */
     XWindowAttributes toInfo;		/* to window info. */
     XWindowAttributes cliIconInfo, *InfoPtr;
     int client_x;			/* Client icon window x position */
     int icon_x;			/* icon name window x position */
     int inwy;				/* Icon name window y position */
     XWindowChanges wc;
     int deltax, deltay;		/* to compute ConfigureNotify */
     Window from, to;
     char *name;			/* Event window name. */
     int mse_x, mse_y;			/* Mouse X and Y coordinates. */
     int new_x, new_y;			/* To U. L. X and Y coordinates. */
     int new_w, new_h;			/* To width and height. */
     int bdw;				/* border width */
     int prev_x;			/* Previous event window X location. */
     int prev_y;			/* Previous event window Y location. */
     int cur_x;				/* Current event window X location. */
     int cur_y;				/* Current event window Y location. */
     int root_x;			/* Root window X location. */
     int root_y;			/* Root window Y location. */
     int ulx, uly;			/* Event window upper left X and Y. */
     int lrx, lry;			/* Event window lower right X and Y. */
     int num_vectors;			/* Number of vectors in box. */
     int status;			/* Routine call return status. */
     int ptrmask;			/* pointer query state. */
     Window root;			/* Mouse root window. */
     Window icon;			/* Icon window. */
     Window sub_win;			/* Mouse position sub-window. */
     XEvent button_event;		/* Button event packet. */
     XSegment box[MAX_BOX_VECTORS];	/* Box vertex buffer. */
     XSegment zap[MAX_ZAP_VECTORS];	/* Zap effect vertex buffer. */
     Boolean iconifying;		/* Are we iconifying? */
     Boolean placed;			/* ignored - we always place icon */
     AwmInfoPtr awi;
     Window junkw;			/* scratch */
/*     int button_cnt = 0;		/* # of button events left in queue */
     
     Entry("NewIconify")
	  
     /*
      * Do not lower or iconify the root window.
      */
     if (window == RootWindow(dpy, scr))
	  Leave(FALSE)
     
     /*
      * Get info on the event window.
      */
     awi = GetAwmInfo(window);
     if (!awi)
	  Leave(FALSE)
     /*
      * Are we iconifying or de-iconifying?
      */
     if (awi->state & ST_ICON) {
	  if ((awi->attrs & AT_POSITION) ||
	      /* moves forbidden for this window - do Iconify instead.
		 However, if not positioned yet, do so. */
	      !(awi->state & ST_PLACED)) {
	       /* must place window instead of just positioning - 
		  skip the re-position function of NewIconify, as we
		  must go directly to initial placement of window. */
	       Leave(Iconify(window, mask, button, x, y))
	  }
          from = awi->icon;
          to = (awi->frame) ? awi->frame : awi->client;
     }
     else if (awi->state & ST_WINDOW) {
          from = (awi->frame) ? awi->frame : awi->client;
          to = (awi->icon) ? awi->icon : MakeIcon(window, FALSE, &placed);
     }
     else {
	  (void) sprintf(error_msg, "NewIconify: Window %x has unknown state '%x'",
		 awi->client, awi->state);
	  Warning(error_msg);
	  Leave(FALSE)
     }
     /*
      * Change the cursor to the icon cursor.
      */
     grab_pointer();
     
     /*
      * Clear the vector buffers.
      */
     bzero(box, sizeof(box));
     if (Zap)
	  bzero(zap, sizeof(zap));
     
     status = XGetWindowAttributes(dpy, from, &fromInfo);
     if (status == FAILURE)
          Leave(FALSE)

     status = XGetWindowAttributes(dpy, to, &toInfo);
     if (status == FAILURE)
          Leave(FALSE)

     if (awi->client_icon) {
	 /* compute overall dimensions */
	 if (to == awi->icon)
	     InfoPtr = &toInfo;
	 else
	     InfoPtr = &fromInfo;
	 AdjustIconGeom(awi, InfoPtr, &cliIconInfo);
    }

     /*
      * Initialize the movement variables.
      */
     ulx = x - (toInfo.width >> 1) - toInfo.border_width;
     uly = y - (toInfo.height >> 1) - toInfo.border_width;
     lrx = x + (toInfo.width >> 1) + toInfo.border_width - 1;
     lry = y + (toInfo.height >> 1) + toInfo.border_width - 1;
     prev_x = x;
     prev_y = y;

     
     /*
      * Store the box.
      */
     if (awi->frame && awi->state & ST_ICON) {
          num_vectors = StoreTitleBox(box, ulx, uly, lrx, lry, awi);
     }
     else {
          num_vectors = StoreBox(box, ulx, uly, lrx, lry);
     }
     
     /*
      * Freeze the server, if requested by the user.
      * This results in a solid box instead of a flickering one.
      */
     if (Freeze)
	  XGrabServer(dpy);
     
     /*
      * Process any outstanding events before drawing the box.
      * Must process any expose events (e.g. for titlebar due to menu 
      * unmap or prior window op)
      * that are generated before now, but save any button events.
      * This code might take a while if we are getting motion events...
      */
     XSync(dpy, FALSE);
/*     while ((QLength(dpy) > button_cnt) && */
	    /* Note - Xlib really needs an AllEventsMask macro */
     while (XCheckMaskEvent(dpy, ~(ButtonPressMask | ButtonReleaseMask),
			    &button_event)) {
	 XPutBackEvent(dpy, &button_event);
	 GetButton(&button_event);
     }
     
     /*
      * Draw the box.
      */
     DrawBox();
     if (Freeze)
	  Frozen = window;
     
     /*
      * We spin our wheels here looking for mouse movement or a change
      * in the status of the buttons.
      */
     Snatched = TRUE;
     while (TRUE) {
	  
	  /*
	   * Check to see if we have a change in mouse button status.
	   * This is how we get out of this "while" loop.
	   */
	  if (XPending(dpy) && !ProcessRequests(box, num_vectors) &&
	      GetButton(&button_event)) {
	       /*
		* Process the pending events, this sequence is the only
		* way out of the loop and the routine.
		*/
	       
	       if ((button_event.type == ButtonPress) && (button == -1)) {
		   button = ((XButtonPressedEvent *)&button_event)->button;
		    /* legal Press due to preselection from menu; stash it */
		   continue;
	       }
	       
	       if ((button_event.type != ButtonPress) && 
		   (button_event.type != ButtonRelease)) {
		    continue; /* spurious menu event... */
	       }
	       
	       /*
		* If we froze the server, then erase the last lines drawn.
		*/
	       if (Freeze) {
		    DrawBox();
		    Frozen = (Window)0;
		    XUngrabServer(dpy);
	       }
    	       break;
	  }
	  else {
	       /*
		* Continue to track the mouse until we get a change
		* in button status.
		*/
	       XQueryPointer(dpy, RootWindow(dpy, scr), 
			     &root, &sub_win, &root_x, &root_y, &cur_x,
			     &cur_y, &ptrmask);
	       
	       /*
		* If the mouse has moved, then make sure the box follows it.
		*/
	       if ((cur_x != prev_x) || (cur_y != prev_y)) {
		    
		    /*
		     * If we've frozen the server, then erase the old box first!
		     */
		    if (Freeze)
			 DrawBox();
		    
		    /*
		     * Set the new box position.
		     */
		    ulx += cur_x - prev_x;
		    uly += cur_y - prev_y;
		    lrx += cur_x - prev_x;
		    lry += cur_y - prev_y;
		    
		    /*
		     * Calculate the vectors for the new box.
		     */
		    
		    if (awi->frame && awi->state & ST_ICON) {
			num_vectors = StoreTitleBox(box, ulx, uly, lrx, lry, awi);
		    }
		    else {
		        num_vectors = StoreBox(box, ulx, uly, lrx, lry);
		    }
		    
		    /*
		     * Draw the new box.
		     */
		    if (Freeze) {
			DrawBox();
			XFlush(dpy);
		    }
	       }
	       
	       /*
		* Save the old box position.
		*/
	       prev_x = cur_x;
	       prev_y = cur_y;
	       
	       /*
		* If server is not frozen, then draw the "flicker" box.
		*/
	       if (!Freeze) {
		    DrawBox();
		    XSync(dpy, FALSE);
		    /* try to make duty cycle of flashing box non-zero */
		    DrawBox();
	       }
	  }
     }
     Snatched = FALSE;
     /*
      * If the button is not a button release of the same button pressed,
      * then abort the operation.
      */
     if ((button_event.type != ButtonRelease) ||
	 (((XButtonReleasedEvent *)&button_event)->button != button)) {
	  ResetCursor(button);
	  ungrab_pointer();
	  Leave(TRUE)
     }
     
     /*
      * If we are here we have committed to iconifying/deiconifying.
      */
     
     /*
      * Determine the coordinates of the icon or window;
      * handle events not on root window (Grab with owner_events == TRUE).
      * Normalize the window or icon coordinates if the user so desires.
      */
     if (! XTranslateCoordinates(dpy, 
				 button_event.xany.window, 
				 RootWindow(dpy, scr),
				 button_event.xbutton.x, 
				 button_event.xbutton.y, 
				 &mse_x, &mse_y,
				 &junkw)) {
	 mse_x = button_event.xbutton.x;
	 mse_y = button_event.xbutton.y;
     }
     /* can't really return zero, we are not doing multiple screens */
     new_w = toInfo.width;
     new_h = toInfo.height;
     bdw = toInfo.border_width;
     new_x = mse_x - (new_w >> 1) - bdw;
     new_y = mse_y - (new_h >> 1) - bdw;
     if ((NIcon && awi->state & ST_WINDOW) || (NWindow && awi->state & ST_ICON)) {
	  if (new_x < 0) new_x = 0;
	  if (new_y < 0) new_y = 0;
	  if ((new_x - 1 + new_w + (bdw << 1)) > ScreenWidth) {
	       new_x = ScreenWidth - new_w - (bdw << 1) + 1;
	  }
	  if ((new_y - 1 + new_h + (bdw << 1)) > ScreenHeight) {
	       new_y = ScreenHeight - new_h - (bdw << 1) + 1;
	  }
     }
     if (awi->state & ST_ICON) {
	  if (!awi->frame)
	       XRemoveFromSaveSet(dpy, awi->client);
	  awi->state ^= ST_ICON;
	  awi->state |= (ST_WINDOW);
     }
     else if (awi->state & ST_WINDOW) {
	  XAddToSaveSet(dpy, awi->client);
	  awi->state ^= ST_WINDOW;
	  awi->state |= ST_ICON;
     }
     else
	  Warning("NewIconify: Window state got trashed!");
     
     /*
      * Move the window into place.
      */
     /*
      * Map the icon/client window.  Raise the window, but not the icon.
      */
     if (to == awi->icon) {
	 if (awi->client_icon) {
	     icon_x = new_x;
	     inwy = new_y + cliIconInfo.height + 
			    (cliIconInfo.border_width << 1);
	     AdjustIconX(&client_x, &icon_x, (unsigned int) cliIconInfo.width, 
			 (unsigned int) toInfo.width);
	     XMoveWindow(dpy, awi->client_icon, client_x, new_y);
	     XMoveWindow(dpy, to, icon_x, inwy);
	     XMapWindow(dpy, awi->client_icon);
	 }
	 else
	     XMoveWindow(dpy, to, new_x, new_y);
	 /* reset client's IconPosition hint for restart */
	 XMapWindow(dpy, to);
	 ResetIconPositionHint(awi, new_x, new_y);
     }
     else {
	 XMoveWindow(dpy, to, new_x, new_y);
	 XMapRaised(dpy, to);
     }
     /* 
      * Map the client window so client sees it
      */
     if (to == awi->frame)
	 XMapWindow(dpy, awi->client);
     if (Zap) {
	  num_vectors = StoreZap(zap,
				 fromInfo.x,
				 fromInfo.y,
				 fromInfo.x + fromInfo.width
				 + (fromInfo.border_width << 1),
				 fromInfo.y + fromInfo.height
				 + (fromInfo.border_width << 1),
				 ulx, uly, lrx, lry);
	  DrawZap();
	  DrawZap();
     }
     /*
      * Unmap the event window.
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
     ungrab_pointer();
     if (awi->state & ST_ICON)
	 XSync(dpy, FALSE);
         /* Must Sync up to insure that the client window UnmapNotifys we
	    generated are ignored in GetButton. */
     if (to == awi->frame) {
	 /* must inform client of window move.  Report 
	 effective position as if client-requested border
	 was in effect. */
	 FrameToClientBorder(awi, &deltax, &deltay);
	 wc.x = new_x + deltax;
	 wc.y = new_y + deltay;
	 XGetWindowAttributes(dpy, awi->client, &toInfo);
	 /* XXX Should cache in awi */
	 wc.width = toInfo.width;
	 wc.height = toInfo.height;
	 SendSyntheticConfigureNotify(awi, &wc);
     }
     Leave(TRUE)
}
