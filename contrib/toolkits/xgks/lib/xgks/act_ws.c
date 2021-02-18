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
 * act_ws.c - functions for workstation activation/deactivation.
 *
 *		gactivatews()
 *		gdeactivatews()
 * 
 * $Header: act_ws.c,v 4.0 89/08/31 16:19:37 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/act_ws.c,v $
 *
 * $Log:	act_ws.c,v $
 * Revision 4.0  89/08/31  16:19:37  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.13  89/06/05  10:15:22  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * 
 * Revision 3.12  89/02/04  15:22:06  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 3.11  88/12/22  17:02:06  amy
 * PTR c1140	gactivatews:  if a MO ws is open, call XgksMoActivateWs to 
 * 		write out the GKS state list to the metafile.
 * 
 * Revision 3.10  88/12/06  16:18:53  amy
 * PTR c1130	GKSERROR calls: changed function name parameter from character
 * 		string to enum. type value.
 * 
 * Revision 3.9  88/09/20  15:24:52  amy
 * Edited log describing MIT changes.
 * 
 * Revision 3.8  88/09/20  14:45:55  amy
 * MIT	Declared internal functions XgksAllocActiveWs, XgksDeleteActiveWs, 
 *	and XgksNoActiveWs.
 * 	Defined XgksDeleteActiveWs static void.
 * 	XgksDeleteActiveWs:  does a simple return (does not return a value).
 * 
 * Revision 3.7  88/08/16  12:49:35  amy
 * No differences between this version and August tape-- no changes made
 * 
 * Revision 3.6  88/08/11  08:21:04  amy
 * gactivatews & gdeactivatews:  added
 * call to VALID_WSID to check for error 20.
 * 
 * Revision 1.5  88/07/28  10:52:39  owens
 * added check for error 20 for PTR c1012
 * 
 * Revision 1.4  88/07/27  10:36:56  owens
 * a simple typo
 * 
 * Revision 1.2  88/07/26  13:55:51  owens
 * found an error in deactivatews, left note for future
 * 
 * Revision 1.1  88/07/21  10:48:18  david
 * Initial revision
 *  
 *
 */

static char *rcsid = "$Header: act_ws.c,v 4.0 89/08/31 16:19:37 amy Exp $";
                     
#include "gks_implem.h" /* d1 */

/*$F
 * gactivatews(ws_id) - ACTIVATE WORKSTATION 
 *
 * Gint ws_id;		workstation to activate, must already be open.
 *
 *	output primitives are sent to and segments are stored on active wss.
 *
 * returns: 0 = OK, or one of 6, 20, 25, 29, 33, 35, 43
 *
 * See also: ANSI standard p.76
 */

gactivatews(ws_id)
	Gint ws_id;
{
	/*
	 * declare internal functions
	 */
	static XgksAllocActiveWs();
	
	WS_STATE_PTR ws;

/* check for valid workstation open and active (must be at least one) */
	GKSERROR ( ((xgks_state.gks_state != GWSOP) && (xgks_state.gks_state != GWSAC)), 6, errgactivatews) /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgactivatews)   /* c1012 */

/* check for workstation opened */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR (((ws = OPEN_WSID(ws_id))==NULL), 25, errgactivatews)  /* c1012 */

/* check for workstation active */
	GKSERROR ((ws->wsstate == GACTIVE), 29, errgactivatews)

/* check for ws category */
	GKSERROR ((WS_CAT(ws) == GMI), 33, errgactivatews);

	GKSERROR ((WS_CAT(ws) == GINPUT), 35, errgactivatews);

	ws->wsstate = GACTIVE;
