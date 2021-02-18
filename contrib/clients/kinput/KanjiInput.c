/*
 *	KanjiInput.c -- $B4A;zF~NOMQ(J Widget
 *		ishisone@sra.co.jp
 */

/*
 * Copyright (C) 1988 by Software Research Associates, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Software Research
 * Associates makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * Author:  Makoto Ishisone, Software Research Associates, Inc., Japan
 */

#ifndef lint
static char *rcsid = "$Header: /private/usr/mmb/ishisone/job/x11/kinput/RCS/KanjiInput.c,v 3.1 89/10/03 13:41:22 ishisone Rel $";
#endif

#include <stdio.h>
#include <X11/Xos.h>

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#ifdef CHECK_PROTOTYPE
#include <X11/Xlib.p>
#include <X11/Intrinsic.p>
#endif
#include <X11/CoreP.h>
#include <wnnerror.h>
#include <jslib.h>
#include "jclib.h"
#include "jilib.h"
#include "cconv.h"
#ifdef HEADER_IS_HERE
#include "WStr.h"
#include "XWStr.h"
#else
#include <WStr.h>
#include <X11/XWStr.h>
#endif
#include "KanjiInput.h"
#include "KanjiInpuP.h"
#include "KISub.h"

#include <X11/Xutil.h>  /* This include depends on something above it */

#ifdef DEBUG
#define PRINT(a)	errprint a
#else
#define PRINT(a)
#endif

#define warn(r)	{ if ((r) < 0) beep(w); }

/* define HAVE_ALLOCA if you have reliable alloca() */
#ifdef __GNUC__
#define LOCAL_ALLOC(x)	__builtin_alloca(x)
#define LOCAL_FREE(x)
#else
#ifdef HAVE_ALLOCA
#ifdef INCLUDE_ALLOCA_H
#include <alloca.h>
#endif
#define LOCAL_ALLOC(x)	alloca(x)
#define LOCAL_FREE(x)
#else
#define LOCAL_ALLOC(x)	malloc(x)
#define LOCAL_FREE(x)	free(x)
#endif /* HAVE_ALLOCA */
#endif /* __GNUC__ */

#define min(x,y)	((x) < (y) ? (x) : (y))
#define max(x,y)	((x) > (y) ? (x) : (y))

/* $B2hLL99?7$N%R%s%H(J */
#define NOTCHANGED	0
#define REDRAW		10

/* $B%b!<%I(J */
#define MODE_NORMAL		0
#define MODE_SELECTION_S	1
#define MODE_SELECTION_L	2
#define MODE_SYMBOL		3

/* $B8uJdA*Br(J */
#define SEL_LEFT	0
#define SEL_RIGHT	1
#define SEL_NEXT	2
#define SEL_PREV	3
#define SEL_TOP		4
#define SEL_BOTTOM	5
#define INC		0
#define DEC		1
#define RESIZE		100

#define StatusHeight(w)	((w)->kinput.fontheight + 2)
#define ModeWidth(w)	((w)->kinput.modewidth)
#define StatusWidth(w)	((w)->core.width - (w)->kinput.modewidth - 3)
#define UpdateHint(w)	((w)->kinput.updatehint)

/* Resources for KanjiInput */

static XtResource resources[] = {
/* Resources from other classes with new defaults */
	{XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
	     XtOffset(KanjiInputWidget, core.width), XtRString, "400"},
	{XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	     XtOffset(KanjiInputWidget, kinput.foreground), XtRString, "Black"},

/* New resources for KanjiInput */
	{XtNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
	     XtOffset(KanjiInputWidget, kinput.G0font),
	     XtRString, "fixed"},
	{XtNkanjiFont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
	     XtOffset(KanjiInputWidget, kinput.G1font),
	     XtRString, "k14"},
	{XtNkanaFont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
	     XtOffset(KanjiInputWidget, kinput.G2font),
	     XtRString, NULL},
	{XtNmodeWidth, XtCModeWidth, XtRInt, sizeof(int),
	     XtOffset(KanjiInputWidget, kinput.modewidth),
	     XtRString, "-1"},
	{XtNendNotify, XtCCallback, XtRCallback, sizeof(caddr_t), 
	     XtOffset(KanjiInputWidget, kinput.endcallback),
	     XtRCallback, (caddr_t)NULL},
	{XtNfixNotify, XtCCallback, XtRCallback, sizeof(caddr_t), 
	     XtOffset(KanjiInputWidget, kinput.fixcallback),
	     XtRCallback, (caddr_t)NULL},
	{XtNconfirmFunc, XtCFunction, XtRPointer, sizeof(int (*)()), 
	     XtOffset(KanjiInputWidget, kinput.confirmfunc),
	     XtRPointer, (caddr_t)NULL},
	{XtNleftMargin, XtCMargin, XtRDimension, sizeof(Dimension), 
	     XtOffset(KanjiInputWidget, kinput.leftmargin),
	     XtRString, "2"},
	{XtNrightMargin, XtCMargin, XtRDimension, sizeof(Dimension), 
	     XtOffset(KanjiInputWidget, kinput.rightmargin),
	     XtRString, "2"},
	{XtNtopMargin, XtCMargin, XtRDimension, sizeof(Dimension), 
	     XtOffset(KanjiInputWidget, kinput.topmargin),
	     XtRString, "1"},

/* following resrouces can't be changed by XtSetValues() */
	{XtNnlines, XtCLines, XtRInt, sizeof(int),
	     XtOffset(KanjiInputWidget, kinput.nlines),
	     XtRString, "2"},
	{ XtNjserver, XtCJserver, XtRString, sizeof(String),
	     XtOffset(KanjiInputWidget, kinput.jservername),
	     XtRString, NULL },
	{ XtNwnnEnvname, XtCWnnEnvname, XtRString, sizeof(String),
	     XtOffset(KanjiInputWidget, kinput.wnnenvname),
	     XtRString, NULL },
	{ XtNwnnEnvrc, XtCWnnEnvrc, XtRString, sizeof(String),
	     XtOffset(KanjiInputWidget, kinput.wnnenvrcfile),
	     XtRString, NULL },
	{ XtNwnnOverrideEnv, XtCWnnOverrideEnv, XtRBoolean, sizeof(Boolean),
	     XtOffset(KanjiInputWidget, kinput.wnnoverrideenv),
	     XtRString, "false" },
	{ XtNccdef, XtCCcdef, XtRString, sizeof(String),
	     XtOffset(KanjiInputWidget, kinput.ccdeffile),
	     XtRString, NULL },
	{XtNwnnEnv, XtCWnnEnv, XtRWnnEnv, sizeof(struct wnn_env *),
	     XtOffset(KanjiInputWidget, kinput.wnnenv),
	     XtRWnnEnv, NULL},
	{XtNccRule, XtCCcRule, XtRCcRule, sizeof(ccRule),
	     XtOffset(KanjiInputWidget, kinput.ccrule),
	     XtRCcRule, NULL},
};


