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
 * $Header: inqpolylines.c,v 4.0 89/08/31 16:21:49 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/inqpolylines.c,v $
 *
 * $Log:	inqpolylines.c,v $
 * Revision 4.0  89/08/31  16:21:49  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.17  89/06/05  10:17:01  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * 
 * Revision 3.16  89/03/15  15:23:31  amy
 * PTR c1156	ginqlinefacil:  changed default min. char. height to 
 * 		adhere to standard.
 * 
 * Revision 3.15  89/02/04  15:24:53  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 3.14  88/12/28  14:46:39  amy
 * PTR c1143	ginqlinefacil:  assignment of number of predefined bundles
 * 		uses a #define, instead of hard-coded value.
 * 		ginqpredlinerep:  accepts for index only a value in the range of
 * 		distinct predefined bundles.  Uses the new predefined array to get the representation.
 * 
 * Revision 3.13  88/12/08  10:40:09  amy
 * PTR c1130	GKSERROR calls:  changed function name parameter from character
 * 		string to enum. type value.
 * Changed spelling of color to colour.
 * 
 * Revision 3.12  88/11/01  13:28:31  amy
 * PTR c1087	ginqlinefacil and ginqpredlinerep:  Added check for error 22,
 * 		ws type invalid.
 * 
 * Revision 3.11  88/09/21  11:21:10  amy
 * MIT	ginqlinefacil:  removed definition of int i, which was not used.
 * 
 * Revision 3.10  88/08/17  09:50:27  amy
 * AUG  ginqlinefacil:  changed reference to a field of fac (a Glnfac)
 *        from "width" to "widths."
 * 
 * Revision 3.9  88/08/11  08:29:14  amy
 * PTR c1012  ginqlineindices & ginqlinerep:  Added call to VALID_WSID
 * to check for error 20.
 * 
 * Revision 1.4  88/07/28  14:20:48  owens
 * added check for error 20 for PTR c1012
 * 
 * Revision 1.3  88/07/28  14:18:07  owens
 * added check for error 20 for PTR c1012
 * 
 * Revision 1.2  88/07/26  17:52:23  owens
 * renamed/replaced VALID_WSID macro for PTR c1012 (DWO)
 * 
 * Revision 1.1  88/07/21  10:49:08  david
 * Initial revision
 *  
 *
 */
 
static char *rcsid = "$Header: inqpolylines.c,v 4.0 89/08/31 16:21:49 amy Exp $";

/*
 * ginqlinefacil(ws_type, fac)
 *	INQUIRE POLYLINE FACILITIES
 *
 * Gchar *ws_type;		type of workstation the inquiry is about.
 * Glnfac fac;                  returned polyline facility values.
 *
 * returns all information in the parameters.
 * errors 0, 8, 22, 23, 39 can occur.
 *
 * See also: ANSI standard p.173
 */

#include <string.h>
#include "gks_implem.h"                 /* d1 */

ginqlinefacil(ws_type, fac)
	Gchar *ws_type;
	Glnfac *fac;
{
	EWSTYPE ewstype;

/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqlinefacil); /*c1147*/

/* check for valid ws_type */
	ewstype = XgksWsTypeToEnum( ws_type );
	GKSERROR( (ewstype == WST_INVALID), 22, errginqlinefacil);    /* c1087 */
	GKSERROR( ewstype != X_WIN, 39, errginqlinefacil); 	

/* set the return values */
	fac->widths = 0;				/* AUG */
	fac->nom = 1.0;
	fac->min = 0.01;				/* c1156 */
	fac->max = 1024.0;
	fac->predefined = PDF_LINE_BNDLS;			/* c1143 */
	fac->types.number = 7;
/* get space for list */
	fac->types.integers = (Gint *)malloc((unsigned) fac->types.number * sizeof(int));
	GKSERROR( (fac->types.integers == NULL), 300, errginqlinefacil);

/* set returned index values */
	fac->types.integers[0] = GLN_LDASH;
	fac->types.integers[1] = GLN_DDOTDASH;
	fac->types.integers[2] = GLN_SDASH;
	fac->types.integers[3] = GLN_SOLID;
	fac->types.integers[4] = GLN_DASH;
	fac->types.integers[5] = GLN_DOT;
	fac->types.integers[6] = GLN_DOTDASH;

	return( OK );
}

