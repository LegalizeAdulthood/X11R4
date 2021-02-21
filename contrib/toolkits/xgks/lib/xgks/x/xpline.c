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
 * University of Illinois at Urbana-Champaign
 * Department of Computer Science
 * 1304 W. Springfield Ave.
 * Urbana, IL    61801
 *
 * (C) Copyright 1987, 1988 by The University of Illinois Board of Trustees.
 * All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 *
 * XGKS polyline primitive output
 * ws : the pointer to current workstation list
 * plin_ptr: the pointer to the output polyline primitive
 *
 */

#include <string.h>
#include <signal.h>

#include "gks_implem.h"

int XgksSIGIO_OFF(Display *dpy);
int XgksSIGIO_ON(Display *dpy);
void xXgksSetForeground(Display *dpy, GC gc, int fg);
void xXgksSetLineAttributes(Display *dpy, GC gc, unsigned int line_width, int line_style, int cap_style, int join_style);
void xXgksSetFillStyle(Display *dpy, GC gc, int fill_style);
void xXgksSetDashes(Display *dpy, GC gc, WS_STATE_PTR ws, Gint i);
void xXgksSetPlineClipRectangles(Display *dpy, GC gc, WS_STATE_PTR ws, XRectangle *rectangle);

/* Not static because the input devices need this information */
DashList xgksDASHES[10] = {                                             /*c1147*/
        { 2, {8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} },     /* AUG */
        {12, {1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 3, 1, 0, 0, 0, 0, 0} },     /*  |  */
        { 8, {2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0} },     /*  |  */
        { 4, {4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} },     /* \ / */
        {16, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0} },     /*  v  */
        { 8, {1, 2, 3, 2, 1, 2, 3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0} },
        { 4, {4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} },
        { 4, {4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} },
        { 4, {4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} },
        { 4, {4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} },
/*

                  {2, 10, 10, 0, 0, 0, 0, 0, 0, 0, 0},
                  {6, 10, 6, 1, 4, 1, 6, 0, 0, 0, 0},
                  {2, 3, 8, 0, 0, 0, 0, 0, 0, 0, 0},
                  {2, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0},
                  {2, 1, 6, 0, 0, 0, 0, 0, 0, 0, 0},
                  {4, 8, 4, 1, 4, 0, 0, 0, 0, 0, 0},
                  {2, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0},
                  {2, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0},
                  {2, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0},
                  {2, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0},
*/
};


int xXgksPolyLine(WS_STATE_PTR ws, PLINE_ST *plin_ptr)
{
    Display *dpy;
    Window win;
    GC gc;
    XPoint *xpe, *ype;

    unsigned int line_width;
    int line_style;

    Glnbundl *idv_ptr, *bdl_ptr, *bundl_ptr;
    Gpoint *pe;
    Gint gi;
    Glnattr *ptr;

    int i, npnt;


    if (ws->ewstype != X_WIN) return(OK);       /*MIT*/


    /* Initialization  */

    XgksSIGIO_OFF(ws->dpy);

    dpy = ws->dpy;
    win = ws->win;
    gc = ws->plinegc;   /* AUG */

    xpe = (XPoint *) malloc(sizeof(plin_ptr->pts[0]) * plin_ptr->num_pts);
    ype = xpe;

    /* Set current GC values for the primitive  */

    ptr = &(plin_ptr->plnattr);
    gi = ptr->line;
    if (gi <1 || gi >= MAX_BUNDL_TBL)
        gi = 1;
    idv_ptr = &(ptr->bundl);
    bdl_ptr = &(ws->lnbundl_table[gi]);

    if (ptr->colour == GBUNDLED)                    /* gc.foreground */
        bundl_ptr = bdl_ptr;
    else
        bundl_ptr = idv_ptr;

    i = bundl_ptr->colour;
    if ( ! WS_AVAIL_COLOUR(ws, i))
        i = 1;
    if ( ws->wscolour == 2 )    {    /* monochrome ? */
        if (i == 0) i = ws->wsbg;
        else if (i == 1) i = ws->wsfg;
    }
    xXgksSetForeground(dpy, gc, i );    /* AUG */       /*c1147*/

    /* the fill style attribute */


    if (ptr->width == GBUNDLED)            /* gc.line_width */
        line_width = bdl_ptr->width;    /* line width    */
    else
        line_width = idv_ptr->width;


    if (ptr->type == GBUNDLED)     /* gc.line_style */
        bundl_ptr = bdl_ptr;
    else                   /* line type     */
        bundl_ptr = idv_ptr;

    i=bundl_ptr->type;
    if (i == GLN_SOLID || !WS_LINE_TYPE(i)) {
        i = GLN_SOLID;
        line_style = LineSolid;
        if (line_width == 1)            /* AUG */
                line_width = 0;         /* AUG */
    }
    else {                   /* set dashed line values */
        if ( i < 0 )
            i += 3;
        else
            i += 1;
        line_style = LineOnOffDash;
        xXgksSetDashes(dpy, gc, ws, i); /* AUG */ /*c1147*/
    }

    xXgksSetLineAttributes(dpy, gc, line_width, line_style, CapButt, JoinMiter); /*AUG*/ /*c1147*/
    xXgksSetFillStyle(dpy, gc, FillSolid);           /*AUG*/ /*c1147*/

    xXgksSetPlineClipRectangles(dpy, gc, ws, &(ws->xclip));     /*AUG*/ /*c1147*/

    /* Display Workstation Transformation */

    pe = plin_ptr->pts;
    npnt = plin_ptr->num_pts;

    for (i = 0; i < npnt; i++) {
        NdcToX(ws, pe, xpe);
        ++xpe;
        ++pe;
    }

    /* Output the primitive  */

    XDrawLines(dpy, win, gc, ype, npnt, CoordModeOrigin);
    XFlush(dpy);

    free(ype);

    XgksSIGIO_ON(ws->dpy);

    return(OK); /*MIT*/
}

