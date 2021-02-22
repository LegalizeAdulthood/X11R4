/*
 *              Copyright IBM Corporation 1989
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
 * Urbana, IL   61801
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
 */

#include <string.h>
#include <signal.h>

#include "gks_implem.h"

int XgksSIGIO_OFF(Display *dpy);
int XgksSIGIO_ON(Display *dpy);

Gint xXgksOpenWs(WS_STATE_PTR wk)
{
        Display *dpy;
        int winX, winY, winH, winW;
        XSetWindowAttributes xswa;
        XWindowAttributes win_att;
        XEvent xev;

        WS_STATE_PTR wk_p;

        int i, j;
        char *ptr, *onoff;

        XWMHints wm_hints; /* window manager hints */
        XSizeHints size_hints;/* window size hints for wm */
        XClassHint class_hints;  /* class hints for wm */

        wm_hints.flags = InputHint | StateHint;
        wm_hints.input = 1;
        wm_hints.initial_state = NormalState;
        wm_hints.icon_pixmap = 0;
        wm_hints.icon_window = 0;
        wm_hints.icon_x = 0;
        wm_hints.icon_y = 0;
        wm_hints.icon_mask = 0;
        wm_hints.window_group = 0;

        size_hints.flags = USPosition| USSize| PPosition| PSize| PMinSize| PMaxSize| PResizeInc;
        size_hints.x = 0;
        size_hints.y = 0;
        size_hints.width = 0;
        size_hints.height = 0;
        size_hints.min_width = 50;
        size_hints.min_height = 50;
        size_hints.max_width = 32767;
        size_hints.max_height = 32767;
        size_hints.width_inc = 1;
        size_hints.height_inc = 1;
        size_hints.min_aspect.x = 1;     /* aspect hint are ignored */
        size_hints.min_aspect.y = 32767; /* see size_hints.flags above */
        size_hints.max_aspect.x = 32767;
        size_hints.max_aspect.y = 1;

        class_hints.res_name = "XGKS";
        class_hints.res_class = "APPLICATION";

        if (wk->ewstype != X_WIN) return (INVALID);

        /* Check if Display wk->conn has been opened  */

        wk_p = NULL;

        for (i = 0; i < MAX_OPEN_WS; i++) {
                if (xgks_state.openedws[i].ws_id < 0)
                        continue;
                if (xgks_state.openedws[i].ws->ewstype != X_WIN)
                        continue;

                wk_p = xgks_state.openedws[i].ws;
                j = STRCMP(wk->conn, wk_p->conn);

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
                        XgksSIGIO_ON(wk->dpy);
                        return (26); /* open display not successful */
                }

                ptr= DisplayString(wk->dpy);
                free (wk->conn);
                if ( (wk->conn = (char *) malloc ( (unsigned) (STRLEN(ptr)+1 ))) == NULL ) {
                    XgksSIGIO_ON(wk->dpy);
                    return (300);
                }

                STRCPY (wk->conn, ptr);

                /* get the screen default colour map ID */

                wk->dclmp = DefaultColormap(wk->dpy, DefaultScreen(wk->dpy));
                wk->wclmp = wk->dclmp;

                XSelectInput(wk->dpy, DefaultRootWindow(wk->dpy), FocusChangeMask );
                xXgksSIGIOStart(wk);
        }


        /* Set up window parameters, create and map window  */

        XgksSIGIO_OFF(wk->dpy);

        dpy = wk->dpy;

        wk->wscolour = DisplayCells(dpy, DefaultScreen(dpy));   /* size of colour map */

        /* init pointer to table of set values to NULL */
        /* The table will be alloc'ed and init'ed in   */
        /* gsetcolourrep on the 1st call */
        wk->set_colour_rep = (Gcobundl *)NULL;

        winW = 640;
        winH = 512;
        winX = (DisplayWidth(dpy, DefaultScreen(dpy)) - winW) >> 1;
        winY = (DisplayHeight(dpy, DefaultScreen(dpy)) - winH) >> 1;

        if (wk->wscolour == 2) {                /* MONOCHROME */
                onoff = XGetDefault( dpy, "xgks", "invertmono");
                if ( (onoff != NULL) && (STRCMP( onoff, "on") == 0) ) {
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

        /* we need Enter & Focus even as the window comes up */
        /* in case the cursor starts in the window (so set   */
        /* the color map to ours) */
        xswa.event_mask = FocusChangeMask | StructureNotifyMask
              | ExposureMask | EnterWindowMask;
        xswa.background_pixel = wk->wsbg;
        xswa.border_pixel = wk->wsfg;
        xswa.do_not_propagate_mask = (wk->event_mask & DeviceEventsMask);

        wk->win = XCreateWindow(dpy, DefaultRootWindow(dpy),
                                winX, winY, winW, winH, 5,
                                DefaultDepth(dpy, DefaultScreen(dpy)),
                                InputOutput, DefaultVisual(dpy, DefaultScreen(dpy)),
                 CWDontPropagate | CWBackPixel | CWBorderPixel | CWEventMask, &xswa);

        if (wk->win == 0) {
                XgksSIGIO_ON(wk->dpy);
                return (26);            /* open window not successful */
        }

        XChangeProperty( wk->dpy, wk->win, XA_WM_NAME, XA_STRING, 8, PropModeReplace, "XGKS", 4);

        XMapWindow(dpy, wk->win);

        /* block the program until the window mapped  */
        /*  (wait 'til we receive an exposure event)  */
        XWindowEvent(dpy, wk->win, ExposureMask, &xev);

        XSync(dpy, 0);


        /* create a GC for this new window  */

        wk->gc = XCreateGC(dpy, wk->win, 0, NULL);
        wk->plinegc=XCreateGC(dpy, wk->win, 0, NULL);
        wk->pmarkgc=XCreateGC(dpy, wk->win, 0, NULL);
        wk->fillareagc=XCreateGC(dpy, wk->win, 0, NULL);
        wk->textgc=XCreateGC(dpy, wk->win, 0, NULL);

        XSetForeground(dpy, wk->gc, wk->wsfg);
        XSetBackground(dpy, wk->gc, wk->wsbg);

        /* set some values  */

        for (i=0; i<MAX_OPEN_WS; i++)
                if (wk->ws_id == xgks_state.openedws[i].ws_id)
                        break;
        xgks_state.openedws[i].win = wk->win;   /* window identifier */

        /* Get currunt window attributes  */

        XGetWindowAttributes(dpy, wk->win, &win_att);

        wk->wbound.x = (Gfloat)win_att.width;
        wk->wbound.y = (Gfloat)win_att.height;

        /* NOTE: we don't have access to argv and */
        /*       argc, so for now we use NULL,0   */
        XSetStandardProperties (wk->dpy,wk->win,"XGKS", /* set props */
             "XGKS", None,NULL,0,&size_hints);          /* and size */

        XSetWMHints (wk->dpy,wk->win,&wm_hints);      /* set hints */

        size_hints.x = wk->wbound.x;                  /* user spec x */
        size_hints.y = wk->wbound.y;                  /* user spec y */
        size_hints.width = winW;                      /* prog spec w */
        size_hints.height = winH;                     /* prog spec h */
        XSetNormalHints (wk->dpy,wk->win,&size_hints);/* set size hints */

        size_hints.flags = 0;                         /* specify none   */
        XSetZoomHints (wk->dpy,wk->win,&size_hints);  /* set zoom hints */

        XSetClassHint (wk->dpy,wk->win,&class_hints); /* set class hint */


        /* Select Input Events */

        XSelectInput(dpy, wk->win, wk->event_mask);

        XSync(dpy, 0);

        XgksSIGIO_ON(wk->dpy);

        /* successful return */

        return (OK);
}


/*
 *  xXgksClearWs(wk) --- clear the corresponding x-window
 */
int xXgksClearWs(WS_STATE_PTR wk)
{

        if (wk->ewstype != X_WIN) return(OK);

        XgksSIGIO_OFF(wk->dpy);
        XClearArea(wk->dpy, wk->win, 0, 0, 0, 0, False);
        XSync(wk->dpy, 0);
        XgksSIGIO_ON(wk->dpy);

        return(OK);

}


/*
 * xXgksCloseWs(ws) --- close the corresponding x-window
 */

int xXgksCloseWs(WS_STATE_PTR ws)
{

        if (ws->ewstype != X_WIN) return(OK);

        XgksSIGIO_OFF(ws->dpy);
        XUnmapWindow(ws->dpy, ws->win);
        XDestroyWindow(ws->dpy, ws->win);
        XFreeGC(ws->dpy, ws->gc);
        XSync(ws->dpy, 0);
        XgksSIGIO_ON(ws->dpy);

        return(OK);
}


/*
 * xXgksHighLight(ws, bd) --- heighlight a primitive
 */

int xXgksHighLight(WS_STATE_PTR ws, Gpoint *bd)
{

        Display *dpy;
        Window win;
        GC gc;

        XPoint box[5];
        int i;

        dpy = ws->dpy;
        win = ws->win;
        gc = ws->gc;


        if (ws->ewstype != X_WIN) return(OK);


        XgksSIGIO_OFF(ws->dpy);
        XSetFunction(dpy, gc, GXinvert);
        XSetLineAttributes(dpy, gc, 0, LineSolid, CapButt, JoinMiter);
        XSetFillStyle(dpy, gc, FillSolid);

        for (i = 0; i < 5; i++) {
                box[i].x = bd->x * ws->ndctoxtrans.xScale + ws->ndctoxtrans.xTrans;
                box[i].y = ws->wbound.y - bd->y * ws->ndctoxtrans.yScale + ws->ndctoxtrans.yTrans;
                bd++;
        }

        XDrawLines(dpy, win, gc, box, 5, CoordModeOrigin);

        XFlush(ws->dpy);

        XSetFunction(dpy, gc, GXcopy);
        XgksSIGIO_ON(ws->dpy);

        return(OK);
}
