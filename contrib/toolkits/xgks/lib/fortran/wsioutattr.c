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
 * GKS Workstation Independent Primitive Attributes :
 *	gspli_    gsln_    gslwsc_   gsplci_
 *	gspmi_    gsmk_    gsmksc_   gspmci_
 *	gstxi_    gstxfp_  gschxp_   gschsp_
 *	gstxci_   gschh_   gschup_   gstxp_
 *	gstxal_   gsfai_   gsfais_   gsfasi_
 *	gsfaci_   gsasf_   gspkid_
 *       
 * David Berkowitz
 * Bruce Haimowitz
 * TCS Development
 * Cambridge MA
 *
 * August 31 1988
 *
 * $Header: wsioutattr.c,v 4.0 89/08/31 18:37:42 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/wsioutattr.c,v $
 * 
 * $Log:	wsioutattr.c,v $
 * Revision 4.0  89/08/31  18:37:42  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.10  89/06/26  16:15:43  amy
 * DCR d1	Declare errfp external for use in error macros.
 * 		gstxfp:  move data declaration to beginning
 * 		of subroutine.
 * 
 * Revision 1.9  89/02/02  09:59:36  amy
 * PTR c1144	Added Set Pattern Size and Set Pattern Reference Point
 * 		functions.
 * 
 * Revision 1.8  89/01/20  13:44:52  mike
 * Minor fixes found using verification suite
 * 
 * Revision 1.7  88/12/05  15:20:54  owens
 * changed errfp to errfpp
 * 
 * Revision 1.5  88/12/05  14:30:36  todd
 * Changes for Code review II.
 * 
 * Revision 1.4  88/11/15  15:16:42  bhaim
 * Error handling
 * 
 * Revision 1.3  88/11/15  13:03:28  bruce
 * *** empty log message ***
 * 
 * Revision 1.2  88/09/28  11:03:14  bruce
 * Added prologues, changed variable names, and added type casting where needed.
 * 
 * Revision 1.1  88/09/27  08:17:39  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: wsioutattr.c,v 4.0 89/08/31 18:37:42 amy Exp $";

#include "fortxgks.h"
#include <xgks.h>

extern FILE *errfp;             /* d1 */

/*$F
 * gspli - Set Polyline Index
 *
 * int *pli - pointer to polyline index

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.89
 */
gspli_ (pli)
int *pli;

{
debug ( ("Set Polyline Index %d \n", *pli) );

gsetlineind ((Gint)*pli);
}

/*$F
 * gsln - Set Line Type
 *
 * int *ltype - pointer to line type id

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.89
 */
gsln_ (ltype)
int *ltype;

{
debug ( ("Set Linetype %d \n", *ltype) );

gsetlinetype ((Gint)*ltype);
}

/*$F
 * gslwsc - Set Linewidth Scale Factor
 *
 * float *lwidth - pointer to line width scale factor

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.90
 */
gslwsc_ (lwidth)
float *lwidth;
{

debug ( ("Set Linewidth Scale Factor %4.1f \n", *lwidth) ); 

gsetlinewidth ((Gfloat)*lwidth);

}

/*$F
 * gsplci - Set Polyline Colour Index
 *
 * int *coli - pointer to polyline colour index

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.90
 */
gsplci_ (coli)
int *coli;
{

debug ( ("Set Polyline Color Index %d \n", *coli) ); 

gsetlinecolorind ((Gint)*coli);

}

/*$F
 * gspmi - Set Polymarker Index
 *
 * int *pmi - pointer to polymarker index

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.91
 */
gspmi_ (pmi)
int *pmi;
{

debug ( ("Set Polymarker Index %d \n", *pmi) ); 

gsetmarkerind ((Gint)*pmi);

}

/*$F
 * gsmk - Set Marker Type
 *
 * int *mtype - pointer to marker type

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.91
 */
gsmk_ (mtype)
int *mtype;

{
debug ( ("Set Markertype %d \n", *mtype) );

gsetmarkertype ((Gint)*mtype);
}

/*$F
 * gsmksc - Set Marker Size Scale Factor
 *
 * float *mszsf - pointer to marker size scale factor

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.92
 */
