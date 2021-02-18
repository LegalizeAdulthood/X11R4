/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xclickcount.c - simple program to count button clicks.
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
#include <X11/Label.h>		/* Athena Label Widget */
#else /* R4 or later */
#include <X11/Xaw/Label.h>		/* Athena Label Widget */
#endif /* X11R3 */
#define MAXLEN 50

char userstring[MAXLEN];

/*ARGSUSED*/
static void IncrementCount(w, event)
Widget w;
XButtonEvent *event;
{
	char string[MAXLEN];
	static int count;
	Arg args[3];
	int i;

	count++;

	sprintf(string, "%s: %d", userstring, count);

	i = 0;
	XtSetArg(args[i], XtNwidth, 100);  i++;
	XtSetArg(args[i], XtNlabel, string);  i++;
	XtSetValues(w, args, i);
}

main(argc, argv)
int argc;
char **argv;
{
	Widget topLevel, hello;
	String trans =
	"<BtnDown>:	IncrementCount()";

	XtTranslations compiled_trans;

	static XtActionsRec hello_actions[] = {
		{"IncrementCount", IncrementCount},
	};

	Arg arg;
	char *p;

	topLevel = XtInitialize(
		argv[0],	/* Application name */
		"XClickcount",	/* Application class */
		NULL,	/* Resource Mgr. options */
		0,	/* number of RM options */
		&argc,	/* number of args */
		argv	/* command line */
		);

	hello = XtCreateManagedWidget(
		"hello",		/* arbitrary widget name */
		labelWidgetClass,	/* widget class from Label.h */
		topLevel,	/* parent widget*/
		NULL,		/* argument list */
		0		/* arg list size */
		);

	XtSetArg(arg, XtNlabel, &p);
	XtGetValues(hello, &arg, 1);

	strcpy(userstring, p);

	XtAddActions(hello_actions, 1);

	compiled_trans = XtParseTranslationTable(trans);

	XtSetArg(arg, XtNtranslations, compiled_trans);
	XtSetValues(hello, &arg, 1);
	/*
	 *  Create windows for widgets and map them.
	 */
	XtRealizeWidget(topLevel);

	/*
	 *  Loop for events.
	 */
	XtMainLoop();
}
