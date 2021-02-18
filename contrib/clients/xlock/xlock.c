#ifndef lint
static char sccsid[] = "@(#)xlock.c 22.4 89/09/23";
#endif
/*-
 * xlock.c - X11 client to lock a display and show a screen saver.
 *
 * Copyright (c) 1988-89 by Patrick Naughton and Sun Microsystems, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * This file is provided AS IS with no warranties of any kind.  The author
 * shall have no liability with respect to the infringement of copyrights,
 * trade secrets or any patents by this file or any part thereof.  In no
 * event will the author be liable for any lost revenue or profits or
 * other special, indirect and consequential damages.
 *
 * Comments and additions should be sent to the author:
 *
 *		       naughton@sun.com
 *
 *		       Patrick J. Naughton
 *		       Window Systems Group, MS 14-40
 *		       Sun Microsystems, Inc.
 *		       2550 Garcia Ave
 *		       Mountain View, CA  94043
 *
 * Revision History:
 * 23-Sep-89: Added fix to allow local hostname:0 as a display.
 *	      Put empty case for Enter/Leave events.
 *	      Moved colormap installation later in startup.
 * 20-Sep-89: Linted and made -saver mode grab the keyboard and mouse.
 *	      Replaced SunView code for life mode with Jim Graham's version,
 *		so I could contrib it without legal problems.
 *	      Sent to expo for X11R4 contrib.
 * 19-Sep-89: Added '?'s on input.
 * 27-Mar-89: Added -qix mode.
 *	      Fixed GContext->GC.
 * 20-Mar-89: Added backup font (fixed) if XQueryLoadFont() fails.
 *	      Changed default font to lucida-sans-24.
 * 08-Mar-89: Added -nice, -mode and -display, built vector for life and hop.
 * 24-Feb-89: Replaced hopalong display with life display from SunView1.
 * 22-Feb-89: Added fix for color servers with n < 8 planes.
 * 16-Feb-89: Updated calling conventions for XCreateHsbColormap();
 *	      Added -count for number of iterations per color.
 *	      Fixed defaulting mechanism.
 *	      Ripped out VMS hacks.
 *	      Sent to expo for X11R3 contrib.
 * 15-Feb-89: Changed default font to pellucida-sans-18.
 * 20-Jan-89: Added -verbose and fixed usage message.
 * 19-Jan-89: Fixed monochrome gc bug.
 * 16-Dec-88: Added SunView style password prompting.
 * 19-Sep-88: Changed -color to -mono. (default is color on color displays).
 *	      Added -saver option. (just do display... don't lock.)
 * 31-Aug-88: Added -time option.
 *	      Removed code for fractals to separate file for modularity.
 *	      Added signal handler to restore host access.
 *	      Installs dynamic colormap with a Hue Ramp.
 *	      If grabs fail then exit.
 *	      Added VMS Hacks. (password 'iwiwuu').
 *	      Sent to expo for X11R2 contrib.
 * 08-Jun-88: Fixed root password pointer problem and changed PASSLENGTH to 20.
 * 20-May-88: Added -root to allow root to unlock.
 * 12-Apr-88: Added root password override.
 *	      Added screen saver override.
 *	      Removed XGrabServer/XUngrabServer.
 *	      Added access control handling instead.
 * 01-Apr-88: Added XGrabServer/XUngrabServer for more security.
 * 30-Mar-88: Removed startup password requirement.
 *	      Removed cursor to avoid phosphor burn.
 * 27-Mar-88: Rotate fractal by 45 degrees clockwise.
 * 24-Mar-88: Added color support. [-color]
 *	      wrote the man page.
 * 23-Mar-88: Added HOPALONG routines from Scientific American Sept. 86 p. 14.
 *	      added password requirement for invokation
 *	      removed option for command line password
 *	      added requirement for display to be "unix:0".
 * 22-Mar-88: Recieved Walter Milliken's comp.windows.x posting.
 *
 * Contributors:
 *  milliken@heron.bbn.com
 *  karlton@wsl.dec.com
 *  dana@thumper.bellcore.com
 *  vesper@3d.dec.com
 */

