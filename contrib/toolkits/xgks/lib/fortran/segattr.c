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
 * GKS Segment Attributes:
 *	
 *	gssgt_ (sgna, m)
 *	gsvis_ (sgna, vis)
 *	gshlit_ (sgna,hil)   
 *	gssgp_ (sgna,prior)   
 *	gsdtec_ (sgna, det)
 *
 * David Berkowitz
 * Bruce Haimowitz
 * TCS Development
 * Cambridge MA
 *
 * August 31 1988
 *
 * $Header: segattr.c,v 4.0 89/08/31 18:37:19 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/segattr.c,v $
 *
 * $Log:	segattr.c,v $
 * Revision 4.0  89/08/31  18:37:19  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.10  89/06/26  16:07:22  amy
 * DCR d1	Declare errfp external for use in error macros.
 * 
 * Revision 1.9  89/06/19  11:05:46  amy
 * PTR c2074	gsdtec:  corrected type casting.
 * 
 * Revision 1.8  89/01/06  08:28:48  bruce
 * Fixed some of the type casting.  PTR# c2019.
 * 
 * Revision 1.7  88/12/30  09:44:34  todd
 * Fixed debug in gssgp.
 * 
 * Revision 1.6  88/12/05  15:20:36  owens
 * changed errfp to errfpp
 * 
 * Revision 1.4  88/12/05  14:27:03  bhaim
 * Changes for Code II
 * 
 * Revision 1.3  88/11/15  16:29:24  bhaim
 * Error handling
 * 
 * Revision 1.2  88/10/06  09:48:57  todd
 * Code review changes.
 * 
 * Revision 1.1  88/09/27  08:17:30  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: segattr.c,v 4.0 89/08/31 18:37:19 amy Exp $";

#include "fortxgks.h"
#include <xgks.h>

/* errfp is needed for error macros d1 */
extern FILE *errfp;              /* d1 */

/*$F
 *  gssgt - Set Segment Transformation
 *
 * Parameters :
 * int *sgna; 		segment name
 * float *m;		transformation matrix

 *
 * See also: ANSI standard p.115
 */


gssgt_ (sgna, m)
int *sgna;
float *m;
{
Gsegattr segattr;

debug ( ("Set Segment Transformation %d  \n", *sgna) );

/* Need all this segment's attributes for gsetsegattr */
segattr.seg = (Gint) *sgna;
ginqsegattr (&segattr); 


MOVE_ARRAY_1X6_TO_2X3(m,segattr.segtran)

gsetsegattr ((Gint) *sgna, &segattr); 

}


/*$F
 * gsvis - Set Visibility
 *
 * Parameters :
 * int *sgna;		segment name
 * int *vis; 		visibliity

 *
 * Returns: 2000, in addition to ANSI standard errors for this function.
 * See also: ANSI standard p.116
 */


gsvis_ (sgna, vis)
int *sgna;
int *vis;
{
Gsegattr segattr;

debug ( ("Set Visibility %d %d \n", *sgna, *vis) );

/* check for enumerated type out of range */
VISIBILITY(*vis,errginqsegattr);

/* Need all this segment's attributes for gsetsegattr */
segattr.seg = (Gint) *sgna;
ginqsegattr (&segattr); 
  
/* Reverse the sense of *vis */
segattr.vis = (Gsegvis) (*vis) ? FORT_GINVIS : FORT_GVISI;  

gsetsegattr ((Gint) *sgna, &segattr); 

}


/*$F
 * gshlit - Set Highlighting
 *
 *
 * Returns: 2000, in addition to ANSI standard errors for this function.
 * See also: ANSI standard 
 */


gshlit_ (sgna,hil)   
int  *sgna;                       /* segment name   */
int  *hil;                        /* highlighting      */
{
Gsegattr segattr;

debug ( ("Set Highlighting %d %d \n", *sgna, *hil) );

/* check enumerated type */
HIGHLIGHTING(*hil,errgsethighlight);

/* Need all this segment's attributes for gsetsegattr */
segattr.seg = (Gint) *sgna;		/* c2019 */
ginqsegattr (&segattr); 
  
segattr.hilight = (Gseghi)*hil;		/* c2019 */
gsetsegattr ((Gint) *sgna, &segattr); 
}


/*$F
 * gssgp - Set Segment Priority
 *
 * Parameters :

 * See also: ANSI standard 
 */


gssgp_ (sgna,prior)   
int  *sgna;                       /* segment name   */
float  *prior;                      /* priority       */
{
Gsegattr segattr;

debug ( ("Set priority %d %f \n", *sgna, *prior) );


/* Need all this segment's attributes for gsetsegattr */
segattr.seg = (Gint) *sgna;		/* c2019 */
ginqsegattr (&segattr); 
  
segattr.pri = (Gfloat) *prior;
gsetsegattr ((Gint) *sgna, &segattr); 
}


/*$F
 * gsdtec - Set Dectability    
 *
 * Parameters :
 * int *sgna; 		segment name
 * int *det;      	detectability

*
* Returns: 2000, in addition to ANSI standard errors for this function.
* See also: ANSI standard p.79
*/


gsdtec_ (sgna, det)
int *sgna;
int *det;

{
Gsegattr segattr;

debug ( ("Set Detectability %d %d \n", *sgna, *det) );

/* check enumerated type */
DETECTABILITY(*det,errginqsegattr);

/* Need all this segment's attributes for gsetsegattr */
segattr.seg = (Gint) *sgna;	/* c2074 */
ginqsegattr (&segattr); 
  
segattr.det = (Gsegdet) *det;
gsetsegattr ((Gint) *sgna, &segattr); 

}
