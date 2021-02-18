/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) InitGraphics.c 5.1 89/12/14 Crucible";
#include <errno.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include "menu.h"
#include "olwm.h"
#include <olgx/olgx.h>

#include "bitmaps/ulresize.h"
#include "bitmaps/llresize.h"
#include "bitmaps/urresize.h"
#include "bitmaps/lrresize.h"
#include "bitmaps/pushpin.h"
#include "bitmaps/pushpinin.h"
#include "bitmaps/windowmark.h"
#include "bitmaps/icon.h"


/* Externs. */
GC	RootGC;
extern Display	*DefDpy;
extern int	DefScreen;
extern char	*TitleBarFontName;
extern char	*IconFontName;
extern char     *MenuTitleFontName;
extern char     *MenuButtonFontName;
extern char	*GlyphFontName;
extern int	ColorDisplay;
extern Bool	f3dUsed;

/* Globals. */
GC	DrawNormalGC, DrawReverseGC;
GC	DrawBusyGC, DrawSelectGC;
GC	IconNormalGC, DrawFrameGC;
GC	MenuTitleNormalGC, MenuShadowGC, MenuCopyGC;
GC	MenuButtonNormalGC, MenuButtonReverseGC;
GC	DrawWhiteGC, DrawBlackGC;
long	MenuForeColor, MenuBackColor;
XFontStruct	*NormalFontInfo;
XFontStruct	*IconFontInfo;
XFontStruct	*MenuTitleFontInfo;
XFontStruct	*MenuButtonFontInfo;
XFontStruct	*GlyphFontInfo;

Cursor		RootWinCursor;
Cursor		MoveWinCursor;
Cursor		FrameWinCursor;
Cursor		IconCursor;
Cursor		ResizeCursor;

/* Adornment pixmaps. */
Pixmap	pixLLresize;
Pixmap	pixLRresize;
Pixmap	pixULresize;
Pixmap	pixURresize;
Pixmap	pixPushPinRev;
Pixmap	pixPushPinInRev;
Pixmap	pixPushPin;
Pixmap	pixPushPinIn;
Pixmap	menuPushPin;
Pixmap	menuPushPinIn;
Pixmap	pixIcon;
Pixmap	pixmapGray;
Pixmap	pixGray;
Pixmap	shadowPix;


#define gray_width 8
#define gray_height 8
static char gray_bits[] = {
   0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa};

/* OLGX context */
Graphics_info *olgx_gismenu;	/* menu buttons */
Graphics_info *olgx_gismisc;	/* miscellaneous */
Graphics_info *olgx_gisbold;	/* misc, but with bold font */

/*
 * InitAdornments	-- read bitmap files and init adornment globals
 */
