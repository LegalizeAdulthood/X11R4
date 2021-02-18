/*
 *	Original Source: ./contrib/windowmgrs/twm/twm.c
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */

/*****************************************************************************/
/**       Copyright 1988 by Evans & Sutherland Computer Corporation,        **/
/**                          Salt Lake City, Utah                           **/
/**                                                                         **/
/**                           All Rights Reserved                           **/
/**                                                                         **/
/**    Permission to use, copy, modify, and distribute this software and    **/
/**    its documentation  for  any  purpose  and  without  fee is hereby    **/
/**    granted, provided that the above copyright notice appear  in  all    **/
/**    copies and that both  that  copyright  notice  and  this  permis-    **/
/**    sion  notice appear in supporting  documentation,  and  that  the    **/
/**    name  of Evans & Sutherland  not be used in advertising or publi-    **/
/**    city pertaining to distribution  of the software without  specif-    **/
/**    ic, written prior permission.                                        **/
/**                                                                         **/
/**    EVANS  & SUTHERLAND  DISCLAIMS  ALL  WARRANTIES  WITH  REGARD  TO    **/
/**    THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILI-    **/
/**    TY AND FITNESS, IN NO EVENT SHALL EVANS &  SUTHERLAND  BE  LIABLE    **/
/**    FOR  ANY  SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY  DAM-    **/
/**    AGES  WHATSOEVER RESULTING FROM  LOSS OF USE,  DATA  OR  PROFITS,    **/
/**    WHETHER   IN  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS    **/
/**    ACTION, ARISING OUT OF OR IN  CONNECTION  WITH  THE  USE  OR PER-    **/
/**    FORMANCE OF THIS SOFTWARE.                                           **/
/*****************************************************************************/

/************************************************************
Copyright 1989 by Information-Technology Promotion Agency Japan (IPA),
Mitsubishi Research Institute, Inc. (MRI), and Fuji Xerox Co., Ltd. (FX)

                        All Rights Reserved

Permission to use, copy, modify and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IPA, MRI or FX not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IPA, MRI or FX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL IPA, MRI or FX BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

********************************************************/

/***********************************************************************
 *
 * $Header: twm.c,v 1.2 88/10/15 19:12:05 jim Exp $
 *
 * twm - "Tom's Window Manager"
 *
 * 27-Oct-87 Thomas E. LaStrange	File created
 *
 ***********************************************************************/

#ifndef lint
static char RCSinfo[] =
"$Header: swm.c,v 1.2 88/10/15 19:12:05 jim Exp $";
#endif

#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include "swm.h"
#include "add_window.h"
#include "gc.h"
#include "parse.h"
#include "version.h"
#include "menus.h"
#include "events.h"
#include "util.h"
#include "gram.h"

#include "swm.bm"
#include "gray.bm"

SwmWindow SwmRoot;		/* the head of the swm window list */

Display *dpy;			/* which display are we talking to */
int screen;			/* the default screen */
int d_depth;			/* copy of DefaultDepth(dpy, screen) */
Visual *d_visual;		/* copy of DefaultVisual(dpy, screen) */

Window Root;			/* the root window */
Window VersionWindow;		/* the swm version window */
Window SizeWindow;		/* the resize dimensions window */
Window ResizeWindow;		/* the window we are resizing */
Window InitialWindow;		/* the window name we are creating */
Colormap CMap;			/* default color map */

MyFont TitleBarFont;		/* title bar font structure */
MyFont MenuFont;		/* menu font structure */
MyFont IconFont;		/* icon font structure */
MyFont SizeFont;		/* resize font structure */
MyFont VersionFont;		/* version font structure */
MyFont InitialFont;		/* window creation font structure */
MyFont DefaultFont;

char *IconDirectory = NULL;	/* icon directory to search */

ColorPair BorderTileC;		/* border tile colors */
ColorPair TitleC;		/* titlebar colors */
ColorPair MenuC;		/* menu colors */
ColorPair MenuTitleC;		/* menu title colors */
ColorPair IconC;		/* icon colors */
ColorPair DefaultC;		/* default colors */
int BorderColor;		/* color of window borders */
int MenuShadowColor;		/* menu shadow color */
int IconBorderColor;		/* icon border color */

