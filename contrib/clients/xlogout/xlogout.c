/*
 * $XConsortium: xlogout.c,v 1.1 89/11/29 18:29:34 jim Exp $
 *
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Jim Fulton, MIT X Consortium
 *          from xcommand.c and popup.c examples
 */

/* An example of popup dialog boxes using X11R4 and the Athena widget set */

#include <stdio.h>			/* For the Syntax message */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Command.h>

static void Quit();	/* the callback of the logout button */

static String fallback_resources[] = {
    "*input: True",
    "*Command.Label: Logout",
    "*allowShellResize: True",
    "*shapeStyle: rectangle",
    "*Font: -adobe-new century schoolbook-bold-r-normal--*-240-*-*-*-*-iso8859-1",
    NULL,
};

static XrmOptionDescRec options[] = {
{"-label",      "*Command.label",       XrmoptionSepArg,        NULL}
};


void main (argc, argv)
    int argc;
    char **argv;
{
    Widget toplevel, button;
    XtAppContext app_con;

    toplevel = XtAppInitialize (&app_con, "XLogout",
				options, XtNumber(options),
				&argc, argv, fallback_resources, NULL, ZERO);

    if (argc != 1) {
	fprintf (stderr, "usage:  %s [-label string]\n", argv[0]);
	/* do not exit since this is the first program run */
    }

    button = XtCreateManagedWidget ("logout", commandWidgetClass, toplevel,
				    NULL, ZERO);
    XtAddCallback (button, XtNcallback, Quit, (XtPointer) toplevel);

    XtRealizeWidget (toplevel);
    XtAppMainLoop (app_con);
}


/*ARGSUSED*/
static void Quit (widget, client_data, call_data)
    Widget widget;		/* unused, the quit button */
    XtPointer client_data;	/* the toplevel shell */
    XtPointer call_data;	/* unused */
{
    Widget toplevel = (Widget) client_data;
    XtUnmapWidget (toplevel);
    XtDestroyApplicationContext (XtWidgetToApplicationContext(toplevel));
    exit (0);
}


