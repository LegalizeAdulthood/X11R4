#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: RubberBand.c,v 5.18 89/11/22 15:12:55 glennw Exp $";
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
 * 000 -- Loretta Guarino Reid, DEC Ultrix Engineering Group
 * 001 -- Ralph R. Swick, DEC/MIT Project Athena
 *	  tailor to uwm; use global resources created by uwm
 */

#include "awm.h"
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#define max(a,b) ( (a) > (b) ? (a) : (b) )
#define min(a,b) ( (a) > (b) ? (b) : (a) )
#define abs(a) ( (a) > 0 ? (a) : -(a))
#define makemult(a, b) ((b==1) ? (a) : (((int)((a) / (b))) * (b)) )

#define BW	PBorderWidth		/* pop-up window border width */
#define VIBW	VPopupPad		/* Vert. text padding for pop-up */
#define HIBW	HPopupPad		/* Hor. text padding for pop-up */

/* used to extract DestroyNotify events for this window */
Bool destroyPred (dpy, event, arg)
Display *dpy;
XEvent *event;
char *arg;
{
    return (event->xany.type == DestroyNotify && 
	    event->xdestroywindow.window == (Window) arg);
}

    /* Get window position, abort if window is destroyed */
Bool AskUser(dpy, scr, window, x, y, width, height, hints, transient, from_wm)
Display *dpy;
int scr;
Window window;
int *x, *y, *width, *height;
XSizeHints *hints;
Window transient;
Boolean *from_wm;    /* did the position come from wm/user or client? */
{
     static Cursor ur = NULL, ul, ll, lr;     /* cursors for rubber banding */
     int change_cursor = FALSE;
     int current_cursor;
     char *text;			/* text for prompt string       */
     int nz;                         /* count where zeros are        */
     int popw, poph;                 /* width and height of prompt window*/
     int x1, y1;                     /* location of mouse            */
     int x2, y2;                     /* other corner of box          */
     int num_vectors;			/* Number of vectors to XDraw. */
     XSegment box[MAX_BOX_VECTORS];	/* Box drawing vertex buffer. */
     int rootx, rooty, mask;         /* for XQueryPointer          */
     Window root, subw;		/* for XQueryPointer */
     int xa = -1, ya = -1, xb = -1, yb = -1;
     int frame_width, frame_height;  /* size of frame */
     int frame_def_width, frame_def_height;  /* US size, adjusted to frame */
     int xinc = 1;
     int yinc = 1;
     int minwidth, minheight;
     int maxwidth, maxheight;
     int chosen = -1;
     int stop = FALSE;
     int abort = FALSE;		/* abort if client window dies */
     int changed = TRUE;
     int box_is_drawn = FALSE;
     int dx, dy;
     int delta;
     int ijunk;
     int hsize, vsize;
     int zero = '0';                 /* zero offset for char conversion  */
     XEvent e;                 /* someplace to put the event   */
     int events;                     /* what events we want.             */
     Window pop, wjunk;                     /* pop up prompt window         */
     int i;
     char *name;
     Bool name_alloced;		/* Don't free name if not malloced */
     int width_offset, height_offset; /* to subtract if resize increments */
     XSetWindowAttributes attr;	/* for setting save unders flag */
     AwmInfoPtr awi;
     
     Entry("AskUser")
     
     /*
      * Gather info about the event window.
      */
     awi = GetAwmInfo(window);
     if (!awi)
	  Leave(FALSE)

     /* Note - currently, we never get called on a transient window */
     if (transient) {
	  XGetGeometry( dpy, window, &wjunk, x, y, width, height,
		       &ijunk, &ijunk);
	  *from_wm = FALSE;
	  Leave(TRUE)
     }

     if (hints->flags & USPosition) {
          if (hints->flags & PMinSize) {
	      if (*width < hints->min_width) *width = hints->min_width;
	      if (*height < hints->min_height) *height = hints->min_height;
	  } else if (hints->flags & PBaseSize) {
	      /* ICCCM sez this is a backup */
	      if (*width < hints->base_width) *width = hints->base_width;
	      if (*height < hints->base_height) *height = hints->base_height;
	  }
          if (hints->flags & PMaxSize) {
	      if (*width > hints->max_width) *width = hints->max_width;
	      if (*height > hints->max_height) *height = hints->max_height;
	  }
	  *from_wm = FALSE;
/* fprintf (stderr, "AskUser: resolved client window position is x=%d, y=%d, width=%d, height=%d, from_wm=%d\n", *x, *y, *width, *height, from_wm );
*/
	  Leave(TRUE)
     }

     if (ur == NULL) {
         ur = XCreateFontCursor(dpy, XC_ur_angle);
	 ul = XCreateFontCursor(dpy, XC_ul_angle);
	 ll = XCreateFontCursor(dpy, XC_ll_angle);
	 lr = XCreateFontCursor(dpy, XC_lr_angle);
     }
     current_cursor = ul;

     events = ButtonPressMask | ButtonReleaseMask;

     /*
      * go get the mouse as soon as you can
      */

     while (1) {
         int res;
	 if ((res = XGrabPointer (dpy, RootWindow(dpy, scr), TRUE, events, 
		    GrabModeAsync, GrabModeAsync, None, ul, CurrentTime )) == 
		    GrabSuccess)
	     /*	must let misc.  pointer events continue to come in, such as
		EnterNotify, hence owner_events = TRUE. */
	     break;
	     sleep (1);
     }

     if (!XFetchName(dpy, window, &name)) {
	 name_alloced = FALSE;	/* Don't free name if not malloced */
	 name = "Unnamed Window";
     }
     else
	 name_alloced = TRUE;
     nz = strlen(name);		/* compute number of characters */
     text = (char *)malloc( nz + 12 );	/* don't forget NULL */
     (void) strcpy(text, name);
     (void) strcat(text, ": 0000x0000");
     nz += 11;
     popw = XTextWidth (PFontInfo, text, nz) + 2 * HIBW;
     poph = PFontInfo->ascent+PFontInfo->descent + 2 * VIBW;

     pop = XCreateSimpleWindow(dpy, RootWindow(dpy, scr), 0, 0, popw, poph,
			       BW, PBorder, PBackground);
     if (SaveUnder) {
	 attr.save_under = True;
	 XChangeWindowAttributes (dpy, pop, CWSaveUnder, &attr);
     }
     XMapWindow (dpy, pop);

     if (hints->flags&PMinSize) {
	  minwidth = hints->min_width;
	  minheight = hints->min_height;
     } else if (hints->flags&PBaseSize) {
	  /* ICCCM sez this is a backup */
	  minwidth = hints->base_width;
	  minheight = hints->base_height;
     } else {
	  minwidth = 0;
	  minheight = 0;
     }
     if (hints->flags & PMaxSize) {
	  maxwidth = max(hints->max_width, minwidth);
	  maxheight = max(hints->max_height, minheight);
     } else {
	  maxwidth = DisplayWidth(dpy, scr);
	  maxheight = DisplayHeight(dpy, scr);
     }
     if (hints->flags & PResizeInc) {
	  xinc = hints->width_inc;
	  yinc = hints->height_inc;
     }
     /* if the user lies to us or sets to an inappropriate value */
     /* use the default rather than die a horrible death via a   */
     /* floating point exception.                                */
     if (xinc == 0) xinc = 1;
     if (yinc == 0) yinc = 1;

     /* Unless the client has set base size, we'll assume that the client's
      * minimum width and height are the appropriate offsets to subtract
      * when resizing with an explicit resize increment.
      */
     if (hints->flags & PBaseSize && hints->flags & PResizeInc) {
	  width_offset = hints->base_width;
	  height_offset = hints->base_height;
     } else if (hints->flags & PMinSize && hints->flags & PResizeInc) {
	  width_offset = hints->min_width;
	  height_offset = hints->min_height;
     } else
	  width_offset = height_offset = 0;
     
     
     XQueryPointer (dpy, RootWindow(dpy, scr), &root, &subw, 
		    &rootx, &rooty, &x1, &y1, &mask);

     /* the first rubber box shown is the minimum size */
     hsize = minwidth; 
     vsize = minheight;
     /* compute frame bbox from client window size */
     frame_width = hsize;
     frame_height = vsize + frameHeightExcess;
     if (awi->attrs & AT_TITLE)
	 frame_height += titleHeightExcess;
     if (awi->attrs & AT_BORDER) {
         frame_width += frameWidthExcess;
	 frame_height += borderHeightExcess;
     }
     else
	 frame_width += frameHeightExcess;
     x2 = x1 + frame_width - 1; 
     y2 = y1 + frame_height - 1;
     
     if (Freeze)
	XGrabServer(dpy);

     while (stop == FALSE) {
	  if ( (xb != max (x1, x2)) || (yb != max (y1, y2))
	      ||(xa != min (x1, x2)) || (ya != min (y1, y2)) ) {
	      /* we have a new geometry; redraw it now if freeze */
	       xa = MIN(x1, x2);
	       ya = MIN(y1, y2);
	       xb = MAX(x1, x2);
	       yb = MAX(y1, y2);
	      if (Freeze && box_is_drawn) {
		   /* erase previous rubber box just before drawing new one */
		  DrawBox();
	      }
		/* Can't freeze because other clients may be coming up??
		   Could we freeze, and thaw occasionally?  This really
		   should not be necessary; clients should wait patiently
		   until server is thawed. */
		num_vectors = StoreResizeBox(box, xa, ya, xb, yb, awi);
		/* immediately redraw box */
		if (Freeze) {
		    DrawBox();
		    box_is_drawn = True;
		    XFlush(dpy);
		}
	    }
	    if (!Freeze) {
		/* flash up box every loop */
		DrawBox();
		XSync(dpy, FALSE);
		/* try to make duty cycle of flashing box non-zero */
		DrawBox();
	    }
	    if (changed) {
	        int Hsize = (hsize - width_offset) / xinc;
		int Vsize = (vsize - height_offset) / yinc;
		int pos = 4;
		int n, digits;

		/* text consists of strcat (name, ": 0000x0000"), nz = len */

		changed = FALSE;
		n = digits = 0;
		if (Hsize > 999) {
		    text[nz - 9] = (n = (Hsize / 1000)) + '0';
		    Hsize -= n * 1000; n = 0; digits++;
		} else {
		    text[nz - 9] = ' ';
		}
		if (Hsize > 99 || digits) {
		    text[nz - 8] = (n = (Hsize / 100)) + '0';
		    Hsize -= n * 100; n = 0; digits++;
		} else {
		    text[nz - 8] = ' ';
		}
		if (Hsize > 9 || digits) {
		    text[nz - 7] = (n = (Hsize / 10)) + '0';
		    Hsize -= n * 10; n = 0; digits++;
		} else {
		    text[nz - 7] = ' ';
		}
		text[nz - 6] = Hsize + '0';

		n = digits = 0;
		if (Vsize > 999) {
		    text[nz - pos--] = (n = (Vsize / 1000)) + '0';
		    Vsize -= n * 1000; n = 0; digits++;
		} 
		if (Vsize > 99 || digits) {
		    text[nz - pos--] = (n = (Vsize / 100)) + '0';
		    Vsize -= n * 100; n = 0; digits++;
		} 
		if (Vsize > 9 || digits) {
		    text[nz - pos--] = (n = (Vsize / 10)) + '0';
		    Vsize -= n * 10; n = 0; digits++;
		} 
		text[nz - pos--] = Vsize + '0';

		while (pos > 0) text[nz - pos--] = ' ';

		XDrawImageString(dpy, pop, PopGC, HIBW, VIBW+PFontInfo->ascent,
			text, nz);
	    }
            if (XPending(dpy)) {
		if (XCheckIfEvent(dpy, &e, destroyPred, (char *)window)) {
		    /* the client window died - handle it now. */
		    if (EventDebug)
			print_event_info("AskUser", &e);
		    abort = TRUE;
		    break;
		}
		if (XCheckMaskEvent(dpy, ButtonPressMask|ButtonReleaseMask, &e)) {
		    if (EventDebug)
			print_event_info("AskUser", &e);
		    if (e.xany.window != RootWindow(dpy, scr))
			/* because we are grabbing with owner_events = TRUE, 
			   event might get reported on gadget, etc. */
			XTranslateCoordinates(dpy, e.xany.window, 
			    RootWindow(dpy, scr), e.xbutton.x, e.xbutton.y, 
			    &x2, &y2, &subw);
		    else {
			x2 = e.xbutton.x;
			y2 = e.xbutton.y;
		    }
                    if ((chosen < 0) && (e.type == ButtonPress)) {
			x1 = x2;
                        y1 = y2;
                        chosen = ((XButtonEvent *)&e)->button;
			if (chosen == Button2)
				change_cursor = TRUE;
                    }
		    else if ((e.type == ButtonRelease) &&
                        ((((XButtonEvent *)&e)->button) == chosen)) {
			stop = TRUE;
		    }
		}
		else {
		    XQueryPointer (dpy, RootWindow(dpy, scr), &root, 
			           &subw, &rootx, &rooty, &x2, &y2, &mask);
		}
	    }
	    else        XQueryPointer (dpy, RootWindow(dpy, scr), &root, 
				     &subw, &rootx, &rooty, &x2, &y2, &mask);
	  if (change_cursor) {
	       if ((x2 >= x1) && (y2 >= y1) &&
		   current_cursor != lr) {
                    XChangeActivePointerGrab (dpy, events, lr, CurrentTime );
                    current_cursor = lr;
               }
	       else if ((x2 >= x1) && (y2 < y1) &&
			current_cursor != ur) {
                    XChangeActivePointerGrab (dpy, events, ur, CurrentTime );
                    current_cursor = ur;
	       }
	       else if ((x2 < x1) && (y2 >= y1) &&
			current_cursor != ll) {
                    XChangeActivePointerGrab (dpy, events, ll, CurrentTime );
                    current_cursor = ll;
	       }
	       else if ((x2 < x1) && (y2 < y1) &&
			(current_cursor != ul)) {
                    XChangeActivePointerGrab (dpy, events, ul, CurrentTime );
                    current_cursor = ul;
	       }
	  }
	  if (chosen != Button2) {
	       x1 = x2;
	       y1 = y2;
	       if (chosen >= 0) {
	           if (hints->flags & PSize || hints->flags & USSize) {
	           /* in new ICCCM, default size is to be obtained from current
		      geometry, and the old size_hints.x/y/w/h is ignored */
		       frame_def_width = *width;
		       frame_def_height = *height;
		   } else if (hints->flags&PMinSize) {
		       frame_def_width = hints->min_width;
		       frame_def_height = hints->min_height;
		   } else if (hints->flags&PMaxSize) {
		       frame_def_width = hints->max_width;
		       frame_def_height = hints->max_height;
		   } else {
		       /* no hints - do the best you can.  Using base size is
			  not the best you can, contrary to ICCCM!!! */
		       frame_def_width = *width;
		       frame_def_height = *height;
		   }
		   frame_def_height += frameHeightExcess;
		   if (awi->attrs & AT_TITLE)
		       frame_def_height += titleHeightExcess;
		   if (awi->attrs & AT_BORDER) {
		       frame_def_width += frameWidthExcess;
		       frame_def_height += borderHeightExcess;
		   }
		   else
		       frame_def_width += frameHeightExcess;
		    x2 = frame_def_width;
		    if (chosen == Button1)
			 y2 = frame_def_height;
		    else
			 y2 = (DisplayHeight(dpy, scr) - y1);
		    x2 = x1 + x2 - 1;
		    y2 = y1 + y2 - 1;
	       }
	  }
	  
	  /* recompute client window size from new frame bbox */
	  dx = abs (x2 - x1) + 1;
	  dy = abs (y2 - y1) + 1 - frameHeightExcess;
	  if (awi->attrs & AT_TITLE)
	      dy -= titleHeightExcess;
	  if (awi->attrs & AT_BORDER) {
	      dx -= frameWidthExcess;
	      dy -= borderHeightExcess;
	  }
	  else
	      dx -= frameHeightExcess;
	  dx = max(min(dx, maxwidth), minwidth);
	  dx = makemult(dx - minwidth, xinc) + minwidth; 
	  dy = max(min(dy, maxheight), minheight);
	  dy = makemult(dy - minheight, yinc) + minheight; 

	  if (hints->flags & PAspect) {
	       if ((dx * hints->max_aspect.y > dy * hints->max_aspect.x)) {
		    delta = makemult((dx * hints->max_aspect.y /
				      hints->max_aspect.x) - dy, yinc); 
		    if ((dy + delta) <= maxheight)
			 dy += delta;
		    else {
			 delta = makemult(dx - hints->max_aspect.x
					  * dy/hints->max_aspect.y, xinc);
			 if ((dx - delta) >= minwidth)
			      dx -= delta;
		    }
	       }
	       if (dx * hints->min_aspect.y < dy * hints->min_aspect.x) {
		    delta = makemult((hints->min_aspect.x *
				      dy/hints->min_aspect.y) - dx, xinc);
		    if (dx + delta <= maxwidth)
			 dx += delta;
		    else {
			 delta = makemult(dy - (dx * hints->min_aspect.y /
						hints->min_aspect.x), yinc); 
			 if ((dy - delta) >= minheight)
			      dy -= delta; 
		    }
	       }
	  }
	  
	  if (dx != hsize) {
	       hsize = dx;
	       changed = TRUE;
	  }
	  if (dy != vsize) {
	       vsize = dy;
	       changed = TRUE;
	  }
	  /* recompute new frame bbox from client window size */
	  frame_width = hsize;
	  frame_height = vsize + frameHeightExcess;
	  if (awi->attrs & AT_TITLE)
	      frame_height += titleHeightExcess;
	  if (awi->attrs & AT_BORDER) {
	      frame_width += frameWidthExcess;
	      frame_height += borderHeightExcess;
	  }
	  else
	      frame_width += frameHeightExcess;
	  if (x2 < x1)
	       x2 = x1 - frame_width + 1;
	  else
	       x2 = x1 + frame_width - 1;
	  
	  if (y2 < y1)
	       y2 = y1 - frame_height + 1;
	  else
	       y2 = y1 + frame_height - 1;
     }
     if (Freeze) {
	if (box_is_drawn)
	    /* erase final rubber box */
	    DrawBox();
	XUngrabServer(dpy);
     }
     XUngrabPointer(dpy, CurrentTime);
     
     XDestroyWindow (dpy, pop);
     if (name_alloced)
	free(name);
     free(text);
     XFlush(dpy);
     if (abort)
	Leave(FALSE)
     *x = min(x1, x2);
     *y = min(y1, y2);
/* compute client window position from new frame bbox; from_wm sez we are
   returning "final" client position, not client-requested position;
   simply adjust position for frame */
     if (awi->attrs & AT_TITLE)
         *y += titlePositionExcess;
     if (awi->attrs & AT_BORDER) {
         *y += borderPositionExcess;
         *x += borderPositionExcess;
     }
     *width = hsize;
     *height = vsize;
     *from_wm = TRUE;
/* fprintf (stderr, "AskUser: resolved client window position is x=%d, y=%d, width=%d, height=%d, from_wm=%d\n", *x, *y, *width, *height, *from_wm );
*/
     Leave(TRUE)
}

