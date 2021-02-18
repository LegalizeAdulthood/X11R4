/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xfarewell.c - simple program to provide a Command widget that
 * 		performs a different action in response to a
 *              click of the first and second pointer buttons.
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
#include <X11/Command.h>		/* Athena Command Widget */
#else /* R4 or later */
#include <X11/Xaw/Command.h>		/* Athena Command Widget */
#endif /* X11R3 */

/* 
 *  Confirm action
 */
/*ARGSUSED*/
static void Confirm(w, event)
Widget w;
XButtonEvent *event;
/*   following action args not used:
 * String *params; 
 * Cardinal *num_params;
 */
{
/*
 * Once we show how to do it, we could pop-up a dialog box to do this.
 * Since we haven't yet, simply print a message to stderr.
 */
	fprintf(stderr, "Are you sure you want to exit?\n\
                Click with the middle pointer button if you're sure.\n");
}

/* 
 *  Quit action
 */
/*ARGSUSED*/
static void Quit(w, event, params, num_params)
Widget w;
XButtonEvent *event;
String *params;	/* unused */
Cardinal *num_params;	/* unused */
{
	fprintf(stderr, "It was nice knowing you.\n");
	exit(0); 
}

main(argc, argv)
int argc;
char **argv;
{
	Widget topLevel, farewell;

	static XtActionsRec two_quits[] = {
		{"confirm", Confirm},
		{"quit", Quit},
	};

	topLevel = XtInitialize(
		argv[0],	/* Application name */
		"XFarewell",	/* Application class */
		NULL,	/* Resource Mgr. options */
		0,	/* number of RM options */
		&argc,	/* number of args */
		argv	/* command line */
		);

	farewell = XtCreateManagedWidget(
		"farewell",		/* arbitrary widget name */
		commandWidgetClass,	/* widget class from Command.h */
		topLevel,	/* parent widget*/
		NULL,		/* argument list */
		0		/* arg list size */
		);

	XtAddActions(two_quits, XtNumber(two_quits));

	/*
	 *  Create windows for widgets and map them.
	 */
	XtRealizeWidget(topLevel);

	/*
	 *  Loop for events.
	 */
	XtMainLoop();
}
