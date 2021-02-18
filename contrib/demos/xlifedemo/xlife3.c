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

#define xcell(i)		((i)*CELLWIDTH)
#define ycell(i)		((i)*CELLHEIGHT)

/* Storage for the old board and new board */
static int **boardrows1, **boardrows2;
/*
 *  Pointers to the current board for the iteration, and the new board
 *  for the next iteration - these pointers point to boardrows[12]
 *  alternately.
 */
static int **nboard, **oboard;
/* Number of columns and rows of cells */
static int nxcells, nycells;
static int nrandom;

/* Boolean variables */
static int running = 0;	/* Are we running the simulation or not ? */
static int notover = 1;	/* zero if user wants to quit */
static int changed = 0;

static void do_life();
static void makeboard();
static void fill_rectangle();
static void setcell();
static void redraw();
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
	static Boolean workproc();
	Widget toplevel;
	Widget w;
	XGCValues gcv;
	XEvent event;

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
	 * We now add a work proc. This procedure will be executed in
	 * between events by the toolkit.
	 */
	(void) XtAddWorkProc(workproc, (caddr_t) NULL);
	/*
	 *  Now process the events. This is a straightforward eventloop
	 */
	XtMainLoop();
	exit(0);
}

/*ARGSUSED*/
static Boolean
workproc(closure)
caddr_t closure;
{
	if (running)
	    do_life();
	return FALSE;
}

/* Actual game of life iteration */
static void
do_life()
{
	int **tmp;
	register int *nrow, *orow, *orowprev, *orownext;
	register int i, j;
	register int neighbours;
#ifdef DEBUG
	int done = 0;
	int filled = 0;
#endif

	changed = 0;
	orow = oboard[0];
	orownext = oboard[1];
	for(i = 1; i <= nycells; i++) {
		orowprev = orow;
		orow = orownext;	/* orow = oboard[i] */
		orownext = oboard[i+1];
		nrow = nboard[i];
		for(j = 1; j <= nxcells; j++) {
			/* calculate cell from oboard to new board */
			neighbours =
			 orowprev[j-1] + orowprev[j] + orowprev[j+1] +
			 orow[j-1] + orow[j+1] +
			 orownext[j-1] + orownext[j] + orownext[j+1];
			if (neighbours == 3 && orow[j] == 0) {
				nrow[j] = 1;
				fill_rectangle(j, i, 1);
#ifdef DEBUG
				done++;
#endif
			} else if (neighbours != 2 && neighbours != 3 &&
			 orow[j] != 0) {
			 	nrow[j] = 0;
				fill_rectangle(j, i, 0);
#ifdef DEBUG
				done++;
#endif
			} else {
				nrow[j] = orow[j];
				/* Since it hasn't changed, we don't update */
			}
#ifdef DEBUG
			if (nrow[j])
				filled++;
#endif
		}
	}
	/* Stop running if we haven't had a change in the cell */
	if (changed == 0)
		running = 0;
	tmp = nboard;
	nboard = oboard;
	oboard = tmp;
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
	changed++;
	XFillRectangle(dpy, win, set ? gc : cleargc, xcell(x), ycell(y),
	 CELLWIDTH, CELLHEIGHT);
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
		(void) printf("Got an event of type %d (%x).\n",
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
	register int i = y + 1;
	register int j = x + 1;

	if (x < 0 || x >= nxcells || y < 0 || y >= nycells)
		return;
	if (set)
		oboard[i][j] = 1;
	else
		oboard[i][j] = 0;

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
	boardrows2 = (int **) XtCalloc(height+2, sizeof(int *));
	for(i = 0; i < height+2; i++) {
		boardrows1[i] = (int *) XtCalloc(width+2, sizeof(int));
		boardrows2[i] = (int *) XtCalloc(width+2, sizeof(int));
	}
	oboard = boardrows1;
	nboard = boardrows2;
}


/* If signals aren't off when redraw is called, bad things may happen */
static void
redraw()
{
	int i, j;
	for(i = 1; i <= nycells; i++) {
		for(j = 1; j <= nxcells; j++) {
			if(oboard[i][j])
				XFillRectangle(dpy, win, gc,
				 xcell(j-1), ycell(i-1),
				 CELLWIDTH, CELLHEIGHT);
		}
	}
}

/* Typical usage of object.h */
typedef struct {
	int i, j;
} Coord;

#define PATTERN_BUF_CHUNK 256
static Coord *pattern_buf = NULL;
static int pattern_buf_size = 0;
static int pattern_buf_count = 0;

/* yet another array of objects */
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
	for(i = 0; i < nycells; i++)
		for(j = 0; j < nxcells; j++)
			oboard[i][j] = nboard[i][j] = 0;
	XClearWindow(dpy, win);
}