#include <stdio.h>
#include <signal.h>
#include <sys/param.h>
#include <string.h>
#include <pwd.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

extern char *crypt();
extern char *getenv();

typedef struct {
    char       *cmdline_arg;
    int		(*lp_reinit) ();
    void	(*lp_callback) ();
    void	(*lp_init) ();
}	    lockProc;

/*
 * Declare external interface routines for supported screen savers.
 */

extern void randomInithop();
extern int  hopdone();
extern void hop();

extern void initlife();
extern int  lifedone();
extern void drawlife();

extern void initqix();
extern int  qixdone();
extern void drawqix();

lockProc    LockProcs[] = {
    {"hop", hopdone, hop, randomInithop},
    {"life", lifedone, drawlife, initlife},
    {"qix", qixdone, drawqix, initqix}
/*
 * New screen savers may be added here.
 * Refer to qix.c for simple sample screen saver.
 */

};
#define NUMPROCS (sizeof(LockProcs) / sizeof(lockProc))

int	    (*reinit) () = NULL;
void	    (*callback) () = NULL;
void	    (*init) () = NULL;

char	   *pname;		/* argv[0] */
Display	   *dsp = NULL;		/* server display connection */
int	    screen;		/* current screen */
Window	    w,			/* window used to cover screen */
	    icon,		/* window used during password typein */
	    root;		/* convenience pointer to the root window */
GC	    gc,			/* main graphics drawing graphics context */
	    textgc;		/* graphics context used for text rendering */
XColor	    black,		/* used for text rendering */
	    white;		/* background of text screen */
Colormap    cmap;		/* colormap */
Cursor	    mycursor;		/* blank cursor */
Pixmap	    lockc,
	    lockm;		/* pixmaps for cursor and mask */
char	    no_bits[] = {0};	/* dummy array for the blank cursor */
int	    passx,		/* position of the ?'s */
	    passy;
XFontStruct *font;
char	   *fontname = NULL;	/* the font used in the password screen */
int	    inittime = -1;	/* time to iterate before calling init */
int	    skipRoot;		/* skip root password check */
int	    color;		/* color or mono */
int	    count = -1;		/* number of pixels to draw in each color */
int	    nicelevel = -1;	/* system priority at which to run process */
int	    lock;		/* locked or just screensaver mode */
int	    verbose = False;	/* print configuration info to stderr? */
char	   *display = NULL;	/* X display variable */
int	    timeout,
	    interval,
	    blanking,
	    exposures;		/* screen saver parameters */

#define ICONX			300
#define ICONY			150
#define ICONW			64
#define ICONH			64
#define ICONLOOPS		600
#define DEFAULT_FONTNAME	"LucidaSans-24"
#define BACKUP_FONTNAME		"fixed"
#define DEFAULT_INITTIME	60
#define DEFAULT_SKIPROOT	True
#define DEFAULT_COUNT		100
#define DEFAULT_NICE		10
#define DEFAULT_DISPLAY		":0"

/* VARARGS1 */
void
error(s1, s2)
    char       *s1,
	       *s2;
{
    fprintf(stderr, s1, pname, s2);
    exit(1);
}


int	    oldsigmask;

void
block()
{
    oldsigmask = sigblock(sigmask(SIGINT)
			  | sigmask(SIGQUIT)
			  | sigmask(SIGFPE)
			  | sigmask(SIGSEGV));
}

void
unblock()
{
    sigsetmask(oldsigmask);
}

void
allowsig()
{
    unblock();
    block();
}


XHostAddress *XHosts;
int	    HostAccessCount;
Bool	    HostAccessState;

void
XGrabHosts(dsp)
    Display    *dsp;
{
    XHosts = XListHosts(dsp, &HostAccessCount, &HostAccessState);
    XRemoveHosts(dsp, XHosts, HostAccessCount);
    XEnableAccessControl(dsp);
}

void
XUngrabHosts(dsp)
    Display    *dsp;
{
    XAddHosts(dsp, XHosts, HostAccessCount);
    XFree(XHosts);
    if (HostAccessState == False)
	XDisableAccessControl(dsp);
}


