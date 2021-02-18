
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
/*	ar5dump.c                               */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  07/88    BT	Original coding
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
char AR_dump_yesstr[] = "Yes";
char  AR_dump_nostr[] = "No";
char  AR_dump_onstr[] = "On";
char AR_dump_offstr[] = "Off";

/**--------------------------------------------------------------------------**/
static bool dumpCmpnProps();
/**--------------------------------------------------------------------------**/
FILE *AR_ofile = NULL;

/**--------------------------------------------------------------------------**/
bool
ArObjectWrite( srcP, ilP, ofile )
  register NullObj_t *srcP;
  Il_state_t *ilP;
  register FILE *ofile;
{
  static char err_msg[] =
  "ArObjectWrite(%#x, %#x): unknown identifier type %#x.  Get help. Exiting.\n";

    if (srcP) {
	switch(Identify(srcP)) {
	case ID_Error:
	case ID_NullObj:
	    ArFprintf(ofile, DECL_START_FMT, "Comment");
	    ArFprintf(ofile, 
		", ArObjectWrite was given an %s object!",
		((Identify(srcP)==ID_Error)? "ID_Error" : "ID_NullObj") );
	    ArFprintf(ofile, DECL_END_FMT, "Comment");
	    break;
	case ID_OpsDecl:
	    ArOpsInfoWrite((Ops_info_t *)srcP, ofile);
	    break;
	case ID_Document:
	    ArDocInfoWrite((Document_info_t *)srcP, ofile);
	    break;
	case ID_Font:
	    ArFontInfoWrite((Font_info_t *)srcP, ofile);
	    break;
	case ID_Color:
	    ArColorInfoWrite((Color_info_t *)srcP, ofile);
	    break;
	case ID_Pattern:
	    ArPatternInfoWrite((Pattern_info_t *)srcP, ofile);
	    break;
	case ID_Page:
	    ArPageInfoWrite((Page_info_t *)srcP, NO, ofile);
	    break;
	case ID_MasterFrame:
	    ArFrameWrite((Frame_t *)srcP, ofile);
	    break;
	case ID_Frame:
	    ArFrameWrite((Frame_t *)srcP, ofile);
	    break;
	case ID_ClassDef:
	case ID_MasterCmpn:
	    ArCmpnWrite((Cmpn_t *)srcP, NULL, ofile);
	    break;
	case ID_Cmpn:
	    ArCmpnWrite((Cmpn_t *)srcP, ilP, ofile);
	    break;
	case ID_MasterTable:
	case ID_Table:
	    ArTableWrite((Table_t *)srcP, ilP, ofile);
	    break;
	case ID_MasterRow:
	case ID_Row:
	    ArRowWrite((Row_t *)srcP, ilP, ofile);
	    break;
	case ID_Cell:
	    ArCellWrite((Cell_t *)srcP, ilP, ofile);
	    break;
	case ID_HeadFoot:
	    ArHeaderFooterWrite((HeaderFooter_t *)srcP, ofile);
	    break;
	case ID_AutoStream:
	    ArAnStreamInfoWrite((An_stream_info_t *)srcP, ofile);
	    break;
	case ID_FileInfo:
	    ArFileInfoWrite((File_info_t *)srcP, ofile);
	    break;
	case ID_State:
	    ArDocWrite((Il_state_t *)srcP, ofile);
	    break;
	case ID_Comment:
	    ArCommentWrite((Comment_t *)srcP, ofile);
	    break;
	case ID_UnknownDecl:
	case ID_UnknownProp:
	    ArUnknownWrite((Unknown_t *)srcP, ofile);
	    break;
	default:
	    ArFprintf(stderr, err_msg, srcP, ofile, *(Identifier_t *)srcP);
	    break;

	}
    }
    else {
	ArFprintf(ofile, DECL_START_FMT, "Comment");
	ArFprintf(ofile, ", NULL srcP passed into ArObjectWrite");
	ArFprintf(ofile, DECL_END_FMT);
	return( NO );
    }
    return( YES );
}
/**--------------------------------------------------------------------------**/
void
ArOpsInfoWrite( opsP, ofile )
  register Ops_info_t *opsP;
  register FILE *ofile;
{
    ArFprintf(ofile, OPS_DECL_FMT, "Ops"); 
    ArFprintf(ofile, " --from ASCII Reader v. %d.%d-- ", 
		ARversion / 100,
		ARversion % 100);
    ArUnknownWrite(opsP->unkP, ofile);
    ArFprintf(ofile, OPS_VERSION_FMT, opsP->version/100, opsP->version%100 );
    if (opsP->style != OPS_STYLE_DEFAULT) {
	ArFprintf(ofile, PROP_YESNO_FMT, "Style", "Metric" /* only choice */);
    }
    ArFprintf(ofile, DECL_END_FMT);
}
/**--------------------------------------------------------------------------**/
void
ArDocInfoWrite( diP, ofile )
  register Document_info_t *diP;
  register FILE *ofile;
{

    ArFprintf(ofile, DECL_START_FMT, "Document");
    ArUnknownWrite(diP->unkP, ofile);
    ArFprintf(ofile,PROP_YESNO_FMT, "Header Page", 
		YesNo(diP->flags.header_page));
    ArFprintf(ofile,PROP_YESNO_FMT, "Double Sided", 
		YesNo(diP->flags.double_sided));
    ArFprintf(ofile,PROP_YESNO_FMT, "Manual Sheet Feed",
		YesNo(diP->flags.manual_feed));
    ArFprintf(ofile,PROP_YESNO_FMT, "Print Rev Bars", 
		YesNo(diP->flags.print_rev));
    ArFprintf(ofile,PROP_YESNO_FMT, "Print Strikes", 
		YesNo(diP->flags.print_strikes));
    ArFprintf(ofile,PROP_YESNO_FMT, "Print Underlines", 
			YesNo(diP->flags.print_underlines));
    ArFprintf(ofile,PROP_YESNO_FMT, "Underline At Descender", 
			YesNo(diP->flags.underline_at_descender));
    ArFprintf(ofile,PROP_YESNO_FMT, "Orientation Inverted", 
			YesNo(diP->flags.orient_inverted));
    if (diP->final_output_dev[0] != '\0') {
	ArFprintf(ofile,PROP_NAME_FMT, 
		"Final Output Device", diP->final_output_dev);
    }
    if (diP->def_printer[0] != '\0') {
	ArFprintf(ofile,PROP_NAME_FMT, "Default Printer", diP->def_printer);
    }
    if (diP->measure_unit != OPS_STYLE_DEFAULT) {
	ArFprintf(ofile,PROP_YESNO_FMT, "MeasurementUnit", 
		    Measure_names[diP->measure_unit]);
    }
    if (diP->A_Pg_style != tok_ARABIC) {
	ArFprintf(ofile,PROP_YESNO_FMT, "A-Page # Style",
		    NumStyle_names[diP->A_Pg_style]);
    }
    if (diP->A_Pg_prefix[0] != '\0') {
	ArFprintf(ofile,PROP_NAME_FMT, "A-Page # Prefix", diP->A_Pg_prefix);
    }
    if (diP->max_page != AR_APAGE_ANY) {
	ArFprintf(ofile,PROP_DECIMAL_FMT, "Maximum Page", diP->max_page);
    }
    ArFprintf(ofile, DECL_END_FMT);
}
/**--------------------------------------------------------------------------**/
void
ArFontInfoWrite( fdP, ofile )
  register Font_info_t *fdP;
  register FILE *ofile;
{

    if (fdP == NULL ) {
	ArFprintf(ofile, DECL_COMMENT_FMT, "no font definitions.");
    }
    else {
	ArFprintf(ofile, DECL_START_FMT, "Font Definitions");
	for( ; fdP != NULL; fdP = (Font_info_t *)fdP->nextP) {
		ArFprintf(ofile, FONT_DEF_FMT,
		     fdP->font_token,
		     fdP->font_name,
		     fdP->font_size,
		     FontFace_names[fdP->font_face] );
	}
	ArFprintf(ofile, DECL_END_FMT);
    }
}
/**--------------------------------------------------------------------------**/
void
ArColorInfoWrite( clrP, ofile )
  register Color_info_t *clrP;
  register FILE *ofile;
{

    if (clrP == NULL) {
	ArFprintf(ofile, DECL_COMMENT_FMT, "no color definitions.");
	return;
    }
    ArFprintf(ofile, DECL_START_FMT, "Color Definitions");
    for ( ; clrP != NULL; clrP = (Color_info_t *)clrP->nextP) {
	ArFprintf(ofile, TYPE_INDEX_FMT, clrP->color.type, clrP->color.index);
	FPRINTF(ofile, COLOR_DESC_FMT,
	    clrP->color.desc.r, clrP->color.desc.g, clrP->color.desc.b);
    }
    ArFprintf(ofile, DECL_END_FMT);
}
/**--------------------------------------------------------------------------**/
void
ArPatternInfoWrite( patP, ofile )
  register  Pattern_info_t *patP;
  register FILE *ofile;
{

    if (patP == NULL) {
	ArFprintf(ofile, DECL_COMMENT_FMT, "no pattern definitions.");
	return;
    }
    ArFprintf(ofile, DECL_START_FMT, "Pattern Definitions");
    for ( ; patP != NULL; patP = (Pattern_info_t *)patP->nextP) {
	ArFprintf(ofile, TYPE_INDEX_FMT,
			patP->pattern.type, patP->pattern.index);
	{ register unsigned short *sP, *lastsP;
	    for (sP = patP->pattern.pattern, 
		 lastsP = &patP->pattern.pattern[AR_PATTERN_SIZE];
		 sP < lastsP; 
		 sP++) {
		    ArFprintf(ofile, PATTERN_HEX_FMT, *sP);
	    }
	}
    }
    ArFprintf(ofile, DECL_END_FMT);
}
/**--------------------------------------------------------------------------**/
void
ArPageInfoWrite( pgP, isMicroDoc, ofile )
  register Page_info_t *pgP;
  bool isMicroDoc;
  register FILE *ofile;
{

    if (pgP == NULL) {
	ArFprintf(ofile, DECL_COMMENT_FMT, 
			"ArPageInfoWrite() given NULL pointer.");
    }
    else {
	if (isMicroDoc == NO) {
	    ArFprintf(ofile, DECL_START_FMT, "Page");
	}
	else {
	    ArFprintf(ofile, CMD_START_LINE_FMT, "!Page");
	}
	ArUnknownWrite(pgP->unkP, ofile);
	ArFprintf(ofile, PROP_DECIMAL_FMT, "Columns", pgP->columns);
	FPRINTF(ofile, PROP_INCH_FMT, "Gutter", R2I(pgP->gutter) );
	FPRINTF(ofile, PROP_INCH_FMT, "Height", R2I(pgP->height) );
	FPRINTF(ofile, PROP_INCH_FMT, "Width", R2I(pgP->width) );
	FPRINTF(ofile, PROP_INCH_FMT, "Top Margin", R2I(pgP->top_margin) );
	FPRINTF(ofile, PROP_INCH_FMT, "Bottom Margin",
			R2I(pgP->bottom_margin) );
	FPRINTF(ofile, PROP_INCH_FMT, "Left Margin", 
			R2I(pgP->left_margin) );
	FPRINTF(ofile, PROP_INCH_FMT, "Right Margin", 
			R2I(pgP->right_margin) );
	ArFprintf(ofile, PROP_YESNO_FMT, "Hyphenation", 
			OnOff(pgP->flags.hyphenation) );
	if (pgP->ladder_count == AR_HYPHEN_ANY) {
	    ArFprintf(ofile, PROP_STRING_FMT, "Consecutive Hyphens", "Any");
	} else {
	    ArFprintf(ofile, PROP_DECIMAL_FMT, "Consecutive Hyphens", 
			pgP->ladder_count);
	}
	ArFprintf(ofile, PROP_DECIMAL_FMT,
		"Margin Stretch", pgP->margin_stretch);
	ArFprintf(ofile, PROP_DECIMAL_FMT, "Margin Shrink", pgP->margin_shrink);
	ArFprintf(ofile, PROP_DECIMAL_FMT, "Frame Margin Stretch", 
			pgP->frame_margin_stretch);
	ArFprintf(ofile, PROP_YESNO_FMT,
			"Feathering", OnOff(pgP->flags.feathering) );
	if (pgP->flags.feathering == YES && 
	    pgP->max_feathering != AR_MAX_FEATHERING_DFLT) {
	      ArFprintf(ofile,PROP_DECIMAL_FMT,
			"Max. Feathering", pgP->max_feathering);
	}
	ArFprintf(ofile, PROP_YESNO_FMT, "Vert. Just. Pages",
		    OnOff(pgP->flags.vert_just_pages));
	switch(pgP->flags.rev_bar_place) {
	    case REV_BAR_LEFT: ArFprintf(ofile, PROP_STRING_FMT,
				"Revision Bar Placement", "Left"); 
			break;
	    case REV_BAR_RIGHT: ArFprintf(ofile, PROP_STRING_FMT,
				"Revision Bar Placement", "Right"); 
			break;
	    /** future ... **/
	    /** case REV_BAR_AUTO: ArFprintf(ofile, PROP_STRING_FMT,	**/
			    /** "Revision Bar Placement", "Automatic");  **/
			/** break; **/
	}
	ArFprintf(ofile, PROP_YESNO_FMT, "Vertical Justify", 
			OnOff(pgP->flags.vert_just) );
	if (isMicroDoc == NO) {
	    FPRINTF(ofile, PROP_INCH_FMT, "Inner Margin", 
			    R2I(pgP->inner_margin) );
	    FPRINTF(ofile, PROP_INCH_FMT, "Outer Margin", 
			    R2I(pgP->outer_margin) );
	    if (pgP->first_page == d_LEFT || pgP->first_page == d_RIGHT) {
		ArFprintf(ofile, PROP_YESNO_FMT, "First Page", 
				(pgP->first_page == d_LEFT)? "Left" : "Right" );
	    }
	    ArFprintf(ofile, PROP_YESNO_FMT, "Bleed", YesNo(pgP->flags.bleed) );
	    if (pgP->l_r_margins == YES) {
		ArFprintf(ofile, PROP_STRING_FMT, "Margins", "Left/Right");
	    }
	    if (pgP->start_page_num == AR_PGNUM_INHERIT) {
		ArFprintf(ofile, PROP_STRING_FMT, "Starting Page #", "Inherit");
	    }
	    else {
		ArFprintf(ofile, PROP_DECIMAL_FMT, "Starting Page #", 
			pgP->start_page_num);
	    }
	    if (pgP->pgnum_prefix[0] != '\0') {
		ArFprintf(ofile, PROP_NAME_FMT,
				"Page # Prefix", pgP->pgnum_prefix);
	    }
	    ArFprintf(ofile, PROP_YESNO_FMT, "Page # Style", 
			    NumStyle_names[pgP->pgnum_style]);
	    ArFprintf(ofile,PROP_DECIMAL_FMT,
			"Depth At Page Break", pgP->depth_pbreak);
	    ArFprintf(ofile,PROP_DECIMAL_FMT, "Depth No Page Break", 
		pgP->depth_no_pbreak);
	    ArFprintf(ofile, PROP_YESNO_FMT, "Balance Columns", 
			    OnOff(pgP->flags.bal_columns) );
	    ArFprintf(ofile, PROP_YESNO_FMT, "Frozen Number Streams", 
			OnOff(pgP->flags.frozen_autonums) );
	    switch(pgP->flags.page_parity) {
	    default:
	    case PG_PARITY_DEFAULT:
		break;
	    case PG_PARITY_EVEN:
		ArFprintf(ofile, PROP_STRING_FMT, "Begin With", "Even");
		break;
	    case PG_PARITY_ODD:
		ArFprintf(ofile, PROP_STRING_FMT, "Begin With", "Odd");
		break;

	    }
	    switch(pgP->flags.turned_pages) {
	    default:
	    case TURNED_0:
		break;
	    case TURNED_90:
		ArFprintf(ofile, PROP_STRING_FMT, "Turned Pages", "90");
		break;
	    case TURNED_270:
		ArFprintf(ofile, PROP_STRING_FMT, "Turned Pages", "270");
		break;
	    }
	}
    }
    ArFprintf(ofile, DECL_END_FMT);
}
/**--------------------------------------------------------------------------**/
void
ArAnStreamInfoWrite( anP, ofile )
  register An_stream_info_t *anP;
  register FILE *ofile;
{
  register short i;

    for ( ; anP; anP = (An_stream_info_t *)anP->nextP) {
	ArFprintf(ofile, DECL_START_FMT, "Autonumber Stream");
	ArFprintf(ofile, OBJ_NAME_FMT, anP->an_name);
	ArFprintf(ofile, AN_LEVELS_FMT, anP->an_levels);
	for ( i = 1; i <= anP->an_levels; i++) {
	  register An_level_t *anlP = &anP->an_level_info[i-1];
	    ArFprintf(ofile, PROP_LEVEL_STRING_FMT, i, "SymbolType", 
			NumStyle_names[anlP->an_symbol]);
	    ArFprintf(ofile, PROP_LEVEL_NAME_FMT, i, "Prefix", 
						anlP->an_prefix);
	    ArFprintf(ofile, PROP_LEVEL_NAME_FMT, i, "Suffix", 
						anlP->an_suffix);
	    ArFprintf(ofile, PROP_LEVEL_DECIMAL_FMT, i, "Starting Value", 
						anlP->start_value);
	    ArFprintf(ofile, PROP_LEVEL_YESNO_FMT, i, "LastOnly", 
						YesNo(anlP->an_last_only));
	    ArFprintf(ofile, PROP_LEVEL_YESNO_FMT, i, "Show", 
						YesNo(anlP->an_show));
	    /** Note an_inherit is not currently used. BT. **/
	}
	ArFprintf(ofile, DECL_END_FMT);
    }
}
/**--------------------------------------------------------------------------**/
void
ArClassDefaultsWrite( cdP, ofile )
  register Cmpn_t *cdP;
  register FILE *ofile;
{
    if (cdP != NULL) {
	ArCmpnWrite(cdP, NULL, ofile);
    }
}
/**--------------------------------------------------------------------------**/
void
ArAttrValsWrite( avP, ofile )
  Attr_t *avP;
  FILE *ofile;
{
    for ( ; avP != NULL; avP = avP->nextP) {
	ArFprintf(ofile, ATTR_VAL_FMT, avP->nameP, avP->stringP);
    }
}
/**--------------------------------------------------------------------------**/
void
ArTextBodyWrite( ssP, ilP, ofile )
  Ssave_t *ssP;
  Il_state_t *ilP;
  FILE *ofile;
{
  int i;
  shar *sP;
  register shar c;

    if((i = SsAmount(ssP)) > 0) {
	for (sP = SsBufStart(ssP); i > 0; ) {
	    if ((c = *sP) == '\0') {
		break;
	    }
	    else if (IS_PADDING(c)) {
		/** skip it **/
		sP++;
		i--;
		continue;
	    }
	    else if (IS_TOKEN(c)) {
		if (ARdebug) { 
		    ArFprintf(stderr, "ArTextBodyWrite: token %#x\n", c); 
		}
		ArTokenWrite(sP, ilP, ofile);
		sP += sizeof(Token_t)/sizeof(shar);/**bump pointer past token**/
		i  -= sizeof(Token_t)/sizeof(shar);/**and update count	     **/
		continue;
	    }
	    else {
		if (c == AR_CMD_CHAR) {
		    /** double '<<' in output. **/
		    ArFprintf(ofile, TEXT_CHAR_FMT, c);
		}
		ArFprintf(ofile, TEXT_CHAR_FMT, c);
		sP++;                /** bump pointer past normal text unit. **/
		i--;
		if (ARdebug) { 
		    ArFprintf(stderr, "ArTextBodyWrite: %C\n", c); 
		}
	    }
	}
    }
    /** ArFprintf(ofile, "\n"); BT **/
}
/**--------------------------------------------------------------------------**/
void
ArCmpnMastersWrite( cmP, ofile )
  register Cmpn_t *cmP;
  register FILE *ofile;
{

    for ( ; cmP != NULL; cmP = (Cmpn_t *)cmP->nextP) {
	ArCmpnWrite(cmP, NULL, ofile);
    }
}
/**--------------------------------------------------------------------------**/
/**	This tests if the first argument's tab settings are a prefix of the  **/
/**	second argument's tab settings.  				     **/
bool 
ArTabsEqual(newP, masterP)
  Tabline_t *newP, *masterP;
{
  /** new entry pointer, master entry pointer **/
  register Tabentry_t *neP, *meP;	

  register short num_stops = newP->num_stops;
  bool same;

    if (num_stops == 0) {
	return( NO );		/** can't be equal if 0 tab stops exist. **/
    }
    for (same = YES, neP = newP->entries, meP = masterP->entries; 
	 num_stops--; 
	 neP++, meP++ ) {
	     if (neP->stop != meP->stop || neP->type != meP->type) {
		 same = NO;
		 break;
	     }
    }
    return( same );	/** return value is, are they equal? **/
}
/**--------------------------------------------------------------------------**/
bool 
ArCmpnWrite(cmP, ilP, ofile)
  register Cmpn_t *cmP;
  register Il_state_t *ilP;
  register FILE *ofile;
{
  register Cmpn_t *masterP;

    if (cmP == NULL || ofile == NULL) {
	return( NO );
    }
    switch(cmP->id) {
	case ID_Cmpn:
	    if (cmP->subcmpn == YES) {
		ArFprintf(ofile, CMPN_START_FMT, cmP->cmpn_name);
	    } else {
		ArFprintf(ofile, CMPN_START_LINE_FMT, cmP->cmpn_name);
	    }
	    masterP = (ilP != NULL? ilP->cmpnP : NULL);
	    for ( ; masterP != NULL; masterP = (Cmpn_t *)masterP->nextP) {
		if (ArShrcmp(cmP->cmpn_name, masterP->cmpn_name) == 0) {
		    break;
		}
	    }
	    break;
	case ID_MasterCmpn:
	    ArFprintf(ofile, DECL_START_FMT, "Class");
	    ArFprintf(ofile, OBJ_NAME_FMT, cmP->cmpn_name);
	    masterP = NULL;
	    break;
	case ID_ClassDef:
	    ArFprintf(ofile, DECL_START_FMT, "ClassDefaults");
	    masterP = NULL;
	    break;
	default:
	    ArFprintf(stderr, "ArCmpnWrite(): given wrong object, type == %d\n",
		    cmP->id);
	    return( NO );
	    break;
    }
    dumpCmpnProps(cmP, masterP, ilP, ofile);
    if (cmP->id != ID_ClassDef) {
	ArTextBodyWrite(&cmP->body, ilP, ofile);
	if (cmP->subcmpn == YES) {
	    ArFprintf(ofile, CMD_PLAIN_FMT, "EndSub");
	}
    }
    return( YES );
}
/**--------------------------------------------------------------------------**/
static bool 
dumpCmpnProps(cmP, masterP, ilP, ofile)
  register Cmpn_t *cmP;
  register Cmpn_t *masterP;
  Il_state_t *ilP;
  register FILE *ofile;
{
    if (cmP == NULL || ofile == NULL) {
	return( NO );
    }
    if (masterP == NULL) {
	masterP = &output_cmpn;
    }
    ArUnknownWrite(cmP->unkP, ofile);
    if (cmP->attrvalsP) {
	ArAttrValsWrite( cmP->attrvalsP, ofile );
    }
#if 0	/** BT **/
    /** I used to think that this test was enough, but it isn't.  a master
	can have content that is a subcomponent with shared content; that
	subcmpn must emit 'contents = shared'.
    **/
    if (cmP->id != ID_Cmpn && cmP->content_type != masterP->content_type) 
#endif	/** BT **/
    if (cmP->content_type != tok_BUF_PRIVATE || 
	cmP->content_type != masterP->content_type) {
	    ArFprintf(ofile, PROP_YESNO_FMT, "Contents", 
			Content_names[cmP->content_type]);
    }
    if (cmP->hidden != masterP->hidden) {
	ArFprintf(ofile, PROP_YESNO_FMT, "Hidden", YesNo(cmP->hidden));
    }
    if (cmP->subcmpn != masterP->subcmpn) {
	ArFprintf(ofile, PROP_YESNO_FMT, "Subcmpn", YesNo(cmP->subcmpn));
    }
    if (ArFontValueEqual(&cmP->font_value, &masterP->font_value) == NO) {
	ArFprintf(ofile, PROP_KEYWORD_EQ_FMT, "Font");
	ArFontValueWrite(&cmP->font_value,(ilP!=NULL? ilP->fontP : NULL),ofile);
    }
    if (cmP->top_margin != masterP->top_margin) {
	FPRINTF(ofile, PROP_INCH_FMT, "Top Margin", 
						   R2I(cmP->top_margin));
    }
    if (cmP->bottom_margin != masterP->bottom_margin) {
	FPRINTF(ofile, PROP_INCH_FMT, "Bottom Margin", 
						   R2I(cmP->bottom_margin));
    }
    if (cmP->left_margin != masterP->left_margin) {
	FPRINTF(ofile, PROP_INCH_FMT, "Left Margin", 
						   R2I(cmP->left_margin));
    }
    if (cmP->right_margin != masterP->right_margin) {
	FPRINTF(ofile, PROP_INCH_FMT, "Right Margin",
						   R2I(cmP->right_margin));
    }
    if (cmP->first_indent != masterP->first_indent) {
	FPRINTF(ofile, PROP_INCH_FMT, "First Indent",
						   R2I(cmP->first_indent));	
    }
    if (cmP->line_spacing != masterP->line_spacing) {
	FPRINTF(ofile, PROP_LINES_FMT, "Line Spacing", 
		       (double)(cmP->line_spacing/(double)AR_LINE_SCALING));
    }
    if (cmP->alignment != masterP->alignment) {
	switch(cmP->alignment) {
	    case d_LEFT:   ArFprintf(ofile, PROP_STRING_FMT, 
					"Alignment", "Left"); break;
	    case d_RIGHT:  ArFprintf(ofile, PROP_STRING_FMT, 
					"Alignment", "Right"); break;
	    case d_CENTER: ArFprintf(ofile, PROP_STRING_FMT, 
					"Alignment", "Center"); break;
	    case d_BOTH: 
	    default:       ArFprintf(ofile, PROP_STRING_FMT, 
					"Alignment", "Both"); break;
	}
    }
    if (cmP->att_prior != masterP->att_prior) {
	ArFprintf(ofile, PROP_YESNO_FMT, "AttachPrevious", YesNo(cmP->att_prior));
    }
    if (cmP->pbreak_within != masterP->pbreak_within) {
	ArFprintf(ofile,PROP_YESNO_FMT, "Allow Break Within",
			    YesNo(cmP->pbreak_within));
    }
    if (cmP->att_next != masterP->att_next) {
	ArFprintf(ofile, PROP_YESNO_FMT, "AttachNext", YesNo(cmP->att_next));
    }
    if (cmP->fill != masterP->fill) {
	switch(cmP->fill) {
	    case fill_ON:    ArFprintf(ofile, PROP_STRING_FMT, 
					"Fill", "On"); break;
	    case fill_OFF:   ArFprintf(ofile, PROP_STRING_FMT, 
					"Fill", "Off"); break;
	    case fill_BLANK: 
	    default:      ArFprintf(ofile, PROP_STRING_FMT, 
					"Fill", "Blank"); break;
	}
    }
    if (cmP->new_page != masterP->new_page) {
	ArFprintf(ofile, PROP_YESNO_FMT, "NewPage", YesNo(cmP->new_page));
    }
    if (cmP->new_column != masterP->new_column) {
	ArFprintf(ofile, PROP_YESNO_FMT, "NewColumn", YesNo(cmP->new_column));
    }
    if (cmP->widow_ctrl != masterP->widow_ctrl) {
	ArFprintf(ofile, PROP_DECIMAL_FMT, "Widow Control ", cmP->widow_ctrl);
    }
    if (cmP->orphan_ctrl != masterP->orphan_ctrl) {
	ArFprintf(ofile, PROP_DECIMAL_FMT, "Orphan Control", cmP->orphan_ctrl);
    }
    if (cmP->straddle != masterP->straddle) {
	ArFprintf(ofile, PROP_YESNO_FMT, "Straddle", YesNo(cmP->straddle));
    }
    if (cmP->kern_space != masterP->kern_space) {
	ArFprintf(ofile,PROP_YESNO_FMT, "TrackKernSpaces",YesNo(cmP->kern_space));
    }
    if (cmP->word_spacing != masterP->word_spacing) {
	if (cmP->word_spacing == NO) {
	    ArFprintf(ofile, PROP_STRING_FMT, "WordSpacing", "Default");
	}
	else {
	    if (cmP->word_sp_max != masterP->word_sp_max) {
		FPRINTF(ofile,PROP_3F_FMT, "WordSpaceMax", 
			cmP->word_sp_max);
	    }
	    if (cmP->word_sp_min != masterP->word_sp_min) {
		FPRINTF(ofile,PROP_3F_FMT, "WordSpaceMin", 
			cmP->word_sp_min);
	    }
	    if (cmP->word_sp_nom != masterP->word_sp_nom) {
		FPRINTF(ofile,PROP_3F_FMT, "WordSpaceNom", 
			cmP->word_sp_nom);
	    }
	}
    }
    if (cmP->letter_spacing != masterP->letter_spacing) {
	if (cmP->letter_spacing == NO) {
	    ArFprintf(ofile, PROP_STRING_FMT, "LetterSpacing", "No");
	}
	else {
	    ArFprintf(ofile, PROP_STRING_FMT, "LetterSpacing", "Yes");
	    FPRINTF(ofile,PROP_3F_FMT, "LetterSpaceMax", 
			cmP->letter_sp_max);
	}
    }
    if (cmP->composition != masterP->composition) {
	ArFprintf(ofile, PROP_YESNO_FMT, "Composition", 
			Composition_names[cmP->composition]);
    }
    if (cmP->hyphenation != masterP->hyphenation) {
	ArFprintf(ofile, PROP_DECIMAL_FMT, "Hyphenation", cmP->hyphenation);
    }
    if (ArShrcmp(cmP->toc_name, masterP->toc_name) != 0) {
	    ArFprintf(ofile, PROP_NAME_FMT, "TOC Doc Name ", cmP->toc_name);
    }
    /** check for tab differences, emit all tabs if different. **/
    if (ArTabsEqual(&cmP->tabs, &masterP->tabs) == NO) {
	ArCmpnTabsWrite(&cmP->tabs, ofile);
    }
    if (cmP->profileP == NULL) {
      if(masterP->profileP != NULL)
	FPRINTF(ofile, ",\n    Profiling = None");
    }
    else {
      Cmpn_profile_t *proP = cmP->profileP;
      while(proP) {
	FPRINTF(ofile, ",\n    %s Profile = %d; %d; %.3f inches",
		proP->side == d_LEFT ? "Left" : "Right",
		proP->start_line,
		proP->line_count,
		R2I(proP->place));
	proP = proP->nextP;
      }
    }
    ArFprintf(ofile, DECL_END_FMT);
    return ( YES );
}
/**--------------------------------------------------------------------------**/
void
ArCmpnTabsWrite(tlP, ofile)
  Tabline_t *tlP;
  FILE *ofile;
{
  Tabentry_t *teP;
  Tab_type_t ttype;
  short num_stops = tlP->num_stops;
  short i, j;
  short tcounts[5];
  static Tab_type_t types[] = { tab_LEFT, tab_RIGHT, tab_CENTER, 
				tab_DECIMAL, tab_DECIMAL_COMMA };
  static char *lhs[] = { LEFT_TAB_FMT,
			 RIGHT_TAB_FMT,
			 CENTER_TAB_FMT,
			 DEC_TAB_FMT,
			 DEC_COMMA_TAB_FMT
			};

    /** special case. **/
    if (num_stops == 0) {
	ArFprintf(ofile, lhs[0]);
	return;
    }
    tcounts[0] = tcounts[1] = tcounts[2] = tcounts[3] = tcounts[4] = 0;
    for (i = 0, teP = tlP->entries; i < num_stops; i++, teP++ ) {
	switch(teP->type) {
	    case tab_LEFT:          ++tcounts[0]; break;
	    case tab_RIGHT:         ++tcounts[1]; break;
	    case tab_CENTER:        ++tcounts[2]; break;
	    case tab_DECIMAL:       ++tcounts[3]; break;
	    case tab_DECIMAL_COMMA: ++tcounts[4]; break;
	}
    }

    for (j = 0; j < sizeof(tcounts)/sizeof(tcounts[0]); j++ ) {
	if (tcounts[j] == 0) { continue; }
	ArFprintf(ofile, lhs[j]);
	ttype = types[j];
	for (i = 0, teP = tlP->entries; i < num_stops; i++, teP++ ) {
	    if (teP->type == ttype) {
		FPRINTF(ofile, 
			TAB_3F_FMT, 
			R2I(teP->stop),
			(--tcounts[j] == 0
			    ? " "         /** at end, space before "inches". **/
			    : (tcounts[j] %6 == 5
				   ? "/\n\t"  /** wrap to another line.      **/
				   : "/"      /** else use simple separator. **/
			) ) );
#if 0	/** BT **/
		FPRINTF(ofile, TAB_3F_FMT, R2I(teP->stop));
		if (--tcounts[j] == 0) {
		    ArFprintf(ofile, " "); 
		    break; 
		}
		else {
		    ArFprintf(ofile, "/");
		}
#endif	/** BT **/
	    }
	}
	ArFprintf(ofile, " inches");
    }
}

