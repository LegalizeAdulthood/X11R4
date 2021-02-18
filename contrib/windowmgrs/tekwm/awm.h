#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: awm.h,v 5.55.1.1 89/11/22 15:25:11 glennw Exp $";
#endif /* RCS_ID */
#endif /* LINT */
 
/*
 * Copyright 1987, 1988 by Digital Equipment Corporation, Maynard,
 * Massachusetts, and the Massachusetts Institute of Technology, Cambridge,
 * Massachusetts.  Copyright 1987 by Jordan Hubbard.  Copyright 1987, 1988 by
 * Ardent Computer Corporation, Sunnyvale, Ca.  Copyright 1989 by Tektronix,
 * Inc., Beaverton, Oregon.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Ardent Computer Corporation, Jordan
 * Hubbard, Digital Equipment, Tektronix or M.I.T.  not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Ardent Computer Corporation,
 * Jordan Hubbard, Digital Equipment, Tektronix and M.I.T.  make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL EQUIPMENT, TEKTRONIX,
 * AND M.I.T.  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL
 * EQUIPMENT, TEKTRONIX, OR M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Glenn Widener, Tektronix, Inc.
 *          P.O. Box 1000
 *          Wilsonville, OR, 97070
 *          glennw@orca.wv.tek.com
 */
 
/*
 * MODIFICATION HISTORY
 *
 * 000 -- M. Gancarz, DEC Ultrix Engineering Group
 * 001 -- R. Kittell, DEC Storage A/D May 19, 1986
 *  Added global vars for warp options.
 * 002 -- Loretta Guarino Reid, DEC Ultrix Engineering Group,
 *  Western Software Lab, Port to X11
 * 003 -- Jordan Hubbard, Ardent Computer
 *  Many additional declarations for awm.
 */
 
#include "malloc.h"	/* malloc checker; declares alloc routines */

#include <stdio.h>
#include "X11/Xos.h"
#include "X11/Xlib.h"
#include "X11/Xatom.h"
#ifndef lex_include	    /* lex defines "input", conflicts with XWMHints */
#include "XICCC.h"
#include "X11/Xutil.h"
#endif

/* start defining these structures right! */
typedef struct {
	short deltax;
	short deltay;
	short border_width;
} XWMDecoration;

XWMDecoration *XGetWMDecoration();
XWMDecoration *XAllocWMDecoration();

#include <errno.h>

/* from <X11/Intrinsic.h>: */
typedef char Boolean;
typedef unsigned long Pixel;
#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

#include "support.h"
 
/* The following belongs in Xutil.h (as soon as it is blessed by MIT) */
char *XGetResource();

#ifndef MIN
#define MIN(x, y)	((x) <= (y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x, y)	((x) >= (y) ? (x) : (y))
#endif

#define VOLUME_PERCENTAGE(x)	((x)*14) 
#define ModMask 0xFF
#define ButtonMask(b)	(((b)==Button1) ? Button1Mask : \
			  (((b)==Button2) ? Button2Mask : Button3Mask))

