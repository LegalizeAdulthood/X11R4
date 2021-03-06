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
 * University of Illinois at Urbana-Champaign
 * Department of Computer Science
 * 1304 W. Springfield Ave.
 * Urbana, IL	61801
 *
 * (C) Copyright 1987, 1988 by The University of Illinois Board of Trustees.
 * All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 * 
 * text.c - functions and data for gks text.
 *		gsetcharexpan()
 *		gsetcharheight()
 *		gsetcharspace()
 *		gsetcharup()
 *		gsettextalign()
 *		gsettextcolourind()
 *		gsettextfontprec()
 *		gsettextind()
 *		gsettextpath()
 *		gsettextrep()
 *		gtext()
 * utility routines:
 *		XgksInitGksText()
 *		XgksInitWssText()
 *		XgksComputeVec()
 * 
 * $Header: text.c,v 4.0 89/08/31 16:25:16 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/text.c,v $
 *
 * $Log:	text.c,v $
 * Revision 4.0  89/08/31  16:25:16  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.21  89/06/05  10:15:02  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * PTR# c1176:	Replaced string function calls with macros that check for
 * 		NULL pointers before making the call.
 * PTR# c1179:	Changed exp field to tx_exp or ch_exp for AIX compiler
 * 		Moved variable declarations illegal in AIX.
 * 
 * Revision 3.20  89/05/04  13:41:19  bruce
 * Closed an unclosed comment in XgksInitGksText().
 * 
 * Revision 3.19  89/03/30  17:56:06  bruce
 * PTR# c1159:	Cleanup changes for PTR
 * 
 * Revision 3.18  89/03/15  17:50:12  amy
 * PTR c1159	XgksInitGksText:  check whether the user has set
 * 		a new fonts path and, if so, use it.  Otherwise use
 * 		the setting for FONTDBDIR from the Makefile.
 * 
 * Revision 3.17  89/02/13  16:55:20  amy
 * No change.
 * 
 * Revision 3.16  89/02/13  16:53:23  amy
 * PTR c1154	XgksInitGksText:  set default precision to GSTRING, and default 
 * 		character spacing to 0.0.
 * 
 * Revision 3.15  89/02/04  15:30:21  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 3.14  88/12/28  14:36:16  amy
 * PTR c1143	XgksInitGksText, XgksInitWssText:  Added new predefined
 * 		representations as per the ANSI standard and changed the
 * 		initialization functions to initialize new workstations with
 * 		the additional predefined representations.
 * 
 * Revision 3.13  88/12/28  14:34:34  amy
 * PTR c1075	XgksInitWssText:  Created and initialized default attribute
 * 		bundle, which is now used to initialize the GKS state list when	
 * 		GKS is opened, and the workstation state list when a new
 * 		workstation is opened.
 * 
 * Revision 3.12  88/12/08  13:24:07  amy
 *  PTR c1130	GKSERROR calls:  changed function name parameter
 * 		from character string to enum. type value.
 * Changed spelling of color to colour.
 * Changed spelling of center to centre.
 * 
 * Revision 3.11  88/08/18  11:06:48  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 3.10  88/08/11  09:03:02  amy
 * PTR c1012  ginqtextextent & gsettextrep:  added call to VALID_WSID
 * to check for error 20.
 * 
 * Revision 1.4  88/07/28  16:27:42  bruce
 * PTR# c1092   An unclosed comment was causing the compiler to ignore
 * the statement that checked for a text index < 1
 * 
 * Revision 1.3  88/07/28  15:08:03  owens
 * added check for error 20 for PTR c1012
 * 
 * Revision 1.2  88/07/26  17:54:18  owens
 * renamed/replaced VALID_WSID macro for PTR c1012 (DWO)
 * 
 * Revision 1.1  88/07/21  10:50:30  david
 * Initial revision
 *  
 *
 */

static char *rcsid = "$Header: text.c,v 4.0 89/08/31 16:25:16 amy Exp $"; 

#include "gks_implem.h"                 /* d1 */

#include <stdio.h>
#include <strings.h>
#include <math.h>  

#define PI	3.1415926535897932384626433 

/* 
 * XgksInitGksText --  gks state list text stuff
 */

/* c1143:  Created predefined reps 1-6 */

/* c1147:  Moved predefined reps to text.h */
#include "text.h"							/* c1147 */

