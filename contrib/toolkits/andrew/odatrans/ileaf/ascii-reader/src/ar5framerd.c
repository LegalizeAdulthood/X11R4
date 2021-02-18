
/*
 * Copyright 1989 by Interleaf, Inc, 10 Canal Park,  Cambridge, MA. 02141
 *
 *		All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Interleaf not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Interleaf, Inc. makes no representations 
 * about the suitability of this software for any purpose. 
 * It is provided "AS IS" without express or implied warranty.
 *
 * INTERLEAF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL INTERLEAF BE LIABLE FOR ANY SPECIAL, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * 
 */

/*************************************************/
/*	ar5frameread.c                           */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  07/88    BT	Original AR version, derived from alMain.c
 */

#ifdef INTERLEAF_INCLUDE_FILES
#include "cdefs.h"
#include "sysstdio.h"
#else
#ifndef AR_DEFINES
#define AR_DEFINES
typedef short bool;
typedef short rsu;
typedef long Rsu;
typedef short shar;
typedef unsigned short ushar;
#endif
#endif
#include "ar5defs.h"
#include "ar5private.h"
/**--------------------------------------------------------------------------**/
#define HORIZ_ALIGN		1
#define VERT_ALIGN		2
#define FRMHEIGHT		3
#define PLACEMENT		4
#define SAMEPAGE		5	
#define FRMWIDTH		6
#define AL_FR_PAGENO		7
#define FR_NAME			8
#define FR_SHARE		9
#define FRMDIAG			10
#define FR_STRADDLE		11
#define CONTENTS_TO_FR_WIDTH	12
#define CONTENTS_TO_FR_HEIGHT	13
#define AUTO_EDIT		14
#define PROPORTIONAL		15
#define FR_RULING   		16	/* temporary */
#define FR_TINT   		17	/* temporary */
#define FR_REPEATING		18
#define FR_REPEAT_SMPG  	19
#define FR_COL_RULE_COLOR	20
#define FR_COL_RULE_PATTERN	21
#define FR_COL_RULE_WIDTH	22
#define FR_ANCHOR_VIS		23
#define FR_HORIZ_REF		24
#define FR_VERT_REF		25
#define NUMBERED		26
#define SUPERSCRIPT		27
#define FR_COL_RULE_VIS		28
#define FRMHIDDEN		29	/** added for dmg 10/4/88 BT **/

struct keytab al_frame_tokens[] = {  /* this list must remain sorted */
    {"Alignment", HORIZ_ALIGN},
    {"Anchor Visible", FR_ANCHOR_VIS},
    {"Auto Edit", AUTO_EDIT},
    {"Baseline", VERT_ALIGN},
    {"Begin on Anchor Page", FR_REPEAT_SMPG},
    {"Diagram", FRMDIAG},
    {"End on Anchor Page", FR_REPEAT_SMPG},
    {"Height", FRMHEIGHT},
    {"Hidden", FRMHIDDEN},
    {"Horizontal Alignment", HORIZ_ALIGN},
    {"Horizontal Reference", FR_HORIZ_REF},
    {"Name", FR_NAME},
    {"Numbered", NUMBERED},
    {"On Anchor Page", FR_REPEAT_SMPG},
    {"Page #", AL_FR_PAGENO},
    {"Placement", PLACEMENT},
    {"Proportional", PROPORTIONAL},
    {"Repeating", FR_REPEATING},
    {"Ruling", FR_RULING},
    {"Ruling Color", FR_COL_RULE_COLOR},
    {"Ruling Pattern", FR_COL_RULE_PATTERN},
    {"Rulings", FR_RULING},			/* note sort order */
    {"Ruling Visible", FR_COL_RULE_VIS},
    {"Ruling Weight", FR_COL_RULE_WIDTH},
    {"Ruling Width", FR_COL_RULE_WIDTH},
    {"Same Column as Anchor", SAMEPAGE},
    {"Same Page", SAMEPAGE},
    {"Same Page as Anchor", SAMEPAGE},
    {"Shared", FR_SHARE},
    {"Shared Contents", FR_SHARE},
    {"Size Contents to Height", CONTENTS_TO_FR_HEIGHT},
    {"Size Contents to Width", CONTENTS_TO_FR_WIDTH},
    {"Straddle", FR_STRADDLE},
    {"Superscript", SUPERSCRIPT},
    {"Tint", FR_TINT},
    {"Vertical Alignment", VERT_ALIGN},
    {"Vertical Reference", FR_VERT_REF},
    {"Visible", FR_ANCHOR_VIS},
    {"Width", FRMWIDTH}
};

/* Most often, these tokens are synonymous with the values stored in the 
 * flags field of its data structure.  they can be directly assigned into
 * the field.	BT
 */
