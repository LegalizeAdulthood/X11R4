/*
 * John Conway's Game of Life. With some simple interaction that makes it a
 * neat game - Mark Moraes.
 */
/*
 *  Possible enhancements: Load, Save, Go through patterns, etc Do it
 *  the right way with Translation management
 */
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Command.h>
#include <X11/cursorfont.h>
#include <X11/Xutil.h>
#include <fcntl.h>
#include <signal.h>
#include "random.h"
#include "object.h"

#ifndef PATFILE
#define PATFILE "xlife.pat"
#endif PATFILE

#define RightButton		Button3
#define MiddleButton		Button2
#define LeftButton		Button1
#define RightButtonMask		Button3Mask
#define MiddleButtonMask	Button2Mask
#define LeftButtonMask		Button1Mask

#define CELLWIDTH 4
#define CELLHEIGHT 4

#define XCELLS	150
#define YCELLS	150

#define ONCODE 10

#define xcell(i)		((i)*CELLWIDTH)
#define ycell(i)		((i)*CELLHEIGHT)

/* Storage for the old board and new board */
static int **boardrows1;
/*
 *  Pointers to the curent board for the iteration, and the new board
 *  for the next iteration - these pointers point to boardrows[12]
 *  alternately.
 */
static int **oboard;
static int *live_cells[2], *neighbor_cells[2];
/* Number of columns and rows of cells */
static int nxcells, nycells;
static int nrandom;

/* Boolean variables */
static int running = 0;	/* Are we running the simulation or not ? */
static int notover = 1;	/* zero if user wants to quit */

/* 
 *  These structures comprise the display list - instead of drawing as
 *  we change the cell, we store it in the display list, and update in
 *  batches - see the comments on the interrupt strategy at
 *  xw_setup_intr(). 
 */


typedef struct {
	int set;
	int x, y;
} RectList;

#define RECT_LIST_CHUNK 1024
RectList *rect_list = NULL;
int rect_list_size = 0;
int rect_list_count = 0;;

static void do_life();
static void makeboard();
static void fill_rectangle();
static void setcell();
static void xw_setup_intr();
static int xw_handle_intr();
static int enable_sigio();
static int disable_sigio();
static int set_sigio_state();
static void reentrant_xflush();
static void redraw();
static void xw_update();
static void readpatterns();
static void randomize();
static void clear();


/*
 *  This is my favourite way of handling X Toolkit arguments -
 *  startargs() and then, setarg(name, value) for all the args I want to
 *  set
 */
#define MAXARGS 32
static int nargs;
static Arg wargs[MAXARGS];
#define startargs()		nargs = 0
#define setarg(name, value)	XtSetArg(wargs[nargs], name, value), nargs++

/* X Windows related variables */
static Cursor WorkingCursor;
static Display *dpy;
static Window win;
static GC gc;
static GC cleargc;

/* X Defaults */
static int defaultWidth = CELLWIDTH * XCELLS;
static int defaultHeight = CELLWIDTH * YCELLS;

static int zero = 0;
static int total_living = 0;
static int Width, Height;
static Pixel fg, bg;
static char *progname;
static char *patfile;

/* Application Resources - no particular widget */
static XtResource application_resources[] = {
	{"name", "Name", XtRString, sizeof(char *),
		(Cardinal)&progname, XtRString, "xlifedemo"},
	{"width", "Width", XtRInt, sizeof(int),
		(Cardinal)&Width, XtRInt, (caddr_t) &defaultWidth},
	{"height", "Height", XtRInt, sizeof(int),
		(Cardinal)&Height, XtRInt, (caddr_t) &defaultHeight},
	{"random", "Random", XtRInt, sizeof(int),
		(Cardinal)&nrandom, XtRInt, (caddr_t) &zero},
	{"foreground", "Foreground", XtRPixel, sizeof(Pixel),
		(Cardinal)&fg, XtRString, (caddr_t) "Black"},
	{"background", "Background", XtRPixel, sizeof(Pixel),
		(Cardinal)&bg, XtRString, (caddr_t) "White"},
	{"patternfile", "PatternFile", XtRString, sizeof(char *),
		(Cardinal)&patfile, XtRString, PATFILE},
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
{"-random",	"*random",	XrmoptionSepArg,	(caddr_t) NULL},
{"-patfile",	"*patternfile",	XrmoptionSepArg,	(caddr_t) NULL},
};


