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
 * Urbana, IL   61801
 *
 * (C) Copyright 1987, 1988 by The University of Illinois Board of Trustees.
 *      All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 *
 */

#include <X11/Xlibint.h>

#include "gks_implem.h"

void XgksFudge(XRectangle *rectangle);

void xXgksSetForeground(Display *dpy, GC gc, int fg)
{
    if( gc->values.foreground!=fg)
       XSetForeground(dpy, gc, fg);
}


void xXgksSetLineAttributes(Display *dpy, GC gc, unsigned int line_width, int line_style, int cap_style, int join_style)
{
    if((gc->values.line_width!=line_width)
           ||(gc->values.line_style!=line_style)
           ||(gc->values.cap_style!=cap_style)
           ||(gc->values.join_style!=join_style))
     XSetLineAttributes(dpy, gc, line_width, line_style, cap_style, join_style);
}


void xXgksSetStipple(Display *dpy, GC gc, Pixmap stipple)
{ if(gc->values.stipple!=stipple)
         XSetStipple(dpy, gc, stipple);
}


void xXgksSetDashes(Display *dpy, GC gc, WS_STATE_PTR ws, Gint i)
{
    if(i!= ws->last_dash_index)
         XSetDashes(dpy, gc, 0, xgksDASHES[i].dashl, xgksDASHES[i].dn);
}


void xXgksSetTile(Display *dpy, GC gc, Pixmap tile)
{
    if(gc->values.tile!=tile)
         XSetTile(dpy, gc, tile);
}


void xXgksSetClipMask(Display *dpy, GC gc, Pixmap pixmap)
{
    if(gc->values.clip_mask!=pixmap)
      XSetClipMask(dpy, gc, pixmap);
}


void xXgksSetPlineClipRectangles(Display *dpy, GC gc, WS_STATE_PTR ws, XRectangle *rectangle)
{
    if((rectangle->x!=ws->last_pline_rectangle.x)
                ||(rectangle->y!=ws->last_pline_rectangle.y)
                ||(rectangle->width!=ws->last_pline_rectangle.width)
                ||(rectangle->height!=ws->last_pline_rectangle.height)){
                      XgksFudge(rectangle); /* c1008 */
                      XSetClipRectangles(dpy,gc,0, 0, rectangle, 1, Unsorted);
                      ws->last_pline_rectangle= *rectangle;
                      }
}

void xXgksSetPmarkerClipRectangles(Display *dpy, GC gc, WS_STATE_PTR ws, XRectangle *rectangle)
{
    if((rectangle->x!=ws->last_pmarker_rectangle.x)
                ||(rectangle->y!=ws->last_pmarker_rectangle.y)
                ||(rectangle->width!=ws->last_pmarker_rectangle.width)
                ||(rectangle->height!=ws->last_pmarker_rectangle.height)){
                      XgksFudge(rectangle); /* c1008 */
                      XSetClipRectangles(dpy,gc,0, 0, rectangle, 1, Unsorted);
                      ws->last_pmarker_rectangle= *rectangle;
                     }
}

void xXgksSetFillAreaClipRectangles(Display *dpy, GC gc, WS_STATE_PTR ws, XRectangle *rectangle)
{
    if((rectangle->x!=ws->last_farea_rectangle.x)
                ||(rectangle->y!=ws->last_farea_rectangle.y)
                ||(rectangle->width!=ws->last_farea_rectangle.width)
                ||(rectangle->height!=ws->last_farea_rectangle.height)){
                      XgksFudge(rectangle); /* c1008 */
                      XSetClipRectangles(dpy,gc,0, 0, rectangle, 1, Unsorted);
                      ws->last_farea_rectangle= *rectangle;
                      }
}

void xXgksSetTextClipRectangles(Display *dpy, GC gc, WS_STATE_PTR ws, XRectangle *rectangle)
{
    if((rectangle->x!=ws->last_text_rectangle.x)
                ||(rectangle->y!=ws->last_text_rectangle.y)
                ||(rectangle->width!=ws->last_text_rectangle.width)
                ||(rectangle->height!=ws->last_text_rectangle.height)){
                      XgksFudge(rectangle); /* c1008 */
                      XSetClipRectangles(dpy,gc,0, 0, rectangle, 1, Unsorted);
                      ws->last_text_rectangle= *rectangle;
                      }
}


void xXgksSetFillStyle(Display *dpy, GC gc, int fill_style)
{
    if( gc->values.fill_style !=fill_style)
       XSetFillStyle(dpy, gc, fill_style);
}

/* the following is just a fudge to make the clipping rectangle a little */
/* larger to keep it from clipping thing on the border.  Of course, this */
/* means that some things will be displayed that should be clipped!  But */
/* until XGKS does it's own clipping instead of passing all the clipping */
/* on to X, you have to choose one evil or the other. */
void XgksFudge(XRectangle *rectangle)
{
  rectangle->width += 2;
  rectangle->height += 2;
}
