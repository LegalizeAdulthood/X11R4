/*
 * Copyright 1987 by Tektronix Inc, Beaverton, Oregon.
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Tektronix not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 *
 *
 * TEKTRONIX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * TEKTRONIX BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/* 
 * $XConsortium: resource.h,v 6.3 89/10/08 16:15:39 rws Exp $ 
 * $Locker:  $ 
 */

#include "X11/Xos.h"
#include "X11/Xlib.h"
#include "debug.h"
#include "X10struct.h"

typedef struct _SerialList SerialList, *SerialListptr;
typedef struct _RequestSerial {
	unsigned long	*serial;	/* serial numbers of requests */
	int		count;		/* # of elements in 'list' */
} RequestSerial, *RequestSerialptr;

/*
 * This holds all the information about windows that we need to keep
 * around.  
 */
typedef struct _WinInfo *WinInfoptr;
typedef struct _WinInfo {
	Window		id;		/* window name */
	Window		icon;		/* this is the window id if we're an */
					/* icon; or its the icon id if we're */
					/* a window */
	Pixmap		tile;
	unsigned long	mask;
	unsigned long	grabmask;
	RequestSerial	mapReq;		/* map requests */
	RequestSerial	configureReq;	/* ConfigureWindow requests */
	Window		unmapTransparent;/* window used for unmap transparent */
	Window		originalParent;	/* original parent (if reparented) */
	short		width;		/* inside width */
	short		height;		/* inside height */
	short		borderWidth;
	short		x, y;		/* upper left outside coordinates */
	Bool		transparent;	/* true if transparent window */
	Bool		hasTransparency;/* true if transparent child */
					/* exists */
	Bool		drawThrough;	/* true if ClipModeDrawThrough */
	Bool		mapped;		/* true if window is mapped */
	Bool		mapRequested;	/* true if MapWindow has been done */
	Bool		foriegn;	/* true if window is real x11 client */
	Bool		tileModeRelative;/* true if tileMode is Relative */
	Bool		isIcon;		/* true if this window is an icon */
	Bool		neverMapped;	/* true if never mapped before */
	Pixmap		savedPixmap;	/* non-zero if this is PixmapSave'd */
	unsigned short	configured;	/* >0 if we need an expose event */
					/* when a ConfigureNotify arrives */
	WinInfoptr	sibling;	/* first in list of siblings */
					/* listed in stacking order */
	WinInfoptr	child;		/* first in list of children */
	WinInfoptr	parent;		/* parent window */
	Window		gcWin;		/* GC window (never free this!) */
	GC		gc;
	int		client;		/* client that created this window */
} WinInfo;

/*
 * Information about pixmaps and bitmaps for X10 are kept in one of these.
 * X11 does not distinguish between bitmaps and pixmaps, because bitmaps are
 * pixmaps of depth one.  The bitmapid field may be superfluous because
 * all we use it for is to keep track of a bitmap used to create a
 * pixmap.  Hence, for referece, if bitmapid != 0 ==> this is a pixmap
 * created from a bitmap.  There is no way to infer whether an id
 * refers to a bitmap or a pixmap, except perhaps by the depth of 1.
 */
typedef struct _PixmapInfo *PixmapInfoptr;
typedef struct _PixmapInfo {
	Pixmap		id;		/* pixmap name */
	XID		bitmapid;	/* bitmap name */
	unsigned int	width;
	unsigned int	height;
	unsigned int	depth;
	GC		gc;
	PixmapInfoptr	next;
	Bool		isSavedPixmap;	/* true if this is PixmapSave pixmap */
	Bool		isBackingStore;	/* true if has been used to restore */
	short		x, y;		/* coordinates of root PixmapSave */
} PixmapInfo;

typedef struct _FontInfo *FontInfoptr;
typedef struct _FontInfo {
	Font		id;		/* font name */
	XFontStruct	*info;
	FontInfoptr	next;
	unsigned int	ascent;		/* X10 ascent of font */
	unsigned int	descent;	/* X10 descent of font */
} FontInfo;

/*
 * Standard macro for looking up a drawable.  Note that we invalidate
 * the savedPixmap, if there was one.
 */
