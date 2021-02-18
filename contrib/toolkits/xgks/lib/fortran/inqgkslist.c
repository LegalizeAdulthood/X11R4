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
 * GKS inquire GKS state list functions:
 *	gqopwk_
 * 	gqacwk_
 *	gqpli_
 *	gqpmi_
 *	gqtxi_
 *	gqchh_
 *	gqchup_
 *	gqchw_
 *	gqchb_
 *	gqtxp_
 *	gqtxal_
 *	gqfai_
 *	gqpa_
 *	gqparf_
 *	gqpkid_
 *  	gqln_
 *	gqlwsc_
 *	gqplci_
 *	gqmk_
 *	gqmksc_
 *	gqpmci_
 *	gqtxfp_
 *	gqchxp_
 *	gqchsp_
 *	gqtxci_
 *	gqfais_
 *	gqfasi_
 *	gqfaci_
 *	gqasf_
 *	gqcntn_
 *	gqentn_
 *	gqnt_
 *	gqclip_
 *	gqopsg_
 *	gqsgus_
 *	gqsim_
 *  
 *        
 * David Berkowitz
 * Bruce Haimowitz
 * TCS Development
 * Cambridge MA
 *
 * September 29, 2988
 *
 * $Header: inqgkslist.c,v 4.0 89/08/31 18:32:42 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/inqgkslist.c,v $
 * 
 * $Log:	inqgkslist.c,v $
 * Revision 4.0  89/08/31  18:32:42  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.16  89/06/26  15:35:16  amy
 *  DCR d1	Declare errfp external for use in error macros.
 * 
 * Revision 1.15  89/03/15  16:13:01  bruce
 * PTR c2020	More stuff for 2020.
 * 
 * Revision 1.14  89/03/15  15:02:35  amy
 * PTR c2043	All calls to VALIDMEMBER:  added value for errind 
 * 		parameter.
 * PTR c2020	Inq. List Element and Inq. Set Member functions:
 * 		return values of output parameters regardless
 * 		of validity of the n parameter, and accept 0 as valid value
 * 		for n.
 * 
 * Revision 1.13  89/01/06  08:22:10  bruce
 * Changed some of the InquireSetMember functions so they will accept a request for
 * member number 0 but in such a case will only return the number of members in the
 * set.  PTR# c2020.
 * 
 * Revision 1.12  88/12/29  17:08:42  todd
 * fixed ptr# c2015
 * Inquire Ws Connection and type need to check if the
 * wtype was equal to the conid. If so then return the 
 * NULL index.
 * 
 * Revision 1.11  88/12/05  15:19:26  owens
 * changed errfp to errfpp
 * 
 * Revision 1.9  88/12/05  14:25:49  bhaim
 * Changes for Code II
 * 
 * Revision 1.8  88/11/16  10:18:46  todd
 * Removed extra call to gerrorhand.
 * 
 * Revision 1.7  88/11/14  07:50:49  bhaim
 * Error handling
 * 
 * Revision 1.6  88/10/11  17:03:00  todd
 * No changes.
 * 
 * Revision 1.5  88/10/11  14:33:49  owens
 * more changes from code review
 * 
 * Revision 1.4  88/09/30  11:28:28  owens
 * more code review changes
 * 
 * Revision 1.3  88/09/29  16:51:37  owens
 * more code review changes
 * 
 * Revision 1.2  88/09/29  16:27:30  owens
 * code review changes
 * 
 * Revision 1.1  88/09/27  08:17:10  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: inqgkslist.c,v 4.0 89/08/31 18:32:42 amy Exp $";

#include <xgks.h>
#include "fortxgks.h"

/* errfp is needed for error macros d1 */
extern FILE *errfp;              /* d1 */

/*$F
 * gqopwk - inquire set member of open workstations
 *
 * int *n            set member requested   
 * int *errind       error indicator       
 * int *ol           number of open workstations 
 * int *wkid         Nth member of set of open workstations 
 *
 * Returns: 2002 in addition to ANSI standard errors for this function
 *
 * See also: ANSI standard p. 147
 */
