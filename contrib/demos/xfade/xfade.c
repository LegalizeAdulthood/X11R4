/*
 * xfade - fade two strings in & out
 *
 * Dave Lemke
 * lemke@sun.com
 *
 * acknowledgements to Hesh for providing the usleep() replacement
 * and inspiring creeping featurism. 
 */

/*
 * define USLEEP if you don't have it in libc
 * define R4 if you have the MIT R4 Xlib and want full ICCCM compliance
 *
 * cc -O -o xfade xfade.c -lX11
 */

/************************************************************
Copyright 1989 by Sun Microsystems, Inc. Mountain View, CA.

		All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of Sun or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific prior written permission. Sun and M.I.T.
make no representations about the suitability of this software for
any purpose. It is provided "as is" without any express or implied warranty.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE. IN NO EVENT SHALL SUN BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.
************************************************************/

#include	<stdio.h>
#include	<sys/types.h>
#include	<string.h>
#include	<ctype.h>
#include	<X11/Xos.h>
#include	<X11/Xlib.h>
#include	<X11/Xutil.h>

#define	MAX_VALUE	65535

#define	max(a, b)	(((a) > (b)) ? (a) : (b))
#define	abs(a)		(((a) > 0) ? (a) : -(a))

#define	FONT	"*-times-medium-r-*"
char       *pattern = FONT;
char      **font_list;
int         num_fonts;
char       *scale_name;

Display    *dpy;
Window      fadewin;
int         screen;
Colormap    cmap;
Visual     *visual;
int         depth;
GC          gc;
int         num_steps = 32;
#ifdef R4
Atom        kill_atom,
            protocol_atom;
#endif

/* relative size of strings */
int         basewidth,
            baseheight;
int         basexloc,
            baseyloc;
int         basescale = 12;

int         full_scaling;

Font        fid;

/* these all get adjusted when the window gets resized */
int         width = 300;
int         height = 100;
int         xloc = 30;
int         yloc = 90;

unsigned    sleeptime = 100000;
unsigned    pausetime = 400000;

long        pixel;
long        masks[2];
XColor		fg1, fg2, bg;

char       *string1 = "Tastes Great";
char       *string2 = "Less Filling";

char       *scale_res();
char       *scaled_font();

extern char *getenv();

