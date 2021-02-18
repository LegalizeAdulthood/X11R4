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
 * GKS Inquiry Functions for Segment State List :
 *	gqsga_
 *	gqaswk_
 *        
 * David Berkowitz
 * Bruce Haimowitz
 * Todd Gill
 * TCS Development
 * Cambridge MA
 *
 * September 28 1988
 *
 * $Header: inqseglist.c,v 4.0 89/08/31 18:32:57 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/inqseglist.c,v $
 * 
 * $Log:	inqseglist.c,v $
 * Revision 4.0  89/08/31  18:32:57  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.17  89/06/26  15:39:52  amy
 * DCR d1	Declare errfp external for use in error macros.
 * 
 * Revision 1.16  89/04/04  09:50:47  bruce
 * PTR# c2043:	Updated for c2043
 * 
 * Revision 1.15  89/03/15  16:18:00  bruce
 * PTR c2020	Number of members in the set should be returned regardless
 * 		of whether VALIDMEMBER returns an error or not.
 * 
 * Revision 1.14  89/03/15  14:52:43  bruce
 * PTR: c2043  Added errind to call to VALIDMEMBER.
 * 
 * Revision 1.13  89/02/04  14:24:43  amy
 * Cosmetic changes to the log.
 * 
 * Revision 1.12  89/01/20  13:26:04  todd
 * PTR c2028	Fixed.
 * 
 * Revision 1.11  89/01/06  08:24:20  bruce
 * PTR c2020	Fixed the var type for the priority parameter in gqsga() for c2019     * 		and changed the VALIDMEMBER macro call so 0 can be passed to return    *		the number of members in the set.
 * 
 * Revision 1.10  88/12/30  09:37:55  todd
 * Fixed debug statement.
 * 
 * Revision 1.9  88/12/28  13:57:03  todd
 * PTR c2010	Fixed binding problems. 
 * 
 * Revision 1.8  88/12/05  15:19:40  owens
 * changed errfp to errfpp
 * 
 * Revision 1.6  88/12/05  14:29:46  todd
 * Changes for Code review II.
 * 
 * Revision 1.5  88/11/29  16:37:40  todd
 * Changed file name.
 * 
 * Revision 1.4  88/11/16  10:18:30  todd
 * Removed extra call to gerrorhand.
 * Added check for ok return code so
 * a return is done before output parameters
 * are set.
 * 
 * Revision 1.3  88/11/07  11:38:24  todd
 * Finished functions and comments.
 * 
 * Revision 1.2  88/10/13  15:37:07  bruce
 * Made corrections to array addressing.
 * 
 * Revision 1.1  88/10/13  12:13:36  amy
 * Initial revision
 * 
 * Revision 1.2  88/09/28  15:07:45  bruce
 * Added function prologues, GKS type casting and changed variable names.
 * 
 * Revision 1.1  88/09/27  08:17:16  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: inqseglist.c,v 4.0 89/08/31 18:32:57 amy Exp $";


#include "fortxgks.h"
#include <xgks.h>

/* errfp is needed for error macros d1 */
extern FILE *errfp;              /* d1 */

/*$F
 * gqsga - Inquire Segment Attributes
 *
 * int   *sgna   - pointer to segment name
 * int   *errind - pointer to error indicator
 * float *segtm  - pointer to segment transformation matrix
 * int   *vis    - pointer to segment visibility
 * int   *high   - pointer to segment highlighting
 * float *sgpr   - pointer to segment priority
 * int   *det    - pointer to segment detectability

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.74
 */
gqsga_ (sgna, errind, segtm, vis, high, sgpr, det)
int *sgna;
int *errind;
float *segtm;
int *vis;
int *high;
float *sgpr;		/* c2019 */
int *det;
{
Gsegattr segattr;

debug ( ("Inquire Segment Attributes %d  \n", *sgna) );

segattr.seg = (Gint)*sgna;
if (*errind = ginqsegattr (&segattr)) return;           /* c2043 */
  
CHANGE_COL_TO_ROW_MAJOR_F(&segattr.segtran,segtm,2,3);  /* c2028 */

/* Reverse the sense of vis */ 
*vis = (int)((segattr.vis) ? FORT_GINVIS : FORT_GVISI);

*high        = (int)segattr.hilight;
*sgpr        = (float)segattr.pri;		/* c2019 */
*det         = (int)segattr.det;

}

/*$F
 * gqaswk - Inquire Set member of Associated Workstations
 *
 * int	*sgna		- pointer to segment name
 * int	*n		- pointer to set member requested
 * int	*errind		- pointer to error indicator
 * int	*ol		- pointer to number of associated workstations
 * int	*wkid		- pointer to nth member of set of associated workstations
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.189
 */
gqaswk_ (sgna, n, errind, ol, wkid)    /*c2010*/
int *sgna;
int *n;
int *errind;
int *ol;
int *wkid;

{
Gintlist asswk;
debug(("Inquire Set member of Associated Workstations \n"));

if (*errind = ginqassocws((Gint) *sgna,&asswk)) return;     /*c2010*/

/* set return values not effected by range query  c2020 */
*ol = (int) asswk.number;

VALIDMEMBER(errind,*n,0,asswk.number,errginqassocws);  /* c2043 c2010 c2020 */
if (*n)				/* c2020 */
  *wkid = asswk.integers[*n-1];	/* c2020 */

free(asswk.integers);
}
 

