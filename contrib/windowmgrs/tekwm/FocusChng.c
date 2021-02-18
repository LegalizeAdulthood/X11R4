#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: FocusChng.c,v 5.20 89/11/22 15:13:53 glennw Exp $";
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
 * 001 -- Jordan Hubbard, U.C. Berkeley.
 *   Hacks for autoraise and titles.
 * 002 -- Jordan Hubbard, Ardent Computer
 *   Window pixmap changes on focus in/out.
 * 1.4 -- Fixed Focus In/Out handling. Created LightsOn() and LightsOff()
 * to break out the actual process of "highlighting".
 * 1.5 -- Added dynamic installation of colormaps.
 */


#include "awm.h"

Window FocusWindow;

HandleFocusIn(ev)
XEvent *ev;
{
     XEnterWindowEvent *e = (XEnterWindowEvent *)ev;
     Window w = e->window;
     AwmInfoPtr awi;
     XEvent event;
     XWindowAttributes xwa;

     Entry("HandleFocusIn")

     if (Snatched)
	 Leave(FALSE)

     awi = GetAwmInfo(w);
     if (!awi) {
	 /* Probably a menu or override_redirect or something...  go ahead and
	    set colormap.  If we did not popup a menu and focus/colormap is
	    already set to a window by user, leave colormap at FocusWindow.
	    (If this is the FocusIn event from user or client setting focus we
	    do need to install cmap.) */
	 if (InstallColormap) {
	     if (FocusSetByUser && (ev->type == EnterNotify) && 
		 (e->mode != NotifyGrab)) {
		 /* must re-install User Focus colormap in case it was
		    de-installed below due to a menu.  Otherwise, this has no
		    effect.  There might be a better way to do this that avoids
		    noops... */
		 /* XXX Optimization- cache FocusWindow colormap */
		 XGetWindowAttributes(dpy, FocusWindow, &xwa);
		 XInstallColormap(dpy, xwa.colormap);
	     }
	     else {
		 XGetWindowAttributes(dpy, w, &xwa);
		 XInstallColormap(dpy, xwa.colormap);
	     }
	 }
         Leave(FALSE)
     }

     if (!(BorderContextHilite || BorderHilite || Hilite || (awi->attrs & AT_RAISE) || InstallColormap))
	 Leave(FALSE)

     if (IsIcon(w, NULL) && InstallColormap) {
	 /* We are now selecting EnterWindowMask on icons */
	 if (FocusSetByUser && (ev->type == EnterNotify)) {
	     /*	must re-install User Focus colormap in case it was de-installed
		above due to a menu.  Otherwise, this has no effect.  */
	     /* XXX Optimization- cache FocusWindow colormap */
	     XGetWindowAttributes(dpy, FocusWindow, &xwa);
	     XInstallColormap(dpy, xwa.colormap);
	     Leave(FALSE)
	 }
	 else {
	     /*	Nobody should be setting input to the icon, but just to make
		sure, process FocusIn events too.. */
	     XInstallColormap(dpy, DefaultColormap(dpy, scr));
	     Leave(FALSE)
	 }
     }

		
     if ((e->type != FocusIn && e->detail == NotifyInferior) && FrameFocus) {
         /* if we enter frame from client, restore frame focus */
	 XSetInputFocus(dpy, awi->client, RevertToPointerRoot, CurrentTime);
	 FocusSetByWM = TRUE;
     }

     if ((e->type == FocusIn || e->detail != NotifyInferior)
	 && (e->focus || FrameFocus)) {
	 /* what is this focus jazz ??? Why do we care whether this
	    window has focus?  And what if this is a FocusIn event?  */
	 if (e->type != FocusIn) {
	     w = (FrameFocus && awi->frame) ? awi->frame : awi->client;
	     if ((awi->attrs & AT_RAISE) && RaiseDelay &&
	         (awi->visibility != VisibilityUnobscured)) {
		/* don't wait around if window already visible */
		 static struct timeval timeout;
		 static Bool uninitialized = TRUE;
		 static unsigned long *readfd, *ptr;
		 int maskcnt, nfound;

		 if(uninitialized) {
		     maskcnt = ((ConnectionNumber(dpy) + 31)/32);
		     readfd = (unsigned long *)malloc(maskcnt * 4);
						/* 4 bytes per long */
/* by experimentation I have learned that tv_usec >= 1 sec is ignored. */
		     timeout.tv_sec = RaiseDelay / 1000;
		     timeout.tv_usec = (RaiseDelay % 1000) * 1000; 
		     for (ptr = readfd; ptr - readfd < maskcnt; *ptr++ = 0);
		     uninitialized = FALSE;
		 }
	         /*
		  * ???  RaiseDelay affects highlighting!  Should we highlight
		  * immediately, raise later?
		  */
/* to make the delay reset with mouse motion, select for mouse moved, then */
		 do {
	             readfd[ConnectionNumber(dpy)/32] |= 
		      1 << (ConnectionNumber(dpy) % 32);
		     nfound = select(ConnectionNumber(dpy) + 1, readfd, 0, 0, &timeout);
		   /* Did we leave this window already? */
	             if (XCheckTypedWindowEvent(dpy, w, LeaveNotify, &event)) {
			 if (EventDebug)
			     print_event_info("HandleFocusIn", &event);
		         if (event.xcrossing.detail != NotifyInferior) {
			     Leave(FALSE)
			 }
		     }
		     /* suck 'em up and wait for more */
		     while (XCheckTypedEvent(dpy, MotionNotify, &event))
			 if (EventDebug)
			     print_event_info("HandleFocusIn", &event);
		 } while (nfound);
		 /* quit when we make it all the way through a timeout
		    without an event */
	     }
	     else if (XCheckTypedWindowEvent(dpy, w, LeaveNotify, &event)) {
		 if (EventDebug)
		     print_event_info("HandleFocusIn", &event);
		 if (event.xcrossing.detail != NotifyInferior)	
		     Leave(FALSE)
	     }
	     if (InstallColormap) {
		 if (FocusSetByUser) {
		     /*	must re-install User Focus colormap in case it was
			de-installed above due to a menu.  Otherwise, this has
			no effect. */
		     /* XXX Optimization- cache FocusWindow colormap */
		     XGetWindowAttributes(dpy, FocusWindow, &xwa);
		     XInstallColormap(dpy, xwa.colormap);
		 }
		 else {
		     /*	XXX minor optimization - keep track of installed
			colormap and only install if changed (also in
			GetButton.c) */
		     XInstallColormap(dpy, awi->client_cmap);
		 }
	     }
	     if (!FocusSetByUser && FocusWindow != awi->client) {
		  /* set focus to client even if they do not get input, so
		     highlighting and colormap handling is consistent.
		     Not strictly legal according to current ICCCM wording.. */
		  if (FrameFocus) {
		      XSetInputFocus(dpy, awi->client, RevertToPointerRoot,
				     CurrentTime);
		      FocusSetByWM = TRUE;
		  }
		  FocusWindow = awi->client;
	     }
	     else
		 Leave(FALSE)
	 }
	 else {
	     FocusWindow = awi->client;
	     FocusSetByWM = FALSE;
	     /* I think that this is superfluous - it is already FALSE! */
	     if (InstallColormap) {
		 /* must re-install User Focus colormap in case it was
		    de-installed above due to a menu.  Otherwise, this has no
		    effect. */
		 XInstallColormap(dpy, awi->client_cmap);
	     }
	 }

	  /*
	   * If Autoraise is set and window not fully visible, raise that puppy..
	   */
	  if ((awi->attrs & AT_RAISE) && 
	      (awi->visibility != VisibilityUnobscured))
	       XRaiseWindow(dpy, (awi->frame) ? awi->frame : awi->client);
	  LightsOn(awi);
     }
     Leave(FALSE)
}