/**--------------------------------------------------------------------------
 * This routine examines the Autosize_t and emits the correct markup. I
 * modified it to deal with autosize objects (frames/cells) that are
 * autosize-ish in one dimension but fixed in the other. For this purpose,
 * I added "fixw" and "fixh" parameters. (CK 7/27/89)
 */
void ArAutosizeWrite(autosizeP, fixw, fixh, ofile)
  Autosize_t *autosizeP;
  Rsu fixw, fixh;
  FILE *ofile;
{
    switch(autosizeP->frame_width) {
    case ASZWFIX:               /* it's not autosize in this dimension */
        FPRINTF(ofile, PROP_INCH_FMT, "Width",  R2I(fixw)  );
	break;
    case ASZINX:
	ArFprintf(ofile, PROP_STRING_FMT, "Width", "Contents");
	break;
#ifdef OBSOLETE  /* this test is now done in ArFrameWrite() CK 8/30/89 */
    case ASZEXX:
	/** ArFprintf(ofile, PROP_STRING_FMT, "Width", "Contents"); BT **/
	ArFprintf(ofile, PROP_STRING_FMT, "Size Contents to Width", "Yes");
	break;
    case ASZEXXCOL:
	/** ArFprintf(ofile, PROP_STRING_FMT, "Width", "Column"); BT **/
	ArFprintf(ofile, PROP_STRING_FMT, "Size Contents to Width", "Yes");
	break;
    case ASZNOX:
	ArFprintf(ofile, PROP_STRING_FMT, "Width", "Contents");
	ArFprintf(ofile, PROP_STRING_FMT, "Size Contents to Width", "No");
	break;
#endif	
    case ASZEXX:        
    case ASZEXXCOL:
    case ASZNOX:
	break;            /* don't do anything on these */
    default:
	ArFprintf(ofile, PROP_STRING_FMT, "Width", 
	    FrameAutoWid_names[autosizeP->frame_width]);
	if (autosizeP->xfrac != AR_PAGE_DENOM) {
	    FPRINTF(ofile, TIMES_OFFSET_FMT, 
			    autosizeP->xfrac / (float)AR_PAGE_DENOM);
	}
	if (autosizeP->xadj != 0) {
	    if (autosizeP->xadj < 0) {
		FPRINTF(ofile, MINUS_OFFSET_INCH_FMT, -R2I(autosizeP->xadj));
	    }
	    else {
		FPRINTF(ofile, PLUS_OFFSET_INCH_FMT, R2I(autosizeP->xadj));
	    }
	}
	break;
    }
    switch(autosizeP->frame_height) {
    case ASZHFIX:               /* it's not autosize in this dimension */
        FPRINTF(ofile, PROP_INCH_FMT, "Height",  R2I(fixh)  );
	break;
    case ASZINY:
	ArFprintf(ofile, PROP_STRING_FMT, "Height", "Contents");
	break;
#ifdef OBSOLETE  /* this test is now done in ArFrameWrite() CK 8/30/89 */
    case ASZEXY:
	/** ArFprintf(ofile, PROP_STRING_FMT, "Height", "Contents"); BT **/
	ArFprintf(ofile, PROP_STRING_FMT, "Size Contents to Height", "Yes");
	break;
    case ASZNOY:
	ArFprintf(ofile, PROP_STRING_FMT, "Height", "Contents");
	ArFprintf(ofile, PROP_STRING_FMT, "Size Contents to Height", "No");
	break;
#endif
    case ASZEXY:
    case ASZNOY:
	break;          /* don't do anything on these */
    default:
	ArFprintf(ofile, PROP_STRING_FMT, "Height", 
	    FrameAutoHgt_names[autosizeP->frame_height]);
	if (autosizeP->yfrac != AR_PAGE_DENOM) {
	    FPRINTF(ofile, TIMES_OFFSET_FMT, 
			    autosizeP->yfrac / (float)AR_PAGE_DENOM);
	}
	if (autosizeP->yadj != 0) {
	    if (autosizeP->yadj < 0) {
		FPRINTF(ofile, MINUS_OFFSET_INCH_FMT, -R2I(autosizeP->yadj));
	    }
	    else {
		FPRINTF(ofile, PLUS_OFFSET_INCH_FMT, R2I(autosizeP->yadj));
	    }
	}
	break;
    }
}

