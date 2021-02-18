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
 * inquiries.c - this file contains inquiry functions for the GKS Description table
 * and GKS state list.
 *		ginqopst
 *		ginqlevelgks
 *		ginqwsmaxnum
 *		ginqopenws
 *		ginqactivews
 *		ginqprimattr
 *		ginqindivattr
 * 
 * $Header: inquiries.c,v 4.0 89/08/31 16:22:09 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/inquiries.c,v $
 *
 * $Log:	inquiries.c,v $
 * Revision 4.0  89/08/31  16:22:09  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.8  89/06/05  10:07:38  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * PTR# c1179:	Changed exp field to tx_exp or ch_exp for AIX
 * 		compiler.  Moved variable declarations.  
 * 
 * Revision 3.7  89/02/04  15:25:39  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 3.6  88/12/09  17:16:50  amy
 * Changed ginqindividualattr to ginqindivattr.
 * Updated prologue with correct function names.
 * 
 * Revision 3.5  88/12/08  10:55:47  amy
 * PTR c1130	GKSERROR calls:  changed function name parameter from character
 * 		string to enum. type value.
 * Changed spelling of color to colour.
 * 
 * Revision 3.4  88/08/17  10:11:38  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 1.2  88/07/26  17:52:38  owens
 * renamed/replaced VALID_WSID macro for PTR c1012 (DWO)
 * 
 * Revision 1.1  88/07/21  10:49:19  david
 * Initial revision
 *  
 *
 */
 
static char rcsid[]="$Header: inquiries.c,v 4.0 89/08/31 16:22:09 amy Exp $";

#include "gks_implem.h"                 /* d1 */


/*$F
 * INQUIRE OPERATING STATE VALUE
 */

ginqopst(state)
Gos *state;
{
	*state = xgks_state.gks_state;		/*c1147*/
	return( OK );
}

/* 
 * Inquiry Functions for GKS Description Table
 *--------------------------------------------
 */

/*$F
 * INQUIRE LEVEL OF GKS
 */

ginqlevelgks(lev)
Glevel *lev;
{
/* check for proper gks state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqlevelgks); /*c1147*/

/* set up the return values */
	*lev = xgks_state.level;		/*c1147*/
	return( OK );
}	       

/*$F
 * INQUIRE WORKSTATION MAXIMUM NUMBERS
 *
 * errors 0, 8
 */

ginqwsmaxnum(maxws)
Gwsmax *maxws;
{
/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqwsmaxnum); /*c1147*/

/* set up the return values */
	*maxws = xgks_state.wsmax;		/*c1147*/
	return( OK );
}

/*
 * Inquiry Functions for GKS State List
 *--------------------------------------
 */

/*$F
 * INQUIRE SET OF OPEN WORKSTATIONS
 *
 * errors 0, 8
 */

ginqopenws(wsids)
Gintlist *wsids;
{
	int i, j;

/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqopenws); /*c1147*/

/* set up the return values */

	if (xgks_state.gks_state == GGKOP) {				/*c1147*/
		wsids->number = 0;
		wsids->integers = 0;
	}
	else {
		j = 0;
		wsids->integers = (Gint *) malloc( (unsigned)MAX_OPEN_WS * sizeof(int));
		GKSERROR( (wsids->integers == NULL), 300, errginqopenws);
		for (i=0; i<MAX_OPEN_WS; i++) {
			if (xgks_state.openedws[i].ws_id != INVALID) {	/*c1147*/
			   wsids->integers[j] = xgks_state.openedws[i].ws_id; /*c1147*/
			   j++;
			}
		}
		wsids->number = j;
	}
	return( OK );
}

/*$F
 * INQUIRE SET OF ACTIVE WORKSTATIONS
 *
 * errors 0, 8
 */

ginqactivews(wsids)	
Gintlist *wsids;
{
	int i, j;

/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqactivews); /*c1147*/

/* set up the return values */

	if (xgks_state.gks_state == GGKOP) {				/*c1147*/
		wsids->number = 0;
		wsids->integers = 0;
	}
	else {
		j = 0;
		wsids->integers = (Gint *) malloc( (unsigned)MAX_ACTIVE_WS * sizeof(int));
		GKSERROR( (wsids->integers == NULL), 300, errginqactivews);	
		for (i=0; i<MAX_ACTIVE_WS; i++) {
			if (xgks_state.activews[i].ws_id != INVALID) {	/*c1147*/
			   wsids->integers[j] = xgks_state.activews[i].ws_id; /*c1147*/
			   j++;
			}
		}
		wsids->number = j;
	}
	return( OK );
}

