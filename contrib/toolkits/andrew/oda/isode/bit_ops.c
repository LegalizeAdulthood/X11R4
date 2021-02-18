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
/* bit_opts.c - operations on bit strings */

#ifndef	lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/bit_ops.c,v 1.1 89/09/21 05:22:01 mss Exp $";
#endif

/* 
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/bit_ops.c,v 1.1 89/09/21 05:22:01 mss Exp $
 *
 *
 * $Log:	bit_ops.c,v $
 * Revision 1.1  89/09/21  05:22:01  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:25:09  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/11  16:11:51  mss
 * added copyright
 * 
 * Revision 1.1  89/07/27  13:02:20  mss
 * Initial revision
 * 
 * Revision 1.1  89/05/16  17:27:26  mss
 * Initial revision
 * 
 * Revision 6.0  89/03/18  23:38:10  mrose
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


/* the first octet indicates how many unused bits are in the last octet --

	prim2bit  -	presentation element to bit string
	bit2prim  -	bit string to presentation element
	bit_on	  -	turn a bit ON
	bit_off	  -	turn a bit OFF
	bit_test  -	test a bit
 */


PElementData ffb (), ffb_aux ();
PE	ffb_pe ();

/*  */

int	bit_on (pe, i)
register PE	pe;
register int	i;
{
    int	    mask;
    register PElementData bp;

    if ((bp = ffb (pe, i, &mask, 1)) == NULLPED)
	return pe_seterr (pe, PE_ERR_NMEM, NOTOK);

    *bp |= mask;

    return OK;
}

/*  */

int	bit_off (pe, i)
register PE	pe;
register int	i;
{
    int	    mask;
    register PElementData bp;

    if ((bp = ffb (pe, i, &mask, 1)) == NULLPED)
	return pe_seterr (pe, PE_ERR_NMEM, NOTOK);

    *bp &= ~mask;

    return OK;
}


/*  */

int	bit_test (pe, i)
register PE	pe;
register int	i;
{
    int	    mask;
    register PElementData bp;

    if ((bp = ffb (pe, i, &mask, 0)) == NULLPED)
	return pe_seterr (pe, PE_ERR_BIT, NOTOK);

    return (*bp & mask ? 1 : 0);
}

/*  */

static PElementData ffb (pe, n, mask, xtnd)
register PE	pe;
register int	n,
	       *mask,
		xtnd;
{
    register int    len,
		    i;
    int     j;
    register PElementData bp;
    register PE    *p,
		    q,
		    r;

    i = (j = n) / 8 + 1;
    if ((bp = ffb_aux (pe, &j, mask)) != NULLPED || !xtnd)
	return bp;

    if (pe -> pe_form == PE_FORM_CONS)
	pe = ffb_pe (pe);

    switch (pe -> pe_form) {
	case PE_FORM_PRIM: 
	    if (pe -> pe_len < (PElementLen) (len = i + 1)) {
		if ((bp = PEDalloc (len)) == NULLPED)
		    return NULLPED;
		bzero ((char *) bp, len);
		if (pe -> pe_prim) {
		    PEDcpy (pe -> pe_prim, bp, pe -> pe_len);
		    if (pe -> pe_inline)
			pe -> pe_inline = 0;
		    else
			PEDfree (pe -> pe_prim);
		}
		pe -> pe_prim = bp, pe -> pe_len = len;
	    }
	    pe -> pe_nbits = n + 1;
	    *mask = 1 << (7 - (n % 8));
	    return (pe -> pe_prim + i);

	case PE_FORM_CONS: 
	    if ((r = pe_alloc (pe -> pe_class, PE_FORM_PRIM, pe -> pe_id))
		    == NULLPE)
		return NULLPED;
	    if ((r -> pe_prim = PEDalloc (len = r -> pe_len = j / 8 + 2))
		    == NULLPED) {
		pe_free (r);
		return NULLPED;
	    }
	    bzero ((char *) r -> pe_prim, len);
	    r -> pe_nbits = j + 1;
	    *mask = 1 << (7 - (j % 8));
	    for (p = &pe -> pe_cons; q = *p; p = &q -> pe_next)
		continue;
	    *p = r;
	    return (r -> pe_prim + len - 1);

	default:
	    return NULLPED;
    }
}

/*  */

static PElementData ffb_aux (pe, n, mask)
register PE	pe;
register int   *n,
	       *mask;
{
    register int    i,
		    nbits;
    register PElementData bp;
    register PE	    p;

    switch (pe -> pe_form) {
	case PE_FORM_PRIM: 
	    if ((nbits = pe -> pe_nbits) > (i = *n)) {
		*mask = 1 << (7 - (i % 8));
		return (pe -> pe_prim + i / 8 + 1);
	    }
	    *n -= nbits;
	    break;

	case PE_FORM_CONS: 
	    for (p = pe -> pe_cons; p; p = p -> pe_next)
		if ((bp = ffb_aux (p, n, mask)) != NULLPED)
		    return bp;
	    break;
    }

    return NULLPED;
}

/*  */

static PE	ffb_pe (pe)
register PE	pe;
{
    register PE	    p,
		    q;

    for (p = pe -> pe_cons, q = NULLPE; p; q = p, p = p -> pe_next)
	continue;

    if (q != NULLPE)
	switch (q -> pe_form) {
	    case PE_FORM_PRIM: 
		return q;

	    case PE_FORM_CONS: 
		return ffb_pe (q);
	}

    return pe;
}
