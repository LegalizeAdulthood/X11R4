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
 * deferral_ws.c - function to set the ramtek workstation deferral mode.
 *		gsetdeferst()
 * 
 * $Header: deferral_ws.c,v 4.0 89/08/31 16:20:21 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/deferral_ws.c,v $
 *
 * $Log:	deferral_ws.c,v $
 * Revision 4.0  89/08/31  16:20:21  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.10  89/06/05  10:16:04  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * 
 * Revision 3.9  89/02/04  15:23:15  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 3.8  88/12/29  13:03:18  amy
 * PTR c1139	gsetdeferst:  do not redraw segments on an MO workstation.
 * 
 * Revision 3.7  88/12/06  17:00:44  amy
 * PTR c1130	GKSERROR calls:  changed function name parameter from character
 * 		string to enum. type value.
 * 
 * Revision 3.6  88/08/16  14:05:21  amy
 * No differences between this version and August tape-- no changes made.
 * 
 * Revision 3.5  88/08/11  08:23:41  amy
 * gsetdeferst:  Added call to VALID_WSID to check for error 20.  PTR c1012.
 * 
 * 
 * Revision 1.4  88/07/28  15:19:43  owens
 * added check for error 20 for PTR c1012
 * 
 * Revision 1.3  88/07/28  11:07:34  owens
 * added check for error 20 for PTR c1012
 * 
 * Revision 1.2  88/07/26  17:51:46  owens
 * renamed/replaced VALID_WSID macro for PTR c1012 (DWO)
 * 
 * Revision 1.1  88/07/21  10:48:37  david
 * Initial revision
 *  
 *
 */
 
static char rcsid[]="$Header: deferral_ws.c,v 4.0 89/08/31 16:20:21 amy Exp $";
                     
#include "gks_implem.h" /* d1 */

/*$F
 * gsetdeferst(ws_id, deferral_mode, regen_mode) - SET DEFERRAL STATE 
 *
 * Gint ws_id;		workstation identifier
 * Gdefmode deferral_mode;	how outputs can be deferred ASAP | BNIG | BNIL | ASTI
 * Girgmode regen_mode;	implicit regeneration mode SUPPRESSED | ALLOWED
 *
 * returns: 0 = OK, or one of 7, 20, 25, 33, 35, 36, 2000
 *
 * See also: ANSI standard p.79
 */
gsetdeferst(ws_id, deferral_mode, regen_mode)
	Gint ws_id;
	Gdefmode deferral_mode;
	Girgmode regen_mode;
{
	WS_STATE_PTR ws;

/* check for proper operating state */
	GKSERROR ((xgks_state.gks_state==GGKOP || xgks_state.gks_state==GGKCL) , 7, errgsetdeferst) /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgsetdeferst)   /* c1012 */

/* check for open ws_id */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR (((ws=OPEN_WSID(ws_id))==NULL) , 25, errgsetdeferst)  /* c1012 */

/* check for valid category */
	GKSERROR ((WS_CAT(ws) == GMI) , 33, errgsetdeferst)
	GKSERROR ((WS_CAT(ws) == GINPUT) , 35, errgsetdeferst)
	GKSERROR ((WS_CAT(ws) == GWISS) , 36, errgsetdeferst)

/* check for valid deferral_mode & regen_mode */
        GKSERROR ( (deferral_mode != GASAP && deferral_mode != GBNIG &&
		deferral_mode != GBNIL && deferral_mode != GASTI),
		2000, errgsetdeferst)

	if (ws->ewstype == MO)  XgksMoDeferWs (ws, deferral_mode, regen_mode);
	
/* see if redraw is necessary */

	ws->wsdus.defmode = deferral_mode;
	ws->wsdus.irgmode = regen_mode;

	if (ws->wsdus.irgmode==GALLOWED && ws->wsdus.nframe==GYES && ws->ewstype!=MO)  /* c1139 */
		XgksReDrawSegWs(ws);
	
	return(OK);
}
