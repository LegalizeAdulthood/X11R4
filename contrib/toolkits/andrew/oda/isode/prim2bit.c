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
/* prim2bit.c - presentation element to bit string */

#ifndef	lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/prim2bit.c,v 1.1 89/09/21 05:50:35 mss Exp $";
#endif

/* 
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/prim2bit.c,v 1.1 89/09/21 05:50:35 mss Exp $
 *
 *
 * $Log:	prim2bit.c,v $
 * Revision 1.1  89/09/21  05:50:35  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:55:23  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/11  16:16:10  mss
 * added copyright
 * 
 * Revision 1.1  89/07/27  13:03:13  mss
 * Initial revision
 * 
 * Revision 1.1  89/05/16  17:28:45  mss
 * Initial revision
 * 
 * Revision 6.0  89/03/18  23:38:49  mrose
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

/*  */

PE	prim2bit (pe)
register PE	pe;
{
    int	    i;
    register PElementData bp;
    register PElementLen len;
    register PE	    p;

    switch (pe -> pe_form) {
	case PE_FORM_PRIM:	/* very paranoid... */
	    if ((bp = pe -> pe_prim) && (len = pe -> pe_len)) {
		if ((i = *bp & 0xff) > 7)
		    return pe_seterr (pe, PE_ERR_BITS, NULLPE);
		pe -> pe_nbits = ((len - 1) * 8) - i;
	    }
	    else
		pe -> pe_nbits = 0;
	    break;

	case PE_FORM_CONS: 
	    pe -> pe_nbits = 0;
	    for (p = pe -> pe_cons; p; p = p -> pe_next) {
		if (prim2bit (p) == NULLPE)
		    return NULLPE;
		pe -> pe_nbits += p -> pe_nbits;
	    }
	    break;
    }

    return pe;
}
