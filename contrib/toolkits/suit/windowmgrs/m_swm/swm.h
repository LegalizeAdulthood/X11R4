/*
 *	Original Source: ./contrib/windowmgrs/twm/twm.h
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
 * $Header: twm.h,v 1.48 88/10/14 07:05:59 toml Exp $
 *
 * twm include file
 *
 * 28-Oct-87 Thomas E. LaStrange	File created
 *
 ***********************************************************************/

#ifndef _SWM_
#define _SWM_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

/* Define this if you want the Focus button on the titlebar */
/* #define FOCUS */

#define BW 2			/* border width */

/* directory to look for bitmaps if the file is not found in the current
 * directory 
 */
#define BITMAPS "/usr/include/X11/bitmaps"

#ifndef TRUE
#define TRUE	1
#define FALSE	0
#endif

#define MAX_BUTTONS	5	/* max mouse buttons supported */

/* contexts for button presses */
#define C_NO_CONTEXT	-1
#define C_WINDOW	0
#define C_TITLE		1
#define C_ICON		2
#define C_ROOT		3
#define C_FRAME		4
#define C_NAME		5
#define NUM_CONTEXTS	7

/* modifiers for button presses */
#define MOD_SIZE	((ShiftMask | ControlMask | Mod1Mask) + 1)

#define TITLE_BAR_SPACE         1	/* 2 pixel space bordering chars */
#define TITLE_BAR_FONT_HEIGHT   15	/* max of 15 pixel high chars */
#define TITLE_BAR_HEIGHT        (TITLE_BAR_FONT_HEIGHT+(2*TITLE_BAR_SPACE))

/* defines for zooming/unzooming */
#define ZOOM_NONE 0
#define ZOOM_VERT 1
#define ZOOM_FULL 2

typedef struct MyFont
{
    char *name;			/* name of the font */
    XFontStruct *font;		/* font structure */
    int height;			/* height of the font */
    int y;			/* Y coordinate to draw characters */
} MyFont;

typedef struct ColorPair
{
    int fore;
    int back;
} ColorPair;

/* for each window that is on the display, one of these structures
 * is allocated and linked into a list 
 */
typedef struct SwmWindow
{
    struct SwmWindow *next;	/* next swm window */
    struct SwmWindow *prev;	/* previous swm window */
    Window w;			/* the child window */
    Window move_edge;		/* the move edge */
    Window resize_rt_edge;      /* th Resize Edge (right_up) */
    Window resize_rb_edge;      /* th Resize Edge (right_bottom) */
    Window resize_lt_edge;      /* th Resize Edge (left_up) */
    Window resize_lb_edge;      /* th Resize Edge (left_bottom) */
    Window frame;		/* the frame window */
    Window title_w;		/* the title bar window */
    Window iconify_w;		/* the iconify button */
    Pixmap iconify_pm;
    Window dummyicon_w;		/* dummy icon window */
    Window dummyicon_label;	/* dummy icon name window */
    Window icon_w;		/* the icon window */
    Window icon_bm_w;		/* the icon bitmap window */
    Window icon_label;          /* the icon shadow name window */
    GC title_gc;		/* gc for the title bar */
    int frame_x;		/* x position of frame */
    int frame_y;		/* y position of frame */
    int frame_width;		/* width of frame */
    int frame_height;		/* height of frame */
    int icon_x;			/* icon text x coordinate */
    int icon_y;			/* icon text y coordiante */
    int icon_w_width;		/* width of the icon window */
    int icon_width;		/* width of the icon bitmap */
    int icon_height;		/* height of the icon bitmap */
    int title_height;		/* height of the title bar */
    char *full_name;		/* full name of the window */
    char *window_name;          /* name of the window */
    char *icon_name;		/* name of the icon */
    int name_width;		/* width of name text */
    XWindowAttributes attr;	/* the child window attributes */
    XSizeHints hints;		/* normal hints */
    XWMHints *wmhints;		/* WM hints */
    int group;			/* group ID */
    int frame_vis;		/* frame visibility */
    int icon_vis;		/* icon visibility */
    XClassHint class;
    short xterm;		/* boolean indicating xterm */
    short iconified;		/* has the window ever been iconified? */
    short icon;			/* is the window an icon now ? */
    short icon_on;		/* is the icon visible */
    short mapped;		/* is the window mapped ? */
    short auto_raise;		/* should we auto-raise this window ? */
    short forced;		/* has had an icon forced upon it */
    short highlight;		/* should highlight this window */
    short iconify_by_unmapping;	/* unmap window to iconify it */
    int save_frame_x;		/* x position of frame */
    int save_frame_y;		/* y position of frame */
    int save_frame_width;	/* width of frame */
    int save_frame_height;	/* height of frame */
    short zoomed;		/* is the window zoomed? */
    int windowFlag;		/* 0/1/2 : RootDeskTop/DeskTop/Application */
    int parent;			/* 0/1   : parentDesktop is RootDesktop/Desktop */
    Window parentId;            /* parent window id */
    int iconTable[20][20];	/* IconTable */
    int initWindow;		/* 0/1   : 0=KillClient 0=Do not KillCllient */
} SwmWindow;

