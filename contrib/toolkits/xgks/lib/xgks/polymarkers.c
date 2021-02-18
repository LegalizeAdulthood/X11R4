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
 * polymarkers.c - functions and data for polymarkers
 *              gpolymarker();
 *              gsetmarkersize();
 *              gsetmarkertype();
 *              gsetmarkercolourind();
 *              gsetmarkerind();
 *              gsetmarkerrep();
 *              s_pmark_asf();
 *
 *              XgksInitGksPmarkers();
 *		XgksInitWssPmarkers();
 * 
 * $Header: polymarkers.c,v 4.0 89/08/31 16:23:52 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/polymarkers.c,v $
 *
 * $Log:	polymarkers.c,v $
 * Revision 4.0  89/08/31  16:23:52  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.12  89/06/05  10:17:50  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * 
 * Revision 3.11  89/02/13  16:47:26  amy
 * PTR c1154	XgksInitGksPmarkers:  initialize polymarker size to 1.0.
 * 
 * Revision 3.10  89/02/04  15:27:46  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 3.9  88/12/28  14:33:01  amy
 * PTR c1143	XgksInitGksPmarkers, XgksInitWssPmarkers:  Added new predefined
 * 		representations as per the ANSI standard and changed the 
 * 		initialization functions to initialize new workstations with
 * 		the additional predefined representations.
 * 
 * Revision 3.8  88/12/28  14:30:06  amy
 * PTR c1075	XgksInitWssPmarkers:  Created and initialized default attribute
 * 		bundle, which is now used to initialize the GKS state list when
 * 		GKS is opened, and the workstation state list when a new
 * 		workstation is opened.
 * 
 * Revision 3.7  88/12/08  12:39:57  amy
 * PTR c1130	GKSERROR calls:  changed function name parameter from character
 * 		string to enum. type value.
 * Changed spelling of color to colour.
 * 
 * Revision 3.6  88/08/18  06:56:12  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 3.5  88/08/11  08:58:43  amy
 * PTR c1012  gsetmarkerrep:  added calls to VALID_WSID to check for
 * error 20.
 * 
 * Revision 1.3  88/07/28  14:52:14  owens
 * added check for error 20 for PTR c1012
 * 
 * Revision 1.2  88/07/26  17:53:30  owens
 * renamed/replaced VALID_WSID macro for PTR c1012 (DWO)
 * 
 * Revision 1.1  88/07/21  10:49:58  david
 * Initial revision
 *  
 *
 */

static char *rcsid = "$Header: polymarkers.c,v 4.0 89/08/31 16:23:52 amy Exp $";

#include "gks_implem.h"                 /* d1 */

/* 
 * XgksInitGksPmarkers-- initialise gks state list polymarker stuff
 */

/* c1143:  Created predefined reps 1-5 */

/* c1147:  Moved them to polymarkers.h */
#include "polymarkers.h"						/*c1147*/

XgksInitGksPmarkers()
{
	xgks_state.gks_mkattr.mark   = 1;	   /* mark indx */ /* c1143 *//*c1147*/
	xgks_state.gks_mkattr.type   = GINDIVIDUAL;/* type ASF */  /* c1143 *//*c1147*/
	xgks_state.gks_mkattr.size   = GINDIVIDUAL;/* size ASF */  /* c1143 *//*c1147*/
	xgks_state.gks_mkattr.colour = GINDIVIDUAL;/* colour ASF *//* c1143 *//*c1147*/
	/* marker bundle */					   /* c1143 */
	xgks_state.gks_mkattr.bundl.type   = def_mkbundl[0].type;  /* c1143 *//*c1147*/
	xgks_state.gks_mkattr.bundl.size   = DEFMRKRSIZE; /*c1154*/  /*c1143*//*c1147*/
	xgks_state.gks_mkattr.bundl.colour = def_mkbundl[0].colour;/* c1143 *//*c1147*/
}
/*
 * XgksInitWssPmarkers(ws) - send the current INDIVIDUAL attributes and BUNDLE
 *	index to the newly opened workstation ws.
 */
