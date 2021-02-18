/* This file is part of xdir, an X-based directory browser.
 * It contains functions for displaying help.
 *
 *	Created: 9/5/88
 *
 *	Win Treese
 *	Cambridge Research Lab/MIT Project Athena
 *	Digital Equipment Corporation
 *
 *	$Source: /udir/treese/Src/Xdir/RCS/help.c,v $
 *	$Athena: help.c,v 1.1 88/09/16 01:29:55 treese Locked $
 */

#if !defined(lint) && !defined(SABER)
static char rcsid_help_c[] = "$Header: help.c,v 2.1 89/03/25 02:48:08 treese Locked $";
#endif

#include <stdio.h>
#include <sys/param.h>
#include <X11/Xos.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/VPaned.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Label.h>

#include "xdir.h"

/* Local variables. */

static Widget HelpShell = NULL;

/*ARGSUSED*/
static void
Help(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	ShowHelp(HelpFile, "Xdir Help");
}

/* Function:	HelpClose() closes a help window.
 * Arguments:	Standard Xt callback arguments.
 * Returns:	Nothing.
 * Notes:
 */

/*ARGSUSED*/
void
HelpClose(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	XtPopdown(HelpShell);
}

/* Function:	CreateHelpWidget() creates the help widget.
 * Arguments:	None.
 * Returns:	Nothing.
 * Notes:
 */

static void
CreateHelpWidget()
{
	Arg args[10];
	Cardinal arg_cnt;
	Widget vpane, box, close;

	HelpShell = XtCreatePopupShell("help", topLevelShellWidgetClass,
					     TopWidget,
					     NULL, 0);

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNgripIndent, -20); arg_cnt++;
  	vpane = XtCreateManagedWidget("vpane", vPanedWidgetClass,
				     HelpShell, args, arg_cnt);

	arg_cnt = 0;
	(void) XtCreateManagedWidget("helplabel", labelWidgetClass, 
				      vpane, args, arg_cnt);

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNfile, "/dev/null"); arg_cnt++;
	XtSetArg(args[arg_cnt], XtNtextOptions, scrollVertical); arg_cnt++;
	(void) XtCreateManagedWidget("text", asciiDiskWidgetClass,
				     vpane, args, arg_cnt);

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNskipAdjust, True); arg_cnt++;
	box = XtCreateManagedWidget("box", boxWidgetClass, vpane,
					args, arg_cnt);

	arg_cnt = 0;
        close = XtCreateManagedWidget("close", commandWidgetClass, box,
					args, arg_cnt);
	XtAddCallback(close, XtNcallback, HelpClose, NULL);

	XtRealizeWidget(HelpShell);
}

/* Function:	ShowHelp() displays a help window.
 * Arguments:	helpfile:	File to display in the window.
 *		label:		Label for window.
 * Returns:	Nothing.
 * Notes:
 */

void
ShowHelp(helpfile, label)
char *helpfile;
char *label;
{
	Arg args[10];
	Cardinal arg_cnt;

	if (HelpShell == NULL)
		CreateHelpWidget();


	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNlabel, label); arg_cnt++;
	XtSetValues(XtNameToWidget(HelpShell, "vpane.helplabel"),
		    args, arg_cnt);

	SetTextFile(XtNameToWidget(HelpShell, "vpane.text"), helpfile);
	XtPopup(HelpShell, XtGrabNone);
}

/* Function:	InitHelp() initializes the help subsystem.
 * Arguments:	parent:	Parent widget for "Help" button.
 * Returns:	Nothing.
 * Notes:
 */

void
InitHelp(parent)
Widget parent;
{
	Cardinal arg_cnt;
	Arg args[10];
	Widget w;

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNlabel, "Help"); arg_cnt++;
        w = XtCreateManagedWidget("help", commandWidgetClass, parent,
					args, arg_cnt);
	XtAddCallback(w, XtNcallback, Help, NULL);
}
