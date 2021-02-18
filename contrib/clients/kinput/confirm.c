/*
 *	confirm.c
 */

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
 *		ishisone@sra.co.jp
 */

#ifndef lint
static char *rcsid = "$Header: /private/usr/mmb/ishisone/job/x11/kinput/RCS/confirm.c,v 1.2 89/10/27 17:15:51 ishisone Rel $";
#endif
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>

static Widget
setupConfirm(toplevel, title, message, ybuttonp, nbuttonp)
Widget toplevel;
char *title;
char *message;
Widget *ybuttonp;
Widget *nbuttonp;
{
	static Widget	shell, tlabel, mlabel, ybutton, nbutton;

	if (shell == NULL) {
		Widget	form;
		Dimension	width, height;
		Screen		*scr = XtScreen(toplevel);
		Arg	args[10];
		int	i;

		XtSetArg(args[0], XtNinput, True);
		shell = XtCreatePopupShell("confirmshell",
					   transientShellWidgetClass,
					   toplevel, args, 1);
		form = XtCreateManagedWidget("form", formWidgetClass,
					     shell, NULL, 0);
		i = 0;
		XtSetArg(args[i], XtNlabel, title); i++;
		XtSetArg(args[i], XtNborderWidth, 0); i++;
		tlabel = XtCreateManagedWidget("title",
					       labelWidgetClass, form,
					       args, i);
		i = 0;
		XtSetArg(args[i], XtNlabel, message); i++;
		XtSetArg(args[i], XtNborderWidth, 0); i++;
		XtSetArg(args[i], XtNfromVert, tlabel); i++;
		XtSetArg(args[i], XtNvertDistance, 10); i++;
		mlabel = XtCreateManagedWidget("message",
					       labelWidgetClass, form,
					       args, i);
		i = 0;
		XtSetArg(args[i], XtNlabel, "Yes"); i++;
		XtSetArg(args[i], XtNfromVert, mlabel); i++;
		XtSetArg(args[i], XtNvertDistance, 10); i++;
		ybutton = XtCreateManagedWidget("button",
						commandWidgetClass, form,
						args, i);
		i = 0;
		XtSetArg(args[i], XtNlabel, "No"); i++;
		XtSetArg(args[i], XtNfromVert, mlabel); i++;
		XtSetArg(args[i], XtNvertDistance, 10); i++;
		XtSetArg(args[i], XtNfromHoriz, ybutton); i++;
		XtSetArg(args[i], XtNhorizDistance, 10); i++;
		nbutton = XtCreateManagedWidget("button",
						commandWidgetClass, form,
						args, i);
		XtRealizeWidget(shell);
		i = 0;
		XtSetArg(args[i], XtNwidth, &width); i++;
		XtSetArg(args[i], XtNheight, &height); i++;
		XtGetValues(shell, args, i);
		XtMoveWidget(shell,
			     (WidthOfScreen(scr) - width) / 2,
			     (HeightOfScreen(scr) - height) / 2);
	} else {
		Arg	arg;
		XtSetArg(arg, XtNlabel, title);
		XtSetValues(tlabel, &arg, 1);
		XtSetArg(arg, XtNlabel, message);
		XtSetValues(mlabel, &arg, 1);
	}

	*ybuttonp = ybutton;
	*nbuttonp = nbutton;

	return shell;
}

/* ARGSUSED */
static void
callbackYes(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	int	*statep = (int *)client_data;
	*statep = 1;
}

/* ARGSUSED */
static void
callbackNo(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	int	*statep = (int *)client_data;
	*statep = -1;
}

int
doConfirm(toplevel, title, message)
Widget toplevel;
char *title;
char *message;
{
	Widget	shell, yesw, now;
	XEvent	event;
	int	state;

	shell = setupConfirm(toplevel, title, message, &yesw, &now);
	XtAddCallback(yesw, XtNcallback, callbackYes, (caddr_t)&state);
	XtAddCallback(now, XtNcallback, callbackNo, (caddr_t)&state);

	state = 0;
	XtPopup(shell, XtGrabExclusive);

	while (!state) {
		XtNextEvent(&event);
		XtDispatchEvent(&event);
	}
	XtRemoveCallback(yesw, XtNcallback, callbackYes, (caddr_t)&state);
	XtRemoveCallback(now, XtNcallback, callbackNo, (caddr_t)&state);
	XtPopdown(shell);

	return state > 0;
}
