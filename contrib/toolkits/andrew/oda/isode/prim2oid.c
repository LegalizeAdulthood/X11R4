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
/* prim2oid.c - presentation element to object identifier */

#ifndef	lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/prim2oid.c,v 1.1 89/09/21 05:51:13 mss Exp $";
#endif

/* 
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/prim2oid.c,v 1.1 89/09/21 05:51:13 mss Exp $
 *
 *
 * $Log:	prim2oid.c,v $
 * Revision 1.1  89/09/21  05:51:13  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:56:03  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/11  16:16:48  mss
 * added copyright
 * 
 * Revision 1.1  89/07/27  13:03:20  mss
 * Initial revision
 * 
 * Revision 1.1  89/05/16  17:28:51  mss
 * Initial revision
 * 
 * Revision 6.0  89/03/18  23:38:55  mrose
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

OID	prim2oid (pe)
register PE	pe;
{
    register unsigned int i,
			 *ip;
    register PElementData dp,
			  ep;
    static OIDentifier oid;
    register OID	o = &oid;

    if (pe -> pe_form != PE_FORM_PRIM
	    || (dp = pe -> pe_prim) == NULLPED
	    || pe -> pe_len == 0)
	return pe_seterr (pe, PE_ERR_PRIM, NULLOID);
    ep = dp + pe -> pe_len;

    if (o -> oid_elements)
	free ((char *) o -> oid_elements);

    for (i = 1; dp < ep; i++) {	/* another whacko ISO encoding... */
	if (*dp == 0x80)
	    return pe_seterr (pe, PE_ERR_OID, NULLOID);

	while (*dp++ & 0x80)
	    if (dp > ep)
		return pe_seterr (pe, PE_ERR_OID, NULLOID);
    }

    if ((ip = (unsigned int *) malloc ((i + 1) * sizeof *ip)) == NULL)
	return pe_seterr (pe, PE_ERR_NMEM, NULLOID);
    o -> oid_elements = ip, o -> oid_nelem = i;
    
    for (dp = pe -> pe_prim; dp < ep; ) {
	i = 0;
	do {
	    i <<= 7; 
	    i |= *dp & 0x7f;
	} while (*dp++ & 0x80);
	if (ip == o -> oid_elements)
	    *ip++ = i / 40, *ip++ = i % 40;
	else
	    *ip++ = i;
    }

    return o;
}
