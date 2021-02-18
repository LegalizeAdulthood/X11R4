/* $Header: clover.c,v 1.2 89/11/28 11:18:17 jim Exp $ */

/* 
 * clover.c - Gosper's LispM "smoking clover" color table hack
 * 
 * Author:	Christopher A. Kent
 * 		Western Research Laboratory
 * 		Digital Equipment Corporation
 * Date:	Wed Feb 22 1989
 */

/*
 * $Log:	clover.c,v $
 * Revision 1.2  89/11/28  11:18:17  jim
 * new version from Chris
 * 
 * Revision 1.4  89/11/27  11:32:56  kent
 * Fixed some problems with -colors.
 * 
 * Revision 1.3  89/09/20  19:20:09  kent
 * Changes to use the default colormap (to avoid technicolor) when possible.
 * 
 * Revision 1.2  89/03/20  09:33:27  kent
 * Fixed a problem where a small integer shadowed the Visual *. Not a problem
 * on the VAX, apparently, but was on the Sun. Maybe this is why it
 * didn't work on the PMAX!
 * 
 * Also added an #ifdef to install the colormap for broken window managers.
 * 
 * Revision 1.1  89/03/20  09:26:31  kent
 * Initial revision
 * 
 */

static char rcs_ident[] = "$Header: clover.c,v 1.2 89/11/28 11:18:17 jim Exp $";

#define	volatile

#include <stdio.h>

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>

#define	NCOLORS	16
#define	R	5432
#define S	4321

#define	MIN(x, y)	((x) < (y))?(x):(y)
#define	MAX(x, y)	((x) > (y))?(x):(y)
#define	CEIL(a, b)	((a)+(b)-1)/(b)
#define	FLOOR(a, b)	CEIL((a)-(b), (b))

Display		*dpy;			/* the display */
Window		w;			/* the window */
int		screen;			/* the screen to use */
Colormap	colorMap;		/* private Colormap resource */
unsigned long	basePixel;		/* where the first color is */
XColor		*colors;		/* color sets */
int		cmap = 0;		/* which color set to use now */
Visual		*v;			/* the appropriate visual */
int		nColors;		/* how many colors (per v) */
Pixel		fg_pixel;		/* specified fg color (ignored) */
Pixel		bg_pixel;		/* specified bg color */
Pixel		borderColor;		/* ... border color */
Cardinal	borderWidth;		/* ... border width */
String		geometry;		/* ... geometry */
int		posX, posY;		/* ... position */
int		width, height;		/* ... sizes */
int		one = 1;
int		radius = R;		/* ... 'radius' of clover */
int		speed = S;		/* ... initial speed */
int		maxColors = NCOLORS;	/* ... maximum colors to use */

char		*malloc();

#define	XtNgeometry	"geometry"
#define	XtCGeometry	"Geometry"
#define	XtNcolors	"colors"
#define	XtCColors	"Colors"
#define	XtNradius	"radius"
#define	XtCRadius	"Radius"
#define	XtNspeed	"speed"
#define	XtCSpeed	"Speed"

static XrmOptionDescRec opTable[] = {
{"-geometry",   "*geometry",	XrmoptionSepArg,  (caddr_t) NULL},
{"-colors",	"*colors",	XrmoptionSepArg,  (caddr_t) NULL}, 
{"-radius",	"*radius",	XrmoptionSepArg,  (caddr_t) NULL},
{"-speed",	"*speed",	XrmoptionSepArg,  (caddr_t) NULL}, 
};