gqopwk_ (n,errind,ol,wkid)   
int *n;        
int *errind;  
int *ol;     
int *wkid;  
{
Gintlist wsids;
debug(("inquire set member of open workstations \n"));
if (*errind = ginqopenws(&wsids)) return;
/* set return values not dependent on range check  c2020 */
*ol = (int)wsids.number;

VALIDMEMBER(errind,*n,0,wsids.number,errginqopenws);	/* c2043 c2020 */
if (*n)                                 /* c2020 */
  *wkid = (int)wsids.integers[(*n)-1];
free(wsids.integers);
}

/*$F
 * gqacwk - inquire set member of active workstations
 *
 * int *n            set member requested   
 * int *errind       error indicator        
 * int *ol           number of open workstations 
 * int *wkid         Nth member of set of open workstations 
 *
 * Returns: 2002 in addition to ANSI standard errors for this function
 *
 * See also: ANSI standard p. 147
 */
gqacwk_ (n,errind,ol,wkid)   
int *n;         
int *errind;   
int *ol;      
int *wkid;   
{
Gintlist wsids;
debug(("Inquire Set Member of Active Workstations %n \n",*n));
if (*errind = ginqactivews(&wsids)) return;

/* set return values not dependent on range check  c2020 */
*ol = (int)wsids.number;

VALIDMEMBER(errind,*n,0,wsids.number,errginqactivews);	/* c2043 c2020 */

if (*n)
  *wkid = (int)wsids.integers[(*n)-1];
free(wsids.integers);
}

/*$F
 * gqpli - inquire list member of polyline indices
 *
 * int *errind;      error indicator   
 * int *pli;         polyline index    
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 148
 */
gqpli_ (errind,pli)   
int *errind;     
int *pli;       
{
Gpriattr primattr;
debug(("inquire list member of polyline indices \n"));
if (*errind = ginqprimattr(&primattr)) return;

*pli = (int)primattr.plnindex;
}

/*$F
 * gqpmi - inquire list member of polymarker indices
 *
 * int *errind;      error indicator   
 * int *pmi;         polymarker index    
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 148
 */
gqpmi_ (errind,pmi)   
int *errind;     
int *pmi;       
{
Gpriattr primattr;
debug(("inquire list member of polymarker indices \n"));
if (*errind = ginqprimattr(&primattr)) return;

*pmi = (int)primattr.pmkindex;
}

/*$F
 * gqtxi - inquire list member of text indices
 *
 * int *errind;      error indicator   
 * int *txi;         text     index    
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 148
 */
gqtxi_ (errind,txi)   
int *errind;   
int *txi;     
{
Gpriattr primattr;
debug(("inquire list member of text indices \n"));
if (*errind = ginqprimattr(&primattr)) return;

*txi = (int)primattr.txindex;
}

/*$F
 * gqchh - inquire character height 
 *
 * int *errind;      error indicator   
 * float *chh;       character height    
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 148
 */
gqchh_ (errind,chh)   
int *errind;   
float *chh;   
{
Gpriattr primattr;
debug(("inquire character height \n"));
if (*errind = ginqprimattr(&primattr)) return;

*chh = (float)primattr.height;
}

/*$F
 * gqchup - inquire character up vector
 *
 * int *errind;      error indicator   
 * float  *chux;     character up X    
 * float  *chuy;     character up Y    
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 148
 */
gqchup_ (errind,chux,chuy)   
int *errind;  
float  *chux;
float  *chuy;
{
Gpriattr primattr;
debug(("inquire character up vector \n"));
if (*errind = ginqprimattr(&primattr)) return;

*chux = (float)primattr.up.x;
*chuy = (float)primattr.up.y;
}

/*$F
 * gqchw - inquire character width
 *
 * int   *errind;      error indicator   
 * float *chw;         character width   
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 148
 */
