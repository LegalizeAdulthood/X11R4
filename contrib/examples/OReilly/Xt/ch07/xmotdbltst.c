/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xtryactions.c - simple program to provide a Command widget that
 * 		performs a different action for each button.
 */

#include <stdio.h>
/*
 * Include files required for all Toolkit programs
 */
#include <X11/Intrinsic.h>	/* Intrinsics Definitions */
#include <X11/StringDefs.h>	/* Standard Name-String definitions */

/*
 * Public include file for widgets we actually use in this file.
 */
#ifdef X11R3
#include <X11/Command.h>		/* Athena Label Widget */
#else /* R4 or later */
#include <X11/Xaw/Command.h>		/* Athena Label Widget */
#endif /* X11R3 */
#define NUM_ACTIONS 3

/*ARGSUSED*/
static void ActionA(w, event)
Widget w;
XButtonEvent *event;
{
	printf("action A executed\n");
}

/*ARGSUSED*/
static void ActionB(w, event)
Widget w;
XButtonEvent *event;
{
	printf("action B executed\n");
}

/*ARGSUSED*/
static void ActionC(w, event)
Widget w;
XButtonEvent *event;
{
	printf("action C executed\n");
}

main(argc, argv)
int argc;
char **argv;
{
	Widget topLevel, trial;
	Arg arg;

	static XtActionsRec trial_actions[] = {
		{"actionA", ActionA},
		{"actionB", ActionB},
		{"actionC", ActionC},
	};

	topLevel = XtInitialize(
		argv[0],	/* Application name */
		"XMotdbltst",	/* Application class */
		NULL,	/* Resource Mgr. options */
		0,	/* number of RM options */
		&argc,	/* number of args */
		argv	/* command line */
		);

	trial = XtCreateManagedWidget(
		"trial",		/* arbitrary widget name */
		commandWidgetClass,	/* widget class from Label.h */
		topLevel,	/* parent widget*/
		NULL,		/* argument list */
		0		/* arg list size */
		);

	XtAddActions(trial_actions, NUM_ACTIONS);

	/*
	 *  Create windows for widgets and map them.
	 */
	XtRealizeWidget(topLevel);

	/*
	 *  Loop for events.
	 */
	XtMainLoop();
}
