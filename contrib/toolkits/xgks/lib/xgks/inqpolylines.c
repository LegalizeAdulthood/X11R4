/*
 *              Copyright IBM Corporation 1989
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
 * Urbana, IL   61801
 *
 * (C) Copyright 1987, 1988 by The University of Illinois Board of Trustees.
 *      All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 *
 */

#include <string.h>

#include "gks_implem.h"

#include "polylines.h"

/*
 * ginqlinefacil(ws_type, fac)
 *      INQUIRE POLYLINE FACILITIES
 *
 * Gchar *ws_type;              type of workstation the inquiry is about.
 * Glnfac fac;                  returned polyline facility values.
 *
 * returns all information in the parameters.
 * errors 0, 8, 22, 23, 39 can occur.
 *
 * See also: ANSI standard p.173
 */

Gint ginqlinefacil(ws_type, fac)
        Gchar *ws_type;
        Glnfac *fac;
{
        EWSTYPE ewstype;

/* check for proper operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqlinefacil);

/* check for valid ws_type */
        ewstype = XgksWsTypeToEnum( ws_type );
        GKSERROR( (ewstype == WST_INVALID), 22, errginqlinefacil);    /* c1087 */
        GKSERROR( ewstype != X_WIN, 39, errginqlinefacil);

/* set the return values */
        fac->widths = 0;
        fac->nom = 1.0;
        fac->min = 0.01;                                /* c1156 */
        fac->max = 1024.0;
        fac->predefined = PDF_LINE_BNDLS;                       /* c1143 */
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
 *      INQUIRE PREDEFINED POLYLINE REPRESENTATION
 *
 * Gchar *ws_type;              workstation type this inquiry is about.
 * Gint idx;                    polyline index.
 * Glnbundl *rep;               predefined polyline bundle values.
 *
 * returns all information in the parameters.
 * errors 0, 8, 22, 23, 39, 60 can occur.
 *
 * See also: ANSI standard p.171
 */

Gint ginqpredlinerep(ws_type, idx, rep)
        Gchar *ws_type;
        Gint idx;
        Glnbundl *rep;
{
        EWSTYPE ewstype;

/* check for proper operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqpredlinerep);

/* check for valid ws_type */
        ewstype = XgksWsTypeToEnum( ws_type );
        GKSERROR( (ewstype == WST_INVALID), 22, errginqpredlinerep);   /* c1087 */
        GKSERROR( ewstype != X_WIN, 39, errginqpredlinerep);

/* check for valid idx */
        GKSERROR((idx < 1 || idx > PDF_LINE_BNDLS), 60, errginqpredlinerep);/*c1143*/

/* set the return values */
        rep->type =  def_lnbundl[idx-1].type;                           /* c1143 */
        rep->width = def_lnbundl[idx-1].width;                          /* c1143 */
        rep->colour = def_lnbundl[idx-1].colour;                        /* c1143 */

        return( OK );
}

/*$F
 * ginqlineindices(ws_id, idxlist) - INQUIRE LIST OF POLYLINE INDICES
 *
 * Gint ws_id;                  workstation inquiry is about.
 * Gintlist *idxlist;           list of defined polyline indices
 *
 * returns all information in the parameters.
 * errors 0, 7, 20, 25, 33, 35, 36 can occur.
 *
 * See also: ANSI standard p.154
 */

Gint ginqlineindices(ws_id, idxlist)
        Gint ws_id;
        Gintlist *idxlist;
{
        WS_STATE_PTR ws;
        int  i;

/* check for proper operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqlineindices);

/* check for invalid workstation id */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqlineindices)

/* check if this workstation is opened */
        GKSERROR( ((ws = OPEN_WSID(ws_id)) == NULL), 25, errginqlineindices);

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
 * Gint ws_id;                  workstation inquiry is about.
 * Gint idx;
 * Gqtype type;
 * Glnbundl *rep;
 *
 * returns all information in the parameters.
 * errors 0, 7, 20, 25, 33, 35, 36, 60 can occur.
 *
 * See also: ANSI standard p.152
 */

Gint ginqlinerep(ws_id, idx, type, rep)
        Gint ws_id;
        Gint idx;
        Gqtype type;
        Glnbundl *rep;
{
        WS_STATE_PTR ws;

/* check for proper operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqlinerep);

/* check for invalid workstation id */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqlinerep)

/* check if this workstation is opened */
        GKSERROR( ((ws = OPEN_WSID(ws_id)) == NULL), 25, errginqlinerep);

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
