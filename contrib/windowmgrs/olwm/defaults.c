/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) defaults.c 5.1 89/12/14 Crucible";
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <strings.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "menu.h"
#include "olwm.h"

/* default constants */

#define	DEFAULTCOLOR	"#40A0C0"

#define TITLEFONT \
	"-b&h-lucida-bold-r-normal-sans-12-*-*-*-*-*-*-*"
#define ICONFONT \
	"-b&h-lucida-medium-r-normal-sans-12-*-*-*-*-*-*-*"
#define MENUTITLEFONT \
	"-b&h-lucida-bold-r-normal-sans-12-*-*-*-*-*-*-*"
#define MENUBUTTONFONT \
	"-b&h-lucida-medium-r-normal-sans-12-*-*-*-*-*-*-*"
#define GLYPHFONT \
	"-sun-open look glyph-*-*-*-*-12-*-*-*-*-*-*-*"

extern	char	*ProgramName;
extern	int	FocusModeOverride;
extern	Bool	f3dSet, f3dUsed;

/* default globals */
char	*TitleBarFontName;
char	*IconFontName;
char    *MenuTitleFontName;
char    *MenuButtonFontName;
char	*GlyphFontName;
int	FlashTime;
Bool	FShowMenuButtons;
Bool	FShowPinnedMenuButtons;
int	ColorDisplay;
IconPreference IconPlacement = AlongRight;
Bool	FocusLenience = False;

unsigned	long	foreColor, backColor, borderColor;
unsigned 	long	bg1Color, bg2Color, bg3Color;	/* for 3D look */

KeyCode FrontKeyCode;
KeyCode HelpKeyCode;
KeyCode OpenKeyCode;

/* application menu strings */

#define TAG	"OpenWindows"

extern	int	DoubleClickTime;

/* private data */

static KeySym FrontKeySym = XK_L5;	/* identical to XK_F15 */
static KeySym HelpKeySym = XK_Help;
static KeySym OpenKeySym = XK_L7;		/* identical to XK_F17 */

static Bool matchString();
static Bool matchBool();

