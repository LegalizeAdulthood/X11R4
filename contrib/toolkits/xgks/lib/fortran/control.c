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
 * GKS Control functions :
 *       gopks()
 *       gclks()
 *       gopwk()
 *	 gclwk()
 *       gacwk()
 *	 gdawk() 
 * 	 gclrwk()
 *	 grsgwk()
 * 	 guwk()
 *	 gsds()
 *	 gmsg()
 *	 gmsgs()
 *
 * David Berkowitz
 * Bruce Haimowitz
 * TCS Development
 * Cambridge MA
 *
 * August 31 1988
 *
 * $Header: control.c,v 1.1 89/09/19 13:25:37 jim Exp $ 
 *
 * $Source: /xsrc/contrib/toolkits/xgks/lib/fortran/RCS/control.c,v $
 * 
 * $Log:	control.c,v $
 * Revision 1.1  89/09/19  13:25:37  jim
 * Initial revision
 * 
 * Revision 4.0  89/08/31  18:09:59  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.29  89/07/11  18:05:28  todd
 * corrected global_errfil initialization
 * 
 * Revision 1.28  89/06/26  17:48:57  amy
 * Put back include of fiodefs.h for STDERR definition.
 * 
 * Revision 1.27  89/06/26  17:33:53  amy
 * DCR d1	Delete inclusion of fio.h.
 * 
 * Revision 1.26  89/06/23  17:03:51  amy
 * DCR 1	Discontinue use of units table lookup for error file
 * 		name.  Use Fortran Inquire Logical Unit Number function 
 * 		instead.  Change global variable errfpp to errfp, pointer to error FILE.
 * 
 * Revision 1.25  89/06/16  14:15:21  bruce
 * PTR# c2069:	Changed opws() so it will used global conid if int conid != 0
 * 
 * Revision 1.24  89/06/16  08:09:07  bruce
 * PTR# c1175:	Corrected calls to realloc that were not storing the
 * 		pointer to the new memory.
 * 
 * Revision 1.23  89/04/20  16:39:14  bruce
 * removed unused variable to eliminate warning.
 * 
 * Revision 1.22  89/04/20  14:59:03  bruce
 * Removed code using xgks_connection,
 * 
 * Revision 1.21  89/03/15  20:35:21  amy
 * Message subset version:  added debug  statements.
 * 
 * Revision 1.20  89/03/15  18:57:03  amy
 * PTR c1130	Added global definition for error file logical unit number
 * 		 and set it in the call to open GKS.
 * 
 * Revision 1.19  89/03/14  20:55:49  amy
 * PTR c1130	Added definition and initialization of global_errfil.
 * 
 * Revision 1.18  89/03/14  20:39:58  amy
 * PTR c2043	Changed calls to VALIDTYPE macro to include value for errind parameter.
 * 
 * Revision 1.17  89/03/07  17:09:49  amy
 * PTR c2008	gopwk:  in checking for workstation type, use double
 * 		quotes around strings, instead of single quotes.
 * 
 * Revision 1.16  89/02/13  15:37:10  amy
 *  
 * 
 * Revision 1.15  89/01/17  11:44:28  todd
 * Corrected comment.
 * 
 * Revision 1.14  89/01/17  11:21:44  todd
 * Added code to use the units file descriptor for MI and MO.
 * Also the connection id can now be passed in the array
 * xgks_connection if this is blank NULL is passed to the
 * "C" binding. PTR c2008
 * 
 * Revision 1.13  89/01/14  14:52:15  todd
 * Answered ptr 2023
 * 
 * Revision 1.11  88/12/05  17:34:55  owens
 * code review changes
 * 
 * Revision 1.10  88/12/05  14:34:08  bhaim
 * Changes for Code II
 * 
 * Revision 1.9  88/11/14  06:30:46  todd
 * Added ginqavailwstypes function.
 * 
 * Revision 1.8  88/11/07  10:17:33  bhaim
 * Changed error file handling
 * 
 * Revision 1.7  88/10/11  17:00:13  todd
 * Added Global_errnum initialization.
 * 
 * Revision 1.6  88/10/11  06:02:50  bhaim
 * No revision
 * 
 * Revision 1.5  88/10/04  11:44:50  bhaim
 * Added type casting
 * 
 * Revision 1.4  88/09/29  10:24:16  todd
 * Added code to allocate space for the 
 * integer area.
 * 
 * Revision 1.3  88/09/28  08:10:08  bhaim
 * Change prolouges
 * 
 * Revision 1.2  88/09/27  11:19:13  bhaim
 * Added comments to parameters
 * 
 * Revision 1.1  88/09/27  08:16:46  todd
 * Initial revision
 * 
 */

