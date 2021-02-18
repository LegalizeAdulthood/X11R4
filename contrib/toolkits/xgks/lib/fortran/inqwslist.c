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
 * GKS Inquiry Functions for Workstation State List :
 *	gqwkc_     gqefai_
 *	gqwks_     gqfar_
 *	gqwkdu_    gqepai_
 *	gqepli_    gqpar_
 *	gqplr_     gqeci_
 *	gqepmi_    gqcr_
 *	gqpmr_     gqwkt_
 *	gqetxi_    gqsgwk_
 *	gqtxr_     gqlcs_
 *	gqtxx_     gqsks_
 *	gqtxxs_    gqchs_
 *	gqpks_     gqsts_
 *        
 * David Berkowitz
 * Bruce Haimowitz
 * Todd Gill 
 * TCS Development
 * Cambridge MA
 *
 * August 31 1988
 *
 * $Header: inqwslist.c,v 4.0 89/08/31 18:33:31 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/inqwslist.c,v $
 * 
 * $Log:	inqwslist.c,v $
 * Revision 4.0  89/08/31  18:33:31  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.33  89/06/26  15:48:47  amy
 * DCR d1	Declare errfp external for use in error macros.
 * 		Change references to errfpp to errfp.
 * 
 * Revision 1.32  89/06/16  08:10:21  bruce
 * PTR# c1175:	Corrected calls to realloc that were not storing the
 * 		pointer to the new memory.
 * 
 * Revision 1.31  89/06/06  09:55:37  bruce
 * PTR# c1179:	Made changes to character expansion field names for
 * 		AIX compiler.
 * 
 * Revision 1.30  89/04/20  16:41:52  bruce
 * PTR# c2067:	Changed the argument type of the initial string from (char **) to
 * 		(char *) as defined in the FORTRAN binding.
 * 
 * Revision 1.29  89/04/10  14:50:06  bruce
 * PTR# c2054:	Corrected initialization of sum to avoid
 * 		compiler error message.
 * 
 * Revision 1.28  89/04/06  16:28:11  bruce
 * PTR# c2043:	corrected TYPEOFRETURNEDVALUES macro to set errind.
 * 
 * Revision 1.27  89/03/14  20:11:23  amy
 * PTR c2054	gqsks:  added code to copy initial points from the 
 * 		stroke record.
 * 		gqchs:  fixed the data packing of the initial choice strings.
 * 
 * Revision 1.26  89/03/14  18:19:07  amy
 * PTR c2046	gqchs and gqpks:  fix interpretation of
 * 		enumerated type for device state.
 * 
 * Revision 1.25  89/03/14  17:27:38  amy
 * PTR c2037	gqsks:  correct dereferencing of pointers in call to ginqstrokest.
 * 
 * Revision 1.24  89/03/07  17:07:28  amy
 * PTR c2045	gqeci:  call correct C function.
 * 
 * Revision 1.23  89/02/04  14:31:53  amy
 * Cosmetic changes to log.
 * 
 * Revision 1.22  89/02/03  17:01:03  amy
 * PTR c2011	Inquire Text Rep:  set default char. exp. factor to value > 0.
 * 
 * Revision 1.21  89/01/24  10:09:24  bruce
 * PTR c1038	Fixed types, set dangling output args, fixed bas gqsts() 
 *		implementation.
 * 
 * Revision 1.20  89/01/20  13:44:38  bruce
 * Initial changes made for PTR# c2038, but it had to be checked in
 * to make a new copy for bld.
 * 
 * Revision 1.19  89/01/14  14:10:39  bruce
 * PTR c2020	Added checks for VALIDMEMBER in inquire list element functions.
 * 
 * Revision 1.18  89/01/06  08:26:30  bruce
 * PTR c2020	Changed the call to the VALIDMEMBER macro to allow set member 0 to    *		be requested.
 * 
 * Revision 1.17  89/01/04  10:53:02  bruce
 * PTRs 2013, 2014, 2015.
 * 
 * Revision 1.16  88/12/28  16:43:50  todd
 * Code review changes.
 * 
 * Revision 1.13  88/12/05  14:30:00  todd
 * Changes for Code review II.
 * 
 * Revision 1.12  88/11/16  10:07:32  bhaim
 * Error Handling
 * 
 * Revision 1.11  88/11/15  15:17:05  bhaim
 * Error handling
 * 
 * Revision 1.10  88/11/15  11:56:20  todd
 * Fixed syntax error.
 * 
 * Revision 1.9  88/11/15  11:22:37  todd
 * Removed global_errnum references.
 * 
 * Revision 1.8  88/11/15  11:08:17  todd
 * Removed extra calls to gerrorhand
 * 
 * Revision 1.7  88/11/07  11:39:09  todd
 * Finished functions and comments
 * 
 * Revision 1.6  88/10/18  07:15:36  bhaim
 * Updated for Code II
 * 
 * Revision 1.5  88/10/13  15:44:34  bruce
 * Fixed type casting and other style errors.
 * 
 * Revision 1.4  88/09/28  15:07:15  bruce
 * Changed mytext to forttext.
 * 
 * Revision 1.3  88/09/28  14:51:35  bruce
 * Had to correct type casting.
 * 
 * Revision 1.2  88/09/28  14:37:03  bruce
 * Added function prologues, GKS type casting and corrected variable names.
 * 
 * Revision 1.1  88/09/27  08:17:21  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: inqwslist.c,v 4.0 89/08/31 18:33:31 amy Exp $";

extern char **wtypes;
extern int NUMWTYPES;

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
 * gqwkc - Inquire Workstation Connection and Type 
 *
 * int *wkid - pointer to workstation id
 * int *errind - error indicator
 * int *conid - pointer to connection id
 * int *wtype - pointer to workstation type

 *
 * Returns: Error 309, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.74
 */
gqwkc_ (wkid,errind,conid,wtype)  
int *wkid;
int *errind;
int *conid;
int *wtype;

{ 
Gwsct ct;
Gint   i;
debug(("Inquire Workstation Connection and Type  \n"));
if (*errind = ginqwsconntype(*wkid,&ct)) return;

/* Only conid of NULL permitted. May change depending on gopenws and conid. */
*conid = 1; /* verif suite wants 1 */

for (i=0; i < NUMWTYPES; i++)   /* c2015 */          
  {
     debug(("wtype = %s cttype = %s \n",wtypes[i],ct.type));
     if ((strcmp(wtypes[i],ct.type) == 0) || ((index(ct.type,':') != 0) && wtypes[i] == NULL)) /* c2015 */
         {
             *wtype = i + 1;
             return;
         }
 }
*errind = 309; 					/* c2043 */
gerrorhand(309,errginqwsconntype,(errfp)); 	/* d1 */
}


/*$F
 * gqwks - inquire  workstation state
 *
 * int  *wkid;        workstation identifier              
 * int  *errind;      error indicator                     
 * int  *state;       workstation state                   
 *
 */
gqwks_ (wkid,errind,state)  
int  *wkid;       
int  *errind;    
int  *state;    

{
*errind = ginqwsst((Gint) *wkid,(Gint *) state);
}

