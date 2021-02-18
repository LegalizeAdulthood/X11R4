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
 *	All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 * 
 * $Header: inqtransform.c,v 4.0 89/08/31 16:22:03 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/inqtransform.c,v $
 *
 * $Log:	inqtransform.c,v $
 * Revision 4.0  89/08/31  16:22:03  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.11  89/06/05  10:17:09  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * 
 * Revision 3.10  89/03/07  17:48:49  amy
 * PTR c2009	ginqclip:  add check for ind == GNOCLIP.  Used to 
 * 		return 0.0,0.0 by 1.0,1.0 if clipping was off.  ANSI Verification 
 * 		Suite expects this function to return the current norm. 
 * 		trans. clipping rectangle regardless of setting of 
 * 		clipping indicator.
 * 
 * Revision 3.9  89/02/04  15:25:33  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 3.8  88/12/08  16:43:45  amy
 * PTR c1130	ginqclip:  changed function name value in GKSERROR call.
 * 
 * Revision 3.7  88/12/08  10:48:34  amy
 * PTR c1130	GKSERROR calls:  changed function name parameter from character
 * 		string to enum. type value.
 * 
 * Revision 3.6  88/08/17  10:05:37  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 3.5  88/08/11  08:31:33  amy
 * PTR c1012  ginqwstran:  added call to VALID_WSID to check for error 20.
 * 
 * Revision 1.3  88/07/28  14:27:02  owens
 * added check for error 20 for PTR c1012
 * 
 * Revision 1.2  88/07/26  17:52:35  owens
 * renamed/replaced VALID_WSID macro for PTR c1012 (DWO)
 * 
 * Revision 1.1  88/07/21  10:49:16  david
 * Initial revision
 *  
 *
 */
 
static char *rcsid = "$Header: inqtransform.c,v 4.0 89/08/31 16:22:03 amy Exp $";

#include "gks_implem.h"                 /* d1 */

/*$F
 * ginqcurntrannum(tran) - INQUIRE CURRENT NORMALIZATION TRANSFORMATION NUMBER
 *
 * Gint	*tran;			the currently selected transformation.
 *
 * returns all information in the parameters.
 * errors 0,
 *
 * See also: ANSI Standard p.147
 */
ginqcurntrannum(tran)
	Gint *tran;
{
/* check proper gks operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqcurntrannum); /*c1147*/

/* set up return values */
	*tran = xgks_state.cur_ntrans;		/*c1147*/
	return( OK );
}

/*$F
 * ginqntrannum(tranlist) - INQUIRE LIST OF NORMALIZATION TRANSFORMATION NUMBERS
 * 
 * Gintlist *tranlist;       OUT list of normalization transformation numbers 
 *
 *	On input, ntrans contains the maximum size for the returned list.
 *
 * returns all information in the parameters
 * errors 0, 8
 *
 * See also: ANSI Standard p.147
 */

ginqntrannum(tranlist)
	Gintlist *tranlist;
{
	int i;

/* check proper gks operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqntrannum); /*c1147*/

/* allocate the memory for the list */
	tranlist->integers = (Gint *) malloc( (MAX_TRANS+1) * sizeof(int));

/* set up return values */
	for(i=0; i <= MAX_TRANS; i++)
		tranlist->integers[i] = xgks_state.ntrans_priority[i]; /*c1147*/
	tranlist->number = MAX_TRANS +1;

	return( OK );
}

/*$F
 * ginqntran(num, tran) - INQUIRE NORMALIZATION TRANSFORMATION
 *
 * Gint num;                    normalization transformation number
 * Gwstran *tran,		transformation structure
 *
 * returns all information in the parameters
 * errors 0, 8, 50
 *
 * See also: ANSI Standard p.148
 */

ginqntran(num, tran)
Gint num;
Gwstran *tran;
{
/* check proper gks operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqntran); /*c1147*/

/* check for valid trans number */
	GKSERROR( (num < 0 || num > MAX_TRANS), 50, errginqntran);

/* set up the return values */
	*tran = xgks_state.ntrans_list[num].ntrans;	/*c1147*/

	return( OK );
}

/*$F
 * ginqclip(clipping) - INQUIRE CLIPPING INDICATOR
 *
 * Gcliprec *clipping,        OUT current clipping indicator and rectangle
 *
 * all information is returned in the parameters
 * errors 0, 8
 *
 * See also: ANSI Standard p.148
 */

ginqclip(clipping)
Gcliprec *clipping;
{

/* check proper gks operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqclip); /*c1147*/

/* set up return values */
	if (xgks_state.cliprec.ind == GCLIP)                /* c2009 */
	   *clipping = xgks_state.cliprec;   /* c1147 */    /* c2009 */
        else /* ...ind == GNOCLIP */                        /* c2009 */
           {                                                /* c2009 */
           clipping->ind = GNOCLIP;                         /* c2009 */
           clipping->rec.xmin =
		 xgks_state.ntrans_list[xgks_state.cur_ntrans].ntrans.v.xmin; /* c2009 */
           clipping->rec.xmax =
		 xgks_state.ntrans_list[xgks_state.cur_ntrans].ntrans.v.xmax; /* c2009 */
           clipping->rec.ymin =
		 xgks_state.ntrans_list[xgks_state.cur_ntrans].ntrans.v.ymin; /* c2009 */
           clipping->rec.ymax =
		 xgks_state.ntrans_list[xgks_state.cur_ntrans].ntrans.v.ymax; /* c2009 */
           }                                                /* c2009 */

	return( OK );
}

/*$F
 * ginqmaxntrannum(maxtran) - INQUIRE MAXIMUM NORMALIZATION TRANSFORMATION NUMBER
 *
 * Gint	*maxtran;			maximum transformation number available
 *
 * returns all information in the parameters
 * errors 0, 8
 *
 * See also: ANSI Standard p.144
 */

ginqmaxntrannum(maxtran)
Gint *maxtran;
{
/* check proper gks operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqmaxntrannum); /*c1147*/

/* set up return values */
	*maxtran = MAX_TRANS;

	return( OK );
}

/*$F
 * ginqwstran(ws_id, wstran)
 *		INQUIRE WORKSTATION TRANSFORMATION
 *
 * Gint ws_id;			workstation identifier
 * Gwsti *wstran;               OUT requested and current transformations
 *
 * NOTE: Each of the four pointers above must point to an array of size 2 of
 *	the appropriate type.  The first element will get the lower left corner
 *	and the second element will get the upper right corner.
 *
 * returns all information in the parameters
 * errors 0, 7, 20, 25, 33, 36
 *
 * See also: ANSI Standard p.162
 */

ginqwstran(ws_id, wstran)
Gint ws_id;
Gwsti *wstran;
{
	WS_STATE_PTR ws;

/* check proper gks operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqwstran); /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqwstran)   /* c1012 */

/* check for open ws_id */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR( ((ws=OPEN_WSID(ws_id)) == NULL), 25, errginqwstran);  /* c1012 */

/* proper category */
	GKSERROR( (ws->ewstype == MI), 33, errginqwstran);
	GKSERROR( (ws->ewstype == WISS), 36, errginqwstran);

/* set up the return values */
	*wstran = ws->wsti;	

	return( OK );
}
