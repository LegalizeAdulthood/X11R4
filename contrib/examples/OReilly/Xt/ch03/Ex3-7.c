/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/* 
 *  xbox.c - simple button box
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

/*
 * Public include files for widgets used in this file.
 */
#ifdef X11R3
#include <X11/Command.h>
#include <X11/Box.h>
#else /* R4 or later */
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Box.h>
#endif /* X11R3 */
/*
 * quit button callback function
 */
/*ARGSUSED*/
void Quit(w, client_data, call_data)
Widget w;
caddr_t client_data, call_data;
{ 
	exit(0); 
}

typedef struct {
	String name;
	String street;
	String city;
} app_stuff;

/*
 * "Press me!" button callback function
 */
/*ARGSUSED*/
void PressMe(w, address, call_data)
Widget w;
app_stuff *address;
caddr_t call_data;
{ 
	/* 
	 * Alternately, the second argument can remain declared as
	 * client_data, and the following cast can be used:
	 *     app_stuff *address = (app_stuff *) client_data; 
	 */

	fprintf(stderr, "%s\n%s\n%s\n", address->name, address->street, address->city); 
}

main(argc, argv)
int argc;
char **argv;
{
	Widget box, quit, pressme, topLevel;

	static app_stuff stuff = {
		"John Doe",
		"1776 Constitution Way",
		"Phily, PA 01029"
	};

	topLevel = XtInitialize(
		argv[0], 	/* application name */
		"XBox1", 	/* application class name */
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

	pressme = XtCreateManagedWidget(
		"pressme",	/* widget name	 */
		commandWidgetClass,	/* widget class */
		box,	/* parent widget*/
		NULL,	/* argument list*/
		0	/* arglist size */
		);

	XtAddCallback(quit, XtNcallback, Quit, 0);
	XtAddCallback(pressme, XtNcallback, PressMe, &stuff);

	XtRealizeWidget(topLevel);

	XtMainLoop();
}
