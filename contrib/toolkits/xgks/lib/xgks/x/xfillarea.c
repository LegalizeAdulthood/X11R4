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
 * XGKS fillarea primitive output ws : the pointer to current workstation list 
 * fill_ptr: the pointer to the output fillarea primitive 
 * 
 * $Header: xfillarea.c,v 4.0 89/08/31 16:28:53 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/x/RCS/xfillarea.c,v $
 *
 * $Log:	xfillarea.c,v $
 * Revision 4.0  89/08/31  16:28:53  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.19  89/06/05  10:26:44  bruce
 * DCR# d1:	Changed include file name from gks_implement.h to gks_implem.h
 * 		for the AIX compiler.  Also added a display argument to all
 * 		calls to SIGIO functions.
 * 
 * Revision 3.18  89/05/22  09:50:34  bruce
 * PTR# c1174:	Changed fill area primitive function so it now uses
 * 		negative values for hatch styles.
 * 
 * Revision 3.17  89/05/19  14:17:22  bruce
 * PTR# c2044:	Corrected accidental transposition of 2-d array.
 * 
 * Revision 3.16  89/02/04  15:29:12  amy
 * PTR c1147	Make global vars. and functions private, or static where possible.
 * 
 * Revision 3.15  88/12/14  13:16:13  amy
 * PTR c1124	xFillArea:  changed call from xSetClipMask to XSetClipMask
 * 		because the intermediary procedure seemed not to pass along the
 * 		parameters correctly.
 * 
 * Revision 3.14  88/12/08  14:16:13  amy
 * PTR c1130	GKSERROR calls:  changed function name parameter from character
 * 		string to enum. type value.
 * Changed spelling of color to colour.
 * 
 * Revision 3.13  88/11/05  11:21:00  amy
 * No change.
 * 
 * Revision 3.12  88/09/26  10:05:49  amy
 * MIT	xFillArea:  always return a value.
 * 
 * Revision 3.11  88/08/19  06:36:55  amy
 * AUG  xFillArea:  replaced assignment of ws->gc to gc by ws->fillareagc.
 *      xFillArea:  replaced some of calls to XSetForeground with xSetForeground.
 *      Replaced calls to functions as follows:
 *      previously         now
 *      ----------         ---
 *      XSetLineAttributes xSetLineAttributes
 *      XSetFillStyle      xSetFillStyle
 *      XSetClipRectangles xSetFillAreaClipRectangles
 *      XSetClipMask       xSetClipMask
 *      XSetTile           xSetTile
 * 
 *      gpoint function deleted.
 * 
 * Revision 1.1  88/07/21  10:52:00  david
 * Initial revision
 *  
 *
 */

static char rcsid[] = "$Header: xfillarea.c,v 4.0 89/08/31 16:28:53 amy Exp $";

#include "string.h"
#include "gks_implem.h"		/* d1 */
#include "signal.h"

