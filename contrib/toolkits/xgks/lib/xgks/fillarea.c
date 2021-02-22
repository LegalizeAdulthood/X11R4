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
 * fillarea.c - functions and data for gks fill areas.
 *              gfillarea ()
 *              gsetfillcolourind ()
 *              gsetfillind ()
 *              gsetfillintstyle ()
 *              gsetfillrep ()
 *              gsetfillstyleind ()
 *              gsetpatrefpt()
 *              gsetpatrep()
 *              gsetpatsize()
 *
 * utility routines:
 *              XgksInitGksFillArea()
 *              XgksInitWssFillArea()
 *
 */

#include "gks_implem.h"

#include "fillarea.h"

void XgksMoGraphicOutput(Gint code, Gint num_pt, Gpoint *pos);
void XgksMoSetGraphicAttr(Gint code, Gint attr);
void XgksMoSetFillIntStyle(Gflinter style);
void XgksMoSetFillRep(WS_STATE_PTR ws, Gint idx, Gflbundl *rep);
void XgksMoSetPatRef(void);
void XgksMoSetPatSize(void);
void XgksMoSetPatRep(WS_STATE_PTR ws, Gint idx, Gptbundl *rep);

/*
 * XgksInitGksFillArea: initialise gks state list fill area stuff
 */

void XgksInitGksFillArea(void)
{
        xgks_state.gks_flattr.fill   = 1;               /* fill index */
        xgks_state.gks_flattr.style  = GINDIVIDUAL;     /* style ASF  */
        xgks_state.gks_flattr.inter  = GINDIVIDUAL;     /* inter ASF  */
        xgks_state.gks_flattr.colour = GINDIVIDUAL;     /* colour ASF */
        /* fill bundle */
        xgks_state.gks_flattr.bundl.inter  = def_flbundl[0].inter;
        xgks_state.gks_flattr.bundl.style  = def_flbundl[0].style;
        xgks_state.gks_flattr.bundl.colour = def_flbundl[0].colour;

        xgks_state.gks_ptattr.widthvec  = def_ptattr.widthvec;
        xgks_state.gks_ptattr.heightvec = def_ptattr.heightvec;
        xgks_state.gks_ptattr.ptp       = def_ptattr.ptp;
}
/*
 * XgksInitWssFillArea(ws) - send predefined attributes and Bundle index to
 *      newly opened workstation ws;
 */


/* The data for predefined patterns */

static
Gint pttn0[] = {1, 0, 0, 0,
                1, 1, 0, 0,
                1, 1, 1, 0,
                1, 1, 1, 1};
static
Gint pttn1[] = {1, 1, 1, 1,
                1, 0, 0, 1,
                1, 0, 0, 1,
                1, 1, 1, 1};
static
Gint pttn2[] = {0, 0, 0, 0,
                0, 1, 1, 0,
                0, 1, 1, 0,
                0, 0, 0, 0};
static
Gint pttn3[] = {0, 0, 1, 1,
                0, 0, 1, 1,
                1, 1, 0, 0,
                1, 1, 0, 0};
static
Gint pttn4[] = {1, 0, 0, 0,
                1, 0, 0, 0,
                1, 0, 0, 0,
                1, 0, 0, 0};
static
Gint pttn5[] = {0, 0, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0,
                1, 1, 1, 1};
static
Gint pttn6[] = {1, 1, 1, 0,
                1, 1, 1, 0,
                1, 1, 1, 0,
                1, 1, 1, 0};
static
Gint pttn7[] = {1, 1, 1, 1,
                1, 1, 1, 1,
                1, 1, 1, 1,
                0, 0, 0, 0};
static
Gint pttn8[] = {1, 1, 1, 0,
                1, 1, 0, 1,
                1, 0, 1, 1,
                0, 1, 1, 1};
static
Gint pttn9[] = {0, 0, 0, 1,
                0, 0, 1, 0,
                0, 1, 0, 0,
                1, 0, 0, 0};
static
Gint pttn10[] = {0, 1, 1, 1,
                1, 0, 1, 1,
                1, 1, 0, 1,
                1, 1, 1, 0};
