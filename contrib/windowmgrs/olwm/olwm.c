/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) olwm.c 5.1 89/12/14 Crucible";
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <memory.h>
#include <sys/wait.h>
#include <sys/time.h>			/* these two need to be included */
#include <sys/resource.h>		/* when using wait3(), per doc */
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "menu.h"
#include "olwm.h"

/*
 * Externals
 */
extern int	ReparentFlag;
extern int	PrintOrphans;
extern int	PrintAll;
extern Window	CurrColormapWindow;
extern Colormap	CurrColormap;
extern int	DialogueBox();

/*
 * Globals
 */
char	*ProgramName;

int	FrameFullAction(), Exit(), FrameLimAction(), DismissAction();
int 	IconFullAction();
int	MakeMenuWin();

int	NoFocusEvent();
Window	NoFocusWin;

/* the default button mapping */
int	ButtonMap[] = {
		-1,		/* Button Zero */
		MB_SELECT,	/* Button1 */
		MB_ADJUST,	/* Button2 */
		MB_MENU,	/* Button3 */
		-1,		/* Button4 */
		-1,		/* Button5 */
		};

/* The Properties button is currently disabled in all of these menus */
Button IconFullButtons[] = {
        { "Open", False, Enabled, {IconFullAction, NULL} },
        { "Full Size", False, Enabled, {IconFullAction, NULL} },
        { "Properties", False, Disabled, {IconFullAction, NULL} },
        { "Back", False, Enabled, {IconFullAction, NULL} },
        { "Refresh", False, Enabled, {IconFullAction, NULL} },
        { "Quit", False, Enabled, {IconFullAction, NULL} },
        };
Menu IconFullMenu = { "Window", IconFullButtons, 6, 0, False, MakeMenuWin};

Button FrameNormButtons[] = {
        { "Close", False, Enabled, {FrameFullAction, NULL} },
        { "Normal Size", False, Enabled, {FrameFullAction, NULL} },
        { "Properties", False, Disabled, {FrameFullAction, NULL} },
        { "Back", False, Enabled, {FrameFullAction, NULL} },
        { "Refresh", False, Enabled, {FrameFullAction, NULL} },
        { "Quit", False, Enabled, {FrameFullAction, NULL} },
        };
Menu FrameNormMenu = { "Window", FrameNormButtons, 6, 0, False, MakeMenuWin};

Button FrameFullButtons[] = {
        { "Close", False, Enabled, {FrameFullAction, NULL} },
        { "Full Size", False, Enabled, {FrameFullAction, NULL} },
        { "Properties", False, Disabled, {FrameFullAction, NULL} },
        { "Back", False, Enabled, {FrameFullAction, NULL} },
        { "Refresh", False, Enabled, {FrameFullAction, NULL} },
        { "Quit", False, Enabled, {FrameFullAction, NULL} },
        };
Menu FrameFullMenu = { "Window", FrameFullButtons, 6, 0, False, MakeMenuWin};

Button DismissButtons[] = {
        { "This Window", False, Enabled, {DismissAction, NULL} },
        { "All Pop-ups", False, Enabled, {DismissAction, NULL} },
	};
Menu DismissMenu = { NULL, DismissButtons, 2, 0, False, NULL};
	
Button FrameLimButtons[] = {
	{ "Dismiss", True, Enabled, {(FuncPtr) 0, (void *) &DismissMenu} },
        { "Back", False, Enabled, {FrameLimAction, (void *)0} },
        { "Refresh", False, Enabled, {FrameLimAction, (void *)0} },
        { "Owner?", False, Enabled, {FrameLimAction, (void *)0} },
	};
Menu FrameLimMenu = { "Window", FrameLimButtons, 4, 0, False, MakeMenuWin};

/* Current Display/Screen */
Display	*DefDpy;
int	DefScreen;
char	*DisplayName = NULL;

/* Command line initializable variables */
int	FocusFollowsMouse = False;	/* Default to ClickToFocus */ 
int	FocusModeOverride = False;	/* set if focus mode set on cmd line */
int	ReparentFlag = True;		/* Reparent existing windows */
Bool	f3dSet = False;			/* use 3D look? */
Bool	f3dUsed = False;

