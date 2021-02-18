/* This file is part of xdir, an X-based directory browser.
 * It contains functions for handling error messages.
 *
 *	Created: 9/3/88
 *
 *	Win Treese
 *	Cambridge Research Lab/MIT Project Athena
 *	Digital Equipment Corporation
 *
 *	$Source: /udir/treese/Src/Xdir/RCS/error.c,v $
 *	$Athena: error.c,v 1.1 88/09/16 01:29:51 treese Locked $
 */

#if !defined(lint) && !defined(SABER)
static char rcsid_util_c[] = "$Header: error.c,v 2.1 89/03/25 02:48:07 treese Locked $";
#endif

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <X11/Xos.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Command.h>

#include "xdir.h"

/* Static variables. */

static Widget ErrorShell;
static Boolean errorInProgress;

/* Function:	ErrorClose() closes the dialog widget.
 * Arguments:	Standard Xt callback.
 * Returns:	Nothing.
 * Notes:
 */

/*ARGSUSED*/
void
ErrorClose(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	XtPopdown(ErrorShell);
	XtDestroyWidget(ErrorShell);
	errorInProgress = False;
}

/* Function:	ErrAbort() prints an error message and exits the program.
 * Arguments:	msg:	Error message to print.
 * Returns:	Doesn't return.
 * Notes:
 */

void
ErrAbort(msg)
char *msg;
{
	fprintf(stderr, msg);
	exit(1);
}

/* Function:	Error() displays an error message.
 * Arguments:	msg:	Message to be displayed.
 *		arg:	Additional information.
 * Returns:	Nothing.
 * Notes:
 */

void
Error(msg, arg)
char *msg;
char *arg;
{
	char message[BUFSIZ];
	Arg args[10];
	Cardinal arg_cnt;
	Widget w, dialog;
	int x, y;
	Position x_root, y_root;

	strcpy(message, msg);
	if (arg != NULL) {
	  strcat(message, " ");
	  strcat(message, arg);
	}

	if (errorInProgress) {
		fprintf(stderr, "An error occurred while an error requestor");
		fprintf(stderr, " is being displayed.\n");
		fprintf(stderr, "The new error is:\n%s", message);
	}
	else
		errorInProgress = True;

	ErrorShell = XtCreatePopupShell("ErrorShell",
					     transientShellWidgetClass,
					     TopWidget,
					     NULL, 0);
	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNlabel, message); arg_cnt++;
	dialog = XtCreateManagedWidget("ErrorDialog", dialogWidgetClass,
					    ErrorShell, args, arg_cnt);

	arg_cnt = 0;
        w = XtCreateManagedWidget("close", commandWidgetClass, dialog,
					args, arg_cnt);
	XtAddCallback(w, XtNcallback, ErrorClose, NULL);

	XtRealizeWidget(ErrorShell);

	/* Center the dialog box in the directory widget. */

	x = (GetWidgetWidth(TopWidget) - GetWidgetWidth(ErrorShell)) / 2;
	y = (GetWidgetHeight(TopWidget) - GetWidgetHeight(ErrorShell)) / 2;
	if (x < 0) x = 0;
	if (y < 0) y = 0;

	XtTranslateCoords(TopWidget, x, y, &x_root, &y_root);

	XtSetArg(args[arg_cnt], XtNx, x_root); arg_cnt++;
	XtSetArg(args[arg_cnt], XtNy, y_root); arg_cnt++;
	XtSetValues(ErrorShell, args, arg_cnt);

#ifdef notdef
/* REMOVE WHEN TOOLKIT BUG IS FIXED.  We shouldn't use XtMoveWidget at all.
 * Use the SetValues. */

	XtMoveWidget(ErrorShell, x_root, y_root);
#endif
	XtPopup(ErrorShell, XtGrabNonexclusive);
}
