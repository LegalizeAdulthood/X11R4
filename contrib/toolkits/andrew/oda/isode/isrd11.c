/* automatically generated by pepy 5.0 #1 (mss.andrew.cmu.edu), do not edit! */

#include <psap.h>

static char *pepyid = "pepy 5.0 #1 (mss.andrew.cmu.edu) of Mon May 22 20:44:32 EDT 1989";

#define	advise	PY_advise

void	advise ();

/* Generated from module ISRD11 */
# line 5 "isrd11.py"


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

#define TK_IMPL_ISRD

#include <isrd.h>

#undef TK_IMPL_ISRD

#define PEPYPARM struct isode2toolkit_t *


#ifndef PEPYPARM
#define PEPYPARM char *
#endif /* PEPYPARM */
extern PEPYPARM NullParm;

/* ARGSUSED */

int	parse_ISRD11_Char__Presentation__Features (pe, explicit, len, buffer, parm)
PE	pe;
int	explicit;
int    *len;
char  **buffer;
PEPYPARM parm;
{
#ifdef DEBUG
    (void) testdebug (pe, "ISRD11.Char-Presentation-Features");
#endif

    switch (PE_ID (pe -> pe_class, pe -> pe_id)) {
        case PE_ID (PE_CLASS_CONT, 0):	/* character-path */
            {
#ifdef DEBUG
                (void) testdebug (pe, "character-path");
#endif

                if (parse_ISRD9_One__Of__Four__Angles (pe, 0, NULLIP, NULLVP, NullParm) == NOTOK)
                    return NOTOK;
            }
            break;
        case PE_ID (PE_CLASS_CONT, 1):	/* line-progression */
            {
#ifdef DEBUG
                (void) testdebug (pe, "line-progression");
#endif

                if (parse_ISRD9_One__Of__Two__Angles (pe, 0, NULLIP, NULLVP, NullParm) == NOTOK)
                    return NOTOK;
            }
            break;
        case PE_ID (PE_CLASS_CONT, 2):	/* character-orientation */
            {
#ifdef DEBUG
                (void) testdebug (pe, "character-orientation");
#endif

                if (parse_ISRD9_One__Of__Four__Angles (pe, 0, NULLIP, NULLVP, NullParm) == NOTOK)
                    return NOTOK;
            }
            break;
        case PE_ID (PE_CLASS_CONT, 6):	/* character-spacing */
            {
#ifdef DEBUG
                (void) testdebug (pe, "character-spacing");
#endif

                if (prim2num (pe) == NOTOK
                        && pe -> pe_errno != PE_ERR_NONE) {
                    advise (NULLCP, "character-spacing bad integer: %s",
                            pe_error (pe -> pe_errno));
                    return NOTOK;
                }
            }
            break;
        case PE_ID (PE_CLASS_CONT, 7):	/* line-spacing */
            {
#ifdef DEBUG
                (void) testdebug (pe, "line-spacing");
#endif

                if (prim2num (pe) == NOTOK
                        && pe -> pe_errno != PE_ERR_NONE) {
                    advise (NULLCP, "line-spacing bad integer: %s",
                            pe_error (pe -> pe_errno));
                    return NOTOK;
                }
            }
            break;
        case PE_ID (PE_CLASS_CONT, 8):	/* alignment */
            {
#ifdef DEBUG
                (void) testdebug (pe, "alignment");
#endif

                if (parse_ISRD9_Alignment (pe, 0, NULLIP, NULLVP, NullParm) == NOTOK)
                    return NOTOK;
            }
            break;
        case PE_ID (PE_CLASS_CONT, 10):	/* graphic-rendition */
            {
#ifdef DEBUG
                (void) testdebug (pe, "graphic-rendition");
#endif

                if (parse_ISRD9_Graphic__Rendition (pe, 0, NULLIP, NULLVP, NullParm) == NOTOK)
                    return NOTOK;
            }
            break;
        case PE_ID (PE_CLASS_CONT, 13):	/* graphic-char-subrepertoire */
            {
#ifdef DEBUG
                (void) testdebug (pe, "graphic-char-subrepertoire");
#endif

                if (prim2num (pe) == NOTOK
                        && pe -> pe_errno != PE_ERR_NONE) {
                    advise (NULLCP, "graphic-char-subrepertoire bad integer: %s",
                            pe_error (pe -> pe_errno));
                    return NOTOK;
                }
            }
            break;
        case PE_ID (PE_CLASS_CONT, 17):	/* graphic-character-sets */
            {
#ifdef DEBUG
                (void) testdebug (pe, "graphic-character-sets");
#endif

            }
            break;

        default:
            advise (NULLCP, "Char-Presentation-Features has unknown choice: %s/%d/0x%x",
                    pe_classlist[pe -> pe_class], pe -> pe_form, pe -> pe_id);
            return NOTOK;
    }

    return OK;
}

