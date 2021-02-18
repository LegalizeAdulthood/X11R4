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
 * $Header: update.c,v 4.0 89/08/31 16:25:41 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/update.c,v $
 *
 * $Log:	update.c,v $
 * Revision 4.0  89/08/31  16:25:41  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.10  89/06/05  10:18:12  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * 
 * Revision 3.9  89/02/04  15:31:01  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 3.8  88/12/29  13:01:31  amy
 * PTR c1139	gupdatews:  do not redraw segments on an MO workstation.
 * 
 * Revision 3.7  88/12/08  13:35:38  amy
 * PTR c1130	GKSERROR calls:  changed function name parameter from character
 *  		string to enum. type value.
 * 
 * Revision 3.6  88/08/18  11:09:20  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 3.5  88/08/11  09:16:39  amy
 * PTR c1012  gupdatews:  added call to VALID_WSID to check for error 20.
 * 
 * Revision 1.3  88/07/28  15:11:33  owens
 * added check for error 20 for PTR c1012
 * 
 * Revision 1.2  88/07/26  17:54:33  owens
 * renamed/replaced VALID_WSID macro for PTR c1012 (DWO)
 * 
 * Revision 1.1  88/07/21  10:50:42  david
 * Initial revision
 *  
 *
 */

static char *rcsid = "$Header: update.c,v 4.0 89/08/31 16:25:41 amy Exp $";

/*$F
 * gupdatews(ws_id, regenflag) - UPDATE WORKSTATION
 *
 * Gint  ws_id;			User selected workstation identifier.
 * Gregen regenflag;		regneration flag GPERFORM | GPOSTPONE
 *
 * Returns: 0, 20, 25, 33, 35, 36
 *
 * See also: ANSI standard p.78
 */

#include "gks_implem.h"                 /* d1 */


gupdatews(ws_id, regenflag)
	Gint	ws_id;
	Gregen	regenflag;
{
	WS_STATE_PTR ws;	/* workstation state list ptr */

/* check proper state */
	GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP) ,7, errgupdatews) /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgupdatews)   /* c1012 */

/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR (((ws=OPEN_WSID(ws_id)) == NULL), 25, errgupdatews)  /* c1012 */
	
/* check for valid ws category */
	GKSERROR ( (WS_CAT(ws)==GMI), 33, errgupdatews);

        GKSERROR ( (WS_CAT(ws)==GINPUT), 35, errgupdatews);
        
        GKSERROR ( (WS_CAT(ws)==GWISS), 36, errgupdatews);

/* check for valid enumeration type */
	GKSERROR ( (regenflag != GPERFORM && regenflag != GPOSTPONE), 2000, errgupdatews);

	if (ws->ewstype == MO) XgksMoUpdateWs (ws, regenflag);
	
	if (regenflag == GPOSTPONE || ws->wsdus.nframe == GNO) return (OK);

        if (ws->ewstype != MO)					/* c1139 */
	        XgksReDrawSegWs (ws);				/* c1139 */

/* if necessary call user defined redraw notifying function */
	if (ws->redrawfuncp != NULL)
		(*(ws->redrawfuncp))( ws_id, GRD_GKS );

	return(OK);
}
