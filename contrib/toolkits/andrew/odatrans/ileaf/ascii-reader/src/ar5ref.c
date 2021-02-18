
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
/*	ar5ref.c                                 */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  08/88    BT	Original version
 */

/**--------------------------------------------------------------------------**/
/**	These routines build a ref token to put in a Cmpn_t buffer.	     **/
/**	Any substructure is built, then a token reference is put into the    **/
/**	buffer supplied by the caller.					     **/
/**--------------------------------------------------------------------------**/
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
#define AUTO_NUM_KEYWORD	0
#define PAGE_NUM_KEYWORD	1
#define CURRENT_PAGE_KEYWORD	2
#define TAG_KEYWORD		3
#define VALUE_KEYWORD		4
#define PAGE_VALUE_KEYWORD	5
#define RANGE_KEYWORD		6
#define OPER_KEYWORD		7
#define RESULT_KEYWORD		8
#define LOOKUP_KEYWORD		9
#define TEXT_KEYWORD		10
#define BLANK_PAGES_KEYWORD	11
#define CUR_BLANK_PAGE_KEYWORD	12
#define ATTR_VAL_KEYWORD	13
#define CURRENT_FRAME_KEYWORD	14
#define TOTAL_FRAME_KEYWORD	15
#define FRAME_START_KEYWORD	16

static struct keytab	ref_inst_keywords[] = 
{
    { "Attribute",	TEXT_KEYWORD },
    { "Attribute Value", ATTR_VAL_KEYWORD },
    { "Auto #",		AUTO_NUM_KEYWORD },
    { "Auto Num",	AUTO_NUM_KEYWORD },
    { "Current Frame",	CURRENT_FRAME_KEYWORD },
    { "Current Page",	CURRENT_PAGE_KEYWORD },
    { "Current Show Blank Page", CUR_BLANK_PAGE_KEYWORD },
    { "Lookup Pairs",	LOOKUP_KEYWORD },
    { "Operation",	OPER_KEYWORD },
    { "Page #",		PAGE_NUM_KEYWORD },
    { "Page Num",	PAGE_NUM_KEYWORD },
    { "Page Value",	PAGE_VALUE_KEYWORD },
    { "Range",		RANGE_KEYWORD },
    { "Result",		RESULT_KEYWORD },
    { "Show Blank Pages", BLANK_PAGES_KEYWORD },
    { "Starting Num",	FRAME_START_KEYWORD },
    { "Tag",		TAG_KEYWORD },
    { "Tag Name",	TAG_KEYWORD },
    { "Total Frames",	TOTAL_FRAME_KEYWORD },
    { "Value",		VALUE_KEYWORD }
};

