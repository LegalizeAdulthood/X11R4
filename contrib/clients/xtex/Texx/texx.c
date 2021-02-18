/*
 * Copyright 1989 Dirk Grunwald
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  Dirk
 * Grunwald and M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Dr. Dirk Grunwald
 * 	Dept. of Computer Science
 * 	Campus Box 430
 * 	Univ. of Colorado, Boulder
 * 	Boulder, CO 80309
 * 
 * 	grunwald@colorado.edu
 * 	
 */ 
#include <sys/types.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <stdio.h>
#include <ctype.h>

#include "libtex/dvistuff.h"
static char *rcsid="$Header: /usr/local/src/Uiuctex/Texx/RCS/texx.c,v 1.20 88/08/29 13:20:48 grunwald Exp Locker: grunwald $" ;
/*
 *	These constants may need to be changed on your implementation
 */

#define BitsPerByte 8
#define BitsPerShort 16
#define BytesPerShort	(sizeof(short))

#define BitsPerByteMask 0x7
#define BitsPerByteShift 3

#define GCallbits ((1L<<(GCLastBit+1))-1)

/*
 *	Note that ROUNDUP evaluates Y twice.
 */
#define ROUNDUP(x,y) (((x)+(y)-1) / (y))



int debug = 0;
static int  obscured = 1;


/*
 *	X-11 related variables
 */

Display *DISP;
static Window win;
static int backpix;
static GC highpix, forepix;
static GC specialGC;
static int defaultScreen;

static Cursor workingCursor;
static Cursor readyCursor;

/*
 *	TeX-Dvi related variables
 */

static int rawDviHeight;
static int rawDviWidth;	/* part of X interface, not dvistuff */
static int maxHeight;
static int maxWidth;

static int  screenWidth, screenHeight;

#define MAX_LEAVES	2
#define LEAF_LEFT	0
#define LEAF_RIGHT	1

static int Leaves;
static int currentLeaf = 0;
static int pageOnLeaf[MAX_LEAVES] = {-1,-1};
static int haveLargePage[MAX_LEAVES] = {-1, -1};

#define SHRINK_NORMAL	0
#define SHRINK_LARGE	1
#define MAX_SHRINKS	2

static int page_w[MAX_SHRINKS], page_h[MAX_SHRINKS];
static int leaf_w[MAX_SHRINKS];

/*
 *	Note: these are used by dvi_draw.c, and are not static
 */
int shrinkFactor[MAX_SHRINKS];
int currentShrink;

static struct glyph **shrunkenGlyphs[MAX_SHRINKS][MAX_FONTFAMILY];
static XImage **shrunkenImages[MAX_SHRINKS][MAX_FONTFAMILY];

static int reverse = 0;

static double specialConv;

char *malloc(), *calloc(), *index();

#define MAX(a,b) ((a) < (b)) ? (b) : (a)
#define MIN(a,b) ((a) < (b)) ? (a) : (b)

/*
 *	I don't know why this is needed, but our sun seems to thing
 *	and is or & vice versa. This is true even running X11R2
 */

/*
#if	defined(sun) || defined(USE_SUN)
#define GXAND GXand
#define GXOR GXor
#else
#define GXAND GXor
#define GXOR  GXand
#endif
*/

int Black_Pixel = 0;