void
GrabKeyboardAndMouse()
{
    Status	status;

    status = XGrabKeyboard(dsp, w, True,
			   GrabModeAsync, GrabModeAsync, CurrentTime);
    if (status != GrabSuccess)
	error("%s: couldn't grab keyboard! (%d)\n", status);
    status = XGrabPointer(dsp, w, True, -1,
			  GrabModeAsync, GrabModeAsync, None, mycursor,
			  CurrentTime);
    if (status != GrabSuccess)
	error("%s: couldn't grab pointer! (%d)\n", status);
}


void
XChangeGrabbedCursor(cursor)
    Cursor	cursor;
{
    XGrabPointer(dsp, w, True, -1,
		 GrabModeAsync, GrabModeAsync, None, cursor, CurrentTime);
}

void
finish()
{
    XSync(dsp, 0);
    XUngrabHosts(dsp);
    XUngrabPointer(dsp, CurrentTime);
    XUngrabKeyboard(dsp, CurrentTime);
    XSetScreenSaver(dsp, timeout, interval, blanking, exposures);
    XDestroyWindow(dsp, w);
    if (color)
	XUninstallColormap(dsp, cmap);
    XFlush(dsp);
    XCloseDisplay(dsp);
}

void
sigcatch()
{
    finish();
    error("%s: caught terminate signal.\nAccess control list restored.\n");
}

int
ReadXString(s, slen)
    char       *s;
    int		slen;
{
    XEvent	event;
    char	keystr[20];
    char	c;
    int		i,
		bp,
		len,
		loops;
    char       *pwbuf = (char *) malloc(slen);

    XSetForeground(dsp, gc, BlackPixel(dsp, screen));
    init(dsp, icon, gc, color, inittime, count);
    bp = 0;
    nice(-nicelevel);		/* make sure we can read the keystrokes... */
    while (True) {
	loops = 0;
	while (!XPending(dsp)) {
	    allowsig();
	    callback();
	    if (reinit())
		init(dsp, icon, gc, color, inittime, count);
	    if (++loops >= ICONLOOPS) {
		nice(nicelevel);
		free(pwbuf);
		return 1;
	    }
	}
	XNextEvent(dsp, &event);
	switch (event.type) {
	case KeyPress:
	    len = XLookupString((XKeyEvent *) & event, keystr, 20, NULL, NULL);
	    for (i = 0; i < len; i++) {
		c = keystr[i];
		switch (c) {
		case 8:	/* ^H */
		case 127:	/* DEL */
		    if (bp > 0)
			bp--;
		    break;
		case 10:	/* ^J */
		case 13:	/* ^M */
		    s[bp] = '\0';
		    nice(nicelevel);
		    free(pwbuf);
		    return 0;
		case 21:	/* ^U */
		    bp = 0;
		    break;
		default:
		    s[bp] = c;
		    if (bp < slen - 1)
			bp++;
		}
	    }
	    memset(pwbuf, '?', slen);
	    XSetForeground(dsp, gc, white.pixel);
	    XFillRectangle(dsp, w, gc, passx, passy - font->ascent,
			   XTextWidth(font, pwbuf, slen),
			   font->ascent + font->descent);
	    XDrawString(dsp, w, textgc, passx, passy, pwbuf, bp);
	    break;

	case ButtonPress:
	    if (((XButtonEvent *) & event)->window == icon) {
		nice(nicelevel);
		free(pwbuf);
		return 1;
	    }
	    break;

	case KeyRelease:
	case ButtonRelease:
	case MotionNotify:
	case LeaveNotify:
	case EnterNotify:
	    break;

	default:
	    fprintf(stderr, "%s: unexpected event: %d\n", pname, event.type);
	    break;
	}
    }
}