/*$F
 * gqwkdu - inquire workstation deferral and update states 
 * int *wkid;		workstation identifier        
 * int *errind;    	error indicator 
 * int *defmod;    	deferral mode
 * int *regmod; 	implicit regeneration mode   
 * int *dempty;		display surface empty  
 * int *nframe;		new frame action necessary at update 
 *
 *
 */

gqwkdu_ (wkid,errind,defmod,regmod,dempty,nframe)  
int *wkid;        
int *errind;     
int *defmod;    
int *regmod;   
int *dempty;  
int *nframe; 
{
Gwsdus du;
debug(("inquire workstation deferral and update states \n")); 
if (*errind = ginqwsdeferupdatest((Gint) *wkid,&du)) return;

*defmod = (int) du.defmode;
*dempty = (int) !(du.dspsurf);
*regmod = (int) du.irgmode;
*nframe = (int) du.nframe;
}

/*$F
 * gqepli - inquire  list of polyline indices
 * int *wkid;         workstation identifier 
 * int *n;            set member requested   
 * int *errind;       error indicator       
 * int *ol;           number of polyline bundle
 * int *pli;          Nth element of list    
 *
 *
 */
gqepli_ (wkid,n,errind,ol,pli)  
int *wkid;        
int *n;          
int *errind;    
int *ol;       
int *pli;     
{
Gintlist indices;
debug(("inquire  list of polyline indices \n"));
if (*errind = ginqlineindices((Gint) *wkid,&indices)) return;
debug (("Number available: %d \n", indices.number) );

/* set return values not dependent on range check */
*ol = (int) indices.number;

VALIDMEMBER(errind,*n,0,indices.number,errginqlineindices);  /* c2020 c2043 */
if (*n)						/* c2020 */
   *pli = (int) indices.integers[(*n)-1];  
free(indices.integers);
}

/*$F
 * gqplr - inquire polyline representation  
 * int *wkid;          workstation identifier 
 * int *pli;           polyline index         
 * int *type;          type of returned values
 * int *errind;        error indicator        
 * int *ltype;         linetype               
 * float *lwidth;      linewidth scale factor
 * int *coli;          polyline colour index  
 *
 * Returns: Error 2004, in addition to ANSI standard errors for this function.
 *
 */
gqplr_ (wkid,pli,type,errind,ltype,lwidth,coli)   
int *wkid;         
int *pli;          
int *type;        
int *errind;     
int *ltype;     
float *lwidth; 
int *coli;    
{
Glnbundl rep;
debug(("inquire polyline representation \n")); 
TYPEOFRETURNEDVALUES(errind,*type,errginqlinerep);  /* c2043 */
if (*errind = ginqlinerep((Gint) *wkid,(Gint) *pli,(Gqtype) *type,&rep)) return;

*ltype = (int) rep.type;
*lwidth = (float)  rep.width;
*coli = (int) rep.colour;
}

/*$F
 * gqepmi - inquire  list of polymarker indices
 * int *wkid;         workstation identifier
 * int *n;            set member requested 
 * int *errind;       error indicator     
 * int *ol;           number of polymarker
 * int *pmi;          Nth element of list
 *
 *
 */
gqepmi_ (wkid,n,errind,ol,pmi)   
int *wkid;        
int *n;          
int *errind;    
int *ol;       
int *pmi;     
{
Gintlist indices;
debug(("inquire  list of polymarker indices \n"));
if (*errind = ginqmarkerindices((Gint) *wkid,&indices)) return;

/* set return values not dependent on range check */
*ol = (int) indices.number;

VALIDMEMBER(errind,*n,0,indices.number,errginqmarkerindices);  /* c2043 c2020 */
if (*n)						/* c2020 */
   *pmi = (int) indices.integers[(*n)-1]; 
free(indices.integers);
}

/*$F
 * gqpmr - inquire polymarker representation
 * int *wkid;          workstation identifier 
 * int *pmi;           polymarker index        
 * int *type;          type of returned values
 * int *errind;        error indicator        
 * int *mtype;         markertype            
 * float *mszsf;       marker size scale factor
 * int *coli;          polyline colour index  
 *
 * Returns: Error 2000, in addition to ANSI standard errors for this function.
 *
 */
gqpmr_ (wkid,pmi,type,errind,mtype,mszsf,coli)   
int *wkid;        
int *pmi;        
int *type;      
int *errind;   
int *mtype;   
float *mszsf;
int *coli;  
{
Gmkbundl rep;
debug(("inquire polymarker representation \n"));
TYPEOFRETURNEDVALUES(errind,*type,errginqmarkerrep);  /* c2043 */
if (*errind = ginqmarkerrep((Gint) *wkid,(Gint) *pmi,(Gqtype) *type,&rep)) return;

*mtype = (int) rep.type;
*mszsf = (float)  rep.size;
*coli = (int) rep.colour;
}

/*$F
 * gqetxi - Inquire list of Text Indices
 * int *wkid;         workstation identifier 
 * int *n;            set member requested   
 * int *errind;       error indicator        
 * int *ol;           number of text bundles 
 * int *txi;          Nth element of list    
 *
 *
 */
gqetxi_ (wkid,n,errind,ol,txi)   
int *wkid;       
int *n;         
int *errind;   
int *ol;      
int *txi;    
{
Gintlist indices;
debug(("Inquire list of Text Indices \n"));
if (*errind = ginqtextindices((Gint) *wkid,&indices)) return;

/* set return values not dependent on range check */
*ol = (int)  indices.number;

VALIDMEMBER(errind,*n,0,indices.number,errginqtextindices);  /* c2043 c2020 */
if (*n)						/* c2020 */
   *txi = (int)  indices.integers[(*n)-1]; 
free(indices.integers); 
}

/*$F
 * gqtxr - Inquire Text Representation
 *
 * int *wkid;          workstation identifier
 * int *txi;           text index            
 * int *type;          type of returned values
 * int *errind;        error indicator      
 * int *font;          text font             
 * int *prec;          text precision         
 * float *chxp;        character expansion factor 
 * float *chsp;        character spacing       
 * int *coli;          polyline colour index  
 *
 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.74
 */
gqtxr_ (wkid,txi,type,errind,font,prec,chxp,chsp,coli)   
int *wkid;         
int *txi;         
int *type;       
int *errind;    
int *font;     
int *prec;    
float *chxp; 
float *chsp;
int *coli; 
{
Gtxbundl rep;
debug(("Inquire Text Representation \n"));
TYPEOFRETURNEDVALUES(errind,*type,errginqtextrep);  /* c2043 */
if (*errind = ginqtextrep((Gint) *wkid,(Gint) *txi,(Gqtype) *type,&rep)) return;

*font = (int)  rep.fp.font;
*prec = (int)  rep.fp.prec;
*chxp = (float) rep.ch_exp;	/* c1179 */
*chsp = (float) rep.space;
*coli = (int) rep.colour;
}

