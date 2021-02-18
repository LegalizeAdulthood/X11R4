/*
 * Copyright 1989 Dirk Grunwald
 *
 *	Alan Kent, Dept. of Computer Science, RMIT, Melbourne, Aus.
 *	added the th first version of marking and printing marked pages,
 *	as well as the ``goto page'' menu.
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
 */

#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sun/fbio.h>

#include <sunwindow/defaults.h>
#include <suntool/sunview.h>
#include <suntool/canvas.h>
#include <suntool/scrollbar.h>
#include <suntool/wmgr.h>

#include "dvistuff.h"
/*
 *	These constants may need to be changed on your implementation
 */
static char *rcsid="$Header: /home/reed/grunwald/Uiuctex/drivers/RCS/texsun.c,v 1.1 89/04/15 18:15:36 grunwald Exp Locker: grunwald $" ;

/*
 *	Note that ROUNDUP evaluates Y twice.
 */
#define ROUNDUP(x,y) (((x)+(y)-1) / (y))

#define DEBUG printf

#define INFORM (Debug > 2)
#define TALK (Debug > 1)
#define VERBOSE (Debug > 0)

int Debug = 0;
static int  obscured = 1;

/*
 *	Check to see if we're running SunView 4.0 -- only there do they
 *	define the ACTION commands.
 */

#ifdef ACTION_NULL_EVENT
#define SUNOS4
#endif


/*
 *	SunTools related variables
 */

static int rootFd;

static Window frame = 0;
static char frameLabel[256];
static char *frameLabelName;

/*
 *	TeXSun Icon
 */

#define MY_ICON_WIDTH	64
#define MY_ICON_WIDTH	64
static short ic_image[256] = {
#include "texsun.icon"
};
mpr_static(texsunIcon_mpr, 64, 64, 1, ic_image);
static	struct icon icon = {64, 64, (struct pixrect *)NULL, 0, 0, 64, 64,
	    &texsunIcon_mpr, 0, 0, 0, 0, NULL, (struct pixfont *)NULL,
	    ICON_BKGRDCLR};

static int clearMode;
static int setMode;
static int copyMode;
static int paintMode;
static int cursorMode;

/*
 *	Menu related things
 */

Menu menuMain;
Menu menuFrame;
Menu menuGoto;
Menu menuPrint;
Menu menuDocument;

char *M_MAIN_FORWARD = "Next Page";
char *M_MAIN_2FORWARD = "Skip 2 Pages";
char *M_MAIN_BACKWARD = "Back a Page";
char *M_MAIN_2BACKWARD = "Back 2 Pages";
char *M_MAIN_QUIT = "Quit";
char *M_MAIN_GOTO = "Goto Page";
char *M_MAIN_DOCUMENT = "Document";
char *M_MAIN_PRINT = "Mark & Print";

char *M_DOCUMENT_AGAIN = "Reload File";
char *M_DOCUMENT_INSPECT = "Detail View";
char *M_DOCUMENT_OVERVIEW = "Overview";
char *M_DOCUMENT_EXPOSE = "Expose";
char *M_DOCUMENT_ZOOM = "Zoom";
char *M_DOCUMENT_UNZOOM = "Un-Zoom";

char *M_PRINT_MARK = "Mark Page";
char *M_PRINT_LMARK = "Mark Left";
char *M_PRINT_RMARK = "Mark Right";
char *M_PRINT_UNMARK = "Unmark Page";
char *M_PRINT_LUNMARK = "Unmark Left";
char *M_PRINT_RUNMARK = "Unmark Right";
char *M_PRINT_MARKALL = "Mark all";
char *M_PRINT_UNMARKALL = "Unmark all";
char *M_PRINT_PRALL = "Print All";
char *M_PRINT_PRMARKED = "Print Marked";
char *M_PRINT_PRUNMARKED = "Print Unmarked";

#define MV_MAIN_FORWARD		1
#define MV_MAIN_BACKWARD	2
#define MV_MAIN_2FORWARD	3
#define MV_MAIN_2BACKWARD	4
#define MV_MAIN_QUIT		11
#define MV_MAIN_GOTO		12
#define MV_MAIN_PRINT		20

#define MV_PRINT_PRALL		100
#define MV_PRINT_PRMARKED	101
#define MV_PRINT_PRUNMARKED	102
#define MV_PRINT_MARKALL	103
#define MV_PRINT_UNMARKALL	104
#define MV_PRINT_LMARK		113
#define MV_PRINT_RMARK		114
#define MV_PRINT_LUNMARK	115
#define MV_PRINT_RUNMARK	116

#define MV_DOCUMENT_AGAIN		205
#define MV_DOCUMENT_INSPECT		206
#define MV_DOCUMENT_OVERVIEW		207
#define MV_DOCUMENT_EXPOSE		208
#define MV_DOCUMENT_ZOOM		209
#define MV_DOCUMENT_UNZOOM		210

/*
 *	Scroll-bar related things.
 */
Scrollbar scrollLeft;
Scrollbar scrollBottom;

/*
 *	Variables use to keep track of where we are in the document
 */

static int globalArg = 0;
static int globalNumber = 0;
static int globalSign = 1;
static int globalLeaf = 0;
static int globalLeafX;
static int globalLeafY;
static Event globalEvent;
static int globalDisplayState;
static Canvas globalCanvas;
    
#define BOGUSDIR -1
#define FOREWARD 0
#define BACKWARD 1
#define ABSOLUTE 2
static int globalDirection;

static int autoReload = 0;
static int autoFit = 1;

static int firstPage = 0;

static Cursor workingCursor;
static Cursor readyCursor;

static char *texsunPrint;
static char *defaultTexsunPrint = "dvipslpr";	/* DEFAULT */

short workingCursorData[] = {
#include <images/hglass.cursor>
};
mpr_static(workingCursorPixrect, 16, 16, 1, workingCursorData);

short readyCursorData[] = {
#include <images/bullseye.cursor>
};
mpr_static(readyCursorPixrect, 16, 16, 1, readyCursorData);

/*
 *	TeX-Dvi related variables
 */

static int rawDviHeight;
static int rawDviWidth;	/* part of X interface, not dvistuff */
static int maxHeight;
static int maxWidth;
static int maxDepth;

static int  screenWidth, screenHeight;

#define MAX_LEAVES	2
#define LEAF_LEFT	0
#define LEAF_RIGHT	1

#define DEFAULT_LEAVES	2

#define SHRINK_NORMAL	0
#define SHRINK_LARGE	1
#define MAX_SHRINKS	2

/*
 *	Canvas for looking at the document
 */

static Canvas normalCanvas[MAX_LEAVES];
static Canvas largeCanvas;

static Pixwin *pasteUpPixwin[MAX_SHRINKS][MAX_LEAVES];
static int largePixwinsAllocated = 0;

/*
 *	Routines associated with the canvas
 */

void canvasSelected();

typedef (*intfuncp)();
int selectLarge();
int selectOne();
int selectTwo();
static intfuncp whichCanvasSelected[MAX_LEAVES] = {selectOne, selectTwo};

void largeCanvasRepaint();
void largeCanvasResize();

static int Leaves;
static int currentLeaf = 0;
static int pageOnLeaf[MAX_LEAVES] = {-1,-1};
static int havePage[MAX_SHRINKS][MAX_LEAVES] = {0, 0, 0, 0};

static int page_w[MAX_SHRINKS], page_h[MAX_SHRINKS];
static int leaf_w[MAX_SHRINKS];

/*
 *	Note: these are used by dvi_draw.c, and are not static
 */
int shrinkFactor[MAX_SHRINKS];
int currentShrink;

static struct glyph **shrunkenGlyphs[MAX_SHRINKS][MAX_FONTFAMILY];

typedef struct mpr_data MprData;
static Pixrect **shrunkenPixrect[MAX_SHRINKS][MAX_FONTFAMILY];

static int reverse = 0;

static double specialConv;

char **globalArgv;
int globalArgc;

/*
 *	Subroutine declerations
 */

char *malloc(), *calloc(), *index();
static void buildLeaf();

