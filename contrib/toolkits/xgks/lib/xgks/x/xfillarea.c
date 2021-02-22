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
 * XGKS fillarea primitive output ws : the pointer to current workstation list
 * fill_ptr: the pointer to the output fillarea primitive
 *
 */

#include <signal.h>
#include <string.h>

#include "gks_implem.h"

int XgksSIGIO_OFF(Display *dpy);
int XgksSIGIO_ON(Display *dpy);
void xXgksSetForeground(Display *dpy, GC gc, int fg);
void xXgksSetLineAttributes(Display *dpy, GC gc, unsigned int line_width, int line_style, int cap_style, int join_style);
void xXgksSetFillStyle(Display *dpy, GC gc, int fill_style);
void xXgksSetFillAreaClipRectangles(Display *dpy, GC gc, WS_STATE_PTR ws, XRectangle *rectangle);
void xXgksSetTile(Display *dpy, GC gc, Pixmap tile);
void xXgksSetStipple(Display *dpy, GC gc, Pixmap stipple);

Gint xXgksFillArea(WS_STATE_PTR ws, FILL_AREA_ST *fill_ptr)
{
    Display *dpy;
    Window win;
    GC gc, stipgc;
    XGCValues gcv;
    XPoint *xpe, *ype, *ptt, *ptt1;
    Pixmap pixm;

    Gflinter fill_inter;
    int fill_style;
    int fill_colour;

    Gflbundl *idv_ptr, *bdl_ptr, *bundl_ptr;
    Gpoint *pe;
    Gint gi, *array;
    Gflattr *ptr;

    int i, w, h, npnt, n, nn;
    int px, py, prev, cur;

    if (ws->ewstype != X_WIN)
        return (OK);

    /* Initialization  */

    XgksSIGIO_OFF(ws->dpy);

    dpy = ws->dpy;
    win = ws->win;
    gc = ws->fillareagc;

    xpe = (XPoint *) malloc(sizeof(fill_ptr->pts[0]) * (fill_ptr->num_pts + 1));
    ype = xpe;

    GKSERROR((xpe == NULL), 300, errxFillArea);

    /* Set current GC values for the primitive  */

    ptr = &(fill_ptr->flattr);
    gi = ptr->fill;
    if (gi < 1 || gi >= MAX_BUNDL_TBL)
        gi = 1;
    idv_ptr = &(ptr->bundl);
    bdl_ptr = &(ws->flbundl_table[gi]);

    /* the colour attribute */

    if (ptr->colour == GBUNDLED)
        bundl_ptr = bdl_ptr;
    else
        bundl_ptr = idv_ptr;

    fill_colour = bundl_ptr->colour;
    if (!WS_AVAIL_COLOUR(ws, fill_colour))
        fill_colour = 1;
    if (ws->wscolour == 2)
    { /* monochrome ? */
        if (fill_colour == 0)
            fill_colour = ws->wsbg;
        else if (fill_colour == 1)
            fill_colour = ws->wsfg;
    }
    xXgksSetForeground(dpy, gc, fill_colour);

    /* the fill interior attribute */

    if (ptr->inter == GBUNDLED)
        bundl_ptr = bdl_ptr;
    else
        bundl_ptr = idv_ptr;

    fill_inter = bundl_ptr->inter;

    /* the fill style attribute */

    if (ptr->style == GBUNDLED)
        fill_style = bdl_ptr->style;
    else
        fill_style = idv_ptr->style;

    if (WS_FILL_TYPE(fill_inter, fill_style))
        if (fill_inter == GHATCH)
            fill_style += 1;
        else
            fill_style -= 1;
    else
        fill_style = 0;

    /* set GC values */

    xXgksSetLineAttributes(dpy, gc, 0, LineSolid, CapButt, JoinMiter);
    xXgksSetFillStyle(dpy, gc, FillSolid);

    /* Display Workstation Transformation */

    pe = fill_ptr->pts;
    npnt = fill_ptr->num_pts;

    for (i = 0; i < npnt; i++)
    {
        NdcToX(ws, pe, xpe);
        ++xpe;
        ++pe;
    }

    if (fill_ptr->pts[0].x != fill_ptr->pts[npnt - 1].x || fill_ptr->pts[0].y != fill_ptr->pts[npnt - 1].y)
    {
        xpe->x = ype->x;
        xpe->y = ype->y;
        npnt += 1;
    }

    /* Output the primitive  */

    if (fill_inter == GHOLLOW)
    {
        xXgksSetFillAreaClipRectangles(dpy, gc, ws, &(ws->xclip));
        XDrawLines(dpy, win, gc, ype, npnt, CoordModeOrigin);
    }
    else if (fill_inter == GSOLID)
    {
        xXgksSetFillAreaClipRectangles(dpy, gc, ws, &(ws->xclip));
        XFillPolygon(dpy, win, gc, ype, npnt, Complex, CoordModeOrigin);
    }
    else if (fill_inter == GPATTERN)
    {
        XSetClipMask(dpy, gc, None);
        pixm = XCreatePixmap(dpy, win,
            ws->ptbundl_table[fill_style].size.x,
            ws->ptbundl_table[fill_style].size.y,
            DefaultDepth(dpy, DefaultScreen(dpy)));

        w = ws->ptbundl_table[fill_style].size.x;
        h = ws->ptbundl_table[fill_style].size.y;
        nn = w * h;
        array = ws->ptbundl_table[fill_style].array;

        /* build the pixmap to tile with */
        ptt1 = (XPoint *) malloc(nn * sizeof(XPoint));
        GKSERROR((ptt1 == NULL), 300, errxFillArea);

        n = 0;
        ptt = ptt1;    /* count & array of same colour cells */
        prev = *array; /* previous colour */
        if (!WS_AVAIL_COLOUR(ws, prev))
            prev = 1;
        if (ws->wscolour == 2)
        { /* monochrome ? */
            if (prev == 0)
                prev = ws->wsbg;
            else if (prev == 1)
                prev = ws->wsfg;
        }
        for (py = 0; py < h; py++)
        {
            for (px = 0; px < w; px++)
            {
                cur = *array;
                if (!WS_AVAIL_COLOUR(ws, cur))
                    cur = 1;
                if (ws->wscolour == 2)
                { /* monochrome ? */
                    if (cur == 0)
                        cur = ws->wsbg;
                    else if (cur == 1)
                        cur = ws->wsfg;
                }
                if (cur != prev)
                { /* colour change */

                    xXgksSetForeground(dpy, gc, prev);
                    XDrawPoints(dpy, pixm, gc, ptt1, n, CoordModeOrigin);
                    n = 0;
                    ptt = ptt1; /* reset */
                    prev = cur;
                }
                ptt->x = px;
                ptt->y = py;
                ptt++;
                n++;
                array++;
            }
        }
        xXgksSetForeground(dpy, gc, prev);
        XDrawPoints(dpy, pixm, gc, ptt1, n, CoordModeOrigin);

        /* load the tile and draw the fill area */
        xXgksSetFillStyle(dpy, gc, FillTiled);
        xXgksSetTile(dpy, gc, pixm);

        xXgksSetFillAreaClipRectangles(dpy, gc, ws, &(ws->xclip));

        XFillPolygon(dpy, win, gc, ype, npnt, Complex, CoordModeOrigin);

        XFreePixmap(dpy, pixm);

        free(ptt1);
    }
    else if (fill_inter == GHATCH)
    {
        /*
 * Hatched fill areas have the fill area colour used for every non-zero cell
 * of the pattern array.  The zero cells are transparent. X maps the ones
 * in the stipple bitmap to the fill colour (Foreground) and ignores the zeros.
 */
        pixm = XCreatePixmap(dpy, win,
            ws->ptbundl_table[-fill_style].size.x,
            ws->ptbundl_table[-fill_style].size.y,
            1);
        gcv.foreground = 1;
        gcv.background = 0;
        stipgc = XCreateGC(dpy, pixm, GCForeground | GCBackground, &gcv);
        XSetClipMask(dpy, stipgc, None);

        w = ws->ptbundl_table[-fill_style].size.x;
        h = ws->ptbundl_table[-fill_style].size.y;
        nn = w * h;
        array = ws->ptbundl_table[-fill_style].array;

        /* build the pixmap to stipple with */
        ptt1 = (XPoint *) malloc(nn * sizeof(XPoint));
        GKSERROR((ptt1 == NULL), 300, errxFillArea);

        n = 0;
        ptt = ptt1;    /* count & array of same colour cells */
        prev = *array; /* previous colour */
        if (!WS_AVAIL_COLOUR(ws, prev))
            prev = 1;
        if (prev != 0)
            prev = 1;
        for (px = 0; px < w; px++)
        {
            for (py = 0; py < h; py++)
            {
                cur = *array;
                if (!WS_AVAIL_COLOUR(ws, cur))
                    cur = 1;
                if (cur != 0)
                    cur = 1;
                if (cur != prev)
                { /* colour change */
                    xXgksSetForeground(dpy, stipgc, prev);
                    XDrawPoints(dpy, pixm, stipgc, ptt1, n, CoordModeOrigin);
                    n = 0;
                    ptt = ptt1; /* reset */
                    prev = cur;
                }
                ptt->x = px;
                ptt->y = py;
                ptt++;
                n++;
                array++;
            }
        }
        xXgksSetForeground(dpy, stipgc, prev);
        XDrawPoints(dpy, pixm, stipgc, ptt1, n, CoordModeOrigin);

        /* load the tile and draw the fill area */
        xXgksSetForeground(dpy, gc, fill_colour);
        xXgksSetFillStyle(dpy, gc, FillStippled);
        xXgksSetStipple(dpy, gc, pixm);

        xXgksSetFillAreaClipRectangles(dpy, gc, ws, &(ws->xclip));

        XFillPolygon(dpy, win, gc, ype, npnt, Complex, CoordModeOrigin);

        XFreeGC(dpy, stipgc);
        XFreePixmap(dpy, pixm);

        free(ptt1);
    }

    XFlush(dpy);
    free(ype);
    XgksSIGIO_ON(dpy);

    return (OK);
}
