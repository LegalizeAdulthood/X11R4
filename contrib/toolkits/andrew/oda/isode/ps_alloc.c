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
/* ps_alloc.c - allocate a presentation stream */

#ifndef	lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/ps_alloc.c,v 1.1 89/09/21 05:52:00 mss Exp $";
#endif

/* 
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/ps_alloc.c,v 1.1 89/09/21 05:52:00 mss Exp $
 *
 *
 * $Log:	ps_alloc.c,v $
 * Revision 1.1  89/09/21  05:52:00  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:56:56  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/11  16:17:41  mss
 * added copyright
 * 
 * Revision 1.1  89/07/27  13:03:31  mss
 * Initial revision
 * 
 * Revision 1.1  89/05/16  17:29:03  mss
 * Initial revision
 * 
 * Revision 6.0  89/03/18  23:39:01  mrose
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


/* A Presentatation Stream (or PStream) is the second generation of
   "generic" I/O stream-based handling.  (For the first attempt,
   take a look at the prototype implementation of the TTI Trusted Mail
   Agent.)  The idea is to present a common, simple I/O paradigm (i.e.,
   the UNIX v7 philosophy) to protocol-translation entities regardless of
   the underlying medium (files, pipes, sockets, or strings).

   New streams are created by a call to ps_alloc().  It allocates memory
   and calls an open routine.  This routine fills in the dispatch vectors
   for read/write and (optionally) close.  It can also fill in any other
   part of the stream's structure it likes.

   Once created, I/O is done using the macros ps_read/ps_write.  These
   return either NOTOK or OK; depending on how things went. The read/write
   routines are invoked as:

	int	iofunc (ps, data, n, in_line)
	PS	ps;
	PElementData data;
	PElementLen  n;
	int	in_line;

   They should read/write upto len bytes, starting at data, and return the
   number of bytes processed, or NOTOK on error.  The routine ps_io() will
   make successive calls to fill/flush the data.  If the read/write routine
   returns NOTOK, it should set ps_errno as well.

   Streams are removed by a call to ps_free ().  It calls the close
   routine, if any, which should de-commission any parts of the stream's
   structure that are in use.  ps_free() will then free the allocated
   memory.
 */

/*  */

PS	ps_alloc (io)
register IFP	io;
{
    register PS	    ps;

    if ((ps = (PS) calloc (1, sizeof *ps)) == NULLPS)
	return NULLPS;

    if ((*io) (ps) == NOTOK) {
	ps_free (ps);
	return NULLPS;
    }

    return ps;
}
