#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: GetButton.c,v 5.48 89/11/22 15:10:16 glennw Exp $";
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
 * 001 -- L. Guarino Reid, DEC Ultrix Engineering Group, Western Software Lab
 *	  February 16, 1987
 *	  Add EnterWindow, LeaveWindow, and MouseMotion as recognized
 *	  awm buttons for awm menus. Add bug fixes to prevent mem faults
 *	  if icon_str is NULL.
 * 002 -- L. Guarino Reid, DEC Ultrix Engineering Group
 *	  April 16, 1987
 *	  Convert to X11
 * 003 -- J. Hubbard, U.C. Berkeley. Title/gadget box event handling.
 *    December 3, 1987.
 * 004 -- J.Hubbard, Ardent Computer. More conformance with ICCCM. Merge of
 * changes from R2 uwm.
 */

#ifndef LINT
static char *sccsid = "@(#)GetButton.c	3.8	1/24/86";
#endif
/*
 *	GetButton - This subroutine is used by the Ardent Window Manager (awm)
 *	to acquire button events.  It waits for a button event to occur
 *	and handles all event traffic in the interim.
 *
 *	File:		GetButton.c
 */

#include "awm.h"
#include "X11/Xutil.h"

extern XContext AwmContext;

#define PAD	1

static Icon_modified = FALSE;

static void CheckMap();

CheckUnRegister(b_ev)
XUnmapEvent *b_ev;	/* Button event packet. */
{
    Window win;
    AwmInfoPtr awi;

    Entry("CheckUnRegister")

    win = b_ev->window;
    /* could be from StructureNotify (Icon) or SubstructureNotify
       (client/frame) */
    if (!(awi = GetAwmInfo(win)))
        Leave_void
    /* if this is synthetic ("unmap request"), or if it is not an unmap from
       iconification or reparent (or other wm operation?)  */
    if (b_ev->send_event || 
        (!OutstandingReparents && !(awi->state & ST_ICON) && (win == awi->client))) {
        /* Client has asked to withdraw - must completely unregister */
        /* Maintaining compatibility with client withdrawing by a real Unmap
	 * operation is a pain - client window deletion throws us into the
	 * protocol error handler, and we have to XSync up after each wm op
	 * that unmaps the client window in order to be sure to ignore our
	 * own Unmaps.  Further, we must set a flag during Reparenting
	 * to detect Unmaps from reparenting!
	 */
	/* fprintf(stderr, "unregistering window %d, icon %d, awi->client %d\n", win, awi->icon, awi->client); */
	UnRegister(win, False); 
        /* set WM_STATE to Withdrawn for this window and undecorate */
    }
    Leave_void
}

UnRegister(win, win_destroyed)
Window win;
Boolean win_destroyed;	    /* is the client window destroyed? */
{
    AwmInfoPtr awi;
    XWindowAttributes xwa;

     Entry("UnRegister")

    if (!(awi = GetAwmInfo(win)))
	/* this will happen if we get a synthetic UnmapNotify after a real
	   one */
	Leave_void
    if (win != awi->title) {
	/* Remove any icon associated with this window.  Reset
	   WM_STATE if window is not being destroyed */
	RemoveIcon(win);
	/* Reset FocusWindow to root so that Colormap setting happens right.
	   Future EnterNotifys will finish resetting things. */
	if (FocusWindow == awi->client) {
	    FocusWindow = RootWindow(dpy, scr);
	    FocusSetByWM = FALSE;
	}
	/* if the window was sent a WM_SAVE_YOURSELF, if we get here the
	   client has "resigned from the session" by exiting, or 
	   has destroyed or unmapped its window, something not within the
	   ICCCM protocol.  Regardless, we must remove it from the kill 
	   list, and make sure it is dead so we can restart the session 
	   without any junk clients lying around. */
	KillClientNow(awi->client);
	if (!win_destroyed) {
	    awi->state &= ~(ST_ICON | ST_WINDOW);
	    /* window is being withdrawn by client */
	    UpdateWMState(awi);	/* will set WM_STATE.icon to NULL */
	    /* restore window position - copy of Cleanup() */
	    if (awi->frame) {
		XGetWindowAttributes(dpy, awi->frame, &xwa);
	    /* we must behave as if we were killed for a reliable result:
	       reparent client to be exactly where it is now (root-relative)
	       and leave it marked decorated for next restart */
		XSetWindowBorderWidth(dpy, awi->client, awi->border_width);
		/* unreparented windows (override_redirect, transient_for)
		   do not have their border widths set */
		if (awi->attrs & AT_TITLE)
		    xwa.y += titlePositionExcess;
		if (awi->attrs & AT_BORDER) {
		    xwa.y += borderPositionExcess;
		    xwa.x += borderPositionExcess;
		}
	    /* subtract out original gravity effect from frame position */
            /* OutstandingReparents++; Don't do this!!! */
		XReparentWindow(dpy, awi->client, RootWindow(dpy, scr),
				xwa.x, xwa.y);
	    /* this Reparent won't catch up in CheckUnRegister because we are
	       deleting awi.  On the other hand, the ReparentNotify won't
	       appear if the client window is destroyed right away... */
		XRemoveFromSaveSet(dpy, awi->client);
	    }
	}
	/* remove the frame/titlebar (if present) */
	/* Future OPTimization:	if (!win_destroyed), save decorations
	   in case this window gets remapped */
	NoDecorate(win, TRUE);
	/* punt the rest of the per-window info */
	XDeleteContext(dpy, awi->client, AwmContext);
	if (awi->wm_hints)
	    XFree(awi->wm_hints);
	if (awi->winGC)
	    XFree(awi->winGC);
	free(awi);
    }
    Leave_void
}