/*ARGSUSED*/	/* ac, av args will be used later */
GetDefaults(dpy, ac, av)
Display	*dpy;
int	ac;
char	**av;
{
	char		*defStr, *XGetDefaults();
	XColor		exact;
	char		*foreName, *backName, *borderName;
	extern	int	FocusFollowsMouse;
	XVisualInfo	*vis;
	XVisualInfo	visTemplate;
	int		i;
	int		nvis;

#define	CMAP	DefaultColormap(dpy,DefaultScreen(dpy))
	foreColor = BlackPixel(dpy, DefaultScreen(dpy));
	backColor = WhitePixel(dpy, DefaultScreen(dpy));
	borderColor = BlackPixel(dpy, DefaultScreen(dpy));

	ColorDisplay = False;
	vis = XGetVisualInfo( dpy, VisualNoMask, &visTemplate, &nvis );
	for ( i=0; i<nvis; ++i ) {
	    if (vis[i].screen == DefaultScreen(dpy)) {
		if ( vis[i].class == StaticColor
		||   vis[i].class == PseudoColor
		||   vis[i].class == DirectColor ) {
		    ColorDisplay = True;
		    break;
		}
	    }
	}
	XFree( vis );

	/*
	 * Determine whether to use 3D.  Abide by the command-line option, if
	 * any was given.  Otherwise, don't use 3D.  In the future, set 3D
	 * if the display can support it.
	 * REMIND: GrayScale visuals can support 3D, but we don't handle this.
	 * REMIND: We should set the 3D option based on a resource.
	 */
	if ( f3dSet )
	    f3dUsed = f3dUsed && ColorDisplay;
	else
	    f3dUsed = /* ColorDisplay */ False;

	if ( ColorDisplay )
	{

		/* get general colours */

		if (((foreName = XGetDefault(dpy, TAG, "foreground"))
			&& (XParseColor(dpy, CMAP, foreName, &exact))
				&& (XAllocColor(dpy, CMAP, &exact))))
					foreColor = exact.pixel;

		if (((backName = XGetDefault(dpy, TAG, "WindowColor"))
			&& (XParseColor(dpy, CMAP, backName, &exact))
				&& (XAllocColor(dpy, CMAP, &exact))))
					backColor = exact.pixel;

		if (((borderName = XGetDefault(dpy, TAG, "borderColor"))
			&& (XParseColor(dpy, CMAP, borderName, &exact))
				&& (XAllocColor(dpy, CMAP, &exact))))
					borderColor = exact.pixel;
	}

	if (!f3dUsed)
	{
		bg1Color = backColor;
		bg2Color = borderColor;
		bg3Color = foreColor;
	}
	else
	{
		if (backColor == WhitePixel(dpy, DefaultScreen(dpy)))
		{
			exact.pixel = backColor;
			XQueryColor(dpy, CMAP, &exact);
			exact.red = (exact.red*8)/10;
			exact.green = (exact.green*8)/10;
			exact.blue = (exact.blue*8)/10;
			if (XAllocColor(dpy, CMAP, &exact))
				bg1Color = exact.pixel;
			else
				f3dUsed = False;
		}
		else
		{
			bg1Color = backColor;
			backColor = WhitePixel(dpy, DefaultScreen(dpy));
		}

		if (f3dUsed)
		{
			exact.pixel = bg1Color;
			XQueryColor(dpy, CMAP, &exact);
			exact.red = exact.red/2;
			exact.green = exact.green/2;
			exact.blue = exact.blue/2;
			if (XAllocColor(dpy, CMAP, &exact))
				bg3Color = exact.pixel;
			else
				f3dUsed = False;
		}

		if (f3dUsed)
		{
			exact.pixel = bg1Color;
			XQueryColor(dpy, CMAP, &exact);
			exact.red = (exact.red*9)/10;
			exact.green = (exact.green*9)/10;
			exact.blue = (exact.blue*9)/10;
			if (XAllocColor(dpy, CMAP, &exact))
				bg2Color = exact.pixel;
			else
				f3dUsed = False;
		}

		if (f3dUsed)
			foreColor = BlackPixel(dpy, DefaultScreen(dpy));
	}
	
	if (defStr = XGetDefault(dpy, TAG, "WorkspaceColor"))
		setWorkspaceColor(dpy, defStr);

	if ((TitleBarFontName = XGetDefault(dpy, "olwm", "TitleFont")) == NULL)
		TitleBarFontName = TITLEFONT;
	if ((IconFontName = XGetDefault(dpy, "olwm", "IconFont")) == NULL)
		IconFontName = ICONFONT;
        if ((MenuTitleFontName = XGetDefault(dpy, "olwm", "MenuTitleFont")) ==
            NULL)
                MenuTitleFontName = MENUTITLEFONT;
        if ((MenuButtonFontName = XGetDefault(dpy, "olwm", "MenuButtonFont"))==
            NULL)
                MenuButtonFontName = MENUBUTTONFONT;
        if ((GlyphFontName = XGetDefault(dpy, "olwm", "GlyphFont")) == NULL)
                GlyphFontName = GLYPHFONT;

	if ((defStr = XGetDefault(dpy, TAG, "MultiClickTimeout")) != NULL)
		DoubleClickTime = atoi(defStr) * 100; /* convert to milisec */

	/* command line arguments take precedence over .Xdefaults */
	if (!FocusModeOverride)
		if ((defStr = XGetDefault(dpy, TAG, "SetInput")) != NULL)
			if (matchString(defStr, "followmouse"))
				FocusFollowsMouse = True;
	
	if ((defStr = XGetDefault(dpy, TAG, "IconLocation")) != NULL)
	{
		if (matchString(defStr,"top"))
			IconPlacement = AlongTop;
		if (matchString(defStr,"bottom"))
			IconPlacement = AlongBottom;
		if (matchString(defStr,"right"))
			IconPlacement = AlongRight;
		if (matchString(defStr,"left"))
			IconPlacement = AlongLeft;
	}

	if ((defStr = XGetDefault(dpy, "olwm", "FlashTime")) != NULL)
		FlashTime = atoi(defStr);
	else
		FlashTime = 100000;

	if ((defStr = XGetDefault(dpy, TAG, "FrontKeySym")) != NULL)
		FrontKeySym = XStringToKeysym(defStr);
	if ((defStr = XGetDefault(dpy, TAG, "HelpKeySym")) != NULL)
		HelpKeySym = XStringToKeysym(defStr);
	if ((defStr = XGetDefault(dpy, TAG, "OpenKeySym")) != NULL)
		OpenKeySym = XStringToKeysym(defStr);

	FrontKeyCode = XKeysymToKeycode( dpy, FrontKeySym );
	HelpKeyCode = XKeysymToKeycode( dpy, HelpKeySym );
	OpenKeyCode = XKeysymToKeycode( dpy, OpenKeySym );

	if ( FrontKeyCode != 0 )
		XGrabKey( dpy, FrontKeyCode, AnyModifier, DefaultRootWindow(dpy),
			False, GrabModeAsync, GrabModeAsync );
	if ( HelpKeyCode != 0 )
		XGrabKey( dpy, HelpKeyCode, AnyModifier, DefaultRootWindow(dpy),
			False, GrabModeAsync, GrabModeAsync );
	if ( OpenKeyCode != 0 )
		XGrabKey( dpy, OpenKeyCode, AnyModifier, DefaultRootWindow(dpy),
			False, GrabModeAsync, GrabModeAsync );

	if ((defStr = XGetDefault(dpy, "olwm", "ShowMenuButtons")) == NULL ||
	    !matchBool(defStr,&FShowMenuButtons))
		FShowMenuButtons = False;

	if ((defStr = XGetDefault(dpy, "olwm", "ShowPinnedMenuButtons")) == NULL ||
	    !matchBool(defStr,&FShowPinnedMenuButtons))
		FShowPinnedMenuButtons = False;

	if ((defStr = XGetDefault(dpy, TAG, "FocusLenience")) != NULL)
	    (void) matchBool(defStr,&FocusLenience);
}