/* Child Process Handling */
void HandleChildSignal();
void ReapChildren();
static int DeadChildren = 0;

/* Following are strictly for debugging purposes and are not mentioned
 * in the usage message.
 * Orphaned events are events that are associated with a window or frame 
 * has no entry in the frame hash table, or events that are not handled by the
 * various event handlers.
 * 'PrintAll' is useful for when verification of an events existance is needed.
 */
int	PrintOrphans = False;		/* Don't print orphan events */
int	PrintAll = False;		/* Don't print all events */

Atom	AtomWinAttr;
Atom	AtomPinOut;
Atom	AtomDecorResize;
Atom	AtomWTBase;
Atom	AtomDecorFooter;
Atom	AtomDecorAdd;
Atom	AtomDecorDelete;
Atom	AtomDecorPin;
Atom	AtomWTProp;
Atom	AtomWTCmd;
Atom	AtomPinIn;
Atom	AtomNone;
Atom	AtomWTNotice;
Atom	AtomMenuFull;
Atom	AtomDecorHeader;
Atom	AtomWTHelp;
Atom	AtomMenuLimited;
Atom	AtomDecorClose;
Atom	AtomWTOther;
Atom	AtomPushpinState;
Atom	AtomShowProperties;
Atom	AtomProtocols;
Atom	AtomSaveYourself;
Atom	AtomTakeFocus;
Atom	AtomDeleteWindow;
Atom	AtomChangeState;
Atom	AtomWMApplyProperties;
Atom	AtomWMResetProperties;
Atom	AtomLeftFooter;
Atom	AtomRightFooter;
Atom	AtomWindowBusy;
Atom	AtomWMState;
Atom	AtomColorMapWindows;
Atom	AtomTargets;
Atom	AtomMultiple;
Atom	AtomTimestamp;
Atom	AtomListLength;
Atom	AtomLength;
Atom	AtomOlwmTimestamp;

char	**Envp;


/*
 * main	-- parse arguments, perform initialization, call event-loop
 *
 * The OpenLook window manager was written before the OpenLook
 * spec. was completed.  Because of this, some of it works in
 * an awkward way.  Specifically, the way a window's focus and
 * select state is shown.
 *
 * For example, when a window is focused in click-to-focus mode
 * the header gets highlighted, (black background, white text).
 * As it was written, the titlebar window sits just inside of the 
 * resize corners.  Because the OpenLook spec. requires the header to 
 * be highlighted in line with the resize corners, blacking in the
 * the background of the titlebar window is one pixel short in each
 * direction of being a big enough highlighted area.  We can't make
 * the titlebar bigger because it would then overlap the resize corners.
 * Now that the spec. is complete, OLWM should be restructured.
 *
 * Overview:
 *
 *	Upon startup OLWM reparents all the client windows unless
 *	the user has told it otherwise (-p option).
 * 	OLWM creates a frame which will contain the client window (the pane)
 *  	and the decoration windows (resize corners, titlebar).  The
 *	titlebar window contains the pushpin, if any.
 *
 *	After creating the decorations for the windows OLWM sits
 * 	in a forever loop getting the next event and calling the
 * 	corresponding window's event handling routine.
 *
 *	Each window has associated with it an event handling 
 * 	routine (EventFrame, EventPushPin) which the main EventLoop
 *	will call when an event happens in that window.  The event
 *	handling routine will call the proper routines to move a 
 *	window, create a menu, etc.
 */