main(argc, argv)
int argc;
char **argv;
{
	static void RepaintCanvas();
	static void RecordMapStatus();
	static void MouseInput();
	Widget toplevel;
	Widget w;
	XGCValues gcv;

	/*
 	 * Create the top level Widget that represents encloses the
	 * application.
	 */
	toplevel = XtInitialize(argv[0], "XLife",
		optionDescList, XtNumber(optionDescList), &argc, argv);

	XtGetApplicationResources(toplevel, 0, application_resources,
		XtNumber(application_resources), NULL, 0 );


	if (argc != 1) {
		(void) fprintf(stderr, "Usage: %s [Xt options]\n", argv[0]);
		exit(-1);
	}

	makeboard(Width/CELLWIDTH, Height/CELLHEIGHT);
	readpatterns(patfile);
	
	/*
	 * Create a simple Core class widget which we'll use for the actual
	 * game.  A Core class widget is basically just a window, with a
	 * simple Xt "wrapper" around it.
	 */
	startargs();
	setarg(XtNwidth, xcell(nxcells));
	setarg(XtNheight, xcell(nycells));
	w = XtCreateManagedWidget(argv[0], widgetClass, toplevel, 
		wargs, nargs);

	/*
	 * Set the procedures for various X Windows actions - exposure events
	 * which arrive when a window needs to be redrawn. The map event lets
	 * you know that the window is now on the screen so you can actually
	 * do stuff. The ButtonPress event lets you know that a mouse button
	 * was pressed.
	 */
	XtAddEventHandler(w, (Cardinal) ExposureMask, NULL, 
	 RepaintCanvas, "redraw_data");
	XtAddEventHandler(w, (Cardinal) StructureNotifyMask, NULL, 
	 RecordMapStatus, "map_data");
	/* One day, we'll use the translation manager here */
	XtAddEventHandler(w, 
	 (Cardinal) ButtonPressMask | ButtonMotionMask | KeyPressMask, NULL,
	 MouseInput, "input_data"); 

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
	 *  make the X Graphic Contexts here - one for copy (setting the
	 *  square to foreground colour), one for erase (setting the
	 *  square to background colour).
	 */
	gcv.foreground = fg;
	gcv.background = bg;
	gcv.function = GXcopy;
	gc = XCreateGC(dpy, win, GCForeground | GCBackground 
	 | GCFunction, &gcv);
	gcv.foreground = bg;
	cleargc = XCreateGC(dpy, win, GCForeground | GCBackground 
	 | GCFunction, &gcv);

	
	(void) seedrnd(getpid());
	randomize(nrandom);
	XFlush(dpy);

	/* 
	 *  Now process the events. Our event processing is
	 *  asynchronous, like GNUPLOT, from which the event code is
	 *  adapted. This is unlike most X applications which are
	 *  interactive, and therefore go into a XtMainLoop here. We go
	 *  into a "display loop", where we start doing output, and get
	 *  interrupted when there's an event to be processed.  as we
	 *  can.
	 */
	
	xw_setup_intr();
	while(notover) {
		if (running)
			do_life();
		else
			pause();
	}
}