XgksInitWssPmarkers(ws)
WS_STATE_PTR ws;
{
	Gint i;			/* Loop counter */

	/* c1143: mkbundl_table[0] never used but initialized anyway */
	ws->mkbundl_table[0] = def_mkbundl[0];				/* c1143 */

	/* c1143: initialize predefined representation bundles */
	for (i=1; i<=PDF_MARK_BNDLS; i++)				/* c1143 */
		/* c1075: use implementation defaults, not current attributes */
		ws->mkbundl_table[i] = def_mkbundl[i-1];		/* c1143 */

	/* c1143: initialize rest of representation bundle array */
	for (i=PDF_MARK_BNDLS+1; i<MAX_BUNDL_TBL; i++)			/* c1143 */
		ws->mkbundl_table[i] = def_mkbundl[0];			/* c1143 */
}

/*$F
 * gpolymarker(num_pts, pts) - POLYMARKER
 *
 * Gint num_pts;         number of points to plot markers at.
 * Gpoint *pts;	         array of world coordinate points.
 *
 * returns 0=OK, 5, 100.
 *
 * See Also: ANSI Standard p.82
 */
gpolymarker(num_pts, pts)
Gint num_pts;
Gpoint *pts;
{
	int cnt;
	Gpoint *ndc_pt;
	OUT_PRIMI  *pmark;

/* check for proper operating state */
	GKSERROR ((xgks_state.gks_state!=GWSAC && xgks_state.gks_state!=GSGOP) ,5, errgpolymarker) /*c1147*/

/* check for valid number of points */
	GKSERROR ((num_pts < 1) ,100, errgpolymarker)

/* open an primitive structure */
	GKSERROR (( (pmark = XgksNewPrimi()) == NULL ) ,300, errgpolymarker)

	pmark->pid = PMARK;
	pmark->primi.pmark.num_pts = num_pts;
/* get memory for transformed locations */
	GKSERROR ((((pmark->primi.pmark.location) = (Gpoint *) malloc((unsigned) num_pts * sizeof(Gpoint))) == NULL) ,
		     300, errgpolymarker)

/* transform the locations to NDC space */
	ndc_pt = pmark->primi.pmark.location;
	for (cnt=0; cnt < num_pts; cnt++, pts++, ndc_pt++)
	        WcToNdc(pts, ndc_pt);
	pmark->primi.pmark.mkattr = xgks_state.gks_mkattr; /* set up attributes for pmarker from gks state list */ /*c1147*/

	XgksProcessPrimi (pmark);

	if (MO_OPENED == TRUE) XgksMoGraphicOutput (12, num_pts, pmark->primi.pmark.location);

	free (pmark->primi.pmark.location);
	free (pmark);

	return(0);
}

/*$F
 * gsetmarkersize(size) - SET MARKER SIZE SCALE FACTOR
 *
 * Gfloat size;		new size for INDIVIDUALly specified polymarkers.
 *
 * returns 0=OK or 8, 71
 *
 * See Also: ANSI Standard p.92
 */
gsetmarkersize(size)
Gfloat size;
{
/* check for proper operating state */
	GKSERROR ((xgks_state.gks_state == GGKCL) ,8, errgsetmarkersize) /*c1147*/

/* check for proper scale factor size */
	GKSERROR ((size < 0.0 )  ,71, errgsetmarkersize)

/* ok to change scale factor */
	xgks_state.gks_mkattr.bundl.size = size;		/*c1147*/

	if (MO_OPENED == TRUE) XgksMoSetGraphicSize (27, size);

	return(0);
}

/*$F
 * gsetmarkertype(type) - SET MARKER TYPE
 *
 * Gint type;		new type for INDIVIDUALly specified markers.
 *
 * returns 0=OK or one of 8, 69
 *
 * See Also: ANSI Standard p.91
 */
gsetmarkertype(type)
Gint type;
{
/* check for proper operating state */
	GKSERROR ((xgks_state.gks_state == GGKCL) ,8, errgsetmarkertype) /*c1147*/

/* check for valid type */
	GKSERROR ((type == 0) ,69, errgsetmarkertype)

/* ok to change the type */
	xgks_state.gks_mkattr.bundl.type = type;		/*c1147*/

	if (MO_OPENED == TRUE) XgksMoSetGraphicAttr (26, type);

	return(0);
}

