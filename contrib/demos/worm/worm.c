/*
 * worm.c: draw wiggly worms.
 *
 * Adapted from a concept in the Dec 87 issue of Scientific American.
 * Makes a nice lockscreen via "lockscreen nice worm".
 *
 * compile: cc worm.c -o worm -lm -lsuntool -lsunwindow -lpixrect
 *
 * usage: worm [-l length] [-s size] [-n number]
 *	where "length" is length of each worm in segments (default 50)
 *	      "size" is size of each segment (default 2)
 *	      "number" is number of worms to draw (default 64)
 *
 * This program looks best on a color monitor. Try these options:
 * worm -n 1	   Just one really fast worm
 * worm -l 2       Paramecia
 * worm -s 500     Mondrian painting (actually enormous worms)
 * worm -l -1      Jackson Pollack painting (actually infinite length worms)
 *
 * -- Thu Dec 17 09:58:48 PST 1987
 * -- Brad Taylor (brad@sun)
 */

/*
 * hacked to use X11 by Dave Lemke (lemke@sun.com)
 * Wed Dec 23 09:57:32 PST 1987
 *
 * additional options:
 *	-S -R -C [-g geometry] [-d display]
 *	
 *	-S	screen saver mode - covers screen
 *	-R	rotate colormap while running
 *	-C	chromocolor worms - colors change as they crawl
 */