static XtResource resources[] = {	/* don't really need some of these */
        {XtNforeground,  XtCForeground,  XtRPixel,  sizeof(Pixel), 
         (Cardinal)&fg_pixel,  XtRString,  "black"}, 
        {XtNbackground,  XtCBackground,  XtRPixel,  sizeof(Pixel), 
         (Cardinal)&bg_pixel,  XtRString,  "white"}, 
        {XtNborderWidth,  XtCBorderWidth,  XtRInt,  sizeof(int), 
         (Cardinal)&borderWidth,  XtRInt,  (caddr_t) &one}, 
        {XtNborder,  XtCBorderColor,  XtRPixel,  sizeof(Pixel), 
         (Cardinal)&borderColor,  XtRString,  "black"},
        {XtNgeometry,  XtCGeometry,  XtRString,  sizeof(char *), 
         (Cardinal)&geometry,  XtRString,  (caddr_t) NULL}, 
        {XtNcolors,  XtCColors,  XtRInt,  sizeof(int), 
         (Cardinal)&maxColors,  XtRInt,  (caddr_t) &maxColors}, 
        {XtNradius,  XtCRadius,  XtRInt,  sizeof(int), 
         (Cardinal)&radius,  XtRInt,  (caddr_t) &radius}, 
        {XtNspeed,  XtCSpeed,  XtRInt,  sizeof(int), 
         (Cardinal)&speed,  XtRInt,  (caddr_t) &speed}, 
};

main(argc, argv)
char	**argv;
{
	Widget		widg;
	XEvent		xev;
	XSizeHints	hint;
	char		text[10];
	int		i;
	KeySym		key;
	Pixmap		p, cloverPixmap();
	XWindowAttributes	attr;
	Bool		mapped = False;
	Cursor		spiral;
	int		cycleDelay, cycleDecrement = 0, currentSpeed;

	/*
	 * We cheat here by using the Toolkit to do the initialization work.
	 * We just ignore the top-level widget that gets created.
	 */

	widg = XtInitialize("clover", "clover", opTable, XtNumber(opTable), 
			&argc, argv);
	dpy = XtDisplay(widg);
	screen = DefaultScreen(dpy);

	XtGetApplicationResources(widg, (caddr_t) NULL, resources, 
				XtNumber(resources), NULL, (Cardinal) 0);

	posX = posY = -1;
	width = height = 0;
	if (geometry) {
		int	mask, gx, gy, gw, gh;

		mask = XParseGeometry(geometry, &gx, &gy, &gw, &gh);
		if (mask & WidthValue)
			width = gw;
		if (mask & HeightValue)
			height = gh;
		if (mask & XValue)
			if (mask & XNegative)
				posX = DisplayWidth(dpy, screen) - 
					width + posX;
			else
				posX = gx;
		if (mask & YValue)
			if (mask & YNegative)
				posY = DisplayHeight(dpy, screen) -
					height + posY;
			else
				posY = gy;
	}

	hint.width = width = width ? width : DisplayWidth(dpy, screen);
	hint.height = height = height ? height : DisplayHeight(dpy, screen);
	hint.x = posX >= 0 ? posX : (DisplayWidth(dpy, screen) - width)/2;
	hint.y = posY >= 0 ? posY : (DisplayHeight(dpy, screen) - height)/2;
	hint.flags = PPosition | PSize;

	w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 
			hint.x, hint.y, 
			hint.width, hint.height, 
			borderWidth, 
			BlackPixel(dpy, screen), 
			WhitePixel(dpy, screen));
	XSetStandardProperties(dpy, w, "Smoking Clover", "clover", None, 
				argv, argc, &hint);
	XSelectInput(dpy, w, 
		ButtonPressMask | ExposureMask | 
		KeyPressMask | StructureNotifyMask);

	buildColormaps();

	p = cloverPixmap(radius);
	
	spiral = XCreateFontCursor(dpy, XC_box_spiral);
	
	XSynchronize(dpy, True);
#ifdef	BOGUS_WM
	XInstallColorMap(dpy, colorMap);
