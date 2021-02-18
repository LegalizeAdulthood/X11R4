/* This file contains interface functions for the Menu widget. */

/***********************************************************
Copyright 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#include <stdio.h>
#include <strings.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Cardinals.h>
#include <X11/Command.h>
#include <X11/MenuShell.h>
#include <X11/MenuBox.h>
#include <X11/Box.h>
#include <X11/Label.h>

#include "Menu.h"

/*
 * call this to create the menu shell and box widgets, and the title.
 * MenuBind is called to bind this menu to an enabling widget with translation
 * BtnDown
 */

PopupMenu *
MenuCreate(parent, enable, name)
	Widget parent, enable;
	char *name;
{
	PopupMenu *menu;
	Arg args[MAX_ARGS];
	Cardinal i;
	void MenuBind();

	if ((menu = XtNew(PopupMenu)) == NULL) {
		(void) fprintf( stderr, "Cannot malloc new menu\n"); 
		perror("MenuCreate");
		return NULL;
	}
	menu->name = name;
	menu->enable = enable;
	menu->num_select = 0;

	MenuBind(enable, name);
	i = 0;
	XtSetArg( args[i], XtNputbackCursor, FALSE); i++;
	menu->shell = XtCreatePopupShell(menu->name, menuShellWidgetClass, 
					 parent, args, i);
	i = 0;
	menu->box = XtCreateManagedWidget("MenuBox", menuboxWidgetClass, 
					  menu->shell, args, i);
	i = 0;
	menu->title = XtCreateManagedWidget(menu->name, labelWidgetClass,
					    menu->box, args, i);

	return menu;
}


/* 
 * binds the button argument with translation BtnDown to the menu name
 */
static void
MenuBind(button, name)
	Widget button;
	String name;
{
	extern char *malloc();
	int len;
	int j;
	static String trans_table_base = "<BtnDown>: MenuPopup( "; 
	static String tail = " )";
	static XtTranslations menuTranslations;
	char *trans_table;
	char c;

	j = 0;
	while (c = name[j++]) 
		if (c == ' ') {
			(void)fprintf(stderr, "menu name cannot contain space.\n");
			return;
		}

	len = strlen(trans_table_base) + strlen(name) + strlen(tail) + 1;
	trans_table = malloc((unsigned)(len * sizeof(char)));
	(void) strcpy(trans_table, trans_table_base);
	(void) strcat(trans_table, name);
	(void) strcat(trans_table, " )");
	len = 0;

	menuTranslations = XtParseTranslationTable( trans_table );
	free(trans_table);
	XtOverrideTranslations(button, menuTranslations);
	return;
}

/*
 * add a selection to an existing menu
 * it should be possible to delete selections
 */

static char buttonTranslations[] =
     "<EnterWindow>:	set()\n\
     <LeaveWindow>:	unset()";

void
MenuAddSelection(menu, name, action, arg)
	PopupMenu *menu;
	char *name;
	XtCallbackProc action;
	caddr_t arg;
{
	Arg args[MAX_ARGS];
	Cardinal i;

	i = 0;
	menu->select[menu->num_select] = XtCreateManagedWidget(name,
						       commandWidgetClass,
						       menu->box, args, i);

	if (action != (XtCallbackProc) NULL) 
		XtAddCallback(menu->select[menu->num_select], XtNcallback, 
			      action, arg);
	XtOverrideTranslations(menu->select[menu->num_select],
				XtParseTranslationTable(buttonTranslations));
	return;
}

void
MenuReady(menu)
	PopupMenu *menu;
{
	XtRealizeWidget(menu->shell);
	return;
}