static void Initialize();
static void Realize();
static void SubwindowRealize();
static void Destroy();
static void Resize();
static void Redraw();
static void SubwindowRedraw();
static Boolean SetValues();

/* function prototype */
#if __STDC__ == 1
static void NewFont(KanjiInputWidget);
static void Layout(KanjiInputWidget);
static void decodekey(KanjiInputWidget, XKeyEvent *);
static void funcDispatch(int, unsigned char *, int, Widget);
static void defAction(unsigned char *, int, Widget);
static void insChar(int, caddr_t);
static void delChar(caddr_t);
static void beginSelection(KanjiInputWidget, int);
static void moveSelection(KanjiInputWidget, int);
static void endSelection(KanjiInputWidget, int);
static void beginSymbol(KanjiInputWidget);
static void moveSymbol(KanjiInputWidget, int);
static void endSymbol(KanjiInputWidget, int);
static void RedrawMode(KanjiInputWidget);
static void RedrawText(KanjiInputWidget);
static void RedrawCandidates(KanjiInputWidget);
static void RedrawSymbol(KanjiInputWidget);
static void UpdateText(KanjiInputWidget);
static void scrupdate(KanjiInputWidget);
static void reverseArea(KanjiInputWidget, int, int, int, int);
static void stippleArea(KanjiInputWidget, int, int, int, int);
static int charwidth(XWSGC, int, int);
static int lbearing(XWSGC, int);
static void quitfep(void);
static int KITextWidth(XWSGC, wchar_t *, int, int);
static int KIDrawString(Display *, Drawable, XWSGC, int, int, wchar_t *, int, int);
static void UpdateRec(KanjiInputWidget);
static void CopyDisplayBuf(KanjiInputWidget);
static void createenverror(int, char *, caddr_t);
static int createConfirm(int, char *, caddr_t);
#else
static void NewFont();
static void Layout();
static void decodekey();
static void funcDispatch();
static void defAction();
static void insChar();
static void delChar();
static void beginSelection();
static void moveSelection();
static void endSelection();
static void beginSymbol();
static void moveSymbol();
static void endSymbol();
static void RedrawMode();
static void RedrawText();
static void RedrawCandidates();
static void RedrawSymbol();
static void UpdateText();
static void scrupdate();
static void reverseArea();
static void stippleArea();
static int charwidth();
static int lbearing();
static void quitfep();
static int KITextWidth();
static int KIDrawString();
static void UpdateRec();
static void CopyDisplayBuf();
static void createenverror();
static int createConfirm();
#endif

static void KeyEventProc();
static void Focusin();
static void Focusout();
static void Enter();
static void Leave();

/* KanjiInput Action Table */
XtActionsRec kanjiInputActionsTable [] = {
	{"focus-in",	Focusin},
	{"focus-out",	Focusout},
	{"enter",	Enter},
	{"leave",	Leave},
	{"key-event",	KeyEventProc},
};

/* KanjiInput Default Event Translations */
char defaultKanjiInputTranslations[] =  
	"<FocusIn>:	focus-in()\n\
	<FocusOut>:	focus-out()\n\
	<Key>:	key-event()\n\
	Shift<Key>:	key-event()\n\
	Ctrl<Key>:	key-event()\n\
	Meta<Key>:	key-event()\n\
	<EnterWindow>:	enter()\n\
	<LeaveWindow>:	leave()";

/*  KanjiInput Class Record */
KanjiInputClassRec kanjiInputClassRec = {
  {
    /* core fields */
    /* superclass       */      (WidgetClass) &widgetClassRec,
    /* class_name       */      "KanjiInput",
    /* widget_size      */      sizeof(KanjiInputRec),
    /* class_initialize */      NULL,
    /* class_part_init  */      NULL,
    /* class_inited     */      FALSE,
    /* initialize       */      Initialize,
    /* initialize_hook  */      NULL,
    /* realize          */      Realize,
    /* actions          */      kanjiInputActionsTable,
    /* num_actions      */      XtNumber(kanjiInputActionsTable),
    /* resources        */      resources,
    /* num_ resource    */      XtNumber(resources),
    /* xrm_class        */      NULLQUARK,
    /* compress_motion  */      TRUE,
    /* compress_exposure*/      TRUE,
    /* compress_enterleave*/    TRUE,
    /* visible_interest */      FALSE,
    /* destroy          */      Destroy,
    /* resize           */      Resize,
    /* expose           */      Redraw,
    /* set_values       */      SetValues,
    /* set_values_hook  */      NULL,
    /* set_values_almost*/      XtInheritSetValuesAlmost,
    /* get_values_hook  */      NULL,
    /* accept_focus     */      NULL,
    /* version          */      XtVersion,
    /* callback_private */      NULL,
    /* tm_table         */      defaultKanjiInputTranslations,
    /* query_geometry	*/	NULL,
    /* display_accelerator */	XtInheritDisplayAccelerator,
    /* extension	*/	NULL
  },
  {
    /* KanjiInput fields */
    /* empty            */	NULL
  }
};

WidgetClass	kanjiInputWidgetClass = (WidgetClass)&kanjiInputClassRec;

static char	*fepfunctbl[] = {
	"convert",
	"convert-s",
	"unconvert",
	"next",	
	"next-s",
	"previous",
	"previous-s",
	"forward",
	"backward",
	"move-top",
	"move-bottom",
	"clear",
	"expand",
	"expand-s",
	"shrink",
	"shrink-s",
	"expand-noconv",
	"expand-noconv-s",
	"shrink-noconv",
	"shrink-noconv-s",
	"fix",
	"fix2",
	"to-hankaku",
	"to-zenkaku",
	"to-hiragana",
	"to-katakana",
	"backspace",
	"delete",
	"carriage-return",
	"fix-and-write",
	"beep",
	"jiscode-begin",
	"jiscode-end",
	"symbol-input",
	"inspect",
	"end-conversion",
};
#define FTSIZE	(sizeof(fepfunctbl) / sizeof(char *))