XgksInitGksText()
{
	xgks_state.gks_txattr.text      = 1; /* text indx */ /* c1143 */ /* c1147 */
        xgks_state.gks_txattr.fp  = GINDIVIDUAL; /* font ASF */ /* c1143 */ /* c1147 */
	xgks_state.gks_txattr.tx_exp = GINDIVIDUAL; /* exp ASF  c1143 c1147 c1179 */
	xgks_state.gks_txattr.space = GINDIVIDUAL; /* space ASF */ /*c1143*/ /*c1147*/
	xgks_state.gks_txattr.colour = GINDIVIDUAL; /* colour ASF */ /* c1143 */ /* c1147 */
	/* text bundle */						/* c1143 */
	xgks_state.gks_txattr.bundl.fp.font= def_txbundl[0].fp.font; /* c1143 */ /*c1147*/
	xgks_state.gks_txattr.bundl.fp.prec= GSTRING;		  /* c1154 c1143 c1147*/
	xgks_state.gks_txattr.bundl.ch_exp	= def_txbundl[0].ch_exp; /* c1143 c1147 c1179 */
	xgks_state.gks_txattr.bundl.space	= DEFCHRSPACE;	/* c1154 c1143 c1147*/
	xgks_state.gks_txattr.bundl.colour	= def_txbundl[0].colour; /* c1143 */ /*c1147*/

	xgks_state.gks_chattr.height = 0.01;  /* height of capital */	/*c1147*/
	xgks_state.gks_chattr.chwidth  = 0.01;				/*c1147*/
	xgks_state.gks_chattr.up.x     = 0.0;  /* angle w.r.t the X axis, in WC */ /*c1147*/
	xgks_state.gks_chattr.up.y     = 1.0;				/*c1147*/
        xgks_state.gks_chattr.base.x   = 1.0;				/*c1147*/   
	xgks_state.gks_chattr.base.y   = 0.0;				/*c1147*/  
 
	xgks_state.gks_chattr.path     = GTP_RIGHT;  /* writing direction w.r.t.up vector */ /*c1147*/
	xgks_state.gks_chattr.align.hor	= GTH_NORMAL;			/*c1147*/
	xgks_state.gks_chattr.align.ver	= GTV_NORMAL;			/*c1147*/

	/* check if user has chosen a new font db dir */            /* c1159 */
	/* (file is not actually opened until xReadFont */          /* c1159 */
	/* in xtext.c)             (DWO)                */          /* c1159 */
        xgks_state.fontdbdir = (char *)getenv("XGKSFontDir");       /* c1159 */
        if (xgks_state.fontdbdir == NULL)                           /* c1159 */
          xgks_state.fontdbdir = FONTDBDIR;       /* use default */ /* c1159 */
 
        /*fprintf(stderr,"FontDir = %s\n",xgks_state.fontdbdir);*/

}

/*
 * XgksInitWssText(ws) - send the current INDIVIDUAL attributes and BUNDLE
 *	index to the newly opened workstation ws.
 */

XgksInitWssText(ws)
WS_STATE_PTR ws;
{
	Gint i;

	/* c1143: txbundl_table[0] never used but initialized anyway */
	ws->txbundl_table[0] = def_txbundl[0];				/* c1143 */

	/* c1143: initialize predefined representation bundles */
	for (i=1; i<=PDF_TEXT_BNDLS; i++)				/* c1143 */
		/* c1075: use implementation defaults, not current attributes */
		ws->txbundl_table[i] = def_txbundl[i-1];		/* c1143 */

	/* c1143: initialize rest of representation bundle array */
	for (i=PDF_TEXT_BNDLS+1; i<MAX_BUNDL_TBL; i++)			/* c1143 */
		ws->txbundl_table[i] = def_txbundl[0];			/* c1143 */
}
/*$F
 * gsetcharexpan(expansion) - SET CHARACTER EXPANSION FACTOR
 *
 * Gfloat expansion;		character expansion factor used when 
 *				text_expansion_asf is INDIVIDUAL.
 *
 * returns 0=OK, or one of 8, 77.
 *
 * See Also: ANSI Standard p.93
 */
gsetcharexpan(expansion)
Gfloat expansion;
{
/* check for proper gks operating state */
	GKSERROR ((xgks_state.gks_state == GGKCL) , 8, errgsetcharexpan) /*c1147*/

/* check for valid expansion factor */
	GKSERROR ((expansion <= 0.0) , 77, errgsetcharexpan)

/* Ok to change the current expansion factor */
	xgks_state.gks_txattr.bundl.ch_exp = expansion;		/* c1179 c1147 */

	if (MO_OPENED == TRUE) XgksMoSetGraphicSize (31, expansion);

	return( OK );
}

