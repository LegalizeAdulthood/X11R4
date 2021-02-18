/* This file is part of xdir, an X-based directory browser.
 * It contains functions for the "Commands" menu.
 *
 *	Created: 2 April 1989
 *
 *	Win Treese
 *	Cambridge Research Lab
 *	Digital Equipment Corporation
 *
 *	$Source: /udir/treese/Src/Xdir/RCS/actions.c,v $
 *	$Athena$
 */

#if !defined(lint) && !defined(SABER)
static char rcsid_xdir_c[] = "$Header: actions.c,v 1.1 89/04/01 20:05:34 treese Exp $";
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

/* Function:	Delete() deletes a file.  It is called from the "Delete"
 *			menu item.
 * Arguments:	Standard Xt callback.
 * Returns:	Nothing.
 * Notes:
 */

/*ARGSUSED*/
static void
Delete(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	printf("simulated delete of %s\n", CurrentSelectionName);
}

/* Function:	Copy() makes a copy of a file.  It is called from the "Copy"
 *			item of the Commands menu.
 * Arguments:	Standard Xt callback.
 * Returns:	Nothing.
 * Notes:
 */

/*ARGSUSED*/
static void
Copy(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	printf("simulated copy of %s\n", CurrentSelectionName);
}

/* Function:	Rename() renames the selected file.  It is called from the
 *			"Rename" item of the Commands menu.
 * Arguments:	Standard Xt callback.
 * Returns:	Nothing.
 * Notes:
 */

/*ARGSUSED*/
static void
Rename(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	printf("simulated rename of %s\n", CurrentSelectionName);
}

/* Function:	Edit() edits the selected file.  It is called from the "Edit"
 *			item of the Commands menu.
 * Arguments:	Standard Xt callback.
 * Returns:	Nothing.
 * Notes:
 */

/*ARGSUSED*/
static void
Edit(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	DisplayFile(CurrentSelectionName, EDIT);
}

/* Function:	InitCommandsMenu() initializes the "Commands" menu.
 * Arguments:	parent:	Parent widget to use.
 * Returns:	Nothing.
 * Notes:
 */

void
InitCommandsMenu(parent)
Widget parent;
{
	Widget menu;
	Widget command;

	command = XtCreateManagedWidget("command", menuButtonWidgetClass,
					parent,	NULL, (Cardinal) 0);

	menu = XtCreatePopupShell("commandMenu", simpleMenuWidgetClass,
				  TopWidget, NULL, (Cardinal) 0);

	AddMenuEntry(menu, "delete", Delete);
	AddMenuEntry(menu, "copy", Copy);
	AddMenuEntry(menu, "rename", Rename);
	AddMenuEntry(menu, "edit", Edit);
}
