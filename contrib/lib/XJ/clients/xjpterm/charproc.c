/*
 * $XConsortium: charproc.c,v 1.64 89/01/04 13:37:50 jim Exp $
 * $Header: charproc.c,v 1.2 89/07/08 16:10:16 kagotani Rel $
 */


#include <X11/copyright.h>

/*
 *	Copyright 1989 OMRON TATEISI ELECTRONICS CO.
 *
 *	Original Source: ./client/kterm/charproc.c
 *
 *	Modified by OMRON TATEISI ELECTRONICS CO., to handle XJp-library.
 *
 *	Author:	Seiji Kuwari	OMRON TATEISI ELECTRONICS CO.
 *				kuwa@frf.omron.junet
 *				uunet!nff.ncl.omron.co.jp!kuwa
 */				

/*
 * Copyright 1988 Massachusetts Institute of Technology
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

/* charproc.c */

#ifdef FACOM
#include <sys/types.h>
#endif

#include <stdio.h>
#ifdef mips			/* !defined(mips) || !defined(SYSTYPE_SYSV) */
# ifndef SYSTYPE_SYSV
# include <sgtty.h>
# endif /* not SYSTYPE_SYSV */
#else
#if !defined(EWS) && !defined(UX386)
# include <sgtty.h>
#endif
#endif /* mips */
#include <ctype.h>
#include <errno.h>
#include <setjmp.h>
#ifdef macII
#undef FIOCLEX					/* redefined from sgtty.h */
#undef FIONCLEX					/* redefined from sgtty.h */
#endif
#include <sys/ioctl.h>

#ifdef UX386
#include <stropts.h>
#include <sys/termio.h>
#ifdef FIONREAD
#undef FIONREAD
#define  FIONREAD I_NREAD
#endif
#endif  /* !UX386 */

#ifdef	SX9100
#ifdef	SYSV
#include <sys/pty.h>
#define	PKT_MODE
#else	/* SYSV */
#include <sys/jtermio.h>
#endif	/* SYSV */
#endif	/* SX9100 */
#include "ptyx.h"
#include "VTparse.h"
#include "data.h"
#include <X11/Xutil.h>
#include "error.h"
#include "main.h"
#include <X11/StringDefs.h>
#ifdef MODEMENU
#include "menu.h"
#endif	/* MODEMENU */
#ifdef	XJP
extern XJp_attributes jp_att;
#endif	/* XJP */

extern void exit(), Bcopy();
static void VTallocbuf();

#define	DEFAULT		-1
#define	TEXT_BUF_SIZE	256
#define TRACKTIMESEC	4L
#define TRACKTIMEUSEC	0L

#define XtNalwaysHighlight	"alwaysHighlight"
#define	XtNboldFont		"boldFont"
#define	XtNc132			"c132"
#define XtNcharClass		"charClass"
#define	XtNcurses		"curses"
#define	XtNcursorColor		"cursorColor"
#define XtNcutNewline		"cutNewline"
#define XtNcutToBeginningOfLine	"cutToBeginningOfLine"
#define XtNgeometry		"geometry"
#define XtNtekGeometry		"tekGeometry"
#define	XtNinternalBorder	"internalBorder"
#define	XtNjumpScroll		"jumpScroll"
#define	XtNlogFile		"logFile"
#define	XtNlogging		"logging"
#define	XtNlogInhibit		"logInhibit"
#define	XtNloginShell		"loginShell"
#define	XtNmarginBell		"marginBell"
#define	XtNpointerColor		"pointerColor"
#define	XtNpointerShape		"pointerShape"
#define XtNmultiClickTime	"multiClickTime"
#define	XtNmultiScroll		"multiScroll"
#define	XtNnMarginBell		"nMarginBell"
#define	XtNreverseWrap		"reverseWrap"
#define	XtNsaveLines		"saveLines"
#define	XtNscrollBar		"scrollBar"
#define	XtNscrollInput		"scrollInput"
#define	XtNscrollKey		"scrollKey"
#define XtNscrollPos    	"scrollPos"
#define	XtNsignalInhibit	"signalInhibit"
#ifdef	STATUSLINE	/* michael */
#define	XtNstatusLine		"statusLine"
#define	XtNstatusNormal		"statusNormal"
#endif	STATUSLINE
#define	XtNtekInhibit		"tekInhibit"
#define	XtNtekStartup		"tekStartup"
#define XtNtiteInhibit		"titeInhibit"
#define	XtNvisualBell		"visualBell"
#define XtNallowSendEvents	"allowSendEvents"
#ifdef	KANJI	/* sano */
#define	XtNkanaFont		"kanaFont"
#if defined(SX9100) || defined(EWS) || defined(FACOM) || defined(UX386) || defined(SUN)
#define	XtNkanaBoldFont		"kanaboldFont"
#else
#define	XtNkanaBoldFont		"kanaBoldFont"
#endif
#define	XtNkanjiFont		"kanjiFont"
#if defined(SX9100) || defined(EWS) || defined(FACOM) || defined(UX386) || defined(SUN)
#define	XtNkanjiBoldFont	"kanjiboldFont"
#else
#define	XtNkanjiBoldFont	"kanjiBoldFont"
#endif
#define	XtNkanjiMode		"kanjiMode"
#ifdef	XJP
#define	XtNjfewOpen		"jfewOpen"
#define	XtNjfewMode		"jfewMode"
#endif	/* XJP */
#endif	KANJI

#define XtCAlwaysHighlight	"AlwaysHighlight"
#define	XtCC132			"C132"
#define XtCCharClass		"CharClass"
#define	XtCCurses		"Curses"
#define XtCCutNewline		"CutNewline"
#define XtCCutToBeginningOfLine	"CutToBeginningOfLine"
#define XtCGeometry		"Geometry"
#define	XtCJumpScroll		"JumpScroll"
#define	XtCLogfile		"Logfile"
#define	XtCLogging		"Logging"
#define	XtCLogInhibit		"LogInhibit"
#define	XtCLoginShell		"LoginShell"
#define	XtCMarginBell		"MarginBell"
#define XtCMultiClickTime	"MultiClickTime"
#define	XtCMultiScroll		"MultiScroll"
#define	XtCColumn		"Column"
#define	XtCReverseVideo		"ReverseVideo"
#define	XtCReverseWrap		"ReverseWrap"
#define XtCSaveLines		"SaveLines"
#define	XtCScrollBar		"ScrollBar"
#define XtCScrollPos     	"ScrollPos"
#define	XtCScrollCond		"ScrollCond"
#define	XtCSignalInhibit	"SignalInhibit"
#ifdef	STATUSLINE	/* michael */
#define	XtCStatusLine		"StatusLine"
#define	XtCStatusNormal		"StatusNormal"
#endif	STATUSLINE
#define	XtCTekInhibit		"TekInhibit"
#define	XtCTekStartup		"TekStartup"
#define XtCTiteInhibit		"TiteInhibit"
#define	XtCVisualBell		"VisualBell"
#define XtCAllowSendEvents	"AllowSendEvents"
#ifdef	KANJI	/* sano */
#define	XtCKanaFont		"KanaFont"
#define	XtCKanaBoldFont		"KanaBoldFont"
#define	XtCKanjiFont		"KanjiFont"
#define	XtCKanjiBoldFont	"KanjiBoldFont"
#define	XtCKanjiMode		"KanjiMode"
#ifdef	XJP
#define	XtCJfewOpen		"JfewOpen"
#define	XtCJfewMode		"JfewMode"
#endif	/* XJP */
#endif	KANJI

#ifdef	KANJI	/* sano */
	/*
	 * To avoid the sign-extension problem, masks lower 8 bites.
	 */
#define	doinput()		(0xff & (bcnt-- > 0 ? *bptr++ : in_put()))
#else	KANJI
#define	doinput()		(bcnt-- > 0 ? *bptr++ : in_put())
#endif	KANJI

#ifndef lint
static char rcs_id[] = "$XConsortium: charproc.c,v 1.64 89/01/04 13:37:50 jim Exp $";
static char kterm_id[] = "$Header: charproc.c,v 1.2 89/07/08 16:10:16 kagotani Rel $";
#endif	/* lint */

static long arg;
static int nparam;
static ANSI reply;
static int param[NPARAM];

static unsigned long ctotal;
static unsigned long ntotal;
static jmp_buf vtjmpbuf;

extern int groundtable[];
extern int csitable[];
extern int dectable[];
extern int eigtable[];
extern int esctable[];
extern int iestable[];
extern int igntable[];
extern int scrtable[];
extern int scstable[];
#ifdef	KANJI	/* sano */
extern int mbcstable[];
static char pending_char;
static int pending_bad;
#endif	KANJI


/* event handlers */
extern void HandleKeyPressed();
extern void HandleStringEvent();
extern void HandleEnterWindow();
extern void HandleLeaveWindow();
extern void HandleFocusChange();
       void HandleKeymapChange();
extern void HandleModeMenu();
extern void HandleInsertSelection();
extern void HandleSelectStart();
extern void HandleSelectExtend();
extern void HandleSelectEnd();
extern void HandleStartExtend();
       void HandleBell();
       void HandleIgnore();
extern void HandleSecure();

/*
 * NOTE: VTInitialize zeros out the entire ".screen" component of the 
 * XtermWidget, so make sure to add an assignment statement in VTInitialize() 
 * for each new ".screen" field added to this resource list.
 */

/* Defaults */
static  Boolean	defaultFALSE	   = FALSE;
static  Boolean	defaultTRUE	   = TRUE;
static  int	defaultBorderWidth = DEFBORDERWIDTH;
static  int	defaultIntBorder   = DEFBORDER;
static  int	defaultSaveLines   = SAVELINES;
static  int	defaultNMarginBell = N_MARGINBELL;
static  int	defaultMultiClickTime = MULTICLICKTIME;

static char defaultTranslations[] =
"\
	    <KeyPress>:		insert()	\n\
 Ctrl ~Meta <Btn1Down>:		mode-menu()	\n\
      ~Meta <Btn1Down>:		select-start()	\n\
      ~Meta <Btn1Motion>:	select-extend() \n\
 Ctrl ~Meta <Btn2Down>:		mode-menu()	\n\
~Ctrl ~Meta <Btn2Down>:		ignore()	\n\
      ~Meta <Btn2Up>:		insert-selection(PRIMARY, CUT_BUFFER0) \n\
~Ctrl ~Meta <Btn3Down>:		start-extend()	\n\
      ~Meta <Btn3Motion>:	select-extend()	\n\
      ~Meta <BtnUp>:		select-end(PRIMARY, CUT_BUFFER0) \n\
	    <BtnDown>:		bell(0)		\
";

static XtActionsRec actionsList[] = { 
    { "bell",		  HandleBell },
    { "ignore",		  HandleIgnore },
    { "insert",		  HandleKeyPressed },
    { "insert-selection", HandleInsertSelection },
    { "keymap", 	  HandleKeymapChange },
    { "mode-menu",	  HandleModeMenu },
    { "secure",		  HandleSecure },
    { "select-start",	  HandleSelectStart },
    { "select-extend",	  HandleSelectExtend },
    { "select-end",	  HandleSelectEnd },
    { "start-extend",	  HandleStartExtend },
    { "string",		  HandleStringEvent },
};

static XtResource resources[] = {
{XtNfont, XtCFont, XtRString, sizeof(char *),
	XtOffset(XtermWidget, misc.f_n), XtRString,
	DEFFONT},
{XtNboldFont, XtCFont, XtRString, sizeof(char *),
	XtOffset(XtermWidget, misc.f_b), XtRString,
	DEFBOLDFONT},
{XtNc132, XtCC132, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, screen.c132),
	XtRBoolean, (caddr_t) &defaultFALSE},
{XtNcharClass, XtCCharClass, XtRString, sizeof(char *),
	XtOffset(XtermWidget, screen.charClass),
	XtRString, (caddr_t) NULL},
{XtNcurses, XtCCurses, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, screen.curses),
	XtRBoolean, (caddr_t) &defaultFALSE},
{XtNcutNewline, XtCCutNewline, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, screen.cutNewline),
	XtRBoolean, (caddr_t) &defaultTRUE},
{XtNcutToBeginningOfLine, XtCCutToBeginningOfLine, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, screen.cutToBeginningOfLine),
	XtRBoolean, (caddr_t) &defaultTRUE},
{XtNbackground, XtCBackground, XtRPixel, sizeof(Pixel),
	XtOffset(XtermWidget, core.background_pixel),
	XtRString, "White"},
{XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	XtOffset(XtermWidget, screen.foreground),
	XtRString, "Black"},
{XtNcursorColor, XtCForeground, XtRPixel, sizeof(Pixel),
	XtOffset(XtermWidget, screen.cursorcolor),
	XtRString, "Black"},
{XtNgeometry,XtCGeometry, XtRString, sizeof(char *),
	XtOffset(XtermWidget, misc.geo_metry),
	XtRString, (caddr_t) NULL},
{XtNalwaysHighlight,XtCAlwaysHighlight,XtRBoolean,
        sizeof(Boolean),XtOffset(XtermWidget, screen.always_highlight),
        XtRBoolean, (caddr_t) &defaultFALSE},
{XtNtekGeometry,XtCGeometry, XtRString, sizeof(char *),
	XtOffset(XtermWidget, misc.T_geometry),
	XtRString, (caddr_t) NULL},
{XtNinternalBorder,XtCBorderWidth,XtRInt, sizeof(int),
	XtOffset(XtermWidget, screen.border),
	XtRInt, (caddr_t) &defaultIntBorder},
{XtNjumpScroll, XtCJumpScroll, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, screen.jumpscroll),
	XtRBoolean, (caddr_t) &defaultTRUE},
{XtNlogFile, XtCLogfile, XtRString, sizeof(char *),
	XtOffset(XtermWidget, screen.logfile),
	XtRString, (caddr_t) NULL},
{XtNlogging, XtCLogging, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, misc.log_on),
	XtRBoolean, (caddr_t) &defaultFALSE},
{XtNlogInhibit, XtCLogInhibit, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, misc.logInhibit),
	XtRBoolean, (caddr_t) &defaultFALSE},
{XtNloginShell, XtCLoginShell, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, misc.login_shell),
	XtRBoolean, (caddr_t) &defaultFALSE},
{XtNmarginBell, XtCMarginBell, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, screen.marginbell),
	XtRBoolean, (caddr_t) &defaultFALSE},
{XtNpointerColor, XtCForeground, XtRPixel, sizeof(Pixel),
	XtOffset(XtermWidget, screen.mousecolor),
	XtRString, "Black"},
{XtNpointerShape,XtCCursor, XtRCursor, sizeof(Cursor),
	XtOffset(XtermWidget, screen.pointer_cursor),
	XtRString, (caddr_t) "xterm"},
{XtNmultiClickTime,XtCMultiClickTime, XtRInt, sizeof(int),
	XtOffset(XtermWidget, screen.multiClickTime),
	XtRInt, (caddr_t) &defaultMultiClickTime},
{XtNmultiScroll,XtCMultiScroll, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, screen.multiscroll),
	XtRBoolean, (caddr_t) &defaultFALSE},
{XtNnMarginBell,XtCColumn, XtRInt, sizeof(int),
	XtOffset(XtermWidget, screen.nmarginbell),
	XtRInt, (caddr_t) &defaultNMarginBell},
{XtNreverseVideo,XtCReverseVideo,XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, misc.re_verse),
	XtRBoolean, (caddr_t) &defaultFALSE},
{XtNreverseWrap,XtCReverseWrap, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, misc.reverseWrap),
	XtRBoolean, (caddr_t) &defaultFALSE},
{XtNsaveLines, XtCSaveLines, XtRInt, sizeof(int),
	XtOffset(XtermWidget, screen.savelines),
	XtRInt, (caddr_t) &defaultSaveLines},
{XtNscrollBar, XtCScrollBar, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, misc.scrollbar),
	XtRBoolean, (caddr_t) &defaultFALSE},
{XtNscrollInput,XtCScrollCond, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, screen.scrollinput),
	XtRBoolean, (caddr_t) &defaultTRUE},
{XtNscrollKey, XtCScrollCond, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, screen.scrollkey),
	XtRBoolean, (caddr_t) &defaultFALSE},
{XtNsignalInhibit,XtCSignalInhibit,XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, misc.signalInhibit),
	XtRBoolean, (caddr_t) &defaultFALSE},
#ifdef	STATUSLINE	/* michael */
{XtNstatusLine, XtCStatusLine, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, screen.statusline),
	XtRBoolean, (caddr_t) &defaultFALSE},
{XtNstatusNormal, XtCStatusNormal, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, misc.statusnormal),
	XtRBoolean, (caddr_t) &defaultFALSE},
#endif	STATUSLINE
{XtNtekInhibit, XtCTekInhibit, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, misc.tekInhibit),
	XtRBoolean, (caddr_t) &defaultFALSE},
{XtNtekStartup, XtCTekStartup, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, screen.TekEmu),
	XtRBoolean, (caddr_t) &defaultFALSE},
{XtNtiteInhibit, XtCTiteInhibit, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, misc.titeInhibit),
	XtRBoolean, (caddr_t) &defaultFALSE},
{XtNvisualBell, XtCVisualBell, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, screen.visualbell),
	XtRBoolean, (caddr_t) &defaultFALSE},
{XtNallowSendEvents, XtCAllowSendEvents, XtRBoolean, sizeof(Boolean),
	XtOffset(XtermWidget, screen.allowSendEvents),
	XtRBoolean, (caddr_t) &defaultFALSE}
#ifdef	KANJI	/* sano */
,
{XtNkanaFont, XtCKanaFont, XtRString, sizeof(char *),
	XtOffset(XtermWidget, misc.f_rn),
	XtRString, DEFKANAFONT},
{XtNkanaBoldFont, XtCKanaFont, XtRString, sizeof(char *),
	XtOffset(XtermWidget, misc.f_rb),
	XtRString, DEFKANABOLDFONT},
{XtNkanjiFont, XtCKanjiFont, XtRString, sizeof(char *),
	XtOffset(XtermWidget, misc.f_kn),
	XtRString, DEFKANJIFONT},
{XtNkanjiBoldFont, XtCKanjiFont, XtRString, sizeof(char *),
	XtOffset(XtermWidget, misc.f_kb),
	XtRString, DEFKANJIBOLDFONT},
#if defined(EWS) || defined(FACOM) || defined(UX386) || defined(SUN)
{XtNkanjiMode,  XtCKanjiMode,  XtRString,  sizeof(char *),
	XtOffset(XtermWidget, misc.k_m),
	XtRString, DEFKANJICODE}
