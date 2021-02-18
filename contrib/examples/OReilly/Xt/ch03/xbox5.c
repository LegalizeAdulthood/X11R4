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

#include <X11/Shell.h>

/*
 * Public include files for widgets used in this file.
 */
#ifdef X11R3
#include <X11/Command.h>
#include <X11/Box.h>
#include <X11/Dialog.h>
#else /* R4 or later */
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Dialog.h>
#endif /* X11R3 */

/*
 * The popup shell ID is global because both dialog and pshell
 * are needed in the dialogDone callback, and both can't be
 * passed in without creating a structure.
 */
Widget pshell, pressme;

/*
 * dialog button
 */
void PrepareDialog(w, topLevel, call_data)
Widget w;
Widget topLevel;	/* client_data */
caddr_t call_data;
{
        Position x, y;
	Dimension width, height;
	Arg arg[2];
	int i;

	/*
 	 * get the coordinates of the middle of topLevel widget.
 	 */
	i = 0;
	XtSetArg(arg[i], XtNwidth, &width);    i++;
	XtSetArg(arg[i], XtNheight, &height);  i++;
	XtGetValues(topLevel, arg, i);
	
	/*
	 * translate coordinates in application top-level window
	 * into coordinates from root window origin.
	 */
        XtTranslateCoords(topLevel,                /* Widget */
                (Position) width/2,        /* x */
                (Position) height/2,       /* y */
                &x, &y);          /* coords on root window */

	/* move popup shell to this position (it's not visible yet) */
	i = 0;
	XtSetArg(arg[i], XtNx, x);  i++;
	XtSetArg(arg[i], XtNy, y);  i++;
	XtSetValues(pshell, arg, i);
}

/*
 * dialog done button
 */
void DialogDone(w, dialog, call_data)
Widget w;
Widget dialog;	/* client_data */
caddr_t call_data;
{
	String string;

        XtPopdown(pshell);

        XtSetSensitive(pressme, TRUE);

	/* 
	 * Here you would use public function provided by widget 
	 * class to get user's value typed.
	 */
}

/*
 * quit button callback function
 */
void Quit(w, client_data, call_data)
Widget w;
caddr_t client_data, call_data;
{ 
	exit(0); 
}

main(argc, argv)
int argc;
char **argv;
{
	Widget box, quit, topLevel, dialog, dialogDone;

	topLevel = XtInitialize(
		argv[0], 	/* application name */
		"XBox5", 	/* application class name */
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

        pshell = XtCreatePopupShell(
                "pshell",
                transientShellWidgetClass,
                topLevel,
                NULL,
                0
                );

        dialog = XtCreateManagedWidget(
                "dialog",               /* widget name   */
                dialogWidgetClass,              /* widget class */
                pshell,                         /* parent widget*/
                NULL,                    /* argument list*/
                0           /* arglist size */
                );

        dialogDone = XtCreateManagedWidget(
                "dialogDone",           /* widget name   */
                commandWidgetClass,             /* widget class */
                dialog,                         /* parent widget*/
                NULL,               /* argument list*/
                0      /* arglist size */
                );

	XtAddCallback(quit, XtNcallback, Quit, 0);

	XtAddCallback(dialogDone, XtNcallback, DialogDone, dialog);

	XtAddCallback(pressme, XtNcallback, PrepareDialog, topLevel);
	XtAddCallback(pressme, XtNcallback, XtCallbackExclusive, pshell);

	XtRealizeWidget(topLevel);

	XtMainLoop();
}
