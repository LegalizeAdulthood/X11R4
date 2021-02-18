/* This file is part of xdir, an X-based directory browser.
 *
 *	Created: 8/13/88
 *
 *	Win Treese
 *	Cambridge Research Lab/MIT Project Athena
 *	Digital Equipment Corporation
 *
 *	$Source: /udir/treese/Src/Xdir/RCS/xdir.c,v $
 *	$Athena: xdir.c,v 1.1 88/09/16 01:30:14 treese Locked $
 */

#if !defined(lint) && !defined(SABER)
static char rcsid_xdir_c[] = "$Header: xdir.c,v 2.1 89/03/25 02:48:12 treese Exp $";
#endif

#include <stdio.h>
#include <sys/param.h>
#include <X11/Xos.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/VPaned.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/List.h>

#define MAIN

#include "xdir.h"

/* Global variables. */

char CurrentSelectionName[MAXPATHLEN];
int CurrentSelectionIndex = 0;
char CurrentDirectory[MAXPATHLEN];
Widget TopWidget;
Widget MyListWidget;
Widget DirLabel;
Widget MyViewport;

/*ARGSUSED*/
static void
FileSelected(w, client_data, ret_val)
Widget w;
caddr_t client_data;
caddr_t ret_val;
{
	XawListReturnStruct *ret_struct = (XawListReturnStruct *) ret_val;

	strcpy(CurrentSelectionName, ret_struct->string);
	CurrentSelectionIndex = ret_struct->list_index;
}

void
main(argc, argv)
int argc;
char *argv[];
{
	Widget vpane;
	Arg args[10];
	Cardinal arg_cnt;
	char **file_list;

	TopWidget = XtInitialize("xdir","Xdir", NULL, 0,
				 (unsigned int*) &argc,argv);
	InitErrorHandlers();
	InitResources(TopWidget);

	argc--; argv++;
#ifdef notdef
	while (argc >0 && *argv[0] == '-') {
		(*argv)++;
		while (**argv) switch (*(*argv)++) {
		}
	}
#endif
	if (*argv) {
		strcpy(CurrentDirectory, *argv);
	}
	else {
		if (getwd(CurrentDirectory) == NULL)
			ErrAbort("cannot find current directory!");
	}

	file_list = MakeFileList(CurrentDirectory);
	if (file_list == NULL)
		exit(1);

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNgripIndent, -20); arg_cnt++;
  	vpane = XtCreateManagedWidget("vpane", vPanedWidgetClass, TopWidget,
				      args, arg_cnt);

	InitMenus(vpane);

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNlabel, CurrentDirectory); arg_cnt++;
	DirLabel = XtCreateManagedWidget("dirname", labelWidgetClass, 
				      vpane, args, arg_cnt);

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNallowVert, True); arg_cnt++;
	MyViewport = XtCreateManagedWidget("vport", viewportWidgetClass, 
				      vpane, args, arg_cnt);

	arg_cnt = 0;
	XtSetArg(args[arg_cnt], XtNlist, file_list); arg_cnt++;
	MyListWidget = XtCreateManagedWidget("list", listWidgetClass,
					     MyViewport, args, arg_cnt);
	XtAddCallback(MyListWidget, XtNcallback, FileSelected,
		      (caddr_t) NULL);

	SetDirIconName(CurrentDirectory);

	InitButtonActions();
	InitTextDisplay();

	XtRealizeWidget(TopWidget);
	XtMainLoop();
}

