/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xgoodbye.c - simple program to put up a banner on the display
 * 		and callback an application function.
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
 * Quit button callback function
 */
/* ARGSUSED */
void Quit(w, client_data, call_data)
Widget w;
caddr_t client_data, call_data;
{ 
	fprintf(stderr, "It was nice knowing you.\n");
	exit(0); 
}

main(argc, argv)
int argc;
char **argv;
{
	Widget topLevel, goodbye;

	topLevel = XtInitialize(
		argv[0],	/* Application name */
		"XGoodbye",	/* Application class */
		NULL,	/* Resource Mgr. options */
		0,	/* number of RM options */
		&argc,	/* number of args */
		argv	/* command line */
		);

	goodbye = XtCreateManagedWidget(
		"goodbye",		/* arbitrary widget name */
		commandWidgetClass,	/* widget class from Command.h */
		topLevel,	/* parent widget*/
		NULL,		/* argument list */
		0		/* arg list size */
		);

	XtAddCallback(goodbye, XtNcallback, Quit, 0);

	/*
	 *  Create windows for widgets and map them.
	 */
	XtRealizeWidget(topLevel);

	/*
	 *  Loop for events.
	 */
	XtMainLoop();
}