#define DEF_DELTA		1
#define DEF_FUNC		GXcopy
#define DEF_WIN_BORDER_WIDTH 	1
#define DEF_TITLE_BORDER_WIDTH 	1
#define DEF_ICON_BORDER_WIDTH 	2
#define DEF_ICON_PAD		4
#define DEF_POP_BORDER_WIDTH 	2
#define DEF_POP_PAD		4
#define DEF_MENU_BORDER_WIDTH 	2
#define DEF_MENU_PAD		4
#define DEF_GADGET_PAD		3
#define DEF_GADGET_BORDER	1
#define DEF_TITLE_PAD		2
#define DEF_VOLUME		4
#define DEF_PUSH		5
#define DEF_BCONTEXT_WIDTH	0
#define DEF_RAISE_DELAY		100	/* milliseconds */
#define DEF_MAX_COLORS		0	/* 0 means take as many as we can */
#ifndef DEF_BCONTEXT_CURSOR
#define DEF_BCONTEXT_CURSOR	XC_plus
#endif	/* DEF_BCONTEXT_CURSOR */
#ifndef	DEF_TITLE_CURSOR
#define DEF_TITLE_CURSOR	XC_left_ptr
#endif	/* DEF_TITLE_CURSOR */
/* used only for resource file names */
#ifndef	NAME
#define NAME			"Tekwm"
#endif	/* NAME */
#ifndef CLASS
#define CLASS			"Tekwm.Wm"
#endif	/* CLASS */
#ifndef WMCLASS
#define WMCLASS			"Wm"	    /* the class of all wm's */
#endif	/* WMCLASS */
#ifndef	DEF_FONT
#define	DEF_FONT		"fixed"
#endif	/* DEF_FONT */
#ifndef	DEF_TITLE_FONT
#define	DEF_TITLE_FONT		"8x13"
#endif	/* DEF_TITLE_FONT */
#ifndef	DEF_ICON_FONT
#define	DEF_ICON_FONT		"8x13"
#endif	/* DEF_ICON_FONT */
#ifndef	DEF_POPUP_FONT
#define	DEF_POPUP_FONT		"9x15"
#endif	/* DEF_POPUP_FONT */
#ifndef	DEF_GADGET_FONT
#define DEF_GADGET_FONT		"fixed"
#endif	/* DEF_GADGET_FONT */
#ifndef	DEF_MENU_FONT
#define	DEF_MENU_FONT		"8x13"
#endif	/* DEF_MENU_FONT */
#ifndef	DEF_BOLD_FONT
#define	DEF_BOLD_FONT		"8x13bold"
#endif	/* DEF_BOLD_FONT */
#define DEF_MENU_DELTA		20

#define PTEXT_LENGTH		9
#define INIT_PTEXT		{'0', '0', '0', '0', 'x', '0', '0', '0', '0'}

#define       PidFileFormat   "/tmp/tekwm%1x_pid"
#ifndef TEMPFILE
#define TEMPFILE		"/tmp/tekwm.XXXXXX"
#endif	/* TEMPFILE */
#ifndef SYSFILE
#define SYSFILE			"/usr/lib/X11/tekwm/system.tekwmrc"
#endif	/* SYSFILE */
#ifndef SESSIONFILE
#define SESSIONFILE		"xusersetup"
#endif	/* SESSIONFILE */

#define CURSOR_WIDTH		16
#define CURSOR_HEIGHT		16
 
#define MAX_ZAP_VECTORS		8
#define MAX_BOX_VECTORS		31	/* a few extra for good meaasure... */
 
#define RESIZE_BOUNDARY		4

#define DRAW_WIDTH		0 	/* use fastest hardware draw */
#define DRAW_VALUE		0xfd
#define DRAW_FUNC		GXxor
#define DRAW_PLANES		1

#define WARP_UPPER_CENTER       1       /* warp to titlebar center */

/*
 * The first BITS_USED bits of the mask are used to define the most generic
 * types. All other bits are free for storing peripheral information.
 * For now, we use the extra bits to specify which gadget(s) are bound
 * to an action. MAX_GADGETS depends on BITS_USED to determine the maximum
 * number of gadgets possible. If you add a new context type, be sure to
 * increment BITS_USED.
 */
#define ROOT			0x1
#define WINDOW			0x2
#define ICON			0x4
#define TITLE			0x8
#define BORDER			0x10
#define GADGET			0x20

#define BITS_USED		6

/* Window states */
#define ST_WINDOW	0x1
#define ST_ICON		0x2
#define ST_PLACED	0x4
#define ST_DECORATED	0x8

/* Window attributes */
#define AT_NONE		0x0
#define AT_TITLE	0x1
#define AT_GADGETS	0x2
#define AT_RAISE	0x4
#define AT_BORDER	0x8
#define AT_INPUT	0x10
#define AT_PINSIDE	0x20
#define AT_SIZE		0x40
#define AT_POSITION	0x80
#define AT_GADGETS_AUTOEXEC	0x100

