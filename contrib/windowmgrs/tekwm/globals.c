#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: globals.c,v 5.31 89/11/22 15:02:10 glennw Exp $";
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
 * 002 -- Loretta Guarino Reid, DEC Ultrix Engineering Group 
 *  Western Software Lab. April 17, 1987
 *  Convert to X11
 * 003 -- Jordan Hubbard, U.C. Berkeley. Misc new vars.
 */
 
#include "awm.h"
#include "X11/Xutil.h"
 
Window Pop;			/* Pop up dimension display window. */
Window Frozen;			/* Contains window id of "gridded" window. */
Window LoginWindow;             /* Window whose res_name == "login" */
Window ConsoleWindow;           /* Window whose res_name == "console" */
Window WMSMWindow;		/* unmapped window for tekwm's WM_PROTOCOLS */
Atom WmClosedown;		/* New proposed ICCCM protocol atoms */
Atom WmPrepareToDie;		/* New proposed ICCCM protocol atoms */
Atom WmDeleteWindow;		/* used in Restart.c, RegisterWm */
Atom WmProtocols;		/* ditto; intern these just once */
Atom WmChangeState;		/* used in GetButton */
XFontStruct *IFontInfo;		/* Icon text font information. */
XFontStruct *PFontInfo;		/* Pop-up text font information. */
XFontStruct *MFontInfo;		/* Menu text font information. */
XFontStruct *TFontInfo;		/* Title text font information. */
XFontStruct *TFontBoldInfo;	/* Title text (bold) font information. */
XFontStruct *GFontInfo;		/* Gadget box text font */
Pixmap GrayPixmap;		/* Gray pixmap. */
Pixmap SolidPixmap;		/* Solid pixmap */
Pixmap IBackPixmap;		/* Icon window background pixmap. */
Pixel BackColor;		/* Generic background color */
Pixel ForeColor;		/* Generic foreground color */
Pixel IBackground;		/* Icon background (for pixmap) color */
Pixel IBorder;			/* Icon window border color. */
Pixel IForeground;		/* Icon foreground (for pixmap) color */
Pixel ITextBackground;		/* Icon text background color */
Pixel ITextForeground;		/* Icon text foreground color */
Pixel MBackground;		/* Menu (title) background color */
Pixel MBorder;			/* Menu border color */
Pixel MForeground;		/* Menu (title) foreground color */
/* note - following 4 resources await implementation in menu */
Pixel MHlBackground;		/* Menu highlight background color */
Pixel MHlForeground;		/* Menu highlight foreground color */
Pixel MTextBackground;		/* Menu window item background color. */
Pixel MTextForeground;		/* Menu window item forground color. */
Pixel PBackground;		/* Pop-up Window background color. */
Pixel PBorder;			/* Pop-Up Window border color. */
Pixel PForeground;		/* Pop-up Window foregrould color */
char *Foreground;		/* foreground color (text) */
char *Background;		/* background color (text) */
char *BCForeground;		/* Border Context foreground pixel */
char *BCBackground;		/* Border Context (pixmap) background pixel */
char *BBackground;		/* Border (pixmap) background pixel */
char *WBorder;			/* Window border color */
char *TBackground;		/* Titlebar background color */
char *TForeground;		/* Titlebar foreground color */
char *TTextBackground;		/* Titlebar text background color */
char *TTextForeground;		/* Titlebar text foreground color */
Cursor ArrowCrossCursor;	/* Arrow cross cursor. */
Cursor TextCursor;		/* Text cursor used in icon windows. */
Cursor IconCursor;		/* Icon Cursor. */
Cursor MenuCursor;		/* Menu cursor. */
Cursor MenuMaskCursor;		/* Menu mask cursor. */
Cursor LeftButtonCursor;	/* Left button main cursor. */
Cursor MiddleButtonCursor;	/* Middle button main cursor. */
Cursor RightButtonCursor;	/* Right button main cursor. */
Cursor TargetCursor;		/* Target (select-a-window) cursor. */
Cursor TitleCursor;		/* Titlebar cursor */
Cursor FrameCursor;		/* Border context cursor */
int GadgetBorder;		/* Width of gadget borders */
int ScreenWidth;		/* Display screen width. */
int ScreenHeight;		/* Display screen height. */
int TBorderWidth;		/* title bar border width. */
int WBorderWidth;		/* Window border width. */
int IBorderWidth;		/* Icon window border width. */
int TitleHeight;		/* Height of title bar(s) (in pixels) */
int titleHeight;		/* Derived height of title bar(s) in pixels */
int frameHeightExcess;		/* base diff between client and frame height */
int frameWidthExcess;		/* diff between client and frame width */
int borderHeightExcess;		/* diff client/frame height due to BContext */
int borderPositionExcess;	/* diff client/frame position due to BContext*/
int titleHeightExcess;		/* diff client/frame height from tb */
int titlePositionExcess;	/* diff client/frame position from tb */
int gadgetHeight;		/* Height of highest gadget */
int NameOffset;			/* Offset for window name */
int PWidth;			/* Pop-up window width. */
int PHeight;			/* Pop-up window height. */
int PBorderWidth;		/* Pop-up window border width. */
int VPopupPad;			/* Pop-up window vertical padding. */
int HPopupPad;			/* Pop-up window horizontal padding. */
int MBorderWidth;		/* Menu window border width. */
int MPadding;			/* Menu window padding. */
int Delta;			/* Mouse movement slop. */
int HIconPad;			/* Icon horizontal padding. */
int VIconPad;			/* Icon vertical padding. */
int HMenuPad;			/* Menu horizontal padding. */
int VMenuPad;			/* Menu vertical padding. */
int Pushval;			/* Number of pixels to push window by. */
int BContext;			/* Border context area width (in pixels) */
int RaiseDelay;			/* Number of milliseconds delay before autoraise */
int NumGadgets;			/* Number of gadgets we're using */
int GadgetPad;			/* Amount of padding for gadgets */
int Volume;			/* Audible alarm volume. */
int status;			/* Routine return status. */
int Maxfd;			/* Maximum file descriptors for select(2). */
int BCursor;			/* Border context cursor */
int TCursor;			/* Title context cursor */
MenuLink *Menus;		/* Linked list of menus. */
GC  IconGC;			/* graphics context for icon */
GC  PopGC;			/* graphics context for pop */
GC  MenuGC;             	/* graphics context for menu */
GC  MenuBgGC;             	/* graphics context for menu */
GC  MenuInvGC;             	/* graphics context for menu background */
GC  DrawGC;			/* graphics context for drawing */
Binding *Blist;			/* Button/key binding list. */