main( argc, argv, envp )
int	argc;
char	**argv;
char	**envp;
{
	int			ii;
	WinInfo			*rootInfo;
	extern int		rootEvent();
	int			ErrorHandler();
	WinInfo			*noFocusWinInfo;
	XSetWindowAttributes	attributes;
	void			ExitOLWM();
	Bool			changeEnvp = False;
	char			**updateEnvp();
	Bool			synchronize = False;

	ProgramName = argv[0];

	/* Set up a signal handler so that if we are killed we put
	 * the windows back into a usable state.  Specifically we
	 * need to reset the focus so they will be able to get input.
	 */
	signal(SIGHUP, ExitOLWM);
	signal(SIGINT, ExitOLWM);
	signal(SIGTERM, ExitOLWM);
	signal(SIGCHLD, HandleChildSignal);

	/* parse the command line arguments */
	for (ii=1; ii<argc; ii++)
	{
		if (argv[ii][0] != '-')
			usage("Unknown argument:", argv[ii]);

		switch (argv[ii][1])
		{
		case 'd':
			DisplayName = argv[++ii];
			changeEnvp = True;
			break;

		case 'f':	/* focus follows mouse */
			FocusFollowsMouse = True;
			FocusModeOverride = True;
			break;

		case 'c':	/* click to type */
			FocusFollowsMouse = False;
			FocusModeOverride = True;
			break;

		case '2':
			f3dSet = True;
			f3dUsed = False;
			break;

		case '3':
			f3dSet = True;
			f3dUsed = True;
			break;
		
		case 'p':
			ReparentFlag = False;
			break;
		
		case 'h':
			usage("Command line arguments accepted", "are:");
			break;

		/* Following are undocumented options that aid in 
		 * understanding the actions of awkward clients,
		 * and debugging olwm.
		 */
		case 'o':
			PrintOrphans = True;
			break;

		case 'a':
			PrintAll = True;
			break;

		case 's':
			synchronize = True;
			break;

		default:
			usage("Unknown flag in argument:", argv[ii]);
			break;
		}
	}

	/* Open the display connection. */
	if ((DefDpy = XOpenDisplay(DisplayName)) == NULL)
	{
		fprintf( stderr, "%s: cannot connect to %s\n",
			ProgramName, DisplayName );
		exit( -1 );
	}

	/* if display was changed on command line, need to change in Envp */
	if ( changeEnvp == True )
		Envp = updateEnvp( envp, "DISPLAY", DisplayName );
	else
		Envp = envp;

	DefScreen = DefaultScreen( DefDpy );

	/* Set up the error handling function. */
	XSetErrorHandler(ErrorHandler);

	/* this can be useful for debuging strange client behaivior */
	if (synchronize)
		XSynchronize(DefDpy, True);

	/* Tell server we need to get mapping requests.
	 * ErrorSensitive is used to check for the possibility
	 * of another window manager running, normally XSelectInput
	 * generates warnings.
	 */
	ErrorSensitive("Perhaps there is another window manager running?");
	XSelectInput( DefDpy, RootWindow(DefDpy, DefScreen),
			KeyPressMask | SubstructureRedirectMask |
			ButtonPressMask | ButtonReleaseMask |
			Button1MotionMask | Button2MotionMask |
			EnterWindowMask );
	XSync(DefDpy, False);
	ErrorInsensitive(DefDpy);

	/* Set defaults. */
	GetDefaults(DefDpy, argc, argv);

	/* Init graphics stuff. */
	InitGraphics( DefDpy );

	/* Initialize icon data */
	InitIcon( DefDpy );

	/* Initialize the pixmaps and related adornment stuff. */
	InitAdornments( DefDpy );

	/* Initialize the atoms we will need to communicate with
	 * Open Look clients.
	 */
	InitAtoms(DefDpy);

	/*
	 * The following code is temporary, being used to generate the
	 * menus on the frame.
	 */
	InitMenus(DefDpy);
	MenuCreate(DefDpy, &IconFullMenu);
	MenuCreate(DefDpy, &FrameNormMenu);
	MenuCreate(DefDpy, &FrameFullMenu);
	MenuCreate(DefDpy, &FrameLimMenu);
	MenuCreate(DefDpy, &DismissMenu);
	InitUserMenu(DefDpy);

	/* Initialize the database. */
	WIInit( DefDpy );

	/*
	 * Force child processes to disinherit the TCP file descriptor.
	 * This helps shell commands forked and exec'ed from menus
	 * to work properly.
	 */
	if (fcntl(ConnectionNumber(DefDpy), F_SETFD, 1) == -1)
	{
		perror( "olwm: child cannot disinherit TCP fd" );
		exit( -1 );
	}

	/*
	 * Make an input only window that is invisible.
	 * This window will have the focus when no client
	 * window has the focus.  This is done so olwm
	 * can beep at the user when they hit a key while
	 * no window has the input focus.
	 */
	attributes.event_mask = KeyPressMask;
	attributes.override_redirect = True;
	NoFocusWin = XCreateWindow(DefDpy, RootWindow(DefDpy, DefScreen), 
				   -10, -10, 10, 10, 0, 0, InputOnly, 
				   CopyFromParent, 
				   CWEventMask | CWOverrideRedirect,
				   &attributes);
	noFocusWinInfo = WICreateInfo(NoFocusWin);
	noFocusWinInfo->frame = noFocusWinInfo;
	noFocusWinInfo->self = NoFocusWin;
	noFocusWinInfo->eventfunc = NoFocusEvent;

	XMapWindow(DefDpy, NoFocusWin);

	/* Set the focus to the NoFocusWin.
	 * 	(RevertToNone is cast from long to 
	 *	int - lint will warn) 
	 */
	XSetInputFocus(DefDpy, NoFocusWin, RevertToNone, CurrentTime);

	/* Setup the colormap globals */
	CurrColormapWindow = RootWindow(DefDpy, DefScreen);
	CurrColormap = None;

	/* Add the root window to the window database. */
	rootInfo = WICreateInfo( RootWindow(DefDpy, DefScreen) );
	rootInfo->drawfunc = NULL;
	rootInfo->eventfunc = rootEvent;
	rootInfo->destroyfunc = NULL;

	if (ReparentFlag)
		ReparentTree(DefDpy);

	XBell(DefDpy, 100);
	EventLoop();

	/*NOTREACHED*/
}

