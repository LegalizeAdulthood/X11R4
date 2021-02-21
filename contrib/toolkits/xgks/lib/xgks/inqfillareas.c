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



/*
 * INQUIRE FILLAREA FACILITIES
 *
 * errors 0, 8, 22, 23, 39 can occur.
 *
 * See also: ANSI standard p.177
 */


extern Gptbundl xgks_xpttn[];           /*c1147*/


Gint ginqfillfacil(ws_type, fac)
        Gchar *ws_type;
        Gflfac *fac;
{
        int i;
        EWSTYPE ewstype;

/* check for proper operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqfillfacil); /*c1147*/

/* check for valid ws_type */
        ewstype = XgksWsTypeToEnum( ws_type );
        GKSERROR( (ewstype == WST_INVALID), 22, errginqfillfacil);  /* c1087 */
        GKSERROR( ewstype != X_WIN, 39, errginqfillfacil);

/* set the return values */
        fac->predefined = PDF_FILL_BNDLS;                       /* c1143 */
        fac->interiors.number = 4;
        fac->hatches.number = 20;

/* get space for list */
        fac->interiors.integers = (Gint *)malloc((unsigned) fac->interiors.number * sizeof(int));
        GKSERROR( (fac->interiors.integers == NULL), 300, errginqfillfacil);

        fac->hatches.integers = (Gint *)malloc((unsigned) fac->hatches.number * sizeof(int));
        GKSERROR( (fac->hatches.integers == NULL), 300, errginqfillfacil);

/* set returned index values */
        fac->interiors.integers[0] = (Gint) GHOLLOW;            /* c1158 */
        fac->interiors.integers[1] = (Gint) GSOLID;             /* c1158 */
        fac->interiors.integers[2] = (Gint) GPATTERN;           /* c1158 */
        fac->interiors.integers[3] = (Gint) GHATCH;             /* c1158 */

        for(i=0; i< fac->hatches.number; i++)
                fac->hatches.integers[i] = -(i+1);      /* c1174 */

        return( OK );
}

/*
 * INQUIRE PREDEFINED FILLAREA REPRESENTATION
 *
 * errors 0, 8, 22, 23, 39, 80, 82 can occur.
 *
 * See also: ANSI standard p.178
 */

/* c1147:  Predefined bundles now kept in fillarea.h */
#include "fillarea.h"                                           /* c1147 */

Gint ginqpredfillrep(ws_type, idx, rep)
        Gchar *ws_type;
        Gint idx;
        Gflbundl *rep;
{
        EWSTYPE ewstype;

/* check for proper operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqpredfillrep); /*c1147*/

/* check for valid ws_type */
        ewstype = XgksWsTypeToEnum( ws_type );
        GKSERROR( (ewstype == WST_INVALID), 22, errginqpredfillrep);     /* c1087 */
        GKSERROR( ewstype != X_WIN, 39, errginqpredfillrep);

/* check for valid idx */
        GKSERROR((idx < 1 || idx > PDF_FILL_BNDLS),80,errginqpredfillrep); /*c1143*/

/* set the return values */
        rep->inter = def_flbundl[idx-1].inter;                          /* c1143 */
        rep->style = def_flbundl[idx-1].style;                          /* c1143 */
        rep->colour = def_flbundl[idx-1].colour;                        /* c1143 */

        return( OK );
}

/*
 * INQUIRE LIST OF FILLAREA INDICES
 *
 * errors 0, 7, 20, 25, 33, 35, 36 can occur.
 *
 * See also: ANSI standard p.161
 */

Gint ginqfillindices(ws_id, idxlist)
        Gint ws_id;
        Gintlist *idxlist;
{
        WS_STATE_PTR ws;
        int  i;

/* check for proper operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqfillindices); /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqfillindices)   /* c1012 */

/* check if this ws_id is opened */
/* DWO 7/26/88  changed macro name from VALID_WSID */
        GKSERROR( ((ws=OPEN_WSID(ws_id))==NULL), 25, errginqfillindices);  /* c1012 */

/* check workstation type */
        GKSERROR( (ws->ewstype == MI), 33, errginqfillindices);
        GKSERROR( (ws->ewstype == WISS), 36, errginqfillindices);

/* get space for list */
        idxlist->number = 20;
        idxlist->integers = (Gint *)malloc((unsigned) idxlist->number * sizeof(int));

/* set the indecies values */
        for (i=0; i<idxlist->number; i++)
                idxlist->integers[i] = i+1;

        return( OK );
}

/*
 * INQUIRE FILLAREA REPRESENTATION
 *
 * errors 0, 7, 20, 25, 33, 35, 36, 80, 81 can occur.
 *
 * See also: ANSI standard p.162
 */

