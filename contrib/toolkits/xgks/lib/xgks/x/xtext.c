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
 *	All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 * 
 * xtext.c - functions and data for gks text.
 * 
 * $Header: xtext.c,v 4.0 89/08/31 16:29:57 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/x/RCS/xtext.c,v $
 *
 * $Log:	xtext.c,v $
 * Revision 4.0  89/08/31  16:29:57  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.25  89/06/05  10:25:00  bruce
 * DCR# d1:	Changed include file name from gks_implement.h 
 * 		to gks_implem.h for AIX compiler.
 * 		Also added a display argument to all calls to 
 * 		SIGIO functions.
 * PTR# c1179:	Changed field name from exp to tx_exp or ch_exp
 * 		for AIX compiler.
 * 
 * Revision 3.24  89/03/30  17:45:49  bruce
 * PTR# c1171:	Changed arg types to xTransMatrix to avoid conversion error
 * PTR# c1159:	allow user to set font path thru environment variable
 * 
 * Revision 3.23  89/02/04  15:30:21  amy
 * PTR c1147	Make global vars. and functions private, or static where possible.
 * 
 * Revision 3.22  88/12/08  16:06:14  amy
 * Changed spelling of color to colour.
 * Changed spelling of center to centre.
 * 
 * Revision 3.21  88/12/02  10:15:31  amy
 * Cosmetic fix to Log entry.
 * 
 * Revision 3.20  88/12/02  10:08:10  amy
 * New 4.3 C compiler--
 * 	xText:  added default labels to switch statements to insure that variables
 * 	xtrans and ytrans are being set.
 * 	xInqTextExtent:  same as for xText.
 * 	xComputeText:  cast wsr_colour as int in call to WS_AVAIL_COLOUR.
 * 	xIdentityMatrix, xTransMatrix, xCopyMatrix, xMultMatrix, ck_ranges, and 
 * 	ApplyMatrix:  changed from int to void functions since no callers expect a
 * 	return value.
 * 
 * Revision 3.19  88/11/28  15:51:25  amy
 * PTR c1136	xMesg:  added X call to set the specified font in the GC.
 * 
 * Revision 3.18  88/10/21  10:08:59  amy
 * PTR c1083:	Corrected character placement and placement of concatenation point
 * 		for all text paths and alignments.
 * 
 * Revision 3.17  88/10/21  08:26:26  amy
 * ILL2	  Second new version from Kelvin (Ill.) to fix shear problem.
 * 
 * Revision 3.16  88/10/12  15:28:51  amy
 * New version from Kelvin (Ill.) to fix shear problem.
 * 
 * Revision 3.15  88/09/26  10:41:30  amy
 * MIT	All functions return a value.
 * 	xDumpMatrix:  conditionally compile with an ifdef statement.
 * 
 * Revision 3.14  88/08/19  12:43:19  amy
 * AUG  xtext:  gc assignment changed from ws->gc to ws->textgc.
 *      Calls changed as follows:
 *      Previously           Now
 *      ----------           ---
 *      XSetClipRectangles   xSetTextClipRectangles
 *      XSetForeground       xSetForeground
 *      XSetLineAttributes   xSetLineAttributes
 *      XSetFillStyle        xSetFillStyle
 *      xMesg:  changes not merged:
 *      topheight definition, assignment and use in call to XFillRectangle.
 * 
 * Revision 3.13  88/08/15  06:58:16  amy
 * PTR  y parameter to call to XFillRectangle changed
 * 
 * Revision 1.2  88/08/08  15:44:09  bruce
 * PTR# c1001 - The parameter for the y position of the call to XFillRectangle that clears the message
 * space had to be adjusted so that it does in fact clear the message.
 * 
 * Revision 1.1  88/07/21  10:52:13  david
 * Initial revision
 *  
 *
 */

static char rcsid[] = "$Header: xtext.c,v 4.0 89/08/31 16:29:57 amy Exp $";


#include <sys/file.h>
#include <math.h>
#include "signal.h"
#include "font.h"
#include "gks_implem.h"		/* d1 */

#define PI	3.1415926535897932384626433

#define MAX_CHAR_STROKE	 100	/* Hope this is enought, it should be ..*/

static								/* c1147 */
Gpoint sndc_ht;    /* ndc_ht */		/*ILL*/
static								/* c1147 */
Gpoint sndc_wt;    /* ndc_wt */		/*ILL*/

static								/* c1147 */
Gfloat stroke_trans[6];			/*ILL2*/
 
static								/* c1147 */
Gfloat wsr_spacing;	/* additional spacing */
static								/* c1147 */
Gint wsr_font;
static								/* c1147 */
unsigned long wsr_colour;

static								/* c1147 */
unsigned int DCYMAX; 

static struct {
	Gint f_loaded;
	FONT	*f_font;
} FontTable[DEFINED_FONT_TYPE+1]; 

static Gint FirstTime = TRUE;

static								/* c1147 */
Display *dpy;
static								/* c1147 */
Window  win;
static								/* c1147 */
GC 	gc;

