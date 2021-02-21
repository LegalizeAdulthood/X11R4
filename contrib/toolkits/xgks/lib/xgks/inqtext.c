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

#include <string.h>
#include "gks_implem.h"

/*
 * ginqtextfacil(ws_type, fac)
 *      INQUIRE TEXT FACILITIES
 *
 * Gchar *ws_type;              type of workstation the inquiry is about.
 * Gtxfac fac;                  returned text facility values.
 *
 * returns all information in the parameters.
 * errors 0, 8, 22, 23, 39 can occur.
 *
 * See also: ANSI standard p.171
 */

Gint ginqtextfacil(ws_type, fac)
        Gchar *ws_type;
        Gtxfac *fac;
{
        EWSTYPE ewstype;
        int i;

/* check for proper operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqtextfacil); /*c1147*/

/* check for valid ws_type */
        ewstype = XgksWsTypeToEnum( ws_type );
        GKSERROR( (ewstype == WST_INVALID), 22, errginqtextfacil);     /* c1087 */
        GKSERROR( ewstype != X_WIN, 39, errginqtextfacil);

/* set the return values */
        fac->fps = 9;

/* get space for list */
        fac->fp_list = (Gtxfp *)malloc((unsigned) fac->fps * sizeof(Gtxfp));

/* set the return values */
        for (i=0; i<fac->fps; i++) {
                fac->fp_list[i].font = i+1;
                fac->fp_list[i].prec = GSTROKE;
        }
        fac->heights = 0.0;
        fac->min_ht = 0.01;                                     /* c1156 */
        fac->max_ht = 1024.0;
        fac->expansions = 0.0;
        fac->min_ex = 0.001;                                    /* c2011 */
        fac->max_ex = 1024.0;
        fac->predefined = PDF_TEXT_BNDLS;                       /* c1143 */

        return( OK );
}

/*$F
 * ginqpredtextrep(ws_type, idx, rep) -
 *      INQUIRE PREDEFINED TEXT REPRESENTATION
 *
 * Gchar *ws_type;              workstation type this inquiry is about.
 * Gint idx;                    text index.
 * Gtxbundl *rep;               predefined text bundle values.
 *
 * returns all information in the parameters.
 * errors 0, 8, 22, 23, 39, 72, 74 can occur.
 *
 * See also: ANSI standard p.171
 */

/* c1147:  Moved predefined text bundles into text.h */
#include "text.h"                                       /*c1147*/

Gint ginqpredtextrep(ws_type, idx, rep)
        Gchar *ws_type;
        Gint idx;
        Gtxbundl *rep;
{
        EWSTYPE ewstype;

/* check for proper operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqpredtextrep); /*c1147*/

/* check for valid ws_type */
        ewstype = XgksWsTypeToEnum( ws_type );
        GKSERROR( (ewstype == WST_INVALID), 22, errginqpredtextrep);    /* c1087 */
        GKSERROR( ewstype != X_WIN, 39, errginqpredtextrep);

/* check for valid idx */
        GKSERROR((idx < 1 || idx > PDF_TEXT_BNDLS), 72, errginqpredtextrep);/*c1143*/

/* set the return values */
        rep->fp.font = def_txbundl[idx-1].fp.font;              /* c1143 */
        rep->fp.prec = def_txbundl[idx-1].fp.prec;              /* c1143 */
        rep->ch_exp = def_txbundl[idx-1].ch_exp;                /* c1143 c1179 */
        rep->space =   def_txbundl[idx-1].space;                /* c1143 */
        rep->colour =   def_txbundl[idx-1].colour;              /* c1143 */

        return( OK );
}

/*$F
 * ginqtextindices(ws_id, idxlist) - INQUIRE LIST OF TEXT INDICES
 *
 * Gint ws_id;                  workstation inquiry is about.
 * Gintlist *idxlist;           list of defined text indices
 *
 * returns all information in the parameters.
 * errors 0, 7, 20, 25, 33, 35, 36 can occur.
 *
 * See also: ANSI standard p.152
 */

Gint ginqtextindices(ws_id, idxlist)
        Gint ws_id;
        Gintlist *idxlist;
{
        WS_STATE_PTR ws;
        int  i;

/* check for proper operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqtextindices); /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqtextindices)   /* c1012 */

/* check if this workstation is opened */
/* DWO 7/26/88  changed macro name from VALID_WSID */
        GKSERROR( ((ws = OPEN_WSID(ws_id)) == NULL), 25, errginqtextindices);  /* c1012 */

/* check workstation type */
        GKSERROR( (ws->ewstype == MI), 33, errginqtextindices);
        GKSERROR( (ws->ewstype == WISS), 36, errginqtextindices);

/* get space for list */
        idxlist->number = 20;
        idxlist->integers = (Gint *)malloc((unsigned) idxlist->number * sizeof(int));

/* set the indecies values */
        for (i=0; i<idxlist->number; i++)
                idxlist->integers[i] = i+1;

        return( OK );
}

/*$F
 * ginqtextrep(ws_id, idx, type, rep) - INQUIRE LIST OF TEXT INDICES
 *
 * Gint ws_id;                  workstation inquiry is about.
 * Gint idx;
 * Gqtype type;
 * Gtxbundl *rep;
 *
 * returns all information in the parameters.
 * errors 0, 7, 20, 25, 33, 35, 36, 72, 73 can occur.
 *
 * See also: ANSI standard p.152
 */

Gint ginqtextrep(ws_id, idx, type, rep)
        Gint ws_id;
        Gint idx;
        Gqtype type;
        Gtxbundl *rep;
{
        WS_STATE_PTR ws;

/* check for proper operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqtextrep); /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqtextrep)   /* c1012 */

/* check if this workstation is opened */
/* DWO 7/26/88  changed macro name from VALID_WSID */
        GKSERROR( ((ws = OPEN_WSID(ws_id)) == NULL), 25, errginqtextrep);  /* c1012 */

/* check workstation type */
        GKSERROR( (ws->ewstype == MI), 33, errginqtextrep);
        GKSERROR( (ws->ewstype == WISS), 36, errginqtextrep);

/* check for valid idx */
        GKSERROR( (idx < 1 || idx > 20), 72, errginqtextrep);

/* set the returned values */
        rep->fp.font = ws->txbundl_table[idx].fp.font;
        rep->fp.prec = ws->txbundl_table[idx].fp.prec;
        rep->ch_exp = ws->txbundl_table[idx].ch_exp;    /* c1179 */
        rep->space = ws->txbundl_table[idx].space;
        rep->colour = ws->txbundl_table[idx].colour;

        return( OK );
}