struct keytab placement_tokens[] = {  /* this list must remain sorted */
    {"At Anchor",		fr_FIXEDPLACE},
    {"Bottom", 			fr_BOTTOM},
    {"Bottom of Page", 		fr_BOTTOM},
    {"Fixed", 			fr_FIXEDPLACE},
    {"Following", 		fr_FOLLOW_ANCHOR},
    {"Following Anchor",	fr_FOLLOW_ANCHOR},
    {"Following Text", 		fr_FOLLOW_TEXT},
    {"Inline", 			fr_FIXEDPLACE},
    {"Middle", 			fr_MIDDLE}, /* obsolete */
    {"Middle of Page", 		fr_MIDDLE}, /* obsolete */
    {"Overlay", 		fr_OVERLAY},
    {"Top", 			fr_TOP},
    {"Top of Page", 		fr_TOP},
    {"Underlay", 		fr_UNDERLAY}
};
#define NUMPLTS (sizeof(placement_tokens) / sizeof(struct keytab))

struct keytab al_frm_halign_names[] = {   /* this list must remain sorted! */
    {"Center",			fr_H_CENTER},
    {"Centered",		fr_H_CENTER},
    {"Inner",			fr_H_INNER},
    {"Left",			fr_H_LEFT},
    {"Outer",			fr_H_OUTER},
    {"Right",			fr_H_RIGHT}
};
#define NFHNAMES (sizeof(al_frm_halign_names) / sizeof(struct keytab))

struct keytab al_frm_valign_names[] = {   /* this list must remain sorted! */
    {"Baseline",		fr_V_BASELN},
    {"Bottom",			fr_V_BOTTOM},
    {"Center",			fr_V_CENTER},
    {"Centered",		fr_V_CENTER},
    {"Top",			fr_V_TOP}
};
#define NFVNAMES (sizeof(al_frm_valign_names) / sizeof(struct keytab))

struct keytab al_frm_wid_names[] = { 	  /* this list must remain sorted! */
    {"Column", 			ASZWCOL},
    {"Contents", 		ASZINX},
    {"Gutter", 			ASZWGUTTER},
    {"Inner Page Margin", 	ASZWIMARG},
    {"Left Page Margin", 	ASZWLMARG},
    {"Outer Page Margin", 	ASZWOMARG},
    {"Page", 			ASZWPAG},
    {"Page With Both Margins", 	ASZWPAGM},
    {"Page With Inner Margin", 	ASZWPAGI},
    {"Page With Left Margin", 	ASZWPAGL},
    {"Page With Outer Margin", 	ASZWPAGO},
    {"Page Without Margins", 	ASZWPAG},
    {"Page With Right Margin", 	ASZWPAGR},
    {"Right Page Margin", 	ASZWRMARG}
};
#define NFWIDNAMES (sizeof(al_frm_wid_names) / sizeof(struct keytab))

struct keytab al_frm_hgt_names[] = {   /* this list must remain sorted! */
    {"Bottom Page Margin", 	ASZHBMARG},
    {"Contents", 		ASZINY},
    {"Page", 			ASZHPAG},
    {"Page With Both Margins", 	ASZHPAGM},
    {"Page With Bottom Margin", ASZHPAGB},
    {"Page Without Margins", 	ASZHPAG},
    {"Page With Top Margin", 	ASZHPAGT},
    {"Top Page Margin", 	ASZHTMARG}
};
#define NFHGTNAMES (sizeof(al_frm_hgt_names) / sizeof(struct keytab))

struct keytab al_frm_href_names[] = { 	  /* this list must remain sorted! */
    {"Anchor", 			fr_X_ANCHOR},
    {"Column", 			fr_X_COLUMN},
    {"Inner Page Margin", 	fr_X_MARGINNER},
    {"Left Gutter", 		fr_X_GUTTLEFT},
    {"Left Page Margin", 	fr_X_MARGLEFT},
    {"Outer Page Margin", 	fr_X_MARGOUTER},
    {"Page With Both Margins",	fr_X_PAGEBOTH},
    {"Page Without Margins", 	fr_X_PAGENONE},
    {"Right Gutter", 		fr_X_GUTTRIGHT},
    {"Right Page Margin", 	fr_X_MARGRIGHT}
};
#define NFHREFNAMES (sizeof(al_frm_href_names) / sizeof(struct keytab))

struct keytab al_frm_vref_names[] = {   /* this list must remain sorted! */
    {"Anchor", 			fr_Y_ANCHOR},
    {"Bottom Page Margin", 	fr_Y_MARGBOTTOM},
    {"Page With Both Margins", 	fr_Y_PAGEBOTH},
    {"Page Without Margins", 	fr_Y_PAGENONE},
    {"Top Page Margin", 	fr_Y_MARGTOP}
};
#define NFVREFNAMES (sizeof(al_frm_vref_names) / sizeof(struct keytab))

