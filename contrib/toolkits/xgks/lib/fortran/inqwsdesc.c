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
 * GKS inquire workstation description table functions:
 *	gqwkca_
 *	gqwkcl_
 *	gqdsp_
 *	gqdwka_
 *	gqplf_
 *	gqpmf_
 *	gqfaf_
 *	gqpaf_
 *	gqcf_
 *	gqpcr_
 *	gqegdp_
 *	gqgdp_
 *	gqlwk_
 *	gqsgp_
 *	gqdsga_
 *	gqli_
 *	gqptxr_
 *	gqdlc_
 *	gqdds_
 *        
 * David Berkowitz
 * Bruce Haimowitz
 * Todd Gill
 * TCS Development
 * Cambridge MA
 *
 * September 29, 2988
 *
 * $Header: inqwsdesc.c,v 4.0 89/08/31 18:33:12 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/inqwsdesc.c,v $
 * 
 * $Log:	inqwsdesc.c,v $
 * Revision 4.0  89/08/31  18:33:12  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.27  89/06/26  15:44:12  amy
 * DCR d1	Declare errfp external for use in error macros.
 * 		Change reference to errfpp to errfp.
 * 
 * Revision 1.26  89/06/06  09:56:31  bruce
 * PTR# c1179:	Made changes to character expansion field names for
 * 		AIX compiler.
 * PTR# c2073:	gqplf, gqpmf and gqfaf were freeing non pointers. 
 * 		free(fac.hatches) should be free(fac.hatches.integers)
 * 
 * Revision 1.25  89/03/31  11:11:14  bruce
 * PTR# c2051:	Moved VALIDMEMBER code back to before switch
 * 		statement so *ptr is set for switch.
 * 		Also corrected structure ref in switch.
 * 
 * Revision 1.24  89/03/15  20:14:02  bruce
 * PTR# c2020:	Functions now return values not dependant on member range
 * 		always.  Request member 0 is OK.
 * 
 * Revision 1.23  89/03/15  15:52:34  bruce
 * PTR# c2051:	Added code to pack datarecord for each pet.
 * 
 * Revision 1.22  89/03/15  15:20:38  bruce
 * PTR c2042:  functions now set errind before calling gerrorhand
 * 	       also includes changes for PTR c2044 that fixed pattern
 * 	       array problems.
 * 
 * Revision 1.21  89/03/15  14:53:23  amy
 * PTR c2043	All calls to VALIDMEMBER:  added value for errind 
 * 		parameter.
 * 
 * PTR c2020	Inq. List Element and Inq. Set Member functions:  
 * 		return values of output parameters regardless
 * 		of validity of the n parameter, and accept 0 as valid value
 * 		for n.
 * 
 * PTR c2051	Pack a data record for every pet of a given input device.
 * 
 * Revision 1.20  89/03/14  18:36:08  amy
 * PTR c2047	gqdst:  fix dereferencing of parameters to call to
 * 		ginqdefstring.
 * 		Correct typo: set il = 2, not rl.
 * 
 * Revision 1.19  89/03/14  17:35:17  amy
 * PTR c2049	Fixed calls to ginqdefloc, gprec and ginqdefpick.
 * 
 * Revision 1.18  89/01/20  13:27:41  todd
 * Corrected calls to gprec.
 * 
 * Revision 1.17  88/12/30  11:17:24  bruce
 * checked out for debugging - no changes.
 * 
 * Revision 1.16  88/12/28  14:27:00  todd
 * Added check for valid wtype.
 * 
 * Revision 1.15  88/12/05  15:19:46  owens
 * changed errfp to errfpp
 * 
 * Revision 1.13  88/12/05  13:35:54  owens
 * code review changes
 * 
 * Revision 1.12  88/11/16  10:19:25  todd
 * Removed extra call to gerrorhand.
 * Added check for ok return code so
 * a return is done before output parameters
 * are set.
 * 
 * Revision 1.11  88/11/07  13:17:07  amy
 * Added stubs for gqegdp and gqgdp.
 * 
 * Revision 1.10  88/11/07  11:34:47  todd
 * Finished functions and comments
 * 
 * Revision 1.9  88/10/11  17:03:27  todd
 * No changes.
 * 
 * Revision 1.8  88/10/11  14:34:03  owens
 * more changes from code review
 * 
 * Revision 1.7  88/10/05  06:58:17  amy
 * Fixed compile errors.
 * 
 * Revision 1.6  88/10/04  15:17:31  amy
 * Added stubs for gqdlc, gqdds, gqppmr, and gqtxf.
 * 
 * Revision 1.5  88/10/04  13:17:18  amy
 * Added stubs for functions gqtxr, gqpfar, gqppar, gqdsk, gqdvl, gqdch, gqdpk, and
 * gqdst.
 * 
 * Revision 1.4  88/09/30  11:28:40  owens
 * more code review changes
 * 
 * Revision 1.3  88/09/29  16:51:56  owens
 * more code review changes
 * 
 * Revision 1.2  88/09/29  16:27:49  owens
 * code review changes
 * 
 * Revision 1.1  88/09/27  08:17:18  todd
 * Initial revision
 * 
 */

static char *rcsid = "$Header: inqwsdesc.c,v 4.0 89/08/31 18:33:12 amy Exp $";

extern char **wtypes;
extern int NUMWTYPES;

#include "fortxgks.h"
#include <xgks.h>

extern FILE *errfp;             /* d1 */

/*$F
 * gqwkca - inquire workstation category
 *
 * int *wktype - pointer to workstation type
 * int *errind - pointer to error indicator         
 * int *wkcat - pointer to workstation category
 *
 * Returns: Error 2000 in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.170
 */
gqwkca_ (wtype, errind, wkcat)
int *wtype;
int *errind;
int *wkcat;
{
Gwscat cat;

debug ( ("Inquire Workstation Category %d  \n", *wtype) );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqwscategory); /* c2043 */

/* wtypes is an array in fortxgks.h that maps  */
/* a FORTRAN integer wtype to a C string wtype */


if (*errind = ginqwscategory ( (Gchar *) wtypes[*wtype-1], &cat)) return;

/* here we take advantage of the fact that C enumerated types are
consecutive integers starting at zero
*/

debug ( ("        Workstation Category is %d  \n", cat) );

*wkcat = (int) cat;

}


/*$F
 * gqwkcl - inquire workstation class
 *
 * int *wtype - pointer to workstation type
 * int *errind - pointer to error indicator
 * int *vrtype - pointer to workstation class (returned)

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p. 171
 */
