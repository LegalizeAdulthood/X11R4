/*
 *		Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 * FORTRAN to C binding for XGKS
 *
 * GKS Error Handling Functions :
 *	gerhnd_
 *
 * Todd Gill
 * TCS Development
 * Cambridge MA
 *
 * September 28 1988
 *
 * $Header: gerhnd.c,v 4.0 89/08/31 18:14:34 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/gerhnd.c,v $ 
 *
 * $Log:	gerhnd.c,v $
 * Revision 4.0  89/08/31  18:14:34  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 4.0  89/08/31  18:05:27  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.3  89/03/15  18:14:43  amy
 * PTR c1130	gerhnd:  change definition of errfil to int.
 * 
 * Revision 1.2  89/01/14  14:01:24  todd
 * Corrected call to gerlog
 * 
 * Revision 1.1  88/12/01  11:50:01  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: gerhnd.c,v 4.0 89/08/31 18:14:34 amy Exp $";

#include <xgks.h>
#include "fortxgks.h"

/*$F
 * gerhnd -  Error Handling
 *
 *   int *errnr    - error number
 *   int *fctid    - function number
 *   int *errfil   - error file
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p. 195
 */
gerhnd_ (errnr,fctid,errfil)
int *errnr;
int *fctid;
int *errfil;	/* c1130 */
{
gerlog_(errnr,fctid,errfil);
}


