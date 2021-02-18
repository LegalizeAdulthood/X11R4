
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
/*	ar5cell.c                             */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/
/**--------------------------------------------------------------------------**/
bool ArParseCell(fiP)
  register File_info_t *fiP;
{
  Cell_t *cellP = NEW(Cell_t);
  shar c;
  short token;
  short temp_short;
  int temp_int;
  Table_rule_t *cur_ruleP = NULL;
  void ArFrameAutoHeight();
  void ArFrameAutoWidth();

    *cellP            = default_table_cell;	/** sets id, nextP, spareP **/

    
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
		if (cellP->autosizeP == NULL)
		    cellP->autosizeP = NEW(Autosize_t);
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

	case AUTO_EDIT:
	    temp_short = FALSE;
	    ArGetYesNoShort(fiP, &temp_short);
	    cellP->flags.auto_edit = temp_short;
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
    }	    /* end while loop */

    return( cellP );
}

/**--------------------------------------------------------------------------**/