Cursor ArrowCursor;		/* title bar cursor */
Cursor ButtonCursor;		/* title bar button cursor */
Cursor MoveCursor;		/* move and resize cursor */
Cursor ClockCursor;		/* wait a while cursor */
Cursor LeftArrowCursor;		/* menu cursor */
Cursor UpperLeftCursor;		/* upper Left corner cursor */
Cursor LowerLeftCursor;		/* lower Left corner cursor */
Cursor UpperRightCursor;	/* upper Right corner cursor */
Cursor LowerRightCursor;	/* lower Right corner cursor */
Cursor UpperCursor;		/* upper side sursor */
Cursor BottomCursor;		/* bottom side sursor */
Cursor LeftCursor;		/* left side sursor */
Cursor RightCursor;		/* right side sursor */
Cursor DotCursor;		/* dot cursor for f.move, etc. from menus */
Cursor SkullCursor;		/* skull and cross bones, f.destroy */

GC MenuNormalGC;		/* normal GC for menus */
GC MenuReverseGC;		/* reverse video GC for menus */
GC MenuXorGC;			/* XOR GC for menus */
GC MenuTitleGC;			/* normal GC for menu titles */
GC IconNormalGC;		/* GC for icons */
GC VersionNormalGC;		/* GC for the version window */
GC SizeNormalGC;		/* GC for the resize window */
GC InitialNormalGC;		/* GC for the initial creation window */
GC DrawGC;			/* GC to draw lines for move and resize */
GC BorderGC;			/* GC to create the "gray" pixmap */

XClassHint NoClass;		/* for applications with no class */

XContext SwmContext;		/* context for swm windows */
XContext MenuContext;		/* context for all menu windows */

int BorderWidth = BW;		/* border width of swm windows */
int EdgeWidth = 4;		/* edge width of swm windows */
unsigned long Black;
unsigned long White;
Pixmap GrayTile;

char Version[100];		/* place to build the version string */
Pixmap UnknownPm = NULL;	/* the unknown icon pixmap */
int UnknownWidth = 0;		/* width of the unknown icon */
int UnknownHeight = 0;		/* height of the unknown icon */
int FirstTime = TRUE;		/* first time we've read .swmrc */
int ReverseVideo = FALSE;	/* flag to do reverse video */
int FocusRoot = TRUE;		/* is the input focus on the root ? */
SwmWindow *Focus = NULL;	/* the swm window that has focus */
int WarpCursor = FALSE;		/* warp cursor on de-iconify ? */
int ForceIcon = FALSE;		/* force the icon to the user specified */
int NoRaiseMove = FALSE;	/* don't raise window following move */
int NoRaiseResize = FALSE;	/* don't raise window following resize */
int NoRaiseDeicon = FALSE;	/* don't raise window on deiconify */
int DontMoveOff = FALSE;	/* don't allow windows to be moved off */
int DoZoom = FALSE;		/* zoom in and out of icons */
int Monochrome = COLOR;		/* is the display monochrome ? */
int TitleFocus = TRUE;		/* focus on window in title bar ? */
int NoTitlebar = FALSE;		/* put title bars on windows */
int DecorateTransients = FALSE;	/* put title bars on transients */
int IconifyByUnmapping = FALSE;	/* simply unmap windows when iconifying */
int BackingStore = TRUE;	/* use backing store for menus */
int SaveUnder = TRUE;		/* use save under's for menus */
int RandomPlacement = FALSE;	/* randomly place windows that no give hints */
char *Home;			/* the HOME environment variable */
int HomeLen;			/* length of Home */
int ParseError;			/* error parsing the .swmrc file */
int Highlight = TRUE;		/* we should highlight the window borders */
int MyDisplayWidth;		/* my copy of DisplayWidth(dpy, screen) */
int MyDisplayHeight;		/* my copy of DisplayHeight(dpy, screen) */

int TitleBarX = TITLE_BAR_HEIGHT + 4;	/* x coordinate ditto */
int HandlingEvents = FALSE;	/* are we handling events yet? */

Window JunkRoot;		/* junk window */
Window JunkChild;		/* junk window */
int JunkX;			/* junk variable */
int JunkY;			/* junk variable */
int JunkWidth;			/* junk variable */
int JunkHeight;			/* junk variable */
int JunkDepth;			/* junk variable */
int JunkBW;			/* junk variable */
int JunkMask;			/* junk variable */
 
int IconTableWidth=74;		/* icon table width & height */
int TempContext;                /* current Button press Context */
int IconWidth=70;		/* swm icon width */
int IconHeight=70;              /* swm icon height */
int IconBMHeight=50;		/* icon bitmap height */
int IconBMX=3;			/* icon bitmap x coodinate */
int IconBMY=3;			/* icon bitmap y coodinate */
int IconLabelY=63;		/* icon label y coodinate */
int IconBorderWidth=1;		/* ivon border width */
int IconShadowWidth=3;		/* icon shadow width */
int ResizeEdgeWidth = 20;       /* resize_edge width */
int ClampTop   = 0;		/* resize Top side */
int ClampBottom= 0;		/* resize Bottom side */
int ClampLeft  = 0;		/* resize Left side */	
int ClampRight = 0;		/* resize Right side */
int ClickTime0;			/* last mouse click time */
int ClickTime1;			/* mouse click time */