static xComputeText(ws, tx)
WS_STATE_PTR ws;
TEXT_ST *tx;
{
	Gint  idx;

	DCYMAX = ws->wbound.y;	/* Need this info later */

	idx=tx->txattr.text;
	if (tx->txattr.text < 1 || tx->txattr.text >= MAX_BUNDL_TBL)
		idx = 1;

/* get the font */
	if (tx->txattr.fp == GINDIVIDUAL) 
		wsr_font = tx->txattr.bundl.fp.font;
	else
		wsr_font = ws->txbundl_table[idx].fp.font;
	
	if (FontTable[wsr_font].f_loaded != TRUE)
		if (xReadFont(wsr_font) != 0) {
			wsr_font = 1;
			if (xReadFont(wsr_font) != 0) {
				fprintf(stderr, "XGKS: can't load font %d from path '%s' - aborting\n",wsr_font,xgks_state.fontdbdir); /* c1159 */
				exit(0);
			}
		}
		
/* font colour */
	if (tx->txattr.colour == GINDIVIDUAL)
		wsr_colour = tx->txattr.bundl.colour;
	else 
		wsr_colour = ws->txbundl_table[idx].colour;

	if ( ! WS_AVAIL_COLOUR(ws, (int)wsr_colour))	/* New 4.3 C compiler */
		wsr_colour = 1;
	if ( ws->wscolour == 2 )	{	/* monochrome ? */
		if (wsr_colour == 0) wsr_colour = ws->wsbg;
		else if (wsr_colour == 1) wsr_colour = ws->wsfg;
	}
	
	/* New Text Transformation code */
   	/* up_vector (with height) in X coordinate */
   	sndc_ht.x = (tx->up_vec.x*ws->ndctoxtrans.xScale);	/*ILL2*/
    	sndc_ht.y = (tx->up_vec.y*ws->ndctoxtrans.yScale);	/*ILL2*/

	/* Base vector (with size) in X coordinate */
  	sndc_wt.x = (tx->base_vec.x*ws->ndctoxtrans.xScale);	/*ILL2*/
   	sndc_wt.y = (tx->base_vec.y*ws->ndctoxtrans.yScale);	/*ILL2*/

	/* apply the char_expansion factor */
	if (tx->txattr.tx_exp == GINDIVIDUAL) {		/* c1179 */
		sndc_wt.x *= tx->txattr.bundl.ch_exp;	/* c1179 */
		sndc_wt.y *= tx->txattr.bundl.ch_exp;	/* c1179 */
	}
	else {
		sndc_wt.x *= ws->txbundl_table[idx].ch_exp;	/* c1179 */
		sndc_wt.y *= ws->txbundl_table[idx].ch_exp;	/* c1179 */
	}
	xIdentityMatrix(stroke_trans);
   	stroke_trans[0] = 
		(sndc_wt.x) / (FontTable[wsr_font].f_font->fnominalx); /*ILL2*/
    	stroke_trans[3] = 
		(sndc_wt.y) / (FontTable[wsr_font].f_font->fnominalx); /*ILL2*/
    	stroke_trans[1] = 
		(sndc_ht.x) / (FontTable[wsr_font].f_font->fnominaly); /*ILL2*/
    	stroke_trans[4] = 
		(sndc_ht.y) / (FontTable[wsr_font].f_font->fnominaly); /*ILL2*/
		
/* additional spacing scale factor */
	if (tx->txattr.space == GINDIVIDUAL)
		wsr_spacing = tx->txattr.bundl.space;
	else
		wsr_spacing = ws->txbundl_table[idx].space;
/*
 * At this point the Scale and Rotate matrix is set up.  The rotation angle
 * is in global variable 'alpha'; the colour is in wsr_colour the font number
 * is in wsr_font.
 */
}

/*$F
 * xXgksText(ws, text)	-- TEXT output
 *
 * Note: this function will output tabs, form feeds, line feeds, and like control
 *	characters as a special symbol.
 */
