/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xhello.c - simple program to put up a banner on the display
 */

/*
 * Include files required for all Toolkit programs
 */
#include <X11/Intrinsic.h>	/* Intrinsics Definitions */
#include <X11/StringDefs.h>	/* Standard Name-String definitions */

/*
 * Public include file for widgets we actually use in this file.
 */
#ifdef X11R3
#include <X11/Label.h>		/* Athena Label Widget */
#else /* R4 or later */
#include <X11/Xaw/Label.h>		/* Athena Label Widget */
#endif /* X11R3 */

/* both R3 and R4 (part of Xt) */
#include <X11/Shell.h>		/* Athena Label Widget */

#include "icon.bit"

main(argc, argv)
int argc;
char **argv;
{
	Widget topLevel, hello;
	Pixmap icon_pixmap;
	Arg arg;

	topLevel = XtInitialize(
		argv[0],	/* Application name */
		"XIcon",	/* Application class */
		NULL,	/* Resource Mgr. options */
		0,	/* number of RM options */
		&argc,	/* number of args */
		argv	/* command line */
		);

	icon_pixmap = XCreateBitmapFromData(XtDisplay(topLevel),
			RootWindowOfScreen(XtScreen(topLevel)),
			icon_bits,
			icon_width, icon_height );

	XtSetArg(arg, XtNiconPixmap, icon_pixmap);
	XtSetValues(topLevel, &arg, 1);

	hello = XtCreateManagedWidget(
		"hello",		/* arbitrary widget name */
		labelWidgetClass,	/* widget class from Label.h */
		topLevel,	/* parent widget*/
		NULL,		/* argument list */
		0		/* arg list size */
		);

	/*
	 *  Create windows for widgets and map them.
	 */
	XtRealizeWidget(topLevel);

	/*
	 *  Loop for events.
	 */
	XtMainLoop();
}
