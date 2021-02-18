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
#include <X11/Command.h>		/* Athena Label Widget */
#include <X11/Box.h>		/* Athena Label Widget */
#else /* R4 or later */
#include <X11/Xaw/Command.h>		/* Athena Label Widget */
#include <X11/Xaw/Box.h>		/* Athena Label Widget */
#endif /* X11R3 */

/*
 * quit button callback function
 */
/* ARGSUSED */
void quit(w, client_data, call_data)
Widget w;
caddr_t client_data, call_data;
{ 
	fprintf(stderr, "It was nice knowing you.\n");
}

main(argc, argv)
int argc;
char **argv;
{
	Widget topLevel, box, goodbye, hello;

	topLevel = XtInitialize(
		argv[0],	/* Application name */
		"XTryAccel",	/* Application class */
		NULL,	/* Resource Mgr. options */
		0,	/* number of RM options */
		&argc,	/* number of args */
		argv	/* command line */
		);

	box = XtCreateManagedWidget(
		"box",		/* arbitrary widget name */
		boxWidgetClass,	/* widget class from Label.h */
		topLevel,	/* parent widget*/
		NULL,		/* argument list */
		0		/* arg list size */
		);

	goodbye = XtCreateManagedWidget(
		"goodbye",		/* arbitrary widget name */
		commandWidgetClass,	/* widget class from Label.h */
		box,	/* parent widget*/
		NULL,		/* argument list */
		0		/* arg list size */
		);

	hello = XtCreateManagedWidget(
		"hello",		/* arbitrary widget name */
		commandWidgetClass,	/* widget class from Label.h */
		box,	/* parent widget*/
		NULL,		/* argument list */
		0		/* arg list size */
		);

	XtAddCallback(goodbye, XtNcallback, quit, 0);
	XtAddCallback(hello, XtNcallback, quit, 0);

	/*
	 *  Create windows for widgets and map them.
	 */
	XtRealizeWidget(topLevel);

	XtInstallAccelerators(goodbye, box);
	XtInstallAccelerators(hello, box);
	/*
	 *  Loop for events.
	 */
	XtMainLoop();
}