/*$F
 * INQUIRE CURRENT PRIMITIVE ATTRIBUTE VALUES
 *
 * errors 0, 8
 */

ginqprimattr(primattr)
Gpriattr *primattr;
{
/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqprimattr); /*c1147*/

/* set up the return values */
	primattr->plnindex = xgks_state.gks_lnattr.line;	/*c1147*/
	primattr->pmkindex = xgks_state.gks_mkattr.mark;	/*c1147*/
	primattr->txindex = xgks_state.gks_txattr.text;		/*c1147*/
	primattr->height   = xgks_state.gks_chattr.height;	/*c1147*/
	primattr->up       = xgks_state.gks_chattr.up;		/*c1147*/
	primattr->chwidth  = xgks_state.gks_chattr.chwidth;	/*c1147*/
	primattr->base     = xgks_state.gks_chattr.base;	/*c1147*/
	primattr->path     = xgks_state.gks_chattr.path;	/*c1147*/
	primattr->align    = xgks_state.gks_chattr.align;	/*c1147*/
	primattr->flindex   = xgks_state.gks_flattr.fill;	/*c1147*/
	primattr->widthvec = xgks_state.gks_ptattr.widthvec;	/*c1147*/
	primattr->heightvec= xgks_state.gks_ptattr.heightvec;	/*c1147*/
	primattr->prp      = xgks_state.gks_ptattr.ptp;		/*c1147*/
	return( OK );
	 
}

/*$F
 * INQUIRE CURRENT INDIVIDUAL ATTRIBUTE VALUES
 *
 * errors 0, 8.
 */


ginqindivattr(indivattr)
Gindattr *indivattr;
{
/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqindivattr); /*c1147*/

/* set up the return values */
	indivattr->lntype  = xgks_state.gks_lnattr.bundl.type;		/*c1147*/
	indivattr->lnwidth = xgks_state.gks_lnattr.bundl.width;		/*c1147*/
	indivattr->lncolour = xgks_state.gks_lnattr.bundl.colour;	/*c1147*/
	indivattr->mktype  = xgks_state.gks_mkattr.bundl.type;		/*c1147*/
	indivattr->mksize  = xgks_state.gks_mkattr.bundl.size;		/*c1147*/
	indivattr->mkcolour = xgks_state.gks_mkattr.bundl.colour;	/*c1147*/
	indivattr->fp      = xgks_state.gks_txattr.bundl.fp;		/*c1147*/
	indivattr->chexp   = xgks_state.gks_txattr.bundl.ch_exp;	/* c1147 c1179 */
	indivattr->chspace = xgks_state.gks_txattr.bundl.space;		/*c1147*/
	indivattr->txcolour = xgks_state.gks_txattr.bundl.colour;	/*c1147*/
	indivattr->flstyle = xgks_state.gks_flattr.bundl.inter;		/*c1147*/
	indivattr->flindex = xgks_state.gks_flattr.bundl.style;		/*c1147*/
	indivattr->flcolour = xgks_state.gks_flattr.bundl.colour;	/*c1147*/
	indivattr->asflist.ln_type  = xgks_state.gks_lnattr.type;	/*c1147*/
	indivattr->asflist.ln_width = xgks_state.gks_lnattr.width;	/*c1147*/
	indivattr->asflist.ln_colour = xgks_state.gks_lnattr.colour;	/*c1147*/
	indivattr->asflist.mk_type  = xgks_state.gks_mkattr.type;	/*c1147*/
	indivattr->asflist.mk_size  = xgks_state.gks_mkattr.size;	/*c1147*/
	indivattr->asflist.mk_colour = xgks_state.gks_mkattr.colour;	/*c1147*/
	indivattr->asflist.tx_fp    = xgks_state.gks_txattr.fp;		/*c1147*/
	indivattr->asflist.tx_exp   = xgks_state.gks_txattr.tx_exp;	/* c1147 c1179 */
	indivattr->asflist.tx_space = xgks_state.gks_txattr.space;	/*c1147*/
	indivattr->asflist.tx_colour = xgks_state.gks_txattr.colour;	/*c1147*/
	indivattr->asflist.fl_inter = xgks_state.gks_flattr.inter;	/*c1147*/
	indivattr->asflist.fl_style = xgks_state.gks_flattr.style;	/*c1147*/
	indivattr->asflist.fl_colour = xgks_state.gks_flattr.colour;	/*c1147*/
	return( OK );
}
