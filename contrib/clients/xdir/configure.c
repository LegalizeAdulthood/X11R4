/* This file is part of xdir, an X-based directory browser.
 * It contains functions for the "Configure" menu.
 *
 *	Created: 10/8/88
 *
 *	Win Treese
 *	Cambridge Research Lab/MIT Project Athena
 *	Digital Equipment Corporation
 *
 *	$Source: /udir/treese/Src/Xdir/RCS/configure.c,v $
 *	$Athena$
 */

#if !defined(lint) && !defined(SABER)
static char rcsid_configure_c[] = "$Header: configure.c,v 1.1 89/08/19 13:27:18 treese Locked $";
#endif

#include <stdio.h>
#include <X11/Xos.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>

#include "xdir.h"

/*ARGSUSED*/
static void
DotFiles(w, client_data, ret_val)
Widget w;
caddr_t client_data;
caddr_t ret_val;
{
	Arg args[10];
	Cardinal arg_cnt;
	char *label;

	if (ShowDotFiles) {
		ShowDotFiles = False;
		label = ShowDotFilesMsg;
	}
	else {
		ShowDotFiles = True;
		label = HideDotFilesMsg;
	}

	DoChangeDir(NULL);

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNlabel, label); arg_cnt++;
	XawSimpleMenuSetEntryValues(w, "dotfiles", args, arg_cnt);
}

/*ARGSUSED*/
static void
BackupFiles(w, client_data, ret_val)
Widget w;
caddr_t client_data;
caddr_t ret_val;
{
	Arg args[10];
	Cardinal arg_cnt;
	char *label;

	if (ShowBackupFiles) {
		ShowBackupFiles = False;
		label = ShowBackupFilesMsg;
	}
	else {
		ShowBackupFiles = True;
		label = HideBackupFilesMsg;
	}

	DoChangeDir(NULL);

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNlabel, label); arg_cnt++;
	XawSimpleMenuSetEntryValues(w, "backupfiles", args, arg_cnt);
}

/* Function:	InitConfigureMenu() initializes the "Configure" menu.
 * Arguments:	parent:	Parent widget to use.
 * Returns:	Nothing.
 * Notes:
 */

void
InitConfigureMenu(parent)
Widget parent;
{
	Cardinal arg_cnt;
	Arg args[10];
	Widget menu;
	Widget config;

	config = XtCreateManagedWidget("config", menuButtonWidgetClass, parent,
					NULL, (Cardinal) 0);

	menu = XtCreatePopupShell("configMenu", simpleMenuWidgetClass,
				  TopWidget, NULL, (Cardinal) 0);

	AddMenuEntry(menu, "dotfiles", DotFiles, NULL);
	AddMenuEntry(menu, "backupfiles", BackupFiles, NULL);

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNlabel,
		 ( ShowDotFiles ? HideDotFilesMsg : ShowDotFilesMsg));
	arg_cnt++;
	XawSimpleMenuSetEntryValues(menu, "dotfiles", args, arg_cnt);

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNlabel,
		 ( ShowBackupFiles ? HideBackupFilesMsg : ShowBackupFilesMsg));
	arg_cnt++;
	XawSimpleMenuSetEntryValues(menu, "backupfiles", args, arg_cnt);
}

