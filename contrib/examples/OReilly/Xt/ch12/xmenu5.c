/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


/* 
 *  xmenu5.c - cascaded spring-loaded menu
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
Widget pshell, menubox, subshell, subbox;


void PlaceMenu(w, event)
Widget w;
XButtonEvent *event;
{
        Arg args[5];
        int i;

        /* should make sure coordinates allow menu to fit on screen */

        /* move submenu shell to slightly left and above button
         * press position */
        i = 0;
        XtSetArg(args[i], XtNx, event->x_root - 10);  i++;
        XtSetArg(args[i], XtNy, event->y_root - 10);  i++;
        XtSetValues(pshell, args, i);
}

void CheckRightAndPopupSubmenu(w, event)
Widget w;
XLeaveWindowEvent *event;
{
	Arg args[3];
	int i;
	Dimension height;

	i = 0;
	XtSetArg(args[i], XtNheight, &height);  i++;
	XtGetValues(w, args, i);

	if ((event->x > 0) && (event->y > 0) && (event->y < height)) {
		/* move submenu shell to start just right of pane,
	 	 * using an arbitrary offset to place pointer in 
		 * first item. */
		i = 0;
		XtSetArg(args[i], XtNx, event->x_root);  i++;
		XtSetArg(args[i], XtNy, event->y_root - 12);  i++;
		XtSetValues(subshell, args, i);
		XtPopup(subshell, XtGrabNonexclusive);
	}
}

void CheckLeftAndPopdownSubmenu(w, event, params)
Widget w;
XLeaveWindowEvent *event;
String params;
{
	Arg arg;
	Dimension sub_height, menu_height;
	Position dummy_menu_x, menu_y;
	int i;

	i = 0;
	XtSetArg(arg, XtNheight, &sub_height);  i++;
	XtGetValues(subbox, &arg, i);

	i = 0;
	XtSetArg(arg, XtNheight, &menu_height);  i++;
	XtGetValues(menubox, &arg, i);

        XtTranslateCoords(menubox,                /* Widget */
                (Position) 0,        /* x */
                (Position) 0,       /* y */
                &dummy_menu_x, &menu_y);          /* coords on root window */

	if ((event->x < 0) && (event->y >= 0) && (event->y <= sub_height) && (event->y_root <= menu_y + menu_height)) {
		/* moved out left side (should really require that it 
		 * move out directly into the main menu) */ 
		XtPopdown(subshell);
	}
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
void SubPaneChosen(w, pane_number, call_data)
Widget w;
int pane_number;  /* client_data */
caddr_t call_data;
{ 
	printf("SubPane %d chosen.\n", pane_number);
	XtPopdown(subshell);
	XtPopdown(pshell);
}

/*
 * menu pane button callback function
 */
void MainPaneChosen(w, pane_number, call_data)
Widget w;
int pane_number;  /* client_data */
caddr_t call_data;
{ 
	printf("Pane %d chosen.\n", pane_number);

	XtPopdown(pshell);
	XtPopdown(subshell);
}

main(argc, argv)
int argc;
char **argv;
{
	Widget box, topLevel, quit, label, menulabel, menupane[10], sublabel, subpane[10];
	int i;
	String buf[50];

        static XtActionsRec trial_actions[] = {
                {"checkRightAndPopupSubmenu", CheckRightAndPopupSubmenu},
                {"checkLeftAndPopdownSubmenu", CheckLeftAndPopdownSubmenu},
                {"placeMenu", PlaceMenu},
        };

	topLevel = XtInitialize(
		argv[0], 	/* application name */
		"XMenu5", 	/* application class name */
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

	label = XtCreateManagedWidget(
		"label",	/* widget name	 */
		labelWidgetClass,	/* widget class */
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

        XtAddActions(trial_actions, XtNumber(trial_actions));

	for (i = 0; i < 10; i++) {
		sprintf(buf, "menupane%d", i);
        	menupane[i] = XtCreateManagedWidget(buf,   /* widget name   */
                	commandWidgetClass, menubox, NULL, 0);

		XtAddCallback(menupane[i], XtNcallback, MainPaneChosen, i);
	}

        subshell = XtCreatePopupShell(
                "subshell",
                transientShellWidgetClass,
                topLevel,
                NULL,
                0
                );

        subbox = XtCreateManagedWidget(
                "subbox",               /* widget name   */
                boxWidgetClass,              /* widget class */
                subshell,                         /* parent widget*/
                NULL,                    /* argument list*/
                0           /* arglist size */
                );

        sublabel = XtCreateManagedWidget(
                "sublabel",               /* widget name   */
                labelWidgetClass,              /* widget class */
                subbox,                         /* parent widget*/
                NULL,                    /* argument list*/
                0           /* arglist size */
                );
	
	for (i = 0; i < 10; i++) {
		sprintf(buf, "subpane%d", i);
        	subpane[i] = XtCreateManagedWidget(buf,   /* widget name   */
                	commandWidgetClass, subbox, NULL, 0);

		XtAddCallback(subpane[i], XtNcallback, SubPaneChosen, i);
	}

	XtAddCallback(quit, XtNcallback, Quit, 0);

        XtAddActions(trial_actions, XtNumber(trial_actions));

	XtRealizeWidget(topLevel);

	XtMainLoop();
}