/*$F
 * gqtxx - Inquire Text Extent
 *
 * int   *wkid    - pointer to workstation id
 * float *px      - pointer to text x position
 * float *py      - pointer to text y position
 * char  *str     - pointer to text string
 * int   *errind  - pointer to error indicator
 * float *cpx     - pointer to text concat point - x
 * float *cpy     - pointer to text concat point - y
 * float *txexpx  - pointer to array containing return values
 * float *txexpy  - pointer to array containing return values
 * long  *len_str - pointer to length of character string
 * long  *txexpx  - pointer to length of x array
 * long  *txexpy  - pointer to length of y array

 * Returns: Error 300, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.158
 */
gqtxx_ (wkid, px, py, str, errind, cpx, cpy, txexpx, txexpy, len_str, len_txexpx, len_txexpy)
int *wkid;
float *px, *py;
char *str;
int *errind;
float *cpx, *cpy;
float *txexpx, *txexpy;
long len_str, len_txexpx, len_txexpy;

{
Gpoint position;
Gextent extent;

debug ( ("Inquire Text Extent \n ") );

if (len_str + 1 > currforttext)
   {
   forttext = (Gchar *)realloc (forttext, len_str + 1);         /* c1175 */
   if (!forttext)
      {
      *errind = 300;    /* c2043 */
      gerrorhand (300, errginqtextextent, (errfp)); 	/* d1 */
      currforttext = 0;
      return;
      }
   currforttext = len_str + 1;
   }

strncpy (forttext, str, len_str);
forttext[len_str] = '\0';

debug ( ("<%s> length %d \n", forttext, len_str) );

position.x = (Gfloat)*px;
position.y = (Gfloat)*py;

if (*errind = ginqtextextent ((Gint)*wkid, position, (Gchar *)forttext, &extent)) return;

*cpx = (float) extent.concat.x;
*cpy = (float) extent.concat.y;

debug ( ("	cpx: %7.2f cpy: %7.2f \n", *cpx, *cpy) );

debug ( ("	llx: %7.2f lly: %7.2f \n", *txexpx, *txexpy) );

txexpx[0] = (float)extent.ll.x;
txexpy[0] = (float)extent.ll.y;

debug ( ("	lrx: %7.2f lry: %7.2f \n", *txexpx, *txexpy) );

txexpx[1] = (float)extent.lr.x;
txexpy[1] = (float)extent.lr.y;

debug ( ("	urx: %7.2f ury: %7.2f \n", *txexpx, *txexpy) );

txexpx[2] = (float)extent.ur.x;
txexpy[2] = (float)extent.ur.y;

debug ( ("	ulx: %7.2f uly: %7.2f \n", *txexpx, *txexpy) );

txexpx[3] = (float)extent.ul.x;
txexpy[3] = (float)extent.ul.y;

}

/*$F
 * gqtxxs - Inquire Text Extent (subset version)
 *
 * int   *wkid     - pointer to workstation id
 * float *px       - pointer to text position x
 * float *py       - pointer to text position y
 * int   *lstr     - pointer to length of string
 * char  *str      - pointer to string
 * int   *errind   - pointer to error indicator
 * float *cpx      - pointer to concat point x
 * float *cpy      - pointer to concat point y
 * float txexpx[4] - pointer to return values - x
 * float txexpy[4] - pointer to return values - y

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.158
 */
gqtxxs_(wkid, px, py, lstr, str, errind, cpx, cpy, txexpx, txexpy)
int   *wkid;
float *px;
float *py;
int   *lstr;
char  *str;
int   *errind;
float *cpx;
float *cpy;
float txexpx[4];
float txexpy[4];
{
Gpoint position;
Gextent extent;

debug ( ("Inquire Text Extent \n") );

position.x = (Gfloat)*px;
position.y = (Gfloat)*py;

if (*lstr + 1 > currforttext)
   {
   forttext = (Gchar *)realloc (forttext, *lstr + 1);   /* c1175 */
   if (!forttext)
      {
      *errind = 300;    /* c2043 */
      gerrorhand (300, errginqtextextent, (errfp)); 	/* d1 */
      currforttext = 0;
      return;
      }
   currforttext = *lstr + 1;
   }

strncpy (forttext, str, *lstr);
forttext[*lstr] = '\0';

debug ( ("	<%s> lstr %d \n", forttext, *lstr) );

*errind = ginqtextextent ((Gint)*wkid, position, (Gchar *)forttext, &extent);
*cpx    = (float)extent.concat.x;
*cpy    = (float)extent.concat.y;

txexpx[0] = (float)extent.ll.x; 
txexpx[1] = (float)extent.lr.x; 
txexpx[2] = (float)extent.ur.x; 
txexpx[3] = (float)extent.ul.x; 

txexpy[0] = (float)extent.ll.y; 
txexpy[1] = (float)extent.lr.y; 
txexpy[2] = (float)extent.ur.y; 
txexpy[3] = (float)extent.ul.y; 

}

/*$F
 * gqefai - Inquire List of fill Area Indices
 *
 * int *wkid;         workstation identifier
 * int *n;            set member requested 
 * int *errind;       error indicator        
 * int *ol;           number of text bundles
 * int *fai;          Nth element of list    
 *

 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.74
 */
gqefai_ (wkid,n,errind,ol,fai)   
int *wkid;       
int *n;         
int *errind;   
int *ol;      
int *fai;    
{
Gintlist indices;
debug(("Inquire List of fill Area Indices \n "));
if (*errind = ginqfillindices((Gint) *wkid,&indices)) return;

/* set return values not dependent on range check */
*ol = indices.number;

VALIDMEMBER(errind,*n,0,indices.number,errginqfillindices);  /* c2043 c2020 */
if (*n)						/* c2020 */
   *fai = indices.integers[(*n)-1];  
free(indices.integers);
}

/*$F
 * gqfar - Inquire Fill Area Representation 
 *
 * int *wkid;          workstation identifier
 * int *fai;           fill area index       
 * int *type;          type of returned values
 * int *errind;        error indicator       
 * int *ints;          fill area interior style 
 * int *styli;         fill area style index
 * int *coli;          polyline colour index 
 *
 *
 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.74
 */
gqfar_ (wkid,fai,type,errind,ints,styli,coli)   
int *wkid;        
int *fai;        
int *type;      
int *errind;   
int *ints;    
int *styli;  
int *coli;  
{
Gflbundl rep;
debug((" Inquire Fill Area Representation  \n"));
TYPEOFRETURNEDVALUES(errind,*type,errginqfillrep);  /* c2043 */
if (*errind = ginqfillrep((Gint) *wkid,(Gint) *fai,(Gqtype) *type,&rep)) return;  /*c2013*/

*ints = (int) rep.inter;
*styli = (int) rep.style;
*coli = (int) rep.colour;
}

/*$F
 * gqepai - Inquire List of Pattern Indices 
 * int *wkid;         workstation identifier
 * int *n;            set member requested 
 * int *errind;       error indicator     
 * int *ol;           number of pattern table
 * int *pai;          Nth element of list   
 *

 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.74
 */
