/*
 * Support drawing routines for Chris Torek's DVI->ImPress program.
 *
 * Requires v1.7 or later ImPress to handle paths.
 * Better if v1.9 or later for arc, circle, and ellipse primitives
 * (define USEGRAPHICS).
 *
 *	Tim Morgan, UC Irvine, 11/17/85
 *
 *
 * At the time these routines are called, the position of the Imagen should
 * have been updated to the upper left corner of the graph (the position
 * the \special appears at in the dvi file).  Then the coordinates in the
 * graphics commands are in terms of a virtual page with axes oriented the
 * same as the Imagen normally has:
 *
 *			0,0
 *			 +-----------> +x
 *			 |
 *			 |
 *			 |
 *			\ /
 *			 +y
 *
 * Angles are measured in the conventional way, from +x towards +y.
 * Unfortunately, that reverses the meaning of "counterclockwise" from
 * what you see in the output.
 *
 * Unfortunately, some 8/300's don't have an aspect ratio which is 1:1.
 * One of ours appears to be 295 dpi in the horizontal direction and 300dpi
 * vertically.  If ASPECT is defined, then dviimp/imagen1 and draw_imp/special
 * use two different variables for the horizontal and vertical resolution, and
 * otherwise, just one.  Because the drawing routines which are defined
 * in ImPress for circles, arcs, and ellipses in V1.9 and later assume that
 * the output device is 1:1, they can't be used if ASPECT is defined, and
 * because I don't want to hack up imagen1 to understand different horizontal
 * and vertical resolutions, we're currently ignoring this problem, and not
 * defining ASPECT.
 */

#define	USEGRAPHICS		/* Only if v1.9 or later imPRESS */
#undef	ASPECT

#ifdef	ASPECT			/* Can't have both! */
#undef	USEGRAPHICS
#endif

#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include "libtex/imPcodes.h"

extern char *malloc();

/*
 *	EXTERNS
 */

extern	int NextFamilyNumber;	/* Number of next ImPress family to use */
extern char *ProgName;

#ifndef	ASPECT
    extern	int	dviDPI;		/* Resolution of device */
#else
    extern	int	DPIx, DPIy;	/* x,y resolution of device */
#endif


#define	TRUE	1
#define	FALSE	0

#define	TWOPI		(3.14157926536*2.0)
#define	MAXPOINTS	600	/* Most number of points in a path */
#define	SPLINEPOINTS	5000	/* Most points in a spline */
#define	RADTOPXL	2607.435436	/* (16383 / (2pi)).  This
					converts from radians to the
					angle units used by ImPress */

/* Convert radian angles to 2**-14 angle units used by ImPress */
#define	RadToImpUnits(a)	((short) ((a)*RADTOPXL + 0.5))

/* Graphics operations */
#define	WHITE	0
#define	SHADE	3
#define	OR	7
#define	BLACK	15

#ifndef	ASPECT
#define	PixPerInX	dviDPI
#define	PixPerInY	dviDPI
#else
#define	PixPerInX	DPIx
#define	PixPerInY	DPIy
#endif

/*
 *	I don' understand the functionality of the GlobalMag & I've wasted
 *	enough tim & paper trying to get this version of conv (from UCI)
 *	to work, so we're sticking with the older one (from Maryland)
 *	until I get the newone to work
 */

#ifdef UNDEF
extern	double	GlobalMag;	/* Mag/1000.0, unrounded */
#define	conv(x, f)\
	((int) ((x) * ((double)(f)/5.0) * (GlobalMag/((double)dviDPI)) + 0.5))
#define	xconv(x)	conv(x, PixPerInX)
#define	yconv(y)	conv(y, PixPerInY)

#else

#define	xconv(x)	((int) ((x) * ( ((float) dviDPI) / 1000.0) + 0.5))
#define	yconv(y)	((int) ((y) * ( ((float) dviDPI) / 1000.0) + 0.5))

#endif

#define	fnum	NextFamilyNumber

#define ImWrt1(x)  putchar(x)
#define putword(w) (putchar ((w) >> 8), putchar (w))
#define ImWrt2(x)  putword(x)