main(argc, argv)
    int argc;
    char **argv;
{
    
    int xargc=argc;
    char **xargv=argv;
    
    char *file;
    char *display = NULL;
    char *option;
    
    int bwidth = 2;
    char *fore_color;
    char *back_color;
    char *high_color;
    char *brdr_color;
    char *mous_color;
    char *geometry = NULL, def[32];
    
    int bdrpix, mouspix;
    XColor cdef;
    XSizeHints hints;
    XSetWindowAttributes xswattrs;
    unsigned long xswattrs_mask;

    double atof();
    
    char *getenv();
    
    ProgName = *argv;
    argv++;
    argc--;
    
    if ((DISP = XOpenDisplay(display)) == NULL) {
	
	fprintf(stderr, "[%s] Can't open display '%s'\n",
		ProgName, XDisplayName(display));
	exit(1);
    }
    Black_Pixel = XBlackPixel(DISP, 0);
    
    if ((option = XGetDefault(DISP, ProgName, "ReverseVideo")) &&
	strcmp(option, "on") == 0)
	reverse = 1;
    if (option = XGetDefault(DISP, ProgName, "BorderWidth"))
	bwidth = atoi(option);
    fore_color = XGetDefault(DISP, ProgName, "ForeGround");
    back_color = XGetDefault(DISP, ProgName, "BackGround");
    high_color = XGetDefault(DISP, ProgName, "Highlight");
    brdr_color = XGetDefault(DISP, ProgName, "Border");
    mous_color = XGetDefault(DISP, ProgName, "Mouse");

    option = XGetDefault(DISP, ProgName, "NormalShrink");
    shrinkFactor[SHRINK_NORMAL] = (option == 0) ? 0 : atoi(option);

    option = XGetDefault(DISP, ProgName, "LargeShrink");
    shrinkFactor[SHRINK_LARGE]  = (option == 0) ? 0 : atoi(option);

    option = XGetDefault(DISP, ProgName, "Blackness");
    dviBlackness = (option == 0) ? DEFAULT_BLACKNESS : atoi(option);

    option = XGetDefault(DISP, ProgName, "Leaves");
    Leaves = (option == 0) ? 0 : atoi(option);

    option = XGetDefault(DISP, ProgName, "Dpi");
    dviDPI = (option == 0) ? DEFAULT_DPI : atoi(option);

    option = XGetDefault(DISP, ProgName, "TopMargin");
    dviVVMargin = (option == 0) ? DEFAULT_VVMARGIN : atof(option) * dviDPI;

    option = XGetDefault(DISP, ProgName, "SideMargin");
    dviHHMargin = (option == 0) ? DEFAULT_HHMARGIN : atof(option) * dviDPI;

    file = NULL;
    
    while (argc) {
	if (strcmp(*argv, "-p") == 0 && argc > 1) {
	    argv++;
	    argc--;
	    dviDPI = atoi(*argv);
	    if (dviDPI <= 0) goto usage;
	} else if (strcmp(*argv, "-rv") == 0) {
	    reverse = !reverse;
	} else if (strcmp(*argv, "-bw") == 0 && argc > 1) {
	    argv++;
	    argc--;
	    bwidth = atoi(*argv);
	} else if (strcmp(*argv, "-fg") == 0 && argc > 1) {
	    argv++;
	    argc--;
	    fore_color = *argv;
	} else if (strcmp(*argv, "-bg") == 0 && argc > 1) {
	    argv++;
	    argc--;
	    back_color = *argv;
	} else if (strcmp(*argv, "-hl") == 0 && argc > 1) {
	    argv++;
	    argc--;
	    high_color = *argv;
	} else if (strcmp(*argv, "-bd") == 0 && argc > 1) {
	    argv++;
	    argc--;
	    brdr_color = *argv;
	} else if (strcmp(*argv, "-ms") == 0 && argc > 1) {
	    argv++;
	    argc--;
	    mous_color = *argv;
	} else if (strcmp(*argv,"-l") == 0 && argc > 1) {
	    argv++;
	    argc--;
	    Leaves = atoi(*argv);
	    if( Leaves < 1 || Leaves > MAX_LEAVES) {
		fprintf(stderr,"[%s] Bad number of leaves(%d), using %d\n",
			ProgName, Leaves, MAX_LEAVES);
		Leaves = MAX_LEAVES;
	    }
	} else if (strcmp(*argv,"-ns") == 0 && argc > 1) {
	    argv++;
	    argc--;
	    shrinkFactor[SHRINK_NORMAL] = atoi(*argv);
	} else if (strcmp(*argv,"-ls") == 0 && argc > 1) {
	    argv++;
	    argc--;
	    shrinkFactor[SHRINK_LARGE] = atoi(*argv);
	} else if (strcmp(*argv,"-bl") == 0 && argc > 1) {
	    argv++;
	    argc--;
	    dviBlackness = atoi(*argv);
	} else if (strcmp(*argv,"-dpi") == 0 && argc > 1) {
	    argv++;
	    argc--;
	    dviDPI = atoi(*argv);
	} else if (strcmp(*argv,"-tm") == 0 && argc > 1) {
	    argv++;
	    argc--;
	    dviVVMargin = dviDPI * atof(*argv);
	} else if (strcmp(*argv,"-sm") == 0 && argc > 1) {
	    argv++;
	    argc--;
	    dviHHMargin = dviDPI * atof(*argv);
	} else if (**argv == '=') {
	    geometry = *argv;
	} else if (**argv != '-') {
	    if (index(*argv, ':') != NULL)
		display = *argv;
	    else
		dviFileName = *argv;
	} else {
	usage:
	    fprintf(stderr, "Usage: %s [-s <shrink>] [-l <leaves>] [-rv] [-fg <color>] [-bg <color>] [-hl <color>] [-bd <color>] [-ms <color>] [=<geometry>] [host:display] dviFile\n",
		    ProgName);
	    exit(1);
	}
	argv++;
	argc--;
    }
    if (dviFileName == NULL)
	goto usage;

    dviInit();

    specialConv = (float) dviDPI / 1000.0;
    
/*
 *	If no shrink factor was chosen, pick one which will work out nicely
 *	on their display
 */
    
    defaultScreen = XDefaultScreen(DISP);
    
    maxHeight = XDisplayHeight(DISP, defaultScreen) - 2 * bwidth;
    maxWidth = XDisplayWidth(DISP, defaultScreen) - 2 * bwidth;
    
    rawDviHeight = dviTallestPage + 2 * dviVVMargin;
    rawDviWidth  = dviWidestPage + 2 * dviHHMargin;
    
    if (Leaves == 0) {
	if (dviTotalPages == 1) {
	    Leaves = 1;
	} else {
	    Leaves = 2;
	}
    }

    if (shrinkFactor[SHRINK_NORMAL] == 0) {
	int sH;
	int sW;
	int sH2;
	int sW2;
	int shrink2;
	
	sH = (rawDviHeight + maxHeight - 1) / maxHeight;
	sW = (Leaves * rawDviWidth  + maxWidth - 1) / maxWidth;
	
	shrinkFactor[SHRINK_NORMAL] = MAX(sW, sH);
	shrinkFactor[SHRINK_NORMAL] = MAX(shrinkFactor[SHRINK_NORMAL], 1);
	
/*
 *	Check to see if we can get another shrink size bigger display
 *	if we cut the margins.
 */
	
	sH2 = (dviTallestPage + (dviHHMargin/32) + maxHeight - 1)
	    / maxHeight;
	sW2 = ( Leaves * (dviWidestPage + (dviVVMargin/32))
	       + maxHeight - 1) / maxHeight;
	
	shrink2 = MAX(sH2, sW2);
	shrink2 = MAX(shrink2, 1);
	
	if (shrink2 < shrinkFactor[SHRINK_NORMAL]) {
	    dviVVMargin /= 32;
	    dviHHMargin /= 32;
	    rawDviHeight = dviTallestPage + 2 * dviVVMargin;
	    rawDviWidth  = dviWidestPage + 2 * dviHHMargin;
	    shrinkFactor[SHRINK_NORMAL] = shrink2;
	}
    }
    
    page_h[SHRINK_NORMAL]=(rawDviHeight + shrinkFactor[SHRINK_NORMAL] - 1)
	/ shrinkFactor[SHRINK_NORMAL];
    leaf_w[SHRINK_NORMAL]=(rawDviWidth + shrinkFactor[SHRINK_NORMAL] - 1)
	/ shrinkFactor[SHRINK_NORMAL];

/*
 *	Based on the shrink factor, choose a shrink factor for the enlarged
 *	display
 */
    if (shrinkFactor[SHRINK_LARGE] == 0) {
	shrinkFactor[SHRINK_LARGE] = shrinkFactor[SHRINK_NORMAL] / 2;
    }
    shrinkFactor[SHRINK_LARGE] = MIN(shrinkFactor[SHRINK_LARGE],
				     shrinkFactor[SHRINK_NORMAL]-1);
    shrinkFactor[SHRINK_LARGE] = MAX(shrinkFactor[SHRINK_LARGE], 1);
    page_h[SHRINK_LARGE]=(rawDviHeight + shrinkFactor[SHRINK_LARGE] - 1)
	/ shrinkFactor[SHRINK_LARGE];
    leaf_w[SHRINK_LARGE]=(rawDviWidth + shrinkFactor[SHRINK_LARGE] - 1)
	/ shrinkFactor[SHRINK_LARGE];
/*
 *	Compute the page size given the number of leaves. We may have
 *	to scale back if everything cant fit.
 */
    
    if (leaf_w[SHRINK_NORMAL] * Leaves <= maxWidth) {
	page_w[SHRINK_NORMAL] = leaf_w[SHRINK_NORMAL] * Leaves;
    } else {
	page_w[SHRINK_NORMAL] = leaf_w[SHRINK_NORMAL];
	Leaves = 1;
    }
    
    screenWidth = (page_w[SHRINK_NORMAL] > maxWidth)
	? maxWidth : page_w[SHRINK_NORMAL];
    screenHeight = (page_h[SHRINK_NORMAL] > maxHeight)
	? maxHeight : page_h[SHRINK_NORMAL];
    
    highpix = XCreateGC(DISP, RootWindow(DISP, defaultScreen), 0, NULL);
    forepix = XCreateGC(DISP, RootWindow(DISP, defaultScreen), 0, NULL);
    
    XCopyGC(DISP, DefaultGC(DISP, 0), GCallbits, highpix);
    XCopyGC(DISP, highpix, GCallbits, forepix);
    
    if (reverse) {
	
	XSetForeground(DISP, forepix, WhitePixel(DISP, defaultScreen));
	XSetForeground(DISP, highpix, WhitePixel(DISP, defaultScreen));
	backpix = BlackPixel(DISP, defaultScreen);
	bdrpix  = WhitePixel(DISP, defaultScreen);
	mouspix = WhitePixel(DISP, defaultScreen);
        if (Black_Pixel) {
	    XSetFunction(DISP, forepix, GXand); /* or */
	    XSetFunction(DISP, highpix, GXand);
	} else {
	    XSetFunction(DISP, forepix, GXor); /* or */
	    XSetFunction(DISP, highpix, GXor);
	}	
    } else {
	
	XSetForeground(DISP, forepix, BlackPixel(DISP, defaultScreen));
	XSetForeground(DISP, highpix, BlackPixel(DISP, defaultScreen));
	backpix = WhitePixel(DISP, defaultScreen);
	bdrpix  = BlackPixel(DISP, defaultScreen);
	mouspix = BlackPixel(DISP, defaultScreen);
        if (Black_Pixel) {
	    XSetFunction(DISP, forepix, GXor); /* and */
	    XSetFunction(DISP, highpix, GXor);
	} else {
	    XSetFunction(DISP, forepix, GXand); /* and */
	    XSetFunction(DISP, highpix, GXand);
	}	
    }
    
    if (DisplayCells(DISP, 0) > 2) {
	Colormap dcm = DefaultColormap(DISP, defaultScreen);
	if (fore_color
	    && XParseColor(DISP, dcm, fore_color,  &cdef)
	    && XAllocColor(DISP, dcm, &cdef)) {
		XSetForeground(DISP, forepix, cdef.pixel);
	    }
	
	if (back_color
	    && XParseColor(DISP, dcm, back_color,  &cdef)
	    && XAllocColor(DISP, dcm, &cdef)) {
		backpix = cdef.pixel;
	    }
	
	if (high_color
	    && XParseColor(DISP, dcm, high_color,  &cdef)
	    && XAllocColor(DISP, dcm, &cdef)) {
		XSetForeground(DISP, highpix, cdef.pixel);
	    }
	
	if (brdr_color
	    && XParseColor(DISP, dcm, brdr_color,  &cdef)
	    && XAllocColor(DISP, dcm, &cdef))
	{
	    /* null statement? */
	}
	
	if (mous_color
	    &&  XParseColor(DISP, dcm, mous_color,  &cdef)
	    && XAllocColor(DISP, dcm, &cdef)) {
		mouspix = cdef.pixel;
	    }
    }
    
    XSetBackground(DISP, forepix, backpix);
    XSetBackground(DISP, highpix, backpix);
    
    sprintf(def, "=%dx%d+0+0", screenWidth, screenHeight);
    
    hints.width = page_w[SHRINK_NORMAL];
    hints.height = page_h[SHRINK_NORMAL];
    hints.x = 0; hints.y = 0;
    hints.flags = (PSize | PPosition);
    
    xswattrs.border_pixel = bdrpix;
    xswattrs.background_pixel = backpix;
    xswattrs_mask = (CWBackPixel|CWBorderPixel);
    
    win = XCreateWindow(DISP, RootWindow(DISP, defaultScreen),
			0, 0,
			screenWidth, screenHeight,
			bdrpix,
			0,	/* depth from parent */
			InputOutput,
			CopyFromParent,
			xswattrs_mask,
			&xswattrs);
    
    XSetStandardProperties(DISP, win,
			   ProgName, "DVI Previewer",
			   None,
			   argv, argc, &hints);
    
    XMapRaised(DISP, win);
    XSelectInput(DISP, win,
		 KeyPressMask|ButtonPressMask | ButtonReleaseMask
		 | ButtonMotionMask |ExposureMask | VisibilityChangeMask);
    
    workingCursor = XCreateFontCursor(DISP, XC_watch);
    readyCursor = XCreateFontCursor(DISP, XC_spraycan);
    
    XDefineCursor(DISP, win, workingCursor);
    XFlush(DISP);

    specialGC = XCreateGC(DISP, RootWindow(DISP, defaultScreen), 0, NULL);
    XCopyGC(DISP, forepix, GCallbits, specialGC);
    
    mainLoop();
    stop_output(0);
}