/************************************************************
Copyright 1988 by Sun Microsystems, Inc. Mountain View, CA.

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
#include	<math.h>
#include	<X11/Xos.h>
#include	<X11/Xlib.h>
#include	<X11/Xutil.h>

#define	NUM_COLORS	256
#define	MIN_COLORS	16
#define SEGMENTS  36
#define PI 3.14159265358979323844

extern char *malloc();
extern long random();

typedef struct _wormstuff {
	int *xcirc;
	int *ycirc;
	int dir;
	int tail;
	int x;
	int y;
}	wormstuff;


wormstuff *worm_init();
int wormlength = 50;
int circsize = 2;
int nworms = 64;
int sintab[SEGMENTS];
int costab[SEGMENTS];
int ncolors;

XWMHints	xwmh =	{
	(InputHint | StateHint), True, NormalState, 0, 0, 0, 0, 0, 0,
};

Display	*dpy;
Window	w;
int	screen;
GC	gc, wgc;
Colormap	cmap;
Bool	is_color = True;
Bool	is_dynamic = False;
Bool	screen_saver = False;
Bool	rotate = False;
Bool	chromocolor = False;
XColor	colors[NUM_COLORS];
extern char	*getenv();
char	*defgeo = "=500x500+10+10";
Visual	*visual;
int	depth = 1;
long	backpixel;
Atom	protocol_atom, kill_atom;

main(argc, argv)
	int argc;
	char **argv;
{
	char	*display = NULL;
	char	*geo = NULL;
	int xsize = 500;
	int ysize = 500;
	int	x = 0, y = 0;
	register int i;
	wormstuff **worm;
	char *cmd;
	char	c;
	extern int optind;
	extern char *optarg;
	XSizeHints	xsh;
	XWindowAttributes	xwa;
	int	vmask;
	XSetWindowAttributes	values;
	Bool	freeze = False;
	Bool	is_visible = True;

	cmd = argv[0];
	while ((c = getopt(argc, argv, "SCRl:n:g:d:s:")) != EOF)
		switch(c)	{
		case 'l':
			wormlength = atoi(optarg);
			break;
		case 'n':
			nworms = atoi(optarg);
			break;
		case 's':
			circsize = atoi(optarg);
			break;
		case 'd':
			display = optarg;
			break;
		case 'g':
			geo = optarg;
			break;
		case 'S':
			screen_saver = True;
			break;
		case 'C':
			chromocolor = True;
			break;
		case 'R':
			rotate = True;
			break;
		case '?':
			usage(cmd);
			}

	srandom(getpid());
	for (i = 0; i < SEGMENTS; i++) {
		sintab[i] = round(circsize * sin(i * 2 * PI / SEGMENTS));
		costab[i] = round(circsize * cos(i * 2 * PI / SEGMENTS));
	}
	if ((dpy = XOpenDisplay(display)) == NULL)	{
		fprintf(stderr,"Can\'t open %s\n", 
			(display ? display : getenv("DISPLAY")));
		exit(0);
		}
	screen = DefaultScreen(dpy);
	ncolors = NUM_COLORS;

	if (screen_saver)	{
		xsize = DisplayWidth(dpy, screen);
		ysize = DisplayHeight(dpy, screen);
		x = 0; y = 0;
		}
	else		{
		int	flags;

		if (geo == NULL)
			geo = defgeo;
		flags = XParseGeometry(geo, &x, &y, &xsize, &ysize);
		if ((flags & XValue) && (flags & XNegative))
			x += DisplayWidth(dpy, screen) - xsize;
		if ((flags & YValue) && (flags & YNegative))
			y += DisplayHeight(dpy, screen) - ysize;
		}

	visual = DefaultVisual(dpy, screen);
	depth = DefaultDepth(dpy, screen);
	cmap = DefaultColormap(dpy, screen);

	/* set up the color map */
	if (DisplayCells(dpy, screen) > 2)
		cmap_init(w);
	else	{
		is_color = False;
		backpixel = BlackPixel(dpy, screen);
		}

	vmask = CWBackPixel | CWColormap;
	values.background_pixel = backpixel;
	values.colormap = cmap;

	w = XCreateWindow(dpy, RootWindow(dpy, screen), x, y, xsize, ysize, 0, 
		depth, InputOutput, visual, vmask, &values);

	xsh.flags = (PPosition | PSize);
	xsh.x = x; xsh.y = y;
	xsh.width = xsize; xsh.height = ysize;
	XSetStandardProperties(dpy, w, "Worms", "Worms", None, 
		argv, argc, &xsh);
	XSetWMHints(dpy, w, &xwmh);

	protocol_atom = XInternAtom(dpy, "WM_PROTOCOLS", False);
	kill_atom = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(dpy, w, &kill_atom, 1);

	XMapRaised(dpy, w);
	XSelectInput(dpy, w, ExposureMask | StructureNotifyMask | 
#ifdef DUMB_WM
		EnterWindowMask | LeaveWindowMask |
#endif
		KeyPressMask | VisibilityChangeMask);
	gc = XCreateGC(dpy, w, 0, 0);
	wgc = XCreateGC(dpy, w, 0, 0);
	XSetForeground(dpy, wgc, WhitePixel(dpy, screen));

	worm = (wormstuff **)malloc((unsigned)(sizeof(wormstuff) * nworms));
	for (i = 0; i < nworms; i++)	{
		worm[i] = worm_init(xsize, ysize, wormlength);
		}

	for (;;) {
		XEvent	e;
		register int	wcolor, chromo;	/* chromo looks best with HSB */

		if (XPending(dpy))	{
			XNextEvent(dpy, &e);

			if (e.type == VisibilityNotify)	{
				XVisibilityEvent *ev = (XVisibilityEvent *) &e;
				if (ev->state == VisibilityFullyObscured)
					is_visible = False;
				else is_visible = True;
				}
			else if (e.type == KeyPress)	{
				if (freeze == False)
					freeze = True;
				else freeze = False;
				}
			else if (e.type == ConfigureNotify)	{
				XConfigureEvent *ev = (XConfigureEvent *) &e;

				xsize = ev->width; ysize = ev->height;
				for (i = 0; i < nworms; i++)	{
					free(worm[i]->xcirc); 
					free(worm[i]->ycirc);
					free(worm[i]);
					worm[i] = worm_init(xsize, ysize, 
						wormlength);
					}
				XClearWindow(dpy, w);
/* really want to remove all the pending graphics requests - can't figure out
 * how...
 * attempted to use GraphicsExposure, but it put so much crap into the
 * queue that the configure was never found... */
				XFlush(dpy);
				}
			else if (e.type == ClientMessage)	{
			XClientMessageEvent *ev = (XClientMessageEvent *)&e;

			if (ev->message_type == protocol_atom && 
			    ev->data.l[0] == kill_atom)
				exit(0);
			}
#ifdef DUMB_WM
			else if (e.type == EnterNotify)
				XInstallColormap(dpy, cmap);
			else if (e.type == LeaveNotify)
				XUninstallColormap(dpy, cmap);
#endif
			/*
			 * since visibvility notify doesn't allow for 
			 * the totally obscured -> partially obscured
			 * case, we have to depend on exposure instead.
			 */
			else if (e.type == Expose)
				is_visible = True;
			}
		if (is_visible)	{
			if (rotate && is_color && is_dynamic)   {
				rotate_colors();
				}
			if (!freeze)	{
			    for (i = 0; i < nworms; i++) {
				wcolor = (((i * ncolors) / nworms) + chromo) 
					% ncolors;
				worm_doit(worm[i], xsize, ysize, 
					colors[wcolor].pixel);
				}
			    }
			/* note that there is a little jump in the worms
			 * if they are frozen and no rotation exists.
			 * doesn't seem to be possible to (easily) get away 
			 * from this */
			}
		if (chromocolor)
			chromo++;
	}
}

