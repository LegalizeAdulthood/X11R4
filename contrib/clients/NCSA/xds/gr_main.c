/*
 * National Center for SuperComputing Applications, University of Illinois
 *
 * This NCSA software product is public domain software.  Permission
 * is hereby granted to do whatever you like with it. Should you wish
 * to make a contribution towards the production of this software, please
 * send us your comments about your experience with the software,  why
 * you liked or disliked it, how you use it, and most importantly, how it
 * helps your work. We will receive your comments at softdev@ncsa.uiuc.edu.
 *
 * Please send bug reports to bugs@ncsa.uiuc.edu
 *
 * Author: Eng-Whatt Toh, National Center for Supercomputing Applications
 *         ewtoh@ncsa.uiuc.edu
 */

/*
 *	File:		gr_main.c
 *	Contents:	Main initialization/termination routines for graphics module
 */

#include "gr_com.h"
#include "gr_xwi.h"
#include "XDS.icon"
#include <signal.h>


/*
 * This is called by the main init function to initialize all variables
 * and other stuff pertaining to the graphics module
 */
void
gr_Init(argc,argv)
int		argc;
char	*argv[];
{
	Arg		argList[2];
	Pixmap	iconPixmap;
	extern	int	gr_CatchInterrupt();

	gr_topLevel = XtInitialize(argv[0],"XDS",NULL,0,&argc,argv);

    XtAddActions(gr_ActionsTable,XtNumber(gr_ActionsTable));

	gr_InitTopLevel(argv[0],gr_topLevel,&gr_topWin);

    (*signal)(SIGINT,gr_CatchInterrupt);

	XtRealizeWidget(gr_topLevel);

	gr_WidgetCursor(gr_topLevel,XC_draped_box);

	iconPixmap = XCreateBitmapFromData(XtDisplay(gr_topWin.shell),
		XtWindow(gr_topWin.shell), XDS_bits, XDS_width, XDS_height);
	XtSetArg(argList[0],XtNiconPixmap, iconPixmap);
	XtSetValues(gr_topLevel, argList, 1);

	gr_TextMsgOut("Product of National Center for Supercomputing Applications.\n");
	gr_PalInit(gr_topWin.shell);
	gr_ImageDefCMap();

	gr_TextMsgOut("\nClick on one of the buttons to open a File Window...\n");
}


/*
 * Main event loop
 */
void
gr_ProcessLoop()
{
	XtMainLoop();

	/*
	while (TRUE)
	{
		if (XtPending() == TRUE)
		{
			XEvent	report;

			XtNextEvent(&report);
			XtDispatchEvent(&report);
		}
	}
	*/
}


/*
 * Termination procedure.  Note that each termination call of a window
 * calls the termination procedures of all the child windows spawned by
 * it.
 */
void
gr_Terminate(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	/* Close all Boss windows.  When a boss dies, all his subordinates
	   gets killed too.  */

	while (gr_topWin.bossWin != NULL)
		gr_CloseBossLevel(gr_topWin.bossWin->shell,
			(caddr_t)gr_topWin.bossWin,(caddr_t)NULL);

	td_Terminate();
}

int
gr_CatchInterrupt(nargs,args)
int nargs;
char *args[];
{
	gr_TextMsgOut("X DataSlice is interrupted... goodbye.\n");
	gr_Terminate(gr_topWin.shell,NULL,NULL);
}
