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
 * GKS Segment Manipulation Functions:
 *        
 *	gcrsg_ (sgna)
 *	gclsg_ ()
 *	grensg_ (old,new)
 *	gdsg_ (sgna)
 *	gdsgwk_ (wkid, sgna)
 *	gasgwk_ (wkid,sgna)
 *	gcsgwk_ (wkid, sgna)
 *	ginsg_ (sgna, m)
 *
 * David Berkowitz
 * Bruce Haimowitz
 * TCS Development
 * Cambridge MA
 *
 * August 31 1988
 *
 * $Header: segment.c,v 4.0 89/08/31 18:37:23 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/segment.c,v $
 * 
 * $Log:	segment.c,v $
 * Revision 4.0  89/08/31  18:37:23  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.8  88/12/05  15:20:39  owens
 * changed errfp to errfpp
 * 
 * Revision 1.6  88/12/05  14:27:14  bhaim
 * 
 * 
 * Revision 1.5  88/11/10  10:24:58  bhaim
 * Error handling
 * 
 * Revision 1.4  88/10/11  17:04:18  todd
 * Added global_errnum initialization.
 * 
 * Revision 1.3  88/10/11  06:12:07  bhaim
 * No changes
 * 
 * Revision 1.2  88/10/06  09:49:34  todd
 * Code review changes.
 * 
 * Revision 1.1  88/09/27  08:17:32  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: segment.c,v 4.0 89/08/31 18:37:23 amy Exp $";

#include "fortxgks.h"
#include <xgks.h>


/*$F
 *  gcrsg - Create Segment
 *
 * Parameters :
 * int *sgna;		segment name

 *
   Returns: ANSI standard errors for this function
 * See also: ANSI standard p111.
 */


gcrsg_ (sgna)
int *sgna;
{

debug ( ("Create Segment %d \n", *sgna) ); 

gcreateseg ((Gint) *sgna);

}


/*$F
 *  gclsg - Close Segment
 *
 * Parameters :

 *
   Returns: ANSI standard errors for this function
 * See also: ANSI standard p111.
 */


gclsg_ ()
{

debug ( ("Close Segment \n") ); 

gcloseseg ();

}

/*$F
 *  grensg - Rename Segment
 *
 * Parameters :
 * int *old; 		old segment name
 * int *new;		new segment name

   Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p.111
 */


grensg_ (old,new)
int *old;
int *new;
{

debug ( ("Rename Segment %d %d \n",*old,*new) ); 

grenameseg ((Gint) *old,(Gint) *new);

}

/*$F
 *  gdsg - Delete Segment
 *
 * Parameters :
 * int *sgna;		segment name

   Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p.112
 */


gdsg_ (sgna)
int *sgna;
{

debug ( ("Delete Segment %d \n", *sgna) ); 

gdelseg ((Gint) *sgna);

}


/*$F
 *  gdsgwk - Delete Segment from Workstation
 *
 * Parameters :
 * int *wkid;		workstation identifier
 * int *sgna;		segment name

   Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p.112
 */


gdsgwk_ (wkid, sgna)
int *wkid;
int *sgna;
{

debug ( ("Delete Segment from Workstation %d %d \n", *wkid, *sgna) ); 

gdelsegws ((Gint) *wkid,(Gint) *sgna);

}


/*$F
 *  gasgwk - Associate Segment with Workstation
 *
 * Parameters :
 * int *wkid; 		workstation identifier
 * int *sgna; 		segment name

   Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p.113
 */


gasgwk_ (wkid,sgna)
int *wkid;
int *sgna;
{

debug ( ("Associate Segment with Workstation %d %d \n", *wkid, *sgna) ); 

gassocsegws ((Gint) *wkid,(Gint) *sgna);

}


/*$F
 *  gcsgwk - Copy Segment to Workstation
 *
 * Parameters :
 * int *wkid; 		workstation identifier
 * int *sgna; 		segment name

   Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p.113
 */


gcsgwk_ (wkid, sgna)
int *wkid;
int *sgna;
{

debug ( ("Copy Segment to Workstation %d %d \n", *wkid, *sgna) ); 

gcopysegws ((Gint)  *wkid,(Gint) *sgna);

}


/*$F
 *  ginsg - Insert Segment
 *
 * Parameters :
 * int   *sgna; 	segment name
 * float *m;	 	transformation matrix	

   Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p.114
 */


ginsg_ (sgna, m)
int   *sgna;
float *m;
{
Gfloat     input_segtran[2][3];
debug ( ("Insert Segment %d  \n", *sgna) );


MOVE_ARRAY_1X6_TO_2X3(m,input_segtran)

ginsertseg ((Gint) *sgna,input_segtran); 

}
