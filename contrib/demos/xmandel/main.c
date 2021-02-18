/*
 * Xmandel - written by John Freeman at Cray Research
 *
 * This file contains the main routine for driving the mandelbrot generator
 */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Paned.h>
#include <stdio.h>
Widget drawform;
int depth;


#define DEFAULT_DRAWING_WIDTH 150
#define DEFAULT_DRAWING_HEIGHT 150

/*
 * setup the internal action routines to be used
 */

static void do_clear (w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    XClearWindow (XtDisplay(drawform), XtWindow(drawform));
}

static void do_quit (w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    exit (0);
}

extern void do_mandel(), do_zoom(), do_unzoom(), do_julia();

static XtActionsRec xmandel_actions[] = {
  { "Quit", do_quit },
  { "Mandel", do_mandel },
  { "Zoom", do_zoom },
  { "Unzoom", do_unzoom },
  { "Julia", do_julia },
  { "Clear", do_clear },
};

static char *button_list[] = {
    "quit", "mandel", "zoom", "unzoom", "julia", "clear", NULL};


main (argc, argv)
    int argc;
    char **argv;
{
    Widget toplevel, pane, box, form;
    Arg av[10];
    Cardinal i;
    char **cpp;

    toplevel = XtInitialize (NULL, "Xmandel", NULL, 0, &argc, argv);
    XtAppAddActions (XtWidgetToApplicationContext (toplevel),
		     xmandel_actions, XtNumber (xmandel_actions));

    depth = DefaultDepthOfScreen (XtScreen(toplevel));	 /* XXX crock */

    pane = XtCreateManagedWidget ("pane", panedWidgetClass, toplevel,
				  NULL, ZERO);

    box = XtCreateManagedWidget ("box", boxWidgetClass, pane, NULL, ZERO);

    for (cpp = button_list; *cpp; cpp++) {
	(void) XtCreateManagedWidget (*cpp, commandWidgetClass, box,
				      NULL, ZERO);
    }

    form = XtCreateManagedWidget ("form", formWidgetClass, pane,
				  NULL, ZERO);

    i = 0;
    XtSetArg (av[i], XtNtop, XtChainTop); i++;
    XtSetArg (av[i], XtNbottom, XtChainBottom); i++;
    XtSetArg (av[i], XtNleft, XtChainLeft); i++;
    XtSetArg (av[i], XtNright, XtChainRight); i++;
    XtSetArg (av[i], XtNwidth, DEFAULT_DRAWING_WIDTH); i++;
    XtSetArg (av[i], XtNheight, DEFAULT_DRAWING_HEIGHT); i++;
    drawform = XtCreateManagedWidget ("window", widgetClass, form, av, i);


    XtRealizeWidget (toplevel);
    XtMainLoop ();
}