rotate_colors()
{
register int	i;
long	temp;

	temp = colors[1].pixel;		/* start at 1 - don't want the black */
	for (i = 1; i < ncolors - 1; i++)	{
		colors[i].pixel = colors[i+1].pixel;
		}
	colors[ncolors - 1].pixel = temp;
	XStoreColors(dpy, cmap, colors, ncolors);
}

wormstuff *
worm_init(xsize, ysize, wormlength)
	int xsize;
	int ysize;
	int wormlength;
{
	register int i;
	wormstuff *ws;

	ws = (wormstuff *)malloc(sizeof(wormstuff));
	if (wormlength > 0) {
		ws->xcirc = (int *)malloc((unsigned)(wormlength * 
						     sizeof(int)));
		ws->ycirc = (int *)malloc((unsigned)(wormlength * 
						     sizeof(int)));
		for (i = 0; i < wormlength; i++) {
			ws->xcirc[i] = xsize / 2;
			ws->ycirc[i] = ysize / 2;
		}
	}
	ws->dir = (unsigned) random() % SEGMENTS;
	ws->tail = 0;
	ws->x = xsize / 2;
	ws->y = ysize / 2;
	return (ws);
}

worm_doit(ws, xsize, ysize, color)
	register wormstuff *ws;
	int xsize;
	int ysize;
	unsigned long color;
{
	register int x, y;

	if (wormlength > 0) {
		ws->tail = (ws->tail + 1) % wormlength;
		x = ws->xcirc[ws->tail];
		y = ws->ycirc[ws->tail];
		XClearArea(dpy, w, x, y, circsize, circsize, False);
	}
	if (random() & 1) {
		ws->dir = (ws->dir + 1) % SEGMENTS;
	} else {
		ws->dir = (ws->dir + SEGMENTS - 1) % SEGMENTS;
	}
	x = (ws->x + costab[ws->dir] + xsize) % xsize;
	y = (ws->y + sintab[ws->dir] + ysize) % ysize;
	if (wormlength > 0) {
		ws->xcirc[ws->tail] = x;
		ws->ycirc[ws->tail] = y;
	}
	if (is_color)
		drawseg(x, y, color);
	else	{
		XFillRectangle(dpy, w, wgc, x, y, circsize, circsize);
		}
	ws->x = x;
	ws->y = y;
}


round(x)
	float x;
{
	return ((int) rint((double)x));
}

drawseg(x, y, c)
	int	x, y;
	unsigned long	c;
{
	XSetForeground(dpy, gc, c);
	XFillRectangle(dpy, w, gc, x, y, circsize, circsize);
}

usage(cmd)
	char *cmd;
{
	(void)fprintf(stderr, 
"usage: %s -S -C -R [-l length] [-s size] [-n number]\n[-g geometry] [-d display]\n -S screensaver\n -C chromocolor\n -R Rotate colormap\n",
		       cmd);
	exit(1);
}