#endif	BOGUS_WM
	XMapRaised(dpy, w);

	cycleDelay = speed;
	currentSpeed = speed;
	while(1) {
		if (mapped && !XPending(dpy)) {
			if (--cycleDelay <= 0) {
				cycle();
				cycleDelay = currentSpeed;
				if (++cycleDecrement%16 == 0)
					currentSpeed >>= 1;
			}
			continue;
		}

		XNextEvent(dpy, &xev);
		switch(xev.type) {
/*		case Expose:*/
		case MapNotify:
			mapped = True;
			break;

		case ConfigureNotify:
			XSynchronize(dpy, True);
			XDefineCursor(dpy, w, spiral);
			XGetWindowAttributes(dpy, w, &attr);
			width = attr.width;
			height = attr.height;
			XFreePixmap(dpy, p);
			p = cloverPixmap(radius);
			XClearWindow(dpy, w);
			XUndefineCursor(dpy, w);
			XSynchronize(dpy, False);
			currentSpeed = speed;
			break;
			
		case UnmapNotify:
			mapped = False;
			break;

		case MappingNotify:
			XRefreshKeyboardMapping(&xev);
			break;

		case ButtonPress:
			currentSpeed = speed;
			break;

		case KeyPress:
			i = XLookupString(&xev, text, 10, &key, 0);
			if (i == 1 && text[0] == 'q') {
#ifdef	BOGUS_WM
				XInstallColormap(dpy, 
						DefaultColormap(dpy, screen));
#endif	BOGUS_WM
				exit(0);
			}
			break;
		}
	}
}

/*
 * Decide how many colors to use (depends on the available Visuals).  Create a
 * private Colormap and that many sets of color cells.  Load the first set with
 * a random selection of colors, and the rest so that the colors slide through
 * the pixel values.
 *
 * It might seem more straightforward to allocate nColors Colormaps, load them
 * and explicitly install them, but well-behaved clients should not install
 * Colormaps explicitly.  Rather, they should change the Colormap attribute and
 * wait for the window manager to do the installation.  This slows things down
 * a lot, so we do explict XStoreColors calls to cycle through the color sets.
 *
 */

/*
 * There are nColors sets of (nColors+1) colors each.  Slots [0..nColors-1]
 * rotate in subsequent sets; slot nColors is always the background color.
 */

#define	COLOR(a, b)	colors[((a)*(nColors+1))+(b)]

buildColormaps()
{
	int		i, j;
	XColor		*c1, *c2, tempColor, bgColor;
	Visual		*findVisual();
	Bool		allocContiguousCells();
	unsigned long	*pixels;

	v = findVisual();
	if (v == NULL) {
		printf("Sorry, clover needs a writable colormap to run\n");
		exit(0);
	}

	/*
	 * Find out the color values of the specified background...
	 */

	bgColor.pixel = bg_pixel;
	XQueryColor(dpy, DefaultColormap(dpy, screen), &bgColor);
	
	colors = (XColor *) malloc((nColors+1) * nColors * sizeof(XColor));
	pixels = (unsigned long *) malloc((nColors+1) * sizeof(unsigned long));

	colorMap = DefaultColormap(dpy, screen);
	if (allocContiguousCells(dpy, colorMap, pixels, nColors+1))
		basePixel = *pixels;
	else {
		colorMap = XCreateColormap(dpy, w, v, AllocAll);
		basePixel = 0;
	}
	free(pixels);

	/*
	 * allocate random colors into first color map, with bg at the end
	 */
	
	for (i = 0; i < nColors; i++) {
		c1 = &COLOR(0, i);
		c1->pixel = i + basePixel;
		if (i == 0) {
			c1->red = 0;
			c1->green = (unsigned short) -65535/6;
			c1->blue = 65535/6;
		} else {
			c2 = &COLOR(0, i-1);
			c1->red = c2->red - random()/6;
			c1->green = c2->green - random()/6;
			c1->blue = c2->blue - random()/6;
		}
		c1->flags = DoRed | DoGreen | DoBlue;

		for (j = 1; j < nColors; j++)
			COLOR(j, i).pixel = i + basePixel;
	}
	c1 = &COLOR(0, nColors);
	c1->pixel = basePixel + nColors;
	c1->red = bgColor.red;
	c1->green = bgColor.green;
	c1->blue = bgColor.blue;
	c1->flags = DoRed | DoGreen | DoBlue;
	XStoreColors(dpy, colorMap, colors, nColors+1);


	/* rotate colors (but not pixels) through other colors */
	for (i = 1; i < nColors; i++) {
		tempColor = COLOR(i-1, 0);
		for (j = 0; j < nColors-1; j++) {
			c1 = &COLOR(i, j);
			c2 = &COLOR(i-1, j+1);
			c1->red = c2->red;
			c1->green = c2->green;
			c1->blue = c2->blue;
			c1->flags = c2->flags;
		}
		c1 = &COLOR(i, nColors-1);
		c2 = &tempColor;
		c1->red = c2->red;
		c1->green = c2->green;
		c1->blue = c2->blue;
		c1->flags = c2->flags;

		COLOR(i, nColors) = COLOR(i-1, nColors);
	}
	i = i;
}