main(argc, argv)
    int         argc;
    char      **argv;
{
    char       *display = NULL;
    register int i;
    char       *cmd;
    XSetWindowAttributes xswat;
    XSizeHints  xsh;
    int         vmask;
    char	*foreground1 = "black";
    char	*foreground2 = "black";
    char	*background = "white";

    cmd = strrchr(argv[0], '/');
    if (cmd)
	cmd++;
    else
	cmd = argv[0];
    for (i = 1; i < argc; i++) {
	if (strncmp(argv[i], "-d", 2) == 0) {
	    if (argv[++i])
		display = argv[i];
	    else
		usage(cmd);
	} else if (strncmp(argv[i], "-st", 3) == 0) {
	    if (argv[i + 1] && argv[i + 2]) {
		string1 = argv[++i];
		string2 = argv[++i];
	    } else
		usage(cmd);
	} else if (strncmp(argv[i], "-sl", 3) == 0) {
	    if (argv[++i])
		sleeptime = atoi(argv[i]);
	    else
		usage(cmd);
	} else if (strncmp(argv[i], "-fg", 2) == 0) {
	    if (argv[i + 1] && argv[i + 2])	{
		foreground1 = argv[++i];
		foreground2 = argv[++i];
	    } else
		usage(cmd);
	} else if (strncmp(argv[i], "-bg", 2) == 0) {
	    if (argv[++i])
		background = argv[i];
	    else
		usage(cmd);
	} else if (strncmp(argv[i], "-pau", 4) == 0) {
	    if (argv[++i])
		pausetime = atoi(argv[i]);
	    else
		usage(cmd);
	} else if (strncmp(argv[i], "-pat", 4) == 0) {
	    if (argv[++i])
		pattern = argv[i];
	    else
		usage(cmd);
	} else
	    usage(cmd);
    }

    /* sanity check time */
    /* if this is too small, it can take over the server */
    if (sleeptime < 100) {
	sleeptime = 100000;
    }
    if ((dpy = XOpenDisplay(display)) == NULL) {
	fprintf(stderr, "Can\'t open %s\n",
		(display ? display : getenv("DISPLAY")));
	exit(1);
    }
    screen = DefaultScreen(dpy);

    /* set up the color map */
    if (DisplayCells(dpy, screen) > 2)	{
	/* find the proper colors */
	XParseColor(dpy, DefaultColormap(dpy, screen), foreground1, &fg1);
	XParseColor(dpy, DefaultColormap(dpy, screen), foreground2, &fg2);
	XParseColor(dpy, DefaultColormap(dpy, screen), background, &bg);
	cmap_init();
    } else {
	fprintf(stderr, "%s doesn't make sense on a monchrome\n", argv[0]);
	exit(1);
    }

    gc = XCreateGC(dpy, RootWindow(dpy, screen), NULL, 0);

    init_fonts();

    vmask = CWColormap | CWEventMask | CWBackPixel;
    xswat.colormap = cmap;
    xswat.event_mask = ExposureMask | StructureNotifyMask;
    xswat.background_pixel = pixel;

    fadewin = XCreateWindow(dpy, RootWindow(dpy, screen),
			    100, 100, width, height, 0, depth,
			    InputOutput, visual, vmask, &xswat);

    xsh.flags = PPosition | PSize;
    xsh.x = 100;
    xsh.y = 100;
    xsh.width = width;
    xsh.height = height;
    XSetStandardProperties(dpy, fadewin, "Fader", "Fader", None,
			   argv, argc, &xsh);

#ifdef R4
    protocol_atom = XInternAtom(dpy, "WM_PROTOCOLS", False);
    kill_atom = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(dpy, fadewin, &kill_atom, 1);
#endif

    XMapWindow(dpy, fadewin);
    while (1) {
	XEvent      ev;

	if (XPending(dpy)) {
	    XNextEvent(dpy, &ev);

	    if (ev.type == Expose && ev.xexpose.count == 0) {
		/* get rid of any in the queue */
		while (XCheckTypedEvent(dpy, Expose, &ev))
		    ;
		redraw();
	    } else if (ev.type == ConfigureNotify) {
		rescale((XConfigureEvent *)&ev);
#ifdef R4
	    } else if (ev.type == ClientMessage) {
		XClientMessageEvent *cmev = (XClientMessageEvent *) & ev;

		if (cmev->message_type == protocol_atom &&
			cmev->data.l[0] == kill_atom)
		    exit(0);
#endif
	    }
	}
	/* fade strings */
	fadeit();
    }
}

usage(cmd)
    char       *cmd;
{
    (void) fprintf(stderr,
		   "usage: %s [-display display] [-string string1 string2]\n\t[-sleep sleeptime in useconds] [-pause pausetime in useconds]\n\t[-fg foreground color] [-bg background color] [-pattern font_pattern]\n", cmd);
    exit(1);
}

/*
 * initializes colormap
 *
 * finds dynamic visual and creates a new colormap
 */