static void	convert();
static void	convert_small();
static void	unconvert();
static void	select_next();
static void	select_next_small();
static void	select_prev();
static void	select_prev_small();
static void	move_forward();
static void	move_backward();
static void	move_top();
static void	move_bottom();
static void	clear_buffer();
static void	symbol_top();
static void	symbol_bottom();
static void	symbol_forward();
static void	symbol_backward();
static void	symbol_next();
static void	symbol_prev();
static void	symbol_select();
static void	symbol_abort();
static void	selection_top();
static void	selection_bottom();
static void	selection_forward();
static void	selection_backward();
static void	selection_next();
static void	selection_prev();
static void	selection_select();
static void	expand_clause();
static void	expand_clause_small();
static void	shrink_clause();
static void	shrink_clause_small();
static void	expand_clause2();
static void	expand_clause2_small();
static void	shrink_clause2();
static void	shrink_clause2_small();
static void	fix();
static void	hankaku();
static void	zenkaku();
static void	hiragana();
static void	katakana();
static void	backspace();
static void	delete();
static void	fixwrite();
static void	beep();
static void	carriageret();
static void	jiscode_begin();
static void	jiscode_end();
static void	symbol_input();
static void	inspect();
static void	convend();

static void	(*func_normal[])() = {
/* convert */		convert,
/* convert-s */		convert_small,
/* unconvert */		unconvert,
/* next */		select_next,
/* next-s */		select_next_small,
/* previous */		select_prev,
/* previous-s */	select_prev_small,
/* forward */		move_forward,
/* backward */		move_backward,
/* move-top */		move_top,
/* move-bottom */	move_bottom,
/* clear */		clear_buffer,
/* expand */		expand_clause,
/* expand-s */		expand_clause_small,
/* shrink */		shrink_clause,
/* shrink-s */		shrink_clause_small,
/* expand-noconv */	expand_clause2,
/* expand-noconv-s */	expand_clause2_small,
/* shrink-noconv */	shrink_clause2,
/* shrink-noconv-s */	shrink_clause2_small,
/* fix */		fix,
/* fix2 */		fix,
/* to-hankaku */	hankaku,
/* to-zenkaku */	zenkaku,
/* to-hiragana */	hiragana,
/* to-katakana */	katakana,
/* backspace */		backspace,
/* delete */		delete,
/* carriage-return */	carriageret,
/* fix-and-write */	fixwrite,
/* beep */		beep,
/* jiscode-begin */	jiscode_begin,
/* jiscode-end */	jiscode_end,
/* symbol-input */	symbol_input,
/* inspect */		inspect,
/* end-conversion */	convend,
};

static void	(*func_selection[])() = {
/* convert */		selection_forward,
/* convert-s */		selection_forward,
/* unconvert */		beep,
/* next */		selection_next,
/* next-s */		selection_next,
/* previous */		selection_prev,
/* previous-s */	selection_prev,
/* forward */		selection_forward,
/* backward */		selection_backward,
/* move-top */		selection_top,
/* move-bottom */	selection_bottom,
/* clear */		clear_buffer,
/* expand */		expand_clause,
/* expand-s */		expand_clause_small,
/* shrink */		shrink_clause,
/* shrink-s */		shrink_clause_small,
/* expand-noconv */	expand_clause2,
/* expand-noconv-s */	expand_clause2_small,
/* shrink-noconv */	shrink_clause2,
/* shrink-noconv-s */	shrink_clause2_small,
/* fix */		fix,
/* fix2 */		fix,
/* to-hankaku */	hankaku,
/* to-zenkaku */	zenkaku,
/* to-hiragana */	hiragana,
/* to-katakana */	katakana,
/* backspace */		backspace,
/* delete */		delete,
/* carriage-return */	selection_select,
/* fix-and-write */	beep,
/* beep */		beep,
/* jiscode-begin */	beep,
/* jiscode-end */	beep,
/* symbol-input */	beep,
/* inspect */		beep,
/* end-conversion */	convend,
};

static void	(*func_symbol[])() = {
/* convert */		beep,
/* convert-s */		beep,
/* unconvert */		beep,
/* next */		symbol_next,
/* next-s */		symbol_next,
/* previous */		symbol_prev,
/* previous-s */	symbol_prev,
/* forward */		symbol_forward,
/* backward */		symbol_backward,
/* move-top */		symbol_top,
/* move-bottom */	symbol_bottom,
/* clear */		clear_buffer,
/* expand */		beep,
/* expand-s */		beep,
/* shrink */		beep,
/* shrink-s */		beep,
/* expand-noconv */	beep,
/* expand-noconv-s */	beep,
/* shrink-noconv */	beep,
/* shrink-noconv-s */	beep,
/* fix */		fix,
/* fix2 */		fix,
/* to-hankaku */	beep,
/* to-zenkaku */	beep,
/* to-hiragana */	beep,
/* to-katakana */	beep,
/* backspace */		backspace,
/* delete */		delete,
/* carriage-return */	symbol_select,
/* fix-and-write */	beep,
/* beep */		beep,
/* jiscode-begin */	beep,
/* jiscode-end */	beep,
/* symbol-input */	symbol_abort,
/* inspect */		beep,
/* end-conversion */	convend,
};


/*
 * $BJQ49%F!<%V%k(J
 */

/* $BA43Q$R$i$,$J(J/$B%+%?%+%J(J <-> $BH>3Q%+%JJQ49MQ%F!<%V%k(J */
#define DAKUON	(0xde<<8)
#define HANDAKUON	(0xdf<<8)
static unsigned short	hiratohan[] = {
	/* $B$"(J */
	0xa7, 0xb1, 0xa8, 0xb2, 0xa9, 0xb3, 0xaa, 0xb4, 0xab, 0xb5,
	/* $B$+(J */
	0xb6, 0xb6|DAKUON, 0xb7, 0xb7|DAKUON, 0xb8, 0xb8|DAKUON,
	0xb9, 0xb9|DAKUON, 0xba, 0xba|DAKUON,
	/* $B$5(J */
	0xbb, 0xbb|DAKUON, 0xbc, 0xbc|DAKUON, 0xbd, 0xbd|DAKUON,
	0xbe, 0xbe|DAKUON, 0xbf, 0xbf|DAKUON,
	/* $B$?(J */
	0xc0, 0xc0|DAKUON, 0xc1, 0xc1|DAKUON, 0xaf, 0xc2, 0xc2|DAKUON,
	0xc3, 0xc3|DAKUON, 0xc4, 0xc4|DAKUON,
	/* $B$J(J */
	0xc5, 0xc6, 0xc7, 0xc8, 0xc9,
	/* $B$O(J */
	0xca, 0xca|DAKUON, 0xca|HANDAKUON,
	0xcb, 0xcb|DAKUON, 0xcb|HANDAKUON,
	0xcc, 0xcc|DAKUON, 0xcc|HANDAKUON,
	0xcd, 0xcd|DAKUON, 0xcd|HANDAKUON,
	0xce, 0xce|DAKUON, 0xce|HANDAKUON,
	/* $B$^(J */
	0xcf, 0xd0, 0xd1, 0xd2, 0xd3,
	/* $B$d(J */
	0xac, 0xd4, 0xad, 0xd5, 0xae, 0xd6,
	/* $B$i(J */
	0xd7, 0xd8, 0xd9, 0xda, 0xdb,
	/* $B$o(J */
	0xdc, 0xdc, 0xb2, 0xb4, 0xa6,
	/* $B$s(J */
	0xdd,
};

