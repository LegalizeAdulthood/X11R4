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
 * GKS inquire error list functions :
 *	gqiqov_
 *     
 *        
 * David Berkowitz
 * Bruce Haimowitz
 * Todd Gill
 * TCS Development
 * Cambridge MA
 *
 * August 31 1988
 *
 * $Header: inqerrlist.c,v 4.0 89/08/31 18:32:30 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/inqerrlist.c,v $
 * 
 * $Log:	inqerrlist.c,v $
 * Revision 4.0  89/08/31  18:32:30  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.13  89/03/15  20:40:21  amy
 * PTR c2043	ginqinputoverflow:  correct errind assignment.
 * 
 * Revision 1.12  88/12/05  15:19:16  owens
 * changed errfp to errfpp
 * 
 * Revision 1.10  88/12/05  14:32:42  todd
 * Changes for code review II.
 * 
 * Revision 1.9  88/11/16  10:17:01  todd
 * Removed extra call to gerrorhand.
 * Added check for ok return code so
 * a return is done before output parameters
 * are set.
 * 
 * Revision 1.8  88/11/07  11:34:12  todd
 * Finished functions and comments.
 * 
 * Revision 1.7  88/10/11  17:02:24  todd
 * No Changes
 * 
 * Revision 1.6  88/10/11  14:34:37  owens
 * more changes from code review
 * 
 * Revision 1.5  88/10/11  13:21:44  todd
 * More code review changes.
 * 
 * Revision 1.4  88/09/30  11:28:54  owens
 * more code review changes
 * 
 * Revision 1.3  88/09/29  16:52:07  owens
 * more code review changes
 * 
 * Revision 1.2  88/09/29  16:28:05  owens
 * code review changes
 * 
 * Revision 1.1  88/09/27  08:17:06  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: inqerrlist.c,v 4.0 89/08/31 18:32:30 amy Exp $";

#include "fortxgks.h"
#include <xgks.h>

/*$F
 * gqiqov - Inquire Input Queue Overflow
 *
 * int	*errind		- pointer to error indicator
 * int	*wkid		- pointer to workstation identifier
 * int	*icl		- pointer to input class (GLOCAT, GSTROK, GVALUA, GCHOIC,
 *			  GPICK, GSTRIN)	
 * int	*idn		- pointer to input device number
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.192
 */
gqiqov_ (errind, wkid, icl, idn)
int *errind;
int *wkid;
int *icl;
int *idn;

{
Gqueue overflow;
debug(("Inquire Input Queue Overflow  \n"));

if (*errind = ginqinputoverflow(&overflow)) return; /* c2043 */

*icl = (int) overflow.class;
*idn = (int) overflow.devno; 
}
