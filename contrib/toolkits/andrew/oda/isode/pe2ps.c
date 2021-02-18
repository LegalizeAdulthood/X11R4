/*
Copyright 1988, 1989 Marshall T. Rose

The ISODE is openly available but is NOT in the public domain. You are
allowed and encouraged to take this software and build commercial
products. However, as a condition of use, you are required to "hold
harmless" all contributors.
Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted,
provided that this notice and the reference to this notice appearing
in each software module be retained unaltered, and that the name of
any contributors shall not be used in advertising or publicity pertaining
to distribution of the software without specific written prior permission.
No contributor makes any representations about the suitability of this
software for any purpose. It is provided "as is" without express or
implied warranty.

ALL CONTRIBUTORS DISCLAIM ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
WARRANTIES OR MERCHANTIBILITY AND FITNESS. IN
NO EVENT SHALL ANY CONTRIBUTOR BE LIABLE FOR
ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFWTARE.

As used above, "contributor" includes, but is not limited to:

	The MITRE Corporation
	The Northrop Corporation
	University College London
	The University of Nottingham
	The Wollongong Group, Inc.
	Marshall T. Rose

In particular, the Northrop Coporation provided the initial
sponsorship for the ISODE and teh Wollongong Group, Inc.,
continued supporting this effort.
*/
/* pe2ps.c - presentation element to presentation stream */

#ifndef	lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/pe2ps.c,v 1.1 89/09/21 05:49:32 mss Exp $";
#endif

/* 
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/pe2ps.c,v 1.1 89/09/21 05:49:32 mss Exp $
 *
 *
 * $Log:	pe2ps.c,v $
 * Revision 1.1  89/09/21  05:49:32  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:54:10  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/11  16:15:05  mss
 * added copyright
 * 
 * Revision 1.1  89/07/27  13:03:00  mss
 * Initial revision
 * 
 * Revision 1.1  89/05/16  17:28:10  mss
 * Initial revision
 * 
 * Revision 6.0  89/03/18  23:38:36  mrose
 * Release 5.0
 * 
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


/* LINTLIBRARY */

#include <stdio.h>
#include "psap.h"
#include "tailor.h"

/*    DATA */

static PElement pe_eoc = { PE_CLASS_UNIV, PE_FORM_PRIM, PE_UNIV_EOC, 0 };

/*  */

int	pe2ps_aux (ps, pe, eval)
register PS	ps;
register PE	pe;
int	eval;
{
    int     result;

    if (eval)
	switch (pe -> pe_form) {
	    case PE_FORM_PRIM: 
	    case PE_FORM_ICONS: 
		break;

	    case PE_FORM_CONS: 
		(void) ps_get_abs (pe);
		break;
	}

    if ((result = pe2ps_aux2 (ps, pe)) != NOTOK)
	result = ps_flush (ps);

    return result;
}


static int  pe2ps_aux2 (ps, pe)
register PS	ps;
register PE	pe;
{
    register PE	    p;

    if (pe -> pe_form == PE_FORM_ICONS) {
	if (ps_write_aux (ps, pe -> pe_prim, pe -> pe_len, 1) == NOTOK)
	    return NOTOK;

	return OK;
    }
    
    if (ps_write_id (ps, pe) == NOTOK || ps_write_len (ps, pe) == NOTOK)
	return NOTOK;

    switch (pe -> pe_form) {
	case PE_FORM_PRIM: 
	    if (ps_write_aux (ps, pe -> pe_prim, pe -> pe_len, 1) == NOTOK)
		return NOTOK;
	    break;

	case PE_FORM_CONS: 
	    if (pe -> pe_len) {
		for (p = pe -> pe_cons; p; p = p -> pe_next)
		    if (pe2ps_aux2 (ps, p) == NOTOK)
			return NOTOK;

		if (pe -> pe_len == PE_LEN_INDF
			&& pe2ps_aux2 (ps, &pe_eoc) == NOTOK)
		    return NOTOK;
	    }
	    break;
    }

    return OK;
}

/*  */

static int  ps_write_id (ps, pe)
register PS	ps;
register PE	pe;
{
    byte    buffer[1 + sizeof (PElementID)];
    register byte  *bp = buffer;
    PElementForm    form;
    register PElementID id;

    if ((form = pe -> pe_form) == PE_FORM_ICONS)
	form = PE_FORM_CONS;
    *bp = ((pe -> pe_class << PE_CLASS_SHIFT) & PE_CLASS_MASK)
		| ((form << PE_FORM_SHIFT) & PE_FORM_MASK);

    if ((id = pe -> pe_id) < PE_ID_XTND)
	*bp++ |= id;
    else {
	register byte *ep;
	register PElementID jd;

	*bp |= PE_ID_XTND;

	ep = buffer;
	for (jd = id; jd != 0; jd >>= PE_ID_SHIFT)
	    ep++;

	for (bp = ep; id != 0; id >>= PE_ID_SHIFT)
	    *bp-- = id & PE_ID_MASK;
	for (bp = buffer + 1; bp < ep; bp++)
	    *bp |= PE_ID_MORE;

	bp = ++ep;
    }

    if (ps_write (ps, buffer, bp - buffer) == NOTOK)
	return NOTOK;

    return OK;
}

/*  */

/* probably should integrate the non-PE_LEN_SMAX case with the algorithm in
   num2prim() for a single, unified routine */

static int  ps_write_len (ps, pe)
register PS	ps;
register PE	pe;
{
    byte    buffer[1 + sizeof (PElementLen)];
    register byte  *bp = buffer,
		   *ep;
    register PElementLen len;

    if ((len = pe -> pe_len) == PE_LEN_INDF)
	*bp++ = PE_LEN_XTND;
    else
	if (len <= PE_LEN_SMAX)
	    *bp++ = len & 0xff;
	else {
	    ep = buffer + sizeof buffer - 1;
	    for (bp = ep; len != 0 && buffer < bp; len >>= 8)
		*bp-- = len & 0xff;
	    *bp = PE_LEN_XTND | ((ep - bp) & 0xff);
	    if (ps_write (ps, bp, ep - bp + 1) == NOTOK)
		return NOTOK;

	    return OK;
	}

    if (ps_write (ps, buffer, bp - buffer) == NOTOK)
	return NOTOK;

    return OK;
}