/* $BA43Q5-9f(J $B"M(J $BH>3QBP1~I=(J */
static struct symzenhan {
	unsigned short	zen;
	unsigned char	han;
}	kigoutohan[] = {
	0xa1a1, 0x20,	0xa1a2, 0xa4,	0xa1a3, 0xa1,	0xa1a4, 0x2c,
	0xa1a5, 0x2e,	0xa1a6, 0xa5,	0xa1a7, 0x3a,	0xa1a8, 0x3b,
	0xa1a9, 0x3f,	0xa1aa, 0x21,	0xa1ab, 0xde,	0xa1ac, 0xdf,
	0xa1b0, 0x5e,	0xa1b2, 0x5f,	0xa1bc, 0xb0,	0xa1bf, 0x2f,
	0xa1c1, 0x7e,	0xa1c3, 0x7c,	0xa1c6, 0x60,	0xa1c7, 0x27,
	0xa1c8, 0x22,	0xa1c9, 0x22,	0xa1ca, 0x28,	0xa1cb, 0x29,
	0xa1cc, 0x5b,	0xa1cd, 0x5d,	0xa1ce, 0x5b,	0xa1cf, 0x5d,
	0xa1d0, 0x7b,	0xa1d1, 0x7d,	0xa1d6, 0xa2,	0xa1d7, 0xa3,
	0xa1dc, 0x2b,	0xa1dd, 0x2d,	0xa1e1, 0x3d,	0xa1e3, 0x3c,
	0xa1e4, 0x3e,	0xa1ef, 0x5c,	0xa1f0, 0x24,	0xa1f3, 0x25,
	0xa1f4, 0x23,	0xa1f5, 0x26,	0xa1f6, 0x2a,	0xa1f7, 0x40,
};
#define KIGOUSIZE	(sizeof(kigoutohan) / sizeof(struct symzenhan))

/* $BH>3Q(J $B"M(J $BA43QJQ49I=(J */
static wchar_t	hantozen[] = {
/* C0 */
0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
0x0008, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d, 0x000e, 0x000f,
0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
0x0018, 0x0019, 0x001a, 0x001b, 0x001c, 0x001d, 0x001e, 0x001f,
/* ASCII */
0xa1a1, 0xa1aa, 0xa1c9, 0xa1f4, 0xa1f0, 0xa1f3, 0xa1f5, 0xa1c7,
0xa1ca, 0xa1cb, 0xa1f6, 0xa1dc, 0xa1a4, 0xa1dd, 0xa1a5, 0xa1bf,
0xa3b0, 0xa3b1, 0xa3b2, 0xa3b3, 0xa3b4, 0xa3b5, 0xa3b6, 0xa3b7,
0xa3b8, 0xa3b9, 0xa1a7, 0xa1a8, 0xa1e3, 0xa1e1, 0xa1e4, 0xa1a9,
0xa1f7, 0xa3c1, 0xa3c2, 0xa3c3, 0xa3c4, 0xa3c5, 0xa3c6, 0xa3c7,
0xa3c8, 0xa3c9, 0xa3ca, 0xa3cb, 0xa3cc, 0xa3cd, 0xa3ce, 0xa3cf,
0xa3d0, 0xa3d1, 0xa3d2, 0xa3d3, 0xa3d4, 0xa3d5, 0xa3d6, 0xa3d7,
0xa3d8, 0xa3d9, 0xa3da, 0xa1ce, 0xa1ef, 0xa1cf, 0xa1b0, 0xa1b2,
0xa1c6, 0xa3e1, 0xa3e2, 0xa3e3, 0xa3e4, 0xa3e5, 0xa3e6, 0xa3e7,
0xa3e8, 0xa3e9, 0xa3ea, 0xa3eb, 0xa3ec, 0xa3ed, 0xa3ee, 0xa3ef,
0xa3f0, 0xa3f1, 0xa3f2, 0xa3f3, 0xa3f4, 0xa3f5, 0xa3f6, 0xa3f7,
0xa3f8, 0xa3f9, 0xa3fa, 0xa1d0, 0xa1c3, 0xa1d1, 0xa1c1, 0x007f,
/* C1 */
0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
0x0088, 0x0089, 0x008a, 0x008b, 0x008c, 0x008d, 0x008e, 0x008f,
0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
0x0098, 0x0099, 0x009a, 0x009b, 0x009c, 0x009d, 0x009e, 0x009f,
/* KANA */
0xa1a1, 0xa1a3, 0xa1d6, 0xa1d7, 0xa1a2, 0xa1a6, 0xa5f2, 0xa5a1,
0xa5a3, 0xa5a5, 0xa5a7, 0xa5a9, 0xa5e3, 0xa5e5, 0xa5e7, 0xa5c3,
0xa1bc, 0xa5a2, 0xa5a4, 0xa5a6, 0xa5a8, 0xa5aa, 0xa5ab, 0xa5ad,
0xa5af, 0xa5b1, 0xa5b3, 0xa5b5, 0xa5b7, 0xa5b9, 0xa5bb, 0xa5bd,
0xa5bf, 0xa5c1, 0xa5c4, 0xa5c6, 0xa5c8, 0xa5ca, 0xa5cb, 0xa5cc,
0xa5cd, 0xa5ce, 0xa5cf, 0xa5d2, 0xa5d5, 0xa5d8, 0xa5db, 0xa5de,
0xa5df, 0xa5e0, 0xa5e1, 0xa5e2, 0xa5e4, 0xa5e6, 0xa5e8, 0xa5e9,
0xa5ea, 0xa5eb, 0xa5ec, 0xa5ed, 0xa5ef, 0xa5f3, 0xa1ab, 0xa1ac,
/* undefined */
0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7,
0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7,
0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff,
};

/* Stipple Pattern */
static char stipple_bits[] = {
   0x55, 0x55, 0x00, 0x00, 0xaa, 0xaa, 0x00, 0x00, 0x55, 0x55, 0x00, 0x00,
   0xaa, 0xaa, 0x00, 0x00, 0x55, 0x55, 0x00, 0x00, 0xaa, 0xaa, 0x00, 0x00,
   0x55, 0x55, 0x00, 0x00, 0xaa, 0xaa, 0x00, 0x00};