/* Actual game of life iteration */
static void
do_life()
{
        register int i, x, y, dx, dy, *cell;
	register int number, total_neighbors;
#ifdef DEBUG
	int done = 0;
	int filled = 0;
#endif

	rect_list_count = 0;
	total_neighbors = 0;
	for(i = 0; i < total_living; i++)
		for(dx = -1; dx <= 1; dx++)
			for(dy = -1; dy <= 1; dy++)
				if (dx != 0 || dy !=0) {
				    x = live_cells[0][i] + dx; 
				    y = live_cells[1][i] + dy;
				    oboard[x][y] ++;
				    if (oboard[x][y] == 1) {
					neighbor_cells[0][total_neighbors] = x;
					neighbor_cells[1][total_neighbors] = y;
					total_neighbors++;
				    }
				}
	for(i = 0; i< total_living; i++) {
		cell = &(oboard[live_cells[0][i]][live_cells[1][i]]);
		number = *cell; 
		*cell = ONCODE;
		if (number != 2+ONCODE && number != 3+ONCODE) {
			total_living --; *cell = 0;
			fill_rectangle(live_cells[0][i], live_cells[1][i], 0);
			live_cells[0][i] = live_cells[0][total_living];
			live_cells[1][i] = live_cells[1][total_living];
			i--;
		}
	  }

	for(i = 0; i< total_neighbors; i++) {
		cell = &(oboard[neighbor_cells[0][i]][neighbor_cells[1][i]]);
		number = *cell;
		*cell = 0;
		if (number == 3 && neighbor_cells[0][i] >=1 && 
		 neighbor_cells[0][i] <= nxcells &&
		 neighbor_cells[1][i] >= 1 &&
		 neighbor_cells[1][i] <= nycells) {
			*cell = ONCODE;
			live_cells[0][total_living] = neighbor_cells[0][i];
			live_cells[1][total_living] = neighbor_cells[1][i];
			total_living++;
			fill_rectangle(neighbor_cells[0][i],
			 neighbor_cells[1][i], 1);
		}
	}

	/* Stop running if we haven't had a change in the cell */
	if (rect_list_count == 0)
		running = 0;
	xw_update();
/*	reentrant_redraw();*/
#ifdef DEBUG
	printf("Done an iteration - changed = %d, filled = %d, running = %d\n",
	 done, filled, running);
#endif
}


/*
 *  Add a rectangle to the display_list. If the last rectangle was the
 *  same colour as the one now (determined by 'set'), then add it to the
 *  current rect_list element by incrementing the count, and putting it
 *  in rect_buf. If it is different, then start a new rect_list
 *  element.
 */
static void
fill_rectangle(x, y, set)
{
	register RectList *cur_rect;

	test_and_grow_object(rect_list, rect_list_count,
	 rect_list_size, RECT_LIST_CHUNK, RectList);
	
	cur_rect = &rect_list[rect_list_count];
	cur_rect->x = xcell(x-1);
	cur_rect->y = ycell(y-1);
	cur_rect->set = set;
	rect_list_count++;
}

static int isMapped = 0;

/*ARGSUSED*/
static void 
RepaintCanvas(w, data, ev)
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
		redraw();
	}

		
#ifdef WINDOWDEBUG
	printf("repaint\n");
#endif
	XFlush(dpy);
}

/*ARGSUSED*/
static void 
RecordMapStatus(w, data, ev)
Widget w;
caddr_t data;
XEvent *ev;
{
	if (ev->type == MapNotify) {
#ifdef WINDOWDEBUG
		printf("window mapped\n");
#endif
		isMapped = TRUE;
	} else if (ev->type = ConfigureNotify) {
#ifdef WINDOWDEBUG
		printf("window resized\n");
#endif
	}
}


/*ARGSUSED*/
static void 
MouseInput(w, data, ev)
Widget w;
caddr_t data;
XEvent *ev;
{
	char ch;
	
#ifdef WINDOWDEBUG
	printf("Input to canvas - %d (0x%x)\n", ev->xany.type, ev->xany.type);
#endif
	switch (ev->xany.type) {
	case ButtonPress:
		if (ev->xbutton.button == LeftButton) {
			/* Turn on cell */
			setcell((int) (ev->xbutton.x / CELLWIDTH),
				(int) (ev->xbutton.y / CELLHEIGHT), 1);
		} else if (ev->xbutton.button == MiddleButton) {
			/* Toggle simulation on/off */
			running = !running;
		} else if (ev->xbutton.button == RightButton && !running) {
			/* Turn off cell */
			setcell((int) (ev->xbutton.x / CELLWIDTH),
				(int) (ev->xbutton.y / CELLHEIGHT), 0);
		}
		break;
	case MotionNotify:
		if (ev->xmotion.state & LeftButtonMask) 
			/* Turn on cell */
			setcell((int) (ev->xmotion.x / CELLWIDTH),
				(int) (ev->xmotion.y / CELLHEIGHT), 1);
		else if ((ev->xmotion.state & RightButtonMask) && !running)
			/* Turn off cell */
			setcell((int) (ev->xmotion.x / CELLWIDTH),
				(int) (ev->xmotion.y / CELLHEIGHT), 0);
		break;
	case KeyPress:
		XLookupString(ev, &ch, 1, NULL, NULL);
#ifdef WINDOWDEBUG
		printf("Keypress - %c (0x%x)\n", ch, ch);
#endif
		switch(ch) {
		case '0': case '1': case '2': case '3': case '4': 
		case '5': case '6': case '7': case '8': case '9':
			break;
		case 'c':
			clear();
			break;
		case 'r':
			redraw();
			break;
		case 'q':
			exit(0);
			/*NOTREACHED*/
		default:
			break;
		}
		break;
	default:
		(void) fprintf(stderr, "Got an event of type %d (%x).\n",
		       ev->xany.type, ev->xany.type);
		break;
	}
}

