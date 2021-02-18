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
 * XGKS open workstation function
 * Open display wk->conn if necessary, then create and map the workstation window 
 * wk: workstation list pointer return: (0)---- open succeeds (21)-- open fails .
 *
 * $Header: xopws.c,v 1.1 89/09/18 18:01:19 jim Exp $
 *
 * $Source: /xsrc/contrib/toolkits/xgks/lib/x/RCS/xopws.c,v $
 *
 * $Log:	xopws.c,v $
 * Revision 1.1  89/09/18  18:01:19  jim
 * Initial revision
 * 
 * Revision 4.0  89/08/31  16:29:18  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.29  89/06/16  08:33:27  bruce
 * PTR# c1185:	Moved call to SIGIO_OFF to a point where it has a valid
 * 		display pointer.
 * 
 * Revision 3.28  89/06/05  10:23:09  bruce
 * DCR# d1:	Changed include file name from gks_implement.h 
 * 		to gks_implem.h for AIX compiler.
 * 		Also added a display argument to all calls to 
 * 		SIGIO functions.
 * PTR# c1179:	Changed field name from exp to tx_exp or ch_exp
 * 		for AIX compiler.
 * 
 * Revision 3.27  89/02/20  13:51:15  amy
 * Fixed comment for c1152.
 * 
 * Revision 3.26  89/02/20  13:48:57  amy
 * PTR c1152	Initialize pointer to table of set colour values to NULL.
 * 
 * Revision 3.25  89/02/20  12:45:58  admin
 * *** empty log message ***
 * 
 * Revision 3.24  89/01/27  12:53:20  amy
 * Cosmetic change to Log.
 * 
 * Revision 3.23  89/01/27  12:46:57  amy
 * PTR c1145	xOpenWs:  Fill in the hints structures to let the window managers know 
 * 		about Xgks.  After opening a window we wait for an ExposeNotify.
 * 		For gsetcolourrep, added call to XInstallColormap.
 * 		Call XFlush after each XInstallColormap.
 * 
 * Revision 3.22  89/01/26  11:37:47  amy
 * PTR c1127	xOpenWs:  call DefaultColormap with DefaultScreen(wk->dpy), instead
 * 		of hard-coded 0.
 * 
 * Revision 3.21  88/12/08  17:53:46  amy
 * Changed Colourmap back to Colormap.
 * 
 * Revision 3.20  88/12/08  14:22:20  amy
 * Changed spelling of color to colour.
 * 
 * Revision 3.19  88/11/01  07:08:09  amy
 * PTR c1096	xOpenWs:  When X cannot open workstation, return GKS error
 * 		26 instead of 21.
 * 
 * Revision 3.18  88/09/26  10:16:41  amy
 * Edited the 'MIT' log entry.
 * 
 * Revision 3.17  88/09/26  10:14:08  amy
 * MIT	remove definition of variable status and its setting (was never referenced).
 * 	xClearWs, xCloseWs, xHighLight:  always return with a value.
 * 
 * Revision 3.16  88/08/19  07:18:18  amy
 * AUG  xOpenWs:  added check for unsuccessful attempt to open window.
 *      In creating GC for window, set the graphics contexts for the pline, pmark, 
 *      fillarea and text fields of the workstation state record.
 *      Changed parm list in call to XSetLineAttributes.
 * 
 * Revision 1.1  88/07/21  10:52:05  david
 * Initial revision
 *  
 *
 */

static char rcsid[] = "$Header: xopws.c,v 1.1 89/09/18 18:01:19 jim Exp $";

#include <string.h>
#include "gks_implem.h"		/* d1 */
#include <signal.h>

Gint xXgksOpenWs(wk)					/*c1147*/
	WS_STATE_PTR wk;