Window rootDesktopWindow; 	/* Root Desktop Window */
SwmWindow *RootDesktop;		/* Root Desktop Window (SwmWindow) */

/***********************************************************************
 *
 *  Procedure:
 *	main - start of swm
 *
 ***********************************************************************
 */

main(argc, argv)
    int argc;
    char *argv[];
{
    Window w, root, parent, *children;
    int nchildren, i;
    int m, d, y;
    char *display_name;
    unsigned long valuemask;	/* mask for create windows */
    XSetWindowAttributes attributes;	/* attributes for create windows */
    int (*old_handler) ();
    int mask;
    SwmWindow *twin;

    XWindowAttributes       info;

    display_name = NULL;
 
    if (argc != 1 && argc != 3)
    {
	fprintf(stderr, "Usage: swm [-display display]\n");
	exit(1);
    }

    if (argc == 3)
    {
	if (strncmp(argv[1], "-d", 2) == 0)
	    display_name = argv[2];
	else
	{
	    fprintf(stderr, "Usage: swm [-display display]\n");
	    exit(1);
	}
    }

    old_handler = signal(SIGINT, SIG_IGN);
    if (old_handler != SIG_IGN)
	signal(SIGINT, Done);

    old_handler = signal(SIGHUP, SIG_IGN);
    if (old_handler != SIG_IGN)
	signal(SIGHUP, Done);

    signal(SIGQUIT, Done);
    signal(SIGTERM, Done);

    Home = (char *)getenv("HOME");
    if (Home == NULL)
	Home = "./";

    HomeLen = strlen(Home);

    NoClass.res_name = NoName;
    NoClass.res_class = NoName;

    SwmRoot.next = NULL;
    SwmRoot.prev = NULL;
    SwmRoot.w = NULL;
    SwmRoot.title_w = NULL;
    SwmRoot.iconify_w = NULL;

    if ((dpy = XOpenDisplay(display_name)) == NULL)
    {
	fprintf(stderr, "swm: can't open the display\n");
	exit(1);
    }


    if (fcntl(ConnectionNumber(dpy), F_SETFD, 1) == -1)
    {
	fprintf(stderr, "swm: child cannot disinherit TCP fd\n");
	exit(1);
    }

    screen = DefaultScreen(dpy);
    d_depth = DefaultDepth(dpy, screen);
    d_visual = DefaultVisual(dpy, screen);
    Root = RootWindow(dpy, screen);
    CMap = DefaultColormap(dpy, screen);
    MyDisplayWidth = DisplayWidth(dpy, screen);
    MyDisplayHeight = DisplayHeight(dpy, screen);

    XSetErrorHandler(Other);
    XSelectInput(dpy, Root,
        SubstructureRedirectMask | KeyPressMask |
        ButtonPressMask | ButtonReleaseMask | ExposureMask | StructureNotifyMask);
    XSync(dpy, screen);

    XSetErrorHandler(Error);
    XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);

    SwmContext = XUniqueContext();
    MenuContext = XUniqueContext();

    /* define cursors */

    ArrowCursor = XCreateFontCursor(dpy, XC_top_left_arrow);
    MoveCursor = XCreateFontCursor(dpy, XC_fleur);
    LeftArrowCursor = XCreateFontCursor(dpy, XC_sb_left_arrow);
    ButtonCursor = XCreateFontCursor(dpy, XC_center_ptr);
    ClockCursor = XCreateFontCursor(dpy, XC_watch);
    UpperLeftCursor = XCreateFontCursor(dpy, XC_top_left_corner);
    LowerLeftCursor = XCreateFontCursor(dpy, XC_bottom_left_corner);
    UpperRightCursor = XCreateFontCursor(dpy, XC_top_right_corner);
    LowerRightCursor = XCreateFontCursor(dpy, XC_bottom_right_corner);
    UpperCursor = XCreateFontCursor(dpy, XC_top_side);
    BottomCursor = XCreateFontCursor(dpy, XC_bottom_side);
    LeftCursor = XCreateFontCursor(dpy, XC_left_side);
    RightCursor = XCreateFontCursor(dpy, XC_right_side);
    DotCursor = XCreateFontCursor(dpy, XC_dot);
    SkullCursor = XCreateFontCursor(dpy, XC_pirate);

    /* setup default colors */

    Black = BlackPixel(dpy, screen);
    White = WhitePixel(dpy, screen);

    DefaultC.fore = Black;
    DefaultC.back = White;
    BorderColor = Black;
    BorderTileC.fore = Black;
    BorderTileC.back = White;
    TitleC.fore = Black;
    TitleC.back = White;
    MenuC.fore = Black;
    MenuC.back = White;
    MenuTitleC.fore = Black;
    MenuTitleC.back = White;
    MenuShadowColor = Black;
    IconC.fore = Black;
    IconC.back = White;
    IconBorderColor = Black;

    /* setup default fonts */

    TitleBarFont.font = NULL;
    TitleBarFont.name = "8x13";		GetFont(&TitleBarFont);
    MenuFont.font = NULL;
    MenuFont.name = "8x13";		GetFont(&MenuFont);
    IconFont.font = NULL;
    IconFont.name = "8x13";		GetFont(&IconFont);
    SizeFont.font = NULL;
    SizeFont.name = "fixed";		GetFont(&SizeFont);
    VersionFont.font = NULL;
    VersionFont.name = "8x13";		GetFont(&VersionFont);
    InitialFont.font = NULL;
    InitialFont.name = "9x15";		GetFont(&InitialFont);
    DefaultFont.font = NULL;
    DefaultFont.name = "fixed";		GetFont(&DefaultFont);

    if (DisplayCells(dpy, screen) < 3)
	Monochrome = MONOCHROME;

    ParseSwmrc(NULL);
    FirstTime = FALSE;
    CreateGCs();

    XGrabServer(dpy);
    XSync(dpy, screen);

    XQueryTree(dpy, Root, &root, &parent, &children, &nchildren);

    GrayTile = MakePixmap(Root, BorderGC, gray_bits, 
     gray_width, gray_height);

    rootDesktopWindow = XCreateSimpleWindow(dpy,Root,30,30,
                MyDisplayWidth, MyDisplayHeight,
                0,Black,DefaultC.back);
    XSetStandardProperties(dpy,rootDesktopWindow,"rootDesktop","rootDesktop",
		None,NULL,0,NULL);
    XSelectInput(dpy, rootDesktopWindow,EnterWindowMask | LeaveWindowMask |
		SubstructureRedirectMask | KeyPressMask | ButtonPressMask |
		ButtonReleaseMask | ExposureMask | StructureNotifyMask);
    RootDesktop = AddWindow(rootDesktopWindow);

    TempDesktop = RootDesktop;

    XQueryTree(dpy, Root, &root, &parent, &children, &nchildren);
    for (i = 0; i < nchildren; i++)
	if (MappedNotOverride(children[i]))
	   {
           XUnmapWindow(dpy, children[i]);
	   twin = SuitWindow(children[i]);
           twin->initWindow = 1;
           XMapWindow(dpy, children[i]);
	   }

    XMapWindow(dpy,RootDesktop->w);

    InitialWindow = XCreateSimpleWindow(dpy, Root,
	0, 0, 5, InitialFont.height + 4, BW, DefaultC.fore, DefaultC.back);

    /* contruct the version string */
    sprintf(Version, "%s", &Revision[1]);
    Version[strlen(Version) - 1] = '\0';
    sscanf(&Date[7], "%d/%d/%d", &y, &m, &d);
    sprintf(Version, "%s  Date: %d/%d/%d %s", Version, m, d, y, &Date[16]);
    Version[strlen(Version) - 2] = '\0';

    VersionWindow = XCreateSimpleWindow(dpy, Root,
	0, 0,
	swm_width + XTextWidth(VersionFont.font, Version,
	    strlen(Version)) + 20, VersionFont.height + 4, BW,
	    DefaultC.fore, DefaultC.back);

    valuemask = CWBackPixmap;
    attributes.background_pixmap = MakePixmap(VersionWindow, VersionNormalGC,
	swm_bits, swm_width, swm_height);

    XCreateWindow(dpy, VersionWindow,
	4, 1,
	swm_width, swm_height,
	0, d_depth, CopyFromParent,
	d_visual, valuemask, &attributes);

    XSelectInput(dpy, VersionWindow, ExposureMask);
    XMapSubwindows(dpy, VersionWindow);
    XMapWindow(dpy, VersionWindow);

    SizeWindow = XCreateSimpleWindow(dpy, Root,
	0, 0,
	100,
	SizeFont.height + 4,
	BW,
	DefaultC.fore, DefaultC.back);


    XUngrabServer(dpy);

    ClickTime0 = time(NULL);
    ClickTime1 = time(NULL);

    HandlingEvents = TRUE;
    InitEvents();
    HandleEvents();
}

