#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: awm.c,v 5.83 89/11/22 15:13:13 glennw Exp $";
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
 *  Western Software Lab. Convert to X11.
 * 002 -- Jordan Hubbard, U.C. Berkeley. Add title bar context stuff.
 * 003 -- Jordan Hubbard, Ardent Computer. Added gadgets, border contexts.
 */

#include <signal.h>
#if defined(vax)
#include <sys/file.h>
#else
/* <fcntl.h> included in Xos.h */
#endif /* vax */
#include <sys/ioctl.h>
#include <ctype.h>
#include "awm.h"
#include "X11/Xutil.h"
#include "X11/cursorfont.h"

#ifdef PCS
#include <unistd.h>
#include <bsd/sys/time.h>
#endif
#ifdef PROFIL
/*
 * Dummy handler for profiling.
 */
ptrap()
{
     exit(0);
}
#endif

Bool ChkMline(), Local();
char *sfilename;
char execfile[NAME_LEN]; /* Pointer to file to exec with -e */
Boolean Snatched;
Boolean desktop_execd = TRUE;
extern FILE *yyin;
extern int errorStatus;
extern int ErrorHandler();
extern int XsSyncFunction();
extern XContext AwmContext;
extern Boolean QuitTerm();
extern Boolean QuitInt();
extern Boolean QuitHup();
extern Boolean QuitQuit();

Window BoundContext;		/* binding context - external for menu hack */
Window ContextWindow;		/* Button window - external for menu hack */
int ContextX, ContextY;		/* mouse X and Y at ButtonPress. */

/*
 * Main program.
 */