xXgksText(ws, tx)  /* c1147 */
WS_STATE_PTR ws;
TEXT_ST *tx;
{
	Gchar *string;
	Gchar *text_ptr;
	Gpoint atsndc;          /*       ||||||||| */
        FONT *cfont;            /* c1171 VVVVVVVVV */
        Gfloat fsize, fwidth, xtrans=0.0, ytrans=0.0, xFyMin(), xFxMax(), xFyMax();
				   /* new xFyMin() funct added  PTR# c1083 */
	struct vcharst *cdef;
	Gint i;


	if (ws->ewstype != X_WIN) return(OK);	/*MIT*/


	XgksSIGIO_OFF(ws->dpy); /* c1147 *//* d1 */

#ifdef TDEBUG
fprintf(stderr, "\n\nEntering ......---->  XTEXT  <-----......\n");
#endif
	if (FirstTime == TRUE) {  	/* First Time initializing .. */
		for (i=0; i<DEFINED_FONT_TYPE; i++) FontTable[i].f_loaded = FALSE;
		FirstTime = FALSE;
	}

	/* X-window attributes init */
	dpy = ws->dpy;
	win = ws->win;
	gc  = ws->textgc;	/* AUG */

	xComputeText(ws, tx);

	cfont = FontTable[wsr_font].f_font;

/* compute transformation to move the whole string to the alignment point */
	/* transform the NDC start point to Scaled NDC */
	atsndc.x = (tx->location->x*ws->ndctoxtrans.xScale) + ws->ndctoxtrans.xTrans;
	atsndc.y = (tx->location->y*ws->ndctoxtrans.yScale) + ws->ndctoxtrans.yTrans;

#ifdef TDEBUG
fprintf(stderr, " from xtext ... ATSNDC.x=%f  atsndc.y=%f\n", atsndc.x, atsndc.y);
#endif

    stroke_trans[2] = atsndc.x;		/*ILL2*/
    stroke_trans[5] = atsndc.y;		/*ILL2*/

#ifdef TDEBUG
fprintf(stderr, "after matrix ops of ATSNDc ..\n");
xDumpMatrix("stroke_trans", stroke_trans);	/*ILL2*/
#endif

	/* find total length or height of string + spacing */
	for ((string=tx->string), fsize=0.0, fwidth = -9999; *string; string++) {
		if (cfont->fcharoffset[*string] >= 0) {		/* defined ? */
			cdef = &(cfont->fchars[cfont->fcharoffset[*string]]);
			switch (tx->chattr.path) {
			case GTP_LEFT:
			case GTP_RIGHT:	fsize += xFxMax(cdef);
					fsize += cfont->fnominalx * wsr_spacing;
					break;
			case GTP_UP:
			case GTP_DOWN:	fsize += xFyMax(cdef) - xFyMin(cdef);
					       /* PTR# c1083     ^^^^^^^^^^  */
					fsize += cfont->fnominaly * wsr_spacing;
					if ((i=xFxMax(cdef)) > fwidth) fwidth=i;
					break;
			}
		}
	}


#ifdef TDEBUG
fprintf(stderr, "xtext 	fsize = %f fwidth=%f\n", fsize, fwidth);
#endif

	/* translate alignment point to origin which is currently at
	 * the LEFT BASE of the first character in the string.
	 */
	switch(tx->chattr.path) {
	default:						/* New 4.3 compiler*/
	case GTP_RIGHT:
		switch(tx->chattr.align.hor) {
		default:					/* New 4.3 compiler*/
		case GTH_NORMAL:
		case GTH_LEFT  :	xtrans = 0;		break;
		case GTH_CENTRE:	xtrans = - fsize / 2.0; break;
		case GTH_RIGHT :	xtrans = - fsize;	break;
		}
		switch(tx->chattr.align.ver) {
		case GTV_TOP:		ytrans = -cfont->ftop; 	 break;
		case GTV_CAP:		ytrans = -cfont->fcap; 	 break;
		case GTV_HALF:		ytrans = -cfont->fhalf;	 break;
		default:					/* New 4.3 compiler*/
		case GTV_NORMAL:
		case GTV_BASE:		ytrans = -cfont->fbase;	 break;
		case GTV_BOTTOM:	ytrans = -cfont->fbottom; break;
		}
		break;

	case GTP_LEFT:
		switch(tx->chattr.align.hor) {
		case GTH_LEFT:
			xtrans = fsize - xFxMax(&(cfont->fchars[cfont->fcharoffset[*(tx->string)]]));
			break;
		case GTH_CENTRE:
			xtrans = fsize / 2.0 - xFxMax(&(cfont->fchars[cfont->fcharoffset[*(tx->string)]]));
			break;
		default:					/* New 4.3 compiler*/
		case GTH_NORMAL:
		case GTH_RIGHT:
			xtrans = - xFxMax(&(cfont->fchars[cfont->fcharoffset[*(tx->string)]]));
			break;
		}
		switch(tx->chattr.align.ver) {
		case GTV_TOP:	ytrans = -cfont->ftop; 	 break;
		case GTV_CAP:	ytrans = -cfont->fcap; 	 break;
		case GTV_HALF:	ytrans = -cfont->fhalf;	 break;
		default:
		case GTV_NORMAL:
		case GTV_BASE:	ytrans = -cfont->fbase;	 break;
		case GTV_BOTTOM:	ytrans = -cfont->fbottom; break;
		}
		break;
	case GTP_UP:
		switch(tx->chattr.align.hor) {
		case GTH_LEFT:		xtrans = 0;		break;
		default:					/* New 4.3 compiler*/
		case GTH_NORMAL:
		case GTH_CENTRE:	xtrans = -fwidth / 2.0;	break;
		case GTH_RIGHT:		xtrans = -fwidth;	break;
		}
		switch(tx->chattr.align.ver) {
		case GTV_TOP:	ytrans = -fsize; 	 break;	/*c1083*/
		case GTV_CAP:	ytrans = -fsize - cfont->fnominaly * wsr_spacing;
								/*c1083*/
				break;
		case GTV_HALF:	ytrans = -fsize/2.0; 	 break;	/*c1083*/
		default:					/* New 4.3 compiler*/
		case GTV_NORMAL:
		case GTV_BASE:	ytrans = 0; 	 break;
		case GTV_BOTTOM:ytrans = - cfont->fbottom; break;
		/* Changed fsize to -fsize   PTR# c1083  */
		}
		break;

	case GTP_DOWN:
		switch(tx->chattr.align.hor) {
		default:					/* New 4.3 compiler*/
		case GTH_LEFT:		xtrans = 0;		break;
		case GTH_NORMAL:
		case GTH_CENTRE:	xtrans = -fwidth / 2.0;	break;
		case GTH_RIGHT:		xtrans = -fwidth;	break;
		}
		switch(tx->chattr.align.ver) {
		default:					/* New 4.3 compiler*/
		case GTV_NORMAL:
		case GTV_TOP:	ytrans = - cfont->ftop; break;
		case GTV_CAP:	ytrans = - cfont->fcap; 
				break;
		case GTV_HALF: 	ytrans = fsize/2.0 - xFyMax(&(cfont->fchars[cfont->fcharoffset[*(tx->string)]]));
				break;
		case GTV_BASE:	ytrans = fsize - xFyMax(&(cfont->fchars[cfont->fcharoffset[*(tx->string)]]));
				break;
		case GTV_BOTTOM:
			ytrans = fsize - xFyMax(&(cfont->fchars[cfont->fcharoffset[*(tx->string)]])) - cfont->fbottom;
			break;
		}
		break;
	}

#ifdef TDEBUG
fprintf(stderr, "ibmrt_ddgks:text: xtrans=%f ytrans=%f\n",xtrans, ytrans);
#endif

	xTransMatrix(stroke_trans, xtrans, ytrans); /*ILL2*//* c1171 */

#ifdef TDEBUG
fprintf(stderr, "ibmrt_ddgks:text: xtrans=%f ytrans=%f\n",xtrans, ytrans);
xDumpMatrix("stroke_trans", stroke_trans);	/*ILL2*/
#endif


/* set up x-system output attribute */
        xXgksSetTextClipRectangles(dpy, gc, ws,  &(ws->xclip));	/* AUG */ /*c1147*/
	xXgksSetForeground(dpy, gc, wsr_colour);		/* AUG */ /*c1147*/
	xXgksSetLineAttributes (dpy, gc, 0, LineSolid, CapButt, JoinMiter); /* AUG */ /*c1147*/
	xXgksSetFillStyle (dpy, gc, FillSolid);			/* AUG */ /*c1147*/

	for (text_ptr = tx->string; *text_ptr; text_ptr++) {
		xPlotStrokeChar(&(cfont->fchars[cfont->fcharoffset[*text_ptr]]));
		/* modify trans to move to LEFT BASE of next char */
		switch(tx->chattr.path) {
		default:					/* New 4.3 compiler*/
		case GTP_RIGHT:	xtrans = xFxMax(&(cfont->fchars[cfont->fcharoffset[*text_ptr]]))
					+ cfont->fnominalx * wsr_spacing;
				ytrans = 0;
				break;
		case GTP_LEFT:	xtrans = - xFxMax(&(cfont->fchars[cfont->fcharoffset[*(text_ptr+1)]]))
					- cfont->fnominalx * wsr_spacing;
				ytrans = 0;
				break;
		case GTP_UP:	ytrans = xFyMax(&(cfont->fchars[cfont->fcharoffset[*text_ptr]])) -
				xFyMin(&(cfont->fchars[cfont->fcharoffset[*(text_ptr+1)]]))
				/* Corrects y pos of char  PTR# c1083 */
					+ cfont->fnominaly * wsr_spacing;
				xtrans = 0;
				break;
		case GTP_DOWN:	ytrans = - xFyMax(&(cfont->fchars[cfont->fcharoffset[*(text_ptr+1)]]))  +
				xFyMin(&(cfont->fchars[cfont->fcharoffset[*(text_ptr)]]))
					- cfont->fnominaly * wsr_spacing;
				/* Corrects y pos of char  PTR# c1083 */
				xtrans = 0;
				break;
		}
		xTransMatrix(stroke_trans, xtrans, ytrans);/*ILL2*//* c1171 */
	}
	
	XgksSIGIO_ON(ws->dpy);	/* c1147 *//* d1 */
	return(OK);	/*MIT*/
}