XContext AwmContext;		/* Main context for awm */

Boolean EventDebug;		/* print info for events received */
Boolean Syncronize;		/* Sync up after each Xlib call */
Boolean PrintProtoErrors;	/* print info for errors received */
Boolean Autoraise;		/* Raise window on input focus? */
Boolean Autoselect;		/* Warp mouse to default menu selection? */
Boolean Autoexecutegadgets;	/* Auto execute menu ops on gadgets? */
Boolean BackingStoreMenus;	/* Backing store for menus? */
Boolean BorderHilite;		/* Should we highlight borders on focus? */
Boolean BorderContextHilite;	/* Should we highlight context on focus? */
Boolean ConstrainResize;	/* Don't resize until pointer leaves window */
Boolean FocusSetByUser;         /* True if f.focus called */
Boolean FocusSetByWM;		/* True if awm set the focus */
Boolean FrameFocus;		/* Treat the frame as part of the window? */
Boolean Freeze;			/* Freeze server during move/resize? */
Boolean Grid;			/* Should the m/r box contain a 9 seg. grid. */
Boolean Hilite;			/* Should we highlight titles on focus? */
Boolean InstallColormap;	/* Should we install colormaps for clients? */
Boolean NIcon;                  /* Normalize icons? */
Boolean NWindow;		/* Normalize windows? */
Boolean NoFocus;		/* True if should not retrieve focus when
				 * client grabs it */