/*
 *	Pixmap manipulation routines
 */

static Pixmap pasteUpPixmap[MAX_SHRINKS][MAX_LEAVES];
static GC pasteFillGC;
static GC pasteCopyGC;

static int largePixmapsAllocated = 0;

/*
 *	Ideally, we'd like to paint up a local pixmap & then transmit the
 *	entire pixmap. However, it looks like pixmaps require interaction
 *	with the server.
 *
 *	So, if the entire page fits on the screen, we'll paint the screen
 *	and when we're done, we'll copy the screen to the pixmap. This
 *	gives the illusion of progress, but allows us to do fast re-paints.
 *
 *	If we're *not* on the display, we'll just draw to the pixmap and
 *	update it at the end
 */

static int buildToPixmap = 1;

static void
clearPixmap(leaf, shrinkSize)
int leaf;
int shrinkSize;
{
    if (leaf< 0 || leaf >= Leaves) {
	fprintf(stderr,"[%s] Reference to bogus leaf in clearPixmap: %d\n",
		ProgName, leaf);
	exit(1);
    }

    if (shrinkSize < 0 || shrinkSize >= MAX_SHRINKS ) {
	fprintf(stderr,"[%s] reference to bogus shrink %d\n",
		ProgName, shrinkSize);
	exit(1);
    }
    XFillRectangle(DISP, pasteUpPixmap[shrinkSize][leaf], pasteFillGC, 0, 0,
		   leaf_w[shrinkSize], page_h[shrinkSize]);
    XSync(DISP, 0);
}