/*
 * Vector Font Routines
 */

/*
 * xPlotStrokeChar(c) - utility function to plot a single stroke precision font character.
 *
 * char c;			the character to plot.
 *
 * returns nothing.
 */
static xPlotStrokeChar(cvp)
	struct vcharst *cvp;
{
	char LastType = 'm';
	int cnt;
	Gpoint tmp;
	XPoint *pt, pts[MAX_CHAR_STROKE];

	cnt = 0;
	pt = pts;
	tmp.x = (int)(stroke_trans[0]* 0 + stroke_trans[1]* 0 + stroke_trans[2] + 0.5); /*ILL2*/
    tmp.y = DCYMAX - (int)(stroke_trans[3]* 0 + stroke_trans[4]* 0 + stroke_trans[5] + 0.5); /*ILL2*/
	pt++; cnt++;

	while ( (cvp->vc_type != 'e') ) {	/* get the next stroke */
		if (cnt >= MAX_CHAR_STROKE) {
			XDrawLines( dpy, win, gc, pts, cnt, CoordModeOrigin);
			pts[0] = pts[--cnt];
			cnt=1;
		}
		switch(cvp->vc_type) {
		case 'm':
			if (LastType == 'd')
				XDrawLines( dpy, win, gc, pts, cnt, CoordModeOrigin);
			LastType = 'm';
			cnt=1;
			pt = pts;
			tmp.x = cvp->vc_x;				     /*ILL2*/
         		tmp.y = cvp->vc_y;				     /*ILL2*/
          		pt->x = (int) (stroke_trans[0]*tmp.x +
            		   stroke_trans[1]*tmp.y + stroke_trans[2] + 0.5);   /*ILL2*/
            		pt->y = (DCYMAX) - (int) (stroke_trans[3]*tmp.x
                	   + stroke_trans[4]*cvp->vc_y +stroke_trans[5]+0.5);/*ILL2*/
			pt++;
			break;
		case 'd':
			LastType = 'd';
			tmp.x = cvp->vc_x;				     /*ILL2*/
            		tmp.y = cvp->vc_y;				     /*ILL2*/
            		pt->x = (int) (stroke_trans[0]*tmp.x +
                	  stroke_trans[1]*tmp.y + stroke_trans[2] + 0.5);    /*ILL2*/
           		 pt->y = (DCYMAX) - (int) (stroke_trans[3]*tmp.x
                	  + stroke_trans[4]*cvp->vc_y + stroke_trans[5]+0.5);/*ILL2*/
			cnt++;
			pt++;
			break;
		default:
			break;
		}
		cvp++;
	}
	if (cnt > 1)
		XDrawLines( dpy, win, gc, pts, cnt, CoordModeOrigin);
	XFlush (dpy);
}
/*
 * xFxMax(struct vcharst *cvp)	returns width of char starting at vcp
 */
static Gfloat xFxMax(cvp)
	struct vcharst *cvp;
{
	while(cvp->vc_type != 'e') {
		switch(cvp->vc_type) {
		case 'S': return((Gfloat) cvp->vc_x);
		}
		cvp++;
	}
	return(0.0);	/* oops, should have found a 'S' record */
}

/*
 * xFyMin(struct vcharst *cvp)	returns depth from base of char starting at vcp
 *     added to correct y positioning of characters with descenders  PTR# c1083 
 */
static Gfloat xFyMin(cvp)
	struct vcharst *cvp;
{
        while(cvp->vc_type != 'e') {
		switch(cvp->vc_type) {
		case 's': return((Gfloat) cvp->vc_y);
		}
		cvp++;
	}
        return(0.0);    /* oops, should have found a 's' record */
}

/*
 * xFyMax(struct vcharst *cvp)	returns height from base of char starting at vcp
 */