#else
{XtNkanjiMode,  XtCKanjiMode,  XtRString,  sizeof(char *),
	XtOffset(XtermWidget, misc.k_m),
	XtRString, (caddr_t) NULL}
#endif
#ifdef	XJP
,
{XtNjfewOpen,  XtCJfewOpen,  XtRBoolean,  sizeof(Boolean),
	XtOffset(XtermWidget, misc.jfew_open),
	XtRBoolean, (caddr_t) &defaultTRUE},
{XtNjfewMode,  XtCJfewMode,  XtRString,  sizeof(char *),
	XtOffset(XtermWidget, misc.j_m),
	XtRString, (caddr_t) NULL}
#endif	/* XJP */
#endif	KANJI
};


#if defined(EWS) || defined(FACOM) || defined(UX386) || defined(SUN)
void VTRealize(), VTExpose(), VTConfigure(), VTDestroy();
void VTInitialize();
#else
static void VTInitialize(), VTRealize(), VTExpose(), VTConfigure();
static void VTDestroy();
#endif

WidgetClassRec xtermClassRec = {
  {
/* core_class fields */	
    /* superclass	  */	(WidgetClass) &widgetClassRec,
    /* class_name	  */	"VT100",
    /* widget_size	  */	sizeof(XtermWidgetRec),
    /* class_initialize   */    NULL,
    /* class_part_initialize */ NULL,
    /* class_inited       */	FALSE,
    /* initialize	  */	VTInitialize,
    /* initialize_hook    */    NULL,				
    /* realize		  */	VTRealize,
    /* actions		  */	actionsList,
    /* num_actions	  */	XtNumber(actionsList),
    /* resources	  */	resources,
    /* num_resources	  */	XtNumber(resources),
    /* xrm_class	  */	NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	FALSE,
    /* compress_enterleave */   TRUE,
    /* visible_interest	  */	FALSE,
    /* destroy		  */	VTDestroy,
    /* resize		  */	VTConfigure,
    /* expose		  */	VTExpose,
    /* set_values	  */	NULL,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    NULL,
    /* get_values_hook    */    NULL,
    /* accept_focus	  */	NULL,
    /* version            */    XtVersion,
    /* callback_offsets   */    NULL,
    /* tm_table           */    defaultTranslations,
    /* query_geometry     */    XtInheritQueryGeometry,
    /* display_accelerator*/    XtInheritDisplayAccelerator,
    /* extension          */    NULL
  }
};

WidgetClass xtermWidgetClass = (WidgetClass)&xtermClassRec;

VTparse()
{
	register TScreen *screen = &term->screen;
	register int *parsestate = groundtable;
	register int c;
	register char *cp;
	register int row, col, top, bot, scstype;
#ifdef	KANJI
	int	mbcs;
	register short  gset;
	register unsigned char *ucp;
#endif	KANJI
	extern int bitset(), bitclr(), finput(), TrackMouse();

	if(setjmp(vtjmpbuf))
		parsestate = groundtable;
#ifdef	KANJI	/* sano */
	pending_char = pending_bad = 0;
	for( ; ; ) {
		if ((c = doinput()) & 0x80) {	/* if MSB is on */
			if (term->flags & EUC_KANJI) {
					/*
					 * Do processing for EUC code
					 */
#define	EKANJI(c)	(0xa1 <= c && c <= 0xfe)/* true if EUC KANJI */
#define	EKANA1(c)	(0x8e == c)		/* true if SS2 */
#define	EKANA2(c)	(0xa1 <= c && c <= 0xdf)/* true if KANA */
				if (EKANJI(c)) {
					*--bptr = c;
					bcnt++;
					top = bcnt > TEXT_BUF_SIZE ?
						TEXT_BUF_SIZE : bcnt;
					ucp = (unsigned char *)bptr;
					while (top > 1 && EKANJI(*ucp)
						       && EKANJI(*(ucp + 1))) {
						top -= 2, bcnt -= 2;
						/*
						 * Strip off of the both MSB
						 * to convert JIS code
						 */
						*ucp &= 0x7f; ucp++;
						*ucp &= 0x7f; ucp++;
					}
					dotext(screen, term->flags, MBCS|'B',
						bptr, ucp);
					bptr = (char *)ucp;
					if (top > 1 && EKANJI(*ucp) &&
							 !EKANJI(*(ucp + 1))) {
						 doinput();
					} else if (1 == top && EKANJI(*ucp) &&
							1 == bcnt) {
						pending_char = doinput();
					}
					continue;
				} else if (EKANA1(c)) {
					if (0 != bcnt) {
						if (EKANA2((unsigned char)*bptr)) {
							dotext(screen, term->flags, 'I', bptr, (bptr + 1));
							doinput();
						}
					} else {
						pending_char = c;
					}
					continue;
				}
			} else if (term->flags & SJIS_KANJI) {
				/*
				 * Do processing for Shift-JIS Kanji code
				 */
#define SJIS1(c) (0x81 <= c && c <= 0x9f) || (0xe0 <= c && c <= 0xef)
		/* SJIS1 is according to the range of 1st byte in Shift JIS */
#define SJIS2(c) (0x40 <= c && c <= 0xfc && c != 0x7f)
		/* SJIS2 is according to the range of 2nd byte in Shift JIS */
#define SJISK(c) (0xa1 <= c && c <= 0xdf)
		/* SJISK is according to the range of KANA (only one byte,
		   the width is the same of ascii char (half of Kanji)) */
				if (SJIS1(c)) {
					*--bptr = c;
					bcnt++;
					top = bcnt > TEXT_BUF_SIZE ? TEXT_BUF_SIZE : bcnt;
					ucp = (unsigned char *)bptr;
					while (top > 1 && SJIS1(*ucp) && SJIS2(*(ucp + 1))) {
						top -= 2, bcnt -= 2;
						/*
						 * Do code conversion.
						 * It is quite complicated....
						 */
						*ucp = (*ucp - ((*ucp <= 0x9f) ? 0x81 : 0xc1)) * 2 + 0x21;
						ucp++;
						if (*ucp <= 0x7e) {
							*ucp -= 0x1f;
						} else if (*ucp <= 0x9e) {
							*ucp -= 0x20;
						} else {
							*ucp -= 0x7e;
							*(ucp - 1) += 1;
						}
						ucp++;
					}
					dotext(screen, term->flags, MBCS|'B',
						bptr, ucp);
					bptr = (char *)ucp;
					if (top > 1 && SJIS1(*ucp)
						    && !SJIS2(*(ucp + 1))) {
						doinput();
					} else if (1 == top && SJIS1(*ucp)
							    && 1 == bcnt) {
						pending_char = doinput();
					}
					continue;
				} else if (SJISK(c)) {
					top = bcnt > TEXT_BUF_SIZE ? TEXT_BUF_SIZE : bcnt;
					cp = bptr;
					*--bptr = c;
					while (top > 0 &&
						SJISK((unsigned char)*cp)) {
						top--, bcnt--, cp++;
					}
					dotext(screen, term->flags, 'I', bptr, cp);
					bptr = cp;
					continue;
				}
			}
#ifdef	SX9100
#define	JKANA(c) (0xa1 <= c && c <= 0xdf)
			else {
				if (JKANA(c)) {
					dotext(screen, term->flags, 'I', (bptr - 1), bptr);
				}
				continue;
			}
#endif	/* SX9100 */
			/*
			 * If kanji code is JIS, then no processing is required
			 * here. The preceeding escape sequence will turn on/off
			 * the MBCS flags when dotext() is called.
			 */
		}
		switch(parsestate[c &= CHAR]) {
#else	KANJI
	for( ; ; )
		switch(parsestate[c = doinput()]) {
#endif
		 case CASE_PRINT:
			/* printable characters */
			top = bcnt > TEXT_BUF_SIZE ? TEXT_BUF_SIZE : bcnt;
			cp = bptr;
			*--bptr = c;
#ifdef	KANJI	/* sano */
			if(screen->curss) {
				dotext(screen, term->flags,
				 screen->gsets[screen->curss], bptr, bptr + 1) ;
				screen->curss = 0;
				bptr++;
				c = *bptr & 0xff;
			}
			if ((gset = screen->gsets[screen->curgl]) & MBCS) {
				if (' ' == c) {
					gset &= ~MBCS;
					while (top > 0 &&
						' ' == (unsigned char)*cp) {
						top--; bcnt--; cp++;
					}
				} else {
					while (top > 0 && 0x20 < *cp &&
						*cp < 0x7f) {
						top--; bcnt--; cp++;
					}
					if ((cp - bptr) & 1) {
						--cp;
						if (top == 0) {
							if (bcnt == 0)
								pending_char = *cp;
							else
								bcnt++;
						} else
							pending_bad = 1;
					}
				}
			} else
				while (top > 0 && isprint((unsigned char)*cp)) {
					top--; bcnt--; cp++;
			}
			if (bptr < cp)
				dotext(screen, term->flags, gset, bptr, cp);
			bptr = cp;
			if (pending_bad) {
				pending_bad = 0;
				bptr++;
			}
#else	KANJI
			while(top > 0 && isprint(*cp)) {
				top--;
				bcnt--;
				cp++;
			}
			if(screen->curss) {
				dotext(screen, term->flags,
				 screen->gsets[screen->curss], bptr, bptr + 1);
				screen->curss = 0;
				bptr++;
			}
			if(bptr < cp)
				dotext(screen, term->flags,
				 screen->gsets[screen->curgl], bptr, cp);
			bptr = cp;
#endif	KANJI
			break;

		 case CASE_GROUND_STATE:
			/* exit ignore mode */
			parsestate = groundtable;
			break;

		 case CASE_IGNORE_STATE:
			/* Ies: ignore anything else */
			parsestate = igntable;
			break;

		 case CASE_IGNORE_ESC:
			/* Ign: escape */
			parsestate = iestable;
			break;

		 case CASE_IGNORE:
			/* Ignore character */
			break;

		 case CASE_BELL:
			/* bell */
			Bell();
			break;

		 case CASE_BS:
			/* backspace */
			CursorBack(screen, 1);
			break;

		 case CASE_CR:
			/* carriage return */
			CarriageReturn(screen);
			break;

#ifdef	SX9100
		 case CASE_CR_ESC:
			/* carriage return after esc */
			CarriageReturn(screen);
			parsestate = groundtable;
			break;
#endif	/* SX9100 */

		 case CASE_ESC:
			/* escape */
			parsestate = esctable;
			break;

		 case CASE_VMOT:
			/*
			 * form feed, line feed, vertical tab
			 */
#ifdef	STATUSLINE	/* michael */
			if (!screen->instatus)
#endif	STATUSLINE
			Index(screen, 1);
			if (term->flags & LINEFEED)
				CarriageReturn(screen);
			if(screen->display->qlen > 0 ||
			 (ioctl(screen->display->fd, FIONREAD, (char *) &arg), arg) > 0)
				xevents();
			break;

		 case CASE_TAB:
			/* tab */
			screen->cur_col = TabNext(term->tabs, screen->cur_col);
			if (screen->cur_col > screen->max_col)
				screen->cur_col = screen->max_col;
			break;

		 case CASE_SI:
			screen->curgl = 0;
			break;

		 case CASE_SO:
			screen->curgl = 1;
			break;

		 case CASE_SCR_STATE:
			/* enter scr state */
			parsestate = scrtable;
			break;

		 case CASE_SCS0_STATE:
			/* enter scs state 0 */
			scstype = 0;
			parsestate = scstable;
			break;

		 case CASE_SCS1_STATE:
			/* enter scs state 1 */
			scstype = 1;
			parsestate = scstable;
			break;

		 case CASE_SCS2_STATE:
			/* enter scs state 2 */
			scstype = 2;
			parsestate = scstable;
			break;

		 case CASE_SCS3_STATE:
			/* enter scs state 3 */
			scstype = 3;
			parsestate = scstable;
			break;

#ifdef	KANJI	/* kagotani */
		 case CASE_MBCS:
			/* enter smbcs mbcsesc */
			mbcs = 1;
			scstype = 0;
			parsestate = mbcstable;
			break;
#endif	KANJI

		 case CASE_ESC_IGNORE:
			/* unknown escape sequence */
			parsestate = eigtable;
			break;

		 case CASE_ESC_DIGIT:
			/* digit in csi or dec mode */
			if((row = param[nparam - 1]) == DEFAULT)
				row = 0;
			param[nparam - 1] = 10 * row + (c - '0');
			break;

		 case CASE_ESC_SEMI:
			/* semicolon in csi or dec mode */
			param[nparam++] = DEFAULT;
			break;

		 case CASE_DEC_STATE:
			/* enter dec mode */
			parsestate = dectable;
			break;

		 case CASE_ICH:
			/* ICH */
			if((c = param[0]) < 1)
				c = 1;
			InsertChar(screen, c);
			parsestate = groundtable;
			break;

		 case CASE_CUU:
			/* CUU */
#ifdef	STATUSLINE	/* michael */
			/* only if not in status line */
			if (!screen->instatus) {
#endif	STATUSLINE
			if((c = param[0]) < 1)
				c = 1;
			CursorUp(screen, c);
#ifdef	STATUSLINE	/* michael */
			}
#endif	STATUSLINE
			parsestate = groundtable;
			break;

		 case CASE_CUD:
			/* CUD */
#ifdef	STATUSLINE	/* michael */
			/* only if not in status line */
			if (!screen->instatus) {
#endif	STATUSLINE
			if((c = param[0]) < 1)
				c = 1;
			CursorDown(screen, c);
#ifdef	STATUSLINE	/* michael */
			}
#endif	STATUSLINE
			parsestate = groundtable;
			break;

		 case CASE_CUF:
			/* CUF */
			if((c = param[0]) < 1)
				c = 1;
			CursorForward(screen, c);
			parsestate = groundtable;
			break;

		 case CASE_CUB:
			/* CUB */
			if((c = param[0]) < 1)
				c = 1;
			CursorBack(screen, c);
			parsestate = groundtable;
			break;

		 case CASE_CUP:
			/* CUP | HVP */
#ifdef	STATUSLINE	/* michael */
			/* only if not in status line */
			if (!screen->instatus) {
#endif	STATUSLINE
			if((row = param[0]) < 1)
				row = 1;
			if(nparam < 2 || (col = param[1]) < 1)
				col = 1;
			CursorSet(screen, row-1, col-1, term->flags);
#ifdef	STATUSLINE	/* michael */
			}
#endif	STATUSLINE
			parsestate = groundtable;
			break;

		 case CASE_ED:
			/* ED */
			switch (param[0]) {
			 case DEFAULT:
			 case 0:
#ifdef	STATUSLINE	/* michael */
				if (screen->instatus)
					ClearRight(screen);
				else
#endif	STATUSLINE
				ClearBelow(screen);
				break;

			 case 1:
#ifdef	STATUSLINE	/* michael */
				if (screen->instatus)
					ClearLeft(screen);
				else
#endif	STATUSLINE
				ClearAbove(screen);
				break;

			 case 2:
#ifdef	STATUSLINE	/* michael */
				if (screen->instatus)
					ClearLine(screen);
				else
#endif	STATUSLINE
				ClearScreen(screen);
				break;
			}
			parsestate = groundtable;
			break;

		 case CASE_EL:
			/* EL */
			switch (param[0]) {
			 case DEFAULT:
			 case 0:
				ClearRight(screen);
				break;
			 case 1:
				ClearLeft(screen);
				break;
			 case 2:
				ClearLine(screen);
				break;
			}
			parsestate = groundtable;
			break;

		 case CASE_IL:
			/* IL */
#ifdef	STATUSLINE	/* michael */
			/* only if not in status line */
			if (!screen->instatus) {
#endif	STATUSLINE
			if((c = param[0]) < 1)
				c = 1;
			InsertLine(screen, c);
#ifdef	STATUSLINE	/* michael */
			}
#endif	STATUSLINE
			parsestate = groundtable;
			break;

		 case CASE_DL:
			/* DL */
#ifdef	STATUSLINE	/* michael */
			/* only if not in status line */
			if (!screen->instatus) {
#endif	STATUSLINE
			if((c = param[0]) < 1)
				c = 1;
			DeleteLine(screen, c);
#ifdef	STATUSLINE	/* michael */
			}
#endif	STATUSLINE
			parsestate = groundtable;
			break;

		 case CASE_DCH:
			/* DCH */
			if((c = param[0]) < 1)
				c = 1;
			DeleteChar(screen, c);
			parsestate = groundtable;
			break;

		 case CASE_TRACK_MOUSE:
		 	/* Track mouse as long as in window and between
			   specified rows */
			TrackMouse(param[0], param[2]-1, param[1]-1,
			 param[3]-1, param[4]-2);
			break;

		 case CASE_DECID:
			param[0] = -1;		/* Default ID parameter */
			/* Fall through into ... */
		 case CASE_DA1:
			/* DA1 */
			if (param[0] <= 0) {	/* less than means DEFAULT */
				reply.a_type   = CSI;
				reply.a_pintro = '?';
				reply.a_nparam = 2;
				reply.a_param[0] = 1;		/* VT102 */
				reply.a_param[1] = 2;		/* VT102 */
				reply.a_inters = 0;
				reply.a_final  = 'c';
				unparseseq(&reply, screen->respond);
			}
			parsestate = groundtable;
			break;

		 case CASE_TBC:
			/* TBC */
			if ((c = param[0]) <= 0) /* less than means default */
				TabClear(term->tabs, screen->cur_col);
			else if (c == 3)
				TabZonk(term->tabs);
			parsestate = groundtable;
			break;

		 case CASE_SET:
			/* SET */
			modes(term, bitset);
			parsestate = groundtable;
			break;

		 case CASE_RST:
			/* RST */
			modes(term, bitclr);
			parsestate = groundtable;
			break;

		 case CASE_SGR:
			/* SGR */
			for (c=0; c<nparam; ++c) {
				switch (param[c]) {
				 case DEFAULT:
				 case 0:
					term->flags &= ~(INVERSE|BOLD|UNDERLINE);
					break;
				 case 1:
				 case 5:	/* Blink, really.	*/
					term->flags |= BOLD;
					break;
				 case 4:	/* Underscore		*/
					term->flags |= UNDERLINE;
					break;
				 case 7:
					term->flags |= INVERSE;
				}
			}
			parsestate = groundtable;
			break;

		 case CASE_CPR:
			/* CPR */
			if ((c = param[0]) == 5) {
				reply.a_type = CSI;
				reply.a_pintro = 0;
				reply.a_nparam = 1;
				reply.a_param[0] = 0;
				reply.a_inters = 0;
				reply.a_final  = 'n';
				unparseseq(&reply, screen->respond);
			} else if (c == 6) {
				reply.a_type = CSI;
				reply.a_pintro = 0;
				reply.a_nparam = 2;
				reply.a_param[0] = screen->cur_row+1;
				reply.a_param[1] = screen->cur_col+1;
				reply.a_inters = 0;
				reply.a_final  = 'R';
				unparseseq(&reply, screen->respond);
			}
			parsestate = groundtable;
			break;

		 case CASE_DECSTBM:
			/* DECSTBM */
#ifdef	STATUSLINE	/* michael */
			/* only if not in status line */
			if (!screen->instatus) {
#endif	STATUSLINE
			if((top = param[0]) < 1)
				top = 1;
			if(nparam < 2 || (bot = param[1]) == DEFAULT
			   || bot > screen->max_row + 1
			   || bot == 0)
				bot = screen->max_row+1;
			if (bot > top) {
				if(screen->scroll_amt)
					FlushScroll(screen);
				screen->top_marg = top-1;
				screen->bot_marg = bot-1;
				CursorSet(screen, 0, 0, term->flags);
			}
#ifdef	STATUSLINE	/* michael */
			}
#endif	STATUSLINE
			parsestate = groundtable;
			break;

		 case CASE_DECREQTPARM:
			/* DECREQTPARM */
			if ((c = param[0]) == DEFAULT)
				c = 0;
			if (c == 0 || c == 1) {
				reply.a_type = CSI;
				reply.a_pintro = 0;
				reply.a_nparam = 7;
				reply.a_param[0] = c + 2;
				reply.a_param[1] = 1;	/* no parity */
				reply.a_param[2] = 1;	/* eight bits */
				reply.a_param[3] = 112;	/* transmit 9600 baud */
				reply.a_param[4] = 112;	/* receive 9600 baud */
				reply.a_param[5] = 1;	/* clock multiplier ? */
				reply.a_param[6] = 0;	/* STP flags ? */
				reply.a_inters = 0;
				reply.a_final  = 'x';
				unparseseq(&reply, screen->respond);
			}
			parsestate = groundtable;
			break;

		 case CASE_DECSET:
			/* DECSET */
			dpmodes(term, bitset);
			parsestate = groundtable;
			if(screen->TekEmu)
				return;
			break;

		 case CASE_DECRST:
			/* DECRST */
			dpmodes(term, bitclr);
			parsestate = groundtable;
			break;

		 case CASE_DECALN:
			/* DECALN */
			if(screen->cursor_state)
				HideCursor();
			for(row = screen->max_row ; row >= 0 ; row--) {
				bzero(screen->buf[2 * row + 1],
				 col = screen->max_col + 1);
				for(cp = screen->buf[2 * row] ; col > 0 ; col--)
					*cp++ = 'E';
			}
			ScrnRefresh(screen, 0, 0, screen->max_row + 1,
			 screen->max_col + 1, False);
			parsestate = groundtable;
			break;

		 case CASE_GSETS:
#ifdef	KANJI	/* sano & kagotani */
			if (mbcs) {
				c |= MBCS;
				mbcs = 0;
			}
#endif	KANJI
			screen->gsets[scstype] = c;
			parsestate = groundtable;
			break;

		 case CASE_DECSC:
			/* DECSC */
			CursorSave(term, &screen->sc);
			parsestate = groundtable;
			break;

		 case CASE_DECRC:
			/* DECRC */
			CursorRestore(term, &screen->sc);
			parsestate = groundtable;
			break;

		 case CASE_DECKPAM:
			/* DECKPAM */
			term->keyboard.flags |= KYPD_APL;
			parsestate = groundtable;
			break;

		 case CASE_DECKPNM:
			/* DECKPNM */
			term->keyboard.flags &= ~KYPD_APL;
			parsestate = groundtable;
			break;

		 case CASE_IND:
			/* IND */
#ifdef	STATUSLINE	/* michael */
			/* only if not in status line */
			if (!screen->instatus)
#endif	STATUSLINE
			Index(screen, 1);
			if(screen->display->qlen > 0 ||
			 (ioctl(screen->display->fd, FIONREAD, (char *)&arg), arg) > 0)
				xevents();
			parsestate = groundtable;
			break;

		 case CASE_NEL:
			/* NEL */
#ifdef	STATUSLINE	/* michael */
			/* only if not in status line */
			if (!screen->instatus)
#endif	STATUSLINE
			Index(screen, 1);
			CarriageReturn(screen);
			if(screen->display->qlen > 0 ||
			 (ioctl(screen->display->fd, FIONREAD, (char *)&arg), arg) > 0)
				xevents();
			parsestate = groundtable;
			break;

		 case CASE_HTS:
			/* HTS */
			TabSet(term->tabs, screen->cur_col);
			parsestate = groundtable;
			break;

		 case CASE_RI:
			/* RI */
#ifdef	STATUSLINE	/* michael */
			/* only if not in status line */
			if (!screen->instatus)
#endif	STATUSLINE
			RevIndex(screen, 1);
			parsestate = groundtable;
			break;

		 case CASE_SS2:
			/* SS2 */
			screen->curss = 2;
			parsestate = groundtable;
			break;

		 case CASE_SS3:
			/* SS3 */
			screen->curss = 3;
			parsestate = groundtable;
			break;

		 case CASE_CSI_STATE:
			/* enter csi state */
			nparam = 1;
			param[0] = DEFAULT;
			parsestate = csitable;
			break;

		 case CASE_OSC:
			/* do osc escapes */
			do_osc(finput);
			parsestate = groundtable;
			break;

		 case CASE_RIS:
			/* RIS */
			VTReset(TRUE);
			parsestate = groundtable;
			break;

		 case CASE_LS2:
			/* LS2 */
			screen->curgl = 2;
			parsestate = groundtable;
			break;

		 case CASE_LS3:
			/* LS3 */
			screen->curgl = 3;
			parsestate = groundtable;
			break;

		 case CASE_LS3R:
			/* LS3R */
			screen->curgr = 3;
			parsestate = groundtable;
			break;

		 case CASE_LS2R:
			/* LS2R */
			screen->curgr = 2;
			parsestate = groundtable;
			break;

		 case CASE_LS1R:
			/* LS1R */
			screen->curgr = 1;
			parsestate = groundtable;
			break;

#ifdef	STATUSLINE	/* michael */
		 case CASE_TO_STATUS:
			if ((c = param[0]) < 1)
				c = 1;
			ToStatus(c - 1);
			parsestate = groundtable;
			break ;

		 case CASE_FROM_STATUS:
			FromStatus();
			parsestate = groundtable;
			break ;

		 case CASE_SHOW_STATUS:
			ShowStatus();
			parsestate = groundtable;
			break ;

		 case CASE_HIDE_STATUS:
			HideStatus();
			parsestate = groundtable;
			break ;

		 case CASE_ERASE_STATUS:
			EraseStatus();
			parsestate = groundtable;
			break ;
#endif	STATUSLINE

		 case CASE_XTERM_SAVE:
			savemodes(term);
			parsestate = groundtable;
			break;

		 case CASE_XTERM_RESTORE:
			restoremodes(term);
			parsestate = groundtable;
			break;
		}
#ifdef	KANJI	/* sano */
	}
#endif	KANJI
}

finput()
{
	return(doinput());
}

static int select_mask;
static int write_mask;

static char v_buffer[4096];
static char *v_bufstr;
static char *v_bufptr;
static char *v_bufend;
#define	ptymask()	(v_bufptr > v_bufstr ? pty_mask : 0)

v_write(f, d, l)
int f;
char *d;
int l;
{
	int r;
	int c = l;

	if (!v_bufstr) {
		v_bufstr = v_buffer;
		v_bufptr = v_buffer;
		v_bufend = &v_buffer[4096];
	}


	if ((1 << f) != pty_mask)
		return(write(f, d, l));

	if (v_bufptr > v_bufstr) {
		if (l) {
			if (v_bufend > v_bufptr + l) {
				Bcopy(d, v_bufptr, l);
				v_bufptr += l;
			} else {
				if (v_bufstr != v_buffer) {
					Bcopy(v_bufstr, v_buffer,
					      v_bufptr - v_bufstr);
					v_bufptr -= v_bufstr - v_buffer;
					v_bufstr = v_buffer;
				}
				if (v_bufend > v_bufptr + l) {
					Bcopy(d, v_bufptr, l);
					v_bufptr += l;
				} else if (v_bufptr < v_bufend) {
					fprintf(stderr, "Out of buffer space\n");
					c = v_bufend - v_bufptr;
					Bcopy(d, v_bufptr, c);
					v_bufptr = v_bufend;
				} else {
					fprintf(stderr, "Out of buffer space\n");
					c = 0;
				}
			}
		}
		if (v_bufptr > v_bufstr) {
			if ((r = write(f, v_bufstr, v_bufptr - v_bufstr)) <= 0)
				return(r);
			if ((v_bufstr += r) >= v_bufptr)
				v_bufstr = v_bufptr = v_buffer;
		}
	} else if (l) {
		if ((r = write(f, d, l)) < 0) {
			if (errno == EWOULDBLOCK)
				r = 0;
			else if (errno == EINTR)
				r = 0;
			else
				return(r);
		}
		if (l - r) {
			if (l - r > v_bufend - v_buffer) {
				fprintf(stderr, "Truncating to %d\n",
						v_bufend - v_buffer);
				l = (v_bufend - v_buffer) + r;
			}
			Bcopy(d + r, v_buffer, l - r);
			v_bufstr = v_buffer;
			v_bufptr = v_buffer + (l - r);
		}
	}
	return(c);
}

in_put()
{
	register TScreen *screen = &term->screen;
	register char *cp;
	register int i;
	static struct timeval trackTimeOut;

#ifdef FACOM
	select_mask = 0;
#else
#ifndef	SX9100
	select_mask = pty_mask;	/* force initial read */
#endif	/* !SX9100 */
#endif
	for( ; ; ) {
#if defined(SX9100) || defined(UX386)
		select_mask = pty_mask; /* force initial read */
#endif
		if((select_mask & pty_mask) && (eventMode == NORMAL)) {
			if(screen->logging)
				FlushLog(screen);
#ifdef	KANJI	/* sano */
			/*
			 * buffer[0] is reserved for the pending_char.
			 */
			if((bcnt = read(screen->respond, bptr = &buffer[1],
#ifdef	SX9100
			 BUF_SIZE - 1)) < 0) {
#else	/* SX9100 */
			 BUF_SIZE)) < 0) {
#endif	/* SX9100 */
#else	KANJI
			if((bcnt = read(screen->respond, bptr = buffer,
			 BUF_SIZE)) < 0) {
#endif	KANJI
				if(errno == EIO)
				/*
					Cleanup (0);
				*/
					exit(0);
				else if(errno != EWOULDBLOCK)
					Panic(
				 "input: read returned unexpected error (%d)\n",
					 errno);
			} else if(bcnt == 0)
				Panic("input: read returned zero\n", 0);
			else {
#ifdef	KANJI	/* sano */
#ifdef	SX9100
#if (defined(XJP) && defined(PKT_MODE))
				if (*bptr == 0) {
					bptr++;
					bcnt--;
					if(screen->logging) {
						screen->logstart++;
						SaveLog();
					}
				} else if (*bptr & PIOCPKT_IOCTL) {
					(void)arrange_ioctl();
					/* PIOCPKT_IOCTL */
					bptr = &buffer[1];
					bcnt = 0;
					continue;
				} else  {
					bptr = &buffer[1];
					bcnt = 0;
					continue;
				}
#endif	/* */
#endif	/* SX9100 */

				if (pending_char) {
					/*
					 * restore pending_char to the top of
					 * the text which just read.
					 */
					*--bptr = pending_char;
					bcnt++;
					pending_char = 0;
				}
#else	KANJI
				/* strip parity bit */
				for(i = bcnt, cp = bptr ; i > 0 ; i--)
					*cp++ &= CHAR;
#endif	KANJI
				if(screen->scrollWidget && screen->scrollinput &&
				 screen->topline < 0)
					/* Scroll to bottom */
					WindowScroll(screen, 0);
				break;
			}
		}
		if(screen->scroll_amt)
			FlushScroll(screen);
		if(screen->cursor_set && (screen->cursor_col != screen->cur_col
		 || screen->cursor_row != screen->cur_row)) {
			if(screen->cursor_state)
				HideCursor();
			ShowCursor();
#ifdef	XJP
			XJP_cursormove(screen, 0, 0);
#endif	/* XJP */
		} else if(screen->cursor_set != screen->cursor_state) {
#ifdef	XJP
			if(screen->cursor_set) {
				ShowCursor();
				XJP_cursormove(screen, 0, 0);
			}
#else	/* XJP */
			if(screen->cursor_set)
				ShowCursor();
#endif	/* XJP */
			else
				HideCursor();
		}
		
	if (waitingForTrackInfo) {
			trackTimeOut.tv_sec = TRACKTIMESEC;
			trackTimeOut.tv_usec = TRACKTIMEUSEC;
			select_mask = pty_mask;
			if ((i = select(max_plus1, &select_mask, (int *)NULL, (int *)NULL,
			 &trackTimeOut)) < 0) {
			 	if (errno != EINTR)
					SysError(ERROR_SELECT);
				continue;
			} else if (i == 0) {
				/* emacs just isn't replying, go on */
				waitingForTrackInfo = 0;
				Bell();
				select_mask = Select_mask;
			}
		} else if (QLength(screen->display))
			select_mask = X_mask;
		else {
			write_mask = ptymask();
			XFlush(screen->display);
			select_mask = Select_mask;
			if (eventMode != NORMAL)
				select_mask = X_mask;
#ifdef	XJP
			if(screen->kinit && screen->kconvert && screen->kmode == XJP_XY)
				Dispatch_move(screen);
#endif	/* XJP */
			if(select(max_plus1, &select_mask, &write_mask, 
				(int *)NULL, (struct timeval *) NULL) < 0){
				if (errno != EINTR)
					SysError(ERROR_SELECT);
				continue;
			} 
		}
		if (write_mask & ptymask())
			v_write(screen->respond, 0, 0);	/* flush buffer */
		if(select_mask & X_mask) {
			if (bcnt <= 0) {
				bcnt = 0;
#ifdef	SX9100
				bptr = &buffer[1];
#else	/* SX9100 */
				bptr = buffer;
#endif	/* SX9100 */
			}
			xevents();
			if (bcnt > 0)
				break;
		}
	}
	bcnt--;
	return(*bptr++);
}

