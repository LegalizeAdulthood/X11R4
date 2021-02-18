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
 * GKS Escape functions :
 *	gesscm_ ()
 *	gessdc_ ()
 *	gessrp_ () 
 * 	gessrn_ () 
 *	gescid_ ()
 *
 * David Berkowitz
 * Bruce Haimowitz
 * TCS Development
 * Cambridge MA
 *
 * August 31 1988
 *
 * $Header: escapes.c,v 4.0 89/08/31 18:13:21 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/escapes.c,v $
 * 
 * $Log:	escapes.c,v $
 * Revision 4.0  89/08/31  18:13:21  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 4.0  89/08/31  18:05:22  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.9  89/06/23  17:17:43  amy
 * DCR d1	Declare errfp external for use in error-handling macros
 * 		expansion.
 * 
 * Revision 1.8  89/06/16  14:14:30  bruce
 * PTR# c2069:	Added GESCID function.
 * 
 * Revision 1.7  88/12/27  14:24:52  todd
 * Corrected spelling mistake.
 * 
 * Revision 1.6  88/12/05  15:18:35  owens
 * changed errfp to errfpp
 * 
 * Revision 1.4  88/12/05  14:31:57  todd
 * Changes for code review II.
 * 
 * Revision 1.3  88/11/16  10:16:25  todd
 * Removed extra call to gerrorhand.
 * Added check for ok return code so
 * a return is done before output parameters
 * are set.
 * 
 * Revision 1.2  88/10/11  17:00:41  todd
 * Added Global_errnum initialization.
 * 
 * Revision 1.1  88/09/27  08:16:51  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: escapes.c,v 4.0 89/08/31 18:13:21 amy Exp $";

#include "fortxgks.h"
#include <xgks.h>

extern char *xgks_connection;           /* c2069 */
extern FILE *errfp;			/* d1 */

/*$F
 * gesscm - Escape Set Colour Mask
 *
 * int	*wsid		- pointer to workstation identifier
 * int	*mask		- pointer to colour planes mask
 *
 * See also: ANSI standard p.80
 */
gesscm_ (wsid, mask)
int    *wsid;
int    *mask;
{
gescsetcolormask ((Gint) *wsid, (Gint) *mask);
}  /*  end  gesscm_  */


/*$F
 * gessdc - Escape Set DC Size
 *
 * int	*wsid		- pointer to workstation identifier
 * int	*xsize		- pointer to maximum x coordinate of DC size
 * int	*ysize		- pointer to maximum y coordinate of DC size
 *
 * See also: ANSI standard p.80
 */
gessdc_ (wsid, xsize, ysize)
int    *wsid;
float  *xsize;
float  *ysize;
{
Gpoint  dcsize;

dcsize.x = *xsize;
dcsize.y = *ysize;
gescsetdcsize (*wsid, dcsize);

}  /*  end  gessdc_  */


/*$F
 * gessrp - Escape Set Storage of Non-segment Primitives
 *
 * int	*wsid		- pointer to workstation identifier
 * int	*store		- pointer to storage flag (GNO, GYES)
 *
 * Returns: Error 2000
 *
 * See also: ANSI standard p.80
 */
gessrp_ (wsid, store) 
int   *wsid;
int   *store;
{
STOREAGEFLAG(*store,errgescstoreprimi);
gescstoreprimi (*wsid, *store);

}  /* end gessrp_  */

/*$F
 * gessrn - Escape Set Redraw Notify
 *
 * int	*wsid		- pointer to workstation identifier
 * int	*func		- pointer to user's redraw function
 *
 * See also: ANSI standard p.80
 *
 */
gessrn_ (wsid, func) 
int   *wsid;
int   (*func)();
{
gescredrawnotify (*wsid, *func);

}  /* end gessrn_  */

/*$F								   c2069   
 * gescid_ - Escape Set Connection Identifier  			   c2069   
 * 								   c2069   
 * char *conid		- server connection identifier		   c2069   
 * long *length		- string length passed by FORTRAN	   c2069   
 *								   c2069   
 */								/* c2069 */  
								/* c2069 */
gescid_ (conid,length) 						/* c2069 */
char *conid;							/* c2069 */
long length;  /* this parameter is passed by FORTRAN/C conventions c2069 */
{    	    							/* c2069 */
 int i;								/* c2069 */
 debug(("gessci - Escape Set Connection Identifier \n"));	/* c2069 */

 if (xgks_connection == NULL)					/* c2069 */
   {								/* c2069 */
     debug(("allocating space for connection \n"));		/* c2069 */
     xgks_connection = (char *) malloc(length+1);		/* c2069 */
   }								/* c2069 */
 /* copy connection to global xgks_connection */		/* c2069 */
 /* conid must be space terminated !          */		/* c2069 */
 for (i=0 ; i < length  && conid[i] != ' '; i++)		/* c2069 */
   xgks_connection[i] = conid[i];				/* c2069 */

 xgks_connection[i] = '\0';					/* c2069 */
 debug(("connection id = %s \n",xgks_connection));		/* c2069 */
}  /* end gessci_ */						/* c2069 */
