/*
 * Copyright (c) 1989  Software Research Associates, Inc.
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

/* $Header: /private/usr/mmb/ishisone/job/x11/kinput/RCS/KanjiInpuP.h,v 3.2 89/10/30 19:18:40 ishisone Exp $ */

#ifndef _KanjiInputP_h
#define _KanjiInputP_h

/* default ccdef file */
#define DEF_CCDEF_FILE	"/usr/lib/X11/ccdef/ccdef.kinput"

/* ステータスラインに表示する候補の最大数 */
#define MAXSELECTION	20
/* ステータスラインに表示する候補を格納するバッファの大きさ */
#define SELBUFSIZE	100

typedef struct {
	int	empty;
} KanjiInputClassPart;

/*
 * Full class record declaration for KanjiInput class
 */
typedef struct _KanjiInputClassRec {
	CoreClassPart		core_class;
	KanjiInputClassPart	kanjiInput_class;
} KanjiInputClassRec;

extern KanjiInputClassRec kanjiInputClassRec; 

typedef struct {
	Position	x, y;
	char		conv;
	wchar_t		*clause;
} ClauseRec;

typedef struct {
	char		visible;
	Position	x, y;
	Dimension	w, h;
} CursorRec;

typedef struct {
	Position	x;
	wchar_t		*candidate;
} CandidateRec;

/*
 * New fields for the KanjiInput widget instance record 
 */
typedef struct _KanjiInputPart {
    /* resource */
	XFontStruct	*G0font, *G1font, *G2font;
	int		nlines;
	int		modewidth;
	Pixel		foreground;
	XtCallbackList	endcallback;
	XtCallbackList	fixcallback;
	int		(*confirmfunc)();
	Dimension	leftmargin, rightmargin;
	Dimension	topmargin, bottommargin;
	String		jservername;
	String		wnnenvname;
	String		wnnenvrcfile;
	Boolean		wnnoverrideenv;
	String		ccdeffile;
	ccRule		ccrule;
	struct wnn_env	*wnnenv;
    /* private */
	Widget		statuswid;
	Widget		modewid;
	Boolean		statusmapped;
	int		updatehint;
	int		curmode;
	int		curwidth;
	int		state;
	int		ascent;
	int		fontheight;
	GC		normgc;
	GC		revgc;
	GC		stipplegc;
	Pixmap		stipplepixmap;
	XWSGC		textgc;
	Boolean		createrule;
	Boolean		createenv;
	ccBuf		ccbuf;
	jcConvBuf	*jcbuf;
	int		nclause;
	int		curclause;
	int		curlcstart;
	int		curlcend;
	int		dotoffset;
	wchar_t		*string;
	int		stringsize;
	ClauseRec	*clrec;
	int		clrecsize;
	CursorRec	cursorrec;
	/* for Candidate Selection */
	CandidateRec	selrec[MAXSELECTION];
	wchar_t		*selstring;
	short		ncand;
	short		curcand;
	short		dcand;
	short		selectionmax;
	short		selectionmin;
	/* for Symbol Input */
	wchar_t		symbolmax;
	wchar_t		symbolmin;
	wchar_t		cursymbol;
} KanjiInputPart;

/*
 * Full instance record declaration
 */

typedef struct _KanjiInputRec {
	CorePart		core;
	KanjiInputPart	kinput;
} KanjiInputRec;

#endif /*_KanjiInputP_h*/