/*$F
 * gsetcharheight(height) - SET CHARACTER HEIGHT
 *
 * Gfloat height;		nominal height of a capital letter.
 *				height in WC space, must be > 0.0.
 *
 * returns 0=OK, or one of 8, 78
 *
 * See Also: ANSI Standard p.95
 *
 * Note : gks_chattr.chwidth is also set here to "height"
 */
gsetcharheight(height)
Gfloat height;
{
/* check for proper gks operating state */
	GKSERROR ((xgks_state.gks_state == GGKCL) , 8, errgsetcharheight) /*c1147*/

/* check for valid character height */
	GKSERROR ((height <= 0.0) , 78, errgsetcharheight)

/* Ok to change the current height */
	xgks_state.gks_chattr.height = height;
	xgks_state.gks_chattr.chwidth = height;

	if (MO_OPENED == TRUE)  XgksMoSetCharUp ();

	return( OK );
}

/*$F
 * gsetcharspace(spacing) - SET CHARACTER SPACING
 *
 * Gflaot spacing;		Additional space between characters when
 *				text_spacing_asf is INDIVIDUAL.
 *
 * returns 0=OK, or 8.
 *
 * See Also: ANSI Standard p.94
 */
gsetcharspace(spacing)
Gfloat spacing;
{
/* check for proper gks operating state */
	GKSERROR ((xgks_state.gks_state == GGKCL) , 8, errgsetcharspace) /*c1147*/

/* ok to change the spacing */
	xgks_state.gks_txattr.bundl.space = spacing;			/*c1147*/

	if (MO_OPENED == TRUE) XgksMoSetGraphicSize (32, spacing);

	return( OK );
}

/*$F
 * gsetcharup(up_vector) - SET CHARACTER UP VECTOR
 * 
 * Gpoint *up_vector;	new up direction for characters (length is irrelevant.)
 *
 * returns 0=OK, 8, 79
 *
 * See Also: ANSI Standard p.95
 *
 * Note : gks_chattr.base is also set to here, to a vector of arbitrary length (in this
 *	  implementation to, 1.0) at a right angle in the clockwise direction to the value
 *	  specified by the parameter
 */
gsetcharup(up_vector)
Gpoint *up_vector;
{
	Gfloat scale;
/* check for proper gks operating state */
	GKSERROR ((xgks_state.gks_state == GGKCL) , 8, errgsetcharup) /*c1147*/

/* check for valid character up vector*/
	GKSERROR ((up_vector->x == 0.0 && up_vector->y == 0.0) , 79, errgsetcharup)

/* Scale to normalize the vector */
	scale = sqrt((up_vector->x*up_vector->x)+(up_vector->y*up_vector->y));
/* Ok to change the current up direction */
	xgks_state.gks_chattr.up.x = up_vector->x / scale;
	xgks_state.gks_chattr.up.y = up_vector->y / scale;

/* Also set the gks_chattr.base vector to right angle to up_vector */
	xgks_state.gks_chattr.base.x =  xgks_state.gks_chattr.up.y;
	xgks_state.gks_chattr.base.y = -xgks_state.gks_chattr.up.x;

	if (MO_OPENED == TRUE)  XgksMoSetCharUp ();
	
	return( OK );
}

/*$F
 * gsettextalign(txalign) - SET TEXT ALIGNMENT
 *
 * Gtxalign *txalign;
 * 	txalign->hor,		GTH_NORMAL | GTH_LEFT | GTH_CENTRE | GTH_RIGHT
 *	txalign->ver;		GTV_NORMAL | GTV_TOP | GTV_CAP | GTV_HALF | GTV_BASE | GTV_BOTTOM
 *
 * returns 0=OK, or 8, 2000
 *
 * See Also: ANSI Standard p.96
 *
 * Note: horizontal is not saying it is NORMAL to be LEFT of CENTRE.
 */
