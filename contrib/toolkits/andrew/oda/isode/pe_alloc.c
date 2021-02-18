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
/* pe_alloc.c - allocate a presentation element */

#ifndef	lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/pe_alloc.c,v 1.1 89/09/21 05:49:46 mss Exp $";
#endif

/* 
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/pe_alloc.c,v 1.1 89/09/21 05:49:46 mss Exp $
 *
 *
 * $Log:	pe_alloc.c,v $
 * Revision 1.1  89/09/21  05:49:46  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:54:25  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/11  16:15:21  mss
 * added copyright
 * 
 * Revision 1.1  89/07/27  13:03:02  mss
 * Initial revision
 * 
 * Revision 1.1  89/05/16  17:28:18  mss
 * Initial revision
 * 
 * Revision 6.0  89/03/18  23:38:40  mrose
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


/* A Presentation Element (or PElement) is an internal representation for
   a presentation type from ISO8825.  The fields of the structure are:

	pe_class:	UNIVersal, APPLication, CONText, or PRIVate
	pe_form:	PRIMative, CONStructor, InlineCONStructor
	pe_id:		identifier

	pe_len:		if a PRIMative, then the length of pe_prim,
			else a scratch value; "indefinite" length elements
			have a pe_len of -1 (PE_LEN_INDF)
	pe_ilen:	if an InlineCONStructor, then the offset to the real
			data portion

	pe_prim:	if a PRIMative or an Inline CONStructor, the
			byte-string
	pe_cons:	if a CONStructor, the first element in the
			singly-linked list of elements

	pe_next:	if the immediate parent is a constructor, the
			next element in the singly-linked list of elements

	pe_cardinal:	if a LIST (SET or SEQ CONStructor), the cardinality
			of the list
	pe_offset:	if a member of a SEQ LIST, the offset in the SEQUENCE

	pe_nbits:	if a BITSTRING, the number of bits in the string

	pe_refcnt:	a hack for ANYs in pepy
 */

/*  */

PE	pe_alloc (class, form, id)
PElementClass class;
PElementForm  form;
PElementID id;
{
    register PE	    pe;

    if ((pe = (PE) calloc (1, sizeof *pe)) == NULLPE)
	return NULLPE;

    pe -> pe_class = class;
    pe -> pe_form = form;
    pe -> pe_id = id;

    return pe;
}