static void
setcell(x, y, set)
int x, y, set;
{
	int xreal = x * CELLWIDTH;
	int yreal = y * CELLHEIGHT;
	register int i = x + 1;
	register int j = y + 1;
	int found, n;

	if (x < 0 || x >= nxcells || y < 0 || y >= nycells)
		return;

	found = 1; n=0;
	while((live_cells[0][n] != i || 
	 live_cells[1][n] != j) && n < total_living) 
		n++;
	if (n == total_living) 
		found = 0;

	if (set && !found) {
		live_cells[0][total_living] = i;
		live_cells[1][total_living] = j;
		oboard[i][j] = ONCODE;
		total_living++;
	} else if (!set && found) {
		live_cells[0][n] = live_cells[0][total_living];
		live_cells[1][n] = live_cells[1][total_living];
		total_living--;
	}
	XFillRectangle(dpy, win, set ? gc : cleargc, xreal, yreal,
	 CELLWIDTH, CELLHEIGHT);
}

static void
makeboard(width, height)
int width, height;
{
	register int i;
	
	nxcells = width;
	nycells = height;
	boardrows1 = (int **) XtCalloc(height+2, sizeof(int *));
	for(i = 0; i < height + 2; i++) {
		boardrows1[i] = (int *) XtCalloc(width+2, sizeof(int));
	}
	oboard = boardrows1;
	live_cells[0] = (int *) XtCalloc((width+2)*(height+2), sizeof(int));
	live_cells[1] = (int *) XtCalloc((width+2)*(height+2), sizeof(int));
	neighbor_cells[0]=(int *) XtCalloc((width+2)*(height+2), sizeof(int));
	neighbor_cells[1]=(int *) XtCalloc((width+2)*(height+2), sizeof(int));
}


static int display_socket_flags = 0;	/*
					 *  nonzero=> server input will
					 *  cause a SIGIO interrupt
					 */

/*
 *  The following code is adapted from GNUPLOT, and uses a pretty neat
 *  scheme for using X events properly in applications that aren't event
 *  driven, but do continuous graphic output, responding asynchronously
 *  to keyboard input. The X and Xt library calls are not known to be
 *  reentrant.  Therefore, because expose events must cause a SIGIO
 *  interrupt handler to redraw the screen and these interrupts can
 *  arrive in the middle of X/Xt library calls, the interrupts must be
 *  disabled while the calls are taking place.  In order to not take a
 *  bad performance hit from calling sigmask on every library request,
 *  the requests are batched.  xw_update then flushes the batched
 *  requests.  It passes the requests off to xw_display, which disables
 *  SIGIO and then processes each item in a specified part of the
 *  display list.  xw_display is also called by the interrupt handler to
 *  redraw the entire window.  No attempt is made to process any
 *  exposure events other than EXPOSE.
 */