gqchw_ (errind,chw)   
int   *errind;     
float *chw;       
{
Gpriattr primattr;
debug(("inquire character width \n"));
if (*errind = ginqprimattr(&primattr)) return;

*chw = (float)primattr.chwidth;
}

/*$F
 * gqchb - 
 *
 * int *errind;      error indicator   
 * float *chbx;      character base X  
 * float *chby;      character base Y  
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 148
 */
gqchb_ (errind,chbx,chby)   
int *errind; 
float *chbx;
float *chby;
{
Gpriattr primattr;
debug(("inquire character base vector \n"));
if (*errind = ginqprimattr(&primattr)) return;

*chbx = (float)primattr.base.x;
*chby = (float)primattr.base.y;
}

/*$F
 * gqtxp - inquire text path
 *
 * int *errind;      error indicator   
 * int *txp;         text path         
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 148
 */
gqtxp_ (errind,txp)   
int *errind;    
int *txp;      
{
Gpriattr primattr;
debug(("inquire text path \n"));
if (*errind = ginqprimattr(&primattr)) return;

*txp = (int)primattr.path;
}

/*$F
 * gqtxal - inquire text alignment
 *
 * int *errind;      error indicator   
 * int *txalh;       text alignment horizontal
 * int *txalv;       text alignment vertical  
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 148
 */
gqtxal_ (errind,txalh,txalv)   
int *errind;    
int *txalh;    
int *txalv;   
{
Gpriattr primattr;
debug(("inquire text alignment \n"));
if (*errind = ginqprimattr(&primattr)) return;

*txalh = (int)primattr.align.hor;
*txalv = (int)primattr.align.ver;
}

/*$F
 * gqfai - inquire fill area index
 *
 * int *errind;      error indicator   
 * int *fai;         fill area index    
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 148
 */
gqfai_ (errind,fai)   
int *errind;   
int *fai;     
{
Gpriattr primattr;
debug(("inquire fill area index \n"));
if (*errind = ginqprimattr(&primattr)) return;

*fai = (int)primattr.flindex;
}

/*$F
 * gqpa - inquire pattern size
 *
 * int *errind;      error indicator   
 * float *pwx,*pwy;  pattern width vector 
 * float *phx,*phy;  pattern width vector 
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 148
 */
gqpa_ (errind,pwx,pwy,phx,phy)   
int *errind;     
float *pwx,*pwy;
float *phx,*phy;
{
Gpriattr primattr;
debug(("inquire pattern size \n"));
if (*errind = ginqprimattr(&primattr)) return;

*pwx = (float)primattr.widthvec.x;
*pwy = (float)primattr.widthvec.y;
*phx = (float)primattr.heightvec.x;
*phy = (float)primattr.heightvec.y;
}

/*$F
 * gqparf - inquire pattern reference point
 *
 * int *errind;      error indicator   
 * float *rfx;       pattern reference point 
 * float *rfy;       pattern reference point 
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 148
 *
 */
gqparf_ (errind,rfx,rfy)   
int *errind;    
float *rfx;    
float *rfy;   
{
Gpriattr primattr;
debug(("inquire pattern reference point \n"));
if (*errind = ginqprimattr(&primattr)) return;

*rfx = (float)primattr.prp.x;
*rfy = (float)primattr.prp.y;
}

/*$F
 * gqpkid - inquire pick identifier
 *
 * int *errind;      error indicator   
 * int *pkid;        pick identifier   
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 148
 */
gqpkid_ (errind,pkid)   
int *errind;    
int *pkid;       
{
debug(("inquire pick identifier \n"));
*errind = ginqcurpickid((Gint *)pkid);

}

/*$F
 * gqln - inquire linetype
 *
 * int *errind;      error indicator   
 * int *ltype;       linetype          
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 149
 */
gqln_ (errind,ltype)   
int *errind;   
int *ltype;   
{
Gindattr indivattr;
debug(("inquire linetype \n"));
if (*errind = ginqindivattr(&indivattr)) return;

*ltype = (int)indivattr.lntype;
}