/*
 * Window Layout
 *
 * 3$B$D$N%&%#%s%I%&$r;}$D(J
 *	core.window		-- $B%F%-%9%HI=<(MQ(J
 *	kinput.statuswin	-- $B<!8uJd(J/$B5-9fI=<(MQ(J
 *	kinput.modewin		-- $BF~NO%b!<%II=<(MQ(J
 *
 * statuswin $B$H(J modewin $B$b(J KanjiInput Widget $B$N0lIt$G$"$k(J --
 * $B$D$^$j>/$7JQ$J$3$H$r$7$F$$$F!"(J_XtRegisterWindow() $B$r(J
 * $B;HMQ$7$F$$$k!#$3$N4X?t$O(J Intrinsics $B$N%I%-%e%a%s%H$K$O(J
 * $B=q$+$l$F$$$J$$(J -- $B$D$^$j@5$7$$%D!<%k%-%C%H$N;H$$J}$G$O$J$$!#(J
 * $B$G$b$3$NJ}$,4JC1$J$s$@$b$s!#(J
 *
 * +----------------------------------------------------+
 * | $B%F%-%9%H(J                                           |
 * |                                                    |
 * |                                                    |
 * |+---------------------------------------++---------+|
 * ||$B8uJd(J                                   || $B%b!<%I(J  ||
 * |+---------------------------------------++---------+|
 * +----------------------------------------------------+
 */


/*
 *
 *	$B%a%=%C%I(J
 *
 */

static void
Initialize(request, new)
Widget request;
Widget new;
{
	KanjiInputWidget	kwid = (KanjiInputWidget)new;
	KanjiInputPart		*kpart = &(kwid->kinput);
	XGCValues		values;
	Pixmap			spixmap;
	extern char		*getenv();
	XtAppContext		appcon = XtWidgetToApplicationContext(new);
	int			i;
	Arg			args[2];

	/* making pixmap */
	spixmap = XCreateBitmapFromData(XtDisplay(new),
					RootWindowOfScreen(XtScreen(new)),
					stipple_bits, 16, 16);
	kpart->stipplepixmap = spixmap;

	/* get GCset */
	values.function = GXcopy;
	values.foreground = kpart->foreground;
	values.background = kwid->core.background_pixel;
	kpart->normgc = XtGetGC(new, GCForeground|GCBackground|GCFunction,
				&values);
	values.function = GXinvert;
	values.plane_mask = kpart->foreground ^ kwid->core.background_pixel;
	kpart->revgc = XtGetGC(new,
			       GCForeground|GCFunction|GCPlaneMask,
			       &values);
	values.function = GXcopy;
	values.fill_style = FillStippled;
	values.stipple = spixmap;
	values.foreground = kwid->core.background_pixel;
	kpart->stipplegc = XtGetGC(new,
				   GCForeground|GCBackground|GCFunction|GCPlaneMask|GCFillStyle|GCStipple,
			       &values);

	NewFont(kwid);

	kwid->kinput.statusmapped = False;

	/* $BJQ49%F%-%9%H$NI=<(9T?t(J */
	/* core.height $B$,;XDj$5$l$F$$$?$i$=$l$rM%@h$9$k(J */
	if (request->core.height > 0) {
		kpart->nlines = (new->core.height -
				 (kpart->topmargin + StatusHeight(kwid))) /
		    		 kpart->fontheight;
	}

	/* $B%&%#%s%I%&$N9b$5(J
	 * core.height $B$,;XDj$5$l$F$$$J$$$+!"I}$,69$9$.$k;~$K$O(J
	 * $BE,Ev$J9b$5$r7W;;$9$k(J
	 */
	if (request->core.height <= 0 || kpart->nlines < 1) {
		if (kpart->nlines < 1)
			kpart->nlines = 1;
		new->core.height = kpart->topmargin +
		    kpart->nlines * kpart->fontheight +
		    StatusHeight(kwid);
	}

	/* subwindow $B$r:n$k(J */
	XtSetArg(args[0], XtNexposeFunc, (caddr_t)SubwindowRedraw);
	XtSetArg(args[1], XtNrealizeFunc, (caddr_t)SubwindowRealize);
	kwid->kinput.statuswid = XtCreateWidget("status",
						kanjiInputSubwindowWidgetClass,
						new,
						args, 2);
	XtSetArg(args[0], XtNexposeFunc, (caddr_t)SubwindowRedraw);
	XtSetArg(args[1], XtNrealizeFunc, (caddr_t)SubwindowRealize);
	kwid->kinput.modewid = XtCreateWidget("mode",
					      kanjiInputSubwindowWidgetClass,
					      new,
					      args, 2);

	/* $B%l%$%"%&%H$r7W;;$9$k(J */
	Layout(kwid);

	/* $BJQ49$N=i4|2=(J */
	kpart->createrule = False;
	kpart->createenv = False;
	if (kpart->ccrule == NULL) {
		if (kpart->ccdeffile == NULL) {
			kpart->ccdeffile = getenv("CC_DEF");
			if (kpart->ccdeffile == NULL) {
				kpart->ccdeffile = DEF_CCDEF_FILE;
			}
		}
		kpart->ccrule = ccParseRule(kpart->ccdeffile, XtWarning);
		kpart->createrule = True;
	}

	if (kpart->wnnenv == NULL) {
		WNN_JSERVER_ID	*server;
		if (kpart->wnnenvname == NULL) kpart->wnnenvname = "";
		if (kpart->wnnenvrcfile == NULL) kpart->wnnenvrcfile = "";
		server = jiOpenServer(kpart->jservername, 30);
		if (server == NULL) {
			kpart->wnnenv = NULL;
			XtAppWarning(appcon, "can't connect to jserver");
		} else {
			kpart->wnnenv = jiCreateEnv(server,
						    kpart->wnnenvname,
						    kpart->wnnoverrideenv,
						    kpart->wnnenvrcfile,
						    createenverror,
						    createConfirm,
						    (caddr_t)new);
			kpart->createenv = True;
		}
	}
	if (kpart->ccrule == NULL || kpart->wnnenv == NULL) {
		XtAppError(appcon, "Kanji-conversion initialization failed.");
	}

	kpart->ccbuf = ccCreateBuf(kpart->ccrule, 16,
				   fepfunctbl, FTSIZE,
				   defAction, insChar, delChar,
				   funcDispatch, NULL, (caddr_t)new);

	kpart->jcbuf = jcCreateBuffer(kpart->wnnenv, 10, 80);

	if (kpart->ccbuf == NULL || kpart->jcbuf == NULL) {
		XtAppError(appcon, "Kanji-conversion initialization failed.");
	}

	/* $B%G%#%9%W%l%$%P%C%U%!$N%3%T!<$r$$$l$kNN0h$N3NJ](J */
	kpart->stringsize = 80;
	kpart->string = (wchar_t *)XtMalloc(80 * sizeof(wchar_t));
	kpart->string[0] = 0;

	/* $B8uJdI=<($N:]$N8uJd$rF~$l$kNN0h$N3NJ](J */
	kpart->selstring = (wchar_t *)XtMalloc(SELBUFSIZE * sizeof(wchar_t));

	/* $B3FJ8@a$N>pJs$rF~$l$kNN0h$N3NJ](J */
	kpart->clrecsize = 10;
	kpart->clrec = (ClauseRec *)XtMalloc(kpart->clrecsize * sizeof(ClauseRec));

	/* $B5-9f$N=i4|2=(J */
	kpart->cursymbol = 0;

	/* $B=i4|%b!<%I$N5-O?(J */
	kpart->curmode = ccGetMode(kpart->ccbuf);
	kpart->state = MODE_NORMAL;
	kpart->nclause = 0;
	kpart->curclause = 0;
	kpart->curlcstart = 0;
	kpart->curlcend = 1;
	kpart->dotoffset = 0;
	kpart->cursorrec.x = kpart->leftmargin;
	kpart->cursorrec.y = kpart->topmargin;
	kpart->cursorrec.w = kpart->curwidth;
	kpart->cursorrec.h = kpart->fontheight;
	UpdateRec((KanjiInputWidget)new);

}