gqwkcl_ (wtype, errind, vrtype)
int *wtype;
int *errind;
int *vrtype;
{
Gwsclass class;

debug ( ("Inquire Workstation Class %d  \n", *wtype) );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqwsclass);  /* c2043 */

/* wtypes is an array in fortxgks.h that maps a FORTRAN integer wtype
to a C string wtype */


if (*errind = ginqwsclass ( (Gchar *) wtypes[*wtype-1], &class)) return;

debug ( ("        Workstation Class is %d  \n", class) );

*vrtype = (int) class;
}


/*$F
 * gqdsp - inquire display space size
 *
 * int *wtype  - workstation type 
 * int *errind - error indicator
 * int *dcunit - display space units
 * float *rx   - raster x 
 * float *ry   - raster y
 * int *lx     - device x
 * int *ly     - device y

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.171
 */
gqdsp_ (wtype, errind, dcunit, rx, ry, lx, ly)
int *wtype;
int *errind;
int *dcunit;
float *rx;
float *ry;
int *lx;
int *ly;

{
Gdspsize dspsz;

debug ( ("Inquire Display Space Size %d \n", *wtype) );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqdisplayspacesize); /* c2043 */

/* wtypes is an array in fortxgks.h that maps a FORTRAN integer wtype
to a C string wtype */


if (*errind = ginqdisplayspacesize ( (Gchar *) wtypes[*wtype-1], &dspsz)) return;

*dcunit	= (int) dspsz.units;
*rx 	= (float) dspsz.device.x;
*ry	= (float) dspsz.device.y;
*lx	= (int) dspsz.raster.x;
*ly	= (int) dspsz.raster.y;

}


/*$F
 * gqdwka - inquire dynamic modification of workstation attributes  
 *
 * int *wtype  - workstation type 
 * int *errind - error indicator
 * int *plbun  - polyline bundle
 * int *pmbun  - polymarker bundle
 * int *txbun  - text bundle
 * int *fabun  - fill area bundle
 * int *parep  - pattern rep.
 * int *colrep - color 
 * int *wktr   - workstation transformation

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.171
 */
gqdwka_ (wtype, errind, plbun, pmbun, txbun, fabun, parep, colrep, wktr)
int *wtype;
int *errind;
int *plbun;
int *pmbun;
int *txbun;
int *fabun;
int *parep;
int *colrep;
int *wktr;

{
Gmodws dyn;

debug ( ("Inquire Dynamic Modification of Workstation Attributes %d \n", *wtype) );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqmodwsattr); /* c2043 */

/* wtypes is an array in fortxgks.h that maps a FORTRAN integer wtype
to a C string wtype */


if (*errind = ginqmodwsattr ( (Gchar *) wtypes[*wtype-1], &dyn)) return;

*plbun	= (int) dyn.line;
*pmbun	= (int) dyn.mark;
*txbun	= (int) dyn.text;
*fabun	= (int) dyn.fill;
*parep	= (int) dyn.pat;
*colrep	= (int) dyn.colour;
*wktr	= (int) dyn.wstran;

}

/*$F
 * gqplf - inquire polyline facilities
 *
 * int *wtype    - workstation type 
 * int *n        - member to return from nlt
 * int *errind   - error indicator
 * int *nlt      - list of return info 
 * int *lt       - *nlt[*n] (nth item in nlt)
 * int *nlw      - number of line widths
 * float *nomlw  - nominal width
 * float *rlwmin - minimum width
 * float *rlwmax - maximum width
 * int *nppli    - number of predefined polyline bundles

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.173
 */
gqplf_ (wtype, n, errind, nlt, lt, nlw, nomlw, rlwmin, rlwmax, nppli)
int *wtype;
int *n;
int *errind;
int *nlt;
int *lt;
int *nlw;
float *nomlw;
float *rlwmin;
float *rlwmax;
int *nppli;

{
Glnfac fac;

debug ( ("Inquire Polyline Facilities %d \n", *wtype) );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqlinefacil); /* c2043 */

/* wtypes is an array in fortxgks.h that maps a FORTRAN integer wtype
to a C string wtype */


if (*errind = ginqlinefacil ( (Gchar *) wtypes[*wtype-1], &fac)) return;

*nlt	= (int) fac.types.number;
*nlw	= (int) fac.widths;
*nomlw	= (float) fac.nom;
*rlwmin	= (float) fac.min;
*rlwmax	= (float) fac.max;
*nppli	= (int) fac.predefined;

VALIDMEMBER (errind,*n,0,*nlt,errginqlinefacil); /* c2043 c2020 */
if (*n)					/* c2020 */
   *lt = (int) fac.types.integers[*n-1];

/* free list returned by function */
free (fac.types.integers);		/* c2073 */
}

/*$F
 * gqpmf - inquire polymarker facilities
 *
 * int *wtype    - workstation type
 * int *n        - member to return from nmt
 * int *errind   - error indicator
 * int *nmt      - array of return values
 * int *mt       - nmt[n]
 * int *nms      - number of marker sizes
 * float *nomms  - nominal size
 * float *rmsmin - minimum size
 * float *rmsmax - maximum size
 * int *nppmi    - number of predefined bundles

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.171
 */
gqpmf_ (wtype, n, errind, nmt, mt, nms, nomms, rmsmin, rmsmax, nppmi)
int *wtype;
int *n;
int *errind;
int *nmt;
int *mt;
int *nms;
float *nomms;
float *rmsmin;
float *rmsmax;
int *nppmi;

{
Gmkfac fac;

debug ( ("Inquire Polymarker Facilities %d \n", *wtype) );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqmarkerfacil);   /* c2043 */

/* wtypes is an array in fortxgks.h that maps a FORTRAN integer wtype
to a C string wtype */


if ( *errind = ginqmarkerfacil ( (Gchar *) wtypes[*wtype-1], &fac)) return;

*nmt	= (int) fac.types.number;
*nms	= (int) fac.sizes;
*nomms	= (float) fac.nom;
*rmsmin	= (float) fac.min;
*rmsmax	= (float) fac.max;
*nppmi	= (int) fac.predefined;


VALIDMEMBER (errind,*n,0,*nmt,errginqmarkerfacil); /* c2043 c2020 */

if (*n)					/* c2020 */
   *mt = (int) fac.types.integers[*n-1];

/* free memory returned by function */
free (fac.types.integers);		/* c2073 */
}