/*
 * EventLoop	-- this is the main event loop, all events during times
 *		   when the server is not grabbed, come through here
 *		   first.
 *
 * Purpose:
 *	This routine sits in a forever loop reading events.  Events are
 *	dispatched to the appropriate window via the winInfo hash table.
 */
EventLoop()
{
	XEvent		event;
	WinInfo		*winInfo;

	for (;;)
	{
		XNextEvent(DefDpy, &event);

		if (DeadChildren > 0)
			ReapChildren();

		/* If this is an expose event, and there are more
		 * comming for this window, don't send it on to the
		 * window.  We don't want to redraw the window more
		 * times than we need to.
		 */
		if ((event.type == Expose) && (event.xexpose.count > 0))
			continue;

		winInfo = WIGetInfo(event.xany.window);

		if (PrintAll)
			DebugEvent(&event, "Debug");

		if ((winInfo != NULL) && (winInfo->eventfunc != NULL))
			(*winInfo->eventfunc)(DefDpy, &event, winInfo);
		else
			if (PrintOrphans)
				DebugEvent(&event, "Orphaned");
	}

	/*NOTREACHED*/
}

/* MouseButton	-- convert a button press to a logical button
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
MouseButton(dpy, event)
Display	*dpy;
XEvent	*event;
{
	return ButtonMap[event->xbutton.button];
}

/* GetSelectButtonMapping -- return the current MB_SELECT button
 *
 *	Currently unused.
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
GetSelectButtonMapping(dpy)
Display	*dpy;
{
	int		i;
	static int	button = -1;
	static int	found = False;

	if (!found)
	{
		for (i=Button1; i<Button5; i++)
			if (ButtonMap[i] == MB_SELECT)
				button = i;

		/* if the user has not specified a Select button,
		 * then use Button1 as a backup.
		 */
		if (button == -1)
			button = Button1;
	}

	return button;
}

/* IntMalloc	-- internal malloc, this isolates the calls to malloc and
 * 		   automatically zeros out the memory allocated.
 */
void *
IntMalloc(size)
unsigned	size;
{
	char	*malloc();
#ifdef __STDC__
	void	*ret = (void *)malloc(size);
#else
	char	*ret = (void *)malloc(size);
#endif

	/* Zero out that memory.  Note that memset expects a signed 3rd arg*/
	memset((char *)ret, 0, (int)size);

	return	ret;
}

