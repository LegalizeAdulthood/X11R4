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
 * polylines.c - all routines for ramtek gks polylines
 *              gpolyline()
 *              gsetlineind()
 *              gsetlinetype()
 *              gsetlinewidth()
 *              gsetlinecolour()
 *              gsetlinerep()
 *
 * utility routines:
 *              XgksInitGksPlines()
 *              XgksInitWssPlines()
 *
 */

#include "gks_implem.h"

void XgksMoGraphicOutput(Gint code, Gint num_pt, Gpoint *pos);
void XgksMoSetGraphicAttr(Gint code, Gint attr);
void XgksMoSetGraphicSize(Gint code, Gfloat size);
void XgksMoSetLineMarkRep(WS_STATE_PTR ws, Gint code, Gint idx, Gint type, Gint colour, Gfloat size);

/*
 * XgksInitGksPlines() -- initialise gks state table polyline stuff
 */

/* c1143:  Created predefined reps 1-5 */

/* c1147:  Moved predefined reps to polylines.h */
#include "polylines.h"                                                  /*c1147*/

void XgksInitGksPlines(void)
{
        xgks_state.gks_lnattr.line  = 1;    /* line indx */ /* c1143 */ /*c1147*/
        xgks_state.gks_lnattr.type  = GINDIVIDUAL; /* type ASF */ /* c1143 */ /*c1147*/
        xgks_state.gks_lnattr.width = GINDIVIDUAL; /* width ASF *//* c1143 */ /*c1147*/
        xgks_state.gks_lnattr.colour = GINDIVIDUAL;/* colour ASF *//* c1143 *//*c1147*/
        /* line bundle */                                          /* c1143 */
        xgks_state.gks_lnattr.bundl.type  = def_lnbundl[0].type; /* c1143 */ /*c1147*/
        xgks_state.gks_lnattr.bundl.width = def_lnbundl[0].width;/* c1143 */ /*c1147*/
        xgks_state.gks_lnattr.bundl.colour = def_lnbundl[0].colour;/* c1143 *//*c1147*/
}
/*
 * XgksInitWssPlines(ws) - Initialize workstation line bundle talbe
 */
void XgksInitWssPlines(WS_STATE_PTR ws)
{
        Gint i;                              /* Loop counter */

        /* c1143: lnbundl_table[0] never used but initialized anyway */
        ws->lnbundl_table[0] = def_lnbundl[0];                          /* c1143 */

        /* c1143: initialize predefined representation bundles */
        for (i=1; i<=PDF_LINE_BNDLS; i++)                               /* c1143 */
           /* c1075: use implementation defaults, not current attributes */
           ws->lnbundl_table[i] = def_lnbundl[i-1];                     /* c1143 */

        /* c1143: initialize rest of representation bundle array */
        for (i=PDF_LINE_BNDLS+1; i<MAX_BUNDL_TBL; i++)                  /* c1143 */
           ws->lnbundl_table[i] = def_lnbundl[0];                       /* c1143 */
}

/*$F
 * gpolyline(num_pts, pts) - POLYLINE
 *
 * Gint num_pts;                number of points to connect with a polyline.
 * Gpoint *pts;         address of an array of world coordinate points.
 *
 * returns: 0=OK, or one of 5, 100
 *
 * See also: ANSI standard p.82
 */
Gint gpolyline(Gint num_pts, Gpoint *pts)
{
        Gpoint *ndc_pt;                 /* temp space for tranformed points */
        int cnt;
        OUT_PRIMI  *line;

/* check for proper operating state */
        GKSERROR ((xgks_state.gks_state!=GWSAC && xgks_state.gks_state!=GSGOP) ,5, errgpolyline) /*c1147*/

/* check for at least two points */
        GKSERROR ((num_pts < 2) ,100, errgpolyline)

/* open an primitive structure */
        GKSERROR (( (line = XgksNewPrimi()) == NULL ) ,300, errgpolyline)

        line->pid = PLINE;
        line->primi.pline.num_pts = num_pts;

/* now get memory for transformed points */
        GKSERROR ((((line->primi.pline.pts) = (Gpoint *) malloc((unsigned)num_pts * sizeof(Gpoint))) == NULL) ,
                    300, errgpolyline)

        ndc_pt = line->primi.pline.pts;
/* transform the WC to NDC */
        for (cnt=0; cnt < num_pts; cnt++, pts++, ndc_pt++)
                WcToNdc(pts, ndc_pt);   /* convert to ndc coordinate */

        line->primi.pline.plnattr = xgks_state.gks_lnattr; /* copy SAF and individual values from GKS state list */ /*c1147*/
        XgksProcessPrimi(line);

        if (MO_OPENED == TRUE) XgksMoGraphicOutput (11, num_pts, line->primi.pline.pts);

        free (line->primi.pline.pts);
        free (line);
        return(0);
}

/*$F
 * gsetlineind(idx) - SET POLYLINE INDEX - selects the current bundle entry.
 *
 * Gint idx;            the new bundle table index to use for BUNDLED attributes
 *
 * returns: 0, 8, 60
 *
 * See also: ANSI Standard p.89
 */
