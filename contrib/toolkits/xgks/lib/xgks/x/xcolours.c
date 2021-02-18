/*
 *		Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 *
 * University of Illinois at Urbana-Champaign
 * Department of Computer Science
 * 1304 W. Springfield Ave.
 * Urbana, IL	61801
 *
 * (C) Copyright 1987, 1988 by The University of Illinois Board of Trustees.
 * All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 * 
 * XGKS colour related functions
 *	xXgksSetColourRep(): 
 *	xXgksInqColourRep():
 * 
 * $Header: xcolours.c,v 4.0 89/08/31 16:28:24 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/x/RCS/xcolours.c,v $
 *
 * $Log:	xcolours.c,v $
 * Revision 4.0  89/08/31  16:28:24  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.14  89/06/05  10:26:24  bruce
 * DCR# d1:	Changed include file name from gks_implement.h to gks_implem.h
 * 		for the AIX compiler.  Also added a display argument to all
 * 		calls to SIGIO functions.
 * 
 * Revision 3.13  89/02/20  13:46:53  amy
 * PTR c1152	xXgksInqColourRep now called only for the GREALIZED case.
 * 
 * Revision 3.12  89/02/04  15:28:33  amy
 * PTR c1147	Make global vars. and functions private, or static where possible.
 * 
 * Revision 3.11  89/01/27  12:40:10  amy
 * PTR c1145	xSetColourRep:  check if the workstation has the focus, and
 * 		if so, change the colour table.
 * 
 * Revision 3.10  89/01/26  11:39:54  amy
 * PTR c1127	xSetColourRep:  call DefaultVisual with DefaultScreen(dpy) , instead of
 * 		hard-coded 0.  Also, call XCreateColormap with correct display pointer.
 * 
 * Revision 3.9  88/12/09  09:50:07  amy
 * Changed XQueryColour back to XQueryColor, XStoreColour back to
 * XStoreColor, and XAllocColour back to XAllocColor.
 * 
 * Revision 3.8  88/12/08  19:07:04  amy
 * Changed StaticColour back to StaticColor, and TrueColour to TrueColor.
 * 
 * Revision 3.7  88/12/08  17:51:00  amy
 * Changed Colourmap back to Colormap.
 * 
 * Revision 3.6  88/12/08  17:24:17  amy
 * Changed XColour back to XColor.
 * 
 * Revision 3.5  88/12/08  13:58:32  amy
 * Changed spelling of color to colour.
 * 
 * Revision 3.4  88/10/28  14:17:02  amy
 * PTR c1111.	xInqColourRep check for valid colour index was disallowing index = 0.
 * 
 * Revision 3.3  88/08/18  14:53:09  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 1.1  88/07/21  10:51:55  david
 * Initial revision
 *  
 *
 */

static char rcsid[] = "$Header: xcolours.c,v 4.0 89/08/31 16:28:24 amy Exp $";

#include <string.h>
#include "gks_implem.h"		/* d1 */
#include <signal.h>

int xXgksSetColourRep(ws, idx, rep)				/*c1147*/
	WS_STATE_PTR ws;
	int idx;
	Gcobundl *rep;