gqepai_ (wkid,n,errind,ol,pai)   
int *wkid;        
int *n;          
int *errind;    
int *ol;       
int *pai;     
{
Gintlist indices;
debug(("Inquire List of Pattern Indices \n"));
if (*errind = ginqpatindices(*wkid,&indices)) return;

/* set return values not dependent on range check */
*ol = (int) indices.number;

VALIDMEMBER(errind,*n,0,indices.number,errginqpatindices);  /* c2043 c2020 */
if (*n)						/* c2020 */
   *pai = (int) indices.integers[(*n)-1]; 
free(indices.integers);
}

/*$F
 * Inquire Pattern Representation  
 *
 * int *wkid;          workstation identifier
 * int *pai;           pattern   index       
 * int *type;          type of returned values
 * int *dimx,*dimy;    maximum pattern dim  
 * int *errind;        error indicator      
 * int *dx,*dy;        pattern dimensions    
 * int *colia[dimx,dimy]; pattern array          
 *

 *
 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.74
 */
gqpar_ (wkid,pai,type,dimx,dimy,errind,dx,dy,colia)   
int *wkid;         
int *pai;         
int *type;       
int *dimx,*dimy;
int *errind;   
int *dx,*dy;  
int *colia;  
{
Gptbundl rep;
debug(("Inquire Pattern Representation \n "));
TYPEOFRETURNEDVALUES(errind,*type,errginqpatrep);  /* c2043 */
if (*errind = ginqpatrep((Gint) *wkid,(Gint) *pai,(Gqtype) *type,&rep)) return;

*dx = (int) rep.size.x; 
*dy = (int) rep.size.y;
debug (("Inquired Pattern Size:  %d - %d\n", *dx, *dy));

if (*dx > *dimx || *dy > *dimy)
   {						/* c2042 */
   *errind = 2001;				/* c2042 c2043 */
   gerrorhand(2001,errginqpatrep,(errfp)); 	/* c2042 */ /* d1 */
   return;					/* c2042 */
   }

/* c2044:  This array does NOT need to be transposed. */

  /* c2044:  Take the dx-by-dy subset starting at (1,1). */
  {							/* c2042 */
  int  col, row;					/* c2042 */

  for (row=0; row < *dy; row++)				/* c2044 */
     for (col=0; col < *dx; col++)			/* c2044 */
        *(colia + (row * *dimx + col)) = *(rep.array + (row * *dx + col));								/* c2044 */
  }
}

/*$F
 * Inquire List element Of Color Indices 
 *
 * int *wkid;         workstation identifier
 * int *n;            set member requested  
 * int *errind;       error indicator        
 * int *ol;           number of colour table entries
 * int *coli;         Nth element of list    
 *

 *
 * Returns: ANSI standard errors for this function.
 *
 */
gqeci_ (wkid,n,errind,ol,coli)   
int *wkid;        
int *n;          
int *errind;    
int *ol;       
int *coli;    
{
Gintlist indices;
debug(("Inquire List element Of Color Indices \n "));
if (*errind = ginqcolourindices((Gint) *wkid, &indices)) return; /* c2045 */

/* set return values not dependent on range check */
*ol = (int) indices.number;

VALIDMEMBER(errind,*n,0,indices.number,errginqcolourindices);  /* c2043 c2020 */
if (*n)						/* c2020 */
  *coli = (int) indices.integers[(*n)-1];
free(indices.integers);
}

/*$F
 * gqcr  -  Inquire Color Representation
 *
 * int *wkid;          workstation identifier
 * int *coli;          colour index          
 * int *type;          type of returned values
 * int *errind;        error indicator     
 * float *cr;          colour (red)         
 * float *cg;          colour (green)        
 * float *cb;          colour (blue)          
 *

 *
 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.74
 */
gqcr_ (wkid,coli,type,errind,cr,cg,cb)   
int *wkid;         
int *coli;        
int *type;       
int *errind;    
float *cr;     
float *cg;    
float *cb;   
{
Gcobundl rep;
debug(("Inquire Color Rep \n "));
TYPEOFRETURNEDVALUES(errind,*type,errginqcolourrep);  /* c2043 */
if (*errind = ginqcolourrep((Gint) *wkid,(Gint) *coli,(Gqtype) *type,&rep)) return;

*cr = (float) rep.red;
*cg = (float) rep.green;
*cb = (float) rep.blue;
}

/*$F
 * gqwkt - Inquire Workstation Transformation
 *
 * int *wkid;          workstation identifier   
 * int *errind;        error indicator           
 * int *tus;           workstation transformation update state
 * float rwindo[4];    requested workstation window   
 * float cwindo[4];    current workstation window      
 * float rviewp[4];    requested workstation viewport 
 * float cviewp[4];    current workstation viewport  

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.74
 */
gqwkt_ (wkid,errind,tus,rwindo,cwindo,rviewp,cviewp)   
int *wkid;         
int *errind;      
int *tus;        
float rwindo[4];
float cwindo[4]; 
float rviewp[4]; 
float cviewp[4];
{
Gwsti  wstran; 

debug(("Inquire Workstation Transformation\n"));
if (*errind = ginqwstran((Gint) *wkid,&wstran)) return;
 
*tus = (int)  wstran.wstus;

rwindo[0] = (float)  wstran.request.w.xmin;
rwindo[1] = (float)  wstran.request.w.xmax;
rwindo[2] = (float)  wstran.request.w.ymin;
rwindo[3] = (float)  wstran.request.w.ymax;
 
cwindo[0] = (float)  wstran.current.w.xmin;
cwindo[1] = (float)  wstran.current.w.xmax;
cwindo[2] = (float)  wstran.current.w.ymin;
cwindo[3] = (float)  wstran.current.w.ymax;

rviewp[0] = (float)  wstran.request.v.xmin;
rviewp[1] = (float)  wstran.request.v.xmax;
rviewp[2] = (float)  wstran.request.v.ymin;
rviewp[3] = (float)  wstran.request.v.ymax;

cviewp[0] = (float)  wstran.current.v.xmin;
cviewp[1] = (float)  wstran.current.v.xmax;
cviewp[2] = (float)  wstran.current.v.ymin;
cviewp[3] = (float)  wstran.current.v.ymax;
}

/*$F
 * gqsgwk - Inquire Set Member of Segment Names on Workstation
 *
 * int *wkid;         workstation identifier 
 * int *n;            set member requested   
 * int *errind;       error indicator        
 * int *ol;           number of segment names
 * int *sgna;         Nth element of list    

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.166
 */
gqsgwk_ (wkid,n,errind,ol,sgna)   
int *wkid;        
int *n;          
int *errind;    
int *ol;       
int *sgna;    
{
Gintlist segs;
debug(("Inquire Set Member of Segment Names on Workstation \n"));

if (*errind = ginqsegnamesws((Gint) *wkid,&segs)) return;

/* set return values not dependent on range check */
*ol = (int)  segs.number;                       /* c2020 */

VALIDMEMBER(errind,*n,0,segs.number,errginqsegnamesws);  /* c2014 c2043 c2020 */
if (*n)					/* c2020 */
  *sgna = (int)  segs.integers[(*n)-1]; /* c2020 */
free(segs.integers);
}