#define GetWinInfoOrSetBadDrawable(client, wid, wp, RETURNEXPR)	\
	WinInfoptr wp = WinLookup(client, wid);				\
	if (wp == NULL) {						\
		Xstatus = X10BadWindow;					\
		RETURNEXPR;						\
	}								\
	if (wp->savedPixmap && WindowMapRequested(wp)) {		\
		PixmapInfoptr pp = PixmapLookup(client, wp->savedPixmap); \
		if (pp != NULL)						\
			pp->isBackingStore = False;			\
		wp->savedPixmap = 0;					\
	}								\
	if (WindowsUnmappedTransparent)					\
		ClearUnmappedTransparentTree(wp);

/*
 * Standard macro for looking up a window and setting BadWindow
 * if the window doesn't exist.
 */
#define GetWinInfoOrSetBadWindow(client, wid, wp, RETURNEXPR)		\
	WinInfoptr wp = WinLookup(client, wid);				\
	if (wp == NULL) {						\
		Xstatus = X10BadWindow;					\
		RETURNEXPR;						\
	}

/*
 * Macro to determine if we need to set the clipping rectangle or not.
 */
#define	CheckTransparentDrawableAndXY(dpy, wp, dptr, xptr, yptr)	\
if (wp->transparent)							\
	ChangeXYAndClipAndDrawable(dpy, wp, dptr, xptr, yptr);

/*
 * This handles the sequence recorder.
 */
typedef struct _sequencelist *SequenceListptr;
typedef struct _sequencelist {
	X10Req		*req;		/* list of x10 requests */
	unsigned long	*x11seq;	/* list of X11 sequence numbers */
	int		last;		/* actually, 1 past the last in list */
	int		head;		/* pointer to next open cell */
	int		tail;		/* pointer to first filled cell */
} SequenceList;

#define SEQ_INCREMENT	50	/* increment in size of sequence queue */
/*
 * return the next/previous pointer in the circular list.
 */
#define NextSeqPtr(last, ptr, increment)			\
	(((ptr)+(increment) >= (last)) ?			\
	    (ptr)+(increment)-(last) : (ptr)+(increment))
#define PrevSeqPtr(last, ptr, decrement)			\
	(((ptr)-(decrement) < 0) ?				\
	    (ptr)-(decrement)+(last) : (ptr)-(decrement))

/*
 * return width of data in cirular list.
 */
#define	WidthOfSeqList(seq)					\
		(((seq)->tail < (seq)->head) ?			\
			(seq)->head - (seq)->tail		\
		      : (seq)->last - (seq)->tail + (seq)->head)


/*
 * Given the X10 graphics function, return the appropriate functgion for X11.
 */
#define	X10toX11function(func)				\
	((unsigned)(func) > 15 || NoTransform		\
		? func : GXFunctionInvert[ func ])

PixmapInfoptr	GetTextPixmap();
PixmapInfoptr	CreatePixmap();
PixmapInfoptr	PixmapLookup();
PixmapInfoptr	BitmapLookup();
PixmapInfoptr	StorePixmapOrBitmap();
FontInfoptr	FontLookup();
WinInfoptr	WinLookup();
WinInfoptr	AddWin();
WinInfoptr	FindDrawableParent();

typedef struct _Client {
	Display		*dpy;		/* the display for this client */
	PixmapInfoptr	pixmaps;	/* list of pixmap resources */
	PixmapInfoptr	textBackground;	/* cached pixmap for Text requests */
	FontInfoptr	fontinfo;	/* list of font resources */
	Window		grabwin;	/* window being grabbed */
	SequenceList	seq;		/* sequence<->request association */
	XEvent		buffer;		/* buffer of 1 */

#define	ChangesPending	0x1		/* set if changes pending that may */
					/* generate events */
#define	GraphicsPending	0x2		/* set if graphics output waiting */
#define	DeviceEvents	0x4		/* set if device events delivered */
	u_char		pendingFlags;

	/*
	 * These next four elements are for all permutations of persistent
	 * GC attributes:
	 *	the X10 clipmode of a window (Clipped or Unclipped)
	 *	transparent vs. opaque windows (opaque or clear)
	 *	GC's for use with 1-bit deep bitmaps
	 */
	GC		opaqueClippedGC;
	GC		opaqueUnclippedGC;
	GC		clearClippedGC;
	GC		clearUnclippedGC;
	GC		bitmapGC;
	GC		pixmapGC;
	RequestSerial	unmapTransReq;	/* list of UnmpapTransparent serials */
	unsigned long	lastEventDispatched; /* sn of last event dispatched */
} Client, *Clientptr;