/*$F
 * gqfaf - inquire fill area facilities
 *
 * int *wtype  - workstation type
 * int *ni     - number of the interior style requested
 * int *nh     - number of the hatch style requested
 * int *errind - error indicator
 * int *nis    - number of available interior styles
 * int *is     - interiors[ni] 
 * int *nhs    - number of available hatch styles
 * int *hs     - hatches[nh]
 * int *npfai  - number of predefined bundles

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.177
 */
gqfaf_ (wtype, ni, nh, errind, nis, is, nhs, hs, npfai)
int *wtype;
int *ni;
int *nh;
int *errind;
int *nis;
int *is;
int *nhs;
int *hs;
int *npfai;

{
Gflfac fac;

debug ( ("Inquire Fill Area Facilities %d \n", *wtype) );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqfillfacil); /* c2043 */

/* wtypes is an array in fortxgks.h that maps a FORTRAN integer wtype
to a C string wtype */

if (*errind = ginqfillfacil ( (Gchar *) wtypes[*wtype-1], &fac)) return;

*nis	= (int) fac.interiors.number;
*nhs	= (int) fac.hatches.number;
*npfai	= (int) fac.predefined;

*is = 0;
VALIDMEMBER (errind,*ni,0,*nis,errginqfillfacil); /* c2043 c2020 */
if (*ni)					/* c2020 */
   *is = (int) fac.interiors.integers[*ni-1];
free (fac.interiors.integers);			/* c2073 */

*hs = 0;
VALIDMEMBER (errind,*nh,0,*nhs,errginqfillfacil); /* c2043 c2020 */
if (*nh)					/* c2020 */
   *hs = (int) fac.hatches.integers[*nh-1];
free (fac.hatches.integers);			/* c2073 */

}

/*$F
 * gqpaf - inquire pattern facilities
 *
 * int *wtype  - workstation type
 * int *errind - error indicator
 * int *nppai  - number of patterns

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.179
 */
gqpaf_ (wtype, errind, nppai)
int *wtype;
int *errind;
int *nppai;

{
debug ( ("Inquire Pattern Facilities %d \n", *wtype) );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqpatfacil); /* c2043 */


*errind = ginqpatfacil ( (Gchar *) wtypes[*wtype-1], (Gint *) nppai);
}

/*$F
 * gqcf - inquire color facilities
 *
 * int *wtype  - workstation type 
 * int *errind - error indicator
 * int *ncoli  - number of colors 
 * int *cola   - color availability
 * int *npci   - number of predefined bundles

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.180
 */
gqcf_ (wtype, errind, ncoli, cola, npci)
int *wtype;
int *errind;
int *ncoli;
int *cola;
int *npci;

{
Gcofac fac;

debug ( ("Inquire Color Facilities %d  \n", *wtype) );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqcolourfacil); /* c2043 */

/* wtypes is an array in fortxgks.h that maps a FORTRAN integer wtype
to a C string wtype */

if (*errind = ginqcolourfacil ( (Gchar *) wtypes[*wtype-1], &fac)) return;

*ncoli	= (int) fac.colours;

/* this is supposed to be 0 for color and 1 for mono
    but we are getting the reverse

   quick hack is to negate it
*/

switch (fac.coavail)
  {
  case GCOLOR : 
    *cola = FORT_GCOLOR;
    break;
  case GMONOCHROME :
    *cola = FORT_GMONOC;
    break;
  };
*npci	= (int) fac.predefined;

debug ( ("        Number of Colors is %d  \n", *ncoli) );
debug ( ("        Color Availibility is %d  \n", *cola) );
debug ( ("        Number of Predefined Bundles is %d  \n", *npci) );

}

/*$F
 * gqppmr - Inquire Predefined Polymarker Representation
 *
 * int	*wtype		- pointer to workstation type 
 * int	*pmi		- pointer to predefined polymarker index
 * int	*errind		- pointer to error indicator
 * int	*mtype		- pointer to marker type
 * float *mszsf		- pointer to marker size scale factor
 * int	*coli		- pointer to polymarker colour index

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.175 
 */
gqppmr_ (wtype, pmi, errind, mtype, mszsf, coli)
int *wtype;
int *pmi;
int *errind;
int *mtype;
float *mszsf;
int *coli;

{
Gmkbundl rep;
debug ( ("Inquire Predefined Polymarker Representation \n") );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqpredmarkerrep); /* c2043 */

if (*errind = ginqpredmarkerrep ((Gchar *) wtypes[*wtype-1], (Gint) *pmi,&rep)) return;

*mtype = (int) rep.type;
*mszsf = (float) rep.size;
*coli = (int) rep.color;
}

/*$F
 * gqpplr - Inquire Predefined Polyline Representation
 *
 * int	*wtype		- pointer to workstation type 
 * int	*pli		- pointer to predefined polline index
 * int	*errind		- pointer to error indicator
 * int	*ltype		- pointer to line type
 * float *lwidth	- pointer to line size scale factor
 * int	*coli		- pointer to polyline colour index

 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.175 
 */
gqpplr_ (wtype, pli, errind, ltype, lwidth, coli)
int *wtype;
int *pli;
int *errind;
int *ltype;
float *lwidth;
int *coli;

{
Glnbundl rep;
debug ( ("Inquire Predefined Polyline Representation: index = %d type = %d\n", *pli,*wtype-1) );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqpredlinerep); /* c2043 */

if ( *errind = ginqpredlinerep ((Gchar *) wtypes[*wtype-1], (Gint) *pli, &rep)) return;

*ltype = (int) rep.type;
*lwidth = (float) rep.width;
*coli = (int) rep.color;
}

/*$F
 * gqtxf - Inquire Text Facilities
 *
 * int	*wtype		- pointer to workstation type 
 * int	*n		- pointer to list element requested
 * int	*errind		- pointer to error indicator
 * int	*nfpp		- pointer to number of text font and precision pairs 
 * int	*font		- pointer to nth element of list of text fonts
 * int	*prec		- pointer to nth element of list of text precisions 
 *			  (GSTRP, GCHARP, GSTRKP)
 * int	*nchh		- pointer to number of available character heights
 * float *minchh	- pointer to minimum character height (DC)
 * float *maxchh	- pointer to maximum character height (DC)
 * int	*nchx		- pointer to number of available character expansion
 *			  factors
 * float *minchx	- pointer to minimum character expansion factor
 * float *maxchx	- pointer to maximum character expansion factor
 * int	*nptxi		- pointer to number of predefined text indices
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.176 
 */
