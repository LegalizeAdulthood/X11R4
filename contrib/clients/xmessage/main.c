/*
 * xmessage - utility for querying users
 *
 * $XConsortium: main.c,v 1.9 89/10/09 16:49:02 jim Exp $
 *
 * Copyright 1988 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Authors:  Jim Fulton, MIT X Consortium
 *           Chris D. Peterson, MIT Project Athena
 */

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

/*
 * data used by xmessage
 */

char *ProgramName;

static struct _QueryResources {
    char *message;
    char *button_list;
    Boolean print_value;
} qres;				/* initialized by resources below */

#define offset(field) XtOffset(struct _QueryResources *, field)
static XtResource resources[] = {
    { "message", "Message", XtRString, sizeof (char *), 
      offset(message), XtRString, (caddr_t) NULL },
    { "buttons", "Buttons", XtRString, sizeof (char *),
      offset(button_list), XtRString, (caddr_t) "okay:0" },
    { "printValue", "PrintValue", XtRBoolean, sizeof (Boolean),
      offset(print_value), XtRString, "true" },
};
#undef offset

static XrmOptionDescRec optionList[] =  {
    { "-message", "*message", XrmoptionSepArg, (caddr_t) NULL },
    { "-buttons", "*buttons", XrmoptionSepArg, (caddr_t) NULL },
    { "-print",   "*printValue", XrmoptionNoArg, (caddr_t) "on" },
    { "-noprint", "*printValue", XrmoptionNoArg, (caddr_t) "off" },
};



/*
 * usage and utility routines
 */

static void usage ()
{
    static char *options[] = {
"    -display displayname        X server to contact",
"    -geometry geomspec          size and position of window",
"    -message string             message string",
"    -buttons string             comma-separated list of button:exitcode",
"    -print                      print the button label when selected",
"    -noprint                    do not print the button label",
"",
NULL};
    char **cpp;

    fprintf (stderr, "usage:  %s  [-options]  [filename]\n\n",
	     ProgramName);
    fprintf (stderr, "where options include:\n");
    for (cpp = options; *cpp; cpp++) {
	fprintf (stderr, "%s\n", *cpp);
    }
    exit (1);
    /* NOTREACHED */
}


/*
 * xmessage main program - make sure that there is a message and that there are
 * buttons; then, create the query box and go.  Callbacks take care of exiting.
 */
main (argc, argv)
    int argc;
    char **argv;
{
    Widget top, queryform, make_queryform();
    extern char *read_file();

    ProgramName = argv[0];

    top = XtInitialize (ProgramName, "XQuery",
			optionList, XtNumber(optionList), &argc, argv);

    XtGetApplicationResources (top, (caddr_t) &qres, resources,
			       XtNumber(resources), NULL, 0);

    /* if extra filename or plain dash (for stdin) */
    if (argc == 2 && (argv[1][0] != '-' || argv[1][1] == '\0')) {
	qres.message = read_file (argv[1]);
    } else if (argc != 1) {
	usage ();
    }

    if (qres.message == NULL) {
	fprintf (stderr, "%s:  no message specified\n", ProgramName);
	exit (1);
    }

    /* make sure that we have buttons, should always be okay... */
    if (!qres.button_list) {
	fprintf (stderr, "%s:  no buttons specified\n", ProgramName);
	exit (1);
    }


    /*
     * create the query form; this is where most of the real work is done
     */
    queryform = make_queryform (top, qres.message, qres.button_list,
				qres.print_value);
    if (!queryform) {
	fprintf (stderr,
		 "%s:  unable to create query form with buttons: %s\n",
		 ProgramName, qres.button_list);
	exit (1);
    }

    XtRealizeWidget (top);		/* realizes queryform as well */
    XtMainLoop ();

    exit (0);
    /* NOTREACHED */
}
