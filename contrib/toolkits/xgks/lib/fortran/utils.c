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
 * GKS Utility functions :
 *	gevtm - evaluate transformation matrix
 *	gactm - accumulate transformation 
 *        
 * David Berkowitz
 * Bruce Haimowitz
 * TCS Development
 * Cambridge MA
 *
 * August 31 1988
 *
 * $Header: utils.c,v 4.0 89/08/31 18:37:34 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/utils.c,v $
 * 
 * $Log:	utils.c,v $
 * Revision 4.0  89/08/31  18:37:34  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.19  89/06/26  16:13:13  amy
 * DCR d1	Declare errfp external for use in error macros.
 * 		Change references to errfpp to errfp.
 * 
 * Revision 1.18  89/04/06  13:41:15  bruce
 * PTR# c2064:	Modified and added #ifdef statements so DEBUG flags
 * 		would not cause problems when disabled.
 * 
 * Revision 1.17  89/03/15  20:40:08  bruce
 * PTR# c2043:	Error checking routine now set errind.
 * 
 * Revision 1.16  89/03/15  15:36:14  bruce
 * PTR c2051:	Corrections to gurec_().
 * 
 * Revision 1.15  89/03/07  17:32:50  amy
 * PTR c2024	gprec:  fixed unclosed comment.
 * 
 * Revision 1.14  88/12/27  14:36:54  todd
 * Added check for size of datarecord.
 * 
 * Revision 1.13  88/12/05  15:20:48  owens
 * changed errfp to errfpp
 * 
 * Revision 1.11  88/12/05  14:30:28  todd
 * Changes for Code review II.
 * 
 * Revision 1.10  88/11/21  08:49:33  owens
 * code review changes
 * 
 * Revision 1.9  88/11/18  11:40:33  owens
 * code review changes
 * 
 * Revision 1.8  88/11/17  11:25:46  todd
 * Corrected casting of *sw in Accumulate Trans Matrix.
 * 
 * Revision 1.7  88/11/16  10:20:45  todd
 * Removed extra call to gerrorhand.
 * Added check for ok return code so
 * a return is done before output parameters
 * are set.
 * 
 * Revision 1.6  88/11/14  06:36:01  todd
 * Added magic number code
 * 
 * Revision 1.5  88/11/07  11:40:03  todd
 * Checked out for testing.
 * 
 * Revision 1.1  88/10/14  10:51:09  david
 * Initial revision
 * 
 * Revision 1.4  88/10/11  17:04:36  todd
 * No changes.
 * 
 * Revision 1.3  88/10/06  09:50:53  todd
 * Added misplaced function gevtm_
 * 
 * Revision 1.2  88/09/27  08:32:24  todd
 * Added include for fortxgks.h 
 * 
 * Revision 1.1  88/09/27  08:17:35  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: utils.c,v 4.0 89/08/31 18:37:34 amy Exp $";

#include "fortxgks.h"
#include <xgks.h>
#include "pdrutils.h"

extern FILE *errfp;             /* d1 */

#define DATARECWIDTH 80 


/*$F
 * gprec - Pack Data Record 
 *
 *   int     *il        -  number of integer entries
 *   int     *ia        -  array containing integers entries
 *   int     *rl        -  number of real entries
 *   float   *ra        -  array containing real entries
 *   int     *sl        -  number of character string entries
 *   int     *lstr      -  lenghts of each character string entry
 *   char    *str       -  character string entries
 *   int     *mldr      -  dimension of data record array
 *   int     *errind    -  error indicator
 *   int     *ldr       -  number of array elements used in datarec
 *   char    *datarec   -  data record
 *
 */
gprec_(il, ia, rl, ra, sl, lstr, str, mldr, errind, ldr, datarec)
int     *il;
int     *ia;
int     *rl;
float   *ra;
int     *sl;
int     *lstr;
char    *str;
int     *mldr;
int     *errind;
int     *ldr;
char    *datarec;

