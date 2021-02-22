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
 * All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 *
 */

#include "gks_implem.h"

#include "polymarkers.h"


/*$F
 * ginqmarkerfacil(ws_type, fac)
 *      INQUIRE POLYMARKER FACILITIES
 *
 * Gchar *ws_type;              type of workstation the inquiry is about.
 * Gmkfac fac;                  returned polymarker facility values.
 *
 * returns all information in the parameters.
 * errors 0, 8, 22, 23, 39 can occur.
 *
 * See also: ANSI standard p.171
 */
Gint ginqmarkerfacil(ws_type, fac)
        Gchar *ws_type;
        Gmkfac *fac;
{
        EWSTYPE ewstype;
        int i;

/* check for proper operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqmarkerfacil);

/* check for valid ws_type */
        ewstype = XgksWsTypeToEnum( ws_type );
        GKSERROR( (ewstype == WST_INVALID), 22, errginqmarkerfacil);     /* c1087 */
        GKSERROR( ewstype != X_WIN, 39, errginqmarkerfacil);

/* set the return values */
        fac->sizes = 0;
        fac->nom = 6.0;
        fac->min = 0.01;                                /* c1156 */
        fac->max = 1024.0;
        fac->predefined = PDF_MARK_BNDLS;                       /* c1143 */
        fac->types.number = 5;

/* get space for list */
        fac->types.integers = (Gint *)malloc((unsigned)fac->types.number * sizeof(int));
        GKSERROR( (fac->types.integers == NULL), 300, errginqmarkerfacil);

/* set returned index values */
        for(i=0; i< fac->types.number; i++)
                fac->types.integers[i] = i+1;

        return( OK );
}

/*$F
 * ginqpredmarkerrep(ws_type, idx, rep) -
 *      INQUIRE PREDEFINED POLYMARKER REPRESENTATION
 *
 * Gchar *ws_type;              workstation type this inquiry is about.
 * Gint idx;                    polymarker index.
 * Gmkbundl *rep;               predefined polymarker bundle values.
 *
 * returns all information in the parameters.
 * errors 0, 8, 22, 23, 39, 66, 68 can occur.
 *
 * See also: ANSI standard p.171
 */

Gint ginqpredmarkerrep(ws_type, idx, rep)
        Gchar   *ws_type;
        Gint idx;
        Gmkbundl *rep;
{
        EWSTYPE ewstype;

/* check for proper operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqpredmarkerrep);

/* check for valid ws_type */
        ewstype = XgksWsTypeToEnum( ws_type );
        GKSERROR( (ewstype == WST_INVALID), 22, errginqpredmarkerrep);  /* c1087 */
        GKSERROR( ewstype != X_WIN, 39, errginqpredmarkerrep);

/* check for valid idx */
        GKSERROR((idx < 1 || idx > PDF_MARK_BNDLS),66,errginqpredmarkerrep);/*c1143*/

/* set the return values */
        rep->type =  def_mkbundl[idx-1].type;                           /* c1143 */
        rep->size =  def_mkbundl[idx-1].size;                           /* c1143 */
        rep->colour = def_mkbundl[idx-1].colour;                        /* c1143 */

        return( OK );
}

/*$F
 * ginqmarkerindices(ws_id, idxlist) - INQUIRE LIST OF POLYMARKER INDICES
 *
 * Gint ws_id;                  workstation inquiry is about.
 * Gintlist *idxlist;           list of defined polymarker indices
 *
 * returns all information in the parameters.
 * errors 0, 7, 20, 25, 33, 35, 36 can occur.
 *
 * See also: ANSI standard p.152
 */

Gint ginqmarkerindices(ws_id, idxlist)
        Gint ws_id;
        Gintlist *idxlist;
{
        WS_STATE_PTR ws;
        int  i;

/* check for proper operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqmarkerindices);

/* check for invalid workstation id */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqmarkerindices)   /* c1012 */

/* check if this ws_id is opened */
        GKSERROR( ((ws = OPEN_WSID(ws_id))==NULL), 25, errginqmarkerindices);  /* c1012 */

/* check workstation type */
        GKSERROR( (ws->ewstype == MI), 33, errginqmarkerindices);
        GKSERROR( (ws->ewstype == WISS), 36, errginqmarkerindices);

/* get space for list */
        idxlist->number = 20;
        idxlist->integers = (Gint *)malloc((unsigned)idxlist->number * sizeof(int));

/* set the indecies values */
        for (i=0; i<idxlist->number; i++)
                idxlist->integers[i] = i+1;

        return( OK );
}

/*$F
 * ginqmarkerrep(ws_id, idx, type, rep) - INQUIRE LIST OF POLYMARKER INDICES
 *
 * Gint ws_id;                  workstation inquiry is about.
 * Gint idx;
 * Gqtype type;
 * Gmkbundl *rep;
 *
 * returns all information in the parameters.
 * errors 0, 7, 20, 25, 33, 35, 36, 66, 67 can occur.
 *
 * See also: ANSI standard p.152
 */

Gint ginqmarkerrep(ws_id, idx, type, rep)
        Gint ws_id;
        Gint idx;
        Gqtype type;
        Gmkbundl *rep;
{
        WS_STATE_PTR ws;

/* check for proper operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqmarkerrep);

/* check for invalid workstation id */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqmarkerrep)   /* c1012 */

/* check if this ws_id is opened */
        GKSERROR( ((ws = OPEN_WSID(ws_id))==NULL), 25, errginqmarkerrep);  /* c1012 */

/* check workstation type */
        GKSERROR( (ws->ewstype == MI), 33, errginqmarkerrep);
        GKSERROR( (ws->ewstype == WISS), 36, errginqmarkerrep);

/* check for valid idx */
        GKSERROR( (idx < 1 || idx > 20), 66, errginqmarkerrep);

/* set the returned values */
        rep->type = ws->mkbundl_table[idx].type;
        rep->size = ws->mkbundl_table[idx].size;
        rep->colour = ws->mkbundl_table[idx].colour;

        return( OK );
}