Bool
allocContiguousCells(dpy, cmap, pixels, npixels)
Display *dpy;
Colormap cmap;
unsigned long *pixels;		/* filled in by routine */
int npixels;
{
    unsigned long *waste = (unsigned long *)NULL;
    int nwaste = 0;
    Bool contig = False;
    int status, i;
    unsigned long masks = NULL;

    while (!contig) {
        status = XAllocColorCells(dpy, cmap, False, &masks, 0, pixels,
		npixels);
        if (!status)
	    break;   /* can't get enough contiguous cells */
        for (i=0; i < (npixels-1); i++) {
            if (pixels[i] + 1 != pixels[i+1]) {
		/* isn't contiguous, keep trying */
                XFreeColors(dpy, cmap, &pixels[i+1], npixels - (i+1), 0);
		if (!waste) 
		    waste = (unsigned long *)malloc((i + 1) *
				sizeof(unsigned long));
		else
		    waste = (unsigned long *)realloc(waste, (nwaste +
				(i + 1)) * sizeof(unsigned long));
		bcopy(pixels, waste+nwaste, (i+1) * sizeof(unsigned long));
		nwaste += (i+1);
		break;
            }
        }
        if (i == (npixels-1))
	    contig = True;
    }
    /* clean up and return 'contig' */
    if (nwaste) {
        XFreeColors(dpy, cmap, waste, nwaste, 0);
        free(waste);
    }
    return (contig);
}

/*
 * Create the image in memory. Assumes several things have already been set up
 * in global variables.
 */

char	*bits;
int	trace = 0;
#define	getPixel(x, y)		bits[(y)*width+(x)]
#define	putPixel(x, y, v)	bits[(y)*width+(x)] = (v)%nColors;
#define	putRawPixel(x, y, v)	bits[(y)*width+(x)] = (v);

Pixmap
cloverPixmap(r)
{
	XImage	*im, *simpleImage(), *cloverImage();
	Pixmap	p;
	XSetWindowAttributes	attr;

	im = cloverImage(r);
	p = XCreatePixmap(dpy, w, width, height, XDefaultDepth(dpy, screen));
	XPutImage(dpy, p, DefaultGC(dpy, screen), im,
					0, 0, 0, 0, width, height);
	XDestroyImage(im);

	attr.background_pixmap = p;
	attr.colormap = colorMap;
	attr.bit_gravity = CenterGravity;
	XChangeWindowAttributes(dpy, w, 
			CWBackPixmap | CWColormap | CWBitGravity,
			&attr);

	return p;
}

XImage *
simpleImage()
{
	int	i, j;
	XImage	*im;

	bits = malloc(width * height);

	for (i = 0; i < height; i++)
		for (j = 0; j< width; j++)
			bits[i * width + j] = i % nColors;

	im = XCreateImage(dpy, v, 
			XDefaultDepth(dpy, screen), 
			ZPixmap, 0, bits, width, height, 0, width);
	free(bits);
	return im;
}