UpdateIconName(win_info, icon, icon_str, icon_w, icon_str_len)
    XWindowAttributes *win_info;	/* Icon window info structure. */
    Window icon;
    char **icon_str;
    unsigned int icon_w;		/* icon window width */
    int icon_str_len;			/* Icon name string length.  */
{
    AwmInfoPtr awi;

    Entry("UpdateIconName")

    if (!(awi = GetAwmInfo(icon)))
	Leave_void
    if (win_info->width != icon_w) {
    /* keep icon text window centered */
    XMoveResizeWindow(dpy, icon, win_info->x + 
				 ((win_info->width - icon_w) / 2), 
				 win_info->y,
				 icon_w, win_info->height);
    }
    XClearWindow(dpy, icon);
    if (icon_str_len != 0) {
    XDrawImageString(dpy, icon, IconGC, HIconPad, VIconPad+IFontInfo->ascent,
		     *icon_str, icon_str_len);
	/* throw the name on the icon for xwininfo */
	if (*icon_str != UnnamedIcon) {
	    *icon_str = realloc (*icon_str, icon_str_len + 8);
	    /* Don't forget NULL */
	    strcpy(*icon_str + icon_str_len, "'s Icon");
	}
    }
    else {
	/* this should be unreachable */
	Warning ("UpdateIconName (GetButton.c): Got a nameless icon!");
	*icon_str = "nameless icon";
	XStoreName(dpy, icon, *icon_str);
	if (awi->client_icon)
	    XStoreName(dpy, awi->client_icon, *icon_str);
	Leave_void
    }
    XStoreName(dpy, icon, *icon_str);
    if (awi->client_icon)
	XStoreName(dpy, awi->client_icon, *icon_str);
    /*
     * Remember to free the icon name string if it was alloced.
     */
    if (*icon_str != UnnamedIcon)
	free(*icon_str);
    Leave_void
}