static Gfloat xFyMax(cvp)
	struct vcharst *cvp;
{
	while(cvp->vc_type != 'e') {
		switch(cvp->vc_type) {
		case 'S': return((Gfloat) cvp->vc_y);
		}
		cvp++;
	}
	return(0.0);	/* oops, should have found a 'S' record */
}

/*
 * xReadFont(f)	Read font number f from the font database.
 */

static int xReadFont(f)
	int f;
{
	char fontpath[1000];
	int fd;
	int size;

	if (f<0 || f>DEFINED_FONT_TYPE+1)
		return(-1);

	if (FontTable[f].f_loaded == TRUE)	/* already loaded */
		return(0);

	/* xgks_state.fontdbdir is set in text.c to either */ /* c1159 */
        /*    FONTDBDIR set by Makefile or the environment */ /* c1159 */
        /*    variable XGKSFontDir      (DWO)              */ /* c1159 */
        sprintf(fontpath, "%s/%d", xgks_state.fontdbdir, f);  /* c1159 */
	if ( (fd = open(fontpath, O_RDONLY, 0644)) < 0) {
		close(fd);
		return(-1);
	}
	if (read(fd, &size, sizeof(int)) != sizeof(int)) {
		close(fd);
		return(-1);
	}
	if ( (FontTable[f].f_font = (FONT *) malloc( (unsigned) size)) == NULL) {
		close(fd);
		return(-1);
	}
	if (read(fd, FontTable[f].f_font, size) != size) {
		free(FontTable[f].f_font);
		close(fd);
		return(-1);
	}
	close(fd);
	FontTable[f].f_loaded = TRUE;
	return(0);
}

/*
 * Matrix operations:
 *
 * a matrix is an array of six REALs which specify the first two columns
 * of a 3x3 2D transformation matrix.
 */
 static xIdentityMatrix(m)				/* New 4.3 C compiler */
	Gfloat m[];
{
	m[0] = 1;	m[3] = 0;	/* 0 */
	m[1] = 0;	m[4] = 1;	/* 0 */
	m[2] = 0;	m[5] = 0;	/* 1 */
	return (OK);/* c1171 */
}

#ifdef TDEBUG		/*ILL2*/
static int xScaleMatrix(m, sx, sy)
	Gfloat m[];
	double sx, sy;
{
	Gfloat sm[6], newm[6];

	xIdentityMatrix(sm);
	sm[0] = sx; sm[4] = sy;
	xMultMatrix(m, sm, newm);	/*ILL*/
	xCopyMatrix(m, newm);
}

static int xRotateMatrix(m, axis, rad)
	Gfloat m[];
	int axis;
	double rad;
{
	Gfloat rm[6], newm[6];
	double c, s;

	c = cos(rad);
	s = sin(rad);

	xIdentityMatrix(rm);
	switch (axis) {
	case 'Z': rm[0] = c; rm[1] = -s; rm[3] = s; rm[4] = c;
		break;
	default:
		return;
	}
	xMultMatrix(m, rm, newm);	/*ILL*/
	xCopyMatrix(m, newm);
}
#endif		/*ILL2*/

static xTransMatrix(m, tx, ty)		/* New 4.3 C compiler */
	Gfloat m[];
	Gfloat tx, ty;/* c1171 */
{
	Gfloat tm[6], newm[6];

	xIdentityMatrix(tm);
	tm[2] = tx; tm[5] = ty;
	xMultMatrix(tm, m, newm);
	xCopyMatrix(m, newm);
	return (OK);/* c1171 */
}

static xCopyMatrix(dest, src)		/* New 4.3 C compiler */
	Gfloat dest[], src[];
{
	int i;

	for (i=0; i<6; i++)
		dest[i] = src[i];
	return(0);/* c1171 */
}

/*
 * matrix multiply: a * b = c
*/
static xMultMatrix(a, b, c)		/* New 4.3 C compiler */
	Gfloat a[], b[], c[];
{
	c[0] = a[0]*b[0] + a[3]*b[1] + 0*b[2];
	c[1] = a[1]*b[0] + a[4]*b[1] + 0*b[2];
	c[2] = a[2]*b[0] + a[5]*b[1] + 1*b[2];

	c[3] = a[0]*b[3] + a[3]*b[4] + 0*b[5];
	c[4] = a[1]*b[3] + a[4]*b[4] + 0*b[5];
	c[5] = a[2]*b[3] + a[5]*b[4] + 1*b[5];
	return(0);/* c1171 */
}

#ifdef TDEBUG		/*MIT*/
static int xDumpMatrix(s, m)
char *s;
Gfloat m[];
{
	int i;

	fprintf(stderr, "xtext:%s", s);
	for (i=0; i<6; i++) 
		fprintf(stderr,"	%10.5f",m[i]);
	fprintf(stderr,"\n");
}
#endif		/*MIT*/
static 						/* New 4.3 C compiler */
ck_ranges(cdef, Xmin, Xmax, Ymin, Ymax)
	struct vcharst *cdef;
	Gfloat *Xmin, *Xmax, *Ymin, *Ymax;
{
	Gint DoneBoth = 0;

	while(DoneBoth<2) {
		switch(cdef->vc_type) {
		case 's':
			if (cdef->vc_x < *Xmin)	*Xmin = cdef->vc_x;
			if (cdef->vc_y < *Ymin)	*Ymin = cdef->vc_y;
			DoneBoth++;
			break;
		case 'S':
			if (cdef->vc_x > *Xmax)	*Xmax = cdef->vc_x;
			if (cdef->vc_y > *Ymax)	*Ymax = cdef->vc_y;
			DoneBoth++;
			break;
		case 'e':
			DoneBoth += 2;
			break;
		}
		cdef++;
	}
        return(0);/* c1171 */
}