/* Session commands */
#define	NULL_SESSION		0  /* no session command called yet	*/
#define	RESTART_SESSION		1  /* RestartSession has been called	*/
#define	END_SESSION		2  /* EndSession has been called	*/
#define	TERMINATE_SESSION	3  /* TerminateSession has been called once */

/* Session related defines */
#define	RES_NAME_LOGIN	 "login"	/* WM_NAME value for the login window */
#define	RES_NAME_CONSOLE "console"	/* WM_NAME value for the console win */

/*
 * Gadgets aren't the sort of embellishments that one uses in quantitity
 * (unless one is designing a truly odd interface), so we keep the information
 * in an array instead of using the usual linked list. MAX_GADGETS is a derived
 * macro (see BITS_USED) that is used to determine the size of the
 * Array.
 *
 */
 
#define MAX_GADGETS		((sizeof(int) * 8) - BITS_USED)

#ifndef FAILURE
#define FAILURE	0
#endif	/* FAILURE */

#define NAME_LEN		256	/* Maximum length string names */
#define EVENTMASK		(ButtonPressMask | ButtonReleaseMask)


#define DrawBox() XDrawSegments(dpy, RootWindow(dpy, scr),DrawGC,box,num_vectors)
#define DrawZap() XDrawSegments(dpy, RootWindow(dpy, scr),DrawGC,zap,num_vectors)

#define gray_width 16
#define gray_height 16
extern char gray_bits[];

#define solid_width 16
#define solid_height 16
extern char solid_bits[];
 
/*
 * All one needs to know about an awm managed window.. (so far...)
 */
#ifndef lex_include
/* can't declare AwmInfo in lex.c because it needs WMHints... */

typedef struct _awminfo {
     Window title, client, frame, icon, client_icon; /* Associated windows */
     Window *gadgets;			/* associated gadgets */
     char *name;			/* The formatted window name */
     Pixmap back, bold;			/* title background and bold pixmaps */
     Pixmap iconPixmap;			/* icon pixmap */
     Pixmap B_back, B_bold;		/* border background and bold pix */
     Pixmap BC_back, BC_bold;		/* BC back and bold pixmaps */
     unsigned int border_width;		/* original border width */
     int state;				/* The state of the window */
     int visibility;			/* window VisibilityChange state */
     int attrs;				/* Window "attributes" */
     GC winGC;				/* GC at proper depth for window */
     Colormap client_cmap;		/* cached for highlighting */
     int client_depth;			/* cached for colormap change */
     Pixel grayPixel;			/* cached for highlighting */
     Pixel TBackgroundPixel;		/* ditto */
     Pixel WBorderPixel;		/* ditto */
     int icon_visibility;		/* icon VisibilityChange state */
     int client_icon_visibility;	/* client icon VisibilityChg state */
     Pixel BCForegroundPixel;		/* cached for highlighting */
     int win_gravity;			/* saved for undecorating */
     XWindowAttributes xwa;		/* cached at startup only, for now */
     XWMHints *wm_hints;		/* cached at startup only, for now */
     Pixmap user_icon;			/* user-specified icon bitmap file */
     Pixmap user_mask;			/* user-specified icon mask file */
     unsigned int user_width;		/* user-specified icon width */
     unsigned int user_height;		/* user-specified icon height */
} AwmInfo, *AwmInfoPtr;

#endif /* lex_include */

/*
 * This whole section has changed substantially. Basically, since all the
 * variables have vanished into the resource manager, the keyword table
 * only needs to keep track of function pointers, boolean pointers and
 * "special" keywords like menu and gagdet. Since some things are still
 * modifiable from menus (only booleans, currently, though this will change),
 * we keep these in the keyword table even though
 * they're no longer directly modifable from the .awmrc
 */

/*
 * Keyword table entry.
 */
typedef struct _keyword {
     char *name;
     int type;
     Boolean *bptr;
     Boolean (*fptr)();
} Keyword;

/*
 * Keyword table type entry.
 */
#define IsFunction	10	/* also used as an actionLine->type */
#define IsMenuMap	2
#define IsMenu		3
/* #define IsDownFunction	4  also used as an actionLine->type */
#define IsParser	5
#define IsQuitFunction	6
#define IsGadget	7
#define IsBoolean	8
#define IsAction	9
#define IsWindowCommand	11	/* also used as an actionLine->type */

