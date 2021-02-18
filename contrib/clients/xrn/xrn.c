
#ifndef lint
static char XRNrcsid[] = "$Header: /users/ricks/xrn-11/R4/RCS/xrn.c,v 1.11 89/11/17 12:53:11 ricks Rel Locker: ricks $";
#endif

/*
 * xrn - an X-based NNTP news reader
 *
 * Copyright (c) 1988, 1989, Ellen M. Sentovich and Rick L. Spickelmier.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * xrn.c: set up the main screens
 *
 */

#include "copyright.h"
#include <X11/Xos.h>
#include "utils.h"
#include "config.h"
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Text.h>
#include "xthelper.h"
#include "xmisc.h"
#include "resources.h"
#include "internals.h"
#include "error_hnds.h"
#include "buttons.h"
#include "xrn.h"


/* global variables that represent the widgets that are dynamically changed */

Widget topLevel;
Widget TopButtonBox;    /* button box containing the command buttons */
Widget BottomButtonBox; /* button box containing the article specific buttons */
Widget TopInfoLine;      /* top button info line                      */
Widget BottomInfoLine;   /* bottom button info line                   */
Widget Text;             /* newsgroup and article subject display     */
Widget ArticleText;      /* article display                           */

int XRNState;            /* XRN status: news and x                    */


