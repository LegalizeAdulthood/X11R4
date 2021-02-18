
#ifndef lint
static char XRNrcsid[] = "$Header: /users/ricks/xrn-11/R4/RCS/xmisc.c,v 1.7 89/11/17 12:53:05 ricks Rel $";
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
 * xmisc.c: routines for handling miscellaneous x functions
 *
 */

#include "copyright.h"
#include <X11/Xos.h>
#include "config.h"
#include "utils.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/cursorfont.h>
#include "xthelper.h"
#include "resources.h"
#include "internals.h"
#include "xrn.h"
#include "xmisc.h"


/* XRN icon */

#include "xrn.icon"

void
xmIconCreate()
{
    char *bits;
    int width, height;
    Pixmap icon_pixmap = (Pixmap) 0;
    Arg arg;
    
    width = xrn_width;
    height = xrn_height;
    bits = xrn_bits;

    /* XXX user sets iconPixmap resource, converter does the right thing.. */
    XtSetArg(arg, XtNiconPixmap, &icon_pixmap);
    XtGetValues(topLevel, &arg, 1);

    if (icon_pixmap == (Pixmap) 0) {
	XtSetArg(arg, XtNiconPixmap,
		 XCreateBitmapFromData(XtDisplay(topLevel),
				       XtScreen(topLevel)->root,
				       bits, width, height));
	XtSetValues(topLevel, &arg, 1);
    }

    if (app_resources.iconGeometry != NIL(char)) {
	int scr, x, y, junk;
	Arg args[2];

	for(scr = 0;	/* yyuucchh */
	    XtScreen(topLevel) != ScreenOfDisplay(XtDisplay(topLevel), scr);
	    scr++);

	XGeometry(XtDisplay(topLevel), scr, app_resources.iconGeometry,
		  "", 0, 0, 0, 0, 0, &x, &y, &junk, &junk);
	XtSetArg(args[0], XtNiconX, x);
	XtSetArg(args[1], XtNiconY, y);
	XtSetValues(topLevel, args, XtNumber(args));
    }
    return;
}


/* 
 * create the normal and busy xrn cursors
 */

/*ARGSUSED*/
void
busyCursor(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    static Cursor busyCursor = (Cursor) 0;
    
    /* define an appropriate busy cursor */
    if (busyCursor == (Cursor) 0) {
	busyCursor = XCreateFontCursor(XtDisplay(topLevel), XC_watch);
    }
    XDefineCursor(XtDisplay(topLevel), XtWindow(topLevel), busyCursor);
    XDefineCursor(XtDisplay(topLevel), XtWindow(Text), busyCursor);
    XDefineCursor(XtDisplay(topLevel), XtWindow(ArticleText), busyCursor);
    xthHandleAllPendingEvents();
    
    return;
}


/*ARGSUSED*/
void
unbusyCursor(widget, client_data, call_data)
Widget widget;
caddr_t client_data;
caddr_t call_data;
{
    static Cursor unBusyCursor = (Cursor) 0;
    static Cursor textCursor = (Cursor) 0;

    if (unBusyCursor == (Cursor) 0) {
	unBusyCursor = XCreateFontCursor(XtDisplay(topLevel), XC_left_ptr);
	textCursor = XCreateFontCursor(XtDisplay(topLevel), XC_xterm);
    }
    XDefineCursor(XtDisplay(topLevel), XtWindow(topLevel), unBusyCursor);
    XDefineCursor(XtDisplay(topLevel), XtWindow(Text), textCursor);
    XDefineCursor(XtDisplay(topLevel), XtWindow(ArticleText), textCursor);
    
    return;
}