main(argc, argv, environ)
int argc;
char **argv;
char **environ;
{
     int hi;			/* Button event high detail. */
     int lo;			/* Button event low detail. */
     int x, y;                  /* Mouse X and Y coordinates. */
     int root_x, root_y;        /* Mouse root X and Y coordinates. */
     int cur_x, cur_y;		/* Current mouse X and Y coordinates. */
     int str_width;             /* Width in pixels of output string. */
     int pop_width, pop_height; /* Pop up window width and height. */
     int context;		/* Root, window, or icon context. */
     int ptrmask;		/* for QueryPointer */
     Boolean func_stat;		/* If true, function swallowed a ButtonUp. */
     Boolean delta_done;	/* If true, then delta functions are done. */
     Boolean local;		/* If true, then do not use system defaults. */
     Boolean nolocal;		/* If true, ignore user defaults */
     register Binding *bptr;	/* Pointer to Bindings list. */
     char *root_name;		/* Root window name. */
     char *display = NULL;	/* Display name pointer. */
     char message[128];		/* Error message buffer. */
     char *rc_file;		/* Pointer to $HOME/.tekwmrc. */
     Window event_win;          /* window for func - frame or client window */
     Window root;		/* Root window for QueryPointer. */
     AwmInfoPtr awi;
     XEvent button_event; 	/* Button input event. */
     XGCValues xgc;		/* to create font GCs */
     XSetWindowAttributes swa;
     unsigned long valuemask;
     int num;
     XIconSize IconSize;
     Boolean startup_initial_state = FALSE; /* should we obey initial_state? */

     /* next three variables are for XQueryWindow */
     unsigned int nkids;
     Window *kiddies = (Window *) NULL;	    /* not reqd by CURRENT Xlib code */
     Window junk;
     
     /* next five variables are for window processing loop */
     unsigned int i;
     XWindowAttributes xwa;
     Window transient;	    /* Future: use to control iconification */
     unsigned long event_mask;

     
     Entry("main")

#ifdef PROFIL
	  signal(SIGTERM, ptrap);
#endif
     
     /* 
      * Parse the command line arguments.
      */
     version(argc, argv);   /* check for -v option */
     Argc = argc;
     Argv = argv;
     Environ = environ;
     execfile[0] = '\0';
     local = nolocal = FALSE;
     if((progname = rindex(argv[0], '/')) == NULL)
	 /* no path separator, use as is */
	 progname = argv[0];
     else
	 progname++;

     argc--, argv++;
     /*
      * The destruction of '-e' args below is to prevent the startup
      * command from being invoked again if we do an f.restart (see
      * Restart.c and Argv).  However, we must retain them for a
      * Session-manager restart! 
      */
     while (argc) {
	  if (**argv == '-') {
	       if (!strcmp(*argv, "-display") || !strcmp(*argv, "-d")) {
		    argc--; argv++;
		    if (argc <= 0)
			 Usage();
		    display = *argv;
	       }
	       else if (!(strcmp(*argv, "-f"))) {
		    argc--, argv++;
		    if ((argc == 0) || (Startup_File[0] != '\0'))
			 Usage();
		    strncpy(Startup_File, *argv, NAME_LEN);
	       }
	       else if (!(strcmp(*argv, "-s"))) {
		    argc--, argv++;
		    if (argc == 0)
			 Usage();
		    strncpy(Session_File, *argv, NAME_LEN);
	       }
	       else if (!(strcmp(*argv, "-e"))) {
		    strcpy(*argv, "--");	/* destroy the arg */
		    argc--; argv++;
		    if ((argc == 0) || (execfile[0] != '\0'))
			 Usage();
		    desktop_execd = FALSE; /* assume we have desktop to run */
		    strncpy(execfile, *argv, NAME_LEN);
	       }
	       /* Destroyed arg, skip over what used to be filename for -e */
	       else if (!(strcmp(*argv, "--"))) {
		    argv += 2; argc -= 2;
		    continue;
	       }
	       else if (!(strcmp(*argv, "-b")))
		    local = TRUE;
	       else if (!(strcmp(*argv, "-h")))
		    startup_initial_state = TRUE;
	       else if (!(strcmp(*argv, "-i")))
		    nolocal = TRUE;
	       
	       else Usage();
	  }
	  else
	       Usage();
	  argc--, argv++;
     }
 
#ifdef CONSOLE
     if (access("/dev/console", W_OK) == 0) {
          freopen("/dev/console", "w", stderr);
	  freopen("/dev/console", "w", stdout);
     }
#endif /* CONSOLE */
     /* Open the damn display */
     if ((dpy = XOpenDisplay(display)) == NULL) {
	  Warning("Unable to open display");
	  exit(1);
     }

     scr = DefaultScreen(dpy);
     if (scr < 0 || ScreenCount(dpy) <= scr) {
	  /* go ahead and abort without starting clients - this is bad news */
	  Warning("Unable to use display - invalid screen number");
	  exit(1);
     }    
     
     /*
      * Set XErrorFunction to be non-terminating.
      */
     XSetErrorHandler(ErrorHandler);
     
     /*
      * Force child processes to disinherit the TCP file descriptor.
      * This helps shell commands forked and exec'ed from menus
      * to work properly. Claims to work under SysV...
      */
     if (fcntl(ConnectionNumber(dpy), F_SETFD, 1) == -1) {
	  perror("tekwm: child cannot disinherit TCP fd");
	  Error("TCP file descriptor problems");
     }
 
     /* Init the context manager stuff */
     AwmContext = XUniqueContext();
     
     /*
      * Get all the defaults we expect from the resource manager.
      */
     FocusSetByUser = FALSE;
     gadgetHeight = 0;	    
     Get_Defaults();

     /*
      *	Make everything syncronous if requested
      */
     if (Syncronize)
	 (void) XSetAfterFunction(dpy, XsSyncFunction);

     /*
      * Initialize the default bindings.
      */
     if (!local)
	  InitBindings();
     
     /*
      * Read in and parse $HOME/.tekwmrc, if it exists.
      */
     if (!nolocal) {
	  sfilename = rc_file = malloc(NAME_LEN);
	  sprintf(rc_file, "%s/.tekwmrc", getenv("HOME"));
	  if ((yyin = fopen(rc_file, "r")) != NULL) {
	       Lineno = 1;
	       yyparse();
	       fclose(yyin);
	       if (Startup_File_Error)
		    Error("Bad .tekwmrc file...aborting");
	  }
     }
     /* 
      * Read in and parse the startup file from the command line, if
      * specified.
      */
     if (Startup_File[0] != '\0') {
	  sfilename = Startup_File;
	  if ((yyin = fopen(Startup_File, "r")) == NULL) {
	       sprintf(message, "Cannot open startup file '%s'", Startup_File);
	       Error(message);
	  }
	  Lineno = 1;
	  yyparse();
	  fclose(yyin);
	  if (Startup_File_Error)
	       Error("Bad startup file...aborting");
     }
 
    /*
     * Verify the menu bindings.
     */
    VerifyMenuBindings();
    if (Startup_File_Error)
        Error("Bad startup file...aborting");

    /*
     * Verify the menu variables.
     */
    VerifyMenuVariables();

     /*
      * Catch some of the basic signals so we don't get rudely killed without
      * cleaning up first.
      */
     if (signal(SIGHUP, SIG_IGN) != SIG_IGN)
	  signal(SIGHUP, QuitHup);
     if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
	  signal(SIGTERM, QuitTerm);
     if (signal(SIGQUIT, SIG_IGN) != SIG_IGN)
	  signal(SIGQUIT, QuitQuit);
     if (signal(SIGINT, SIG_IGN) != SIG_IGN)
	  signal(SIGINT, QuitInt);

     /*
      * If the root window has not been named, name it.
      */
     XFetchName(dpy, RootWindow(dpy, scr), &root_name);
     if (root_name == NULL) 
	  XStoreName(dpy, RootWindow(dpy, scr), " X Root Window ");
     else
	  free(root_name);
     /* register the root window.  Awi->state is irrelevant */
     RegisterWindow(RootWindow(dpy, scr), TRUE, NULL);

     ScreenHeight = DisplayHeight(dpy, scr);
     ScreenWidth = DisplayWidth(dpy, scr);

     /*
      * Tell 'em about what great icon support we have!
      */
     IconSize.min_width = IconSize.min_height = 0;
     IconSize.max_width = ScreenWidth;
     IconSize.max_height = ScreenHeight;
     IconSize.width_inc = IconSize.height_inc = 1;
     XSetIconSizes(dpy, RootWindow(dpy, scr), &IconSize, 1);

     /*
      * check the gadgets.
      */
     if (CheckGadgets())
	  Error("Error in gadget declarations. Exiting...\n");

     /*
      * Store all the cursors.
      */
     StoreCursors();
     
     /*
      * Set initial focus to PointerRoot.
      */
     XSetInputFocus(dpy, PointerRoot, None, CurrentTime);
     FocusWindow = RootWindow(dpy, scr);

     /*
      * All ready to be a window manager - make sure one is not already
      * running before taking over.
      * This now works via an unmapped window with a WM_CLASS hint with
      * res_class= "*Wm*".
      * By this time XOpenDisplay has loaded display in dpy.
      */
     RegisterWm();

     /* 
      * watch for initial window mapping and window destruction
      */
     
#define ROOTEVENTMASK (SubstructureRedirectMask | \
		    SubstructureNotifyMask | FocusChangeMask | EnterWindowMask)
     /* need to get DestroyNotify on children of root, in case no frame or 
	child dies during sizing, hence SubstructureNotifyMask */

     swa.event_mask = ROOTEVENTMASK;
     XChangeWindowAttributes(dpy, RootWindow(dpy, scr), CWEventMask, &swa);

     Init_Borders(); /* do this now for transient window borders */

     /*
      * Before we go creating more windows, we buzz through the ones that
      * are currently mapped and reparent and/or select on them as necessary
      * (for autoraise and titles).
      */
     
     if (XQueryTree(dpy, DefaultRootWindow(dpy), &junk, &junk, &kiddies,
		    &nkids)) {
	  for (i = 0; i < nkids; i++) {
	       XGetWindowAttributes(dpy, kiddies[i], &xwa);
	       /* check to see if it's a real top-level window */
	       if (xwa.class == InputOutput && xwa.map_state == IsViewable) {
		    /* ICCCM VIOLATION!!! we must handle override_redirect
		        windows for the protocol converter */
		    if (startup_initial_state)
		    /* obey XWMHints.initial_state - even override_redirect */
			awi = RegisterWindow(kiddies[i], FALSE, 
					     &xwa);
		    else
			/* obey WMState */
			awi = RegisterWindow(kiddies[i], TRUE,
					     &xwa);
	       }	       
	  }
	  for (i = 0; i < nkids; i++) {
	       /* now, loop through to find any client icon windows, and
		  unregister them */
	      if (((awi = GetAwmInfo(kiddies[i])) != (AwmInfoPtr) NULL) &&
		  (awi->wm_hints->flags & IconWindowHint))
		  UnRegister(awi->wm_hints->icon_window, True);
		/* lie about win death so UnRegister won't mess with window */
	  }
	  for (i = 0; i < nkids; i++) {
	      if ((awi = GetAwmInfo(kiddies[i])) != NULL) {
		    event_mask = PropertyChangeMask | FocusChangeMask;
		    if (InstallColormap)
		    /* select for ColormapChange before caching client_cmap */
			event_mask |= ColormapChangeMask;
		    XSelectInput(dpy, kiddies[i], event_mask);
		    awi->state |= ST_PLACED;
		    /* Possibly add a frame */
		    /* don't decorate if transient_for or override_redirect,
		       except for border color.  We could also set BorderWidth,
		       but then we would have to handle win_gravity, adding
		       more map delays, so punt it for now.  Note that
		       everything else must work on transient_for or
		       override_redirect windows.  Note also that a
		       TransientFor of None still causes transient treatment.*/
		    if ((awi->xwa.override_redirect
			&& !(awi->attrs & (AT_TITLE | AT_BORDER)))  || 
			 /* client can request that override_redirect window be
			    decorated via WM_CLASS and wm_options resources */
			XGetTransientForHint(dpy, kiddies[i], &transient))
		        SetFrameColors(awi);  
			 /* Actually, create client border pixmap */
		    else
			if (!Decorate(kiddies[i], FALSE)) {
		     /* window went away; go ahead and trash awm info now */
			    UnRegister(kiddies[i], True);
			    continue;
			}
		   	/* go ahead and abort if client died */

		    /* now select rest of input depending on reparenting */
		    if (!awi->frame || !FrameFocus)
			/* hmm, do we need to adjust this on NoDecorate?? */
			 event_mask |= (EnterWindowMask | LeaveWindowMask);
		    if (!awi->frame)
			event_mask |= VisibilityChangeMask;
		    if (awi->attrs & AT_RAISE)
			event_mask |= PointerMotionMask;
			/* always ask for motion in client window 
			   in case client is too */
		    XSelectInput(dpy, kiddies[i], event_mask);
		    /* initialize everything, including client colormap */
		    LightsOff(awi, TRUE);
		    /* initialize until we get a focus in after mapping */
		    if (awi->state & ST_ICON) {
		        /* window was an icon (WM_STATE or hints.initial_state) */
                        /* fool Iconify about the state */
		         awi->state ^= ST_ICON;    
		         awi->state |= ST_WINDOW;  
		        /* calling Iconify with zeros causes it to not zap or warp*/
                         Iconify(kiddies[i], 0, 0, 0, 0);
		    }
		    else {
			/* window was Normal. */
			/* If the client has an icon_window, we must unmap
			   it.  Can't go ahead and make the icon, because we
			   aren't ready to place it. */
			if (awi->wm_hints->flags & IconWindowHint) {
			    XUnmapWindow(dpy, awi->wm_hints->icon_window);
			}
			/* Set WM_STATE. */
			UpdateWMState(awi);
			/* Map its new frame. */
		        if (awi->frame) {
		            XMapWindow(dpy, awi->frame);
		            XMapWindow(dpy, awi->client);
		        }
		        else
		            XMapWindow(dpy, awi->client);
		    }
	       }
	  }
	  if (kiddies)
	      XFree(kiddies);
     }
     /* 
      * Grab the mouse buttons according to the map structure.
      * Do it after Registering windows to save XGetAttributes calls.
      */
     Grab_Buttons();
     
     /*
      * Calculate size of the resize pop-up window.
      */
     valuemask = CWBorderPixel | CWBackPixel;
     swa.border_pixel = PBorder;
     swa.background_pixel = PBackground;
     if (SaveUnder) {
	  swa.save_under = True;
	  valuemask |= CWSaveUnder;
     }
     str_width = XTextWidth(PFontInfo, PText, strlen(PText));
     pop_width = str_width + (HPopupPad << 1);
     PWidth = pop_width + (PBorderWidth << 1);
     pop_height = PFontInfo->ascent + PFontInfo->descent + (VPopupPad << 1);
     PHeight = pop_height + (PBorderWidth << 1);
     
     /*
      * Create the pop-up window.  Create it at (0, 0) for now.  We will
      * move it where we want later.
      */
     Pop = XCreateWindow(dpy, RootWindow(dpy, scr),
			 0, 0,
			 pop_width, pop_height,
			 PBorderWidth,
			 0,
			 CopyFromParent,
			 CopyFromParent,
			 valuemask,
			 &swa);
     if (Pop == FAILURE)
	  Error("Can't create pop-up dimension display window.");

    /*
     * Create the menus for later use.
     */
    CreateMenus();
     
     /*
      * Create graphics context.
      */
     xgc.font = IFontInfo->fid;
     xgc.graphics_exposures = FALSE;
     xgc.foreground = ITextForeground;
     xgc.background = ITextBackground;
     IconGC = XCreateGC(dpy, 
			RootWindow(dpy, scr),
			(GCForeground | GCBackground | GCGraphicsExposures |
			 GCFont), &xgc);
     
    xgc.foreground = MTextForeground;
    xgc.background = MTextBackground;
    xgc.font = MFontInfo->fid;
    MenuGC = XCreateGC(dpy, 
    	RootWindow(dpy, scr),
	(GCForeground|GCBackground|GCFont), &xgc);
    xgc.background = MTextForeground;
    xgc.foreground = MTextBackground;
    MenuBgGC = XCreateGC(dpy, 
    	RootWindow(dpy, scr),
	(GCForeground|GCBackground|GCFont), &xgc);
    xgc.function = GXinvert;
    xgc.plane_mask = MTextForeground ^ MTextBackground;
    MenuInvGC = XCreateGC(dpy, 
    	RootWindow(dpy, scr), GCFunction|GCPlaneMask, &xgc);

     xgc.foreground = PForeground;
     xgc.background = PBackground;
     xgc.font = PFontInfo->fid;
     PopGC = XCreateGC(dpy, 
		       RootWindow(dpy, scr),
		       (GCForeground | GCBackground | GCFont), &xgc);
     xgc.line_width = DRAW_WIDTH;
     xgc.foreground = DRAW_VALUE;
     xgc.function = DRAW_FUNC;
     xgc.subwindow_mode = IncludeInferiors;
     DrawGC = XCreateGC(dpy, RootWindow(dpy, scr), 
			GCLineWidth | GCForeground | GCFunction |
			GCSubwindowMode, &xgc);

     /*
      * As our last "startup" task, invoke the execfile if was specified.
      */
     if (!desktop_execd) {
         if (fork() == 0) {
	     /* don't permit ctl-C or kill to awm to kill the -e program */
#ifdef SYSV
	     signal (SIGINT, SIG_IGN);
	     signal (SIGQUIT, SIG_IGN);
	     signal (SIGTERM, SIG_IGN);
	     signal (SIGHUP, SIG_IGN);
#else /* BSD */
	     setpgrp(0, 0);
#endif /* SYSV */
	     execlp("sh", "sh", "-c", execfile, 0);
	    /* the things we have to do to get execfile invoked from $PATH! */
	    /* if we get here, something went wrong! */
	     (void) sprintf(error_msg, "Can't execute sh -c %s:", execfile);
	     Warning(error_msg);
	     perror("");
	     _exit(127);
	 }
	 else
	     desktop_execd = TRUE;
     }
     /*
      * Tell the user we're alive and well.
      */
     XBell(dpy, VOLUME_PERCENTAGE(Volume));
     
     /* 
      * Main command loop.
      */
     while (TRUE) {
	  
	  delta_done = func_stat = FALSE;
	  
	  /*
	   * Get the next mouse button event.  Spin our wheels until
	   * a ButtonPressed event is returned.
	   * Note that mouse events within an icon window are handled
	   * in the "GetButton" function or by the icon's owner if
	   * it is not awm.
	   */
	  while (TRUE) {
	       if (!GetButton(&button_event))
		    continue;
	       else if (button_event.type == ButtonPress)
		    break;
	  }
	  ContextWindow = button_event.xbutton.window;
	  /* save mouse coords in case we want them later for a delta action */
	  ContextX = button_event.xbutton.x;
	  ContextY = button_event.xbutton.y;
	  
	  /*
	   * Okay, determine the event window and mouse coordinates.
	   */
	  if (! XTranslateCoordinates(dpy, 
					 ContextWindow,
					 RootWindow(dpy, scr),
					 button_event.xbutton.x, 
					 button_event.xbutton.y,
					 &x, &y,
					 &event_win))
	       continue;
	  
	  awi = GetAwmInfo(ContextWindow);
	  if (!awi)
	       continue;

          if (awi->frame == ContextWindow)
               context = BORDER;
	  else if (awi->title == ContextWindow)
	       context = TITLE;
	  else if (IsGadgetWin(ContextWindow, &num))
	       context = GADGET | (1 << (num + BITS_USED));
	  else if ((awi->icon == ContextWindow) ||
		   (awi->client_icon == ContextWindow))
	       context = ICON;
	  else if (awi->client == RootWindow(dpy, scr)) {
	       event_win = RootWindow(dpy, scr);
	       context = ROOT;
	  }
	  else
	       context = WINDOW;

	  /*
	   * Get the button event detail.
	   */
	  lo = button_event.xbutton.button;
	  hi = button_event.xbutton.state;
	  
	  /*
	   * Determine which function was selected and invoke it.
	   */
	  for(bptr = Blist; bptr; bptr = bptr->next) {
	       if ((bptr->button != lo) ||
		   (((int)bptr->mask & ModMask) != hi))
		    continue;
	       
	       if ((bptr->context & context) != context) {
		    continue;
               }
	       if (!(bptr->mask & ButtonDown))
		    continue;
	       
	       /*
		* Found a match! Stash context for auto-select menus.
		*/
		BoundContext = bptr->context;

	       /*
		* Whether we we got the event by selection or a grab, grab the
		* pointer so the button up does not get sent to a client
		* window, and invoke the function.  This is necessary even if
		* the context was window, because f.iconify will unmap the
		* grabbed window, losing the passive grab.  Note - the context
		* for the trapped up event in the client window will be root!
		* It needs to be adjusted to WINDOW.
	        */
	       if (XGrabPointer( dpy, RootWindow(dpy, scr),
                                 TRUE, EVENTMASK, GrabModeAsync,
                                 GrabModeAsync, None,
                                 None, CurrentTime )
                     != GrabSuccess) {
		   /* don't interfere with event selection, or cursor. */
                   Warning( "main: Could not re-grab pointer");
		    /* this can happen if user gin gets ahead of us, so that
		       we are responding to a root window press by grabbing,
		       after the next press goes to a client and it grabs. */
		   continue;
		}

      	       if ((*bptr->func)(event_win, (int)bptr->mask & ModMask,
				 bptr->button,
				 x, y,
				 bptr->menu, bptr->menuname))
		    func_stat = TRUE;
	       break;
	  }
	  
	  /*
	   * If the function ate the ButtonUp event, then restart the loop.
	   * (move, iconify, resize)
	   */
	  
/* fprintf(stderr, "finished button down func: func_stat = %d\n", func_stat);*/
	  if (func_stat) {
	       XUngrabPointer( dpy, CurrentTime);
	       continue;
	  }
	  while (TRUE) {
	       /*
		* Wait for the next button event.
		*/
	       if (XPending(dpy) && GetButton(&button_event)) {
		    ContextWindow = button_event.xbutton.window;
		    /*
		     * If it's a press, or a release of a button other than the
		     * one that was pressed, or the modifiers have changed,
		     * don't do the function bound to 'ButtonUp'.
		     * XXX ButtonMask() does not handle Buttons 4 & 5!
		     */
		    if (button_event.type == ButtonPress)
			 break;
		    if (button_event.type != ButtonRelease)
			 continue;	/* e.g. LeaveNotify, ignore */
		    if (lo != button_event.xbutton.button)
			 break;
		    if ((hi | ButtonMask(lo)) != button_event.xbutton.state)
			 break;
		    /*
		     * Okay, determine the event window and mouse coordinates.
		     */
		    if (! XTranslateCoordinates(dpy, 
						   ContextWindow,
						   RootWindow(dpy, scr),
						   button_event.xbutton.x,
						   button_event.xbutton.y,
						   &x, &y,
						   &event_win))
			break;
		    if(event_win == None)
			event_win = RootWindow(dpy, scr);
		    awi = GetAwmInfo(event_win);
		    if (!awi) {
			/* Oops!  Got an unregistered window with button up
			 * event.  Shouldn't we be breaking at this point, so
			 * we don't sit here waiting for another
			 * ButtonRelease?  (was continue;) */
			break;
		    }
		    /* Note - because we Grabbed on RootWindow, if the button
		     * was pressed outside a client window, then released in
		     * a client window (not on root window), the
		     * ContextWindow for the up event will be root!  It needs
		     * to be adjusted to the client window.
		     */
		    if ((context != WINDOW) && 
			(ContextWindow == RootWindow(dpy, scr)) &&
			(event_win != RootWindow(dpy, scr))) {
			ContextWindow = awi->client;
			context = WINDOW;
		    }
		    else if (awi->frame == ContextWindow)
			 context = BORDER;
		    else if (awi->title == ContextWindow)
			 context = TITLE;
		    else if (IsGadgetWin(ContextWindow, &num))
			 context = GADGET | (1 << (num + BITS_USED));
		    else if ((awi->icon == ContextWindow) ||
			    (awi->client_icon == ContextWindow))
			 context = ICON;
		    else if (awi->client == RootWindow(dpy, scr)) {
			 context = ROOT;
		    }
		    else
			 context = WINDOW;
		    /*
		     * Determine which function was selected and invoke it.
		     * Recompute the event window/position
		     */
		    for(bptr = Blist; bptr; bptr = bptr->next) {
			 
			 if ((bptr->button != lo) ||
			     (((int)bptr->mask & ModMask) != hi))
			      continue;
			 
			 if (!((bptr->context & context) == context)) {
			      continue;
                         }
			 
			 if (!(bptr->mask & ButtonUp))
			      continue;
			 
	  	         /*
			  * Found a match!  Stash context for auto-select
			  * menus.
			  */
			 BoundContext = bptr->context;

			 /*
			  * Invoke the function.
			  */
			 
			 (*bptr->func)(event_win,
				       (int)bptr->mask & ModMask,
				       bptr->button,
				       x, y,
				       bptr->menu, bptr->menuname);
			 break;
		    }
/*fprintf(stderr, "finished button up func: func_stat = %d\n", func_stat);*/
		    break;
	       }
	       XQueryPointer(dpy, RootWindow(dpy, scr),
			     &root, &junk, &root_x, &root_y, &cur_x, &cur_y,
			     &ptrmask);
	       if (!delta_done &&
		   (abs(cur_x - x) > Delta || abs(cur_y - y) > Delta)) {
		    /*
		     * Delta functions are done once (and only once.)
		     */
		    delta_done = TRUE;
		    
		    /*
		     * Take the delta event window's coordinates from the
		     * original ButtonPress event.
		     */
		    /*
		     * Determine which function was selected and invoke it.
		     */
		    for(bptr = Blist; bptr; bptr = bptr->next) {
			 
			 if ((bptr->button != lo) ||
			     (((int)bptr->mask & ModMask) != hi))
			      continue;
			 
			 if (!((bptr->context & context) == context))
			      continue;
			 
			 if (!(bptr->mask & DeltaMotion))
			      continue;
			 
	  	         /*
			  * Found a match!  Stash context for auto-select
			  * menus.
			  */
			 BoundContext = bptr->context;

			 /*
			  * Invoke the function.
			  */
			 
			 if ((*bptr->func)(event_win,
					   (int)bptr->mask & ModMask,
					   bptr->button,
					   x, y,
					   bptr->menu, bptr->menuname)) {
			      func_stat = TRUE;
			      break;
			 }
		    }
		    /*
		     * If the function ate the ButtonUp event,
		     * then restart the loop.
		     */
		    
/*fprintf(stderr, "finished button delta func: func_stat = %d\n", func_stat);*/
		    if (func_stat)
			 break;
	       }
	  }
	  /* done processing this button down - ungrab pointer once all buttons
	     are released, so spurious buttons don't go to client.  Depend on
	     function to absorb all buttons (not being done!) */
/*fprintf(stderr, "finished all funcs: func_stat = %d\n", func_stat);*/
	  if (!func_stat && ((button_event.type == ButtonPress) ||
			     /* else it is a ButtonRelease */
			     IsButtonDown(button_event.xbutton.state))) {
	      /* some button still down, wait for it to go up */
	      WaitForButtonsUp();
          }
          XUngrabPointer( dpy, CurrentTime);
     }
}