/* ARGSUSED */

int	parse_ISRD11_Character__Content__Defaults (pe, explicit, len, buffer, parm)
register PE	pe;
int	explicit;
int    *len;
char  **buffer;
PEPYPARM parm;
{
    int p0_count = 0;
    register PE p0;

#ifdef DEBUG
    (void) testdebug (pe, "ISRD11.Character-Content-Defaults");
#endif

    if (explicit) {
        if (pe -> pe_class != PE_CLASS_UNIV
                || pe -> pe_form != PE_FORM_CONS
                || pe -> pe_id != PE_CONS_SET) {
            advise (NULLCP, "Character-Content-Defaults bad class/form/id: %s/%d/0x%x",
                    pe_classlist[pe -> pe_class], pe -> pe_form, pe -> pe_id);
            return NOTOK;
        }
    }
    else
        if (pe -> pe_form != PE_FORM_CONS) {
            advise (NULLCP, "Character-Content-Defaults bad form: %d", pe -> pe_form);
            return NOTOK;
        }

    if ((p0 = prim2set (pe)) == NULLPE) {
        advise (NULLCP, "Character-Content-Defaults bad set: %s",
                pe_error (pe -> pe_errno));
        return NOTOK;
    }
    pe = p0;

    if (p0 = set_find (pe, PE_CLASS_CONT, 0)) {
#ifdef DEBUG
        (void) testdebug (p0, "character-path");
#endif

        if (parse_ISRD9_One__Of__Four__Angles (p0, 0, NULLIP, NULLVP, NullParm) == NOTOK)
            return NOTOK;
        p0_count ++;
    }
    if (p0 = set_find (pe, PE_CLASS_CONT, 1)) {
#ifdef DEBUG
        (void) testdebug (p0, "line-progression");
#endif

        if (parse_ISRD9_One__Of__Two__Angles (p0, 0, NULLIP, NULLVP, NullParm) == NOTOK)
            return NOTOK;
        p0_count ++;
    }
    if (p0 = set_find (pe, PE_CLASS_CONT, 2)) {
#ifdef DEBUG
        (void) testdebug (p0, "character-orientation");
#endif

        if (parse_ISRD9_One__Of__Four__Angles (p0, 0, NULLIP, NULLVP, NullParm) == NOTOK)
            return NOTOK;
        p0_count ++;
    }
    if (p0 = set_find (pe, PE_CLASS_CONT, 6)) {
#ifdef DEBUG
        (void) testdebug (p0, "character-spacing");
#endif

        if (prim2num (p0) == NOTOK
                && p0 -> pe_errno != PE_ERR_NONE) {
            advise (NULLCP, "character-spacing bad integer: %s",
                    pe_error (p0 -> pe_errno));
            return NOTOK;
        }
        p0_count ++;
    }
    else {
        /* set default here using yp -> yp_default */
    }

    if (p0 = set_find (pe, PE_CLASS_CONT, 7)) {
#ifdef DEBUG
        (void) testdebug (p0, "line-spacing");
#endif

        if (prim2num (p0) == NOTOK
                && p0 -> pe_errno != PE_ERR_NONE) {
            advise (NULLCP, "line-spacing bad integer: %s",
                    pe_error (p0 -> pe_errno));
            return NOTOK;
        }
        p0_count ++;
    }
    else {
        /* set default here using yp -> yp_default */
    }

    if (p0 = set_find (pe, PE_CLASS_CONT, 8)) {
#ifdef DEBUG
        (void) testdebug (p0, "alignment");
#endif

        if (parse_ISRD9_Alignment (p0, 0, NULLIP, NULLVP, NullParm) == NOTOK)
            return NOTOK;
        p0_count ++;
    }
    if (p0 = set_find (pe, PE_CLASS_CONT, 10)) {
#ifdef DEBUG
        (void) testdebug (p0, "graphic-rendition");
#endif

        if (parse_ISRD9_Graphic__Rendition (p0, 0, NULLIP, NULLVP, NullParm) == NOTOK)
            return NOTOK;
        p0_count ++;
    }
    if (p0 = set_find (pe, PE_CLASS_CONT, 13)) {
#ifdef DEBUG
        (void) testdebug (p0, "graphic-char-subrepertoire");
#endif

        if (prim2num (p0) == NOTOK
                && p0 -> pe_errno != PE_ERR_NONE) {
            advise (NULLCP, "graphic-char-subrepertoire bad integer: %s",
                    pe_error (p0 -> pe_errno));
            return NOTOK;
        }
        p0_count ++;
    }
    else {
        /* set default here using yp -> yp_default */
    }

    if (p0 = set_find (pe, PE_CLASS_CONT, 15)) {
#ifdef DEBUG
        (void) testdebug (p0, "widow-size");
#endif

        if (prim2num (p0) == NOTOK
                && p0 -> pe_errno != PE_ERR_NONE) {
            advise (NULLCP, "widow-size bad integer: %s",
                    pe_error (p0 -> pe_errno));
            return NOTOK;
        }
        p0_count ++;
    }
    else {
        /* set default here using yp -> yp_default */
    }

    if (p0 = set_find (pe, PE_CLASS_CONT, 16)) {
#ifdef DEBUG
        (void) testdebug (p0, "orphan-size");
#endif

        if (prim2num (p0) == NOTOK
                && p0 -> pe_errno != PE_ERR_NONE) {
            advise (NULLCP, "orphan-size bad integer: %s",
                    pe_error (p0 -> pe_errno));
            return NOTOK;
        }
        p0_count ++;
    }
    else {
        /* set default here using yp -> yp_default */
    }

    if (p0 = set_find (pe, PE_CLASS_CONT, 17)) {
#ifdef DEBUG
        (void) testdebug (p0, "graphic-character-sets");
#endif

        p0_count ++;
    }
    if (p0 = set_find (pe, PE_CLASS_CONT, 19)) {
#ifdef DEBUG
        (void) testdebug (p0, "indentation");
#endif

        if (prim2num (p0) == NOTOK
                && p0 -> pe_errno != PE_ERR_NONE) {
            advise (NULLCP, "indentation bad integer: %s",
                    pe_error (p0 -> pe_errno));
            return NOTOK;
        }
        p0_count ++;
    }
    else {
        /* set default here using yp -> yp_default */
    }

    if (p0 = set_find (pe, PE_CLASS_CONT, 20)) {
#ifdef DEBUG
        (void) testdebug (p0, "kerning-offset");
#endif

        if (parse_ISRD9_Kerning__Offset (p0, 0, NULLIP, NULLVP, NullParm) == NOTOK)
            return NOTOK;
        p0_count ++;
    }
    if (p0 = set_find (pe, PE_CLASS_CONT, 21)) {
#ifdef DEBUG
        (void) testdebug (p0, "proportional-line-spacing");
#endif

        if (parse_ISRD9_Proportional__Line__Spacing (p0, 0, NULLIP, NULLVP, NullParm) == NOTOK)
            return NOTOK;
        p0_count ++;
    }
    if (p0 = set_find (pe, PE_CLASS_CONT, 22)) {
#ifdef DEBUG
        (void) testdebug (p0, "pairwise-kerning");
#endif

        if (parse_ISRD9_Pairwise__Kerning (p0, 0, NULLIP, NULLVP, NullParm) == NOTOK)
            return NOTOK;
        p0_count ++;
    }
    if (p0_count != pe -> pe_cardinal)
        advise (NULLCP, "warning: extra or duplicate members present in SET");

    return OK;
}
