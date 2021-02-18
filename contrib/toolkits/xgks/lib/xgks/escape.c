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
 * $Header: escape.c,v 4.0 89/08/31 16:20:27 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/escape.c,v $
 *
 * $Log:	escape.c,v $
 * Revision 4.0  89/08/31  16:20:27  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.23  89/06/05  10:16:09  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * 
 * Revision 1.22  89/02/04  15:23:20  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 1.21  88/12/07  16:44:09  amy
 * Changed spelling of color to colour.
 * 
 * Revision 1.20  88/12/06  17:09:09  amy
 *  PTR c1130	GKSERROR calls:  changed function name parameter from character
 * 		string to enum. type value.
 * 
 * Revision 1.19  88/08/19  14:25:20  amy
 * gescinqxattr:  changed identifier 'wsid' to 'ws_id.'
 * 
 * Revision 1.18  88/08/16  14:13:35  amy
 * AUG  gescinqxattr:  function added; checks for errors 20 & 25 added.
 * 
 * Revision 1.17  88/08/11  09:18:13  amy
 * PTR c1012  gescsetcolourmask, gescsetdcsize, gescstoreprimi, &
 * gescredrawnotify:  added call to VALID_WSID to check for error 20.
 * 
 * Revision 1.2  88/07/26  17:51:49  owens
 * renamed/replaced VALID_WSID macro for PTR c1012 (DWO)
 * 
 * Revision 1.1  88/07/21  10:48:39  david
 * Initial revision
 *  
 *
 */
 
static char *rcsid = "$Header: escape.c,v 4.0 89/08/31 16:20:27 amy Exp $";
                     
#include "gks_implem.h" /* d1 */

/*
 * INQUIRE X ATTRIBUTES
 */

gescinqxattr( ws_id, dpy, win, gc )			/* AUG */
	Gint ws_id;
	Display **dpy;
	Window *win;
	GC *gc;
{
	WS_STATE_ENTRY *ws;

/* check for proper gks state */
	GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP) ,7, errgescinqxattr) /*c1147*/

/* check for valid ws_id */
/* ALP 8/16/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgescinqxattr)   /* c1012 */

/* check for workstation opened */
/* ALP 8/16/88  changed macro name from VALID_WSID */
	GKSERROR (((ws=OPEN_WSID(ws_id)) == NULL) ,25, errgescinqxattr)  /*c1012*/

/* check for valid workstation category*/
	GKSERROR ( (WS_CAT(ws)==GMO), 31, errgescinqxattr);

	GKSERROR ( (WS_CAT(ws)==GMI), 33, errgescinqxattr);

	GKSERROR ( (WS_CAT(ws)==GINPUT), 35, errgescinqxattr);
	
	GKSERROR ( (WS_CAT(ws)==GWISS), 36, errgescinqxattr);

	*dpy = ws->dpy;
	*win = ws->win;
	*gc = ws->gc;

	return( OK );
}

/* 
 * gescsetcolourmask( ws_id, mask ) ---- to change the colour plane mask
 * Gint ws_id;			workstation identifier.
 * unsigned long mask;		the desired colour plane mask.
 */
gescsetcolourmask( ws_id, mask )
Gint ws_id;	
unsigned long mask;
{
	WS_STATE_PTR ws;

/* check for proper gks state */
	GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP) ,7, errgescsetcolourmask) /*c1147*/

/* check for valid ws_id */
/* ALP 8/8/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgescsetcolourmask)   /* c1012 */

/* check for workstation opened */
/* ALP 8/8/88  changed macro name from VALID_WSID */
	GKSERROR (((ws=OPEN_WSID(ws_id)) == NULL) ,25, errgescsetcolourmask)  /*c1012*/

/* check for valid workstation category*/
	GKSERROR ( (WS_CAT(ws)==GMI), 33, errgescsetcolourmask);

	GKSERROR ( (WS_CAT(ws)==GINPUT), 35, errgescsetcolourmask);
	
	GKSERROR ( (WS_CAT(ws)==GWISS), 36, errgescsetcolourmask);

/* set the colour plane mask value in workstation GC */
	XSetPlaneMask( ws->dpy, ws->gc, mask );

	return(OK);
}
/* 
 * gescsetdcsize( ws_id, size ) ---- to change the workstation DC space size.
 * Gint ws_id;			workstation identifier.
 * Gpoint size;			the size of workstation DC space.
 */
gescsetdcsize( ws_id, size )
Gint ws_id;	
Gpoint size;
{
	WS_STATE_PTR ws;

/* check for proper gks state */
	GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP) ,7, errgescsetdcsize) /*c1147*/