/*
 * Get defaults from the resource manager. Most of these things used to be
 * in the rc file, but they really belong here, I think.
 */
Get_Defaults()
{
     
     /*
      * Get the pixmap search path, if it exists.
      */
     awmPath = GetStringRes("pixmapPath", "FilePath", NULL);

     /* Default foreground/background colors (text, for use in client cmap) */
     Foreground = GetStringRes("foreground", NULL, "black");
     Background = GetStringRes("background", NULL, "white");
     if (Reverse) { /* Swap the foreground and background */
	  char *tmp;
	  
	  tmp = Foreground;
	  Foreground = Background;
          Background = tmp;
     }
     WBorder = GetStringRes("borderColor", "Foreground", Foreground);
     
     EventDebug = GetBoolRes("eventDebug", "Debug", FALSE);
     Syncronize = GetBoolRes("syncronize", "Debug", FALSE);
     PrintProtoErrors = GetBoolRes("printProtoErrors", "Debug", FALSE);
     Autoraise = GetBoolRes("autoRaise", "Auto", FALSE);
     Autoselect = GetBoolRes("autoSelect", "Warp", FALSE);
     Autoexecutegadgets = GetBoolRes("gadget.autoExecute", "Auto", TRUE);
     BackingStoreMenus = GetBoolRes("backingStoreMenus", "BackingStore", FALSE);
    /* (left over from RTL menus, may want to use?) */
     BorderHilite = GetBoolRes("border.highlight", NULL, Hilite);
     BorderContextHilite = GetBoolRes("borderContext.highlight", NULL, Hilite);
     ConstrainResize = GetBoolRes("constrainResize", NULL, FALSE);
     FrameFocus = GetBoolRes("frameFocus", NULL, TRUE);
     Freeze = GetBoolRes("freeze", NULL, FALSE);
     if (EventDebug)
	 /* tekwm will lock up during rubber banding if event debug is being
	    printed to a frozen xterm window. */
         Freeze = FALSE;
     Grid = GetBoolRes("grid", NULL, FALSE);
     Hilite = GetBoolRes("highlight", NULL, FALSE);
     InstallColormap = GetBoolRes("installColormap", "ClientSupport", FALSE);
     NIcon = GetBoolRes("normali", NULL, TRUE);
     NWindow = GetBoolRes("normalw", NULL, TRUE);
     NoFocus = GetBoolRes("noFocus", NULL, FALSE);
     Push = GetBoolRes("pushRelative", NULL, FALSE);
     ResizeRelative = GetBoolRes("resizeRelative", NULL, FALSE);
     Reverse = GetBoolRes("reverseVideo", NULL, FALSE);
     RootResizeBox = GetBoolRes("rootResizeBox", NULL, FALSE);
     SaveUnder = GetBoolRes("saveUnder", NULL, FALSE);
     ShowName = GetBoolRes("showName", NULL, TRUE);
     Titles = GetBoolRes("titles", NULL, TRUE);
     UseGadgets = GetBoolRes("gadgets", NULL, TRUE);
     Wall = GetBoolRes("wall", NULL, FALSE);
     WarpOnDeIconify = GetBoolRes("warpOnDeIconify", "Warp", FALSE);
     WarpOnIconify = GetBoolRes("warpOnIconify", "Warp", FALSE);
     WarpOnRaise = GetBoolRes("warpOnRaise", "Warp", FALSE);
     Zap = GetBoolRes("zap", NULL, FALSE);

     HIconPad = GetIntRes("icon.hPad", "Icon.Pad", DEF_ICON_PAD);
     VIconPad = GetIntRes("icon.vPad", "Icon.Pad", DEF_ICON_PAD);
     RaiseDelay = GetIntRes("raiseDelay", "Delay", DEF_RAISE_DELAY);
     /* TBorderWidth = GetIntRes("title.borderWidth", NULL, DEF_TITLE_BORDER_WIDTH);
	obsolete, due to conflict with contextBorder. */
     WBorderWidth = GetIntRes("borderWidth", NULL, DEF_WIN_BORDER_WIDTH);
     IBorderWidth = GetIntRes("icon.borderWidth", NULL, DEF_ICON_BORDER_WIDTH);
     PBorderWidth = GetIntRes("popup.borderWidth", NULL, DEF_POP_BORDER_WIDTH);
     VPopupPad = GetIntRes("popup.vPad", "Popup.Pad", DEF_POP_PAD);
     HPopupPad = GetIntRes("popup.hPad", "Popup.Pad", DEF_POP_PAD);
     MBorderWidth = GetIntRes("menu.borderWidth", NULL, DEF_MENU_BORDER_WIDTH);
     VMenuPad = GetIntRes("menu.vPad", "Menu.Pad", DEF_MENU_PAD);
     HMenuPad = GetIntRes("menu.hPad", "Menu.Pad", DEF_MENU_PAD);
     Delta = GetIntRes("delta", NULL, DEF_DELTA);
     GadgetBorder = GetIntRes("gadget.borderWidth", NULL, DEF_GADGET_BORDER);
     GadgetPad = GetIntRes("gadget.hPad", "Gadget.Pad", DEF_GADGET_PAD);
     Volume = GetIntRes("volume", NULL, DEF_VOLUME);
     Pushval = GetIntRes("push", NULL, DEF_PUSH);
     BContext = GetIntRes("borderContext.width", NULL, DEF_BCONTEXT_WIDTH);
     if(!BContext)
	  BorderContextHilite = FALSE;  /* there is none to highlight! */

     /* Default foreground/background colors for use in default map */
     ForeColor = GetColorRes("foreground", NULL, BlackPixel(dpy, scr));
     BackColor = GetColorRes("background", NULL, WhitePixel(dpy, scr));
     
     if (Reverse) { /* Swap the forecolor and backcolor */
	  Pixel tmp;
	  
	  tmp = ForeColor;
	  ForeColor = BackColor;
          BackColor = tmp;
     }
     
     /*
      * Retrieve the information structure for the specifed fonts and
      * set the global font information pointers.
      */
     IFontInfo = GetFontRes("icon.font", NULL, DEF_ICON_FONT);
     PFontInfo = GetFontRes("popup.font", NULL, DEF_POPUP_FONT);
     MFontInfo = GetFontRes("menu.font", NULL, DEF_MENU_FONT);

     IForeground = GetColorRes("icon.foreground", NULL, ForeColor);
     IBackground = GetColorRes("icon.background", NULL, BackColor);
     IBorder = GetColorRes("icon.borderColor", "Icon.Foreground", IForeground);
     ITextForeground = GetColorRes("icon.text.foreground", 
				   "Icon.Label.Foreground", IForeground);
     ITextBackground = GetColorRes("icon.text.background", 
				   "Icon.Label.Background", IBackground);
     /*
      * Create and store the grey and solid pixmaps (using default colormap)
      * GrayPixmap is now used only for Icon padding and gadget pixmap errors.
      */
     GrayPixmap = XCreatePixmapFromBitmapData(dpy, RootWindow(dpy, scr),
					      gray_bits,
					      gray_width, gray_height,
					      IForeground,
					      IBackground,
					      DefaultDepth(dpy, scr));
     SolidPixmap = XCreatePixmapFromBitmapData(dpy, RootWindow(dpy, scr),
					       solid_bits,
					       solid_width, solid_height,
					       ForeColor,
					       BackColor,
					       DefaultDepth(dpy, scr));

     PForeground = GetColorRes("popup.foreground", NULL, ForeColor);
     PBackground = GetColorRes("popup.background", NULL, BackColor);
     PBorder = GetColorRes("popup.borderColor", "Popup.Foreground", PBorder);
     MTextForeground = GetColorRes("menu.foreground", NULL, ForeColor);
     MTextBackground = GetColorRes("menu.background", NULL, BackColor);
    /* defaults are used in the reverse of normal menu text entry colors */
     MForeground = GetColorRes("menu.title.foreground", 
			       "Menu.Label.Foreground", MTextForeground);
     MBackground = GetColorRes("menu.title.background", 
			       "Menu.Label.Background", MTextBackground);
    /* HlForeground is really for the text, therfore really the highlight bg */
     MHlForeground = GetColorRes("menu.highlight.foreground", 
				 "Menu.Label.Foreground", MTextForeground);
     MHlBackground = GetColorRes("menu.highlight.background", 
				 "Menu.Label.Background", MTextBackground);
     MBorder = GetColorRes("menu.borderColor", "Menu.Foreground", MTextForeground);

     IBackPixmap = GetPixmapRes("icon.text.pixmap", NULL, GrayPixmap, IForeground,
				IBackground);
     
#ifdef NEATEN
     AbsMinWidth = GetIntRes("neaten.absMinWidth", NULL, DEFAULT_ABS_MIN);
     AbsMinHeight = GetIntRes("neaten.absMinHeight", NULL, DEFAULT_ABS_MIN);
     
     RetainSize = GetBoolRes("neaten.retainSize", NULL, TRUE);
     KeepOpen = GetBoolRes("neaten.keepOpen", NULL, TRUE);
     Fill = GetBoolRes("neaten.fill", NULL, TRUE);
     UsePriorities = GetBoolRes("neaten.usePriorities", NULL, TRUE);
     FixTopOfStack = GetBoolRes("neaten.fixTopOfStack", NULL, TRUE);
     
     PrimaryIconPlacement = GetStringRes("neaten.primaryIconPlacement",
					 NULL, DEF_PRIMARY_PLACEMENT);
     SecondaryIconPlacement = GetStringRes("neaten.secondaryIconPlacement",
					   NULL, DEF_SECONDARY_PLACEMENT);
#endif
     
     Leave_void
}

