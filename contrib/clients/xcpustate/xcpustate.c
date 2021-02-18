/*
*  Displays CPU state distribution
*/
#ifndef lint
static char rcsid[] = "$Header: /csri3/X.V11R3/localstuff/xcpustate/RCS/xcpustate.c,v 1.2 89/10/05 23:03:11 xwindows Exp $";
#endif /*lint*/

#include <stdio.h>
#include <sys/types.h>

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Form.h>
#include <X11/Label.h>
#include <X11/Xmu.h>
#include "Bar.h"
#include "s.h"

#define MAXSTR 512
#define DEF_INTERVAL	1
#define DEF_COUNT		-1

char *progname;
static int defaultInterval = DEF_INTERVAL;
static int defaultCount = DEF_COUNT;
static int count, interval;

/* Application Resources - no particular widget */
static XtResource application_resources[] = {
    {"interval", "Interval", XtRInt, sizeof(int),
	    (Cardinal)&interval, XtRInt, (caddr_t) &defaultInterval},
    {"count", "Count", XtRInt, sizeof(int),
	    (Cardinal)&count, XtRInt, (caddr_t) &defaultCount},
};

/*
*  Command line options table. The command line is parsed for these,
*  and it sets/overrides the appropriate values in the resource
*  database
*/
static XrmOptionDescRec optionDescList[] = {
    {"-interval",   ".interval",    XrmoptionSepArg,    (caddr_t) NULL},
    {"-count",	    ".count",	    XrmoptionSepArg,    (caddr_t) NULL},
};

/*
*  DON'T CHANGE THE ORDER OF THE ARGS IN THE VARIOUS ARG STRUCTS. IF
*  YOU WANT TO ADD STUFF, ADD IT AT THE END OF THE STRUCT, BECAUSE WE
*  REFER TO SOME OF THE ELEMENTS BY POSITION IN THE CODE.
*/
/* No spacing between the widgets on the Form */
static Arg form_args[] = {
    {XtNdefaultDistance, (XtArgVal) 0},
};

static Arg subform_args[] = {
    {XtNfromVert, (XtArgVal) 0},
    {XtNdefaultDistance, (XtArgVal) 0},
    {XtNborderWidth, (XtArgVal) 0},
    {XtNtop, (XtArgVal) XtChainTop},
    {XtNbottom, (XtArgVal) XtChainTop}, /* ChainBottom causes strange resize */
    {XtNright, (XtArgVal) XtChainRight},
    {XtNleft, (XtArgVal) XtChainLeft},
};

static Arg bar_args[] = {
    {XtNfromHoriz, (XtArgVal) 0},
    {XtNorientation, (XtArgVal) XtorientHorizontal},
    {XtNborderWidth, (XtArgVal) 1},
    {XtNlength, (XtArgVal) ((Dimension) 200)},
    {XtNthickness, (XtArgVal) ((Dimension) 20)},
    {XtNtop, (XtArgVal) XtChainTop},
    {XtNbottom, (XtArgVal) XtChainTop},
    {XtNright, (XtArgVal) XtChainRight},
    {XtNleft, (XtArgVal) XtChainLeft},
    {XtNvertDistance, (XtArgVal) 0},
    {XtNhorizDistance, (XtArgVal) 0},
    {XtNresizable, (XtArgVal) TRUE},
};

static Arg label_args[] = {
    {XtNlabel, (XtArgVal) 0},
    {XtNjustify, (XtArgVal) XtJustifyLeft},
    {XtNborderWidth, (XtArgVal) 0},
    {XtNtop, (XtArgVal) XtChainTop},
    {XtNbottom, (XtArgVal) XtChainTop},
    {XtNright, (XtArgVal) XtChainLeft},
    {XtNleft, (XtArgVal) XtChainLeft},
    {XtNvertDistance, (XtArgVal) 0},
    {XtNhorizDistance, (XtArgVal) 0},
    {XtNresizable, (XtArgVal) FALSE},
};

void
usage()
{
    (void) fprintf(stderr, "\
Usage: %s [Xt options] [-count iterations] [-interval delay_seconds\n",
     progname);
    exit(-1);
}

char *
xmalloc(n)
int n;
{
    extern char *malloc();
    char *cp = malloc((unsigned) n);
    if (cp == NULL) {
	    (void) fprintf(stderr, "Couldn't malloc %d bytes\n", n);
	    exit(-1);
    }
    return cp;
}

static int nbars;
static Widget *bar;
static char **barnames;
    
main(argc, argv)
int argc;
char **argv;
{
    int i;
    static XtTimerCallbackProc update_display();
    Widget topLevel;
    Widget form, label;
	Widget subform = NULL;
    extern char *strchr(/* const char *, char */);

    if ((progname = strchr(argv[0], '/')) == NULL)
	progname = argv[0];
    else
	progname++;

    nbars = num_bars();
    if (nbars == 0) {
	(void) fprintf(stderr, "num_bars returned 0 - something is wrong\n");
	exit(-1);
    }
    bar = (Widget *) xmalloc(nbars * sizeof(Widget));
    barnames = label_bars(nbars);

    topLevel = XtInitialize(progname, "CPUStateMonitor",
			    optionDescList, XtNumber(optionDescList),
			    &argc, argv);

    if (argc > 1)
	usage();
	
    XtGetApplicationResources(topLevel, 0, application_resources,
			      XtNumber(application_resources), NULL, 0 );

    form = XtCreateManagedWidget("form", formWidgetClass, topLevel,
				 form_args, XtNumber(form_args));
    
    for(i = 0; i < nbars; i++) {
#define FORMNAMEFMT	"form%d"
	char formname[sizeof(FORMNAMEFMT) + 32];
#define BARNAMEFMT	"bar%d"
	char barname[sizeof(BARNAMEFMT) + 32];

	if (i > 0)
	    subform_args[0].value = (XtArgVal) subform;
	(void) sprintf(formname, FORMNAMEFMT, i);
	subform = XtCreateManagedWidget(formname, formWidgetClass, form,
				 subform_args, XtNumber(subform_args));
	label_args[0].value = (XtArgVal) barnames[i];
	label = XtCreateManagedWidget(barnames[i], labelWidgetClass, subform,
				      label_args, XtNumber(label_args));
	(void) sprintf(barname, BARNAMEFMT, i);
	bar_args[0].value = (XtArgVal) label;
	bar[i] = XtCreateManagedWidget(barname, barWidgetClass, subform,
				    bar_args, XtNumber(bar_args));
    }
    XtRealizeWidget(topLevel);

    init_bars(nbars);
    
    (void) XtAddTimeOut((unsigned long) (interval * 1000), update_display,
			(caddr_t) NULL);
    XtMainLoop();
}

/* ARGSUSED */
static XtTimerCallbackProc
update_display(closure, id)
caddr_t closure;
XtIntervalId *id;
{
    display_bars(nbars);
    
    if (count > 0) {
	if (--count == 0)
	    return;
    }
    (void) XtAddTimeOut((unsigned long) (interval * 1000), update_display,
			(caddr_t) NULL);
}

void
draw_bar(i, states, nstates)
int i;
int states[];
int nstates;
{
    SetBarValues((Widget) bar[i], states, nstates);
}
