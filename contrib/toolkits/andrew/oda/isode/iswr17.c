/* automatically generated by pepy 5.0 #1 (mss.andrew.cmu.edu), do not edit! */

#include <psap.h>

static char *pepyid = "pepy 5.0 #1 (mss.andrew.cmu.edu) of Mon May 22 20:44:32 EDT 1989";

#define	advise	PY_advise

void	advise ();

/* Generated from module ISWR17 */
# line 5 "iswr17.py"


/*
 *
 * (C) Copyright 1989 by Carnegie Mellon University
 *
 * Permission to use, copy, modify, and distribute these programs
 * and their documentation for any purpose and without fee is
 * hereby granted, provided that this copyright and permission
 * notice appear on all copies and supporting documentation, and
 * that the name of Carnegie Mellon University not be used in
 * advertising or publicity pertaining to distribution of the
 * programs without specific prior permission and notice be given
 * in supporting documentation that copying and distribution is
 * by permission of Carnegie Mellon University.
 *
 * Carnegie Mellon University makes no representations about the
 * suitability of this software for any purpose.  It is provided
 * as is, without express or implied warranty.
 *
 * Software by Ann Marks and James T. Lui,
 * Information Technology Center, Carnegie Mellon University,
 * except where noted.
 *
 */

#include <odatk.h>

#define TK_IMPL_ISWR

#include <iswr.h>

#undef TK_IMPL_ISWR

#define PEPYPARM struct isode2toolkit_t *


#ifndef PEPYPARM
#define PEPYPARM char *
#endif /* PEPYPARM */
extern PEPYPARM NullParm;

/* ARGSUSED */

int	build_ISWR17_Geo__Gr__Presentation__Features (pe, explicit, len, buffer, parm)
PE     *pe;
int	explicit;
int	len;
char   *buffer;
PEPYPARM parm;
{
    if (((*pe) = pe_alloc (PE_CLASS_UNIV, PE_FORM_PRIM, PE_PRIM_NULL)) == NULLPE) {
        advise (NULLCP, "Geo-Gr-Presentation-Features: out of memory");
        return NOTOK;
    }

#ifdef DEBUG
    (void) testdebug ((*pe), "ISWR17.Geo-Gr-Presentation-Features");
#endif


    return OK;
}

/* ARGSUSED */

int	build_ISWR17_Geo__Gr__Content__Defaults (pe, explicit, len, buffer, parm)
register PE     *pe;
int	explicit;
int	len;
char   *buffer;
PEPYPARM parm;
{
    PE	p0_z = NULLPE;
    register PE *p0 = &p0_z;

    if (((*pe) = pe_alloc (PE_CLASS_UNIV, PE_FORM_CONS, PE_CONS_SET)) == NULLPE) {
        advise (NULLCP, "Geo-Gr-Content-Defaults: out of memory");
        return NOTOK;
    }
    (*p0) = NULLPE;

    if ((*p0) != NULLPE)
        if (set_add ((*pe), (*p0)) == NOTOK) {
            advise (NULLCP, "Geo-Gr-Content-Defaults bad set: %s",
                    pe_error ((*pe) -> pe_errno));
            return NOTOK;
        }
    (*p0) = NULLPE;

    if ((*p0) != NULLPE)
        if (set_add ((*pe), (*p0)) == NOTOK) {
            advise (NULLCP, "Geo-Gr-Content-Defaults bad set: %s",
                    pe_error ((*pe) -> pe_errno));
            return NOTOK;
        }
    (*p0) = NULLPE;

    if ((*p0) != NULLPE)
        if (set_add ((*pe), (*p0)) == NOTOK) {
            advise (NULLCP, "Geo-Gr-Content-Defaults bad set: %s",
                    pe_error ((*pe) -> pe_errno));
            return NOTOK;
        }
    (*p0) = NULLPE;

    if ((*p0) != NULLPE)
        if (set_add ((*pe), (*p0)) == NOTOK) {
            advise (NULLCP, "Geo-Gr-Content-Defaults bad set: %s",
                    pe_error ((*pe) -> pe_errno));
            return NOTOK;
        }
    (*p0) = NULLPE;

    if ((*p0) != NULLPE)
        if (set_add ((*pe), (*p0)) == NOTOK) {
            advise (NULLCP, "Geo-Gr-Content-Defaults bad set: %s",
                    pe_error ((*pe) -> pe_errno));
            return NOTOK;
        }
    (*p0) = NULLPE;

    if ((*p0) != NULLPE)
        if (set_add ((*pe), (*p0)) == NOTOK) {
            advise (NULLCP, "Geo-Gr-Content-Defaults bad set: %s",
                    pe_error ((*pe) -> pe_errno));
            return NOTOK;
        }
    (*p0) = NULLPE;

    if ((*p0) != NULLPE)
        if (set_add ((*pe), (*p0)) == NOTOK) {
            advise (NULLCP, "Geo-Gr-Content-Defaults bad set: %s",
                    pe_error ((*pe) -> pe_errno));
            return NOTOK;
        }
    (*p0) = NULLPE;

    if ((*p0) != NULLPE)
        if (set_add ((*pe), (*p0)) == NOTOK) {
            advise (NULLCP, "Geo-Gr-Content-Defaults bad set: %s",
                    pe_error ((*pe) -> pe_errno));
            return NOTOK;
        }
    (*p0) = NULLPE;

    if ((*p0) != NULLPE)
        if (set_add ((*pe), (*p0)) == NOTOK) {
            advise (NULLCP, "Geo-Gr-Content-Defaults bad set: %s",
                    pe_error ((*pe) -> pe_errno));
            return NOTOK;
        }
    (*p0) = NULLPE;

    if ((*p0) != NULLPE)
        if (set_add ((*pe), (*p0)) == NOTOK) {
            advise (NULLCP, "Geo-Gr-Content-Defaults bad set: %s",
                    pe_error ((*pe) -> pe_errno));
            return NOTOK;
        }
    (*p0) = NULLPE;

    if ((*p0) != NULLPE)
        if (set_add ((*pe), (*p0)) == NOTOK) {
            advise (NULLCP, "Geo-Gr-Content-Defaults bad set: %s",
                    pe_error ((*pe) -> pe_errno));
            return NOTOK;
        }
    (*p0) = NULLPE;

    if ((*p0) != NULLPE)
        if (set_add ((*pe), (*p0)) == NOTOK) {
            advise (NULLCP, "Geo-Gr-Content-Defaults bad set: %s",
                    pe_error ((*pe) -> pe_errno));
            return NOTOK;
        }

#ifdef DEBUG
    (void) testdebug ((*pe), "ISWR17.Geo-Gr-Content-Defaults");
#endif


    return OK;
}
