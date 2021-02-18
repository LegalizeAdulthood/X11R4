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
 * GKS Inquire GKS Description Table functions :
 *	gqlvks_
 *	gqewk_
 *	gqwkm_
 *	gqmntn_
 *        
 * David Berkowitz
 * Bruce Haimowitz
 * TCS Development
 * Cambridge MA
 *
 * August 31 1988
 *
 * $Header: inqgksdesc.c,v 4.0 89/08/31 18:32:35 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/inqgksdesc.c,v $
 * 
 * $Log:	inqgksdesc.c,v $
 * Revision 4.0  89/08/31  18:32:35  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.15  89/06/26  15:33:39  amy
 * DCR d1	Declare errfp external for use in error macros.
 * 
 * Revision 1.14  89/03/15  15:01:11  amy
 * PTR c2043	All calls to VALIDMEMBER:  added value for errind 
 * 		parameter.
 *  
 * PTR c2020	Inq. List Element and Inq. Set Member functions:
 * 		return values of output parameters regardless
 * 		of validity of the n parameter, and accept 0 as valid value
 * 		for n.
 * 
 * Revision 1.13  89/01/06  09:39:09  todd
 * Fixed VALIDMEMBER call inq inqavailwstypes.
 * 
 * Revision 1.12  88/12/05  15:19:19  owens
 * changed errfp to errfpp
 * 
 * Revision 1.10  88/12/05  14:30:46  todd
 * Changes for Code review II.
 * 
 * Revision 1.9  88/11/16  10:17:09  todd
 * Removed extra call to gerrorhand.
 * Added check for ok return code so
 * a return is done before output parameters
 * are set.
 * 
 * Revision 1.8  88/11/09  07:03:11  bhaim
 * Added GQWKM and GQMNTN
 * 
 * Revision 1.7  88/11/08  13:47:22  bhaim
 * Added code for gqewk
 * 
 * Revision 1.6  88/10/11  17:02:38  todd
 * Added global_errnum initialization.
 * 
 * Revision 1.5  88/10/05  07:08:12  amy
 * Removed gqopwk from this file-- it belongs in inqgkslist.c.
 * 
 * Revision 1.4  88/10/04  14:23:04  amy
 * Added stub for gqmntn.
 * 
 * Revision 1.3  88/10/03  15:31:21  mike
 * Minor corrections made after Amy\'s review
 * 
 * Revision 1.2  88/09/27  16:22:19  mike
 * Fixed Code Review problems: prologues, casts, variable names, and range checks
 * 
 * Revision 1.1  88/09/27  08:17:08  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: inqgksdesc.c,v 4.0 89/08/31 18:32:35 amy Exp $";


#include "fortxgks.h"
#include <xgks.h>

/* errfp is needed for error macros d1 */
extern FILE *errfp;              /* d1 */

/*$F
 * gqlvks - Inquire Level of GKS
 *
 * int *errind - pointer to error indicator
 * int *level - pointer to level of GKS
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.145
 */
gqlvks_ (errind, level)
int *errind;
int *level;
{

debug ( ("Inquire Level of Kernel System  \n") );

*errind = ginqlevelgks ((Glevel *)level);

}
/*$F
 * gqewk - Inquire List  of Available Workstation Types
 *
 * int *n              list element requested 
 * int *errind         error indicator           
 * int *number         number of workstation types
 * int *wktyp          Nth element of list       

 *
 * Returns: 2002 in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.146
 */
gqewk_ (n,errind,number,wktyp)
int *n;      
int *errind;
int *number;
int *wktyp;
 { 
Gstrlist wstypes;
if (*errind = ginqavailwstypes (&wstypes)) return;
free(*(wstypes.strings));

/* set return values not dependent on range check  c2020 */
*number = (int) wstypes.number;

VALIDMEMBER(errind,*n,0,wstypes.number,errginqavailwstypes); /* c2043 */

/* Since the workstation type in fortran is an
   integer, simply return the integer type requested */ 
if (*n)			/* c2020 */
   *wktyp   = (int)  *n;
}

/*$F
 * gqwkm - Inquire Workstation Maximum Numbers
 *
 * int  *errind         - error indicator
 * int  *mxopwk         - max open workstations
 * int  *mxacwk         - max simultaneously active workstations
 * int  *mxwkas         - max workstations associated with segment
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.146
 */
gqwkm_ (errind,mxopwk,mxacwk,mxwkas)   
int  *errind;
int  *mxopwk;
int  *mxacwk;
int  *mxwkas;
{
Gwsmax maxws;
if (*errind = ginqwsmaxnum(&maxws)) return;
*mxopwk = (int) maxws.open;
*mxacwk = (int) maxws.active;
*mxwkas = (int) maxws.assoc;
}

/*$F
 * gqmntn - Inquire Maximum Normalization Transformation Number
 *
 * int	*errind		- pointer to error indicator
 * int	*maxtnr		- pointer to maximum normalization transformation number
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.146
 */
gqmntn_ (errind, maxtnr)
int *errind;
int *maxtnr;

{
debug(("Inquire Maximum Normalization Transformation Number"));
*errind = ginqmaxntrannum((Gint *)maxtnr);
}
