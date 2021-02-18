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
 * GKS Input Request Functions:
 *	grqlc_ ()
 *	grqsk_ ()
 *	grqvl_ ()
 *	grqch_ ()
 *	grqpk_ ()
 *	grqst_ ()
 *	grqsts_ ()
 *        
 * David Berkowitz
 * Bruce Haimowitz
 * TCS Development
 * Cambridge MA
 *
 * August 31 1988
 *
 * $Header: inputreq.c,v 4.0 89/08/31 18:31:08 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/inputreq.c,v $
 * 
 * $Log:	inputreq.c,v $
 * Revision 4.0  89/08/31  18:31:08  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.11  89/06/26  15:28:28  amy
 * DCR d1	Declare errfp external for use in error macros.  
 * 		Change references to errfpp to errfp.
 * 
 * Revision 1.10  89/06/16  08:10:04  bruce
 * PTR# c1175:	Corrected calls to realloc that were not storing the
 * 		pointer to the new memory.
 * 
 * Revision 1.9  89/05/04  17:49:57  bruce
 * PTR# c2065:	Changes were made to handle cases where more points
 * 		are given to device than alloted space allows.
 * 
 * Revision 1.8  89/03/31  11:15:57  bruce
 * Changed debug statement.
 * 
 * Revision 1.7  89/01/06  09:40:20  todd
 * Added extern's.
 * 
 * Revision 1.6  88/12/05  15:19:06  owens
 * changed errfp to errfpp
 * 
 * Revision 1.4  88/12/05  14:32:27  todd
 * Changes for code review II.
 * 
 * Revision 1.3  88/11/22  16:04:27  bhaim
 * Code Review changes
 * 
 * Revision 1.2  88/10/11  17:00:58  todd
 * Added Global_errnum initialization.
 * 
 * Revision 1.1  88/09/27  08:17:03  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: inputreq.c,v 4.0 89/08/31 18:31:08 amy Exp $";

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
 * grqlc - Request Locator 
 *
 * Parameters :
 * int *wkid;		workstation identifier
 * int *lcdnr;		locatordevice number
 * int *stat;		status
 * int *tnr;		normalization transformation number
 * float *px;		locator x position
 * float *py;		locator y position

 *
 * Returns:  ANSI standard errors for this function.
 * See also: ANSI standard p.131
 */


grqlc_ (wkid, lcdnr, stat, tnr, px, py)
int *wkid;
int *lcdnr;
int *stat;
int *tnr;
float *px;
float *py;

{
Gqloc response;

debug ( ("Request Locator %d %d \n", *wkid, *lcdnr) );

if (greqloc ((Gint) *wkid,(Gint) *lcdnr, &response)) return; 
  
/* Reverse the sense of status */
*stat = (int) (response.status) ? FORT_GNONE : FORT_GOK;

*tnr  = (int) response.loc.transform;
*px   = (float) response.loc.position.x;
*py   = (float) response.loc.position.y;

}


/*$F
 * grqsk - Request Stroke 
 *
 * Parameters :
 * int *wkid; 		workstation identifier
 * int *skdnr;		stroke device number
 * int *n; 		maximum number of points
 * int *stat;		status
 * int *tnr; 		normalization transformation number
 * int *np; 		number of points
 * float *pxa;          x points in stroke
 * float *pya;          y points in stroke

 * Returns: ANSI standard errors for this function.
 * See also: ANSI standard p.132
 */


grqsk_ (wkid, skdnr, n, stat, tnr, np, pxa, pya)
int *wkid;
int *skdnr;
int *n;
int *stat;
int *tnr;
int *np;
float *pxa;
float *pya;

{
Gqstroke response;
int i,temp;	/* c2065 */

debug ( ("Request Stroke %d %d \n", *wkid, *skdnr) );
debug ( ("number of points = %d buffsize = %d \n", *np, *n) );

if (*n > currfortpoints)       /* Check amount of memory needed */
  {
   fortpoints = (Gpoint *)realloc(fortpoints,                   /* c1175 */
                                (*n) * sizeof(Gpoint));         /* realloc more memory */
   if (fortpoints == NULL)     /* If memory allocation unsuccessful call error routine */ 
     {
      gerrorhand(300,errgreqstroke,(errfp)); 	/* d1 */
      currfortpoints = 0;
      return;
     }
   else
      currfortpoints = *n;
  }

response.stroke.points = (Gpoint *) fortpoints;

if (greqstroke ((Gint) *wkid,(Gint) *skdnr, &response)) return; 

/* Reverse the sense of status */
*stat = (int) (response.status) ? FORT_GNONE : FORT_GOK;
*tnr  = (int) response.stroke.transform;
*np   = (int) response.stroke.n_points;

if (*np > *n)  /* c2065 */
   temp = *n;
else
   temp = *np;

for (i=0; i<(temp); i++)	/* c2065 */
	{
	pxa[i] = (float) response.stroke.points[i].x;
	pya[i] = (float) response.stroke.points[i].y;
	}
debug ( ("Request Stroke Completed") );

}


/*$F
 * grqvl - Request Valuator 
 *
 * Parameters :
 * int *wkid;		workstation identifier
 * int *vldnr; 		valuator device number
 * int *stat;		status
 * int *val; 		value

 *
 * Returns: ANSI standard errors for this function.
 * See also: ANSI standard p.133
 */


