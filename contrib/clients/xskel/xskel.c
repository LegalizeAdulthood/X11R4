/*
 * A simple window, with trivial mouse interaction - Mark Moraes.
 */
#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xos.h>
#include <X11/cursorfont.h>
#include <X11/Xutil.h>

#define APP_NAME		"xskel"
#define APP_CLASS		"XSkel"

#ifndef OUTPUT_ONLY
# define RightButton		Button3
# define MiddleButton		Button2
# define LeftButton		Button1
# define RightButtonMask	Button3Mask
# define MiddleButtonMask	Button2Mask
# define LeftButtonMask		Button1Mask
#endif	/* OUTPUT_ONLY */

#define CELLWIDTH 4
#define CELLHEIGHT 4

#define XCELLS	150
#define YCELLS	150

#define xcell(i)		((i)*CELLWIDTH)
#define ycell(i)		((i)*CELLHEIGHT)

/*
 * If MAXARGS isn't enough to hold all the arguments you pass any Xt
 * procedure, the program aborts
 */
#define MAXARGS 32
static int nargs;
static Arg wargs[MAXARGS];
#define startargs()		nargs = 0
#define setarg(name, value)	\
	if (nargs < MAXARGS) \
		XtSetArg(wargs[nargs], name, value), nargs++; \
	else \
		abort()

static Cursor WorkingCursor;
static Display *dpy;
static Window win;
static GC gc;
static GC cleargc;

static int running = 0;

/* Defaults */
static int defaultWidth = CELLWIDTH * XCELLS;
static int defaultHeight = CELLWIDTH * YCELLS;

static int Width, Height;
static Pixel fg, bg;
static char *progname;

/* Application Resources - no particular widget */
static XtResource application_resources[] = {
	{"name", "Name", XtRString, sizeof(char *),
		(Cardinal)&progname, XtRString, APP_NAME},
	{"width", "Width", XtRInt, sizeof(int),
		(Cardinal)&Width, XtRInt, (caddr_t) &defaultWidth},
	{"height", "Height", XtRInt, sizeof(int),
		(Cardinal)&Height, XtRInt, (caddr_t) &defaultHeight},
	{"foreground", "Foreground", XtRPixel, sizeof(Pixel),
		(Cardinal)&fg, XtRString, (caddr_t) "Black"},
	{"background", "Background", XtRPixel, sizeof(Pixel),
		(Cardinal)&bg, XtRString, (caddr_t) "White"},
};

/*
 *  Command line options table. The command line is parsed for these,
 *  and it sets/overrides the appropriate values in the resource
 *  database
 */
static XrmOptionDescRec optionDescList[] = {
{"-width",	"*width",	XrmoptionSepArg, 	(caddr_t) NULL},
{"-height",	"*height",	XrmoptionSepArg,	(caddr_t) NULL},
{"-fg",		"*foreground",	XrmoptionSepArg,	(caddr_t) NULL},
{"-bg",		"*background",	XrmoptionSepArg,	(caddr_t) NULL},
};



