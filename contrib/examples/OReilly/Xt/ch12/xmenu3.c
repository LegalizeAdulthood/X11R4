/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/* 
 *  xmenu3.c - pulldown menu invoked with MenuButton widget
 */

/*
 *  So that we can use fprintf:
 */
#include <stdio.h>

/* 
 * Standard Toolkit include files:
 */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <X11/Shell.h>

/*
 * Public include files for widgets used in this file.
 */
#ifdef X11R3
#include <X11/Command.h>
#include <X11/MenuButton.h>
#include <X11/Box.h>
#include <X11/Label.h>
#else /* R4 or later */
#include <X11/Xaw/Command.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Label.h>
#endif /* X11R3 */

Widget pshell;

/*
 * quit button callback function
 */
/* ARGSUSED */
void Quit(w, client_data, call_data)
Widget w;
caddr_t client_data, call_data;
{ 
	exit(0); 
}

/*
 * menu pane button callback function
 */
/* ARGSUSED */
void PaneChosen(w, pane_number, call_data)
Widget w;
int pane_number;  /* client_data */
caddr_t call_data;
{ 
	printf("Pane %d chosen.\n", pane_number);
	/* popdown the parent of the parent (pshell) */
        XtPopdown(pshell);
}

main(argc, argv)
int argc;
char **argv;
{
	Widget topLevel, box, pressme, quit, menubox, menulabel, menupane[10];
	int i;
	Arg arg;
	String buf[50];

	topLevel = XtInitialize(
		argv[0], 	/* application name */
		"XMenu3", 	/* application class name */
		NULL, 	/* application resources (not used) */
		0, 	/* application resource count */
		&argc, 	/* command line argument count */
		argv);	/* command-line string */

	box = XtCreateManagedWidget(
		"box", 	/* widget name */
		boxWidgetClass,	/* widget class */
		topLevel,	/* parent widget*/
		NULL,	/* argument list*/
		0	/* arglist size */
		);

	quit = XtCreateManagedWidget(
		"quit",	/* widget name */
		commandWidgetClass,	/* widget class */
		box,	/* parent widget*/
		NULL,	/* argument list*/
		0	/* arglist size */
		);

	i = 0;
	XtSetArg(arg, XtNmenuName, "pshell");
	pressme = XtCreateManagedWidget(
		"pressme",	/* widget name	 */
		menuButtonWidgetClass,	/* widget class */
		box,	/* parent widget*/
		&arg,	/* argument list*/
		1	/* arglist size */
		);

        pshell = XtCreatePopupShell(
                "pshell",
                transientShellWidgetClass,
                pressme,
                NULL,
                0
                );

        menubox = XtCreateManagedWidget(
                "menubox",               /* widget name   */
                boxWidgetClass,              /* widget class */
                pshell,                         /* parent widget*/
                NULL,                    /* argument list*/
                0           /* arglist size */
                );
	
        menulabel = XtCreateManagedWidget(
                "menulabel",               /* widget name   */
                labelWidgetClass,              /* widget class */
                menubox,                         /* parent widget*/
                NULL,                    /* argument list*/
                0           /* arglist size */
                );

	for (i = 0; i < 10; i++) {
		sprintf(buf, "menupane%d", i);
        	menupane[i] = XtCreateManagedWidget(buf,   /* widget name   */
                	commandWidgetClass, menubox, NULL, 0);

		XtAddCallback(menupane[i], XtNcallback, PaneChosen, i);
	}

	XtAddCallback(quit, XtNcallback, Quit, 0);

	XtRealizeWidget(topLevel);

	XtMainLoop();
}
