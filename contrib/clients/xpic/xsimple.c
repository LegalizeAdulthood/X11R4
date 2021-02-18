/* Dumb program to experiment with dash styles - cannibalized from plotspice */

#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

Display *dpy;
Window win;
GC gc;
#define WINNAME			"dashdesign"

/*
 *  Each line consists of the length of the ON, followed by the length
 *  of the OFF dash, PostScript style.
 */
char dashlist[32];
char *progname;

XFontStruct *xfont;

#define xfontnamesmall	"8x13"
#define xfontnamebig	"cour.b.18"

#define xfontwidththresh	1200		/* use big font if screen wider than this */

XPoint line_xvertices[] = {
	{0, 0},
	{200, 200},
	{0, 200},
	{200, 0},
	{200, 200},
};
int nvertices = 5;

int window_x, window_y, window_xorg, window_yorg;
int window_ysize100 = 25;
int window_xsize100 = 25;
int window_borderwidth = 3;
	
setdashes(dashes, n)
char *dashes;
{
	int i;
	printf("dashlist is { ");
	for(i = 0; i < n; i++)
		printf("%d ", dashes[i]);
	printf(" }\n");
	(void) fflush(stdout);
	XSetDashes(dpy, gc, 0, dashes, n);
	XClearWindow(dpy, win);
	XDrawLines (dpy, win, gc, line_xvertices, nvertices, CoordModeOrigin);
	XFlush(dpy);
}

main(argc, argv)
char **argv;
{
	char buf[128];
	char *s, *s1;
	int i = 0;

	progname = argv[0];
	
	CreateXWindow();
	while((s = gets(buf)) != NULL) {
		while((s1 = strtok(s, " \t\n")) != NULL) {
			dashlist[i++] = atoi(s1);
			s = NULL;
		}
		setdashes(dashlist, i);
		i = 0;
	}
}

	
/* 
 *  This creates the X Window, gets some rudimentary defaults from the
 *  defaults file, - no, we don't use the resource manager yet! - and
 *  returns. All useful stuff is in global variables, notably dpy, win,
 *  gc, and xfont. It uses the global variable progname, which is
 *  assumed to contain the basename(argv[0]). And it expects WINNAME to
 *  be defined to the string that is used for the window name. Basically
 *  modified from Dave Rosenthal's hellow world using Xlib program.
 *  Purists who like the X Toolkit can try writing a %$@$#% graph
 *  widget. Gah!
 */
#define	ARG_FONT		"font"
#define	ARG_BORDER_COLOR	"bordercolor"
#define	ARG_FOREGROUND		"foreground"
#define	ARG_BACKGROUND		"background"
#define ARG_BORDER		"border"
#define	ARG_GEOMETRY		"geometry"
#define DEFAULT_GEOMETRY	""
XWMHints	xwmh = {
    (InputHint|StateHint),	/* flags */
    False,			/* input */
    NormalState,		/* initial_state */
    0,				/* icon pixmap */
    0,				/* icon window */
    0, 0,			/* icon location */
    0,				/* icon mask */
    0,				/* Window group */
};