cmap_init()
{
    XVisualInfo vinfo;
    int         planes;
    XColor      colors[4];

    planes = DisplayPlanes(dpy, screen);
    /* see what kind of visual we're dealing with */
    if (!(XMatchVisualInfo(dpy, screen, planes, PseudoColor, &vinfo)
	  || XMatchVisualInfo(dpy, screen, planes, GrayScale, &vinfo)
	  || XMatchVisualInfo(dpy, screen, planes, DirectColor, &vinfo))) {
	fprintf(stderr, "%s can't run on this display\n");
	exit(1);
    }
    visual = vinfo.visual;
    depth = vinfo.depth;

    /* try to use default cmap to minimize flashing */
    if (visual == DefaultVisual(dpy, screen))	{
	cmap = DefaultColormap(dpy, screen);
	if (XAllocColorCells(dpy, cmap, True, masks, 2, &pixel, 1) == 0) {
	    cmap = XCreateColormap(dpy, RootWindow(dpy, screen), visual, 
		AllocNone);
	    if (XAllocColorCells(dpy, cmap, True, masks, 2, &pixel, 1) == 0) {
		fprintf(stderr, "Couldn't allocate all colors - exiting\n");
		exit(1);
	    }
	}
    } else	{
	cmap = XCreateColormap(dpy, RootWindow(dpy, screen), visual, AllocNone);
	if (XAllocColorCells(dpy, cmap, True, masks, 2, &pixel, 1) == 0) {
	    fprintf(stderr, "Couldn't allocate all colors - exiting\n");
	    exit(1);
	}
    }

    /* background */
    colors[0].flags = DoRed | DoGreen | DoBlue;
    colors[0].pixel = pixel;
    colors[0].red = bg.red;
    colors[0].green = bg.green;
    colors[0].blue = bg.blue;

    /* color 1 */
    colors[1].flags = DoRed | DoGreen | DoBlue;
    colors[1].pixel = pixel | masks[0];
    colors[1].red = fg1.red;
    colors[1].green = fg1.green;
    colors[1].blue = fg1.blue;

    /* color 2 */
    colors[2].flags = DoRed | DoGreen | DoBlue;
    colors[2].pixel = pixel | masks[1];
    colors[2].red = fg2.red;
    colors[2].green = fg2.green;
    colors[2].blue = fg2.blue;

    /* both */
    colors[3].flags = DoRed | DoGreen | DoBlue;
    colors[3].pixel = pixel | masks[0] | masks[1];
    colors[3].red = fg2.red;
    colors[3].green = fg2.green;
    colors[3].blue = fg2.blue;

    XStoreColors(dpy, cmap, colors, 4);
}

/*
 * find all the fonts we have that match the pattern.
 * flag things if we've got a scalable font
 *
 * calculate the base scales and locations
 */
init_fonts()
{
    int         dir,
                ascent,
                descent;
    XCharStruct overall1,
                overall2;
    char       *fname;
    int         f;
    XFontStruct *pfont;

    font_list = XListFonts(dpy, pattern, 50, &num_fonts);
    if (num_fonts == 0) {
	fprintf(stderr, "Sorry, can't find any fonts matching %s\n", pattern);
	exit(1);
    }
    if ((f = found_scalable()) != -1) {
	full_scaling = 1;
	scale_name = font_list[f];
	fname = scale_res(basescale, 75, 75);
	pfont = XLoadQueryFont(dpy, fname);
	if (pfont == (XFontStruct *) 0) {
	    fprintf(stderr, "can't get font\n");
	    exit(1);
	}
    } else {
	full_scaling = 0;
	fname = scaled_font(basescale);
	pfont = XLoadQueryFont(dpy, fname);
	if (pfont == (XFontStruct *) 0) {
	    fprintf(stderr, "can't get font\n");
	    exit(1);
	}
    }


    XTextExtents(pfont, string1, strlen(string1), &dir, &ascent,
		 &descent, &overall1);

    XTextExtents(pfont, string2, strlen(string2), &dir, &ascent,
		 &descent, &overall2);

    basexloc = XTextWidth(pfont, "A", 1);
    baseyloc = pfont->ascent + pfont->descent;

    basewidth = max(overall1.width, overall2.width) +
	2 * basexloc;
    baseheight = max((overall1.ascent + overall1.descent),
		     (overall2.ascent + overall2.descent)) * 2;

    width = basewidth;
    height = baseheight;

    /* dump the extent info, and reload as an ID */
    XFreeFont(dpy, pfont);
    fid = XLoadFont(dpy, fname);
    free(fname);

    XSetFont(dpy, gc, fid);
}

/*
 * returns the font (as an index into the list) if it finds a scalable
 * version
 */