static 					/* New 4.3 C compiler */
ApplyMatrix(m, in, out)
	Gfloat m[];
	Gpoint *in, *out;
{
	out->x = m[0]*in->x + m[1]*in->y + m[2]*1.0;
	out->y = m[3]*in->x + m[4]*in->y + m[5]*1.0;
        return(0);/* c1171 */
}

/* 
 * xXgksInqTextExtent () -- X's text inquire extent routine
 *
 */
xXgksInqTextExtent (ws, tx, ndc_points)  /* c1147 */
WS_STATE_PTR ws;
TEXT_ST *tx;
Gpoint ndc_points[5];
{
	Gpoint start; 
	Gchar *string;
        Gpoint atsndc;          /*       ||||||||| */
        FONT *cfont;            /* c1171 VVVVVVVVV */
        Gfloat fsize, fwidth, xtrans=0.0, ytrans=0.0, xFyMin(), xFxMax(), xFyMax();
				   /* new xFyMin() funct added  PTR# c1083 */
	Gfloat Xmin, Xmax, Ymin, Ymax;
	Gfloat XminFirst, XmaxFirst, YminFirst, YmaxFirst;
	struct vcharst *cdef;
	Gint i;

	if (FirstTime == TRUE) {  	/* First Time initializing .. */
		for (i=0; i<DEFINED_FONT_TYPE; i++) FontTable[i].f_loaded = FALSE;
		FirstTime = FALSE;
	}

	xComputeText(ws, tx);

	cfont = FontTable[wsr_font].f_font;

/* compute transformation to move the whole string to the alignment point */
	/* transform the NDC start point to scaled NDC */
 	atsndc.x = (tx->location->x*ws->ndctoxtrans.xScale) + ws->ndctoxtrans.xTrans;
	atsndc.y = (tx->location->y*ws->ndctoxtrans.yScale) + ws->ndctoxtrans.yTrans;

	stroke_trans[2] = atsndc.x;		/*ILL2*/
        stroke_trans[5] = atsndc.y;		/*ILL2*/

	/* find total length or height of string + spacing */
	/* find minimum and maximum X & Y */
	Xmin = Ymin =  1500.0;
	Xmax = Ymax = -1500.0;
	for (string=tx->string, fsize=0.0, fwidth = -9999; *string; string++) {
		if (cfont->fcharoffset[*string] >= 0) {		/* defined ? */
			cdef = &(cfont->fchars[cfont->fcharoffset[*string]]);
			ck_ranges(cdef, &Xmin, &Xmax, &Ymin, &Ymax);
			switch (tx->chattr.path) {
			case GTP_LEFT:
			case GTP_RIGHT:	fsize += xFxMax(cdef);
					fsize += cfont->fnominalx * wsr_spacing;
					break;
			case GTP_UP:
			case GTP_DOWN:	fsize += xFyMax(cdef) - xFyMin(cdef);
					       /* PTR# c1083     ^^^^^^^^^^  */
					fsize += cfont->fnominaly * wsr_spacing;
					if ((i=xFxMax(cdef)) > fwidth) fwidth=i;
			/* This was xFyMax ------^ now fixed.  PTR# c1082*/
					break;
			}
		}
	}
	/* translate alignment point to origin which is currently at
	 * the LEFT BASE of the first character in the string.
	 */
	switch(tx->chattr.path) {
	default:						/* New 4.3 compiler*/
	case GTP_RIGHT:
		switch(tx->chattr.align.hor) {
		default:					/* New 4.3 compiler*/
		case GTH_NORMAL:
		case GTH_LEFT:		xtrans = 0;	break;
		case GTH_CENTRE:	xtrans = - fsize / 2.0; break;
		case GTH_RIGHT:		xtrans = - fsize;	break;
		}
		switch(tx->chattr.align.ver) {
		case GTV_TOP:	ytrans = -cfont->ftop; 	 break;
		case GTV_CAP:	ytrans = -cfont->fcap; 	 break;
		case GTV_HALF:	ytrans = -cfont->fhalf;	 break;
		default:					/* New 4.3 compiler*/
		case GTV_NORMAL:
		case GTV_BASE:	ytrans = -cfont->fbase;	 break;
		case GTV_BOTTOM:ytrans = -cfont->fbottom; break;
		}
		break;

	case GTP_LEFT:
		switch(tx->chattr.align.hor) {
		case GTH_LEFT:
			xtrans = fsize - xFxMax(&(cfont->fchars[cfont->fcharoffset[*(tx->string)]]));
			break;
		case GTH_CENTRE:
			xtrans = fsize / 2.0 - xFxMax(&(cfont->fchars[cfont->fcharoffset[*(tx->string)]]));
			break;
		default:					/* New 4.3 compiler*/
		case GTH_NORMAL:
		case GTH_RIGHT:
			xtrans = - xFxMax(&(cfont->fchars[cfont->fcharoffset[*(tx->string)]]));
			break;
		}
		switch(tx->chattr.align.ver) {
		case GTV_TOP:	ytrans = -cfont->ftop; 	 break;
		case GTV_CAP:	ytrans = -cfont->fcap; 	 break;
		case GTV_HALF:	ytrans = -cfont->fhalf;	 break;
		default:					/* New 4.3 compiler*/
		case GTV_NORMAL:
		case GTV_BASE:	ytrans = -cfont->fbase;	 break;
		case GTV_BOTTOM:ytrans = -cfont->fbottom; break;
		}
		break;
	case GTP_UP:
		switch(tx->chattr.align.hor) {
		case GTH_LEFT:	xtrans = 0;		break;
		default:					/* New 4.3 compiler*/
		case GTH_NORMAL:
		case GTH_CENTRE:	xtrans = -fwidth / 2.0;	break;
		case GTH_RIGHT:		xtrans = -fwidth;	break;
		}
		switch(tx->chattr.align.ver) {
		case GTV_TOP:	ytrans = -fsize; 	 break;		/*c1083*/
		case GTV_CAP:	ytrans = -fsize - cfont->fnominaly * wsr_spacing;/*c1083*/
				break;
		case GTV_HALF:	ytrans = -fsize/2.0; 	 break;		/*c1083*/
		default:					/* New 4.3 compiler*/
		case GTV_NORMAL:
		case GTV_BASE:	ytrans = 0; 	 break;
		case GTV_BOTTOM:ytrans = - cfont->fbottom; break;
		/* Changed fsize to -fsize   PTR# c1083  */
		}
		break;

	case GTP_DOWN:
		switch(tx->chattr.align.hor) {
		case GTH_LEFT:		xtrans = 0;		break;
		default:					/* New 4.3 compiler*/
		case GTH_NORMAL:
		case GTH_CENTRE:	xtrans = -fwidth / 2.0;	break;
		case GTH_RIGHT:		xtrans = -fwidth;	break;
		}
		switch(tx->chattr.align.ver) {
		default:					/* New 4.3 compiler*/
		case GTV_NORMAL:
		case GTV_TOP:	ytrans = - cfont->ftop; break;
		case GTV_CAP:	ytrans = - cfont->fcap; 
				break;
		case GTV_HALF: 	ytrans = fsize/2.0 - xFyMax(&(cfont->fchars[cfont->fcharoffset[*(tx->string)]]));
				break;
		case GTV_BASE:	ytrans = fsize - xFyMax(&(cfont->fchars[cfont->fcharoffset[*(tx->string)]]));
				break;
		case GTV_BOTTOM:
			ytrans = fsize - xFyMax(&(cfont->fchars[cfont->fcharoffset[*(tx->string)]])) - cfont->fbottom;
				break;
		}
		break;
	}

	xTransMatrix(stroke_trans, xtrans, ytrans); /*ILL2*//* c1171 */

#ifdef TDEBUG
	fprintf(stderr, "ibmrt_ddgks:text: xtrans=%f ytrans=%f\n",xtrans, ytrans);
	DumpMatrix("stroke_trans", stroke_trans);	/*ILL2*/
#endif

	/* find minimum and maximum X & Y of First Character */
	XminFirst = YminFirst = 9999.0;
	XmaxFirst = YmaxFirst = -9999.0;
	cdef = &(cfont->fchars[cfont->fcharoffset[*(tx->string)]]);
	ck_ranges(cdef, &XminFirst, &XmaxFirst, &YminFirst, &YmaxFirst);
#ifdef TDEBUG
	fprintf(stderr, "ibmrt_ddgks:i_text_extent:First min %f %f max %f %f\n",
		XminFirst, YminFirst, XmaxFirst, YmaxFirst);
#endif

/* we know all we need to know so send the bounding box through the trans */

	switch(tx->chattr.path) {
	default:						/* New 4.3 compiler*/
	case GTP_RIGHT:
		ndc_points[1].x = XminFirst; ndc_points[1].y = Ymin;
		ndc_points[2].x = fsize;     ndc_points[2].y = Ymin;
		ndc_points[3].x = fsize;     ndc_points[3].y = Ymax;
		ndc_points[4].x = XminFirst; ndc_points[4].y = Ymax;
		switch(tx->chattr.align.hor) {
		default:					/* New 4.3 compiler*/
		case GTH_NORMAL:
		case GTH_LEFT: 	 ndc_points[0].x = fsize;	break;
		case GTH_CENTRE: ndc_points[0].x = (fsize-XminFirst) /2; break;
		/* when align is CENTRE define concat pt as start pt PTR#c1083*/
		case GTH_RIGHT:  ndc_points[0].x = XminFirst; break;
		}
		switch(tx->chattr.align.ver) {
		case GTV_TOP: ndc_points[0].y = cfont->ftop; break;
		case GTV_CAP: ndc_points[0].y = cfont->fcap; break;
		case GTV_HALF:ndc_points[0].y = cfont->fhalf; break;
		default:					/* New 4.3 compiler*/
		case GTV_NORMAL:
		case GTV_BASE:  ndc_points[0].y = cfont->fbase; break;
		case GTV_BOTTOM:ndc_points[0].y = cfont->fbottom; break;
		}
		break;
	case GTP_LEFT:
		ndc_points[1].x = -fsize + XmaxFirst; ndc_points[1].y = Ymin;
		ndc_points[2].x = XmaxFirst; ndc_points[2].y = Ymin;
		ndc_points[3].x = XmaxFirst; ndc_points[3].y = Ymax;
		ndc_points[4].x = -fsize + XmaxFirst; ndc_points[4].y = Ymax;
		switch(tx->chattr.align.hor) {
		case GTH_LEFT: ndc_points[0].x = XmaxFirst; break;
		case GTH_CENTRE: ndc_points[0].x = XmaxFirst - fsize/2; break;
		/* when align is CENTRE define concat pt as start pt PTR#c1083*/
		default:					/* New 4.3 compiler*/
		case GTH_NORMAL:
		case GTH_RIGHT: ndc_points[0].x = -fsize + XmaxFirst; break;
		}
		switch(tx->chattr.align.ver) {
		case GTV_TOP: ndc_points[0].y = cfont->ftop; break;
		case GTV_CAP: ndc_points[0].y = cfont->fcap; break;
		case GTV_HALF: ndc_points[0].y = cfont->fhalf; break;
		default:					/* New 4.3 compiler*/
		case GTV_NORMAL:
		case GTV_BASE: ndc_points[0].y = cfont->fbase; break;
		case GTV_BOTTOM: ndc_points[0].y = cfont->fbottom; break;
		}
		break;
	case GTP_UP:
		ndc_points[1].x = Xmin; ndc_points[1].y = YminFirst;
		ndc_points[2].x = Xmax; ndc_points[2].y = YminFirst;
		ndc_points[3].x = Xmax; ndc_points[3].y = fsize;
		ndc_points[4].x = Xmin; ndc_points[4].y = fsize;
		switch(tx->chattr.align.hor) {
		case GTH_LEFT: ndc_points[0].x = Xmin; break;
		default:					/* New 4.3 compiler*/
		case GTH_NORMAL:
		case GTH_CENTRE: ndc_points[0].x = (Xmax-Xmin)/2.0; break;
		case GTH_RIGHT:  ndc_points[0].x = Xmax; break;
		}
		switch(tx->chattr.align.ver) {
		case GTV_TOP: ndc_points[0].y = YminFirst; break;
		case GTV_CAP: ndc_points[0].y = YminFirst; break;
		case GTV_HALF: ndc_points[0].y = fsize/2; break;
		/* when align is HALF define concat pt as start pt PTR#c1083*/
		default:					/* New 4.3 compiler*/
		case GTV_NORMAL:
		case GTV_BASE:ndc_points[0].y = fsize; break;
		case GTV_BOTTOM: ndc_points[0].y = fsize; break;
		}
		break;
	case GTP_DOWN:
		ndc_points[1].x = Xmin; ndc_points[1].y = -fsize + YmaxFirst;
		ndc_points[2].x = Xmax; ndc_points[2].y = -fsize + YmaxFirst;
		ndc_points[3].x = Xmax; ndc_points[3].y = YmaxFirst;
		ndc_points[4].x = Xmin; ndc_points[4].y = YmaxFirst;
		switch(tx->chattr.align.hor) {
		case GTH_LEFT: ndc_points[0].x = Xmin; break;
		default:					/* New 4.3 compiler*/
		case GTH_NORMAL:
		case GTH_CENTRE: ndc_points[0].x = (Xmax-Xmin)/2.0; break;
		case GTH_RIGHT: ndc_points[0].x = Xmax; break;
		}
		switch(tx->chattr.align.ver) {
		default:					/* New 4.3 compiler*/
		case GTV_NORMAL:					/*c1083*/
		case GTV_TOP: ndc_points[0].y = -fsize + YmaxFirst; break;
		case GTV_CAP: ndc_points[0].y = -fsize + YmaxFirst; break;
		case GTV_HALF: ndc_points[0].y = YmaxFirst - fsize/2; break;
		/* when align is HALF define concat pt as start pt PTR#c1083*/
		case GTV_BASE: ndc_points[0].y = YmaxFirst; break;
		case GTV_BOTTOM: ndc_points[0].y = YmaxFirst; break;
		}
		break;
	}

/* send the points through the text alignment matrix to get device coordinates
 * and the inverse of the workstation transformation to get NDC coordinates
 */
	for (i=0; i<5; i++) {
		ApplyMatrix(stroke_trans, &(ndc_points[i]), &start);	/*ILL2*/
	        ndc_points[i].x = (start.x - ws->ndctoxtrans.xTrans) / ws->ndctoxtrans.xScale; 
		ndc_points[i].y = (start.y - ws->ndctoxtrans.yTrans) / ws->ndctoxtrans.yScale;
	}

	return(OK);
}