int
getPassword()
{
#define PASSLENGTH 20
    char	buffer[PASSLENGTH];
    char	userpass[PASSLENGTH];
    char	rootpass[PASSLENGTH];
    struct passwd *pw;
    XWindowAttributes xgwa;
    char       *user = getenv("USER");
    char       *name = "Name: ";
    char       *pass = "Password: ";
    char       *info = "Enter password to unlock; select icon to lock.";
    char       *validate = "Validating login...";
    char       *invalid = "Invalid login.";
    int		y,
		left,
		done;

    XGetWindowAttributes(dsp, w, &xgwa);

    XChangeGrabbedCursor(XCreateFontCursor(dsp, XC_left_ptr));

    XSetForeground(dsp, gc, WhitePixel(dsp, screen));
    XFillRectangle(dsp, w, gc, 0, 0, xgwa.width, xgwa.height);

    XMapWindow(dsp, icon);
    XRaiseWindow(dsp, icon);

    left = ICONX + ICONW + font->max_bounds.width;
    y = ICONY + font->ascent;

    XDrawString(dsp, w, textgc, left, y, name, strlen(name));
    XDrawString(dsp, w, textgc, left + 1, y, name, strlen(name));
    XDrawString(dsp, w, textgc,
		left + XTextWidth(font, name, strlen(name)), y,
		user, strlen(user));

    y += font->ascent + font->descent + 2;
    XDrawString(dsp, w, textgc, left, y, pass, strlen(pass));
    XDrawString(dsp, w, textgc, left + 1, y, pass, strlen(pass));

    passx = left + 1 + XTextWidth(font, pass, strlen(pass))
	+ XTextWidth(font, " ", 1);
    passy = y;

    y = ICONY + ICONH + font->ascent + 2;
    XDrawString(dsp, w, textgc, ICONX, y, info, strlen(info));

    XFlush(dsp);

    y += font->ascent + font->descent + 2;

    pw = getpwuid(0);
    strcpy(rootpass, pw->pw_passwd);

    pw = getpwuid(getuid());
    strcpy(userpass, pw->pw_passwd);

    done = False;
    while (!done) {

	if (ReadXString(buffer, PASSLENGTH)) {
	    XChangeGrabbedCursor(mycursor);
	    XUnmapWindow(dsp, icon);
	    XSetForeground(dsp, gc, WhitePixel(dsp, screen));
	    return 1;
	}
	XSetForeground(dsp, gc, WhitePixel(dsp, screen));
	XFillRectangle(dsp, w, gc, ICONX, y - font->ascent,
		       XTextWidth(font, validate, strlen(validate)),
		       font->ascent + font->descent + 2);

	XDrawString(dsp, w, textgc, ICONX, y, validate, strlen(validate));

	done = !((strcmp(crypt(buffer, userpass), userpass))
		 && (skipRoot || strcmp(crypt(buffer, rootpass), rootpass)));

	if (!done) {
	    XFlush(dsp);
	    sleep(1);
	    XFillRectangle(dsp, w, gc, ICONX, y - font->ascent,
			   XTextWidth(font, validate, strlen(validate)),
			   font->ascent + font->descent + 2);

	    XDrawString(dsp, w, textgc, ICONX, y, invalid, strlen(invalid));
	}
    }
    return 0;
}

void
justDisplay()
{
    XEvent	event;

    init(dsp, w, gc, color, inittime, count);
    do {
	while (!XPending(dsp)) {
	    callback();
	    if (reinit())
		init(dsp, w, gc, color, inittime, count);
	}
	XNextEvent(dsp, &event);
    } while (event.type != ButtonPress && event.type != KeyPress);
}

void
lockDisplay()
{
    do {
	justDisplay();
    } while (getPassword());
}

void
usage(s)
    char       *s;
{
    int		i;

    fprintf(stderr, "%s\nusage: %s [-display dsp] [-mode %s",
	    s, pname, LockProcs[0].cmdline_arg);
    for (i = 1; i < NUMPROCS; i++)
	fprintf(stderr, " | %s", LockProcs[i].cmdline_arg);
    fprintf(stderr, "]\n");
    fprintf(stderr, "\t%s %s\n",
	    "[-time n] [-count n] [-nice n]",
	    "[-font f] [-mono] [-saver] [-root] [-v]");
    exit(1);
}