/*
 * Button/key binding type.
 */
typedef struct _binding {
    struct _binding *next;
    int context;
    int mask;
    int button;
    Boolean (*func)();
    char *menuname;
    struct _menuinfo *menu;
} Binding;
 
/*
 * Key expression type.
 */
typedef struct _keyexpr {
    char *name;
    int mask;
} KeyExpr;
 
/*
 * Context expression type.
 */
typedef struct _contexpr {
    char *name;
    int mask;
} ContExpr;

/*
 * Button modifier type.
 */
typedef struct _buttonmodifier {
    char *name;
    int mask;
} ButtonModifier;

/*
 * Gravity expression type.
 */
typedef struct _gravityexpr {
     char *name;
     int mask;
} GravityExpr;

/*
 * Button modifier mask definitions.
 * bits 13 and 14 unused in key masks, according to X.h
 * steal bit 15, since we don't use AnyModifier
 */

#define DeltaMotion	(1<<13)
#define ButtonUp	(1<<14)
#define ButtonDown	AnyModifier
#define ButtonMods	DeltaMotion+ButtonUp+ButtonDown

/* 
 * Button and mask redefinitions, for X11
 */
#define LeftMask 	Button1Mask
#define MiddleMask 	Button2Mask
#define RightMask 	Button3Mask
#define LeftButton	Button1
#define MiddleButton	Button2
#define RightButton	Button3

/*
 * Declaration specific information for gadgets. This defines only gadget
 * types, not the actual gadgets. The pixmap member is only used if a pixmap
 * is being displayed. Gravity and offset are purely optional.
 */

#define NoGadgetGravity		0
#define LeftGadgetGravity	1
#define RightGadgetGravity	2
#define CenterGadgetGravity	3

typedef struct _gadgetdecl {	/*   Declaration (type) information */
     unsigned char *name;	/* Either text label or pixmap file name */
     unsigned char *data;	/* If pixmap file, this is the data from it */
     char *forecolor;		/* foreground color for pixmap */
     char *backcolor;		/* background color for pixmap */
     XFontStruct *fontInfo;	/* font for text */
     int high, wide;		/* width and height of pixmap or text */
     int gravity;		/* stick to the left or right? */
     int offset;		/* offset from previous item */
} GadgetDecl;

/*
 * MenuInfo data type.
 */
typedef struct _menuinfo {
    char *name;			/* Name of this menu. */
    Window w;			/* Menu window. */
    int iheight;		/* Height of an item. */
    int width;			/* Width of this menu. */
    int height;			/* Height of this menu. */
    Pixmap image;		/* Saved image of the menu. */
    char *foreground;		/* Name of foreground color. */
    char *background;		/* Name of background color. */
    char *fghighlight;		/* Name of hightlight foreground color. */
    char *bghighlight;		/* Name of highlight background color. */
    XColor fg;			/* Foreground color definition. */
    XColor bg;			/* Background color definition. */
    XColor hlfg;		/* Foreground highlight color definition. */
    XColor hlbg;		/* Background highlight color definition. */
    struct _actionline *line;	/* Linked list of menu items. */
} MenuInfo;
 
/*
 * Action Line data type.
 */
typedef struct _actionline {
    struct _actionline *next;	/* Pointer to next line. */
    char *name;			/* Name of this line. */
    int type;			/* Type of action, IsText, etc, ... */
    Window w;			/* Subwindow for this line. */
    char *text;			/* Text string to be acted upon. */
    Boolean (*func)();		/* Window manager function to be invoked. */
    struct _menuinfo *menu;	/* Menu to be invoked. */
    char *foreground;		/* Name of foreground color. */
    char *background;		/* Name of background color. */
    XColor fg;			/* Foreground color definition. */
    XColor bg;			/* Background color definition. */
} ActionLine;
 
/*
 * ActionLine->type definitions.
 */
