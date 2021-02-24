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
 */

#include <xgksesc.h>

#include "gks_implem.h"

void xXgksUpdateTrans(WS_STATE_PTR ws);
void XgksUpdateWsClip(WS_STATE_PTR ws, Glimit *bound);
void XgksXReDrawWs(WS_STATE_PTR ws);

/*
 * INQUIRE X ATTRIBUTES
 */

Gint gescinqxattr(Gint ws_id, Display **dpy, Window *win, GC *gc)
{
    WS_STATE_ENTRY *ws;

    /* check for proper gks state */
    GKSERROR((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errgescinqxattr);

    /* check for valid ws_id */
    GKSERROR((!VALID_WSID(ws_id)), 20, errgescinqxattr);

    /* check for workstation opened */
    GKSERROR(((ws = OPEN_WSID(ws_id)) == NULL), 25, errgescinqxattr);

    /* check for valid workstation category*/
    GKSERROR((WS_CAT(ws) == GMO), 31, errgescinqxattr);

    GKSERROR((WS_CAT(ws) == GMI), 33, errgescinqxattr);

    GKSERROR((WS_CAT(ws) == GINPUT), 35, errgescinqxattr);

    GKSERROR((WS_CAT(ws) == GWISS), 36, errgescinqxattr);

    *dpy = ws->dpy;
    *win = ws->win;
    *gc = ws->gc;

    return (OK);
}

/*
 * gescsetcolourmask( ws_id, mask ) ---- to change the colour plane mask
 * Gint ws_id;                  workstation identifier.
 * unsigned long mask;          the desired colour plane mask.
 */
Gint gescsetcolourmask(Gint ws_id, unsigned long mask)
{
    WS_STATE_PTR ws;

    /* check for proper gks state */
    GKSERROR((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errgescsetcolourmask);

    /* check for valid ws_id */
    GKSERROR((!VALID_WSID(ws_id)), 20, errgescsetcolourmask);

    /* check for workstation opened */
    GKSERROR(((ws = OPEN_WSID(ws_id)) == NULL), 25, errgescsetcolourmask);

    /* check for valid workstation category*/
    GKSERROR((WS_CAT(ws) == GMI), 33, errgescsetcolourmask);

    GKSERROR((WS_CAT(ws) == GINPUT), 35, errgescsetcolourmask);

    GKSERROR((WS_CAT(ws) == GWISS), 36, errgescsetcolourmask);

    /* set the colour plane mask value in workstation GC */
    XSetPlaneMask(ws->dpy, ws->gc, mask);

    return (OK);
}

/*
 * gescsetdcsize( ws_id, size ) ---- to change the workstation DC space size.
 * Gint ws_id;                  workstation identifier.
 * Gpoint size;                 the size of workstation DC space.
 */
Gint gescsetdcsize(Gint ws_id, Gpoint size)
{
    WS_STATE_PTR ws;

    /* check for proper gks state */
    GKSERROR((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errgescsetdcsize);

    /* check for valid ws_id */
    GKSERROR((!VALID_WSID(ws_id)), 20, errgescsetdcsize);

    /* check for workstation opened */
    GKSERROR(((ws = OPEN_WSID(ws_id)) == NULL), 25, errgescsetdcsize);

    /* check for valid workstation category */
    GKSERROR((WS_CAT(ws) == GMI), 33, errgescsetdcsize);

    GKSERROR((WS_CAT(ws) == GINPUT), 35, errgescsetdcsize);

    GKSERROR((WS_CAT(ws) == GWISS), 36, errgescsetdcsize);

    /* set the colour plane mask value in workstation GC */
    ws->size = size;

    /* move the pending transformation to the current transformation */
    if (ws->wsti.wstus != GNOTPENDING)
    {
        ws->wsti.current.w = ws->wsti.request.w;
        ws->wsti.current.v = ws->wsti.request.v;

        ws->wsti.wstus = GNOTPENDING;
    }

    /* check and modify the workstation vieport values */
    if (ws->wsti.current.v.xmin > size.x)
        ws->wsti.current.v.xmin = size.x;
    if (ws->wsti.current.v.ymin > size.y)
        ws->wsti.current.v.ymin = size.y;
    if (ws->wsti.current.v.xmax > size.x)
        ws->wsti.current.v.xmax = size.x;
    if (ws->wsti.current.v.ymax > size.y)
        ws->wsti.current.v.ymax = size.y;

    /* update the workstation transformation matrix and clip region */
    xXgksUpdateTrans(ws);
    XgksUpdateWsClip(ws, &(xgks_state.cliprec.rec));

    /* redraw the workstation content */
    XgksXReDrawWs(ws);

    return (OK);
}

/*
 * gescstoreprimi( ws_id, store ) ---- to set store non-segment output primitives
 * Gint ws_id;                  workstation identifier.
 * Gstore store;
 */
Gint gescstoreprimi(Gint ws_id, Gstore store)
{
    WS_STATE_PTR ws;

    /* check for proper gks state */
    GKSERROR((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errgescstoreprimi);

    /* check for valid ws_id */
    GKSERROR((!VALID_WSID(ws_id)), 20, errgescstoreprimi);

    /* check for workstation opened */
    GKSERROR(((ws = OPEN_WSID(ws_id)) == NULL), 25, errgescstoreprimi);

    /* check for valid workstation category*/
    GKSERROR((WS_CAT(ws) == GMI), 33, errgescstoreprimi);

    GKSERROR((WS_CAT(ws) == GINPUT), 35, errgescstoreprimi);

    GKSERROR((WS_CAT(ws) == GWISS), 36, errgescstoreprimi);

    /* set the non-segment output primitive storage value */
    ws->primi_store = store;

    return (OK);
}

/*
 * gescredrawnotify( ws_id, funcp ) ---- to set the user defined redraw notifying function pointer
 * Gint ws_id;                  workstation identifier.
 * Gint (*funcp)();             the pointer of redraw notifying function.
 */
Gint gescredrawnotify(Gint ws_id, void (*funcp)(Gint ws_id, Gredraw redraw))
{
    WS_STATE_PTR ws;
    /* check for proper gks state */
    GKSERROR((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errgescredrawnotify);

    /* check for valid ws_id */
    GKSERROR((!VALID_WSID(ws_id)), 20, errgescredrawnotify);

    /* check for workstation opened */
    GKSERROR(((ws = OPEN_WSID(ws_id)) == NULL), 25, errgescredrawnotify);

    /* check for valid workstation category*/
    GKSERROR((WS_CAT(ws) == GMI), 33, errgescredrawnotify);

    GKSERROR((WS_CAT(ws) == GINPUT), 35, errgescredrawnotify);

    GKSERROR((WS_CAT(ws) == GWISS), 36, errgescredrawnotify);

/* set the colour plane mask value in workstation GC */

#ifdef ESCAPEDEBUG
    fprintf(stderr, "redrawnotify=%d\n", ws->redrawfuncp);
#endif

    ws->redrawfuncp = funcp;

#ifdef ESCAPEDEBUG
    fprintf(stderr, "redrawnotify=%d\n", ws->redrawfuncp);
#endif

    return (OK);
}