/***********************************************************************
 *
 *  Procedure:
 *	Done - cleanup and exit swm
 *
 *  Returned Value:
 *	none
 *
 *  Inputs:
 *	none
 *
 *  Outputs:
 *	none
 *
 *  Special Considerations:
 *	none
 *
 ***********************************************************************
 */

void
Done()
{
    SwmWindow *tmp;			/* temp swm window structure */
    unsigned x, y;
    XWindowChanges xwc;		/* change window structure */
    unsigned int xwcm;		/* change window mask */

    /* put a border back around all windows */

    for (tmp = SwmRoot.next; tmp != NULL; tmp = tmp->next)
    {
	XGetGeometry(dpy, tmp->w, &JunkRoot, &x, &y, &JunkWidth, &JunkHeight,
	    &JunkBW, &JunkDepth);

	xwcm = CWX | CWY | CWBorderWidth;

	xwc.x = x - (2 * BorderWidth);
	xwc.y = y - (2 * BorderWidth);

	xwc.border_width = BorderWidth;

	XConfigureWindow(dpy, tmp->w, xwcm, &xwc);
    }

    XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
    XCloseDisplay(dpy);
    exit(0);
}

/***********************************************************************
 *
 *  Procedure:
 *	Error - X error handler.  If we got here it is probably,
 *		because the client window went away and we haven't 
 *		got the DestroyNotify yet.
 *
 *  Inputs:
 *	dpy	- the connection to the X server
 *	event	- the error event structure
 *
 ***********************************************************************
 */