void
BuildPointersFromString(s)
    char       *s;
{
    int		i;

    for (i = 0; i < NUMPROCS; i++) {
	if (!strncmp(LockProcs[i].cmdline_arg, s, strlen(s))) {
	    reinit = LockProcs[i].lp_reinit;
	    callback = LockProcs[i].lp_callback;
	    init = LockProcs[i].lp_init;
	    if (verbose)
		fprintf(stderr, "Mode: %s\n", s);
	    return;
	}
    }
    usage("Unknown Mode.");
}

main(argc, argv)
    int		argc;
    char       *argv[];
{
    XSetWindowAttributes xswa;
    XGCValues	xgcv;
    int		i;
    char       *s;
    int		n;

    pname = argv[0];
    color = True;
    lock = True;
    skipRoot = -1;

    for (i = 1; i < argc; i++) {
	s = argv[i];
	n = strlen(s);
	if (!strncmp("-display", s, n)) {
	    if (++i >= argc)
		usage(s);
	    display = argv[i];
	} else if (!strncmp("-mono", s, n)) {
	    color = False;
	} else if (!strncmp("-saver", s, n)) {
	    lock = False;
	} else if (!strncmp("-root", s, n)) {
	    skipRoot = False;
	} else if (!strncmp("-verbose", s, n)) {
	    verbose = True;
	} else if (!strncmp("-time", s, n)) {
	    if (++i >= argc)
		usage(s);
	    inittime = atoi(argv[i]);
	    if (inittime < 1)
		usage("-time argument must be positive.");
	} else if (!strncmp("-count", s, n)) {
	    if (++i >= argc)
		usage(s);
	    count = atoi(argv[i]);
	    if (count < 1)
		usage("-count argument must be positive.\n");
	} else if (!strncmp("-nice", s, n)) {
	    if (++i >= argc)
		usage(s);
	    nicelevel = atoi(argv[i]);
	} else if (!strncmp("-font", s, n)) {
	    if (++i >= argc)
		usage(s);
	    fontname = argv[i];
	} else if (!strncmp("-mode", s, n)) {
	    if (++i >= argc)
		usage(s);
	    BuildPointersFromString(argv[i]);
	} else {
	    fprintf(stderr, "Bad switch: ");
	    usage(s);
	}
    }

    if (display == NULL)
	display = getenv("DISPLAY");

    if (display != NULL) {
	char	   *colon = strchr(display, ':');
	int	    n = colon - display;
	char	    hostname[MAXHOSTNAMELEN];

	if (gethostname(hostname, MAXHOSTNAMELEN))
	    error("%s: Can't get local hostname.\n");

	if (colon == NULL)
	    error("%s: Malformed -display argument, \"%s\"\n", display);

	if (n) {
	    if (strncmp(display, "unix", n)
	     && strncmp(display, "localhost", n)
	     && strncmp(display, hostname, n)) {
		*colon = (char) 0;
		error("%s: can't lock %s's display\n", display);
	     }
	}
    } else
	display = DEFAULT_DISPLAY;

    if (lock) {
	block();
	signal(SIGINT, sigcatch);
	signal(SIGQUIT, sigcatch);
	signal(SIGSEGV, sigcatch);
    }
    if (!(dsp = XOpenDisplay(display)))
	error("%s: unable to open display %s.\n", display);

    if (fontname == NULL)
	fontname = XGetDefault(dsp, pname, "font");
    if (fontname == NULL)
	fontname = DEFAULT_FONTNAME;

    if (count == -1) {
	s = XGetDefault(dsp, pname, "count");
	if (s != NULL)
	    count = atoi(s);
	else
	    count = DEFAULT_COUNT;
    }
    if (nicelevel == -1) {
	s = XGetDefault(dsp, pname, "nice");
	if (s != NULL)
	    nicelevel = atoi(s);
	else
	    nicelevel = DEFAULT_NICE;
    }
    if (inittime == -1) {
	s = XGetDefault(dsp, pname, "time");
	if (s != NULL)
	    inittime = atoi(s);
	else
	    inittime = DEFAULT_INITTIME;
    }
    if (init == NULL) {
	s = XGetDefault(dsp, pname, "mode");
	if (s != NULL) {
	    BuildPointersFromString(s);
	} else {
	    reinit = LockProcs[0].lp_reinit;
	    callback = LockProcs[0].lp_callback;
	    init = LockProcs[0].lp_init;
	    if (verbose)
		fprintf(stderr, "Mode: %s\n", LockProcs[0].cmdline_arg);
	}
    }
    if (skipRoot == -1) {
	s = XGetDefault(dsp, pname, "root");
	if (s != NULL)
	    skipRoot = !(strncmp("on", s, strlen(s)) == 0);
	else
	    skipRoot = DEFAULT_SKIPROOT;
    }
    if (verbose) {
	fprintf(stderr,
		"%s: Font: %s, Time: %d, Root: %s, Nice: %d\n", pname,
		fontname, inittime, skipRoot ? "False" : "True", nicelevel);
    }
    font = XLoadQueryFont(dsp, fontname);
    if (font == NULL) {
	fprintf(stderr, "%s: can't find font: %s, using %s...\n",
		pname, fontname, BACKUP_FONTNAME);
	font = XLoadQueryFont(dsp, BACKUP_FONTNAME);
	if (font == NULL)
	    error("%s: can't even find %s!!!\n", BACKUP_FONTNAME);
    }
    screen = DefaultScreen(dsp);
    if (color)
	color = (DisplayCells(dsp, screen) > 2);
    root = RootWindow(dsp, screen);

    if (color) {
	if (XCreateHSBColormap(dsp, screen, &cmap, DisplayCells(dsp, screen),
			       0.0, 1.0, 1.0, 1.0, 1.0, 1.0, True) != Success)
	    error("%s: couldn't create colormap.");
    } else
	cmap = DefaultColormap(dsp, screen);

    black.pixel = BlackPixel(dsp, screen);
    XQueryColor(dsp, cmap, &black);

    white.pixel = WhitePixel(dsp, screen);
    XQueryColor(dsp, cmap, &white);

    lockc = XCreateBitmapFromData(dsp, root, no_bits, 1, 1);
    lockm = XCreateBitmapFromData(dsp, root, no_bits, 1, 1);
    mycursor = XCreatePixmapCursor(dsp, lockc, lockm, &black, &black, 0, 0);
    XFreePixmap(dsp, lockc);
    XFreePixmap(dsp, lockm);

    xswa.cursor = mycursor;
    xswa.override_redirect = True;
    xswa.background_pixel = black.pixel;
    xswa.event_mask = KeyPressMask | ButtonPressMask;

    w = XCreateWindow(dsp, root,
		      0, 0,
		      DisplayWidth(dsp, screen),
		      DisplayHeight(dsp, screen),
		      0, CopyFromParent, InputOutput, CopyFromParent,
		      CWCursor | CWOverrideRedirect |
		      CWBackPixel | CWEventMask, &xswa);

    xswa.cursor = XCreateFontCursor(dsp, XC_target);
    xswa.background_pixel = white.pixel;
    xswa.event_mask = ButtonPressMask;

    icon = XCreateWindow(dsp, w,
			 ICONX, ICONY,
			 ICONW, ICONH,
			 1, CopyFromParent, InputOutput, CopyFromParent,
			 CWCursor | CWBackPixel | CWEventMask, &xswa);

    XMapWindow(dsp, w);
    XRaiseWindow(dsp, w);

    xgcv.foreground = white.pixel;
    xgcv.background = black.pixel;
    gc = XCreateGC(dsp, w, GCForeground | GCBackground, &xgcv);

    xgcv.foreground = black.pixel;
    xgcv.background = white.pixel;
    xgcv.font = font->fid;
    textgc = XCreateGC(dsp, w, GCFont | GCForeground | GCBackground, &xgcv);

    XGetScreenSaver(dsp, &timeout, &interval, &blanking, &exposures);
    XSetScreenSaver(dsp, 0, 0, 0, 0);	/* disable screen saver */
    GrabKeyboardAndMouse();

    if (lock) {
	XGrabHosts(dsp);
	allowsig();
    }
    nice(nicelevel);
    srandom(getpid());

    if (color)
	XInstallColormap(dsp, cmap);

    if (lock)
	lockDisplay();
    else
	justDisplay();

    finish();

    if (lock)
	unblock();

    exit(0);
}
