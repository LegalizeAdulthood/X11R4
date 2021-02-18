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
 * GKS Event Input functions :
 *	gwait_
 *	gflush_
 *	ggtlc_
 *	ggtsk_
 *	ggtvl_
 *	ggtch_
 *	ggtpk_
 *	ggtst_
 *        
 * David Berkowitz
 * Bruce Haimowitz
 * TCS Development
 * Cambridge MA
 *
 * August 31 1988
 *
 * $Header: inputevent.c,v 4.0 89/08/31 18:29:01 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/inputevent.c,v $
 * 
 * $Log:	inputevent.c,v $
 * Revision 4.0  89/08/31  18:29:01  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 4.0  89/08/31  18:18:57  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.16  89/06/23  17:24:23  amy
 * DCR d1	Changed references to errfpp to errfp.
 * 
 * Revision 1.15  89/06/16  08:09:54  bruce
 * PTR# c1175:	Corrected calls to realloc that were not storing the
 * 		pointer to the new memory.
 * 
 * Revision 1.14  89/05/04  17:50:02  bruce
 * PTR# c2065:	Changes were made to handle cases where more points
 * 		are given to device than alloted space allows.
 * 
 * Revision 1.13  89/04/03  09:57:00  bruce
 * PTR# c2063:	Corrected variable used for getstroke.
 * 
 * Revision 1.12  89/03/07  17:03:55  amy
 * PTR c2040	ggtch:  added switch statement on status returned that uses
 * 		correct enumerated type values.
 * 		ggtpk:  same as for ggtch.
 * 
 * Revision 1.11  88/12/27  14:34:05  todd
 * Added extern's for global var's.
 * 
 * Revision 1.10  88/12/05  15:18:44  owens
 * changed errfp to errfpp
 * 
 * Revision 1.8  88/12/05  14:33:03  todd
 * Changes for code review II.
 * 
 * Revision 1.7  88/11/16  10:21:14  todd
 * Removed extra call to gerrorhand.
 * 
 * Revision 1.6  88/10/13  12:30:39  bhaim
 * Changes for Code II
 * 
 * Revision 1.5  88/10/11  17:00:45  todd
 * Added Global_errnum initialization.
 * 
 * Revision 1.4  88/10/03  15:31:11  mike
 * Minor corrections made after Amy\'s review
 * 
 * Revision 1.3  88/09/27  17:00:38  mike
 * Fixed Code Review problems: prologues, casts, variable names, and range checks
 * 
 * Revision 1.2  88/09/27  12:59:43  todd
 * Added includes 
 * 
 * Revision 1.1  88/09/27  08:16:57  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: inputevent.c,v 4.0 89/08/31 18:29:01 amy Exp $";

#include "fortxgks.h"
#include <xgks.h>

extern FILE *errfp;			/* d1 */
extern char *forttext;
extern int *fortint;
extern Gpoint *fortpoints;
extern int currforttext;
extern int currfortint;
extern int currfortpoints;


/*$F
 * gwait - Await Event
 *
 * float *tout - pointer to timeout (seconds)
 * int *wkid - pointer to workstation identifier
 * int *icl - pointer to input class
 * int *idnr - pointer to logical input device number
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.138
 */
gwait_ (tout, wkid, icl, idnr)
float *tout;
int *wkid;
int *icl;
int *idnr;

{
Gevent event;

debug ( ("Await Event %d %d %d %f \n", *wkid, *icl, *idnr, *tout) );

if (gawaitevent ((Gfloat)*tout, &event)) return; 

*wkid = (int)event.ws;
*icl  = (int)event.class; 
*idnr = (int)event.dev;

debug ( ("Await Event input class = %d  \n",*icl) );

}

/*$F
 * gflush - Flush Device Events
 *
 * int *wkid - pointer to workstation identifier
 * int *icl - pointer to input class
 * int *idnr - pointer to logical input device number
 *
 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.139
 */
gflush_ (wkid, icl, idnr)
int *wkid;
int *icl;
int *idnr;

{

debug ( ("Flush Device Input class = %d device = %d \n",*icl,*idnr) );

INPUTCLASS (*icl, errgflushevents);

gflushevents ((Gint)*wkid, (Giclass)*icl, (Gint)*idnr); 

}

/*$F
 * ggtlc - Get Locator 
 *
 * Parameters :

 * int *tnr;		normalization transformation number
 * float *lpx;		locator x position
 * float *lpy;		locator y position

 *
 * See also: ANSI standard 
 */
ggtlc_ (tnr,lpx,lpy) 
int *tnr;
float *lpx;
float *lpy; 
{ 
Gloc response;


if (ggetloc (&response)) return; 

*tnr  = (int) response.transform;
*lpx   = (float) response.position.x;
*lpy   = (float) response.position.y;
}

/*$F
 * ggtsk -  Get Stroke 
 *
 * Parameters :

 * int *n; 		maximum number of points
 * int *tnr; 		normalization transformation number
 * int *np; 		number of points
 * float *pxa;          x points in stroke
 * float *pya;          y points in stroke

 * See also: ANSI standard 
 */