#define IsShellCommand		1
#define IsText			2
#define IsTextNL		3
#define IsDownFunction		4
#define IsMenuFunction		5
#define IsImmFunction		6	/* Immediate (context-less) function. */
#define IsVar			7	/* we're setting a boolean variable */
			       /* (awm menu leftover ifwe want to add this) */
/* #define IsWindowCommand	11  also used as a keyword table type */

/*
 * Menu Link data type.  Used by the parser when creating a linked list
 * of menus. 
 */
typedef struct _menulink {
    struct _menulink *next;	/* Pointer to next MenuLink. */
    struct _menuinfo *menu;	/* Pointer to the menu in this link. */
} MenuLink;

/*
 * External variable definitions.
 */
extern int errno;
extern Window Pop;		/* Pop-up dimension display window. */
extern Window Frozen;		/* Contains window id of "gridded" window. */
extern Window FocusWindow;	/* in FocusChng.c - keep track of focus */
extern Window LoginWindow;	/* window with res_name=="login" */
extern Window ConsoleWindow;	/* window with res_name=="console" */
extern Window WMSMWindow;	/* unmapped window for tekwm's WM_PROTOCOLS */
extern Atom WmClosedown;	/* New proposed ICCCM protocol atoms */
extern Atom WmPrepareToDie;	/* New proposed ICCCM protocol atoms */
extern Atom WmDeleteWindow;	/* used in Restart.c, RegisterWm */
extern Atom WmProtocols;	/* ditto; intern these just once */
extern Atom WmChangeState;	/* used in GetButton */
extern XFontStruct *IFontInfo;	/* Icon text font information. */
extern XFontStruct *PFontInfo;	/* Pop-up text font information. */
extern XFontStruct *MFontInfo;	/* Menu text font information. */
extern XFontStruct *TFontInfo;	/* Title text font information. */
extern XFontStruct *TFontBoldInfo;/* Title text (bold) font information. */
extern XFontStruct *GFontInfo; /* Gadget box text font */
extern Pixmap GrayPixmap;	/* Gray pixmap. */
extern Pixmap SolidPixmap;
extern Pixmap IBackPixmap;	/* Icon window background pixmap. */
extern Pixel BackColor;		/* default background color */
extern Pixel ForeColor;		/* default foreground color */
extern Pixel IBackground;	/* Icon pixmap background color */
extern Pixel IBorder;		/* Icon window border pixel. */
extern Pixel IForeground;	/* Icon pixmap foreground color */
extern Pixel ITextForeground;	/* Icon window text forground color. */
extern Pixel ITextBackground;	/* Icon window text background color. */
extern Pixel MBackground;	/* Menu (title) background color */
extern Pixel MBorder;		/* Menu border color */
extern Pixel MForeground;	/* Menu (title) foreground color */
/* note - following 4 resources await implementation in menu */
extern Pixel MHlBackground;	/* Menu highlight background color */
extern Pixel MHlForeground;	/* Menu highlight foreground color */
extern Pixel MTextBackground;	/* Menu window item background color. */
extern Pixel MTextForeground;	/* Menu window item forground color. */
extern Pixel PBackground;	/* Pop-up window background color. */
extern Pixel PBorder;		/* Pop-Up Window border pixel. */
extern Pixel PForeground;	/* Pop-up window forground color. */
extern char *BCForeground;	/* Border Context foreground pixel */
extern char *BCBackground;	/* Border Context (pixmap) background pixel */
extern char *BBackground;	/* Border (pixmap) background pixel */
extern char *WBorder;		/* Window border pixel */
extern char *TTextForeground;	/* Title text foreground pixel */
extern char *TTextBackground;	/* Title text background pixel */
extern char *TForeground;	/* Title (pixmap) foreground pixel */
extern char *TBackground;	/* Title (pixmap) background pixel */
extern char *Foreground;	/* default forground color (text) */
extern char *Background;	/* default background color (text) */
extern Cursor ArrowCrossCursor; /* Arrow cross cursor. */
extern Cursor TextCursor;	/* Text cursor used in icon windows. */
extern Cursor IconCursor;	/* Icon Cursor. */
extern Cursor MenuCursor;	/* Menu cursor. */
extern Cursor MenuMaskCursor;	/* Menu mask cursor. */
extern Cursor LeftButtonCursor;	/* Left button main cursor. */
extern Cursor MiddleButtonCursor;/* Middle button main cursor. */
extern Cursor RightButtonCursor;/* Right button main cursor. */
extern Cursor TargetCursor;	/* Target (select-a-window) cursor. */
extern Cursor TitleCursor;	/* Title bar cursor */
extern Cursor FrameCursor;	/* Frame cursor */
extern int GadgetBorder;	/* Width of gadget borders */
extern int ScreenWidth;		/* Display screen width. */
extern int ScreenHeight;	/* Display screen height. */
extern int TBorderWidth;	/* title bar border width - obsolete. */
extern int WBorderWidth;	/* Window border width. */
extern int IBorderWidth;	/* Icon window border width. */
extern int TitleHeight;		/* Height in pixels of title bar(s) */
extern int titleHeight;		/* Derived height of title bar(s) */
extern int frameHeightExcess;	/* base diff between client and frame height */
extern int frameWidthExcess;	/* diff between client and frame width */
extern int borderHeightExcess;	/* diff client/frame height due to BContext */
extern int borderPositionExcess;/* diff client/frame position due to BContext*/
extern int titleHeightExcess;	/* diff client/frame height from tb */
extern int titlePositionExcess;	/* diff client/frame position from tb */
extern int gadgetHeight;	/* Height of highest gadget */
extern int NameOffset;		/* Offset for window name */
extern int PWidth;		/* Pop-up window width (including borders). */
extern int PHeight;		/* Pop-up window height (including borders). */
extern int PBorderWidth;	/* Pop-up window border width. */
extern int VPopupPad;		/* Pop-up window vertical padding. */
extern int HPopupPad;		/* Pop-up window horizontal padding. */
extern int MBorderWidth;	/* Menu window border width. */
/* following is declared in Menu.c */
extern int Delay;		/* Delay between clicks of double click. */
extern int Delta;		/* Mouse movement slop. */
extern int HIconPad;		/* Icon horizontal padding. */
extern int VIconPad;		/* Icon vertical padding. */
extern int HMenuPad;		/* Menu horizontal padding. */
extern int VMenuPad;		/* Menu vertical padding. */
extern int Pushval;		/* Number of pixels to push window by. */
extern int BContext;		/* Width of border context area in pixels */
extern int RaiseDelay;		/* Delay in milliseconds before autoraising windows */
extern int NumGadgets;		/* Number of gadgets used */
extern int GadgetPad;		/* Padding between gadgets */
extern int Volume;		/* Audible alarm volume. */
extern int status;		/* Routine return status. */
extern int Maxfd;		/* Maximum file descriptors for select(2). */
extern int BCursor;		/* Border context cursor */
extern int TCursor;		/* Title context cursor */
extern MenuLink *Menus;		/* Linked list of menus. */
extern GC  IconGC;		/* graphics context for icon */
extern GC  PopGC;		/* graphics context for pop */
extern GC  MenuGC;		/* graphics context for menu */
extern GC  MenuBgGC;		/* graphics context for menu */
extern GC  MenuInvGC;		/* graphics context for menu background */
extern GC  DrawGC;		/* graphics context for zap */

