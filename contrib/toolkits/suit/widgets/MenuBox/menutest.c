/* This file is a test program for the menu widget.
 *
 *	Created: 9/2/88
 *
 *	Win Treese
 *	Cambridge Research Lab/MIT Project Athena
 *	Digital Equipment Corporation
 *
 *	$Source$
 *	$Athena$
 */

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

#if !defined(lint) && !defined(SABER)
static char rcsid_xdir_c[] = "$Header$";
#endif

#include <stdio.h>
#include <X11/Xos.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Viewport.h>
#include <X11/Box.h>
#include <X11/Command.h>
#include <X11/VPaned.h>
#include <X11/MenuBox.h>
#include <X11/MenuShell.h>
#include <X11/Menu.h>

Widget TopWidget;

/*ARGSUSED*/
static void
Quit(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	printf("bye....\n");
	exit(0);
}

/*ARGSUSED*/
static void
Select1(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	printf("Selection 1\n");
}

/*ARGSUSED*/
static void
Select2(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	printf("Selection 2\n");
}

static String menuTrans = "<BtnDown>:MenuPopup(MenuDemo)";

void
main(argc, argv)
int argc;
char *argv[];
{
	Widget vport, vpane;
	Arg args[10];
	Cardinal arg_cnt;
	PopupMenu *mymenu;
	Widget box, w;
	XtTranslations parsed_trans;

	TopWidget = XtInitialize("xtest","Xtest", NULL, 0,
				 (unsigned int*) &argc,argv);

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNskipAdjust, True); arg_cnt++;
	box = XtCreateManagedWidget("box", boxWidgetClass, TopWidget,
					args, arg_cnt);

	arg_cnt = 0;
	w = XtCreateManagedWidget("quit", commandWidgetClass, box,
					args, arg_cnt);
	XtAddCallback(w, XtNcallback, Quit, NULL);

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNlabel, "Menu"); arg_cnt++;
        w = XtCreateManagedWidget("view", commandWidgetClass, box,
					args, arg_cnt);

	mymenu = MenuCreate(TopWidget, w, "MenuDemo");

	MenuAddSelection(mymenu, "Selection 1", Select1, NULL);
	MenuAddSelection(mymenu, "Selection 2", Select2, NULL);
	MenuAddSelection(mymenu, "Quit", Quit, NULL);
	parsed_trans = XtParseTranslationTable(menuTrans);
	XtOverrideTranslations(w, parsed_trans);
	MenuReady(mymenu);

	XtRealizeWidget(TopWidget);
	XtMainLoop();
}

