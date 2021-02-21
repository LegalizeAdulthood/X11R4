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

#include "gks_implem.h"

/*$F
 * ginqcurntrannum(tran) - INQUIRE CURRENT NORMALIZATION TRANSFORMATION NUMBER
 *
 * Gint *tran;                  the currently selected transformation.
 *
 * returns all information in the parameters.
 * errors 0,
 *
 * See also: ANSI Standard p.147
 */
Gint ginqcurntrannum(tran)
        Gint *tran;
{
/* check proper gks operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqcurntrannum); /*c1147*/

/* set up return values */
        *tran = xgks_state.cur_ntrans;          /*c1147*/
        return( OK );
}

/*$F
 * ginqntrannum(tranlist) - INQUIRE LIST OF NORMALIZATION TRANSFORMATION NUMBERS
 *
 * Gintlist *tranlist;       OUT list of normalization transformation numbers
 *
 *      On input, ntrans contains the maximum size for the returned list.
 *
 * returns all information in the parameters
 * errors 0, 8
 *
 * See also: ANSI Standard p.147
 */

Gint ginqntrannum(tranlist)
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
 * Gwstran *tran,               transformation structure
 *
 * returns all information in the parameters
 * errors 0, 8, 50
 *
 * See also: ANSI Standard p.148
 */

Gint ginqntran(num, tran)
Gint num;
Gwstran *tran;
{
/* check proper gks operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqntran); /*c1147*/

/* check for valid trans number */
        GKSERROR( (num < 0 || num > MAX_TRANS), 50, errginqntran);

/* set up the return values */
        *tran = xgks_state.ntrans_list[num].ntrans;     /*c1147*/

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

Gint ginqclip(clipping)
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
 * Gint *maxtran;                       maximum transformation number available
 *
 * returns all information in the parameters
 * errors 0, 8
 *
 * See also: ANSI Standard p.144
 */

Gint ginqmaxntrannum(maxtran)
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
 *              INQUIRE WORKSTATION TRANSFORMATION
 *
 * Gint ws_id;                  workstation identifier
 * Gwsti *wstran;               OUT requested and current transformations
 *
 * NOTE: Each of the four pointers above must point to an array of size 2 of
 *      the appropriate type.  The first element will get the lower left corner
 *      and the second element will get the upper right corner.
 *
 * returns all information in the parameters
 * errors 0, 7, 20, 25, 33, 36
 *
 * See also: ANSI Standard p.162
 */

Gint ginqwstran(ws_id, wstran)
Gint ws_id;
Gwsti *wstran;
{
        WS_STATE_PTR ws;

/* check proper gks operating state */
        GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqwstran); /*c1147*/

/* check for invalid workstation id */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqwstran)   /* c1012 */

/* check for open ws_id */
        GKSERROR( ((ws=OPEN_WSID(ws_id)) == NULL), 25, errginqwstran);  /* c1012 */

/* proper category */
        GKSERROR( (ws->ewstype == MI), 33, errginqwstran);
        GKSERROR( (ws->ewstype == WISS), 36, errginqwstran);

/* set up the return values */
        *wstran = ws->wsti;

        return( OK );
}
