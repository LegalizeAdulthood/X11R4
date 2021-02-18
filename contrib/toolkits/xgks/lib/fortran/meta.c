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
 * GKS Metafile functions :
 *	gwitm_
 *	ggtitm_
 *	grditm_
 *	giitm_
 *      
 * David Berkowitz
 * Bruce Haimowitz
 * TCS Development
 * Cambridge MA
 *
 * August 31 1988
 *
 * $Header: meta.c,v 4.0 89/08/31 18:37:00 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/meta.c,v $
 * 
 * $Log:	meta.c,v $
 * Revision 4.0  89/08/31  18:37:00  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.11  88/12/05  15:20:17  owens
 * changed errfp to errfpp
 * 
 * Revision 1.9  88/12/05  14:35:21  bhaim
 * Changes for Code II
 * 
 * Revision 1.8  88/11/16  10:19:47  todd
 * Removed extra call to gerrorhand.
 * Added check for ok return code so
 * a return is done before output parameters
 * are set.
 * 
 * Revision 1.7  88/10/13  12:29:51  bhaim
 * Changes for Code II
 * 
 * Revision 1.6  88/10/11  17:03:44  todd
 * Added global_errnum initialization.
 * 
 * Revision 1.5  88/10/04  13:53:33  amy
 * Added stub for giitm.
 * 
 * Revision 1.4  88/10/03  15:31:18  mike
 * Minor corrections made after Amy\'s review
 * 
 * Revision 1.3  88/09/27  17:10:28  mike
 * Fixed module prologue
 * 
 * Revision 1.2  88/09/27  15:36:21  mike
 * checked out by mistake
 * 
 * Revision 1.1  88/09/27  08:17:23  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: meta.c,v 4.0 89/08/31 18:37:00 amy Exp $";

#include <xgks.h>
#include "fortxgks.h"

/*$F
 * gwitm - Write Item
 *
int *wkid;                         workstation identifier  
int *type;                         item type               
int *idrl;                         item data record length 
int *ldr;                          dimension of data record
char *datarec;                     data record             
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.142
 */
gwitm_ (wkid,type,idrl,ldr,datarec)   
int *wkid;                         /* workstation identifier  */
int *type;                         /* item type               */
int *idrl;                         /* item data record length */
int *ldr;                          /* dimension of data record*/
char *datarec;                     /* data record             */
{
gwritegksm((Gint)*wkid,(Gint)*type,(Gint)*idrl,(Gchar *)datarec);
}

/*$F
 * ggtitm - Get Item
 *
int *wkid;                          workstation identifier 
int *type;                          item type              
int *idrl;                          item data rec length   
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.142
 */
ggtitm_ (wkid,type,idrl)   
int *wkid;                          /* workstation identifier */
int *type;                          /* item type              */
int *idrl;                          /* item data rec length   */
{
Ggksmit   result;
if (ggetgksm((Gint)*wkid,&result)) return;

*type = (int)result.type;
*idrl = (int)result.length;
}

/*$F
 * giitm - Interpret Item
 *
int *wkid;                          workstation identifier 
int *midrl;                         max item data record   
int *mldr;                          dim of data record     
char *datarec;                      data record            
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.143
 */
grditm_ (wkid,midrl,mldr,datarec)   
int *wkid;                          /* workstation identifier */
int *midrl;                         /* max item data record   */
int *mldr;                          /* dim of data record     */
char *datarec;                      /* data record            */
{
greadgksm((Gint)*wkid,(Gint)*midrl,(Gchar *)datarec);
}

/*$F
 * giitm - Interpret Item
 *
 * int	*type		- pointer to item type
 * int	*idrl		- pointer to item data record length
 * int	*ldr		- pointer to dimension of data record array
 * char	*datrec[ldr]	- pointer to data record
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.143
 */
giitm_ (type, idrl, ldr, datarec)
int *type;
int *idrl;
int *ldr;
char *datarec;

{
Ggksmit   typeandlength;
typeandlength.type = *type;
typeandlength.length = *idrl;
ginterpret(&typeandlength,(Gchar *)datarec);
}