/* check if max number of allowable active ws would be exceeded */
	GKSERROR ((XgksAllocActiveWs (ws_id,ws)!=0), 43, errgactivatews)

	if (xgks_state.gks_state == GWSOP)			/*c1147*/	
		xgks_state.gks_state = GWSAC;	/* change operating state *//*c1147*/

	if (MO_OPENED == TRUE)					/* c1140 */
		XgksMoActivateWs( ws );				/* c1140 */

        return (OK);
}
/*$F
 * gdeactivatews(ws_id) - DEACTIVATE WORKSTATION
 *
 * Gint ws_id;		workstation to deactivate, must already be active.
 *
 *	While a workstation is inactive, primitives are not sent to it nor
 *	does it store new segments.  Segments already stored on this ws are
 *	retained.
 *
 * returns: 0 = OK, or one of 3, 20, 30, 33, 35
 *
 * See also: ANSI standard p.76
 */

gdeactivatews(ws_id)
	Gint ws_id;
{
	/*
	 * declare internal functions
	 */
	static void XgksDeleteActiveWs();
	static XgksNoActiveWs();
	
	WS_STATE_PTR ws;

/* first check for proper state */
	GKSERROR ((xgks_state.gks_state != GWSAC), 3, errgdeactivatews) /*c1147*/

/* check for ws invalid */
/* DWO 7/26/88  added check for ws_id validity in the name of PTR c1012 */
        GKSERROR ((!VALID_WSID(ws_id)), 20, errgdeactivatews);  /* c1012 */

/* DWO 7/26/88  used to return 20, now returns 30 in the name of PTR c1012 */
/*              (if it isn't open, it can't be active)                     */
/* check for  ws active (open) */
        GKSERROR(((ws=OPEN_WSID(ws_id))==NULL), 30, errgdeactivatews);  /* c1012 */

/* check for workstation active */
	GKSERROR ((ws->wsstate == GINACTIVE), 30, errgdeactivatews)

/* check for ws category */
	GKSERROR ((WS_CAT(ws) == GMI), 33, errgdeactivatews);

	GKSERROR ((WS_CAT(ws) == GINPUT), 35, errgdeactivatews);

	ws->wsstate = GINACTIVE;
	XgksDeleteActiveWs (ws_id);
	if (XgksNoActiveWs() == TRUE)	
		xgks_state.gks_state = GWSOP;	/* change operating state *//*c1147*/
        return (OK);
}

/*
 * XgksNoActiveWs() - return 1 if no open workstations are active
 *	else return 0;
 */
static XgksNoActiveWs()
{
	Gint i;
	for (i=0; i<MAX_ACTIVE_WS; i++) 
		if (xgks_state.activews[i].ws_id != INVALID) return (FALSE);/*c1147*/
	return (TRUE);
}
/* 
 * XgksAllocActiveWs (ws_id)
 *  Gint  ws_id;
 *
 * tries to allocate a slot in activews[] in gks state list and assign ws_id into the slot
 *
 * return INVALID if there's no empty slot
 *
 */
static XgksAllocActiveWs (ws_id,ws)
Gint ws_id;
WS_STATE_PTR ws;
{
   Gint i;

   for (i=0; i<MAX_ACTIVE_WS; i++) 
     	if (xgks_state.activews[i].ws_id == INVALID) {		/*c1147*/
		xgks_state.activews[i].ws_id = ws_id;		/*c1147*/
		xgks_state.activews[i].ws = ws;			/*c1147*/
		return(0);
	}
   return (INVALID);
}
/*
 * XgksDeleteActiveWs (ws_id)
 *   Gint ws_id;
 *
 * delete <ws_id> from activews[] in gks state list by setting corresponding entry to INVALID
 *
 */
static void XgksDeleteActiveWs (ws_id)
Gint  ws_id;
{
	Gint i;

	for (i=0; i<MAX_ACTIVE_WS; i++) {
		if (xgks_state.activews[i].ws_id == ws_id) {		/*c1147*/
			xgks_state.activews[i].ws_id = INVALID;		/*c1147*/
			xgks_state.activews[i].win = INVALID;		/*c1147*/
			xgks_state.activews[i].ws = NULL;		/*c1147*/
			return;
		}
	}
}