ggtsk_ (n,tnr,np,pxa,pya)
int *n;
int *tnr;
int *np;
float *pxa;
float *pya;  
{ 
Gstroke response;
int i,temp;	/* c2065 */

debug ( ("Get Stroke \n") ); /* 2063 */
debug ( ("number of points = %d buffsize = %d \n", *np, *n) );/* c2063 */

if (*n > currfortpoints)  /* c2063 */     /* Check amount of memory needed */
  {
   fortpoints = (Gpoint *)realloc(fortpoints,           /* c1175 */
                        (*n) * sizeof(Gpoint) );        /* c2063 */  /* realloc more memory */
   if (fortpoints == NULL) /* c2063 */    /* If memory allocation unsuccessful call error routine */
     {
      gerrorhand(300,errggetstroke,(errfp));		/* d1 */
      return;
     }
   else
      currfortpoints = *n;
  }

response.points = (Gpoint *) fortpoints;
if (ggetstroke (&response)) return; 

*tnr  = (int) response.transform;
*np   = (int) response.n_points;

if ( *np > *n )   /* c2065 */
   temp = *n;
else
   temp = *np;

for (i=0; i<(temp); i++)    /* c2065 */
	{
	pxa[i] = (float) response.points[i].x;
	pya[i] = (float) response.points[i].y;
	}
debug ( ("Get Stroke Completed") );
}

/*$F
 * grqvl - Request Valuator 
 *
 * Parameters :
 * float *val; 		value

 *
 * See also: ANSI standard 
 */
ggtvl_ (val)  
float *val;
{
Gfloat response;

debug ( ("Request Valuator \n") );

if (ggetval ((Gfloat *)&response)) return; 
  
*val  = (float) response;
}

/*$F
 * ggtch - Get Choice
 *
 * int *stat - pointer to status
 * int *chnr - pointer to choice number
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard 
 */
ggtch_ (stat, chnr)
int *stat;
int *chnr;
{
Gchoice response;

debug ( ("Get Choice \n") );

if (ggetchoice (&response)) return; 

switch (response.status)       /* C2040 added switch */
{
    case GC_OK : *stat = (int) FORT_GOK;
            break;
    case GC_NOCHOICE : *stat = (int) FORT_GNCHOI;
            break;
    case GC_NONE : *stat = (int) FORT_GNONE;
            break;
 }
*chnr  = (int)response.choice;

debug ( ("  Get Choice  returning status %d choice %d \n", *stat, *chnr) );
}

/*$F
 * ggtpk - Get Pick
 *
 * int *stat - pointer to status
 * int *sgna - pointer to segment name
 * int *pkid - pointer to pick identifier
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.140
 */
ggtpk_ (stat, sgna, pkid)
int *stat;
int *sgna;
int *pkid;
{
Gpick response;

debug ( ("Get Pick \n") );

if (ggetpick (&response)) return; 

switch (response.status)       /* C2040 added switch */
{
    case GP_OK : *stat = (int) FORT_GOK;
            break;
    case GP_NOPICK : *stat = (int) FORT_GNPICK;
            break;
    case GP_NONE : *stat = (int) FORT_GNONE;
            break;
} 
*sgna  = (int)response.seg;
*pkid  = (int)response.pickid;

debug ( ("    returning status %d seg %d pickid %d \n", *stat, *sgna, *pkid) );
}

/*$F
 * ggtst - Get String
 *
 * Parameters :
 * int *lostr;		number of characters returned
 * char *str;		character string

 *
 * See also: ANSI standard 
 */
ggtst_ (lostr,str,length) 
int *lostr;
char *str; 
long length;
{ 
Gchar *response;

debug ( ("Get String \n") );


if (length > currforttext)   /* Check amount of memory needed */
  {
   forttext = (Gchar *)realloc(forttext,                /* c1175 */
                        length * sizeof(char));         /* realloc more memory */
   if (forttext == NULL)     /* If memory allocation unsuccessful call error routine */ 
     {
      gerrorhand(300,errggetstring,(errfp));		/* d1 */
      return;
     }
   else
      currforttext = length;
  }
response = (Gchar *) forttext;

if (ggetstring (response)) return; 
  
debug(("response string = %s ",response));

*lostr = MIN(80,strlen(response));
strncpy (str,(char *) response,*lostr);
  
}
/*$F
 * ggtsts - Get String (Subset Version)
 *
 * Parameters :
 * int *lostr;		number of characters returned
 * char *str;		character string

 *
 * See also: ANSI standard 
 */
ggtsts_ (lostr,str,length) 
int *lostr;
char *str; 
long length;
{ 
Gchar *response;

debug ( ("Get String (Subset Version) \n") ); 


if (length > currforttext)   /* Check amount of memory needed */
  {
   forttext = (Gchar *)realloc(forttext,                /* c1175 */
                        (*lostr) * sizeof(char));       /* realloc more memory */
   if (forttext == NULL)     /* If memory allocation unsuccessful call error routine */ 
     {
      gerrorhand(300,errggetstring,(errfp));		/* d1 */
      return;
     }
   else
      currforttext = (*lostr) * sizeof(char);
  }
response = (Gchar *) forttext;

if (ggetstring (response)) return; 
  
debug(("response string = %s ",response));

*lostr = MIN(strlen(response),80);
strncpy (str,(char *) response,*lostr);
  
}
