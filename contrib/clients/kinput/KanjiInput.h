/*
 *	KanjiInput.h
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
 */

/* $Header: /private/usr/mmb/ishisone/job/x11/kinput/RCS/KanjiInput.h,v 3.1 89/10/03 13:42:20 ishisone Rel $ */

#ifndef _KanjiInput_h
#define _KanjiInput_h

/* KanjiInput Resources */
#define XtNkanjiFont	"kanjiFont"
#define XtNkanaFont	"kanaFont"
#define XtNfixNotify	"fixNotify"
#define XtNendNotify	"endNotify"
#define XtNconfirmFunc	"confirmFunc"

#define XtNmodeWidth	"modeWidth"
#define XtCModeWidth	"ModeWidth"

#define XtNjserver	"jserver"
#define XtCJserver	"Jserver"
#define XtNwnnEnvname	"wnnEnvname"
#define XtCWnnEnvname	"WnnEnvname"
#define XtNwnnEnvrc	"wnnEnvrc"
#define XtCWnnEnvrc	"WnnEnvrc"
#define XtNwnnOverrideEnv	"wnnOverrideEnv"
#define XtCWnnOverrideEnv	"WnnOverrideEnv"
#define XtNccdef	"ccdef"
#define XtCCcdef	"Ccdef"
#define XtNwnnEnv	"wnnEnv"
#define XtCWnnEnv	"WnnEnv"
#define XtRWnnEnv	"WnnEnv"
#define XtNccRule	"ccRule"
#define XtCCcRule	"CcRule"
#define XtRCcRule	"CcRule"

#define XtNnlines	"nlines"
#define XtCLines	"Lines"
#define XtNleftMargin	"leftMargin"
#define XtNrightMargin	"rightMargin"
#define XtNtopMargin	"topMargin"
#define XtNlineSpacing	"lineSpacing"
#define XtCSpacing	"Spacing"

extern WidgetClass kanjiInputWidgetClass;

typedef struct _KanjiInputClassRec *KanjiInputWidgetClass;
typedef struct _KanjiInputRec      *KanjiInputWidget;

/* Interface function */
extern void KIFixString();

#endif /*_KanjiInput_h*/
/* DON'T ADD STUFF AFTER THIS #endif */