extern Boolean EventDebug;	/* print info for events received */
extern Boolean Syncronize;	/* Sync up after each Xlib call */
extern Boolean PrintProtoErrors;/* print info for errors received */
extern Boolean Autoraise;	/* Raise window on input focus? */
extern Boolean Autoselect;	/* Warp mouse to default menu selection? */
extern Boolean Autoexecutegadgets; /* Auto execute menu ops on gadgets? */
extern Boolean BackingStoreMenus;/* Backing store for menus? */
extern Boolean BorderHilite;	/* Should we highlight borders on focus? */
extern Boolean BorderContextHilite; /* Should we highlight context on focus? */

extern Boolean ConstrainResize;	/* Don't resize until pointer leaves window */
extern Boolean FocusSetByUser;     /* True if f.focus called */
extern Boolean FocusSetByWM;	/* True if awm set the focus */
extern Boolean FrameFocus;	/* Should frame be considered part of window */
extern Boolean Freeze;		/* Freeze server during move/resize? */
extern Boolean Grid;		/* Should the m/r box contain a 9 seg. grid. */
extern Boolean Hilite;		/* Should we highlight titles on focus? */
extern Boolean InstallColormap;	/* Install colormap for clients? */
extern Boolean NIcon;		/* Normalize icons? */
extern Boolean NWindow;		/* Normalize windows? */
extern Boolean NoFocus;		/* True if should not grab focus back when
				 * client takes it */