struct keytab al_frm_repeat_names[] = {
    {"Begin",			fr_REP_BEGIN},
    {"End",			fr_REP_END},
    {"Finish",			fr_REP_END},
    {"No",			fr_REP_NONE},
    {"Start",			fr_REP_BEGIN},
    {"Yes",			fr_REP_BEGIN}
};
#define NFREPNAMES (sizeof(al_frm_repeat_names) / sizeof(struct keytab))

/**--------------------------------------------------------------------------**/
Frame_t *ArBuildFrame(fiP, isMaster, isCell)
  register File_info_t *fiP;
  bool isMaster;
  bool isCell;
{
  Frame_t *frP = NEW(Frame_t);
  ArDgHead_t *diagramP = NULL;
  int c;
  register short token;
  int units;
  float measure;
  short temp_short;
  int temp_int;
  bool frame_a_was_dumped = FALSE;
  bool negative;
  int straddle;
  Table_rule_t *cur_ruleP = NULL;
  bool super_set = FALSE, numbered_set = FALSE;
  void ArFrameAutoHeight();
  void ArFrameAutoWidth();


    if(al_micro) {
	ArTellError(ERR_MICRO_FRAME);
	free(frP);
	ArSkipFrame(fiP);
	return( NULL );
    }
    else if (isCell == YES) {
	*frP            = default_fr_cell;
	 frP->autosizeP = NEW(Autosize_t);
	*frP->autosizeP = default_fr_cell_autosize;
	 frP->rulingsP  = NEW(TL_ruling_t);
	*frP->rulingsP  = default_TL_ruling;
    }
    else {
	*frP            = default_frame;
	frP->id         = (isMaster? ID_MasterFrame : ID_Frame);
	frP->flags.cell = NO;
    }

    if (isMaster == NO && isCell == NO) {
	/** a frame instance needs to gobble ','  **/
	ArSkipPastComma(fiP);	/** should be right on it. **/
    }
    while((c = ArGetchNW(fiP)) != AR_CMD_TERM && c != EOF) {
	if (c == '@') {		/* indicates a frame attribute */
	    arParseUserAttr(fiP, &frP->attrvalsP);
	    continue;
	}
	ArGetWordShrC(fiP, c, al_token_shr, AR_MAX_WORD-1, 0);
	if((c = ArGetchNW(fiP)) != '=') {
	    ArUnGetch(c, fiP);
	    ArTellError(ERR_MISS_EQ);
	    ArSkipPastComma(fiP);
	    continue;
	}
	switch(ArLRTBPrefix(fiP)) {
	case LD_BOTTOM:
	case LD_RIGHT:
	    /* ENHANCE error message */
	default:
	case LD_TOP:
	    cur_ruleP = &frP->rulingsP->top;
	    break;
	case LD_LEFT:
	    cur_ruleP = &frP->rulingsP->left;
	    break;
	}

        switch(ArTokenShr(fiP, al_token_shr, al_frame_tokens,
			  NUM_OF(al_frame_tokens), LOOSE)) {
	case NO_EQUALS:
	    break;
	case FR_NAME:
	    ArGetWordShrC(fiP, NOCHAR, frP->name, AR_MAX_FRAMENAME-1, COMMA);
	    break;

	case PLACEMENT:	    /* this should be first on input */
	    token = ArGetTokenComma(fiP, placement_tokens, NUMPLTS);
	    if (token  == -1) {
		ArTellError(ERR_PLACEMENT);
		break;
	    }
	    frP->flags.placement = token;
	    frP->flags.fixed = (token == fr_FIXEDPLACE)? fr_FIXED : fr_FLOAT;
/** should either set frP->flags.z_locate or else it is redundant and should
    be removed from ar5defs.h and all places where it is used/initialized.
**/
	    break;

	case HORIZ_ALIGN:
	    frP->xofs = 0;
	    if (ArGetFloat(fiP, &measure)) {
		if ((units = ArGetUnitMeas(fiP)) != -1 && units != LINES)
		    frP->xofs = ArCvtUnits(units, measure);
		else
		    ArTellError(ERR_RSU_UNITS);
		frP->flags.h_align = fr_H_LEFT;
	    }
	    else {
		token = ArGetTokenAlpha(fiP, al_frm_halign_names, NFHNAMES);
		if (token == -1) {
		    ArTellError(ERR_FRM_HALIGN);
		    frP->flags.h_align = fr_H_CENTER;
		}
		else {
		    frP->flags.h_align = token;
		    negative = FALSE;

		    switch (c = ArGetchNW(fiP)) {
		    case '-':
			negative = TRUE;
			/* FALL THROUGH */
		    case '+':
			if (ArGetRsu(fiP, &temp_int))
			    frP->xofs = negative ? -temp_int : temp_int;
			else {
			    ArTellError(ERR_FRM_HALIGN);
			    ArSkipPastComma(fiP);
			}
			break;
		    case ',':
			break;
		    case '>':
			ArUnGetch(c, fiP);
			break;
		    default:
			ArTellError(ERR_EXPECTED_COMMA);
			ArSkipPastComma(fiP);
			break;
		    }
		}
	    }
#if 0	/** BT **/
	    if (!(frP->etype & EFFLOAT) && !(frP->etype3 & EF_ZMASK)) {
		ArTellError(ERR_HALIGN_FIXED);
		break;
	    }
#endif	/** BT **/
	    break;

	case VERT_ALIGN:
	    frP->yofs = frP->a = 0;

	    if (ArGetFloat(fiP, &measure)) {
		frame_a_was_dumped = TRUE;
		units = ArGetUnitMeas(fiP);
		if (units != -1 && units != LINES)
		    frP->a = ArCvtUnits(units, measure);
		else
		    ArTellError(ERR_RSU_UNITS);
		frP->flags.v_align = fr_V_BOTTOM;
	    }
	    else {
		token = ArGetTokenAlpha(fiP, al_frm_valign_names, NFVNAMES);
		if (token == -1) {
		    ArTellError(ERR_FRM_VALIGN);
		    frP->flags.v_align = fr_V_TOP;
		}
		else {
		    frP->flags.v_align = token;

		    switch (c = ArGetchNW(fiP)) {
		    case '-':
			ArUnGetch(c, fiP);
			/* FALL THROUGH */
		    case '+':
			if (ArGetRsu(fiP, &temp_int))
			    frP->yofs = temp_int;
			else {
			    ArTellError(ERR_FRM_VALIGN);
			    ArSkipPastComma(fiP);
			}
			break;
		    case ',':
			break;
		    case '>':
			ArUnGetch(c, fiP);
			break;
		    default:
			ArTellError(ERR_EXPECTED_COMMA);
			ArSkipPastComma(fiP);
			break;
		    }
		}
	    }
#if 0	/** BT **/
	    if (frP->etype & EFFLOAT && !(frP->etype3 & EF_ZMASK)) {
		ArTellError(ERR_VALIGN_FLOAT);
		break;
	    }
#endif	/** BT **/
	    break;

	case FR_HORIZ_REF:
	    token = ArGetTokenComma(fiP, al_frm_href_names, NFHREFNAMES);
	    if (token == -1)
		ArTellError(ERR_FRM_HREF);
	    else {
		frP->flags.x_locate = token;
	    }
	    break;

	case FR_VERT_REF:
	    token = ArGetTokenComma(fiP, al_frm_vref_names, NFVREFNAMES);
	    if (token == -1)
		ArTellError(ERR_FRM_VREF);
	    else {
		frP->flags.y_locate = token;
	    }
	    break;

	case FRMWIDTH:
#if 0	/** BT **/
	    if (frP->autosz)
		frP->autosz->flags &= ~(ASZWMSK | ASZINX);
#endif	/** BT **/
	    if((token = ArGetTokenAlpha(fiP,al_frm_wid_names,NFWIDNAMES))== -1){
		ArGetPZRsu(fiP, &frP->width);
		if(frP->autosizeP)
		  frP->autosizeP->frame_width = ASZWFIX;
	    } else {
		if (frP->autosizeP == NULL) {
		    frP->autosizeP  = NEW(Autosize_t);
		    *frP->autosizeP = default_fr_autosize;
		}
		ArFrameAutoWidth(fiP, frP->autosizeP, token);
	    }
	    /* width is set down below for table cells */
	    break;

	case FRMHEIGHT:
#if 0	/** BT **/
	    if (frP->autosz)
		frP->autosz->flags &= ~(ASZHMSK | ASZINY);
#endif	/** BT **/
	    if((token = ArGetTokenAlpha(fiP,al_frm_hgt_names,NFHGTNAMES))== -1){
		ArGetPZRsu(fiP, &frP->height);
		if(frP->autosizeP)
		  frP->autosizeP->frame_height = ASZHFIX;
	    } else {
		if (frP->autosizeP == NULL) {
		    frP->autosizeP  = NEW(Autosize_t);
		    *frP->autosizeP = default_fr_autosize;
		}
		ArFrameAutoHeight(fiP, frP->autosizeP, token);
	    }
	    break;

	case CONTENTS_TO_FR_WIDTH:
	    temp_short = FALSE;
	    ArGetYesNoShort(fiP, &temp_short);
	    if (temp_short) {
		frP->flags.cont_width = YES;
#ifdef NEVER_MS /* MS 1 May 89 */
		if (frP->autosizeP == NULL)
		    frP->autosizeP = NEW(Autosize_t);
		if (frP->autosizeP->frame_width == ASZWCOL) {
		    frP->autosizeP->frame_width = ASZEXXCOL;
		}
		else {
		    frP->autosizeP->frame_width = ASZEXX;
		}
#endif		
	    }else{
		frP->flags.cont_width = NO;
	    }
#ifdef NEVER_MS    
	    else if (frP->autosizeP != NULL) { 
	      frP->autosizeP->frame_width = ASZNOX; 
	    }
#endif	    
	    break;

	case CONTENTS_TO_FR_HEIGHT:
	    temp_short = FALSE;
	    ArGetYesNoShort(fiP, &temp_short);
	    if (temp_short) {
		frP->flags.cont_height = YES;
#ifdef NEVER_MS
		if (frP->autosizeP == NULL) {
		    frP->autosizeP = NEW(Autosize_t);
		    *frP->autosizeP = default_fr_autosize;
		}
		frP->autosizeP->frame_height = ASZEXY;
#endif		
	    }else{
		frP->flags.cont_height = NO;
	    }
#ifdef NEVER_MS    
	    else if (frP->autosizeP != NULL) { 
	      frP->autosizeP->frame_height = ASZNOY; 
	    }
#endif	    
	    break;

	case SAMEPAGE:
	    temp_short = 0;
    	    ArGetYesNoShort(fiP, &temp_short);
	    if(frP->flags.fixed == fr_FLOAT) {
		frP->flags.same_page = temp_short;
	    }
	    break;
	case AL_FR_PAGENO:  /* TPS discards this.  AR preserves it. */
	    c = ArGetchNW(fiP);
	    ArGetWordShrC(fiP, c, al_token_shr, AR_MAX_QSTR-1, COMMA);
	    frP->page_numP = 
		(shar *)ArSafeMalloc(sizeof(shar)*(1+ArShrlen(al_token_shr)));
	    ArShrcpy(frP->page_numP, al_token_shr);
	    break;
	case FRMHIDDEN:
	    temp_short = NO;
	    ArGetYesNoShort(fiP, &temp_short);
	    frP->flags.hidden = temp_short;
	    break;
	case FR_SHARE:
	    temp_short = NO;
	    ArGetYesNoShort(fiP, &temp_short);
	    frP->flags.shared = temp_short;
	    break;
	case FR_REPEATING:
	    token = ArGetTokenComma(fiP, al_frm_repeat_names, NFREPNAMES);
	    if (token == -1) {
		ArTellError(ERR_FRM_REPEAT);
		ArSkipPastComma(fiP);
	    }
	    else {
		frP->flags.repeat_info = token;
	    }
	    break;
	case FR_REPEAT_SMPG:
	    temp_short = FALSE;
	    ArGetYesNoShort(fiP, &temp_short);
	    frP->flags.repeat_same_page = temp_short;
	    break;
	case FR_ANCHOR_VIS:
	    temp_short = FALSE;
	    ArGetYesNoShort(fiP, &temp_short);
	    frP->flags.show_anchor = temp_short;
	    break;
	case PROPORTIONAL:
	    temp_short = FALSE;
	    ArGetYesNoShort(fiP, &temp_short);
	    frP->flags.proportional = temp_short;
	    break;
	case FRMDIAG:
	    /* Before ASCII version 5.1, frames looked like:
	     *	    <Frame, ...>V4(gx, )<End Frame>
	     * now they look like:
	     *	    <Frame, ..., Diagram = V4(gx...)>
	     * This was changed because the diagram is missing when
	     * shared, and to improve error recovery etc.
	     */
	    /* Apply effectivity before reading in the dfrm. */

	    if ((diagramP = ArSaveDg(fiP)) == NULL) {
		ArFprintf(stderr, 
			"ArBuildFrame: error reading  diagram.\n");
		/** free( frP ); BT **/
		return( NULL );
	    }
	    frP->diagram = *diagramP;
	    break;

	case AUTO_EDIT:
	    temp_short = FALSE;
	    ArGetYesNoShort(fiP, &temp_short);
	    frP->flags.auto_edit = temp_short;
	    break;
	case FR_RULING:
	    /* this is left in temporarily until old documents using
	       this format fade away */
	    temp_short = FALSE;
	    ArGetYesNoShort(fiP, &temp_short);
	    if(temp_short) {
		/** EcTblRulDefault(frame); BT **/
		ArFprintf(stderr, 
		    "ArBuildFrame(): found obsolete 'Rulings = ...';");
		ArFprintf(stderr, 
		    " please resave this document in TPS 4.0 and repeat.\n");
	    }
	    break;
	case FR_STRADDLE:
	    ArGetInt(fiP, &straddle, YES, NO);
	    if ( straddle < 1 ){
	       frP -> straddle = 1;
	    }else{
	       frP -> straddle = straddle;
	    }
	    break;
	case FR_TINT:
	    {
		int temp = 0;
		ArGetInt(fiP, &temp, YES, NO);
		if(temp >= 0 && temp <= AR_MAX_COLORS)
		    frP->tint = temp;
	    }
	    break;

	case FR_COL_RULE_COLOR:
	    if(ArGetInt(fiP, &temp_int, YES, NO) && 
	       temp_int >= 0            && 
	       temp_int <= AR_MAX_COLORS) {
		    cur_ruleP->color = temp_int;
		    cur_ruleP->visible = YES;
	    }
	    break;
	case FR_COL_RULE_PATTERN:
	    if(ArGetInt(fiP, &temp_int, YES, NO) && 
	       temp_int >= 0            && 
	       temp_int <= AR_MAX_PATTERNS) {
		    cur_ruleP->pattern = temp_int;
	    }
	    break;
	case FR_COL_RULE_WIDTH:
	    if(ArGetInt(fiP, &temp_int, NO, NO) == YES) {
		if(temp_int >= 0 && temp_int <= 6)
		    cur_ruleP->width = temp_int;
	    }
	    cur_ruleP->visible = YES;
	    cur_ruleP->double_rule  = NO;
	    if((c = ArGetchNW(fiP)) != COMMA) {
		ArGetWordShrC(fiP, c, al_token_shr, AR_MAX_WORD-1, COMMA);
		if(ArStrncmpShr(al_token_shr, "Double", 6, LOOSE) == 0)
		    cur_ruleP->double_rule = YES;
	    }
	    break;
	case FR_COL_RULE_VIS:
	    if (ArGetYesNoShort(fiP, &temp_short) == YES){
		cur_ruleP->visible = temp_short;
	    }
	    break;
	case NUMBERED:
	    if((c = ArGetchNW(fiP)) == '"' || c == '\'') {
		/** gobble it **/
	    }
	    else {
		ArUnGetch(c, fiP);
	    }
	    if((c = ArGetchNW(fiP)) == AR_CMD_CHAR)	/* < */
		/* Autonum */
		ArGetWordShrC(fiP, NOCHAR, al_token_shr, AR_MAX_WORD-1, COMMA); 
	    else
		ArUnGetch(c, fiP);
	    numbered_set = TRUE;
#if 0	/** BT **/
	    if((c = ArGetchNW(fiP)) == '"' || c == '\'') {
		/** gobble it **/
	    }
	    else {
		ArUnGetch(c, fiP);
	    }
#endif	/** BT **/
	    frP->anumP = ArBuildAutonum(fiP);
	    if((c = ArGetchNW(fiP)) == '"' || c == '\'') {
		/** gobble it **/
	    }
	    else {
		ArUnGetch(c, fiP);
	    }
	    if((c = ArGetchNW(fiP)) == COMMA) {
		/** gobble it **/
	    }
	    else {
		ArUnGetch(c, fiP);
	    }
	    break;
	case SUPERSCRIPT:
	    temp_short = FALSE;
	    ArGetYesNoShort(fiP, &temp_short);
	    if(temp_short) 
	    {
		if(numbered_set && frP->anumP)
		{
		    frP->flags.super_num = YES;
		}
		else
		{
		    super_set = TRUE;
		}
	    }
	    break;
	default      :
	    frP->unkP = (Unknown_t *)ArBuildUnknownProp(fiP, frP->unkP);
	    ArSkipPastComma(fiP);
	    break;
	}
    }	    /* end while loop */

    if (frame_a_was_dumped) {
	frP->yofs = frP->a;
	frP->a -= frP->height;
    }

    /** ArFprintf(stderr, "Adding Frame %Q.\n", frP->name); BT **/
    return( frP );
}
/**--------------------------------------------------------------------------**/
/*  ArFrameAutoWidth
 *	Process auto-width token.
 */