Boolean GetButton(b_ev)
XEvent *b_ev;	/* Button event packet. */
{
     char *icon_str;		/* Icon's name string. */
     int icon_str_len;		/* Icon name string length.  */
#ifndef NO_ICON_TYPIN     
#define STRLEN 50
     XKeyPressedEvent *kp_event;/* Key pressed event. */
     register int key_char;	/* Key press character code. */
     register int icon_x;	/* Icon window X coordinate. */
     register int icon_y;	/* Icon window Y coordinate. */
     char kbd_str[STRLEN];      /* Keyboard string. */
     int nbytes;                /* Keyboard string length. */
     int i;                     /* Iteration counter. */
     Window iW;
     int ix, iy;
     unsigned int border_width, iw, ih;
#endif /* NO_ICON_TYPIN */
     unsigned int icon_w;	/* Icon window width. */
     unsigned int icon_h;	/* Icon window height. */    
     int status;		/* Routine call return status. */
     Window icon;		/* Icon window. */
     XWMHints *wm_hints;
     XWindowAttributes win_info;	/* Icon window info structure. */
     Window win;		/* scratch */
     AwmInfoPtr awi;
     int argc;			/* Count of command line parameters. */
     char **argv;		/* Pointer to command line parameters. */
     Entry("GetButton")
  
     /*
      * Get next event from input queue and store it in the event packet
      * passed to GetButton.
      */
  
     /*	YUK!!  In order to catch UnmapNotifys that we caused before processing
	another (de)iconify button event, handle them immediately */
     if (XCheckTypedEvent(dpy, UnmapNotify, b_ev)) {
	 if (EventDebug)
	     print_event_info("mainloop (flushing Unmaps): ", b_ev);
	 CheckUnRegister((XUnmapEvent *)b_ev);
	 /* don't hang if this is the only outstanding event... */
	 Leave(FALSE);
     }
     
     XNextEvent(dpy, b_ev);
     if (EventDebug)
	 print_event_info("mainloop", b_ev); /* debugging for event handling */

     /*
      * The event occured on the root window, check for substructure
      * changes. Otherwise, it must be a mouse button event. 
      */

     if (b_ev->xany.window == RootWindow(dpy, scr)) {
	    /* warning: xany->window is the event request window, not the
	       subject window for many events */
	  switch (b_ev->xany.type) {
	  case CreateNotify:
	  case ConfigureNotify:
	  case GravityNotify:
	  case CirculateNotify:
	       Leave(FALSE)
		    
	  case MappingNotify:
	       XRefreshKeyboardMapping(b_ev);
	       Leave(FALSE)

	  case MapRequest: 
		/* map notify xany.window field is "event" window (root) */
	       CheckMap(b_ev->xmap.window);
	       Leave(FALSE)

	  case MapNotify:
	       /* handle override_redirect windows */
	        /* not sure this is really needed */
	       /* XXX really need to cache this */
	       status = XGetWindowAttributes(dpy, b_ev->xmap.window, &win_info);
	       if (status == FAILURE)
		   Leave(FALSE)
	       if (win_info.override_redirect)
		   CheckMap(b_ev->xmap.window);
	       Leave(FALSE)

	  case ConfigureRequest: 
	       Configure((XConfigureEvent *)b_ev);
	       Leave(FALSE)
	       
	  case CirculateRequest: 
	       Circulate((XCirculateEvent *)b_ev);
	       Leave(FALSE)
	       
	  case UnmapNotify:
	       /* not sure this is really needed */
	       CheckUnRegister((XUnmapEvent *)b_ev);
	       Leave(FALSE)
	       
	  case ReparentNotify:
	       /* process only the copy obtained from the frame window */
	       Leave(FALSE)

	  case DestroyNotify:
	       win = b_ev->xdestroywindow.window;
		/* destroy notify xany.window field is "event" window (root) */
	       /* don't assume that all client windows are in frames...
		  requires SubstructureNotifyMask */
               if (!(awi = GetAwmInfo(win)))
	           Leave(FALSE)
	       if (IsIcon(win, NULL))
		   /* icon destroyed (normally by client) - cleanup */
		   RemoveIcon(win);
	       else
		   UnRegister(win, True);
 	       Leave(FALSE)

          case EnterNotify:
	      last_server_time = b_ev->xcrossing.time;
	      /* install colormaps here, since HandleFocusIn is not called. */
	      /* could we not do this off FocusIn, and install DefaultCmap? */
              if (InstallColormap) {
		  /* must re-install User Focus colormap in case it was
		     de-installed due to a menu.  Otherwise, this has no
		     effect. */
	          if (!(awi = GetAwmInfo(FocusWindow))) {
		      (void) sprintf(error_msg, "GetButton - FocusWindow '%x' not registered!", FocusWindow);
		      Warning(error_msg);
		      Leave(FALSE)
		  }
		  XInstallColormap(dpy, awi->xwa.colormap);
	      }
	      Leave(FALSE)

	  case FocusIn:
	       if (b_ev->xfocus.detail == NotifyPointerRoot && !NoFocus) {
		    if (FocusSetByUser) {
			 /* convert user focus back to pointer root if a client
			    grabs the focus??? */
			 XSetInputFocus(dpy, PointerRoot, None,
					CurrentTime);
			 FocusSetByUser = FALSE;
			 FocusWindow = RootWindow(dpy, scr);
		    }
	       }
	       /* install colormaps here, since HandleFocusIn is not called. */
               if (InstallColormap) {
		   /* must re-install User Focus colormap in case it was
		      de-installed due to a menu.  Otherwise, this has no
		      effect. */
	           if (!(awi = GetAwmInfo(FocusWindow))) {
		       (void) sprintf(error_msg, "GetButton - FocusWindow '%x' not registered!", FocusWindow);
		       Warning(error_msg);
		       Leave(FALSE)
		   }
		   XInstallColormap(dpy, awi->xwa.colormap);
	      }
	       Leave(FALSE)
	       
	  case FocusOut:
	       if (b_ev->xfocus.detail == NotifyPointerRoot && !NoFocus) {
                    if (!FocusSetByUser && !FocusSetByWM) {
   	                 XSetInputFocus(dpy, PointerRoot, None, CurrentTime);
			 FocusWindow = RootWindow(dpy, scr);
		    }
	       }
	       Leave(FALSE)

	  case ButtonPress:
	  case ButtonRelease:
	       last_server_time = b_ev->xbutton.time;
	       Leave(TRUE)

	  default: 
	       Leave(FALSE) 
	  }
     }
     
     /*
      * If the event type is EnterWindow, LeaveWindow, or MouseMoved,
      * we are processing a menu or following mouse for autoraise.
      * If the event type is ButtonPress or ButtonRelease,
      * we have a button event.
      * If it's an expose, then we may have exposed a title bar.
      * If it's a Notify, we've probably frobbed a titled window.
      */
     
     switch (b_ev->type) {
	  XEvent e_fake;

     case MotionNotify: 
	  last_server_time = b_ev->xmotion.time;
	  if(GetAwmInfo(b_ev->xmotion.window))
	      Leave(FALSE)
	      /* eat useless motion over client windows just in case... */
	  else
	      Leave(TRUE)

     case ButtonPress: 
     case ButtonRelease: 
	  last_server_time = b_ev->xbutton.time;
	  Leave(TRUE)

     case FocusIn:
	  if (!FocusSetByWM) {
	       e_fake.xcrossing.type = FocusIn;
	       e_fake.xcrossing.focus = TRUE;
	       e_fake.xcrossing.window = b_ev->xfocus.window;
	       HandleFocusIn(&e_fake);
	  }
	  Leave(FALSE)

     case FocusOut:
	  if (!FocusSetByWM) {
	       e_fake.xcrossing.type = FocusOut;
	       e_fake.xcrossing.focus = TRUE;
	       e_fake.xcrossing.window = b_ev->xfocus.window;
	       HandleFocusOut(&e_fake);
	  }
	  Leave(FALSE)
	  
     case EnterNotify:
	  last_server_time = b_ev->xcrossing.time;
	  HandleFocusIn(b_ev);
	  Leave(TRUE)

     case LeaveNotify: 
	  last_server_time = b_ev->xcrossing.time;
	  HandleFocusOut(b_ev);
	  Leave(TRUE)

     case ConfigureRequest:
	  Configure((XConfigureEvent *)b_ev);
	  Leave(FALSE)

     case VisibilityNotify:
	  UpdateVisibility((XVisibilityEvent *)b_ev);
	  Leave(FALSE);

     case ColormapNotify:
	  ReAllocateColors((XColormapEvent *)b_ev);
	  Leave(FALSE)

     case MapRequest:
	  CheckMap(b_ev->xmap.window);
	  Leave(FALSE);

     case MapNotify:
	  /* handle override_redirect windows */
          status = XGetWindowAttributes(dpy, b_ev->xmap.window, &win_info);
	  if (status == FAILURE)
	      Leave(FALSE)
	  if (win_info.override_redirect)
	  /* don't unregister override_redirect windows */
	      CheckMap(b_ev->xmap.window);
          Leave(FALSE)
	  
     case UnmapNotify:
	  CheckUnRegister((XUnmapEvent *)b_ev);
	  Leave(FALSE)

     case ReparentNotify:
	  /* OK, we got a ReparentNotify that matches an UnmapNotify - other
	     Unmaps should be from the client (We will get this event on the
	     frame as well, but we only need one, and we want to ignore any
	     ReparentNotifies that result from destroying the frame.) */
          if (!(awi = GetAwmInfo(b_ev->xreparent.event)))
               Leave(FALSE)
          OutstandingReparents--;
       Leave(FALSE)

     case DestroyNotify:
	  /* can we get a destroy on an icon here (we use StructureNotify)? */
	  UnRegister(b_ev->xdestroywindow.window, True);
          Leave(FALSE)

     case ClientMessage:
	  if ((b_ev->xclient.message_type == WmProtocols) &&
	      (b_ev->xclient.window == WMSMWindow) &&
	      (b_ev->xclient.data.l[0] == WmPrepareToDie)) {
	  /* we've been asked to die - cleanup and wait patiently for the ax */
	     Cleanup();
	  /* update WM_COMMAND on our special wm property window, signaling
	     that we are ready to die. Note that on f.restart we do NOT want
	     to do the -e option, but here we do.  So restore it. */
	     argc = Argc;
	     argv = Argv;
	     while (argc--) {
		 if (!(strcmp(*argv, "--"))) {
		     **(argv + 1) = 'e';
		 }
	    	 argv++;
	     }
	     XSetCommand(dpy, WMSMWindow, Argv, Argc);
	     XSync(dpy, FALSE);
	     /* Theoretically, we should be able to restart if we get
	      * more input.  But I don't trust this at the moment.
	      * Protocol says we should wait for session manager to kill us.
	      */
	      Leave(FALSE)
	  }
	  if ((b_ev->xclient.message_type == WmChangeState) &&
	      (b_ev->xclient.data.l[0] == IconicState)) {
	      /* request to move from Normal to Iconic.  Ignore if not in
	         Normal state (not mapped) */
              if (!(awi = GetAwmInfo(b_ev->xclient.window)))
                  Leave(FALSE)
	      if (awi->state & ST_WINDOW) {
	          /* calling Iconify with zeros causes it to not zap/warp.
		     Must take existing icon placement, no warping, could
		     zap, but would require new call flag, so we don't */
		  Iconify (b_ev->xclient.window, 0, 0, 0, 0);
	      }
	  }
          Leave(FALSE)

     case PropertyNotify:
	  last_server_time = b_ev->xproperty.time;
	  win = b_ev->xproperty.window;
          if (!(awi = GetAwmInfo(win)))
               Leave(FALSE)
	  switch(b_ev->xproperty.atom) {

	  case XA_WM_COMMAND:
	       /* If client was told to prepare to die, kill it */ 
	       KillClientNow(b_ev->xproperty.window);
	       break;

	  case XA_WM_HINTS:
	       /* update wm_hints cache */
	       if (wm_hints = XGetWMHints(dpy, win)) {
		   if (awi->wm_hints)
		       XFree(awi->wm_hints);
		   awi->wm_hints = wm_hints;
	       }
	       else {
		   /* might be deleted - same as no flags set */
		   if (!awi->wm_hints)
		       awi->wm_hints = XAllocWMHints();
		   awi->wm_hints->flags = 0;
	       }
	  case XA_WM_CLIENT_MACHINE:
	       break;

	  case XA_WM_ICON_NAME:
	       /*
		* Icon was modified by type-in (I still think that's a
		* gross feature, but some people like it... sigh),
		* ignore this event.
		*/
	       if (Icon_modified == TRUE) {
		    Icon_modified = FALSE;	/* reset */
		    Leave(FALSE)
	       }
	       if (awi->icon) {
		    icon = awi->icon;
		    status = XGetWindowAttributes(dpy, icon, &win_info);
		    icon_str = GetDefaultIconSize(awi->client, &icon_w, 
						 &icon_h, &icon_str_len, TRUE);
		    UpdateIconName(&win_info, icon, &icon_str, icon_w, 
				   icon_str_len);
	       }
	       break;

	  case XA_WM_ICON_SIZE:
	       break;

	  case XA_WM_NAME:
	       if (awi->title) {
		    if (awi->name && (awi->name != untitledWindowName))
			 free(awi->name);
		    awi->name = (char *)GetTitleName(awi->client);
		    PaintTitle(win, (FocusWindow == awi->client));
	       }
	       break;

	  case XA_WM_NORMAL_HINTS:
	  case XA_WM_SIZE_HINTS:
	  case XA_WM_ZOOM_HINTS:
	       break;

	  /* default:
	       fprintf(stderr, "tekwm: Got unknown property %d\n",
		       b_ev->xproperty.atom); */
	  }
	  Leave(FALSE)
	  
     case Expose:
	  win = b_ev->xexpose.window;
          if (!(awi = GetAwmInfo(win))) /* probably a menu */
               Leave(FALSE)
	  if (awi->title == win) {
	       if (b_ev->xexpose.count == 0) {
		    XEvent evt;

		    /* Eat up any previous exposes */
		    while (XCheckTypedWindowEvent(dpy, awi->title, Expose,
						  &evt))
			if (EventDebug)
			    print_event_info("mainloop", &evt);
		    PaintTitle(win, (FocusWindow == awi->client));
		    if (awi->gadgets)
			 PaintGadgets(win);
	       }
	       Leave(FALSE)
	  }
	  break;
	  
     default:
	  break;
     }
     
     /*
      * If we got this far, the event must be for an icon.
      */
     if (!(awi = GetAwmInfo(b_ev->xany.window)))
          Leave(FALSE)
     /* get attributes of icon name window */
     icon = awi->icon;
     if (!awi->icon)
	  Leave(FALSE)
     status = XGetWindowAttributes(dpy, icon, &win_info);
     if (status == FAILURE)
	  Leave(FALSE)
     
     if (b_ev->type == MapNotify || 
	 b_ev->type == UnmapNotify ||
	 b_ev->type == CreateNotify ||
	 b_ev->type == ReparentNotify ||
	 b_ev->type == GravityNotify ||
	 b_ev->type == CirculateNotify ||
	 b_ev->type == MappingNotify ||
	 b_ev->type == ConfigureNotify)
	  Leave(FALSE)
     
     /*
      * Initialize the icon position variables.
      */
#ifndef NO_ICON_TYPIN     
     icon_x = win_info.x;
     icon_y = win_info.y;
#endif /* NO_ICON_TYPIN */
     
     /*
      * Get the name of the window associated with the icon and
      * determine its length.
      */

     icon_str = GetDefaultIconSize(awi->client, &icon_w, &icon_h, 
				   &icon_str_len, TRUE);
     
     /*
      * If the event is a window exposure event and the icon's name string
      * is not of zero length, simply repaint the text in the icon window
      * and return FALSE.  Note that this is how the icon name gets painted the
      * first time!
      * Don't redraw until we get the last expose event. 
      */
     if (b_ev->type == Expose && (b_ev->xexpose.count == 0)
	 && (!Freeze || Frozen == 0)) {
	  UpdateIconName(&win_info, icon, &icon_str, icon_w, icon_str_len);
	  Leave(FALSE)
     }

#ifndef NO_ICON_TYPIN     
     /*
      * If we have gotten this far event can only be a key pressed event.
      */
     if (b_ev->type != KeyPress) {
	fprintf(stderr, "tekwm: Got unexpected event on icon: ignoring it.\n");
	Leave(FALSE)
     }
	
     kp_event = (XKeyPressedEvent *) b_ev;
     last_server_time = b_ev->xkey.time;
     
     /* 
      * We convert the key pressed event to ascii.
      */
     nbytes = XLookupString(kp_event, kbd_str, STRLEN, NULL);
     
     /*
      * If kbd_str is a "non-string", then don't do anything.
      */
     if (nbytes == 0) {
	  if (icon_str && (icon_str != UnnamedIcon))
	       free(icon_str);
	  Leave(FALSE)
     }
     for (i = 0; i < nbytes; i++) {
	  key_char = kbd_str[i];
	  /*
	   * If the key was <DELETE>, then delete a character from the end of
	   * the name, return FALSE.
	   *
	   * If the key was <CTRL-U>, then wipe out the entire window name
	   * and return FALSE.
	   *
	   * All other ctrl keys are squashed and we return FALSE.
	   *
	   * All printable characters are appended to the window's name, which
	   * may have to be grown to allow for the extra length.
	   */
	  if (key_char == '\177') {
	       /*
		* <DELETE>
		*/
	       if (icon_str_len > 0) {
		    icon_str_len--;
		    icon_str[icon_str_len] = '\0';
	       }
	  }
	  else if (key_char == '\025') {
	       /*
		* <CTRL-U>
		*/
	       if (icon_str_len > 0) {
		    icon_str_len = 0;
		    icon_str[0] = '\0';
	       }
	  }
	  else if (key_char < IFontInfo->min_char_or_byte2 ||
		   key_char > IFontInfo->max_char_or_byte2) {
	       /*
		* Any other random (non-printable) key; ignore it.
		*/
	       /* do nothing */ ;
			   }
	  else {
	       /*
		* ASCII Alphanumerics.
		*/
	       if ((icon_str == NULL) || (icon_str == UnnamedIcon)) {
		   icon_str = (char *) malloc (icon_str_len + 2);
		   if (icon_str == UnnamedIcon)
			strcpy(icon_str, UnnamedIcon);
	       }
	       else
		    icon_str = (char *) realloc(icon_str, (icon_str_len + 2));
	       if (icon_str == NULL) {
		    errno = ENOMEM;
		    Error("GetButton -> Realloc of window name string memory failed.");
	       }
	       icon_str[icon_str_len] = key_char;
	       icon_str[icon_str_len + 1] = '\0';
	       icon_str_len += 1;
	  }
     }
     
     /*
      * Now that we have changed the size of the icon we have to reconfigure
      * it so that everything looks good.  Oh yes, don't forget to move the
      * mouse so that it stays in the window!
      */
     
     /*
      * Set the window name to the new string. Icon_modified is a kludge
      * that tells us to avoid the next PropertyNotify, as it's a result of
      * this call.
      */
     XSetIconName(dpy, awi->client, icon_str);
     Icon_modified = TRUE;
     /*
      * Determine the new icon window configuration.
      */
     GetIconSize(icon_str, &icon_w, &icon_h, &icon_str_len);
     
     if (awi->client_icon) {
	if (!XGetGeometry(dpy, awi->client_icon, &iW, &ix, &iy,
		  &iw, &ih, &border_width, &i)) {
	    /* major hosed! */
	    Warning("tekwm: internal error: client icon window destroyed!");
	    Leave(FALSE)
	}
	/* move name icon to re-center on client icon */
	icon_x = ix + (((int) iw - (int) icon_w) / 2);
     }
     else {
	/* don't force on-screen if we have to move client window */
	if (icon_x < 0) icon_x = 0;
	if (icon_y < 0) icon_y = 0;
	if (icon_x - 1 + icon_w + (IBorderWidth << 1) > ScreenWidth) {
	    icon_x = ScreenWidth - icon_w - (IBorderWidth << 1) + 1;
	}
	if (icon_y - 1 + icon_h + (IBorderWidth << 1) > ScreenHeight) {
	    icon_y = ScreenHeight - icon_h - (IBorderWidth << 1) + 1;
	}
     }
     
     XMoveResizeWindow(dpy, icon, icon_x, icon_y, icon_w, icon_h);
     XWarpPointer(dpy, None, icon, 
		  0, 0, 0, 0, (icon_w >> 1), (icon_h >> 1));
     
     /* 
      * Free the local storage and return FALSE.
      */
     if (icon_str && (icon_str != UnnamedIcon))
	  free(icon_str);
#endif /* NO_ICON_TYPIN */
     Leave(FALSE)
}