gqtxf_ (wtype, n, errind, nfpp, font, prec, nchh, minchh, maxchh, nchx, minchx, maxchx, nptxi)
int *wtype;
int *n;
int *errind;
int *nfpp;
int *font;
int *prec;
int *nchh;
float *minchh;
float *maxchh;
int *nchx;
float *minchx;
float *maxchx;
int *nptxi;

{
Gtxfac fac;
debug ( ("Inquire Text Facilities  \n") );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqtextfacil); /* c2043 */

if (*errind = ginqtextfacil ((Gchar *) wtypes[*wtype-1], &fac)) return;

*nfpp = (int) fac.fps;
*nchh = (int)  fac.heights;
*minchh = (float) fac.min_ht;
*maxchh = (float)  fac.max_ht;
*nchx = (int) fac.expansions;
*minchx = (float) fac.min_ex;
*maxchx = (float)  fac.max_ex;
*nptxi = (int)  fac.predefined;

VALIDMEMBER (errind,*n,0,fac.fps,errginqtextfacil); /* c2043 c2020 */
if (*n) {					/* c2020 */
   *font = (int) fac.fp_list[*n-1].font;
   *prec  = (int) fac.fp_list[*n-1].prec;
   }

free(fac.fp_list);
}

/*$F
 * gqpcr - inquire predefined color representation
 *
 * int *wtype   - workstation type 
 * int *pci     - predefined color index
 * int *errind  - error indicator
 * float *red   - % of red
 * float *green - % of green
 * float *blue  - % of blue

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.180 
 */
gqpcr_ (wtype, pci, errind, red, green, blue)
int *wtype;
int *pci;
int *errind;
float *red;
float *green;
float *blue;

{
Gcobundl rep;

debug ( ("Inquire Predefined Color Representation %d  %d \n", *wtype, *pci) );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqpredcolourrep); /* c2043 */

/* wtypes is an array in fortxgks.h that maps a FORTRAN integer wtype
to a C string wtype */

if (*errind = ginqpredcolourrep ( (Gchar *) wtypes[*wtype-1], (Gint) *pci, &rep)) return;

*red	= (float) rep.red;
*green 	= (float) rep.green;
*blue	= (float) rep.blue;

}

/*$F
 * gqlwk - inquire maximum length of workstation state table
 *
 * int *wtype  - workstation type 
 * int *errind - error indicator
 * int *mplbte - polyline
 * int *mpmbte - polymarker
 * int *mtxbte - text
 * int *mfabte - fill area
 * int *mpai   - pattern
 * int *mcoli  - color

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.182
 */
gqlwk_ (wtype, errind, mplbte, mpmbte, mtxbte, mfabte, mpai, mcoli)
int *wtype;
int *errind;
int *mplbte;
int *mpmbte;
int *mtxbte;
int *mfabte;
int *mpai;
int *mcoli;

{
Gwstables tables;

debug ( ("Inquire Maximum Length of Workstation State Tables %d \n", *wtype) );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqmaxwssttables); /* c2043 */

/* wtypes is an array in fortxgks.h that maps a FORTRAN integer wtype
to a C string wtype */

if (*errind = ginqmaxwssttables ( (Gchar *) wtypes[*wtype-1], &tables)) return;

*mplbte	= (int) tables.line;
*mpmbte	= (int) tables.mark;
*mtxbte	= (int) tables.text;
*mfabte	= (int) tables.fill;
*mpai	= (int) tables.pat;
*mcoli	= (int) tables.colour;

}

/*$F
 * gqsgp - inquire number of segment priorities supported
 *
 * int *wtype  - workstation type 
 * int *errind - error indicator
 * int *nsg    - number of segment priorities supported (returned)

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.183
 */
gqsgp_ (wtype, errind, nsg)
int *wtype;
int *errind;
int *nsg;
{

debug ( ("Inquire Number of Segment Priorities Supported %d \n", *wtype) );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqnumsegpri); /* c2043 */

*errind = ginqnumsegpri ( (Gchar *) wtypes[*wtype-1], (Gint *) nsg);

debug ( ("        Number of Segment Priorities Supported %d \n", *nsg) );

}

/*$F
 * gqdsga - inquire dynamic modification of segment attributes
 *
 * int *wtype  - workstation type 
 * int *errind - error indicator
 * int *sgtr   - segment transformation
 * int *vonoff - appearing (turning visible)
 * int *voffon - disappearing (turning invisible)
 * int *high   - highlighting
 * int *sgpr   - segment priority
 * int *add    - addition of primitive to segment
 * int *sgdel  - deletion of segment

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.184
 */
gqdsga_ (wtype, errind, sgtr, vonoff, voffon, high, sgpr, add, sgdel)
int *wtype;
int *errind;
int *sgtr;
int *vonoff;
int *voffon;
int *high;
int *sgpr;
int *add;
int *sgdel;

{
Gmodseg dyn;

debug ( ("Inquire Dynamic Modification of Segment Attributes %d \n", *wtype) );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqmodsegattr); /* c2043 */

/* wtypes is an array in fortxgks.h that maps a FORTRAN integer wtype
to a C string wtype */

if (*errind = ginqmodsegattr ( (Gchar *) wtypes[*wtype-1], &dyn)) return;

*sgtr	= (int) dyn.transform;
*vonoff	= (int) dyn.appear;
*voffon	= (int) dyn.disappear;
*high	= (int) dyn.highlight;
*sgpr	= (int) dyn.priority;
*add	= (int) dyn.addition;
*sgdel	= (int) dyn.deletion;

}

/*$F
 * gqli - inquire number of available logical input devices
 *
 * int *wtype  - workstation type 
 * int *errind - error indicator
 * int *nlcd   - number of locator devices
 * int *nskd   - number of stroke devices
 * int *nvld   - number of valuator devices
 * int *nchd   - number of choice devices
 * int *npcd   - number of pick devices
 * int *nstd   - number of string devices

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.184
 */
gqli_ (wtype, errind, nlcd, nskd, nvld, nchd, npcd, nstd)
int *wtype;
int *errind;
int *nlcd;
int *nskd;
int *nvld;
int *nchd;
int *npcd;
int *nstd;

{
Gnumdev num;

debug ( ("Inquire Number of Availible Logical Input Devices %d \n", *wtype) );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqnumavailinput); /* c2043 */

/* wtypes is an array in fortxgks.h that maps a FORTRAN integer wtype
to a C string wtype */