found_scalable()
{
    int         i,
                j;
    char       *d;
    int         scale;

    for (i = 0; i < num_fonts; i++) {
	d = font_list[i];
	for (j = 0; j < 7; j++) {
	    d = strchr(d, '-');
	    if (d == NULL)
		goto next;
	    d++;		/* skip over '-' */
	}
	scale = atoi(d);
	if (scale == 0)
	    return i;
next:
	;
    }
    return -1;
}

/*
 * returns the scale for the given font name
 */
return_scale(fname)
    char       *fname;
{
    int         i;
    char       *d;

    d = fname;
    for (i = 0; i < 7; i++) {
	d = strchr(d, '-');
	if (d == NULL)
	    return -1;
	d++;		/* skip over '-' */
    }
    return atoi(d);
}

/*
 * builds a font name with the given scale and resolution out of scale_name
 */
char       *
scale_res(scale, xres, yres)
    int         scale,
                xres,
                yres;
{
    char       *newname;
    char        foo[10];
    int         dashes = 0;
    char       *c,
               *n,
               *f;

    c = scale_name;
    n = newname = (char *) malloc(strlen(scale_name) * 2 + 1);
    while (*c != '\0') {
	*n = *c;
	if (*c == '-') {
	    n++;
	    c++;		/* skip dash */
	    dashes++;
	    if (dashes == 7) {
		sprintf(foo, "%1d", scale);
		f = foo;
		while (isdigit(*c))
		    c++;
		while (*f != '\0')
		    *n++ = *f++;
	    } else if (dashes == 9) {
		sprintf(foo, "%1d", xres);
		f = foo;
		while (isdigit(*c))
		    c++;
		while (*f != '\0')
		    *n++ = *f++;
	    } else if (dashes == 10) {
		sprintf(foo, "%1d", yres);
		f = foo;
		while (isdigit(*c))
		    c++;
		while (*f != '\0')
		    *n++ = *f++;
	    }
	} else {
	    n++;
	    c++;
	}
    }
    *n = '\0';
    return newname;
}

/*
 * returns the font name the server knows closest to the
 * given scale
 */
char       *
scaled_font(scale)
    int         scale;
{
    int         i;
    int         cvalue = 10000;
    int         closest;
    int         newscale;

    for (i = 0; i < num_fonts; i++) {
	newscale = return_scale(font_list[i]);
	if (abs(scale - newscale) < cvalue) {
	    closest = i;
	    cvalue = abs(scale - newscale);
	}
    }
    return strdup(font_list[closest]);
}

/*
 * handle a configure notify event.  rescale the font for the
 * new window size, and adjust the text location
 */
rescale(ev)
    XConfigureEvent *ev;
{
    char       *fname;
    static	init = 0;
    int         scale,
                xres,
                yres;

    if (init && ev->width == width && ev->height == height)
	return;

    init = 1;
    width = ev->width;
    height = ev->height;

    xres = (int) ((float) width / (float) basewidth *
		  (float) basescale * 75.0);
    yres = (int) ((float) height / (float) baseheight *
		  (float) basescale * 75.0);
    xloc = basexloc * xres / (75 * basescale);
    yloc = baseyloc * yres / (75 * basescale);

    if (full_scaling) {
	scale = 1;
	/* normalize to avoid overflowing things */
	if (xres > 1000 && yres > 1000) {
	    xres /= 10;
	    yres /= 10;
	    scale = 10;
	}
	fname = scale_res(scale, xres, yres);
    } else {
	scale = (int) ((float) height / (float) baseheight *
		       (float) basescale);
	fname = scaled_font(scale);
    }
    XUnloadFont(dpy, fid);
    fid = XLoadFont(dpy, fname);
    free(fname);
    XSetFont(dpy, gc, fid);

}

/*
 * deal with exposure.
 */