static void
CheckMap(window)
Window window;
{
     XWMHints *XGetWMHints();
     XWindowAttributes xwa;
     Window transient;
     Binding *bptr;
     AwmInfoPtr awi;
     unsigned long event_mask;
     Boolean from_wm;

     Entry("CheckMap")

     if (XGetWindowAttributes(dpy, window, &xwa) == FAILURE)
	 Leave_void
     if (xwa.class != InputOutput)
	 Leave_void	/* don't mess with InputOnly windows */
     /*
      *	Handle any registered grabs for this window.  We do this here because
      * we may be about to make an icon out of this window and we want to
      * register any potential grabs on it before this happens.
      */
     awi = GetAwmInfo(window);
     if (!awi) {
         /* a new client window! */
	 for (bptr = Blist; bptr; bptr = bptr->next)
	     if (bptr->context & WINDOW)
		 Grab(bptr->mask, window);
	 /* select for ColormapChange before caching awi->xwa.colormap */
	 event_mask = PropertyChangeMask | FocusChangeMask;
	 if (InstallColormap)
	     event_mask |= ColormapChangeMask;
	 XSelectInput(dpy, window, event_mask);
	 awi = RegisterWindow(window, FALSE,
			      &xwa);
	 /* more efficient to place before Decorating, but don't place if
	    initially iconic or do either if transient_for, except for border
	    width/color.  Note that everything else must work on transient_for
            windows. */
         if ((xwa.override_redirect && !(awi->attrs & (AT_TITLE | AT_BORDER)))
	     /*	client can request that override_redirect window be decorated
		via WM_CLASS and wm_options resources */
	     || XGetTransientForHint(dpy, window, &transient)) {
    /* Jordan's patch 7 says Place the window; I don't yet see a good 
       reason for this - the only real effects are wasted time and setting
	USxxx in the size hints, which are ignored on transient wins.
        Note - PlaceWindow assumes it is not getting a Transient or O.R.! */
	/*     awi->attrs = AT_NONE;
	     if (transient == (Window) None)
	         transient = RootWindow(dpy, scr);
	     PlaceWindow(window, transient, &from_wm); */
	     awi->state |= ST_PLACED;
	     SetFrameColors(awi);   /* Actually, create client border pixmap */
	 }
	 else {
	     from_wm = FALSE;  /* kludge - tell Decorate that this is
		client-supplied position unless PlaceWindow says otherwise */
	     if ((awi->state & ST_WINDOW) && !xwa.override_redirect)
		 /* don't ever place override_redirect wins */
		 if (! PlaceWindow(window, (Window) None, &from_wm)) {
		     /* window is destroyed; go ahead and trash awm info now */
		     UnRegister(window, True);
		     Leave_void
		 }
	     if (xwa.override_redirect)
		 awi->state |= ST_PLACED;
	     /*	if it's a transient window, we won't rubber-band.  Note that
	      * this call always sets transient.
	      */
	     if (!Decorate(window, from_wm)) {
		/* no frame created - client window must have gone away */
		 UnRegister(window, True);
		 Leave_void
	     }
	 }
	 /* it's not mapped yet... */
	 /* now select rest of input depending on reparenting */
	 if (!awi->frame || !FrameFocus)
	     event_mask |= (EnterWindowMask | LeaveWindowMask);
	 if (!awi->frame)
	     event_mask |= VisibilityChangeMask;
	 if (awi->attrs & AT_RAISE)
	     event_mask |= PointerMotionMask;
	 XSelectInput(dpy, window, event_mask);
	 LightsOff(awi, TRUE);
	 /* initialize until we get a focus in after mapping */
	 if (awi->state & ST_ICON) {
	     /*	Window wants to be created as an icon.  
		Leave size and position alone, create as icon. */
             /* Fool Iconify about the state */
	      awi->state ^= ST_ICON;    
	      awi->state |= ST_WINDOW;	
	     /* calling Iconify with zeros causes it to not zap or warp */
              Iconify(window, 0, 0, 0, 0);
	      Leave_void
	 }
	 else
	     /* window wants to be Normal.  Set WM_STATE. */
	     UpdateWMState(awi);
     }
     else if (!xwa.override_redirect) {
     /*	MapRequest on an existing window - the client wants to go from iconic
	to normal.  In this case, we are to ignore initial_state. */
	 if (awi->state & ST_ICON) {
	     Iconify(window, 1, 0, 0, 0);	/* de-iconify */
	     /* do not raise the window */
	     /* do zap and warp - mask is otherwise ignored in Iconify*/
	 }
	 /* if already de-iconified, do nothing */
	 Leave_void
     }
     if ((awi->state & ST_WINDOW) && !xwa.override_redirect) {
	  if (awi->frame) {
	      XMapRaised(dpy, awi->frame);
	      XMapWindow(dpy, awi->client);
	  }
	  else
	      XMapRaised(dpy, awi->client);
	      /* may be undecorated and have no frame some day */
	  XFlush(dpy);
     }
     Leave_void
}