/**--------------------------------------------------------------------------**/
static LookupPair_t *
refGetLookupPairs(fiP)
  File_info_t *fiP;
{
  shar buf[AR_MAX_QSTR], buf2[AR_MAX_QSTR];
  LookupPair_t *lkP;
  LookupPair_t *resultP = NULL;


    while (ArGetWordShrC(fiP, NOCHAR, buf,  NUM_OF(buf)-1,  0) == YES &&
	   ArGetWordShrC(fiP, NOCHAR, buf2, NUM_OF(buf2)-1, 0) == YES) {
	    /** get a new one. **/
	    lkP = NEW(LookupPair_t);
	    /** save this pair of names.  **/
	    lkP->nameP = (shar *)ArSafeMalloc((1+ArShrlen(buf))*sizeof(shar));
	    ArShrcpy(lkP->nameP, buf);
	    lkP->stringP = (shar *)ArSafeMalloc((1+ArShrlen(buf2))*sizeof(shar));
	    ArShrcpy(lkP->stringP, buf2);
	    /** grow the list (backwards, but that's okay). **/
	    lkP->nextP = resultP;
	    resultP = lkP;
    }
    return( resultP );
}
/**--------------------------------------------------------------------------**/
bool
arBufGetRef(fiP, ssP)
  File_info_t *fiP;
  Ssave_t *ssP;
{
  Token_t      *tkP;
  Ref_t        *refP;
  register int	keyword;
  short		sbuf     [AR_MAX_QSTR];
  LookupPair_t *first_lp = NULL;
  short		range = 0;
  short		oper = attr_OPER_MAX;
  short		type;
  short		temp_short;

    tkP           = NEW(Token_t);
    tkP->token    = TOKEN_SET(tok_Ref);
    refP          = NEW(Ref_t);
    tkP->u.refP   = refP;

    type = ref_CUR_PAGE_PLAIN;
    refP->frame_start = 1;
    refP->tag[0] = '\0';

    for ( ;;  ArSkipPastComma(fiP) ){
	switch ( ArGetTextShr( fiP, sbuf, NUM_OF(sbuf)) ){
	default:
	case EOF:
	    ArTellError( ERR_EXPECTED_TERM);
	    /* Fall through... */
	case AL_TEXT_ERROR:
	    ArSkipPastTerm(fiP);
	    goto DONE_READING;

	case AL_TEXT_QUOTED:
	case AL_TEXT_OK:
	    break;
	}
	if ( sbuf[0] == '\0' ){
	    /* Use "keyword" as a temp. */
	    if ( ( keyword = ArGetchNW(fiP) ) == al_cmd_term ){
		goto DONE_READING;
	    }
	    ArUnGetch(keyword, fiP);
	    if ( keyword != ',' ){
		ArTellError2( ERR_UNEXPECTED_CHARACTER, (long)keyword, 0L);
	    }
	    continue;
	}
	keyword = ArTokenShr( fiP, sbuf, ref_inst_keywords,
			     NUM_OF(ref_inst_keywords), LOOSE);
	/* Get the argument for those that need it. */
	switch ( keyword ){
	case TAG_KEYWORD:
	case VALUE_KEYWORD:
	case PAGE_VALUE_KEYWORD:
	case RANGE_KEYWORD:
	case OPER_KEYWORD:
	case BLANK_PAGES_KEYWORD:
	case FRAME_START_KEYWORD:
	    /* The keyword must be followed by an equals. */
	    if ( ArGetchNW(fiP) != '=' ){
		ArTellError( ERR_MISS_EQ);
		continue;
	    }
	    ArGetTextShr( fiP, sbuf, NUM_OF(sbuf));
	    if ( sbuf[0] == '\0' ){
		ArTellError( ERR_MISSING_ARG_AFTER_EQUALS);
		continue;
	    }
	    break;
	case RESULT_KEYWORD:
	case TEXT_KEYWORD:
	case LOOKUP_KEYWORD:
	    /* The keyword must be followed by an equals. */
	    if ( ArGetchNW(fiP) != '=' ){
		ArTellError( ERR_MISS_EQ);
		continue;
	    }
	    /* THIS CALL SHOULD BE REPLACED BY A CALL THAT RETURNS A SHORT
	     * STRING!! */
	    if (keyword == RESULT_KEYWORD || keyword == TEXT_KEYWORD) {
		/* LOOKPAIRS are read later */
		ArGetTextShr( fiP, sbuf, AR_MAX_ATTRVALSIZE);
		if ( sbuf[0] == '\0' ){
		    ArTellError( ERR_MISSING_ARG_AFTER_EQUALS);
		    continue;
		}
	    }
	    break;
	}
	/* Finally, do the real work of handling the keyword. */
	switch ( keyword ){
	case FRAME_START_KEYWORD:
	    if (*sbuf != 0) {
		if (ArCvtIntShr(sbuf, &temp_short, YES) == YES) {
		    refP->frame_start = temp_short;
		}
	    }
	    break;
	case CURRENT_PAGE_KEYWORD:
	    type = ref_CUR_PAGE_PLAIN;
	    refP->tag[0] = '\0';
	    break;
	case CUR_BLANK_PAGE_KEYWORD:
	    type = ref_CUR_PAGE_BLANK;
	    refP->tag[0] = '\0';
	    break;
	case AUTO_NUM_KEYWORD:
	    type = ref_ANUM_NUM;
	    refP->tag[0] = '\0';
	    break;
	case PAGE_NUM_KEYWORD:
	    type = ref_ANUM_PAGE;
	    break;
	case ATTR_VAL_KEYWORD:
	    type = ref_ATTR_VALUE;
	    break;
	case CURRENT_FRAME_KEYWORD:
	    type = ref_CUR_FRAME;
	    break;
	case TOTAL_FRAME_KEYWORD:
	    type = ref_TOTAL_FRAME;
	    break;
	case TAG_KEYWORD:
	    sbuf[AR_MAX_REFTAGSIZE] = '\0';
	    ArShrcpy(refP->tag, sbuf);
	    break;
	case VALUE_KEYWORD:
	case PAGE_VALUE_KEYWORD:
	    ArShrcpy(refP->value, sbuf);
	    break;
	case RANGE_KEYWORD:
	    range = attr_RANGE_PAGE;
	    if (!ArStrncmpShr(sbuf, "Sheet", 256, LOOSE)) {
		range = attr_RANGE_SHEET;
		break;
	    }
	    if (!ArStrncmpShr(sbuf, "Cmpn", 256, LOOSE)) {
		range = attr_RANGE_CMPN;
		break;
	    }
	    break;
	case OPER_KEYWORD:
	    oper = attr_OPER_MAX;
	    if (!ArStrncmpShr(sbuf, "Min", 256, LOOSE)) {
		oper = attr_OPER_MIN;
		break;
	    }
	    break;
	case BLANK_PAGES_KEYWORD:
	    if (!ArStrncmpShr(sbuf, "Yes", 256, LOOSE)) {
		type = ref_CUR_PAGE_BLANK;
	    }
	    break;
	case TEXT_KEYWORD:
	    ArShrcpy(refP->attribute, sbuf);
	    break;
	case RESULT_KEYWORD:
	    ArShrcpy(refP->result, sbuf);
	    break;
	case LOOKUP_KEYWORD:
	    first_lp = refGetLookupPairs(fiP);
	    break;
	default:
	    ArTellError2( ERR_NO_SUCH_CVAL, (long)sbuf, 0L);
	    continue;
	}
    }
DONE_READING:
    refP->type = type;
    if (type == ref_ATTR_VALUE) {
	refP->operation       = oper;
	refP->range           = range;
	refP->pairsP          = first_lp;
    }

    BufEnter(tkP, ssP);
    /** ArSkipPastTerm(fiP);	 BT **/
    return( YES );
}
/**--------------------------------------------------------------------------**/
void
ArRefWrite(refP, ofile)
  Ref_t *refP;
  FILE  *ofile;
{
    if (refP != NULL && ofile != NULL) {
	ArFprintf(ofile, CMD_START_LINE_FMT, "Ref");
	switch( refP->type ) {
	case ref_CUR_PAGE_PLAIN:
	    ArFprintf(ofile, PROP_KEYWORD_FMT, "Current Page");
	    if (refP->value[0] != '\0') {
		ArFprintf(ofile, PROP_NAME_FMT, "PageValue", refP->value);
	    }
	    break;
	case ref_CUR_PAGE_BLANK:
	    ArFprintf(ofile, PROP_KEYWORD_FMT, "Current Show Blank Page");
	    if (refP->value[0] != '\0') {
		ArFprintf(ofile, PROP_NAME_FMT, "PageValue", refP->value);
	    }
	    break;
	case ref_ANUM_NUM:
	    ArFprintf(ofile, PROP_KEYWORD_FMT, "Auto#");
	    ArFprintf(ofile, PROP_NAME_FMT, "Tag", refP->tag );
	    if (refP->value[0] != '\0') {
		ArFprintf(ofile, PROP_NAME_FMT, "Value", refP->value);
	    }
	    break;
	case ref_ANUM_PAGE:
	    ArFprintf(ofile, PROP_KEYWORD_FMT, "Page#");
	    ArFprintf(ofile, PROP_NAME_FMT, "Tag", refP->tag );
	    if (refP->value[0] != '\0') {
		ArFprintf(ofile, PROP_NAME_FMT, "PageValue", refP->value);
	    }
	    break;
	case ref_ATTR_VALUE:
	    ArFprintf(ofile, PROP_KEYWORD_FMT, 
			     "AttributeValue");
	    ArFprintf(ofile, PROP_NAME_FMT, 
			     "Attribute", 
			     refP->attribute);
	    ArFprintf(ofile, PROP_STRING_FMT, 
			     "Operation", 
		             Attr_oper_names[refP->operation]);
	    ArFprintf(ofile, PROP_STRING_FMT, 
			     "Range", 
			     Attr_range_names[refP->range]);
	    ArFprintf(ofile, PROP_NAME_FMT, 
			     "Result", 
			     refP->result);
	    if(refP->pairsP != NULL) {
		  LookupPair_t *lkP;
		  ArFprintf(ofile, PROP_KEYWORD_EQ_FMT, "LookupPairs");
		  for(lkP = refP->pairsP; lkP != NULL; lkP = lkP->nextP) {
		    ArFprintf(ofile,  NAME_PAIR_FMT, lkP->nameP, lkP->stringP);
		  }
	    }
	    break;
	case ref_CUR_FRAME:
	    ArFprintf(ofile, PROP_KEYWORD_FMT, "CurrentFrame");
	    if (refP->frame_start != 1) {
		ArFprintf(ofile, PROP_STRING_DECIMAL_FMT, 
			    "StartingNum", 
			    refP->frame_start);
	    }
	    break;
	case ref_TOTAL_FRAME:
	    ArFprintf(ofile, PROP_KEYWORD_FMT, "TotalFrames");
	    if (refP->frame_start != 1) {
		ArFprintf(ofile, PROP_STRING_DECIMAL_FMT, 
			    "StartingNum", 
			    refP->frame_start);
	    }
	    break;
	default:
	    ArFprintf(stderr, "ArRefWrite(): unknown type %d.  Get help.\n",
			refP->type);
	    /** exit( -1 ); BT **/
	    break;
	}
	ArFprintf(ofile, CMD_END_FMT);
    }
}
/**--------------------------------------------------------------------------**/
static void *
LookupPairsFree(lkP)
  LookupPair_t *lkP;
{
  LookupPair_t *nextP;

    for ( ; lkP != NULL; ) {
	nextP = lkP->nextP;
	FREE((char *)lkP->nameP);
	FREE((char *)lkP->stringP);
	FREE((char *)lkP);
	lkP = nextP;
    }
}
/**--------------------------------------------------------------------------**/
void ArRefFree(refP)
  Ref_t *refP;
{
    if (refP != NULL) {
	LookupPairsFree(refP->pairsP);
	FREE(refP);
    }
}
/**--------------------------------------------------------------------------**/