if (*errind = ginqnumavailinput ( (Gchar *) wtypes[*wtype-1], &num)) return;

*nlcd	= (int) num.locator;
*nskd	= (int) num.stroke;
*nvld	= (int) num.valuator;
*nchd	= (int) num.choice;
*npcd	= (int) num.pick;
*nstd	= (int) num.string;

}

/*$F
 * gqptxr - Inquire Predefined Text Representation
 *
 * int	*wtype		- pointer to workstation type
 * int	*ptxi		- pointer to predefined text index
 * int	*errind		- pointer to error indicator         
 * int	*font		- pointer to text font
 * int	*prec		- pointer to text precision (GSTRP, GCHARP, GSTRKP)
 * float *chxp		- character expansion factor
 * float *chsp		- character spacing
 * int	*coli		- pointer to text colour index
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.177
 */
gqptxr_ (wtype, ptxi, errind, font, prec, chxp, chsp, coli)
int *wtype;
int *ptxi;
int *errind;
int *font;
int *prec;
float *chxp;
float *chsp;
int *coli;
{
Gtxbundl rep;
debug ( ("Inquire Prederined Text Representation \n ") );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqpredtextrep); /* c2043 */

if (*errind = ginqpredtextrep ((Gchar *) wtypes[*wtype-1],(Gint) *ptxi,&rep)) return;

*font = (int) rep.fp.font;
switch (rep.fp.prec)
  {
  case GSTRING :
    *prec = FORT_GSTRP;
    break;
  case GCHAR   :
    *prec = FORT_GCHARP;
    break;
  case GSTROKE :
    *prec = FORT_GSTRKP;
    break;
  }
*chxp = (float)  rep.ch_exp;	/* c1179 */
*chsp = (float) rep.space;
*coli = (int)  rep.color;
}

/*$F
 * gqpfar - Inquire Predefined Fill Area Representation
 *
 * int	*wtype		- pointer to workstation type
 * int	*pfai		- pointer to predefined fill area index
 * int	*errind		- pointer to error indicator         
 * int 	*ints		- pointer to fill area interior style (GHOLLO, GSOLID, 
 *			  GPATTR, GHATCH)
 * int 	*styli		- pointer to fill area style index
 * int 	*coli		- pointer to fill area colour index
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.178
 */
gqpfar_ (wtype, pfai, errind, ints, styli, coli)
int *wtype;
int *pfai;
int *errind;
int *ints;
int *styli;
int *coli;
{
Gflbundl rep;
debug ( ("Inquire Predefined Fill Area Representation \n") );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqpredfillrep); /* c2043 */

if (*errind = ginqpredfillrep (wtypes[*wtype-1],*pfai,&rep)) return;

*ints = (int) rep.inter; 
*styli = (int) rep.style;
*coli = (int) rep.color;
}

/*$F
 * gqppar - Inquire Predefined Pattern Representation
 *
 * int	*wtype		- pointer to workstation type
 * int	*ppai		- pointer to predefined pattern index
 * int	*dimx, *dimy	- pointers to maximum pattern array dimensions
 * int	*errind		- pointer to error indicator         
 * int	*dx, *dy 	- pointer to pattern array dimensions
 * int	*colia[dimx,dimy] - pointer to pattern array
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.179
 */
gqppar_ (wtype, ppai, dimx, dimy, errind, dx, dy, colia)
int *wtype;
int *ppai;
int *dimx;
int *dimy;
int *errind;
int *dx;
int *dy;
int *colia;
{
Gptbundl rep;
debug ( ("Inquire Predefined Pattern Representation \n") );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqpredpatrep); /* c2043 */

if (*errind = ginqpredpatrep ((Gchar *) wtypes[*wtype-1], (Gint) *ppai, &rep)) 
  return; 

*dx = (int) rep.size.x;
*dy = (int) rep.size.y;
if ((*dx > *dimx) || (*dy > *dimy))                     /* c2042 */
{                                                       /* c2042 */
  *errind = 2001;                                       /* c2042 c2043 */
  gerrorhand (2001,errginqpredpatrep,(errfp));		/* c2042 */ /* d1 */
  return;                                               /* c2042 */
}

/* c2044:  This array does NOT need to be transposed. */

  /* c2044:  Take the dx-by-dy subset starting at (1,1). */
  {                                                             /* c2044 */
  int col, row;                                                 /* c2044 */

  for (row=0; row < *dy; row++)                                 /* c2044 */
     for (col=0; col < *dx; col++)                              /* c2044 */
        *(colia + (row * *dimx + col)) = *(rep.array + (row * *dx + col));
                                /* c2044 */
  }                                                             /* c2044 */

/* free memory malloc'ed in ginqpredpatrep in inqfillareas.c by */
/*     rep->array = (Gint *)malloc( j * sizeof(int));           */
free (rep.array);

}

/*$F
 * gqdlc - Inquire Default Locator Device Data
 *
 * int	*wtype		- pointer to workstation type
 * int	*devno		- pointer to logical input device number
 * int	*n		- pointer to list element requested
 * int	*mldr		- pointer to dimension of data record array
 * int	*errind		- pointer to error indicator         
 * float *dpx,*dpy	- pointers to default initial locator position
 * int	*ol		- pointer to number of available prompt/echo types
 * int	*pet		- pointer to nth element of list of available prompt/echo 
 *			  types
 * float *earea[4]	- pointer to default echo area in device coordinates
 *			  XMIN, XMAX, YMIN, YMAX
 * int	*ldr		- pointer to number of array elements used in data record
 * char *datrec		- pointer to data record
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.185
 */
gqdlc_ (wtype, devno, n, mldr, errind, dpx, dpy, ol, pet, earea, ldr, datrec)
int *wtype;
int *devno;
int *n;
int *mldr;
int *errind;
float *dpx;
float *dpy;
int *ol;
int *pet;
float *earea;
int *ldr;
char *datrec;
{
Gdefloc data;
int il;
int ia;
int ra;
int rl;
int sl;
int *lstr;
char *str;
debug ( ("Inquire Default Locator Device Data \n") );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqdefloc); /* c2043 */

if (*errind = ginqdefloc ((Gchar *) wtypes[*wtype-1], (Gint) *devno, &data )) return; /* c2049 */