/*
 * setWorkspaceColor -- set the root window background.
 * REMIND: this routine doesn't work properly for GrayScale visuals.
 */
static
setWorkspaceColor( dpy, colorname )
Display	*dpy;
char	*colorname;
{
	XColor color;
	GC gc;
	XGCValues gcv;
	Pixmap bitmap = 0;
	Pixmap pixmap;
	unsigned int width, height;
	int valid = 0;
	int docolor = 0;

#define gray_width 8
#define gray_height 8
static char gray_bits[] = {
	0x55, 0xaa, 0x55, 0xaa,
	0x55, 0xaa, 0x55, 0xaa};

	if ( colorname != NULL )
	{
		if ( *colorname == '/' )
		{
			unsigned int x, y;

			if ( BitmapSuccess == XReadBitmapFile( dpy,
							DefaultRootWindow(dpy),
							colorname,
							&width, &height,
							&bitmap, &x, &y ))
			valid = 1;
		}
		else if ( *colorname == '#' && ColorDisplay )
		{
			XParseColor( dpy,
				     DefaultColormap(dpy,DefaultScreen(dpy)),
				     colorname, &color );

			if ( XAllocColor( dpy,
					DefaultColormap(dpy,DefaultScreen(dpy)),
					&color ))
				valid = docolor = 1;
		}
		else if ( ColorDisplay )
		{
			/* It is a color name */
			/*
			 * NOTE:  this could possibly fail on a full
			 * dynamic colormap. If it does, tough -- 
			 * we''ll just set it to whatever junk
			 * comes back for now. 
			 * Presumably this won''t happen since
			 * 'props' was able to display the color.
			 */
			if ( XAllocNamedColor(dpy,
					DefaultColormap(dpy,DefaultScreen(dpy)),
					colorname, &color, &color))
			valid = docolor = 1;
		}
	}

	if ( !valid && ColorDisplay ) 
	{
		XParseColor( dpy, DefaultColormap(dpy,DefaultScreen(dpy)),
			     DEFAULTCOLOR, &color );
		if ( XAllocColor( dpy, DefaultColormap(dpy,DefaultScreen(dpy)),
				  &color ))
		valid = docolor = 1;
	}

	if ( !valid ) {
		bitmap = XCreateBitmapFromData( dpy, DefaultRootWindow(dpy),
			gray_bits, gray_width, gray_height );
		width = gray_width;
		height = gray_height;
	}

	if ( docolor ) {
		XSetWindowBackground(dpy, DefaultRootWindow(dpy), color.pixel);
	}
	else
	{
		gcv.foreground = BlackPixel(dpy,DefaultScreen(dpy));
		gcv.background = WhitePixel(dpy,DefaultScreen(dpy));
		gc = XCreateGC( dpy, DefaultRootWindow(dpy),
				GCForeground|GCBackground, &gcv);
		pixmap = XCreatePixmap(dpy, DefaultRootWindow(dpy),
				       width, height,
					(unsigned int)DefaultDepth(dpy,
							DefaultScreen(dpy)));
		XCopyPlane( dpy, bitmap, pixmap, gc, 0, 0, width, height,
			    0, 0, (unsigned long)1 );
		XSetWindowBackgroundPixmap(dpy, DefaultRootWindow(dpy), pixmap);
		XFreeGC( dpy, gc );
		XFreePixmap( dpy, bitmap );
		XFreePixmap( dpy, pixmap );
	}
	XClearWindow( dpy, DefaultRootWindow(dpy) );
}

#define BSIZE 100

/*
 * Determine whether value matches pattern, irrespective of case.
 * This routine is necessary because not all systems have strcasecmp().
 */
static Bool
matchString( value, pattern )
    char *value;
    char *pattern;
{
    char buf[BSIZE];
    char *p;

    strncpy(buf,value,BSIZE);
    buf[BSIZE-1] = '\0'; /* force null termination */

    for ( p=buf; *p; ++p )
	if (isupper(*p))
	    *p = tolower(*p);

    return 0 == strcmp(buf,pattern);
}


/*
 * Match any of the following booleans: yes, no, 1, 0, on, off, t, nil, true, 
 * false.  Return the boolean matched in ret, and return True.  Otherwise, 
 * return False.
 */
static Bool
matchBool( value, ret )
    char *value;
    Bool *ret;
{
    if (matchString(value,"yes") ||
	matchString(value,"on") ||
	matchString(value,"t") ||
	matchString(value,"true") ||
	0 == strcmp(value,"1")) {
	*ret = True;
	return True;
    }

    if (matchString(value,"no") ||
	matchString(value,"off") ||
	matchString(value,"nil") ||
	matchString(value,"false") ||
	0 == strcmp(value,"0")) {
	*ret = False;
	return True;
    }

    return False;
}
