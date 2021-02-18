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
 *
 * FORTRAN to C binding for XGKS
 *
 * GKS Inquire Operating Sate:
 *
 *	gqops_(opsta)
 *
 * David Berkowitz
 * Bruce Haimowitz
 * TCS Development
 * Cambridge MA
 *
 * August 31 1988
 *
 * $Header: inqstate.c,v 4.0 89/08/31 18:33:02 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/inqstate.c,v $
 * 
 * $Log:	inqstate.c,v $
 * Revision 4.0  89/08/31  18:33:02  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.9  89/01/20  13:45:16  mike
 * No changes.
 * 
 * Revision 1.8  88/12/05  15:19:43  owens
 * changed errfp to errfpp
 * 
 * Revision 1.6  88/12/05  14:29:54  todd
 * Changes for Code review II.
 * 
 * Revision 1.5  88/11/16  10:19:10  todd
 * Removed extra call to gerrorhand.
 * Added check for ok return code so
 * a return is done before output parameters
 * are set.
 * 
 * Revision 1.4  88/10/13  12:31:53  bhaim
 * Changes for Code II
 * 
 * Revision 1.3  88/10/11  17:03:22  todd
 * No changes.
 * 
 * Revision 1.2  88/09/29  13:54:43  todd
 * Added includes.
 * 
 * Revision 1.1  88/09/27  08:17:17  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: inqstate.c,v 4.0 89/08/31 18:33:02 amy Exp $";


#include <xgks.h>
#include "fortxgks.h"


/*$F
 * gqops - Inquire Operating State Value 
 *
 * Parameters :
 * *opsta                             operating state 
 *
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.144
 */

gqops_(opsta)
int  *opsta; 
{
debug(("Inquire Operating State Value \n "));
ginqopst((Gos *) opsta);
}