void
ArFrameAutoWidth(fiP, autosizeP, autowidth)
  File_info_t *fiP;
  Autosize_t *autosizeP;
  short autowidth;
{
    int		c;
    bool 	negative;
    float 	measure;
    int		temp_int;

    autosizeP->frame_width = autowidth;

    while (TRUE) {
	negative = FALSE;

	switch (c = ArGetchNW(fiP)) {
	case '*':
	    if (ArGetFloat(fiP, &measure))
		autosizeP->xfrac = measure * AR_PAGE_DENOM;
	    else
		goto AUTO_WIDTH_ERR;
	    break;

	case '-':
	    negative = TRUE;
	case '+':
	    if (ArGetRsu(fiP, &temp_int))
		autosizeP->xadj = negative ? -temp_int : temp_int;
	    else
		goto AUTO_WIDTH_ERR;
	    return;

	case ',':
	    return;

	case '>':
	    ArUnGetch('>', fiP);
	    return;

AUTO_WIDTH_ERR:
	default:
	    ArTellError(ERR_FRM_AUTOWID);
	    ArSkipPastComma(fiP);
	    return;
	}
    }
}
	    
/*  ArFrameAutoHeight
 *	Process auto-height token.
 */
void
ArFrameAutoHeight(fiP, autosizeP, autoheight)
  File_info_t *fiP;
  Autosize_t *autosizeP;
  short autoheight;
{
    int		c;
    bool 	negative;
    float 	measure;
    int		temp_int;

    autosizeP->frame_height = autoheight;

    while (TRUE) {
	negative = FALSE;

	switch (c = ArGetchNW(fiP)) {
	case '*':
	    if (ArGetFloat(fiP, &measure))
		autosizeP->yfrac = measure * AR_PAGE_DENOM;
	    else
		goto AUTO_HEIGHT_ERR;
	    break;

	case '-':
	    negative = TRUE;
	case '+':
	    if (ArGetRsu(fiP, &temp_int))
		autosizeP->yadj = negative ? -temp_int : temp_int;
	    else
		goto AUTO_HEIGHT_ERR;
	    return;

	case ',':
	    return;

	case '>':
	    ArUnGetch('>', fiP);
	    return;

AUTO_HEIGHT_ERR:
	default:
	    ArTellError(ERR_FRM_AUTOHGT);
	    ArSkipPastComma(fiP);
	    return;
	}
    }
}
/**--------------------------------------------------------------------------**/
/**	NOTE	this function is called by ArBuildRow(), in ar5table.c	     **/
/**--------------------------------------------------------------------------**/
bool
ArParseCell(fiP, cellP)
  File_info_t *fiP;
  register Cell_t* cellP;
{
  shar c;
  short token;
  short temp_short;
  int temp_int;
  Table_rule_t *cur_ruleP = NULL;
  void ArFrameAutoHeight();
  void ArFrameAutoWidth();

    *cellP = default_table_cell;	/** sets id, nextP, spareP **/
    
    while((c = ArGetchNW(fiP)) != AR_CMD_TERM && c != EOF) {
	ArGetWordShrC(fiP, c, al_token_shr, AR_MAX_WORD-1, 0);
	if((c = ArGetchNW(fiP)) != '=') {
	    ArUnGetch(c, fiP);
	    ArTellError(ERR_MISS_EQ);
	    ArSkipPastComma(fiP);
	    continue;
	}
	switch(ArLRTBPrefix(fiP)) {
	case LD_BOTTOM:
	case LD_HEADER:
	case LD_FOOTER:
	case LD_RIGHT:
	    /* ENHANCE error message */
	    ArFprintf(stderr, 
		"ArParseCell(): unexpected prefix in %Q, ignoring property.\n",
		al_token_shr);
	default:
	case LD_TOP:
	    cur_ruleP = &cellP->rulings.top;
	    break;
	case LD_LEFT:
	    cur_ruleP = &cellP->rulings.left;
	    break;
	}

        switch(ArTokenShr(fiP, al_token_shr, al_frame_tokens,
			  NUM_OF(al_frame_tokens), LOOSE)) {
	case NO_EQUALS:
	    break;
	case FRMWIDTH:
	    if((token = ArGetTokenAlpha(fiP,al_frm_wid_names,NFWIDNAMES))== -1){
		ArGetPZRsu(fiP, &cellP->width);
	    } else {
		if (cellP->autosizeP == NULL) {
		     cellP->autosizeP = NEW(Autosize_t);
		    *cellP->autosizeP = default_fr_autosize;
		}
		ArFrameAutoWidth(fiP, cellP->autosizeP, token);
	    }
	    /* ????? width is set down below for table cells */
	    break;

	case FRMHEIGHT:
	    if((token = ArGetTokenAlpha(fiP,al_frm_hgt_names,NFHGTNAMES))== -1){
		ArGetPZRsu(fiP, &cellP->height);
	    }
	    else {
		if (cellP->autosizeP == NULL) {
		     cellP->autosizeP = NEW(Autosize_t);
		    *cellP->autosizeP = default_fr_autosize;
		}
		ArFrameAutoHeight(fiP, cellP->autosizeP, token);
	    }
	    break;

	case CONTENTS_TO_FR_WIDTH:
	    temp_short = FALSE;
	    ArGetYesNoShort(fiP, &temp_short);
	    if (temp_short) {
		if (cellP->autosizeP == NULL) {
		    cellP->autosizeP = NEW(Autosize_t);
		    *cellP->autosizeP = default_fr_autosize;
		}
		if (cellP->autosizeP->frame_width == ASZWCOL) {
		    cellP->autosizeP->frame_width = ASZEXXCOL;
		}
		else {
		    cellP->autosizeP->frame_width = ASZEXX;
		}
	    }
	    else if (cellP->autosizeP != NULL) {
		cellP->autosizeP->frame_width = ASZNOX;
	    }
	    break;

	case CONTENTS_TO_FR_HEIGHT:
	    temp_short = FALSE;
	    ArGetYesNoShort(fiP, &temp_short);
	    if (temp_short) {
		if (cellP->autosizeP == NULL) {
		    cellP->autosizeP = NEW(Autosize_t);
		    *cellP->autosizeP = default_fr_autosize;
		}
		cellP->autosizeP->frame_height = ASZEXY;
	    }
	    else if (cellP->autosizeP != NULL) {
		cellP->autosizeP->frame_height = ASZNOY;
	    }
	    break;

	case VERT_ALIGN:
	    {
		token = ArGetTokenAlpha(fiP, al_frm_valign_names, NFVNAMES);
		if (token == -1) {
		    ArTellError(ERR_FRM_VALIGN);
		    cellP->v_align = fr_V_TOP;
		}
		else {
		    cellP->v_align = token;
		}
	    }
	    break;
	case AUTO_EDIT:
	    temp_short = FALSE;
	    ArGetYesNoShort(fiP, &temp_short);
	    cellP->auto_edit = temp_short;
	    break;
	case FR_STRADDLE:
	    ArGetInt(fiP, &temp_int, YES, NO);
	    if(temp_int < 1) temp_int = 1;
	    cellP->straddle = temp_int;
	    break;
	case FR_RULING:
	    /* this is left in temporarily until old documents using
	       this format fade away */
	    temp_short = FALSE;
	    ArGetYesNoShort(fiP, &temp_short);
	    if(temp_short) {
		/** EcTblRulDefault(frame); BT **/
		ArFprintf(stderr, 
		    "ArBuildFrame(): found obsolete 'Rulings = ...';");
		ArFprintf(stderr, 
			" please resave this document in the current TPS.\n");
	    }
	    break;
	case FR_COL_RULE_COLOR:
	    if(ArGetInt(fiP, &temp_int, YES, NO) && 
	       temp_int >= 0            && 
	       temp_int <= AR_MAX_COLORS) {
		    cur_ruleP->color = temp_int;
		    cur_ruleP->visible = YES;
	    }
	    break;
	case FR_COL_RULE_PATTERN:
	    if(ArGetInt(fiP, &temp_int, YES, NO) && 
	       temp_int >= 0            && 
	       temp_int <= AR_MAX_PATTERNS) {
		    cur_ruleP->pattern = temp_int;
	    }
	    break;
	case FR_COL_RULE_WIDTH:
	    if(ArGetInt(fiP, &temp_int, NO, NO) == YES) {
		if(temp_int >= 0 && temp_int <= 6)
		    cur_ruleP->width = temp_int;
	    }
	    cur_ruleP->visible = YES;
	    cur_ruleP->double_rule  = NO;
	    if((c = ArGetchNW(fiP)) != COMMA) {
		ArGetWordShrC(fiP, c, al_token_shr, AR_MAX_WORD-1, COMMA);
		if(ArStrncmpShr(al_token_shr, "Double", 6, LOOSE) == 0)
		    cur_ruleP->double_rule = YES;
	    }
	    break;
	case FR_COL_RULE_VIS:
	    if (ArGetYesNoShort(fiP, &temp_short) == YES){
		cur_ruleP->visible = temp_short;
	    }
	    break;
	default:
	    cellP->unkP = (Unknown_t *)ArBuildUnknownProp(fiP, cellP->unkP);
	    ArSkipPastComma(fiP);
	    break;
	}
	if((c = ArGetchNW(fiP)) != ',')   /* ADDED BY CK 10/3/89 */
	  ArUnGetch(c, fiP);              /* make sure we eat a closing comma */
    }	    /* end while loop */

    return( YES );
}

/**--------------------------------------------------------------------------**/
