/*
 *	$XConsortium: ptyx.h,v 1.20 88/11/16 18:13:26 rws Exp $
 *	$Header: ptyx.h,v 1.1 89/05/30 14:53:51 kagotani Rel $
 */

#include <X11/copyright.h>

/*
 *	Copyright 1989 OMRON TATEISI ELECTRONICS CO.
 *
 *	Original Source: ./client/kterm/ptyx.h
 *
 *	Modified by OMRON TATEISI ELECTRONICS CO., to handle XJp-library.
 *
 *	Author:	Seiji Kuwari	OMRON TATEISI ELECTRONICS CO.
 *				kuwa@frf.omron.junet
 *				uunet!nff.ncl.omron.co.jp!kuwa
 */				

/*
 * Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior permission.
 *
 *
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/* ptyx.h */
/* @(#)ptyx.h	X10/6.6	11/10/86 */

#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#ifdef	XJP
#include "XJplib.h"
#endif	/* XJP */

/* Extra Xlib definitions */
#define AllButtonsUp(detail, ignore)  (\
		((ignore) == Button1) ? \
				(((detail)&(Button2Mask|Button3Mask)) == 0) \
				: \
		 (((ignore) == Button2) ? \
		  		(((detail)&(Button1Mask|Button3Mask)) == 0) \
				: \
		  		(((detail)&(Button1Mask|Button2Mask)) == 0)) \
		)


#define Max(i, j)       ((i) > (j) ? (i) : (j))
#define Min(i, j)       ((i) < (j) ? (i) : (j))

#define MAX_COLS	200
#define MAX_ROWS	128

/*
** System V definitions
*/

#ifdef SYSV

#ifdef JOBCONTROL
#ifndef	SX9100
#define	getpgrp		getpgrp2
#endif	SX9100
#else	/* !JOBCONTROL */
#define	getpgrp(x)	(x)
#endif	/* !JOBCONTROL */

#define	killpg(x,sig)	kill(-x,sig)

#define	dup2(fd1,fd2)	((fd1 == fd2) ? fd1 : \
				(close(fd2), fcntl(fd1, F_DUPFD, fd2)))

#endif	/* !SYSV */

/*
** allow for mobility of the pty master/slave directories
*/
#ifndef PTYDEV
#ifdef hpux
#define	PTYDEV		"/dev/ptym/ptyxx"
#else	/* !hpux */
#define	PTYDEV		"/dev/ptyxx"
#endif	/* !hpux */
#endif	/* !PTYDEV */

#ifndef TTYDEV
#ifdef hpux
#define TTYDEV		"/dev/pty/ttyxx"
#else	/* !hpux */
#define	TTYDEV		"/dev/ttyxx"
#endif	/* !hpux */
#endif	/* !TTYDEV */

#ifndef PTYCHAR1
#ifdef hpux
#define PTYCHAR1	"zyxwvutsrqp"
#else	/* !hpux */
#define	PTYCHAR1	"pqrstuvwxyz"
#endif	/* !hpux */
#endif	/* !PTYCHAR1 */

#ifndef PTYCHAR2
#ifdef hpux
#define	PTYCHAR2	"fedcba9876543210"
#else	/* !hpux */
#define	PTYCHAR2	"0123456789abcdef"
#endif	/* !hpux */
#endif	/* !PTYCHAR2 */

#ifdef KANJI
#ifndef WCHAR_T
#define WCHAR_T
typedef unsigned short	wchar_t;
#endif
#endif

/* Until the translation manager comes along, I have to do my own translation of
 * mouse events into the proper routines. */

typedef enum {NORMAL, LEFTEXTENSION, RIGHTEXTENSION} EventMode;

/*
 * The origin of a screen is 0, 0.  Therefore, the number of rows
 * on a screen is screen->max_row + 1, and similarly for columns.
 */

typedef char **ScrnBuf;

/*
 * ANSI emulation.
 */
