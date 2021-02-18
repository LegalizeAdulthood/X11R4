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
 * GKS Error Handling Functions :
 *	geclks_
 *	gerlog_
 *        
 * David Berkowitz
 * Bruce Haimowitz
 * TCS Development
 * Cambridge MA
 *
 * September 28 1988
 *
 * $Header: error.c,v 4.0 89/08/31 18:11:46 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/error.c,v $
 * 
 * $Log:	error.c,v $
 * Revision 4.0  89/08/31  18:11:46  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 4.0  89/08/31  18:05:15  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.17  89/06/23  17:12:12  amy
 * DCR d1	Discontinue use of units table lookup for error file
 * 		name.  Use Fortran Inquire Logical Unit Number function
 * 		instead.   Change global variable errfpp to errfp, pointer to error FILE. 
 * 
 * Revision 1.16  89/06/16  14:14:49  bruce
 * PTR# c2069:	Added variable declaration for global FORTRAN connection id.
 * 
 * Revision 1.15  89/03/15  18:58:52  amy
 * PTR c1130	Removed extern statement for global_errfil (it's declared 
 * 		external in fortxgks.h).
 * 
 * Revision 1.14  89/03/14  20:59:19  amy
 * PTR c1130	Added extern statement for global_errfil.
 * 
 * Revision 1.13  89/03/14  20:48:55  amy
 * PTR c1130	Corrected the argument lists in the FORTRAN error
 * 		functions so they take an int as the error file descriptor, 
 * 		then used that int in gerlog to index into the units table
 * 		to get the error file pointer.
 * 
 * Revision 1.12  89/03/14  20:42:19  amy
 * Fixed typo.
 * 
 * Revision 1.11  89/03/14  20:07:51  amy
 * PTR c2054	gerrorhand:  rewrite to use file descriptor pointer where
 * 		needed, and integer where needed.
 * 
 * Revision 1.10  89/01/06  10:35:34  owens
 * *** empty log message ***
 * 
 * Revision 1.9  88/12/05  14:31:30  todd
 * Changes for code review II.
 * 
 * Revision 1.8  88/11/18  10:52:44  owens
 * code review changes
 * 
 * Revision 1.7  88/11/16  10:12:02  todd
 * Removed extra call to gerrorhand.
 * Added check for ok return code so
 * a return is done before output parameters
 * are set.
 * 
 * Revision 1.6  88/11/08  12:55:06  bhaim
 * Added <xgks.h>
 * 
 * Revision 1.5  88/11/08  10:43:44  bhaim
 * Implemented error functions
 * 
 * Revision 1.4  88/10/11  17:00:33  todd
 * Added Global_errnr initialization.
 * 
 * Revision 1.3  88/09/30  10:23:17  bruce
 * Made corrections to prologues and type casting
 * 
 * Revision 1.2  88/09/28  15:21:29  bruce
 * Changed file prologue.
 * 
 * Revision 1.1  88/09/27  08:16:50  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: error.c,v 4.0 89/08/31 18:11:46 amy Exp $";

#include <xgks.h>
#include "fortxgks.h"

extern void inqlun_();           /* d1 */
extern char global_fname[];      /* d1 */
extern FILE *errfp; 		 /* d1 */

char *xgks_connection = NULL;	/* c2069 */

/*$F
 * gclks - emergency close gks
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p. 195
 */
geclks_ ()  
{ 
debug(("emergency close gks \n"));
gemergencyclosegks ();
}


/*$F
 * gerrorhand -  Error Handling
 *
 *	This function replaces the gerrorhand in the C bindings version of GKS.
 *	This has to be done because the user needs the ability to redefine the
 *	error handling function and a FORTRAN programmer can't redefine
 *	gerrorhand(), so we give the FORTRAN binding a gerrorhand() that calls 
 *	a gerhnd_() function that the user can redefine.  
 *
 *	gerrorhand() is called internally to the FORTRAN binding (when a binding
 *	function determines that there's an error)  When the user wants to call the
 *	error handler, he can only call gerhnd_().
 *
 *	gerrorhand() needs to have a file pointer as the 3rd argument so C binding
 *	functions can call it correctly but it does not use the argument.  Instead
 *	it will pass the global error file unit id to gerhnd_().
 *
 *  Gint errnr     - error number  
 *  Gchar *fctid   - function that detected the error
 *  Gfile *errfp   - file for error output.
 *   
 * See also: ANSI standard p. 195
 */
gerrorhand( errnr, fctid, errfp)
Gint errnr;
Gint fctid;
Gfile *errfp;
{
int opsta;			/* c2054 */
FILE *errfil;			/* c1130 d1 */

ginqopst((Gos *) &opsta);	/* c2054 */
debug (("FORTRAN Errorhand:  errnr = %d, fctid = %d\n",errnr, fctid));
if (opsta == GGKCL) {/* don't use the errfp it's not defined */ /* d1 c2054 */
  errfil = stderr;		/* c1130 c2069 d1 */
  gerhnd_((int *) &errnr,(int *) &fctid, &errfil); /* c2054 c1130 */
  }
else
  gerhnd_((int *) &errnr,(int *) &fctid, errfp); /* c2054 c1130 d1 */
/* the errfp parameter will be ignored by gerlog_ d1 */
debug (("Exit errorhand\n"));
}


/*$F
 * gerlog -  Error Logging
 *
 *   int *errnr    - error number            
 *   int *fctid    - function number         
 *   int *errfil   - error file             
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p. 196
 */
gerlog_ (errnr,fctid,errfil) 
int *errnr;  
int *fctid;   
int *errfil; 	/* c1130 */ 
{ 
int opsta;	/* d1 */
int i;		/* d1 */
char fname[80];	/* d1 */

debug (("Error log:  errnr = %d, fctid = %d\n",*errnr, *fctid));

/* Set up corresponding file descriptor for logical unit number. */
/* inqlun is an external routine written in FORTRAN that returns */
/* a file name give a logical unit number errfile.               */

ginqopst((Gos *) &opsta);                /* d1 */

/* logical unit numbers 5 and 6 are reserved. (stdin and stdout)     d1 */
if (opsta == GGKCL || global_errfil == FORTSTDIN  || global_errfil == FORTSTDOUT) /* d1 */
   errfp = stderr;                              /* d1 */
else
  {
   for (i = 0; i < 80 ; i++)                    /* d1 */
      fname[i] = ' ';                           /* d1 */
   /* pass pointers to FORTRAN                     d1 */
   inqlun_(&global_errfil,fname);               /* d1 */
   for ( i = 0; fname[i] != ' '; i++);          /* d1 */
   /* NULL terminate string                        d1 */
   fname[i] = '\0';                             /* d1 */
   debug(("error file name = %s \n",fname));    /* d1 */
   debug(("current file name = %s \n",global_fname));    /* d1 */
   if (strcmp(fname,global_fname) != 0)         /* d1 */
     {                                          /* d1 */
       debug(("changing error file \n"));       /* d1 */
       if (strlen(fname) == 0)                  /* d1 */
          errfp = stderr;                       /* d1 */
       else                                     /* d1 */
          errfp = fopen(fname,"w");             /* d1 */
       strcpy(global_fname,fname);              /* d1 */
      }                                         /* d1 */
  }
gerrorlog ((Gint)*errnr, (Gint) *fctid ,(Gfile *) errfp); /* c1130 */ /* d1 */
debug (("Exit error log\n"));
}