/*
 * Look up string resource. If undefined, return "def_val"
 */
char *GetStringRes(string, string_class, def_val)
char *string,  *string_class, *def_val;
{
     char *cp, *safe_cp;
     int len;
     
     Entry("GetStringRes")
     
     if (cp = XGetResource(dpy, NAME, progname, CLASS, string, string_class)) {
	  if (!(len = strlen(cp)))
	       Leave(def_val)
	  else {
	       /* database gets freed when per-client resources are read */
	       safe_cp = malloc(len+1);
	       strcpy(safe_cp, cp);
	       Leave(safe_cp)
	  }
     }
     Leave(def_val)
}

/*
 * Look up boolean resource. If undefined or bogus, return "def_val"
 */
Boolean GetBoolRes(resource, resource_class, def_val)
char *resource, *resource_class;
Boolean def_val;
{
     char *cp;
     Bool resval;     
     Entry("GetBoolRes")

     if (cp = XGetResource(dpy, NAME, progname, CLASS, resource, resource_class)) {
	  if ((resval = Pred(cp)) != -1)
	       /* Got a valid value */
	       Leave(resval)
     }
     Leave(def_val)
}

/*
 * Look up integer resource. If undefined or non-numeric,
 * return def_val.
 */
int GetIntRes(resource, resource_class, def_val)
char *resource, *resource_class;
int def_val;
{
     char *cp;
     
     Entry("GetIntRes")

     if (cp = XGetResource(dpy, NAME, progname, CLASS, resource, resource_class)) {
	  if (!strlen(cp) || !((*cp >= '0' && *cp <= '9') || *cp == '-'))
	       Leave(def_val)
	  Leave(atoi(cp))
     }
     Leave(def_val)
}