InitAdornments(dpy)
Display	*dpy;
{
	pixLLresize = XCreatePixmapFromBitmapData(dpy,
					RootWindow(dpy, DefaultScreen(dpy)),
					llresize_bits,
					llresize_width,
					llresize_height,
					BlackPixel(dpy, DefaultScreen(dpy)),
					WhitePixel(dpy, DefaultScreen(dpy)),
					DefaultDepth(dpy, DefaultScreen(dpy)));
	pixLRresize = XCreatePixmapFromBitmapData(dpy,
					RootWindow(dpy, DefaultScreen(dpy)),
					lrresize_bits,
					lrresize_width,
					lrresize_height,
					BlackPixel(dpy, DefaultScreen(dpy)),
					WhitePixel(dpy, DefaultScreen(dpy)),
					DefaultDepth(dpy, DefaultScreen(dpy)));
	pixULresize = XCreatePixmapFromBitmapData(dpy,
					RootWindow(dpy, DefaultScreen(dpy)),
					ulresize_bits,
					ulresize_width,
					ulresize_height,
					BlackPixel(dpy, DefaultScreen(dpy)),
					WhitePixel(dpy, DefaultScreen(dpy)),
					DefaultDepth(dpy, DefaultScreen(dpy)));
	pixURresize = XCreatePixmapFromBitmapData(dpy,
					RootWindow(dpy, DefaultScreen(dpy)),
					urresize_bits,
					urresize_width,
					urresize_height,
					BlackPixel(dpy, DefaultScreen(dpy)),
					WhitePixel(dpy, DefaultScreen(dpy)),
					DefaultDepth(dpy, DefaultScreen(dpy)));
	pixPushPin = XCreatePixmapFromBitmapData(dpy,
					RootWindow(dpy, DefaultScreen(dpy)),
					pushpin_bits,
					pushpin_width,
					pushpin_height,
					foreColor,
					backColor,
					DefaultDepth(dpy, DefaultScreen(dpy)));
	pixPushPinIn = XCreatePixmapFromBitmapData(dpy,
					RootWindow(dpy, DefaultScreen(dpy)),
					pushpinin_bits,
					pushpinin_width,
					pushpinin_height,
					foreColor,
					backColor,
					DefaultDepth(dpy, DefaultScreen(dpy)));
	menuPushPin = XCreatePixmapFromBitmapData(dpy,
					RootWindow(dpy, DefaultScreen(dpy)),
					pushpin_bits,
					pushpin_width,
					pushpin_height,
					BlackPixel(dpy, DefaultScreen(dpy)),
					WhitePixel(dpy, DefaultScreen(dpy)),
					DefaultDepth(dpy, DefaultScreen(dpy)));
	menuPushPinIn = XCreatePixmapFromBitmapData(dpy,
					RootWindow(dpy, DefaultScreen(dpy)),
					pushpinin_bits,
					pushpinin_width,
					pushpinin_height,
					BlackPixel(dpy, DefaultScreen(dpy)),
					WhitePixel(dpy, DefaultScreen(dpy)),
					DefaultDepth(dpy, DefaultScreen(dpy)));
	pixPushPinRev = XCreatePixmapFromBitmapData(dpy,
					RootWindow(dpy, DefaultScreen(dpy)),
					pushpin_bits,
					pushpin_width,
					pushpin_height,
					backColor,
					foreColor,
					DefaultDepth(dpy, DefaultScreen(dpy)));
	pixPushPinInRev = XCreatePixmapFromBitmapData(dpy,
					RootWindow(dpy, DefaultScreen(dpy)),
					pushpinin_bits,
					pushpinin_width,
					pushpinin_height,
					backColor,
					foreColor,
					DefaultDepth(dpy, DefaultScreen(dpy)));
	pixIcon = XCreatePixmapFromBitmapData(dpy,
					RootWindow(dpy, DefaultScreen(dpy)),
					icon_bits,
					icon_width,
					icon_height,
					BlackPixel(dpy, DefaultScreen(dpy)),
					WhitePixel(dpy, DefaultScreen(dpy)),
					DefaultDepth(dpy, DefaultScreen(dpy)));
}


