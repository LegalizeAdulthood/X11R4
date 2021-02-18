/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

/*	Function Name: R3Initialize
 *	Description: A convience routine for initializing the toolkit,
 *                 for use in R3 to use explicit application contexts.
 *	Arguments: app_context_return - The application context of the
 *                                      application
 *                 application_class  - The class of the application.
 *                 options            - The option list.
 *                 num_options        - The number of options in the above list
 *                 argc_in_out, argv_in_out - number and list of command line
 *                                            arguments.
 *	Returns: The shell widget.
 */
	
Widget
R3Initialize(app_context_return, application_class, options, num_options,
		argc_in_out, argv_in_out)
XtAppContext * app_context_return;
String application_class;
XrmOptionDescRec *options;
Cardinal num_options, *argc_in_out;
String *argv_in_out;
{
    XtAppContext app_con;
    Display * dpy;
    Widget root;
    
    XtToolkitInitialize();
    
    app_con = XtCreateApplicationContext();

    dpy = XtOpenDisplay(app_con, (String) NULL, NULL, application_class,
			options, num_options, argc_in_out, argv_in_out);

    if (dpy == NULL)
	XtErrorMsg("invalidDisplay","xtInitialize","XtToolkitError",
                   "Can't Open display", (String *) NULL, (Cardinal *)NULL);

    root = XtAppCreateShell(NULL, application_class, 
			    applicationShellWidgetClass,dpy, NULL, 0);
    
    if (app_context_return != NULL)
	*app_context_return = app_con;

    return(root);
}
