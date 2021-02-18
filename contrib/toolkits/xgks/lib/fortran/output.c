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
 * GKS Output functions :
 *        
 *	gpl_ (n, px, py)
 *	gpm_ (n, px, py)
 *	gtx_ (px, py, chars, length)
 *	gtxs_ (px, py, lstr, chars)
 *	gfa_ (n, px, py)
 *	gca_ (px,py,qx,qy,dimx,dimy,isc,isr,dx,dy,colia)
 *
 * David Berkowitz
 * Bruce Haimowitz
 * TCS Development
 * Cambridge MA
 *
 * August 31 1988
 *
 * $Header: output.c,v 4.0 89/08/31 18:37:06 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/output.c,v $
 * 
 * $Log:	output.c,v $
 * Revision 4.0  89/08/31  18:37:06  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.17  89/06/26  15:55:02  amy
 * DCR d1	Declare errfp external for use in error macros.
 * 		Change references to errfpp to errfp.
 * 
 * Revision 1.16  89/06/16  08:10:37  bruce
 * PTR# c1175:	Corrected calls to realloc that were not storing the
 * 		pointer to the new memory.
 * 
 * Revision 1.15  89/02/04  15:08:30  amy
 * Cosmetic changes to log.
 * 
 * Revision 1.14  89/01/30  09:26:53  mike
 * PTR c2018	The colour index array used by GCA (cellarray) does not need to
 * 		be transposed, so the transposal code was removed.  Since the array      *		can now be passed directly into the C binding, the array no longer 
 * 		needs to be duplicated, so the duplication code was also removed.
 * 
 * Revision 1.13  89/01/14  14:11:53  bruce
 * PTRs c2021 & c2022 Made changes to Polymarker - it wasn't copying points from  
 * 		FORTRAN to C binding correctly.  Also fixed translation of 
 * 		Cellarray colour array from FORTRAN to C. 
 * 
 * 
 * Revision 1.12  88/12/27  14:34:53  todd
 * Added extern's for global vars
 * 
 * Revision 1.11  88/12/05  15:20:22  owens
 * changed errfp to errfpp
 * 
 * Revision 1.9  88/12/05  14:30:19  todd
 * Changes for Code review II.
 * 
 * Revision 1.8  88/11/18  13:58:49  owens
 * code review changes
 * 
 * Revision 1.7  88/11/16  10:20:13  todd
 * Removed extra call to gerrorhand.
 * Added check for ok return code so
 * a return is done before output parameters
 * are set.
 * 
 * Revision 1.6  88/11/10  10:22:29  bhaim
 * Error handling
 * 
 * Revision 1.5  88/11/07  13:16:45  amy
 * Repaired compile bug.
 * 
 * Revision 1.4  88/11/07  12:58:01  amy
 * Added stub for ggdp.
 * 
 * Revision 1.3  88/10/11  17:04:09  todd
 * Added global_errnum initialization.
 * 
 * Revision 1.2  88/10/03  15:31:02  todd
 * *** empty log message ***
 * 
 * Revision 1.1  88/09/27  08:17:25  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: output.c,v 4.0 89/08/31 18:37:06 amy Exp $";

#include "fortxgks.h"
#include <xgks.h>

extern FILE *errfp;             /* d1 */
extern char *forttext;
extern int *fortint;
extern Gpoint *fortpoints;
extern int currforttext;
extern int currfortint;
extern int currfortpoints;

/*$F
 * gpl - Polyline
 *
 * Parameters :
 * int *n;            	Number of points
 * float *px, *py;	Coordinates of points WC
 *
 * Returns error 300 in addition to ANSI standard errors 
 *
 * See also: ANSI standard p.82
 */

gpl_ (n, px, py)
int *n;
float *px, *py;
{
int i;

debug ( ("Polyline %d \n", *n) );

if (*n > currfortpoints) /* Check amount of memory needed */
  {
   fortpoints = (Gpoint *)realloc((char *)fortpoints,   /* c1175 */
                                *n * (sizeof(Gpoint))); /* realloc more memory */
   if (!fortpoints) /* If memory allocation unsuccessful call error routine */
     {
      gerrorhand(300,errgpolyline,(errfp)); 	/* d1 */
      currfortpoints = 0;
      return;
     }
   else
      currfortpoints = *n;
  }


for (i=0; i<(*n); i++)
	{
	fortpoints[i].x = (Gfloat) px[i];
	fortpoints[i].y = (Gfloat) py[i];
        }

gpolyline ((Gint) *n,(Gpoint *) fortpoints);

}