gsettextalign(txalign)
Gtxalign *txalign;
{
/* check for proper gks operating state */
	GKSERROR ((xgks_state.gks_state == GGKCL) , 8, errgsettextalign) /*c1147*/

/* check for valid enumration type */
	GKSERROR ((txalign->hor!=GTH_NORMAL && txalign->hor!=GTH_LEFT && 
		txalign->hor!=GTH_RIGHT && txalign->hor!=GTH_CENTRE), 2000, errgsettextalign)
	GKSERROR ((txalign->ver!=GTV_NORMAL && txalign->ver!=GTV_TOP && txalign->ver!=GTV_CAP && 
		txalign->ver!=GTV_HALF && txalign->ver!=GTV_BASE && txalign->ver!=GTV_BOTTOM), 2000, errgsettextalign)

/* ok to change alignment */
	xgks_state.gks_chattr.align = (*txalign);

	if (MO_OPENED == TRUE) XgksMoSetTextAlign (txalign);

	return( OK );
}

/*$F
 * gsettextcolourind(colour) - SET TEXT COLOUR INDEX
 *
 * Gint colour;			new colour for INDIVIDUALly specified text.
 *
 * returns 0=OK, or one of 8, 92
 *
 * See Also: ANSI Standard p.94
 */
gsettextcolourind(colour)
Gint colour;
{
/* check for proper gks operating state */
	GKSERROR ((xgks_state.gks_state == GGKCL) , 8, errgsettextcolourind) /*c1147*/

/* check for valid colour index */
	GKSERROR ((colour < 0) , 92, errgsettextcolourind)

/* ok to change the colour */
	xgks_state.gks_txattr.bundl.colour = colour;		/*c1147*/

	if (MO_OPENED == TRUE) XgksMoSetGraphicAttr (33, colour);

	return( OK );
}

/*$F
 * gsettextfontprec(txfp) - SET TEXT FONT AND PRECISION
 *
 * Gtxfp  *txfp;
 * 	txfp->font	new font for INDIVIDUALly specified text.
 * 	txfp->prec	precision with which that font is output, GSTRING | GCHAR | GSTROKE
 *
 * returns 0=OK, or one of 8, 75, 2000
 *
 * See Also: ANSI Standard p.93
 */
gsettextfontprec(txfp)
Gtxfp *txfp;
{
/* check for proper gks operating state */
	GKSERROR ((xgks_state.gks_state == GGKCL) , 8, errgsettextfontprec) /*c1147*/

/* check for valid font number */
	GKSERROR ((txfp->font == 0) , 75, errgsettextfontprec)

/* check for valid precision enumeration type */
	GKSERROR ((txfp->prec!=GSTRING && txfp->prec!=GCHAR  && txfp->prec!=GSTROKE), 2000, errgsettextfontprec)

/* ok to change the font and precision*/
	xgks_state.gks_txattr.bundl.fp = (*txfp);		/*c1147*/

	if (MO_OPENED == TRUE) XgksMoSetTextFP (txfp);

	return( OK );
}

/*$F
 * gsettextind(idx) - SET TEXT INDEX
 *
 * Gint idx;			new current bundle entry for BUNDLED attributes.
 *
 * returns 0=OK, or one of 8, 72.
 *
 * See Also: ANSI Standard p.92
 *
 */
gsettextind(idx)
Gint idx;
{
/* check for proper gks operating state */
	GKSERROR ((xgks_state.gks_state == GGKCL) , 8, errgsettextind) /*c1147*/

/* check if valid font number */     /* c1092   comment wasn't closed! */
	GKSERROR ((idx < 1) , 72, errgsettextind)

/* ok to change the index */
	xgks_state.gks_txattr.text = idx;			/*c1147*/

	if (MO_OPENED == TRUE) XgksMoSetGraphicAttr (29, idx);

	return( OK );
}

/*$F
 * gsettextpath(path) - SET TEXT PATH
 *
 * Gtxpath path;		new text path GTP_RIGHT | GTP_LEFT | GTP_UP | GTP_DOWN.
 *
 * returns 0=OK or one of 8, 2000
 *
 * See Also: ANSI Standard p.95
 */
gsettextpath(path)
Gtxpath path;
{
/* check for proper gks operating state */
	GKSERROR ((xgks_state.gks_state == GGKCL) , 8, errgsettextpath) /*c1147*/

/* check for valid enumeration type */
	GKSERROR ((path!=GTP_RIGHT && path!=GTP_LEFT && path!=GTP_UP && path!=GTP_DOWN), 2000, errgsettextpath)

/* set the text path */
	xgks_state.gks_chattr.path = path;

	if (MO_OPENED == TRUE) XgksMoSetTextPath (path);

	return( OK );
}

