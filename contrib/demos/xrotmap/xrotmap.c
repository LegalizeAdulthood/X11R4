/*
 * xrotmap - rotate X colormap
 *
 * installs (and keeps installed) its own colormap which it continously
 * rotates.  current colormap is an HSB model, which rotates very cleanly.
 *
 * Dave Lemke
 * lemke@sun.com
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

Display	*dpy;
Window	dummy;
int	screen;
Colormap	cmap;
XColor	colors[NUM_COLORS];
Visual	*visual;
int	ncolors;
int	depth;
char	*cmd;

extern	char	*getenv();

main(argc, argv)
	int argc;
	char **argv;
{
	char	*display = NULL;
	register int i;
	XSetWindowAttributes	xswat;
	int	vmask;

	cmd = argv[0];
	for (i = 1; i < argc; i++)	{
		if (strncmp(argv[i], "-d", 2) == 0)	{
			if (argv[++i])
				display = argv[i];
			else
				usage(cmd);
			}
		else
			usage(cmd);
		}

	if ((dpy = XOpenDisplay(display)) == NULL)	{
		fprintf(stderr,"Can\'t open %s\n", 
			(display ? display : getenv("DISPLAY")));
		exit(0);
		}
	screen = DefaultScreen(dpy);
	ncolors = NUM_COLORS;

	/* set up the color map */
	if (DisplayCells(dpy, screen) > 2)
		cmap_init();
	else	{
		fprintf(stderr,"%s doesn't make sense on a monchrome\n", cmd);
		exit(0);
		}

	vmask = CWColormap;
	xswat.colormap = cmap;
	/* make dummy window to get colormap events with */
	dummy = XCreateWindow(dpy, RootWindow(dpy, screen), 0, 0, 10, 10, 0, depth,
		InputOutput, visual, vmask, &xswat);

	XInstallColormap(dpy, cmap);
	XSelectInput(dpy, dummy, ColormapChangeMask);

	while (1)	{
		XEvent	ev;

		if (XPending(dpy))	{
			XNextEvent(dpy, &ev);

			/* if anyone uninstalls our map, stuff it
			 * back in */
			if(ev.type == ColormapNotify)	{
				XColormapEvent	*e = (XColormapEvent *)&ev;

				if (e->state == ColormapUninstalled)
					XInstallColormap(dpy, cmap);
				}
			}

		/* rotate the colormap */
		rotate_colors();
		}
}

/*
 * rotates colormap
 */

rotate_colors()
{
register int	i;
long	temp;

	temp = colors[0].pixel;
	for (i = 0; i < ncolors - 1; i++)	{
		colors[i].pixel = colors[i+1].pixel;
		}
	colors[ncolors - 1].pixel = temp;
	XStoreColors(dpy, cmap, colors, ncolors);
}

usage(cmd)
	char *cmd;
{
	(void)fprintf(stderr, "usage: %s [-d display]\n", cmd);
	exit(1);
}

/*
 * initializes colormap
 *
 * finds dynamic visual and creates a new colormap, then fills it
 * with an HSB ramp
 */
cmap_init()
{
int	pixels[NUM_COLORS];
int	pmasks;
register int	i;
XVisualInfo	vinfo;
int	num_vis, vmask;
int	planes;

	planes = DisplayPlanes(dpy, screen);
	/* see what kind of visual we're dealing with */
	if (!(XMatchVisualInfo(dpy, screen, planes, PseudoColor, &vinfo)
		|| XMatchVisualInfo(dpy, screen, planes, GrayScale, &vinfo)
		|| XMatchVisualInfo(dpy, screen, planes, DirectColor, &vinfo)))  {
		fprintf(stderr,"%s doesn't make sense on a monchrome\n", cmd);
		exit(0);
		}

read_write_map:
	visual = vinfo.visual;
	depth = vinfo.depth;
	cmap = XCreateColormap(dpy, RootWindow(dpy, screen), visual, AllocNone);
	ncolors = vinfo.colormap_size;
	if (XAllocColorCells(dpy, cmap, False, &pmasks, 0, pixels, 
	    ncolors) == 0)	{
		fprintf(stderr, "Couldn't allocate all colors - exiting\n");
		exit(0);
		}
	{
	double	hue, sat, bright, r, g, b;

	sat = 0.9;
	bright = 1.0;

	/* fill cmap with HSB ramp */
	for (i = 0; i < ncolors; i++)	{
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
}

/*
 * converts an HSB triplet to an RGB triplet
 */
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
