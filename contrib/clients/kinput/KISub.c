/*
 * Copyright (C) 1989 by Software Research Associates, Inc.
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

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#ifdef CHECK_PROTOTYPE
#include <X11/Xlib.p>
#include <X11/Intrinsic.p>
#endif
#include <X11/CoreP.h>
#include "KISubP.h"

#ifndef lint
static char	*rcsid = "$Header: /private/usr/mmb/ishisone/job/x11/kinput/RCS/KISub.c,v 1.1 89/10/03 13:37:54 ishisone Rel $";
#endif

/* resources */
static XtResource	resources[] = {
	{XtNrealizeFunc, XtCFunction, XtRPointer, sizeof(XtRealizeProc),
	     XtOffset(KanjiInputSubwindowWidget, subwin.realizeproc),
	     XtRPointer, (caddr_t)NULL},
	{XtNexposeFunc, XtCFunction, XtRPointer, sizeof(XtExposeProc),
	     XtOffset(KanjiInputSubwindowWidget, subwin.exposeproc),
	     XtRPointer, (caddr_t)NULL},
};

/* functions */
static void	Realize();
static void	Redraw();

/* class record */
KanjiInputSubwindowClassRec	kanjiInputSubwindowClassRec = {
  {
  /* core field */
    /* superclass		*/	(WidgetClass)&widgetClassRec,
    /* class_name		*/	"KanjiInputSubwindow",
    /* size			*/	sizeof(KanjiInputSubwindowRec),
    /* class_initialize		*/	NULL,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	NULL,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	NULL,
    /* num_actions		*/	0,
    /* resources		*/	resources,
    /* resource_count		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	FALSE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	NULL,
    /* resize			*/	XtInheritResize,
    /* expose			*/	Redraw,
    /* set_values		*/	NULL,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	NULL,
    /* query_geometry		*/	NULL,
    /* display_accelerlator	*/	NULL,
    /* extension		*/	NULL,
  },
};

WidgetClass	kanjiInputSubwindowWidgetClass =
    (WidgetClass)&kanjiInputSubwindowClassRec;

static void
Realize(w, mask, attr)
Widget w;
XtValueMask *mask;
XSetWindowAttributes *attr;
{
	KanjiInputSubwindowWidget	kw = (KanjiInputSubwindowWidget)w;

	if (kw->subwin.realizeproc) {
		(*kw->subwin.realizeproc)(w, mask, attr);
	} else {
		XtCreateWindow(w, InputOutput, (Visual *)CopyFromParent,
			       *mask, attr);
	}
}

static void
Redraw(w, ev, region)
Widget w;
XEvent *ev;
Region region;
{
	KanjiInputSubwindowWidget	kw = (KanjiInputSubwindowWidget)w;

	if (kw->subwin.exposeproc) {
		(*kw->subwin.exposeproc)(w, ev, region);
	}
}


