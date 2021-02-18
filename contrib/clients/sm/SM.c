/***********************************************************

$Header: SM.c,v 3.0 89/11/20 09:25:31 mikew Exp $

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
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include "PatchLevel.h"
#include "Top.h"
#include "ICC.h"
#include "WinInfo.h"
#include "Atoms.h"
#include "Home.h"
#include "State.h"

extern ApplicationData applData;
Widget          homeForm;

/*
 * Description: The main routine for sm
 * 
 * Side Effects: Manages the session
 */

int
main(argc, argv)
    int             argc;	/* number of command line argument s*/
    char          **argv;	/* command line arguments */
{
    Widget          topForm;
    Arg             args[1];

    /* TopSetup calls XtInitialize which parses argv for us. */
    topForm = TopSetup(argc, argv);
    /* Get the bindings for the atoms that we use.  */
    if (InternAtoms(XtDisplay(topForm)))
	Fatal("main", "Window manager isn't ICCCM compliant.\n");
    /* If the user specified -print just print the state and exit. */
    if (applData.print) {
	applData.ignore = True;
	/*
	 * The following three lines are a hack to create an unmapped window
	 * to create time stamps with.
	 */

	XtSetArg(args[0], XtNmappedWhenManaged, False);
	XtSetValues(topShell, args, 1);
    }
    /* Build the top-level widget. */
    homeForm = HomeSetup(topForm);
    if (!applData.ignore)
	if (StateRestore() == -1)
	    Fatal("main", "Couldn't restore the state.\n");
    /* Display the top-level widget. */
    TopStart(homeForm);
    if (applData.print) {
	State        state;

	StateInit(&state);
	if (StateGet(&state, XtDisplay(homeForm)) < 0)
	  Fatal("main", "Couldn't get state.\n");
	if (StatePrint(stdout, &state) < 0)
	  Fatal("main", "Couldn't print state.\n");
	StateCleanup(&state);
    } else {
	/* Let Xt handle everything until the user tells us otherwise. */
	XtMainLoop();
    }
    exit(0);
}

/*
 * Description: Display a nice little usage message
 * 
 * Side Effects: Exits
 */

void 
Usage()
{
    (void) fprintf(stderr, "usage: sm [-print] [-ignore]\n");
    exit(1);
}