/*
 * Basically the algorithm is to draw a series of Bresenham lines from the
 * center.  The "interference pattern" is built by incrementing the pixel value
 * of (x,y) every time it's touched; the resulting pattern is a product of the
 * vagaries of integer arithmetic.
 */

XImage *
cloverImage(r)
{
	XImage	*im;
	int	maxX, maxY, midX, midY, x, f, y;
	int	val, yy, x1, y1;
	int	i, o;
	char	*b;

	bits = malloc(width * height);
	if (bits == NULL) {
		perror("No memory");
		exit(-1);
	}

	maxX = width - 1;
	maxY = height - 1;
	midX = maxX / 2;
	midY = maxY / 2;

	for (y = 0; y < height; y++) {
		b = &bits[y*width];
		for (x = 0; x < width; x++)
			*b++ = nColors;		/* fill in background */
	}

	/*
	 * Fill in the first semi-quadrant.
	 */

	x = r;
	f = 0;
	for (y = 0; y < x; y++) {
		if (f > x) {
			x--;
			f = f-x - (x-1);
		}
		clipLine(midX, midY, x+midX, y+midY, 0, 0, maxX, maxY);
		f = f+y + y+1;
	}

	/*
	 * Copy to the other seven, adjusting the horizontal and diagonal.
	 */

	for (x = midX; x < maxX; x++) {
/*		putPixel(x, midY, (getPixel(x, midY) << 1) - 1);*/
		if (x - midX + midY <= maxY)
			putPixel(x, x-midX+midY, 
				(getPixel(x, x-midX+midY) << 1) - 1);
		yy = MIN(maxY, x + midY - midX);
		for (y = midY; y <= yy; y++) {
			val = getPixel(x, y);
			x1 = x;
			y1 = y;
			for (i = 0; i < 4; i++) {
				if ((y1 < maxY) && (y1 > 0)) {
					putPixel(midX + midX - x1, y1, val);
					putPixel(x1, y1, val);
				}
				o = x1;
				x1 = midX + midY - y1;
				y1 = midY + o - midX;
			}
		}
	}

	if (basePixel != 0) 	/* bias by the first pixel value */
		for (y = 0; y < maxY; y++)
			for (x = 0; x < maxX; x++)
				putRawPixel(x, y, getPixel(x, y) + basePixel);
				
	im = XCreateImage(dpy, v, 
			XDefaultDepth(dpy, screen), 
			ZPixmap, 0, bits, width, height, 0, width);
	free(bits);
	return im;
}

/*
 * (xe, ye) and (xf, yf) are the corners of a rectangle to clip a line to.
 * (x0, y0) and (xn, yn) are the endpoints of the line to clip.
 * The function argument that's being computed is the semi-quadrant;
 *  dx and dy are used to determine whether we're above or below the diagonal,
 *  since (x0, y0) is always the midpoint of the pattern.
 * (The LispM has the origin at lower left, instead of upper left, so
 * the numbers don't correspond to the normal Cartesian plane quadrants.)
 *
 * This routine is very general, but the calling code only builds lines in the
 * first semi-quadrant and then copies them everywhere else.
 */

clipLine(x0, y0, xn, yn, xe, ye, xf, yf)
{
	int	dx, dy;

	dx = abs(xn - x0);
	dy = abs(yn - y0);

	if (xn > x0) {				/* moving right */
		if (yn >= y0) {			/* moving up */
			if (dx > dy)		/* below diagonal */
				line(0, x0, y0, dx, dy, xe, ye, xf, yf);
			else
				line(1, y0, x0, dy, dx, ye, xe, yf, xf);
		} else {
			if (dx > dy)
				line(7, x0, -y0, dx, dy, xe, -yf, xf, -ye);
			else
				line(6, -y0, x0, dy, dx, -yf, xe, -ye, xf);
		}
	} else {
		if (yn >= y0) {
			if (dx > dy)
				line(3, -x0, y0, dx, dy, -xf, ye, -xe, yf);
			else
				line(2, y0, -x0, dy, dx, ye, -xf, yf, -xe);
		} else {
			if (dx > dy)
				line(4, -x0, -y0, dx, dy, -xf, -yf, -xe, -ye);
			else
				line(5, -y0, -x0, dy, dx, -yf, -xf, -ye, -xe);
		}
	}
}