/*$F
 * gsettextrep(ws_id, idx, bundle) - SET TEXT REPRESENTATION
 *
 * Gint ws_id;			workstation identifier.
 * Gint	idx;			bundle table entry to set.
 * Gtxbundl *bundle;		address of text bundle entry structure.
 *
 * returns 0=OK or one of 7, 20, 25, 33, 35, 36, 72, 75, 76, 77, 93, 2000
 *
 * See Also: ANSI Standard p.102
 */
gsettextrep(ws_id, idx, bundle)
Gint ws_id;
Gint idx;
Gtxbundl *bundle;
{
	WS_STATE_PTR ws;

/* check for proper gks operating state */
	GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP) , 7, errgsettextrep) /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgsettextrep)   /* c1012 */

/* check for open workstation identifier */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR (((ws=OPEN_WSID(ws_id))==NULL) , 25, errgsettextrep)  /* c1012 */

/* check for proper workstation category */
	GKSERROR ((WS_CAT(ws)== GMI) , 33, errgsettextrep)

	GKSERROR ((WS_CAT(ws)== GINPUT) , 35, errgsettextrep)

	GKSERROR ((WS_CAT(ws) == GWISS) , 36, errgsettextrep)

/* check for valid text bundle table index */
	GKSERROR ((idx < 1 || idx >= MAX_BUNDL_TBL) , 72, errgsettextrep)

/* check for valid font */
	GKSERROR ((bundle->fp.font == 0 ) , 75, errgsettextrep)

/* check if font type is supported by the workstation */
	GKSERROR ((!WS_FONT_TYPE(bundle->fp.font)), 76, errgsettextrep)

/* check for valid character expansion */
	GKSERROR ((bundle->ch_exp <= 0.0) , 77, errgsettextrep)

/* check for valid colour index */
	GKSERROR ((!WS_AVAIL_COLOUR(ws,bundle->colour)) , 93, errgsettextrep)

/* check for valid precision enumeration type */
	GKSERROR ((bundle->fp.prec!=GSTRING && bundle->fp.prec!=GCHAR  && bundle->fp.prec!=GSTROKE), 
		   2000, errgsettextrep)

/* finally we get to change the bundle table entry */

	if (ws->ewstype == MO)
		XgksMoSetTextRep (ws, idx, bundle);

	ws->txbundl_table[idx] = (*bundle);

	return( OK );
}

/*
 * XgksComputeVec(up_vec, base_vec) - utility function to compute the text vectors.
 *			base on current [gks_chattr] attributes calculate two unit 
 *		        vectors in NDC space, these two vectors are placed on NDC
 *			origin. 
 *
 *   NOTE : 	These vectors contain information of both direction and magnitude
 *
 */
XgksComputeVec(up_vec, base_vec)
Gpoint *up_vec, *base_vec;
{
	Gpoint wc_window_origin, ndc_origin;
	Gpoint ht, wt;

/* vector tail -- figuring out translation from 
		  WC origin to NDC origin */
	wc_window_origin.x = 0.0;
	wc_window_origin.y = 0.0;
	WcToNdc(&wc_window_origin, &ndc_origin);
	
/* vector parallel to gks_chattr.up vector and length equal char_height */
	ht.x = xgks_state.gks_chattr.up.x * xgks_state.gks_chattr.height;
	ht.y = xgks_state.gks_chattr.up.y * xgks_state.gks_chattr.height;

/* convert it to ndc space */
	WcToNdc(&ht, up_vec);
	up_vec->x -= ndc_origin.x;  /* translate to NDC origin */
	up_vec->y -= ndc_origin.y;

/* head of width vector */
/* vector perpendicular to gks_text_up_vector and length equal to char_height */
	wt.x = xgks_state.gks_chattr.base.x * xgks_state.gks_chattr.chwidth;
	wt.y = xgks_state.gks_chattr.base.y * xgks_state.gks_chattr.chwidth;

/* convert it to ndc space */
	WcToNdc(&wt, base_vec);
	base_vec->x -= ndc_origin.x;	/* Again translate to origin */
	base_vec->y -= ndc_origin.y;

}

/*$F
 * gtext(at,string) - TEXT
 *
 * Gpoint *at;		World coordinate where text string is to be output.
 * Gchar *string;	The text to output.
 *
 * returns 0=OK, or one of 5, 101;
 *
 * See Also: ANSI Standard p.83
 *
 */
