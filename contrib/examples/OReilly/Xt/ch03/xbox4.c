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
void Quit(w, client_data, call_data)
Widget w;
caddr_t client_data, call_data;
{ 
	exit(0); 
}

XtCallbackRec quit_callback_list[]={
	{Quit, 0},
        {(XtCallbackProc) NULL, (caddr_t) NULL},
};

/*
 * Note: callback list comes before argument list, because
 * callback list must be declared before it is used in the argument 
 * list.
 */
Arg quit_button_args[] = {
	XtNcallback, (XtArgVal) quit_callback_list,
};
/*
 * End of quit button stuff
 */

/*
 * "Press me!" button callback function
 */
void pressme(w, client_data, call_data)
Widget w;
caddr_t client_data, call_data;
{ 
	fprintf(stderr, "Thankyou!\n"); 
}

XtCallbackRec pressme_callback_list[]={
	{pressme, 0},
        {(XtCallbackProc) NULL, (caddr_t) NULL},
};

Arg pressme_args[] = {
	XtNcallback, (XtArgVal) pressme_callback_list,
};
/*
 * End of press me button stuff.
 */

main(argc, argv)
int argc;
char **argv;
{
	Widget box, quit, pressme, topLevel;
	WidgetClass wc;

	topLevel = XtInitialize(
		argv[0], 	/* application name */
		"XBox4", 	/* application class name */
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
		quit_button_args,	/* argument list*/
		XtNumber(quit_button_args)	/* arglist size */
		);

	pressme = XtCreateManagedWidget(
		"pressme",	/* widget name	 */
		commandWidgetClass,	/* widget class */
		box,	/* parent widget*/
		pressme_args,	/* argument list*/
		XtNumber(pressme_args)	/* arglist size */
		);

	XtRealizeWidget(topLevel);

	XtMainLoop();
}