/*
 * process a string of characters according to the character set indicated
 * by charset.  worry about end of line conditions (wraparound if selected).
 */
dotext(screen, flags, charset, buf, ptr)
register TScreen	*screen;
unsigned	flags;
#ifdef	KANJI	/* sano */
short		charset;
#else	KANJI
char		charset;
#endif	KANJI
char	*buf;
char	*ptr;
{
	register char	*s;
	register int	len;
	register int	n;
	register int	next_col;
#ifdef	KANJI	/* sano */
	register int	do_wrap = 0;
#endif	KANJI

	switch (charset) {
	case 'A':	/* United Kingdom set			*/
		for (s=buf; s<ptr; ++s)
			if (*s == '#')
				*s = '\036';	/* UK pound sign*/
		break;

	case 'B':	/* ASCII set				*/
#ifdef	KANJI	/* sano */
		/*
		 * 'H' should be removed here because it is *not*
		 * ascii but Swedish character set. Some Kanji text
		 * still includes ESC-(-H, which does not designate
		 * Romaji nor ascii character set. This is due to
		 * the mis-printing of early JIS documentation.
		 */
	case 'J':
	case 'H':
	case '@':
#endif	KANJI
		break;

	case '0':	/* special graphics (line drawing)	*/
		for (s=buf; s<ptr; ++s)
			if (*s>=0x5f && *s<=0x7e)
				*s = *s == 0x5f ? 0x7f : *s - 0x5f;
		break;

#ifdef	KANJI	/* sano */
	case 'I':
		flags |= KANACS;
		for (s = buf; s < ptr; ++s)
			*s &= 0x7f;
		break;
		/*
		 * ESC-$-(-@ and ESC-$-(-B designate the Kanji character set.
		 * set KCS flags...
		 */
	case MBCS|'@':
	case MBCS|'B':
		flags |= KCS;
		break;
#endif
	default:	/* any character sets we don't recognize*/
		return;
	}

	len = ptr - buf; 
	ptr = buf;
	while (len > 0) {
		n = screen->max_col-screen->cur_col+1;
		if (n <= 1) {
#ifdef	STATUSLINE	/* michael */
			if (screen->do_wrap && (flags&WRAPAROUND) &&
			    !screen->instatus) {
#else	STATUSLINE
			if (screen->do_wrap && (flags&WRAPAROUND)) {
#endif	STATUSLINE
				Index(screen, 1);
				screen->cur_col = 0;
				screen->do_wrap = 0;
				n = screen->max_col+1;
			} else
				n = 1;
		}
		if (len < n)
			n = len;
#ifdef	KANJI
		if (flags & KCS) {
			if (n == 1) {
				if (flags & WRAPAROUND) {
					n--; do_wrap = 1;
				} else
					n++;
			} else
				if (n & 1)
					n--;
		}
#endif
		next_col = screen->cur_col + n;
		WriteText(screen, ptr, n, flags);
		/*
		 * the call to WriteText updates screen->cur_col.
		 * If screen->cur_col != next_col, we must have
		 * hit the right margin, so set the do_wrap flag.
		 */
#ifdef	KANJI	/* sano */
		screen->do_wrap = (screen->cur_col < next_col) | do_wrap;
#else	KANJI
		screen->do_wrap = (screen->cur_col < next_col);
#endif	KANJI
		len -= n;
		ptr += n;
	}
}
 
#ifdef	SX9100
bit_mbc_on(str, len)
char	*str;
int	len;
{
	register char *p;

	for (p = str; len > 0; p++, len--)
		*p |= 0x80;
}
#endif	/* SX9100 */

/*
 * write a string str of length len onto the screen at
 * the current cursor position.  update cursor position.
 */
WriteText(screen, str, len, flags)
register TScreen	*screen;
register char	*str;
register int	len;
unsigned	flags;
{
	register int cx, cy;
	register unsigned fgs = flags;
	GC	currentGC;
#ifdef	SX9100	/* KANA */
	int	kana_bit_on = 0;
#endif	/* SX9100 */	/* KANA */
 
#ifdef	STATUSLINE	/* michael */
   if(screen->instatus && screen->reversestatus)
	fgs ^= INVERSE;
   if(screen->cur_row - screen->topline <= screen->max_row ||
      screen->instatus) {
#else	STATUSLINE
   if(screen->cur_row - screen->topline <= screen->max_row) {
#endif	STATUSLINE
	/*
	if(screen->cur_row == screen->cursor_row && screen->cur_col <=
	 screen->cursor_col && screen->cursor_col <= screen->cur_col + len - 1)
		screen->cursor_state = OFF;
	 */
	if(screen->cursor_state)
		HideCursor();

	/*
	 *	make sure that the correct GC is current
	 */

#ifdef	KANJI	/* sano */
	if (fgs & KCS) {	/* kanji */
		if (fgs & BOLD) {
			if (fgs & INVERSE) {
				currentGC = screen->w_reverseboldGC;
			} else {
				currentGC = screen->w_normalboldGC;
			}
		} else {
			if (fgs & INVERSE) {
				currentGC = screen->w_reverseGC;
			} else {
				currentGC = screen->w_normalGC;
			}
		}
	} else if (fgs & KANACS) {	/* kana */
		if (fgs & BOLD) {
			if (fgs & INVERSE) {
#ifdef	SX9100	/* KANA */
				if (screen->r_reverseboldGC ==
						screen->reverseboldGC)
					kana_bit_on = 1;
#endif	/* SX9100 */	/* KANA */
				currentGC = screen->r_reverseboldGC;
			} else {
#ifdef	SX9100	/* KANA */
				if (screen->r_normalboldGC ==
						screen->normalboldGC)
					kana_bit_on = 1;
#endif	/* SX9100 */	/* KANA */
				currentGC = screen->r_normalboldGC;
			}
		} else {
			if (fgs & INVERSE) {
#ifdef	SX9100	/* KANA */
				if (screen->r_reverseGC == screen->reverseGC)
					kana_bit_on = 1;
#endif	/* SX9100 */	/* KANA */
				currentGC = screen->r_reverseGC;
			} else {
#ifdef	SX9100	/* KANA */
				if (screen->r_normalGC == screen->normalGC)
					kana_bit_on = 1;
#endif	/* SX9100 */	/* KANA */
				currentGC = screen->r_normalGC;
			}
		}
	} else
#endif	KANJI
	if (fgs & BOLD)
		if (fgs & INVERSE)
			currentGC = screen->reverseboldGC;
		else	currentGC = screen->normalboldGC;
	else  /* not bold */
		if (fgs & INVERSE)
			currentGC = screen->reverseGC;
		else	currentGC = screen->normalGC;

	if (fgs & INSERT)
		InsertChar(screen, len);
      if (!(AddToRefresh(screen))) {
		if(screen->scroll_amt)
			FlushScroll(screen);
	cx = CursorX(screen, screen->cur_col);
	cy = CursorY(screen, screen->cur_row)+screen->fnt_norm->ascent;
#ifdef	KANJI	/* sano & michael */
	BreakKanji(screen);
	screen->cur_col += len;
	BreakKanji(screen);
	screen->cur_col -= len;
	if (fgs & KCS) {
		/*
		 * Draw Kanji string with currentGC. If no k_boldGC nor
		 * k_reverseboldGC, then overstrike it.
		 */
		XDrawImageString16(screen->display, TextWindow(screen),
			currentGC, cx, cy, (XChar2b *)str, len / 2);
		if ((fgs & BOLD) && screen->w_enbolden)
			if (currentGC == screen->w_normalGC ||
			    currentGC == screen->w_reverseGC)
				XDrawString16(screen->display, 
					TextWindow(screen), currentGC,
					cx + 1, cy, (XChar2b *)str, len / 2);
	} else if (fgs & KANACS) {
		/*
		 * Draw Kanji string with currentGC. If no k_boldGC nor
		 * k_reverseboldGC, then overstrike it.
		 */
#ifdef	SX9100	/* KANA */
		if (kana_bit_on)
			bit_mbc_on(str, len);
#endif	/* SX9100 */	/* KANA */	
		XDrawImageString(screen->display, TextWindow(screen),
			currentGC, cx, cy, str, len);
		if ((fgs & BOLD) && screen->r_enbolden)
			if (currentGC == screen->r_normalGC ||
			    currentGC == screen->r_reverseGC)
				XDrawString(screen->display, 
					TextWindow(screen), currentGC,
					cx + 1, cy, str, len);
	} else {
#endif	KANJI
 	XDrawImageString(screen->display, TextWindow(screen), currentGC,
			cx, cy, str, len);

	if((fgs & BOLD) && screen->enbolden) 
/*
 * Bug fix by michael
 * original:	if (currentGC == screen->normalGC || screen->reverseGC) 
 */
		if (currentGC == screen->normalGC ||
		    currentGC == screen->reverseGC)
			XDrawString(screen->display, TextWindow(screen),
			      	currentGC,cx + 1, cy, str, len);
#ifdef	KANJI	/* sano */
	}
#endif	KANJI

	if(fgs & UNDERLINE) 
		XDrawLine(screen->display, TextWindow(screen), currentGC,
			cx, cy+1,
			cx + len * FontWidth(screen), cy+1);
	/*
	 * the following statements compile data to compute the average 
	 * number of characters written on each call to XText.  The data
	 * may be examined via the use of a "hidden" escape sequence.
	 */
	ctotal += len;
	++ntotal;
      }
    }
	ScreenWrite(screen, str, flags, len);
	CursorForward(screen, len);
}
 
/*
 * process ANSI modes set, reset
 */
modes(term, func)
XtermWidget	term;
int		(*func)();
{
	register int	i;

	for (i=0; i<nparam; ++i) {
		switch (param[i]) {
		case 4:			/* IRM				*/
			(*func)(&term->flags, INSERT);
			break;

		case 20:		/* LNM				*/
			(*func)(&term->flags, LINEFEED);
			break;
		}
	}
}

/*
 * process DEC private modes set, reset
 */
dpmodes(term, func)
XtermWidget	term;
int		(*func)();
{
	register TScreen	*screen	= &term->screen;
	register int	i, j;
	extern int bitset();

	for (i=0; i<nparam; ++i) {
		switch (param[i]) {
		case 1:			/* DECCKM			*/
			(*func)(&term->keyboard.flags, CURSOR_APL);
			break;
		case 2:			/* ANSI/VT52 mode		*/
			if (func == bitset) {
				screen->gsets[0] =
					screen->gsets[1] =
					screen->gsets[2] =
					screen->gsets[3] = 'B';
				screen->curgl = 0;
				screen->curgr = 2;
			}
			break;
		case 3:			/* DECCOLM			*/
			if(screen->c132) {
				ClearScreen(screen);
				CursorSet(screen, 0, 0, term->flags);
				if((j = func == bitset ? 132 : 80) !=
				 ((term->flags & IN132COLUMNS) ? 132 : 80) ||
				 j != screen->max_col + 1) {
				        Dimension replyWidth, replyHeight;
					XtGeometryResult status;

					status = XtMakeResizeRequest (
					    (Widget) term, 
					    (Dimension) FontWidth(screen) * j
					        + 2*screen->border
						+ screen->scrollbar,
					    (Dimension) FontHeight(screen)
						* (screen->max_row + 1)
#ifdef	STATUSLINE	/* michael */
						+ screen->statusheight
#endif	STATUSLINE
						+ 2 * screen->border,
					    &replyWidth, &replyHeight);

					if (status == XtGeometryYes ||
					    status == XtGeometryDone) {
					    ScreenResize (&term->screen,
							  replyWidth,
							  replyHeight,
							  &term->flags);
					}
				}
				(*func)(&term->flags, IN132COLUMNS);
			}
			break;
		case 4:			/* DECSCLM (slow scroll)	*/
			if (func == bitset) {
				screen->jumpscroll = 0;
				if (screen->scroll_amt)
					FlushScroll(screen);
			} else
				screen->jumpscroll = 1;
			(*func)(&term->flags, SMOOTHSCROLL);
			break;
		case 5:			/* DECSCNM			*/
			j = term->flags;
			(*func)(&term->flags, REVERSE_VIDEO);
			if ((term->flags ^ j) & REVERSE_VIDEO)
				ReverseVideo(term);
			break;

		case 6:			/* DECOM			*/
			(*func)(&term->flags, ORIGIN);
			CursorSet(screen, 0, 0, term->flags);
			break;

		case 7:			/* DECAWM			*/
			(*func)(&term->flags, WRAPAROUND);
			break;
		case 8:			/* DECARM			*/
#ifdef DO_AUTOREPEAT
			j = term->flags;
			(*func)(&term->flags, AUTOREPEAT);
			if ((term->flags ^ j) & AUTOREPEAT)
				if(term->flags & AUTOREPEAT)
					XAutoRepeatOn(screen->display);
				else
					XAutoRepeatOff(screen->display);
#endif /* DO_AUTOREPEAT */
			break;
		case 9:			/* MIT bogus sequence		*/
			if(func == bitset)
				screen->send_mouse_pos = 1;
			else
				screen->send_mouse_pos = 0;
			break;
#ifdef	SX9100	/* for WNN , cursor invisible */
		case 25:
			if(func == bitset)
				term->flags &= ~(CINV);/* cursor visible for wnn */
			else
				term->flags |= CINV; /* cursor invisible for wnn */
			break;
#endif	/* SX9100 */
		case 38:		/* DECTEK			*/
			if(func == bitset & !(screen->inhibit & I_TEK)) {
				if(screen->logging) {
					FlushLog(screen);
					screen->logstart = Tbuffer;
				}
				screen->TekEmu = TRUE;
			}
			break;
		case 40:		/* 132 column mode		*/
#ifdef	SX9100
			if (func == bitset)
				screen->c132 = 1;
			else
				screen->c132 = 0;
#else	/* SX9100 */
			(*func)(&screen->c132, 1);
#endif	/* SX9100 */
			break;
		case 41:		/* curses hack			*/
#ifdef	SX9100
			if (func == bitset)
				screen->curses = 1;
			else
				screen->curses = 0;
#else	/* SX9100 */
			(*func)(&screen->curses, 1);
#endif	/* SX9100 */
			break;
		case 44:		/* margin bell			*/
#ifdef	SX9100
			if (func == bitset)
				screen->marginbell = 1;
			else
				screen->marginbell = 0;
#else	/* SX9100 */
			(*func)(&screen->marginbell, 1);
#endif	/* SX9100 */
			if(!screen->marginbell)
				screen->bellarmed = -1;
			break;
		case 45:		/* reverse wraparound	*/
			(*func)(&term->flags, REVERSEWRAP);
			break;
		case 46:		/* logging		*/
			if(func == bitset)
				StartLog(screen);
			else
				CloseLog(screen);
			break;
		case 47:		/* alternate buffer		*/
			if(func == bitset)
				ToAlternate(screen);
			else
				FromAlternate(screen);
			break;
#ifdef	STATUSLINE	/* michael */
		case 48:
			j = screen->reversestatus;
			(*func)(&screen->reversestatus, 1);
			if (j != screen->reversestatus)
				ScrnRefresh(screen, screen->max_row + 1, 0, 1,
					screen->max_col + 1);
			break;
#endif	STATUSLINE
		case 1000:		/* xtem bogus sequence		*/
			if(func == bitset)
				screen->send_mouse_pos = 2;
			else
				screen->send_mouse_pos = 0;
			break;
		case 1001:		/* xtem sequence w/hilite tracking */
			if(func == bitset)
				screen->send_mouse_pos = 3;
			else
				screen->send_mouse_pos = 0;
			break;
		}
	}
}

/*
 * process xterm private modes save
 */
savemodes(term)
XtermWidget term;
{
	register TScreen	*screen	= &term->screen;
	register int i;

	for (i = 0; i < nparam; i++) {
		switch (param[i]) {
		case 1:			/* DECCKM			*/
			screen->save_modes[0] = term->keyboard.flags &
			 CURSOR_APL;
			break;
		case 3:			/* DECCOLM			*/
			if(screen->c132)
				screen->save_modes[1] = term->flags &
				 IN132COLUMNS;
			break;
		case 4:			/* DECSCLM (slow scroll)	*/
			screen->save_modes[2] = term->flags & SMOOTHSCROLL;
			break;
		case 5:			/* DECSCNM			*/
			screen->save_modes[3] = term->flags & REVERSE_VIDEO;
			break;
		case 6:			/* DECOM			*/
			screen->save_modes[4] = term->flags & ORIGIN;
			break;

		case 7:			/* DECAWM			*/
			screen->save_modes[5] = term->flags & WRAPAROUND;
			break;
		case 8:			/* DECARM			*/
#ifdef DO_AUTOREPEAT
			screen->save_modes[6] = term->flags & AUTOREPEAT;
#endif /* DO_AUTOREPEAT */
			break;
		case 9:			/* mouse bogus sequence */
			screen->save_modes[7] = screen->send_mouse_pos;
			break;
		case 40:		/* 132 column mode		*/
			screen->save_modes[8] = screen->c132;
			break;
		case 41:		/* curses hack			*/
			screen->save_modes[9] = screen->curses;
			break;
		case 44:		/* margin bell			*/
			screen->save_modes[12] = screen->marginbell;
			break;
		case 45:		/* reverse wraparound	*/
			screen->save_modes[13] = term->flags & REVERSEWRAP;
			break;
		case 46:		/* logging		*/
			screen->save_modes[14] = screen->logging;
			break;
		case 47:		/* alternate buffer		*/
			screen->save_modes[15] = screen->alternate;
			break;
#ifdef	STATUSLINE	/* michael */
		case 48:
			screen->save_modes[16] = screen->reversestatus;
			break;
#endif	STATUSLINE
		case 1000:		/* mouse bogus sequence		*/
		case 1001:
			screen->save_modes[7] = screen->send_mouse_pos;
			break;
		}
	}
}

/*
 * process xterm private modes restore
 */
restoremodes(term)
XtermWidget term;
{
	register TScreen	*screen	= &term->screen;
	register int i, j;

	for (i = 0; i < nparam; i++) {
		switch (param[i]) {
		case 1:			/* DECCKM			*/
			term->keyboard.flags &= ~CURSOR_APL;
			term->keyboard.flags |= screen->save_modes[0] &
			 CURSOR_APL;
			break;
		case 3:			/* DECCOLM			*/
			if(screen->c132) {
				ClearScreen(screen);
				CursorSet(screen, 0, 0, term->flags);
				if((j = (screen->save_modes[1] & IN132COLUMNS)
				 ? 132 : 80) != ((term->flags & IN132COLUMNS)
				 ? 132 : 80) || j != screen->max_col + 1) {
				        Dimension replyWidth, replyHeight;
					XtGeometryResult status;
					status = XtMakeResizeRequest (
					    (Widget) term,
					    (Dimension) FontWidth(screen) * j 
						+ 2*screen->border
						+ screen->scrollbar,
					    (Dimension) FontHeight(screen)
						* (screen->max_row + 1)
#ifdef	STATUSLINE	/* michael */
						+ screen->statusheight
#endif	STATUSLINE
						+ 2*screen->border,
					    &replyWidth, &replyHeight);

					if (status == XtGeometryYes ||
					    status == XtGeometryDone) {
					    ScreenResize (&term->screen,
							  replyWidth,
							  replyHeight,
							  &term->flags);
					}
				}
				term->flags &= ~IN132COLUMNS;
				term->flags |= screen->save_modes[1] &
				 IN132COLUMNS;
			}
			break;
		case 4:			/* DECSCLM (slow scroll)	*/
			if (screen->save_modes[2] & SMOOTHSCROLL) {
				screen->jumpscroll = 0;
				if (screen->scroll_amt)
					FlushScroll(screen);
			} else
				screen->jumpscroll = 1;
			term->flags &= ~SMOOTHSCROLL;
			term->flags |= screen->save_modes[2] & SMOOTHSCROLL;
			break;
		case 5:			/* DECSCNM			*/
			if((screen->save_modes[3] ^ term->flags) &
			 REVERSE_VIDEO) {
				term->flags &= ~REVERSE_VIDEO;
				term->flags |= screen->save_modes[3] &
				 REVERSE_VIDEO;
				ReverseVideo(term);
			}
			break;
		case 6:			/* DECOM			*/
			term->flags &= ~ORIGIN;
			term->flags |= screen->save_modes[4] & ORIGIN;
			CursorSet(screen, 0, 0, term->flags);
			break;

		case 7:			/* DECAWM			*/
			term->flags &= ~WRAPAROUND;
			term->flags |= screen->save_modes[5] & WRAPAROUND;
			break;
		case 8:			/* DECARM			*/
#ifdef DO_AUTOREPEAT
			if((screen->save_modes[6] ^ term->flags) & AUTOREPEAT) {
#ifdef	SX9100	/* bug fix */
				term->flags &= ~AUTOREPEAT;
				term->flags |= screen->save_modes[6] &
				 AUTOREPEAT;
#else	/* SX9100 */
				term->flags &= ~REVERSE_VIDEO;
				term->flags |= screen->save_modes[6] &
				 REVERSE_VIDEO;
#endif	/* SX9100 */
				if(term->flags & AUTOREPEAT)
					XAutoRepeatOn(screen->display);
				else
					XAutoRepeatOff(screen->display);
			}
#endif /* DO_AUTOREPEAT */
			break;
		case 9:			/* MIT bogus sequence		*/
			screen->send_mouse_pos = screen->save_modes[7];
			break;
		case 40:		/* 132 column mode		*/
			screen->c132 = screen->save_modes[8];
			break;
		case 41:		/* curses hack			*/
			screen->curses = screen->save_modes[9];
			break;
		case 44:		/* margin bell			*/
			if(!(screen->marginbell = screen->save_modes[12]))
				screen->bellarmed = -1;
			break;
		case 45:		/* reverse wraparound	*/
			term->flags &= ~REVERSEWRAP;
			term->flags |= screen->save_modes[13] & REVERSEWRAP;
			break;
		case 46:		/* logging		*/
			if(screen->save_modes[14])
				StartLog(screen);
			else
				CloseLog(screen);
			break;
		case 47:		/* alternate buffer		*/
			if(screen->save_modes[15])
				ToAlternate(screen);
			else
				FromAlternate(screen);
			break;
#ifdef	STATUSLINE	/* michael */
		case 48:		/* reverse status line */
			if (screen->save_modes[16] != screen->reversestatus) {
				screen->reversestatus = screen->save_modes[16];
				ScrnRefresh(screen, screen->max_row + 1, 0, 1,
					    screen->max_col + 1);
			}
			break;
#endif	STATUSLINE
		case 1000:		/* mouse bogus sequence		*/
		case 1001:
			screen->send_mouse_pos = screen->save_modes[7];
			break;
		}
	}
}

/*
 * set a bit in a word given a pointer to the word and a mask.
 */
bitset(p, mask)
int	*p;
{
	*p |= mask;
}

/*
 * clear a bit in a word given a pointer to the word and a mask.
 */
bitclr(p, mask)
int	*p;
{
	*p &= ~mask;
}

unparseseq(ap, fd)
register ANSI	*ap;
{
	register int	c;
	register int	i;
	register int	inters;

	c = ap->a_type;
	if (c>=0x80 && c<=0x9F) {
		unparseputc(ESC, fd);
		c -= 0x40;
	}
	unparseputc(c, fd);
	c = ap->a_type;
	if (c==ESC || c==DCS || c==CSI || c==OSC || c==PM || c==APC) {
		if (ap->a_pintro != 0)
			unparseputc((char) ap->a_pintro, fd);
		for (i=0; i<ap->a_nparam; ++i) {
			if (i != 0)
				unparseputc(';', fd);
			unparseputn((unsigned int) ap->a_param[i], fd);
		}
		inters = ap->a_inters;
		for (i=3; i>=0; --i) {
			c = (inters >> (8*i)) & 0xff;
			if (c != 0)
				unparseputc(c, fd);
		}
		unparseputc((char) ap->a_final, fd);
	}
}

unparseputn(n, fd)
unsigned int	n;
int fd;
{
	unsigned int	q;

	q = n/10;
	if (q != 0)
		unparseputn(q, fd);
	unparseputc((char) ('0' + (n%10)), fd);
}

unparseputc(c, fd)
char c;
int fd;
{
	char	buf[2];
	register i = 1;
	extern XtermWidget term;

	if((buf[0] = c) == '\r' && (term->flags & LINEFEED)) {
		buf[1] = '\n';
		i++;
	}
	if (write(fd, buf, i) != i)
		Panic("unparseputc: error writing character\n", 0);
}

ToAlternate(screen)
register TScreen *screen;
{
	extern ScrnBuf Allocate();

	if(screen->alternate)
		return;
	if(!screen->altbuf)
#ifdef	STATUSLINE	/* michael */
		screen->altbuf = Allocate(screen->max_row + 2, screen->max_col
		 + 1);
#else	STATUSLINE
		screen->altbuf = Allocate(screen->max_row + 1, screen->max_col
		 + 1);
#endif	STATUSLINE
	SwitchBufs(screen);
	screen->alternate = TRUE;
}

FromAlternate(screen)
register TScreen *screen;
{
	if(!screen->alternate)
		return;
	screen->alternate = FALSE;
	SwitchBufs(screen);
}

SwitchBufs(screen)
register TScreen *screen;
{
	register int rows, top;
	char *save [2 * MAX_ROWS];

	if(screen->cursor_state)
		HideCursor();
	rows = screen->max_row + 1;
	Bcopy((char *)screen->buf, (char *)save, 2 * sizeof(char *) * rows);
	Bcopy((char *)screen->altbuf, (char *)screen->buf, 2 * sizeof(char *) *
	 rows);
	Bcopy((char *)save, (char *)screen->altbuf, 2 * sizeof(char *) * rows);

	if((top = -screen->topline) <= screen->max_row) {
		if(screen->scroll_amt)
			FlushScroll(screen);
#ifdef	STATUSLINE	/* michael */
		if(top == 0 && !screen->statusline)
#else	STATUSLINE
		if(top == 0)
#endif	STATUSLINE
			XClearWindow(screen->display, TextWindow(screen));
		else
			XClearArea(
			    screen->display,
			    TextWindow(screen),
			    (int) screen->border + screen->scrollbar,
			    (int) top * FontHeight(screen) + screen->border,
			    (unsigned) Width(screen),
			    (unsigned) (screen->max_row - top + 1)
				* FontHeight(screen),
			    FALSE);
	}
	ScrnRefresh(screen, 0, 0, rows, screen->max_col + 1, False);
}

VTRun()
{
	register TScreen *screen = &term->screen;
	register int i;
	
	if (!screen->Vshow) {
	    XtRealizeWidget (term->core.parent);
	    set_vt_visibility (TRUE);
	} 

	if (screen->allbuf == NULL) VTallocbuf ();

	screen->cursor_state = OFF;
	screen->cursor_set = ON;
	if(screen->select || screen->always_highlight)
		VTSelect();
	if (L_flag > 0) {
		XWarpPointer (screen->display, None, VWindow(screen),
			    0, 0, 0, 0,
			    FullWidth(screen) >> 1, FullHeight(screen) >>1);
		L_flag = -1;
	}
	bcnt = 0;
#ifdef	SX9100
	bptr = &buffer[1];
#else	/* SX9100 */
	bptr = buffer;
#endif	/* SX9100 */
	while(Tpushb > Tpushback) {
		*bptr++ = *--Tpushb;
		bcnt++;
	}
	bcnt += (i = Tbcnt);
	for( ; i > 0 ; i--)
		*bptr++ = *Tbptr++;
#ifdef	SX9100
	bptr = &buffer[1];
#else	/* SX9100 */
	bptr = buffer;
#endif	/* SX9100 */
	if(!setjmp(VTend))
		VTparse();
	HideCursor();
	screen->cursor_set = OFF;
	if (!screen->always_highlight)
	    VTUnselect ();
	reselectwindow (screen);
}

/*ARGSUSED*/
static void VTExpose(w, event, region)
Widget w;
XEvent *event;
Region region;
{
	register TScreen *screen = &term->screen;

#ifdef DEBUG
	if(debug)
		fputs("Expose\n", stderr);
#endif	/* DEBUG */
	if (event->type == Expose)
		HandleExposure (screen, (XExposeEvent *)event);
}

static void VTGraphicsOrNoExpose (event)
XEvent *event;
    {
	register TScreen *screen = &term->screen;
	if (screen->incopy <= 0) {
		screen->incopy = 1;
		if (screen->scrolls > 0)
			screen->scrolls--;
	}
	if (event->type == GraphicsExpose)
	  if (HandleExposure (screen, (XExposeEvent *)event))
		screen->cursor_state = OFF;
	if ((event->type == NoExpose) || ((XGraphicsExposeEvent *)event)->count == 0) {
		if (screen->incopy <= 0 && screen->scrolls > 0)
			screen->scrolls--;
		if (screen->scrolls)
			screen->incopy = -1;
		else
			screen->incopy = 0;
	}
}

/*ARGSUSED*/
static void VTNonMaskableEvent (w, closure, event)
Widget w;
caddr_t closure;
XEvent *event;
    {
    switch (event->type) {
       case MappingNotify:
	  XRefreshKeyboardMapping (event);
	  break;
       case GraphicsExpose:
       case NoExpose:
	  VTGraphicsOrNoExpose (event);
	  break;
	  }
    }




static void VTConfigure(w)
Widget w;
{
#ifdef	XJP
	register TScreen	*screen = &term->screen;
#endif	/* XJP */
       if (XtIsRealized(w))
#ifdef	XJP
	{
	  if (screen->kconvert && screen->kmode != XJP_ROOT) {
		jp_att.p_width = jp_att.width
			       = screen->kcw_width = term->core.width;
		jp_att.p_height = screen->kcw_full_height
				= screen->kcw_height + term->core.height
				+ jp_att.bp;
		if (screen->kmode == XJP_UNDER) {
			jp_att.y = screen->kcw_posy = term->core.height;
			XJp_change(screen->jpreq_id,
					XJP_PWIDTH|XJP_PHEIGHT|
					XJP_WIDTH|XJP_Y, &jp_att);
		} else {
			jp_att.status_y = term->core.height
					+ FontHeight(screen)/2;
			XJp_change(screen->jpreq_id,
					XJP_PWIDTH|XJP_PHEIGHT|
					XJP_WIDTH|XJP_STATUS, &jp_att);
		}
	  }
#ifdef	STATUSLINE	/* kato */
	{
          ScreenResize (&term->screen, term->core.width, term->core.height, &term->flags);
	  XClearWindow(term->screen.display, term->screen.fullVwin.window);
	  ScrnRefresh(&term->screen, 0, 0,
		term->screen.max_row,
		term->screen.max_col);
	}
#else	STATUSLINE
          ScreenResize (&term->screen, term->core.width, term->core.height, &term->flags);
#endif	STATUSLINE
	}
#else	/* XJP */
#ifdef	STATUSLINE	/* kato */
	{
          ScreenResize (&term->screen, term->core.width, term->core.height, &term->flags);
	  XClearWindow(term->screen.display, term->screen.fullVwin.window);
	  ScrnRefresh(&term->screen, 0, 0,
		term->screen.max_row,
		term->screen.max_col);
	}
#else	STATUSLINE
          ScreenResize (&term->screen, term->core.width, term->core.height, &term->flags);
#endif	STATUSLINE
#endif	/* XJP */

}

static Boolean failed = FALSE;

int VTInit ()
{
    register TScreen *screen = &term->screen;

    if (failed) return (0);
    XtRealizeWidget (term->core.parent);
    if (screen->allbuf == NULL) VTallocbuf ();
    return (1);
}

static void VTallocbuf ()
{
    register TScreen *screen = &term->screen;
    int nrows = screen->max_row + 1;

    /* allocate screen buffer now, if necessary. */
    if (screen->scrollWidget)
      nrows += screen->savelines;
    screen->allbuf = (ScrnBuf) Allocate (nrows, screen->max_col + 1);
    if (screen->scrollWidget)
      screen->buf = &screen->allbuf[2 * screen->savelines];
    else
      screen->buf = screen->allbuf;
    return;
}

static void VTInitialize (request, new)
   XtermWidget request, new;
{
   /* Zero out the entire "screen" component of "new" widget,
      then do field-by-field assigment of "screen" fields
      that are named in the resource list. */

   bzero ((char *) &new->screen, sizeof(new->screen));
   new->screen.c132 = request->screen.c132;
   new->screen.curses = request->screen.curses;
   new->screen.foreground = request->screen.foreground;
   new->screen.cursorcolor = request->screen.cursorcolor;
   new->screen.border = request->screen.border;
   new->screen.jumpscroll = request->screen.jumpscroll;
   new->screen.logfile = request->screen.logfile;
   new->screen.marginbell = request->screen.marginbell;
   new->screen.mousecolor = request->screen.mousecolor;
   new->screen.multiscroll = request->screen.multiscroll;
   new->screen.nmarginbell = request->screen.nmarginbell;
   new->screen.savelines = request->screen.savelines;
   new->screen.scrollinput = request->screen.scrollinput;
   new->screen.scrollkey = request->screen.scrollkey;
   new->screen.visualbell = request->screen.visualbell;
   new->screen.TekEmu = request->screen.TekEmu;
   new->misc.re_verse = request->misc.re_verse;
   new->screen.multiClickTime = request->screen.multiClickTime;
   new->screen.charClass = request->screen.charClass;
   new->screen.cutNewline = request->screen.cutNewline;
   new->screen.cutToBeginningOfLine = request->screen.cutToBeginningOfLine;
   new->screen.always_highlight = request->screen.always_highlight;
   new->screen.pointer_cursor = request->screen.pointer_cursor;
   new->misc.titeInhibit = request->misc.titeInhibit;
#ifdef	STATUSLINE	/* kato */
   new->screen.statusline = request->screen.statusline;
   new->screen.reversestatus = request->screen.reversestatus;
#endif	STATUSLINE

    /*
     * set the colors if reverse video; this is somewhat tricky since
     * there are 5 colors:
     *
     *     background - paper		white
     *     foreground - text		black
     *     border - border			black (foreground)
     *     textcursor - block		black (foreground)
     *     mousecursor - mouse		black (foreground)
     *
     */
    if (new->misc.re_verse) {
	unsigned long fg = new->screen.foreground;
	unsigned long bg = new->core.background_pixel;

	if (new->screen.mousecolor == fg) new->screen.mousecolor = bg;
	if (new->screen.cursorcolor == fg) new->screen.cursorcolor = bg;
	if (new->core.border_pixel == fg) new->core.border_pixel = bg;
	new->screen.foreground = bg;
	new->core.background_pixel = fg;
    }	

   new->keyboard.flags = 0;
   new->screen.display = new->core.screen->display;
   new->core.height = new->core.width = 1;
      /* dummy values so that we don't try to Realize the parent shell 
	 with height or width of 0, which is illegal in X.  The real
	 size is computed in the xtermWidget's Realize proc,
	 but the shell's Realize proc is called first, and must see
	 a valid size. */

   /* look for focus related events on the shell, because we need
    * to care about the shell's border being part of our focus.
    */
   XtAddEventHandler(XtParent(new), EnterWindowMask, FALSE,
		HandleEnterWindow, (Opaque)NULL);
   XtAddEventHandler(XtParent(new), LeaveWindowMask, FALSE,
		HandleLeaveWindow, (Opaque)NULL);
   XtAddEventHandler(XtParent(new), FocusChangeMask, FALSE,
		HandleFocusChange, (Opaque)NULL);
   XtAddEventHandler(new, 0L, TRUE,
		VTNonMaskableEvent, (Opaque)NULL);

   set_character_class (new->screen.charClass);

   /* create it, but don't realize it */
   ScrollBarOn (new, TRUE, FALSE);

   return;
}


static void VTDestroy (w)
Widget w;
{
    XtFree(((XtermWidget)w)->screen.selection);
}

/*ARGSUSED*/
static void VTRealize (w, valuemask, values)
Widget w;
XtValueMask *valuemask;
XSetWindowAttributes *values;
{
	unsigned int width, height;
	register TScreen *screen = &term->screen;
	register int i, j;
	XPoint	*vp;
	static short failed;
	int xpos, ypos, pr;
	extern char *malloc();
	XGCValues		xgcv;
	XtGCMask			mask;
	XSizeHints		sizehints;
	XWMHints		wmhints;
	extern int		VTgcFontMask;
	int scrollbar_width;
#ifdef	XJP
	extern int kcw_on_flg;

	if (term->misc.jfew_open)
		kcw_on_flg = 1;
#endif	/* XJP */

	if(failed)
		return;
	
	TabReset (term->tabs);

#ifdef	KANJI	/* sano */
	screen->fnt_r_norm = screen->fnt_r_bold =
	screen->fnt_w_norm = screen->fnt_w_bold =
#endif	KANJI
	screen->fnt_norm = screen->fnt_bold = NULL;
	
	/* do the XFont calls */

	if ((screen->fnt_norm = XLoadQueryFont(screen->display, term->misc.f_n)) == NULL) {
	    fprintf(stderr, "%s: Could not open font %s; using server default\n", 
		    xterm_name, term->misc.f_n);
	    screen->fnt_norm =
		  XQueryFont(screen->display,
			     DefaultGC(screen->display,
				       DefaultScreen(screen->display)
				       )->gid
			     );
	    VTgcFontMask = 0;
	}

	if (!term->misc.f_b || !VTgcFontMask
	    || !(screen->fnt_bold = XLoadQueryFont(screen->display, term->misc.f_b))) {
		screen->fnt_bold = screen->fnt_norm;
		screen->enbolden = TRUE;  /*no bold font */
	}
#ifdef	KANJI	/* sano */
	if (!term->misc.f_rn ||
		!(screen->fnt_r_norm =
			XLoadQueryFont(screen->display, term->misc.f_rn)))
		screen->fnt_r_norm = screen->fnt_norm;
	if (!term->misc.f_rb ||
		!(screen->fnt_r_bold =
			XLoadQueryFont(screen->display, term->misc.f_rb))) {
		screen->fnt_r_bold = screen->fnt_r_norm;
		screen->r_enbolden = TRUE;	/* no bold font */
	}

	if (!term->misc.f_kn ||
		!(screen->fnt_w_norm =
			XLoadQueryFont(screen->display, term->misc.f_kn)))
		screen->fnt_w_norm = screen->fnt_norm;
	if (!term->misc.f_kb ||
		!(screen->fnt_w_bold =
			XLoadQueryFont(screen->display, term->misc.f_kb))) {
		screen->fnt_w_bold = screen->fnt_w_norm;
		screen->w_enbolden = TRUE;	/* no bold font */
	}
#endif	KANJI

	/* find the max width and higth of the font */

	screen->fullVwin.f_width = screen->fnt_norm->max_bounds.width;
	screen->fullVwin.f_height = screen->fnt_norm->ascent +
				    screen->fnt_norm->descent;

	/* making cursor */
	{
	    unsigned long fg, bg;

	    bg = term->core.background_pixel;
	    if (screen->mousecolor == term->core.background_pixel) {
		fg = screen->foreground;
	    } else {
		fg = screen->mousecolor;
	    }

	    if (!screen->pointer_cursor) 
	    	screen->pointer_cursor = make_xterm (fg, bg);
	    else 
	        recolor_cursor (screen->pointer_cursor, fg, bg);
	}

	scrollbar_width = (term->misc.scrollbar ? 
			   screen->scrollWidget->core.width : 0);
	i = 2 * screen->border + scrollbar_width;
	j = 2 * screen->border;
#ifdef	STATUSLINE	/* michael */
	if (screen->statusline)
		j += (screen->statusheight = screen->fullVwin.f_height + 2);
#endif	STATUSLINE


	/* set defaults */
#ifdef	KANJI	/* ishisone */
	xpos = 0; ypos = 0; width = 80; height = 24;
#else	/* KANJI */
	xpos = 1; ypos = 1; width = 80; height = 24;
#endif	/* KANJI */

	pr = XParseGeometry(term->misc.geo_metry, &xpos, &ypos, &width, &height);

#ifdef	XJP
	if (screen->kmode == XJP_UNDER) {
		screen->kcw_height = screen->fullVwin.f_height * 2;
		jp_att.bp = screen->fullVwin.f_height/7;
	} else if (screen->kmode == XJP_XY) {
		screen->kcw_height = screen->fullVwin.f_height * 2;
		jp_att.bp = 0;
	} else {
		screen->kcw_height = 0;
		jp_att.bp = 0;
	}
#endif	/* XJP */
	screen->max_col = width;
	screen->max_row = height;
	width = width * screen->fullVwin.f_width + i;
	height = height * screen->fullVwin.f_height + j;

	if ((pr & XValue) && (XNegative&pr)) 
	  xpos += DisplayWidth(screen->display, DefaultScreen(screen->display)) 
			- width - (term->core.parent->core.border_width * 2);
	if ((pr & YValue) && (YNegative&pr))
	  ypos += DisplayHeight(screen->display, DefaultScreen(screen->display)) 
			- height - (term->core.parent->core.border_width * 2);

	/* set up size hints for window manager */
	sizehints.min_width = 2 * screen->border + scrollbar_width;
	sizehints.min_height = 2 * screen->border;
#ifdef	STATUSLINE	/* ishisone */
	sizehints.min_height += screen->statusheight;
#endif
#ifdef	XJP
	if(term->misc.jfew_open && screen->kmode != XJP_ROOT)
		sizehints.min_height += (screen->kcw_height + jp_att.bp);
#endif	/* XJP */
	sizehints.width_inc = FontWidth(screen);
	sizehints.height_inc = FontHeight(screen);
	sizehints.flags = PMinSize|PResizeInc;
	sizehints.x = xpos;
	sizehints.y = ypos;
	if ((XValue&pr) || (YValue&pr)) 
	  sizehints.flags |= USSize|USPosition;
	else sizehints.flags |= PSize|PPosition;
	sizehints.width = width;
	sizehints.height = height;
#ifdef	XJP
	if(term->misc.jfew_open && screen->kmode != XJP_ROOT)
		sizehints.height += (screen->kcw_height + jp_att.bp);
#endif	/* XJP */
	if ((WidthValue&pr) || (HeightValue&pr)) 
	  sizehints.flags |= USSize;
	else sizehints.flags |= PSize;

	(void) XtMakeResizeRequest((Widget) term,
				   (Dimension)width, (Dimension)height,
				   &term->core.width, &term->core.height);

	/* XXX This is bogus.  We are parsing geometries too late.  This
	 * is information that the shell widget ought to have before we get
	 * realized, so that it can do the right thing.
	 */
        if (sizehints.flags & USPosition)
	    XMoveWindow (XtDisplay(term), term->core.parent->core.window,
			 sizehints.x, sizehints.y);

	XSetNormalHints(XtDisplay(term), term->core.parent->core.window,
		&sizehints);

#ifdef	XJP
	open_wnmo();
#endif	/* XJP */
        screen->fullVwin.fullwidth = width;
        screen->fullVwin.fullheight = height;
        screen->fullVwin.width = width - i;
        screen->fullVwin.height = height - j;

	if (term->misc.re_verse && (term->core.border_pixel == term->core.background_pixel))
		values->border_pixel = term->core.border_pixel = term->screen.foreground;
	
#ifdef STATUSLINE
	values->bit_gravity = ForgetGravity;
#else
	values->bit_gravity = NorthWestGravity;
#endif
#ifdef	SAVE_UNDER
	values->save_under = 1;
	values->backing_store = Always;
#endif	/* SAVE_UNDER */
	term->screen.fullVwin.window = term->core.window =
	  XCreateWindow(XtDisplay(term), XtWindow(term->core.parent),
		term->core.x, term->core.y,
#ifdef	XJP
		term->core.width, 
		(term->misc.jfew_open && screen->kmode != XJP_ROOT)?
		(term->core.height + screen->kcw_height + jp_att.bp):
		term->core.height, term->core.border_width,
#else	/* XJP */
		term->core.width, term->core.height, term->core.border_width,
#endif	/* XJP */
		(int) term->core.depth,
		InputOutput, CopyFromParent,	
#ifdef	SAVE_UNDER
		*valuemask|CWBitGravity|CWSaveUnder|CWBackingStore, values);
#else	/* SAVE_UNDER */
		*valuemask|CWBitGravity, values);
#endif	/* SAVE_UNDER */

	/* do the GC stuff */

	mask = VTgcFontMask | GCForeground | GCBackground 
	  	| GCGraphicsExposures | GCFunction;

	xgcv.graphics_exposures = TRUE;	/* default */
	xgcv.function = GXcopy;
	xgcv.font = screen->fnt_norm->fid;
	xgcv.foreground = screen->foreground;
	xgcv.background = term->core.background_pixel;

	screen->normalGC = XtGetGC((Widget)term, mask, &xgcv);
#ifdef	KANJI	/* sano */
	if (screen->fnt_r_norm == screen->fnt_norm) {
		screen->r_normalGC = screen->normalGC;
	} else {
		xgcv.font = screen->fnt_r_norm->fid;
		screen->r_normalGC = XtGetGC((Widget)term, mask, &xgcv) ;
	}
	if (screen->fnt_w_norm == screen->fnt_norm) {
		screen->w_normalGC = screen->normalGC;
	} else {
		xgcv.font = screen->fnt_w_norm->fid;
		screen->w_normalGC = XtGetGC((Widget)term, mask, &xgcv) ;
	}
#endif	KANJI

	if (screen->enbolden) { /* there is no bold font */
		xgcv.font = screen->fnt_norm->fid;
		screen->normalboldGC = screen->normalGC;
	} else {
		xgcv.font = screen->fnt_bold->fid;
		screen->normalboldGC = XtGetGC((Widget)term, mask, &xgcv);
	}
#ifdef	KANJI	/* sano */
	if (screen->r_enbolden) {	/* there is no bold kana font */
		xgcv.font = screen->fnt_r_norm->fid;
		screen->r_normalboldGC = screen->r_normalGC;
	} else {
		xgcv.font = screen->fnt_r_bold->fid;
		screen->r_normalboldGC = XtGetGC((Widget)term, mask, &xgcv);
	}
	if (screen->w_enbolden) {	/* there is no bold kanji font */
		xgcv.font = screen->fnt_w_norm->fid;
		screen->w_normalboldGC = screen->w_normalGC;
	} else {
		xgcv.font = screen->fnt_w_bold->fid;
		screen->w_normalboldGC = XtGetGC((Widget)term, mask, &xgcv);
	}
#endif	KANJI

	xgcv.font = screen->fnt_norm->fid;
	xgcv.foreground = term->core.background_pixel;
	xgcv.background = screen->foreground;

	screen->reverseGC = XtGetGC((Widget)term, mask, &xgcv);

#ifdef	KANJI	/* sano */
	if (screen->fnt_r_norm == screen->fnt_norm) {
		screen->r_reverseGC = screen->reverseGC;
	} else {
		xgcv.font = screen->fnt_r_norm->fid;
		screen->r_reverseGC = XtGetGC((Widget)term, mask, &xgcv);
	}
	if (screen->fnt_w_norm == screen->fnt_norm) {
		screen->w_reverseGC = screen->reverseGC;
	} else {
		xgcv.font = screen->fnt_w_norm->fid;
		screen->w_reverseGC = XtGetGC((Widget)term, mask, &xgcv);
	}
#endif	KANJI

	if (screen->enbolden) /* there is no bold font */
		xgcv.font = screen->fnt_norm->fid;
	else
		xgcv.font = screen->fnt_bold->fid;

	screen->reverseboldGC = XtGetGC((Widget)term, mask, &xgcv);

#ifdef	KANJI	/* sano */
	if (screen->r_enbolden) {	/* there is no bold kana font */
		xgcv.font = screen->fnt_r_norm->fid;
	} else {
		xgcv.font = screen->fnt_r_bold->fid;
	}
	screen->r_reverseboldGC = XtGetGC((Widget)term, mask, &xgcv);

	if (screen->w_enbolden) {	/* there is no bold kanji font */
		xgcv.font = screen->fnt_w_norm->fid;
	} else {
		xgcv.font = screen->fnt_w_bold->fid;
	}
	screen->w_reverseboldGC = XtGetGC((Widget)term, mask, &xgcv);
#endif	KANJI

	/* we also need a set of caret (called a cursor here) gc's */

	xgcv.font = screen->fnt_norm->fid;

	/*
	 * Let's see, there are three things that have "color":
	 *
	 *     background
	 *     text
	 *     cursorblock
	 *
	 * And, there are four situation when drawing a cursor, if we decide
	 * that we like have a solid block of cursor color with the letter
	 * that it is highlighting shown in the background color to make it
	 * stand out:
	 *
	 *     selected window, normal video - background on cursor
	 *     selected window, reverse video - foreground on cursor
	 *     unselected window, normal video - foreground on background
	 *     unselected window, reverse video - background on foreground
	 *
	 * Since the last two are really just normalGC and reverseGC, we only
	 * need two new GC's.  Under monochrome, we get the same effect as
	 * above by setting cursor color to foreground.
	 */

	{
	    unsigned long cc = screen->cursorcolor;
	    unsigned long fg = screen->foreground;
	    unsigned long bg = term->core.background_pixel;

	    if (cc != fg && cc != bg) {
		/* we have a colored cursor */
		xgcv.foreground = fg;
		xgcv.background = cc;
		screen->cursorGC = XtGetGC ((Widget) term, mask, &xgcv);

		if (screen->always_highlight) {
		    screen->reversecursorGC = (GC) 0;
		    screen->cursoroutlineGC = (GC) 0;
		} else {
		    xgcv.foreground = bg;
		    xgcv.background = cc;
		    screen->reversecursorGC = XtGetGC ((Widget) term, mask, &xgcv);
		    xgcv.foreground = cc;
		    xgcv.background = bg;
		    screen->cursoroutlineGC = XtGetGC ((Widget) term, mask, &xgcv);
		}
	    } else {
		screen->cursorGC = (GC) 0;
		screen->reversecursorGC = (GC) 0;
		screen->cursoroutlineGC = (GC) 0;
	    }
	}
#ifdef	KANJI	/* michael */
	xgcv.font = screen->fnt_r_norm->fid;
	{
	    unsigned long cc = screen->cursorcolor;
	    unsigned long fg = screen->foreground;
	    unsigned long bg = term->core.background_pixel;

	    if (cc != fg && cc != bg) {
		/* we have a colored cursor */
		xgcv.foreground = fg;
		xgcv.background = cc;
		screen->r_cursorGC = XtGetGC ((Widget) term, mask, &xgcv);

		xgcv.foreground = bg;
		xgcv.background = cc;
		screen->r_reversecursorGC = XtGetGC ((Widget) term, mask, &xgcv);
	    } else {
		screen->r_cursorGC = (GC) 0;
		screen->r_reversecursorGC = (GC) 0;
	    }
	}

	xgcv.font = screen->fnt_w_norm->fid;
	{
	    unsigned long cc = screen->cursorcolor;
	    unsigned long fg = screen->foreground;
	    unsigned long bg = term->core.background_pixel;

	    if (cc != fg && cc != bg) {
		/* we have a colored cursor */
		xgcv.foreground = fg;
		xgcv.background = cc;
		screen->w_cursorGC = XtGetGC ((Widget) term, mask, &xgcv);

		xgcv.foreground = bg;
		xgcv.background = cc;
		screen->w_reversecursorGC = XtGetGC ((Widget) term, mask, &xgcv);
	    } else {
		screen->w_cursorGC = (GC) 0;
		screen->w_reversecursorGC = (GC) 0;
	    }
	}
#endif	KANJI
	

	/* Reset variables used by ANSI emulation. */

	screen->gsets[0] = 'B';			/* ASCII_G		*/
#ifdef	KANJI	/* sano */
	screen->gsets[1] = 'I';
#else	KANJI
	screen->gsets[1] = 'B';
#endif	KANJI
	screen->gsets[2] = 'B';			/* DEC supplemental.	*/
	screen->gsets[3] = 'B';
	screen->curgl = 0;			/* G0 => GL.		*/
	screen->curgr = 2;			/* G2 => GR.		*/
	screen->curss = 0;			/* No single shift.	*/

	XDefineCursor(screen->display, VShellWindow, screen->pointer_cursor);

        screen->cur_col = screen->cur_row = 0;
	screen->max_col = Width(screen)  / screen->fullVwin.f_width - 1;
	screen->top_marg = 0;
	screen->bot_marg = screen->max_row = Height(screen) /
				screen->fullVwin.f_height - 1;

	screen->sc.row = screen->sc.col = screen->sc.flags = NULL;

	/* Mark screen buffer as unallocated.  We wait until the run loop so
	   that the child process does not fork and exec with all the dynamic
	   memory it will never use.  If we were to do it here, the
	   swap space for new process would be huge for huge savelines. */
	if (!tekWidget)			/* if not called after fork */
	  screen->buf = screen->allbuf = NULL;

	screen->do_wrap = NULL;
	screen->scrolls = screen->incopy = 0;
/*	free((char *)fInfo);	*/
	vp = &VTbox[1];
#ifdef R2SERVER
	(vp++)->x = FontWidth(screen) - 1;
	(vp++)->y = screen->fnt_norm->max_bounds.ascent +
		    screen->fnt_norm->max_bounds.descent - 1;
	(vp++)->x = -(FontWidth(screen) - 1);
	vp->y = -(screen->fnt_norm->max_bounds.ascent +
		    screen->fnt_norm->max_bounds.descent - 1);
#else
	(vp++)->x = FontWidth(screen) - 1;
	(vp++)->y = FontHeight(screen) - 1;
	(vp++)->x = -(FontWidth(screen) - 1);
	vp->y = -(FontHeight(screen) - 1);
#endif
	screen->box = VTbox;
#ifdef	KANJI	/* sano */
	vp = &VTwbox[1];
#ifdef R2SERVER
	(vp++)->x = FontWidth(screen) * 2 - 1;
	(vp++)->y = screen->fnt_norm->max_bounds.ascent +
		    screen->fnt_norm->max_bounds.descent - 1;
	(vp++)->x = -(FontWidth(screen) * 2 - 1);
	vp->y = -(screen->fnt_norm->max_bounds.ascent +
		    screen->fnt_norm->max_bounds.descent - 1);
#else
	(vp++)->x = FontWidth(screen) * 2 - 1;
	(vp++)->y = FontHeight(screen) - 1;
	(vp++)->x = -(FontWidth(screen) * 2 - 1);
	vp->y = -(FontHeight(screen) - 1);
#endif
	screen->w_box = VTwbox;
#endif	KANJI
#ifdef	STATUSLINE	/* michael */
	status_box[0].x = screen->border - 1;
#endif	STATUSLINE

	screen->savedlines = 0;

	if (term->misc.scrollbar) {
		screen->scrollbar = 0;
		ScrollBarOn (term, FALSE, TRUE);
	}
	CursorSave (term, &screen->sc);
	VTUnselect();
	return;
}

/*
 * Shows cursor at new cursor position in screen.
 */
ShowCursor()
{
	register TScreen *screen = &term->screen;
	register int x, y, flags;
#ifdef	KANJI	/* sano */
	register int offset = 0;
	unsigned char cc[2];
#else	KANJI
	char c;
#endif	KANJI
	GC	currentGC;
	Boolean	in_selection;

	if (eventMode != NORMAL) return;

#ifdef	STATUSLINE	/* michael */
	if (!screen->instatus &&
	    screen->cur_row - screen->topline > screen->max_row)
#else	STATUSLINE
	if (screen->cur_row - screen->topline > screen->max_row)
#endif	STATUSLINE
		return;
#ifdef	KANJI	/* sano */
	cc[0] = screen->buf[y = 2 * (screen->cursor_row = screen->cur_row)]
#else	KANJI
	c = screen->buf[y = 2 * (screen->cursor_row = screen->cur_row)]
#endif	KANJI
	 [x = screen->cursor_col = screen->cur_col];
	flags = screen->buf[y + 1][x];
#ifdef	KANJI	/* sano */
	if (cc[0] == 0) {
		cc[0] = ' ';
		flags &= ~(KCS|KANACS);
	} else if (flags & KCS) {
		if (flags & KCS1)
			cc[1] = screen->buf[y][x + 1];
		else {
			offset = -1;
			cc[1] = cc[0];
			cc[0] = screen->buf[y][x - 1];
		}
	}
#else	KANJI
	if (c == 0)
		c = ' ';
#endif	KANJI

	if (screen->cur_row > screen->endHRow ||
	    (screen->cur_row == screen->endHRow &&
	     screen->cur_col >= screen->endHCol) ||
	    screen->cur_row < screen->startHRow ||
	    (screen->cur_row == screen->startHRow &&
	     screen->cur_col < screen->startHCol))
	    in_selection = False;
	else
	    in_selection = True;

#ifdef	STATUSLINE	/* michael */
	if(screen->instatus && screen->reversestatus)
/*	if (screen->instatus)	*/
		flags ^= INVERSE;
#endif	STATUSLINE

	if(screen->select || screen->always_highlight) {
#ifdef	KANJI	/* sano & micheal */
		if (flags & KCS) {
			if (( (flags & INVERSE) && !in_selection) ||
			    (!(flags & INVERSE) &&  in_selection)){
			    /* text is reverse video */
				if (screen->w_cursorGC) {
					currentGC = screen->w_cursorGC;
				} else {
					if (flags & BOLD) {
						currentGC =
							screen->w_normalboldGC;
					} else {
						currentGC =
							screen->w_normalGC;
					}
				}
			} else {	/* normal video */
				if (screen->w_reversecursorGC) {
					currentGC = screen->w_reversecursorGC;
				} else {
					if (flags & BOLD) {
						currentGC =
							screen->w_reverseboldGC;
					} else {
						currentGC =
							screen->w_reverseGC;
					}
				}

			}
		} else if (flags & KANACS) {
			if (( (flags & INVERSE) && !in_selection) ||
			    (!(flags & INVERSE) &&  in_selection)){
			    /* text is reverse video */
				if (screen->r_cursorGC) {
					currentGC = screen->r_cursorGC;
				} else {
					if (flags & BOLD) {
						currentGC =
							screen->r_normalboldGC;
					} else {
						currentGC =
							screen->r_normalGC;
					}
				}
			} else {	/* normal video */
				if (screen->r_reversecursorGC) {
					currentGC = screen->r_reversecursorGC;
				} else {
					if (flags & BOLD) {
						currentGC =
							screen->r_reverseboldGC;
					} else {
						currentGC =
							screen->r_reverseGC;
					}
				}

			}
		} else
#endif	KANJI
		if (( (flags & INVERSE) && !in_selection) ||
		    (!(flags & INVERSE) &&  in_selection)){
		    /* text is reverse video */
		    if (screen->cursorGC) {
			currentGC = screen->cursorGC;
		    } else {
			if (flags & BOLD) {
				currentGC = screen->normalboldGC;
			} else {
				currentGC = screen->normalGC;
			}
		    }
		} else { /* normal video */
		    if (screen->reversecursorGC) {
			currentGC = screen->reversecursorGC;
		    } else {
			if (flags & BOLD) {
				currentGC = screen->reverseboldGC;
			} else {
				currentGC = screen->reverseGC;
			}
		    }
		}
	} else { /* not selected */
#ifdef	KANJI	/* sano */
		if (flags & KCS) {
			if (( (flags & INVERSE) && !in_selection) ||
			    (!(flags & INVERSE) &&  in_selection)) {
			    /* text is reverse video */
				if (screen->w_reversecursorGC) {
					currentGC = screen->w_reversecursorGC;
				} else {
					currentGC = screen->w_reverseGC;
				}
			} else {	/* normal video */
				if (screen->w_cursorGC) {
					currentGC = screen->w_cursorGC;
				} else {
					currentGC = screen->w_normalGC;
				}
			}
		} else if (flags & KANACS) {
			if (( (flags & INVERSE) && !in_selection) ||
			    (!(flags & INVERSE) &&  in_selection)) {
			    /* text is reverse video */
				if (screen->r_reversecursorGC) {
					currentGC = screen->r_reversecursorGC;
				} else {
					currentGC = screen->r_reverseGC;
				}
			} else {	/* normal video */
				if (screen->r_cursorGC) {
					currentGC = screen->r_cursorGC;
				} else {
					currentGC = screen->r_normalGC;
				}
			}
		} else
#endif	KANJI

/*
 * Bug fix by michael
 * Originials:
 *		if(flags & INVERSE) { /* is reverse video */
/*			currentGC = screen->reverseGC;
 *		} else { /* normal video */
/*			currentGC = screen->normalGC;
 *		}
 */
		if (( (flags & INVERSE) && !in_selection) ||
		    (!(flags & INVERSE) &&  in_selection)) {
		    /* text is reverse video */
			if (screen->reversecursorGC) {
				currentGC = screen->reversecursorGC;
			} else {
				currentGC = screen->reverseGC;
			}
		} else { /* normal video */
			if (screen->cursorGC) {
				currentGC = screen->cursorGC;
			} else {
				currentGC = screen->normalGC;
			}
		}
	    
	}


#ifdef	KANJI	/* sano */
	x = CursorX (screen, screen->cur_col + offset);
#else	KANJI
	x = CursorX (screen, screen->cur_col);
#endif	KANJI
	y = CursorY(screen, screen->cur_row) + 
	  screen->fnt_norm->ascent;
#ifdef	KANJI	/* sano */
#ifdef	SX9100
	if (!(term->flags & CINV) ){
#endif	/* SX9100 */
	if (flags & KCS) {
		/*
		 * If the cursor is on a Kanji character, then it should
		 * be drawn as the double sized cursor. If the screen is
		 * selected, the currentGC should be k_reverseGC or
		 * k_reverseboldGC, and then inversed cursor is obtained.
		 * If the screen is not selected, draw a box with w_box.
		 */
		XDrawImageString16(screen->display, TextWindow(screen),
			currentGC, x, y, (XChar2b *)cc, 1);
		if ((flags & BOLD) && screen->w_enbolden) /* no bold kanji font*/
			XDrawString16(screen->display, TextWindow(screen),
				currentGC, x + 1, y, (XChar2b *)cc, 1);
		if (flags & UNDERLINE)
			XDrawLine(screen->display, TextWindow(screen),
				currentGC, x, y + 1,
				x + FontWidth(screen) * 2, y + 1);
		if (!screen->select) {
			screen->w_box->x = x;
			screen->w_box->y = y - screen->fnt_norm->ascent;
			XDrawLines(screen->display, TextWindow(screen),
				currentGC, screen->w_box, NBOX,
				CoordModePrevious);
		}
	} else {
		XDrawImageString(screen->display, TextWindow(screen),
			currentGC, x, y, cc, 1);
		if (flags & KANACS) {
			if ((flags & BOLD) && screen->r_enbolden) /* no bold */
				XDrawString(screen->display, TextWindow(screen),
					currentGC, x + 1, y, cc, 1);
		} else {
			if ((flags & BOLD) && screen->enbolden) /* no bold */
				XDrawString(screen->display, TextWindow(screen),
					currentGC, x + 1, y, cc, 1);
		}
		if (flags & UNDERLINE)
			XDrawLine(screen->display, TextWindow(screen),
				currentGC, x, y + 1,
				x + FontWidth(screen), y + 1);
		if (!screen->select) {
			screen->box->x = x;
			screen->box->y = y - screen->fnt_norm->ascent;
			XDrawLines(screen->display, TextWindow(screen),
				currentGC, screen->box,
				NBOX, CoordModePrevious);
		}
	}
#ifdef	SX9100
	}
#endif	/* SX9100 */
#else	KANJI
	XDrawImageString(screen->display, TextWindow(screen), currentGC,
		x, y, &c, 1);

	if((flags & BOLD) && screen->enbolden) /* no bold font */
		XDrawString(screen->display, TextWindow(screen), currentGC,
			x + 1, y, &c, 1);
	if(flags & UNDERLINE) 
		XDrawLine(screen->display, TextWindow(screen), currentGC,
			x, y+1, x + FontWidth(screen), y+1);
	if (!screen->select && !screen->always_highlight) {
		screen->box->x = x;
		screen->box->y = y - screen->fnt_norm->ascent;
		XDrawLines (screen->display, TextWindow(screen), 
			    screen->cursoroutlineGC ? screen->cursoroutlineGC 
			    			    : currentGC,
			    screen->box, NBOX, CoordModePrevious);
	}
#endif	KANJI
	screen->cursor_state = ON;
}

/*
 * hide cursor at previous cursor position in screen.
 */
HideCursor()
{
	register TScreen *screen = &term->screen;
	GC	currentGC;
	register int x, y, flags;
#ifdef	STATUSLINE	/* michael */
	register int instatus;
#endif	STATUSLINE
#ifdef	KANJI	/* sano */
	register offset = 0;
	unsigned char cc[2];
#else	KANJI
	char c;
#endif	KANJI
	Boolean	in_selection;

#ifdef	STATUSLINE	/* michael */
	if(!(instatus = screen->cursor_row > screen->max_row) &&
	   screen->cursor_row - screen->topline > screen->max_row)
#else	STATUSLINE
	if(screen->cursor_row - screen->topline > screen->max_row)
#endif	STATUSLINE
		return;
#ifdef	KANJI	/* sano */
	cc[0] = screen->buf[y = 2 * screen->cursor_row][x = screen->cursor_col];
#else	KANJI
	c = screen->buf[y = 2 * screen->cursor_row][x = screen->cursor_col];
#endif	KANJI
	flags = screen->buf[y + 1][x];

	if (screen->cursor_row > screen->endHRow ||
	    (screen->cursor_row == screen->endHRow &&
	     screen->cursor_col >= screen->endHCol) ||
	    screen->cursor_row < screen->startHRow ||
	    (screen->cursor_row == screen->startHRow &&
	     screen->cursor_col < screen->startHCol))
	    in_selection = False;
	else
	    in_selection = True;

#ifdef	STATUSLINE	/* michael */
	if(screen->instatus && screen->reversestatus)
/*	if (screen->instatus)	*/
		flags ^= INVERSE;
#endif	STATUSLINE
#ifdef	KANJI	/* sano */
	if (flags & KCS) {
		if (( (flags & INVERSE) && !in_selection) ||
		    (!(flags & INVERSE) &&  in_selection)) {
			if (flags & BOLD) {
				currentGC = screen->w_reverseboldGC;
			} else {
				currentGC = screen->w_reverseGC;
			}
		} else {
			if (flags & BOLD) {
				currentGC = screen->w_normalboldGC;
			} else {
				currentGC = screen->w_normalGC;
			}
		}
	} else if (flags & KANACS) {
		if (( (flags & INVERSE) && !in_selection) ||
		    (!(flags & INVERSE) &&  in_selection)) {
			if (flags & BOLD) {
				currentGC = screen->r_reverseboldGC;
			} else {
				currentGC = screen->r_reverseGC;
			}
		} else {
			if (flags & BOLD) {
				currentGC = screen->r_normalboldGC;
			} else {
				currentGC = screen->r_normalGC;
			}
		}
	} else
#endif	KANJI
	if (( (flags & INVERSE) && !in_selection) ||
	    (!(flags & INVERSE) &&  in_selection)) {
		if(flags & BOLD) {
			currentGC = screen->reverseboldGC;
		} else {
			currentGC = screen->reverseGC;
		}
	} else {
		if(flags & BOLD) {
			currentGC = screen->normalboldGC;
		} else {
			currentGC = screen->normalGC;
		}
	}

#ifdef	KANJI	/* sano */
	if (cc[0] == 0) {
		cc[0] = ' ';
		flags &= ~(KCS|KANACS);
	} else if (flags & KCS) {
		if (flags & KCS1)
			cc[1] = screen->buf[y][x + 1];
		else {
			offset = -1;
			cc[1] = cc[0];
			cc[0] = screen->buf[y][x - 1];
		}
	}
#else 	KANJI
	if (c == 0)
		c = ' ';
#endif	KANJI
#ifdef	KANJI	/* sano */
	x = CursorX (screen, screen->cursor_col + offset);
#else	KANJI
	x = CursorX (screen, screen->cursor_col);
#endif	KANJI
#ifdef	STATUSLINE	/* michael */
	y = (instatus ? (screen->cursor_row * FontHeight(screen) + 1) :
 	     ((screen->cursor_row - screen->topline) * FontHeight(screen))) +
#else	STATUSLINE
	y = (((screen->cursor_row - screen->topline) * FontHeight(screen))) +
#endif	STATUSLINE
	 screen->border;
	y = y+screen->fnt_norm->ascent;
#ifdef	KANJI	/* sano */
#ifdef	SX9100
	if (!(term->flags & CINV) ){
#endif	/* SX9100 */
	if (flags & KCS) {
		XDrawImageString16(screen->display, TextWindow(screen),
			currentGC, x, y, (XChar2b *)cc, 1);
		if ((flags & BOLD) && screen->w_enbolden)
			XDrawString16(screen->display, TextWindow(screen),
				currentGC, x + 1, y, (XChar2b *)cc, 1);
		if (flags & UNDERLINE)
			XDrawLine(screen->display, TextWindow(screen),
				currentGC, x, y + 1,
				x + FontWidth(screen) * 2, y + 1);
	} else {
		XDrawImageString(screen->display, TextWindow(screen),
			currentGC, x, y, cc, 1);
		if (flags & KANACS) {
			if ((flags & BOLD) && screen->r_enbolden)
				XDrawString(screen->display, TextWindow(screen),
					currentGC, x + 1, y, cc, 1);
		} else {
			if ((flags & BOLD) && screen->enbolden)
				XDrawString(screen->display, TextWindow(screen),
					currentGC, x + 1, y, cc, 1);
		}
		if (flags & UNDERLINE)
			XDrawLine(screen->display, TextWindow(screen),
				currentGC, x, y + 1,
				x + FontWidth(screen), y + 1);
	}
#ifdef	SX9100
	}
#endif	/* SX9100 */
#else	KANJI
	XDrawImageString(screen->display, TextWindow(screen), currentGC,
		x, y, &c, 1);
	if((flags & BOLD) && screen->enbolden)
		XDrawString(screen->display, TextWindow(screen), currentGC,
			x + 1, y, &c, 1);
	if(flags & UNDERLINE) 
		XDrawLine(screen->display, TextWindow(screen), currentGC,
			x, y+1, x + FontWidth(screen), y+1);
#endif	KANJI
	screen->cursor_state = OFF;
}

VTSelect()
{
	register TScreen *screen = &term->screen;

	if (VShellWindow)
	  XSetWindowBorder (screen->display, VShellWindow,
			    term->core.border_pixel);
}

VTUnselect()
{
	register TScreen *screen = &term->screen;

	if (VShellWindow)
	  XSetWindowBorderPixmap (screen->display, VShellWindow,
				  screen->graybordertile);
}

VTReset(full)
int full;
{
	register TScreen *screen = &term->screen;

	/* reset scrolling region */
	screen->top_marg = 0;
	screen->bot_marg = screen->max_row;
	term->flags &= ~ORIGIN;
	if(full) {
		TabReset (term->tabs);
		term->keyboard.flags = NULL;
		screen->gsets[0] = 'B';
		screen->gsets[1] = 'B';
		screen->gsets[2] = 'B';
		screen->gsets[3] = 'B';
		screen->curgl = 0;
		screen->curgr = 2;
		screen->curss = 0;
		ClearScreen(screen);
		screen->cursor_state = OFF;
		if (term->flags & REVERSE_VIDEO)
			ReverseVideo(term);

		term->flags = term->initflags;
		if(screen->c132 && (term->flags & IN132COLUMNS)) {
		        Dimension junk;
			XtMakeResizeRequest(
			    (Widget) term,
			    (Dimension) 80*FontWidth(screen)
				+ 2 * screen->border + screen->scrollbar,
			    (Dimension) FontHeight(screen)
#ifdef	STATUSLINE	/* michael */
			        * (screen->max_row + 1) + screen->statusheight +
				2 * screen->border,
#else	STATUSLINE
			        * (screen->max_row + 1) + 2 * screen->border,
#endif	STATUSLINE
			    &junk, &junk);
			XSync(screen->display, FALSE);	/* synchronize */
			if(QLength(screen->display) > 0)
				xevents();
		}
		CursorSet(screen, 0, 0, term->flags);
#ifdef	SX9100
		set_jterm();
#endif	/* SX9100 */
	}
	longjmp(vtjmpbuf, 1);	/* force ground state in parser */
}

#ifdef	STATUSLINE	/* michael */
ToStatus(col)
int col;
{
	register TScreen *screen = &term->screen;

	if (screen->cursor_state)
		HideCursor();
	if (col > screen->max_col)
		col = screen->max_col;
	if (!screen->instatus) {
		if (!screen->statusline)
			ShowStatus();
		CursorSave(term, &screen->statussc);
		screen->instatus = TRUE;
		screen->cur_row = screen->max_row + 1;
	}
	screen->cur_col = col;
}

FromStatus()
{
	register TScreen *screen = &term->screen;

	if (!screen->instatus)
		return;
	screen->instatus = FALSE;
	CursorRestore(term, &screen->statussc);
}

ShowStatus()
{
	register TScreen *screen = &term->screen;
	register int border = 2 * screen->border;

	if (screen->statusline)
		return;
	screen->statusline = 1;
	screen->statusheight = FontHeight(screen) + 2;
	ResizeScreen(term, border + screen->scrollbar, border);
}

HideStatus()
{
	register TScreen *screen = &term->screen;
	register int border = 2 * screen->border;
	register int i, j;

	if (!screen->statusline)
		return;
	if (screen->instatus)
		FromStatus();
	screen->statusline = 0;
	screen->statusheight = 0;
	bzero(screen->buf[i = 2 * (screen->max_row + 1)],
		j = screen->max_col + 1);
	bzero(screen->buf[i + 1], j);
	ResizeScreen(term, border + screen->scrollbar, border);
}

EraseStatus()
{
	register TScreen *screen = &term->screen;
	register int i, j, pix;

	if (!screen->statusline)
		return;
	bzero(screen->buf[i = 2 * (screen->max_row + 1)],
		j = screen->max_col + 1);
	bzero(screen->buf[i + 1], j) ;
	XFillRectangle(screen->display, TextWindow(screen),
		screen->reversestatus ? screen->normalGC : screen->reverseGC,
		screen->border - 1 + screen->scrollbar,
		(screen->max_row + 1) * FontHeight(screen) +
		screen->border,
		j * FontWidth(screen) + 2, screen->statusheight);
	if (!screen->reversestatus)
		StatusBox(screen);
}

StatusBox(screen)
register TScreen *screen;
{
	status_box[0].x = screen->scrollbar;
	status_box[0].y = (screen->max_row + 1) * FontHeight(screen) +
		screen->border;
	status_box[3].x = -(status_box[1].x = (screen->max_col + 1) *
		FontWidth(screen) + screen->border + 1);
	status_box[4].y = -(status_box[2].y = FontHeight(screen) + 1);
	XDrawLines(screen->display, TextWindow(screen), screen->normalGC,
		status_box, NBOX, CoordModePrevious);
}
#endif	STATUSLINE


#ifdef MODEMENU
#define	MMENU_SCROLLBAR		0
#define	MMENU_SCROLL		(MMENU_SCROLLBAR+1)
#define	MMENU_VIDEO		(MMENU_SCROLL+1)
#define	MMENU_WRAP		(MMENU_VIDEO+1)
#define	MMENU_REVERSEWRAP	(MMENU_WRAP+1)
#define	MMENU_NLM		(MMENU_REVERSEWRAP+1)
#define	MMENU_CURSOR		(MMENU_NLM+1)
#define	MMENU_PAD		(MMENU_CURSOR+1)
#ifdef DO_AUTOREPEAT
#define	MMENU_REPEAT		(MMENU_PAD+1)
#define	MMENU_SCROLLKEY		(MMENU_REPEAT+1)
#else /* else not DO_AUTOREPEAT */
#define MMENU_SCROLLKEY		(MMENU_PAD+1)
#endif /* DO_AUTOREPEAT */
#define	MMENU_SCROLLINPUT	(MMENU_SCROLLKEY+1)
#ifdef	STATUSLINE	/* michael */
#define	MMENU_STATUS		(MMENU_SCROLLINPUT+1)
#define	MMENU_REVSTATUS		(MMENU_STATUS+1)
#define	MMENU_C132		(MMENU_REVSTATUS+1)
#else	STATUSLINE
#define	MMENU_C132		(MMENU_SCROLLINPUT+1)
#endif	STATUSLINE
#define	MMENU_CURSES		(MMENU_C132+1)
#define	MMENU_MARGBELL		(MMENU_CURSES+1)
#define	MMENU_TEKWIN		(MMENU_MARGBELL+1)
#define	MMENU_ALTERN		(MMENU_TEKWIN+1)
#ifdef	KANJI	/* sano */
#define	MMENU_EUC		(MMENU_ALTERN+1)
#define	MMENU_SJIS		(MMENU_EUC+1)
#ifdef	XJP
#define	MMENU_JINPUT		(MMENU_SJIS+1)
#define	MMENU_LINE		(MMENU_JINPUT+1)
#else	/* XJP */
#define	MMENU_LINE		(MMENU_SJIS+1)
#endif	/* XJP */
#else	KANJI
#define	MMENU_LINE		(MMENU_ALTERN+1)
#endif	KANJI
#define	MMENU_RESET		(MMENU_LINE+1)
#define	MMENU_FULLRESET		(MMENU_RESET+1)
#define	MMENU_TEKMODE		(MMENU_FULLRESET+1)
#define	MMENU_HIDEVT		(MMENU_TEKMODE+1)

static char *vtext[] = {
	"Scrollbar",
	"Jump Scroll",
	"Reverse Video",
	"Auto Wraparound",
	"Reverse Wraparound",
	"Auto Linefeed",
	"Application Cursor Mode",
	"Application Keypad Mode",
#ifdef DO_AUTOREPEAT
	"Auto Repeat",
#endif /* DO_AUTOREPEAT */
	"Scroll to bottom on key press",
	"Scroll to bottom on tty output",
#ifdef	STATUSLINE	/* michael */
	"Status Line",
	"Reverse Status Line",
#endif	STATUSLINE
	"Allow 80/132 switching",
	"Curses Emulation",
	"Margin Bell",
	"Tek Window Showing",
	"Alternate Screen",
#ifdef	KANJI	/* sano */
	"EUC Mode",
	"Shift-JIS Mode",
#ifdef	XJP
	"jinput",
#endif	/* XJP */
#endif	KANJI
	"-",
	"Soft Reset",
	"Full Reset",
	"Select Tek Mode",
	"Hide VT Window",
	0,
};


static int menutermflags;
static int menukbdflags;
static int t132;
static int taltern;
static int tcurses;
static int tmarginbell;
static int tscrollbar;
static int tscrollkey;
static int tscrollinput;
static Boolean tshow;
#ifdef	STATUSLINE	/* michael */
static int tstatusline;
static int trevstatus;
#endif	STATUSLINE
#ifdef	XJP
static	int kconvert;
#endif	/* XJP */

Menu *setupmenu(menu)
register Menu **menu;
{
	register TScreen *screen = &term->screen;
	register char **cp;
	register int flags = term->flags;
	register int kflags = term->keyboard.flags;

	if (*menu == NULL) {
		if ((*menu = NewMenu("Modes")) == NULL)
			return(NULL);
		for(cp = vtext ; *cp ; cp++)
			AddMenuItem(*menu, *cp);
		if(!(flags & SMOOTHSCROLL))
			CheckItem(*menu, MMENU_SCROLL);
		if(flags & REVERSE_VIDEO)
			CheckItem(*menu, MMENU_VIDEO);
		if(flags & WRAPAROUND)
			CheckItem(*menu, MMENU_WRAP);
		if(flags & REVERSEWRAP)
			CheckItem(*menu, MMENU_REVERSEWRAP);
		if(flags & LINEFEED)
			CheckItem(*menu, MMENU_NLM);
		if(kflags & CURSOR_APL)
			CheckItem(*menu, MMENU_CURSOR);
		if(kflags & KYPD_APL)
			CheckItem(*menu, MMENU_PAD);
#ifdef DO_AUTOREPEAT
		if(flags & AUTOREPEAT)
			CheckItem(*menu, MMENU_REPEAT);
#endif /* DO_AUTOREPEAT */
		if(tscrollbar = (screen->scrollbar > 0)) {
			CheckItem(*menu, MMENU_SCROLLBAR);
			if(tscrollkey = screen->scrollkey)
				CheckItem(*menu, MMENU_SCROLLKEY);
			if(tscrollinput = screen->scrollinput)
				CheckItem(*menu, MMENU_SCROLLINPUT);
		} else {
			tscrollkey = FALSE;
			DisableItem(*menu, MMENU_SCROLLKEY);
			tscrollinput = FALSE;
			DisableItem(*menu, MMENU_SCROLLINPUT);
		}
#ifdef	STATUSLINE	/* michael */
		if (tstatusline = screen->statusline) {
			CheckItem(*menu, MMENU_STATUS);
			if (trevstatus = screen->reversestatus)
				CheckItem(*menu, MMENU_REVSTATUS);
		} else {
			trevstatus = FALSE;
			DisableItem(*menu, MMENU_REVSTATUS);
		}
#endif	STATUSLINE
		if(t132 = screen->c132)
			CheckItem(*menu, MMENU_C132);
		if(tcurses = screen->curses)
			CheckItem(*menu, MMENU_CURSES);
		if(tmarginbell = screen->marginbell)
			CheckItem(*menu, MMENU_MARGBELL);
		if(tshow = screen->Tshow)
			CheckItem(*menu, MMENU_TEKWIN);
		else
			DisableItem(*menu, MMENU_HIDEVT);
#ifndef	SX9100
		DisableItem(*menu, MMENU_ALTERN);
#endif	/* SX9100 */
		if(taltern = screen->alternate) {
			CheckItem(*menu, MMENU_ALTERN);
		}
		DisableItem(*menu, MMENU_LINE);
		if(screen->inhibit & I_TEK) {
			DisableItem(*menu, MMENU_TEKWIN);
			DisableItem(*menu, MMENU_TEKMODE);
		}
#ifdef	KANJI	/* sano */
		if (flags & EUC_KANJI)
			CheckItem(*menu, MMENU_EUC);
		if (flags & SJIS_KANJI)
			CheckItem(*menu, MMENU_SJIS);
#ifdef	XJP
		if(kconvert = screen->kconvert)
		  CheckItem(*menu, MMENU_JINPUT);
#endif	/* XJP */
#endif	KANJI
		menutermflags = flags;
		menukbdflags = kflags;
		return(*menu);
	}
	menutermflags ^= flags;
	menukbdflags ^= kflags;
	if (menutermflags & SMOOTHSCROLL)
		SetItemCheck(*menu, MMENU_SCROLL, !(flags & SMOOTHSCROLL));
	if (menutermflags & REVERSE_VIDEO)
		SetItemCheck(*menu, MMENU_VIDEO, flags & REVERSE_VIDEO);
	if (menutermflags & WRAPAROUND)
		SetItemCheck(*menu, MMENU_WRAP, flags & WRAPAROUND);
	if (menutermflags & REVERSEWRAP)
		SetItemCheck(*menu, MMENU_REVERSEWRAP, flags & REVERSEWRAP);
	if (menutermflags & LINEFEED)
		SetItemCheck(*menu, MMENU_NLM, flags & LINEFEED);
	if (menukbdflags & CURSOR_APL)
		SetItemCheck(*menu, MMENU_CURSOR, kflags & CURSOR_APL);
	if (menukbdflags & KYPD_APL)
		SetItemCheck(*menu, MMENU_PAD, kflags & KYPD_APL);
#ifdef DO_AUTOREPEAT
        if (menutermflags & AUTOREPEAT)
                SetItemCheck(*menu, MMENU_REPEAT, flags & AUTOREPEAT);
#endif /* DO_AUTOREPEAT */
	if(tscrollbar != (screen->scrollbar > 0)) {
		SetItemCheck(*menu, MMENU_SCROLLBAR, (tscrollbar =
		 (screen->scrollbar > 0)));
		SetItemDisable(*menu, MMENU_SCROLLKEY, !tscrollbar);
		SetItemCheck(*menu, MMENU_SCROLLKEY,
		 tscrollkey = (tscrollbar && screen->scrollkey));
		SetItemDisable(*menu, MMENU_SCROLLINPUT, !tscrollbar);
		SetItemCheck(*menu, MMENU_SCROLLINPUT,
		 tscrollinput = (tscrollbar && screen->scrollinput));
	} else if (tscrollbar) {
		if (tscrollkey != screen->scrollkey)
			SetItemCheck(*menu, MMENU_SCROLLKEY,
			 tscrollkey = screen->scrollkey);
		if (tscrollinput != screen->scrollinput)
			SetItemCheck(*menu, MMENU_SCROLLINPUT,
			 tscrollinput = screen->scrollinput);
	}
#ifdef	STATUSLINE	/* michael */
	if (tstatusline != screen->statusline) {
		SetItemCheck(*menu, MMENU_STATUS,
			tstatusline = screen->statusline);
		SetItemDisable(*menu, MMENU_REVSTATUS, !tstatusline);
		SetItemCheck(*menu, MMENU_REVSTATUS,
			trevstatus = (tstatusline && screen->reversestatus));
	} else if (tstatusline) {
		if (tstatusline != screen->reversestatus)
			SetItemCheck(*menu, MMENU_REVSTATUS,
				trevstatus = screen->reversestatus);
	}
#endif	STATUSLINE
	if(t132 != screen->c132)
		SetItemCheck(*menu, MMENU_C132, (t132 = screen->c132));
	if(tcurses != screen->curses)
		SetItemCheck(*menu, MMENU_CURSES, (tcurses = screen->curses));
	if(tmarginbell != screen->marginbell)
		SetItemCheck(*menu, MMENU_MARGBELL, (tmarginbell =
		screen->marginbell));
	if(tshow != screen->Tshow) {
		SetItemCheck(*menu, MMENU_TEKWIN, (tshow = screen->Tshow));
		SetItemDisable(*menu, MMENU_HIDEVT, !tshow);
	}
	if(taltern != screen->alternate) {
		SetItemCheck(*menu, MMENU_ALTERN, (taltern =
		 screen->alternate));
	}
#ifdef	KANJI	/* sano */
	if (menutermflags & EUC_KANJI)
		SetItemCheck(*menu, MMENU_EUC, flags & EUC_KANJI);
	if (menutermflags & SJIS_KANJI)
		SetItemCheck(*menu, MMENU_SJIS, flags & SJIS_KANJI);
#ifdef	XJP
	if (kconvert !=  screen->kconvert)
		SetItemCheck(*menu, MMENU_JINPUT, (kconvert = screen->kconvert));
#endif	/* XJP */
#endif	KANJI
	menutermflags = flags;
	menukbdflags = kflags;
	return(*menu);
}

domenufunc(item)
int item;
{
	register TScreen *screen = &term->screen;

	switch (item) {
	case MMENU_SCROLL:
		term->flags ^= SMOOTHSCROLL;
		if (term->flags & SMOOTHSCROLL) {
			screen->jumpscroll = FALSE;
			if (screen->scroll_amt)
				FlushScroll(screen);
		} else
			screen->jumpscroll = TRUE;
		break;

	case MMENU_VIDEO:
#ifdef	SX9100
		term->flags ^= REVERSE_VIDEO;
#endif	/* SX9100 */
		ReverseVideo(term);
		break;

	case MMENU_WRAP:
		term->flags ^= WRAPAROUND;
		break;

	case MMENU_REVERSEWRAP:
		term->flags ^= REVERSEWRAP;
		break;

	case MMENU_NLM:
		term->flags ^= LINEFEED;
		break;

	case MMENU_CURSOR:
		term->keyboard.flags ^= CURSOR_APL;
		break;

	case MMENU_PAD:
		term->keyboard.flags ^= KYPD_APL;
		break;

#ifdef DO_AUTOREPEAT
	case MMENU_REPEAT:
		term->flags ^= AUTOREPEAT;
		if (term->flags & AUTOREPEAT)
			XAutoRepeatOn(screen->display);
		else
			XAutoRepeatOff(screen->display);
		break;
#endif /* DO_AUTOREPEAT */

	case MMENU_SCROLLBAR:
		if(screen->scrollbar)
			ScrollBarOff(screen);
		else
			ScrollBarOn (term, FALSE, FALSE);
		break;

	case MMENU_SCROLLKEY:
		screen->scrollkey = !screen->scrollkey;
		break;

	case MMENU_SCROLLINPUT:
		screen->scrollinput = !screen->scrollinput;
		break;

#ifdef	STATUSLINE	/* michael */
	case MMENU_STATUS:
		if (screen->statusline)
			HideStatus();
		else
			ShowStatus();
		break;

	case MMENU_REVSTATUS:
		screen->reversestatus = !screen->reversestatus;
		ScrnRefresh(screen, screen->max_row + 1, 0, 1,
			screen->max_col + 1);
		break;
#endif	STATUSLINE
		
	case MMENU_C132:
		screen->c132 = !screen->c132;
		break;

	case MMENU_MARGBELL:
		if(!(screen->marginbell = !screen->marginbell))
			screen->bellarmed = -1;
		break;
#ifdef	SX9100
	case MMENU_ALTERN:
		if(screen->alternate)
			FromAlternate(screen);
		else
			ToAlternate(screen);
		break;
#endif	/* SX19100 */

	case MMENU_CURSES:
		screen->curses = !screen->curses;
		break;

	case MMENU_FULLRESET:
		VTReset(TRUE);
		break;

	case MMENU_RESET:
		VTReset(FALSE);
		break;

	case MMENU_HIDEVT:
		set_vt_visibility (FALSE);
		reselectwindow(screen);
		/* drop through */

	case MMENU_TEKMODE:
		end_vt_mode ();
		break;

	case MMENU_TEKWIN:
		if (!screen->Tshow) {
		    set_tek_visibility (TRUE);
		} else {
		    set_tek_visibility (FALSE);
		    end_tek_mode ();
		}
		reselectwindow(screen);
		break;
#ifdef	KANJI	/* sano */
	case MMENU_EUC:
		term->flags ^= EUC_KANJI;
		if (term->flags & EUC_KANJI)
			/*
			 * Reset SJIS_KANJI mode because
			 * EUC_KANJI and SJIS_KANJI are specified exclusively.
			 */
			term->flags &= ~SJIS_KANJI;
#ifdef	SX9100
		set_jterm();
#endif	/* SX9100 */
		break;
	case MMENU_SJIS:
		term->flags ^= SJIS_KANJI;
		if (term->flags & SJIS_KANJI)
			term->flags &= ~EUC_KANJI;
#ifdef	SX9100
		set_jterm();
#endif	/* SX9100 */
		break;
#ifdef	XJP
	case MMENU_JINPUT:
		if (!screen->kconvert) {
			open_wnmo();
			set_jinput();
		} else {
			unset_jinput();
		}
		break;
#endif	/* XJP */
#endif	KANJI
	}
}
#endif	/* MODEMENU */

/*
 * set_character_class - takes a string of the form
 * 
 *                 low[-high]:val[,low[-high]:val[...]]
 * 
 * and sets the indicated ranges to the indicated values.
 */

int set_character_class (s)
    register char *s;
{
    register int i;			/* iterator, index into s */
    int len;				/* length of s */
    int acc;				/* accumulator */
    int low, high;			/* bounds of range [0..127] */
    int base;				/* 8, 10, 16 (octal, decimal, hex) */
    int numbers;			/* count of numbers per range */
    int digits;				/* count of digits in a number */
    static char *errfmt = "%s:  %s in range string \"%s\" (position %d)\n";
    extern char *ProgramName;

    if (!s || !s[0]) return;

    base = 10;				/* in case we ever add octal, hex */
    low = high = -1;			/* out of range */

    for (i = 0, len = strlen (s), acc = 0, numbers = digits = 0;
	 i < len; i++) {
	char c = s[i];

	if (isspace(c)) {
	    continue;
	} else if (isdigit(c)) {
	    acc = acc * base + (c - '0');
	    digits++;
	    continue;
	} else if (c == '-') {
	    low = acc;
	    acc = 0;
	    if (digits == 0) {
		fprintf (stderr, errfmt, ProgramName, "missing number", s, i);
		return (-1);
	    }
	    digits = 0;
	    numbers++;
	    continue;
	} else if (c == ':') {
	    if (numbers == 0)
	      low = acc;
	    else if (numbers == 1)
	      high = acc;
	    else {
		fprintf (stderr, errfmt, ProgramName, "too many numbers",
			 s, i);
		return (-1);
	    }
	    digits = 0;
	    numbers++;
	    acc = 0;
	    continue;
	} else if (c == ',') {
	    /*
	     * now, process it
	     */

	    if (high < 0) {
		high = low;
		numbers++;
	    }
	    if (numbers != 2) {
		fprintf (stderr, errfmt, ProgramName, "bad value number", 
			 s, i);
	    } else if (SetCharacterClassRange (low, high, acc) != 0) {
		fprintf (stderr, errfmt, ProgramName, "bad range", s, i);
	    }

	    low = high = -1;
	    acc = 0;
	    digits = 0;
	    numbers = 0;
	    continue;
	} else {
	    fprintf (stderr, errfmt, ProgramName, "bad character", s, i);
	    return (-1);
	}				/* end if else if ... else */

    }

    if (low < 0 && high < 0) return (0);

    /*
     * now, process it
     */

    if (high < 0) high = low;
    if (numbers < 1 || numbers > 2) {
	fprintf (stderr, errfmt, ProgramName, "bad value number", s, i);
    } else if (SetCharacterClassRange (low, high, acc) != 0) {
	fprintf (stderr, errfmt, ProgramName, "bad range", s, i);
    }

    return (0);
}

/* ARGSUSED */
static void HandleKeymapChange(w, event, params, param_count)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *param_count;
{
    static XtTranslations keymap, original;
    static XtResource resources[] = {
	{ XtNtranslations, XtCTranslations, XtRTranslationTable,
	      sizeof(XtTranslations), 0, XtRTranslationTable, (caddr_t)NULL}
    };
    char mapName[1000];
    char mapClass[1000];

    if (*param_count != 1) return;

    if (original == NULL) original = w->core.tm.translations;

    if (strcmp(params[0], "None") == 0) {
	XtOverrideTranslations(w, original);
	return;
    }
    (void) sprintf( mapName, "%sKeymap", params[0] );
    (void) strcpy( mapClass, mapName );
    if (islower(mapClass[0])) mapClass[0] = toupper(mapClass[0]);
    XtGetSubresources( w, &keymap, mapName, mapClass,
		       resources, (Cardinal)1, NULL, (Cardinal)0 );
    if (keymap != NULL)
	XtOverrideTranslations(w, keymap);
}


/* ARGSUSED */
static void HandleBell(w, event, params, param_count)
    Widget w;
    XEvent *event;		/* unused */
    String *params;		/* [0] = volume */
    Cardinal *param_count;	/* 0 or 1 */
{
    int percent = (*param_count) ? atoi(params[0]) : 0;

    XBell( XtDisplay(w), percent );
}


/* ARGSUSED */
static void HandleIgnore(w, event, params, param_count)
    Widget w;
    XEvent *event;		/* unused */
    String *params;		/* unused */
    Cardinal *param_count;	/* unused */
{
    /* do nothing, but check for funny escape sequences */
    (void) SendMousePosition(w, event);
}