Gint gsetlineind(Gint idx)
{
/* check for proper operating state */
        GKSERROR ((xgks_state.gks_state == GGKCL) ,8, errgsetlineind) /*c1147*/

/* check for valid polyline bundle table index */
        GKSERROR ((idx < 1) ,60, errgsetlineind)

        xgks_state.gks_lnattr.line = idx; /* change the bundle entry index (gks state table) */ /*c1147*/

        if (MO_OPENED == TRUE) XgksMoSetGraphicAttr (21, idx);

        return(0);
}

/*$F
 * gsetlinetype(type) - SET LINETYPE - changes the line style when the linetype
 *      asf is INDIVIDUAL.
 *
 * Gint type;           new style for linetype.
 *
 * returns: 0, 8, 63
 *
 * See also: ANSI Standard p.89
 */
Gint gsetlinetype(Gint type)
{
/* check for proper operating state */
        GKSERROR ((xgks_state.gks_state == GGKCL) ,8, errgsetlinetype) /*c1147*/

/* check for valid linetype */
        GKSERROR ((type == 0) ,63, errgsetlinetype)

/* change the gks state list */
        xgks_state.gks_lnattr.bundl.type = type;                /*c1147*/

        if (MO_OPENED == TRUE) XgksMoSetGraphicAttr (22, type);

        return(0);
}

/*$F
 * gsetlinewidth(width) - SET LINEWIDTH SCALE FACTOR - change the line width
 *      scale factor in the gks state list for INDIVIDUAL asf.
 *
 * Gfloat width;
 *
 * returns: 0, 8 65
 *
 * See also: ANSI Standard p.90
 */
Gint gsetlinewidth(Gfloat width)
{
/* check for proper operating state */
        GKSERROR ((xgks_state.gks_state == GGKCL) ,8, errgsetlinewidth) /*c1147*/

/* check for invalid width */
        GKSERROR ((width < 0) ,65, errgsetlinewidth)

/* now change the gks state table */
        xgks_state.gks_lnattr.bundl.width = width;              /*c1147*/

        if (MO_OPENED == TRUE) XgksMoSetGraphicSize (23, width);

        return(0);
}

/*$F
 * gsetlinecolourind(idx) - SET POLYLINE COLOUR INDEX - change the colour for
 *      INDIVIDUALly specified polylines.
 *
 * Gint idx;            the new colour index to use.
 *
 * returns: 0, 8, 92
 *
 * See also: ANSI Standard p.90
 */
Gint gsetlinecolourind(Gint idx)
{
/* check for proper operating state */
        GKSERROR ((xgks_state.gks_state == GGKCL) ,8, errgsetlinecolourind) /*c1147*/

/* check for invalid colour */
        GKSERROR ((idx < 0) ,92, errgsetlinecolourind)

/* change the gks state list */
        xgks_state.gks_lnattr.bundl.colour = idx;               /*c1147*/

        if (MO_OPENED == TRUE) XgksMoSetGraphicAttr (24, idx);

        return(0);
}

/*$F
 * gsetlinerep(ws_id, idx, rep) - SET POLYLINE REPRESENTATION
 *
 * Gint ws_id;                  workstation identifier.
 * Gint idx,                    polyline bundle entry to set.
 * Glndundl *rep;               line bundle entry pointer
 *
 * returns: 0, 7, 20, 25, 33, 35, 36, 60, 63, 64, 65, 93
 *
 * See also: ANSI Standard p.100
 */
Gint gsetlinerep(Gint ws_id, Gint idx, Glnbundl *rep)
{
        WS_STATE_PTR ws;

/* check for proper gks state */
        GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP) ,7, errgsetlinerep) /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgsetlinerep)   /* c1012 */

/* check for open ws_id */
/* DWO 7/26/88  changed macro name from VALID_WSID */
        GKSERROR (((ws=OPEN_WSID(ws_id)) == NULL) ,25, errgsetlinerep)  /* c1012 */

/* check for valid workstation category*/
        GKSERROR ( (WS_CAT(ws)==GMI), 33, errgsetlinerep);

        GKSERROR ( (WS_CAT(ws)==GINPUT), 35, errgsetlinerep);

        GKSERROR ( (WS_CAT(ws)==GWISS), 36, errgsetlinerep);

/* check for valid idx */
        GKSERROR ((idx < 1 || idx >= MAX_BUNDL_TBL) ,60, errgsetlinerep)

/* check for valid linetype */
        GKSERROR ((rep->type == 0) ,63, errgsetlinerep)

        GKSERROR ((!WS_LINE_TYPE(rep->type)) ,64, errgsetlinerep)

/* check for valid width */
        GKSERROR ((rep->width < 0) ,65, errgsetlinerep)

/* check for valid colour  */
        GKSERROR ((!WS_AVAIL_COLOUR(ws, rep->colour)) ,93, errgsetlinerep)

        if (ws->ewstype == MO)
                XgksMoSetLineMarkRep (ws, 51, idx, rep->type, rep->width, rep->colour);

/* tell the workstation */
        ws->lnbundl_table[idx] = (*rep);

        return( OK );
}