typedef struct _FontMap {
	char	*fontname;
	int	fontindex;
} FontMap, *FontMapptr;

/*
 * This is because I have a stupid profiler that ignores static functions.
 */
#ifdef PROFILE
#define	StaticFunction
#else
#define StaticFunction static
#endif

/*
 * indices for known x11 fonts.
 */
#define	FONT_FIXED		0
#define	FONT_VARIABLE		1
#define	FONT_MAX		1

#ifndef NULL
#define	NULL	0
#endif

#ifndef	BUFSIZ
#define	BUFSIZ	1024
#endif

/*
 * This is selected on all windows that we know about, except for
 * foriegn windows that we discover.
 * Force exposure, StructureNotify and enter/leave events on
 * all windows... these are used for:
 *	Exposure:	to propogate upward to event window,
 *			like X10 does.
 *	enter/leave:	to propogate upward...
 *	StructureNotify: to get the real notification of destroy
 */
#define BasicEventMask	 (ExposureMask		\
			| StructureNotifyMask	\
			| EnterWindowMask	\
			| LeaveWindowMask)

/*
 * event status types
 */
#define	EventOK			1
#define	EventNotSelected	2
#define	EventWindowMissing	3
#define	EventSubWindowMissing	4
#define	EventSelectedByChild	5
#define	EventLockToggleMode	6
#define	EventUnImplemented	7
#define	EventHasNoX10Equiv	8
#define	EventModeField		9
#define	EventNoChange		10
#define	EventSuppressed		11
#define	EventParentNotMapped	12
#define	EventAlreadyMapped	13
#define	EventUnknown		14
#define	EventBelongsToServer	15

int	AfterFunction();
char	**X11FontNames();
char	*ToLower();
char	*Xalloc();
char	*Xrealloc();
char	*StaticAlloc();
char	*X10ProcPixmapGet();
Bool	WindowMapped();
Bool	CausedByRequest();
Bool	InvalidHost();
unsigned long X10toX11EventMask();
unsigned long X11toX10EventMask();
unsigned long X11toX10KeyButState();
unsigned long X10ButtonMaskToX11KeyMask();
unsigned long X10ButtonMaskToX11Button();
unsigned long X10toX11pixel();
void	InvalidateClips();
void	SendEventsToX10Client();
void	SetClipMode();
void	SetTileMode();
void	UpdateWinTree();
void	FreePixmapInfo();
void	FreeFontInfo();
void	FreeWinInfo();
void	FreeClientWindows();
void	Reparent();
void	ResetCache();
void	SetCacheSize();
void	ShowCachePerformance();
void	ShowWinTree();
void	RaiseLowestChild();
void	LowerHighestChild();
void	LowerWindow();
void	RaiseWindow();
void	MarkMappedWinTree();
void	DeleteUnmapTransparentRequest();
void	AddRequestSerial();
void	ClearUnmappedTransparentTree();
void	ClearUnmappedTransparent();
void	MarkWindowDeleted();
XImage	*CreateX10Image();

extern Clientptr	clist[];
extern u_char		GXFunctionInvert[];
extern u_char		Xstatus;
extern Bool		LockToggleMode;
extern Bool		GrabUnviewable;
extern Bool		OverrideRedirect;
extern Bool		FocusKeyboardAllowed;
extern Bool		Bigendian;
extern Bool		NoTransform;
extern Bool		DoSync;
extern Bool		GrabServerAllowed;
extern Bool		ServerGrabbed;
extern int		GrabMouseAllowed;
extern int		MinimumTileSize;
extern int		FileDescriptors;
extern int		WindowsUnmappedTransparent;
extern unsigned int	BestWidth;
extern unsigned int	BestHeight;
extern Colormap		ScreenColormap;
extern Visual		*ScreenVisual;
extern unsigned long	ScreenWhitePixel;
extern unsigned long	ScreenBlackPixel;
extern int		ScreenColorCells;
extern int		ScreenDepth;
extern char		*VisualClassNames[];
extern Time		LastKnownServerTime;
extern Display		*PrimaryConnection;
extern int		PrimaryClient;
extern char		*X11EventNames[];
extern char		*program;
