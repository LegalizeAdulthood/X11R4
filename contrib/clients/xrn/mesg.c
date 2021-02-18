

#ifndef lint
static char XRNrcsid[] = "$Header: /users/ricks/xrn-11/R4/RCS/mesg.c,v 1.3 89/11/17 12:51:30 ricks Rel Locker: ricks $";
#endif

/*
 * xrn - an X-based NNTP news reader
 *
 * Copyright (c) 1988, 1989, Ellen M. Sentovich and Rick L. Spickelmier.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * mesg.c: message box
 */

#include "copyright.h"
#include <X11/Xos.h>
#include <stdio.h>
#include "utils.h"
#include "config.h"
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/cursorfont.h>
#include "xthelper.h"
#include "resources.h"
#include "xrn.h"
#include "mesg.h"
#include <varargs.h>

char error_buffer[2048];
/* entire widget */
static Widget MesgTopLevel = (Widget) 0;
/* text window */
static Widget MesgText = (Widget) 0;

#define MESG_SIZE 4096
static char MesgString[MESG_SIZE];


/*ARGSUSED*/
static void
dismissFunction(data)
caddr_t data;
{
    XtPopdown(MesgTopLevel);
    XtDestroyWidget(MesgTopLevel);
    MesgTopLevel = (Widget) 0;
    return;
}


#ifdef lint
#define BUTTON(nm,lbl)
#else
#ifdef __STDC__
#define BUTTON(nm,lbl)				\
static XtCallbackRec nm##Callbacks[] = {	\
    {nm##Function, NULL},			\
    {NULL, NULL}				\
};						\
static Arg nm##Args[] = {			\
    {XtNname, (XtArgVal) #nm},			\
    {XtNlabel, (XtArgVal) #lbl},		\
    {XtNcallback, (XtArgVal) nm##Callbacks}	\
};
#else
#define BUTTON(nm,lbl)				\
static XtCallbackRec nm/**/Callbacks[] = {	\
    {nm/**/Function, NULL},			\
    {NULL, NULL}				\
};						\
static Arg nm/**/Args[] = {			\
    {XtNname, (XtArgVal) "nm"},			\
    {XtNlabel, (XtArgVal) "lbl"},		\
    {XtNcallback, (XtArgVal) nm/**/Callbacks}	\
};
#endif
#endif


BUTTON(dismiss,dismiss);

void
mesgPane(fmtString, va_alist)
char *fmtString;
va_dcl
/*
 * brings up a new vertical pane, not moded
 *
 * the pane consists of 3 parts: title bar, scrollable text window,
 * button box
 */
{
    va_list args;
    Widget pane, buttonBox, label;
    Arg fontArgs[1];
    Arg paneArgs[4];
    static Arg labelArgs[] = {
	{XtNlabel, (XtArgVal) "Information (can be left up or dismissed)"},
	{XtNskipAdjust, (XtArgVal) True},
    };
    static Arg boxArgs[] = {
	{XtNskipAdjust, (XtArgVal) True},
    };
    static Arg shellArgs[] = {
	{XtNinput, (XtArgVal) True},
	{XtNsaveUnder, (XtArgVal) False},
	{XtNheight,  (XtArgVal) 150},
	{XtNwidth,  (XtArgVal) 500},
    };
    static Arg textArgs[] = {
	{XtNstring,  (XtArgVal) MesgString},
	{XtNlength,  (XtArgVal) sizeof(MesgString) + 1},
	{XtNtextOptions, (XtArgVal) (scrollVertical|wordBreak)},
	{XtNeditType,  (XtArgVal) XawtextEdit},
    };

    va_start(args);
    if ((XRNState & XRN_X_UP) != XRN_X_UP) {
	(void) vfprintf(stderr, fmtString, args);
	(void) fprintf(stderr, "\n");
	return;
    }

    if (MesgTopLevel == (Widget) 0) {
	MesgTopLevel = XtCreatePopupShell("Information", topLevelShellWidgetClass,
					  topLevel, shellArgs, XtNumber(shellArgs));

	XtSetArg(paneArgs[0], XtNx, 0);
	XtSetArg(paneArgs[1], XtNy, 0);
	XtSetArg(paneArgs[2], XtNwidth, 0);
	XtSetArg(paneArgs[3], XtNheight, 0);
	XtGetValues(topLevel, paneArgs, XtNumber(paneArgs));
	pane = XtCreateManagedWidget("pane", panedWidgetClass, MesgTopLevel,
				     paneArgs, XtNumber(paneArgs));

	(void) vsprintf(MesgString, fmtString, args);

	label = XtCreateManagedWidget("label", labelWidgetClass, pane,
				      labelArgs, XtNumber(labelArgs));

	MesgText = XtCreateManagedWidget("text", asciiStringWidgetClass, 
					 pane, textArgs, XtNumber(textArgs));

	buttonBox = XtCreateManagedWidget("box", boxWidgetClass, 
					 pane, boxArgs, XtNumber(boxArgs));

	XtCreateManagedWidget("dismiss", commandWidgetClass, buttonBox,
			      dismissArgs, XtNumber(dismissArgs));
    
	XtRealizeWidget(MesgTopLevel);

	XtSetArg(fontArgs[0], XtNheight, 0);
	XtGetValues(label, fontArgs, XtNumber(fontArgs));
	XawPanedSetMinMax(label, (int) fontArgs[0].value, (int) fontArgs[0].value);
	XawPanedAllowResize(MesgText, True);
	
	XDefineCursor(XtDisplay(MesgTopLevel), XtWindow(MesgTopLevel),
		      XCreateFontCursor(XtDisplay(MesgTopLevel), XC_left_ptr));

	/* xthCenterWidgetOverCursor(MesgTopLevel); */
	XtPopup(MesgTopLevel, XtGrabNone);

	XawTextSetLastPos(MesgText, (XawTextPosition) utStrlen(MesgString));

    } else {
	long len;
	long newlen;
	char addBuff[MESG_SIZE];

	(void) vsprintf(addBuff, fmtString, args);
	len = utStrlen(MesgString);
	newlen = utStrlen(addBuff);

	if ((len + 8 + newlen) > MESG_SIZE) {
	    (void) strcpy(MesgString, addBuff);
	    XawTextSetLastPos(MesgText, (XawTextPosition) newlen);
	    XawTextSetInsertionPoint(MesgText, (XawTextPosition) newlen);
	    XawTextInvalidate(MesgText, 0, newlen);
	    return;
	}

	(void) strcat(&MesgString[len], "\n------\n");
	(void) strcat(&MesgString[len+8], addBuff);
	XawTextSetLastPos(MesgText,
		                (XawTextPosition) (len + 8 + newlen));
	XawTextSetInsertionPoint(MesgText,
		                (XawTextPosition) (len + 8 + newlen));

    }
    
    return;
}


void
info(msg)
char *msg;
/*
 * put an informational 'msg' into the top information label
 */
{
    Arg infoLineArg[1];

    if ((XRNState && XRN_X_UP) == XRN_X_UP) {
	XtSetArg(infoLineArg[0], XtNlabel, msg);
	XtSetValues(TopInfoLine, infoLineArg, XtNumber(infoLineArg));
    } else {
	(void) fprintf(stderr, "XRN: %s\n", msg);
    }
    return;
}


void
infoNow(msg)
char *msg;
/*
 * put an informational 'msg' into the top information label and force an update
 */
{
    info(msg);
    if ((XRNState && XRN_X_UP) == XRN_X_UP) {
	xthHandleAllPendingEvents();
    }
    return;
}

