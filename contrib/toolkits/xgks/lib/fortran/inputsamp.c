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
 * GKS Sample Input functions :
 *	gsmlc_
 *	gsmsk_
 *	gsmvl_
 *	gsmch_
 *	gsmpk_
 *	gsmst_
 *        
 * David Berkowitz
 * Bruce Haimowitz
 * TCS Development
 * Cambridge MA
 *
 * August 31 1988
 *
 * $Header: inputsamp.c,v 4.0 89/08/31 18:31:12 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/inputsamp.c,v $
 * 
 * $Log:	inputsamp.c,v $
 * Revision 4.0  89/08/31  18:31:12  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.14  89/06/26  15:31:37  amy
 * DCR d1	Declare errfp external for use in error macros.
 * 		Change reference to errfpp to errfp.
 * 
 * Revision 1.13  89/06/16  08:10:09  bruce
 * PTR# c1175:	Corrected calls to realloc that were not storing the
 * 		pointer to the new memory.
 * 
 * Revision 1.12  89/05/04  17:48:49  bruce
 * PTR# c2065:	Changes were made to handle cases where more points
 * 		are given to device than alloted space allows.
 * 
 * Revision 1.11  88/12/30  09:52:12  todd
 * Added externs
 * 
 * Revision 1.10  88/12/05  15:19:11  owens
 * changed errfp to errfpp
 * 
 * Revision 1.8  88/12/05  14:32:36  todd
 * Changes for code review II.
 * 
 * Revision 1.7  88/11/21  09:28:45  todd
 * Removed extra includes.
 * 
 * Revision 1.6  88/11/16  10:16:52  todd
 * Removed extra call to gerrorhand.
 * Added check for ok return code so
 * a return is done before output parameters
 * are set.
 * 
 * Revision 1.5  88/11/14  07:21:59  todd
 * No changes
 * 
 * Revision 1.4  88/11/14  07:15:47  bhaim
 * Error handling
 * 
 * Revision 1.3  88/10/13  12:29:21  bhaim
 * Changes for Code II
 * 
 * Revision 1.2  88/09/27  17:10:24  mike
 * Fixed module prologue
 * 
 * Revision 1.1  88/09/27  08:17:05  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: inputsamp.c,v 4.0 89/08/31 18:31:12 amy Exp $";

#include <xgks.h>
#include "fortxgks.h" 

extern FILE *errfp;             /* d1 */
extern char *forttext;
extern int *fortint;
extern Gpoint *fortpoints;
extern int currforttext;
extern int currfortint;
extern int currfortpoints;


/*$F
 * gsmlc - Sample Locator 
 *
 * Parameters :
 * int *wkid;		workstation identifier
 * int *lcdnr;		locatordevice number
 * int *tnr;		normalization transformation number
 * float *lpx;		locator x position
 * float *lpy;		locator y position

 *
 * See also: ANSI standard p.135
 */
gsmlc_ (wkid,lcdnr,tnr,lpx,lpy)   
int *wkid;
int *lcdnr;
int *tnr;
float *lpx;
float *lpy;
{
Gloc response;

debug ( ("Sample Locator %d %d \n", *wkid, *lcdnr) );

if (gsampleloc ((Gint) *wkid,(Gint) *lcdnr, &response)) return; 
  

*tnr  = (int) response.transform;
*lpx   = (float) response.position.x;
*lpy   = (float) response.position.y;
}

/*$F
 * gsmsk - Sample Stroke 
 *
 * Parameters :
 * int *wkid; 		workstation identifier
 * int *skdnr;		stroke device number
 * int *n; 		maximum number of points
 * int *tnr; 		normalization transformation number
 * int *np; 		number of points
 * float *pxa;          x points in stroke
 * float *pya;          y points in stroke

 * See also: ANSI standard p.137 
 */