HandleFocusOut(ev)
XEvent *ev;
{
     XLeaveWindowEvent *e = (XLeaveWindowEvent *)ev;
     Window w = e->window;
     AwmInfoPtr awi;
     XEvent event;

     Entry("HandleFocusOut")

     awi = GetAwmInfo(w);
     if (!awi)
	  /* Either a menu or override_redirect or a recently unregistered 
	     window or something. */
         Leave(FALSE)

     if (!(BorderContextHilite || BorderHilite || Hilite || (awi->attrs & AT_RAISE)) || Snatched)
	 Leave(FALSE)

     if ((e->type != FocusOut && e->detail == NotifyInferior) 
	    	       && !FocusSetByUser && FrameFocus) {
         /* if we enter client from frame, turn off frame focus.  This is an
	    optimization so that if the pointer moves out of the window, the wm
	    does not have to wake up for focus to move. */
	 XSetInputFocus(dpy, PointerRoot, None, CurrentTime);
	 FocusSetByWM = TRUE;
     }

     if ((e->type == FocusOut || e->detail != NotifyInferior)
	 && (e->focus || FrameFocus)) {
	 /* what is this focus jazz ??? */
	  if (e->type != FocusOut) {
	       w = (FrameFocus && awi->frame) ? awi->frame : awi->client;
	       /* Did we come back into this window? */
	       if (XCheckTypedWindowEvent(dpy, w, EnterNotify, &event)) {
		    if (EventDebug)
			print_event_info("HandleFocusIn", &event);
		    if (event.xcrossing.detail != NotifyInferior)
			 Leave(FALSE)
	       }
	       if (!FocusSetByUser) {
		    if (FrameFocus) {
			 XSetInputFocus(dpy, PointerRoot, None, CurrentTime);
			 FocusSetByWM = TRUE;
		    }
		    FocusWindow = RootWindow(dpy, scr);
	       }
	       else
		    Leave(FALSE)
	  }
	  else {
	       FocusWindow = RootWindow(dpy, scr);
	       /* I think that this is superfluous - it is already FALSE! */
	       FocusSetByWM = FALSE;
	  }
	  LightsOff(awi, FALSE);
     }
     Leave(FALSE)
}