#define INQ	0x05
#define	FF	0x0C			/* C0, C1 control names		*/
#define	LS1	0x0E
#define	LS0	0x0F
#define	CAN	0x18
#define	SUB	0x1A
#define	ESC	0x1B
#define US	0x1F
#define	DEL	0x7F
#define HTS     ('H'+0x40)
#define	SS2	0x8E
#define	SS3	0x8F
#define	DCS	0x90
#define	OLDID	0x9A			/* ESC Z			*/
#define	CSI	0x9B
#define	ST	0x9C
#define	OSC	0x9D
#define	PM	0x9E
#define	APC	0x9F
#define	RDEL	0xFF

#define	NBOX	5			/* Number of Points in box	*/
#define	NPARAM	10			/* Max. parameters		*/

#define	MINHILITE	32

typedef struct {
	unsigned char	a_type;
	unsigned char	a_pintro;
	unsigned char	a_final;
	unsigned char	a_inters;
	char	a_nparam;		/* # of parameters		*/
	char	a_dflt[NPARAM];		/* Default value flags		*/
	short	a_param[NPARAM];	/* Parameters			*/
	char	a_nastyf;		/* Error flag			*/
} ANSI;

typedef struct {
	int		row;
	int		col;
	unsigned	flags;	/* Vt100 saves graphics rendition. Ugh! */
	char		curgl;
	char		curgr;
#ifdef	KANJI	/* sano */
	short		gsets[4];
#else	KANJI
	char		gsets[4];
#endif	KANJI
} SavedCursor;

#define	TEKNUMFONTS	4
/* Actually there are 5 types of lines, but four are non-solid lines */
#define	TEKNUMLINES	4

typedef struct {
	int	x;
	int	y;
	int	fontsize;
	int	linetype;
} Tmodes;

typedef struct {
	int Twidth;
	int Theight;
} T_fontsize;

typedef struct {
	short *bits;
	int x;
	int y;
	int width;
	int height;
} BitmapBits;

#define	SAVELINES		64      /* default # lines to save      */