static void
clearWindow(leaf)
int leaf;
{
    XFillRectangle(DISP, win, pasteFillGC,
		   leaf * leaf_w[SHRINK_NORMAL], 0,
		   leaf_w[SHRINK_NORMAL], page_h[SHRINK_NORMAL]);
}

allocatePixmaps()
{
    int i;
    XWindowAttributes wa;

    XGetWindowAttributes(DISP, RootWindow(DISP,0), &wa);
    for (i = 0; i < Leaves; i++ ) {
	pasteUpPixmap[SHRINK_NORMAL][i] =
	    XCreatePixmap(DISP, win, leaf_w[SHRINK_NORMAL],
			  page_h[SHRINK_NORMAL], wa.depth);
	if (pasteUpPixmap[SHRINK_NORMAL][i] == 0) {
	    fprintf(stderr,"[%s] erp! out of PIXMAP memory!\n",
		    ProgName);
	    exit(1);
	}
    }

    largePixmapsAllocated = 0;

    if (pasteFillGC == 0) {
	pasteFillGC = XCreateGC(DISP, RootWindow(DISP, defaultScreen),0,NULL);
	pasteCopyGC = XCreateGC(DISP, RootWindow(DISP, defaultScreen),0,NULL);
	XCopyGC(DISP, forepix, GCallbits, pasteFillGC);
	XCopyGC(DISP, forepix, GCallbits, pasteCopyGC);
	if (reverse) {
	    XSetForeground(DISP, pasteFillGC, BlackPixel(DISP, defaultScreen));
	} else {
	    XSetForeground(DISP, pasteFillGC, WhitePixel(DISP, defaultScreen));
	}
	XSetFunction(DISP, pasteFillGC, GXcopy);
	XSetFunction(DISP, pasteCopyGC, GXcopy);
    }

/*
 *	Clear the pixmap
 */
    for (i = 0; i < Leaves; i++) {
	clearPixmap(i, SHRINK_NORMAL);
    }
}

allocateLargePixmaps()
{
    int i;
    XWindowAttributes wa;

    XGetWindowAttributes(DISP, RootWindow(DISP,0), &wa);
    for (i = 0; i < Leaves; i++) {
	pasteUpPixmap[SHRINK_LARGE][i] =
	    XCreatePixmap(DISP, win, leaf_w[SHRINK_LARGE],
			  page_h[SHRINK_LARGE], wa.depth);

	clearPixmap(i, SHRINK_LARGE);
    }

    largePixmapsAllocated = 1;
}

/*
 *	Prepare everything for re-reading a .dvi file
 */
reReadFile()
{
    void buildLeaf();
    int new_page_w[MAX_SHRINKS], new_page_h[MAX_SHRINKS];
    int new_leaf_w[MAX_SHRINKS];

    dviResetAll();

    if (dviInit()) {
	fprintf(stderr,"Unable to reload dvi file (%s), exiting\n",
		dviFileName);
	stop_output();
    }

    dviCurrentPage = pageOnLeaf[0];

    rawDviHeight = dviTallestPage + 2 * dviVVMargin;
    rawDviWidth  = dviWidestPage + 2 * dviHHMargin;
    
    new_page_h[SHRINK_NORMAL]=(rawDviHeight + shrinkFactor[SHRINK_NORMAL] - 1)
	/ shrinkFactor[SHRINK_NORMAL];
    new_leaf_w[SHRINK_NORMAL]=(rawDviWidth + shrinkFactor[SHRINK_NORMAL] - 1)
	/ shrinkFactor[SHRINK_NORMAL];

    if (new_leaf_w[SHRINK_NORMAL] * Leaves <= maxWidth) {
	new_page_w[SHRINK_NORMAL] = new_leaf_w[SHRINK_NORMAL] * Leaves;
    } else {
	new_page_w[SHRINK_NORMAL] = new_leaf_w[SHRINK_NORMAL];
	Leaves = 1;
    }

    if (new_leaf_w[SHRINK_NORMAL] != leaf_w[SHRINK_NORMAL]
	| new_page_h[SHRINK_NORMAL] != page_h[SHRINK_NORMAL]) {

	    int i;

	    page_h[SHRINK_NORMAL] = new_page_h[SHRINK_NORMAL] ;
	    leaf_w[SHRINK_NORMAL] = new_leaf_w[SHRINK_NORMAL] ;

	    for (i = 0; i < Leaves; i++ ) {
		if (pasteUpPixmap[SHRINK_NORMAL][i] != 0) {
		    XFreePixmap(DISP, win, pasteUpPixmap[SHRINK_NORMAL][i]);
		}
		if (pasteUpPixmap[SHRINK_LARGE][i] != 0) {
		    XFreePixmap(DISP, win, pasteUpPixmap[SHRINK_LARGE][i]);
		}
	    }

	    allocatePixmaps();
	}

    haveLargePage[0] = -1;
    haveLargePage[1] = -1;

    buildLeaf(LEAF_LEFT, dviCurrentPage, SHRINK_NORMAL);
    if (Leaves == 2) {
	buildLeaf(LEAF_RIGHT, dviCurrentPage + 1, SHRINK_NORMAL);
    }
}

/*
 *	display the normal sized leaf
 */
static void
displayLeaves()
{
    int leaf;

    XClearWindow(DISP,win);
    for (leaf = 0; leaf < Leaves; leaf++) {
	XCopyArea(DISP, pasteUpPixmap[SHRINK_NORMAL][leaf], win, forepix,
		  0, 0,
		  leaf_w[SHRINK_NORMAL], page_h[SHRINK_NORMAL],
		  leaf_w[SHRINK_NORMAL] * leaf, 0);
    }

}

static void displayLeaf(leaf)
int leaf;
{
    clearWindow(leaf);
    XCopyArea(DISP, pasteUpPixmap[SHRINK_NORMAL][leaf], win, forepix,
	      0, 0,
	      leaf_w[SHRINK_NORMAL], page_h[SHRINK_NORMAL],
	      leaf_w[SHRINK_NORMAL] * leaf, 0);
}