static void
Realize(w, valueMask, attributes)
Widget w;
XtValueMask *valueMask;
XSetWindowAttributes *attributes;
{
	KanjiInputWidget	kwid = (KanjiInputWidget)w;
	int	x, y, width, height;
	
	*valueMask |= CWBitGravity;
	attributes->bit_gravity = ForgetGravity;
	
	XtCreateWindow(w, InputOutput, (Visual *)CopyFromParent,
		       *valueMask, attributes);
	XtRealizeWidget(kwid->kinput.statuswid);
	XtRealizeWidget(kwid->kinput.modewid);
	XtMapWidget(kwid->kinput.statuswid);
	XtMapWidget(kwid->kinput.modewid);
}

static void
SubwindowRealize(w, valueMask, attributes)
Widget w;
XtValueMask *valueMask;
XSetWindowAttributes *attributes;
{
	*valueMask |= CWBitGravity;
	attributes->bit_gravity = ForgetGravity;
	
	XtCreateWindow(w, InputOutput, (Visual *)CopyFromParent,
		       *valueMask, attributes);
}

/* ARGSUSED */
static void
Redraw(w, event, region)
Widget w;
XEvent *event;
Region region;
{
	KanjiInputWidget kwid = (KanjiInputWidget)w;
	
	RedrawText(kwid);
}

/* ARGSUSED */
static void
SubwindowRedraw(w, event, region)
Widget w;
XEvent *event;
Region region;
{
	KanjiInputWidget	kwid = (KanjiInputWidget)XtParent(w);
	
	/* always redraw the entire window (for now) */
	if (event->xexpose.window == XtWindow(kwid->kinput.modewid)) {
		RedrawMode(kwid);
	} else {
		RedrawCandidates(kwid);
		RedrawSymbol(kwid);
	}
}

static void
Destroy(w)
Widget w;
{
	KanjiInputWidget	kwid = (KanjiInputWidget)w;
	KanjiInputPart	*kpart = &(kwid->kinput);

	XtReleaseGC(w, kpart->normgc);
	XtReleaseGC(w, kpart->revgc);
	XtWSReleaseGCSet(w, kpart->textgc);
	XFreePixmap(XtDisplay(w), kpart->stipplepixmap);

	ccDestroyBuf(kpart->ccbuf);
	jcDestroyBuffer(kpart->jcbuf, 1);
	if (kpart->createrule) {
		ccFreeRule(kpart->ccrule);
	}
	if (kpart->createenv) {
		WNN_JSERVER_ID	*server = kpart->wnnenv->js_id;
		(void)jiDeleteEnv(kpart->wnnenv);
		(void)jiCloseServer(server);
	}
}

static void
Resize(w)
Widget w;
{
	/* $B%l%$%"%&%H$r7W;;$7D>$9(J */
	Layout((KanjiInputWidget)w);
}

/* ARGSUSED */
static Boolean
SetValues(cur, req, new)
Widget cur;
Widget req;
Widget new;
{
	KanjiInputWidget	kcur = (KanjiInputWidget)cur;
	KanjiInputWidget	knew = (KanjiInputWidget)new;
	Boolean			redraw = False;
	Boolean			layout = False;

	if (kcur->kinput.G0font != knew->kinput.G0font ||
	    kcur->kinput.G1font != knew->kinput.G1font ||
	    kcur->kinput.G2font != knew->kinput.G2font) {
		/* $B%U%)%s%H$,JQ$o$C$?(J */
		XtWSReleaseGCSet(new, knew->kinput.textgc);
		NewFont(knew);
		redraw = True;
	}

	if (kcur->kinput.modewidth != knew->kinput.modewidth) {
		layout = True;
		redraw = True;
	}

	if (kcur->core.width != knew->core.width ||
	    kcur->core.height != knew->core.height) {
		layout = True;
		redraw = True;
	} else if (kcur->kinput.nlines != knew->kinput.nlines) {
		Dimension	height;
		if (knew->kinput.nlines < 1)
			knew->kinput.nlines = 1;
		height = knew->kinput.topmargin +
		    knew->kinput.nlines * knew->kinput.fontheight +
		    StatusHeight(knew);
		if (XtMakeResizeRequest(knew, knew->core.width, height,
					NULL, NULL) == XtGeometryYes)
			knew->core.height = height;
		layout = True;
		redraw = True;
	}
	if (layout)
		Layout(knew);

	return redraw;
}


static void
Focusin()
{
}

static void
Focusout()
{
}

static void
Enter()
{
}

static void
Leave()
{
}

static void
NewFont(w)
KanjiInputWidget w;
{
	KanjiInputPart	*kpart = &(w->kinput);
	int		ascent, descent;
	XGCValues	values;
	
	/* get GCset */
	values.function = GXcopy;
	values.foreground = kpart->foreground;
	values.background = w->core.background_pixel;
	kpart->textgc = XtWSGetGCSet((Widget)w,
				     GCFont|GCForeground|GCBackground|GCFunction,
				     &values,
				     kpart->G0font, kpart->G1font,
				     kpart->G2font, (XFontStruct *)NULL);

	/* $B%+!<%=%k$NI}(J ('l'$B$NI}(J) */
	if (kpart->G0font != NULL) {
		kpart->curwidth = charwidth(kpart->textgc, 'l', 0);
	} else {
		kpart->curwidth = 8;
	}

	/* $B%U%)%s%H$N9b$5(J */
	XWSFontHeight(kpart->textgc, NULL, 0, &ascent, &descent);
	kpart->ascent = ascent;
	kpart->fontheight = ascent + descent;

	/* $B%b!<%II=<(MQNN0h$NI}$r3NJ](J */
	if (kpart->modewidth <= 0) {
		/* default width (10n) */
		if (kpart->G0font != NULL) {
			kpart->modewidth = charwidth(kpart->textgc, 'n', 0) * 10;
		} else {
			kpart->modewidth = 64;
		}
	}
}