/*$F
 * ginqpredlinerep(ws_type, idx, rep) -
 *	INQUIRE PREDEFINED POLYLINE REPRESENTATION
 *
 * Gchar *ws_type;		workstation type this inquiry is about.
 * Gint idx;                    polyline index.
 * Glnbundl *rep;               predefined polyline bundle values.
 *
 * returns all information in the parameters.
 * errors 0, 8, 22, 23, 39, 60 can occur.
 *
 * See also: ANSI standard p.171
 */

/* c1147:  Predefined bundles in polylines.h */
#include "polylines.h"					/*c1147*/

ginqpredlinerep(ws_type, idx, rep)
	Gchar *ws_type;
	Gint idx;
	Glnbundl *rep;
{
	EWSTYPE ewstype;

/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqpredlinerep); /*c1147*/

/* check for valid ws_type */
	ewstype = XgksWsTypeToEnum( ws_type );
	GKSERROR( (ewstype == WST_INVALID), 22, errginqpredlinerep);   /* c1087 */
	GKSERROR( ewstype != X_WIN, 39, errginqpredlinerep); 	

/* check for valid idx */
	GKSERROR((idx < 1 || idx > PDF_LINE_BNDLS), 60, errginqpredlinerep);/*c1143*/

/* set the return values */
	rep->type =  def_lnbundl[idx-1].type;				/* c1143 */
	rep->width = def_lnbundl[idx-1].width;				/* c1143 */
	rep->colour = def_lnbundl[idx-1].colour;			/* c1143 */

	return( OK );
}

/*$F
 * ginqlineindices(ws_id, idxlist) - INQUIRE LIST OF POLYLINE INDICES
 *
 * Gint ws_id;			workstation inquiry is about.
 * Gintlist *idxlist;           list of defined polyline indices
 *
 * returns all information in the parameters.
 * errors 0, 7, 20, 25, 33, 35, 36 can occur.
 *
 * See also: ANSI standard p.154
 */

ginqlineindices(ws_id, idxlist)
	Gint ws_id;
	Gintlist *idxlist;
{
	WS_STATE_PTR ws;
	int  i;

/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqlineindices); /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqlineindices)   /* c1012 */

/* check if this workstation is opened */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR( ((ws = OPEN_WSID(ws_id)) == NULL), 25, errginqlineindices);  /* c1012 */

/* check workstation type */
	GKSERROR( (ws->ewstype == MI), 33, errginqlineindices);
	GKSERROR( (ws->ewstype == WISS), 36, errginqlineindices);

/* get space for list */
	idxlist->number = 20;
	idxlist->integers = (Gint *)malloc((unsigned) idxlist->number * sizeof(int));

/* set the indecies values */
	for (i=0; i<idxlist->number; i++) 
		idxlist->integers[i] = i+1;

	return( OK );
}

/*$F
 * ginqlinerep(ws_id, idx, type, rep) - INQUIRE LIST OF POLYLINE INDICES
 *
 * Gint ws_id;			workstation inquiry is about.
 * Gint idx;
 * Gqtype type;
 * Glnbundl *rep;
 *
 * returns all information in the parameters.
 * errors 0, 7, 20, 25, 33, 35, 36, 60 can occur.
 *
 * See also: ANSI standard p.152
 */

ginqlinerep(ws_id, idx, type, rep)
	Gint ws_id;
	Gint idx;
	Gqtype type;
	Glnbundl *rep;
{
	WS_STATE_PTR ws;

/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqlinerep); /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqlinerep)   /* c1012 */

/* check if this workstation is opened */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR( ((ws = OPEN_WSID(ws_id)) == NULL), 25, errginqlinerep);  /* c1012 */

/* check workstation type */
	GKSERROR( (ws->ewstype == MI), 33, errginqlinerep);
	GKSERROR( (ws->ewstype == WISS), 36, errginqlinerep);

/* check for valid idx */
	GKSERROR( (idx < 1 || idx > 20), 60, errginqlinerep);

/* set the returned values */
	rep->type = ws->lnbundl_table[idx].type;
	rep->width = ws->lnbundl_table[idx].width;
	rep->colour = ws->lnbundl_table[idx].colour;

	return( OK );
}
