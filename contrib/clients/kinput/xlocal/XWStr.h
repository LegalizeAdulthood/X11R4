/*
 *	XWStr.h -- header file for X Wide-Character String Library
 */

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
 *		ishisone@sra.co.jp
 */

/* $Header: /private/usr/mmb/ishisone/job/x11/xlocal/RCS/XWStr.h,v 1.5 89/09/12 10:13:35 ishisone Rel $ */

#ifndef _XWStr_h
#define _XWStr_h

#include "WStr.h"

typedef struct {
	GC		gc;
	XFontStruct	*font;
	int		flag;
} FontEnt;

typedef struct {
	FontEnt	fe[4];
} XWSGCSet;

typedef XWSGCSet	*XWSGC;

/* XWSGCSet.flag */
#define TWOB		1	/* 2-byte character set */
#define FONTQUERY	2	/* XQueryFont()'ed by this library */
#define GCCREAT		4	/* XtGetGC()'ed by this library */

#define	GSET(c)	((c) & 0x8000 ? ((c) & 0x80 ? 1 : 3) : ((c) & 0x80 ? 2 : 0))

#if __STDC__ == 1
/* function prototype */
extern XWSGC XWSSetGCSet(GC, GC, GC, GC);
#ifdef _XtIntrinsic_h
extern XWSGC XtWSGetGCSet(Widget, unsigned long, XGCValues *,
			  XFontStruct *, XFontStruct *,
			  XFontStruct *, XFontStruct *);
extern void XtWSReleaseGCSet(Widget, XWSGC);
#endif
extern void XtWSDestroyGCSet(XWSGC);
extern int XWSDrawString(Display *, Drawable, XWSGC, int, int, wchar_t *, int);
extern int XWSDrawImageString(Display *, Drawable, XWSGC,
			      int, int, wchar_t *, int);
extern void XWSFontHeight(XWSGC, wchar_t *, int, int *, int *);
extern int XWSTextWidth(XWSGC, wchar_t *, int);
extern void XWSTextExtents(XWSGC, wchar_t *, int, int *, int *, XCharStruct *);
#else
extern XWSGC XWSSetGCSet();
extern XWSGC XtWSGetGCSet();
extern void XtWSDestroyGCSet();
extern void XtWSReleaseGCSet();
extern int XWSDrawString();
extern int XWSDrawImageString();
extern void XWSFontHeight();
extern int XWSTextWidth();
extern void XWSTextExtents();
#endif

#endif
