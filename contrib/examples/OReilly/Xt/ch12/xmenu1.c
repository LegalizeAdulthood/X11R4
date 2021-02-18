/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/* 
 *  xmenu1.c - simple spring-loaded menu
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
#include <X11/Box.h>
#include <X11/Label.h>
#else /* R4 or later */
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Label.h>
#endif /* X11R3 */
/*
 * The popup shell ID is global because both dialog and pshell
 * are needed in the dialogDone callback, and both can't be
 * passed in without creating a structure.
 */
Widget pshell;

void PlaceMenu(w, event)
Widget w;
XButtonEvent *event;
{
	Arg args[5];
	int i;

	/* should make sure coordinates allow menu to fit on screen */
	
	/* move submenu shell to slightly left and above button 
	 * press position */
	i = 0;
	XtSetArg(args[i], XtNx, event->x_root - 10);  i++;
	XtSetArg(args[i], XtNy, event->y_root - 10);  i++;
	XtSetValues(pshell, args, i);
}

/*
 * quit button callback function
 */
void Quit(w, client_data, call_data)
Widget w;
caddr_t client_data, call_data;
{ 
	exit(0); 
}

/*
 * menu pane button callback function
 */
void PaneChosen(w, pane_number, call_data)
Widget w;
int pane_number;  /* client_data */
caddr_t call_data;
{ 
	printf("Pane %d chosen.\n", pane_number);
        XtPopdown(pshell);
}

main(argc, argv)
int argc;
char **argv;
{
	Widget topLevel, box, label, quit, menubox, menulabel, menupane[10];
	int i;
	String buf[50];

        static XtActionsRec trial_actions[] = {
                {"placeMenu", PlaceMenu},
        };

	topLevel = XtInitialize(
		argv[0], 	/* application name */
		"XMenu1", 	/* application class name */
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

	label = XtCreateManagedWidget(
		"label",	/* widget name */
		labelWidgetClass,	/* widget class */
		box,	/* parent widget*/
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

        pshell = XtCreatePopupShell(
                "pshell",
                transientShellWidgetClass,
                topLevel,
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

        XtAddActions(trial_actions, XtNumber(trial_actions));

	XtAddCallback(quit, XtNcallback, Quit, 0);

	XtRealizeWidget(topLevel);

	XtMainLoop();
}
