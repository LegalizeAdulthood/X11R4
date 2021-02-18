
#ifndef lint
static char XRNrcsid[] = "$Header: /users/ricks/xrn-11/R4/RCS/dialogs.c,v 1.15 89/11/17 12:51:14 ricks Rel $";
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
 * dialogs.c: create simple moded dialog boxes
 */

#include "copyright.h"
#include <X11/Xos.h>
#include "config.h"
#include "utils.h"
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Dialog.h>
#include "xthelper.h"
#include "xmisc.h"
#include "dialogs.h"

#ifdef VMS
Widget MyNameToWidget();
#define XtNameToWidget MyNameToWidget
#endif /* VMS */

Widget
CreateDialog(parent, title, textField, args, count)
Widget parent;           /* parent window                         */
char *title;             /* title of the dialog box               */
char *textField;	 /* default text field                    */
struct DialogArg *args;  /* description of the buttons            */
unsigned int count;      /* number of buttons                     */
/*
 * create a simple moded dialog box
 */
{
    Widget popup, dialog;
    Arg dargs[3];
    int i;
    Widget typein;
    Widget bb;
    Arg bbArgs[3];
    static char *accel = "#override\n\
                          <Key>0xff0a: set() notify() unset()\n\
                          <Key>0xff0d: set() notify() unset()";
    static Arg shellArgs[] = {
	{XtNallowShellResize, (XtArgVal) True},
	{XtNinput, (XtArgVal) True},
    };

    /* override does not get titlebar, transient does */
    popup = XtCreatePopupShell("popup", transientShellWidgetClass, parent,
			       shellArgs, XtNumber(shellArgs));
    
    /* create the dialog box */
    XtSetArg(dargs[0], XtNvalue, textField);
    XtSetArg(dargs[1], XtNlabel, title);
    XtSetArg(dargs[2], XtNinput, True);
    dialog = XtCreateManagedWidget("dialog", dialogWidgetClass, popup,
					     dargs, XtNumber(dargs));

    /* add the buttons */
    for (i = 0; i < count; i++) {
	Arg bargs[2];
	Widget button;
	static XtCallbackRec callbacks[] = {
	    {busyCursor, NULL},
	    {NULL, NULL},
	    {unbusyCursor, NULL},
	    {NULL, NULL},
	};

	callbacks[1].callback = args[i].handler;
	callbacks[1].closure = args[i].data;

	XtSetArg(bargs[0], XtNlabel, args[i].buttonName);
	XtSetArg(bargs[1], XtNcallback, callbacks);

	button = XtCreateManagedWidget("command", commandWidgetClass, dialog,
					 bargs, XtNumber(bargs));

	if (i == count - 1) {
	    bb = button;
	}
    }

    if ((typein = XtNameToWidget(dialog, "value")) != 0) {
	XtSetKeyboardFocus(dialog, typein);
    }

    XtSetArg(bbArgs[0], XtNaccelerators, 0);
    XtSetArg(bbArgs[1], XtNborderWidth, 0);
    XtSetArg(bbArgs[2], XtNhighlightThickness, 0);
    XtGetValues(bb, bbArgs, XtNumber(bbArgs));

    XtSetArg(bbArgs[0], XtNaccelerators, XtParseAcceleratorTable(accel));
    XtSetArg(bbArgs[1], XtNborderWidth, bbArgs[1].value + 1);
    XtSetArg(bbArgs[2], XtNhighlightThickness, bbArgs[1].value + 1);
    XtSetValues(bb, bbArgs, XtNumber(bbArgs));
	
    XtRealizeWidget(popup);

#ifndef ACCELERATORBUG
    XtInstallAccelerators(dialog, bb);
    XtInstallAccelerators(popup, bb);
#endif
    if (typein != 0) {
	XtInstallAccelerators(typein, bb);
    }

    return(popup);
}


void
FocusPopUp(popup, data, event)
Widget popup;
caddr_t data;
XEvent *event;
{
    if (event->type == MapNotify) {
	XSetInputFocus(XtDisplay(popup), XtWindow(popup),
		     RevertToPointerRoot,  CurrentTime);
	XtRemoveEventHandler(popup, XtAllEvents, True, FocusPopUp, 0);
    }
    return;
}


void
PopUpDialog(popup)
Widget popup;
{
    xthCenterWidgetOverCursor(popup);
    XtPopup(popup, XtGrabExclusive);
    XtAddEventHandler(popup, StructureNotifyMask, False, FocusPopUp, 0);
    return;
}


void
PopDownDialog(dialog)
Widget dialog;
/*
 * pop down the dialog (do not destroy, it will be used again)
 */
{
    Window focus;
    int revert;

    XtPopdown(dialog);
    XtDestroyWidget(dialog);
    return;
}


char *
GetDialogValue(popup)
Widget popup;
{
    return XawDialogGetValueString(XtNameToWidget(popup, "dialog"));
}


/*
 * simple confirmation box
 */

static int retVal;

static void
cbHandler(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    retVal = (int) client_data;
    return;
}


int
ConfirmationBox(parent, message)
Widget parent;
char *message;
/*
 * pop up a confirmation box and return either 'XRN_CB_ABORT' or 'XRN_CB_CONTINUE'
 */
{
    static struct DialogArg args[] = {
	{"no", cbHandler, (caddr_t) XRN_CB_ABORT},
	{"yes", cbHandler, (caddr_t) XRN_CB_CONTINUE},
    };
    XEvent ev;
    Widget widget;

    retVal = -1;

    widget = CreateDialog(parent, message, DIALOG_NOTEXT, args, XtNumber(args));
    PopUpDialog(widget);

    for(;;) {
	XtNextEvent(&ev);
	(void) XtDispatchEvent(&ev);
	if (retVal != -1) {
	    PopDownDialog(widget);
	    XtDestroyWidget(widget);
	    /* xthHandleAllPendingEvents(); */
	    return(retVal);
	}
    }
}