*dpx = (float) data.position.x;
*dpy = (float) data.position.y;
*ol = (int) data.pets.number;
earea[0] = (float) data.e_area.xmin;
earea[1] = (float) data.e_area.xmax;
earea[2] = (float) data.e_area.ymin;
earea[3] = (float) data.e_area.ymax;
VALIDMEMBER(errind,*n,0,data.pets.number,errginqdefloc); /* c2043 */
if (*n)                                 /* c2020 */
   *pet = (int) data.pets.integers[*n-1];
switch(*pet)
 { 
  case 1: /* default */
          il = 0;                                                    /*c2051*/
          rl = 0;                                                    /*c2051*/
          sl = 0;                                                    /*c2051*/
          ra = NULL;                                                 /*c2051*/
          ia = NULL;                                                 /*c2051*/
          str = NULL;                                                /*c2051*/
          lstr = NULL;                                               /*c2051*/
          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /*c2051*/
          break;
  case 2:
          il = 0;                                                    /*c2051*/
          rl = 0;                                                    /*c2051*/
          sl = 0;                                                    /*c2051*/
          ra = NULL;                                                 /*c2051*/
          ia = NULL;                                                 /*c2051*/
          str = NULL;                                                /*c2051*/
          lstr = NULL;                                               /*c2051*/
          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /*c2051*/
          break;
  case 3:
          il = 0;                                                    /*c2051*/
          rl = 0;                                                    /*c2051*/
          sl = 0;                                                    /*c2051*/
          ra = NULL;                                                 /*c2051*/
          ia = NULL;                                                 /*c2051*/
          str = NULL;                                                /*c2051*/
          lstr = NULL;                                               /*c2051*/
          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /*c2051*/
          break;
  case 4:
          il = 0;                                                    /*c2051*/
          rl = 0;                                                    /*c2051*/
          sl = 0;                                                    /*c2051*/
          ra = NULL;                                                 /*c2051*/
          ia = NULL;                                                 /*c2051*/
          str = NULL;                                                /*c2051*/
          lstr = NULL;                                               /*c2051*/
          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /*c2051*/
          break;
  case 5:
          il = 0;                                                    /*c2051*/
          rl = 0;                                                    /*c2051*/
          sl = 0;                                                    /*c2051*/
          ra = NULL;                                                 /*c2051*/
          ia = NULL;                                                 /*c2051*/
          str = NULL;                                                /*c2051*/
          lstr = NULL;                                               /*c2051*/
          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /*c2051*/
          break;
  case 6:
          il = 0;                                                    /*c2051*/
          rl = 0;                                                    /*c2051*/
          sl = 0;                                                    /*c2051*/
          ra = NULL;                                                 /*c2051*/
          ia = NULL;                                                 /*c2051*/
          str = NULL;                                                /*c2051*/
          lstr = NULL;                                               /*c2051*/
          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /*c2051*/
 }
}

/*$F
 * gqdsk - Inquire Default Stroke Device Data
 *
 * int	*wtype		- pointer to workstation type
 * int	*devno		- pointer to logical input device number
 * int	*n		- pointer to list element requested
 * int	*mldr		- pointer to dimension of data record array
 * int	*errind		- pointer to error indicator         
 * int	*dbufsk		- pointer to maximum input buffer size
 * int	*ol		- pointer to number of available prompt/echo types
 * int	*pet		- pointer to nth element of list of available prompt/echo 
 *			  types
 * float *earea[4]	- pointer to default echo area in device coordinates
 *			  XMIN, XMAX, YMIN, YMAX
 * int	*buflen		- pointer to buffer length for stroke
 * int	*ldr		- pointer to number of array elements used in data record
 * char *datrec		- pointer to data record
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.186
 */
gqdsk_ (wtype, devno, n, mldr, errind, dbufsk, ol, pet, earea, buflen, ldr, datrec)
int *wtype;
int *devno;
int *n;
int *mldr;
int *errind;
int *dbufsk;
int *ol;
int *pet;
float *earea;
int *buflen;
int *ldr;
char *datrec;
{
Gdefstroke data;
Gint il;
Gint rl;
Gint sl;
Gint *ia;
Gfloat *ra;
Gchar **str;
Gint *lstr;

debug ( ("Inquire Default Stroke Device Data \n") );   /* c2043 */
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqdefstroke); /* c2043 */

if (*errind = ginqdefstroke ((Gchar*) wtypes[*wtype-1], (Gint) *devno, &data)) return;

*dbufsk = (int) data.bufsiz;
*ol = (int) data.pets.number;
earea[0] = (float) data.e_area.xmin;
earea[1] = (float) data.e_area.xmax;
earea[2] = (float) data.e_area.ymin;
earea[3] = (float) data.e_area.ymax;
VALIDMEMBER (errind,*n,0,data.pets.number,errginqdefstroke); /* c2043 c2051*/
if (*n)                                                      /* c2020 c2051*/
   *pet = (int) data.pets.integers[*n-1];                    /* c2051 */
switch(*pet)
 { 
  case 1:  /* default pet */
          il = 2;
          rl = 3;
          sl = 0;
          *ldr = il + rl + sl;
          ia = (Gint *) malloc(il*sizeof(Gint *));
          ra = (Gfloat *) malloc(rl*sizeof(Gfloat *));
          str = NULL;
          lstr = NULL;
          ia[0] = data.record.pet1.bufsiz;
          ia[1] = data.record.pet1.editpos;
          ra[0] = data.record.pet1.interval.x;
          ra[1] = data.record.pet1.interval.y;
          ra[2] = data.record.pet1.time;

          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /*c2038 2049 */
          free(ia);
          free(ra);
          *buflen = (int) data.record.pet1.bufsiz;                  /*c2051*/
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
          *buflen = (int) data.record.pet2.bufsiz;
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
          *buflen = (int) data.record.pet3.bufsiz;
          break;
  case 4: 
          il = 0;                                                    /*c2051*/
          rl = 0;						     /*c2051*/
          sl = 0;					 	     /*c2051*/
	  ra = NULL;					             /*c2051*/
	  ia = NULL;					             /*c2051*/
	  str = NULL;					             /*c2051*/
	  lstr = NULL;					             /*c2051*/
          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /*c2051*/
          *buflen = (int) data.record.pet4.bufsiz;
 }
}

