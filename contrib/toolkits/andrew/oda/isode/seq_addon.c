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
/* seq_addon.c - add a member to the end of a sequence (efficiency hack) */

#ifndef	lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/seq_addon.c,v 1.1 89/09/21 05:53:55 mss Exp $";
#endif

/* 
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/seq_addon.c,v 1.1 89/09/21 05:53:55 mss Exp $
 *
 *
 * $Log:	seq_addon.c,v $
 * Revision 1.1  89/09/21  05:53:55  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:58:54  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/11  16:19:33  mss
 * added copyright
 * 
 * Revision 1.1  89/08/04  01:57:53  mss
 * Initial revision
 * 
 * Revision 1.1  89/05/16  17:29:44  mss
 * Initial revision
 * 
 * Revision 6.0  89/03/18  23:39:15  mrose
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

int	seq_addon (pe, last, new)
PE	pe,
        last,
        new;
{
    if (pe == NULLPE)
	return NOTOK;
    if (last == NULLPE)
	return seq_add (pe, new, -1);
    new -> pe_offset = pe -> pe_cardinal++;
    last -> pe_next = new;
    return OK;
}
