/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xbanner0.c - simple program to put up a banner on the display
 */

/*
 * Include files required for all Toolkit programs
 */
#include <X11/Intrinsic.h>	/* Intrinsics Definitions */
#include <X11/StringDefs.h>	/* Standard Name-String definitions */

#include <X11/Shell.h>		/* Needed for applicationShellWidgetClass */

/*
 * Public include file for widgets we actually use in this file.
 */
#ifdef X11R3
#include <X11/Label.h>		/* Athena Label Widget */
#else /* R4 or later */
#include <X11/Xaw/Label.h>		/* Athena Label Widget */
#endif /* X11R3 */

main(argc, argv)
int argc;
char **argv;
{
	XtAppContext app_con1, app_con2;
	Widget topLevel1, topLevel2, hello, goodbye;
	Display *display1, *display2;

	XtToolkitInitialize();

	app_con1 = XtCreateApplicationContext();
	app_con2 = XtCreateApplicationContext();

	display1 = XtOpenDisplay(app_con1, "spike:0", argv[0], "XBanner", NULL, 0, &argc, argv);

	display2 = XtOpenDisplay(app_con2, "ncd:0", argv[0], "XBanner", NULL, 0, &argc, argv);

	topLevel1 = XtAppCreateShell(app_con1, "XBanner", applicationShellWidgetClass, display1, NULL, 0);

	topLevel2 = XtAppCreateShell(app_con2, "XBanner", applicationShellWidgetClass, display2, NULL, 0);

	hello = XtCreateManagedWidget(
		"hello",		/* arbitrary widget name */
		labelWidgetClass,	/* widget class from Label.h */
		topLevel1,	/* parent widget*/
		NULL,		/* argument list */
		0		/* arg list size */
		);

	goodbye = XtCreateManagedWidget(
		"goodbye",		/* arbitrary widget name */
		labelWidgetClass,	/* widget class from Label.h */
		topLevel2,	/* parent widget*/
		NULL,		/* argument list */
		0		/* arg list size */
		);
	/*
	 *  Create windows for widgets and map them.
	 */
	XtRealizeWidget(topLevel1);
	XtRealizeWidget(topLevel2);

	/*
	 *  Loop for events.
	 */
	XtAppMainLoop(app_con1);

	/* 
	 * Notice that since XtAppMainLoop never returns, the
	 * second XtAppMainLoop call is never reached unless there
	 * are multiple threads.
	 */
	XtAppMainLoop(app_con2);
}