Boolean Push;			/* Relative=TRUE, Absolute=FALSE. */
Boolean ResizeRelative;		/* resizes should be relative to window edge */
Boolean Reverse;		/* Reverse video? */
Boolean RootResizeBox;		/* Resize window is placed over sized window? */
Boolean SaveUnder;		/* True if should use saveunders if possible */
Boolean ShowName;		/* If True, print window names in titlebars */
Boolean Startup_File_Error = FALSE;/* Startup file error flag. */
Boolean Titles;			/* Title bar frob on windows? */
Boolean UseGadgets;		/* Gadget bars on titles? */
Boolean Wall;			/* restrict to root window boundries? */
Boolean WarpOnDeIconify;        /* Warp to upper right corner on de-iconify. */
Boolean WarpOnIconify;          /* Warp to icon center on iconify. */
Boolean WarpOnRaise;		/* Warp to upper right corner on raise. */
Boolean Zap;			/* Should the the zap effect be used. */
 
char PText[9] = INIT_PTEXT;	/* Pop-up window dummy text. */
int PTextSize = sizeof(PText);	/* Pop-up window dummy text size. */
 
unsigned int OutstandingReparents = 0;/* Any ReparentNotifys coming? */
int Lineno = 1;			/* Line count for parser. */
char untitledWindowName[] = " Untitled Window ";
				/* default window name - don't free it! */
char UnnamedIcon[] = "Unnamed Icon";
				/* default icon name - don't free it! */
char Startup_File[NAME_LEN] = "";/* Startup file name. */
char Session_File[NAME_LEN] = SESSIONFILE; /* Session startup file name. */
char *TBackPixmapData;		/* Bitmap data file title background */
char *TBoldPixmapData;		/* ditto, except highlighted */
char *BCPixmapData;		/* Border Context area pixmap data */
char *BCBoldPixmapData;		/* Border Context bold pixmap data */
char *BPixmapData;		/* Border pixmap data */
char *BBoldPixmapData;		/* Border bold pixmap data */
char *awmPath;			/* Optional pathlist to search for pixmaps */
int  Argc;			/* Count of command line parameters. */
char **Argv;			/* Pointer to command line parameters. */
char **Environ;			/* Pointer to environment. */

char gray_bits[] = {
     0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa,
     0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa,
     0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa,
     0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa
};