gsmsk_ (wkid,skdnr,n,tnr,np,pxa,pya)   
int *wkid;
int *skdnr;
int *n;
int *tnr;
int *np;
float *pxa;
float *pya;
{
Gstroke response;
int i,temp;	/* c2065 */

debug ( ("Sample Stroke %d %d \n", *wkid, *skdnr) );
debug ( ("number of points = %d buffsize = %d \n", *np, *n) );  /* c2065 */

if (*n > currfortpoints)     /*c2065*/  /* Check amount of memory needed */
  {
   fortpoints = (Gpoint *)realloc(fortpoints,           /* c1175 */
				(*n) * sizeof(Gpoint)); /* c2065 */ /* realloc more memory */
   if (fortpoints == NULL) /* If memory allocation unsuccessful, error  c2065 */ 
     {
      gerrorhand(300,errgsamplestroke,(errfp)); 	/* d1 */
      return;
     }
   else
      currfortpoints = *n;	/* c2065 */
  }

response.points = (Gpoint *) fortpoints;

if (gsamplestroke ((Gint) *wkid,(Gint) *skdnr, &response)) return; 

*tnr  = (int) response.transform;
*np   = (int) response.n_points;

if ( *np > *n )  /* c2065 */
   temp = *n;
else
   temp = *np;

for (i=0; i<(temp); i++)  /* c2065 */
	{
	pxa[i] = (float) response.points[i].x;
	pya[i] = (float) response.points[i].y;
	}
debug ( ("Sample Stroke Completed") );
}

/*$F
 * grqvl - Request Valuator 
 *
 * Parameters :
 * int *wkid;		workstation identifier
 * int *vldnr; 		valuator device number
 * float *val; 		value

 *
 * See also: ANSI standard  p.133
 */

gsmvl_ (wkid,vldnr,val)   
int *wkid;
int *vldnr;
float *val;
{
Gfloat response;

debug ( ("Request Valuator %d %d \n", *wkid, *vldnr) );

if (gsampleval ((Gint) *wkid,(Gint) *vldnr, (Gfloat *)&response)) return; 
  
*val  = (float) response;
}

/*$F
 * gsmch - Sample Choice 
 *
 * Parameters :
 * int *wkid; 		workstation identifier
 * int *chdnr; 		choice device number
 * int *stat;		status
 * int *chnr; 		choice identifier

 *
 * See also: ANSI standard  p.136
 */

gsmch_ (wkid,chdnr,stat,chnr)   
int *wkid;
int *chdnr;
int *stat;
int *chnr;
{
Gchoice response;
debug ( ("Sample Choice %d %d \n", *wkid, *chdnr) );

if (gsamplechoice ((Gint) *wkid, (Gint) *chdnr, &response)) return;
switch(response.status)
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
 * gsmpk - Sample Pick 
 *
 * Parameters :
 * int *wkid; 		workstation identifier
 * int *pkdnr; 		pick device number
 * int *stat;		status 
 * int *sgna;		segment name
 * int *pkid; 		pick identifier

 *
 * See also: ANSI standard  p.137
 */

gsmpk_ (wkid,pkdnr,stat,sgna,pkid)   
int *wkid;
int *pkdnr;
int *stat;
int *sgna;
int *pkid;
{
Gpick response;

debug ( ("Sample Pick %d %d \n", *wkid, *pkdnr) );

if (gsamplepick ((Gint) *wkid,(Gint) *pkdnr, &response)) return;
  
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
 * gsmst - Sample String
 *
 * Parameters :
 * int *wkid; 		workstation identifier
 * int *stdnr; 		string device number
 * int *lostr;		number of characters returned
 * char *str;		character string

 *
 * See also: ANSI standard p.134
 */
gsmst_ (wkid,stdnr,lostr,str)   
int *wkid;
int *stdnr;
int *lostr;
char *str;
{
Gchar *response;

debug ( ("Sample String  %d %d \n", *wkid, *stdnr) );


response = (Gchar *) str;
if (gsamplestring ((Gint) *wkid,(Gint) *stdnr, response)) return; 
  
debug(("response string = %s ",str));

*lostr = strlen(response);
  
}


/*$F
 * gsmst - Sample String (Subset version)
 *
 * Parameters :
 * int *wkid; 		workstation identifier
 * int *stdnr; 		string device number
 * int *lostr;		number of characters returned
 * char *str;		character string

 *
 * See also: ANSI standard p.134
 */
gsmsts_ (wkid,stdnr,lostr,str)   
int *wkid;
int *stdnr;
int *lostr;
char *str;
{
Gchar *response;

debug ( ("Sample String (Subset Version)  %d %d \n", *wkid, *stdnr) );


response = (Gchar *) str;
if (gsamplestring ((Gint) *wkid,(Gint) *stdnr, response)) return; 
  
debug(("response string = %s ",str));

*lostr = MIN(80,strlen(response));
  
}
