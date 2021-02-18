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

/* for R3 and R4 (part of Xt) */
#include <X11/Shell.h>

#include <stdio.h>

main(argc, argv)
int argc;
char **argv;
{
	Widget topLevel, hello;
	XrmValue from, to;
	Screen *screen;
	static char icon_file_name[] = "./icon.bit";
	Arg arg;

	static XtConvertArgRec screenConvertArg[] = {
		{
		XtAddress, 
		(caddr_t) NULL, 
		sizeof(Screen *)
		}
	};

	topLevel = XtInitialize(
		argv[0],	/* Application name */
		"XIcon2",	/* Application class */
		NULL,	/* Resource Mgr. options */
		0,	/* number of RM options */
		&argc,	/* number of args */
		argv	/* command line */
		);

	screen = XtScreen(topLevel);

	screenConvertArg[0].address_id = (caddr_t) &screen;
#ifdef X11R3
	XtAddConverter(XtRString, XtRPixmap, XmuCvtStringToPixmap,
		screenConvertArg, XtNumber(screenConvertArg));
#else /* R4 or later */
	XtAddConverter(XtRString, XtRPixmap, XmuCvtStringToBitmap,
		screenConvertArg, XtNumber(screenConvertArg));
#endif

	from.size = strlen(icon_file_name);
	from.addr = icon_file_name;

	XtConvert(topLevel, XtRString, &from, XtRPixmap, &to);

	if (to.addr != NULL) {
        	XtSetArg(arg, XtNiconPixmap, *to.addr);
        	XtSetValues(topLevel, &arg, 1);
	}
	else
		fprintf(stderr, "xhello: unable to find icon pixmap file.\n");

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