{
	Display *dpy;
	int winX, winY, winH, winW;
	XSetWindowAttributes xswa;
	XWindowAttributes win_att;
	XEvent xev;

	WS_STATE_PTR wk_p;

	int i, j;
	char *ptr, *onoff;

        XWMHints wm_hints; /* window manager hints */       /* c1145 c1179 */
        XSizeHints size_hints;/* window size hints for wm *//* c1145 c1179 */
        XClassHint class_hints;  /* class hints for wm */   /* c1145 c1179 */

        wm_hints.flags = InputHint | StateHint; /* c1179 */
        wm_hints.input = 1;                     /* c1179 */
        wm_hints.initial_state = NormalState;   /* c1179 */
        wm_hints.icon_pixmap = NULL;            /* c1179 */
        wm_hints.icon_window = NULL;            /* c1179 */
        wm_hints.icon_x = 0;                    /* c1179 */
        wm_hints.icon_y = 0;                    /* c1179 */
        wm_hints.icon_mask = NULL;              /* c1179 */
        wm_hints.window_group = NULL;           /* c1179 */

        size_hints.flags = USPosition| USSize| PPosition| PSize| PMinSize| PMaxSize| PResizeInc;
                       /* c1179 */
        size_hints.x = 0;                       /* c1179 */
        size_hints.y = 0;                       /* c1179 */
        size_hints.width = 0;                   /* c1179 */
        size_hints.height = 0;                  /* c1179 */
        size_hints.min_width = 50;              /* c1179 */
        size_hints.min_height = 50;             /* c1179 */
        size_hints.max_width = 32767;           /* c1179 */
        size_hints.max_height = 32767;          /* c1179 */
        size_hints.width_inc = 1;               /* c1179 */
        size_hints.height_inc = 1;              /* c1179 */
        size_hints.min_aspect.x = 1;     /* aspect hint are ignored    c1179*/
        size_hints.min_aspect.y = 32767; /* see size_hints.flags above c1179*/
        size_hints.max_aspect.x = 32767;        /* c1179 */
        size_hints.max_aspect.y = 1;            /* c1179 */

        class_hints.res_name = "XGKS";          /* c1179 */
        class_hints.res_class = "APPLICATION";  /* c1179 */

	if (wk->ewstype != X_WIN) return (INVALID);

	/* Check if Display wk->conn has been opened  */

	wk_p = NULL;

	for (i = 0; i < MAX_OPEN_WS; i++) {
		if (xgks_state.openedws[i].ws_id < 0)			/*c1147*/
			continue;
		if (xgks_state.openedws[i].ws->ewstype != X_WIN)	/*c1147*/
			continue;

		wk_p = xgks_state.openedws[i].ws;			/*c1147*/
		j = STRCMP(wk->conn, wk_p->conn);			/* c1176 */

		if ((j == 0) && (wk_p != wk))
			break;

		wk_p = NULL;
	}


	if (wk_p != NULL) {
		wk->dpy = wk_p->dpy;               /* has been opened  */
		wk->dclmp = wk_p->dclmp;
		wk->wclmp = wk_p->dclmp;
	}
	else { 
		/* open a new display */

		wk->dpy = XOpenDisplay(wk->conn);

		/* Check if open display successful  */

		if (wk->dpy == NULL) {
			XgksSIGIO_ON(wk->dpy);			/*c1147*//* d1 */
			return (26); /* open display not successful */ /*c1096*/
		}

		ptr= DisplayString(wk->dpy); 
		free (wk->conn);
		if ( (wk->conn = (char *) malloc ( (unsigned) (STRLEN(ptr)+1 ))) == NULL ) {/* c1176 */
		    XgksSIGIO_ON(wk->dpy);			/*c1147*//* d1 */
		    return (300);
		}

		STRCPY (wk->conn, ptr);		/* c1176 */

		/* get the screen default colour map ID */

		wk->dclmp = DefaultColormap(wk->dpy, DefaultScreen(wk->dpy));/*c1127*/
		wk->wclmp = wk->dclmp;

		XSelectInput(wk->dpy, DefaultRootWindow(wk->dpy), FocusChangeMask );
		xXgksSIGIOStart(wk);				/*c1147*//* d1 */
	}


	/* Set up window parameters, create and map window  */

	XgksSIGIO_OFF(wk->dpy);                 /* c1185 */

	dpy = wk->dpy;

	wk->wscolour = DisplayCells(dpy, DefaultScreen(dpy));	/* size of colour map */

	/* init pointer to table of set values to NULL */     /* c1152 */
        /* The table will be alloc'ed and init'ed in   */     /* c1152 */
        /* gsetcolourrep on the 1st call (DWO)         */     /* c1152 */
        wk->set_colour_rep = (Gcobundl *)NULL;                /* c1152 */

	winW = 640;
	winH = 512;
	winX = (DisplayWidth(dpy, DefaultScreen(dpy)) - winW) >> 1;
	winY = (DisplayHeight(dpy, DefaultScreen(dpy)) - winH) >> 1;

	if (wk->wscolour == 2) {		/* MONOCHROME */
		onoff = XGetDefault( dpy, "xgks", "invertmono");
		if ( (onoff != NULL) && (STRCMP( onoff, "on") == 0) ) {	/* c1176 */
			wk->wsbg = WhitePixel(dpy, DefaultScreen(dpy));
			wk->wsfg = BlackPixel(dpy, DefaultScreen(dpy));              
		}
		else {
			wk->wsfg = WhitePixel(dpy, DefaultScreen(dpy));
			wk->wsbg = BlackPixel(dpy, DefaultScreen(dpy));              
		}
	}
	else {
		wk->wsfg = 1;
		wk->wsbg = 0;
	}

	wk->event_mask = StructureNotifyMask | EnterWindowMask | 
	      ExposureMask | LeaveWindowMask
	      | KeyPressMask
	      | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask
	      | FocusChangeMask;

#define DeviceEventsMask (KeyPressMask | KeyReleaseMask | \
			  ButtonPressMask | ButtonReleaseMask | \
			  PointerMotionMask | Button1MotionMask | \
			  Button2MotionMask | Button3MotionMask | \
			  Button4MotionMask | Button5MotionMask | \
			  ButtonMotionMask)

	/* we need Enter & Focus even as the window comes up */  /* c1145 */
        /* in case the cursor starts in the window (so set   */  /* c1145 */
	/* the color map to ours)  (DWO)                     */  /* c1145 */
	xswa.event_mask = FocusChangeMask | StructureNotifyMask  /* c1145 */
              | ExposureMask | EnterWindowMask;                  /* c1145 */
	xswa.background_pixel = wk->wsbg;
	xswa.border_pixel = wk->wsfg;
	xswa.do_not_propagate_mask = (wk->event_mask & DeviceEventsMask);

	wk->win = XCreateWindow(dpy, DefaultRootWindow(dpy),
				winX, winY, winW, winH, 5,
				DefaultDepth(dpy, DefaultScreen(dpy)),
				InputOutput, DefaultVisual(dpy, DefaultScreen(dpy)),
	         CWDontPropagate | CWBackPixel | CWBorderPixel | CWEventMask, &xswa);

	if (wk->win == NULL) {		/* AUG */
		XgksSIGIO_ON(wk->dpy);	/* AUG *//* d1 */		/*c1147*/
		return (26);            /* open window not successful   AUG */
	}				/* AUG */
	
	XChangeProperty( wk->dpy, wk->win, XA_WM_NAME, XA_STRING, 8, PropModeReplace, "XGKS", 4);

	XMapWindow(dpy, wk->win);

	/* block the program until the window mapped  */   /* c1145 */
        /*  (wait 'til we receive an exposure event)  */   /* c1145 */
  	XWindowEvent(dpy, wk->win, ExposureMask, &xev);    /* c1145 */           
	
	XSync(dpy, 0);


	/* create a GC for this new window  */

	wk->gc = XCreateGC(dpy, wk->win, 0, NULL);
	wk->plinegc=XCreateGC(dpy, wk->win, 0, NULL);		/* AUG */
        wk->pmarkgc=XCreateGC(dpy, wk->win, 0, NULL);		/* AUG */
        wk->fillareagc=XCreateGC(dpy, wk->win, 0, NULL);	/* AUG */
        wk->textgc=XCreateGC(dpy, wk->win, 0, NULL);		/* AUG */
 
	XSetForeground(dpy, wk->gc, wk->wsfg);
	XSetBackground(dpy, wk->gc, wk->wsbg);

	/* set some values  */

	for (i=0; i<MAX_OPEN_WS; i++)
		if (wk->ws_id == xgks_state.openedws[i].ws_id)	/*c1147*/
			break;
	xgks_state.openedws[i].win = wk->win;	/* window identifier */ /*c1147*/

	/* Get currunt window attributes  */

	XGetWindowAttributes(dpy, wk->win, &win_att);		/*MIT*/

	wk->wbound.x = (Gfloat)win_att.width;
	wk->wbound.y = (Gfloat)win_att.height;

	/* NOTE: we don't have access to argv and */    /* c1145 */
        /*       argc, so for now we use NULL,0   */    /* c1145 */
        XSetStandardProperties (wk->dpy,wk->win,"XGKS", /* set props    c1145 */
             "XGKS", None,NULL,0,&size_hints);          /* and size     c1145 */

        XSetWMHints (wk->dpy,wk->win,&wm_hints);      /* set hints      c1145 */

	size_hints.x = wk->wbound.x;                  /* user spec x    c1145 */
	size_hints.y = wk->wbound.y;                  /* user spec y    c1145 */
	size_hints.width = winW;                      /* prog spec w    c1145 */
	size_hints.height = winH;                     /* prog spec h    c1145 */
        XSetNormalHints (wk->dpy,wk->win,&size_hints);/* set size hints c1145 */

	size_hints.flags = 0;                         /* specify none   c1145 */
        XSetZoomHints (wk->dpy,wk->win,&size_hints);  /* set zoom hints c1145 */

        XSetClassHint (wk->dpy,wk->win,&class_hints); /* set class hint c1145 */


	/* Select Input Events */

	XSelectInput(dpy, wk->win, wk->event_mask); 

	XSync(dpy, 0);

	XgksSIGIO_ON(wk->dpy);	/* d1 */	/*c1147*/

	/* successful return */

	return (OK);
}