/*
 * Turns higlighting off for a window. Counterpart to LightsOn
 */
LightsOff(awi, init)
AwmInfoPtr awi;
Bool init;
{
    Pixel borderPixel;
    int i;

     Entry("LightsOff")

    if (BorderHilite || init) {
       /* set the window border to defined pixmap, or "gray" stipple pixmap
	  on mono without BorderHilite */
	if (awi->B_back) {
	    if(WBorderWidth)
		XSetWindowBorderPixmap(dpy, awi->client, awi->B_back);	    
	    if (awi->frame && awi->title) {
		if(WBorderWidth)
		    XSetWindowBorderPixmap(dpy, awi->title, awi->B_back);
	        if ((awi->attrs & AT_GADGETS) && GadgetBorder)
		    for (i = 0; i < NumGadgets && Gadgets[i]; i++) {
			if (awi->gadgets[i])
			    XSetWindowBorderPixmap(dpy, awi->gadgets[i], 
						   awi->B_back);
		    }
	    }
	}
	else {
	    if (BorderHilite)
		/* Cached in SetFrameColors */
	        borderPixel = awi->grayPixel;
	        /* Default is a gray stipple on monochrome or solid gray on
		 * color servers if border.highlight is on and focus is out,
		 * else a solid borderColor color.
		 */
	    else    /* init */
		/* Cached in SetFrameColors */
		borderPixel = awi->WBorderPixel;
	    if(WBorderWidth)
		XSetWindowBorder(dpy, awi->client, borderPixel);
	    if (awi->frame && awi->title) {
		if(WBorderWidth)
		    XSetWindowBorder(dpy, awi->title, borderPixel);
	        if ((awi->attrs & AT_GADGETS) && GadgetBorder)
		    for (i = 0; i < NumGadgets && Gadgets[i]; i++) {
			if (awi->gadgets[i])
			    XSetWindowBorder(dpy, awi->gadgets[i], borderPixel);
		    }
	    }
	}
    }

    if ((BorderContextHilite || init) && awi->frame && BContext &&
	(awi->attrs & AT_BORDER)) {
       /* set the border context to defined pixmap, or "gray" stipple pixmap
	  on mono without BorderHilite */
	if (awi->BC_back) {
	    XSetWindowBackgroundPixmap(dpy, awi->frame, awi->BC_back);
	}
	else {
	    if (BorderContextHilite)
		/* Cached in SetFrameColors */
	        borderPixel = awi->grayPixel;
	        /* Default is a gray stipple on monochrome or solid gray on
		 * color servers if border.highlight is on and focus is out,
		 * else a solid borderColor color.
		 */
	    else    /* init */
		/* Cached in SetFrameColors */
		borderPixel = awi->BCForegroundPixel;
	    XSetWindowBackground(dpy, awi->frame, borderPixel);
	}
	XClearWindow(dpy, awi->frame);  /* make background repaint */
    }

    if ((Hilite || init) && awi->frame && awi->title) {
	if (awi->back)
	    XSetWindowBackgroundPixmap(dpy, awi->title, awi->back);
	else if (init)	/* no need to redo if no Hilite */
	    XSetWindowBackground(dpy, awi->title, awi->TBackgroundPixel);
	PaintTitle(awi->title, FALSE);
    }
    XFlush(dpy);    /* see changes fast */
    Leave(FALSE)
}