typedef struct {
/* These parameters apply to both windows */
	Display		*display;	/* X display for screen		*/
	int		respond;	/* socket for responses
					   (position report, etc.)	*/
	long		pid;		/* pid of process on far side   */
	int		uid;		/* user id of actual person	*/
	int		gid;		/* group id of actual person	*/
	GC		normalGC;	/* normal painting		*/
	GC		reverseGC;	/* reverse painting		*/
	GC		normalboldGC;	/* normal painting, bold font	*/
	GC		reverseboldGC;	/* reverse painting, bold font	*/
#ifdef	KANJI	/* sano */
	GC		r_normalGC;	/* normal GR painting		*/
	GC		r_reverseGC;	/* reverse GR painting		*/
	GC		r_normalboldGC;	/* normal GR painting, bold font*/
	GC		r_reverseboldGC;/* reverse GR painting, bold font*/
	GC		w_normalGC;	/* normal kanji painting	*/
	GC		w_reverseGC;	/* reverse kanji painting	*/
	GC		w_normalboldGC;	/* normal kanji, bold font	*/
	GC		w_reverseboldGC;/* reverse kanji, bold font	*/
#endif	KANJI
	GC		cursorGC;	/* normal cursor painting	*/
	GC		reversecursorGC;/* reverse cursor painting	*/
#ifdef	KANJI	/* michael */
	GC		r_cursorGC;	/* normal kana cursor painting	*/
	GC		r_reversecursorGC;/* reverse kana cursor 	*/
	GC		w_cursorGC;	/* normal kanji cursor painting	*/
	GC		w_reversecursorGC;/* reverse kanji cursor 	*/
#endif	KANJI
	GC		cursoroutlineGC;/* for painting lines around    */
	Pixel		foreground;	/* foreground color		*/
	Pixel		cursorcolor;	/* Cursor color			*/
	Pixel		mousecolor;	/* Mouse color			*/
	int		border;		/* inner border			*/
	Pixmap		graybordertile;	/* tile pixmap for border when
						window is unselected	*/
	Cursor		arrow;		/* arrow cursor			*/
	unsigned short	send_mouse_pos;	/* user wants mouse transition  */
					/* and position information	*/
	int		select;		/* xterm selected		*/
	Boolean		visualbell;	/* visual bell mode		*/
	int		logging;	/* logging mode			*/
	Boolean		allowSendEvents;/* SendEvent mode		*/
	Boolean		grabbedKbd;	/* keyboard is grabbed		*/
	int		logfd;		/* file descriptor of log	*/
	char		*logfile;	/* log file name		*/
	char		*logstart;	/* current start of log buffer	*/
	int		inhibit;	/* flags for inhibiting changes	*/

/* VT window parameters */
	struct {
		Window	window;		/* X window id			*/
		int	width;		/* width of columns		*/
		int	height;		/* height of rows		*/
		int	fullwidth;	/* full width of window		*/
		int	fullheight;	/* full height of window	*/
		int	f_width;	/* width of fonts in pixels	*/
		int	f_height;	/* height of fonts in pixels	*/
	} fullVwin;
	Cursor pointer_cursor;		/* pointer cursor in window	*/

	/* Terminal fonts must be of the same size and of fixed width */
	XFontStruct	*fnt_norm;	/* normal font of terminal	*/
	XFontStruct	*fnt_bold;	/* bold font of terminal	*/
#ifdef	KANJI	/* sano */
	XFontStruct	*fnt_r_norm;	/* normal GR font of terminal	*/
	XFontStruct	*fnt_r_bold;	/* bold GR font of terminal	*/
	XFontStruct	*fnt_w_norm;	/* normal kanji font of terminal*/
	XFontStruct	*fnt_w_bold;	/* bold kanji font of terminal	*/
#endif	KANJI
	int		enbolden;	/* overstrike for bold font	*/
#ifdef	KANJI	/* sano */
	int		r_enbolden;	/* overstrike for GR bold	*/
	int		w_enbolden;	/* overstrike for kanji bold	*/
	XPoint		*w_box;		/* draw unselected cursor	*/
#endif	KANJI
	XPoint		*box;		/* draw unselected cursor	*/

	int		cursor_state;	/* ON or OFF			*/
	int		cursor_set;	/* requested state		*/
	int		cursor_col;	/* previous cursor column	*/
	int		cursor_row;	/* previous cursor row		*/
	int		cur_col;	/* current cursor column	*/
	int		cur_row;	/* current cursor row		*/
	int		max_col;	/* rightmost column		*/
	int		max_row;	/* bottom row			*/
	int		top_marg;	/* top line of scrolling region */
	int		bot_marg;	/* bottom line of  "	    "	*/
	Widget		scrollWidget;	/* pointer to scrollbar struct	*/
	int		scrollbar;	/* if > 0, width of scrollbar, and
						scrollbar is showing	*/
	int		topline;	/* line number of top, <= 0	*/
	int		savedlines;     /* number of lines that've been saved */
	int		savelines;	/* number of lines off top to save */
	Boolean		scrollinput;	/* scroll to bottom on input	*/
	Boolean		scrollkey;	/* scroll to bottom on key	*/
	
	ScrnBuf		buf;		/* screen buffer (main)		*/
	ScrnBuf		allbuf;		/* screen buffer (may include
					   lines scrolled off top	*/
	ScrnBuf		altbuf;		/* alternate screen buffer	*/
	Boolean		alternate;	/* true if using alternate buf	*/
	unsigned short	do_wrap;	/* true if cursor in last column
					   and character just output    */
	int		incopy;		/* 0 if no RasterCopy exposure
					   event processed since last
					   RasterCopy			*/
	Boolean		c132;		/* allow change to 132 columns	*/
	Boolean		curses;		/* cludge-ups for more and vi	*/
	Boolean		marginbell;	/* true if margin bell on	*/
	int		nmarginbell;	/* columns from right margin	*/
	int		bellarmed;	/* cursor below bell margin	*/
	Boolean 	multiscroll;	/* true if multi-scroll		*/
	int		scrolls;	/* outstanding scroll count	*/
	SavedCursor	sc;		/* data for restore cursor	*/
	int		save_modes[19];	/* save dec private modes	*/
#ifdef	STATUSLINE	/* michael */
	Boolean		statusline;	/* status line showing		*/
	int		statusheight;	/* status line height		*/
	Boolean		instatus;	/* cursor in status line	*/
	SavedCursor	statussc;	/* status line restore cursor	*/
	Boolean		reversestatus;	/* status line reversed		*/
#endif	STATUSLINE

	/* Improved VT100 emulation stuff.				*/
#ifdef	KANJI	/* sano */
	short		gsets[4];	/* G0 through G3.		*/
#else	KANJI
	char		gsets[4];	/* G0 through G3.		*/
#endif	KANJI
	char		curgl;		/* Current GL setting.		*/
	char		curgr;		/* Current GR setting.		*/
	char		curss;		/* Current single shift.	*/
	int		scroll_amt;	/* amount to scroll		*/
	int		refresh_amt;	/* amount to refresh		*/
	Boolean		jumpscroll;	/* whether we should jumpscroll */
	Boolean         always_highlight; /* whether to highlight cursor */
#ifdef	XJP
	Boolean		kinit;
	Boolean		kconvert;
	int		kmode;
	int		kcw_posx;
	int		kcw_posy;
	int		kcw_width;
	int		kcw_height;
	int		kcw_full_height;
	XJp_id		jpreq_id;
#endif	/* XJP */

/* Tektronix window parameters */
	GC		TnormalGC;	/* normal painting		*/
	GC		TcursorGC;	/* normal cursor painting	*/
	Pixel		Tforeground;	/* foreground color		*/
	Pixel		Tbackground;	/* Background color		*/
	Pixel		Tcursorcolor;	/* Cursor color			*/
	Pixmap		Tbgndtile;	/* background tile pixmap	*/
	int		Tcolor;		/* colors used			*/
	Boolean		planeused;	/* is xorplane being used	*/
	Boolean		cellsused;	/* are color cells being used	*/
	XColor		colorcells[3];	/* color cells for Tek		*/
	Boolean		Vshow;		/* VT window showing		*/
	Boolean		Tshow;		/* Tek window showing		*/
	Boolean		waitrefresh;	/* postpone refresh		*/
	struct {
		Window	window;		/* X window id			*/
		int	width;		/* width of columns		*/
		int	height;		/* height of rows		*/
		int	fullwidth;	/* full width of window		*/
		int	fullheight;	/* full height of window	*/
		double	tekscale;	/* scale factor Tek -> vs100	*/
	} fullTwin;
	XPoint		**Tbox;		/* draw unselected cursor	*/
	int		xorplane;	/* z plane for inverts		*/
	GC		linepat[TEKNUMLINES]; /* line patterns		*/
	XFontStruct 	*Tfont[TEKNUMFONTS]; /* Tek fonts		*/
	int		tobaseline[TEKNUMFONTS]; /* top to baseline for
							each font	*/
	Boolean		TekEmu;		/* true if Tektronix emulation	*/
	int		cur_X;		/* current x			*/
	int		cur_Y;		/* current y			*/
	Tmodes		cur;		/* current tek modes		*/
	Tmodes		page;		/* starting tek modes on page	*/
	int		margin;		/* 0 -> margin 1, 1 -> margin 2	*/
	int		pen;		/* current Tektronix pen 0=up, 1=dn */
	char		*TekGIN;	/* nonzero if Tektronix GIN mode*/
	int		multiClickTime;	 /* time between multiclick selects */
	char		*charClass;	/* for overriding word selection */
	Boolean		cutNewline;	/* whether or not line cut has \n */
	Boolean		cutToBeginningOfLine;  /* line cuts to BOL? */
#ifdef KANJI
	wchar_t		*selection;	/* the current selection */
#else
	char		*selection;	/* the current selection */
#endif
	int		selection_size; /* size of allocated buffer */
	int		selection_length; /* number of significant bytes */
	int		selection_time;	/* latest event timestamp */
	int		startHRow, startHCol, /* highlighted text */
			endHRow, endHCol,
			startHCoord, endHCoord;
	Atom*		selection_atoms; /* which selections we own */
	Cardinal	sel_atoms_size;	/*  how many atoms allocated */
	Cardinal	selection_count; /* how many atoms in use */
} TScreen;