CreateXWindow()
{
	char       *fontName;		/* Name of font for string */
	XFontStruct *fontstruct;	/* Font descriptor */
	unsigned long ftw, fth, pad;	/* Font size parameters */
	unsigned long fg, bg, bd;	/* Pixel values */
	unsigned long bw;		/* Border width */
	char       *tempstr;		/* Temporary string */
	XColor      color;		/* Temporary color */
	Colormap    cmap;		/* Color map to use */
	XGCValues   gcv;		/* Struct for creating GC */
	XEvent      event;		/* Event received */
	XSizeHints  xsh;		/* Size hints for window manager */
	char       *geomSpec;		/* Window geometry string */
	XSetWindowAttributes xswa;	/* Temporary Set Window Attribute struct */
	/* Stuff returned from the XGetGeometry on the RootWindow */
	Window     dummyWindow;
	int        width, height;
	int        x, y;
	int        depth, borderwidth;

	dpy = XOpenDisplay ((char *) NULL);
	if (dpy == NULL)
	{	(void) fprintf (stderr, "%s: Can't get display %s\n", 
		 progname, XDisplayName(NULL));
		exit (2);
	}
	XGetGeometry (dpy, DefaultRootWindow(dpy), &dummyWindow, &x, &y, 
	 &width, &height, &borderwidth, &depth);
	if ((fontName = XGetDefault(dpy, progname, ARG_FONT)) == NULL)
	{	/* Ok - choose an intelligent default */
		if (width > xfontwidththresh)
			fontName = xfontnamebig;
		else
			fontName = xfontnamesmall;
	}
	if ((xfont = XLoadQueryFont(dpy, fontName)) == NULL)
	{	(void) fprintf(stderr, "%s: display %s doesn't know font %s\n",
			progname, DisplayString(dpy), fontName);
		exit(1);
	}
	fth = xfont->max_bounds.ascent + xfont->max_bounds.descent;
	ftw = xfont->max_bounds.width;

	/*
	 * Select colors for the border,  the window background,  and the
	 * foreground.  We use the default colormap to allocate the colors in.
	 * See Sections 2.2.1, 5.1.2, & 10.4.
	 */
	cmap = DefaultColormap(dpy, DefaultScreen(dpy));
	if ((tempstr = XGetDefault(dpy, progname, ARG_BORDER_COLOR)) == NULL ||
		XParseColor(dpy, cmap, tempstr, &color) == 0 ||
		XAllocColor(dpy, cmap, &color) == 0) {
		bd = BlackPixel(dpy, DefaultScreen(dpy));
	}
	else
	{	bd = color.pixel;
	}
	if ((tempstr = XGetDefault(dpy, progname, ARG_BACKGROUND)) == NULL ||
		XParseColor(dpy, cmap, tempstr, &color) == 0 ||
		XAllocColor(dpy, cmap, &color) == 0) {
		bg = WhitePixel(dpy, DefaultScreen(dpy));
	}
	else
	{	bg = color.pixel;
	}
	if ((tempstr = XGetDefault(dpy, progname, ARG_FOREGROUND)) == NULL ||
		XParseColor(dpy, cmap, tempstr, &color) == 0 ||
		XAllocColor(dpy, cmap, &color) == 0) {
		fg = BlackPixel(dpy, DefaultScreen(dpy));
	}
	else
	{	fg = color.pixel;
	}
	/*
	 * Set the border width of the window.
	 */
	pad = 0;
	if ((tempstr = XGetDefault(dpy, progname, ARG_BORDER)) == NULL)
		bw = window_borderwidth;
	else
		bw = atoi(tempstr);

	/*
	 * Deal with providing the window with an initial position & size.
	 * Fill out the XSizeHints struct to inform the window manager. See
	 * Sections 9.1.6 & 10.3.
	 */
	geomSpec = XGetDefault(dpy, progname, ARG_GEOMETRY);
	if (geomSpec == NULL)
	{	/*
		 * The defaults database doesn't contain a specification of the
		 * initial size & position - fit the window to the text and locate
		 * it in the center of the screen.
		 */
		xsh.flags = (PPosition | PSize);
		xsh.width = window_x = width * window_xsize100 / 100;
		xsh.height = window_y = height * window_ysize100 / 100;
		xsh.x = window_xorg = width * (95 - window_xsize100) / 100;
		xsh.y = window_yorg = height / 20;
	}
	else
	{	int         bitmask;

		bzero(&xsh, sizeof(xsh));
		bitmask = XGeometry(dpy, DefaultScreen(dpy), geomSpec, 
		 DEFAULT_GEOMETRY,bw, ftw, fth, pad, pad, &(xsh.x), &(xsh.y),
		 &(xsh.width), &(xsh.height));
		if (bitmask & (XValue | YValue))
		{	xsh.flags |= USPosition;
		}
		if (bitmask & (WidthValue | HeightValue))
		{	xsh.flags |= USSize;
		}
	}

	/*
	 * Create the Window with the information in the XSizeHints, the
	 * border width,  and the border & background pixels. See Section 3.3.
	 */
	win = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy),
				      xsh.x, xsh.y, xsh.width, xsh.height,
				      bw, bd, bg);

	/*
	 * Set the standard properties for the window managers. See Section
	 * 9.1.
	 */
	XSetStandardProperties(dpy, win, WINNAME, WINNAME, None, &progname, 1,
	 &xsh);
	XSetWMHints(dpy, win, &xwmh);

	/*
	 * Ensure that the window's colormap field points to the default
	 * colormap,  so that the window manager knows the correct colormap to
	 * use for the window.  See Section 3.2.9. Also,  set the window's Bit
	 * Gravity to reduce Expose events.
	 */
	xswa.colormap = DefaultColormap(dpy, DefaultScreen(dpy));
	xswa.bit_gravity = CenterGravity;
	XChangeWindowAttributes(dpy, win, (CWColormap | CWBitGravity), &xswa);

	/*
	 * Create the GC for writing the text.  See Section 5.3.
	 */
	gcv.font = xfont->fid;
	gcv.foreground = fg;
	gcv.background = bg;
	gcv.line_width = 1;
	gcv.line_style = LineOnOffDash;
	gc = XCreateGC(dpy, win, (GCFont | GCForeground | GCBackground
	 | GCLineWidth | GCLineStyle), &gcv);

	/*
	 * Specify the event types we're interested in - only Exposures.  See
	 * Sections 8.5 & 8.4.5.1
	 */
	XSelectInput(dpy, win, ExposureMask);

	/*
	 * Map the window to make it visible.  See Section 3.5.
	 */
	XMapWindow(dpy, win);

	/* 
	 *  Loop forever, examining each event. We neeed to do this
	 *  because, unlike X10, X11 does not guarantee that the window
	 *  is mapped when it comes back from an XMapWindow. So we wait
	 *  till we get an Exposure Event - then we know we can go ahead.
	 */
	while (1) {
		/*
		 * Get the next event
		 */
		XNextEvent(dpy, &event);

		/*
		 * On the last of each group of Expose events,  repaint the entire
		 * window.  See Section 8.4.5.1.
		 */
		if (event.type == Expose && event.xexpose.count == 0)
		{	XWindowAttributes xwa;	/* Temp Get Window Attribute struct */
		    int         x, y;

		    /*
		     * Remove any other pending Expose events from the queue to
		     * avoid multiple repaints. See Section 8.7.
		     */
		    while (XCheckTypedEvent(dpy, Expose, &event));

		    /*
		     * Find out how big the window is now,  so that we can center
		     * the text in it.
		     */
		    if (XGetWindowAttributes(dpy, win, &xwa) == 0)
			break;
		    window_x = xwa.width;
		    window_y = xwa.height;
		    /*
		     * Fill the window with the background color.
		     */
		    XClearWindow(dpy, win);
		    /* We don't want any more events */
		    XSelectInput(dpy, win, (unsigned long) 0);
		    gcv.graphics_exposures = False;
		    XChangeGC(dpy, gc, (GCGraphicsExposures), &gcv);
		    XSync(dpy, 1);
		    break;
		}
	}
}