void
Error(dpy, event)
Display *dpy;
XErrorEvent *event;
{
    SwmWindow *tmp;			/* temp swm window structure */
    char buffer[BUFSIZ];

    /* Look for the window in the list, if it's there, remove it
     * from the list, and continue on, else ignore the error.
     * This is assumes that swm makes no errors. HaHa
    for (tmp = SwmRoot.next; tmp != NULL; tmp = tmp->next)
    {
	if (tmp->w == event->resourceid)
	{
	    if (tmp == Focus)
	    {
		FocusOnRoot();
	    }
	    XDeleteContext(dpy, tmp->w, SwmContext);
            XDeleteContext(dpy, tmp->move_edge, SwmContext);
            XDeleteContext(dpy, tmp->resize_rt_edge, SwmContext);
            XDeleteContext(dpy, tmp->resize_rb_edge, SwmContext);
            XDeleteContext(dpy, tmp->resize_lt_edge, SwmContext);
            XDeleteContext(dpy, tmp->resize_lb_edge, SwmContext);
	    XDeleteContext(dpy, tmp->frame, SwmContext);
	    XDeleteContext(dpy, tmp->title_w, SwmContext);
	    XDeleteContext(dpy, tmp->iconify_w, SwmContext);
	    XDeleteContext(dpy, tmp->icon_w, SwmContext);
            XDeleteContext(dpy, tmp->dummyicon_w, SwmContext);
	    XDestroyWindow(dpy, tmp->frame);
	    XDestroyWindow(dpy, tmp->icon_w);
            XDestroyWindow(dpy, tmp->dummyicon_w);
	    tmp->prev->next = tmp->next;
	    if (tmp->next != NULL)
		tmp->next->prev = tmp->prev;

	    free((char *)tmp);
	    return;
	}
    }
     */

    return;

    /*
    XGetErrorText(dpy, event->error_code, buffer, BUFSIZ);
    (void) fprintf(stderr, "X Error: %s\n", buffer);
    (void) fprintf(stderr, "  Request Major code: %d\n", event->request_code);
    (void) fprintf(stderr, "  Request Minor code: %d\n", event->minor_code);
    (void) fprintf(stderr, "  ResourceId 0x%x\n", event->resourceid);
    (void) fprintf(stderr, "  Error Serial #%d\n", event->serial);
    (void) fprintf(stderr, "  Current Serial #%d\n", dpy->request);

    Done();
    */
}

/***********************************************************************
 *
 *  Procedure:
 *	Other - error handler called if something else has set 
 *		the attributes on the root window.  Typically
 *		another window manager.
 *
 ***********************************************************************
 */

void
Other(dpy, event)
Display *dpy;
XErrorEvent *event;
{
    fprintf(stderr, "swm: Are you running another window manager?\n");
    exit(1); 
}