/*
 * Highlight a window. This may involve shuffling pixmaps around
 * and affecting more than just the client window.
 */
LightsOn(awi)
AwmInfoPtr awi;
{
    Pixel borderPixel;
    int i;

     Entry("LightsOn")

    if (BorderHilite) {
	if (awi->B_bold) {
	    if(WBorderWidth)
		XSetWindowBorderPixmap(dpy, awi->client, awi->B_bold);	    
	    if (awi->frame && awi->title) {
		if(WBorderWidth)
		    XSetWindowBorderPixmap(dpy, awi->title, awi->B_bold);
	        if ((awi->attrs & AT_GADGETS) && GadgetBorder)
		    for (i = 0; i < NumGadgets && Gadgets[i]; i++) {
			if (awi->gadgets[i])
			    XSetWindowBorderPixmap(dpy, awi->gadgets[i], 
						   awi->B_bold);
		    }
	    }
	}
	else {
	    /* Cached in SetFrameColors */
	    borderPixel = awi->WBorderPixel;
	    if(WBorderWidth)
		XSetWindowBorder(dpy, awi->client, borderPixel);
	    if (awi->frame && awi->title) {
		if(WBorderWidth)
		    XSetWindowBorder(dpy, awi->title, borderPixel);
	        if ((awi->attrs & AT_GADGETS) && GadgetBorder)
		    for (i = 0; i < NumGadgets && Gadgets[i]; i++) {
			if (awi->gadgets[i])
			    XSetWindowBorder(dpy, awi->gadgets[i], borderPixel);
		    }
	    }
	}
    }

    if (Hilite && awi->frame && awi->bold && awi->title) {
	XSetWindowBackgroundPixmap(dpy, awi->title,
					awi->bold);
	PaintTitle(awi->title, TRUE);
    }
    if (BorderContextHilite && awi->frame && BContext &&
	(awi->attrs & AT_BORDER)) {
	if (awi->BC_bold)
	    XSetWindowBackgroundPixmap(dpy, awi->frame,
					    awi->BC_bold);
	else
	    XSetWindowBackground(dpy, awi->frame, awi->BCForegroundPixel);
	XClearWindow(dpy, awi->frame);  /* make background repaint */
    }
    XFlush(dpy);    /* see changes fast */
    Leave(FALSE)
}
