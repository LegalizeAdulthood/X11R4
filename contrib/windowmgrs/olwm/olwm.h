/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

/* @(#) olwm.h 5.1 89/12/14 Crucible */

#define ABS(a)		(((a) < 0) ? -(a) : (a))
#define	MAX(a,b)	(((a) > (b)) ? (a) : (b))

/* Determine the size of an object type in 32bit multiples.
 * Rounds up to make sure the result is large enough to hold the object. */
#define LONG_LENGTH(a)	((long)(( sizeof(a) + 3 ) / 4))

/* a type for the four possible focus modes */
typedef enum { Passive, NoInput, LocallyActive, GloballyActive } FocusMode;

typedef struct {
        long flags;
        enum {
                MENU_FULL = 0,          /* Close, Zoom, Props, Scale, Back,
                                         * Refresh, Quit
                                         */
                MENU_LIMITED = 1,       /* Dismiss, Scale, Refresh */
                MENU_NONE = 2,  	/* No Menu */
        } menu_type;
        enum { PinIn, PinOut } pushpin_initial_state;
} WMDecorations;

/* value for flags */
#define WMDecorationHeader      (1L<<0)
#define WMDecorationFooter      (1L<<1)
#define WMDecorationPushPin     (1L<<2)
#define WMDecorationCloseButton (1L<<3)
#define WMDecorationOKButton    (1L<<4)
#define WMDecorationResizeable  (1L<<5)

/* window kinds */
#define BASEWINDOW		0
#define CMDWINDOW		1
#define PROPWINDOW		2
#define HELPWINDOW		3
#define NOTICEWINDOW		4
#define OTHERWINDOW		5
#define TRANSIENTWINDOW		6

#define MAX_CHILDREN	10	/* size of child window array */

#ifdef NOT_USED
/* values for state field */
#define	CLIENTMAPPED	(1L << 0)	/* client has mapped itself */
#define	CLIENTUNMAPPED	(1L << 1)	/* client unmapped itself */
#define	ICONIFIED	(1L << 2)	/* base window iconified */
#define	WMUNMAPPED	(1L << 3)	/* wm unmapped */
#define	ICON		(1L << 4)	/* this wininfo is an icon*/
#endif /* NOT_USED */

/* Number of pixels user must move before we will think
 * s/he wants to move a window or draw a select box.
 */
#define MOVETHRESH	5

/* Default name for a window if none provided 
 */
#define DEFAULTWINNAME "No Name"

/*
 * The following structure is defined in Xutil.h, but it differs between X11R3
 * and X11R4 to reflect the acceptance of the ICCCM (version 1.0) as a
 * Consortium standard.  This replacement is necessary so that olwm can be
 * assured of getting the right structure, independent of the include files
 * present on the system it's compiled on.  Defining this this way allows olwm
 * to accomodate clients that use the R4-based structure as well as older
 * clients.
 */
typedef struct {
	long	flags;
	struct {
	    int x;
	    int y;
	    int width;
	    int height;
	} pad;			/* x, y, w, h replaced by pad[4] for X11R4 */
	int	min_width;
	int	min_height;
	int	max_width;
	int	max_height;
	int	width_inc;
	int	height_inc;
	struct	{
	    int x;
	    int y;
	} min_aspect, max_aspect;
	int	base_width;	/* added in X11R4 */
	int	base_height;	/* added in X11R4 */
	int	win_gravity;	/* added in X11R4 */
} NewXSizeHints;

/*
 * The following definitions are new in ICCCM version 1.0.  Olwm requires 
 * them, but they may not be present if olwm is being compiled against an old 
 * version of Xutil.h (e.g. from X11R3).  We define them here if necessary.
 */

#ifndef PBaseSize
#define PBaseSize	(1L << 8) /* program specified base for incrementing */
#endif

#ifndef PWinGravity
#define PWinGravity	(1L << 9) /* program specified window gravity */
#endif

#ifndef WithdrawnState
#define WithdrawnState 0        /* for windows that are not mapped */
#endif

/*
 * The WinInfo structure contains information about each window.
 * A window can be the Frame, Pane, TitleBar, Resize Corner, Pushpin,
 * Menu, or Icon.
 *
 * A TitleBar, for example, would have a WinInfo structure whose frame
 * field would point to the frame in which the title bar is.
 */