gtext(at,string)
Gpoint *at;
Gchar *string;
{
	OUT_PRIMI  *text;

/* check for proper gks operating state */
	GKSERROR ((xgks_state.gks_state != GWSAC && xgks_state.gks_state != GSGOP) , 5, errgtext) /*c1147*/

/* open an primitive structure */
	GKSERROR ( ((text = XgksNewPrimi()) == NULL), 300, errgtext);

	text->pid = TEXT;

/* get memory for starting location of text */
	GKSERROR ( ( ((text->primi.text.location) = (Gpoint *) malloc ((unsigned) sizeof(Gpoint)))==NULL), 
		  300, errgtext);

/* get momory for text string */
	GKSERROR ( ( (text->primi.text.string= (Gchar *)malloc ((unsigned) STRLEN(string) + 1)) == NULL ), 300, errgtext);	/* c1176 */

	STRCPY ((text->primi.text.string), string);	/* c1176 */

	WcToNdc(at, (text->primi.text.location));

/* make sure vectors are loaded */
	XgksComputeVec (&(text->primi.text.up_vec), &(text->primi.text.base_vec)); 

	text->primi.text.txattr = xgks_state.gks_txattr;		/*c1147*/
	text->primi.text.chattr = xgks_state.gks_chattr;

	XgksProcessPrimi (text);

	if (MO_OPENED == TRUE) XgksMoText (text->primi.text.location, string);

	free (text->primi.text.string);
	free (text->primi.text.location);
	free (text);

	return( OK );
}

/*$F
 * ginqtextextent(ws_id, position, string, extent) - INQUIRE LIST OF TEXT EXTENT
 *
 * Gint ws_id;			workstation inquiry is about.
 * Gpoint position;
 * Gchar *string;
 * Gextent *extent;
 *
 * errors 0, 7, 20, 25, 39, 101 can occur.
 *
 * See also: ANSI standard p.158
 */

ginqtextextent(ws_id, position, string, extent)
	Gint ws_id;
	Gpoint position;
	Gchar *string;
	Gextent *extent;
{
	WS_STATE_PTR ws;
	TEXT_ST text;
	Gpoint *ndc_ptr, *wc_ptr, ndc_points[5];

/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqtextextent); /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqtextextent)   /* c1012 */

/* check if this workstation is opened */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR( ((ws = OPEN_WSID(ws_id)) == NULL), 25, errginqtextextent);  /* c1012 */

/* check workstation type */
	GKSERROR( (WS_CAT(ws) != GOUTIN && WS_CAT(ws) != GOUTPUT), 39, errginqtextextent);

/* check for valid string */
	GKSERROR ( (STRLEN(string) == 0), 101, errginqtextextent);	/* c1176 */
/*
 * The actual inquiries have to be done in x-rountine 
 *  
 * The way to do it is to construct a fake structure and
 * let the x-routine crank everything out
 */
/* get memory for starting location of text */
        GKSERROR ( ( ((text.location) = (Gpoint *) malloc ((unsigned) sizeof(Gpoint)))==NULL), 300, errginqtextextent);

/* get momory for text string */
        GKSERROR ( ( (text.string= (Gchar *)malloc ((unsigned) STRLEN(string) + 1)) == NULL ), 300, errginqtextextent);	/* c1176 */

        STRCPY ((text.string), string);	/* c1176 */

        WcToNdc(&(position), text.location);

/* make sure vectors are loaded */
        XgksComputeVec (&(text.up_vec), &(text.base_vec)); 

        text.txattr = xgks_state.gks_txattr;			/*c1147*/
        text.chattr = xgks_state.gks_chattr;

	xXgksInqTextExtent (ws, &text, ndc_points);		/*c1147*/

        ndc_ptr = ndc_points; wc_ptr = &(extent->concat);
        NtNdcToWc(xgks_state.cur_ntrans, ndc_ptr, wc_ptr);	/*c1147*/ 

        ndc_ptr++;  wc_ptr = &(extent->ll);
        NtNdcToWc(xgks_state.cur_ntrans, ndc_ptr, wc_ptr);	/*c1147*/ 

        ndc_ptr++;  wc_ptr = &(extent->lr);
        NtNdcToWc(xgks_state.cur_ntrans, ndc_ptr, wc_ptr); 	/*c1147*/

        ndc_ptr++;  wc_ptr = &(extent->ur);
        NtNdcToWc(xgks_state.cur_ntrans, ndc_ptr, wc_ptr);	/*c1147*/ 

        ndc_ptr++;  wc_ptr = &(extent->ul);
        NtNdcToWc(xgks_state.cur_ntrans, ndc_ptr, wc_ptr);	/*c1147*/ 

	free( text.location );
	free( text.string );

	return( OK );
}