static void
swapLeaf(from, to)
int from;
int to;
{
    Pixmap tempPix;
    int tempNum;

    if (to < 0 || to >= Leaves) {
	fprintf(stderr,"[%s] bogus to leaf %d in swapLeaf\n",
		ProgName, to);
	exit(1);
    }
    if (from < 0 || from >= Leaves) {
	fprintf(stderr,"[%s] bogus from leaf %d in swapLeaf\n",
		ProgName, from);
	exit(1);
    }

    tempPix = pasteUpPixmap[SHRINK_NORMAL][to];
    pasteUpPixmap[SHRINK_NORMAL][to] = pasteUpPixmap[SHRINK_NORMAL][from];
    pasteUpPixmap[SHRINK_NORMAL][from] = tempPix;

    tempPix = pasteUpPixmap[SHRINK_LARGE][to];
    pasteUpPixmap[SHRINK_LARGE][to] = pasteUpPixmap[SHRINK_LARGE][from];
    pasteUpPixmap[SHRINK_LARGE][from] = tempPix;

    tempNum = pageOnLeaf[to];
    pageOnLeaf[to] = pageOnLeaf[from];
    pageOnLeaf[from] = tempNum;

    tempNum = haveLargePage[to];
    haveLargePage[to] = haveLargePage[from];
    haveLargePage[from] = tempNum;
}

static void
buildLeaf(leaf, page, shrinkSize)
int leaf;
int page;
int shrinkSize;
{
    XDefineCursor(DISP, win, workingCursor);

    if (leaf < 0 || leaf >= Leaves) {
	fprintf(stderr,"[%s] bogus leaf %d in buildLeaf\n",
		ProgName, leaf);
	exit(1);
    }

    if (shrinkSize < 0 || shrinkSize >= MAX_SHRINKS ) {
	fprintf(stderr,"[%s] Bogus shrink size %d in buildLeaf\n",
		ProgName, shrinkSize);
	exit(1);
    }

    if (shrinkSize == SHRINK_LARGE && !largePixmapsAllocated) {
	allocateLargePixmaps();
    }

/*
 *	If this is a normal size page, kill the information
 *	for the large version.
 */
    if (shrinkSize == SHRINK_NORMAL) {
	haveLargePage[leaf] = -1;
    }

/*
 *	Determine if this is a valid page. If it's not, we'll just clear
 *	the particular leaf.
 */

    if (page < 0 || page >= dviTotalPages) {
	clearPixmap(leaf, shrinkSize);

	if (shrinkSize == SHRINK_NORMAL) {
	    clearWindow(leaf);
	}
    } else {
	
	if (obscured || shrinkSize == SHRINK_LARGE) {
	    buildToPixmap = 1;	
	} else {
	    buildToPixmap = 0;
	}
	
	if (buildToPixmap) {
	    clearPixmap(leaf, shrinkSize);
	} else {
	    clearPixmap(leaf, shrinkSize);
	    if (shrinkSize == SHRINK_NORMAL) {
		clearWindow(leaf);
	    }
	}

/*
 *	Note that dviPreparePage may change dviCurrentPage
 */

	currentLeaf = leaf;
	currentShrink = shrinkSize;

	dviPreparePage(page);

	page = dviCurrentPage;

/*
 *	Save the constructed page if we were painting it to the screen
 */
	put_border(0, 0, leaf_w[currentShrink], page_h[currentShrink], 1);

	if (! buildToPixmap && shrinkSize == SHRINK_NORMAL) {
	    XCopyArea(DISP, win, pasteUpPixmap[SHRINK_NORMAL][leaf], forepix,
		      leaf * leaf_w[currentShrink], 0,
		      screenWidth, screenHeight, 0,0);
	} else if (shrinkSize == SHRINK_NORMAL) {
		displayLeaf(leaf);
	    }
    }

    pageOnLeaf[leaf] = dviCurrentPage;
    XDefineCursor(DISP, win, readyCursor);

    XFlush(DISP);
}
    
/*
 *	interfaces to dvistuff
 */

/*
 *	Whenever a new font is registers, we create a shrunken Glyph
 *	table for it. However, we don't shrink the glyphs -- that's
 *	done on the fly by the putChar routine.
 */

DviFont *
applicationNewFont(f, key)
struct font *f;
int key;
{
    int shrink;

    if (key < 0 || key > MAX_FONTFAMILY) {
	fprintf(stderr,"[%s] bogus key in Newfont = %d\n",
		ProgName, key);
	exit(1);
    }
    
    for (shrink = SHRINK_NORMAL; shrink <= SHRINK_LARGE; shrink++) {
	
	if (shrunkenGlyphs[shrink][key] == 0) {
	    int lth = sizeof(struct glyph *) * MAX_GLYPH;
	    struct glyph **g;
	    
	    g = (struct glyph **) malloc( lth );
	    bzero(g, lth);
	    shrunkenGlyphs[shrink][key] = g;
	}
	
	if (shrunkenImages[shrink][key] == 0) {
	    int lth = sizeof(XImage *) * MAX_GLYPH;
	    XImage **image;
	    
	    image = (XImage **) malloc(lth);
	    bzero(image, lth);
	    shrunkenImages[shrink][key] = image;
	}
    }

    return(f);
}

/*
 *	When we reset a font, we only need to free the storage for the
 *	shrunken glyphs. We keep the glyph table available because we're
 *	very likely to fill it in again.
 */

void
applicationResetFont(fi, key)
struct fontinfo *fi;
int key;
{
    int i;
    struct glyph **theseGlyphs;
    XImage **theseImages;
    int shrink;
    
    
    for (shrink = SHRINK_NORMAL; shrink <= SHRINK_LARGE; shrink++) {
	theseGlyphs = shrunkenGlyphs[shrink][key];
	if (theseGlyphs != 0) {
	    for (i = 0; i < MAX_GLYPH; i++) {
		struct glyph *g;
		
		g = theseGlyphs[i];
		
		if (g != 0) {
		    if ( g -> g_raster != 0) {
			free(g -> g_raster);
		    }
		    free(g);
		    theseGlyphs[i] = 0;
		}
	    }
	}
	
	theseImages = shrunkenImages[shrink][key];
	
	if (theseImages != 0) {
	    for (i = 0; i < MAX_GLYPH; i++) {
		if (theseImages[i] != 0) {
		    XDestroyImage(theseImages[i]);
		}
		theseImages[i] = 0;
	    }
	}
    }
}