/*$F
 * gqlcs - Inquire Locator Device State
 *
 * Input Parameters:
 *
 * int   	wkid - Workstation Identifier 
 * int  	lcdnr - Locator Device Number
 * int		type - type of returned values
 * 
 * Output Parameters:
 *
 * int 		*errind - Error Indicator 
 * int 		*mode - Operating Mode
 * int 		*esw - Echo Switch
 * int 		*tnr - Initial Normalization Transformation
 * float 	*ipx,ipy - Initial Locator Position (WC)
 * int 		*pet - prompt/echo type
 * float         *earea[4] - Echo Area (DC)
 * int  	*ldr - Number of array elements used in data record.
 * char		*datrec[80] - Data Record
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.166
 */
gqlcs_ (wkid,lcdnr,type,mldr,errind,mode,esw,tnr,ipx,ipy,pet,earea,ldr,datrec)  
int *wkid;
int *lcdnr;
int *type;
int *mldr;
int *errind; 
int *mode; 
int *esw; 
int *tnr; 
float *ipx,*ipy; 
int *pet; 
float *earea; 
int *ldr; 
char *datrec; 
{ 
Glocst state;

Gint il;
Gint rl;
Gint sl;
Gint *ia;
Gfloat *ra;
Gchar **str;
Gint *lstr;



debug(("Inquire Locator Device State \n "));

if (*errind = ginqlocst((Gint) *wkid, (Gint) *lcdnr, (Gqtype) *type, &state )) return;

*mode = (int) state.mode;
*esw = (int) !state.esw;		/* c2034 */
*tnr = (int) state.loc.transform;
*ipx = (float) state.loc.position.x;
*ipy = (float) state.loc.position.y;
*pet = (int) state.pet;
earea[0] = (float) state.e_area.xmin;
earea[1] = (float) state.e_area.xmax;
earea[2] = (float) state.e_area.ymin;
earea[3] = (float) state.e_area.ymax;
switch(*pet)
 {
  case 1: 
          il = 0;                                                    /*c2051*/
          rl = 0;						     /*c2051*/
          sl = 0;					 	     /*c2051*/
	  ra = NULL;					             /*c2051*/
	  ia = NULL;					             /*c2051*/
	  str = NULL;					             /*c2051*/
	  lstr = NULL;					             /*c2051*/
          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /*c2051*/
          break;
  case 2: 
          il = 0;                                                    /*c2051*/
          rl = 0;						     /*c2051*/
          sl = 0;					 	     /*c2051*/
	  ra = NULL;					             /*c2051*/
	  ia = NULL;					             /*c2051*/
	  str = NULL;					             /*c2051*/
	  lstr = NULL;					             /*c2051*/
          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /*c2051*/
          break;
  case 3: 
          il = 0;                                                    /*c2051*/
          rl = 0;						     /*c2051*/
          sl = 0;					 	     /*c2051*/
	  ra = NULL;					             /*c2051*/
	  ia = NULL;					             /*c2051*/
	  str = NULL;					             /*c2051*/
	  lstr = NULL;					             /*c2051*/
          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /*c2051*/
          break;
  case 4: 
  	  il = 7;
 	  rl = 1;
	  sl = 0;
	  *ldr = il + rl + sl; 
	  ia = (Gint *) malloc(il*sizeof(Gint *));
 	  ra = (Gfloat *) malloc(rl*sizeof(Gfloat *));
 	  str = NULL;
 	  lstr = NULL;
          ia[0] = state.record.pet4.acf;
          ia[1] = state.record.pet4.ln.type;
          ia[2] = state.record.pet4.ln.width;
          ia[3] = state.record.pet4.ln.color;
          ia[4] = state.record.pet4.ln.line;
          ia[5] = state.record.pet4.ln.bundl.type;
          ia[6] = state.record.pet4.ln.bundl.color;
	  ra[0] = state.record.pet4.ln.bundl.width;
	  gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /* c2038 2049 */
	  free(ia);
	  free(ra);
	  
 	  break;
  case 5:  
	  if (state.record.pet5.pfcf == GPF_POLYLINE)
            {
	     il = 8;
	     rl = 1;
	     sl = 0;
	     lstr = NULL;
 	     str = NULL;
             *ldr = il + rl + sl;
	     ia = (Gint *) malloc(il*sizeof(Gint *));
 	     ra = (Gfloat *) malloc(rl*sizeof(Gfloat *));
             ia[0] = state.record.pet5.acf;
             ia[1] = state.record.pet5.pfcf;
             ia[2] = state.record.pet5.attr.ln.type;
             ia[3] = state.record.pet5.attr.ln.width;
             ia[4] = state.record.pet5.attr.ln.color;
             ia[5] = state.record.pet5.attr.ln.line;
             ia[6] = state.record.pet5.attr.ln.bundl.type;
             ia[7] = state.record.pet5.attr.ln.bundl.color;
   	     ra[1] = state.record.pet5.attr.ln.bundl.width;
   	     gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /* c2038 2049 */
	     free(ia);
 	     free(ra);
	    }
	  else
	    { 
             il = 9;
	     rl = 0;
	     sl = 0;
             *ldr = il + rl + sl;
	     ia = (Gint *) malloc(il*sizeof(Gint *));
 	     ra = NULL;
	     str = NULL;
	     lstr = NULL;
             ia[0] = state.record.pet5.acf;
             ia[1] = state.record.pet5.pfcf;
             ia[2] = state.record.pet5.attr.fl.inter;
             ia[3] = state.record.pet5.attr.fl.style;
             ia[4] = state.record.pet5.attr.fl.color;
             ia[5] = state.record.pet5.attr.fl.fill;
             ia[6] = state.record.pet5.attr.fl.bundl.inter;
             ia[7] = state.record.pet5.attr.fl.bundl.style;
             ia[8] = state.record.pet5.attr.fl.bundl.color;
   	     gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /* c2038 2049 */
	     free(ia);
            }
	     
          break;
  case 6: 
          il = 0;                                                    /*c2051*/
          rl = 0;						     /*c2051*/
          sl = 0;					 	     /*c2051*/
	  ra = NULL;					             /*c2051*/
	  ia = NULL;					             /*c2051*/
	  str = NULL;					             /*c2051*/
	  lstr = NULL;					             /*c2051*/
          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /*c2051*/
 }
}

/*$F
 * gqsks - Inquire Stroke Device State
 *
 * Input Parameters:
 *
 * int   	*wkid - Workstation Identifier 
 * int  	*skdnr - Stroke Device Number
 * int		*type - type of returned values
 * int		 *n  - maximum number of points
 * int		 *mldr - dimension of datarecord array
 * 
 * Output Parameters:
 *
 * int		 *errind - error indicator
 * int		 *mode - operating mode
 * int		 *esw - echo switch 
 * int		 *itnr - initial normalization transformation number
 * int		 *np - number of points
 * float         *pxa,*pya - initial points in stroke (WC) 
 * int 		 *pet - prompt/echo type
 * float  	 *earea - echo area
 * int 		 *buflen - buflen
 * int 		 *ldr number of array elements used in data record
 * char		 *datrec - data record
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.167
 */
