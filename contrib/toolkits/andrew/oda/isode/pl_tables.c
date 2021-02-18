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
/* pl_tables.c - tables for presentation lists */

#ifndef	lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/pl_tables.c,v 1.1 89/09/21 05:50:23 mss Exp $";
#endif

/* 
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/pl_tables.c,v 1.1 89/09/21 05:50:23 mss Exp $
 *
 *
 * $Log:	pl_tables.c,v $
 * Revision 1.1  89/09/21  05:50:23  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:55:09  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/11  16:15:58  mss
 * added copyright
 * 
 * Revision 1.1  89/07/27  13:03:09  mss
 * Initial revision
 * 
 * Revision 1.1  89/05/16  17:28:43  mss
 * Initial revision
 * 
 * Revision 6.0  89/03/18  23:38:48  mrose
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

/*    DATA */

char   *pe_classlist[] = {
    "UNIV",
    "APPL",
    "CONT",
    "PRIV"
};

int	pe_maxclass = sizeof pe_classlist / sizeof pe_classlist[0];


char   *pe_univlist[] = {
    "EOC",
    "BOOL",
    "INT",
    "BITS",
    "OCTS",
    "NULL",
    "OID",
    "ODE",
    "EXTN",
    "REAL",
    "ENUM",
    "ENCR",
    NULLCP,
    NULLCP,
    NULLCP,
    NULLCP,
    "SEQ",
    "SET",
    "NUMS",
    "PRTS",
    "T61S",
    "VTXS",
    "IA5S",
    "UTCT",
    "GENT",
    "GFXS",
    "VISS",
    "GENS",
    "CHRS",
};

int	pe_maxuniv = sizeof pe_univlist / sizeof pe_univlist[0];


int	pe_maxappl = 0;
char  **pe_applist = NULL;

int	pe_maxpriv = 0;
char  **pe_privlist = NULL;