#define MAXFUNC(a,b) ((a) < (b)) ? (b) : (a)
#define MINFUNC(a,b) ((a) < (b)) ? (a) : (b)
#define ABSFUNC(a) ( (a) < 0 ? (-(a)) : (a))
#define SWAPINT(a,b) {int xxxtmp = a; a = b; b = xxxtmp;}

stop_output()
{
    window_set(frame, FRAME_NO_CONFIRM, TRUE, 0);
    window_destroy (frame);
    dviFini();
    exit (0);
}

main(argc, argv)
    int argc;
    char **argv;
{
    int xargc=argc;
    char **xargv=argv;
    int i;

    char *display = NULL;
    char option[120];
    int optionEnd;
    char *poption;

    char *rootFdName[WIN_NAMESIZE];
    
    int bwidth = 2;
    char *geometry = NULL, def[32];
    
    int bdrpix, mouspix;
    unsigned long xswattrs_mask;

    double topMargin;
    double sideMargin;

    struct rect frame_rect;

    double atof();
    char *getenv();

    char *ptr;
    
/*
 *	Choose an explicit name so we always find the options.
 */

    ProgName = "texsun";
    argv++;
    argc--;
    dviFileName = NULL;

    strcpy(option, ProgName);
    sprintf(option,"/%s/", ProgName);
    optionEnd = strlen(option);
    option[optionEnd] = 0;
    reverse = (int) defaults_get_boolean(strcat(option,"ReverseVideo"), 0, 0);

    option[optionEnd] = 0;
    shrinkFactor[SHRINK_NORMAL] =
	defaults_get_integer(strcat(option,"NormalShrink"), 0, 0);

    option[optionEnd] = 0;
    shrinkFactor[SHRINK_LARGE] =
	defaults_get_integer(strcat(option,"LargeShrink"), 0, 0);

    option[optionEnd] = 0;
    dviDPI = defaults_get_integer(strcat(option,"Dpi"), DEFAULT_DPI, 0);

    option[optionEnd] = 0;
    Leaves = defaults_get_integer(strcat(option,"Leaves"), 0, 0);

    option[optionEnd] = 0;
    poption = defaults_get_string(strcat(option,"TopMargin"), 0, 0);
    topMargin = ( (poption == 0) ? 1.0 : atof(poption));

    option[optionEnd] = 0;
    poption = defaults_get_string(strcat(option,"SideMargin"), 0, 0);
    sideMargin = ( (poption == 0) ? 1.0 : atof(poption));

    option[optionEnd] = 0;
    dviBlackness = defaults_get_integer(strcat(option,"Blackness"),
					DEFAULT_BLACKNESS, 0);
    optionEnd = strlen(option);
    option[optionEnd] = 0;
    autoReload = (int) defaults_get_boolean(strcat(option,"AutoReload"), 0, 0);

    optionEnd = strlen(option);
    option[optionEnd] = 0;
    autoFit = (int) defaults_get_boolean(strcat(option,"AutoFit"), autoFit, 0);

    optionEnd = strlen(option);
    option[optionEnd] = 0;
    texsunPrint = (char *) defaults_get_string(strcat(option,"TeXsunPrint"), texsunPrint, 0);

    globalArgc = argc;
    globalArgv = argv;

    while (argc) {
	if (strcmp(*argv, "-rv") == 0) {
	    reverse = !reverse;
	} else if (strcmp(*argv, "-rvOn") == 0 ) {
	    reverse = 1;
	} else if (strcmp(*argv, "-rvOff") == 0) {
	    reverse = 0;
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
	} else if (strcmp(*argv,"-dpi") == 0 && argc > 1) {
	    argv++;
	    argc--;
	    dviDPI = atoi(*argv);
	} else if (strcmp(*argv,"-tm") == 0 && argc > 1) {
	    argv++;
	    argc--;
	    topMargin = atof(*argv);
	} else if (strcmp(*argv,"-sm") == 0 && argc > 1) {
	    argv++;
	    argc--;
	    sideMargin = atof(*argv);
	} else if (strcmp(*argv, "-bl") == 0 && argc > 1) {
	    argv++;
	    argc--;
	    dviBlackness = atoi(*argv);
	} else if (strcmp(*argv, "-pg") == 0 && argc > 1) {
	    argv++;
	    argc--;
	    firstPage = atoi(*argv);
	} else if (strcmp(*argv,"-autoReload") == 0) {
	    autoReload = ! autoReload;
	} else if (strcmp(*argv,"-autoFit") == 0) {
	    autoFit = !autoFit;
	} else if (strcmp(*argv,"-debug") == 0 && argc > 1) {
	    argv++;
	    argc--;
	    Debug = atoi(*argv);
	} else if (**argv != '-') {
	    dviFileName = *argv;
	} else {
	usage:
	    fprintf(stderr, "Usage: %s	[-ns <shrink>] [-ls <shrink>] [-dpi <dots>] \n",
		    ProgName);
	    fprintf(stderr, "		[-l <leaves>] [-rv] [-tm <inches>] [-sm <inches>]  dviFile\n");
	    exit(1);
	}
	argv++;
	argc--;
    }

    if (dviFileName == NULL)
	goto usage;

/*
 *	Set things which depend on DPI
 */

    dviHHMargin = dviDPI * sideMargin;
    dviVVMargin = dviDPI * topMargin;

    if (INFORM) {
	fprintf(stderr, "[texsun] call dviInit();\n");
    }

    if (dviInit()) {
	dviFini();
	exit(1);
    }

    if (TALK) {
	fprintf(stderr,"page is %d wide, %d high\n",
		dviTallestPage, dviWidestPage);
    }

    if (((double) dviTallestPage * (double) dviWidestPage) > 4.0e7) {
	fprintf(stderr,"[%s] Warning: Your page size is %d wide and %d tall,\n",
		ProgName, dviTallestPage, dviWidestPage);
	fprintf(stderr,"which may be too big to be displayed\n");
    }

    specialConv = (double) dviDPI / 1000.0;
    
/*
 *	If no shrink factor was chosen, pick one which will work out nicely
 *	on their display
 */
    if (INFORM) {
	fprintf(stderr, "[texsun] set screenSpecific\n");
    }

    setScreenSpecifics();
    
    rawDviHeight = dviTallestPage + 2 * dviVVMargin;
    rawDviWidth  = dviWidestPage + 2 * dviHHMargin;
    
    if (Leaves == 0) {
	if (dviTotalPages == 1) {
	    Leaves = 1;
	} else {
	    Leaves = DEFAULT_LEAVES;
	}
    }

    if (INFORM) {
	fprintf(stderr, "[texsun] choose screenSpecific\n");
    }

    if (shrinkFactor[SHRINK_NORMAL] == 0) {
	int sH;
	int sW;
	int sH2;
	int sW2;
	int shrink2;
	
	sH = (rawDviHeight + maxHeight - 1) / maxHeight;
	sW = (Leaves * rawDviWidth  + maxWidth - 1) / maxWidth;
	
	shrinkFactor[SHRINK_NORMAL] = MAXFUNC(sW, sH);
	shrinkFactor[SHRINK_NORMAL] = MAXFUNC(shrinkFactor[SHRINK_NORMAL], 1);
	
/*
 *	Check to see if we can get another shrink size bigger display
 *	if we cut the margins.
 */
	
	sH2 = (dviTallestPage + (dviHHMargin/32) + maxHeight - 1)
	    / maxHeight;
	sW2 = ( Leaves * (dviWidestPage + (dviVVMargin/32))
	       + maxHeight - 1) / maxHeight;
	
	shrink2 = MAXFUNC(sH2, sW2);
	shrink2 = MAXFUNC(shrink2, 1);
	
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

    shrinkFactor[SHRINK_LARGE] = MINFUNC(shrinkFactor[SHRINK_LARGE],
				     shrinkFactor[SHRINK_NORMAL]-1);
    shrinkFactor[SHRINK_LARGE] = MAXFUNC(shrinkFactor[SHRINK_LARGE], 1);

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

    if (INFORM) {
	fprintf(stderr,"[texsun] create the frame windows\n");
    }

/*
 *	This is an assumption about the window environment.
 */
    
    (void)we_getparentwindow(rootFdName);
    if ((rootFd = open(rootFdName, 0)) < 0) {
	(void)fprintf(stderr, "%s: can't open root window %s\n",
		      ProgName, rootFdName);
	dviFini();
	exit(1);
    }

    /* Create the short file name by pointing to the last part of */
    /* the filename */

    frameLabelName = &dviFileName[ strlen(dviFileName) - 1 ];
    while( frameLabelName != dviFileName && *frameLabelName != '/' ) {
      frameLabelName--;
    }

    if (frameLabelName != dviFileName && *frameLabelName == '/') {
      frameLabelName++;
    }

/* create the frame */
    sprintf(frameLabel,"TeXsun : %s, (%4.2f by %4.2f)",
	    frameLabelName,
	    (float) ((float) rawDviWidth / ( (float) dviDPI)),
	    (float) ((float) rawDviHeight / ( (float) dviDPI)) );

    
    frame = window_create (0, FRAME,
			   FRAME_LABEL, frameLabel,
			   FRAME_ICON, &icon,
			   FRAME_DONE_PROC, stop_output,
			   FRAME_SUBWINDOWS_ADJUSTABLE, FALSE,
			   WIN_X, 0,
			   WIN_Y, 0,
			   WIN_WIDTH, page_w[SHRINK_NORMAL],
			   WIN_HEIGHT, page_h[SHRINK_NORMAL],
			  0);
    if (frame == 0) {
	fprintf(stderr,"Unable to create window\n");
	dviFini();
	exit(1);
    }

    if (reverse) {
	clearMode = PIX_NOT(PIX_COLOR(1) | PIX_CLR);
	setMode = PIX_NOT(PIX_COLOR(0) | PIX_SET);
	copyMode = PIX_SRC;
	paintMode = PIX_NOT(PIX_SRC) & PIX_DST;
	cursorMode = PIX_SRC ^ PIX_DST;
    } else {
	clearMode = PIX_COLOR(1) | PIX_CLR;
	setMode = PIX_COLOR(0) | PIX_SET;
	copyMode = PIX_SRC;
	paintMode = PIX_SRC | PIX_DST;
	cursorMode = PIX_SRC ^ PIX_DST;
    }


    readyCursor = cursor_create(CURSOR_IMAGE, &readyCursorPixrect,
				CURSOR_OP, cursorMode, 0);
    workingCursor = cursor_create(CURSOR_IMAGE, &workingCursorPixrect,
				  CURSOR_OP, cursorMode, 0);

    initMarks();

    menuPrint = menu_create(0);

    if (Leaves == 1) {
      menu_set( menuPrint,
	       MENU_STRING_ITEM, M_PRINT_MARK, MV_PRINT_LMARK,
	       MENU_STRING_ITEM, M_PRINT_UNMARK, MV_PRINT_LUNMARK,
	       0);
    } else {
      menu_set( menuPrint,
	       MENU_STRING_ITEM, M_PRINT_LMARK, MV_PRINT_LMARK,
	       MENU_STRING_ITEM, M_PRINT_RMARK, MV_PRINT_RMARK,
	       MENU_STRING_ITEM, M_PRINT_LUNMARK, MV_PRINT_LUNMARK,
	       MENU_STRING_ITEM, M_PRINT_RUNMARK, MV_PRINT_RUNMARK,
	       0);
    }
    menu_set( menuPrint,
	     MENU_STRING_ITEM, M_PRINT_MARKALL, MV_PRINT_MARKALL,
	     MENU_STRING_ITEM, M_PRINT_UNMARKALL, MV_PRINT_UNMARKALL,

	     MENU_STRING_ITEM, M_PRINT_PRALL, MV_PRINT_PRALL,
	     MENU_STRING_ITEM, M_PRINT_PRMARKED, MV_PRINT_PRMARKED,
	     MENU_STRING_ITEM, M_PRINT_PRUNMARKED, MV_PRINT_PRUNMARKED,
	     0);

    menuDocument =
      menu_create(
		  MENU_STRING_ITEM, M_DOCUMENT_INSPECT, MV_DOCUMENT_INSPECT,
		  MENU_ITEM,
		  MENU_STRING, M_DOCUMENT_OVERVIEW,
		  MENU_VALUE, MV_DOCUMENT_OVERVIEW,
		  MENU_INACTIVE, TRUE,
		  0,
		  MENU_STRING_ITEM, M_DOCUMENT_AGAIN, MV_DOCUMENT_AGAIN,
		  0);
		  
			    
    menuFrame = (Menu) window_get(frame, WIN_MENU);
    if (Leaves == 1) {
	menuMain =
	  menu_create(
		      MENU_STRING_ITEM, M_MAIN_FORWARD, MV_MAIN_FORWARD,
		      MENU_STRING_ITEM, M_MAIN_BACKWARD, MV_MAIN_BACKWARD,
		      MENU_PULLRIGHT_ITEM, M_MAIN_GOTO, menuGoto,
		      MENU_PULLRIGHT_ITEM, M_MAIN_DOCUMENT, menuDocument,
		      MENU_PULLRIGHT_ITEM, M_MAIN_PRINT, menuPrint,
		      MENU_PULLRIGHT_ITEM, "Frame", menuFrame,
		      MENU_STRING_ITEM, M_MAIN_QUIT, MV_MAIN_QUIT,
		      MENU_INITIAL_SELECTION, MENU_SELECTED,
		      MENU_INITIAL_SELECTION_SELECTED, TRUE,
/*			MENU_JUMP_AFTER_SELECTION, TRUE, */
/*			MENU_JUMP_AFTER_NO_SELECTION, TRUE, */
		      0);
      } else {
	menuMain =
	  menu_create(
		      MENU_STRING_ITEM, M_MAIN_FORWARD, MV_MAIN_FORWARD,
		      MENU_STRING_ITEM, M_MAIN_2FORWARD, MV_MAIN_2FORWARD,
		      MENU_STRING_ITEM, M_MAIN_BACKWARD, MV_MAIN_BACKWARD,
		      MENU_STRING_ITEM, M_MAIN_2BACKWARD, MV_MAIN_2BACKWARD,
		      MENU_PULLRIGHT_ITEM, M_MAIN_GOTO, menuGoto,
		      
		      MENU_PULLRIGHT_ITEM, M_MAIN_DOCUMENT, menuDocument,
		      
		      MENU_PULLRIGHT_ITEM, M_MAIN_PRINT, menuPrint,
		      
		      MENU_PULLRIGHT_ITEM, "Frame", menuFrame,
		      MENU_STRING_ITEM, M_MAIN_QUIT, MV_MAIN_QUIT,
		      
		      MENU_INITIAL_SELECTION, MENU_SELECTED,
		      MENU_INITIAL_SELECTION_SELECTED, TRUE,
		      /*
			MENU_JUMP_AFTER_SELECTION, TRUE,
			MENU_JUMP_AFTER_NO_SELECTION, TRUE,
			*/
		      0);
    }

    allocatePixwins();

    for (i = 0; i < Leaves; i++) {
	window_fit(normalCanvas[i]);
    }
    window_fit(frame);

    globalDisplayState = SHRINK_NORMAL;
    dviCurrentPage = firstPage;

    for (i = 0; i < Leaves; i++) {
	pageOnLeaf[i] = firstPage + i;
    }

    displayLeaves();

    globalArg = 0;
    globalNumber = 0;

    window_main_loop (frame);

    /* terminate frame */
    stop_output(0);
}

/*
 *	Sun Screen-specific things
 */
setScreenSpecifics()
{
    struct fbtype FbType;
    int Fb;    

    Fb = open("/dev/fb", O_RDONLY, 0);
    
    if (Fb < 0) {
	perror("open");
	fprintf(stderr,"Unable to open /dev/fb\n");
	dviFini();
	exit(1);
    }
    if (ioctl(Fb, FBIOGTYPE, &FbType) == -1) {
	perror("ioctl");
	fprintf(stderr,"Unable to determine screen size\n");
	dviFini();
	exit(1);
    }

    maxWidth = FbType.fb_width;
    maxHeight = FbType.fb_height;
    maxDepth = FbType.fb_depth;

    close(Fb);
}
static void
clearLeaf(leaf, shrinkSize)
int leaf;
int shrinkSize;
{
    Pixwin *w;
    int i;

    if (leaf< 0 || leaf >= Leaves) {
	fprintf(stderr,"[%s] Reference to bogus leaf in clearLeaf: %d\n",
		ProgName, leaf);
	stop_output();
	exit(1);
    }

    if (shrinkSize < 0 || shrinkSize >= MAX_SHRINKS ) {
	fprintf(stderr,"[%s] reference to bogus shrink %d\n",
		ProgName, shrinkSize);
	stop_output();
	exit(1);
    }

    w = pasteUpPixwin[shrinkSize][leaf];

    i = pw_writebackground(w,
		       0, 0,
		       leaf_w[shrinkSize], page_h[shrinkSize],
		       clearMode);
    if (Debug) {
	printf("Clear the pixmap(%x) = %d\n", w, i);
    }
}

allocatePixwins()
{
    int i;
    
    largePixwinsAllocated = 0;
    
    for (i = 0; i < Leaves; i++ ) {
	normalCanvas[i] = window_create(frame, CANVAS,
					CANVAS_AUTO_SHRINK, FALSE,
					CANVAS_AUTO_EXPAND, FALSE,
					CANVAS_WIDTH, leaf_w[SHRINK_NORMAL],
					CANVAS_HEIGHT, page_h[SHRINK_NORMAL],
					WIN_WIDTH, leaf_w[SHRINK_NORMAL],
					WIN_HEIGHT, page_h[SHRINK_NORMAL],
					CANVAS_RETAINED, TRUE,
					WIN_EVENT_PROC, whichCanvasSelected[i],
					WIN_BOTTOM_MARGIN, 0,
					WIN_LEFT_MARGIN, 0,
					WIN_TOP_MARGIN, 0,
					WIN_RIGHT_MARGIN, 0,

					WIN_MENU, menuMain,

					WIN_CONSUME_KBD_EVENT,
					WIN_TOP_KEYS,

					WIN_CONSUME_KBD_EVENT,
					WIN_RIGHT_KEYS,

					WIN_CONSUME_KBD_EVENT,
					WIN_LEFT_KEYS,

					WIN_CONSUME_KBD_EVENT,
					WIN_ASCII_EVENTS,
					0);
	
	if (normalCanvas[i] == 0) {
	    fprintf(stderr,"Unable to create canvas\n");
	    dviFini();
	    stop_output();
	    exit(1);
	}

	pasteUpPixwin[SHRINK_NORMAL][i] =
	    (Pixwin*) canvas_pixwin(normalCanvas[i]);

	clearLeaf(i, SHRINK_NORMAL);
    
/*
 *	Lay out the windows
 */
	if (i > 0) {
	    window_set(normalCanvas[i],
		       WIN_RIGHT_OF, normalCanvas[i-1],
		       0);
	}
    }
}

allocateLargePixwins()
{
    int i;
    Pixwin *p;
    int jj;
    largeCanvas =  window_create(frame,
				 CANVAS,
				 CANVAS_AUTO_SHRINK, FALSE,
				 CANVAS_AUTO_EXPAND, TRUE,
				 CANVAS_WIDTH, leaf_w[SHRINK_LARGE],
				 CANVAS_HEIGHT, page_h[SHRINK_LARGE],
				 WIN_WIDTH, leaf_w[SHRINK_LARGE],
				 WIN_HEIGHT, page_h[SHRINK_LARGE],
				 CANVAS_RETAINED, TRUE,
				 WIN_EVENT_PROC, selectLarge,
				 WIN_VERTICAL_SCROLLBAR, scrollbar_create(0),
				 WIN_HORIZONTAL_SCROLLBAR, scrollbar_create(0),
				 WIN_BOTTOM_MARGIN, 0,
				 WIN_LEFT_MARGIN, 0,
				 WIN_TOP_MARGIN, 0,
				 WIN_RIGHT_MARGIN, 0,
				 WIN_SHOW, FALSE,

				 WIN_MENU, menuMain,
				 
				 WIN_CONSUME_KBD_EVENT,
				 WIN_TOP_KEYS,
				 
				 WIN_CONSUME_KBD_EVENT,
				 WIN_RIGHT_KEYS,
				 
				 WIN_CONSUME_KBD_EVENT,
				 WIN_LEFT_KEYS,
				 
				 WIN_CONSUME_KBD_EVENT,
				 WIN_ASCII_EVENTS,
				 0);
    window_fit(largeCanvas);

    p = (Pixwin*) canvas_pixwin(largeCanvas);

    for (jj = 0; jj < Leaves; jj++) {
	pasteUpPixwin[SHRINK_LARGE][jj] = p;
    }
    clearLeaf(0, SHRINK_LARGE);	/* its both leaves, really */
    largePixwinsAllocated = 1;
}

static void
copyLeaf(from, to)
int from;
int to;
{
    Canvas tempCan;
    Pixwin *tempWin;
    int i;
    int tempNum;

    if (to < 0 || to >= Leaves) {
	fprintf(stderr,"[%s] bogus to leaf %d in copyLeaf\n",
		ProgName, to);
	stop_output();
    }
    if (from < 0 || from >= Leaves) {
	fprintf(stderr,"[%s] bogus from leaf %d in copyLeaf\n",
		ProgName, from);
	stop_output();
    }

    pw_rop(pasteUpPixwin[SHRINK_NORMAL][to],
	   0, 0, leaf_w[SHRINK_NORMAL], page_h[SHRINK_NORMAL],
	   copyMode,
	   (pasteUpPixwin[SHRINK_NORMAL][from]) -> pw_prretained,
	   0,0);

    SWAPINT(pageOnLeaf[to], pageOnLeaf[from]);
    SWAPINT(havePage[SHRINK_LARGE][to], havePage[SHRINK_LARGE][from]);
    SWAPINT(havePage[SHRINK_NORMAL][to], havePage[SHRINK_NORMAL][from]);

    clearLeaf(from, globalDisplayState);
}

static void
buildLeaf(leaf, shrinkSize)
int leaf;
int shrinkSize;
{
    int i;
    int page;

    if (leaf < 0 || leaf >= Leaves) {
	fprintf(stderr,"[%s] bogus leaf %d in buildLeaf\n",
		ProgName, leaf);
	stop_output();
    }

    if (shrinkSize < 0 || shrinkSize >= MAX_SHRINKS ) {
	fprintf(stderr,"[%s] Bogus shrink size %d in buildLeaf\n",
		ProgName, shrinkSize);
	stop_output();
    }

    setCursor(workingCursor);

    if (shrinkSize == SHRINK_LARGE && !largePixwinsAllocated) {
	allocateLargePixwins();
    }

/*
 *	Determine if this is a valid page. If it's not, we'll just clear
 *	the particular leaf.
 */

    page = pageOnLeaf[leaf];

    if (page < 0 || page >= dviTotalPages) {
	clearLeaf(leaf, shrinkSize);
    } else {
	clearLeaf(leaf, shrinkSize);
	currentLeaf = leaf;
	currentShrink = shrinkSize;
	dviPreparePage(page);	/* may change dviCurrentPage */
	page = dviCurrentPage;
    }

    if (shrinkSize == SHRINK_LARGE) {
	for (i = 0; i < Leaves; i++) {
	    havePage[SHRINK_LARGE][i] = 0;
	}
    }
    pageOnLeaf[leaf] = dviCurrentPage;
    havePage[shrinkSize][leaf] = 1;

    setCursor(readyCursor);
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
	dviFini();
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

	if (shrunkenPixrect[shrink][key] == 0) {
	    int lth = sizeof(Pixrect *) * MAX_GLYPH;
	    Pixrect  **g;
	    
	    g = (Pixrect **) malloc( lth );
	    bzero(g, lth);
	    shrunkenPixrect[shrink][key] = g;
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
    Pixrect **thesePixrect;
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

	thesePixrect = shrunkenPixrect[shrink][key];
	if (thesePixrect != 0) {
	    for (i = 0; i < MAX_GLYPH; i++) {
		Pixrect *g;
		
		g = thesePixrect[i];
		
		if (g != 0) {
		    free(g -> pr_data);
		    free(g);
		    thesePixrect[i] = 0;
		}
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
    int key;
    Pixrect *image;
    
    key = dviCurrentFont -> family;

    g = shrunkenGlyphs[currentShrink][key][charCode];

    if (g == 0) {

	MprData *newMpr = (MprData *) malloc(sizeof(MprData));
	Pixrect *newPixrect = (Pixrect *) malloc(sizeof(Pixrect));
	extern struct pixrectops mem_ops;

	g = dviShrinkGlyph(dviCurrentFont -> f -> f_gly[charCode],
			   shrinkFactor[currentShrink],
			   shrinkFactor[currentShrink]);
	shrunkenGlyphs[currentShrink][key][charCode] = g;

	newMpr -> md_linebytes = SHRUNK_GLYPH_BYTES_WIDE(g);
	newMpr -> md_image = (short *) g -> g_raster;
	newMpr -> md_offset.x = 0;
	newMpr -> md_offset.y = 0;
	newMpr -> md_primary = 0;
	newMpr -> md_flags = 0;

	newPixrect -> pr_ops = &mem_ops;
	newPixrect -> pr_size.x = g -> g_width;
	newPixrect -> pr_size.y = g -> g_height;
	newPixrect -> pr_depth =1;
	newPixrect -> pr_data = (caddr_t) newMpr;
	shrunkenPixrect[currentShrink][key][charCode] = newPixrect;
    }

    if (g == 0 || !HASRASTER(g)) return;

    hh /= shrinkFactor[currentShrink];
    vv /= shrinkFactor[currentShrink];

    x = hh - g -> g_xorigin;
    y = vv - g -> g_yorigin;

    image = shrunkenPixrect[currentShrink][key][charCode];

    pw_rop(pasteUpPixwin[currentShrink][currentLeaf],
	   x,y,
	   g->g_width, g->g_height,
	   paintMode,
	   image,
	   0,0);
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

    nh = (h + shrinkFactor[currentShrink] - 1) / shrinkFactor[currentShrink];
    nw = w / shrinkFactor[currentShrink];

    if (nh == 0 && h != 0) {
	nh = 1;
    }
    if (nw == 0 && w != 0) {
	nw = 1 ;
    }

    put_rectangle(hh, vv - nh, nw, nh);
}

/*
 *	This code attempts to support the same set of specials
 *	as imagen1
 */ 
static char *
skipWhite(c)
char *c;
{
    while (c != 0 && *c != 0 && isspace(*c)) c++;
    return(c);
}

static char *
skipNonWhite(c)
char *c;
{
    while (c != 0 && *c != 0 && ! isspace(*c)) c++;
    return(c);
}

/*
 *	The code to handle the \specials generated by tpic was written by
 *	Tim Morgan at Univ. of Calif, Irvine
 */

#define	COMLEN	128

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
    else if (strcmp(command, "ps::[begin]") == 0) psfigBegin(cp);
    else if (strcmp(command, "ps::plotfile") == 0) 	/* do nothing */;
    else if (strcmp(command, "ps::[end]") == 0) 	/* do nothing */;
    else if (strcmp(command, "ps:plotfile") == 0)	/* do nothing */;
    else if (strcmp(command, "ps:") == 0)		/* do nothing */;
    else if (strcmp(command, "ps::") == 0) 		/* do nothing */;
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
    pw_vector(pasteUpPixwin[currentShrink][currentLeaf],
	      xconv(fx), yconv(fy), xconv(tx), yconv(ty),
	      paintMode, -1);
}


/*
 * Draw a dot at (x,y)
 */
void dot_at(x, y)
{
    line_btw(x,y,x+1,y);
}


static void do_op(x0,y0,x1,y1,op,rect)
int x0,y0,x1,y1,op;
struct pixrect *rect;
{
    pw_rop(pasteUpPixwin[currentShrink][currentLeaf],
	   xconv(x0) + pen_size, yconv(y0) + pen_size,
	   x1 - pen_size, y1 - pen_size,
	   op, rect, 0, 0);
}


/*
 * Apply the requested attributes to the last path (box) drawn.
 * Attributes are reset.
 */
    void do_attribute_path(last_min_x, last_max_x, last_min_y, last_max_y)
    int last_min_x, last_max_x, last_min_y, last_max_y;
{
    static struct pixrect *shade_pr;
    
    if (last_min_x < last_max_x && last_min_y < last_max_y) {
        if (whiten) {
	    do_op(last_min_x, last_min_y,
		  xconv(last_max_x) - xconv(last_min_x),
		  yconv(last_max_y) - yconv(last_min_y),
		  setMode, (struct pixrect *) 0);
        }
        else if (blacken) {
	    do_op(last_min_x, last_min_y,
		  xconv(last_max_x) - xconv(last_min_x),
		  yconv(last_max_y) - yconv(last_min_y),
		  PIX_NOT(setMode), (struct pixrect *) 0);
        }
        else if (shade) {
            if (!shade_pr) {
                shade_pr = mem_create(3, 3, 1);
                if (!shade_pr) Fatal("Out of memory -- cannot create pixrect");
                pr_put(shade_pr, 0, 0, 1);
            }
	    pw_replrop(pasteUpPixwin[currentShrink][currentLeaf],
		       xconv(last_min_x) + pen_size,
		       yconv(last_min_y) + pen_size,
		       xconv(last_max_x) - xconv(last_min_x) - pen_size,
		       yconv(last_max_y) - yconv(last_min_y) - pen_size,
		       copyMode, shade_pr, 0, 0);
        }
    }
    shade = blacken = whiten = FALSE;
}

/*
 * Draw the bounding box for a \psfig special.
 *
 * expected format of the command string is
 *
 * width height bbllx bblly bburx bbury
 *
 * *ll* means lower-left, *ur* means upper-right.
 *
 * We just draw the bounding box.
 */
psfigBegin(cp)
char *cp;
{
  int bbllx, bblly;
  int bburx, bbury;
  int width, height;

  sscanf(cp, " %d %d %d %d %d %d ",
	 &width, &height,
	 &bbllx, &bblly, &bburx, &bbury);

  bbllx = ( dviHH) / shrinkFactor[currentShrink];
  bblly = ( dviVV) / shrinkFactor[currentShrink];

  width = fromSP(width) / shrinkFactor[currentShrink];
  height = fromSP(height) / shrinkFactor[currentShrink];

  put_border( bbllx, bblly, width, height, 1);
}

/*
 *	Prepare everything for re-reading a .dvi file
 */
reReadFile()
{
    int i;
    int new_page_w[MAX_SHRINKS], new_page_h[MAX_SHRINKS];
    int new_leaf_w[MAX_SHRINKS];

    finiMarks();
    dviResetAll();

    if (dviInit()) {
	fprintf(stderr,"Unable to reload dvi file (%s), exiting\n",
		dviFileName);
	stop_output();
    }

    initMarks();

    dviCurrentPage = pageOnLeaf[0];

    rawDviHeight = dviTallestPage + 2 * dviVVMargin;
    rawDviWidth  = dviWidestPage + 2 * dviHHMargin;

    newFrameLabel();
    
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

    for (i = 0; i < Leaves; i++ ) {
	window_set(normalCanvas[i],
		   CANVAS_WIDTH, new_leaf_w[SHRINK_NORMAL],
		   CANVAS_HEIGHT, new_page_h[SHRINK_NORMAL],
		   0);

	pasteUpPixwin[SHRINK_NORMAL][i] =
	    (Pixwin*) canvas_pixwin(normalCanvas[i]);

	clearLeaf(i, SHRINK_NORMAL);
	
	if (largeCanvas != 0) {
	    window_set(normalCanvas[i],
		       CANVAS_WIDTH, new_leaf_w[SHRINK_NORMAL],
		       CANVAS_HEIGHT, new_page_h[SHRINK_NORMAL],
		       0);
	    pasteUpPixwin[SHRINK_LARGE][i] =
		(Pixwin *) canvas_pixwin(largeCanvas);
	    clearLeaf(i, SHRINK_LARGE);
	}
    }

    for (i = 0; i < Leaves; i++) {
	havePage[SHRINK_LARGE][i] = 0;
	havePage[SHRINK_NORMAL][i] = 0;
    }

    displayLeaves();

    globalArg = 0;
    globalNumber = 0;
}

displayLeaves()
{
    int i;
    if (globalDisplayState == SHRINK_NORMAL) {
	for (i = 0; i < Leaves; i++) {
	    if (!havePage[SHRINK_NORMAL][i]) {
		buildLeaf(i, SHRINK_NORMAL);
	    }
	}
	displayNormalShrink();
    }  else {
	if (!havePage[SHRINK_LARGE][globalLeaf]) {
	    buildLeaf(globalLeaf, SHRINK_LARGE);
	}
	displayLargeShrink();
    }
}

displayLargeShrink()
{
    int i;
    int sx;
    int sy;

/*
 *	Build the enlarged page if its not already there -- do this before
 *	we attempt to display it because were going to spend some time
 *	generating it.
 */

    for (i = 0; i < Leaves; i++) {
	window_set(normalCanvas[i],
		   WIN_SHOW, FALSE,
		   0);
	window_set(largeCanvas,
		   WIN_SHOW, FALSE,
		   0);
    }

    if (globalLeaf < 0 || globalLeaf >= Leaves) {
	fprintf(stderr,"globalLeaf is incorrectly set\n");
	stop_output();
    }

    menu_set(menu_find(menuMain,MENU_STRING, M_DOCUMENT_INSPECT,0),
	     MENU_INACTIVE, TRUE, 0);
    menu_set(menu_find(menuMain, MENU_STRING, M_DOCUMENT_OVERVIEW,0),
	     MENU_INACTIVE, FALSE, 0);

    if (autoFit) {
	Rect *rect = (Rect *) window_get(frame, WIN_SCREEN_RECT, 0);

	int w = leaf_w[SHRINK_LARGE]
	    + (int) scrollbar_get(SCROLLBAR,SCROLL_WIDTH)
		+ (int) window_get(frame, WIN_RIGHT_MARGIN)
		+ (int) window_get(frame, WIN_LEFT_MARGIN);

	int h = page_h[SHRINK_LARGE]
	    + (int) scrollbar_get(SCROLLBAR,SCROLL_WIDTH)
		+ (int) window_get(frame, WIN_TOP_MARGIN)
		+ (int) window_get(frame, WIN_BOTTOM_MARGIN);

	window_set(frame,
		   WIN_WIDTH, MINFUNC(rect -> r_width, w),
		   WIN_HEIGHT, MINFUNC(rect -> r_height, h),
		   0);
    }

    window_set(largeCanvas,
	       CANVAS_WIDTH, leaf_w[SHRINK_LARGE],
	       CANVAS_HEIGHT, page_h[SHRINK_LARGE],
	       WIN_SHOW, TRUE,
	       WIN_X, 0,
	       WIN_Y, 0,
	       WIN_HEIGHT, WIN_EXTEND_TO_EDGE,
	       WIN_WIDTH, WIN_EXTEND_TO_EDGE,
	       0);
}

displayNormalShrink()
{
    int i;

    for (i = 0; i < Leaves; i++) {
	window_set(largeCanvas,
		   WIN_SHOW, FALSE,
		   0);
    }

    window_set(normalCanvas[0],
	       WIN_X,0,
	       WIN_Y,0,
	       WIN_WIDTH, leaf_w[SHRINK_NORMAL],
	       WIN_HEIGHT, page_h[SHRINK_NORMAL],
	       0);
    for (i = 1; i < Leaves; i++) {
	window_set(normalCanvas[i],
		   WIN_RIGHT_OF, normalCanvas[i-1],
		   WIN_WIDTH, leaf_w[SHRINK_NORMAL],
		   WIN_HEIGHT, page_h[SHRINK_NORMAL],
		   0);
    }

    for (i = 0; i < Leaves; i++) {
	window_set(normalCanvas[i], WIN_SHOW, TRUE, 0);
    }

     if (autoFit) {
	window_fit(frame);
    }

    menu_set(menu_find(menuMain,MENU_STRING, M_DOCUMENT_OVERVIEW,0),
	     MENU_INACTIVE, TRUE, 0);
    menu_set(menu_find(menuMain, MENU_STRING, M_DOCUMENT_INSPECT,0),
	     MENU_INACTIVE, FALSE, 0);
}

/*
 *	SelectLarge is a little garbagey. The implementation went from having
 *	two large pixwins to one, and to avoid re-writing a bunch of code,
 *	we're just mapping two pixwins onto one.
 *
 *	However, when turning pages, we don't really know which leaf
 *	we're looking at. Hence, selectLarge scans ``havePage'' to find
 *	which leaf we currently have.
 */
selectLarge(win, inputEvent, arg)
Window *win;
Event *inputEvent;
caddr_t arg;
{
    for (globalLeaf = 0; globalLeaf < Leaves; globalLeaf++) {
	if (havePage[SHRINK_LARGE][globalLeaf]) break;
    }

    canvasSelected(largeCanvas, inputEvent, arg);
}

selectOne(win, inputEvent, arg)
Window *win;
Event *inputEvent;
caddr_t arg;
{
    globalLeaf = 0;
    canvasSelected(normalCanvas[0], inputEvent, arg);
}

selectTwo(win, inputEvent, arg)
Window *win;
Event *inputEvent;
caddr_t arg;
{
    globalLeaf = 1;
    canvasSelected(normalCanvas[1], inputEvent, arg);
}

void
canvasSelected(thisCanvas, inputEvent, arg)
Canvas thisCanvas;
Event *inputEvent;
caddr_t arg;
{
    int ch;
    Event *ie;
    Pixwin *pw;
    int i;
    int rebuild;
    
/*
 *	Figure out which leaf this event is from
 */
    ie = inputEvent;

    switch(event_id(ie)) {
	
    case BUT(1) :
    case BUT(2) :
	if (event_is_down(ie)) {
	    int mx;
	    int my;
/*
 *	Now, find top left of screen (where we scroll to)
 */
	    if (globalDisplayState == SHRINK_LARGE) {
		mx = event_x(ie);
		my = event_y(ie);
	    } else {
		mx = (event_x(ie) * leaf_w[SHRINK_LARGE])
		    / leaf_w[SHRINK_NORMAL];
		my = (event_y(ie) * page_h[SHRINK_LARGE])
		    / page_h[SHRINK_NORMAL];
	    }
	    
	    mx -= (( (int) window_get(frame,WIN_WIDTH)
		   - (int) window_get(frame, WIN_TOP_MARGIN)
		   - (int) window_get(frame, WIN_BOTTOM_MARGIN))/2);
	    mx = MAXFUNC(mx, 0);
	    my -= (( (int) window_get(frame,WIN_HEIGHT)
		   - (int) window_get(frame, WIN_RIGHT_MARGIN)
		   - (int) window_get(frame, WIN_LEFT_MARGIN))/2);
	    my = MAXFUNC(my, 0);

	    globalDisplayState = SHRINK_LARGE;
	    displayLeaves();

	    scrollbar_scroll_to(window_get(largeCanvas,
					   WIN_VERTICAL_SCROLLBAR),
				my);
	    scrollbar_scroll_to(window_get(largeCanvas,
					   WIN_HORIZONTAL_SCROLLBAR),
				mx);
	}
	return;

    case BUT(3):
	if (event_is_down(ie)) {
	    Event *localEvent;
	    int menuItem;

	    if (globalDisplayState == SHRINK_NORMAL) {
		localEvent = canvas_window_event(normalCanvas[globalLeaf], ie);
		menuItem = (int)menu_show(menuMain,
					  normalCanvas[globalLeaf],
					  localEvent, 0);
	    } else {
		localEvent = canvas_window_event(largeCanvas, ie);
		menuItem = (int)menu_show(menuMain,
					  largeCanvas,
					  localEvent, 0);
	    }

	    if (menuItem < 0) {
		globalDirection = ABSOLUTE;
		globalArg = 1;
		globalSign = 1;
		globalNumber = - menuItem;
		turnPage();
	    } else {
		switch (menuItem) {
		case MV_MAIN_FORWARD :
		    globalDirection = FOREWARD;
		    turnPage();
		    break;
	        case MV_MAIN_2FORWARD :
		    globalDirection = FOREWARD;
		    globalArg = 1;
		    globalSign = 1;
		    globalNumber = 2;
		    turnPage();
		    break;
		case MV_MAIN_BACKWARD :
		    globalDirection = BACKWARD;
		    turnPage();
		    break;
		case MV_MAIN_2BACKWARD :
		    globalDirection = BACKWARD;
		    globalArg = 1;
		    globalSign = 1;
		    globalNumber = 2;
		    turnPage();
		    break;
	        case MV_MAIN_QUIT :
		    stop_output(0);
		    break;

/* Document menu */

	        case MV_DOCUMENT_AGAIN :
		    reReadFile();
		    break;
	        case MV_DOCUMENT_INSPECT :
		    globalDisplayState = SHRINK_LARGE;
		    displayLeaves();
		    break;
	        case MV_DOCUMENT_OVERVIEW :
		    globalDisplayState = SHRINK_NORMAL;
		    displayLeaves();
		    break;

/*	PRINT sub-menu	*/

		case MV_PRINT_PRMARKED :
		case MV_PRINT_PRUNMARKED :
		    printPages(menuItem);
		    break;
		case MV_PRINT_LMARK :
		    markLeaf(LEAF_LEFT);
		    break;
		case MV_PRINT_RMARK :
		    markLeaf(LEAF_RIGHT);
		    break;
		case MV_PRINT_LUNMARK :
		    unmarkLeaf(LEAF_LEFT);
		    break;
		case MV_PRINT_RUNMARK :
		    unmarkLeaf(LEAF_RIGHT);
		    break;
		case MV_PRINT_PRALL :
		    printPages( menuItem );
		    break;
		case MV_PRINT_MARKALL :
		case MV_PRINT_UNMARKALL :
		    markAllPages(menuItem);
		    break;
		}
	    }
	}
	return;

#ifndef SUNOS4
	/*	These functions provided by SunView 4.0		 */

    case KEY_LEFT(7):	/* OPEN */
	if (win_inputposevent(ie)) {
	    break;
	} else {
	    int frameFd = window_fd(frame);

	    int Iconic = (int) window_get(frame, FRAME_CLOSED,0);

	    if (Iconic) {
		wmgr_open(frameFd, rootFd);
	    } else {
		wmgr_close(frameFd, rootFd);
	    }
	}
	break;
	
    case KEY_LEFT(5) : /* EXPOSE */
	if (win_inputposevent(ie)) {
	    break;
	} else {
	    int frameFd = window_fd(frame);

	    if (event_shift_is_down(ie) ||
		win_getlink(frameFd, WL_COVERING) == WIN_NULLLINK) {
		    wmgr_bottom(frameFd, rootFd);
		} else {
		    wmgr_top(frameFd, rootFd);
		}
	}
	break;

#endif /* SUNOS4 */

    case KEY_LEFT(4) : /* UNDO */
	if (event_is_down(ie)) {
	    if (globalDisplayState == SHRINK_NORMAL) {
		globalDisplayState = SHRINK_LARGE;
	    } else {
		globalDisplayState = SHRINK_NORMAL;
	    }
	    displayLeaves();
	}
	return;

    case KEY_LEFT(3) : /* PROPS */
    case KEY_LEFT(6) : /* PUT */
    case KEY_LEFT(8) : /* GET */
    case KEY_LEFT(9) : /* FIND */
	break;

    case 'q':
    case '\003':	/* control-C */
    case '\004':	/* control-D */
    case KEY_LEFT(1):	/* STOP */
#ifdef SUNOS4
    case ACTION_CUT:	/* I assume this is delete ? */
#endif /* SUNOS4 */
    case KEY_LEFT(10) : /* DELETE */
    {
	stop_output(0);
    }
	break;
	
    case 'n':
    case 'f':
    case ' ':
    case '\n' :
    case '\r' :
	/* scroll forward */
	globalDirection = FOREWARD;
	turnPage();
	return;
	
    case 'p':
    case 'b':
    case '\b':
    case 0177 : /* DEL */
	/* scroll backward */
	globalDirection = BACKWARD;
	turnPage();
	return;
    case 'g':
	/* go to absolute page */
	globalDirection = ABSOLUTE; /* may not be, but tough */
	turnPage();
	return;

    case 'R':
#ifdef SUNOS4
      case ACTION_AGAIN:
#endif
    case KEY_LEFT(2):	/* AGAIN */
	reReadFile();
	return;
	
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
	if (! globalArg) {
	    globalArg = 1;
	    globalSign = 1;
	    globalNumber = 0;
	}
	globalNumber = 10*globalNumber + (event_id(ie) - '0');
	return;
	
    case '-':
	if (! globalArg) {
	    globalArg = 1;
	    globalSign = -1;
	    globalNumber = 0;
	    return;
	}
	break;

    default:
	globalArg = 0;
	globalNumber = 1;
	globalSign = 1;
	return ;
    }
    return;
}

/*
 *	This procedure turns pages based on the values of globalArg
 *	and globalNumber
 */

turnPage()
{
    int newLeft;
    int newRight;
/*
 *	canonicalize the request. globalArg is always > 0, were moving in
 *	some globalDirection.
 */
    
    if (globalArg  == 0 || globalNumber == 0) {
	globalNumber = 1;
    }
    
    globalNumber = globalNumber * globalSign;
    
    if (globalNumber < 0) {
	if (globalDirection == FOREWARD) {
	    globalDirection = BACKWARD;
	} else if (globalDirection == BACKWARD){
	    globalDirection = FOREWARD;
	}
	globalNumber = -globalNumber;
    }
    
/* 
 *	Turn pages 
 */
    if (currentShrink == SHRINK_LARGE && globalDirection == BACKWARD
	&& globalNumber == 1 && globalLeaf == 1) {
	    globalLeaf = 0;
	    newLeft = pageOnLeaf[LEAF_LEFT];
	} else  {
	    if (globalDirection == BACKWARD) {
		newLeft = pageOnLeaf[LEAF_LEFT] - globalNumber;
	    } else if (globalDirection == FOREWARD) {
		newLeft = pageOnLeaf[LEAF_LEFT] + globalNumber;
	    } else {
		newLeft = globalNumber - 1;	/* pages globalNumbered at 0 */
	    }
	}

    newLeft = MINFUNC(newLeft, dviTotalPages-1);
    newLeft = MAXFUNC(newLeft, 0);

    if (Leaves == 1) {
	if (newLeft != pageOnLeaf[LEAF_LEFT]) {
	    pageOnLeaf[LEAF_LEFT] = newLeft;
	    havePage[SHRINK_NORMAL][LEAF_LEFT] = 0;
	    havePage[SHRINK_LARGE][LEAF_LEFT] = 0;
	    havePage[SHRINK_NORMAL][LEAF_RIGHT] = 0;
	    havePage[SHRINK_LARGE][LEAF_RIGHT] = 0;
	}
    } else {
/*
 *	If you're in a double-leaf display, and you started looking at
 *	leaf 1, but you're at the beginnign of the document, you need to
 *	start looking at leaf 0
 */

	if (newLeft == 0 && globalLeaf == 1) {
	    globalLeaf = 0;
	}
    
	newRight = newLeft + 1;
	if (pageOnLeaf[LEAF_LEFT] != newLeft) {
	    if (newLeft == pageOnLeaf[LEAF_RIGHT]) {
		copyLeaf(LEAF_RIGHT, LEAF_LEFT); /* right -> left */
	    } else if (newRight == pageOnLeaf[LEAF_LEFT]) {
		copyLeaf(LEAF_LEFT, LEAF_RIGHT); /* left -> right */
	    }
	    
	    if (pageOnLeaf[LEAF_LEFT] != newLeft) {
		pageOnLeaf[LEAF_LEFT] = newLeft;
		havePage[SHRINK_NORMAL][LEAF_LEFT] = 0;
		havePage[SHRINK_LARGE][LEAF_LEFT] = 0;
	    }
	    
	    if (pageOnLeaf[LEAF_RIGHT] != newRight) {
		pageOnLeaf[LEAF_RIGHT] = newRight;
		havePage[SHRINK_NORMAL][LEAF_RIGHT] = 0;
		havePage[SHRINK_LARGE][LEAF_RIGHT] = 0;
	    }
	}
    }
    displayLeaves();
    newFrameLabel();

    globalNumber = 1;
    globalSign = 1;
    globalDirection = FOREWARD;
    globalArg = 0;
}

put_border(x, y, w, h, t)
int x, y, w, h, t;
{
    put_rectangle(x, y, w, t);
    put_rectangle(x, y, t, h);
    put_rectangle(x, y + h - t, w, t);
    put_rectangle(x + w - t, y, t, h);
}

put_rectangle(x, y, w, h)
int x, y, w, h;
{
    int sm;

    pw_writebackground(pasteUpPixwin[currentShrink][currentLeaf],
	      x, y, w, h,
	      setMode);
}


char *
intToStr(n)
int n;
{
    char buf[10];
    char *p;

    /* make sure 4 bytes, last char for showing marked pages */
    sprintf(buf, "%3d ", n);
    p = malloc(strlen(buf)+1);
    if (p == NULL) {
	fprintf(stderr,"Out of memory building menu\n");
	stop_output(0);
    }
    strcpy(p, buf);
    return(p);
}


static char **menuText;
static char *marks;


buildGotoMenu()
{
    int i;

    menuText = (char **) malloc((dviTotalPages + 1) * sizeof(char *));

    if (menuText == NULL) {
	fprintf(stderr,"out of memory\n");
	stop_output(0);
    }

    if (menuGoto != 0) {
      menu_destroy( menuGoto );
    }

    menuGoto = menu_create(0);

/* reset the menu pointer in the main menu */

    if (menuMain != 0) {
      menu_set(menu_find(menuMain, MENU_STRING, M_MAIN_GOTO, 0),
	       MENU_PULLRIGHT, menuGoto, 0);
    }
	       

    for (i = 0; i < dviTotalPages; i++) {
	menuText[i] = intToStr(i+1);
	menu_set(menuGoto, MENU_STRING_ITEM, menuText[i], -i-1, 0);
    }

    if (dviTotalPages > 10) {
	menu_set(menuGoto, MENU_NCOLS, 5, 0);
    }
}

initMarks()
{
    int i;

    marks = (char *) malloc(dviTotalPages);
    if (marks == NULL) {
	fprintf(stderr, "out of memory\n");
	stop_output(0);
    }

    for (i = 0; i < dviTotalPages; i++) {
	marks[i] = 0;
    }

    buildGotoMenu();

}

finiMarks()
{
  int i;

  if (marks != 0) {
    free(marks);
  }

  /* clear gotoMenu */

  for (i = 0; i < dviTotalPages + 1; i++ ) {
    if (menuText[i] != 0) {
      free(menuText[i]);
    }
  }

}

markPage(pagenum)
int pagenum;
{
    if (pagenum >= 0) {
	marks[pagenum] = 1;
	/* this makes nasty assumptions about intToStr() */
	menuText[pagenum][3] = '*';
    }
}

markLeaf(leaf)
int leaf;
{
    markPage(pageOnLeaf[leaf]);
    newFrameLabel();
}


unmarkPage(pagenum)
int pagenum;
{
    if (pagenum >= 0) {
	marks[pagenum] = 0;
	/* this makes nasty assumptions about intToStr() */
	menuText[pagenum][3] = ' ';
    }
}

unmarkLeaf(leaf)
int leaf;
{
    unmarkPage(pageOnLeaf[leaf]);
    newFrameLabel();
}

/* Print some pages. The Menu selection dictates what pages get printed */

printPages(menuItem)
int menuItem;
{
    char *dvips;
    int cnt;
    int i;
    char buf[20];
    char printCommand[1024];

    char *fileToPrint;
    int deletePrintedFile;

    if ( menuItem == MV_PRINT_PRALL ) {
      fileToPrint = dviFileName;
      deletePrintedFile = 0;
    }
    else {

      int dviSelectBufferLength = 2048;
      char dviSelectBuffer[ 2048 ];

      fileToPrint = tmpnam( 0 );
      deletePrintedFile = 1;

      /* Now run dviselect to extract the pages */

      sprintf(dviSelectBuffer, "dviselect -i %s -o %s ",
	      dviFileName, fileToPrint);

      cnt = 0;
      for (i = 0; i < dviTotalPages; i++) {
	if (   (marks[i] && menuItem == MV_PRINT_PRMARKED)
	    || (!marks[i] && menuItem == MV_PRINT_PRUNMARKED )) {
	  sprintf(buf, " =%d", i + 1);
	  strcat(dviSelectBuffer, buf);
	  cnt++;
	}
      }

      if (cnt != 0) {
	window_set(frame, FRAME_LABEL, dviSelectBuffer, 0);
	setCursor(workingCursor);
	system(dviSelectBuffer);
      }
      else {

	/* nothing to print? */

	unlink( fileToPrint );
	return;
      }
    }

    /* Now print pages */


    if (texsunPrint == 0) {
      dvips = getenv("TEXSUNPRINT");
      if (dvips == NULL) {
	fprintf(stderr,"Using default ``%s'' to print\n",
		defaultTexsunPrint);
	dvips = defaultTexsunPrint;
      }
    }
    else {
      dvips = texsunPrint;
    }

    sprintf(printCommand, "%s %s",
	    dvips, fileToPrint);

    window_set(frame, FRAME_LABEL, printCommand, 0);
    setCursor(workingCursor);
    system(printCommand);
    newFrameLabel();
    setCursor(readyCursor);
    
    if ( deletePrintedFile ) {
      unlink( fileToPrint );
    }
}

markAllPages(menuItem)
int menuItem;
{
  int i;
  int mark = (menuItem == MV_PRINT_MARKALL);
    
  for ( i = 0; i < dviTotalPages; i++ ) {
    if (mark) {
      markPage(i);
    } else {
      unmarkPage(i);
    }
  }
  newFrameLabel();
}


newFrameLabel()
{
    int pagenum;

    pagenum = pageOnLeaf[LEAF_LEFT];
    if (pagenum < 0)
	return;
    if (Leaves == 1) {
	sprintf(frameLabel,"TeXsun  page %d of %d  %s (%4.2f by %4.2f)",
	    pagenum + 1, dviTotalPages,
	    frameLabelName,
	    (float) ((float) rawDviWidth / ( (float) dviDPI)),
	    (float) ((float) rawDviHeight / ( (float) dviDPI)) );
	if (marks[pagenum])
	    strcat(frameLabel, "   [marked]");
    } else if (pageOnLeaf[LEAF_LEFT] == pageOnLeaf[LEAF_RIGHT]) {
	sprintf(frameLabel,"TeXsun  page %d of %d  %s (%4.2f by %4.2f)",
	    pagenum + 1, dviTotalPages,
	    frameLabelName,
	    (float) ((float) rawDviWidth / ( (float) dviDPI)),
	    (float) ((float) rawDviHeight / ( (float) dviDPI)) );
	if (marks[pagenum])
	    strcat(frameLabel, "   [left marked]");
    } else {
	sprintf(frameLabel,"TeXsun  pages %d and %d of %d  %s (%4.2f by %4.2f)",
	    pagenum + 1, pageOnLeaf[LEAF_RIGHT] + 1, dviTotalPages,
	    frameLabelName,
	    (float) ((float) rawDviWidth / ( (float) dviDPI)),
	    (float) ((float) rawDviHeight / ( (float) dviDPI)) );
	if (marks[pagenum])
	    strcat(frameLabel, "   [left marked]");
	if (marks[pageOnLeaf[LEAF_RIGHT]])
	    strcat(frameLabel, "   [right marked]");
    }

    window_set ( frame, FRAME_LABEL, frameLabel, 0);
}


setCursor(cursor)
Cursor cursor;
{
    int i;

    for (i = 0; i < Leaves; i++) {
	window_set(normalCanvas[i], WIN_CURSOR, cursor, 0);
	if (largePixwinsAllocated) {
	    window_set(largeCanvas, WIN_CURSOR, cursor, 0);
	}
    }
}