gqsks_ (wkid,skdnr,type,n,mldr,errind,mode,esw,itnr,np,pxa,pya,pet,earea,buflen,ldr,datrec)
int *wkid;
int *skdnr;
int *type;
int *n;
int *mldr;
int *errind;
int *mode;
int *esw;
int *itnr;
int *np;
float *pxa,*pya;
int *pet;
float *earea;
int *buflen;
int *ldr;
char *datrec;
{ 
Gstrokest state;
Gint il;
Gint rl;
Gint sl;
Gint *ia;
Gfloat *ra;
Gchar **str;
Gint *lstr;
int  i;		/* c2038 */

debug(("Inquire Stroke Device State \n "));

if (*errind = ginqstrokest((Gint) *wkid, (Gint) *skdnr, (Gqtype) *type, &state)) return;  /* c2037 */

*mode = (int) state.mode;
*esw = (int) !state.esw;             /* c2034 */
*itnr = (int) state.stroke.transform;
*np = (int) state.stroke.n_points;
for (i=0;i<*np;i++) {
   pxa[i] = (float) state.stroke.points[i].x;	/* c2038 */
   pya[i] = (float) state.stroke.points[i].y;	/* c2038 */
   }
*pet = (int) state.pet;
earea[0] = (float) state.e_area.xmin;
earea[1] = (float) state.e_area.xmax;
earea[2] = (float) state.e_area.ymin;
earea[3] = (float) state.e_area.ymax;
*buflen = state.record.pet1.bufsiz;	/* c2038 */

switch(*pet)
 {
  case 1: 
   	  il = 2;
 	  rl = 3;
	  sl = 0;
	  *ldr = il + rl + sl; 
	  ia = (Gint *) malloc(il*sizeof(Gint *));
 	  ra = (Gfloat *) malloc(rl*sizeof(Gfloat *));
 	  str = NULL;
 	  lstr = NULL;
          ia[0] = state.record.pet1.bufsiz;
          ia[1] = state.record.pet1.editpos;
	  ra[0] = state.record.pet1.interval.x;
	  ra[1] = state.record.pet1.interval.y;
	  ra[2] = state.record.pet1.time;

	  gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /* c2038 2049 */
	  free(ia);
	  free(ra);
	  break;
  case 2: 
  	  il = 2;
 	  rl = 3;
	  sl = 0;
	  *ldr = il + rl + sl; 
	  ia = (Gint *) malloc(il*sizeof(Gint *));
 	  ra = (Gfloat *) malloc(rl*sizeof(Gfloat *));
 	  str = NULL;
 	  lstr = NULL;
          ia[0] = state.record.pet2.bufsiz;
          ia[1] = state.record.pet2.editpos;
	  ra[0] = state.record.pet2.interval.x;
	  ra[1] = state.record.pet2.interval.y;
	  ra[2] = state.record.pet2.time;

	  gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /* c2038 2049 */
	  free(ia);
	  free(ra);
break;
  case 3: 
  	  il = 9;
 	  rl = 4;
	  sl = 0;
	  *ldr = il + rl + sl; 
	  ia = (Gint *) malloc(il*sizeof(Gint *));
 	  ra = (Gfloat *) malloc(rl*sizeof(Gfloat *));
 	  str = NULL;
 	  lstr = NULL;
	  ia[0] = state.record.pet3.bufsiz;
	  ia[1] = state.record.pet3.editpos;
	  ia[2] = state.record.pet3.acf;
	  ia[3] = state.record.pet3.mk.type;
	  ia[4] = state.record.pet3.mk.size;
	  ia[5] = state.record.pet3.mk.color;
	  ia[6] = state.record.pet3.mk.mark;
	  ia[7] = state.record.pet3.mk.bundl.type;
	  ia[8] = state.record.pet3.mk.bundl.color;
	  
	  ra[0] = state.record.pet3.interval.x;
	  ra[1] = state.record.pet3.interval.y;
	  ra[2] = state.record.pet3.time;
	  ra[3] = state.record.pet3.mk.bundl.size;
	  gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /* c2038 2049 */
	  free(ia);
	  free(ra);
          break;

  case 4: 
  	  il = 9;
 	  rl = 4;
	  sl = 0;
	  *ldr = il + rl + sl; 
	  ia = (Gint *) malloc(il*sizeof(Gint *));
 	  ra = (Gfloat *) malloc(rl*sizeof(Gfloat *));
 	  str = NULL;
 	  lstr = NULL;
	  ia[0] = state.record.pet4.bufsiz;
	  ia[1] = state.record.pet4.editpos;
	  ia[2] = state.record.pet4.acf;
	  ia[3] = state.record.pet4.ln.type;
	  ia[4] = state.record.pet4.ln.width;
	  ia[5] = state.record.pet4.ln.color;
	  ia[6] = state.record.pet4.ln.line;
	  ia[7] = state.record.pet4.ln.bundl.type;
	  ia[8] = state.record.pet4.ln.bundl.color;
	  
	  ra[0] = state.record.pet4.interval.x;
	  ra[1] = state.record.pet4.interval.y;
	  ra[2] = state.record.pet4.time;
	  ra[3] = state.record.pet4.ln.bundl.width;
	  gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /* c2038 2049 */
	  free(ia);
	  free(ra);
	  
 }
}
/*$F
 * gqchs - Inquire Choice Device State 
 * 
 * int		 *wkid - workstation identifier
 * int		 *chdnr - choice device number 
 * int		 *mldr - dimension of data record array
 *
 * Output Parameters:
 *
 * int		 *errind - error indicator
 * int		 *mode - operating mode 
 * int		 *esw - echo switch 
 * int		 *istat - initial status
 * int		 *ichnr - initial choice number
 * int		 *pet prompt/echo type
 * float	 *earea echo area (DC)
 * int		 *ldr number of array elements used in data record
 * char		 *datrec data record
 *

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.168
 */