gsmksc_ (mszsf)
float *mszsf;

{
debug ( ("Set Marker Size Scale Factor %7.2f \n", *mszsf) );

gsetmarkersize ((Gfloat)*mszsf);

}

/*$F
 * gspmci - Set Polymarker Colour Index
 *
 * int *coli - pointer to polymarker colour index

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.92
 */
gspmci_ (coli)
int *coli;
{

debug ( ("Set PolyMaker Color Index %d \n", *coli) ); 

gsetmarkercolorind ((Gint)*coli);

}

/*$F
 * gstxi - Set Text Index
 *
 * int *txi - pointer to text index

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.92
 */
gstxi_ (txi)
int *txi;
{

debug ( ("Set Text Index %d \n", *txi) ); 

gsettextind ((Gint)*txi);

}

/*$F
 * gstxfp - Set Text Font And Precision
 *
 * int *font - pointer to text font
 * int *prec - pointer to text precision

 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.93
 */
gstxfp_ (font, prec)
int *font;
int *prec;
{
Gtxfp txfp;

debug ( ("Set Text Font and Precision %d %d \n", *font, *prec) );

txfp.font = (Gint)*font;
TEXTPRECISION (*prec, errgsettextfontprec)
txfp.prec = (Gtxprec)*prec;

gsettextfontprec (&txfp);

}

/*$F
 * gschxp - Set Character Expansion Factor
 *
 * float *chxp - pointer to character expansion factor

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.93
 */
gschxp_ (chxp)
float *chxp;
{

debug ( ("Set Character Expansion Factor %4.1f \n", *chxp) ); 


gsetcharexpan ((Gfloat)*chxp);

}

/*$F
 * gschsp - Set Character Spacing
 *
 * float *chsp - pointer to character spacing

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.94
 */
gschsp_ (chsp)
float *chsp;
{

debug ( ("Set Character Spacing %7.2f \n", *chsp) ); 

gsetcharspace ((Gfloat)*chsp);

}

/*$F
 * gstxci - Set Text Colour Index
 *
 * int *coli - pointer to text color index

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.94
 */
gstxci_ (coli)
int *coli;
{

debug ( ("Set Text Color Index %d \n", *coli) ); 

gsettextcolorind ((Gint)*coli);

}

/*$F
 * gschh - Set Character Height
 *
 * float *chh - pointer to character height

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.95
 */
gschh_ (chh)
float *chh;
{

debug ( ("Set Character Height %7.2f \n", *chh) );

gsetcharheight ((Gfloat)*chh);

}

/*$F
 * gschup - Set Character Up Vector
 *
 * float *chux - pointer to x component of up vector
 * float *chuy - pointer to y component of up vector

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.95
 */
gschup_ (chux, chuy)
float *chux, *chuy;
{
Gpoint charup;

debug ( ("Set Character Up Vector %7.2f %7.2f \n", *chux, *chuy) );

charup.x = (Gfloat)*chux;
charup.y = (Gfloat)*chuy;

gsetcharup (&charup);

}

/*$F
 * gstxp - Set Text Path
 *
 * int *txp - pointer to text path

 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.95
 */
gstxp_ (txp)
int *txp;
{

debug ( ("Set Text Path  %d \n", *txp) ); 

TEXTPATH (*txp, errgsettextpath)
gsettextpath ((Gtxpath)*txp);

}

/*$F
 * gstxal - Set Text Alignment
 *
 * int *txalh - pointer to horizontal text alignment
 * int *txalv - pointer to vertical text alignment

 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.96
 */
gstxal_ (txalh, txalv)
int *txalh;
int *txalv;
{
Gtxalign txalign;

debug ( ("Set Text Alignment %d %d \n", *txalh, *txalv) ); 

TEXTALIGNMENTHORIZONTAL (*txalh, errgsettextalign)
txalign.hor = (Gtxhor)*txalh;
TEXTALIGNMENTVERTICAL (*txalv, errgsettextalign)
txalign.ver = (Gtxver)*txalv;

gsettextalign (&txalign);

}