void
applicationPutChar(hh, vv, charCode)
int hh;
int vv;
int charCode;
{
    register struct glyph *g;
    int x,y;
    XImage *image;
    int key;
    
    key = dviCurrentFont -> family;

    g = shrunkenGlyphs[currentShrink][key][charCode];

    if (g == 0) {
	g = dviShrinkGlyph(dviCurrentFont -> f -> f_gly[charCode],
			   shrinkFactor[currentShrink],
			   shrinkFactor[currentShrink]);
	shrunkenGlyphs[currentShrink][key][charCode] = g;
    }

    if (g == 0 || !HASRASTER(g)) return;

    hh /= shrinkFactor[currentShrink];
    vv /= shrinkFactor[currentShrink];

    x = hh - g -> g_xorigin;
    y = vv - g -> g_yorigin;

    image = shrunkenImages[currentShrink][key][charCode];

    if (image == 0 ) {
	image = XCreateImage(DISP, DefaultVisual(DISP, defaultScreen),
			     1, XYBitmap,
			     0, g->g_raster,
			     g->g_width, g->g_height,
			     16, 0);

	image -> bitmap_unit = 8;
	image -> bitmap_bit_order = MSBFirst;

	shrunkenImages[currentShrink][key][charCode] = image;
    }

    if (buildToPixmap) {
	XPutImage(DISP, pasteUpPixmap[currentShrink][currentLeaf],
		  forepix, image,
		  0, 0,
		  x, y,
		  g->g_width, g->g_height);
    } else {
	XPutImage(DISP, win,
		  forepix, image,
		  0, 0,
		  x + currentLeaf * leaf_w[currentShrink], y,
		  g->g_width, g->g_height);
	XSync(DISP, 0);
    }
}

void 
applicationSetRule(hh, vv, h, w)
int hh, vv;
int h, w;
{
/* (w,h) specifies lower left corner of rule box */
    int nh, nw;

    hh /= shrinkFactor[currentShrink];
    vv /= shrinkFactor[currentShrink];

    nh = h / shrinkFactor[currentShrink];
    nw = w / shrinkFactor[currentShrink];

    if (nh == 0 && h != 0) {
	nh = 1;
    }
    if (nw == 0 && w != 0) {
	nw = 1 ;
    }

    put_rectangle(hh, vv - nh, nw, nh, forepix);
}

/*
 *	The code to handle the \specials generated by tpic was modified
 *	by Dirk Grunwald using the code Tim Morgan at Univ. of Calif, Irvine
 *	wrote for TeXsun.
 */

#define	COMLEN	4

void applicationDoSpecial(cp)
char *cp;
{
    char command[COMLEN], *orig_cp;
    register int len;

    orig_cp = cp;
    while (isspace(*cp)) ++cp;
    len = 0;
    while (!isspace(*cp) && *cp && len < COMLEN-1) command[len++] = *cp++;
    command[len] = '\0';
    if (strcmp(command, "pn") == 0) set_pen_size(cp);
    else if (strcmp(command, "fp") == 0) flush_path();
    else if (strcmp(command, "da") == 0) flush_dashed(cp, 0);
    else if (strcmp(command, "dt") == 0) flush_dashed(cp, 1);
    else if (strcmp(command, "pa") == 0) add_path(cp);
    else if (strcmp(command, "ar") == 0) arc(cp);
    else if (strcmp(command, "sp") == 0) flush_spline();
    else if (strcmp(command, "sh") == 0) shade_last();
    else if (strcmp(command, "wh") == 0) whiten_last();
    else if (strcmp(command, "bk") == 0) blacken_last();
    else fprintf(stderr, "[%s] special \"%s\" not implemented\n",
		ProgName, orig_cp);
}

/* Things we need from dvi_draw, unfortunately */
extern int pen_size, blacken, whiten, shade;

#define	toint(x)	((int) ((x) + 0.5))
#define	xconv(x) toint((specialConv*(x)+dviHH)/shrinkFactor[currentShrink])
#define	yconv(y) toint((specialConv*(y)+dviVV)/shrinkFactor[currentShrink])


/*
 * Draw a line from (fx,fy) to (tx,ty).
 * Right now, we ignore pen_size.
 */
void line_btw(fx, fy, tx, ty)
int fx, fy, tx, ty;
{
    if (buildToPixmap) {
	XDrawLine(DISP, pasteUpPixmap[currentShrink][currentLeaf],
		   specialGC, xconv(fx), yconv(fy), xconv(tx), yconv(ty));
    } else {
	int offset = currentLeaf * leaf_w[currentShrink];
	XDrawLine(DISP, win,
		   specialGC,
		  xconv(fx) + offset, yconv(fy),
		  xconv(tx) + offset, yconv(ty));
    }
}

/*
 * Draw a dot at (x,y)
 */
void dot_at(x, y)
{
    line_btw(x,y,x+1,y);
}

/*
 * Apply the requested attributes to the last path (box) drawn.
 * Attributes are reset.
 */
void do_attribute_path(last_min_x, last_max_x, last_min_y, last_max_y)
int last_min_x, last_max_x, last_min_y, last_max_y;
{

#ifdef UNDEF
/*
 *	This is the code as it appears in TeXsun. I dont have the time to
 *	make this do the shading in X-11. If someone does this, please post
 *	diffs to the net.
 *
 *	Dirk Grunwald
 */

    static struct pixrect *shade_pr;
 
    if (last_min_x < last_max_x && last_min_y < last_max_y) {
        if (whiten) {
	    do_op(last_min_x, last_min_y,
		  xconv(last_max_x) - xconv(last_min_x),
		  yconv(last_max_y) - yconv(last_min_y),
		  PIX_SRC, (struct pixrect *) 0);
        }
        else if (blacken) {
	    do_op(last_min_x, last_min_y,
		xconv(last_max_x) - xconv(last_min_x),
		yconv(last_max_y) - yconv(last_min_y),
                PIX_NOT(PIX_SRC), (struct pixrect *) 0);
        }
        else if (shade) {
            if (!shade_pr) {
                shade_pr = mem_create(3, 3, 1);
                if (!shade_pr) Fatal("Out of memory -- cannot create pixrect");
                pr_put(shade_pr, 0, 0, 1);
            }
	    if (buildToPixmap) {
		pr_replrop(pasteUpPixmap[currentShrink][currentLeaf],
		    xconv(last_min_x) + pen_size,
		    yconv(last_min_y) + pen_size,
		    xconv(last_max_x) - xconv(last_min_x) - pen_size,
		    yconv(last_max_y) - yconv(last_min_y) - pen_size,
		    PIX_SRC, shade_pr, 0, 0);
	    }
	    else {
		int offset = currentLeaf*leaf_w[currentShrink]
		pw_replrop(ptube,
		    xconv(last_min_x) + pen_size + offset,
		    yconv(last_min_y) + pen_size,
		    xconv(last_max_x) - xconv(last_min_x) - pen_size + offset,
		    yconv(last_max_y) - yconv(last_min_y) - pen_size,
		    PIX_SRC, shade_pr, 0, 0);
	    }
        }
    }
    shade = blacken = whiten = FALSE;
#endif
}