{   int i,j;
    Gdatarec *pdr;
    int *intptr;
    float *realptr;
    char *fortstrptr, *strptr;
    char **strptrptr;
    int space;

    debug ( ("Pack Data Record \n") );

    {
	int intspace, realspace, stringspace, headerspace, i;

	/* calculate space needed (in bytes) */
	/* header  */
	headerspace =  sizeof(int)	/* Magic Number		*/
          + sizeof(int)			/* # of ints		*/
	  + sizeof(int *)		/* -> to ints		*/
	  + sizeof(int)			/* # of reals		*/
	  + sizeof(float *)		/* -> to reals		*/
	  + sizeof(int)			/* # of strings		*/
	  + sizeof(char **)		/* -> to ->s to strings	*/
	  + *sl	* sizeof(char **);	/* ->s to strings		*/ 
	debug ( ("    header needs %d bytes \n", headerspace) );

	/* integers */
	intspace  = *il * sizeof(int);
	debug ( ("    %d integers need %d bytes \n", *il, intspace) );

	/* reals */
	realspace = *rl * sizeof(float);
	debug ( ("    %d reals need %d bytes \n", *rl, realspace) );

	/* strings */
	stringspace = 0;
	for (i=0; i<*sl; i++)
	    stringspace += (lstr[i] + 1);
	debug ( ("    %d strings need %d bytes \n", *sl, stringspace) );

	space = headerspace + intspace + realspace + stringspace;
	debug ( ("    total space needed id %d bytes \n", space) );
    }
    
    if (space > (*mldr)*80)
      { 
       *errind = 2001;          /* c2043 */
       gerrorhand(2001,errgprec,(errfp)); 	/* d1 */
       return;
      }

    /* let pdr point to the passed in data area */
    pdr = (Gdatarec *) datarec;
    debug ( ("    pdr is at 0x%x \n", pdr) );

    /* set "magic number" so we can tell data is not garbage" */ /* c2024 */
    pdr->magicnum = MAGICNUMBER;
    pdr->nint = *il;
    pdr->nfloat = *rl;
    pdr->nstring = *sl;

    /* setup pointer to beginning of integers */
    intptr = (int *) &(pdr->data);
    debug ( ("    data starts at 0x%x \n", intptr) );

    /* fill in pointer to integers    */
    pdr->intptr = (*il > 0) ? intptr : NULL;

    /* fill in the integer array */
    debug ( ("    ints: ") );
    for (i=0; i<*il; i++)
    {
	*(intptr++) = ia[i];
	debug ( ("%d ", ia[i]) );
    }
    debug ( ("\n") );

    /* setup pointer to beginning of reals */
    realptr = (float *) intptr;

    /* fill in pointer to reals  */ 
    pdr->floatptr = (*rl > 0) ? realptr : NULL;

    /* fill in the reals */
    debug ( ("    reals: ") );
    for (i=0; i<*rl; i++)
    {
	*(realptr++) = ra[i];
	debug ( ("%7.2f ", ra[i]) );
    }
    debug ( ("\n") );

    /* setup pointer to beginning of string pointers */
    pdr->strptr = (*sl) ? (char **) realptr : NULL;
    strptrptr = (char **) realptr;

    /* point past the string pointers */
    strptr = (char *) strptrptr + *sl * sizeof(char *);
    fortstrptr = str;

    /* for each string */
    debug ( ("    strings: \n") );
    for (i=0; i<*sl; i++) 
    {
#ifdef DEBUG
	char *debugptr = strptr;
#endif DEBUG

	/* fill in pointer to string */
	*(strptrptr++) = strptr;

	/* copy the string ... */
	for (j=0; j<lstr[i]; j++) 
	    *(strptr++) = *(fortstrptr++);

	/* ... and NULL terminate it */
	*(strptr++) = '\0';

	debug ( ("        %d <%s> \n", lstr[i], debugptr) );
    }

#ifdef DEBUG            /* c2064 */
    dumppdr (datarec);
#endif                  /* c2064 */

    *errind = 0;
    
    /* note: although a little confusing, the +1 and -1 are correct */
    /*       (pos-1)/width+1                                        */ 
    *ldr   = ((strptr - datarec -1 ) / DATARECWIDTH) + 1;
    debug ( ("    returning ldr %d \n", *ldr) );
}

/*$F
 * gurec - unpack data record
 *
 * Input Parameters:
 *
 * int *ldr		number of array elements used in datarec
 * char *datarec	data record
 * int *ill		dimension of integer array
 * int *irl		dimension of real array
 * int *isl 		dimension of char array
 *
 *
 *
 * Output Parameters:
 *
 * int *errind		error indicator
 * int *il		number of integer entries
 * int *ia		array containing integer entries
 * int *rl		number of real entries
 * int *ra 		array containing real entries
 * int *sl		number of character string entries
 * int *lstr		length of each character string entry
 * char *str		char string entries
 *
 */

