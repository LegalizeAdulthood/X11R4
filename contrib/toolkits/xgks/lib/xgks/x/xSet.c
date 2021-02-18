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
 * Urbana, IL	61801
 *
 * (C) Copyright 1987, 1988 by The University of Illinois Board of Trustees.
 *	All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 *
 * $Log:	xSet.c,v $
 * Revision 1.1  89/09/18  17:24:56  jim
 * Initial revision
 * 
 * Revision 4.0  89/08/31  16:28:06  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.4  89/06/05  10:25:22  bruce
 * DCR# d1:	Changed include file name from gks_implement.h to gks_implem.h
 * 		for the AIX compiler.  Also added a display argument to all
 * 		calls to SIGIO functions.
 * 
 * Revision 1.3  89/02/20  11:55:38  amy
 * PTR c1008	xXgksSet<primitive>ClipRectangles:  added call to
 * 		XgksFudge to fudge clipping rectangle and make
 * 		clipping more accurate.
 * 		XgksFudge subroutine added.
 * 
 * Revision 1.2  89/02/04  15:28:00  amy
 * PTR c1147	Make global vars. and functions private, or static where possible.
 * 
 * Revision 1.1  88/08/18  14:41:12  amy
 * Initial revision
 * 
*/

static char *rscid="$Header: xSet.c,v 1.1 89/09/18 17:24:56 jim Exp $";

#include "../include/gks_implem.h"		/* d1 */


xXgksSetForeground(dpy, gc, fg)					/*c1147*/
Display *dpy;
GC gc;
int fg;
{
if( gc->values.foreground!=fg)
       XSetForeground(dpy, gc, fg);				/*c1147*/
}


xXgksSetLineAttributes(dpy, gc, line_width, line_style, cap_style, join_style) /*c1147*/
Display *dpy;
GC gc;
unsigned int line_width;
int line_style, cap_style, join_style;
{

if((gc->values.line_width!=line_width)
           ||(gc->values.line_style!=line_style)
           ||(gc->values.cap_style!=cap_style)
           ||(gc->values.join_style!=join_style))
     XSetLineAttributes(dpy, gc, line_width, line_style, cap_style, join_style);
}


xXgksSetStipple(dpy, gc, stipple)				/*c1147*/
Display *dpy;
GC gc;
Pixmap stipple;
{ if(gc->values.stipple!=stipple)
         XSetStipple(dpy, gc, stipple);
}


xXgksSetDashes(dpy, gc, ws, i) 				/*c1147*/
Display *dpy;
GC gc;
WS_STATE_PTR ws;
Gint i;
{ if(i!= ws->last_dash_index)
         XSetDashes(dpy, gc, 0, xgksDASHES[i].dashl, xgksDASHES[i].dn); /*c1147*/
}



xXgksSetTile(dpy, gc, tile)				/*c1147*/
Display *dpy;
GC gc;
Pixmap tile;
{ if(gc->values.tile!=tile)
         XSetTile(dpy, gc, tile);
}


xXgksSetClipMask(dpy, gc, pixmap)			/*c1147*/
Display *dpy;
GC gc;
Pixmap pixmap;
{ if(gc->values.clip_mask!=pixmap)
      XSetClipMask(dpy, gc, pixmap);
}


xXgksSetPlineClipRectangles(dpy,gc, ws, rectangle)		/*c1147*/
Display *dpy;
GC gc;
WS_STATE_PTR ws;
XRectangle *rectangle;
{ if((rectangle->x!=ws->last_pline_rectangle.x)
                ||(rectangle->y!=ws->last_pline_rectangle.y)
                ||(rectangle->width!=ws->last_pline_rectangle.width)
                ||(rectangle->height!=ws->last_pline_rectangle.height)){
		      XgksFudge(rectangle); /* c1008 */
                      XSetClipRectangles(dpy,gc,0, 0, rectangle, 1, Unsorted);
                      ws->last_pline_rectangle= *rectangle;
                      }
}

xXgksSetPmarkerClipRectangles(dpy,gc, ws, rectangle)		/*c1147*/
Display *dpy;
GC gc;
WS_STATE_PTR ws;
XRectangle *rectangle;
{ if((rectangle->x!=ws->last_pmarker_rectangle.x)
                ||(rectangle->y!=ws->last_pmarker_rectangle.y)
                ||(rectangle->width!=ws->last_pmarker_rectangle.width)
                ||(rectangle->height!=ws->last_pmarker_rectangle.height)){
		      XgksFudge(rectangle); /* c1008 */
                      XSetClipRectangles(dpy,gc,0, 0, rectangle, 1, Unsorted);
                      ws->last_pmarker_rectangle= *rectangle;
                     }
}

xXgksSetFillAreaClipRectangles(dpy,gc, ws, rectangle)		/*c1147*/
Display *dpy;
GC gc;
WS_STATE_PTR ws;
XRectangle *rectangle;
{ if((rectangle->x!=ws->last_farea_rectangle.x)
                ||(rectangle->y!=ws->last_farea_rectangle.y)
                ||(rectangle->width!=ws->last_farea_rectangle.width)
                ||(rectangle->height!=ws->last_farea_rectangle.height)){
		      XgksFudge(rectangle); /* c1008 */
                      XSetClipRectangles(dpy,gc,0, 0, rectangle, 1, Unsorted);
                      ws->last_farea_rectangle= *rectangle;
                      }
}

xXgksSetTextClipRectangles(dpy,gc, ws, rectangle)		/*c1147*/
Display *dpy;
GC gc;
WS_STATE_PTR ws;
XRectangle *rectangle;
{ if((rectangle->x!=ws->last_text_rectangle.x)
                ||(rectangle->y!=ws->last_text_rectangle.y)
                ||(rectangle->width!=ws->last_text_rectangle.width)
                ||(rectangle->height!=ws->last_text_rectangle.height)){
		      XgksFudge(rectangle); /* c1008 */
                      XSetClipRectangles(dpy,gc,0, 0, rectangle, 1, Unsorted);
                      ws->last_text_rectangle= *rectangle;
                      }
}


xXgksSetFillStyle(dpy, gc, fill_style)			/*c1147*/
Display *dpy;
GC gc;
int fill_style;
{ if( gc->values.fill_style !=fill_style)
       XSetFillStyle(dpy, gc, fill_style);
}

/* the following is just a fudge to make the clipping rectangle a little */
/* larger to keep it from clipping thing on the border.  Of course, this */
/* means that some things will be displayed that should be clipped!  But */
/* until XGKS does it's own clipping instead of passing all the clipping */
/* on to X, you have to choose one evil or the other. (DWO)              */
XgksFudge ( rectangle )    /* c1008 */
  XRectangle *rectangle;   /* c1008 */
  {                        /* c1008 */
  rectangle->width += 2;   /* c1008 */
  rectangle->height += 2;  /* c1008 */
  }                        /* c1008 */
