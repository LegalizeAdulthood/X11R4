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
 * $Header: inqpixel.c,v 4.0 89/08/31 16:21:37 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/inqpixel.c,v $
 *
 * $Log:	inqpixel.c,v $
 * Revision 4.0  89/08/31  16:21:37  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.8  89/06/05  10:16:50  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * 
 * Revision 3.7  89/02/04  15:24:48  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 3.6  88/12/08  10:16:28  amy
 * PTR c1130	GKSERROR calls:  changed function name parameter from character
 * 		string to enum. type value.
 * 
 * Revision 3.5  88/08/17  09:45:32  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 3.4  88/08/11  08:28:23  amy
 * PTR c1012  ginqpixel, ginqpixelarray, ginqpixelarraydim:
 * Added call to VALID_WSID to check for error 20.
 * 
 * Revision 1.4  88/07/28  14:20:35  owens
 * added check for error 20 for PTR c1012
 * 
 * Revision 1.3  88/07/28  14:16:16  owens
 * added check for error 20 for PTR c1012
 * 
 * Revision 1.2  88/07/26  17:52:20  owens
 * renamed/replaced VALID_WSID macro for PTR c1012 (DWO)
 * 
 * Revision 1.1  88/07/21  10:49:05  david
 * Initial revision
 *  
 *
 */

static char rcsid[] = "$Header: inqpixel.c,v 4.0 89/08/31 16:21:37 amy Exp $";

#include <string.h>
#include "gks_implem.h"                 /* d1 */

ginqpixelarraydim(ws_id, rect, dim)
Gint ws_id;
Grect *rect;
Gipoint *dim;
{
	WS_STATE_PTR ws;

/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqpixelarraydim); /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqpixelarraydim)   /* c1012 */

/* check if this workstation is opened */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR( ((ws = OPEN_WSID(ws_id)) == NULL), 25, errginqpixelarraydim);  /* c1012 */

/* check workstation type */
	GKSERROR( (ws->ewstype != X_WIN), 39, errginqpixelarraydim);

/* get the returned values from the X-server */
	xXgksInqPixelarrayDim(ws, rect, dim);		/*c1147*/

	return( OK );
}

/*$F
 * ginqpixelarray(ws_id, point, dimen, pxarr)	INQUIRE PIXEL ARRAY
 *
 * Gint ws_id;		        workstation identifier.
 * Gpoint *point;		pixel array location pointer.
 * Gipoint *dimen;		pixel array dimension pointer.
 * Gpxarray *pxarr;		OUT pixel array.
 *
 * returns all information in the parameters.
 * errors 7, 20, 25, 39 40 91 can occur.
 *
 * See also: ANSI standard p.191
 */

ginqpixelarray(ws_id, point, dimen, pxarr)
Gint ws_id;
Gpoint *point;
Gipoint *dimen;
Gpxarray *pxarr;
{
	WS_STATE_PTR ws;

/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqpixelarray); /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqpixelarray)   /* c1012 */

/* check if this workstation is opened */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR( ((ws = OPEN_WSID(ws_id)) == NULL), 25, errginqpixelarray);  /* c1012 */

/* check workstation type */
	GKSERROR( (ws->ewstype != X_WIN), 39, errginqpixelarray);

/* check for valid dimensions of colour array */
	GKSERROR( (dimen->x < 1 || dimen->y < 1), 91, errginqpixelarray);

/* get the returned values from the X-server */
	xXgksInqPixelarray(ws, point, dimen, pxarr);	/*c1147*/

	return( OK );
}

/*$F
 * ginqpixel(ws_id, ppoint, pix)	INQUIRE PIXEL
 *
 * Gint ws_id;		        workstation identifier.
 * Gpoint *ppoint;		pixel location pointer.
 * Gint *pix;			OUT pixel colour.
 *
 * returns all information in the parameters.
 * errors 7, 20, 25, 39 40 can occur.
 *
 * See also: ANSI standard p.191
 */

ginqpixel(ws_id, ppoint, pix)
Gint ws_id;
Gpoint *ppoint;
Gint *pix;
{
	WS_STATE_PTR ws;

/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqpixel); /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqpixel)   /* c1012 */

/* check if this workstation is opened */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR( ((ws = OPEN_WSID(ws_id)) == NULL), 25, errginqpixel);  /* c1012 */

/* check workstation type */
	GKSERROR( (ws->ewstype != X_WIN), 39, errginqpixel);

/* get the returned values from the X-server */
	xXgksInqPixel(ws, ppoint, pix);		/*c1147*/

	return( OK );
}
