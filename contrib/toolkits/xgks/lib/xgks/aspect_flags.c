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
 * aspect_flags.c - functions dealing with the Aspect Source Flages.
 *              s_aspect_flags();
 */

#include "gks_implem.h"
#include "metafile.h"

/*$F
 * gsetasf (asf) - SET ASPECT SOURCE FLAGS
 * Gasfs  *asf;    Array of the 13 aspect flags INDIVIDUAL | BUNDLED.
 *
 * returns: 0=OK or 8
 *
 * See Also: ANSI Standard p.101
 */
Gint gsetasf(Gasfs *asf)
{
/* check for proper gks state */
        GKSERROR ((xgks_state.gks_state == GGKCL), 8, errgsetasf)       /*c1147*/

/* Set the asf in gks list */
        xgks_state.gks_lnattr.type      = asf->ln_type;         /*c1147*/
        xgks_state.gks_lnattr.width     = asf->ln_width;        /*c1147*/
        xgks_state.gks_lnattr.colour    = asf->ln_colour;       /*c1147*/
        xgks_state.gks_mkattr.type      = asf->mk_type;         /*c1147*/
        xgks_state.gks_mkattr.size      = asf->mk_size;         /*c1147*/
        xgks_state.gks_mkattr.colour    = asf->mk_colour;       /*c1147*/
        xgks_state.gks_txattr.fp        = asf->tx_fp;           /*c1147*/
        xgks_state.gks_txattr.tx_exp    = asf->tx_exp;          /* c1179 c1147*/
        xgks_state.gks_txattr.space     = asf->tx_space;        /*c1147*/
        xgks_state.gks_txattr.colour    = asf->tx_colour;       /*c1147*/
        xgks_state.gks_flattr.inter     = asf->fl_inter;        /*c1147*/
        xgks_state.gks_flattr.style     = asf->fl_style;        /*c1147*/
        xgks_state.gks_flattr.colour    = asf->fl_colour;       /*c1147*/

        if (MO_OPENED == TRUE) XgksMoSetAsf();

        return(0);
}

/*
 * XgksInitGksAsf() - give gks_asfs the value GINDIVIDUAL.
 */
void XgksInitGksAsf(void)
{
        xgks_state.gks_lnattr.type      = GINDIVIDUAL;          /*c1147*/
        xgks_state.gks_lnattr.width     = GINDIVIDUAL;          /*c1147*/
        xgks_state.gks_lnattr.colour    = GINDIVIDUAL;          /*c1147*/
        xgks_state.gks_mkattr.type      = GINDIVIDUAL;          /*c1147*/
        xgks_state.gks_mkattr.size      = GINDIVIDUAL;          /*c1147*/
        xgks_state.gks_mkattr.colour    = GINDIVIDUAL;          /*c1147*/
        xgks_state.gks_txattr.fp        = GINDIVIDUAL;          /*c1147*/
        xgks_state.gks_txattr.tx_exp    = GINDIVIDUAL;          /* c1179 c1147*/
        xgks_state.gks_txattr.space     = GINDIVIDUAL;          /*c1147*/
        xgks_state.gks_txattr.colour    = GINDIVIDUAL;          /*c1147*/
        xgks_state.gks_flattr.inter     = GINDIVIDUAL;          /*c1147*/
        xgks_state.gks_flattr.style     = GINDIVIDUAL;          /*c1147*/
        xgks_state.gks_flattr.colour    = GINDIVIDUAL;          /*c1147*/
}