static	int xx[MAXPOINTS], yy[MAXPOINTS], pathlen,
	pensize = 2;	/* Size we want Imagen to draw at, default 2 pixels */

#define	MAXPENSIZE 20		/* Imagen restriction */

static int
	family_defined = FALSE,	/* Have we chosen family yet? */
	texture_defined = FALSE,/* Have we done a set_texture yet? */
	whiten_next = FALSE,	/* Should next object be whitened? */
	blacken_next = FALSE,	/* Should next object be blackened? */
	shade_next = FALSE;	/* Should next object be shaded? */

/* Predefined shading (texture) glyph */
/* First, define size of glyph */
#define	THEIGHT	32		/* bits high */
#define	TWIDTH	4		/* bytes wide */
/* Next, declare the bit map for the glyph */
static char stexture[THEIGHT][TWIDTH]={
	{0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
	{0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
	{0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
	{0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
	{0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
	{0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
	{0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
	{0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}};

/*
 * Copy a default texture into the stexture array
 */
static void glyph_init()
{
	static char btexture[THEIGHT][TWIDTH]={
	{0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
	{0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
	{0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
	{0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
	{0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
	{0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
	{0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
	{0xcc, 0xcc, 0xcc, 0xcc}, {0x00, 0x00, 0x00, 0x00},
	{0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00}};

	int i;
	for (i=0; i<THEIGHT; i++)
		bcopy(btexture[i],stexture[i],TWIDTH);

}


/*
 * Push the state of the Imagen and set up a new virtual coord system
 */
static void push_location()
{
    ImWrt1(imP_PUSH);
    ImWrt1(imP_SET_HV_SYSTEM);
    ImWrt1(0140);
}


/*
 * Create the pushed virtual page, and pop the state of the printer
 */
static void pop_location()
{
    ImWrt1(imP_POP);
}


/*
 * Set the pen size
 * Called as \special{pn size}
 *	 eg: \special{pn 8}
 * The size is the number of milli-inches for the diameter of the pen.
 * This routine converts that value to device-dependent pixels, and makes
 * sure that the resulting value is within legal bounds.
 */
static void im_pensize(cp)
char *cp;
{
    int size;
    
    if (sscanf(cp, " %d ", &size) != 1) return;
    pensize = yconv(size);
    if (pensize < 1) pensize = 1;
    else if (pensize > MAXPENSIZE) pensize = MAXPENSIZE;
}


/*
 * Make sure the pen size is set.  Since we push and pop the state,
 * this has to be sent for each different object (I think).
 */
static void set_pen_size()
{
    ImWrt1(imP_SET_PEN);
    ImWrt1(pensize);
}


/*
 * Actually apply the attributes (shade, whiten, or blacken) to the currently
 * defined path/figure.
 */
static void do_attributes()
{
    static int family;		/* Family of downloaded texture glyph */
    static int member;		/* Member of family */
    int i,j;			/* Loop through glyph array */

    if (shade_next) {
	shade_next = FALSE;
	if (!family_defined) {
	    family_defined = TRUE;
	    family = fnum++;
	    member = -1;
	}
	if (!texture_defined) {
	    texture_defined = TRUE;
	    member++;
	    ImWrt1(imP_BGLY);
	    ImWrt1((family & 0x7e) >> 1);
	    ImWrt1((family & 0x01) << 7 | (member & 0x7f));
	    /*ImWrt2(0); */		/* Advance width */
	    ImWrt2(32);
	    ImWrt2(TWIDTH*8);	/* pixel width (8 x number of bytes) */
	    /*ImWrt2(0); */		/* left offset */
	    ImWrt2(32);
	    ImWrt2(THEIGHT);	/* and height of glyph */
	    /*ImWrt2(0); */		/* top offset */
	    ImWrt2(32);
	    for (i=0; i<THEIGHT; i++)/* Do rows */
		for (j=0; j<TWIDTH; j++) ImWrt1(stexture[i][j]);
	}
	ImWrt1(imP_SET_TEXTURE);
	ImWrt1((family & 0x7e) >> 1);
	ImWrt1((family & 0x01) << 7 | (member & 0x7f));
	ImWrt1(imP_FILL_PATH);
	ImWrt1(SHADE);
	glyph_init(); /* reinitialize the array */
    }
    else if (whiten_next) {
	whiten_next = FALSE;
	ImWrt1(imP_FILL_PATH);
	ImWrt1(WHITE);
    }
    else if (blacken_next) {
	blacken_next = FALSE;
	ImWrt1(imP_FILL_PATH);
	ImWrt1(BLACK);
    }
}


/*
 * Flush the path that we've built up with im_drawto()
 * Called as \special{fp}
 */
static void im_flushpath()
{
    register int i;

    push_location();
    if (pathlen <= 0) return;
    set_pen_size();
    ImWrt1(imP_CREATE_PATH);
    ImWrt2(pathlen);
    for (i=1; i<=pathlen; i++) {
	ImWrt2(xx[i]);
	ImWrt2(yy[i]);
    }
    pathlen = 0;
    ImWrt1(imP_DRAW_PATH);
    ImWrt1(BLACK);
    do_attributes();
    pop_location();
}


/* Helper routine for dashed_line() */
static void connect(x0, y0, x1, y1)
int x0, y0, x1, y1;
{
    set_pen_size();
    ImWrt1(imP_CREATE_PATH);
    ImWrt2(2);		/* Path length */
    ImWrt2(x0);	ImWrt2(y0);/* The path */
    ImWrt2(x1);	ImWrt2(y1);
    ImWrt1(imP_DRAW_PATH);
    ImWrt1(BLACK);
}


/* Another helper.  Draw a dot at the indicated point */
static void dot_at(x, y)
int x,y;
{
    set_pen_size();
    ImWrt1(imP_CREATE_PATH);
    ImWrt2(1);			/* Path length */
    ImWrt2(x);	ImWrt2(y);	/* The path */
    ImWrt1(imP_DRAW_PATH);
    ImWrt1(BLACK);
}


/*
 * Draw a dashed or dotted line between the first pair of points in the array
 * Called as \special{da <inchesperdash>}	(dashed line)
 *	  or \special{dt <inchesperdot>}	(dotted line)
 *	eg:  \special{da 0.05}
 */
static void dashed_line(cp, dotted)
char *cp;
int dotted;			/* boolean */
{
    int i, numdots, x0, y0, x1, y1;
    double cx0, cy0, cx1, cy1;
    double d, spacesize, a, b, dx, dy, pixperdash;
    float inchesperdash;

    if (sscanf(cp, " %f ", &inchesperdash) != 1) return;
    if (pathlen <= 1) return;
    pixperdash = inchesperdash * ((double) PixPerInY);
    x0 = xx[1]; x1 = xx[2];
    y0 = yy[1]; y1 = yy[2];
    dx = x1 - x0;
    dy = y1 - y0;
    push_location();
    if (dotted) {
	numdots = sqrt(dx*dx + dy*dy) / pixperdash + 0.5;
	if (numdots > 0)
	    for (i = 0; i <= numdots; i++) {
		a = (double) i / (double) numdots;
		cx0 = ((double) x0) + (a*dx) + 0.5;
		cy0 = ((double) y0) + (a*dy) + 0.5;
		dot_at((int) cx0, (int) cy0);
	    }
    }
    else {
	d = sqrt(dx*dx + dy*dy);
	if (d <= 2 * pixperdash) {
	    connect(x0, y0, x1, y1);
	    pathlen = 0;
	    pop_location();
	    return;
	}
	numdots = d / (2 * pixperdash) + 1;
	spacesize = (d - numdots * pixperdash) / (numdots - 1);
	for (i=0; i<numdots-1; i++) {
	    a = i * (pixperdash + spacesize) / d;
	    b = a + pixperdash / d;
	    cx0 = ((double) x0) + (a*dx) + 0.5;
	    cy0 = ((double) y0) + (a*dy) + 0.5;
	    cx1 = ((double) x0) + (b*dx) + 0.5;
	    cy1 = ((double) y0) + (b*dy) + 0.5;
	    connect((int) cx0, (int) cy0, (int) cx1, (int) cy1);
#ifndef UNDEF
	    a = b;
	    b = a + spacesize / d;
#else
	    b += spacesize / d;
#endif UIUC
	}
	cx0 = ((double) x0) + (b*dx) + 0.5;
	cy0 = ((double) y0) + (b*dy) + 0.5;
	connect((int) cx0, (int) cy0, x1, y1);
    }
    pathlen = 0;
    pop_location();
}


/*
 * Virtually draw to a given x,y position on the virtual page.
 * X and Y are expressed in thousandths of an inch, and this
 * routine converts them to pixels.
 *
 * Called as \special{pa <x> <y>}
 *	 eg:  \special{pa 0 1200}
 */
static void im_drawto(cp)
char *cp;
{
    int x,y;

    if (sscanf(cp, " %d %d ", &x, &y) != 2) return;

    if (++pathlen >= MAXPOINTS)
	error(1, 0, "Too many points specified");
    xx[pathlen] = xconv(x);
    yy[pathlen] = yconv(y);
}

#ifndef	USEGRAPHICS
/*
 * Helper routine for im_arc().
 * Convert x and y to integers, then call im_drawto() normally.
 */
static void im_fdraw(x, y)
double x, y;
{
    int ix,iy;

    ix = (int) x + 0.5;
    iy = (int) y + 0.5;
    im_drawto(ix, iy);
}


/*
 * Draw the indicated arc on the virtual page and flush it.
 * The arc is always drawn counter clockwise from start_angle to end_angle on
 * the virtual page.  That is, clockwise in the real world (since +y is down).
 * It is assumed that start_angle has been adjusted to be in the range
 *	0.0 <= start_angle < 2*PI
 * and that end_angle is the smallest suitable angle >= start_angle.  Thus
 * end_angle MAY be >= 2*PI.
 *
 * Called as \special{ar <xcenter> <ycenter> <xradius> <yradius>
 *			 <startangle> <endangle>}
 *
 * <xcenter>,<ycenter>,<xradius>,<yradius> are in 1/1000's of an inch.
 * <startangle> and <endangle> are in radians.
 *
 *	eg:	\special{ar 240 240 30 30 0.000 6.283}
 */
static void im_arc(cp)
char *cp;
{
    int xc, yc, xrad, yrad;
    float start_angle, end_angle;
    double angle, theta, r, xcenter, ycenter, xradius, yradius;
    int n;

    if (sscanf(cp, " %d %d %d %d %f %f ", &xc, &yc, &xrad, &yrad, &start_angle,
	&end_angle) != 6) return;
    xcenter = xc;		/* Convert to floating point */
    ycenter = yc;
    xradius = xrad;
    yradius = yrad;
    r = (xradius+yradius)/2.0;
    theta = sqrt(1.0 / r);
    n = TWOPI / theta + 0.5;
    if (n<6) n = 6;
    if (n>80) n = 80;
    theta = TWOPI / n;

    im_fdraw( xcenter + xradius*cos(start_angle),
		ycenter + yradius*sin(start_angle) );
    angle = start_angle + theta;
    while (angle < end_angle) {
	im_fdraw(xcenter + xradius*cos(angle),
	    ycenter + yradius*sin(angle) );
	angle += theta;
    }
    im_fdraw( xcenter + xradius*cos(end_angle),
		ycenter + yradius*sin(end_angle) );
    im_flushpath();
}

#else	USEGRAPHICS

/* Same routine as above, but it uses the special graphics primitives */
static void im_arc(cp)
char *cp;
{
    int xc, yc, xrad, yrad;
    float start_angle, end_angle;
    short alpha0, alpha1;

    if (sscanf(cp, " %d %d %d %d %f %f ", &xc, &yc, &xrad, &yrad, &start_angle,
	&end_angle) != 6) return;
    push_location();
    set_pen_size();
    ImWrt1(imP_SET_ABS_H);
    ImWrt2(xconv(xc));
    ImWrt1(imP_SET_ABS_V);
    ImWrt2(yconv(yc));

/*
 * If end_angle > TWOPI, we can't simply use it, since it will be > 16383,
 * and thus an illegal angle.  Simply subtracting TWOPI will make it <
 * start_angle, which will reverse the direction the arc is drawn, resulting
 * in the wrong arc.  So we also exchange start_angle and end_angle.  But then
 * start_angle < end_angle, so the arc goes back to its original direction!
 * So we also subtract TWOPI from the original start_angle, foring end_angle
 * to be negative (since 0<=start_angle<TWOPI originally), which will cause
 * the Imagen to draw in a true CCW direction (opposite of normal).
 */
    if (end_angle > TWOPI) {
	double temp;
	temp = end_angle - TWOPI;
	end_angle = start_angle - TWOPI;
	start_angle = temp;
    }
    if (xrad >= yrad-1 && xrad <= yrad+1) {		/* Circle or arc */
	alpha0 = RadToImpUnits(start_angle);
	alpha1 = RadToImpUnits(end_angle);
	ImWrt1(CIRC_ARC);
	ImWrt2(xconv((double) xrad));
	ImWrt2(alpha0);
	ImWrt2(alpha1);
    }
    else {			/* Ellipse */
	ImWrt1(ELLIPSE_ARC);
	ImWrt2(xconv((double) xrad));
	ImWrt2(yconv((double) yrad));
	ImWrt2(0);		/* alphaoff */
	ImWrt2(0);		/* zero start angle */
	ImWrt2(16383);		/* two pi end angle */
    }
    ImWrt1(imP_DRAW_PATH);
    ImWrt1(BLACK);
    do_attributes();
    pop_location();
}
#endif	USEGRAPHICS


/*
 * Create a spline through the points in the array.
 * Called like flush path (fp) command, after points
 * have been defined via pa command(s).
 *
 * eg:	\special{sp}
 */
int splinex[SPLINEPOINTS], spliney[SPLINEPOINTS], splinelen;
static void flush_spline()
{
    int xp, yp, N;
    double t1, t2, t3, w;
    int i, j, steps;

    push_location();
    set_pen_size();
    ImWrt1(imP_CREATE_PATH);
    splinelen = 0;
    N = pathlen + 1;
    xx[0] = xx[1];
    yy[0] = yy[1];
    xx[N] = xx[N-1];
    yy[N] = yy[N-1];
    for (i = 0; i < N-1; i++) {	/* interval */
	steps = (dist(xx[i],yy[i], xx[i+1],yy[i+1]) +
		dist(xx[i+1],yy[i+1], xx[i+2],yy[i+2])) / 20;
	for (j = 0; j < steps; j++) {	/* points within */
		w = ((double) j) / ((double) steps);
		t1 = 0.5 * w * w;
		w -= 0.5;
		t2 = 0.75 - w * w;
		w -= 0.5;
		t3 = 0.5 * w * w;
		xp = t1 * xx[i+2] + t2 * xx[i+1] + t3 * xx[i] + 0.5;
		yp = t1 * yy[i+2] + t2 * yy[i+1] + t3 * yy[i] + 0.5;
		if (splinelen >= SPLINEPOINTS)
		    error(1, 0, "Too many points in spline");
		splinex[splinelen] = xp;
		spliney[splinelen++] = yp;
	}
    }
    ImWrt2(splinelen);
    for (i=0; i<splinelen; i++) {
	ImWrt2(splinex[i]);
	ImWrt2(spliney[i]);
    }

    pathlen = 0;
    ImWrt1(imP_DRAW_PATH);
    ImWrt1(BLACK);
    pop_location();
}


static int dist(x1, y1, x2, y2)	/* integer distance from x1,y1 to x2,y2 */
{
	double dx, dy;

	dx = x2 - x1;
	dy = y2 - y1;
	return sqrt(dx*dx + dy*dy) + 0.5;
}


/*
 * Whiten the interior of the next figure (path).  Command is:
 *	\special{wh}
 */
static void im_whiten()
{
    whiten_next = TRUE;
}


/*
 * Blacken the interior of the next figure (path).  Command is:
 *	\special{bk}
 */
static void im_blacken()
{
    blacken_next = TRUE;
}


/*
 * Shade the interior of the next figure (path) with the predefined
 * texture.  Command is:
 *	\special{sh}
 */
static void im_shade()
{
    shade_next = TRUE;
}


/*
 * Define the texture array.  Command is:
 *	\special{tx 32bits 32bits ....}
 */
static void im_texture(pcount,bitpattern)
int pcount, bitpattern[32];
{
    int i,j,k;
    unsigned long ul_one;

#ifdef	DEBUG
    if (sizeof ul_one != TWIDTH)
	error(1, 0, "pointer/size mismatch");
#endif
    j = 0;
    for (k=0; k < THEIGHT/pcount; k++) {
	for (i=0; i<pcount; i++) {
	    ul_one = htonl((unsigned long) bitpattern[i]);
	    bcopy((char *) &ul_one, stexture[j++], TWIDTH);
	}
    }
    texture_defined = FALSE;
}


/*
 * This routine takes the string argument for a tx command and
 * parses out the separate bitpatterns to call im_texture with.
 * Written by Tinh Tang
 */
static void do_texture(t)
char *t;
{
    int bitpattern[32];
    int pcount = 0;

    while (isspace (*t)) t++;
    while (*t) {
	if (sscanf(t, "%x", &bitpattern[pcount++]) != 1) {
	    error(0, 0, "malformed tx command");
	    return;
	}
	while (*t && !isspace(*t)) t++;/* Skip to space */
	while (*t && isspace(*t)) t++;/* Skip to nonspace */
    }
    if (pcount != 4 && pcount != 8 && pcount != 16 && pcount != 32) {
	error(0, 0, "malformed tx command");
	return;
    }
    im_texture(pcount, bitpattern);
}

	
#define	COMLEN	3		/* Length of a tpic command plus one */

DoSpecial(k)
int k;
{
    char *spstring, *cp, command[COMLEN];
    register int len;

    spstring = malloc((unsigned) (k+1));
    if (spstring == NULL) error(2, 0, "Out of memory");
    len = 0;
    while (k--) spstring[len++] = getchar();
    spstring[len] = '\0';
    cp = spstring;
    while (isspace(*cp)) ++cp;
    len = 0;
    while (!isspace(*cp) && *cp && len < COMLEN-1) command[len++] = *cp++;
    command[len] = '\0';
    if (strcmp(command, "pn") == 0) im_pensize(cp);
    else if (strcmp(command, "fp") == 0) im_flushpath();
    else if (strcmp(command, "da") == 0) dashed_line(cp, 0);
    else if (strcmp(command, "dt") == 0) dashed_line(cp, 1);
    else if (strcmp(command, "pa") == 0) im_drawto(cp);
    else if (strcmp(command, "ar") == 0) im_arc(cp);
    else if (strcmp(command, "sp") == 0) flush_spline();
    else if (strcmp(command, "sh") == 0) im_shade();
    else if (strcmp(command, "wh") == 0) im_whiten();
    else if (strcmp(command, "bk") == 0) im_blacken();
    else if (strcmp(command, "tx") == 0) do_texture(cp);
    else error(0, 0, "warning: ignoring \\special");

    free(spstring);
}

/***********************************************************************
 *
 *	This is an alternate dvistuff compatible interface to the specials.
 */

void newDoSpecial(cp)
char *cp;
{
    char command[COMLEN], *orig_cp;
    register int len;

    orig_cp = cp;
    while (isspace(*cp)) ++cp;
    len = 0;
    while (!isspace(*cp) && *cp && len < COMLEN-1) command[len++] = *cp++;
    command[len] = '\0';
    if (strcmp(command, "pn") == 0) im_pensize(cp);
    else if (strcmp(command, "fp") == 0) im_flushpath();
    else if (strcmp(command, "da") == 0) dashed_line(cp, 0);
    else if (strcmp(command, "dt") == 0) dashed_line(cp, 1);
    else if (strcmp(command, "pa") == 0) im_drawto(cp);
    else if (strcmp(command, "ar") == 0) im_arc(cp);
    else if (strcmp(command, "sp") == 0) flush_spline();
    else if (strcmp(command, "sh") == 0) im_shade();
    else if (strcmp(command, "wh") == 0) im_whiten();
    else if (strcmp(command, "bk") == 0) im_blacken();
    else if (strcmp(command, "tx") == 0) do_texture(cp);
    else fprintf(stderr, "[%s] special \"%s\" not implemented\n",
		ProgName, orig_cp);
}