/*
 * InitAtoms -- initialize the atoms needed to communicate with Open
 *	Look clients
 */
static
InitAtoms(dpy)
Display	*dpy;
{
	/* ICCCM specific flags */
	AtomColorMapWindows = XInternAtom(dpy, "WM_COLORMAP_WINDOWS", False);
	AtomWMState = XInternAtom(dpy, "WM_STATE", False);
	AtomChangeState = XInternAtom(dpy, "WM_CHANGE_STATE" , False);
	AtomProtocols = XInternAtom(dpy, "WM_PROTOCOLS" , False);
	AtomTakeFocus = XInternAtom(dpy, "WM_TAKE_FOCUS" , False);
	AtomSaveYourself = XInternAtom(dpy, "WM_SAVE_YOURSELF" , False);
	AtomDeleteWindow = XInternAtom(dpy, "WM_DELETE_WINDOW" , False);

	/* OpenLook specific flags */
	AtomWinAttr = XInternAtom(dpy, "_OL_WIN_ATTR" , False);
	AtomPushpinState = XInternAtom(dpy, "_OL_PIN_STATE" , False);
	AtomWindowBusy = XInternAtom(dpy, "_OL_WIN_BUSY" , False);
	AtomLeftFooter = XInternAtom(dpy, "_OL_WINMSG_ERROR" , False);
	AtomRightFooter = XInternAtom(dpy, "_OL_WINMSG_STATE" , False);
	AtomShowProperties = XInternAtom(dpy, "_OL_SHOW_PROPS" , False);
	AtomWMApplyProperties = XInternAtom(dpy, "_OL_PROPS_APPLY" , False);
	AtomWMResetProperties = XInternAtom(dpy, "_OL_PROPS_RESET" , False);
	AtomPinOut = XInternAtom(dpy, "_OL_PIN_OUT" , False);
	AtomDecorResize = XInternAtom(dpy, "_OL_DECOR_RESIZE" , False);
	AtomWTBase = XInternAtom(dpy, "_OL_WT_BASE" , False);
	AtomDecorFooter = XInternAtom(dpy, "_OL_DECOR_FOOTER" , False);
	AtomDecorAdd = XInternAtom(dpy, "_OL_DECOR_ADD" , False);
	AtomDecorDelete = XInternAtom(dpy, "_OL_DECOR_DEL" , False);
	AtomDecorPin = XInternAtom(dpy, "_OL_DECOR_PIN" , False);
	AtomWTProp = XInternAtom(dpy, "_OL_WT_PROP" , False);
	AtomWTCmd = XInternAtom(dpy, "_OL_WT_CMD" , False);
	AtomPinIn = XInternAtom(dpy, "_OL_PIN_IN" , False);
	AtomNone = XInternAtom(dpy, "_OL_NONE" , False);
	AtomWTNotice = XInternAtom(dpy, "_OL_WT_NOTICE" , False);
	AtomMenuFull = XInternAtom(dpy, "_OL_MENU_FULL" , False);
	AtomDecorHeader = XInternAtom(dpy, "_OL_DECOR_HEADER" , False);
	AtomWTHelp = XInternAtom(dpy, "_OL_WT_HELP" , False);
	AtomMenuLimited = XInternAtom(dpy, "_OL_MENU_LIMITED" , False);
	AtomDecorClose = XInternAtom(dpy, "_OL_DECOR_CLOSE" , False);
	AtomWTOther = XInternAtom(dpy, "_OL_WT_OTHER" , False);
	AtomOlwmTimestamp = XInternAtom(dpy,"_OLWM_TIMESTAMP",False);

	/* ICCCM selection atoms */
	AtomTargets = XInternAtom(dpy,"TARGETS",False);
	AtomMultiple = XInternAtom(dpy,"MULTIPLE",False);
	AtomTimestamp = XInternAtom(dpy,"TIMESTAMP",False);
	AtomListLength = XInternAtom(dpy,"LIST_LENGTH",False);
	AtomLength = XInternAtom(dpy,"LENGTH",False);
}

