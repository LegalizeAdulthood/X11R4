/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/*
 * xfileinput.c - simple program to test XtAddInput for getting
 * 		  file input.
 * 	
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
#else /* R4 or later */
#include <X11/Xaw/Command.h>		/* Athena Label Widget */
#endif /* X11R3 */
/*
 * quit button callback function
 */
/* ARGSUSED */
void Quit(w, client_data, call_data)
Widget w;
caddr_t client_data, call_data;
{ 
	fprintf(stderr, "It was nice knowing you.\n");
	exit(0); 
}

/* ARGSUSED */
get_file_input(client_data, fid, id)
caddr_t client_data;	/* unused */
int *fid;
XtInputId *id;
{
	char buf[BUFSIZ];
	int nbytes;
	int i;

	if ((nbytes = read(*fid, buf, BUFSIZ)) == -1)
		perror("get_file_input");

	if (nbytes)
		for (i = 0; i < nbytes; i++)
			putchar(buf[i]);
}

main(argc, argv)
int argc;
char **argv;
{
	Widget topLevel, goodbye;
	FILE *fid;
	String filename;

	topLevel = XtInitialize(
		argv[0],	/* Application name */
		"XFileInput",	/* Application class */
		NULL,	/* Resource Mgr. options */
		0,	/* number of RM options */
		&argc,	/* number of args */
		argv	/* command line */
		);

	if (argv[1] == NULL) {
		fprintf(stderr, "xfileinput: filename must be specified on command line.\n");
		exit(1);
	}
	
	filename = argv[1];

	goodbye = XtCreateManagedWidget(
		"goodbye",		/* arbitrary widget name */
		commandWidgetClass,	/* widget class from Label.h */
		topLevel,	/* parent widget*/
		NULL,		/* argument list */
		0		/* arg list size */
		);

	XtAddCallback(goodbye, XtNcallback, Quit, 0);

	/* open file, pipe, or socket */
	if ((fid = fopen(filename, "r")) == NULL)
		fprintf(stderr, "xfileinput: couldn't open input file");

	/* register function to handle that input */
	XtAddInput(fileno(fid), XtInputReadMask, get_file_input, NULL);

	/*
	 *  Create windows for widgets and map them.
	 */
	XtRealizeWidget(topLevel);

	/*
	 *  Loop for events.
	 */
	XtMainLoop();
}
