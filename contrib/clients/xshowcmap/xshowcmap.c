/*
 * xshowcmap - display the default colormap
 *
 * Copyright 1988 Siemens
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Siemens not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Siemens makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Author:  Claus Gittinger, Siemens Munich, unido!sinix!claus@uunet.uu.net
 */

#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>

Display     *dpy;
int         screen;
int         ncells, nhoriz, nvert;
GC          gc;

#define NMAXCOLOR	256
#define SQWIDTH		32
#define SQHEIGHT	32
#define BDWIDTH		1	/* separating grid */

int sqWidth = SQWIDTH;
int sqHeight = SQHEIGHT;
int mainWidth = 0;
int mainHeight = 0;

Window      rootWindow, mainWindow;

#include "icon.bit"

main(argc,argv)
int argc ;
char *argv[] ;
{
	int i;
	Visual visual;
	XGCValues xgcv;
	XSetWindowAttributes xswa;
	XSizeHints sizehints;
	char *ProgName, *server, *geom, *borderColor;
	int borderWidth = 1;
	extern double sqrt();
	int h, v, color;
        long BdPixel;

	server = "";
	geom   = "";
	borderColor = "black";

	ProgName = argv[0];

	/********************************/
	/** parse command line options **/
	/********************************/

	for (i=1; i<argc; i++) {
	    if (argv[i][0] == '=')
		geom = argv[i];
	    else if (index(argv[i],':') != NULL)
		server = argv[i];
	    else if (strcmp(argv[i], "-bd") == 0)
		borderColor = argv[++i];
	    else if (strcmp(argv[i], "-bw") == 0)
		sscanf(argv[++i], "%d", &borderWidth);
	    else
		usage(ProgName);
	}

	dpy = XOpenDisplay(server);
	if (dpy == NULL) {
	    fprintf(stderr, "can't open display \"%s\"\n",server);
	    exit(1);
	}
	screen = DefaultScreen(dpy);
        ncells = DisplayCells(dpy, screen);

	/*
	 * actually this is not needed;
	 * but had problems sometimes with bad rgb-data bases
	 * to make xshowcmap run also in this case ...
	 */
	if (strcmp(borderColor, "black") == 0)
	    BdPixel = BlackPixel(dpy, screen);
	else
	    BdPixel = requestColor(borderColor);

	/*************************************/
	/** configure the window size hints **/
	/*************************************/

	nhoriz = (int)sqrt((double)ncells);
	nvert = nhoriz;
	while (nhoriz * nvert < ncells)
	    nvert++;

	sizehints.flags = PMinSize | PPosition | PSize;
	sizehints.min_width = sqWidth;
	sizehints.min_height = sqHeight;
	sizehints.width = nhoriz*sqWidth + (nhoriz-1)*BDWIDTH;
	sizehints.height = nvert*sqHeight + (nvert-1)*BDWIDTH;
	sizehints.x = 100;
	sizehints.y = 300;

	if (geom && strlen(geom)) {
	    int width, height, flags;

	    flags = XParseGeometry(geom, &sizehints.x, &sizehints.y,
					 &width, &height);
	    if (flags & (XValue | YValue))
		sizehints.flags |= USPosition;
	}

	/*******************************************************************/
	/** create the palette main window and set its standard properties **/
	/*******************************************************************/

	xswa.event_mask = KeyPressMask | StructureNotifyMask;
	xswa.event_mask |= ExposureMask;

	xswa.background_pixel = BdPixel;
	xswa.border_pixel = BdPixel;

	visual.visualid = CopyFromParent;

	rootWindow = RootWindow(dpy, screen);

	mainWindow = XCreateWindow(dpy, rootWindow,
			       sizehints.x, sizehints.y,
			       sizehints.width, sizehints.height,
			       borderWidth,
			       DefaultDepth(dpy,screen),
			       InputOutput,
			       &visual,
			       CWEventMask | CWBackPixel | CWBorderPixel,
			       &xswa);

	/*******************************************************************/
	/** define its name and icon-bitmap                               **/
	/*******************************************************************/

	XSetStandardProperties(dpy, mainWindow, "colors","Colors",
			       XCreateBitmapFromData(dpy, rootWindow,
						     icon_bits,
						     icon_width, icon_height),
			       argv, argc, &sizehints);

        gc = XCreateGC(dpy, mainWindow, 0L, NULL);

	XMapWindow(dpy, mainWindow);

	eventloop();
	exit(0) ;
}

usage(name)
char *name;
{
        fprintf(stderr, "usage: %s [geometry] [display]\n", name);
        fprintf(stderr, "          [-bd color] [-bw number]\n");
        exit(1);
}

requestColor(name)
char *name;
{
	XColor truecolor, availcolor;

	if (XAllocNamedColor(dpy,
			     DefaultColormap(dpy, DefaultScreen(dpy)),
			     name, &availcolor, &truecolor) == 0) {
	    fprintf(stderr, "Color '%s' unknown\n", name);
	    exit(1);
	}
	if (truecolor.red != availcolor.red ||
		truecolor.green != availcolor.green ||
		truecolor.blue != availcolor.blue) {
	    fprintf(stderr, "Warning: %s color bay be wrong\n", name);
	}

	return availcolor.pixel;
}

eventloop() {
	XEvent event;
	XExposeEvent *ee = (XExposeEvent *)&event;
	XKeyPressedEvent *ke = (XKeyPressedEvent *)&event;
	XButtonPressedEvent *be = (XButtonPressedEvent *)&event;
        XConfigureEvent *ce = (XConfigureEvent *)&event;
	char keybuff[10];
	int nchar;
	int color, h, v;

	while(1) {
	    XNextEvent(dpy, &event);
	    switch(event.type) {
		case KeyPress:
		    nchar = XLookupString(ke, keybuff, 1, NULL, NULL);

		    if (nchar > 0) {
			switch (keybuff[0]) {
			    case 'q':
			    case 'Q':
				exit(0);
			}
		    }
		    break;

                case ConfigureNotify:
		    if (ce->width == mainWidth && ce->height == mainHeight)
			break;
		    sqWidth = ce->width / nhoriz;
		    sqHeight = ce->height / nvert;
		    color = 0;
		    for (v=0; v<nvert; v++)
	    		for (h=0; h<nhoriz; h++, color++) {
			    XSetForeground(dpy, gc, color);
			    XFillRectangle(dpy, mainWindow, gc, 
					  h*(sqWidth+1), v*(sqHeight+1), sqWidth, sqHeight);
		    }
                    break;

		case Expose:
		    if (ee->count)
			break;

		    color = 0;
		    for (v=0; v<nvert; v++)
	    		for (h=0; h<nhoriz; h++, color++) {
			    XSetForeground(dpy, gc, color);
			    XFillRectangle(dpy, mainWindow, gc, 
					  h*(sqWidth+1), v*(sqHeight+1), sqWidth, sqHeight);
		        }
		    break;

		default:
		    break;
	    }
	}
}