/* check for valid ws_id */
/* ALP 8/8/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgescsetdcsize)   /* c1012 */

/* check for workstation opened */
/* ALP 8/8/88  changed macro name from VALID_WSID */
	GKSERROR (((ws=OPEN_WSID(ws_id)) == NULL) ,25, errgescsetdcsize)  /* c1012 */

/* check for valid workstation category */
	GKSERROR ( (WS_CAT(ws)==GMI), 33, errgescsetdcsize);

	GKSERROR ( (WS_CAT(ws)==GINPUT), 35, errgescsetdcsize);
	
	GKSERROR ( (WS_CAT(ws)==GWISS), 36, errgescsetdcsize);

/* set the colour plane mask value in workstation GC */
	ws->size = size;

/* move the pending transformation to the current transformation */
	if (ws->wsti.wstus != GNOTPENDING) {
		ws->wsti.current.w = ws->wsti.request.w;
		ws->wsti.current.v = ws->wsti.request.v;
	
		ws->wsti.wstus = GNOTPENDING;
	}

/* check and modify the workstation vieport values */
	if (ws->wsti.current.v.xmin > size.x) ws->wsti.current.v.xmin = size.x;
	if (ws->wsti.current.v.ymin > size.y) ws->wsti.current.v.ymin = size.y;
	if (ws->wsti.current.v.xmax > size.x) ws->wsti.current.v.xmax = size.x;
	if (ws->wsti.current.v.ymax > size.y) ws->wsti.current.v.ymax = size.y;

/* update the workstation transformation matrix and clip region */
	xXgksUpdateTrans(ws);					/*c1147*/
	XgksUpdateWsClip(ws, &(xgks_state.cliprec.rec));	/*c1147*/

/* redraw the workstation content */
	XgksXReDrawWs(ws);					/*c1147*/

	return(OK);
}
/* 
 * gescstoreprimi( ws_id, store ) ---- to set store non-segment output primitives
 * Gint ws_id;			workstation identifier.
 * Gstore store;		
 */
gescstoreprimi( ws_id, store )
Gint ws_id;	
Gstore store;
{
	WS_STATE_PTR ws;

/* check for proper gks state */
	GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP) ,7, errgescstoreprimi) /*c1147*/

/* check for valid ws_id */
/* check for invalid workstation id */
/* ALP 8/8/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgescstoreprimi)   /* c1012 */

/* check for workstation opened */
/* ALP 8/8/88  changed macro name from VALID_WSID */
	GKSERROR (((ws=OPEN_WSID(ws_id)) == NULL) ,25, errgescstoreprimi)  /* c1012 */

/* check for valid workstation category*/
	GKSERROR ( (WS_CAT(ws)==GMI), 33, errgescstoreprimi);

	GKSERROR ( (WS_CAT(ws)==GINPUT), 35, errgescstoreprimi);
	
	GKSERROR ( (WS_CAT(ws)==GWISS), 36, errgescstoreprimi);

/* set the non-segment output primitive storage value */
	ws->primi_store = store;

	return(OK);
}
/* 
 * gescredrawnotify( ws_id, funcp ) ---- to set the user defined redraw notifying function pointer
 * Gint ws_id;			workstation identifier.
 * Gint (*funcp)();		the pointer of redraw notifying function.
 */
gescredrawnotify( ws_id, funcp )
Gint ws_id;	
Gint (*funcp)();
{
	WS_STATE_PTR ws;
/* check for proper gks state */
	GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP) ,7, errgescredrawnotify) /*c1147*/

/* check for valid ws_id */
/* check for invalid workstation id */
/* ALP 8/8/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgescredrawnotify)   /* c1012 */

/* check for workstation opened */
/* ALP 8/8/88  changed macro name from VALID_WSID */
	GKSERROR (((ws=OPEN_WSID(ws_id)) == NULL) ,25, errgescredrawnotify)  /* c1012 */

/* check for valid workstation category*/
	GKSERROR ( (WS_CAT(ws)==GMI), 33, errgescredrawnotify);

	GKSERROR ( (WS_CAT(ws)==GINPUT), 35, errgescredrawnotify);
	
	GKSERROR ( (WS_CAT(ws)==GWISS), 36, errgescredrawnotify);

/* set the colour plane mask value in workstation GC */


#ifdef ESCAPEDEBUG
fprintf(stderr,"redrawnotify=%d\n",ws->redrawfuncp);
#endif

	ws->redrawfuncp = funcp;

#ifdef ESCAPEDEBUG
fprintf(stderr,"redrawnotify=%d\n",ws->redrawfuncp);
#endif

	return(OK);
}