extern Boolean Push;		/* Relative=TRUE, Absolute=FALSE. */
extern Boolean ResizeRelative;	/* Relative=TRUE, Absolute=FALSE. */
extern Boolean Reverse;		/* Reverse video? */
extern Boolean RootResizeBox;	/* Should resize box obscure window? */
extern Boolean SaveUnder;	/* True if should use saveunders if possible */
extern Boolean ShowName;	/* Display names in title bars */
extern Boolean Snatched;	/* We're in the middle of an no-highlight/raise op */
extern Boolean Startup_File_Error;	/* Startup file error flag. */
extern Boolean Titles;		/* Title bars on windows? */
extern Boolean UseGadgets;	/* Gadget boxes in title bars? */
extern Boolean Wall;		/* Don't allow windows past edges of screen */
extern Boolean WarpOnDeIconify;/* Warp to upper right corner on de-iconify. */
extern Boolean WarpOnIconify;   /* Warp to icon center on iconify. */
extern Boolean WarpOnRaise;	/* Warp to upper right corner on raise. */
extern Boolean Zap;		/* Should the the zap effect be used. */
 
extern char PText[];		/* Pop-up window dummy text. */
extern int PTextSize;		/* Pop-up window dummy text size. */

extern unsigned int OutstandingReparents;/* Any ReparentNotifys coming? */
extern int Lineno;		/* Line count for parser. */
extern char untitledWindowName[];
				/* default window name - don't free it! */
extern char UnnamedIcon[];
				/* default icon name - don't free it! */
extern char Startup_File[];	/* Startup file name. */
extern char Session_File[];	/* Session file name. */
extern char *IFontName;		/* Icon font name. */
extern char *PFontName;		/* Pop-up font name. */
extern char MFontName[];	/* Menu font name. */
extern char *TFontName;		/* Title font name. */
extern char *GFontName;		/* Gadget font name */
extern char *TFontBoldName;	/* Bold Title font name. */
extern char *TBackPixmapName;	/* Title pixmap file */
extern char *TBoldPixmapName;	/* Title (highlighted) pixmap file */
extern char *TBackPixmapData;	/* Bitmap data file title background */
extern char *TBoldPixmapData;	/* ditto, except highlighted */
extern char *BCPixmapData;	/* Border Context area pixmap data */
extern char *BCBoldPixmapData;	/* Border Context bold pixmap data */
extern char *BPixmapData;	/* Border pixmap data */
extern char *BBoldPixmapData;	/* Border bold pixmap data */
extern char *awmPath;		/* Pathlist for pixmap files */
extern int  Argc;		/* Count of command line parameters. */
extern char **Argv;		/* Pointer to command line parameters. */
extern char **Environ;		/* Pointer to environment. */
 
extern char *DefaultBindings[];	/* Default bindings string array. */
extern Keyword KeywordTable[];	/* Keyword lookup table. */
extern Binding *Blist;		/* Button/key bindings list. */
extern KeyExpr KeyExprTbl[];	/* Key expression table. */
extern ContExpr ContExprTbl[];	/* Context expression table. */
extern ButtonModifier ButtModTbl[];/* Button modifier table. */
extern GravityExpr GravityExprTbl[]; /* Gravity expression table. */