gqchs_ (wkid,chdnr,mldr,errind,mode,esw,istat,ichnr,pet,earea,ldr,datrec)
int *wkid;
int *chdnr;
int *mldr;
int *errind;
int *mode;
int *esw;
int *istat;
int *ichnr;
int *pet;
float *earea;
int *ldr;
char *datrec;
{ 
Gchoicest state;
Gint il;
Gint rl;
Gint sl;
Gint sum = 0;	/* c2054 */
Gint *ia;
Gfloat *ra;
Gchar *str;
Gint *lstr;
int i;

debug(("Inquire Choice Device State \n "));

if (*errind = ginqchoicest((Gint) *wkid, (Gint) *chdnr, &state)) return; /* c2039 */

*mode = (int) state.mode;            /* c2028 */
*esw = (int) !state.esw;            /* c2028 */
*istat = ((int) state.choice.status + 1) % 3; /* c2046 c2028 */ 
*ichnr = (int) state.choice.choice; /* c2028 */
*pet = (int) state.pet;             /* c2028 */
earea[0] = (float) state.e_area.xmin;
earea[1] = (float) state.e_area.xmax;
earea[2] = (float) state.e_area.ymin;
earea[3] = (float) state.e_area.ymax;

switch (*pet)
 {
  case 1: 
         il = 0;                                                    /*c2051*/
         rl = 0;						    /*c2051*/
         sl = 0;					 	    /*c2051*/
	 ra = NULL;					             /*c2051*/
	 ia = NULL;					             /*c2051*/
	 str = NULL;					             /*c2051*/
	 lstr = NULL;					             /*c2051*/
         gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /*c2051*/
	 break; 

  case 2:
          il = state.record.pet2.number;
          rl = 0;
          sl = 0;
          *ldr = il + rl + sl;
          ia = (Gint *) malloc(il * sizeof(Gint *));
          ra = NULL;
	  str = NULL;
	  lstr = NULL;
	  for (i = 0; i < il; i++)
	    ia[i] = state.record.pet2.enable[i];

          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /* c2038 2049 */
	  free(ia);
          break;

  case 3: 
          il = 0;
          rl = 0;
          sl = state.record.pet3.number;
          *ldr = il + rl + sl;
          ra = NULL;
	  ia = NULL;
          lstr = (Gint *) malloc(sl * sizeof(Gint));
	  for (i = 0; i < sl; i++)					/* c2054 */
 	    sum += lstr[i] = strlen(state.record.pet3.strings[i]);	/* c2054 */
          str = (Gchar *) malloc(sum * sizeof(Gchar));			/* c2054 */
	  *str = '\0';							/* c2054 */
	  for (i = 0; i < sl; i++)					/* c2054 */
 	    strcat (str, state.record.pet3.strings[i]);			/* c2054 */

          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /*c2038 2049*/
	  free(ia);
	  free(str);
	  free(lstr);
          break;
case 4:
          il = 1;
          rl = 0;
          sl = state.record.pet4.number;
          *ldr = il + rl + sl;
          ia = (Gint *) malloc(il * sizeof(Gint *));
          ra = NULL;
	  ia[0] = state.record.pet4.number;
          lstr = (Gint *) malloc(sl * sizeof(Gint));
	  for (i = 0; i < sl; i++)
 	    sum += lstr[i] = strlen(state.record.pet3.strings[i]);
          str = (Gchar *) malloc(sum * sizeof(Gchar));
	  *str = '\0';
	  for (i = 0; i < sl; i++)
 	    strcat (str, state.record.pet3.strings[i]);

          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /* c2038 2049 */
	  free(ia);
	  free(lstr);
	  free(str);
          break;
  case 5: 
   	  il = state.record.pet5.number + 2;
	  rl = 0;
	  sl = 0;
          *ldr = il + rl + sl;
          ia = (Gint *) malloc(il * sizeof(Gint *));
	  ra = NULL; 
 	  str = NULL;
	  lstr = NULL;
	  ia[0] = state.record.pet5.seg;
	  ia[1] = state.record.pet5.number;
	  for (i=2; i<il; i++) 
	    ia[i] = state.record.pet5.pickids[i];
          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /* c2038  2049*/
	  free(ia);

 }
}


/*$F
 * gqpks - Inquire Pick Device State
 * 
 * int		 *wkid - workstation identifier
 * int		 *pkdnr - pick device number 
 * int		 *type - type of returned values
 * int		 *mldr - dimension of data record array
 *
 * Output Parameters:
 *
 * int		 *errind - error indicator
 * int		 *mode - operating mode 
 * int		 *esw - echo switch
 * int		 *istat - initial status
 * int		 *isgna - initial segment
 * int		 *ipkid  - initial pick identifier
 * int		 *pet - prompt/echo type
 * float	 *earea echo area (DC)
 * int  	 *ldr - number of array elements used in data record 
 * char 	 *datrec - data record
 *

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.169
 */
gqpks_ (wkid,pkdnr,type,mldr,errind,mode,esw,istat,isgna,ipkid,pet,earea,ldr,datrec)
int *wkid;
int *pkdnr;
int *type;
int *mldr;
int *errind;
int *mode;
int *esw;
int *istat;
int *isgna;
int *ipkid;
int *pet;
float *earea;
int *ldr;
char *datrec;
{ 
Gpickst state;
int il;
int rl;
int sl;
int *ia;
float *ra;
char *str;
int *lstr;
debug(("Inquire Pick Device State \n "));

if (*errind = ginqpickst((Gint) *wkid,(Gint) *pkdnr, (Gqtype) *type, &state)) return;

*mode = (int) state.mode;
/* reverse sense of esw */
*esw = (int) !state.esw;             /* c2028 */
*istat = ((int) state.pick.status + 1) % 3;	/* c2046 */
*isgna = (int) state.pick.seg;
*ipkid = (int) state.pick.pickid;
*pet = (int) state.pet;
earea[0] = (float) state.e_area.xmin;
earea[1] = (float) state.e_area.xmax;
earea[2] = (float) state.e_area.ymin;
earea[3] = (float) state.e_area.ymax;
switch (*pet)
 {
  case 1: 
          il = 0;                                                    /*c2051*/
          rl = 0;						     /*c2051*/
          sl = 0;					 	     /*c2051*/
	  ra = NULL;					             /*c2051*/
	  ia = NULL;					             /*c2051*/
	  str = NULL;					             /*c2051*/
	  lstr = NULL;					             /*c2051*/
          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /*c2051*/
          break;
  case 2: 
          il = 0;                                                    /*c2051*/
          rl = 0;						     /*c2051*/
          sl = 0;					 	     /*c2051*/
	  ra = NULL;					             /*c2051*/
	  ia = NULL;					             /*c2051*/
	  str = NULL;					             /*c2051*/
	  lstr = NULL;					             /*c2051*/
          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /*c2051*/
          break;
  case 3: 
          il = 0;                                                    /*c2051*/
          rl = 0;						     /*c2051*/
          sl = 0;					 	     /*c2051*/
	  ra = NULL;					             /*c2051*/
	  ia = NULL;					             /*c2051*/
	  str = NULL;					             /*c2051*/
	  lstr = NULL;					             /*c2051*/
          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /*c2051*/
 }
}

/*$F
 * gqsts - Inquire String Device State
 * 
 * int		 *wkid - workstation identifier
 * int		 *stdnr - string device number
 * int		 *mldr - dimension of data record array 
 *
 * Output Parameters:
 *
 * int		 *errind - error indicator
 * int		 *mode - operating mode
 * int		 *esw - echo switch
 * int		 *lostr - number of characters returned
 * char		 *istr - initial string
 * int		 *pet - prompt echo type
 * float         *earea - echo area (DC)
 * int		 *buflen - input buffer size
 * int		 *inipos - initial cursor position
 * int		 *ldr - number of array elements used in data record
 * char		 *datrec data record
 *

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.170
 */