static void
Layout(w)
KanjiInputWidget w;
{
	KanjiInputPart	*kpart = &(w->kinput);
	int		x, y, width, height;

	/* $BJQ49%F%-%9%H$NI=<(9T?t(J */
	kpart->nlines = (w->core.height -
			 (kpart->topmargin + StatusHeight(w))) /
	    		 kpart->fontheight;

	/* $B%9%F!<%?%9(J/$B%b!<%I%&%#%s%I%&$N%l%$%"%&%H(J */
	x = 0;
	y = w->core.height - StatusHeight(w);
	width = w->core.width - kpart->modewidth - 3;
	height = kpart->fontheight;
	XtConfigureWidget(kpart->statuswid, x, y, width, height, 1);
	x = width + 1;
	width = kpart->modewidth;
	XtConfigureWidget(kpart->modewid, x, y, width, height, 1);
}

/*
 *
 *	$B%-!<F~NOMQ%U%!%s%/%7%g%s(J
 *
 */
static void
KeyEventProc(w, event)
Widget w;
XEvent *event;
{
	UpdateHint((KanjiInputWidget)w) = NOTCHANGED;
	decodekey((KanjiInputWidget)w, (XKeyEvent *)event);
	scrupdate((KanjiInputWidget)w);
}

static void
decodekey(w, event)
KanjiInputWidget w;
XKeyEvent *event;
{
	(void)ccConvchar(w->kinput.ccbuf, event);
}

/* ARGSUSED */
static void
funcDispatch(func, str, nbytes, w)
int func;
unsigned char *str;
int nbytes;
Widget w;
{
	KanjiInputWidget	kw = (KanjiInputWidget)w;

	switch (kw->kinput.state) {
	case MODE_SELECTION_S:
	case MODE_SELECTION_L:
		(*func_selection[func])(kw);
		break;
	case MODE_SYMBOL:
		(*func_symbol[func])(kw);
		break;
	default:
		(*func_normal[func])(kw);
		break;
	}
}

/* ARGSUSED */
static void
defAction(str, nbytes, w)
unsigned char *str;
int nbytes;
Widget w;
{
	PRINT(("defAction invoked\n"));
	beep(w);
}


#include "Func.c"

/*
 *
 *	$BI=<(MQ%U%!%s%/%7%g%s(J
 *
 */

/* $B%+%l%s%H%b!<%I$NI=<((J */
static void
RedrawMode(w)
KanjiInputWidget w;
{
	Window	win = XtWindow(w->kinput.modewid);
	wchar_t	*modestr = ccGetModePrompt(w->kinput.ccbuf);
	int	modestrlen = wstrlen(modestr);
	int	width;
	int	x;

	XClearWindow(XtDisplay(w), win);
	width = KITextWidth(w->kinput.textgc, modestr, modestrlen, 1);
	if (width > w->kinput.modewidth)
		x = 0;
	else
		x = (w->kinput.modewidth - width) / 2;

	(void)KIDrawString(XtDisplay(w), win, w->kinput.textgc,
		      x, w->kinput.ascent, modestr, modestrlen, 1);
}

/* $B%F%-%9%H$NI=<((J */
#include "Update.c"

/* $B8uJd$NI=<((J */
#include "Status.c"

/* $B2hLL99?7(J */
static void
scrupdate(w)
KanjiInputWidget w;
{
	int	newmode = ccGetMode(w->kinput.ccbuf);
	if (w->kinput.curmode != newmode) {
		w->kinput.curmode = newmode;
		RedrawMode(w);
	}
	if (UpdateHint(w) != NOTCHANGED)
		UpdateText(w);
}


/*
 *
 * $B3F<o%f!<%F%#%j%F%#!&%U%!%s%/%7%g%s(J
 *
 */

/* $BNN0h$NH?E>(J */
static void
reverseArea(w, x, y, width, height)
KanjiInputWidget w;
int x;
int y;
int width;
int height;
{
/*	Following code should work, but on SUN CG4 color screen,
	it doesn't.
	XCopyArea(XtDisplay(w), XtWindow(w), XtWindow(w),
		  w->kinput.revgc, x, y, width, height, x, y);
*/
	XFillRectangle(XtDisplay(w), XtWindow(w), w->kinput.revgc,
		       x, y, width, height);
}

/* stipple $B$GH?E>(J */
static void
stippleArea(w, x, y, width, height)
KanjiInputWidget w;
int x;
int y;
int width;
int height;
{
	XFillRectangle(XtDisplay(w), XtWindow(w), w->kinput.stipplegc,
		       x, y, width, height);
}

/* $BJ8;zI}$r5a$a$k(J */
static int
charwidth(gcset, c, addlbearing)
XWSGC gcset;
int c;
int addlbearing;
{
	int     	width, nonPrinting;
	int     	gset;
	XFontStruct	*font;
	int		lb;

	nonPrinting = (c < ' ' || c == 0x7f);
	if (nonPrinting)
		c ^= 0x40;
	
	switch (c & 0x8080) {
	case 0:
		gset = 0;
		break;
	case 0x8080:
		gset = 1;
		break;
	case 0x80:
		gset = 2;
		break;
	case 0x8000:
		gset = 3;
		break;
	}

	font = gcset->fe[gset].font;
	if (font == NULL)
		return 0;
	c &= 0x7f7f;

	if (gcset->fe[gset].flag & TWOB) {
		if (font->per_char &&
		    (c >> 8) >= font->min_byte1 &&
		    (c >> 8) <= font->max_byte1 &&
		    (c & 0xff) >= font->min_char_or_byte2 &&
		    (c & 0xff) <= font->max_char_or_byte2) {
			int	cols = font->max_char_or_byte2 -
			    font->min_char_or_byte2 + 1;
			width = font->per_char[((c>>8) - font->min_byte1) * cols
					       + (c&0x7f) - font->min_char_or_byte2].width;
			lb = font->per_char[((c>>8) - font->min_byte1) * cols
					       + (c&0x7f) - font->min_char_or_byte2].lbearing;
		} else {
			width = font->min_bounds.width;
			lb = font->min_bounds.lbearing;
		}
		if (nonPrinting)
			width += charwidth(gcset, '^', addlbearing);
	} else {
		if (font->per_char &&
		    (c >= font->min_char_or_byte2 && c <= font->max_char_or_byte2)) {
			width = font->per_char[c - font->min_char_or_byte2].width;
			lb = font->per_char[c - font->min_char_or_byte2].lbearing;
		} else {
			width = font->min_bounds.width;
			lb = font->min_bounds.lbearing;
		}
		if (nonPrinting)
			width += charwidth(gcset, '^', addlbearing);
	}

	if (addlbearing && !nonPrinting && lb < 0)
		width -= lb;

	return width;
}