redraw()
{
    /* clear out window */
    XSetForeground(dpy, gc, pixel);
    XSetPlaneMask(dpy, gc, (long) ~0);
    XFillRectangle(dpy, fadewin, gc, 0, 0, width, height);

    /* clear it out */
    XSetPlaneMask(dpy, gc, masks[0]);
    XSetForeground(dpy, gc, 0);
    XFillRectangle(dpy, fadewin, gc, 0, 0, width, height);

    /* draw first string */
    XSetForeground(dpy, gc, pixel | masks[0]);
    XDrawString(dpy, fadewin, gc, xloc, yloc, string1, strlen(string1));

    /* clear it out */
    XSetPlaneMask(dpy, gc, masks[1]);
    XSetForeground(dpy, gc, 0);
    XFillRectangle(dpy, fadewin, gc, 0, 0, width, height);

    /* draw second string */
    XSetForeground(dpy, gc, pixel | masks[1]);
    XDrawString(dpy, fadewin, gc, xloc, yloc, string2, strlen(string2));
}

#define	UP	1
#define	DOWN	0

/*
 * fade the two strings back and forth
 *
 * here's where we need the speed
 */
fadeit()
{
    static int  i;
    static      direction = UP;
    static      init = 0;
    static XColor colors[3];
    static	short d1red, d1green, d1blue;
    static	short d2red, d2green, d2blue;
    static	short ddred, ddgreen, ddblue;

    if (!init) {
	init = 1;
	colors[0].flags = DoRed | DoGreen | DoBlue;
	colors[0].pixel = pixel | masks[0];
	colors[0].red = fg1.red;
	colors[0].green = fg1.green;
	colors[0].blue = fg1.blue;

	colors[1].flags = DoRed | DoGreen | DoBlue;
	colors[1].pixel = pixel | masks[1];
	colors[1].red = bg.red;
	colors[1].green = bg.green;
	colors[1].blue = bg.blue;

	colors[2].flags = DoRed | DoGreen | DoBlue;
	colors[2].pixel = pixel | masks[0] | masks[1];
	colors[2].red = fg1.red;
	colors[2].green = fg1.green;
	colors[2].blue = fg1.blue;

	d1red = (bg.red - fg1.red)/num_steps;
	d1green = (bg.green - fg1.green)/num_steps;
	d1blue = (bg.blue - fg1.blue)/num_steps;

	d2red = (bg.red - fg2.red)/num_steps;
	d2green = (bg.green - fg2.green)/num_steps;
	d2blue = (bg.blue - fg2.blue)/num_steps;

	ddred = (fg1.red - fg2.red)/num_steps;
	ddgreen = (fg1.green - fg2.green)/num_steps;
	ddblue = (fg1.blue - fg2.blue)/num_steps;

	i = 0;
    }
    if (direction == UP) {
	colors[0].red += d1red;
	colors[0].green += d1green;
	colors[0].blue += d1blue;

	colors[1].red -= d2red; 
	colors[1].green -= d2green;
	colors[1].blue -= d2blue;

	colors[2].red -= ddred; 
	colors[2].green -= ddgreen;
	colors[2].blue -= ddblue;
    } else {
	colors[0].red -= d1red;
	colors[0].green -= d1green;
	colors[0].blue -= d1blue;

	colors[1].red += d2red; 
	colors[1].green += d2green;
	colors[1].blue += d2blue;

	colors[2].red += ddred; 
	colors[2].green += ddgreen;
	colors[2].blue += ddblue;
    }
    XStoreColors(dpy, cmap, colors, 3);
    i++;
    if (i == num_steps) {
	direction = (direction == UP) ? DOWN : UP;
	i = 0;
	usleep(pausetime);	/* pause at the end of each cycle */
    }
    /* sleep a bit */
    usleep(sleeptime);
    XSync(dpy, 0);
}


#ifdef USLEEP

#include <signal.h>
#include <time.h>

usleep(value)
long value;
{
	void stopme();
	struct itimerval ntval, otval;

	ntval.it_interval.tv_sec = 0;
	ntval.it_interval.tv_usec = 0;
	ntval.it_value.tv_sec = 0;
	ntval.it_value.tv_usec = value;
	signal(SIGALRM, stopme);
	setitimer(ITIMER_REAL, &ntval, &otval);
	pause();
}

void
stopme()
{
	signal(SIGALRM, SIG_DFL);
}

#endif /* USLEEP */

