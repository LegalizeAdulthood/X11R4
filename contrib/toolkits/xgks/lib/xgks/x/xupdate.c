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
 * The following routines are just valid for Display window workstations 
 *
 * xUpdateTrans(ws): update the constants for transformations from NDC to DC
 * and then from DC to WIN.  The DC space is assumed to be 1280(X) by 1024(Y).
 * Every time the user changes the workstation window, the worstation viewport,
 * or the window space, this routine will be called.
 *
 * xUpdateClip(): update the clipping area constants in the X window.
 * It is calculated from NDC clipping area to X window clipping area.
 * Every time the user selects a new NDC transformation this routine will be
 * called for each opened display workstation.
 * 
 * $Header: xupdate.c,v 4.0 89/08/31 16:30:11 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/x/RCS/xupdate.c,v $
 *
 * $Log:	xupdate.c,v $
 * Revision 4.0  89/08/31  16:30:11  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.7  89/06/05  10:27:07  bruce
 * DCR# d1:	Changed include file name from gks_implement.h to gks_implem.h
 * 		for the AIX compiler.  Also added a display argument to all
 * 		calls to SIGIO functions.
 * 
 * Revision 3.6  89/02/20  11:58:32  amy
 * PTR c1008	xXgksUpdateTrans:  add fudge factor to make clipping
 * 		more accurate.
 * 
 * Revision 3.5  89/02/04  15:30:44  amy
 * PTR c1147	Make global vars. and functions private, or static where possible.
 * 
 * Revision 3.4  88/12/28  10:58:47  amy
 * xclip.width xclip.height:  cast values to unsigned short before assigning
 * to these variables.  xclip is an XRectangle, and the width and height
 * fields are unsigned shorts (see /andrew/usr/include/X11/Xlib.h).
 * 
 * Revision 3.3  88/08/19  12:49:54  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 1.1  88/07/21  10:52:22  david
 * Initial revision
 *  
 *
 */

static char rcsid[] = "$Header: xupdate.c,v 4.0 89/08/31 16:30:11 amy Exp $";


#include "string.h"
#include "gks_implem.h"		/* d1 */

xXgksUpdateTrans(ws)					/*c1147*/
	WS_STATE_PTR ws;
{

	float D_WWX, D_WWY, D_WVX, D_WVY,	/* size of worstation window and viewport */
	      D_DCX, D_DCY, D_XWX, D_XWY;       /* size of DC space and X window space    */

	float scale_WT,		       /* scale factor for WT transformation(NDC->DC) */
	      scale_DT;		       /* scale factor for DT transformation(DC->WIN) */

	float flt1, flt2;

	D_DCX = ws->size.x;
	D_DCY = ws->size.y;

	D_WWX = (float) (ws->wsti.current.w.xmax - ws->wsti.current.w.xmin);
	D_WWY = (float) (ws->wsti.current.w.ymax - ws->wsti.current.w.ymin);

	D_WVX = (float) (ws->wsti.current.v.xmax - ws->wsti.current.v.xmin);
	D_WVY = (float) (ws->wsti.current.v.ymax - ws->wsti.current.v.ymin);

	if ((flt1 = D_WVX / D_WWX) < (flt2 = D_WVY / D_WWY))
		scale_WT = flt1;
	else
		scale_WT = flt2;

	D_XWX = (float) (ws->wbound.x);
	D_XWY = (float) (ws->wbound.y);

	/* -1 is a fudge to fill window */
	if ((flt1 = D_XWX / (D_DCX)) < (flt2 = (D_XWY-1) / (D_DCY))) /* c1008 */
		scale_DT = flt1;
	else
		scale_DT = flt2;

/* NDC to DC transformation */
	ws->ndctodctrans.xScale = scale_WT;
	ws->ndctodctrans.yScale = scale_WT;
	ws->ndctodctrans.xTrans = ws->wsti.current.v.xmin - ws->wsti.current.w.xmin * scale_WT;
	ws->ndctodctrans.yTrans = ws->wsti.current.v.ymin - ws->wsti.current.w.ymin * scale_WT;

/* DC to X transformation */
	ws->dctoxtrans.xScale = scale_DT;
	ws->dctoxtrans.yScale = scale_DT;
	ws->dctoxtrans.xTrans = 0.0;
	ws->dctoxtrans.yTrans = 1.0;  /* c1008 - fudge to fill window */

/* NDC to X transformation */
	ws->ndctoxtrans.xScale = ws->ndctodctrans.xScale * ws->dctoxtrans.xScale;
	ws->ndctoxtrans.yScale = ws->ndctodctrans.yScale * ws->dctoxtrans.yScale;
	ws->ndctoxtrans.xTrans = ws->ndctodctrans.xTrans * ws->dctoxtrans.xScale + ws->dctoxtrans.xTrans;
	ws->ndctoxtrans.yTrans = ws->ndctodctrans.yTrans * ws->dctoxtrans.yScale + ws->dctoxtrans.yTrans;
}



xXgksUpdateClip(ws)					/*c1147*/
	WS_STATE_PTR ws;
{
	ws->xclip.x = (short) (ws->clip.xmin * ws->ndctoxtrans.xScale + ws->ndctoxtrans.xTrans);
	ws->xclip.y = (short) (ws->wbound.y - (ws->clip.ymax * ws->ndctoxtrans.yScale + ws->ndctoxtrans.yTrans));
	ws->xclip.width = (unsigned short) ((ws->clip.xmax - ws->clip.xmin) * ws->ndctoxtrans.xScale);
	ws->xclip.height = (unsigned short) ((ws->clip.ymax - ws->clip.ymin) * ws->ndctoxtrans.yScale);
}
