#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: Move.c,v 5.12 89/11/22 15:11:40 glennw Exp $";
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
 *	  Western Software Lab. Port to X11
 */

#include "awm.h"

Boolean Move(window, mask, button, x, y)
Window window;				/* Event window. */
/*ARGSUSED*/
int mask;				/* Button/key mask. */
int button;				/* Button event detail. */
int x, y;				/* Event mouse position. */
{
     register int prev_x;		/* Previous event window X location. */
     register int prev_y;		/* Previous event window Y location. */
     XWindowAttributes window_info;	/* Event window information. */
     XWindowAttributes cliIconInfo;
     int client_x;			/* Client icon window x position */
     int icon_x;			/* icon name window x position */
     int inwy;				/* Icon name window y position */
     XWindowChanges wc;
     int deltax, deltay;		/* to compute ConfigureNotify */
     int cur_x;				/* Current event window X location. */
     int cur_y;				/* Current event window Y location. */
     int root_x;				/* Root window X location. */
     int root_y;				/* Root window Y location. */
     int ulx, uly;			/* Event window upper left X and Y. */
     int lrx, lry;			/* Event window lower right X and Y. */
     int init_ulx, init_uly;		/* Init window upper left X and Y. */
     int init_lrx, init_lry;		/* Init window lower right X and Y. */
     int overall_width, overall_height; /* includes border */
     int num_vectors;			/* Number of vectors in box. */
     int ptrmask;			/* state of ptr when queried */
     Window sub_window;			/* Query mouse event sub-window. */
     Window root;			/* Query mouse event root. */
     AwmInfoPtr awi;
     XEvent button_event;		/* Button event packet. */
     XSegment box[MAX_BOX_VECTORS];	/* Box vertex buffer. */
     XSegment zap[MAX_ZAP_VECTORS];	/* Zap effect verted buffer. */
     
     Entry("Move")
	  
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
         window = (awi->frame) ? awi->frame : awi->client;
         /* make sure we draw boxes based on frame if present */
     }

     /*
      * Change the cursor.
      */
     grab_pointer();
     /*
      * Clear the vector buffers.
      */
     bzero(box, sizeof(box));
     if (Zap)
	  bzero(zap, sizeof(zap));
     
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
     init_ulx = ulx = window_info.x;
     init_uly = uly = window_info.y;
     overall_width = window_info.width + (window_info.border_width << 1);
     overall_height = window_info.height + (window_info.border_width << 1);
     init_lrx = lrx = window_info.x + overall_width - 1;
     init_lry = lry = window_info.y + overall_height - 1;
     
     /*
      * Store the box.
      */
     if (IsIcon(window, NULL)) {
          num_vectors = StoreBox(box, ulx, uly, lrx, lry);
     }
     else {
          num_vectors = StoreTitleBox(box, ulx, uly, lrx, lry, awi);
     }
     
     /*
      * Initialize the previous location variables.
      */
     prev_x = x;
     prev_y = y;
     
     /*
      * Freeze the server, if requested by the user.
      * This results in a solid box instead of a flickering one.
      */
     if (Freeze)
	  XGrabServer(dpy);
     Snatched = True;
     
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
      * Now draw the box.
      */
     
     DrawBox();
     Frozen = window;
     
     /*
      * Main loop.
      */
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
	       
	       if ((button_event.type == ButtonRelease) &&
		   (((XButtonReleasedEvent *)&button_event)->button == button)) {
		    
		    /*
		     * The button was released, so move the window.
		     */
		    
		    if (Wall) {
			 if (ulx < 0)
			     ulx = 0;
			 if (uly < 0)
			     uly = 0;
			 if (ulx > (ScreenWidth - overall_width))
			     ulx = (ScreenWidth - overall_width);
			 if (uly > (ScreenHeight - overall_height))
			     uly = (ScreenHeight - overall_height);
		         lrx = ulx + overall_width - 1;
			 lry = uly + overall_height - 1;
		    }
#ifdef titan /* 5x4 alignment */
		    ulx += (ulx % 5);
		    uly += (uly % 4);
#endif /* titan */
		    if (Zap) {
			 num_vectors = StoreZap(zap,
						init_ulx, init_uly,
						init_lrx, init_lry,
						ulx, uly,
						lrx, lry);
			 DrawZap();
			 DrawZap();
		    }
		    if ((window == awi->icon) && (awi->client_icon)) {
			/* UL position is overall bbox.  Center both windows on
			   overall bbox, top/bottom borders touch */
			inwy = uly + cliIconInfo.height + 
			       (cliIconInfo.border_width << 1);
			icon_x = ulx;
			AdjustIconX(&client_x, &icon_x, 
				    (unsigned int) cliIconInfo.width, 
				    (unsigned int) window_info.width);
			XMoveWindow(dpy, awi->client_icon, client_x, uly);
			XMoveWindow(dpy, window, icon_x, inwy);
		    }
		    else
			XMoveWindow(dpy, window, ulx, uly);
		    Snatched = False;
		    ungrab_pointer();
		    if (window == awi->icon)
			/* reset client's IconPosition hint for restart */
			ResetIconPositionHint(awi, ulx, uly);
		    else if (window == awi->frame) {
			/* must inform client of window move.  Report 
			effective position as if client-requested border
			was in effect. */
			FrameToClientBorder(awi, &deltax, &deltay);
			wc.x = ulx + deltax;
			wc.y = uly + deltay;
			XGetWindowAttributes(dpy, awi->client, &window_info);
			/* XXX Should cache in awi */
			wc.width = window_info.width;
			wc.height = window_info.height;
			SendSyntheticConfigureNotify(awi, &wc);
		    }
		    Leave(TRUE)
	       }
	       else {
		    /*
		     * Some other button event occured, this aborts the
		     * current operation.
		     */
		    
		    ResetCursor(button);
		    Snatched = False;
		    ungrab_pointer();
		    Leave(TRUE)
	       }
	  }
	  
	  /*
	   * Take care of all the little things that have changed.
	   */
	  XQueryPointer(dpy, 
			RootWindow(dpy, scr), &root,  &sub_window,
			&root_x, &root_y, &cur_x, &cur_y, &ptrmask);
	  if ((cur_x != prev_x) || (cur_y != prev_y)) {
	       
	       /*
		* If we've frozen the server, then erase the old box first!
		*/
	       if (Freeze)
		    DrawBox();
	       
	       /*
		* Box position has changed.
		*/
	       ulx += cur_x - prev_x;
	       uly += cur_y - prev_y;
	       lrx += cur_x - prev_x;
	       lry += cur_y - prev_y;
	       
	       /*
		* Box needs to be restored.
		*/
	       if (IsIcon(window, NULL)) {
		   num_vectors = StoreBox(box, ulx, uly, lrx, lry);
	       }
	       else {
		   num_vectors = StoreTitleBox(box, ulx, uly, lrx, lry, awi);
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
	   * Save old box position.
	   */
	  prev_x = cur_x;
	  prev_y = cur_y;
	  
	  /*
	   * If server is not frozen, then draw the "flicker" box.
	   */
	  if (!Freeze) {
	       /* if not freeze, flash up box whether changed or not */
	       DrawBox();
	       XSync(dpy, FALSE); /* try to make duty cycle of box non-zero */
	       DrawBox();
	  }
     }
}