Configure(event)
XConfigureRequestEvent *event;
{
     XWindowChanges values;
     
     Entry("Configure")

     values.x = event->x;
     values.y = event->y;
     values.width = event->width;
     values.height = event->height;
     values.border_width = event->border_width;
     values.stack_mode = event->detail;
     values.sibling = event->above;
     /* yes, these fields really are the same! */

     ConfigureWindow(event->window, event->value_mask, &values, FALSE);
     Leave_void
}

Circulate(event)
XCirculateEvent *event;
{
     Entry("Circulate")

     if (event->place == PlaceOnTop)
	  XRaiseWindow(event->display, event->window);
     else
	  XLowerWindow(event->display, event->window);
     Leave_void
}

ReAllocateColors(event)
XColormapEvent * event;
{
    AwmInfoPtr awi;
    int i;

    Entry("ReAllocateColors")

    if (! event->new)
	Leave_void
    if (! (awi = GetAwmInfo(event->window)) || event->window != awi->client)
	Leave_void

    /* we have a client colormap change - cache it and reset colors! */
    if (event->colormap) {
	/* awi->client_cmap = event->colormap; */
	/* R3 server bug - colormap field was bogus - should be fixed,
	   but just to be safe... */
	XGetWindowAttributes(dpy, awi->client, &(awi->xwa));
	awi->client_cmap = awi->xwa.colormap;
    }
    else
	/* use Default if client colormap is None (i.e. freed) */
	awi->xwa.colormap = DefaultColormap(dpy, scr);
	awi->client_cmap = awi->xwa.colormap;
    SetTitleColors(awi);
    SetFrameColors(awi);
    SetGadgetColors(awi);
    /* now repaint everything with the new pixels, installing colormap if
       this is the colormap window and Focus is already set */
    if (FocusWindow == awi->client) {
        if (InstallColormap) {
	    /* XXX minor optimization - keep track of installed colormap
	       and only install if changed (also in GetButton.c) */
	    XInstallColormap(dpy, awi->xwa.colormap);
	}
	LightsOn(awi);
    }
    else
	LightsOff(awi, FALSE);	    /* Assume borders already initialized */
    PaintTitle(awi->client, (FocusWindow == awi->client));
    if (awi->gadgets) {
	/* make gadget window background repaint */
	for (i=0; awi->gadgets[i]; i++)
	    XClearWindow(dpy, awi->gadgets[i]);
	PaintGadgets(awi->client);
    }
    Leave_void
}