#define MAX(a, b) (((a)>(b))?(a):(b))
xXgksMesg (ws, mesg)  /* c1147 */
WS_STATE_PTR ws;
MESG_ST *mesg;
{
	
	Display *dpy;
	Window  win;
	GC	gc;
	Gpoint  dc_pos;
        int height, width, tempwidth, topheight;	/*ILL*/
	XPoint  x_pos;
	XRectangle x_clip;
        XFontStruct  *MFontInfo;
	if (ws->ewstype != X_WIN) return(OK);		/*MIT*/

	XgksSIGIO_OFF(ws->dpy); /* c1147 *//* d1 */

	dpy = ws->dpy;
	win = ws->win;
	gc  = ws->gc;

	dc_pos.x = 150.0;
	dc_pos.y = 200.0;
	DcToX ( ws, &dc_pos, &x_pos);

	x_clip.x = 0;
	x_clip.y = 0;
	x_clip.width  = ws->wbound.x;
	x_clip.height = ws->wbound.y;

        /* must clear a rectangle for the message */
        MFontInfo=XLoadQueryFont(dpy, "6x10");	/* c1136 */
	XSetFont(dpy, gc, MFontInfo->fid); 	/* PTR# c1136 */
        tempwidth=XTextWidth(MFontInfo, mesg->string, STRLEN(mesg->string));	/* c1176 */
        width=MAX( tempwidth, ws->last_message_width);
        ws->last_message_width=tempwidth;
        height=MFontInfo->ascent+MFontInfo->descent;
	topheight=MFontInfo->ascent;		/*ILL*/
        XSetForeground(dpy, gc, ws->wsbg);
     XFillRectangle(dpy, win, gc, x_pos.x, x_pos.y-topheight, width, height); /*ILL*/
	/*XFillRectangle(dpy, win, gc, x_pos.x, x_pos.y - height + MFontInfo->descent, width, height);*/
                          /* the y position needed correction - PTR# c1001 */

	if ( ws->wscolour == 2 )		/* monochrome ? */
		XSetForeground(dpy, gc, ws->wsfg );
	else
		XSetForeground(dpy, gc, 1 );

	/* the fill style attribute */
	XSetClipRectangles (dpy, gc, 0, 0, &x_clip, 1, Unsorted);
	XDrawString (dpy, win, gc, x_pos.x, x_pos.y, mesg->string, STRLEN(mesg->string));/* c1176 */

	XgksSIGIO_ON(ws->dpy);	/* c1147 *//* d1 */

	return(OK);	/*MIT*/
}

