/***********************************************************

$Header: Top.c,v 3.0 89/11/20 09:25:42 mikew Exp $

Copyright 1989 by Mike Wexler, Santa Clara, Ca.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Mike Wexler or not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

MIKE WEXLER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#include "SM.h"
#include <X11/StringDefs.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Shell.h>
#include <X11/Form.h>
#include "Top.h"
#include "Home.h"

extern ApplicationData applData;
static String   displayName;
Widget          topShell;


/*
 * Description: Set up top-level shellWidget and initialize the toolkit
 * 
 * Outputs:	Widget - top level widget
 * 
 * Side Effects: Initializes the toolkit
 */

Widget 
TopSetup(argc, argv)
    int             argc;	/* argument count from command line */
    char          **argv;	/* argument list from command line */
{
    Arg             args[1];
    static XrmOptionDescRec options[] = {
	{"-print", "print", XrmoptionNoArg, "on"},
	{"-ignore", "ignore", XrmoptionNoArg, "on"},
    };

    static XtResource tsResources[] = {
	{"print", "print", XtRBoolean, sizeof (applData.print),
	   XtOffset(ApplicationDataPtr, print), XtRString, "False"},
	{"ignore", "ignore", XtRBoolean, sizeof (applData.ignore),
	   XtOffset(ApplicationDataPtr, ignore), XtRString, "False"},
	{"stateFile", "stateFile", XtRString, sizeof (applData.stateFile),
	   XtOffset(ApplicationDataPtr, stateFile), XtRString, "sm/State"},
	{"defaultState", "defaultState", XtRString, 
	   sizeof (applData.defaultState),
	   XtOffset(ApplicationDataPtr, defaultState), XtRString, "DefaultState"},
	{"shell", "shell", XtRString, sizeof (applData.shell),
	   XtOffset(ApplicationDataPtr, shell), XtRString, "/bin/sh"},
	{"defaultCommand", "defaultCommand", XtRString, 
	   sizeof (applData.defaultCommand),
	   XtOffset(ApplicationDataPtr, defaultCommand), XtRString, "xterm -C &"},
	{"geometryString", "geometryString", XtRString, 
	   sizeof (applData.geometry),
	   XtOffset(ApplicationDataPtr, geometry), XtRString, "-geometry"},
	{"iconicString", "iconic", XtRString, sizeof (applData.iconic),
	   XtOffset(ApplicationDataPtr, iconic), XtRString, "-iconic"},
    };

    topShell = XtInitialize(NULL, "SM", options,
			    XtNumber(options), &argc, argv);
    if (argc != 1)
	Usage();

    XtGetApplicationResources(topShell, &applData, tsResources,
			      XtNumber(tsResources), NULL, 0);

    XtSetArg(args[0], XtNallowShellResize, True);
    XtSetValues(topShell, args, 1);
    displayName = XDisplayString(XtDisplay(topShell));
    return (topShell);
}

/*
 * Description: Realize the top-level shell
 * 
 * Side Effects: Realizes the top-level shell.
 */

/* Realize the top-level shell. */
void 
TopStart(homeForm)
    Widget          homeForm;	/* child to be managed */
{
    XtManageChild(homeForm);
    XtManageChild(confirmForm);
    XtRealizeWidget(topShell);
}