/*ARGSUSED*/	/* winInfo, menu, and idx args will be used 
		 * when more sophisticated ExitFunc is written */
int 
ExitFunc(dpy, winInfo, menu, idx)
Display *dpy;
WinInfo *winInfo;
Menu *menu;
int idx;
{
	int	screen;

	/* multiple screens not yet supported */
	screen = DefaultScreen( dpy );

	/* If Exit button is selected, will return 0 */
	if ( DialogueBox( dpy, screen,
			  "Please confirm exit from window system",
		     	  "Exit", "Cancel", 1, -1, -1 ) == 0 )
		Exit(dpy);
}

/*ARGSUSED*/	/* dpy, winInfo, menu, and idx args will be used 
		 * when Clipboard is implemented */
int 
ClipboardFunc(dpy, winInfo, menu, idx)
Display *dpy;
WinInfo *winInfo;
Menu *menu;
int idx;
{
	fprintf(stderr,"Clipboard not yet implemented\n");
}

/*ARGSUSED*/	/* dpy, winInfo, menu, and idx args will be used 
		 * when Print Screen is implemented */
int 
PrintScreenFunc(dpy, winInfo, menu, idx)
Display *dpy;
WinInfo *winInfo;
Menu *menu;
int idx;
{
	fprintf(stderr,"Print screen not yet implemented\n");
}


/*
 * RefreshFunc -- called when the "Refresh Screen" item has been selected on
 *	the olwm menu
 */
/*ARGSUSED*/	/* winInfo, menu, and idx args will be used later */
int 
RefreshFunc(dpy, winInfo, menu, idx)
Display	*dpy;
WinInfo	*winInfo;
Menu	*menu;
int	idx;
{
	Window	w;
	XSetWindowAttributes xswa;

	/* We create a window over the whole screen, map it,
	 * then destroy it.
	 */
	xswa.override_redirect = True;
	w = XCreateWindow(dpy, 
			  RootWindow(dpy, DefaultScreen(dpy)),
			  0, 0,
			  DisplayWidth(dpy,DefaultScreen(dpy)),
			  DisplayHeight(dpy,DefaultScreen(dpy)),
			  0,
			  DefaultDepth(dpy,DefaultScreen(dpy)), 
			  InputOutput, 
			  DefaultVisual(dpy,DefaultScreen(dpy)),
			  CWOverrideRedirect, 
			  &xswa);

	XMapRaised(dpy, w);
	XDestroyWindow(dpy, w);
	XFlush(dpy);
}


#define WORKSPACEPROPS "props"

/*
 * PropertiesFunc -- called when the "Properties ..." item has been selected 
 * on the root menu.  REMIND: this and AppMenuFunc should be merged.
 */
/*ARGSUSED*/	/* dpy, winInfo, menu, and idx args will be used later */
int
PropertiesFunc(dpy,winInfo,menu,idx)
Display	*dpy;
WinInfo	*winInfo;
Menu	*menu;
int	idx;
{
	if (!fork())
	{
		execlp(WORKSPACEPROPS,WORKSPACEPROPS,(char *)0);

		/* this should put up a dialog box */
		fprintf(stderr,"Unable to exec \"%s\" command.\n",
			WORKSPACEPROPS);
		exit(0);
		/*NOTREACHED*/
	}
}

/*
 * Exit -- exit routine called by menus.
 *	Kill all running applications.
 */
