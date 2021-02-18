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
 * GKS Workstation Attribute Functions :
 *	gsplr_
 *	gspmr_
 *	gstxr_
 *	gsfar_
 *	gspar_
 *	gscr_
 *        
 * David Berkowitz
 * Bruce Haimowitz
 * TCS Development
 * Cambridge MA
 *
 * August 31 1988
 *
 * $Header: rep.c,v 4.0 89/08/31 18:37:12 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/rep.c,v $
 * 
 * $Log:	rep.c,v $
 * Revision 4.0  89/08/31  18:37:12  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.15  89/06/26  16:02:23  amy
 *  DCR d1	Declare errfp external for use in error macros.
 * 		Change references to errfpp to errfp.
 * 
 * Revision 1.14  89/06/16  08:10:43  bruce
 * PTR# c1175:	Corrected calls to realloc that were not storing the
 * 		pointer to the new memory.
 * 
 * Revision 1.13  89/06/06  09:56:24  bruce
 * PTR# c1179:	Made changes to character expansion field names for
 * 		AIX compiler.
 * 
 * Revision 1.12  89/04/07  09:54:59  bruce
 * PTR# c2044:	Corrected 2D handling of arrays.
 * 
 * Revision 1.11  89/03/16  10:25:34  amy
 * Removed Set Pattern Size and Reference Point functions-- they
 * were already in wsioutattr.c.
 * 
 * Revision 1.10  89/03/16  10:22:12  amy
 * PTR c1144	Added Set Pattern Size and Set Pattern Reference Point
 * 		functions.
 * 
 * Revision 1.9  89/01/06  09:00:00  bruce
 * Implemented pattern rep setting so user can specify rep as a subset block from
 * a larger 2-d array.  This seems to work properly, evenly though the verification
 * reports an error.  c2017.
 * 
 * Revision 1.8  88/12/27  14:37:36  todd
 * Added externs for global vars.
 * 
 * Revision 1.7  88/12/05  15:20:30  owens
 * changed errfp to errfpp
 * 
 * Revision 1.5  88/12/05  14:26:15  bhaim
 * Changes for Code II
 * 
 * Revision 1.4  88/11/14  09:28:26  bruce
 * Fixed type casting.
 * 
 * Revision 1.3  88/09/28  14:11:48  bruce
 * Added function prologues, added type casting for GKS and corrected variable 
 * names.
 * 
 * Revision 1.2  88/09/27  11:27:29  todd
 * Removed misplaced functions.
 * 
 * Revision 1.1  88/09/27  08:17:28  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: rep.c,v 4.0 89/08/31 18:37:12 amy Exp $";

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
 * gsplr - Set Polyline Representation
 *
 * int   *wkid   - pointer to workstation id
 * int   *pli    - pointer to polyline index
 * int   *ltype  - pointer to polyline type
 * float *lwidth - pointer to polyline width
 * int   *coli   - pointer to polyline colour

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.100
 */
gsplr_ (wkid, pli, ltype, lwidth, coli)
int *wkid;
int *pli;
int *ltype;
float *lwidth;
int  *coli;

{
Glnbundl rep;

debug ( ("Set Polyline Representation %d  %d \n", *wkid, *pli) );

rep.type = (Gint)*ltype;
rep.width = (Gfloat)*lwidth;
rep.colour = (Gint)*coli;

gsetlinerep ((Gint)*wkid, (Gint)*pli, &rep);

}

/*$F
 * gspmr - Set Polymarker Representation
 *
 * int   *wkid  - pointer to workstation id
 * int   *pmi   - pointer to polymarker index
 * int   *mtype - pointer to polymarker type
 * float *mszsf - pointer to polymarker size scale factor
 * int   *coli  - pointer to polymarker colour index

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.101
 */
gspmr_ (wkid, pmi, mtype, mszsf, coli)
int *wkid;
int *pmi;
int *mtype;
float *mszsf;
int  *coli;

{
Gmkbundl rep;

debug ( ("Set Polymarker Representation %d  %d \n", *wkid, *pmi) );

rep.type = (Gint)*mtype;
rep.size = (Gfloat)*mszsf;
rep.colour = (Gint)*coli;

gsetmarkerrep ((Gint)*wkid, (Gint)*pmi, &rep);

}

/*$F
 * gstxr - Set Text Representation
 *
 * int   *wkid - pointer to workstation id
 * int   *txi  - pointer to text index
 * int   *font - pointer to text font
 * int   *prec - pointer to text precision
 * float *chxp - pointer to character expansion factor
 * float *chsp - pointer to character spacing
 * int   *coli - pointer to text colour index

 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.102
 */