extern SwmWindow SwmRoot;

extern Display *dpy;
extern int screen;
extern int d_depth;
extern Visual *d_visual;

extern Window Root;
extern Window VersionWindow;
extern Window SizeWindow;
extern Window ResizeWindow;
extern Window InitialWindow;
extern Colormap CMap;

extern MyFont TitleBarFont;
extern MyFont MenuFont;
extern MyFont IconFont;
extern MyFont SizeFont;
extern MyFont VersionFont;
extern MyFont InitialFont;
extern MyFont DefaultFont;

extern char *IconDirectory;

extern ColorPair BorderTileC;
extern ColorPair TitleC;
extern ColorPair MenuC;
extern ColorPair MenuTitleC;
extern ColorPair IconC;
extern ColorPair DefaultC;
extern int BorderColor;
extern int MenuShadowColor;
extern int IconBorderColor;

extern Cursor ArrowCursor;
extern Cursor ButtonCursor;
extern Cursor MoveCursor;
extern Cursor ClockCursor;
extern Cursor LeftArrowCursor;
extern Cursor UpperLeftCursor;
extern Cursor LowerLeftCursor;
extern Cursor UpperRightCursor;
extern Cursor LowerRightCursor;
extern Cursor UpperCursor;
extern Cursor BottomCursor;
extern Cursor LeftCursor;
extern Cursor RightCursor;
extern Cursor DotCursor;
extern Cursor SkullCursor;

extern GC MenuNormalGC;
extern GC MenuReverseGC;
extern GC MenuXorGC;
extern GC MenuTitleGC;
extern GC IconNormalGC;
extern GC VersionNormalGC;
extern GC SizeNormalGC;
extern GC InitialNormalGC;
extern GC DrawGC;
extern GC BorderGC;

extern XClassHint NoClass;

extern XContext SwmContext;
extern XContext MenuContext;

extern int BorderWidth;
extern int EdgeWidth;
extern unsigned long Foreground;
extern unsigned long Background;
extern unsigned long Black;
extern unsigned long White;
extern Pixmap GrayTile;

extern char Version[100];
extern Pixmap UnknownPm;
extern int UnknownWidth;
extern int UnknownHeight;
extern int FirstTime;
extern int ReverseVideo;
extern int FocusRoot;
extern SwmWindow *Focus;
extern int WarpCursor;
extern int DontMoveOff;
extern int DoZoom;
extern int ForceIcon;
extern int NoRaiseMove;
extern int NoRaiseResize;
extern int NoRaiseDeicon;
extern int Monochrome;
extern int TitleFocus;
extern int NoTitlebar;
extern int DecorateTransients;
extern int IconifyByUnmapping;
extern int BackingStore;
extern int SaveUnder;
extern int RandomPlacement;
extern char *Home;
extern int HomeLen;
extern int ParseError;
extern int Highlight;
extern int MyDisplayWidth;
extern int MyDisplayHeight;

extern int TitleBarX;
extern int HandlingEvents;

extern Window JunkRoot;
extern Window JunkChild;
extern int JunkX;
extern int JunkY;
extern int JunkWidth;
extern int JunkHeight;
extern int JunkDepth;
extern int JunkBW;
extern int JunkMask;


extern void Done();
extern void Error();
extern void Other();

extern int TempContext;
extern int IconTable[20][20];
extern int IconTableWidth;
extern int IconWidth, IconHeight;
extern int IconBMHeight;
extern int IconBMX, IconBMY;
extern int IconLabelY;
extern int IconBorderWidth;
extern int IconShadowWidth;
extern int ResizeEdgeWidth;

extern int ClampTop;
extern int ClampBottom;
extern int ClampLeft;
extern int ClampRight;

extern int ClickTime0,ClickTime1;

extern SwmWindow *RootDesktop;
extern SwmWindow *TempDesktop;

#endif _SWM_