Exit(dpy)
Display	*dpy;
{
	WinInfo		*winInfo;
	FrameInfo	*frameInfo;
	XEvent		clientEvent;

	winInfo = 0;
	while(winInfo = EnumFrames(winInfo))
	{
		frameInfo = (FrameInfo *)(winInfo->priv.ptr);

		if (frameInfo->protocols & DELETE_WINDOW)
		{
			/* Send a ClientMessage of type WM_DELETE_WINDOW. */
			clientEvent.xclient.type = ClientMessage;
			clientEvent.xclient.message_type = AtomProtocols;
			clientEvent.xclient.format = 32;
			clientEvent.xclient.display = dpy;
			clientEvent.xclient.window = GetPane(winInfo);
			clientEvent.xclient.data.l[0] = AtomDeleteWindow;
			clientEvent.xclient.data.l[1] = CurrentTime;

			/* Send the event. */
			XSendEvent(dpy, 
				   clientEvent.xclient.window, 
				   False,
				   NoEventMask, 
				   &clientEvent);
		}
		else
		{
			/* Forcibly remove the client */
			XKillClient(dpy, GetPane(winInfo));
		}
	}

	XSync(dpy, True);
	/* Exit the window manager. */
	exit(0);
	/*NOTREACHED*/
}

/* This is so we can exit OLWM cleanly. */
void
ExitOLWM()
{
	WinInfo		*winInfo;
	WinInfo		*iconWinInfo;
	IconInfo	*iconInfo;
	FrameInfo	*frameInfo;
	Window		pane;

	/* Reset the input focus so it will be
	 * RevertToRoot instead of RevertToNone.
	 * If we don't do this, when OLWM exits
	 * the user won't be able to get any client
	 * window to get the focus.
	 * 	(RevertToPointerRoot is cast from long to 
	 *	int - lint will warn)
	 */
	XSetInputFocus(DefDpy, PointerRoot, RevertToPointerRoot,CurrentTime);

	/*
	 * Go through the list of windows.  Unmap all icons that are on the
	 * screen.  Reparent all windows back to the root, suitably offset
	 * according to their window-gravities.  Also remap all non-withdrawn
	 * windows, and remove all Withdrawn windows from the save-set (so
	 * they don't get remapped.  REMIND: We have to do this because
	 * Withdrawn windows are still left reparented inside the frame; this
	 * shouldn't be the case.
	 */
	winInfo = 0;
	while(winInfo = EnumFrames(winInfo))
	{
		if (winInfo->frame->winState == Iconic)
		{
			iconWinInfo = GetIcon(winInfo->frame);
			iconInfo = (IconInfo *)(iconWinInfo->priv.ptr);
			if (iconInfo->iconWin)
				XUnmapWindow(DefDpy, iconInfo->iconWin);
		}

		frameInfo = (FrameInfo *)(winInfo->frame->priv.ptr);
		pane = GetPane(winInfo->frame);
		XReparentWindow(DefDpy,pane,DefaultRootWindow(DefDpy),
		    winInfo->frame->x + frameInfo->offsetX,
		    winInfo->frame->y + frameInfo->offsetY);
		XSetWindowBorderWidth(DefDpy,pane,
				      frameInfo->oldBorderWidth);

		if (winInfo->frame->winState == Withdrawn) {
		    XChangeSaveSet(DefDpy, pane, SetModeDelete);
		} else {
		    XMapWindow(DefDpy,pane);
		}
	}

	XSync(DefDpy, True);
	/* Exit OLWM */
	exit(0);

	/*NOTREACHED*/
}  /* End ExitOLWM */


/*
 * HandleChildSignal - keep track of children that have died
 */
static void
HandleChildSignal()
{
	++DeadChildren;
}

/*
 * ReapChildren - wait() for all dead child processes
 */
static void
ReapChildren()
{
	int oldmask;
	int pid;
	union wait status;

	oldmask = sigblock(sigmask(SIGCHLD));
	while (DeadChildren>0)
	{
		pid = wait3(&status, WNOHANG, (struct rusage *)0);
		if (pid == 0)
		{
			fputs("olwm: got SIGCHLD but found no children\n", stderr);
			DeadChildren = 0;
			break;
		}
		if (WIFSTOPPED(status))
		{
			kill(pid, SIGKILL);
		}
		--DeadChildren;
	}
	(void) sigsetmask(oldmask);
}

/*
 * NoFocusEvent - handle events that happen while the NoFocusWin has the focus.
 *
 * Purpose - When no client window has the focus, this window has
 * 	     the focus.  (Should have the focus).  Then, anytime the
 *	     user presses a key we can beep at them.
 *
 * Input:
 *	event	- event which happened 'in' the NoFocusWin.
 *
 *	winInfo	- winInfo structure for the NoFocusWin.
 */