gstxr_ (wkid, txi, font, prec, chxp, chsp, coli)
int *wkid;
int *txi;
int *font;
int *prec;
float *chxp;
float *chsp;
int  *coli;

{
Gtxbundl rep;

rep.fp.font = (Gint)*font;
TEXTPRECISION (*prec, errgsettextrep)
rep.fp.prec = (Gtxprec)*prec;
rep.ch_exp = (Gfloat)*chxp;	/* c1179 */
rep.space = (Gfloat)*chsp;
rep.colour = (Gint)*coli;

debug ( ("Set Text  Representation %d  %d \n", *wkid, *txi) );

gsettextrep ((Gint)*wkid, (Gint)*txi, &rep);


}

/*$F
 * gsfar - Set Fill Area Representation
 *
 * int *wkid  - pointer to workstation id
 * int *fai   - pointer to fill area index
 * int *ints  - pointer to fill area interior style
 * int *styli - pointer to fill area interior style index
 * int *coli  - pointer to fill area colour index

 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.102
 */
gsfar_ (wkid, fai, ints, styli, coli)
int *wkid;
int *fai;
int *ints;
int *styli;
int *coli;

{
Gflbundl rep;

debug ( ("Set Fill Area Representation %d  %d \n", *wkid, *fai) );

FILLAREAINTERIORSTYLE (*ints, errgsetfillrep)
rep.inter = (Gflinter)*ints;
rep.style = (Gint)*styli;
rep.colour = (Gint)*coli;

gsetfillrep ((Gint)*wkid, (Gint)*fai, &rep);

}

/*$F
 * gspar - Set Pattern Representation
 *

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.104
 */
gspar_ (wkid,pai,dimx,dimy,isc,isr,dx,dy,colia)  
int *wkid;
int *pai;
int *dimx,*dimy;
int *isc,*isr;
int *dx,*dy;
int *colia;
{ 
Gptbundl rep;
Gint *parray;
int sizeofmatrix;

rep.size.x = (Gfloat) *dx;			/* c2017 */
rep.size.y = (Gfloat) *dy;			/* c2017 */
debug (("Set Pattern Size:  %d - %d\n", rep.size.x, rep.size.y));

sizeofmatrix = sizeof(Gint)*(*dx)*(*dy);	/* c2017 */
if (sizeofmatrix > currfortint)       /* Check amount of memory needed */
  {
   fortint = (Gint *)realloc(fortint,sizeofmatrix); /* realloc more memory  c1175 */
   if (fortint == NULL)     /* If memory allocation unsuccessful call error routine */ 
     {
      gerrorhand(300,errgsetpatrep,(errfp)); 	/* d1 */
      return;
     }
   else
      currfortint = sizeofmatrix;
  }

parray = fortint;

/* c2044:  This array does NOT need to be transposed. */

  /* c2044:  Take the dx-by-dy subset starting at (isc,isr). */
  {                                                     /* c2044 */
  int i,j;                                              /* c2044 */
  int *fortstart; /* ptr to colia(isc,isr) */           /* c2044 */

  fortstart = colia + ((*isr - 1)*(*dimx) + (*isc - 1));
                        /* c2044 */
  for (i = 0; i < *dy; i++)                             /* c2044 */
     for (j = 0; j < *dx; j++)                          /* c2044 */
        *(parray + (i*(*dx) + j)) = (Gint) *(fortstart + (i*(*dimx) + j));
                                /* c2044 */
  }                                                     /* c2044 */

rep.array = (Gint * )parray;                            /* c2044 */

gsetpatrep ((Gint)*wkid, (Gint)*pai, &rep);


}

/*$F
 * gscr - Set Colour Representation
 *
 * int   *wkid - pointer to workstation id
 * int   *ci   - pointer to colour index
 * float *cr   - pointer to red value
 * float *cg   - pointer to green value
 * float *cb   - pointer to blue value

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.105
 */

gscr_ (wkid, ci, cr, cg, cb)
int *wkid;
int *ci;
float *cr;
float *cg;
float *cb;

{
Gcobundl rep;

debug ( ("Set Color Representation %d  %d \n", *wkid, *ci) );

rep.red = (Gfloat)*cr;
rep.green = (Gfloat)*cg;
rep.blue = (Gfloat)*cb;

gsetcolourrep ((Gint)*wkid, (Gint)*ci, &rep);

}