/*$F
 * gsfai - Set Fill Area Index
 *
 * int *fai - pointer to fill area index

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.98
 */
gsfai_ (fai)
int *fai;
{

debug ( ("Set Fill Area Index %d \n", *fai) ); 

gsetfillind ((Gint)*fai);

}

/*$F
 * gsfais - Set Fill Area Interior Style
 *
 * int *ints - pointer to fill area interior style

 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.96
 */
gsfais_ (ints)
int *ints;
{

debug ( ("Set Fill Area Interior Style %d \n", *ints) ); 

FILLAREAINTERIORSTYLE (*ints, errgsetfillintstyle)
gsetfillintstyle ((Gflinter)(*ints));

}

/*$F
 * gsfasi - Set Fill Area Style Index
 *
 * int *styli - pointer to fill area style index

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.97
 */
gsfasi_ (styli)
int *styli;
{

debug ( ("Set Fill Area Style Index %d \n", *styli) ); 

gsetfillstyleind ((Gint)*styli);

}

/*$F
 * gsfaci - Set Fill Area Colour Index
 *
 * int *coli - pointer to fill area colour index

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.98
 */
gsfaci_ (coli)
int *coli;
{

debug ( ("Set Fill Area Color Index %d \n", *coli) ); 

gsetfillcolorind ((Gint)*coli);

}

/*$F
 * gsasf - Set Aspect Source Flags
 *
 * int lasf[13] - array of aspect source flags

 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.99
 */
gsasf_ (lasf)
int  lasf[13];
{
Gasfs asfs;
debug ( ("Set Aspect Source Flags \n") );
ASPECTSOURCE(lasf[0], errgsetasf)
asfs.ln_type     = lasf[0]; 
ASPECTSOURCE(lasf[1], errgsetasf)
asfs.ln_width    = lasf[1]; 
ASPECTSOURCE(lasf[2], errgsetasf)
asfs.ln_colour   = lasf[2]; 
ASPECTSOURCE(lasf[3], errgsetasf)
asfs.mk_type     = lasf[3]; 
ASPECTSOURCE(lasf[4], errgsetasf)
asfs.mk_size     = lasf[4]; 
ASPECTSOURCE(lasf[5], errgsetasf)
asfs.mk_colour   = lasf[5]; 
ASPECTSOURCE(lasf[6], errgsetasf)
asfs.tx_fp       = lasf[6]; 
ASPECTSOURCE(lasf[7], errgsetasf)
asfs.tx_exp      = lasf[7]; 
ASPECTSOURCE(lasf[8], errgsetasf)
asfs.tx_space    = lasf[8]; 
ASPECTSOURCE(lasf[9], errgsetasf)
asfs.tx_colour   = lasf[9]; 
ASPECTSOURCE(lasf[10], errgsetasf)
asfs.fl_inter    = lasf[10]; 
ASPECTSOURCE(lasf[11], errgsetasf)
asfs.fl_style    = lasf[11]; 
ASPECTSOURCE(lasf[12], errgsetasf)
asfs.fl_colour   = lasf[12]; 

gsetasf (&asfs);

}

/*$F
 * gspkid - Set Pick Identifier
 *
 * int	*pkid		- pointer to pick identifier
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.99
 */
gspkid_ (pkid)
int *pkid;

{
gsetpickid ((Gint)*pkid);
}

/*$F
 * gspa - Set Pattern Size - added entire function for PTR# c1144
 *
 * float *szx   - pointer to x size value
 * float *szy   - pointer to y size value

 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.105
 */

gspa_ (szx, szy)
float *szx;
float *szy;

{
Gpoint size;

debug ( ("Set Pattern Size  %f %f\n", *szx, *szy) );

size.x = *szx;
size.y = *szy;

gsetpatsize (&size);

}

/*$F
 * gsparf - Set Pattern Reference Point - added entire function for PTR# c1144
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

gsparf_ (rfx, rfy)
float *rfx;
float *rfy;

{
Gpoint ref;

debug ( ("Set Pattern Reference Point  %f %f\n", *rfx, *rfy) );

ref.x = *rfx;
ref.y = *rfy;

gsetpatrefpt (&ref);

}
