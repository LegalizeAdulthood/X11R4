#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/Cardinals.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <Xcu/Wlm.h>

static Arg toplevel_args[] =
    {
     {XtNallowShellResize, (XtArgVal) True}
    } ;

static Arg wlm_args[] =
    {
     {XtNfile, (XtArgVal) NULL}
    } ;

Widget toplevel, wlm ;

main (argc, argv)
    int argc ;
    char **argv ;
{
Cardinal i ;

if (argc < 2)
    {
    fprintf (stderr, "\nUsage: %s wlm_layout_file\n\n",
	     argv[0]) ;
    exit (1) ;
    }

toplevel = XtInitialize (NULL, "top", NULL, NULL, &argc, argv);

XtSetArg (wlm_args[0], XtNfile, argv[1]) ;
XtSetValues (toplevel, toplevel_args, XtNumber(toplevel_args)) ;
wlm = XtCreateManagedWidget ( "wlm", xcuWlmWidgetClass, toplevel, wlm_args, ONE) ;

XtRealizeWidget (toplevel) ;

XcuWlmSample (wlm) ;
for (;;)
    {
    fprintf (stderr, "\nMaking a request...\n") ;
    XcuWlmRequest ( wlm) ;
    }
}

#include "xlayout.wh"