/* meaning of bits in screen.select flag */
#define	INWINDOW	01	/* the mouse is in one of the windows */
#define	FOCUS		02	/* one of the windows is the focus window */

#define MULTICLICKTIME 250	/* milliseconds */

typedef struct
{
	unsigned	flags;
} TKeyboard;

typedef struct _Misc {
    char *geo_metry;
    char *T_geometry;
    char *f_n;
    char *f_b;
#ifdef	KANJI	/* sano */
    char *f_rn;
    char *f_rb;
    char *f_kn;
    char *f_kb;
    char *k_m;
#ifdef	XJP
    Boolean jfew_open;
    char * j_m;
#endif	/* XJP */
#endif
    Boolean log_on;
    Boolean login_shell;
    Boolean re_verse;
    Boolean reverseWrap;
    Boolean logInhibit;
    Boolean signalInhibit;
    Boolean tekInhibit;
    Boolean scrollbar;
    Boolean titeInhibit;
#ifdef	STATUSLINE	/* michael */
    Boolean statusnormal;
#endif	STATUSLINE
} Misc;

typedef struct {int foo;} XtermClassPart, TekClassPart;

typedef struct _XtermClassRec {
    CoreClassPart  core_class;
    XtermClassPart xterm_class;
} XtermClassRec;

