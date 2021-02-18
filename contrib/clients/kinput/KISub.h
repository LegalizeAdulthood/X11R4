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

/* $Header: /private/usr/mmb/ishisone/job/x11/kinput/RCS/KISub.h,v 1.2 89/10/27 16:58:41 ishisone Rel $ */

#ifndef _KISub_h
#define _KISub_h

/* KanjiInputSubwindow Resources */
#define XtNexposeFunc	"exposeFunc"
#define XtNrealizeFunc	"realizeFunc"

extern WidgetClass kanjiInputSubwindowWidgetClass;

typedef struct _KanjiInputSubwindowClassRec *KanjiInputSubwindowWidgetClass;
typedef struct _KanjiInputSubwindowRec      *KanjiInputSubwindowWidget;

#endif /*_KISub_h*/
/* DON'T ADD STUFF AFTER THIS #endif */