/**--------------------------------------------------------------------------**/
void
ArFrameWrite(frP, ofile)
  register Frame_t*	frP;
  FILE *ofile;
{
  register Autosize_t  *autosizeP = frP->autosizeP;
  bool floating = frP->flags.fixed == fr_FLOAT;
  bool overlay  = floating && (frP->flags.placement == fr_OVERLAY || 
			     frP->flags.placement == fr_UNDERLAY);

    switch (Identify(frP)) {
    case ID_MasterFrame:
    case ID_Frame:
	/** it's cool. **/
	break;
    default:
	ArFprintf(stderr, "ArFrameWrite(): given wrong object,  type == %d.\n",
	    Identify(frP));
	return;
	break;
    }
    if (frP->id==ID_MasterFrame) {
	ArFprintf(ofile, DECL_START_FMT, "MasterFrame");
    }
    else {
	ArFprintf(ofile, CMD_START_LINE_FMT, "Frame"); 
    }

    /* Frames in table cells have no names so don't write anything */
    if(frP->name[0] != 0) {
      ArFprintf(ofile, PROP_NAME_FMT, "Name", frP->name );
    }

    if ( frP -> straddle != default_frame.straddle ){
       ArFprintf(ofile,PROP_DECIMAL_FMT,"Straddle",frP -> straddle);
    }
    if (frP->flags.hidden == YES) {
	ArFprintf(ofile, PROP_STRING_FMT, "Hidden", "yes");
    }
    ArFprintf(ofile, PROP_KEYWORD_EQ_FMT, "Placement");

    if (floating) {
	ArFprintf(ofile, PLAIN_STRING_FMT, 
			FramePlace_names[frP->flags.placement]);
	ArFprintf(ofile, PROP_STRING_FMT, "Horizontal Alignment",
		    FrameHAlign_names[frP->flags.h_align]);

	if (frP->xofs != 0) {
	    if (frP->xofs < 0) {
		FPRINTF(ofile, MINUS_OFFSET_INCH_FMT, R2I(-frP->xofs));
	    }
	    else {
		FPRINTF(ofile, PLUS_OFFSET_INCH_FMT, R2I( frP->xofs));
	    }
	}
	if (frP->flags.same_page == YES && !overlay) {
	    ArFprintf(ofile, PROP_YESNO_FMT, "Same Page", "yes");
	}
    }
    else {
	ArFprintf(ofile, PLAIN_STRING_FMT, 
			FramePlace_names[fr_FIXED]);
    }

    ArUnknownWrite(frP->unkP, ofile);

    if (!floating || overlay) {
	ArFprintf(ofile, PROP_STRING_FMT, "Vertical Alignment", 
			FrameVAlign_names[frP->flags.v_align]);

	if (frP->yofs != 0) {
	    if (frP->yofs < 0) {
		FPRINTF(ofile, MINUS_OFFSET_INCH_FMT, R2I(-frP->yofs));
	    }
	    else {
		FPRINTF(ofile, PLUS_OFFSET_INCH_FMT, R2I( frP->yofs));
	    }
	}
    }

    if (overlay) {
	ArFprintf(ofile, PROP_STRING_FMT, "Horizontal Reference",
		FrameHRef_names[frP->flags.x_locate]);
	ArFprintf(ofile, PROP_STRING_FMT, "Vertical Reference",
		FrameVRef_names[frP->flags.y_locate]);
    }

    if (autosizeP) {
	ArAutosizeWrite(autosizeP, frP->width, frP->height, ofile);
    }
    else {
	FPRINTF(ofile, PROP_INCH_FMT, "Width",  R2I(frP->width)  );
	FPRINTF(ofile, PROP_INCH_FMT, "Height", R2I(frP->height) );
    }

    /* Need to write this out explicitly because frames in diagrams
     * have different defaults from frames in table cells.
     */
    ArFprintf(ofile, PROP_STRING_FMT,
	      "Size Contents to Height",
	      frP->flags.cont_height ? "Yes" : "No");
    ArFprintf(ofile, PROP_STRING_FMT,
	      "Size Contents to Width",
	      frP->flags.cont_width ? "Yes" : "No");

    if (frP->flags.shared) {
	ArFprintf(ofile, PROP_STRING_FMT, "Shared Contents", "yes");
    }

    if (frP->rulingsP != NULL) {
	ArRulingWrite(&frP->rulingsP->top,   NULL, "TopRuling",  ofile);
	ArRulingWrite(&frP->rulingsP->left,  NULL, "LeftRuling", ofile);
    }

    if (frP->flags.auto_edit) {
	ArFprintf(ofile, PROP_STRING_FMT, "AutoEdit", "Yes");
    }

    if (frP->flags.repeat_info == fr_REP_BEGIN  ||
	frP->flags.repeat_info == fr_REP_END)           {
	if (frP->flags.repeat_info == fr_REP_BEGIN) {
	    ArFprintf(ofile, PROP_STRING_FMT, "Repeating", "Begin");
	    ArFprintf(ofile, PROP_YESNO_FMT, "Begin On Anchor Page", 
		YesNo(frP->flags.repeat_same_page));
	}
	else {
	    ArFprintf(ofile, PROP_STRING_FMT, "Repeating", "End");
	    ArFprintf(ofile, PROP_YESNO_FMT, "End On Anchor Page", 
		YesNo(frP->flags.repeat_same_page));
	}
    }
    if (frP->anumP != NULL) {
	ArFprintf(ofile, PROP_KEYWORD_EQ_FMT, "Numbered");
	ArFprintf(ofile, "'");
	ArAutonumWrite(frP->anumP, ofile);
	ArFprintf(ofile, "'");
	if (frP->flags.super_num) {
	   ArFprintf(ofile, PROP_STRING_FMT, "Superscript", "Yes");
	}
    }
    if (frP->attrvalsP) {
	ArAttrValsWrite( frP->attrvalsP, ofile );
    }
    if (!frP->flags.show_anchor) {
	ArFprintf(ofile, PROP_STRING_FMT, "Anchor Visible", "No");
    }
    if (frP->page_numP && *frP->page_numP != '\0') {
	ArFprintf(ofile, PROP_NAME_FMT, "Page#", frP->page_numP);
    }

    /*  Do not dump the frame contents if it is an instance of a
     *  shared-content frame. 
     *  [This is inoperative. BT] Note special case of setting EFSHARED above.
     *  [Do dump a frame if it is a master.  BT]
     */
    /** don't try to dump an empty diagram.  Probably it is shared contents,
	but that is a weaker test, so we don't bother to make it.  If
	the contents are there, we dump them, otherwise forget it.
    **/
    if (frP->diagram.contents != NULL) {
	ArFprintf(ofile, PROP_KEYWORD_FMT, "Diagram");
	ArEmitDg(ofile, &frP->diagram);
    }

    if (frP->id==ID_MasterFrame) {
	ArFprintf(ofile, DECL_END_FMT);
    }
    else {
	ArFprintf(ofile, CMD_END_FMT); 
    }
}
/**--------------------------------------------------------------------------**/
void
ArFrameMastersWrite( frP, ofile )
  register Frame_t *frP;
  register FILE *ofile;
{

    for ( ; frP != NULL; frP = (Frame_t *)frP->nextP) {
	ArFrameWrite(frP, ofile);
    }
}
/**--------------------------------------------------------------------------**/
void
ArHeaderFooterWrite( hfP, ofile )
  register HeaderFooter_t *hfP;
  register FILE *ofile;
{

    for ( ; hfP; hfP = (HeaderFooter_t *)hfP->nextP) {
	ArFprintf(ofile, CMD_START_LINE_FMT, HeaderFooter_names[hfP->type]);
	ArUnknownWrite(hfP->unkP, ofile);
	ArFprintf(ofile, PROP_KEYWORD_FMT, "Frame");
	ArEmitDg(ofile, &hfP->diagram );
	ArFprintf(ofile, CMD_END_FMT);
    }
}
/**--------------------------------------------------------------------------**/
void
ArFileInfoWrite( fiP, ofile )
  register File_info_t *fiP;
  register FILE *ofile;
{
    ArFprintf(ofile, DECL_START_FMT, "Comment");
    ArFprintf(ofile, 
	", File_info_t at %#x ==> ifile %#x%s, saved_char %#x,\n",
	fiP, fiP->ifile, (fiP->ifile==stdin? " (stdin)" : ""), fiP->saved_char);
    show_Ssave(&fiP->inqueue);
    ArFprintf(ofile, DECL_END_FMT);
}
/**--------------------------------------------------------------------------**/
bool ArDocDeclWrite(ilP, ofile)
  register Il_state_t *ilP;
  register FILE *ofile;
{
    AR_ofile = ofile;

    ArOpsInfoWrite       ( &ilP->ops_info,        ofile );
    ArDocInfoWrite       ( &ilP->doc_info,        ofile );
    ArFontInfoWrite      (  ilP->fontP,           ofile );
    ArColorInfoWrite     (  ilP->colorP,          ofile );
    ArPatternInfoWrite   (  ilP->patternP,        ofile );
    ArPageInfoWrite      ( &ilP->page_info, NO,   ofile );
    ArAnStreamInfoWrite  (  ilP->an_streamP,      ofile );
    ArClassDefaultsWrite (  ilP->class_defaultsP, ofile );
    ArCmpnMastersWrite   (  ilP->cmpnP,           ofile );
    ArFrameMastersWrite  (  ilP->frameP,          ofile );
    ArHeaderFooterWrite  (  ilP->hdftP,           ofile );
    ArTableMastersWrite  (  ilP->tableP,          ofile );
    return( YES );
}
/**--------------------------------------------------------------------------**/
bool ArDocBodyWrite(ilP, ofile)
  register Il_state_t *ilP;
  register FILE *ofile;
{
  Cmpn_t *bodyP;

    if (ilP != NULL && ofile != NULL) {
	AR_ofile = ofile;
	for (bodyP=(Cmpn_t *)ilP->bodyP; 
	    bodyP!=NULL; 
	    bodyP=(Cmpn_t *)bodyP->nextP) {
		ArObjectWrite(bodyP, ilP, ofile);
	}
	ArFprintf(ofile, "\n\n");    /** a blank line at the end of file. **/
    }
    return( YES );
}
/**--------------------------------------------------------------------------**/
bool
ArDocWrite(ilP, ofile)
  Il_state_t *ilP;
  FILE *ofile;
{
    return( ArDocDeclWrite(ilP, ofile) && ArDocBodyWrite(ilP, ofile) );
}
/**--------------------------------------------------------------------------**/
void
ArCommentWrite(comP, ofile)
  Comment_t *comP;
  FILE *ofile;
{
    if (comP != NULL && SsAmount(&comP->text) > 0) {
	ArFprintf(ofile, COMMENT_DECL_FMT, SsBufStart(&comP->text));
    }
}
/**--------------------------------------------------------------------------**/
void ArMicrodocWrite( mdP, ofile)
  Microdoc_t *mdP;
  FILE *ofile;
{
NullObj_t*	contP;	/* Use temp pointer - don't damage structure */

    if (mdP == NULL || ofile == NULL) {
	return;
    }
    ArPageInfoWrite(&mdP->micro_page, YES, ofile);
    contP = mdP->contentsP;
    while (contP != NULL) {
	ArObjectWrite(contP, AR_il_stateP, ofile);
	contP = contP->nextP;
    }
    ArFprintf(ofile, CMD_PLAIN_FMT, "EndText");
}
/**--------------------------------------------------------------------------**/