static char *rcsid = "$Header: control.c,v 1.1 89/09/19 13:25:37 jim Exp $";



#include "fortxgks.h"
#include <xgks.h>
#include <sys/types.h>
#include <sys/stat.h>

char *malloc();
char *realloc();        /* c1175 */

#define INITPOINTS 2048

#define INITTEXT 512

#define INITINT 1024

char **wtypes;

int NUMWTYPES;

extern char *xgks_connection;           /* c2069 */

Gpoint *fortpoints = (Gpoint *)NULL;    /* c1175 */
int currfortpoints = 0;                 /* c1175 current # of points */

char *forttext = (Gchar *)NULL;         /* c1175 */
int currforttext = 0;                   /* c1175 current amount of text */

int *fortint = (Gint *)NULL;            /* c1175 */
int currfortint = 0;                    /* c1175 current space for integers */

FILE *errfp;            		/* d1 */
char  global_fname[80];			/* d1; zeroed by default */

int   retval;                 		/* return value from C calls *//* c1130 */
int global_errfil;		/* logical unit no. of error file *//* c1130 */


/*$F
 * gopks - Open  GKS
 *   initialize the static points array (used throughout fortran binding) 
 *   initialize the static text area    (ditto)
 *   initialize the static integer area (ditto ditto)
 *   set up file pointer (see below for description of tricky stuff)
 *
 *  int *errfile  - error message file
 *  int *memory   - amount of memory units
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.74
 */
gopks_ (errfile, memory)
int *errfile;    
int *memory;    
{
Gstrlist wstype;
Gint i;
debug(("Open  GKS \n"));

/* initialize the static points array */
fortpoints = (Gpoint *) realloc (fortpoints, INITPOINTS * (sizeof (Gpoint) ) ); /* c1175 */
currfortpoints = INITPOINTS;

/* initialize the static text area */
forttext =  (Gchar *) realloc (forttext, INITTEXT * sizeof(Gchar)); /* c1175 */
currforttext = INITTEXT;

/* initialize the static integer area */
fortint = (Gint *) realloc (fortint, INITINT * sizeof(Gint));   /* c1175 c2074 */
currfortint = INITINT;

debug ( ("errfile = %d \n",*errfile) );

global_errfil = *errfile;		/* c1130 */

/* Set up corresponding file descriptor for logical unit number  d1 */
/* inqlun is an external routine written in FORTRAN than returns d1 */
/* a file name give a logical unit number errfile                d1 */
if (global_errfil != FORTSTDIN && global_errfil != FORTSTDOUT) /* d1 */
 {                                              /* d1 */
   for (i = 0; i < 80 ; i++)                    /* d1 */
     global_fname[i] = ' ';                     /* d1 */
   /* pass pointers to FORTRAN                     d1 */
   inqlun_(errfile,global_fname);               /* d1 */
   for ( i = 0; global_fname[i] != ' '; i++)    /* d1 */
      debug(("name = %c",global_fname[i]));     /* d1 */
   /* NULL terminate string                        d1 */
   global_fname[i] = '\0';                      /* d1 */
   if (strlen(global_fname) == 0)               /* d1 */
    errfp = stderr;                             /* d1 */
   else                                         /* d1 */
    errfp = fopen(global_fname,"w");            /* d1 */
 }                                              /* d1 */
else                                            /* d1 */
   errfp = stderr;                              /* d1 */

debug(("Error filename = %s length = %d \n",global_fname,strlen(global_fname)));/* d1 */

gopengks ((Gfile *)(errfp), (Glong)*memory);	/* d1 */

/* inquire available workstation types */
ginqavailwstypes(&wstype);

/* allocate space to store ws types */
NUMWTYPES = wstype.number;
wtypes = (char **) malloc(NUMWTYPES * sizeof(char *));

for (i = 0 ; i < wstype.number ; i++ )
 { 
  wtypes[i] = wstype.strings[i];
  debug(("wstype %d = %s \n",i,wtypes[i]));
 }

/* free memory malloc'ed by ginqavailwstypes */
free (wstype.strings);

}