{
	Display *dpy;
	Window win;
	Colormap dclmp, wclmp;
	XColor colour_ret;
	XColor *colours, *clrs;
	Visual *visual;
	int ncolours;

	int i;

	if (ws->ewstype != X_WIN) return (OK);

	/* restore the Signal Default Function */

	XgksSIGIO_OFF(ws->dpy);			/* d1 *//*c1147*/

	/* initial some values and check the index value */

	dpy = ws->dpy;
	win = ws->win;
	dclmp = ws->dclmp;
	wclmp = ws->wclmp;
	visual = DefaultVisual(dpy, DefaultScreen(dpy) ); /* c1127 */
	ncolours = ws->wscolour;

	if ( ncolours < 3 ) {
		XgksSIGIO_ON(ws->dpy);    /* for black&white screen */	/*c1147*//* d1 */
		return(0);
	}

	if ( (idx < 0) || (idx >=ncolours) ) {				/*c1111*/
		XgksSIGIO_ON(ws->dpy); /* index value out of the size of the colour map */ /*c1147 d1 */
		return(1);
	}

	/* if window colour map has not been created, create a window colour map */
	/* for different visual type create corresponding type of colour map */
	/* copy the entries from the screen default colour map               */

	if (dclmp == wclmp) {

		colours = (XColor *) malloc(sizeof(colour_ret) * ncolours);

		switch(visual->class) {
			case StaticGray:
			case StaticColor:
			case TrueColor: 
				ws->wclmp = XCreateColormap(dpy, win, 
 					visual, AllocNone); /* c1127 */
				break;
			default:
				ws->wclmp = XCreateColormap(dpy, win, 
 					visual, AllocAll); /* c1127 */
				break;
		}

		wclmp = ws->wclmp;

		clrs = colours;

		for (i=0; i<ncolours; i++) {
			clrs->pixel = i;
			clrs->flags = DoRed | DoGreen | DoBlue;
			clrs++;
		}

		XQueryColors(dpy, dclmp, colours, ncolours);
		XStoreColors(dpy, wclmp, colours, ncolours);

		XSetWindowColormap(dpy, win, wclmp);

		XSync(dpy, 0);

		free(colours);

	}

	/* set user defined colour representation in the window colour map */

	colour_ret.pixel = (unsigned long)idx;
	colour_ret.red = (short)(65535 * rep->red);
	colour_ret.green = (short)(65535 * rep->green);
	colour_ret.blue = (short)(65535 * rep->blue);
	colour_ret.flags = DoRed | DoGreen | DoBlue;
	XStoreColor(dpy, wclmp, &colour_ret);

	/* If this ws has the focus, then change the color  */ /* c1145 */
        /* right now, otherwise the actual change will have */ /* c1145 */
        /* to wait until we do have the focus.  (focus_ws   */ /* c1145 */
        /* is updated in xevent.c)    (DWO)                 */ /* c1145 */
        if ((ws != NULL) && (ws == xgks_state.focus_ws)) /* c1145 */ /*c1147*/
            {                                                  /* c1145 */
            XInstallColormap(dpy, ws->wclmp);                  /* c1145 */
            XFlush (dpy);                                      /* c1145 */
            /* NOTE: If we don't flush, switching quickly */   /* c1145 */
            /*       in and out of the window could cause */   /* c1145 */
            /*       the wrong color map to be displayed. */   /* c1145 */
            /*                                (DWO)       */   /* c1145 */
            }                                                  /* c1145 */
        

	/* store the interrupt of I/O signals */

	XgksSIGIO_ON(ws->dpy);				/* d1 *//*c1147*/

	return(0);
}


	    
int xXgksInqColourRep(ws, idx, type, rep)			/*c1147*/
	WS_STATE_PTR ws;
	int idx;
	Gqtype type;
	Gcobundl *rep;

{
	Display *dpy;
	Colormap wclmp;
	XColor colour_ret;
	int ncolours;

/*****************************************************************/ /* c1152 */
/* NOTE: This routine is now only called for the GREALIZED case! */ /* c1152 */
/*       When type == GSET, everything is handled in             */ /* c1152 */
/*       ginqcolourrep() in colors.c  (DWO)                      */ /* c1152 */
/*****************************************************************/ /* c1152 */

	if (ws->ewstype != X_WIN) return (OK);

	/* restore the Signal Default Function */

	XgksSIGIO_OFF(ws->dpy);			/* d1 *//*c1147*/

	/* check the validity of the index value */

	dpy = ws->dpy;
	wclmp = ws->wclmp;
	ncolours = ws->wscolour;

	/* removed check for valid idx here because that */ /* PTR# c1111 */
        /* check has already been done before this       */ /* PTR# c1111 */
        /* routine is called                             */ /* PTR# c1111 */

	/* get the RGB values */

	colour_ret.pixel = idx;
	XQueryColor(dpy, wclmp, &colour_ret);

	XAllocColor(dpy, wclmp, &colour_ret);   /* c1152 */
	
	/* set the returned RGB values */

	rep->red = (Gfloat)colour_ret.red / 65535.0;
	rep->green = (Gfloat)colour_ret.green / 65535.0;
	rep->blue = (Gfloat)colour_ret.blue / 65535.0;

	XgksSIGIO_ON(ws->dpy);				/* d1 *//*c1147*/

	return(0);
}
	    

/* c1147:  Deleted xInqColourInd */