/*
 * The next routine sets up the interrupt handler for asynchronous
 *  refresh on expose events.  Method:
 *	1. Remember previous status flags for socket, as these flags
 *	   will be diddled to request and unrequest SIGIO interrupts later.
 *	2. Use signal() to catch SIGIO interrupts with the interrupt
 *		 handler.  We must do this before calling enable_sigio,
 *	   otherwise some signals could happen with nobody to catch
 *		 them, causing a core dump.
 *	3. Use fcntl() to request that the current process be the one
 *	   to which SIGIO interrupts are sent when I/O generates them.
 *	   Otherwise, they are sent to process 0 for some reason.
 *	4. Call enable_sigio(), which uses fcntl() again to request a SIGIO
 *	   when data appears on the X server socket.  Supposedly this
 *	   will work in 4.3 and in Ultrix 2.0, even though certain BSD
 *	   doc says that this call only works when the channel passed it
 *	   is a tty channel.  (Other docs say it works on ttys and
 *	   sockets).  So it is possible that this will lose on some old
 *	   4.2bsd systems.  I am not sure what to do then.
 */
static void
xw_setup_intr ()
{
	static int xw_handle_intr ();

	int socket = XConnectionNumber(dpy); /* 
				       *  channel number of the tcp
				       *  socket to the server
				       */
	
	display_socket_flags = fcntl (socket, F_GETFL, 0);

	(void) signal (SIGIO, xw_handle_intr);
	(void) fcntl (socket, F_SETOWN, getpid());
	(void) enable_sigio ();
}

/*
 *  Request a SIGIO interrupt to be sent whenever input is available on
 *  the socket connected to the X server.  Return old state.
 */
static int
enable_sigio ()
{
	return set_sigio_state (display_socket_flags | FASYNC);
}
 
/*
 *  Cancel request for SIGIO interrupt on input from the server.  The
 *  interrupt should be turned off whenever "normal" traffic (i.e.,
 *  plotting and anything else that does not have to asynchronously
 *  notify xlifedemo that something is happening) is taking place.  Return
 *  old state.
 */
static int
disable_sigio ()
{
	return set_sigio_state (display_socket_flags & ~FASYNC);
}

/*
 *  Set state of sigio to VALUE, if it was something different.  Return
 *  old value so it can be restored.
 */
static int
set_sigio_state (value)
int value;
{
	int socket = XConnectionNumber(dpy);/*
					    *  channel number of the tcp
					    *  socket to the server
					    */
	int oldval = display_socket_flags;

	/* Only call fcntl if we're asking for a real state change (old
	 state different from new state). */
	if (display_socket_flags != value) {
		(void) fcntl (socket, F_SETFL, value);
		display_socket_flags = value;
	}
	return oldval;
}

/*
 *  actually handle the interrupt by simply repeating everything on the
 *  display list up to the last xw_flush().
 */
static int
xw_handle_intr()
{
	XEvent event;
	int old_intr;

	/*
	 *  first, we need to get rid of the X events on the queue.
	 *  However, since reading the events in order to throw them
	 *  away involves Xlib calls, we need to disable SIGIO while it
	 *  is happening.
	 */

	old_intr = disable_sigio ();

	/* Check the X queue for actual exposure events.  This is necessary
	 for two reasons:
	 1. SIGIO interrupts seem to get sent here even though the SIGIO
		request is carefully turned off during any activity that can
		cause traffic on the socket (a kernel bug?).
	 2. The Xlib documentation lies when it says XSelectInput
		masks events other than the ones selected.  I.e., sometimes
		events can arrive that were not asked for.  Therefore,
		we must make sure the purported exposure event really is one.
	*/

	while (XtPending() > 0) {
		XtNextEvent (&event);
		XtDispatchEvent(&event);
	}
	(void) set_sigio_state (old_intr);
}

/*
 *  Cause an XFlush() call, with interrupts disabled.
 */
static void
reentrant_xflush ()
{
	int old_sigio = disable_sigio ();
	XFlush (dpy);
	(void) set_sigio_state (old_sigio);
}

/* If signals aren't off when redraw is called, bad things may happen */
static void
redraw()
{
	int i;
	for(i = 0; i < total_living; i++) 
	   XFillRectangle(dpy, win, gc,
	    xcell(live_cells[0][i]-1), ycell(live_cells[1][i]-1),
				 CELLWIDTH, CELLHEIGHT);
		}