InitGraphics(dpy)
Display	*dpy;
{
	XGCValues       values;
	unsigned long pixvals[5];

        /* Load up them font doggies. */
        if ((NormalFontInfo = XLoadQueryFont(dpy, TitleBarFontName))==NULL)
                NormalFontInfo = XLoadQueryFont(dpy, "9x15");

        if ((IconFontInfo = XLoadQueryFont(dpy, IconFontName))==NULL)
                IconFontInfo = XLoadQueryFont(dpy, "9x15");

	if ((MenuTitleFontInfo = XLoadQueryFont(dpy, MenuTitleFontName))==NULL)
                MenuTitleFontInfo = XLoadQueryFont(dpy, "9x15");

        if ((MenuButtonFontInfo = XLoadQueryFont(dpy,MenuButtonFontName))==NULL)
                MenuButtonFontInfo = XLoadQueryFont(dpy, "9x15");
	
	if ((GlyphFontInfo = XLoadQueryFont(dpy, GlyphFontName)) == NULL)
	{
		fprintf(stderr, "OLWM fatal error:  can't load glyph font \"%s\"; exiting\n",GlyphFontName);
		exit(1);
	}


        /* Create a GC for drawing in the root window. */
        values.function = GXxor;
        values.foreground = ~((~0L) << DisplayPlanes(dpy,DefaultScreen(dpy)));
	values.subwindow_mode = IncludeInferiors;
        RootGC = XCreateGC( dpy, RootWindow(dpy, DefaultScreen(dpy)),
                        ( GCFunction | GCForeground |
			  GCBackground | GCSubwindowMode ),
                        &values );

	MenuForeColor = bg1Color;
	MenuBackColor = bg2Color;

	/* Create GC's for drawing menu title and unselected buttons.
	 * The menu title is kept as a seperate GC because it uses a
	 * different font.
	 */
        values.function = GXcopy;
        values.foreground = MenuForeColor;
        values.background = MenuBackColor;
	values.graphics_exposures = False;
        MenuTitleNormalGC = XCreateGC( dpy, RootWindow(dpy, DefaultScreen(dpy)),
                        	       ( GCFunction | GCForeground | 
					 GCBackground | GCGraphicsExposures  ),
                        		 &values );
        MenuButtonNormalGC = XCreateGC( dpy, 
					RootWindow(dpy, DefaultScreen(dpy)),
                        		( GCFunction | GCForeground | 
					  GCBackground | GCGraphicsExposures  ),
                        		&values );

	/* Create a GC for drawing selected buttons */
        values.function = GXcopy;
        values.foreground = MenuBackColor;
        values.background = MenuForeColor;
	values.graphics_exposures = False;
        MenuButtonReverseGC = XCreateGC( dpy, 
					RootWindow(dpy, DefaultScreen(dpy)),
                        		( GCFunction | GCForeground | 
					  GCBackground | GCGraphicsExposures  ),
                        		&values );
	
	/* Install the menu title font. */
	XSetFont(dpy, MenuTitleNormalGC, MenuTitleFontInfo->fid);

	/* Install the font for the menu buttons. */
	XSetFont(dpy, MenuButtonNormalGC, MenuButtonFontInfo->fid);
	XSetFont(dpy, MenuButtonReverseGC, MenuButtonFontInfo->fid);

	/* Create a GC for icon name. */
        values.function = GXcopy;
        values.foreground = BlackPixel(dpy, DefaultScreen(dpy));
        values.background = WhitePixel(dpy, DefaultScreen(dpy));
	values.font = IconFontInfo->fid;
	values.line_width = FRAME_OUTLINE_WIDTH;
	values.graphics_exposures = False;
        IconNormalGC = XCreateGC( dpy, RootWindow(dpy, DefaultScreen(dpy)),
                        (GCFont | GCFunction | GCForeground | GCBackground |
			 GCGraphicsExposures | GCLineWidth),
                        &values );

	/* Create a GC for text and line graphics (black on white). */
        values.function = GXcopy;
        values.foreground = borderColor;
        values.background = backColor;
	values.font = NormalFontInfo->fid;
	values.line_width = 1;
	values.graphics_exposures = False;
        DrawSelectGC = XCreateGC( dpy, RootWindow(dpy, DefaultScreen(dpy)),
                        (GCFont | GCFunction | GCForeground | GCBackground |
			 GCGraphicsExposures | GCLineWidth),
                        &values );

	/* Create a GC for text and line graphics (black on white). */
        values.function = GXcopy;
        values.foreground = borderColor;
        values.background = backColor;
	values.font = NormalFontInfo->fid;
	values.line_width = FRAME_OUTLINE_WIDTH;
	values.graphics_exposures = False;
        DrawFrameGC = XCreateGC( dpy, RootWindow(dpy, DefaultScreen(dpy)),
                        (GCFont | GCFunction | GCForeground | GCBackground |
			 GCGraphicsExposures | GCLineWidth),
                        &values );

	/* Create a GC for text and line graphics (black on white).
	 * DrawBlackGC and DrawWhiteGC are used for drawing objects that
	 * are always black and white, like the resize corners.
	 */
        values.function = GXcopy;
        values.foreground = BlackPixel(dpy, DefaultScreen(dpy));
        values.background = WhitePixel(dpy, DefaultScreen(dpy));
	values.font = NormalFontInfo->fid;
	values.line_width = 1;
	values.graphics_exposures = False;
        DrawBlackGC = XCreateGC( dpy, RootWindow(dpy, DefaultScreen(dpy)),
                        (GCFont | GCFunction | GCForeground | GCBackground |
			 GCGraphicsExposures | GCLineWidth),
                        &values );

	/* Create a GC for text and line graphics (white on black).  */
        values.function = GXcopy;
        values.foreground = WhitePixel(dpy, DefaultScreen(dpy));
        values.background = BlackPixel(dpy, DefaultScreen(dpy));
	values.font = NormalFontInfo->fid;
	values.line_width = 1;
	values.graphics_exposures = False;
        DrawWhiteGC = XCreateGC( dpy, RootWindow(dpy, DefaultScreen(dpy)),
                        (GCFont | GCFunction | GCForeground | GCBackground |
			 GCGraphicsExposures | GCLineWidth),
                        &values );

	/* Create a GC for text and line graphics (fore on back). */
	/* This GC is used for the frame borders and titlebar
	 * lines in focus follows mouse mode.
	 */
        values.function = GXcopy;
        values.foreground = foreColor;
        values.background = backColor;
	values.font = NormalFontInfo->fid;
	values.line_width = FRAME_SELECTED_WIDTH;
	values.graphics_exposures = False;
        DrawNormalGC = XCreateGC( dpy, RootWindow(dpy, DefaultScreen(dpy)),
                        (GCFont | GCFunction | GCForeground | GCBackground |
			 GCGraphicsExposures | GCLineWidth),
                        &values );

	/* Create a GC the opposite of the previous one. */
        values.function = GXcopy;
        values.foreground = backColor;
        values.background = foreColor;
	values.line_width = FRAME_SELECTED_WIDTH;
	values.font = NormalFontInfo->fid;
	values.graphics_exposures = False;
        DrawReverseGC = XCreateGC(dpy, RootWindow(dpy, DefaultScreen(dpy)),
                        (GCFont | GCFunction | GCForeground | GCBackground |
			 GCGraphicsExposures | GCLineWidth),
                        &values);

	/* This is a bad name for this, all of the the gray items need
	 * to be figured out. Something like stipGray, pixColorGray, and
	 * pixGray.			???
	 */
	pixmapGray = XCreatePixmapFromBitmapData(dpy,
					RootWindow(dpy, DefaultScreen(dpy)),
					gray_bits,
					gray_width,
					gray_height,
					WhitePixel(dpy,DefaultScreen(dpy)),
					BlackPixel(dpy,DefaultScreen(dpy)),
					DefaultDepth(dpy, DefaultScreen(dpy)));

	pixGray = XCreatePixmapFromBitmapData(dpy,
					RootWindow(dpy, DefaultScreen(dpy)),
					gray_bits,
					gray_width,
					gray_height,
					WhitePixel(dpy,DefaultScreen(dpy)),
					BlackPixel(dpy,DefaultScreen(dpy)),
					1);

	/* A black and white version of pixGray. */
	shadowPix = XCreatePixmapFromBitmapData(dpy,
					RootWindow(dpy, DefaultScreen(dpy)),
					gray_bits,
					gray_width,
					gray_height,
					WhitePixel(dpy, DefaultScreen(dpy)),
					BlackPixel(dpy, DefaultScreen(dpy)),
					1);

	/* Create a GC for the busy title bar. */
        values.function = GXcopyInverted;
        values.foreground = backColor;
        values.stipple = pixGray;
	values.graphics_exposures = False;
	values.fill_style = FillStippled;
        DrawBusyGC = XCreateGC(dpy, RootWindow(dpy, DefaultScreen(dpy)),
                        (GCStipple | GCFunction | GCForeground |
			 GCGraphicsExposures | GCFillStyle),
                        &values);

	/* Create a GC for use in draw the menu shadow and the grayed out
	 * menu buttons.
	 */
        values.function = GXcopy;
        values.foreground = BlackPixel(dpy, DefaultScreen(dpy));
        values.background = WhitePixel(dpy, DefaultScreen(dpy));
        values.stipple = shadowPix;
	values.graphics_exposures = False;
	values.fill_style = FillStippled;
	values.subwindow_mode = IncludeInferiors;
        MenuShadowGC = XCreateGC(dpy, RootWindow(dpy, DefaultScreen(dpy)),
                        (GCStipple | GCFunction | GCForeground | GCBackground |
			 GCGraphicsExposures | GCFillStyle | GCSubwindowMode),
                        &values);
	/* Since this GC is used for the menu buttons, the font must
	 * be set up here as well.
	 */
	XSetFont(dpy, MenuShadowGC, MenuButtonFontInfo->fid);

	/* For doing the copy of a portion of the root window. */
        values.function = GXcopy;
        values.foreground = BlackPixel(dpy, DefaultScreen(dpy));
        values.background = WhitePixel(dpy, DefaultScreen(dpy));
	values.graphics_exposures = False;
	values.subwindow_mode = IncludeInferiors;
        MenuCopyGC = XCreateGC(dpy, RootWindow(dpy, DefaultScreen(dpy)),
                        (GCFunction | GCForeground | GCBackground |
			 GCGraphicsExposures | GCSubwindowMode),
                        &values);

	/* Get the cursors. */
	RootWinCursor = XCreateFontCursor(dpy, XC_left_ptr);
#ifdef NOT
	MoveWinCursor = XCreateFontCursor(dpy, XC_top_left_arrow);
	FrameWinCursor =XCreateFontCursor(dpy, XC_circle);
	IconCursor = XCreateFontCursor(dpy, XC_cross_reverse);
	ResizeCursor = XCreateFontCursor(dpy, XC_cross);
#else
	MoveWinCursor = RootWinCursor;
	FrameWinCursor = RootWinCursor;
	IconCursor = RootWinCursor;
	ResizeCursor = RootWinCursor;
#endif /* NOT */


	XDefineCursor(dpy, RootWindow(dpy, DefaultScreen(dpy)), RootWinCursor);

	/* OLGX initialisation */
	/* gis for drawing menu buttons */
	olgx_gismenu = olgx_initialize(dpy, DefaultScreen(dpy), f3dUsed);
	olgx_set_text_font(olgx_gismenu, MenuButtonFontInfo);
	olgx_set_glyph_font(olgx_gismenu, GlyphFontInfo);
	pixvals[OLGX_WHITE] = backColor;
	pixvals[OLGX_BG1] = bg1Color;
	pixvals[OLGX_BG2] = bg2Color;
	pixvals[OLGX_BG3] = bg3Color;
	pixvals[OLGX_BLACK] = foreColor;
	olgx_set_color(olgx_gismenu, pixvals);

	/* gis for drawing menu buttons */
	olgx_gisbold = olgx_initialize(dpy, DefaultScreen(dpy), f3dUsed);
	olgx_set_text_font(olgx_gisbold, MenuTitleFontInfo);
	olgx_set_glyph_font(olgx_gisbold, GlyphFontInfo);
	pixvals[OLGX_WHITE] = backColor;
	pixvals[OLGX_BG1] = bg1Color;
	pixvals[OLGX_BG2] = bg2Color;
	pixvals[OLGX_BG3] = bg3Color;
	pixvals[OLGX_BLACK] = foreColor;
	olgx_set_color(olgx_gisbold, pixvals);

	/* gis for drawing miscellaneous stuff (pushpins, resize, etc.)*/
	olgx_gismisc = olgx_initialize(dpy, DefaultScreen(dpy), f3dUsed);
	olgx_set_text_font(olgx_gismisc, MenuButtonFontInfo);
	olgx_set_glyph_font(olgx_gismisc, GlyphFontInfo);
	pixvals[OLGX_WHITE] = backColor;
	pixvals[OLGX_BG1] = bg1Color;
	pixvals[OLGX_BG2] = bg2Color;
	pixvals[OLGX_BG3] = bg3Color;
	pixvals[OLGX_BLACK] = foreColor;
	olgx_set_color(olgx_gismisc, pixvals);
}

/* UninitGraphics	- free up any server resources that have been consumed,
 *			  and restore root cursor.
 *
 *	Currently unused.
 */
UninitGraphics()
{
	XUndefineCursor(DefDpy, RootWindow(DefDpy, DefScreen));
	olgx_destroy(olgx_gismenu);
}