int
mainLoop()
{
    int ch, arg, sign, number;
    XEvent event;
    XWindowAttributes query;

    char trbuf[100];
    char *string;
    int nbytes=0;

    int repaint;
    int rebuild;

#define BOGUSDIR -1
#define FOREWARD 0
#define BACKWARD 1
#define ABSOLUTE 2

    int direction;

/*
 *
 *	We need to initialized 'obscured'.
 *	Ideally, we would  like to know if we are covered at all, but
 *	the best I can come up with is a way to determine if we're
 *	mapped. This works well for wm, uwm & no window manager at all.
 */

    XClearWindow(DISP,win);
    XSync(DISP, 0);

    XGetWindowAttributes(DISP, win, &query);
    obscured = ! (query.map_state == IsViewable);

    allocatePixmaps();
    dviCurrentPage = 0;
    buildLeaf(LEAF_LEFT, 0, SHRINK_NORMAL);

    if (Leaves == 2) {
	XGetWindowAttributes(DISP, win, &query);	/* status changed? */
	obscured = ! (query.map_state == IsViewable);
	buildLeaf(LEAF_RIGHT, 1, SHRINK_NORMAL);
    }

    arg = 0;
    number = 0;

    for (;;) {

	repaint = 1;
	rebuild = 0;
	direction = BOGUSDIR;

	XDefineCursor(DISP, win, readyCursor);

	bzero(&event, sizeof(event));
	XNextEvent (DISP, &event);

	string = "";
	
	switch (event.type) {

/*
 *	If were painting to the display, and were not completely
 *	visible, wed better really paint to the pixmap, otherwise,
 *	all the stuff painted in an obscured region wont be in the pixmap.
 */

	case VisibilityNotify:
	    obscured = (event.xvisibility.state != VisibilityUnobscured);
	    continue;
	    
	case Expose:
            if ( event.xexpose.count > 0 ) break; /* no more flicker!
                                                   * [eichin:19880328.1332EST]
                                                   */
	    string = "\f";
	    nbytes = 1;
	    break;
	    
	    
	case ButtonPress:
	{
	    int leaf;
	    int otherLeaf;
	    int normalLeafW = leaf_w[SHRINK_NORMAL];
	    int leafX;
	    int leafY;
	    int lastLeaf = -1;
	    
	    int x = event.xbutton.x;
	    int y = event.xbutton.y;
	    int lastButton = event.xbutton.button;
	    
/*
 *	Determine which leaf was pointed to & where we will display it
 */
	    leaf = x / normalLeafW;
	    if (leaf < 0 || leaf >= Leaves) {
		fprintf(stderr,"[%s] error in button press, leaf = %d\n",
			ProgName, leaf);
		exit(1);
	    }
	    
	    if (leaf != lastLeaf && lastLeaf != -1) {
		displayLeaf(lastLeaf);
	    }
	    lastLeaf = leaf;
	    
	    if (Leaves == 1) {
		otherLeaf = leaf;
	    } else {
		otherLeaf = 1 - leaf;
	    }
	    
/*
 *	Build the enlarged page if its not already there
*/
	    if (haveLargePage[leaf] != pageOnLeaf[leaf]) {
		buildLeaf(leaf, pageOnLeaf[leaf], SHRINK_LARGE);
		haveLargePage[leaf] = pageOnLeaf[leaf];
	    }
	    
/*
 *	Determine where he was looking, build a (x,y) such that
 *	the thing he was pointing at will be on the screen, as close to
 *	center as possible.
 */

	    leafX = (x - (leaf * normalLeafW));
	    leafX = (leafX * shrinkFactor[SHRINK_NORMAL])
		/ shrinkFactor[SHRINK_LARGE];
	    
	    leafX -= (leaf_w[SHRINK_NORMAL] / 2);
	    leafX = MAX(leafX, 0);
	    leafX = MIN(leafX, leaf_w[SHRINK_LARGE]);
	    
	    leafY = (y * shrinkFactor[SHRINK_NORMAL])
		/ shrinkFactor[SHRINK_LARGE];
	    
	    leafY -= (page_h[SHRINK_NORMAL] / 2);
	    leafY = MAX(leafY, 0);
	    leafY = MIN(leafY, page_h[SHRINK_LARGE]);
	    
/*
 *	Display the enlarged image on the leaf, wait for another
 *	button event (the release, we hope)& then restore the image as it was
 *      While we wait, we move the picture around according to the
 *      users wishes.
 */
	    clearWindow(otherLeaf);
	    XCopyArea(DISP, pasteUpPixmap[SHRINK_LARGE][leaf],
		      win, forepix,
		      leafX, leafY,
		      leaf_w[SHRINK_NORMAL], page_h[SHRINK_NORMAL],
		      leaf_w[SHRINK_NORMAL] * otherLeaf, 0);
	    
	    bzero(&event, sizeof(event));
	    for(;;) {
		XNextEvent(DISP, &event);
		if (event.type == ButtonRelease
		    && event.xbutton.button == lastButton) {
		    break;
		}
		if (event.type == MotionNotify) {
		  int leaf;
		  int otherLeaf;
		  int normalLeafW = leaf_w[SHRINK_NORMAL];
		  int leafX;
		  int leafY;
		  int lastLeaf = -1;
		  
		  int x = event.xbutton.x;
		  int y = event.xbutton.y;
		  int lastButton = event.xbutton.button;
		  
		  /*
		   *	Determine which leaf was pointed to & where we will display it
		   */
		  leaf = x / normalLeafW;
		  if (leaf < 0 || leaf >= Leaves) {
		    fprintf(stderr,"[%s] error in button press, leaf = %d\n",
			    ProgName, leaf);
		    exit(1);
		  }
		  
		  lastLeaf = leaf;
		  
		  if (Leaves == 1) {
		    otherLeaf = leaf;
		  } else {
		    otherLeaf = 1 - leaf;
		  }
		  
		  /*
		   *	Determine where he was looking, build a (x,y) such that
		   *	the thing he was pointing at will be on the screen, as close to
		   *	center as possible.
		   */
		  
		  leafX = (x - (leaf * normalLeafW));
		  leafX = (leafX * shrinkFactor[SHRINK_NORMAL])
		   / shrinkFactor[SHRINK_LARGE];
		  
		  leafX -= (leaf_w[SHRINK_NORMAL] / 2);
		  leafX = MAX(leafX, 0);
		  leafX = MIN(leafX, leaf_w[SHRINK_LARGE]);
		  
		  leafY = (y * shrinkFactor[SHRINK_NORMAL])
		   / shrinkFactor[SHRINK_LARGE];
		  
		  leafY -= (page_h[SHRINK_NORMAL] / 2);
		  leafY = MAX(leafY, 0);
		  leafY = MIN(leafY, page_h[SHRINK_LARGE]);
		  
		  /*
		   *	Display the  new enlarged image on the leaf.
		   */
		  clearWindow(otherLeaf);
		  XCopyArea(DISP, pasteUpPixmap[SHRINK_LARGE][leaf],
			    win, forepix,
			    leafX, leafY,
			    leaf_w[SHRINK_NORMAL], page_h[SHRINK_NORMAL],
			    leaf_w[SHRINK_NORMAL] * otherLeaf, 0);
	      }
	    }
	    displayLeaf(otherLeaf);
	    break;
	}

	case ButtonRelease:
	    break;
	    
	case KeyPress:
	    string = trbuf;
	    nbytes = XLookupString(&event, string, 100, NULL, NULL);
	    
	    break;
	}

	if (nbytes == 0) continue;

	if (nbytes > 1) goto bad;

	switch (ch = *string) {
	case 'q':
	case '\003':	/* control-C */
	case '\004':	/* control-D */
	    stop_output(0);
	    break;

	case 'n':
	case 'f':
	case ' ':
	case '\n' :
	case '\r' :
	    /* scroll forward */
	    direction = FOREWARD;
	    rebuild = 1;
	    break;

	case 'p':
	case 'b':
	case '\b':
	case 0177 : /* DEL */
	    /* scroll backward */
	    direction = BACKWARD;
	    rebuild = 1;
	    break;

	case 'R' :
	    reReadFile();
	    break;

	case '\f':
	case 'r' :
	    /* repaint current page */
	    repaint = 1;
	    break;

	case 'g':
	    /* go to absolute page */
	    direction = ABSOLUTE; /* may not be, but tough */
	    rebuild = 1;
	    break;

	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	    if (! arg) {
		arg = 1;
		sign = 1;
		number = 0;
	    }
	    number = 10*number + sign*(ch - '0');

	    repaint = 0;
	    continue;

	case '-':
	    if (! arg) {
		arg = 1;
		sign = -1;
		number = 0;
		repaint = 0;
		continue;
	    } else
		goto bad;
	default:
	    goto bad;
	}

	if (rebuild && direction != BOGUSDIR) {
	    int newLeft;
	    int newRight;
/*
 *	canonicalize the request. arg is always > 0, were moving in
 *	some direction.
 */

	    if (!arg || number == 0) {
		number = 1;
	    }


	    if (number < 0) {
		if (direction == FOREWARD) {
		    direction = BACKWARD;
		} else if (direction == BACKWARD){
		    direction = FOREWARD;
		}
		number = -number;
	    }

/* 
 *	Turn pages back
 */
 	    if (direction == BACKWARD) {
		newLeft = pageOnLeaf[LEAF_LEFT] - number;
	    } else if (direction == FOREWARD) {
		newLeft = pageOnLeaf[LEAF_LEFT] + number;
	    } else {
		newLeft = number - 1;	/* pages numbered at 0 */
	    }

	    newLeft = MIN(newLeft, dviTotalPages-1);
	    newLeft = MAX(newLeft, 0);

	    if (Leaves == 1) {
		if (newLeft != pageOnLeaf[LEAF_LEFT]) {
		    buildLeaf(LEAF_LEFT, newLeft, SHRINK_NORMAL);
		}
	    } else {
		newRight = newLeft + 1;
		
		if (pageOnLeaf[LEAF_LEFT] != newLeft) {
		    if (newLeft == pageOnLeaf[LEAF_RIGHT]) {
			swapLeaf(LEAF_RIGHT, LEAF_LEFT);
			displayLeaf(LEAF_LEFT);
		    } else if (newRight == pageOnLeaf[LEAF_LEFT]) {
			swapLeaf(LEAF_LEFT, LEAF_RIGHT);
			displayLeaf(LEAF_RIGHT);
		    }
		    
		    if (pageOnLeaf[LEAF_LEFT] != newLeft) {
			buildLeaf(LEAF_LEFT, newLeft, SHRINK_NORMAL);
		    }
		    
		    if (pageOnLeaf[LEAF_RIGHT] != newRight) {
			buildLeaf(LEAF_RIGHT, newRight, SHRINK_NORMAL);
		    }
		}
	    }
	    repaint = 0;
	    rebuild = 0;
	    arg = 0;
/*
 *	Only repaint it if we need to
 */
	}

	if (repaint){
	    displayLeaves();
	    repaint = 0;
	}

	continue;

    bad:
	
	arg = 0;		/* throw away numeric argument */
	continue;

    }
}

stop_output(sig)
{
    dviFini();
    exit(sig);
}

put_border(x, y, w, h, t)
int x, y, w, h, t;
{
    put_rectangle(x, y, w, t, highpix);
    put_rectangle(x, y, t, h, highpix);
    put_rectangle(x, y + h - t, w, t, highpix);
    put_rectangle(x + w - t, y, t, h, highpix);
}

put_rectangle(x, y, w, h, pix)
int x, y, w, h;
GC pix;
{
    if (buildToPixmap) {
	XFillRectangle(DISP, pasteUpPixmap[currentShrink][currentLeaf],
		       pix, x, y, w, h);
    } else {
	XFillRectangle(DISP, win,
		       pix, x + currentLeaf * leaf_w[currentShrink], y, w, h);
    }
}