/*$F
 * gpm - Polymarker 
 *
 * Parameters :
 * int *n;		Number of points
 * float *px, *py;	Coordinates of points WC
 *
 * Returns error 300 in addition to ANSI standard errors 
 *
 * See also: ANSI standard p.82
 */


gpm_ (n, px, py)
int *n;
float *px, *py;
{
int i;

debug ( ("Polymarker %d \n", *n) );

if (*n > currfortpoints) /* Check amount of memory needed */
  {
   fortpoints = (Gpoint *)realloc(fortpoints,           /* c1175 */
                                *n * sizeof(Gpoint));   /* realloc more memory */
   if (!fortpoints)/* If memory allocation unsuccessful call error routine */ 
     {
      gerrorhand(300,errgpolymarker,(errfp)); 	/* d1 */
      currfortpoints = 0;
      return;
     }
   else
      currfortpoints = *n;
  }

for (i=0; i<(*n); i++)
	{
	fortpoints[i].x = (Gfloat) px[i];		/* c2022 */
	fortpoints[i].y = (Gfloat) py[i];		/* c2022 */
	}

gpolymarker ((Gint) *n, (Gpoint *) fortpoints);

}



/*$F
 * gtx - Text
 *
 * Parameters :
 * float *px, *py;      Text position WC
 * char *chars;         String of characters 
 * long length;         this parameter is passed by FORTRAN/C conventions 
 *
 * Returns error 300 in addition to ANSI standard errors
 *
 * See also: ANSI standard p.83
 */


gtx_ (px, py, chars, length)
float *px, *py;
char *chars;
long length; /* this parameter is passed by FORTRAN/C conventions */
{
Gpoint position;

debug ( ("Text ") );

/* note: use length+1 because of the NULL at the end */
if (length+1 > currforttext) /* Check amount of memory needed */
  {
   forttext = (Gchar *)realloc(forttext,                        /* c1175 */
                                (length+1) * sizeof(Gchar));    /* realloc more memory */
   if (!forttext) /* If memory allocation unsuccessful call error routine */
     {
      gerrorhand(300,errgtext,(errfp)); 	/* d1 */
      currforttext = 0; 
      return;
     }
   else
      currforttext = length+1;
  }

strncpy ((char *) forttext, chars, length);
forttext[length] = '\0';

debug ( ("<%s> length %d \n", forttext, length) );

position.x = (Gfloat) *px;
position.y = (Gfloat) *py;

gtext (&position,(Gchar *) forttext );

}


/*$F
 * gtxs - Text (Subset Version).
 *
 * Parameters :
 * float *px, *py; 	Text position WC
 * int  *lstr; 		Length of string.
 * char *chars;         Pointer to string
 *
 * Returns error 300 in addition to ANSI standard errors
 *
 * See also: ANSI standard p.83
 */


gtxs_ (px, py, lstr, chars)
float *px, *py;
int  *lstr; 
char *chars;
{
Gpoint position;

debug ( ("Text (Subset Version) ") );

/* note: use *lstr+1 because it's a NULL terminated string */
if (*lstr+1 > currforttext) /* Check amount of memory needed */
  {
   forttext = (Gchar *)realloc(forttext,                        /* c1175 */
                                (*lstr+1)*sizeof(Gchar));       /* realloc more memory */
   if (!forttext) /* If memory allocation unsuccessful call error routine */
     {
      gerrorhand(300,errgtext,(errfp)); 	/* d1 */
      return;
     }
   else
      currforttext = *lstr+1;
  }

strncpy (forttext, chars, *lstr);
forttext[*lstr] = '\0';

debug ( ("<%s> length %d \n", forttext, *lstr) );

position.x = (Gfloat) *px;
position.y = (Gfloat) *py;

gtext (&position,(Gchar *) forttext );

}


