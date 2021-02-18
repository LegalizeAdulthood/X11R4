#ifndef lint
static char	*RCSid = "$Header$";
#endif

/*
 * menu.c - routines to handle the menu.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94086
 * davy@riacs.edu
 *
 * $Log$
 */
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/List.h>
#include <stdio.h>

#include "xpostit.h"

Widget	menuwidget;
Widget	listwidget;

static String menustrings[] = {
#define MenuCreate_1p5x2	0
	"Create 1.5x2 Note",
#define MenuCreate_3x3		1
	"Create 3x3 Note  ",
#define MenuCreate_3x5		2
	"Create 3x5 Note  ",
#define MenuRaiseAll		3
	"Raise All Notes  ",
#define MenuLowerAll		4
	"Lower All Notes  ",
#define MenuSaveAll		5
	"Save All Notes   ",
#define MenuExit		6
	"Exit             "
};

static void	LeaveListWidget();
static void	HandleListSelection();

/*
 * CreateMenuWidget - create the widget used for the menu.
 */
void
CreateMenuWidget()
{
	Arg args[8];
	register int nargs;
	XtCallbackRec callbacks[2];

	/*
	 * Set the callback.
	 */
	bzero(callbacks, sizeof(callbacks));
	SetCallback(HandleListSelection, NULL);

	/*
	 * The menu is done with a popup shell.
	 */
	menuwidget = XtCreatePopupShell("Menu", overrideShellWidgetClass,
			toplevel, NULL, 0);

	/*
	 * Create the list widget.
	 */
	nargs = 0;
	SetArg(XtNdefaultColumns, 1);
	SetArg(XtNlist, menustrings);
	SetArg(XtNcallback, callbacks);
	SetArg(XtNnumberStrings, XtNumber(menustrings));

	listwidget = XtCreateWidget("menu", listWidgetClass, menuwidget,
			args, nargs);

	/*
	 * Add an event handler for leaving the list window.
	 */
	XtAddEventHandler(listwidget, LeaveWindowMask, False, LeaveListWidget,
			(caddr_t) NULL);

	/*
	 * Let the top level shell know we're here.
	 */
	XtManageChild(listwidget);
	XtRealizeWidget(menuwidget);
}

/*
 * HandleListSelection - callback from list widget to handle a selection.
 */
static void
HandleListSelection(w, client_data, call_data)
caddr_t client_data, call_data;
Widget w;
{
	XtListReturnStruct *xlrs;

	xlrs = (XtListReturnStruct *) call_data;

	/*
	 * Get rid of the menu.
	 */
	XtListUnhighlight(listwidget);
	XtPopdown(menuwidget);

	/*
	 * Dispatch the command.
	 */
	switch (xlrs->index) {
	case MenuCreate_1p5x2:
		CreateNewNote(PostItNote_1p5x2);
		break;
	case MenuCreate_3x3:
		CreateNewNote(PostItNote_3x3);
		break;
	case MenuCreate_3x5:
		CreateNewNote(PostItNote_3x5);
		break;
	case MenuRaiseAll:
		RaiseAllNotes();
		break;
	case MenuLowerAll:
		LowerAllNotes();
		break;
	case MenuSaveAll:
		SaveAllNotes();
		break;
	case MenuExit:
		ByeBye();
		break;
	}
}

/*
 * LeaveListWidget - lower the menu when the mouse leaves the list window.
 */
static void
LeaveListWidget(w, client_data, event)
caddr_t client_data;
XEvent *event;
Widget w;
{
	XtListUnhighlight(listwidget);
	XtPopdown(menuwidget);
}