#define	plot(x, y)	putPixel((x), (y), getPixel((x), (y))+1)

/*
 * Clip symmetric segment (x0, y0) thru (xn, yn) to the rectangle 
 * (xe, ye) < (xf, yf).
 *
 * The original says:
 *
 * "This routine incorrectly assumes that the subsegment starts prior to the
 * midpoint of the supersegment.  The 'divide for nearest integer' (i.e.,
 * divide for remainder of minimum magnitude), which is simulated by the FLOOR
 * and CEIL of num and (dx <<1), always rounds up on the half integer case, but
 * should round down (for symmetry) if startup is in 2nd half. It would be
 * nice to have these other flavors of divide.'
 */

line(fun, x0, y0, dx, dy, xe, ye, xf, yf)
{
	int	x, num, lx;
	int	xx, y, x00, f;
	int	x11;

	x = MAX(x0,  MAX(xe, 
			(dy == 0)? xe :
				   x0 + CEIL(dx * (((ye - y0)<<1) - 1), 
						(dy << 1))));
	num = dx + 2*dy*(x - x0);
	lx = MIN(xf, (dy == 0) ? xf :
				x0 + CEIL(dx * (((yf - y0)<<1) - 1), 
						(dy << 1)));
	xx = MIN(lx, x0 + (dx>>1));
	y = y0 + FLOOR(num, (dx<<1));
	f = (FLOOR(num, (dx<<1)) - dx) >> 1;

	for (x00 = x; x00 < xx; x00++,f+=dy) {
		if (f+f > dx) {
			f -= dx;
			y++;
		}
		switch(fun) {
		case 0:	plot(x00, y);	break;
		case 1:	plot(y, x00);	break;
		case 2:	plot(-y, x00);	break;
		case 3:	plot(-x00, y);	break;
		case 4:	plot(-x00, -y);	break;
		case 5:	plot(-y, -x00);	break;
		case 6:	plot(y, -x00);	break;
		case 7:	plot(x00, -y);	break;
		}
	}

	for (x11 = x00; x11 < lx; x11++, f+=dy) {
		if (f + f > dx) {
			f -= dx;
			y++;
		}
		switch(fun) {
		case 0:	plot(x11, y);	break;
		case 1:	plot(y, x11);	break;
		case 2:	plot(-y, x11);	break;
		case 3:	plot(-x11, y);	break;
		case 4:	plot(-x11, -y);	break;
		case 5:	plot(-y, -x11);	break;
		case 6:	plot(y, -x11);	break;
		case 7:	plot(x11, -y);	break;
		}
	}
}

/*
 * Install the next set of colors in the cycle.
 */

cycle()
{
	cmap = ++cmap % nColors;
	XStoreColors(dpy, colorMap, &colors[cmap * (nColors+1)], nColors+1);
}

/*
 * Find an appropriate visual (and set the screen and nColors as a side effect)
 * to run on.
 */

int	classes[] = {
	PseudoColor,
	DirectColor,
	GrayScale, 
	0
};

Visual *
findVisual()
{
	int		howMany, i, max, *class;
	XVisualInfo	*vip, vTemplate;

	for (class = classes; *class; class++) {
		vTemplate.class = *class;
		vip = XGetVisualInfo(dpy, VisualClassMask, 
					&vTemplate, &howMany);
		if (vip) {
			max = 0;
			for (i = 0; i < howMany; i++) {
				if (vip->colormap_size > max)
					v = vip->visual;
					max = vip->colormap_size;
			}
			screen = vip->screen;
			nColors = MIN(maxColors, vip->colormap_size);
			if (nColors == vip->colormap_size)
			    nColors--;	/* leave room for background */
			return v;
		}
	}
	return NULL;
}
