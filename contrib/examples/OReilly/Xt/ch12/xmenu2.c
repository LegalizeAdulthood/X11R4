/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/* 
 *  xmenu2.c - pop up using command (placement code).
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
#include <X11/Label.h>
#else /* R4 or later */
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Label.h>
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
void PlaceMenu(w, topLevel, call_data)
Widget w;
Widget topLevel;	/* client_data */
caddr_t call_data;
{
        Position x, y;
	Arg arg[2];
	int i;
	int val;

	/*
	 * translate coordinates in application top-level window
	 * into coordinates from root window origin.
	 */
        XtTranslateCoords(pressme,                /* Widget */
                (Position) 0,        /* x */
                (Position) 0,       /* y */
                &x, &y);          /* coords on root window */

	/* move popup shell one pixel above and left of this position 
	 * (it's not visible yet) */
	i = 0;
	XtSetArg(arg[i], XtNx, x - 1);  i++;
	XtSetArg(arg[i], XtNy, y  + 1);  i++;
	XtSetValues(pshell, arg, i);
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

/*
 * menu pane button callback function
 */
void PaneChosen(w, pane_number, call_data)
Widget w;
int pane_number;  /* client_data */
caddr_t call_data;
{ 
	printf("Pane %d chosen.\n", pane_number);
	XtPopdown(pshell);
}

main(argc, argv)
int argc;
char **argv;
{
	Widget topLevel, box, quit, menubox, menulabel, menupane[10];
	int i;
	String buf[50];

	topLevel = XtInitialize(
		argv[0], 	/* application name */
		"XMenu2", 	/* application class name */
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
                pressme,
                NULL,
                0
                );

        menubox = XtCreateManagedWidget(
                "menubox",               /* widget name   */
                boxWidgetClass,              /* widget class */
                pshell,                         /* parent widget*/
                NULL,                    /* argument list*/
                0           /* arglist size */
                );
	
        menulabel = XtCreateManagedWidget(
                "menulabel",               /* widget name   */
                labelWidgetClass,              /* widget class */
                menubox,                         /* parent widget*/
                NULL,                    /* argument list*/
                0           /* arglist size */
                );

	for (i = 0; i < 10; i++) {
		sprintf(buf, "menupane%d", i);
        	menupane[i] = XtCreateManagedWidget(buf,   /* widget name   */
                	commandWidgetClass, menubox, NULL, 0);

		XtAddCallback(menupane[i], XtNcallback, PaneChosen, i);
	}

	XtAddCallback(quit, XtNcallback, Quit, 0);

	XtAddCallback(pshell, XtNpopupCallback, PlaceMenu, topLevel);

	XtRealizeWidget(topLevel);

	XtMainLoop();
}
