/* This file is part of xdir, an X-based directory browser.
 * It contains functions for handling the menus.
 *
 *	Created: 8/13/88
 *
 *	Win Treese
 *	Cambridge Research Lab
 *	Digital Equipment Corporation
 *
 *	$Source: /udir/treese/Src/Xdir/RCS/menus.c,v $
 *	$Athena: menus.c,v 1.1 88/09/16 01:29:36 treese Locked $
 */

#if !defined(lint) && !defined(SABER)
static char rcsid_menus_c[] = "$Header: menus.c,v 2.1 89/03/25 02:48:09 treese Exp $";
#endif

#include <stdio.h>
#include <X11/Xos.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Box.h>

#include "xdir.h"


/* Function:	InitMenus() initializes the menus.
 * Arguments:	parent:	Parent widget to use.
 * Returns:	Nothing.
 * Notes:
 */

void
InitMenus(parent)
Widget parent;
{
	Widget box;
	Arg args[10];
	Cardinal arg_cnt;

	arg_cnt = 0;
	box = XtCreateManagedWidget("box", boxWidgetClass, parent,
					args, arg_cnt);

	InitActionsMenu(box);
	InitConfigureMenu(box);
	InitCommandsMenu(box);
	InitHelp(box);
}

/* Function:	AddMenuEntry adds an entry to a menu.
 * Arguments:	menu:	Menu to be added to.
 *		name:	Name of the menu entry.
 *		callback: The callback routine for this entry.
 * Returns:	Nothing.
 * Notes:
 *	This is just a wrapper for the SimpleMenu functions.
 *	It does not handle adding resources other than callbacks.
 */

void
AddMenuEntry(menu, name, callback)
Widget menu;
String name;
XtCallbackProc callback;
{
	XawSimpleMenuAddEntry(menu, name, NULL, (Cardinal) 0);
	XawSimpleMenuAddEntryCallback(menu, name, callback, NULL);
}