extern GadgetDecl **Gadgets;	/* Gadgets declared. See gram.y */
extern int scr;
extern Display *dpy;		/* Display info pointer. */
extern int last_server_time;
 
#ifdef PROFIL
int ptrap();
#endif
 
extern char *progname;		/* argv[0] */

extern char error_msg[];	/* temporary for printing warnings */

/*
 * External routine typing.
 */
extern Boolean Beep();
extern Boolean CircleDown();
extern Boolean CircleUp();
extern Boolean Continue();
extern Boolean EndSession();
extern Boolean Focus();
extern Boolean UnFocus();
extern Boolean GetButton();
extern Boolean Iconify();
extern Boolean IsIcon();
extern Boolean IsWmName();	 /* string compare of window's WM_NAME */
extern Boolean IsClassResName(); /* string compare of window's res_name */
extern Boolean IsClassResClass();/* string compare of window's class_name */
extern Boolean Lower();
extern Boolean DoAction();
extern Boolean DoWindowCommand();
extern Bool Menu();
extern Boolean Move();
extern Boolean MoveOpaque();
extern Boolean Neaten();
extern Boolean NewIconify();
extern Boolean Pause();
extern Boolean ShoveDown();
extern Boolean ShoveLeft();
extern Boolean ShoveRight();
extern Boolean ShoveUp();
extern Boolean Quit();
extern Boolean Raise();
extern Boolean RaiseAndLower();
extern Boolean Redraw();
extern Boolean Refresh();
extern Boolean ResetBindings();
extern Boolean ResetMenus();
extern Boolean Resize();
extern Boolean Restart();
extern Boolean RestartSession();
extern Boolean FDecorate();
extern Boolean FNoDecorate();
extern Boolean CanonicalDelete();
extern Boolean TerminateClient();
extern Boolean KillClient();
extern Boolean GetBoolRes();
extern Boolean ConfigureWindow();
extern Boolean IsButtonDown();
extern int StoreCursors();
extern int StoreBox();
extern int StoreTitleBox();
extern int StoreGridBox();
extern int StoreTitleGridBox();
extern int StoreZap();
extern int Error();
extern int XError();
extern int CreateMenus();
extern int GetIntRes();
extern Window Reparent(), Decorate();
extern unsigned char *expand_metachars();
extern char *stash();
extern char *GetIconName();
extern void AdjustIconGeom();
extern char *expand_from_path();
extern char *GetStringRes();
extern char *GetPixmapDataRes();
extern char *GetDefaultIconSize();
extern void GetIconSize();
extern void AdjustIconX();
extern Pixmap GetPixmapRes();
extern Pixel GetColorRes();
extern Pixel GetPixel();
extern Pixel LookupColor();
extern XFontStruct *GetFontRes();
extern Drawable GetPixmapFromCache();
#ifndef lex_include
extern AwmInfoPtr GetAwmInfo();
extern AwmInfoPtr RegisterWindow();
extern AwmInfoPtr IsTitled();
extern AwmInfoPtr IsGadgetWin();
#endif /* lex_include */

extern void Init_Titles(), Init_Frames();
extern void NoDecorate();
extern void PaintTitle();
extern void SetBorderNormalColors();
extern void SetBorderPixmaps();
extern void FreePixmapFromCache();
extern void SetFrameColors();
extern void SetTitleColors();
extern void SetGadgetColors();
extern void SetGadgetColor();
extern void ResetGadgets();
extern void PutGadgets();
extern void PaintGadgets();
extern void TerminateSession();
extern void Warning();
extern void print_event_info();

#ifdef	NEATEN
#define DEFAULT_ABS_MIN		64
#define SEPARATION		2
#define DEF_PRIMARY_PLACEMENT	"Top"
#define DEF_SECONDARY_PLACEMENT	"Left"

extern int AbsMinWidth;
extern int AbsMinHeight;
extern Boolean RetainSize;
extern Boolean KeepOpen;
extern Boolean Fill;
extern Boolean UsePriorities;
extern Boolean FixTopOfStack;
extern char *PrimaryIconPlacement;
extern char *SecondaryIconPlacement;
#endif	/* NEATEN */