typedef struct _TekClassRec {
    CoreClassPart core_class;
    TekClassPart tek_class;
} TekClassRec;

/* define masks for flags */
#define CAPS_LOCK	0x01
#define KYPD_APL	0x02
#define CURSOR_APL	0x04


#define N_MARGINBELL	10
#define MAX_TABS	320
#define TAB_ARRAY_SIZE	10	/* number of ints to provide MAX_TABS bits */

typedef unsigned Tabs [TAB_ARRAY_SIZE];

typedef struct _XtermWidgetRec {
    CorePart	core;
    TKeyboard	keyboard;	/* terminal keyboard		*/
    TScreen	screen;		/* terminal screen		*/
    unsigned	flags;		/* mode flags			*/
    unsigned	initflags;	/* initial mode flags		*/
    Tabs	tabs;		/* tabstops of the terminal	*/
    Misc	misc;		/* miscelaneous parameters	*/
} XtermWidgetRec, *XtermWidget;

typedef struct _TekWidgetRec {
    CorePart core;
} TekWidgetRec, *TekWidget;

#define BUF_SIZE 4096

/* masks for terminal flags */

#define INVERSE		0x01	/* invert the characters to be output */
#define UNDERLINE	0x02	/* true if underlining */
#define BOLD		0x04
#ifdef	KANJI	/* sano */
#define	KCS		0x18	/* true if on KANJI char */
#define	KCS1		0x08	/* true if on the first byte of a Kanji char */
#define	KCS2		0x10	/* true if on the second byte of a Kanji char */
#define	KANACS		0x20	/* true if on KANA char */
#define WRAPAROUND	0x2000
#define REVERSE_VIDEO	0x4000	/* true if screen white on black */
#define ORIGIN		0x8000	/* true if in origin mode */
#define INSERT		0x10000	/* true if in insert mode */
#define SMOOTHSCROLL	0x20000	/* true if in smooth scroll mode */
#define AUTOREPEAT	0x40000	/* true if in autorepeat mode */
#define IN132COLUMNS	0x80000	/* true if in 132 column mode */
#define LINEFEED	0x100000
#define	REVERSEWRAP	0x200000/* true if reverse wraparound mode */
#define	EUC_KANJI	0x400000/* true if in EUC mode */
#define	SJIS_KANJI	0x800000/* true if in SJIS mode */
#if defined(SX9100) || defined(EWS) || defined(FACOM) || defined(UX386)
#define	CINV		0x1000000
#endif
#else	KANJI
#define WRAPAROUND	0x08
#define REVERSE_VIDEO	0x10	/* true if screen white on black */
#define ORIGIN		0x20	/* true if in origin mode */
#define INSERT		0x40	/* true if in insert mode */
#define SMOOTHSCROLL	0x80	/* true if in smooth scroll mode */
#ifdef DO_AUTOREPEAT
#define AUTOREPEAT	0x100	/* true if in autorepeat mode */
#endif /* DO_AUTOREPEAT */
#define IN132COLUMNS	0x200	/* true if in 132 column mode */
#define LINEFEED	0x400
#define	REVERSEWRAP	0x800	/* true if reverse wraparound mode */
#endif	KANJI

