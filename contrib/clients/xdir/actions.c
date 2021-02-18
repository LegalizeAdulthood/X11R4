/* This file is part of xdir, an X-based directory browser.
 * It contains functions for the "Action" menu.
 *
 *	Created: 10/8/88
 *
 *	Win Treese
 *	Cambridge Research Lab/MIT Project Athena
 *	Digital Equipment Corporation
 *
 *	$Source: /udir/treese/Src/Xdir/RCS/actions.c,v $
 *	$Athena$
 */

#if !defined(lint) && !defined(SABER)
static char rcsid_xdir_c[] = "$Header: actions.c,v 1.2 89/08/19 13:26:24 treese Locked $";
#endif

#include <stdio.h>
#include <sys/param.h>
#include <X11/Xos.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/MenuButton.h>

#include "xdir.h"

/*ARGSUSED*/
static void
Update(w, client_data, ret_val)
Widget w;
caddr_t client_data;
caddr_t ret_val;
{
	DoChangeDir(NULL);

}

/* Function:	Quit() exits the program.  It is called from the "QUIT" button.
 * Arguments:	Standard Xt callback.
 * Returns:	Nothing.
 * Notes:
 */

/*ARGSUSED*/
static void
Quit(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	exit(0);
}

/* Function:	ViewFile() displays a selected file.
 * Arguments:	Standard Xt callback arguments.
 * Returns:	Nothing.
 * Notes:
 */

/*ARGSUSED*/
static void
ViewFile(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	if (CurrentSelectionName[0] != EOS)
		DisplayFile(CurrentSelectionName, NOEDIT);
	else
		Error("You have not selected a file to view.\n", NULL);
}

/*ARGSUSED*/
static void
AboutXdir(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	ShowHelp(AboutFile, "About Xdir");
}

/* Function:	ChangeDir() changes to the selected directory.
 * Arguments:	Standard Xt callback arguments.
 * Returns:	Nothing.
 * Notes:
 */

/*ARGSUSED*/
static void
ChangeDir(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	if (CurrentSelectionName[0] == EOS) {
		Error("No directory selected.", "");
		return;
	}
	if (! IsDirectory(CurrentSelectionName)) {
		Error(CurrentSelectionName, "is not a directory.");
		return;
	}
	DoChangeDir(CurrentSelectionName);
}

/* Function:	ParentDir() changes to the parent directory.
 * Arguments:	Standard Xt callback arguments.
 * Returns:	Nothing.
 * Notes:
 */

/*ARGSUSED*/
static void
ParentDir(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	DoChangeDir("..");
}

/* Function:	DoChangeDir() actually changes the directory and displays
 *		the new listing.
 * Arguments:	dir:	Pathname of new directory.
 * Returns:	Nothing.
 * Notes:
 *	NULL for dir means to rebuild the file list for the current directory
 *	(as in an update to the directory or change in filename filter).
 */

void
DoChangeDir(dir)
char *dir;
{
	char **filelist;
	char *p;
	Arg args[10];
	Cardinal arg_cnt;
	char newdir[MAXPATHLEN];

	strcpy(newdir, CurrentDirectory);
	if (dir != NULL) {
		if ( !strcmp(dir, "..")) {	/* Parent directory. */
			p = rindex(newdir, '/');
			*p = EOS;
			if (newdir[0] == EOS)
			  strcpy(newdir, "/");
		}
		else	{
			if ( strcmp(newdir, "/") ) /* At the root already */
			  strcat(newdir, "/");
			strcat(newdir, dir);
		}
	}
	if ( (filelist = MakeFileList(newdir)) == NULL) {
		Error("Unable to list directory", newdir);
		return;
	}

	strcpy(CurrentDirectory, newdir);
	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNlabel, CurrentDirectory); arg_cnt++;
	XtSetValues(DirLabel, args, arg_cnt);

	SetDirIconName(CurrentDirectory);
	XawListChange(MyListWidget, filelist, 0, 0, True);
	CurrentSelectionName[0] = '\0';
}

/*ARGSUSED*/
static void
ItemPicked(w, event, params, param_count)
Widget w;
XEvent *event;
String *params;
Cardinal *param_count;
{
	if (IsDirectory(CurrentSelectionName))
	  DoChangeDir(CurrentSelectionName);
	else
	  DisplayFile(CurrentSelectionName, NOEDIT);
}


/* The following static tables support the translation table changes.
 * They are down here in order to take advantage of the functions declarations
 * above.
 */

static XtActionsRec actionTable[] = {
  {"ParentDir", ParentDir},
  {"ItemPicked", ItemPicked},
};

static Cardinal action_count = sizeof(actionTable)/sizeof(XtActionsRec);

/* Function:    InitButtonActions() initializes other mouse actions.
 * Arguments:   None.
 * Returns:     Nothing.
 * Notes:
 */

void
InitButtonActions()
{
        XtAddActions(actionTable, action_count);
}

/* Function:	InitActionsMenu() initializes the "Actions" menu.
 * Arguments:	parent:	Parent widget to use.
 * Returns:	Nothing.
 * Notes:
 */

void
InitActionsMenu(parent)
Widget parent;
{
	Widget menu;
	Widget action;

	action = XtCreateManagedWidget("action", menuButtonWidgetClass, parent,
					NULL, (Cardinal) 0);

	menu = XtCreatePopupShell("actionMenu", simpleMenuWidgetClass,
				  TopWidget, NULL, (Cardinal) 0);

	AddMenuEntry(menu, "view", ViewFile);
	AddMenuEntry(menu, "chdir", ChangeDir);
	AddMenuEntry(menu, "parentdir", ParentDir);
	AddMenuEntry(menu, "about", AboutXdir);
	AddMenuEntry(menu, "update", Update);
	AddMenuEntry(menu, "quit", Quit);
}