/*$F
 * gqdvl - Inquire Default Valuator Device Data
 *
 * int	*wtype		- pointer to workstation type
 * int	*devno		- pointer to logical input device number
 * int	*n		- pointer to list element requested
 * int	*mldr		- pointer to dimension of data record array
 * int	*errind		- pointer to error indicator         
 * float *dval		- pointer to default initial value
 * int	*ol		- pointer to number of available prompt/echo types
 * int	*pet		- pointer to nth element of list of available prompt/echo 
 *			  types
 * float *earea[4]	- pointer to default echo area in device coordinates
 *			  XMIN, XMAX, YMIN, YMAX
 * float *loval, *hival - pointer to minimal and maximal values
 * int	*ldr		- pointer to number of array elements used in data record
 * char *datrec		- pointer to data record
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.186
 */
gqdvl_ (wtype,devno,n, mldr, errind, dval, ol, pet, earea, loval, hival, ldr, datrec)
int *wtype;
int *devno;
int *n;
int *mldr;
int *errind;
float *dval;
int *ol;
int *pet;
float *earea;
float *loval, *hival;
int *ldr;
char *datrec;
{
Gdefval data;
Gint il;
Gint rl;
Gint sl;
Gint *ia;
Gfloat *ra;
Gchar **str;
Gint *lstr;

debug ( ("Inquire Default Valuator Device Data \n ") );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqdefval); /* c2043 */

if (*errind = ginqdefval ((Gchar *) wtypes[*wtype-1], (Gint) *devno, &data)) return;

*dval = (float) data.value;
*ol = (int) data.pets.number;
earea[0] = (float) data.e_area.xmin;
earea[1] = (float) data.e_area.xmax;
earea[2] = (float) data.e_area.ymin;
earea[3] = (float) data.e_area.ymax;
VALIDMEMBER (errind,*n,0,data.pets.number,errginqdefval); /* c2043 */
if (*n)					/* c2020 */
   *pet = (int) data.pets.integers[*n-1];
switch (*pet)
 { 
  case 1: /* default device */
          il = 0;
          rl = 2;
          sl = 0;
          ia = NULL;
          ra = (Gfloat *) malloc(rl*sizeof(Gfloat *));
          str = NULL;
          lstr = NULL;
          *ldr = il + rl + sl;
          ra[0] = data.record.pet1.low;
          ra[1] = data.record.pet1.high;
          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec);/*c2038 2049 */
          free(ra);

          *loval = (float) data.record.pet1.low;
          *hival = (float) data.record.pet1.high;
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
          *loval = (float) data.record.pet2.low;
          *hival = (float) data.record.pet2.high;
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
          *loval = (float) data.record.pet3.low;
          *hival = (float) data.record.pet3.high;
          break;
 }

}

/*$F
 * gqdch - Inquire Default Choice Device Data
 *
 * int	*wtype 		- pointer to workstation type
 * int	*devno		- pointer to logical input device number
 * int	*n		- pointer to list element requested
 * int	*mldr		- pointer to dimension of data record array
 * int	*errind		- pointer to error indicator         
 * int	*malt		- pointer to maximum number of alternatives
 * int	*ol		- pointer to number of available prompt/echo types
 * int	*pet		- pointer to nth element of list of available prompt/echo 
 *			  types
 * float *earea[4]	- pointer to default echo area in device coordinates
 *			  XMIN, XMAX, YMIN, YMAX
 * int	*ldr		- pointer to number of array elements used in data record
 * char *datrec		- pointer to data record
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.187
 */
gqdch_ (wtype, devno, n, mldr, errind, malt, ol, pet, earea, ldr, datrec)
int *wtype;
int *devno;
int *n;
int *mldr;
int *errind;
int *malt;
int *ol;
int *pet;
float *earea;
int *ldr;
char *datrec;
{
Gdefchoice data;
Gint il;
Gint rl;
Gint sl;
Gint *ia;
Gfloat *ra;
Gchar **str;
Gint *lstr;
int i;

debug ( ("Inquire Default Choice Device Data \n  ") );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqdefchoice); /* c2043 */
if (*errind = ginqdefchoice ((Gchar *) wtypes[*wtype-1], (Gint) *devno, &data)) return;

*malt = (int)  data.choices;
*ol = (int) data.pets.number;
earea[0] = (float) data.e_area.xmin;
earea[1] = (float) data.e_area.xmax;
earea[2] = (float) data.e_area.ymin;
earea[3] = (float) data.e_area.ymax;
VALIDMEMBER (errind,*n,0,data.pets.number,errginqdefchoice); /* c2043 */
if (*n)					/* c2020 */
   *pet = (int) data.pets.integers[*n-1];
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
  case 3:  /* Default pet */
          il = 1;
          rl = 0;
          sl = data.record.pet3.number;
          *ldr = il + rl + sl;
          ia = (Gint *) malloc(il * sizeof(Gint *));
          ra = NULL;
          ia[0] = data.record.pet3.number;
          lstr = (Gint *) malloc(sl * sizeof(Gint));
          for (i = 0; i < sl; i++)
            lstr[i] = strlen(data.record.pet3.strings[i]);
          str = (Gchar **) malloc(sl * sizeof(Gchar *));
          for (i = 0; i < sl; i++)
            str[i] = data.record.pet3.strings[i];

          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec);/*c2038 2049 */
          free(ia);
          free(str);
          free(lstr);
          break;
  case 4: 
          il = 0;                                                    /*c2051*/
          rl = 0;						     /*c2051*/
          sl = 0;					 	     /*c2051*/
	  ra = NULL;					             /*c2051*/
	  ia = NULL;					             /*c2051*/
	  str = NULL;					             /*c2051*/
	  lstr = NULL;					             /*c2051*/
          gprec_(&il,ia,&rl,ra,&sl,lstr,str,mldr,errind,ldr,datrec); /*c2051*/
          break;
  case 5: 
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
 * gqdpk - Inquire Default Pick Device Data
 *
 * int	*wtype		- pointer to workstation type
 * int	*devno		- pointer to logical input device number
 * int	*n		- pointer to list element requested
 * int	*mldr		- pointer to dimension of data record array
 * int	*errind		- pointer to error indicator         
 * int	*ol		- pointer to number of available prompt/echo types
 * int	*pet		- pointer to nth element of list of available prompt/echo 
 *			  types
 * float *earea[4]	- pointer to default echo area in device coordinates
 *			  XMIN, XMAX, YMIN, YMAX
 * int	*ldr		- pointer to number of array elements used in data record
 * char	*datrec		- pointer to data record
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.188
 */
gqdpk_ (wtype, devno, n, mldr, errind, ol, pet, earea, ldr, datrec)
int *wtype;
int *devno;
int *n;
int *mldr;
int *errind;
int *ol;
int *pet;
float *earea;
int *ldr;
char *datrec;
{
Gdefpick data;
int il;
int ia;
int ra;
int rl;
int sl;
int *lstr;
char *str;
debug ( ("Inquire Default Pick Device Data") );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqdefpick); /* c2043 */

