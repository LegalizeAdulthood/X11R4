/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xhello.c - simple program to put up a banner on the display
 */

#include <X11/IntrinsicP.h>	/* temporary, until we find out how
				 * to register a type converter without
				 * referencing the core structure */

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

#define OURPIXMAPNAME "xlogo64"

typedef struct {
	Screen *screen;
} AppData, *AppDataPtr;

AppData app_data;

main(argc, argv)
int argc;
char **argv;
{
	Widget topLevel, hello;
	Arg hello_args[5];
	int i;
	XrmValue from, to;
	static char pixmap_name[] = "xlogo64";
	Pixmap pixmap;

	static XtConvertArgRec screenConvertArg[] = {
		{
		XtBaseOffset, 
		(caddr_t) XtOffset(AppDataPtr, app_data.screen), 
		sizeof(Screen *)
		}
	};

	topLevel = XtInitialize(
		argv[0],	/* Application name */
		"XHello",	/* Application class */
		NULL,	/* Resource Mgr. options */
		0,	/* number of RM options */
		&argc,	/* number of args */
		argv	/* command line */
		);

	app_data.screen = XtScreen(topLevel);

	XtAddConverter("String", "Pixmap", XmuCvtStringToPixmap,
			screenConvertArg, XtNumber(screenConvertArg));

        from.addr = OURPIXMAPNAME;
	from.size = sizeof(OURPIXMAPNAME);

	XtConvert(topLevel, XtRString, &from, XtRPixmap, &to);

	pixmap = *(Pixmap *) to.addr;

	i = 0;
	XtSetArg(hello_args[i], XtNpixmap, pixmap);

	hello = XtCreateManagedWidget(
		hello,		/* arbitrary widget name */
		labelWidgetClass,	/* widget class from Label.h */
		topLevel,	/* parent widget*/
		hello_args,		/* argument list */
		1		/* arg list size */
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