static
Gint pttn11[] = {1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1};
static
Gint pttn12[] = {1, 0, 0, 0, 0, 0, 0, 0,
                1, 0, 0, 0, 0, 0, 0, 0,
                1, 0, 0, 0, 0, 0, 0, 0,
                1, 0, 0, 0, 0, 0, 0, 0,
                1, 0, 0, 0, 0, 0, 0, 0,
                1, 0, 0, 0, 0, 0, 0, 0,
                1, 0, 0, 0, 0, 0, 0, 0,
                1, 1, 1, 1, 1, 1, 1, 1};
static
Gint pttn13[] = {1, 0, 0, 0, 0, 0, 0, 1,
                0, 1, 0, 0, 0, 0, 1, 0,
                0, 0, 1, 0, 0, 1, 0, 0,
                0, 0, 0, 1, 1, 0, 0, 0,
                0, 0, 0, 1, 1, 0, 0, 0,
                0, 0, 1, 0, 0, 1, 0, 0,
                0, 1, 0, 0, 0, 0, 1, 0,
                1, 0, 0, 0, 0, 0, 0, 1};
static
Gint pttn14[] = {0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                1, 1, 1, 1, 1, 1, 1, 1,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                1, 1, 1, 1, 1, 1, 1, 1};
static
Gint pttn15[] = {1, 0, 0, 1, 0, 0, 0, 0,
                1, 0, 0, 1, 0, 0, 0, 0,
                1, 0, 0, 1, 0, 0, 0, 0,
                1, 0, 0, 1, 0, 0, 0, 0,
                1, 0, 0, 1, 0, 0, 0, 0,
                1, 0, 0, 1, 0, 0, 0, 0,
                1, 0, 0, 1, 0, 0, 0, 0,
                1, 0, 0, 1, 0, 0, 0, 0};
static
Gint pttn16[] = {
        1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1,
        1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1,
        1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
        0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1,
        1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0,
        1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0,
        1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1,
        1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1,
        1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1,
        1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
        0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1,
        1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0,
        1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0,
        1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1
};
static
Gint pttn17[] = {
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
        1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1,
        1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1,
        1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
        0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0,
        0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0,
        1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
        1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1,
        1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1,
        1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
        0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0,
        0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0,
        1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0
};
static
Gint pttn18[] = {
        1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0,
        1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1,
        0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0,
        0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1,
        1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
        1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0,
        1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0,
        1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0,
        1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1,
        0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0,
        0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1,
        1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
        1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0,
        1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0
};
static
Gint pttn19[] = {
        1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
        0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
        1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
        0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
        0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1
};
Gptbundl xgks_xpttn[] = {
        {{4, 4}, pttn0},
        {{4, 4}, pttn1},
        {{4, 4}, pttn2},
        {{4, 4}, pttn3},
        {{4, 4}, pttn4},
        {{4, 4}, pttn5},
        {{4, 4}, pttn6},
        {{4, 4}, pttn7},
        {{4, 4}, pttn8},
        {{4, 4}, pttn9},
        {{4, 4}, pttn10},
        {{4, 4}, pttn11},
        {{8, 8}, pttn12},
        {{8, 8}, pttn13},
        {{8, 8}, pttn14},
        {{8, 8}, pttn15},
        {{16, 16}, pttn16},
        {{16, 16}, pttn17},
        {{16, 16}, pttn18},
        {{16, 16}, pttn19},
        {{4, 4}, pttn0}
};


Gint XgksInitWssFillArea(WS_STATE_PTR ws)
{
        Gint i, j, k;

        /* flbundl_table[0] never used but initialized anyway */
        ws->flbundl_table[0] = def_flbundl[0];

        /* initialize predefined representation bundles */
        for (i=1; i<=PDF_FILL_BNDLS; i++)
           /* use implementation defaults, not current attributes */
           ws->flbundl_table[i] = def_flbundl[i-1];

        /* initialize rest of representation bundle array */
        for (i=PDF_FILL_BNDLS+1; i<MAX_BUNDL_TBL; i++)
           ws->flbundl_table[i] = def_flbundl[0];

        for (i=0; i<MAX_BUNDL_TBL; i++) {                  /* set default pattern table values */
                j = xgks_xpttn[i].size.x * xgks_xpttn[i].size.y;
                ws->ptbundl_table[i] = xgks_xpttn[i];
                ws->ptbundl_table[i].array = (Gint *)malloc( j * sizeof(int));
                GKSERROR( (ws->ptbundl_table[i].array == NULL), 300, errXgksInitWssFillArea);

                for (k=0; k < j; k++)
                  ws->ptbundl_table[i].array[k] = xgks_xpttn[i].array[k];
        }
        return 0;
}