/*
 * Try to load pixmap file named by resource. Return 0 if unsuccessful.
 * Otherwise, set width, height and return data.
 */
char *GetPixmapDataResAppl(def_appl_name, appl_name, appl_class,
			   resource, resource_class, wide, high)
char *def_appl_name, *appl_name, *appl_class;
char *resource, *resource_class;
int *wide, *high;
{
     char *cp, *cp2;
     char *data;
     int junk;

     Entry("GetPixmapDataResAppl")
     
     if (cp = XGetResource(dpy, def_appl_name, appl_name, appl_class,
			   resource, resource_class)) {
	  cp2 = expand_from_path(cp);
	  if (!cp2) {
	       (void) sprintf(error_msg, "Can't find pixmap file '%s' named in \nresource '%s', class '%s'",
		       cp, resource, resource_class);
	       Warning(error_msg);
	       Leave(0)
	  }
	  if (XmuReadBitmapDataFromFile(cp2, wide, high, &data, &junk, &junk)
	      != BitmapSuccess) {
	       (void) sprintf(error_msg, "Can't find pixmap file '%s' named in\nresource '%s', class '%s'",
		       cp, resource, resource_class);
	       Warning(error_msg);
	  }
	  else {
	       Leave(data)
	  }
     }
     Leave(0)
}

char *GetPixmapDataRes(resource, resource_class, wide, high)
char *resource, *resource_class;
int *wide, *high;
{
     Entry("GetPixmapDataRes")
     
     Leave(GetPixmapDataResAppl(NAME, progname, CLASS, 
			   resource, resource_class, wide, high))
}

/*
 * Try to allocate pixmap resources named by "string", return "def_pix"
 * if not found.
 */
Pixmap GetPixmapRes(resource, resource_class, def_pix, fg, bg)
char *resource, *resource_class;
Pixmap def_pix;
Pixel fg, bg;
{
     char *data;
     Pixmap tmp;
     int wide, high;

     Entry("GetPixmapRes")

     if (data = GetPixmapDataRes(resource, resource_class, &wide, &high)) {
	  tmp = XCreatePixmapFromBitmapData(dpy, RootWindow(dpy, scr),
					    data, wide, high, fg, bg,
					    DefaultDepth(dpy, scr));
	  XFree(data);
	  if (!tmp) {
	       (void) sprintf(error_msg, "Can't create pixmap from file named in\nresource '%s', class '%s': using default.", 
		       resource, resource_class);
	       Warning(error_msg);
	       tmp = def_pix;
	  }
     }
     else
	  tmp = def_pix;
     Leave(tmp)
}

/*
 * Try to allocate bitmap resource named by "string", return none
 * if not found.
 */
Pixmap GetBitmapRes(def_appl_name, appl_name, appl_class, 
		    resource, resource_class, width_return, height_return)
char *def_appl_name, *appl_name, *appl_class;
char *resource, *resource_class;
unsigned int *width_return, *height_return;
{
     char *data;
     Pixmap tmp;
     int wide, high;

     Entry("GetBitmapRes")

     if (data = GetPixmapDataResAppl(def_appl_name, appl_name, appl_class,
				    resource, resource_class, &wide, &high)) {
	  tmp = XCreatePixmapFromBitmapData(dpy, RootWindow(dpy, scr),
					    data, wide, high, 1, 0, 1);
		/* one fg, zero bg, 1 bit deep */
	  XFree(data);
	  if (tmp) {
	      if (width_return)
		  *width_return = wide;
	      if (height_return)
		  *height_return = high;
	  }
	  else {
	       (void) sprintf(error_msg, "Can't create bitmap from file named in\nresource '%s', class '%s', application class '%s': using default.", 
		       resource, resource_class, appl_name);
	       Warning(error_msg);
	  }
     }
     else
	 tmp = (Pixmap)0;
     Leave(tmp)
}

/*
 * Try to allocate color named by resource, return "color" if not found.
 * This routine is only used for allocating colors from
 * the default colormap.
 */
Pixel GetColorRes(resource, resource_class, color)
char *resource, *resource_class;
Pixel color;
{
     char *cp;
     Pixel tmp_color;
     Boolean status;
     
     Entry("GetColorRes")

     if (cp = XGetResource(dpy, NAME, progname, CLASS, resource, resource_class)) {
	  tmp_color = LookupColor(cp, DefaultColormap(dpy, scr), &status);
	  if (!status) /* lookup succeeded */
	       Leave(tmp_color)
	  else
	       tmp_color = color;
     }
     else
	  tmp_color = color;
     Leave(tmp_color)
}

/*
 * Try and get font resource, using "default" if not found. If
 * neither are available, use server default.
 */