gurec_(ldr,datarec,ill,irl,isl,errind,il,ia,rl,ra,sl,lstr,str)
int *ldr;
char *datarec;
int *ill;
int *irl;
int *isl;
int *errind;
int *il;
int *ia;
int *rl;
int *ra;
int *sl;
int *lstr;
char *str;
{
 int i;
 char **strptr;

 debug(("Unpack Data Record ill = %d irl = %d isl = %d \n",*ill,*irl,*isl));

 /* check "magic number" so we can tell data is not garbage"*/
 if (pdrmagicnum(datarec) != MAGICNUMBER)
   {
    *errind = 2003;             /* c2043 */
    gerrorhand(2003,errgurec,(errfp)); 	/* d1 */
    return;
   } 
 if (*ill < (pdrintnum(datarec))|| *irl < (pdrrealnum(datarec)) || *isl < (pdrstringnum(datarec))) /* c2051 */
   {   
    *errind = 2001;             /* c2043 */
    gerrorhand(2001,errgurec,(errfp)); 	/* d1 */
    return;
   }

 *il = pdrintnum(datarec);
 *rl = pdrrealnum(datarec);
 *sl = pdrstringnum(datarec);

 for(i=0; i < *il; i++)
  ia[i] = (int) pdrintindex(datarec,i);
 
 for(i=0; i < *rl; i++)
  ra[i] = (float) pdrrealindex(datarec,i);
 
 strptr = (char **) str;

 for(i=0; i < *sl; i++)
  {
   strptr[i] =  pdrstringindex(datarec,i);
   lstr[i] = (int) strlen(strptr[i]);
  }
 
}

/*$F
 * gevtm - evaluate transformation matrix
 *
 * float *x0   - reference point X
 * float *y0   - reference point Y 
 * float *dx   - shift x
 *
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.193
 */
gevtm_ (x0,y0,dx,dy,phi,fx,fy,sw,mout)
float *x0;
float *y0;
float *dx;
float *dy;
float *phi;
float *fx;
float *fy;
int   *sw;
float *mout;

{
Gpoint ppoint,pshift,pscale;
Gfloat result[2][3];

debug ( ("Evaluate Transformation Matrix \n") );
 
COORDINATESWITCH(*sw,errgevaltran);
 
ppoint.x = *x0;
ppoint.y = *y0;
pshift.x = *dx;
pshift.y = *dy;
pscale.x = *fx;
pscale.y = *fy;
 
if (gevaltran(&ppoint,&pshift, (Gfloat) *phi,&pscale, (Gcsw) *sw, (Gfloat *) result)) return;
 
MOVE_ARRAY_2X3_TO_1X6 (result,mout);

}

/*$F
 * gactm - accumulate transformation 
 *
 * float *minp - input segment transformation
 * float *x0   - reference point X
 * float *y0   - reference point Y 
 * float *dx   - shift x
 * float *dy   - shift y
 * float *phi  - rotation angle in radians 
 * float *fx   - scaling factor X
 * float *fy   - scaling factor Y
 * int   *sw   - coordinate switch GWC | GNDC
 * float *mout - resulting transformation matrix
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.193
 */
gactm_ (minp, x0, y0, dx, dy, phi, fx, fy, sw, mout)
float *minp;
float *x0;
float *y0;
float *dx;
float *dy;
float *phi;
float *fx;
float *fy;
int   *sw;
float *mout;

{
Gpoint ppoint,pshift,pscale;
Gfloat temp_array[2][3];
Gfloat result[2][3];

debug ( ("Accumulate Transformation Matrix \n") );

COORDINATESWITCH(*sw,errgaccumtran);

ppoint.x = *x0;
ppoint.y = *y0;
pshift.x = *dx;
pshift.y = *dy;
pscale.x = *fx;
pscale.y = *fy;
 
MOVE_ARRAY_1X6_TO_2X3 (minp,temp_array);
 
if (gaccumtran ( (Gfloat *) temp_array, &ppoint, &pshift, (Gfloat) *phi, &pscale, (Gint) *sw, (Gfloat *) result)) return;
 
MOVE_ARRAY_2X3_TO_1X6 (result,mout);

}
    