/* 
 *  xXgksClearWs(wk) --- clear the corresponding x-window
 */
xXgksClearWs (wk)					/*c1147*/
WS_STATE_PTR wk;
{

	if (wk->ewstype != X_WIN) return(OK);	/*MIT*/

	XgksSIGIO_OFF(wk->dpy);		/* d1 *//*c1147*/
	XClearArea(wk->dpy, wk->win, 0, 0, 0, 0, False);
	XSync(wk->dpy, 0);
	XgksSIGIO_ON(wk->dpy);		/* d1 *//*c1147*/

	return(OK);	/*MIT*/

}


/*
 * xXgksCloseWs(ws) --- close the corresponding x-window
 */

xXgksCloseWs(ws)					/*c1147*/
WS_STATE_PTR ws;
{

	if (ws->ewstype != X_WIN) return(OK);	/*MIT*/

	XgksSIGIO_OFF(ws->dpy);		/* d1 *//*c1147*/
	XUnmapWindow(ws->dpy, ws->win);
	XDestroyWindow(ws->dpy, ws->win);
	XFreeGC(ws->dpy, ws->gc);
	XSync(ws->dpy, 0);
	XgksSIGIO_ON(ws->dpy);		/* d1 *//*c1147*/

	return(OK);	/*MIT*/
}


