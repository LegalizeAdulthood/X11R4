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

/* $Header: /private/usr/mmb/ishisone/job/x11/kinput/RCS/KISubP.h,v 1.1 89/10/03 13:38:19 ishisone Rel $ */

#ifndef _KISubP_h
#define _KISubP_h

#include "KISub.h"

typedef struct {
	int	empty;
} KanjiInputSubwindowClassPart;

typedef struct _KanjiInputSubwindowClassRec {
	CoreClassPart			core_class;
	KanjiInputSubwindowClassPart	kanjiInputSubwindow_class;
} KanjiInputSubwindowClassRec;

extern KanjiInputSubwindowClassRec kanjiInputSubwindowClassRec; 

typedef struct _KanjiInputSubwindowPart {
    /* resource */
	XtRealizeProc	realizeproc;
	XtExposeProc	exposeproc;
} KanjiInputSubwindowPart;

typedef struct _KanjiInputSubwindowRec {
	CorePart		core;
	KanjiInputSubwindowPart	subwin;
} KanjiInputSubwindowRec;

#endif /*_KISubP_h*/
