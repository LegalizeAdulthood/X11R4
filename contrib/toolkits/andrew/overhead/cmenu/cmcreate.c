/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/cmenu/RCS/cmcreate.c,v 2.6 89/06/05 01:18:58 zs01 Exp $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/cmenu/RCS/cmcreate.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/cmenu/RCS/cmcreate.c,v 2.6 89/06/05 01:18:58 zs01 Exp $";
#endif /* lint */

#include <X11/Xlib.h>

#include <cmintern.h>
#include <crsrimg.h>
#include <wormimg.h>

#define FONTHEIGHT(f) (f->max_bounds.ascent + f->max_bounds.descent)

static struct cmenudata *menuDataList = NULL;

static Window GetMenuWindow(display, parent, dp)
Display *display;
Window parent;
struct cmenudata *dp;
{

    XSetWindowAttributes xswa;
    Pixmap cursorPixmap;
    Pixmap cursorMaskPixmap;
    XColor foreColor;
    XColor backColor;
    Window rootWindow = RootWindow(display, DefaultScreen(display));

    /*
      * Make the menu's cursor.
*/
    cursorPixmap = XCreateBitmapFromData(display, rootWindow, (char *) cursor_bits,
					  (int)cursor_width, (int)cursor_height);


    cursorMaskPixmap = XCreatePixmapFromBitmapData(display, rootWindow, (char *) cursormask_bits,
						    (int)cursormask_width, (int)cursormask_height, 0, 1, 1);

    /*
      * Set up the Color Stuctures for the cursor. 
	  */
    foreColor.pixel = BlackPixel(display, DefaultScreen(display));
    XQueryColor(display, DefaultColormap(display, DefaultScreen(display)), &foreColor);
    backColor.pixel = WhitePixel(display, DefaultScreen(display));
    XQueryColor(display, DefaultColormap(display, DefaultScreen(display)), &backColor);

    xswa.cursor = XCreatePixmapCursor(display, cursorPixmap, cursorMaskPixmap,
				       &foreColor, &backColor,
				       cursor_x_hot, cursor_y_hot);        

    XFreePixmap(display, cursorPixmap);
    XFreePixmap(display, cursorMaskPixmap);

    if (xswa.cursor == NULL) {
	_cmErrorCode = cmE_CREATE_CURSOR;
	return NULL;
    }

    xswa.background_pixmap = None;
    xswa.override_redirect = TRUE;
    xswa.save_under = TRUE;
    xswa.event_mask = ButtonPressMask | ButtonReleaseMask |
      ExposureMask | PointerMotionHintMask;

    return XCreateWindow(display, parent, 0, 0, 1, 1, 0, 0, InputOutput,
			  CopyFromParent, CWBackPixmap | CWOverrideRedirect |
			  ((dp->useSaveUnder) ? CWSaveUnder : 0) | CWEventMask | CWCursor, &xswa);
}