cmap_init(win)
	Window	win;
{
int	pixels[NUM_COLORS];
int	pmasks;
register int	i;
XVisualInfo	vinfo;
int	num_vis, vmask;
XColor	stat_colors[NUM_COLORS];
int	planes;

#define	VALUES	256
#define	rnd(x)	(random() % x)

/* #define	RANDOM		/* use an random colormap - messy */
#define	HSB		/* use an HSB colormap - makes colorwheel look neat */

	planes = DisplayPlanes(dpy, screen);
	/* see what kind of visual we're dealing with */
	if (XMatchVisualInfo(dpy, screen, planes, PseudoColor, &vinfo)
		|| XMatchVisualInfo(dpy, screen, planes, GrayScale, &vinfo)
		|| XMatchVisualInfo(dpy, screen, planes, DirectColor, &vinfo))
	    goto read_write_map;
	else
	    goto read_only_map;

read_write_map:
	visual = vinfo.visual;
	depth = vinfo.depth;
	cmap = XCreateColormap(dpy, RootWindow(dpy, screen), visual, AllocNone);
	ncolors = vinfo.colormap_size;
	/* grab as many color cells as we can */
	for (i = ncolors; i > MIN_COLORS; i--)	{
		if (XAllocColorCells(dpy, cmap, False, &pmasks, 0, pixels, i))
			break;
		}
	if (i == MIN_COLORS)	{
		fprintf(stderr,
		    "Couldn't allocate even %d colors - exiting\n", MIN_COLORS);
		exit(0);
		}
	ncolors = i;
#ifdef RANDOM
	/* make the black for background */
	backpixel = colors[0].pixel = pixels[0];
	colors[0].red = colors[0].green = colors[0].blue = 0;
	colors[0].flags = DoRed | DoGreen | DoBlue;
	for (i = 1; i < ncolors; i++)	{
		colors[i].pixel = pixels[i];
		colors[i].red = rnd(VALUES) << 8;
		colors[i].green = rnd(VALUES) << 8;
		colors[i].blue = rnd(VALUES) << 8;
		colors[i].flags = DoRed | DoGreen | DoBlue;
		}
	XStoreColors(dpy, cmap, colors, ncolors);
#else
#ifdef HSB
/* this colormap makes things look a lot nicer when worms goes
 * into freeze mode.
 */
	{
	double	hue, sat, bright, r, g, b;

	sat = 0.9;
	bright = 1.0;

	/* make the black for background */
	backpixel = colors[0].pixel = pixels[0];
	colors[0].red = colors[0].green = colors[0].blue = 0;
	colors[0].flags = DoRed | DoGreen | DoBlue;
	for (i = 1; i < ncolors; i++)	{
		hue = (float) i / (float) ncolors;
		hsb2rgb(hue, sat, bright, &r, &g, &b);
		colors[i].pixel = pixels[i];
		colors[i].red = (int) (r * 255.0) << 8;
		colors[i].green = (int) (g * 255.0) << 8;
		colors[i].blue = (int) (b * 255.0) << 8;
		colors[i].flags = DoRed | DoGreen | DoBlue;
		}
	XStoreColors(dpy, cmap, colors, ncolors);
	}
#else
	for (i = 0; i < ncolors; i++)
		stat_colors[i].pixel = i;
	XQueryColors(dpy, DefaultColormap(dpy, screen), stat_colors, ncolors);
	XStoreColors(dpy, cmap, stat_colors, ncolors);
	bcopy(stat_colors, colors, ncolors * sizeof(XColor));
#endif
#endif
	is_dynamic = True;

	return;

read_only_map:

	is_dynamic = False;
	visual = vinfo.visual;
	depth = vinfo.depth;
	/* for a Static colormap, just make each worm a random pixel */
	for (i = 0; i < NUM_COLORS; i++)	{
		colors[i].pixel = rnd(NUM_COLORS);
		}
}

hsb2rgb(h, s, i, r, g, b)
double	h, s, i;
double	*r, *g, *b;
{
register int	j;
register double	f, p, q, t;

	if (s == 0.0)
		*r = *g = *b = i;
	else	{
		h -= floor(h);	/* remove anything over 1 */
		h *= 6.0;
		j = floor(h);
		f = h - (float) j;
		p = i * (1.0 - s);
		q = i * (1.0 - s * f);
		t = i * (1.0 - (s * (1.0 - f)));

		switch(j)	{
			case	0:
				*r = i;
				*g = t;
				*b = p;
				break;
			case	1:
				*r = q;
				*g = i;
				*b = p;
				break;
			case	2:
				*r = p;
				*g = i;
				*b = t;
				break;
			case	3:
				*r = p;
				*g = q;
				*b = i;
				break;
			case	4:
				*r = t;
				*g = p;
				*b = i;
				break;
			case	5:
				*r = i;
				*g = p;
				*b = q;
				break;
			}
		}
}