typedef enum { Normal, Iconic, Withdrawn } WinState;
typedef struct _wininfo {
	Window			self;
	int			x, y;
	unsigned int		width, height;
	struct	_wininfo	*frame;
	WinState		winState;
	int			winType;
	Colormap		colormap;	/* None by default */
	Window			warpWin;	/* window to warp to */
	Bool			warpBack;
	int			warpRetX, warpRetY;
	int			pendingUnmaps;
	int			(*drawfunc)();	/* redraw function */
	int			(*eventfunc)();	/* event handler */
	int			(*destroyfunc)();
	int			(*selectfunc)(); /* window changed select */
	int			(*focusfunc)();  /* window changed focus */
	int			(*configfunc)();  /* window reconfigured */
	int			(*propfunc)();  /* property changed */
	union 			
	{
		void			*ptr;
		long			val;
	} priv;
} WinInfo;

/* The priv field of the WinInfo structure is used to point to
 * a structure which contains information about the window, or
 * it is a value associated with the window.
 *
 * Frame	- The priv.ptr field points to a FrameInfo
 *		  structure which contains information about
 *		  the frame.  e.g.  what decorations it has,
 *		  whether it currently has the focus, what
 *		  pane is in it.
 *
 * TitleBar	- The priv.ptr field points to a TitleInfo
 *		  structure which contains information about
 *		  the titlbar.  e.g.  the text field which
 *		  will appear in the titlebar, the dimensions
 *		  of the text field, the location of the text
 *		  field.
 *
 * PushPin	- The priv.val field contains the state of
 *		  the pushpin.  It is set to PUSHPIN_IN or
 *		  PUSHPIN_OUT.
 *
 * Menu Window  - The priv.ptr field points to a Menu structure
 *		  which contains information about the menu
 *		  that will go inside this window.  It is a
 *		  pointer to a copy of the Menu structure that
 *		  was used to draw the menu before the user
 *		  pinned it.
 *
 * Icon		- The priv.ptr field points to a iconInfo structure
 *		  which contains information about the icon.
 *		  e.g.  width, height, its pixmap.
 *
 */

typedef struct	_childwininfo {
	int			num;
	WinInfo			*info[MAX_CHILDREN];
} ChildWinInfo;
	
#define	FOOTLEN	50L
#define	FOLLOWNUM	10

typedef struct _followblock {
	WinInfo		*followers[FOLLOWNUM];
	struct	_followblock	*next;
} FollowBlock;

typedef struct _frameinfo {
	Window			pane;
	ChildWinInfo		children;
	char			*name;		/* for title bar */
	char			*iconname;
	Bool			iconified;
	int			oldBorderWidth;
	NewXSizeHints		*normHints;
	Bool			hasMessages;
	XWMHints		*wmHints;
	Bool			isFocus;
	Menu			*frameMenu;
	char			*leftfooter;
	char			*rightfooter;
	WMDecorations		*winDecors;
	WinInfo			*leader;
	FollowBlock		*followers;
	Bool			isSelected;
	int			restoreY, restoreWidth, restoreHeight;
	int			protocols;
	FocusMode		focusMode;
	int			offsetX, offsetY;
	long			configMask;	/* mask for ConfigureWindow */
} FrameInfo;

#define		WMWindowNotBusy		0
#define		WMWindowIsBusy		1

/* protocols bits */
#define		TAKE_FOCUS		(1<<0)
#define		SAVE_YOURSELF		(1<<1)
#define		DELETE_WINDOW		(1<<2)

typedef struct _titleinfo {
	char			*name;		/* pointer to title letters */
	int			decorWidth;	/* width of decors in title 
						 * area in pix*/
	int			titleOff;	/* offset in pix */
	int			nameLength;	/* length of name in chars */
	int			nameWidth;	/* width of name in pix */
	int			x, y;		/* title position */
	long			busyState;
	ChildWinInfo		children;
} TitleInfo;

typedef struct _iconSlot {
	Bool	occupied;
	int	orgx, orgy;	/* original x, y */
	int	x, y, w, h;
	struct _iconSlot *next;
} IconSlot;

typedef enum { AlongTop, AlongBottom, AlongRight, AlongLeft } IconPreference;

typedef struct {
	IconSlot	*iconslot;
	Pixmap		iconPix;
	Window		iconWin;
	int		iconX, iconY;
	unsigned int	iconWidth, iconHeight;
	char		*name;
	int		nameX, nameY;
	int		nameWidth;
	int		nameLength;
} IconInfo;

typedef struct
{
	char *menuString;
	char *execString;
} MenuStr;