if (*errind = ginqdefpick ((Gchar *) wtypes[*wtype-1], (Gint) *devno, &data)) return; /* c2049 */

*ol = (int) data.pets.number;
earea[0] = (float) data.e_area.xmin;
earea[1] = (float) data.e_area.xmax;
earea[2] = (float) data.e_area.ymin;
earea[3] = (float) data.e_area.ymax;
VALIDMEMBER (errind,*n,0,data.pets.number,errginqdefpick); /* c2043 */
if (*n)					/* c2020 */
   *pet = (int) data.pets.integers[*n-1];
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
 * gqdst - Inquire Default String Device Data
 *
 * int	*wtype		- pointer to workstation type
 * int	*devno		- pointer to logical input device number
 * int	*n 		- pointer to list element requested
 * int	*mldr		- pointer to dimension of data record array
 * int	*errind		- pointer to error indicator
 * int	*mbuff		- pointer to maximum string buffer size         
 * int	*ol		- pointer to number of available prompt/echo types
 * int	*pet		- pointer to nth element of list of available prompt/echo 
 *			  types
 * float *earea[4]	- pointer to default echo area in device coordinates
 *			  XMIN, XMAX, YMIN, YMAX
 * int	*buflen		- pointer to buffer length of string
 * int	*ldr		- pointer to number of array elements used in data record
 * char	*datrec		- pointer to data record
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.188
 */
gqdst_ (wtype, devno, n, mldr, errind, mbuff, ol, pet, earea, buflen, ldr, datrec)
int *wtype;
int *devno;
int *n;
int *mldr;
int *errind;
int *mbuff;
int *ol;
int *pet;
float *earea;
int *buflen;
int *ldr;
char *datrec;
{
Gdefstring data;
Gint il;
Gint rl;
Gint sl;
Gint *ia;
Gfloat *ra;
Gchar **str;
Gint *lstr;

debug ( ("Inquire Default String Device Data \n ") );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqdefstring); /* c2043 */
if (*errind = ginqdefstring ((Gchar *) wtypes[*wtype-1], (Gint) *devno, &data)) return; /* c2047 */

*mbuff = (int) data.bufsiz;
*ol = (int) data.pets.number;
earea[0] = (float) data.e_area.xmin;
earea[1] = (float) data.e_area.xmax;
earea[2] = (float) data.e_area.ymin;
earea[3] = (float) data.e_area.ymax;
VALIDMEMBER (errind,*n,0,data.pets.number,errginqdefstring); /* c2043 */
if (*n)					/* c2020 */
   *pet = (int) data.pets.integers[*n-1];
switch (*pet)
 { 
  case 1: 
          il = 2;		/* c2047 */
          rl = 0;
          sl = 0;
          lstr = NULL;
          ra = NULL;
          str = NULL;
          *ldr = il + rl + sl;
          ia = (Gint *) malloc(il*sizeof(Gint *));
          ia[0] = data.record.pet1.bufsiz;
          ia[1] = data.record.pet1.position;
          gprec_(&il,ia,&rl,ra,&sl,lstr,*str,mldr,errind,ldr,datrec);/* c2049 */
				/* ^^ c2038 c2047 ^^ */
          free(ia);
          *buflen = (int) data.record.pet1.bufsiz;
          break;
  }
}

/*$F
 * gqdds - Inquire Default Deferral State Values
 *
 * int	*wtype		- pointer to workstation type
 * int	*errind		- pointer to error indicator
 * int	*defmod		- pointer to default deferral mode (GASAP, GBNIG, GBNIL
 *			  GASTI)         
 * int	*regmod		- pointer to default implicit regeneration mode (GSUPPD,
 *			  GALLOW)
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.173
 */
gqdds_ (wtype, errind, defmod, regmod)
int *wtype;
int *errind;
int *defmod;
int *regmod;
{
Gdefer def;
debug ( ("Inquire Default Deferral State Values \n") );
VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqdefdeferst); /* c2043 */
if (*errind = ginqdefdeferst ((Gchar *) wtypes[*wtype-1], &def )) return;

*defmod = (int) def.defmode;
switch (def.irgmode)
  {
  case GSUPPRESSED:
    *regmod = FORT_GSUPPD;
    break;
  case GALLOWED:
    *regmod = FORT_GALLOW;
    break;
  }

}

/*$F
 * gqegdp - Inquire List Element of Available Generalized Drawing Primitives
 *
 * int	*wtype		- pointer to workstation type
 * int  *n		- list element requested
 * int	*errind		- pointer to error indicator
 * int	*ngdp		- number of available GDPs         
 * int	*gdpl		- nth element of list of GDP identifiers
 *
 * Returns: error 2002 as well as ANSI standard errors for this function.
 *
 * See also: ANSI standard p.181
 */

gqegdp_ (wtype, n, errind, ngdp, gdpl)
  int *wtype;
  int  *n;
  int *errind;
  int *ngdp;
  int *gdpl;
{
Ggdplist gdps;

debug ( ("Inquire List Element of Available Generalized Drawing Primitives\n") );

VALIDTYPE(errind,*wtype,1,NUMWTYPES,errginqavailgdp); /* c2043 */
if (*errind = ginqavailgdp((Gchar *) wtypes[*wtype-1], &gdps)) return;

/* set return values not dependent on range check  c2020 */
*ngdp = gdps.number;

/* is the element they are asking for in the list?... */
VALIDMEMBER(errind,*n,0,gdps.number,errginqavailgdp);  /* c2043 c2020 */
if (*n)				/* c2020 */
   *gdpl = (int)gdps.indices[*n-1];

/* free memory allocated by the C call */
free (gdps.functions);
free (gdps.indices);
}

/*$F
 * gqgdp - Inquire Generalized Drawing Primitive
 *
 * int	*wtype		- pointer to workstation type
 * int  *gdp		- GDP identifier
 * int	*errind		- pointer to error indicator
 * int	*nbnd		- number of sets of attributes used         
 * int	*bndl		- list of sets of attributes used (GPLBND, GPMBND, 
 *			  GTXBND, GFABND)
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.182
 */
gqgdp_ (wtype, gdp, errind, nbnd, bndl)
int *wtype;
int  *gdp;
int *errind;
int *nbnd;
int *bndl;
{

}