UpdateVisibility(event)
XVisibilityEvent * event;
{
    AwmInfoPtr awi;

    Entry("UpdateVisibility")

    awi = GetAwmInfo(event->window);
    if (!awi)
	Leave_void
    if (event->window == awi->icon)
	awi->icon_visibility = event->state;
    else if (event->window == awi->client_icon)
	awi->client_icon_visibility = event->state;
    else
	awi->visibility = event->state;
    Leave_void
}

    /* position the window, abort if window is destroyed */
Bool PlaceWindow(window, transient, from_wm)
Window window;
Window transient;
Boolean *from_wm;
{
     XSizeHints wsh;
     int deltax, deltay;
     XWindowChanges xwc;
     AwmInfoPtr awi;
     unsigned long flags;
     Window jW;
     int j, border_width;

     Entry("PlaceWindow")

     flags = CWX | CWY | CWWidth | CWHeight;
     awi = GetAwmInfo(window);
     /* in new ICCCM, default size is to be obtained from current geometry,
	and the old size_hints.x/y/w/h is ignored */
     /* XXX Optimization - Start caching geometry here? */
     if (!XGetGeometry(dpy, awi->client, &jW, &xwc.x, &xwc.y, 
		  &xwc.width, &xwc.height, &border_width, &j))
	 /* client window died before completion - abort */
	Leave(FALSE)
     /* Note - I believe that if the window previously had a frame, we can
        still use the client window geometry for positioning */
     if (awi->frame) {
        /* we can be called from Iconify with the frame (already decorated) */
        window = awi->client;
        /* XXX Optimization - cache geometry */
	XGetGeometry(dpy, awi->frame, &jW, &xwc.x, &xwc.y, &j, &j, &j, &j);
        /* if framed we must revert to the client-requested position */
	ComputeGravity(awi, &deltax, &deltay);
	xwc.x -= deltax;
	xwc.y -= deltay;
     }
     if (!XGetNormalHints(dpy, window, &wsh))
	 wsh.flags = 0;

     CheckConsistency(&wsh);
     if (!AskUser(dpy, scr, window, &xwc.x, &xwc.y, &xwc.width, &xwc.height,
		  &wsh, transient, from_wm))
	 /* client window died before completion - abort */
	 Leave(FALSE)

     ConfigureWindow(window, flags, &xwc, *from_wm);
     /* modify client's hints so we don't re-query the user on restart.
        Also, if the position was from wm, set win_gravity so that 
        original client border width has no effect on position on restart. */
     wsh.flags |= (USPosition | USSize);
     if (*from_wm) {
         wsh.flags &= ~PWinGravity;	/* i.e. NorthWestGravity */
	 awi->win_gravity = NorthWestGravity;	 
     }
     if (awi->frame)
	  XSetNormalHints(dpy, awi->frame, &wsh);
     XSetNormalHints(dpy, awi->client, &wsh);
     awi->state |= ST_PLACED;
     Leave(TRUE)
}