xXgksFillArea(ws, fill_ptr)				/*c1147*/
	WS_STATE_PTR ws;
	FILL_AREA_ST *fill_ptr;
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

	if (ws->ewstype != X_WIN) return(OK);	/*MIT*/

	/* Initialization  */

	XgksSIGIO_OFF(ws->dpy);		/* d1 *//*c1147*/

	dpy = ws->dpy;
	win = ws->win;
	gc = ws->fillareagc;	/* AUG */

	xpe = (XPoint *) malloc(sizeof(fill_ptr->pts[0]) * (fill_ptr->num_pts+1));
	ype = xpe;

	GKSERROR( (xpe == NULL), 300, errxFillArea);

	/* Set current GC values for the primitive  */

	ptr = &(fill_ptr->flattr);
	gi = ptr->fill;
	if (gi <1 || gi >= MAX_BUNDL_TBL)
		gi = 1;
	idv_ptr = &(ptr->bundl);
	bdl_ptr = &(ws->flbundl_table[gi]);

	/* the colour attribute */

	if (ptr->colour == GBUNDLED) 
		bundl_ptr = bdl_ptr;
	else
		bundl_ptr = idv_ptr;

	fill_colour = bundl_ptr->colour;
	if ( ! WS_AVAIL_COLOUR(ws, fill_colour))
		fill_colour = 1;
	if ( ws->wscolour == 2 )	{	/* monochrome ? */
		if (fill_colour == 0) fill_colour = ws->wsbg;
		else if (fill_colour == 1) fill_colour = ws->wsfg;
	}
	xXgksSetForeground(dpy, gc, fill_colour );  /* AUG */ /*c1147*/

	/* the fill interior attribute */	/* c1174 */

	if (ptr->inter == GBUNDLED)
		bundl_ptr = bdl_ptr;
	else
		bundl_ptr = idv_ptr;

	fill_inter = bundl_ptr->inter;

	/* the fill style attribute */		/* c1174 */

	if (ptr->style == GBUNDLED)
		fill_style = bdl_ptr->style;
	else
		fill_style = idv_ptr->style;

        if (WS_FILL_TYPE(fill_inter,fill_style))        /* c1174 */
           if (fill_inter == GHATCH)                    /* c1174 */
                fill_style += 1;                        /* c1174 */
           else                                         /* c1174 */
                fill_style -= 1;                        /* c1174 */
        else                                            /* c1174 */
                fill_style = 0;                         /* c1174 */

	/* set GC values */

	xXgksSetLineAttributes(dpy, gc, 0, LineSolid, CapButt, JoinMiter); /* AUG */ /*c1147*/
	xXgksSetFillStyle(dpy, gc, FillSolid);		/* AUG */ /*c1147*/

	/* Display Workstation Transformation */

	pe = fill_ptr->pts;
	npnt = fill_ptr->num_pts;

	for (i = 0; i < npnt; i++) {
		NdcToX(ws, pe, xpe);
		++xpe;
		++pe;
	}

	if (fill_ptr->pts[0].x != fill_ptr->pts[npnt-1].x || 
	    fill_ptr->pts[0].y != fill_ptr->pts[npnt-1].y) {
		xpe->x = ype->x;
		xpe->y = ype->y;
		npnt += 1;
	}

	/* Output the primitive  */

	if (fill_inter == GHOLLOW) {
		xXgksSetFillAreaClipRectangles(dpy, gc, ws, &(ws->xclip)); /* AUG */ /*c1147*/
		XDrawLines(dpy, win, gc, ype, npnt, CoordModeOrigin);
	}
	else if (fill_inter == GSOLID) {
		xXgksSetFillAreaClipRectangles(dpy, gc, ws, &(ws->xclip)); /* AUG */ /*c1147*/
		XFillPolygon(dpy, win, gc, ype, npnt, Complex, CoordModeOrigin);
	}
	else if (fill_inter == GPATTERN) {
		XSetClipMask(dpy, gc, None); /* AUG - xSet.. doesn't work with 'None' c1124 */
		pixm = XCreatePixmap(dpy, win,
			ws->ptbundl_table[fill_style].size.x, 
			ws->ptbundl_table[fill_style].size.y,
			DefaultDepth(dpy, DefaultScreen(dpy)));

		w = ws->ptbundl_table[fill_style].size.x; 
		h = ws->ptbundl_table[fill_style].size.y;
		nn = w * h;
		array = ws->ptbundl_table[fill_style].array;

/* build the pixmap to tile with */
		ptt1 = (XPoint *)malloc( nn * sizeof(XPoint));
		GKSERROR( (ptt1 == NULL), 300, errxFillArea);

		n = 0; ptt = ptt1;	/* count & array of same colour cells */
		prev = *array;		/* previous colour */
		if ( ! WS_AVAIL_COLOUR(ws, prev))
			prev = 1;
		if ( ws->wscolour == 2 ) {	/* monochrome ? */
			if (prev == 0) prev = ws->wsbg;
			else if (prev == 1) prev = ws->wsfg;
		}
		for( py=0; py<h; py++) {                        /* c2044 */
			for( px=0; px<w; px++) {                /* c2044 */
				cur = *array;
				if ( ! WS_AVAIL_COLOUR(ws, cur))
					cur = 1;
				if ( ws->wscolour == 2 ) { /* monochrome ? */
					if (cur == 0) cur = ws->wsbg;
					else if (cur == 1) cur = ws->wsfg;
				}
				if ( cur != prev ) {	/* colour change */

					xXgksSetForeground(dpy, gc, prev ); /* AUG */ /*c1147*/
					XDrawPoints(dpy, pixm, gc, ptt1, n, CoordModeOrigin );
					n = 0; ptt = ptt1;	/* reset */
					prev = cur;
				}
				ptt->x = px; ptt->y = py; ptt++;
				n++;
				array++;
			}
		}
		xXgksSetForeground(dpy, gc, prev );  /* AUG */ /*c1147*/
		XDrawPoints(dpy, pixm, gc, ptt1, n, CoordModeOrigin );

/* load the tile and draw the fill area */
		xXgksSetFillStyle(dpy, gc, FillTiled);	/* AUG */ /*c1147*/
		xXgksSetTile(dpy, gc, pixm);		/* AUG */ /*c1147*/

		xXgksSetFillAreaClipRectangles(dpy, gc, ws,  &(ws->xclip)); /* AUG */ /*c1147*/

		XFillPolygon(dpy, win, gc, ype, npnt, Complex, CoordModeOrigin);

		XFreePixmap(dpy, pixm);
		
		free(ptt1);
	}
	else if (fill_inter == GHATCH) {
/*
 * Hatched fill areas have the fill area colour used for every non-zero cell
 * of the pattern array.  The zero cells are transparent. X maps the ones
 * in the stipple bitmap to the fill colour (Foreground) and ignores the zeros.
 */
		pixm = XCreatePixmap(dpy, win,
			ws->ptbundl_table[-fill_style].size.x, 		/* c1174 */
			ws->ptbundl_table[-fill_style].size.y,		/* c1174 */
			1 );
		gcv.foreground = 1;
		gcv.background = 0;
		stipgc = XCreateGC(dpy, pixm, GCForeground|GCBackground, &gcv);
		XSetClipMask(dpy, stipgc, None);

		w = ws->ptbundl_table[-fill_style].size.x; 		/* c1174 */
		h = ws->ptbundl_table[-fill_style].size.y;		/* c1174 */
		nn = w * h;
		array = ws->ptbundl_table[-fill_style].array;		/* c1174 */

/* build the pixmap to stipple with */
		ptt1 = (XPoint *)malloc( nn * sizeof(XPoint));
		GKSERROR( (ptt1 == NULL), 300, errxFillArea);

		n = 0; ptt = ptt1;	/* count & array of same colour cells */
		prev = *array;		/* previous colour */
		if ( ! WS_AVAIL_COLOUR(ws, prev))
			prev = 1;
		if (prev != 0) prev = 1;
		for( px=0; px<w; px++) {
			for( py=0; py<h; py++) {
				cur = *array;
				if ( ! WS_AVAIL_COLOUR(ws, cur))
					cur = 1;
				if (cur != 0) cur = 1;
				if ( cur != prev ) {	/* colour change */
					xXgksSetForeground(dpy, stipgc, prev ); /*c1147*/
					XDrawPoints(dpy, pixm, stipgc, ptt1, n, CoordModeOrigin );
					n = 0; ptt = ptt1;	/* reset */
					prev = cur;
				}
				ptt->x = px; ptt->y = py; ptt++;
				n++;
				array++;
			}
		}
		xXgksSetForeground(dpy, stipgc, prev ); /*c1147*/
		XDrawPoints(dpy, pixm, stipgc, ptt1, n, CoordModeOrigin );

/* load the tile and draw the fill area */
		xXgksSetForeground(dpy, gc, fill_colour ); /* AUG */ /*c1147*/
		xXgksSetFillStyle(dpy, gc, FillStippled);  /* AUG */ /*c1147*/
		xXgksSetStipple(dpy, gc, pixm);		   /* AUG */ /*c1147*/

		xXgksSetFillAreaClipRectangles(dpy, gc, ws, &(ws->xclip)); /* AUG */ /*c1147*/

		XFillPolygon(dpy, win, gc, ype, npnt, Complex, CoordModeOrigin);

		XFreeGC(dpy, stipgc);
		XFreePixmap(dpy, pixm);
		
		free(ptt1);
	}

	XFlush(dpy);
	free( ype );
	XgksSIGIO_ON(dpy);		/* d1 *//*c1147*/

	return(OK);
}



