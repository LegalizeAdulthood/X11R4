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
 * $Header: cellarray.c,v 4.0 89/08/31 16:19:49 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/cellarray.c,v $
 *
 * $Log:	cellarray.c,v $
 * Revision 4.0  89/08/31  16:19:49  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.8  89/06/05  10:15:57  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * 
 * Revision 3.7  89/02/23  14:22:24  amy
 * PTR c2018	gcellarray:  change orientation of cellarray to match GKS standard.
 * 
 * Revision 3.6  89/02/04  15:22:19  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 3.5  88/12/07  15:43:55  amy
 * PTR c1130	GKSERROR calls:  changed function name parameter from a character
 * 		string to an enum. type value.
 * Changed spellings from color to colour.
 * 
 * Revision 3.4  88/08/16  12:58:05  amy
 * Added Log variable to incorporate RCS log messages
 * 
 * Revision 1.2  88/07/26  17:51:27  owens
 * renamed/replaced VALID_WSID macro for PTR c1012 (DWO)
 * 
 * Revision 1.1  88/07/21  10:48:23  david
 * Initial revision
 *  
 *
 */
 
static char *rcsid = "$Header: cellarray.c,v 4.0 89/08/31 16:19:49 amy Exp $";
                     
#include "gks_implem.h" /* d1 */


/*
 * gcellarray(rect, dim, row, colour) - CELLARRAY
 * 
 * Grect *rect;		cell array rectangle pointer.
 * Gipoint *dim;	colour index array dimensions pointer.
 * Gint row;		colour index array row length.
 * Gint *colour;	colour index array.
 *
 * returns: 0=OK, or one of 5, 91
 *
 * See also: ANSI standard p.85
 */


gcellarray(rect, dim, row, colour)
Grect *rect;
Gipoint *dim;
Gint row;
Gint *colour;
{
	Gpoint pnt;			/* temp space for tranformed points */
	Gint i, cnt, *p1, *p2;
	OUT_PRIMI  *cell;

/* check for proper operating state */
	GKSERROR ((xgks_state.gks_state!=GWSAC && xgks_state.gks_state!=GSGOP) ,5, errgcellarray) /*c1147*/

/* check array size */
	GKSERROR ( (dim->x < 1 || dim->y < 1), 91, errgcellarray)	/* AUG */

/* open an primitive structure */
	GKSERROR (( (cell = XgksNewPrimi()) == NULL ) ,300, errgcellarray)
	
	cell->pid = CELL_ARRAY;		
	cell->primi.cell_array.dim = *dim;
	cell->primi.cell_array.rowsize = row;

	cnt = row * dim->y;

/* now get memory for colour index array */
	GKSERROR (((cell->primi.cell_array.colour = (Gint *) malloc((unsigned)(cnt * sizeof(Gint)))) == NULL) ,
			 300, errgcellarray);

/* transform the WC to NDC */
	pnt = rect->ll;
	WcToNdc(&pnt, &(cell->primi.cell_array.ll));		/* c2018 */
	pnt = rect->ur;
	WcToNdc(&pnt, &(cell->primi.cell_array.ur));		/* c2018 */
	pnt.x = rect->ll.x;
	pnt.y = rect->ur.y;
	WcToNdc(&pnt, &(cell->primi.cell_array.ul));		/* c2018 */
	pnt.x = rect->ur.x;
	pnt.y = rect->ll.y;
	WcToNdc(&pnt, &(cell->primi.cell_array.lr));		/* c2018 */

	p1 = colour;
	p2 = cell->primi.cell_array.colour;
	for (i=0; i<cnt; i++, p1++, p2++)
		*p2 = *p1;

	if (MO_OPENED == TRUE) 
		XgksMoCellArray (&(cell->primi.cell_array.ll), &(cell->primi.cell_array.ur), 
			&(cell->primi.cell_array.lr), row, cell->primi.cell_array.colour, 
			&(cell->primi.cell_array.dim));

/* process this primitive */
	XgksProcessPrimi(cell);
	free(cell->primi.cell_array.colour);
	free (cell);
	return(0);
}