/*$F
 * gsetmarkercolourind(colour) - SET POLYMARKER COLOUR INDEX
 *
 * Gint colour;		new colour for INDIVIDULALly specified markers.
 *
 * returns 0=OK, or one of 8, 92
 *
 * See Also: ANSI Standard p.92
 */
gsetmarkercolourind(colour)
Gint colour;
{
/* check for proper operating state */
	GKSERROR ((xgks_state.gks_state == GGKCL) ,8, errgsetmarkercolourind) /*c1147*/

/* check for proper colour index */
	GKSERROR ((colour < 0) ,92, errgsetmarkercolourind)

/* ok to change the colour */
	xgks_state.gks_mkattr.bundl.colour = colour;		/*c1147*/

	if (MO_OPENED == TRUE) XgksMoSetGraphicAttr (28, colour);

	return(0);
}

/*$F
 * gsetmarkerind(idx) - SET POLYMARKER INDEX
 *
 * Gint idx;	             new bundle table index for BUNDLEd attributes.
 *
 * returns 0=OK or one of 8, 66
 *
 * See Also: ANSI Standard p.91
 */
gsetmarkerind(idx)
Gint idx;
{
/* check for proper operating state */
	GKSERROR ((xgks_state.gks_state == GGKCL) ,8, errgsetmarkerind) /*c1147*/

/* check for valid polymarker index */
	GKSERROR ((idx < 1) ,66, errgsetmarkerind)

/* change the bundle table index in the gks state table */
	xgks_state.gks_mkattr.mark = idx;			/*c1147*/

	if (MO_OPENED == TRUE) XgksMoSetGraphicAttr (25, idx);

	return(0);
}

/*$F
 * gsetmarkerrep(ws_id, idx, rep) - SET POLYMARKER REPRESENTATION
 *
 * Gint ws_id;		workstation identifier.
 * Gint idx,		polymarker bundle entry to set.
 * Gmkbundl *rep;	pmark bundle representation
 *
 * returns 0=OK or one of 7, 20, 25, 33, 35, 36, 66, 69, 70, 71, 93
 *
 * See Also: ANSI Standard p.101
 */
gsetmarkerrep(ws_id, idx, rep)
Gint ws_id;
Gint idx;
Gmkbundl *rep;
{
	WS_STATE_PTR ws;

/* check for proper operating state */
	GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP) ,7, errgsetmarkerrep) /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgsetmarkerrep)   /* c1012 */

/* check for open ws_id */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR (((ws=OPEN_WSID(ws_id)) == NULL) ,25, errgsetmarkerrep)  /* c1012 */

/* check for valid workstation category */
	GKSERROR ( (WS_CAT(ws)==GMI), 33, errgsetmarkerrep)

	GKSERROR ( (WS_CAT(ws)==GINPUT), 35, errgsetmarkerrep)

	GKSERROR ( (WS_CAT(ws)==GWISS), 36, errgsetmarkerrep)

/* check for valid polymarker index */
	GKSERROR ((idx<1 || idx>=MAX_BUNDL_TBL) ,66, errgsetmarkerrep)

/* check for valid marker type */
	GKSERROR ((rep->type == 0) ,69, errgsetmarkerrep)

	GKSERROR ((!WS_MARKER_TYPE(rep->type)), 70, errgsetmarkerrep)

/* check for valid factor size */
	GKSERROR ((rep->size < 0.0) ,71, errgsetmarkerrep)

/* check for proper colour index */
	GKSERROR ((!WS_AVAIL_COLOUR(ws, rep->colour)) ,93, errgsetmarkerrep)

	if (ws->ewstype == MO)
		XgksMoSetLineMarkRep (ws, 52, idx, rep->type, rep->size, rep->colour);

/* tell the workstation */
	ws->mkbundl_table[idx] = (*rep);
	return(0);
}