gqsts_ (wkid,stdnr,mldr,errind,mode,esw,lostr,istr,pet,earea,buflen,inipos,ldr,datrec)  
int *wkid;
int *stdnr;
int *mldr;
int *errind;
int *mode;
int *esw;
int *lostr;
char *istr;	/* c2038 c2067 */
int *pet;
float *earea;
int *buflen;
int *inipos;
int *ldr;
char *datrec;
{ 
Gstringst state;
Gint il;
Gint rl;
Gint sl;
Gint *ia;
Gfloat *ra;
Gchar **str;
Gint *lstr;


debug(("Inquire String Device State \n "));

if (*errind = ginqstringst((Gint) *wkid, (Gint) *stdnr, &state)) return;

*mode = (int) state.mode;
*esw = (int) !state.esw;
*lostr = (int) strlen(state.string); 
strcpy (istr, state.string);    /* c2038 c2067 */
*pet = (int) state.pet;
earea[0] = (float) state.e_area.xmin;
earea[1] = (float) state.e_area.xmax;
earea[2] = (float) state.e_area.ymin;
earea[3] = (float) state.e_area.ymax;
*buflen = (int) state.record.pet1.bufsiz;	/* c2038 */
*inipos = (int) state.record.pet1.position;	/* c2038 */
il = 2;
rl = 0;
sl = 0;
lstr = NULL;
ra = NULL;
str = NULL;
*ldr = il + rl + sl;
ia = (Gint *) malloc(il*sizeof(Gint *));
ia[0] = state.record.pet1.bufsiz;
ia[1] = state.record.pet1.position;
gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /* c2038 */
free(ia);
}

/*$F
 * gqvls - Inquire Valuator Device State
 *
 * int	*wkid		- pointer to workstation id
 * int	*vldnr		- pointer to valuator device number
 * int	*mldr		- pointer to dimension of data record array
 * int	*errind		- pointer to error indicator
 * int	*mode		- pointer to operating mode (GREQU, GSAMPL, GEVENT)
 * int	*esw		- pointer to echo switch (GNECHO, GECHO)
 * float *ival		- pointer to initial value
 * int	*pet		- pointer to prompt/echo type
 * float *earea[4]	- pointer to echo area in device coordinates
 *			  XMIN, XMAX, YMIN, YMAX
 * float *loval, *hival	- pointers to minimal and maximal values
 * int	*ldr		- pointer to number of array elements used in data record
 * char	*datrec[mldr]	- pointer to data record
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.168
 */
gqvls_ (wkid, vldnr, mldr, errind, mode, esw, ival, pet, earea, loval, hival, ldr, datrec)
int *wkid;
int *vldnr;
int *mldr;
int *errind;
int *mode;
int *esw;
float *ival;
int *pet;
float *earea;
float *loval;
float *hival;
int *ldr;
char *datrec;

{
Gvalst state;
Gint il;
Gint rl;
Gint sl;
Gint *ia;
Gfloat *ra;
Gchar **str;
Gint *lstr;

debug(("Inquire Valuator Device State \n  "));
if (*errind = ginqvalst((Gint) *wkid, (Gint) *vldnr, &state)) return;

*mode = (int) state.mode;
*esw = (int) !state.esw;
*ival = (float)  state.val;
*pet = (int) state.pet;
earea[0] = (float) state.e_area.xmin;
earea[1] = (float) state.e_area.xmax;
earea[2] = (float) state.e_area.ymin;
earea[3] = (float) state.e_area.ymax;
*loval = (float) state.record.pet1.low;		/* c2038 */
*hival = (float) state.record.pet1.high;	/* c2038 */
switch (*pet)
 {
  case 1: 
          il = 0;
          rl = 2;
          sl = 0;
 	  ia = NULL;
          ra = (Gfloat *) malloc(rl*sizeof(Gfloat *));
 	  str = NULL;
          lstr = NULL;
          *ldr = il + rl + sl;
	  ra[0] = state.record.pet1.low;
	  ra[1] = state.record.pet1.high;
          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /* c2038 */
   	  free(ra);	

          break;
  case 2:
          il = 0;
          rl = 2;
          sl = 0;
 	  ia = NULL;
          ra = (Gfloat *) malloc(rl*sizeof(Gfloat *));
 	  str = NULL;
          lstr = NULL;
          *ldr = il + rl + sl;
	  ra[0] = state.record.pet2.low;
	  ra[1] = state.record.pet2.high;
          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /* c2038 */
   	  free(ra);	
          break;
  case 3:
          il = 0;
          rl = 2;
          sl = 0;
 	  ia = NULL;
          ra = (Gfloat *) malloc(rl*sizeof(Gfloat *));
 	  str = NULL;
          lstr = NULL;
          *ldr = il + rl + sl;
	  ra[0] = state.record.pet3.low;
	  ra[1] = state.record.pet3.high;
          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /* c2038 2049 */
   	  free(ra);	
          break;
 }
}

/*$F
 * gqstss - Inquire String Device State - Subset Version
 *
 * int	*wkid		- pointer to workstation id
 * int	*stdnr		- pointer to string device number
 * int	*mldr		- pointer to dimension of data record array
 * int	*errind		- pointer to error indicator
 * int	*mode		- pointer to operating mode (GREQU, GSAMPL, GEVENT)
 * int	*esw		- pointer to echo switch (GNECHO, GECHO)
 * int 	*lostr		- pointer to number of characters returned
 * char	*istr		- pointer to initial string
 * int	*pet		- pointer to prompt/echo type
 * float *earea[4]	- pointer to echo area in device coordinates
 *			  XMIN, XMAX, YMIN, YMAX
 * int	*buflen		- pointer to input buffer size
 * int	*inipos		- pointer to initial cursor position
 * int	*ldr		- pointer to number of array elements used in data record
 * char	*datrec[mldr]	- pointer to data record
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.170
 */
gqstss_ (wkid, stdnr, mldr, errind, mode, esw, lostr, istr, pet, earea, buflen, inipos, ldr, datrec)
int *wkid;
int *stdnr;
int *mldr;
int *errind;
int *mode;
int *esw;
int *lostr;
char *istr;		/* c2067 */
int *pet;
float *earea;
int *buflen;
int *inipos;
int *ldr;
char *datrec;

{
Gstringst state;
int il;
int rl;
int sl;
int *ia;
float *ra;
char *str;
int *lstr;
debug(("Inquire String Device State (SUBSET VERSION) \n "));

if (*errind = ginqstringst((Gint) *wkid, (Gint) *stdnr, &state)) return;

*mode = (int) state.mode;
*esw = (int) !state.esw;
*lostr = (int) strlen(state.string); 
strcpy (istr, state.string);    /* c2038 c2067 */
*pet = (int)  state.pet;
earea[0] = (float) state.e_area.xmin;
earea[1] = (float) state.e_area.xmax;
earea[2] = (float) state.e_area.ymin;
earea[3] = (float) state.e_area.ymax;
*buflen = (int) state.record.pet1.bufsiz;	/* c2038 */
*inipos = (int) state.record.pet1.position;	/* c2038 */
il = 0;                                                    /*c2051*/
rl = 0;	 					           /*c2051*/
sl = 0;					 	           /*c2051*/
ra = NULL;					           /*c2051*/
ia = NULL;					           /*c2051*/
str = NULL;					           /*c2051*/
lstr = NULL;					           /*c2051*/
gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /*c2051*/
}
