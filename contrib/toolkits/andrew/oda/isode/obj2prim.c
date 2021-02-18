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
/* obj2prim.c - object identifier to presentation element */

#ifndef	lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/obj2prim.c,v 1.1 89/09/21 05:48:04 mss Exp $";
#endif

/* 
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/obj2prim.c,v 1.1 89/09/21 05:48:04 mss Exp $
 *
 *
 * $Log:	obj2prim.c,v $
 * Revision 1.1  89/09/21  05:48:04  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:52:41  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/11  16:13:52  mss
 * added copyright
 * 
 * Revision 1.1  89/07/27  13:02:50  mss
 * Initial revision
 * 
 * Revision 1.1  89/05/16  17:27:53  mss
 * Initial revision
 * 
 * Revision 6.0  89/03/18  23:38:28  mrose
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

PE	obj2prim (o, class, id)
register OID	o;
PElementClass	class;
PElementID	id;
{
    register int    i,
		    m,
		    n,
		   *mp,
		   *np;
    register unsigned int j,
			 *ip;
    register PElementData dp,
			  ep;
    register PE	    pe;

    if (o == NULLOID || o -> oid_nelem <= 1)
	return NULLPE;

    if ((pe = pe_alloc (class, PE_FORM_PRIM, id)) == NULLPE)
	return NULLPE;

    if ((np = (int *) malloc ((unsigned) (o -> oid_nelem) * sizeof *np))
	    == NULL) {
	pe_free (pe);
	return NULLPE;
    }

    for (i = n = 0, ip = o -> oid_elements, mp = np;
	    i < o -> oid_nelem;
	    i++, ip++) {
	if (ip == o -> oid_elements)
	    j = *ip++ * 40, i++, j+= *ip;
	else
	    j = *ip;
	m = 0;
	do {
	    m++;
	    j >>= 7;
	}
	while (j);
	n += (*mp++ = m);
    }

    if ((pe -> pe_prim = PEDalloc (pe -> pe_len = n)) == NULLPED) {
	free ((char *) np);
	pe_free (pe);
	return NULLPE;
    }

    dp = pe -> pe_prim; 
    for (i = 0, ip = o -> oid_elements, mp = np;
	    i < o -> oid_nelem;
	    i++, ip++) {
	if (ip == o -> oid_elements)
	    j = *ip++ * 40, i++, j += *ip;
	else
	    j = *ip;

	ep = dp + (m = *mp++) - 1;
	for (dp = ep; m-- > 0; j >>= 7)
	    *dp-- = (j & 0x7f) | 0x80;
	*ep &= ~0x80;
	dp = ep + 1;
    }

    free ((char *) np);

    return pe;
}