/*$F
 * gfillarea(num_pts, pts) - FILL AREA
 *
 * Gint num_pts;                Number of points that will define a polygon, >2
 * Gpoint *pts;                 Array of those points.
 *
 * returns 0=OK or one of 5, 100.
 *
 * See Also: ANSI Standard p.83
 */
Gint gfillarea(Gint num_pts, Gpoint *pts)
{
        Gpoint *ndc_pt;
        OUT_PRIMI *flarea;
        Gint cnt;

/* check for proper operating state */
        GKSERROR ((xgks_state.gks_state != GWSAC && xgks_state.gks_state != GSGOP), 5, errgfillarea)

/* check for a valid number of points */
        GKSERROR ((num_pts < 3) , 100, errgfillarea)

/* open an primitive structure */
        GKSERROR (( (flarea = XgksNewPrimi()) == NULL ), 300, errgfillarea);

        flarea->pid = FILL_AREA;
        flarea->primi.fill_area.num_pts = num_pts;

/* now get memory for transformed points */
        GKSERROR ((((flarea->primi.fill_area.pts) =
                  (Gpoint *) malloc((unsigned)num_pts * sizeof(Gpoint))) == NULL) , 300, errgfillarea)

        ndc_pt = flarea->primi.fill_area.pts;
/* transform the WC to NDC */
        for (cnt=0; cnt<num_pts; cnt++, pts++, ndc_pt++)
                WcToNdc(pts, ndc_pt);   /* convert to ndc coordinate */

        flarea->primi.fill_area.flattr = xgks_state.gks_flattr;
        flarea->primi.fill_area.ptattr = xgks_state.gks_ptattr;

        XgksProcessPrimi(flarea);

        if (MO_OPENED == TRUE) XgksMoGraphicOutput (14, num_pts, flarea->primi.fill_area.pts);

        free (flarea->primi.fill_area.pts);
        free (flarea);

        return(0);
}

/*$F
 * gsetfillcolourind (colour) - SET FILL AREA COLOUR INDEX
 *
 * Gint colour;         new colour for INDIVIDUALly specified fill areas.
 *
 * returns 0=OK, or one of 8, 92
 *
 * See Also: ANSI Standard p.98
 */
Gint gsetfillcolourind(Gint colour)
{
/* check for proper operating state */
        GKSERROR ((xgks_state.gks_state == GGKCL) , 8, errgsetfillcolourind)

/* check for valid colour index */
        GKSERROR ((colour < 0) , 92, errgsetfillcolourind)

/* change the colour */
        xgks_state.gks_flattr.bundl.colour = colour;

        if (MO_OPENED == TRUE) XgksMoSetGraphicAttr (40, colour);

        return(0);
}

/*$F
 * gsetfillind (idx) - SET FILL AREA INDEX
 *
 * Gint idx;            new fill area bundle entry for INDIVIDUAL aspects.
 *
 * returns 0=OK, or one of 8, 80
 *
 * See Also: ANSI Standard p.96
 */
Gint gsetfillind(Gint idx)
{
/* check for proper operating state */
        GKSERROR ((xgks_state.gks_state == GGKCL) , 8, errgsetfillind)

/* check for valid index */
        GKSERROR ((idx < 1 ), 80, errgsetfillind)

/* ok to change the idx */
        xgks_state.gks_flattr.fill = idx;

        if (MO_OPENED == TRUE) XgksMoSetGraphicAttr (37, idx);

        return(0);
}