/*$F
 * gclks - CLOSE GKS
 *   free memory malloc'ed for use while GKS was open 
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.74
 */
gclks_ ()
{

debug ( ("Close GKS \n") );

gclosegks ();

/* free memory allocated for use while GKS was open */
free (fortpoints);
free (forttext);
free (fortint);
free (wtypes);
if (xgks_connection != NULL)            /* c2069 */
  free (xgks_connection);               /* c2069 */

}


/*$F
 * gopwk - Open Workstation 
 * int *wkid                             workstation identifier  
 * int *conid                            connection identifier  
 * int *wtype                            workstation type      
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.74
 */
gopwk_ (wkid, conid, wtype)
int *wkid;                            
int *conid;                          
int *wtype;                         
{
Gchar *connection;
char *getenv();

debug ( ("Open Workstation %d conid %d type %d \n",*wkid, *conid, *wtype-1) ); 

debug(("wstype = <%s>\n",wtypes[*wtype-1]));

VALIDTYPE((int *) NULL,*wtype,1,NUMWTYPES,errgopenws);    /* c2023 c2043 */

if (strcmp(wtypes[*wtype-1],"MI") == 0 || 		/* c2051 */
    strcmp(wtypes[*wtype-1],"MO") == 0)                 /* c2051 c2008 */
    connection = global_fname; 				/* c2008  c2069 d1 */
else 							/* c2008 */
    if (xgks_connection != NULL && xgks_connection[0] != NULL)  /* c2069 */
     {                                                          /* c2069 */
       debug(("Using xgks_connection \n"));                     /* c2069 */
       connection = malloc(strlen(xgks_connection) +1);         /* c2069 */
       strcpy(connection,xgks_connection);                      /* c2069 */
     }                                                          /* c2069 */
    else                                                        /* c2069 */
       connection = NULL;                                       /* c2069 */
debug(("connection id = %s \n ",connection));

gopenws ((Gint)*wkid, connection, wtypes[*wtype -1]);
}


/*$F
 * gclwk -  Close Workstation
 * int *wkid;                              workstation identifier   
 *
 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.74
 */
gclwk_ (wkid)
int *wkid;  
{

debug ( ("Close Workstation %d \n", *wkid) );

gclosews ((Gint)*wkid);
}


/*$F
 * gacwk -  Activate Workstation
 * int *wkid                            workstation identifier      
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.75
 */
gacwk_ (wkid)
int *wkid;  
{

debug ( ("Activate Workstation %d \n", *wkid) );

gactivatews ((Gint)*wkid);
}


/*$F
 * gdawk -  Deactivate Workstation
 * int *wkid                              workstation identifier     
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.76
 */
gdawk_ (wkid)
int *wkid;  
{

debug ( ("Deactivate Workstation %d \n", *wkid) );

gdeactivatews ((Gint)*wkid);
}


/*$F
 * gclrwk -  Clear Workstation
 * int *wkid;                              workstation identifier     
 * int *cofl;                              control flag              
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.76
 */
gclrwk_ (wkid, cofl)
int *wkid;                            
int *cofl;                           
{
debug ( ("Clear Workstation %d (%d) \n",*wkid, *cofl) ); 
CLEARCONTROLFLAG(*cofl,errgclearws);

gclearws ((Gint)*wkid, (Gclrflag)*cofl );
}


/*$F
 * grsgwk - Redraw all segments on workstation
 * int *wkid;                               workstation identifier    
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.77
 */
grsgwk_ (wkid)
int *wkid;            
{

debug ( ("Redraw Workstation %d \n", *wkid) );

gredrawsegws ((Gint)*wkid);
}