/*ARGSUSED*/
main(argc, argv)
int argc;
char **argv;
{
    Widget titlebar, frame;
    static Arg frameArgs[] = {			/* main window description */
	{XtNx, (XtArgVal) 10},
	{XtNy, (XtArgVal) 10},
	{XtNheight, (XtArgVal) 800},
	{XtNwidth, (XtArgVal) 680},
    };
    static char title[LABEL_SIZE];
    static Arg labelArgs[] = {
	{XtNlabel, (XtArgVal) title},
	{XtNskipAdjust, (XtArgVal) True},
    };
    static Arg genericArgs[] = {
	{XtNskipAdjust, (XtArgVal) True},
    };
    static Arg boxArgs[] = {
	{XtNallowResize, (XtArgVal) True},
	{XtNskipAdjust, (XtArgVal) True},
    };
    static XawTextSelectType selarray[] = {XawselectLine, XawselectNull};
    static Arg textArgs[] = {			/* newsgroup/subject text window */
	{XtNstring,  (XtArgVal) ""},
	{XtNtextOptions, (XtArgVal) scrollVertical},
	{XtNselectTypes, (XtArgVal) selarray },
	{XtNskipAdjust, (XtArgVal) False},
    };
    Arg sizeArgs[2], fontArgs[1];
    int fontHeight;
    static Arg articleTextArgs[] = {		/* article/all text window */
	{XtNstring,  (XtArgVal) ""},
	{XtNtextOptions, (XtArgVal) (scrollVertical|wordBreak)},
	{XtNselectTypes, (XtArgVal) selarray},
	{XtNmin, (XtArgVal) 100},
	{XtNmax, (XtArgVal) 800},
	{XtNskipAdjust, (XtArgVal) False},
    };

    XRNState = 0;

    topLevel = Initialize(argc, argv);

    ehInstallSignalHandlers();
    ehInstallErrorHandlers();

    if (app_resources.geometry != NIL(char)) {
	int bmask;
	bmask = XParseGeometry(app_resources.geometry,       /* geometry specification */
			       (int *) &frameArgs[0].value,    /* x      */
			       (int *) &frameArgs[1].value,    /* y      */
			       (int *) &frameArgs[3].value,    /* width  */
			       (int *) &frameArgs[2].value);   /* height */

	/* handle negative x and y values */
	if ((bmask & XNegative) == XNegative) {
	    frameArgs[0].value += (XtArgVal) DisplayWidth(XtDisplay(topLevel),
							  DefaultScreen(XtDisplay(topLevel)));
	    frameArgs[0].value -= (int) frameArgs[3].value;
	}
	if ((bmask & YNegative) == YNegative) {
	    frameArgs[1].value += (XtArgVal) DisplayHeight(XtDisplay(topLevel),
							   DefaultScreen(XtDisplay(topLevel)));
	    frameArgs[1].value -= (int) frameArgs[2].value;
	}
    }
    
    /* create the pane and its widgets */
    
    frame = XtCreateManagedWidget("vpane", panedWidgetClass, topLevel,
				  frameArgs, XtNumber(frameArgs));
    
#ifdef __DATE__
    (void) sprintf(title, "xrn - version %s (compiled on %s)",
		   XRN_VERSION, __DATE__);
#else
    (void) sprintf(title, "xrn - version %s",
		   XRN_VERSION);
#endif
    
    titlebar = XtCreateManagedWidget("titlebar", labelWidgetClass, frame,
				     labelArgs, XtNumber(labelArgs));

    Text = XtCreateManagedWidget("index", asciiStringWidgetClass, frame,
					 textArgs, XtNumber(textArgs));

    XtSetArg(fontArgs[0], XtNheight, 0);
    XtGetValues(Text, fontArgs, XtNumber(fontArgs));

    /* 6 = 2 * yMargin + 2 */
    fontHeight = (int) fontArgs[0].value - 6;
    
    XtSetArg(sizeArgs[0], XtNmin,
	     app_resources.topLines * fontHeight + 4);
    XtSetArg(sizeArgs[1], XtNmax,
	     app_resources.topLines * fontHeight + 4);
    XtSetValues(Text, sizeArgs, XtNumber(sizeArgs));

    TopInfoLine = XtCreateManagedWidget("indexinfo", labelWidgetClass, frame,
					genericArgs, XtNumber(genericArgs));

    TopButtonBox = XtCreateManagedWidget("indexbuttons", boxWidgetClass, frame,
					  boxArgs, XtNumber(boxArgs));

    /* article display text window */
    XtSetArg(articleTextArgs[4], XtNmax, frameArgs[3].value);
    ArticleText = XtCreateManagedWidget("text", asciiStringWidgetClass, frame,
					 articleTextArgs, XtNumber(articleTextArgs));
    
    BottomInfoLine = XtCreateManagedWidget("textinfo", labelWidgetClass, frame,
					   genericArgs, XtNumber(genericArgs));

    BottomButtonBox = XtCreateManagedWidget("textbuttons", boxWidgetClass, frame,
					     boxArgs, XtNumber(boxArgs));

    /* initialize the news system, read the newsrc file */
    initializeNews();
    XRNState |= XRN_NEWS_UP;

    /* set up the text window, mode buttons, and question */
    determineMode();

    /* create the icon */
    xmIconCreate();
    
    XtRealizeWidget(topLevel);

    XtSetArg(fontArgs[0], XtNheight, 0);
    XtGetValues(titlebar, fontArgs, XtNumber(fontArgs));
    XawPanedSetMinMax(titlebar,
		      (int) fontArgs[0].value, (int) fontArgs[0].value);
    
    XtSetArg(fontArgs[0], XtNheight, 0);
    XtGetValues(TopInfoLine, fontArgs, XtNumber(fontArgs));
    XawPanedSetMinMax(TopInfoLine,
		      (int) fontArgs[0].value, (int) fontArgs[0].value);
    
    XtSetArg(fontArgs[0], XtNheight, 0);
    XtGetValues(BottomInfoLine, fontArgs, XtNumber(fontArgs));
    XawPanedSetMinMax(BottomInfoLine,
		      (int) fontArgs[0].value, (int) fontArgs[0].value);
    
    XtSetArg(fontArgs[0], XtNheight, 0);
    XtGetValues(TopButtonBox, fontArgs, XtNumber(fontArgs));
    XtSetArg(fontArgs[0], XtNheight, 0);
    XtGetValues(BottomButtonBox, fontArgs, XtNumber(fontArgs));
    
    /* allow user resizing of the text windows */
    XawPanedSetMinMax(Text, 10, (int) frameArgs[3].value);
    XawPanedSetMinMax(ArticleText, 100, (int) frameArgs[3].value);
    XawPanedAllowResize(Text, True);
    XawPanedAllowResize(ArticleText, True);

    unbusyCursor((Widget) 0, (caddr_t) 0, (caddr_t) 0);
    
    XRNState |= XRN_X_UP;

    XtMainLoop();

    exit(0);
}       