SendSyntheticConfigureNotify(awi, xwc)
AwmInfoPtr awi;
XWindowChanges *xwc;
{
    static XConfigureEvent Event;
    static Bool uninitialized = True;

    if (uninitialized) {
        Event.type = ConfigureNotify;
        Event.override_redirect = False;
        /* or we would not have seen it. */
        uninitialized = False;
    }
    Event.window = Event.event = awi->client;
    Event.x = xwc->x + WBorderWidth - awi->border_width;
    Event.y = xwc->y + WBorderWidth - awi->border_width;
    /* "adjust" so that x,y + client-requested width = real inner position */
    Event.width = xwc->width;
    Event.height = xwc->height;
    Event.border_width = awi->border_width;
	/* final ICCCM says that we should send client-requested width */
    Event.above = None;
    /* should we find out stacking result of frame configure? */
    /* currently, the ICCCM explicitly does not specify this field */
    XSendEvent(dpy, awi->client, False, StructureNotifyMask, &Event);
    /* no need to propogate to ourself! */
    Leave_void
}

Boolean ConfigureWindow(w, mask, xwc, from_wm)
Window w;
unsigned long mask;
XWindowChanges *xwc;
Boolean from_wm;	/* user configure via wm? */
{
     XWindowChanges config_request;
     AwmInfoPtr awi;
    Window root;
    int client_x, client_y;
    unsigned int client_width, client_height;
    unsigned int client_depth;
    int deltax, deltay;
    XSizeHints wsh;
    Window wjunk;
    int ijunk, tempx, tempy;
    Bool gotBorderRequest = FALSE;

     Entry("ConfigureWindow")

     awi = GetAwmInfo(w);
     if (!awi) {
	 /* client configure of a window we have not registered - do what he
	    sez */
	 XConfigureWindow(dpy, w, mask, xwc);
	 Leave(TRUE)
     }

     if (mask & CWBorderWidth) {
	 awi->border_width = xwc->border_width;
	 /* Stash new client-specified border width.  Original
	  * client-specified border width stashed in Decorate.  We never
	  * change border width, and don't let client do it either */
	 mask &= ~CWBorderWidth;
	 gotBorderRequest = TRUE;
     }
     config_request = *xwc;
     if (awi->frame) {
	 XGetGeometry(dpy, (Drawable)awi->client, &root, &client_x,
		      &client_y, &client_width, &client_height,
		     &ijunk, &client_depth);
	 /* XXX This should be cached in awi */
	 /* prepare to reconfigure the frame */
	 if (from_wm) {
	     /*	convert client-requested position to frame.  Warning - this
		code assumes that from_wm is always with mask = CWX, CWY,
		CWWidth, CWHeight */
	     if (awi->attrs & AT_TITLE)
		 config_request.y -= titlePositionExcess;
	     if (awi->attrs & AT_BORDER) {
		 config_request.y -= borderPositionExcess;
		 config_request.x -= borderPositionExcess;
	     }
	 }
	 else {
	     /* if actually requested by client, obey window gravity hint */
	     if (XGetNormalHints(dpy, awi->client, &wsh) && 
		 (wsh.flags & PWinGravity))
		 awi->win_gravity = wsh.win_gravity;
	     else
		 awi->win_gravity = NorthWestGravity;
	     /* Note that awi->border_width is set when first Decorated 
		(tekwm startup) or first Configured (initial map), so
	        no need to do so here unless client-requested */
	     /* convert position to frame, taking into account gravity hint */
	     ComputeGravity(awi, &deltax, &deltay);
	     /*	XXX if client did not specify a new position, use current frame
		position.  Warning - this code assumes that from_wm is always
		with mask = CWX, CWY, CWWidth, CWHeight */
	     if ((mask & (CWX | CWY)) != (CWX | CWY))
		 XGetGeometry(dpy, awi->frame, &wjunk,
			      &tempx, &tempy, 
			      &ijunk, &ijunk, &ijunk, &ijunk);
	     if (mask & CWX)
		 config_request.x += deltax;
	     else
		 config_request.x = tempx;
	     if (mask & CWY)
		 config_request.y += deltay;
	     else
		 config_request.y = tempy;
	     if (!(mask & CWWidth))
		 config_request.width = client_width;
	     if (!(mask & CWHeight))
		 config_request.height = client_height;    
	 }
	 /* Now that we actually have a usable client size, convert to frame */
	 config_request.height += frameHeightExcess;
 	 if (awi->attrs & AT_TITLE)
	     config_request.height += titleHeightExcess;
	 if (awi->attrs & AT_BORDER) {
	     config_request.width += frameWidthExcess;
	     config_request.height += borderHeightExcess;
	 }
         else
	     config_request.width += frameHeightExcess;
         if (Wall) {
	     /*	For resizing, keep opposite corner in place and shrink to fit.
		For moves, keep size the same */
	     if (config_request.x < 0) {
		 if (mask & CWWidth)
		     config_request.width += config_request.x;
	         config_request.x = 0;
	     }
	     if (config_request.y < 0) {
		 if (mask & CWHeight)
		     config_request.height += config_request.y;
	         config_request.y = 0;
	     }
	     /* at this point, all request values are set for frame, which 
		has no border */
	     if (config_request.x > ScreenWidth - config_request.width) {
		 if (mask & CWHeight)
		     config_request.width = ScreenWidth - config_request.x;
		 else
		     config_request.x = ScreenWidth - config_request.width;
	     }
	     if (config_request.y > ScreenHeight - config_request.height) {
		 if (mask & CWWidth)
		     config_request.height = ScreenHeight - config_request.y;
		 else
		     config_request.y = ScreenHeight - config_request.height;
	     }
	 }
#ifdef titan /* 5x4 alignment */
         config_request.x += (config_request.x % 5);
	 config_request.y += (config_request.y % 4);
#endif

	 /* client can reset win_gravity but not move client window, causing
	    frame to move */
	 config_request.sibling = None;
	 /* protect ourselves from a nefarious client! */
	 XConfigureWindow(dpy, awi->frame, mask | CWX | CWY, &config_request);
	 /* note that this will automatically take care of 
	    any stacking request */
	 config_request.height -= frameHeightExcess;
 	 if (awi->attrs & AT_TITLE)
	     config_request.height -= titleHeightExcess;
	 if (awi->attrs & AT_BORDER) {
	     config_request.width -= frameWidthExcess;
	     config_request.height -= borderHeightExcess;
	 }
         else
	     config_request.width -= frameHeightExcess;
	 if (awi->title && (mask & CWWidth))
             XResizeWindow(dpy, awi->title, config_request.width, titleHeight);
	 /* PutGadgets gets title window size from server */
	 if (awi->gadgets)
	      PutGadgets(w);
	 if ((config_request.width == client_width) && 
	     (config_request.height == client_height)) {
	 /* the client window won't be reconfigured - send synthetic Notify
	    with final client window root-relative position */
	     if (awi->attrs & AT_TITLE)
	         config_request.y += titlePositionExcess;
	     if (awi->attrs & AT_BORDER) {
	         config_request.y += borderPositionExcess;
	         config_request.x += borderPositionExcess;
	     }
	     SendSyntheticConfigureNotify(awi, &config_request);
	     Leave(TRUE)
	 }
         mask &= ~(CWStackMode | CWSibling | CWX | CWY);
         /* if reparented, we don't restack client and client window 
	    position never changes */
     }
     else if (gotBorderRequest) {
	 XWindowAttributes xwa;
	 XGetWindowAttributes(dpy, awi->client, &xwa);
	 /* Workaround for x10tox11 clients that insist on setting the border
	    width (e.g. Frame): allow them to do so, since we did not set
	    their border widths in the first place. */
	 if (xwa.override_redirect)
	     mask |= CWBorderWidth;
         /* All the real work for wins about to be decorated is done in
	  * Decorate (Reparent) */
     }

     /* Decorate takes the window placement from the client window */
     XConfigureWindow(dpy, awi->client, mask, &config_request);
     /* no synthetic configureNotify required for non-reparented windows */
     Leave(TRUE)
}

int
ProcessRequests(box, num_vectors)
XSegment *box;
int num_vectors;
{
	XEvent event;

	XPeekEvent(dpy, &event);
	switch (event.type) {

	case MapRequest:
	case ConfigureRequest:
	case CirculateRequest:

	    DrawBox();
	    GetButton(&event);
	    DrawBox();
	    return TRUE;
	}
	return FALSE;
}