#ifdef	KANJI	/* sano */
#define	ATTRIBUTES	0x3f	/* attributes mask including KCS/KANACS */
#else	KANJI
#define	ATTRIBUTES	0x07	/* attributes mask */
#endif	KANJI
#define CHAR		0177

#ifdef	KANJI	/* sano */
#define	MBCS		0x80	/* MBCS stands for Multi Byte Character Set */
#endif	KANJI

#define VWindow(screen)		(screen->fullVwin.window)
#define VShellWindow		term->core.parent->core.window
#define TextWindow(screen)      (screen->fullVwin.window)
#define TWindow(screen)		(screen->fullTwin.window)
#define TShellWindow		tekWidget->core.parent->core.window
#define Width(screen)		(screen->fullVwin.width)
#define Height(screen)		(screen->fullVwin.height)
#define FullWidth(screen)	(screen->fullVwin.fullwidth)
#define FullHeight(screen)	(screen->fullVwin.fullheight)
#define FontWidth(screen)	(screen->fullVwin.f_width)
#define FontHeight(screen)	(screen->fullVwin.f_height)
#define TWidth(screen)		(screen->fullTwin.width)
#define THeight(screen)		(screen->fullTwin.height)
#define TFullWidth(screen)	(screen->fullTwin.fullwidth)
#define TFullHeight(screen)	(screen->fullTwin.fullheight)
#define TekScale(screen)	(screen->fullTwin.tekscale)

#define CursorX(screen,col) ((col) * FontWidth(screen) + screen->border \
			+ screen->scrollbar)
#ifdef	STATUSLINE	/* michael */
#define	CursorY(screen,row) ((screen->instatus?\
			((row) * FontHeight(screen) + 1)\
			: (((row) - screen->topline) * FontHeight(screen)))\
			+ screen->border)
#else	STATUSLINE
#define CursorY(screen,row) ((((row) - screen->topline) * FontHeight(screen)) \
			+ screen->border)
#endif	STATUSLINE

#define	TWINDOWEVENTS	(KeyPressMask | ExposureMask | ButtonPressMask |\
			 ButtonReleaseMask | StructureNotifyMask |\
			 EnterWindowMask | LeaveWindowMask | FocusChangeMask)

#define	WINDOWEVENTS	(TWINDOWEVENTS | PointerMotionMask)


#define TEK_LINK_BLOCK_SIZE 1024

typedef struct Tek_Link
{
	struct Tek_Link	*next;	/* pointer to next TekLink in list
				   NULL <=> this is last TekLink */
	short count;
	char *ptr;
	char data [TEK_LINK_BLOCK_SIZE];
} TekLink;

/* flags for cursors */
#define	OFF		0
#define	ON		1
#define	CLEAR		0
#define	TOGGLE		1

/* flags for inhibit */
#define	I_LOG		0x01
#define	I_SIGNAL	0x02
#define	I_TEK		0x04

extern Cursor make_tcross();
extern Cursor make_xterm();
extern Cursor make_wait();
extern Cursor make_arrow();

#ifdef	XJP
#define	UNDER_MODE	0
#define	INLINE_MODE	1
#define DEFAULT_MODE	2
#endif	/* XJP */
