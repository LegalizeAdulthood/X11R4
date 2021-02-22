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
 * All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: David Owens
 *
 * gdp.c - generalized drawing primitive code
 *
 *    ginqavailgdp - inquire available gdp's
 *    ginqgdp      - inquire specific gdp
 *    ggdp         - generalized drawing primitive
 *
 */

#include "gks_implem.h"

/*
 * ginqavailgdp(type,gdps) - INQUIRE LIST OF AVAILABLE GDP's
 *
 * Gchar    *type         type of workstation the inquiry is about
 * Ggdplist *gdps         OUT list of gdp's
 *
 * errors 8,22,23,39
 *
 * See also: ANSI standard p.181
 *
 */
Gint ginqavailgdp(Gchar *type, Ggdplist *gdps)
{
    EWSTYPE ewstype;

    /* check for proper state */
    GKSERROR((xgks_state.gks_state == GGKCL), 8, errginqavailgdp);

    /* check for valid and existing workstation */
    ewstype = XgksWsTypeToEnum(type);
    GKSERROR(ewstype == WST_INVALID, 22, errginqavailgdp);
    GKSERROR(ewstype == WST_INVALID, 23, errginqavailgdp);
    /* I know they both check the same thing.  The difference between */
    /* the two isn't clear                   TODO                     */

    /* check that ws is OUTPUT or OUTIN */
    GKSERROR(ewstype != X_WIN, 39, errginqavailgdp);

    /* there are no gdp's defined... */
    gdps->number = 0;
    gdps->functions = (Gint(*)()) NULL; /* pointer to a function */
    gdps->indices = (Gint *) NULL;

    return (OK);
}

/*
 * ginqgdp(type,function,fac) - INQUIRE GDP
 *
 * Gchar *type        type of workstation the inquiry is about
 * Gint  function     the GDP identifier
 * Gint  *fac         OUT list of sets of attributes used
 *
 * errors 8,22,23,39,41
 *
 * See also: ANSI standard p.181
 *
 */
Gint ginqgdp(Gchar *type, Gint function, Ggdpfac *fac)
{
    EWSTYPE ewstype;

    /* check for proper state */
    GKSERROR((xgks_state.gks_state == GGKCL), 8, errginqgdp);

    /* check for valid and existing workstation */
    ewstype = XgksWsTypeToEnum(type);
    GKSERROR(ewstype == WST_INVALID, 22, errginqgdp);
    GKSERROR(ewstype == WST_INVALID, 23, errginqgdp);
    /* I know they both check the same thing.  The difference between */
    /* the two isn't clear                   TODO                     */

    /* check that ws is OUTPUT or OUTIN */
    GKSERROR(ewstype != X_WIN, 39, errginqgdp);

    /* check that the ws can generate the specified gdp              */
    /* (until some gdp's are implimented, error 41 will be returned) */
    GKSERROR(TRUE, 41, errginqgdp);
}

/*
 * ggdp(npoints,points,function,data) - GENERALIZED DRAWING PRIMITIVE
 *
 * Gint    npoints  - number of points
 * Gpoint  *points  - points array
 * Gint    function - GDP identifier
 * Ggdprec *data    - GDP data record pointer
 *
 * errors 5,100,102,103,104,105
 *
 * See also: ANSI standard p.86
 */

Gint ggdp(Gint npoints, Gpoint *points, Gint function, Ggdprec *data)
{
    /* check for proper state */
    GKSERROR((xgks_state.gks_state != GWSAC && xgks_state.gks_state != GSGOP), 5, errggdp);

    /* check for "number of points is invalid" */
    GKSERROR((npoints < 0), 100, errggdp);

    /* check for "GDP identifier is invalid */
    GKSERROR((function <= 0 || function > 0), 102, errggdp);
    /* NOTE: Since there are no gdp's defined, none are valid.  Someday, when  */
    /*       there are some, the function>0 will be replaced with function>MAX */

    /* check for "content of gdp data record is invalid" */
    /* GKSERROR ( (????), 103, errggdp); */

    /* check for "at least one ws is not able to generate specified gdp */
    /* GKSERROR ( (????), 104, errggdp); */

    /* check for "at least one ws is not able to generate specified gdp under */
    /*     the current transformations and clipping rectangle"                */
    /* GKSERROR ( (????), 105, errggdp); */
}