/*$F
 * gfa - Fill Area
 *
 * Parameters :
 * int *n;		Number of points
 * float *px, *py;      Coordinates of points WC 
 *
 * Returns error 300 in addition to ANSI standard errors
 *
 * See also: ANSI standard p.83
 */


gfa_ (n, px, py)
int *n;
float *px, *py;
{
int i;

debug ( ("Fill Area %d \n", *n) );

if (*n > currfortpoints) /* Check amount of memory needed */
  {
   fortpoints = (Gpoint *)realloc(fortpoints,                   /* c1175 */
                                *n * sizeof(Gpoint));           /* realloc more memory */
   if (!fortpoints) /* If memory allocation unsuccessful call error routine */
     {
      gerrorhand(300,errgfillarea,(errfp)); 	/* d1 */
      currfortpoints = 0;
      return;
     }
   else
      currfortpoints = *n;
  }

for (i=0; i<(*n); i++)
	{
	fortpoints[i].x = px[i];
	fortpoints[i].y = py[i];
	}

gfillarea ((Gint) *n,(Gpoint *) fortpoints);

}


/*$F
 * gca - Cell Array
 *
 * Parameters :
 * float *px;                        +---------0 <--(*qx,*qy)
 * float *py;                        |         | 
 * float *qx;                        |         | 
 * float *qy;           (*px,*py)--> 0---------+          
 * int   *dimx; 	columns of entire colour index array 
 * int   *dimy;		rows of entire colour index array 
 * int   *isc; 		index of start column
 * int   *isr; 		index of start row
 * int   *dx;      	number of columns in array subset
 * int   *dy;		number of rows in array subset
 * int   *colia;  	colour index array 
 * 
 *
 * Returns error 300 in addition to ANSI standard errors
 *
 * See also: ANSI standard p.85
 */


gca_ (px,py,qx,qy,dimx,dimy,isc,isr,dx,dy,colia)
float *px;
float *py;
float *qx;
float *qy;
int   *dimx;
int   *dimy;
int   *isc;
int   *isr;
int   *dx;
int   *dy;
int   *colia;

{

Grect      rectangle;
Gipoint    dimensions;


debug ( ("Cell Array \n") );

/* c2018:  No longer need our own copy of array, so don't need to allocate. */
 
rectangle.ll.x = (Gfloat) *px;
rectangle.ll.y = (Gfloat) *py;
rectangle.ur.x = (Gfloat) *qx;
rectangle.ur.y = (Gfloat) *qy;

dimensions.x  = (Gint) *dx;
dimensions.y  = (Gint) *dy;

/* Because of the way the standard sets up the color array, it does ** c2018 */
/* NOT have to be transformed from column major to row major.       ** c2018 */
/* We can more or less pass the parameters in verbatim from here.   ** c2018 */

gcellarray (&rectangle, &dimensions,(Gint) *dimx, (colia));  /* c2018 */
 

}

/*$F
 * ggdp - Generalized Drawing Primitive
 *
 * Parameters :
 * int *n;            	Number of points
 * float *pxa, *pya;    Coordinates of points in WC
 * int *primid;    	GDP identifier
 * int *ldr;    	dimension of data record array
 * char *datrec[80,*ldr]; data record

 *
 * See also: ANSI standard p.86
 */

ggdp_ (n, pxa, pya, primid, ldr, datarec)
int *n;
float *pxa, *pya;
int *primid;
int *ldr;
char *datarec;
{
Gint    function;
Gpoint  *points;
Ggdprec *data;
int     i;

/* allocate memory for C Gpoint array */
points = (Gpoint *) malloc ((*n) * sizeof(Gpoint));
if (points = NULL) 
  {
  gerrorhand(300,errggdp,(errfp)); 	/* d1 */
  return;
  }

/* convert from two arrays of floats to an array of points */  
for (i=0; i<*n; i++)
  {
  points[i].x = (Gfloat) pxa[i];
  points[i].y = (Gfloat) pya[i];
  }

function = (Gint) *primid;

/* NOTE: someday this will be a little more complicated */
data = (Ggdprec *) datarec;  /* convert FORTRAN data to C data */

ggdp (*n,points,function,data);
}