XFontStruct *GetFontRes(resource, resource_class, dflt)
char *resource, *resource_class, *dflt;
{
     char *cp;
     XFontStruct *tmp;
     static XFontStruct *def_font = 0;
     
     Entry("GetFontRes")
     
     if (!def_font)
	  def_font = XLoadQueryFont(dpy, "fixed");

     if (cp = XGetResource(dpy, NAME, progname, CLASS, resource, resource_class)) {
	 if (tmp = XLoadQueryFont(dpy, cp))
	      Leave(tmp)
	 else {
	      (void) sprintf(error_msg, "Can't load font '%s' specified by resource '%s', class '%s': trying '%s'.",
		      cp, resource, resource_class, dflt);
	      Warning(error_msg);
	 }
     }
     if (!dflt) /* NULL means we're not supposed to try again */
	  Leave((XFontStruct *) NULL)
     if (tmp = XLoadQueryFont(dpy, dflt))
	  Leave(tmp)
     else {
	  (void) sprintf(error_msg, "Can't load default font '%s' for resource '%s', class '%s': using server default.",
		dflt, resource, resource_class);
	  Warning(error_msg);
     }
     Leave(def_font)
}

AwmInfoPtr GetAwmInfo(w)
Window w;
{
     static AwmInfoPtr tmp;
     
     Entry("GetAwmInfo")
     
     if (!XFindContext(dpy, w, AwmContext, &tmp))
	  Leave(tmp)
     else
	  Leave((AwmInfoPtr)NULL)
}

AwmInfoPtr RegisterWindow(w, start, xwa)
Window w;
Bool start;	    /* are we starting up wm? */
XWindowAttributes *xwa; /* is this an override_redirect window? */
{
     AwmInfoPtr tmp;
     XClassHint clh;
     Boolean res_class_is_malloced;
     XWMState *get_wm_state;
     char *cp = 0;
     char *i, *j;
     XWindowAttributes xwattr;

     Entry("RegisterWindow")
     
     tmp = (AwmInfoPtr)malloc(sizeof(AwmInfo));
     tmp->client = w;
     tmp->title = tmp->frame = tmp->icon = tmp->client_icon = (Window)0;
     tmp->gadgets = (Window *)0;
     tmp->name = (char *)0;
     tmp->back = tmp->bold = tmp->BC_back = tmp->BC_bold =
	  tmp->iconPixmap = tmp->user_icon = tmp->user_mask = (Pixmap)0;
     tmp->state = ST_WINDOW;
     tmp->winGC = XCreateGC(dpy, w, (unsigned long)0, 0);
     if (xwa) {
	 tmp->xwa = *xwa;
     }
    else {
	XGetWindowAttributes(dpy, w, &xwattr);
	tmp->xwa = xwattr;
    }
    tmp->client_cmap = tmp->xwa.colormap;
    tmp->client_depth = tmp->xwa.depth;
    /* needed for root window focus in. XXX can collapse now...*/
     /*
      * Determine attribute set by first turning on all attributes
      * that are set by booleans and then (possibly) override them
      * by checking to see what an individual window might want.
      * Override_redirect windows are not reparented by default, 
      * but they are autoraised.  Set AT_TITLE and AT_BORDER off here 
      * and use that to decide whether to reparent override_redirect wins.
      */
     tmp->attrs = AT_NONE;
     if (Titles && !tmp->xwa.override_redirect)
	  tmp->attrs |= AT_TITLE;
     if (UseGadgets)
	  tmp->attrs |= AT_GADGETS;
     if (Autoexecutegadgets)
	  tmp->attrs |= AT_GADGETS_AUTOEXEC;
     if (BContext && !tmp->xwa.override_redirect)
	  tmp->attrs |= AT_BORDER;
     if (Autoraise)
	  tmp->attrs |= AT_RAISE;

     /* If starting wm, get WM_STATE, if present, and use it for current state.
	If not present, make everything NormalState. If not starting wm, 
	ignore WM_STATE in case client is reusing a Withdrawn window. */
     if (start && (get_wm_state = XGetWMState(dpy, w))) {
 	 switch (get_wm_state->state) {
	     case NormalState:
		/* ST_WINDOW */
		 break;

	     case WithdrawnState:
		 /* RegisterWindow is only called for Mapped windows, so
		    The only way to get here is if the client mapped the window
		    while we were not looking (i.e. dead).  So key off
		    initial_state. */
		 start = FALSE;
		 break;

	     default:  /* IconicState or anything else */
		 tmp->state = ST_ICON;
	     /* If some wm added new states, treat them as iconic.  */
	 }
	 XFree(get_wm_state);
     }

     if (tmp->wm_hints = XGetWMHints(dpy, w)) {
	  if ((tmp->wm_hints->flags & InputHint) && tmp->wm_hints->input)
     		tmp->attrs |= AT_INPUT;
	  if (!start && (tmp->wm_hints->flags & StateHint) && 
	      (tmp->wm_hints->initial_state != NormalState))
	     /* We are mapping the window; ignore WM_STATE, use initial_state.
		If some wm added new states, treat them as iconic.
		There is no WithdrawnState here.
		Assume NormalState if mapping without WM_HINTS */
	      tmp->state = ST_ICON;
     }
     else {
	 /* no hints has same semantics as no flags set */
	 tmp->wm_hints = XAllocWMHints();
	 tmp->wm_hints->flags = 0;
     }
     /* WMState is set in Iconify */
     clh.res_name = clh.res_class = (char *)NULL;
     res_class_is_malloced = False;
     if (!XGetClassHint(dpy, w, &clh)) {
	 /* if fails, use WM_NAME, after compressing out whitespace */
	 if (XFetchName(dpy, w, &cp)) {
	     for (i = j = cp; *i; i++) {
		 if(!isspace(*i))
		     *j++ = *i;
	     }
	     *j = (char) NULL;
	     /* uncapitalize window name for resource name */
	     *cp = tolower(*cp);
	     clh.res_name = cp;
	     /* capitalize window name for resource class */
	     clh.res_class = malloc(strlen(clh.res_name) + 1);
	     res_class_is_malloced = True;
	     *clh.res_class = toupper(*clh.res_name);
	     if (*clh.res_class)
		/* there was at least on non-null char */
		 strcpy(clh.res_class + 1, clh.res_name + 1);
	 }
     }
     if (clh.res_name) {
	  if (cp= (XGetResource(dpy, clh.res_name, clh.res_name, clh.res_class,
			       "wm_option.title", (char *) NULL)))
	       tmp->attrs = SetOptFlag(tmp->attrs, AT_TITLE, Pred(cp));
	  if (cp= (XGetResource(dpy, clh.res_name, clh.res_name, clh.res_class,
			       "wm_option.gadgets", (char *) NULL)))
	       tmp->attrs = SetOptFlag(tmp->attrs, AT_GADGETS, Pred(cp));
	  if (cp= (XGetResource(dpy, clh.res_name, clh.res_name, clh.res_class,
			       "wm_option.gadgets.autoExecute", (char *) NULL)))
	       tmp->attrs = SetOptFlag(tmp->attrs, AT_GADGETS_AUTOEXEC, Pred(cp));
	  if (cp= (XGetResource(dpy, clh.res_name, clh.res_name, clh.res_class,
			       "wm_option.borderContext", (char *) NULL)))
	       tmp->attrs = SetOptFlag(tmp->attrs, AT_BORDER, Pred(cp));
	  if (cp= (XGetResource(dpy, clh.res_name, clh.res_name, clh.res_class,
			       "wm_option.autoRaise", "Wm_option.Auto")))
	       tmp->attrs = SetOptFlag(tmp->attrs, AT_RAISE, Pred(cp));
	  if (cp= (XGetResource(dpy, clh.res_name, clh.res_name, clh.res_class,
			       "wm_option.positionInsideBorder",
			       (char *) NULL)))
	       tmp->attrs = SetOptFlag(tmp->attrs, AT_PINSIDE, Pred(cp));
	  if (cp= (XGetResource(dpy, clh.res_name, clh.res_name, clh.res_class,
			       "wm_option.fixedSize",
			       "Wm_option.FixedCompletely")))
	       tmp->attrs = SetOptFlag(tmp->attrs, AT_SIZE, Pred(cp));
	  if (cp= (XGetResource(dpy, clh.res_name, clh.res_name, clh.res_class,
			       "wm_option.fixedPosition", 
			       "Wm_option.FixedCompletely")))
	       tmp->attrs = SetOptFlag(tmp->attrs, AT_POSITION, Pred(cp));
	  tmp->user_icon = GetBitmapRes(clh.res_name, clh.res_name, 
				clh.res_class,
			       "wm_option.icon.pixmap", 
			       "Wm_option.Icon.Pixmap",
				&tmp->user_width, &tmp->user_height);
	  tmp->user_mask = GetBitmapRes(clh.res_name, clh.res_name, 
				clh.res_class,
			       "wm_option.icon.mask", 
			       "Wm_option.Icon.Mask",
				(unsigned int *)NULL, (unsigned int *)NULL);
	  /* mask size irrelevant */
	  if (res_class_is_malloced)
	      free(clh.res_class);
	  else
	      XFree(clh.res_class);
	  XFree(clh.res_name);
     }
     if(XSaveContext(dpy, w, AwmContext, tmp)) {
	(void) sprintf(error_msg, "ouch - something broke the XSaveContext call!");
	Warning(error_msg);
     }
     Leave(tmp)
}

/*
 * Sets bit "flag" conditionally, based on state of "mask" and
 * "predicate" (mask denotes current state, predicate denotes
 * whether change is desired and if so, what state).
 */
