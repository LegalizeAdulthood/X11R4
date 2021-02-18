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
 * GKS inquire pixel functions :	
 *	gqpxad_
 *	gqpxa_
 *	gqpx_
 *        
 * David Berkowitz
 * Bruce Haimowitz
 * Todd Gill
 * TCS Development
 * Cambridge MA
 *
 * August 31 1988
 *
 * $Header: inqpixel.c,v 4.0 89/08/31 18:32:52 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/inqpixel.c,v $
 * 
 * $Log:	inqpixel.c,v $
 * Revision 4.0  89/08/31  18:32:52  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.18  89/06/26  15:38:08  amy
 * DCR d1	Declare errfp external for use in error macros.
 * 		Change reference to errfpp to errfp.
 * 
 * Revision 1.17  89/03/27  08:52:39  bruce
 * PTR# c2050:	More changes for 2050.  Input parameters have to be copied
 * 		into the input structure.
 * 
 * Revision 1.16  89/03/15  14:42:16  bruce
 * PTR c2050:  Fixed numerous problems in inq pixel array (GQPXA):
 *    - check for error 2001 was returning 2002
 *     - input param (dimen) being used as output param
 *    - C call was missing param
 *    - return matrix unnecessarily transposed
 * 
 * Revision 1.15  89/03/15  14:19:44  bruce
 * PTR c2056   Removed formal parameter from gqpx_.
 * 
 * Revision 1.14  88/12/05  15:19:36  owens
 * changed errfp to errfpp
 * 
 * Revision 1.12  88/12/05  14:29:20  todd
 * Changes for Code review II.
 * 
 * Revision 1.11  88/11/16  10:18:08  todd
 * Removed extra call to gerrorhand.
 * Added check for ok return code so
 * a return is done before output parameters
 * are set.
 * 
 * Revision 1.10  88/11/07  11:38:03  todd
 * Finished functions and comments.
 * 
 * Revision 1.9  88/10/11  17:03:14  todd
 * No changes.
 * 
 * Revision 1.8  88/10/11  14:34:25  owens
 * more changes from code review
 * 
 * Revision 1.7  88/10/11  13:21:09  todd
 * More code review changes.
 * 
 * Revision 1.6  88/09/30  11:28:48  owens
 * more code review changes
 * 
 * Revision 1.5  88/09/29  16:52:04  owens
 * more code review changes
 * 
 * Revision 1.4  88/09/29  16:28:01  owens
 * code review changes
 * 
 * Revision 1.3  88/09/29  10:14:20  todd
 * No changes.
 * 
 * Revision 1.2  88/09/27  12:44:48  todd
 * Added missing function.
 * 
 * Revision 1.1  88/09/27  08:17:13  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: inqpixel.c,v 4.0 89/08/31 18:32:52 amy Exp $";

#include "fortxgks.h"
#include <xgks.h>

extern FILE *errfp; 		/* d1 */


/*$F
 * gqpxad - Inquire Pixel Array Dimensions
 *
 * int	*wkid		- pointer to workstation identifier
 * float *px,*py	- pointer to upper left corner in world coordinates
 * float *qx,*qy	- pointer to lower right corner in world coordinates
 * int	*errind		- pointer to error indicator
 * int	*n,m		- pointer to dimensions of pixel array
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.190
 */
gqpxad_ (wkid, px, py, qx, qy, errind, n, m)
int *wkid;
float *px;
float *py;
float *qx;
float *qy;
int *errind;
int *n;
int *m;

{
Grect rect;
Gipoint dim;
debug (("Inquire Pixel Array Dimensions \n"));

rect.ll.x = (Gfloat) *px;
rect.ll.y = (Gfloat) *py;
rect.ur.x = (Gfloat) *qx;
rect.ur.y = (Gfloat) *qy;
if (*errind = ginqpixelarraydim((Gint) *wkid, &rect, &dim)) return;

*n = (int) dim.x;
*m = (int) dim.y;
}

/*$F
 * gqpxa - Inquire Pixel Array
 *
 * int	*wkid		- pointer to workstation identifier
 * float *px,*py	- pointer to upper left corner in world coordinates
 * int	*dimx,*dimy	- pointers to dimensions of colour index array
 * int	*isc,*isr	- pointers to start column, start row
 * int	*dx,*dy		- pointers to size of requested pixel array
 * int	*errind		- pointer to error indicator
 * int	*invval		- pointer to presence of invalid values (GABSNT, GPRSNT)
 * int	*colia[dimx,dimy] - pointer to colour index array
 *
 * Returns: Error 2002 in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.191
 */
gqpxa_ (wkid, px, py, dimx, dimy, isc, isr, dx, dy, errind, invval, colia)
int *wkid;
float *px;
float *py;
int *dimx;
int *dimy;
int *isc;
int *isr;
int *dx;
int *dy;
int *errind;
int *invval;
int *colia;

{
Gpoint   point;                                         /* c2050 */
Gipoint  dimen;
Gpxarray pxarr;

debug(("Inquire Pixel Array \n"));

dimen.x = *dx;                                          /* c2050 */
dimen.y = *dy;                                          /* c2050 */

if (*dx > *dimx || *dy > *dimy)                         /* c2050 */
  {                                                     /* c2050 */
   *errind = 2001;                                      /* c2050 */
   gerrorhand(2001,errginqpixelarray,(errfp));		/* c2050 */ /* d1 */
   return;                                              /* c2050 */
  }                                                     /* c2050 */

point.x = *px;                                          /* c2050 */
point.y = *py;                                          /* c2050 */

if (*errind = ginqpixelarray((Gint) *wkid, &point, &dimen, &pxarr)) return;
							/* c2050 */

*invval = (int) pxarr.covalid;

/* c2050:  This array does not need to be transposed; however, we must   */
/* fit the dx-by-dy array obtained into the dimx-by-dimy array provided. */
   {                                                    /* c2050 */
   int  col, row;                                       /* c2050 */

   for (row=0; row < *dy; row++)                        /* c2050 */
      for (col=0; col < *dx; col++)                     /* c2050 */
         *(colia + (row * *dimx + col)) = *(pxarr.array + (row * *dx + col));
                        /* c2050 */      
   }                                                    /* c2050 */

free( pxarr.array );                                    /* c2050 */
}

/*$F
 * gqpx - Inquire Pixel
 *
 * int	*wkid		- pointer to workstation identifier
 * float *px,*py	- pointer to upper left corner in world coordinates
 * int	*errind		- pointer to error indicator
 * int	*coli		- pointer to colour index
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.191
 */
gqpx_ (wkid, px, py, errind, coli)    /* 2056 */
int *wkid;
float *px;
float *py;
int *errind;
int *coli;

{
Gpoint ppoint;

ppoint.x = *px;
ppoint.y = *py;

*errind = ginqpixel((Gint) *wkid, &ppoint, (Gint) coli);

}