static int GetGCs(dp, window, def_env)
struct cmenudata *dp;
Window window;
char *def_env;
{
    Display *display = dp->dpy;    
    GC tempgc;
    XGCValues gcv;
    Pixmap grayMap = NULL;
    Pixmap wormIcon = NULL;
    XWindowAttributes windowAttributes;
    int screen = DefaultScreen(display);
    static long grayImage[] = {0x55555555,
    0xAAAAAAAA,
    0x55555555,
    0xAAAAAAAA,
    0x55555555,
    0xAAAAAAAA,
    0x55555555,
    0xAAAAAAAA
    };
    unsigned long tileOrStipple = 0;
    unsigned long color;
    int colorDisplay = FALSE;

    if (XGetWindowAttributes(display, window, &windowAttributes) > 0)
	if (windowAttributes.depth > 1) {

	    XColor grayColor;

	    grayColor.red = grayColor.green = grayColor.blue = 43256; /* 2/3 intensity gray. */
	    grayColor.flags = DoRed | DoGreen | DoBlue;

	    if (XAllocColor(display, DefaultColormap(display, DefaultScreen(display)), &grayColor) != 0) {
		color = gcv.foreground = grayColor.pixel;
		colorDisplay = TRUE;
		gcv.fill_style = FillSolid;
		tileOrStipple = 0;
	    }
	    else
		gcv.fill_style = FillOpaqueStippled;
	}
	else
	    gcv.fill_style = FillTiled;
    else
	gcv.fill_style = FillOpaqueStippled;

    if (gcv.fill_style != FillSolid) {
	if ((grayMap = XCreatePixmapFromBitmapData(display, RootWindow(display, screen), (char *) grayImage, 32, 8, BlackPixel(display, screen), WhitePixel(display, DefaultScreen(display)), 1)) == NULL)
	    return -1;

	gcv.foreground = BlackPixel(display,screen);
	if (gcv.fill_style == FillTiled) {
	    gcv.tile = grayMap;
	    tileOrStipple = GCTile;
	}
	else {
	    gcv.stipple = grayMap;
	    tileOrStipple = GCStipple;
	}
    }

    gcv.background = WhitePixel(display,screen);
    gcv.font = dp->selectionFont->fid;
    tempgc = XCreateGC(display, window, GCFillStyle | GCForeground |
			GCBackground | GCFont | tileOrStipple, &gcv);

    if (grayMap != NULL)
	XFreePixmap(display, grayMap);

    if (tempgc != NULL)
	dp->grayGC = tempgc;
    else
	goto error;

    gcv.foreground = WhitePixel(display, screen);
    gcv.background = BlackPixel(display, screen);
    if ((dp->whiteGC = XCreateGC(display, window, GCForeground | GCBackground, &gcv)) == NULL)
	goto error;

    gcv.foreground = BlackPixel(display, screen);
    gcv.background = WhitePixel(display, screen);
    gcv.font = dp->titleFont->fid;
    if ((dp->titleBlackGC = XCreateGC(display, window, GCForeground | GCBackground | GCFont, &gcv)) == NULL)
	goto error;

    gcv.font = dp->selectionFont->fid;
    if ((dp->blackGC = XCreateGC(display, window, GCForeground | GCBackground | GCFont, &gcv)) == NULL)
	goto error;

    gcv.subwindow_mode = IncludeInferiors;
    gcv.graphics_exposures = FALSE;
    gcv.function = GXcopy;
    if ((dp->saveUnderGC = XCreateGC(display, window, GCSubwindowMode | GCGraphicsExposures | GCFunction, &gcv)) == NULL)
	goto error;


    if (!colorDisplay || (XGetDefault(display, def_env, "ColorHighlight") == NULL)) {
	gcv.plane_mask = BlackPixel(display, screen) ^ WhitePixel(display, screen);
	gcv.function = GXinvert;
	if ((dp->invertGC = XCreateGC(display, window, GCFunction | GCPlaneMask, &gcv)) == NULL)
	    goto error;
	dp->highlightUsingGray = FALSE;
    }
    else
	dp->highlightUsingGray = TRUE;

    if ((wormIcon = XCreatePixmapFromBitmapData(display, RootWindow(display, screen), (char *) worm_bits, worm_width, worm_height, BlackPixel(display, screen), WhitePixel(display, screen), DefaultDepth(display, screen))) == NULL)
	goto error;
    else {
	dp->wormWidth = worm_width;
	dp->wormHeight = worm_height;
	dp->wormIcon = wormIcon;
    }

    return 0;

error:

      if (colorDisplay)
	  XFreeColors(display, DefaultColormap(display, DefaultScreen(display)), &color, 1, 0);

    if (dp->grayGC != NULL)
	XFreeGC(display, dp->grayGC);
    if (dp->whiteGC != NULL)
	XFreeGC(display, dp->whiteGC);
    if (dp->titleBlackGC != NULL)
	XFreeGC(display, dp->titleBlackGC);
    if (dp->blackGC != NULL)
	XFreeGC(display, dp->blackGC);
    if (dp->saveUnderGC != NULL)
	XFreeGC(dp->saveUnderGC);
    if (dp->invertGC != NULL)
	XFreeGC(dp->invertGC);

    if (dp->wormIcon != NULL) /* Never happens, but here for safety when someone adds another item... */
	XFreePixmap(dp->wormIcon);

    return -1;
}