/* lbearing $B$r5a$a$k(J */
static int
lbearing(gcset, c)
XWSGC gcset;
int c;
{
	int     	gset;
	int		lb;
	XFontStruct	*font;
	
	c = (c < ' ' || c == 0x7f) ? '^' : c;

	switch (c & 0x8080) {
	case 0:
		gset = 0;
		break;
	case 0x8080:
		gset = 1;
		break;
	case 0x80:
		gset = 2;
		break;
	case 0x8000:
		gset = 3;
		break;
	}

	font = gcset->fe[gset].font;
	if (font == NULL)
		return 0;
	c &= 0x7f7f;

	if (gcset->fe[gset].flag & TWOB) {
		if (font->per_char &&
		    (c >> 8) >= font->min_byte1 &&
		    (c >> 8) <= font->max_byte1 &&
		    (c & 0xff) >= font->min_char_or_byte2 &&
		    (c & 0xff) <= font->max_char_or_byte2) {
			int	cols = font->max_char_or_byte2 -
			    font->min_char_or_byte2 + 1;
			lb = font->per_char[((c>>8) - font->min_byte1) * cols
					    + (c&0x7f) - font->min_char_or_byte2].lbearing;
		} else
			lb = font->min_bounds.lbearing;
	} else {
		if (font->per_char &&
		    (c >= font->min_char_or_byte2 && c <= font->max_char_or_byte2))
			lb = font->per_char[c - font->min_char_or_byte2].lbearing;
		else
			lb = font->min_bounds.lbearing;
	}

	return lb;
}

static int
KITextWidth(wgc, str, n, top)
XWSGC wgc;
wchar_t *str;
int n;
int top;
{
	int	wid;
	wchar_t	*p, *q;
	int	nctl;

	/* $B%3%s%H%m!<%k%3!<%I$r(J '^?' $B7A<0$KD>$9(J */

	p = str; q = p + n;
	nctl = 0;
	while (p < q) {
		if (*p < 0x20 || *p == 0x7f)
			nctl++;
		p++;
	}
	if (nctl > 0) {
		wchar_t	*r, *s;

		s = (wchar_t *)LOCAL_ALLOC(sizeof(wchar_t) * (n + nctl));
		p = str; q = p + n;
		r = s;
		while (p < q) {
			if (*p < 0x20 || *p == 0x7f) {
				*r++ = '^';
				*r++ = *p ^ 0x40;
			} else {
				*r++ = *p;
			}
			p++;
		}
		wid = XWSTextWidth(wgc, s, n + nctl);
		if (top) {
			int	lb;
			if ((lb = lbearing(wgc, *s)) < 0)
				wid -= lb;
		}
		LOCAL_FREE(s);
	} else {
		/* $B%3%s%H%m!<%k%3!<%I$,$J$$(J */
		wid = XWSTextWidth(wgc, str, n);
		if (top) {
			int	lb;
			if ((lb = lbearing(wgc, *str)) < 0)
				wid -= lb;
		}
	}
	return wid;
}

static int
KIDrawString(dpy, win, wgc, x, y, str, n, top)
Display *dpy;
Drawable win;
XWSGC wgc;
int x;
int y;
wchar_t *str;
int n;
int top;
{
	wchar_t	*p, *q;
	int	nctl;
	int	lb = 0;

	/* $B%3%s%H%m!<%k%3!<%I$r(J '^?' $B7A<0$KD>$9(J */

	if (n < 0)
		return 0;

	/*
	 * $BJ8@a$N@hF,(J (top != 0) $B$N;~$K$O!"@hF,$NJ8;z$,A0$NJ8@a$K(J
	 * $B?)$$9~$^$J$$$h$&$K!"(Jleft-side bearing $B$,Ii(J($B$D$^$jA0$NJ8;z$K$+$+$k(J)
	 * $B$N$H$-$K$O%F%-%9%H$r$=$NJ,1&$K$:$i$7$F=q$/(J
	 */
	if (top)
		lb = lbearing(wgc, *str);
	if (lb > 0)
		lb = 0;

	x -= lb;
	p = str; q = p + n;
	nctl = 0;
	while (p < q) {
		if (*p < 0x20 || *p == 0x7f)
			nctl++;
		p++;
	}
	if (nctl > 0) {
		wchar_t	*r, *s;
		int	wid;

		s = (wchar_t *)LOCAL_ALLOC(sizeof(wchar_t) * (n + nctl));
		p = str; q = p + n;
		r = s;
		while (p < q) {
			if (*p < 0x20 || *p == 0x7f) {
				*r++ = '^';
				*r++ = *p ^ 0x40;
			} else {
				*r++ = *p;
			}
			p++;
		}
		wid = XWSDrawString(dpy, win, wgc, x, y, s, n + nctl) - lb;
		LOCAL_FREE(s);
		return wid;
	} else {
		/* $B%3%s%H%m!<%k%3!<%I$,$J$$(J */
		return XWSDrawString(dpy, win, wgc, x, y, str, n) - lb;
	}
	/* NOTREACHED */
}

static void
createenverror(type, s, data)
int type;
char *s;
caddr_t data;
{
	Widget w = (Widget)data;
	char	line[1024];

	(void)sprintf(line, "%s: %s",
		      (type == TYPE_ERROR) ? "ERROR" : "Warning", s);
	XtAppWarning(XtWidgetToApplicationContext(w), line);
}

static int
createConfirm(type, file, data)
int type;
char *file;
caddr_t data;
{
	KanjiInputWidget w = (KanjiInputWidget)data;

	if (w->kinput.confirmfunc) {
		return (*w->kinput.confirmfunc)((Widget)w, type, file);
	} else {
		return 1;
	}
}

static void
quitfep()
{
	PRINT(("quitfep() called\n"));
}

#ifdef DEBUG
errprint(char *fmt, int a1, int a2, int a3, int a4, int a5)
{
	fprintf(stderr, fmt, a1, a2, a3, a4, a5);
}
#endif


/*
 * $B%$%s%?!<%U%'%$%9%U%!%s%/%7%g%s(J
 */
void
KIFixString(w)
Widget w;
{
	fix((KanjiInputWidget)w);
	UpdateHint((KanjiInputWidget)w) = REDRAW;
	scrupdate((KanjiInputWidget)w);
}