/*$F
 * gqlwsc - inquire linewidth scale factor
 *
 * int *errind;      error indicator   
 * float *lwidth;    linewidth         
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 149
 */
gqlwsc_ (errind,lwidth)   
int *errind;     
float *lwidth;  
{
Gindattr indivattr;
debug(("inquire linewidth scale factor \n"));
if (*errind = ginqindivattr(&indivattr)) return;

*lwidth = (float)indivattr.lnwidth;
}

/*$F
 * gqplci - inquire polyline color index
 *
 * int *errind;      error indicator   
 * int *coli;        polyline colour index 
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 149
 */
gqplci_ (errind,coli)   
int *errind;      /* error indicator   */
int *coli;        /* polyline colour index */
{
Gindattr indivattr;
debug(("inquire polyline color index \n"));
if (*errind = ginqindivattr(&indivattr)) return;

*coli = (int)indivattr.lncolour;
}

/*$F
 * gqmk - inquire markertype
 *
 * int *errind;      error indicator   
 * int *mtype;       marker type       
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 149
 */
gqmk_ (errind,mtype)   
int *errind;      /* error indicator   */
int *mtype;       /* marker type       */
{
Gindattr indivattr;
debug(("inquire markertype \n"));
if (*errind = ginqindivattr(&indivattr)) return;

*mtype = (int)indivattr.mktype;
}

/*$F
 * gqmksc - inquire marker size scale factor
 *
 * int   *errind;    error indicator   
 * float *mszsf;     marker size scale factor 
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 149
 */
gqmksc_ (errind,mszsf)   
int   *errind;
float *mszsf;
{
Gindattr indivattr;
debug(("inquire marker size scale factor \n"));
if (*errind = ginqindivattr(&indivattr)) return;

*mszsf = (float)indivattr.mksize;
}

/*$F
 * gqpmci - inquire polymarker color index
 *
 * int *errind;      error indicator   
 * int *coli;        polymarker colour index 
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 149
 */
gqpmci_ (errind,coli)   
int *errind;      /* error indicator   */
int *coli;        /* polymarker colour index */
{
Gindattr indivattr;
debug(("inquire polymarker color index \n"));
if (*errind = ginqindivattr(&indivattr)) return;

*coli = (int)indivattr.mkcolour;
}

/*$F
 * gqtxfp - inquire text font and precision
 *
 * int *errind;      error indicator   
 * int *font;        text font         
 * int *prec;        text precision    
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 149
 */
gqtxfp_ (errind,font,prec)   
int *errind;   
int *font;      
int *prec;       
{
Gindattr indivattr;
debug(("inquire text font and precision \n"));
if (*errind =  ginqindivattr(&indivattr)) return;

*font = (int)indivattr.fp.font;
*prec = (int)indivattr.fp.prec;
}

/*$F
 * gqchxp - inquire character expansion factor
 *
 * int   *errind;      error indicator   
 * float *chxp;        character expansion factor 
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 149
 */
gqchxp_ (errind,chxp)   
int   *errind;      /* error indicator   */
float *chxp;        /* character expansion factor */
{
Gindattr indivattr;
debug(("inquire character expansion factor \n"));
if (*errind = ginqindivattr(&indivattr)) return;

*chxp = (float)indivattr.chexp;
}

/*$F
 * gqchsp - inquire character spacing
 *
 * int *errind;      error indicator   
 * float *chsp;      character spacing 
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 149
 */
gqchsp_ (errind,chsp)   
int *errind;      /* error indicator   */
float *chsp;      /* character spacing */
{
Gindattr indivattr;
debug(("inquire character spacing \n"));
if (*errind = ginqindivattr(&indivattr)) return;
*chsp = indivattr.chspace;
}

/*$F
 * gqtxci - inquire text color index
 *
 * int *errind;      error indicator   
 * int *coli;        text colour index 
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 149
 */