static int GetDisplayInfo(dpy, menu, def_env)
Display *dpy;
struct cmenu *menu;
char *def_env;
{

    struct cmenudata *dp;
    Window rootWindow;
    char *def_val;
    char *fontName;

    /*
      * Set up the default environment name.
	  */
    if (def_env == NULL || *def_env == '\0')
	def_env = "cmenu";

    /*
      Get menu data structure
      */

    for (dp = menuDataList; dp != NULL  && (dp->dpy != dpy
      || (strcmp(dp->def_env, def_env) != 0));
    dp = dp->next)
	;

    if (dp != NULL)  {
	menu->gMenuData = dp;
	return TRUE;
    }

    dp = (struct cmenudata *) malloc(sizeof(struct cmenudata));

    dp->dpy = dpy;
    dp->def_env = (char *) malloc(strlen(def_env) + 1);
    strcpy(dp->def_env, def_env);

    /*
      * Get the RootWindow and default pixel colors.
	  */
    rootWindow = RootWindow(dpy, DefaultScreen(dpy));

    if ((def_val = XGetDefault(dpy, def_env, "UseSaveUnder")) != NULL) {
	dp->useSaveUnder = (*def_val != 'n');
    }
    else
	dp->useSaveUnder = TRUE;

    if ((def_val = XGetDefault(dpy, def_env, "SaveBits")) != NULL) {
	dp->saveBits = (*def_val != 'n');
    }
    else
	dp->saveBits = TRUE;


    if ((def_val = XGetDefault(dpy, def_env, "ClickInterval")) != NULL)
	dp->clickInterval = atoi(def_val);
    else
	dp->clickInterval = cmenu_DefaultClickTime;

    if ((dp->menuWindow = GetMenuWindow(dpy, rootWindow, dp)) == NULL)  {
	_cmErrorCode = cmE_CREATE_INPUTONLY;
	goto error;
    }

    /*
      Now have to deal with font information
      */
    if ((fontName = XGetDefault(dpy, def_env, "TitleFont")) == NULL)
	fontName = cmenu_DefaultTitleFont;

    if ((dp->titleFont = XLoadQueryFont(dpy, fontName)) == NULL && (dp->titleFont = XLoadQueryFont(dpy, "fixed")) == NULL) {
	_cmErrorCode = cmE_OPEN_FONT;
	goto error;
    }

    dp->titleFontHeight = FONTHEIGHT(dp->titleFont);
    dp->titleFontAscent = dp->titleFont->max_bounds.ascent;
    dp->titleFontDescent = dp->titleFont->max_bounds.descent;

    if ((dp->yTitleOffset = dp->titleFontHeight / 4) < 2)
	dp->yTitleOffset = 2;

    dp->yShift = dp->titleFontHeight + dp->yTitleOffset + 2;
    dp->xShift = 16;

    dp->xTitleOffset = dp->titleFont->max_bounds.lbearing + dp->xShift / 3;

    if ((fontName = XGetDefault(dpy, def_env, "SelectionFont")) == NULL)
	fontName = cmenu_DefaultSelectionFont;

    if ((dp->selectionFont = XLoadQueryFont(dpy, fontName)) == NULL && (dp->selectionFont = XLoadQueryFont(dpy, "fixed")) == NULL) {
	_cmErrorCode = cmE_OPEN_FONT;
	goto error;
    }

    dp->selectionFontHeight = FONTHEIGHT(dp->selectionFont) + 2; /* 2 pixels for selection box overlap. */
    dp->selectionFontAscent = dp->selectionFont->max_bounds.ascent + 2;
    dp->xSelectionOffset = dp->xShift + dp->selectionFont->max_bounds.lbearing;

    dp->bottomMargin = dp->selectionFontHeight / 2;

    dp->overlapWidth = dp->xShift / 2;

    /*
      Creates GC's and Windows
      */
    if (GetGCs(dp, rootWindow, def_env) < 0)  {
	_cmErrorCode = cmE_CREATE_GC;
	goto error;
    }

    dp->next = menuDataList;
    menuDataList = dp;

    menu->gMenuData = dp;
    return 0;

error:
      return -1;
}

struct cmenu *cmenu_Create(dpy, parent, def_env, freeFunction)
Display *dpy;		/* Display structure pointer. */
Window parent;		/* Window ID of the menu's parent window. */
register char *def_env;	/* X Defaults program environment name. */
void *(*freeFunction)();
{

    struct cmenu *menu;

    /*
      * Calloc the cmenu structure, the initial pane and the bounding box.
      */
    ;
    if ((menu = (struct cmenu *) malloc(sizeof(struct cmenu))) == NULL) {
	_cmErrorCode = cmE_CALLOC;
	goto error;
    }

    if (GetDisplayInfo(dpy, menu, def_env) < 0)  {
	goto error;
    }

    /*
      * Construct the cmenu object.
      */

    menu->panes = NULL;
    menu->numberOfPanes = 0;
    menu->wormPane = -1;
    menu->wormSelection = -1;
    menu->freeFunction = freeFunction; /* Used for free data associated with selections. */
    /*
      * Return the completed cmenu.
      */
    _cmErrorCode = cmE_NO_ERROR;
    return(menu);

error:
    if (menu != NULL)
	  free(menu);

    return NULL;
}