/* MenuInfo structures form a system paralell to WinInfo structures for
 * handling menu events. */
typedef struct {
	Bool		valid;
	Menu		*menu;
	WinInfo		*winInfo;
	Bool		childActive;
	Menu		*childMenu;
	Bool		leftPin;  /* NOT USED!!!  pointer has left pushpin */
	Bool		pinIn;			/* current pinState */
	int		litButton;		/* highlighted button */
} MenuInfo;

NewXSizeHints	*GetSizeHints();
WinInfo	*WICreateInfo();
WinInfo *WIGetInfo();
Window	GetPane();
WinInfo	*GetLeader();
char	*GetName();
WinInfo	*GetIcon();
char	*GetIconName();
XWMHints	*GetWMHints();
FollowBlock	*GetFollowers();
Bool		ToggleSelection();
Bool		IsSelected();
WinInfo		*EnumSelections();
WinInfo		*EnumFrames();
WinInfo		*CheckAndMapWin();

extern Atom AtomWinAttr;
extern Atom AtomWTNotice;
extern Atom AtomWTOther;
extern Atom AtomWTHelp;
extern Atom AtomWTBase;
extern Atom AtomWTCmd;
extern Atom AtomWTProp;
extern Atom AtomMenuFull;
extern Atom AtomMenuLimited;
extern Atom AtomNone;
extern Atom AtomPinIn;
extern Atom AtomPinOut;
extern Atom AtomDecorAdd;
extern Atom AtomDecorClose;
extern Atom AtomDecorFooter;
extern Atom AtomDecorResize;
extern Atom AtomDecorHeader;
extern Atom AtomDecorPin;
extern Atom AtomDecorDelete;
extern Atom AtomDecorClose;
extern Atom AtomDecorFooter;
extern Atom AtomDecorResize;
extern Atom AtomDecorHeader;
extern Atom AtomTakeFocus;
extern Atom AtomProtocols;
extern Atom AtomSaveYourself;
extern Atom AtomChangeState;
extern Atom AtomDeleteWindow;
extern Atom AtomPushpinState;
extern Atom AtomShowProperties;
extern Atom AtomWMState;
extern Atom AtomTargets;
extern Atom AtomMultiple;
extern Atom AtomTimestamp;
extern Atom AtomListLength;
extern Atom AtomLength;

extern	unsigned long	foreColor, backColor, borderColor;
extern  unsigned long	bg1Color, bg2Color, bg3Color;


/* types of button presses */
#define	CLICK		0
#define	DOUBLECLICK	1
#define	DRAG		2

/* Anchor points */
#define	AnchorUR	0
#define	AnchorUL	1
#define	AnchorLR	2
#define	AnchorLL 	3

/* Sizes of various graphics bits.  I don't know what will happen
 * if these are not even numbers. */
#define FRAME_OUTLINE_WIDTH	2
#define FRAME_SELECTED_WIDTH	1  /* Width of selected frame in addition 
				    * to the FRAME_OUTLINE_WIDTH.
				    */


/* offsets within title bar of adornments */
#define	SHINEOFF		11 /* 15 */
#define	PUSHOFF			0  /* 5 */
#define	OKOFF			50

/* mouse button definitions */
#define	MB_MENU		1
#define MB_ADJUST	2
#define MB_SELECT	3



WMDecorations	*GetOLWinDecors();
WMDecorations	*GetDecors();

#define	WINDOWMENU(winInfo)	(((FrameInfo *)(winInfo->priv.ptr))->frameMenu)

extern	Menu	FrameFullMenu;
extern	Menu	FrameNormMenu;
extern	Menu	IconFullMenu;
extern	Menu	FrameLimMenu;

/* adornment pixmaps */
extern	Pixmap	pixLLresize;
extern	Pixmap	pixLRresize;
extern	Pixmap	pixULresize;
extern	Pixmap	pixURresize;
extern	Pixmap	pixPushPinRev;
extern	Pixmap	pixPushPinInRev;
extern	Pixmap	pixPushPin;
extern	Pixmap	pixPushPinIn;
extern	Pixmap	menuPushPin;
extern	Pixmap	menuPushPinIn;
extern	Pixmap	pixIcon;
extern	Pixmap	pixmapGray;
extern	Pixmap	pixGray;

/* debug flags */
extern int	PrintOrphans;

/* miscellaneous Xdefaults */
extern Bool FShowMenuButtons;
extern Bool FShowPinnedMenuButtons;
