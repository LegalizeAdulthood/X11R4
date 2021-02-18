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
 * $Header: xpline.c,v 4.0 89/08/31 16:29:31 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/x/RCS/xpline.c,v $
 *
 * $Log:	xpline.c,v $
 * Revision 4.0  89/08/31  16:29:31  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.19  89/06/05  10:26:57  bruce
 * DCR# d1:	Changed include file name from gks_implement.h to gks_implem.h
 * 		for the AIX compiler.  Also added a display argument to all
 * 		calls to SIGIO functions.
 * 
 * Revision 3.18  89/02/04  15:29:45  amy
 * PTR c1147	Make global vars. and functions private, or static where possible.
 * 
 * Revision 3.17  88/12/08  14:24:31  amy
 * Changed spelling of color to colour.
 * 
 * Revision 3.16  88/09/26  10:32:17  amy
 * MIT	xPolyLine returns a value.
 * 
 * Revision 3.15  88/08/19  07:41:01  amy
 * AUG  DASHES array initialization changed.
 *      xPolyLine:  gc assignment changed from ws->gc to ws->plinegc.
 *      Call to XSetForeground changed to xSetForeground.
 *      Line width set to 0 only for solid lines.
 *      Calls changed as follows:
 *      Previously        Now
 *      ----------        ---
 *      XSetDashes        xSetDashes
 *      XSetLineAttributes xSetLineAttributes
 *      XSetFillStyle     xSetFillStyle
 *      XSetClipRectangles xSetPlineClipRectangles
 * 
 * Revision 1.1  88/07/21  10:52:08  david
 * Initial revision
 *  
 *
 */
 
static char rcsid[] = "$Header: xpline.c,v 4.0 89/08/31 16:29:31 amy Exp $";
 
#include "string.h"
#include "gks_implem.h"		/* d1 */
#include "signal.h"
 
/* Not static because the input devices need this information */
DashList xgksDASHES[10] = {						/*c1147*/
	{ 2, {8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} },	/* AUG */
	{12, {1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 3, 1, 0, 0, 0, 0, 0} },	/*  |  */
	{ 8, {2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0} },	/*  |  */
	{ 4, {4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} },	/* \ / */
	{16, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0} },	/*  v  */
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
 
 
xXgksPolyLine(ws, plin_ptr)					/*c1147*/
    WS_STATE_PTR ws;
    PLINE_ST *plin_ptr;
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
 
 
    if (ws->ewstype != X_WIN) return(OK);	/*MIT*/
 
 
    /* Initialization  */
 
    XgksSIGIO_OFF(ws->dpy);		/* d1 *//*c1147*/
 
    dpy = ws->dpy;
    win = ws->win;
    gc = ws->plinegc;	/* AUG */
 
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
    xXgksSetForeground(dpy, gc, i );	/* AUG */	/*c1147*/
 
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
	if (line_width == 1)		/* AUG */
		line_width = 0;		/* AUG */
    }
    else {                   /* set dashed line values */
        if ( i < 0 )
            i += 3;
        else
            i += 1;
        line_style = LineOnOffDash;
        xXgksSetDashes(dpy, gc, ws, i);	/* AUG */ /*c1147*/
    }

    xXgksSetLineAttributes(dpy, gc, line_width, line_style, CapButt, JoinMiter); /*AUG*/ /*c1147*/
    xXgksSetFillStyle(dpy, gc, FillSolid);	     /*AUG*/ /*c1147*/

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
 
    XgksSIGIO_ON(ws->dpy);		/* d1 *//*c1147*/

    return(OK);	/*MIT*/
}
 