/*
 * xXgksHighLight(ws, bd) --- heighlight a primitive
 */

xXgksHighLight(ws, bd)					/*c1147*/
Gpoint *bd;
WS_STATE_PTR ws;
{

	Display *dpy;
	Window win;
	GC gc;

	XPoint box[5];
	int i;

	dpy = ws->dpy;
	win = ws->win;
	gc = ws->gc;


	if (ws->ewstype != X_WIN) return(OK);	/*MIT*/


	XgksSIGIO_OFF(ws->dpy);		/* d1 *//*c1147*/
	XSetFunction(dpy, gc, GXinvert);
	XSetLineAttributes(dpy, gc, 0, LineSolid, CapButt, JoinMiter);	/* AUG */
	XSetFillStyle(dpy, gc, FillSolid);

	for (i = 0; i < 5; i++) {
		box[i].x = bd->x * ws->ndctoxtrans.xScale + ws->ndctoxtrans.xTrans; 
		box[i].y = ws->wbound.y - bd->y * ws->ndctoxtrans.yScale + ws->ndctoxtrans.yTrans; 
		bd++;
	}

	XDrawLines(dpy, win, gc, box, 5, CoordModeOrigin);

	XFlush(ws->dpy);
	
	XSetFunction(dpy, gc, GXcopy);
	XgksSIGIO_ON(ws->dpy);		/* d1 *//*c1147*/

	return(OK);	/*MIT*/
}
