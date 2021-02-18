#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: Resize.c,v 5.16 89/11/22 15:12:30 glennw Exp $";
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
 * 001 -- Loretta Guarino Reid, DEC Ultrix Engineering Group
 *        Convert to X11
 * 002 -- Jordan Hubbard, U.C. berkeley.
 *	Added alternate placement of resize window, code for title bar
 * 	support.
 */

#include "awm.h"
#include "X11/Xutil.h"

#define max(a,b) ( (a) > (b) ? (a) : (b) )
#define min(a,b) ( (a) > (b) ? (b) : (a) )
#define makemult(a, b) ((b==1) ? (a) : (((int)((a) / (b))) * (b)) )

Boolean Resize(window, mask, button, x0, y0)
Window window;                          /* Event window. */
int mask;                               /* Button/key mask. */
int button;                             /* Button event detail. */
int x0, y0;                             /* Event mouse position. */
{
     XWindowAttributes client_info;	/* client window info. */
     XWindowAttributes frame_info;	/* frame window info */
     int clientx1, clienty1;		/* client overall ul corner */
     int clientinsidex1, clientinsidey1;/* client ul corner inside border */
     int clientwidth, clientheight;	/* client overall size */
     float yszoverxsz;			/* temp ratio */
     int x1, y1;                        /* fixed box corner   */
     int x2, y2;                        /* moving box corner   */
     int x, y;
     int xinc, yinc;
     int minwidth, minheight;
     int maxwidth, maxheight;
     int ox, oy;			/* which quadrant of window */
     int pop_x, pop_y;			/* location of pop window */
     int hsize, vsize;			/* dynamic size */
     int delta;	
     int junk_x, junk_y;
     int ptrmask;			/* pointer status word */
     int num_vectors;			/* Number of vectors to XDraw. */
     Window assoc;			/* Window represented by the icon. */
     Window sub_win;			/* Mouse query sub window. */
     Window root;			/* Root query window. */
     XEvent button_event, *b_ev;	/* Button event packet. */
     XSegment box[MAX_BOX_VECTORS];	/* Box drawing vertex buffer. */
     XSegment zap[MAX_ZAP_VECTORS];	/* Zap drawing vertex buffer. */
     Boolean stop;			/* Should the window stop changing? */
     XSizeHints sizehints;
     XWindowChanges values;
     int width_offset, height_offset;	/* to subtract if resize increments */
     int x_offset, y_offset;		/* add to pointer to get anchor */
     AwmInfoPtr awi;

     Entry("Resize")

     /*
      * Do nothing if the event window is the root window.
      */
     if (window == RootWindow(dpy, scr))
	  Leave(FALSE)

     /*
      * Gather info about the event window.
      */
     awi = GetAwmInfo(window);
     if (!awi)
	  Leave(FALSE)
     if (awi->attrs & AT_SIZE)
	 /* resizes forbidden for this window */
	 Leave(FALSE)
     /*
      * Do not resize an icon window.
      */
     if (IsIcon(window, NULL))
	  Leave(FALSE)

     /*
      * Turn on the resize cursor.
      */
     grab_pointer();

     window = awi->client;
     b_ev = &button_event;

     status = XGetWindowAttributes(dpy, window, &client_info);
     if (status == FAILURE) {
	 ungrab_pointer();
	 Leave(FALSE)
     }

     if (awi->frame) { /* we have to compute root-relative position */
	  status = XGetWindowAttributes(dpy, awi->frame, &frame_info);
	  if (status == FAILURE) {
	      ungrab_pointer();
	      Leave(FALSE)
	  }
	  client_info.y = frame_info.y;
	  client_info.x = frame_info.x;
	  if (awi->attrs & AT_TITLE)
	       client_info.y += titleHeightExcess;    /* why was it +2 ??? */
	  if (awi->attrs & AT_BORDER) {
	      client_info.y += borderHeightExcess;
	      client_info.x += borderPositionExcess;
	  }
     }
     else {
	 frame_info.y = client_info.y;
	 frame_info.x = client_info.x;
	 frame_info.width = client_info.width;
	 frame_info.height = client_info.height;
     }

     /*
      * Clear The vector buffers.
      */
     bzero(box, sizeof(box));
     if (Zap)
	  bzero(zap, sizeof(zap));

     /*
      * If we are here then we have a resize operation in progress.
      */
     
     /*
      * calculate fixed point (x1, y1) and varying point (x2, y2).
      */
     hsize = client_info.width;
     vsize = client_info.height;
     /* overall client window size, including one pixel of borders */
     clientwidth = hsize + 2;
     clientheight = vsize + 2;
     /* use innermost pixel of client border for grid picking */
     clientinsidex1 = client_info.x + WBorderWidth - 1;
     clientinsidey1 = client_info.y - 1;
/*   clientinsidey1 = client_info.y + WBorderWidth - 1; XXX this is wrong, I 
can't figure out why!! */
     /* use overall frame size for sizing window. */
     x1 = frame_info.x;
     y1 = frame_info.y;
     x2 = frame_info.x + frame_info.width - 1;
     y2 = frame_info.y + frame_info.height - 1;

     /*
      * Get the event window size hint.  XXX Optimization - cache it!
      */
     sizehints.flags = 0;
     XGetSizeHints(dpy, window, &sizehints, XA_WM_NORMAL_HINTS); 
     CheckConsistency(&sizehints);

     /* until there are better WM_HINTS, we'll assume that the client's
      * minimum width and height are the appropriate offsets to subtract
      * when resizing with an explicit resize increment.
      */
     if ((sizehints.flags & PBaseSize) && (sizehints.flags & PResizeInc)) {
	  width_offset = sizehints.base_width;
	  height_offset = sizehints.base_height;
     } else if ((sizehints.flags & PMinSize) && (sizehints.flags & PResizeInc)) {
	  width_offset = sizehints.min_width;
	  height_offset = sizehints.min_height;
     } else
	  width_offset = height_offset = 0;
 
     /*
      * Decide what resize mode we are in. Always rubberband in both
      * directions if window is too small.  
      * Mode 0 = lower or right side fixed (in y1 or x1)
      * Mode 1 = this dimension fixed
      * Mode 2 = top or left side fixed (in y1 or x1)
      * We compute the regions bounded by lines that intersect client window
      * border at the points RESIZE_BOUNDARY (1/4) in from the corners:
      * to choose x handling:
      * y = x * 2ysz/xsz - ysz/2	    top 1/4 left to bottom 1/4 right
      * y = 3 * ysz/2 - x * 2ysz/xsz	    top 1/4 right to bottom 1/4 left
      * to choose y handling:
      * y = x * ysz/(xsz * 2) + ysz/4	    left 3/4 up to right 3/4 down
      * y = 3 * ysz/4 - x * ysz/(xsz * 2)   right 3/4 up to left 3/4 down 
      */
     if (client_info.width > 2 && client_info.height > 2) {
	  yszoverxsz = ((float)clientheight) / (float)clientwidth;
	  ox = (y0 - clientinsidey1) <
	       (((int)((x0 - clientinsidex1) * yszoverxsz) << 1) - 
	       (clientheight >> 1)) ?
	       1 : 0;
/* fprintf (stderr, "yszoverxsz = %f\n", yszoverxsz);
fprintf (stderr, "((x0 - clientinsidex1) * yszoverxsz) = %f\n", ((x0 - clientinsidex1) * yszoverxsz));
fprintf (stderr, "ylim = %d, first ox = %d\n", (((int)((x0 - clientinsidex1) * yszoverxsz) << 1) - (clientheight >> 2)), ox);
*/
	  ox = (y0 - clientinsidey1) >
	       (((3 * clientheight) >> 1) - 
	       (((int)((x0 - clientinsidex1) * yszoverxsz)) << 1)) ? 
	       (ox ? 2 : 1) : (ox ? 1 : 0);
/* fprintf (stderr, "y0 - clientinsidey1 = %d\n", y0 - clientinsidey1);
fprintf (stderr, "(int)((x0 - clientinsidex1) * yszoverxsz) << 1 = %d\n", (int)((x0 - clientinsidex1) * yszoverxsz) << 1);
fprintf (stderr, "(int)((x0 - clientinsidex1) * yszoverxsz) = %d\n", (int)((x0 - clientinsidex1) * yszoverxsz));
 fprintf (stderr, "ylim = %d, second ox = %d\n", (((clientheight * 3) >> 1) - ((int)((x0 - clientinsidex1) * yszoverxsz) << 1)), ox);
*/
	  oy = (y0 - clientinsidey1) >
	       (((int)((x0 - clientinsidex1) * yszoverxsz) >> 1) + 
	       (clientheight >> 2)) ?
	       1 : 0;
/* fprintf (stderr, "ylim = %d, first oy = %d\n", (((int)((x0 - clientinsidex1) * yszoverxsz) >> 1) + (clientheight >> 2)), oy);
*/
	  oy = (y0 - clientinsidey1) >
	       (((3 * clientheight) >> 2) - 
	       ((int)((x0-clientinsidex1) * yszoverxsz) >> 1)) ?
	       (oy ? 2 : 1) : (oy ? 1 : 0);
/* fprintf (stderr, "ylim = %d, second oy = %d\n", (((3 * clientheight) >> 2) - ((int)((x0-clientinsidex1) * yszoverxsz) >> 1)), oy);
*/
	  if ((ox + oy) & 1) {  /* they are never both 1 */
	       if (ox & 1) {
		    /* fix up size hints so that we will never change width */
		    sizehints.min_width = sizehints.max_width =
			 client_info.width;
		    if ((sizehints.flags & (PMinSize | PBaseSize)) == 0) {
			 sizehints.min_height = 0;
			 sizehints.flags |= PMinSize;
		    }
		    if ((sizehints.flags & PMaxSize) == 0) {
			 sizehints.max_height = DisplayHeight(dpy, scr);
			 sizehints.flags |= PMaxSize;
		    }
	       }
	       if (oy & 1) {
		    /* fix up size hints so that we will never change height */
		    sizehints.min_height = sizehints.max_height =
			 client_info.height;
		    if ((sizehints.flags & (PMinSize | PBaseSize))==0) {
			 sizehints.min_width = 0;
			 sizehints.flags |= PMinSize;
		    }
		    if ((sizehints.flags & PMaxSize)==0) {
			 sizehints.max_width = DisplayWidth(dpy, scr);
			 sizehints.flags |= PMaxSize;
		    }
	       }
	  }
     }
     else ox = oy = 2;
     /* change fixed point to one that shouldn't move */
     if (oy == 0) { 
	  y = y1; y1 = y2; y2 = y;
     }
     if (ox == 0) { 
	  x = x1; x1 = x2; x2 = x;
     }
     if (sizehints.flags & PMinSize) {
	  minwidth = sizehints.min_width;
	  minheight = sizehints.min_height;
     } else if (sizehints.flags & PBaseSize) {
	  /* ICCCM sez this is a backup */
	  minwidth = sizehints.base_width;
	  minheight = sizehints.base_height;
     } else {
	  minwidth = 0;
	  minheight = 0;
     }
     if (sizehints.flags & PMaxSize) {
	  maxwidth = max(sizehints.max_width, minwidth);
	  maxheight = max(sizehints.max_height, minheight);
     } else {
	  /* allow titlebar to be resized off-screen */
	  maxwidth = DisplayWidth(dpy, scr);
	  maxheight = DisplayHeight(dpy, scr);
     }
     if (sizehints.flags & PResizeInc) {
	  xinc = sizehints.width_inc;
	  yinc = sizehints.height_inc;
     } else {
	  xinc = 1;
	  yinc = 1;
     }
     switch (ox) {
     case 0:
	  pop_x = x1 - PWidth + 1;
	  break;
     case 1:
	  pop_x = x1 + (frame_info.width - PWidth) / 2;
	  break;
     case 2:
	  pop_x = x1;
	  break;
     }
     switch (oy) {
     case 0:
	  pop_y = y1 - PHeight - 1;
	  break;
     case 1:
	  pop_y = y1 + (frame_info.height - PHeight) / 2;
	  break;
     case 2:
	  pop_y = y1;
	  break;
     }
     /*
      * Double expose on the target window is too expensive for some reason
      * or another. Paint the popup window in the upper left hand
      * corner of the screen if RootResizeBox is TRUE. This is also
      * more-or-less consistent with the position of the map request popup.
      */
     if (RootResizeBox == TRUE)
	  values.x = values.y = 0;
     else {
	  if (pop_x < 0 || pop_x > (DisplayWidth(dpy, scr) - PWidth))
	       pop_x = 0;
	  if (pop_y < 0 || pop_y > (DisplayHeight(dpy, scr) - PHeight))
	       pop_y = 0;
	  values.x = pop_x;
	  values.y = pop_y;
     }
     values.stack_mode = Above;
     XConfigureWindow(dpy, Pop, CWX|CWY|CWStackMode, &values);
     XMapWindow(dpy, Pop);
     num_vectors = StoreResizeBox(box, MIN(x1, x2), MIN(y1, y2),
				       MAX(x1, x2), MAX(y1, y2), awi);

     /*
      * If we freeze the server, then we will draw solid
      * lines instead of flickering ones during resizing.
      */
     if (Freeze)
	  XGrabServer(dpy);
     /* protect us from ourselves */
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
	 if (EventDebug)
	     print_event_info("Resize: putting back ", &button_event);
	 XPutBackEvent(dpy, &button_event);
	 GetButton(&button_event);
     }

     if (ResizeRelative) {
	  x_offset = x2 - x0;
	  y_offset = y2 - y0;
     }
     else
	  x_offset = y_offset = 0;

     /*
      * Now draw the box.
      */
     DrawBox();
     Frozen = window;
 
     stop = FALSE;
     x = -1; y = -1;
     
     while (!stop) {
	  if (x != x2 || y != y2) {
	       x = x2; y = y2;
	       /*
		* If we've frozen the server, then erase
		* the old box.
		*/
	       if (Freeze)
		    DrawBox();
	       num_vectors = StoreResizeBox(box, MIN(x1, x2), MIN(y1, y2),
						 MAX(x1, x2), MAX(y1, y2), awi);

	       if (Freeze) {
		   DrawBox();
		   XFlush(dpy);
	       }

	       {
		    int Hsize = (hsize - width_offset) / xinc;
		    int Vsize = (vsize - height_offset) / yinc;
		    int pos = 5;
		    int n, digits;

		    n = digits = 0;
		    if (Hsize > 999) {
			PText[0] = (n = (Hsize / 1000)) + '0';
			Hsize -= n * 1000; n = 0; digits++;
		    } else {
			PText[0] = ' ';
		    }
		    if (Hsize > 99 || digits) {
			PText[1] = (n = (Hsize / 100)) + '0';
			Hsize -= n * 100; n = 0; digits++;
		    } else {
			PText[1] = ' ';
		    }
		    if (Hsize > 9 || digits) {
			PText[2] = (n = (Hsize / 10)) + '0';
			Hsize -= n * 10; n = 0; digits++;
		    } else {
			PText[2] = ' ';
		    }
		    PText[3] = Hsize + '0';

		    n = digits = 0;
		    if (Vsize > 999) {
			PText[pos++] = (n = (Vsize / 1000)) + '0';
			Vsize -= n * 1000; n = 0; digits++;
		    }
		    if (Vsize > 99 || digits) {
			PText[pos++] = (n = (Vsize / 100)) + '0';
			Vsize -= n * 100; n = 0; digits++;
		    }
		    if (Vsize > 9 || digits) {
			PText[pos++] = (n = (Vsize / 10)) + '0';
			Vsize -= n * 10; n = 0; digits++;
		    }
		    PText[pos++] = Vsize + '0';

		    while (pos < PTEXT_LENGTH) PText[pos++] = ' ';
	       }
	       /*
		* If the font is not fixed width we have to
		* clear the window to guarantee that the characters
		* that were there before are erased.
		*/
	       if (!(PFontInfo->per_char))
		    XClearWindow(dpy, Pop);
	       XDrawImageString(
				dpy, Pop, PopGC,
				HPopupPad, VPopupPad+PFontInfo->ascent,
				PText, PTextSize);
	  }
	  if (!Freeze) {
	       /* if not freeze, flash up box whether changed or not */
	       DrawBox();
	       XSync(dpy, FALSE); /* try to make duty cycle of box non-zero */
	       DrawBox();
	  }

	  if (XPending(dpy) && !ProcessRequests(box, num_vectors) && GetButton(&button_event)) {
	       if ((button_event.type == ButtonPress) && (button == -1)) {
		   button = ((XButtonPressedEvent *)&button_event)->button;
		   /* legal Press due to preselection from menu; stash it */
		   continue;
	       }

	       if ((button_event.type != ButtonPress) && 
		   (button_event.type != ButtonRelease)) {
		    continue; /* spurious menu event... */
	       }

	       if (Freeze) {
		    DrawBox();
		    Frozen = (Window)0;
		    XUngrabServer(dpy);
	       }

	       if (b_ev->xany.type == ButtonRelease &&
		   b_ev->xbutton.button == button)
		    stop = TRUE;
	       else {
		    XUnmapWindow(dpy, Pop);
		    ResetCursor(button);
		    Snatched = False;
		    ungrab_pointer();
		    Leave(TRUE)
	       }
	  }
	  else {
	       XQueryPointer(dpy, RootWindow(dpy, scr), &root, 
			     &sub_win, &x2, &y2, &junk_x, &junk_y, &ptrmask);
	       x2 += x_offset;	/* get to anchor point */
	       y2 += y_offset;
	  }
	  /* save all this computing if there is no change, to stay responsive
	     */
	  if (x != x2 || y != y2) {
	  /* recompute client window size from new frame bbox */
	  hsize = abs (x2 - x1) + 1;
	  vsize = abs (y2 - y1) + 1 - frameHeightExcess;
	  if (awi->attrs & AT_TITLE)
	      vsize -= titleHeightExcess;
	  if (awi->attrs & AT_BORDER) {
	      hsize -= frameWidthExcess;
	      vsize -= borderHeightExcess;
	  }
	  else
	      hsize -= frameHeightExcess;
	  hsize = max(min(hsize, maxwidth), minwidth);
	  hsize = makemult(hsize - minwidth, xinc) + minwidth;

	  vsize = max(min(vsize, maxheight), minheight);
	  vsize = makemult(vsize - minheight, yinc) + minheight; 
	  if (sizehints.flags & PAspect) {
	       if ((hsize * sizehints.max_aspect.y > 
		    vsize * sizehints.max_aspect.x)) {
		    delta = makemult((hsize * sizehints.max_aspect.y /
				      sizehints.max_aspect.x) - vsize, 
				     yinc);
		    if ((vsize + delta <= maxheight))
			 vsize += delta;
		    else {
			 delta = makemult(hsize - 
					  (sizehints.max_aspect.x *
					   vsize/sizehints.max_aspect.y), 
					  xinc);
			 if (hsize - delta >= minwidth)
			      hsize -= delta; 
		    }
	       }  
	       if (hsize * sizehints.min_aspect.y < vsize *
		   sizehints.min_aspect.x) {
		    delta = makemult((sizehints.min_aspect.x * 
				      vsize/sizehints.min_aspect.y) - hsize, 
				     xinc);
		    if (hsize + delta <= maxwidth)
			 hsize += delta;
		    else {
			 delta = makemult(vsize - 
					  (hsize*sizehints.min_aspect.y /
					   sizehints.min_aspect.x), 
					  yinc); 
			 if (vsize - delta >= minheight)
				vsize -= delta; 
		    }
	       }
	  }
	  /* recompute new frame bbox from client window size */
	  frame_info.width = hsize;
	  frame_info.height = vsize + frameHeightExcess;
	  if (awi->attrs & AT_TITLE)
	      frame_info.height += titleHeightExcess;
	  if (awi->attrs & AT_BORDER) {
	      frame_info.width += frameWidthExcess;
	      frame_info.height += borderHeightExcess;
	  }
	  else
	      frame_info.width += frameHeightExcess;
	  if (ox == 0)
	       x2 = x1 - frame_info.width + 1;
	  else
	       x2 = x1 + frame_info.width - 1;
	  
	  if (oy == 0)
	       y2 = y1 - frame_info.height + 1;
	  else
	       y2 = y1 + frame_info.height - 1;
/* fprintf (stderr, "adjusted frame x1 = %d, y1 = %d, x2 = %d, y2 = %d, width = %d, height = %d\n client hsize = %d, vsize = %d\n", x1, y1, x2, y2, frame_info.width, frame_info.height, hsize, vsize);
*/
	  }
     }
     clientx1 = min (x1, x2);
     clienty1 = min (y1, y2);
     if (awi->attrs & AT_TITLE)
         clienty1 += titlePositionExcess;
     if (awi->attrs & AT_BORDER) {
         clienty1 += borderPositionExcess;
         clientx1 += borderPositionExcess;
     }
     if ((clientx1 != client_info.x) || (clienty1 != client_info.y) || 
	 (hsize != client_info.width) ||
	 (vsize != client_info.height)) {
	  XWindowChanges xwc;
	       
	  xwc.x = clientx1;
	  xwc.y = clienty1;
	  xwc.width = hsize;
	  xwc.height = vsize;
	  ConfigureWindow(window, CWX | CWY | CWHeight | CWWidth, &xwc, TRUE);
     }
     Snatched = False;
     ungrab_pointer();
     XUnmapWindow(dpy, Pop);
     Leave(TRUE)
}

CheckConsistency(hints)
XSizeHints *hints;
{
     Entry("CheckConsistency")

     if (hints->min_height < 0)
	  hints->min_height = 0;
     if (hints->min_width < 0)
	  hints->min_width = 0;
     
     if (hints->max_height <= 0 || hints->max_width <= 0)
	  hints->flags &= ~PMaxSize;
     
     hints->min_height = min(DisplayHeight(dpy, scr), hints->min_height);
     hints->min_width =  min(DisplayWidth(dpy, scr),  hints->min_width);
     
     hints->max_height = min(DisplayHeight(dpy, scr), hints->max_height);
     hints->max_width =  min(DisplayWidth(dpy, scr),  hints->max_width);
     
     if ((hints->flags & PMinSize) && (hints->flags & PMaxSize) && 
	 ((hints->min_height > hints->max_height) ||
	  (hints->min_width > hints->max_width)))
	  hints->flags &= ~(PMinSize|PMaxSize);
     
     if ((hints->flags & PAspect) && 
	 (hints->min_aspect.x * hints->max_aspect.y > 
	  hints->max_aspect.x * hints->min_aspect.y))
	  hints->flags &= ~(PAspect);
     Leave_void
}
