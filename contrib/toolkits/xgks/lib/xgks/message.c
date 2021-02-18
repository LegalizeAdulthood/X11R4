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
 * message.c - workstation message function
 *              gmessage()
 *
 * 
 * $Header: message.c,v 4.0 89/08/31 16:22:34 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/message.c,v $
 *
 * $Log:	message.c,v $
 * Revision 4.0  89/08/31  16:22:34  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.9  89/05/30  09:34:04  bruce
 * PTR# c1176:	Replace calls to string functions with macros that check for NULL
 * PTR# c1179:	Shorten include file name for AIX compiler.
 * 
 * Revision 3.8  89/02/04  15:26:08  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 3.7  88/12/08  11:08:46  amy
 *  PTR c1130	GKSERROR calls:  changed function name parameter from character
 * 		string to enum. type value.
 * 
 * Revision 3.6  88/09/21  13:25:03  amy
 * MIT	Include strings.h
 * 
 * Revision 3.5  88/08/17  13:09:47  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 3.4  88/08/11  08:33:05  amy
 * PTR c1012  gmessage:  added call to VALID_WSID to check for error 20.
 * 
 * Revision 1.3  88/07/28  14:30:27  owens
 * added check for error 20 for PTR c1012
 * 
 * Revision 1.2  88/07/26  17:52:51  owens
 * renamed/replaced VALID_WSID macro for PTR c1012 (DWO)
 * 
 * Revision 1.1  88/07/21  10:49:29  david
 * Initial revision
 *  
 *
 */

static char rcsid[]="$Header: message.c,v 4.0 89/08/31 16:22:34 amy Exp $";
                     
#include "gks_implem.h"                 /* d1 */
#include <strings.h>		/*MIT*/

static WS_ENTRY	 tmp_act[MAX_ACTIVE_WS];	/* Make sure message will only be output to 
						   the specified workstaiton, this is done by
						   setting saving current activews[] and unset
						   rest of the ws_id beside the one we wnat */
/*$F 
 * gmessage(ws_id, string) - MESSAGE - display a message on the workstation in an
 *      implementation dependent way.  In this implementation it will display on
 *      the stdout device.
 *
 * Gint ws_id;          the workstation id, not used since output to stdout
 * Gchar *string;                the message to print
 *
 * returns: 0 = OK, or one of 7, 20, 25, 33, 36
 *
 * See also: ANSI standard p.80
 */
gmessage(ws_id, string)
Gint ws_id;
Gchar *string;
{
	WS_STATE_PTR ws;
	OUT_PRIMI *mesg;
	Gos tmp_os;
	Gint i;

/* check for proper operating state */
        GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP) , 7, errgmessage) /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgmessage)   /* c1012 */

/* DWO 7/26/88  changed macro name from VALID_WSID */
        GKSERROR (((ws=OPEN_WSID(ws_id))==NULL) , 25, errgmessage)  /* c1012 */

/* check for valid workstation category */
	GKSERROR ((WS_CAT(ws) == GMI) , 33, errgmessage)

	GKSERROR ((WS_CAT(ws) == GWISS) , 36, errgmessage)

	if (ws->ewstype == MO) XgksMoMessage (ws, string);

/* open an primitive structure */
        GKSERROR ( ((mesg = XgksNewPrimi()) == NULL), 300, errgmessage);

        mesg->pid = MESG;

/* get momory for mesg string */
        GKSERROR ( ( (mesg->primi.mesg.string= (Gchar *)malloc ((unsigned) STRLEN(string) + 1)) == NULL ), 300, errgmessage);

        STRCPY ((mesg->primi.mesg.string), string);

	/* Have to avaoid message being saved as part of segments 
	   so save current gks_state, and if it's GSGOP, then set it to GWSAC
	   and restore gks_state, after output is done 
	   
	   Also we have to changed the xgks_state.activews[] array to only one
	   worstation active */

	tmp_os = xgks_state.gks_state;					 /*c1147*/
	for (i=0; i<MAX_ACTIVE_WS; i++) tmp_act[i] = xgks_state.activews[i]; /*c1147*/

	if (xgks_state.gks_state == GSGOP) xgks_state.gks_state = GWSAC; /*c1147*/

	for (i=0; i<MAX_ACTIVE_WS; i++) {
		if (xgks_state.openedws[i].ws_id == ws_id)		/*c1147*/ 
		   xgks_state.activews[0] = xgks_state.openedws[i];	/*c1147*/
		if (i != 0) 
		   xgks_state.activews[i].ws_id = INVALID;		/*c1147*/
	}

        XgksProcessPrimi (mesg);

	for (i=0; i<MAX_ACTIVE_WS; i++) xgks_state.activews[i] = tmp_act[i]; /*c1147*/
	xgks_state.gks_state = tmp_os;				/*c1147*/

        free (mesg->primi.mesg.string);
        free (mesg);

	return(OK);
}