gqtxci_ (errind,coli)   
int *errind;      /* error indicator   */
int *coli;        /* text colour index */
{
Gindattr indivattr;
debug(("inquire text color index \n"));
if (*errind = ginqindivattr(&indivattr)) return;

*coli = (int)indivattr.txcolour;
}

/*$F
 * gqfais - inquire fill area interior style
 *
 * int *errind;      error indicator          
 * int *ints;        fill area interior style 
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 149
 */
gqfais_ (errind,ints)   
int *errind;      /* error indicator          */
int *ints;        /* fill area interior style */
{
Gindattr indivattr;
debug(("inquire fill area interior style \n"));
if (*errind = ginqindivattr(&indivattr)) return;
*ints = (int)indivattr.flstyle;
}

/*$F
 * gqfasi - inquire fill area style index
 *
 * int *errind;      error indicator  
 * int *styli;       fill area style index 
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 149
 */
gqfasi_ (errind,styli)   
int *errind;      /* error indicator   */
int *styli;       /* fill area style index */
{
Gindattr indivattr;
debug(("inquire fill area style index \n"));
if (*errind = ginqindivattr(&indivattr)) return;
*styli = (int)indivattr.flindex;
}

/*$F
 * gqfaci - inquire fill area color index
 *
 * int *errind;      error indicator   
 * int *coli;        fill area colour index 
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 149
 */

gqfaci_ (errind,coli)   
int *errind;      /* error indicator   */
int *coli;        /* fill area colour index */
{
Gindattr indivattr;
debug(("inquire fill area color index"));
if (*errind = ginqindivattr(&indivattr)) return;
*coli= (int)indivattr.flcolour;
}

/*$F
 * gqasf - inquire aspect source flags
 *
 * int *errind;      error indicator   
 * int *lasf;        list of aspect source flags 
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 149
 */
gqasf_ (errind,lasf)   
int *errind;    
int *lasf;       
{
Gindattr indivattr;
debug(("inquire aspect source flags \n"));
if (*errind = ginqindivattr(&indivattr)) return;

lasf[0] = (int)indivattr.asflist.ln_type;
lasf[1] = (int)indivattr.asflist.ln_width;
lasf[2] = (int)indivattr.asflist.ln_colour;
lasf[3] = (int)indivattr.asflist.mk_type;
lasf[4] = (int)indivattr.asflist.mk_size;
lasf[5] = (int)indivattr.asflist.mk_colour;
lasf[6] = (int)indivattr.asflist.tx_fp;
lasf[7] = (int)indivattr.asflist.tx_exp;
lasf[8] = (int)indivattr.asflist.tx_space;
lasf[9] = (int)indivattr.asflist.tx_colour;
lasf[10] = (int)indivattr.asflist.fl_inter;
lasf[11] = (int)indivattr.asflist.fl_style;
lasf[12] = (int)indivattr.asflist.fl_colour;
}

/*$F
 * gqcntn - inquire current normalization transformation number
 *
 * int *errind;      error indicator   
 * int *ctnr;        current transformation number 
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 149
 */
gqcntn_ (errind,ctnr)   
int *errind;     
int *ctnr; 
{
debug(("inquire current normalization transformation number \n"));
*errind = ginqcurntrannum((Gint *)ctnr);
}

/*$F
 * gqentn - inquire list element of normalization transformation 
 *
 * int *n;           set member requested   
 * int *errind;      error indicator        
 * int *ol;          number of open workstations 
 * int *tnr;         Nth member of transformation list
 *
 * Returns: 2002 in addition to ANSI standard errors for this function
 *
 * See also: ANSI standard p. 150
 */
gqentn_ (n,errind,ol,tnr)   
int *n;        
int *errind;  
int *ol;     
int *tnr;   
{
Gintlist tranlist;
debug(("inquire list element of normalization transformation %d \n",*n)); 
if (*errind = ginqntrannum(&tranlist)) return;

/* set return values not dependent on range check  c2020 */
*ol = tranlist.number;		/* c2020 */

VALIDMEMBER(errind,*n,0,tranlist.number,errginqntrannum);  /* c2043 c2020 */
if (*n)				/* c2020 */
  *tnr = tranlist.integers[(*n)-1];/* c2020 */
free(tranlist.integers);
}