/*$F
 * guwk  -  Update Workstation 
 * int *wkid;                            workstation identifier   
 * int *regfl;                           update regeneration flag
 *
 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.78
 */
guwk_ (wkid, regfl)
int *wkid;   
int *regfl;
{
debug ( ("Update Workstation %d %d \n", *wkid, *regfl) );
REGENERATIONFLAG(*regfl,errgupdatews);


/* Reverse the sense of *regfl */
gupdatews ((Gint)*wkid, ((Gregen)(*regfl)?GPERFORM:GPOSTPONE));
}


/*$F
 * gsds - Set Deferral State
 * int *wkid;                                workstation identifier    
 * int *defmod;                              deferral mode             
 * int *regmod;                              implicit regeneration mode
 *
 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.79
 */
gsds_ (wkid, defmod, regmod)
int *wkid;                               
int *defmod;                            
int *regmod;                           
{
Girgmode C_regmod;
Gdefmode C_defmod;

debug ( ("Set Deferal State %d (%d) (%d) \n",*wkid, *defmod, *regmod) ); 
DEFERRALMODE(*defmod,errgsetdeferst);
IMPLICITREGENERATIONMODE(*regmod,errgsetdeferst);
switch (*defmod)
 {
  case FORT_GASAP : C_defmod = GASAP; 
       break;
  case FORT_GBNIG : C_defmod = GBNIG; 
       break;
  case FORT_GBNIL : C_defmod = GBNIL; 
       break;
  case FORT_GASTI : C_defmod = GASTI; 
       break;
  default:
    C_defmod = NULL;              /* suppress compiler warning */
    fprintf(stderr,"error unknown defmod\n");
 }

switch (*regmod) 
  {
  case FORT_GSUPPD: 
    C_regmod = GSUPPRESSED; 
    break;
  case FORT_GALLOW: 
    C_regmod = GALLOWED; 
    break; 
  default:
    C_regmod = NULL;              /* suppress compiler warning */
    fprintf(stderr,"error unknown regmod\n");
  }
gsetdeferst ((Gint)*wkid,C_defmod,C_regmod);
}


/*$F
 * gmsg - Message
 * int *wkid;                                workstation identifier    
 * char *mess;                               message                   
 * long length;                              length of message         
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.80
 */
gmsg_(wkid, mess, length)
int *wkid;                               
char *mess;                             
long length;                           
{

debug ( ("Message ") );

/* make sure we have enought temp. storage to hold the string */
if (length > currforttext) 
  {
  forttext = (Gchar *)realloc (forttext,length*sizeof(Gchar)); /* c1175 */
  if (forttext == NULL)
    {
    gerrorhand(300,errgmessage,(errfp));	/* d1 */
    currforttext = 0;
    return;
    }
  else
    currforttext = length;
  }
  
strncpy (forttext, mess, length);
forttext[length] = '\0';

debug ( ("<%s> length %d \n", forttext, length) );

gmessage ((Gint)*wkid,(Gchar *)forttext );

}


/*$F
 * gmsgs - Message FORTRAN 77 Subset Version
 * int *wkid;                                workstation identifier  
 * int *lstr;                                length of string       
 * char *mess;                               message               
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.74
 */
gmsgs_(wkid, lstr, mess)
int *wkid;                               
int *lstr;                              
char *mess;                            
{

debug ( ("Message (Subset Version) length = %d \n",*lstr) );

/* make sure we have enought temp. storage to hold the string */
if (*lstr > currforttext) 
  {
  forttext = (Gchar *)realloc (forttext,(*lstr)*sizeof(Gchar)); /* c1175 */
  if (forttext == NULL)
    {
    gerrorhand(300,errgmessage,(errfp));	/* d1 */
    currforttext = 0;
    return;
    }
  else
    currforttext = *lstr;
  }

strncpy (forttext, mess, *lstr);
forttext[*lstr] = '\0';

debug ( ("<%s> length %d \n", forttext, *lstr) );

gmessage ((Gint)*wkid,(Gchar *)forttext );

debug ( ("Exiting Message (Subset)\n"));
}