/*ARGSUSED*/	/* winInfo arg included for consistency */
NoFocusEvent(dpy, event, winInfo)
Display *dpy;
XEvent  *event;
WinInfo *winInfo;
{
	XEvent dummy;

	switch(event->type)
	{
		case KeyPress:
			XSync(dpy,0);
			/* eat all keystrokes on the NoFocusWin */
			while (XCheckTypedWindowEvent(dpy,winInfo->self,
						      KeyPress,&dummy))
			    ;
			XBell(dpy, 100);
			break;
		
		default:
			DebugEvent(event, "NoFocusEvent");
			break;
	}
}

/*
 * usage(s1, s2)	-- print informative message regarding usage
 */
static
usage(s1, s2)
char	*s1, *s2;
{
	fprintf(stderr, "%s %s\n", s1, s2);
	fprintf(stderr, "usage: %s [-f][-p][-d <display>]\n", ProgramName);
	fprintf(stderr, "\t-d <display> Use specified display\n");
	fprintf(stderr, "\t-f\tFocus follows mouse input mode\n");
	fprintf(stderr, "\t-c\tClick to focus input mode (default)\n");
	fprintf(stderr, "\t-p\tDon't reparent existing windows\n");
	fprintf(stderr, "\t-3\tUse 3D look\n");
	fprintf(stderr, "\t-2\tUse 2D look\n");
	exit(1);
}

/*
 * updateEnvp - modify environment vector
 *
 * Purpose - Duplicate environment vector and set the named environment 
 *	     variable to the value passed.  Notice that space is created 
 *	     inside this routine for the new environment vector, but not freed.
 *
 * Input:
 *	envp	 - current environment vector
 *	
 *	variable - environment variable to be changed
 *
 *	newValue - new value for the variable
 *
 * Returns:
 *	New environment vector.
 */
static char **
updateEnvp( envp, variable, newValue )
char	**envp;
char	*variable;
char	*newValue;
{
	int	count;		/* counter for number of env variables */
	int	numVariables;
	char	**newEnvp;
	char	*StringDup();
	char	*newSetting;

	/* create new string to be put into environment */
	/* need space for null at end, and "=" in the middle */
	newSetting = (char *)IntMalloc( (unsigned)(strlen( variable ) 
					+ strlen( newValue ) + 2 )
				      );
	sprintf( newSetting, "%s=%s", variable, newValue );

	/* count existing env vector entries and make pointers in newEnvp */
	for ( count = 0 ; envp[count] != NULL ; count++ )
		;

	/* number of variables in env vector */
	numVariables = count;

	/* make space for numVariables + 2 pointers in newEnvp 
	 * (+ 1 for the NULL at end and + 1 in case the variable being 
	 * changed isn't already in the environment)
	 */ 
	newEnvp = (char **)IntMalloc( (unsigned)(
					sizeof(char *) * (numVariables + 2)
				      ));

	/* copy existing env vector */
	for ( count = 0 ; count < numVariables ; count++ )
		newEnvp[count] = StringDup( envp[count] ); 
	
	/* search through, checking for variable in question */
	for ( count = 0 ; count < numVariables ; count++ )
		if ( !strcmp( newEnvp[count], variable ) )
			break;
	
	newEnvp[count] = newSetting;

	if ( count == numVariables )	/* finished loop without match */
		numVariables++;		/* added 1 more var to the env */
	else
		free( newEnvp[numVariables+1] );	/* don't need */

	/* make sure the last entry in the vector is NULL */
	newEnvp[numVariables] = NULL;

	return( newEnvp );
}

/*
 * StringDup( oldString ) - allocate space for, then copy a string
 *
 * Returns:
 *	newString
 */
char *
StringDup( oldString )
char	*oldString;
{
	char	*newString;

	newString = (char *)IntMalloc( (unsigned)(strlen( oldString ) + 1) );
	strcpy( newString, oldString );
	return( newString );
}