int SetOptFlag(mask, flag, predicate)
int mask, flag, predicate;
{
     Entry("SetOptFlag")

     switch (predicate) {
     case FALSE:
          if (mask & flag)
	       Leave(mask ^ flag)
          else
	       Leave(mask)

     case TRUE:
	  Leave(mask | flag)

     case -1:
     default:
	  /* invalid value detected by predicate */
	  Leave(mask)
     }
}

/*
 * check whether a string denotes an "on" or "off" value. Return 0
 * if "off", 1 if "on" and -1 if undefined (or null).
 */
Pred(s)
char *s;
{
     int i, len;
     char *tmp;
     Boolean ret = -1;
     Entry("Pred")

     if (!s)
	  Leave(-1)
     len = strlen(s);
     if (!len)
	  Leave(-1)
     tmp = (char *)malloc(len + 1);
     if (!tmp) {
	  (void) sprintf(error_msg, "Pred: Can't allocate storage for '%s'!", s);
	  Warning(error_msg);
	  Leave(-1)
     }
     strcpy(tmp, s);
     for (i = 0; i < len; i++)
	       tmp[i] = tolower(tmp[i]);
     if (*tmp == 'y' || !strcmp(tmp, "on")
	 || !strcmp(tmp, "true")
	 || !strcmp(tmp, "enable"))
	  ret = TRUE;
     else if (*tmp == 'n' || !strcmp(tmp, "off")
	      || !strcmp(tmp, "false")
	      || !strcmp(tmp, "disable"))
	  ret = FALSE;
     free(tmp);
     Leave(ret)
}

/*
 * Initialize the default bindings.  First, write the character array
 * out to a temp file, then point the parser to it and read it in.
 * Afterwards, we unlink the temp file.
 */
InitBindings()
{
     char *mktemp();
     char *tempfile;
     register FILE *fp;		/* Temporary file pointer. */
     register char **ptr;	/* Default bindings string array pointer. */
     
     Entry("InitBindings")
     
     /*
      * Create and write the temp file.
      */
     /*
      * This used to just call mktemp() on TEMPFILE, which was very
      * evil as it involved writing on a string constant. This extra
      * mastication is necessary to prevent that.
      */
     tempfile = (char *)malloc(strlen(TEMPFILE) + 1);
     strcpy(tempfile, TEMPFILE);
     (void) mktemp(tempfile);
     if ((fp = fopen(tempfile, "w")) == NULL) {
	  perror("tekwm: cannot create temp file");
	  exit(1);
     }
     for (ptr = DefaultBindings; *ptr; ptr++) {
	  fputs(*ptr, fp);
	  fputc('\n', fp);
     }
     fclose(fp);
     
     /*
      * Read in the bindings from the temp file and parse them.
      */
     sfilename = "Built-in default bindings";
     if ((yyin = fopen(tempfile, "r")) == NULL) {
	  perror("tekwm: cannot open temp file");
	  exit(1);
     }
     Lineno = 1;
     yyparse();
     fclose(yyin);
     unlink(tempfile);
     free(tempfile);
     if (Startup_File_Error)
	  Error("Bad default bindings...aborting");

     /*
      * Parse the system startup file, if one exists.
      */
     if ((yyin = fopen(SYSFILE, "r")) != NULL) {
	  sfilename = SYSFILE;
	  Lineno = 1;
	  yyparse();
	  fclose(yyin);
	  if (Startup_File_Error)
	       Error("Bad system startup file...aborting");
     }
     Leave_void
     }

/*
 * Verify menu bindings by checking that a menu that is mapped actually
 * exists.  Stash a pointer in the binding to the relevant menu info data
 * structure.
 * Check nested menu consistency.
 */
VerifyMenuBindings()
{
    Binding *bptr;
    MenuLink *mptr;

    for(bptr = Blist; bptr; bptr = bptr->next) {
        if (bptr->func == Menu) {
            for(mptr = Menus; mptr; mptr = mptr->next) {
                if(!(strcmp(bptr->menuname, mptr->menu->name))) {
                    bptr->menu = mptr->menu;
                    break;
                }
            }
            if (mptr == NULL) {
		(void)sprintf(error_msg, "non-existent menu reference: \"%s\"",
                        bptr->menuname);
		Warning(error_msg);
                Startup_File_Error = TRUE;
            }
        }
    }
    CheckMenus();
}

/*
 * Verify that the menu variables have reasonable values
 */
VerifyMenuVariables()
{
    /*
     * If we pushrelative, we divide the window size by
     * the push variable. If it's zero, we die a sad death.
     * So lets use the default push value in this case.
     */
    if (!Pushval && Push) Pushval = DEF_PUSH;
}


/*
 * Check nested menu consistency by verifying that every menu line that
 * calls another menu references a menu that actually exists.
 */
CheckMenus()
{
    MenuLink *ptr;
    Bool errflag = FALSE;

    for(ptr = Menus; ptr; ptr = ptr->next) {
        if (ChkMline(ptr->menu))
            errflag = TRUE;
    }
    if (errflag)
        Error("Nested menu inconsistency");
}

Bool ChkMline(menu)
MenuInfo *menu;
{
    ActionLine *ptr;
    MenuLink *lptr;
    Bool errflag = FALSE;

    for(ptr = menu->line; ptr; ptr = ptr->next) {
        if (ptr->type == IsMenuFunction) {
            for(lptr = Menus; lptr; lptr = lptr->next) {
                if(!(strcmp(ptr->text, lptr->menu->name))) {
                    ptr->menu = lptr->menu;
                    break;
                }
            }
            if (lptr == NULL) {
		(void)sprintf(error_msg, "non-existent menu reference: \"%s\"",
                        ptr->text);
		Warning(error_msg);
                errflag = TRUE;
            }
        }
    }
    return(errflag);
}

/*
 * Grab the mouse buttons according to the bindings list.
 */

Grab_Buttons()
{
     Binding *bptr;
     Boolean NeedRootInput=FALSE;
     XSetWindowAttributes swa;
     
     Entry("Grab_Buttons")
     
     /*
      * don't grab buttons if you don't have to - allow application
      * access to buttons unless context includes window.
      */
     for (bptr = Blist; bptr; bptr = bptr->next) {
	  if (bptr->context & WINDOW) /* We gotta grab on windows */
	       GrabAll(bptr->mask);
	  if (bptr->context & ROOT)
	      /* must select on root, regardless of window grabs */
	       NeedRootInput = TRUE;
     }
     /* 
      * select for root input
      */
     
     if (NeedRootInput) {
	swa.event_mask = ROOTEVENTMASK | EVENTMASK | OwnerGrabButtonMask;
	XChangeWindowAttributes(dpy, RootWindow(dpy, scr), CWEventMask, &swa);
     }

     Leave_void
}

/*
 * Register a grab on all windows in the hierarchy. This is better than
 * registering a grab on the RootWindow since it leaves button/key chords
 * available for other contexts.
 */
GrabAll(mask)
unsigned int mask;
{
     unsigned int nkids;
     Window *kiddies = (Window *) NULL;	    /* not reqd by CURRENT Xlib code */
     Window junk;
     unsigned int i;
     XWindowAttributes xwa;
     AwmInfoPtr awi;
     
     Entry("GrabAll")
     
     if (XQueryTree(dpy, DefaultRootWindow(dpy), &junk, &junk, &kiddies, &nkids)) {
	  
	  for (i = 0; i < nkids; i++) {
	       /* check to see if it's a popup or something */
	      if ((awi = GetAwmInfo(kiddies[i])) != NULL) {
		  xwa = awi->xwa;
		  kiddies[i] = awi->client;
	      }
	      else
		  XGetWindowAttributes(dpy, kiddies[i], &xwa);
	      if (xwa.class == InputOutput && xwa.map_state == IsViewable &&
		  xwa.override_redirect == False)
		  Grab(mask, kiddies[i]);
	  }
	  if (kiddies)
	      XFree(kiddies);
     }
     else
	  Error("Can't XQueryTree in GrabAll!\n");
     Leave_void
}

/*
 * Grab a mouse button according to the given mask.
 */
Grab(mask, w)
unsigned int mask;
Window w;
{
     unsigned int m = LeftMask | MiddleMask | RightMask;
     
     Entry("Grab")
     
     switch (mask & m) {
     case LeftMask:
	  XGrabButton(dpy, LeftButton, mask & ModMask, w, TRUE, EVENTMASK,
		      GrabModeAsync, GrabModeAsync, None, LeftButtonCursor);
	  break;
	  
     case MiddleMask:
	  XGrabButton(dpy, MiddleButton, mask & ModMask, w, TRUE, EVENTMASK,
		      GrabModeAsync, GrabModeAsync, None, MiddleButtonCursor);
	  break;
	  
     case RightMask:
	  XGrabButton(dpy, RightButton, mask & ModMask, w, TRUE, EVENTMASK,
		      GrabModeAsync, GrabModeAsync, None, RightButtonCursor);
	  break;
     }
     Leave_void
}

/*
 * Restore cursor to normal state.
 */