/*$F
 * gsetfillintstyle(style) - SET FILL AREA INTERIOR STYLE
 *
 * Gflinter style;              new style for INDIVIDUALly specified fill areas
 *                              GHOLLOW | GSOLID | GPATTERN | GHATCH
 *
 * returns 0=OK, or 8
 *
 * See Also: ANSI Standard p.96
 */
Gint gsetfillintstyle(Gflinter style)
{
/* check for proper operating state */
        GKSERROR ((xgks_state.gks_state == GGKCL) , 8, errgsetfillintstyle)

/* check for valid style */
        if (style != GHOLLOW && style != GSOLID && style != GPATTERN && style != GHATCH )
                style = GHOLLOW;

        xgks_state.gks_flattr.bundl.inter = style;

        if (MO_OPENED == TRUE) XgksMoSetFillIntStyle (style);

        return(0);
}

/*$F
 * gsetfillrep (ws_id, idx, rep) - SET FILL AREA REPRESENTATION
 *
 * Gint ws_id;                  workstation identifier.
 * Gint idx,                    fill area bundle entry to set.
 * Gflbundl *rep                fill area bundle info
 *
 * returns 0=OK, or one of 7, 20, 25, 33, 35, 36, 80, 83, 85, 86, 93, 2000
 *
 * See Also: ANSI Standard p.103
 */
Gint gsetfillrep(Gint ws_id, Gint idx, Gflbundl *rep)
{
        WS_STATE_PTR ws;

/* check for proper gks state */
        GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP) , 7, errgsetfillrep)

/* check for invalid workstation id */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgsetfillrep)

/* check for open workstation identifier */
        GKSERROR (((ws=OPEN_WSID(ws_id)) == NULL) , 25, errgsetfillrep)

/* Check for valid workstation category */
        GKSERROR ((WS_CAT(ws) == GMI), 33, errgsetfillrep)
        GKSERROR ((WS_CAT(ws) == GINPUT), 35, errgsetfillrep)
        GKSERROR ((WS_CAT(ws) == GWISS), 36, errgsetfillrep)

/* check for valid fill area bundle table index */
        GKSERROR ((idx < 1 || idx >= MAX_BUNDL_TBL) , 80, errgsetfillrep)

/* check for valid interior style */
        GKSERROR((rep->inter != GHOLLOW && rep->inter != GSOLID && rep->inter != GPATTERN && rep->inter != GHATCH ) , 83, errgsetfillrep)

/* check for style index equal to zero */
        if ((rep->inter == GPATTERN) || (rep->inter == GHATCH))
                GKSERROR ((rep->style == 0), 84, errgsetfillrep);

/* valid pattern index */
        if (rep->inter == GPATTERN)
                GKSERROR ((!WS_FILL_TYPE(rep->inter,rep->style)), 85, errgsetfillrep);

/* valid hatch index */
        if (rep->inter == GHATCH)
                GKSERROR ((!WS_FILL_TYPE(rep->inter,rep->style)), 86, errgsetfillrep); /* c1174 */

/* check for valid colour index */
        GKSERROR ((!WS_AVAIL_COLOUR(ws,rep->colour)) , 93, errgsetfillrep)

        if (ws->ewstype == MO)
                XgksMoSetFillRep (ws, idx, rep);

/* finally get to tell the workstation */
        ws->flbundl_table[idx] = (*rep);

        return(0);
}

/*$F
 * gsetfillstyleind (idx) - SET FILL AREA STYLE INDEX
 *
 * Gint idx;    new fill area style index for INDIVIDUALly specified fills
 *
 * returns 0=OK, or one of 8, 84
 *
 * See Also: ANSI Standard p.97
 */
Gint gsetfillstyleind(Gint idx)
{
/* check for proper operating state */
        GKSERROR ((xgks_state.gks_state == GGKCL) , 8, errgsetfillstyleind)

/* check for valid style index */
        GKSERROR ((idx == 0) , 84, errgsetfillstyleind)

/* ok to proceed */
        xgks_state.gks_flattr.bundl.style = idx;

        if (MO_OPENED == TRUE) XgksMoSetGraphicAttr (39, idx);

        return(0);
}