/*$F
 * gqnt - inquire normalization transformation
 *
 * int  *ntnr;       normalization transformation number 
 * int  *errind;     error indicator                     
 * float window[4];  window limits                       
 * float viewpt[4];  viewport limits                     
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 150
 */
gqnt_ (ntnr,errind,window,viewpt)   
int  *ntnr;       
int  *errind;    
float window[4];
float viewpt[4];
{
Gwstran  trans;
debug(("inquire normalization transformation  %d\n",*ntnr));
if (*errind = ginqntran((Gint) *ntnr,&trans)) return;

window[0] = (float)trans.w.xmin;
window[1] = (float)trans.w.xmax;
window[2] = (float)trans.w.ymin;
window[3] = (float)trans.w.ymax;

viewpt[0] = (float)trans.v.xmin;
viewpt[1] = (float)trans.v.xmax;
viewpt[2] = (float)trans.v.ymin;
viewpt[3] = (float)trans.v.ymax;
}

/*$F
 * gqclip - inquire clipping indicator
 *
 * int *errind;      error indicator 
 * int *clsw;        clipping indicator 
 * float clrect[4];  clipping rectangle 
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 151
 */
gqclip_ (errind,clsw,clrect)   
int *errind;   
int *clsw;      
float clrect[4]; 
{ 
Gcliprec clip;
debug(("Inquire Clipping indicator \n"));
if (*errind = ginqclip(&clip)) return;

switch (clip.ind)
 {
  case GCLIP: *clsw = FORT_GCLIP;
       debug(("Clipping indicator  = GCLIP\n"));
      break;
  case GNOCLIP: *clsw = FORT_GNCLIP;
       debug(("Clipping indicator = GNOCLIP \n"));
      break;
 }

clrect[0] = (float)clip.rec.xmin;
clrect[1] = (float)clip.rec.xmax;
clrect[2] = (float)clip.rec.ymin;
clrect[3] = (float)clip.rec.ymax; 
}

/*$F
 * gqopsg - inquire name of open segment
 *
 * int *errind;      error indicator   
 * int *sgna;        name of open segment 
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 151
 */
gqopsg_ (errind,sgna)   
int *errind;   
int *sgna;    
{
debug(("Inquire Name of Open Segment \n"));
*errind = ginqnameopenseg((Gint *)sgna);
}

/*$F
 * gqsgus - inquire set member of segment names in use
 *
 * int *n;           set member requested   
 * int *errind;      error indicator        
 * int *ol;          number of open workstations 
 * int *sgna;        Nth member of set of segment names in use 
 *
 * Returns: 2002 in addition to ANSI standard errors for this function
 *
 * See also: ANSI standard p. 152
 */
gqsgus_ (n,errind,ol,sgna)   
int *n;         
int *errind;   
int *ol;      
int *sgna;   
{
Gintlist segs;
debug(("Inquire Set Member %d of Segment Names in Use \n",*n));
if (*errind = ginqsegnames(&segs)) return;

/* set return values not dependent on range check  c2020 */
*ol = segs.number;

VALIDMEMBER(errind,*n,0,segs.number,errginqsegnames);  /* c2043 c2020 */
if (*n)                                 /* c2020 */
  *sgna = segs.integers[(*n)-1];
free(segs.integers);
}

/*$F
 * gqsim - inquire more simulataneous events
 *
 * int *errind;      error indicator   
 * int *flag;        name of open segment 
 *
 * Returns: ANSI standard errors for this function
 *
 * See also: ANSI standard p. 152
 */
gqsim_ (errind,flag)   
int *errind;   
int *flag;    
{
debug(("Inquire More Simulataneous Events \n"));
*errind = ginqmoreevents((Gsimultev *)flag);
}
