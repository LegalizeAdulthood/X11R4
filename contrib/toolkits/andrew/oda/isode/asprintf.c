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
/* asprintf.c - sprintf with errno */

#ifndef	lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/asprintf.c,v 1.1 89/09/21 05:20:57 mss Exp $";
#endif

/* 
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/asprintf.c,v 1.1 89/09/21 05:20:57 mss Exp $
 *
 *
 * $Log:	asprintf.c,v $
 * Revision 1.1  89/09/21  05:20:57  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:24:56  mss
 * Initial revision
 * 
 * Revision 1.4  89/09/19  21:22:29  mss
 * reversed vrpinf flags back
 * 
 * Revision 1.3  89/09/19  17:17:28  jr
 * Comment out some code
 * 
 * Revision 1.2  89/09/11  16:11:09  mss
 * added copyright
 * 
 * Revision 1.1  89/07/27  13:02:18  mss
 * Initial revision
 * 
 * Revision 1.1  89/05/16  15:35:11  mss
 * Initial revision
 * 
 * Revision 6.0  89/03/18  23:24:54  mrose
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
#include <varargs.h>
#include "general.h"
#include "manifest.h"

/*    DATA */

extern int errno;

/*  */

void	asprintf (bp, ap)		/* what, fmt, args, ... */
char *bp;
va_list	ap;
{
    char   *what;

    what = va_arg (ap, char *);

    _asprintf (bp, what, ap);
}


void	_asprintf (bp, what, ap)	/* fmt, args, ... */
register char *bp;
char   *what;
va_list	ap;
{
    register int    eindex;
    char   *fmt;

    eindex = errno;

    *bp = NULL;
    fmt = va_arg (ap, char *);

    if (fmt) {
#ifndef	VSPRINTF
	struct _iobuf iob;
#endif

#ifndef	VSPRINTF
	iob._flag = _IOWRT | _IOSTRG;
#ifndef	vax
	iob._ptr = (unsigned char *) bp;
#else
	iob._ptr = bp;
#endif
	iob._cnt = BUFSIZ;
	_doprnt (fmt, ap, &iob);
	putc (NULL, &iob);
#else
	(void) vsprintf (bp, fmt, ap);
#endif
	bp += strlen (bp);

    }

    if (what) {
	if (*what) {
	    (void) sprintf (bp, " %s: ", what);
	    bp += strlen (bp);
	}
	(void) strcpy (bp, sys_errname (eindex));
	bp += strlen (bp);
    }

    errno = eindex;
}