Gint ginqfillrep(ws_id, idx, type, rep)
        Gint ws_id;
        Gint idx;
        Gqtype type;
        Gflbundl *rep;
{
        WS_STATE_PTR ws;

/* check for proper operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqfillrep); /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqfillrep)   /* c1012 */

/* check if this ws_id is opened */
/* DWO 7/26/88  changed macro name from VALID_WSID */
        GKSERROR( ((ws=OPEN_WSID(ws_id))==NULL), 25, errginqfillrep);  /* c1012 */

/* check workstation type */
        GKSERROR( (ws->ewstype == MI), 33, errginqfillrep);
        GKSERROR( (ws->ewstype == WISS), 36, errginqfillrep);

/* check for valid idx */
        GKSERROR( (idx < 1 || idx > 20), 80, errginqfillrep);

/* set the returned values */
        rep->inter = ws->flbundl_table[idx].inter;
        rep->style = ws->flbundl_table[idx].style;
        rep->colour = ws->flbundl_table[idx].colour;

        return( OK );
}

/*
 * INQUIRE PATTERN FACILITIES
 *
 * errors 0, 8, 22, 23, 39 can occur.
 *
 * See also: ANSI standard p.179
 */

Gint ginqpatfacil(ws_type, fac)
        Gchar *ws_type;
        Gint *fac;
{
        EWSTYPE ewstype;

/* check for proper operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqpatfacil); /*c1147*/

/* check for valid ws_type */
        ewstype = XgksWsTypeToEnum( ws_type );
        GKSERROR( (ewstype ==  WST_INVALID), 22, errginqpatfacil);  /* c1087 */
        GKSERROR( ewstype != X_WIN, 39, errginqpatfacil);

/* set the return values */
        (*fac) = 20;

        return( OK );
}

/*
 * INQUIRE PREDEFINED PATTERN REPRESENTATION
 *
 * errors 0, 8, 22, 23, 39, 85, 89 can occur.
 *
 * See also: ANSI standard p.179
 */

Gint ginqpredpatrep(ws_type, idx, rep)
        Gchar *ws_type;
        Gint idx;
        Gptbundl *rep;
{
        EWSTYPE ewstype;
        int i, j;

/* check for proper operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqpredpatrep); /*c1147*/

/* check for valid ws_type */
        ewstype = XgksWsTypeToEnum( ws_type );
        GKSERROR( (ewstype == WST_INVALID), 22, errginqpredpatrep);    /* c1087 */
        GKSERROR( ewstype != X_WIN, 39, errginqpredpatrep);

/* check for valid idx */
        GKSERROR( (idx < 1 || idx > 20), 85, errginqpredpatrep);

/* set the return values */
        *rep = xgks_xpttn[idx-1];                               /*c1147*/
        j = rep->size.x * rep->size.y;
        rep->array = (Gint *)malloc( j * sizeof(int));
        GKSERROR( (rep->array == NULL), 300, errginqpredpatrep);

        for (i=0; i < j; i++)
                rep->array[i] = xgks_xpttn[idx-1].array[i];     /*c1147*/

        return( OK );
}

/*
 * INQUIRE LIST OF PATTERN INDICES
 *
 * errors 0, 7, 20, 25, 33, 35, 36 can occur.
 *
 * See also: ANSI standard p.162
 */

Gint ginqpatindices(ws_id, idxlist)
        Gint ws_id;
        Gintlist *idxlist;
{
        WS_STATE_PTR ws;
        int  i;

/* check for proper operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqpatindices); /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqpatindices)   /* c1012 */

/* check if this ws_id is opened */
/* DWO 7/26/88  changed macro name from VALID_WSID */
        GKSERROR( ((ws=OPEN_WSID(ws_id))==NULL), 25, errginqpatindices);  /* c1012 */

/* check workstation type */
        GKSERROR( (ws->ewstype == MI), 33, errginqpatindices);
        GKSERROR( (ws->ewstype == WISS), 36, errginqpatindices);

/* get space for list */
        idxlist->number = 20;
        idxlist->integers = (Gint *)malloc((unsigned) idxlist->number * sizeof(int));

/* set the indecies values */
        for (i=0; i<idxlist->number; i++)
                idxlist->integers[i] = i+1;

        return( OK );
}

/*
 * INQUIRE PATTERN REPRESENTATION
 *
 * errors 0, 7, 20, 25, 33, 35, 36, 85, 88 can occur.
 *
 * See also: ANSI standard p.152
 */

Gint ginqpatrep(ws_id, idx, type, rep)
        Gint ws_id;
        Gint idx;
        Gqtype type;
        Gptbundl *rep;
{
        WS_STATE_PTR ws;
        int i, j;

/* check for proper operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqpatrep); /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqpatrep)   /* c1012 */

/* check if this ws_id is opened */
/* DWO 7/26/88  changed macro name from VALID_WSID */
        GKSERROR( ((ws=OPEN_WSID(ws_id))==NULL), 25, errginqpatrep);  /* c1012 */

/* check workstation type */
        GKSERROR( (ws->ewstype == MI), 33, errginqpatrep);
        GKSERROR( (ws->ewstype == WISS), 36, errginqpatrep);

/* check for valid idx */
        GKSERROR( (idx < 1 || idx > 20), 85, errginqpatrep);

/* set the returned values */
        *rep = ws->ptbundl_table[idx-1];
        j = rep->size.x * rep->size.y;
        rep->array = (Gint *)malloc( j * sizeof(int));
        GKSERROR( (rep->array == NULL), 300, errginqpatrep);

        for (i=0; i < j; i++)
                rep->array[i] = ws->ptbundl_table[idx-1].array[i];



        return( OK );
}
