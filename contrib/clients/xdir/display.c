/* This file is part of xdir, an X-based directory browser.
 * It contains functions for displaying a file.
 *
 *	Created: 8/13/88
 *
 *	Win Treese
 *	Cambridge Research Lab/MIT Project Athena
 *	Digital Equipment Corporation
 *
 *	$Source: /udir/treese/Src/Xdir/RCS/display.c,v $
 *	$Athena: display.c,v 1.1 88/09/16 01:29:46 treese Exp $
 */

#if !defined(lint) && !defined(SABER)
static char rcsid_display_c[] = "$Header: display.c,v 2.2 89/08/19 13:20:17 treese Locked $";
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

static Widget TextShell;
static Widget TextBox;
static Widget Text;
static Widget TextVPane;
static Boolean textPopped;
static Widget TextLabel;

/* Function:	TextHelp() displays help on the file display widget.
 * Arguments:	Standard Xt callback arguments.
 * Returns:	Nothing
 * Notes:
 */

/*ARGSUSED*/
void
TextHelp(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	ShowHelp(ViewHelpFile, "Xdir: Help on Viewing Files");
}

/* Function:	TextClose() closes a file display window.
 * Arguments:	Standard Xt callback arguments.
 * Returns:	Nothing.
 * Notes:
 */

/*ARGSUSED*/
void
TextClose(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	XtPopdown(TextShell);
	textPopped = False;
}

/* Function:	InitTextDisplay() initializes the text display windows.
 * Arguments:	None.
 * Returns:	Nothing.
 * Notes:
 */

void
InitTextDisplay()
{
	Arg args[10];
	Cardinal arg_cnt;
	Widget w;

	TextShell = XtCreatePopupShell("TextShell",
					     topLevelShellWidgetClass,
					     TopWidget,
					     NULL, 0);

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNgripIndent, -20); arg_cnt++;
  	TextVPane = XtCreateManagedWidget("text-vpane", vPanedWidgetClass,
				     TextShell, args, arg_cnt);

	arg_cnt = 0;
	TextLabel = XtCreateManagedWidget("textname", labelWidgetClass, 
				      TextVPane, args, arg_cnt);

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNfile, "/dev/null"); arg_cnt++;
	XtSetArg(args[arg_cnt], XtNtextOptions, scrollVertical); arg_cnt++;
	Text = XtCreateManagedWidget("ViewText", asciiDiskWidgetClass,
				     TextVPane,	args, arg_cnt);

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNskipAdjust, True); arg_cnt++;
	TextBox = XtCreateManagedWidget("text-Box", boxWidgetClass, TextVPane,
					args, arg_cnt);

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNlabel, "Close"); arg_cnt++;
        w = XtCreateManagedWidget("close", commandWidgetClass, TextBox,
					args, arg_cnt);
	XtAddCallback(w, XtNcallback, TextClose, NULL);

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNlabel, "Help"); arg_cnt++;
        w = XtCreateManagedWidget("text-help", commandWidgetClass, TextBox,
					args, arg_cnt);
	XtAddCallback(w, XtNcallback, TextHelp, NULL);
	XtRealizeWidget(TextShell);
}

/* Function:	DisplayFile() displays a text file.
 * Arguments:	filename:	Name of file to display.
 * Returns:	Nothing.
 * Notes:
 */

void
DisplayFile(filename, edit)
char *filename;
Boolean edit;
{
	Arg args[10];
	Cardinal arg_cnt;
	char fullpath[MAXPATHLEN];
	int access_mode;

	MakeFullPath(filename, fullpath);

	access_mode = R_OK | ((edit) ? W_OK : 0);
	if ( access(fullpath, access_mode) ) {
		Error("Cannot open file", fullpath);
		return;
	}

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNlabel, fullpath); arg_cnt++;
	XtSetValues(TextLabel, args, arg_cnt);

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNeditType,
		 ((edit) ? XawtextEdit : XawtextRead)); arg_cnt++;
	XtSetValues(Text, args, arg_cnt);

	SetTextFile(Text, fullpath);

	if (! textPopped) {
		XtPopup(TextShell, XtGrabNone);
		textPopped = True;
	}
}

/* Function:	SetTextFile() sets the text file of the specified widget.
 * Arguments:	w:		Widget to set.
 *		filename:	Name of filename.
 * Returns:	Nothing.
 * Notes:
 */

void
SetTextFile(w, filename)
Widget w;
char *filename;
{
	Arg args[10];
	Cardinal arg_cnt;
	XawTextSource new_src;
	XawTextSource old_src;

	old_src = XawTextGetSource(w);
	
	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNfile, filename); arg_cnt++;
	new_src = XawDiskSourceCreate(w, args, arg_cnt);

	XawTextSetSource(w, new_src, (XawTextPosition) 0);
	XawDiskSourceDestroy(old_src);
}