grqvl_ (wkid, vldnr, stat, val)
int *wkid;
int *vldnr;
int *stat;
float *val;

{
Gqval response;

debug ( ("Request Valuator %d %d \n", *wkid, *vldnr) );

if (greqval ((Gint) *wkid,(Gint) *vldnr, &response)) return; 
  
/* Reverse the sense of status */
*stat = (int) (response.status) ? FORT_GNONE : FORT_GOK;

*val  = (float) response.val;

}


/*$F
 * grqch - Request Choice 
 *
 * Parameters :
 * int *wkid; 		workstation identifier
 * int *chdnr;  	choice device number
 * int *stat; 		status
 * int *chnr;		choice number

 *
 * Returns: ANSI standard errors for this function.
 * See also: ANSI standard p.133
 */


grqch_ (wkid, chdnr, stat, chnr)
int *wkid;
int *chdnr;
int *stat;
int *chnr;

{
Gchoice response;
debug ( ("Request Choice %d %d \n", *wkid, *chdnr) );

if (greqchoice ((Gint) *wkid, (Gint) *chdnr, &response)) return;
debug ( (" status before switch  response.status = %d  response.choice = %d \n", response.status, response.choice) );

switch (response.status)
    {
    case GC_OK : *stat = (int) FORT_GOK;
	    break;
    case GC_NOCHOICE : *stat = (int) FORT_GNCHOI;
	    break; 
    case GC_NONE : *stat = (int) FORT_GNONE;
	    break;
    }

*chnr  = (int) response.choice;
}


/*$F
 * grqpk - Request Pick 
 *
 * Parameters :
 * int *wkid; 		workstation identifier
 * int *pkdnr; 		pick device number
 * int *stat;		status 
 * int *sgna;		segment name
 * int *pkid; 		pick identifier

 *
 * Returns: ANSI standard errors for this function.
 * See also: ANSI standard p.134
 */


grqpk_ (wkid, pkdnr, stat, sgna, pkid)
int *wkid;
int *pkdnr;
int *stat;
int *sgna;
int *pkid;
{
Gpick response;

debug ( ("Request Pick %d %d \n", *wkid, *pkdnr) );

if (greqpick ((Gint) *wkid,(Gint) *pkdnr, &response)) return;
  
switch (response.status)
    {
    case GP_OK: *stat = (int) FORT_GOK;
    break;
    case GP_NOPICK: *stat = (int) FORT_GNPICK;
    break;
    case GP_NONE : *stat = (int) FORT_GNONE;
    break;
    }

*sgna    = (int) response.seg;
*pkid    = (int) response.pickid;

}


/*$F
 * grqst - Request String
 *
 * Parameters :
 * int *wkid; 		workstation identifier
 * int *stdnr; 		string device number
 * int *stat;		status
 * int *lostr;		number of characters returned
 * char *str;		character string

 *
 * Returns: ANSI standard errors for this function.
 * See also: ANSI standard p.134
 */

grqst_ (wkid, stdnr, stat, lostr, str, length)
int *wkid;
int *stdnr;
int *stat;
int *lostr;
char *str;
long length;
{
Gqstring response;

debug ( ("Request String %d %d \n", *wkid, *stdnr) );

if (length > currforttext)
   {
    forttext = (Gchar *)realloc(forttext,(length) * sizeof(char));      /* c1175 */
    if (forttext == NULL)
       { 
 	gerrorhand(300,errgreqstring,(errfp)); 	/* d1 */
        currforttext = 0;
        return;
       }
    else
        currforttext = (length);
    }
response.string = (Gchar *) forttext;

if (greqstring ((Gint) *wkid,(Gint) *stdnr, &response)) return; 
  
switch (response.status)
    {
    case GOK: *stat = (int) FORT_GOK;
    break;
    case GNONE: *stat = (int) FORT_GNONE;
    break;
    }

debug(("response string = %s ",response.string));

*lostr = (int) MIN(length,strlen(response.string));
strncpy(str,(char *) response.string,*lostr);
  
}

/*$F
 * grqsts - Request String (Subset Version)
 *
 * Parameters :
 * int *wkid; 		workstation identifier
 * int *stdnr; 		string device number
 * int *stat;		status
 * int *lostr;		number of characters returned
 * char *str;		character string

 *
 * Returns: ANSI standard errors for this function.
 * See also: ANSI standard p.134
 */

grqsts_ (wkid, stdnr, stat, lostr, str, length)
int *wkid;
int *stdnr;
int *stat;
int *lostr;
char *str;
long length;
{
Gqstring response;

debug ( ("Request String (Subset Version)  %d %d \n", *wkid, *stdnr) );

if (length > currforttext)
   {
    forttext = (Gchar *)realloc(forttext,(length) * sizeof(char));      /* c1175 */
    if (forttext == NULL)
       { 
 	gerrorhand(300,errgreqstring,(errfp)); 	/* d1 */
        currforttext = 0;
        return;
       }
    else
        currforttext = (length);
    }
response.string = (Gchar *) forttext;

if (greqstring ((Gint) *wkid,(Gint) *stdnr, &response)) return; 
  
switch (response.status)
    {
    case GOK: *stat = (int) FORT_GOK;
    break;
    case GNONE: *stat = (int) FORT_GNONE;
    break;
    }

debug(("response string = %s ",response.string));

*lostr = MIN(80,strlen(response.string));  
strncpy(str,(char *) response.string,*lostr);

}