static void
reentrant_redraw ()
{
	int old_sigio = disable_sigio ();
	XClearWindow(dpy, win);
	redraw();
	XFlush(dpy);
	(void) set_sigio_state (old_sigio);
}

static void
xw_update()
{
	int old_sigio = disable_sigio ();
	register int i;
	register RectList *cur_rect = rect_list;

#ifdef DEBUG
	printf("-- update: %d rects\n", rect_list_count);
#endif
	for(i = 0; i < rect_list_count; i++, cur_rect++) {
		XFillRectangle(dpy, win, rect_list[i].set ? gc : cleargc,
		 cur_rect->x, cur_rect->y, CELLWIDTH, CELLHEIGHT);
	}
	rect_list_count = 0;
	XFlush(dpy);
	
	(void) set_sigio_state (old_sigio);
}

typedef struct {
	int i, j;
} Coord;

#define PATTERN_BUF_CHUNK 256
static Coord *pattern_buf = NULL;
static int pattern_buf_size = 0;
static int pattern_buf_count = 0;

typedef struct {
	Coord *coord;
	int ncoords;
} Pattern;

#define PATTERN_CHUNK 32
static Pattern *patterns = NULL;
static int patterns_size = 0;
static int patterns_count = 0;

/* Opens file and appends patterns to list of predefined patterns. */
static void
readpatterns(file)
char *file;
{
	FILE *fp;
	char buf[128];
	int i, j;
	int newpattern = 0;

	if ((fp = fopen(file, "r")) == NULL) {
		(void) fprintf(stderr, "%s: Couldn't open file %s\n", 
		 progname, file);
		return;
	}
	test_and_grow_object(patterns, patterns_count,
	 patterns_size, PATTERN_CHUNK, Pattern);
	test_and_grow_object(pattern_buf, pattern_buf_count,
	 pattern_buf_size, PATTERN_BUF_CHUNK, Coord);
	patterns[patterns_count].coord = &pattern_buf[pattern_buf_count];
	patterns[patterns_count].ncoords = 0;
	while(fgets(buf, sizeof(buf), fp) != NULL) {
		if (sscanf(buf, " %d %d", &i, &j) == 2) {
			newpattern++;
			test_and_grow_object(pattern_buf, pattern_buf_count,
			 pattern_buf_size, PATTERN_BUF_CHUNK, Coord);
			pattern_buf[pattern_buf_count].i = i;
			pattern_buf[pattern_buf_count].j = j;
			pattern_buf_count++;
		} else if (newpattern != 0) {
			/* Lousy parsing - we assume end of pattern */
			patterns[patterns_count].ncoords = newpattern;
			patterns_count++;
			test_and_grow_object(patterns, patterns_count,
			 patterns_size, PATTERN_CHUNK, Pattern);
			patterns[patterns_count].coord =
			 &pattern_buf[pattern_buf_count];
			patterns[patterns_count].ncoords = 0;
			newpattern = 0;
		}
	}
}


/* Puts pattern m with origin at col i, row j */
static void
putpattern(m, x, y)
{
	register int k;
	Coord *p;
	register int yoffset = y;
	register int xoffset = x;

	p = patterns[m].coord;
	k = patterns[m].ncoords;
	while(--k >= 0)
		setcell(xoffset + p[k].j, yoffset + p[k].i, 1);
}


static void
randomize(nrandom)
{
	int x, y;
	int i;
	int nx, ny;

	if (patterns_count == 0)
		return;
	nx = nxcells - 2;
	ny = nycells - 2;
	if (nrandom == 0)
		nrandom = (nxcells + nycells) / 5;
	while(nrandom-- > 0) {
		x = rnd(nx) + 1;
		y = rnd(ny) + 1;
		i = rnd(patterns_count);
		putpattern(i, x, y);
	}
}

static void
clear()
{
	register int i, j;
	int old_sigio = disable_sigio ();
	for(i = 0; i < nycells; i++)
		for(j = 0; j < nxcells; j++)
			oboard[i][j] = 0;
	total_living = 0;
	XClearWindow(dpy, win);
	XSync(dpy, True);
	(void) set_sigio_state (old_sigio);
}