ResetCursor(button)
int button;
{
     Entry("ResetCursor")
     
     switch (button) {
     case LeftButton:
	  XChangeActivePointerGrab(dpy, EVENTMASK, LeftButtonCursor,
				   CurrentTime);
	  break;
	  
     case MiddleButton:
	  XChangeActivePointerGrab(dpy, EVENTMASK, MiddleButtonCursor,
				   CurrentTime);
	  break;
	  
     case RightButton:
	  XChangeActivePointerGrab(dpy, EVENTMASK, RightButtonCursor,
				   CurrentTime);
	  break;
     }
     Leave_void
}

/*
 * error routine for .tekwmrc parser
 */
yyerror(s)
char*s;
{
     Entry("yyerror")
     
     (void) sprintf(error_msg, "%s: Line %d: %s", sfilename, Lineno, s);
     Warning(error_msg);
     Startup_File_Error = TRUE;
     Leave_void
}

/*
 * warning routine for .tekwmrc parser
 */
yywarn(s)
char*s;
{
     Entry("yywarn")
     
     (void) sprintf(error_msg, "%s: Line %d: %s", sfilename, Lineno, s);
     Warning(error_msg);
     Leave_void
}

/*
 * Print usage message and quit.
 */
Usage()
{
     Entry("Usage")
	  
     fprintf (stderr,
	    "usage:  %s [-display host:display] [-b] [-h] [-f file] [-s file] [-e file]\n\n",
	     Argv[0]);
     fputs("The -b option bypasses system and default bindings\n", stderr);
     fputs("The -h option obeys the client's StateHint at startup\n", stderr);
     fputs("The -i option ignores the $HOME/.tekwmrc file\n", stderr);
     fputs("The -f option specifies an alternate startup file\n", stderr);
     fputs("The -e option specifies a program/script to exec after startup\n",
	   stderr);
     fputs("The -s option specifies the session restart program to exec on an\nf.restartsession (default xusersetup)\n", stderr);
     exit(1);
}

/*
 * error handler for X I/O errors
 */
/*ARGSUSED*/
XIOError(dsp)
Display *dsp;
{
     fprintf(stderr, "tekwm: got an XIOError - connection to X server lost.\n");
     perror("tekwm");
     exit(3);
}

/*
 * Check for a window with WM_CLASS = "*Wm*", if found, terminate the wm.
 * Then create one of your own.
 */
RegisterWm()
{
    unsigned int nkids;
    Window *kiddies = (Window *) NULL;	    /* not reqd by CURRENT Xlib code */
    Window junk;
    Window old_WMSMWin = None;
    int count, kid, i;
    static XClientMessageEvent Event;
    Atom *wm_protocols;
    int timeout = 45;		    /* seconds - can be very large */
    XEvent p_ev;
    XClassHint *class_hint;
    Boolean gotit = FALSE;

    Entry("RegisterWm")
     
    class_hint = XAllocClassHint();	/* in Xicc */
    /* this is startup, initialize these here */
    WmProtocols = XInternAtom(dpy, "WM_PROTOCOLS", False);
    WmPrepareToDie = XInternAtom(dpy, "WM_PREPARE_TO_DIE", False);
    WmClosedown = XInternAtom(dpy, "WM_CLOSEDOWN", False);
    WmDeleteWindow = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    WmChangeState = XInternAtom(dpy, "WM_CHANGE_STATE", False);
    Event.type = ClientMessage;
    Event.message_type = WmProtocols;
    Event.format = 32;	/* timestamp */
    Event.data.l[0] = WmPrepareToDie;
    Event.data.l[1] = (long) 0;
    /* we have no events yet, so can't set time */

    /* looking at _XEventToWire, these are the fields to set */
    /* make sure that another wm does not come up simultaneously and
       create a protocols window after we have hunted for one. */
    XGrabServer(dpy);
    if (XQueryTree(dpy, DefaultRootWindow(dpy), &junk, &junk, &kiddies, &nkids)) {
	for (kid = 0; kid < nkids; kid++) {
	    /* should be faster if we start from bottom of window stack */
	    if (IsClassHintWm(kiddies[kid], class_hint)) {
		old_WMSMWin = kiddies[kid];
		break;	    /* surely there are not two WMSM windows!!! */
	    }
	}
	if (kiddies)
	    XFree(kiddies);
    }
    else
	Error("Can't XQueryTree in RegisterWm!");
    /* now initialize our own WM_PROTOCOLS window */
    WMSMWindow = XCreateSimpleWindow(dpy, RootWindow(dpy, scr),
				     0, 0, 1, 1, 0, BlackPixel(dpy, scr), 
				     BlackPixel(dpy, scr));
    if (WMSMWindow == NULL) Error("Couldn't create tekwm WM_PROTOCOLS window");

    XLowerWindow(dpy, WMSMWindow);	    
    /* Find it again faster.  Also, this insures that a newer tekwm will find
       the latest WMSMWindow, in case one is running and two are started. */
    XSetWMProtocols (dpy, WMSMWindow, &WmPrepareToDie, 1);  /* in Xicc */
    XStoreName(dpy, WMSMWindow, "Tekwm WM_PROTOCOLS window");
    /* just for xwininfo */
    class_hint->res_name = NAME;
    class_hint->res_class = CLASS;
    XSetClassHint(dpy, WMSMWindow, class_hint);
    XFree(class_hint);
    /* XXX really should initialize WM_COMMAND and WM_CLIENT_MACHINE here */

    /* we have the wm's window, therefore it must be running.  Do a Terminate()
       on it.  Wait right here until the wm says its safe, then do an
       XKillClient().  If the wm has already disconnected, we drop into the
       error handler and come back.  If the wm takes "forever", assume that it
       disconnected. */
    if (old_WMSMWin != None) {
        if (XGetWMProtocols(dpy, old_WMSMWin, &wm_protocols, &count)) {
	    for (i=0; (i < count) && (wm_protocols[i] != WmPrepareToDie); i++);
	    XFree(wm_protocols);
	    if (i < count) {
		/* got a match - send message and wait */
		XSelectInput(dpy, old_WMSMWin, PropertyChangeMask);

		/* tell wm to save itself, then update WM_COMMAND */
		Event.window = old_WMSMWin;
		XSendEvent(dpy, old_WMSMWin, False, NULL, &Event);
		/* OK, now it is safe to let the original wm run - a new one
		   will see our WMSMWindow first and may kill us before we are
		   done. */
		XUngrabServer(dpy);
		XSync(dpy, FALSE);
		while (timeout--) {
		    if (XPending(dpy)) {
			/* there really should be no other events */
			XNextEvent(dpy, &p_ev);
			if ((p_ev.type == PropertyNotify) &&
			    (p_ev.xproperty.atom == XA_WM_COMMAND))
			    break;
		    }
		    if (!XGetWMProtocols(dpy, old_WMSMWin, 
					 &wm_protocols, &count)) {
			break;
			/* window disappeared; window manager must have gone
			   away; proceed */
		    }
		    else
			XFree(wm_protocols);
		    sleep(1);	/* second * 60 counts */
		}
		if(!timeout) {
		    fprintf(stderr, "Hmm, window manager is not responding to WM_SAVE_YOURSELF - kill it!\n");
		}
	    }
	}
	/* make sure its really dead, even if the prop went away! */
	XKillClient(dpy, old_WMSMWin);
    }
    /* OK, now it is safe to let a new wm run - it will see our WMSMWindow
       first and may kill us before we are done. */
    XUngrabServer(dpy);
    XSync(dpy, FALSE);

    Leave_void
}

IsClassHintWm(window, class_hint)
Window window;
XClassHint *class_hint;		/* points to a usable class hint - faster */
{
    char *name;
     
    Entry("IsClassHintWm")
     
    if (XGetClassHint(dpy, window, class_hint)) {
	name = strtok(class_hint->res_class, ".");	    /* get first class name */
	while (name) {
	    if (!strcmp(name, WMCLASS)) {	    /* found "Wm" */
	        XFree(class_hint->res_name);
		XFree(class_hint->res_class);
		Leave(TRUE)
	    }
	    name = strtok(NULL, ".");    /* get next class name in hierarchy */
	}
        XFree(class_hint->res_name);
	XFree(class_hint->res_class);
    }
    Leave(FALSE)
}

/*
 *  A button is down; wait until all buttons are up.
 */
WaitForButtonsUp()
{
    XEvent button_event;			/* Button event packet. */

    Entry("WaitForButtonsUp")
     
    while (TRUE) {
        if (!GetButton(&button_event)) continue;
        if (button_event.type != ButtonRelease) continue;
	/* Keep going until all buttons released, to swallow chorded buttons
	   released last. */
	if (IsButtonDown(button_event.xbutton.state)) continue;
	break;
    }
    Leave_void
}
/*
 *  Is a button still down after a ButtonRelease?
 */
Boolean IsButtonDown(state)
int state;	    	    /* button_event.xbutton.state */
{
    Entry("IsButtonDown")
     
    /* All buttons are released if there was only one left down before the
    release */
    for (state &= (Button1Mask | Button2Mask | Button3Mask |
		   Button4Mask | Button5Mask);
	 !(state & 1);
         state = (unsigned int)(state) >> 1); 
    if (state & ~1)
	Leave(TRUE)
    else
	Leave(FALSE)
}