char solid_bits[] = {
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

int scr;
Display *dpy;
int last_server_time;

#ifdef NEATEN
int AbsMinWidth;
int AbsMinHeight;
Boolean RetainSize;
Boolean KeepOpen;
Boolean Fill;
Boolean UsePriorities;
Boolean FixTopOfStack;
char *PrimaryIconPlacement;
char *SecondaryIconPlacement;
#endif

char *progname;			/* argv[0] */

char error_msg[1024];		/* temporary for printing warnings */

/*
 * Keyword lookup table for parser.
 * Note - we have removed Booleans, as many do
 * not work due to the fact that the code assumes that they do not change
 * after startup, and at least one of them can crash Tekwm.
 */
Keyword KeywordTable[] =
{
    { "gadget",		IsGadget,	0, 0 },
    { "f.beep",		IsImmFunction,	0, Beep },
    { "f.circledown",	IsImmFunction,	0, CircleDown },
    { "f.circleup",	IsImmFunction,	0, CircleUp },
    { "f.continue",	IsImmFunction,	0, Continue },
    { "f.decorate",	IsFunction,	0, FDecorate },
    { "f.exit",		IsQuitFunction,	0, Quit },
    { "f.focus",	IsFunction,	0, Focus },
    { "f.unfocus",	IsImmFunction,	0, UnFocus },
    { "f.iconify",	IsDownFunction,	0, Iconify },
    /* Iconify is Down function if de-iconifying an unplaced window,
       or iconifying an unplaced icon */
    { "f.lower",	IsFunction,	0, Lower },
    { "f.action",	IsAction,	0, DoAction },
    { "f.windowcommand",IsWindowCommand,0, DoWindowCommand },
    { "f.menu",		IsMenuMap,	0, Menu },
    { "f.move",		IsDownFunction,	0, Move },
    { "f.moveopaque",	IsDownFunction,	0, MoveOpaque },
    { "f.newiconify",	IsDownFunction,	0, NewIconify },
    { "f.neaten",	IsImmFunction,	0, Neaten },
    { "f.pause",	IsImmFunction,	0, Pause },
    { "f.pushdown",	IsFunction,	0, ShoveDown },
    { "f.pushleft",	IsFunction,	0, ShoveLeft },
    { "f.pushright",	IsFunction,	0, ShoveRight },
    { "f.pushup",	IsFunction,	0, ShoveUp },
    { "f.raise",	IsFunction,	0, Raise },
    { "f.raiseandlower",IsFunction,	0, RaiseAndLower },
    { "f.redraw",	IsFunction,	0, Redraw },
    { "f.refresh",	IsImmFunction,	0, Refresh },
    { "f.resize",	IsDownFunction,	0, Resize },
    { "f.restart",	IsQuitFunction,	0, Restart },
    { "f.restartsession",IsImmFunction, 0, RestartSession },
    { "f.endsession"    ,IsImmFunction, 0, EndSession },
    { "f.destroy",      IsFunction,	0, CanonicalDelete },
    { "f.delete",       IsFunction,	0, CanonicalDelete },
    { "f.terminate",    IsFunction,	0, TerminateClient },
    { "f.kill",         IsFunction,	0, KillClient },
    { "f.nodecorate",   IsFunction,     0, FNoDecorate },
    { "menu",		IsMenu,		0, 0 },
    { "resetbindings",	IsParser,	0, ResetBindings },
    { "resetmenus",	IsParser,	0, ResetMenus },
    { "resetgadgets",	IsParser,	0, ResetGadgets },
    { NULL,		NULL,		NULL, NULL }
};
 
/*
 * Key expression table for parser.
 */
KeyExpr KeyExprTbl[] = {
    { "ctrl", ControlMask },
    { "c", ControlMask },
    { "lock", LockMask },
    { "l", LockMask },
    { "shift", ShiftMask },
    { "s", ShiftMask },
    { "meta", Mod1Mask },
    { "m", Mod1Mask },
    { "mod1", Mod1Mask },
    { "1", Mod1Mask },
    { "mod2", Mod2Mask },
    { "2", Mod2Mask },
    { "mod3", Mod3Mask },
    { "3", Mod3Mask },
    { "mod4", Mod4Mask },
    { "4", Mod4Mask },
    { "mod5", Mod5Mask },
    { "5", Mod5Mask },
    { NULL, NULL }
};
 
/*
 * Context expression table for parser.
 */
ContExpr ContExprTbl[] = {
    { "icon", ICON },
    { "i", ICON },
    { "root", ROOT },
    { "r", ROOT },
    { "window", WINDOW },
    { "w", WINDOW },
    { "t", TITLE },
    { "title", TITLE },
    { "g", GADGET },
    { "gadget", GADGET },
    { "border", BORDER },
    { "b", BORDER },
    { NULL, NULL }
};

/*
 * Gravity expressions for parser.
 */
GravityExpr GravityExprTbl[] = {
    { "noGravity", NoGadgetGravity },
    { "nogravity", NoGadgetGravity },
    { "NoGravity", NoGadgetGravity },
    { "leftGravity", LeftGadgetGravity },
    { "leftgravity", LeftGadgetGravity },
    { "LeftGravity", LeftGadgetGravity },
    { "rightGravity", RightGadgetGravity },
    { "rightgravity", RightGadgetGravity },
    { "RightGravity", RightGadgetGravity },
    { "centerGravity", CenterGadgetGravity },
    { "centerGravity", CenterGadgetGravity },
    { "CenterGravity", CenterGadgetGravity },
    { NULL, NULL}
};

/*
 * Button expression table for parser.
 */
ButtonModifier ButtModTbl[] = {
    { "left", LeftMask },
    { "l", LeftMask },
    { "middle", MiddleMask },
    { "m", MiddleMask },
    { "right", RightMask },
    { "r", RightMask },
    { "move", DeltaMotion },
    { "motion", DeltaMotion },
    { "delta", DeltaMotion },
    { "down", ButtonDown },
    { "d", ButtonDown },    /* not documented - used in Bindings.c */
    { "up", ButtonUp },
    { "u", ButtonUp },	    /* not documented - used in Bindings.c */
    { NULL, NULL }
};