main(argc, argv)
int argc;
char **argv;
{
	Widget toplevel;
	Widget w;
	XGCValues gcv;
#ifndef OUTPUT_ONLY
	void RepaintCanvas();
	void RecordMapStatus();
	void MouseInput();
#else
	XEvent ev;
#endif

	/*
 	 * Create the top level Widget that represents encloses the
	 * application.
	 */
	toplevel = XtInitialize(argv[0], APP_CLASS,
		optionDescList, XtNumber(optionDescList), (Cardinal *) &argc,
		argv);

	XtGetApplicationResources(toplevel, (caddr_t) 0, application_resources,
		XtNumber(application_resources), (ArgList) NULL, (Cardinal)0);


	if (argc != 1) {
		(void) fprintf(stderr, "Usage: %s [Xt options]\n", argv[0]);
		exit(-1);
	}

	/*
	 * Create a simple Core class widget which we'll use for the actual
	 * game.  A Core class widget is basically just a window, with a
	 * simple Xt "wrapper" around it.
	 */
	startargs();
	setarg(XtNwidth, Width);
	setarg(XtNheight, Height);
	w = XtCreateManagedWidget(argv[0], widgetClass, toplevel, 
		wargs, XtNumber(wargs));

#ifndef OUTPUT_ONLY
	/*
	 * Set the procedures for various X Windows actions - exposure events
	 * which arrive when a window needs to be redrawn. The map event lets
	 * you know that the window is now on the screen so you can actually
	 * do stuff. The ButtonPress event lets you know that a mouse button
	 * was pressed.
	 */
	XtAddEventHandler(w, (EventMask) ExposureMask, False, 
	 RepaintCanvas, "redraw_data");
	XtAddEventHandler(w, (EventMask) StructureNotifyMask, False, 
	 RecordMapStatus, "map_data");
	/* One day, we'll use the translation manager here */
	XtAddEventHandler(w, 
	 (EventMask) ButtonPressMask | ButtonMotionMask, False,
	 MouseInput, "input_data");
#endif

	/*
	 * Create the windows, and set their attributes according to the Widget
	 * data.
	 */
	XtRealizeWidget(toplevel);
	
	/* We need these for the raw Xlib calls */
	win = XtWindow(w);
	dpy = XtDisplay(w);

	WorkingCursor = XCreateFontCursor(dpy, XC_top_left_arrow);
	XDefineCursor(dpy, win, WorkingCursor);

	/*
	 *  make the GC stuff here - one for copy, one for invert. Remember
	 *  to change the both appropriately
	 */
	gcv.foreground = fg;
	gcv.background = bg;
	gcv.function = GXcopy;
	gc = XCreateGC(dpy, win, GCForeground | GCBackground 
	 | GCFunction, &gcv);
	gcv.foreground = bg;
	cleargc = XCreateGC(dpy, win, GCForeground | GCBackground 
	 | GCFunction, &gcv);

	/*
	 *  Now process the events.
	 */
#ifndef OUTPUT_ONLY
	XtMainLoop();
#else
	/* Wait for first exposure event so we know window has been mapped */
	XWindowEvent(dpy, win, (long) ExposureMask, &ev);
	/* pause is just for the demo. Usually we'd return or draw something */
	pause();
#endif
}

#ifndef OUTPUT_ONLY

static int isMapped = 0;

void
setcell(x, y, gc)
int x, y;
GC gc;
{
	int xreal = x * CELLWIDTH;
	int yreal = y * CELLHEIGHT;
	
	XFillRectangle(dpy, win, gc, xreal, yreal,
	 CELLWIDTH, CELLHEIGHT);
}

/*ARGSUSED*/
void RepaintCanvas(w, data, ev)
Widget w;
caddr_t data;
XEvent *ev;
{
	if (!isMapped)
		return;
	/*
	 * Redraw the array
	 */
	if (ev && ev->xexpose.count == 0) {
		XEvent event;
		/* Skip all excess redraws */
		while (XCheckTypedEvent(dpy, Expose, &event))
			;
	}

	XClearWindow(dpy, ev->xany.window);
		
#ifdef WINDOWDEBUG
	(void) printf("repaint\n");
#endif
	XFlush(dpy);
}

/*ARGSUSED*/
void RecordMapStatus(w, data, ev)
Widget w;
caddr_t data;
XEvent *ev;
{
	if (ev->type == MapNotify) {
#ifdef WINDOWDEBUG
		(void) printf("window mapped\n");
#endif
		isMapped = TRUE;
	} else if (ev->type = ConfigureNotify) {
#ifdef WINDOWDEBUG
		(void) printf("window resized\n");
#endif
	}
}


/*ARGSUSED*/
void MouseInput(w, data, ev)
Widget w;
caddr_t data;
XEvent *ev;
{
#ifdef WINDOWDEBUG
	(void) printf("Input to canvas - %d (0x%x)\n", ev->xany.type, ev->xany.type);
#endif
	switch (ev->xany.type) {
	case ButtonPress:
		if (ev->xbutton.button == LeftButton) {
			/* Turn on cell */
			setcell((int) (ev->xbutton.x / CELLWIDTH),
				(int) (ev->xbutton.y / CELLHEIGHT), gc);
		} else if (ev->xbutton.button == MiddleButton) {
			/* Toggle simulation on/off */
			running = !running;
		}
		break;
	case MotionNotify:
		if (ev->xmotion.state & LeftButtonMask) 
			/* Turn on cell */
			setcell((int) (ev->xmotion.x / CELLWIDTH),
				(int) (ev->xmotion.y / CELLHEIGHT), gc);
		break;
	default:
		(void) printf("Got an event of type %d (%x).\n",
		       ev->xany.type, ev->xany.type);
		break;
	}
}
#endif