/*$F
 * gsetpatrefpt (ref) - SET PATTERN REFERENCE POINT
 *
 * Gpoint *ref; new pattern reference point
 *
 * returns 0=OK, or 8
 *
 * See Also: ANSI Standard p.98
 */

Gint gsetpatrefpt(Gpoint *ref)
{
/* check for proper operating state */
        GKSERROR ((xgks_state.gks_state == GGKCL) , 8, errgsetpatrefpt )

/* ok to proceed */
        xgks_state.gks_ptattr.ptp = *ref;

        if (MO_OPENED == TRUE) XgksMoSetPatRef ();

        return(0);
}



/*$F
 * gsetpatsize (siz) - SET PATTERN SIZE
 *
 * Gpoint   siz;  new pattern size to be set in GKS state list.
 *
 * returns 0=OK, or one of 8, 87
 *
 * See Also: ANSI Standard p.98
 */
Gint gsetpatsize(Gpoint  *siz)
{
/* check for proper operating state */
        GKSERROR ((xgks_state.gks_state == GGKCL) , 8, errgsetpatsize )

/* check for valid style index */
        GKSERROR ( ((siz->x < 0) || (siz->y < 0)) , 87, errgsetpatsize )

/* ok to proceed */
        xgks_state.gks_ptattr.widthvec.x = siz->x;
        xgks_state.gks_ptattr.widthvec.y = 0.0;
        xgks_state.gks_ptattr.heightvec.x = 0.0;
        xgks_state.gks_ptattr.heightvec.y = siz->y;

        if (MO_OPENED == TRUE) XgksMoSetPatSize ();

        return(0);
}


/*$F
 * gsetpatrep(ws_id, idx, rep) - SET PATTERN REPRESENTATION
 *
 * Gint ws_id;                  workstation identifier.
 * Gint idx;                    pattern index.
 * Gptbundl *rep;               pattern representation pointer
 *
 * returns 0, 7, 20, 25, 33, 35, 36, 79, 83, 84, 86
 *
 * See Also: ANSI Standard p.104
 */

Gint gsetpatrep(Gint ws_id, Gint idx, Gptbundl *rep)
{
        WS_STATE_PTR ws;
        Gint i, j;
        Gint *slot;     /* c1102 */

/* check for proper gks state */
        GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP) , 7, errgsetpatrep)

/* check for invalid workstation id */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgsetpatrep)

/* check for open workstation */
        GKSERROR ((!(ws=OPEN_WSID(ws_id))) , 25, errgsetpatrep)

/* check proper workstation category */
        GKSERROR ((WS_CAT(ws) == GMI), 33, errgsetpatrep)
        GKSERROR ((WS_CAT(ws) == GWISS), 36, errgsetpatrep)

/* valid pattern index? */
        GKSERROR ((idx < 1 || idx >= MAX_BUNDL_TBL) , 85, errgsetpatrep)

/* valid dimensions */
        GKSERROR ((rep->size.x < 1 || rep->size.y < 1) , 91, errgsetpatrep) /*c1101*/

/* valid colour indices */                                /* c1102 */
        i = rep->size.x * rep->size.y;                   /* c1102 */
        for (j=0,slot=rep->array; j < i; j++,slot++)     /* c1102 */
           GKSERROR ((*slot < 0), 93, errgsetpatrep)      /* c1102 */

/* valid colour values? */
/*      i = ws->ptbundl_table[idx-1].size.x * ws->ptbundl_table[idx-1].size.y;
 *      for (j=0; j < i; j++)
 *              GKSERROR( !WS_AVAIL_COLOUR(ws,rep->array[j]), 93, errgsetpatrep);
 */

/* STEP 2: set the pattern bundle table values */
        free(ws->ptbundl_table[idx-1].array);
        ws->ptbundl_table[idx-1] = *rep;
        ws->ptbundl_table[idx-1].array = (Gint *)malloc( i * sizeof(int));
        GKSERROR( (ws->ptbundl_table[idx-1].array == NULL), 300, errgsetpatrep);

        for (j=0; j < i; j++)
                ws->ptbundl_table[idx-1].array[j] = rep->array[j];

        if (ws->ewstype == MO)
                XgksMoSetPatRep (ws, idx, rep);

        return (OK);
}
