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
 * XGKS colour related functions
 *      xXgksSetColourRep():
 *      xXgksInqColourRep():
 *
 */

#include <signal.h>
#include <string.h>

#include "gks_implem.h"

int XgksSIGIO_OFF(Display *dpy);
int XgksSIGIO_ON(Display *dpy);

int xXgksSetColourRep(ws, idx, rep)
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

    if (ws->ewstype != X_WIN)
        return (OK);

    /* restore the Signal Default Function */

    XgksSIGIO_OFF(ws->dpy);

    /* initial some values and check the index value */

    dpy = ws->dpy;
    win = ws->win;
    dclmp = ws->dclmp;
    wclmp = ws->wclmp;
    visual = DefaultVisual(dpy, DefaultScreen(dpy));
    ncolours = ws->wscolour;

    if (ncolours < 3)
    {
        XgksSIGIO_ON(ws->dpy); /* for black&white screen */
        return (0);
    }

    if ((idx < 0) || (idx >= ncolours))
    {
        XgksSIGIO_ON(ws->dpy); /* index value out of the size of the colour map */
        return (1);
    }

    /* if window colour map has not been created, create a window colour map */
    /* for different visual type create corresponding type of colour map */
    /* copy the entries from the screen default colour map               */

    if (dclmp == wclmp)
    {
        colours = (XColor *) malloc(sizeof(colour_ret) * ncolours);

        switch (visual->class)
        {
        case StaticGray:
        case StaticColor:
        case TrueColor:
            ws->wclmp = XCreateColormap(dpy, win,
                visual, AllocNone);
            break;
        default:
            ws->wclmp = XCreateColormap(dpy, win,
                visual, AllocAll);
            break;
        }

        wclmp = ws->wclmp;

        clrs = colours;

        for (i = 0; i < ncolours; i++)
        {
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

    colour_ret.pixel = (unsigned long) idx;
    colour_ret.red = (short) (65535 * rep->red);
    colour_ret.green = (short) (65535 * rep->green);
    colour_ret.blue = (short) (65535 * rep->blue);
    colour_ret.flags = DoRed | DoGreen | DoBlue;
    XStoreColor(dpy, wclmp, &colour_ret);

    /* If this ws has the focus, then change the color  */
    /* right now, otherwise the actual change will have */
    /* to wait until we do have the focus.  (focus_ws   */
    /* is updated in xevent.c) */
    if ((ws != NULL) && (ws == xgks_state.focus_ws))
    {
        XInstallColormap(dpy, ws->wclmp);
        XFlush(dpy);
        /* NOTE: If we don't flush, switching quickly */
        /*       in and out of the window could cause */
        /*       the wrong color map to be displayed. */
    }

    /* store the interrupt of I/O signals */

    XgksSIGIO_ON(ws->dpy);

    return (0);
}

int xXgksInqColourRep(ws, idx, type, rep)
    WS_STATE_PTR ws;
int idx;
Gqtype type;
Gcobundl *rep;

{
    Display *dpy;
    Colormap wclmp;
    XColor colour_ret;
    int ncolours;

    /* NOTE: This routine is called for the GREALIZED case! */
    /*       When type == GSET, everything is handled in */
    /*       ginqcolourrep() in colors.c */

    if (ws->ewstype != X_WIN)
        return (OK);

    /* restore the Signal Default Function */

    XgksSIGIO_OFF(ws->dpy);

    /* check the validity of the index value */

    dpy = ws->dpy;
    wclmp = ws->wclmp;
    ncolours = ws->wscolour;

    /* removed check for valid idx here because that */
    /* check has already been done before this       */
    /* routine is called                             */

    /* get the RGB values */

    colour_ret.pixel = idx;
    XQueryColor(dpy, wclmp, &colour_ret);

    XAllocColor(dpy, wclmp, &colour_ret);

    /* set the returned RGB values */

    rep->red = (Gfloat) colour_ret.red / 65535.0;
    rep->green = (Gfloat) colour_ret.green / 65535.0;
    rep->blue = (Gfloat) colour_ret.blue / 65535.0;

    XgksSIGIO_ON(ws->dpy);

    return (0);
}
