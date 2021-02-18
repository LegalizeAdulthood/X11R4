
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
/*	ar5cmpn.c                               */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  07/88    BT	Original AR version, derived from alMain.c
 *  08/89    CK Changes to saving of profile info in ArParseCmpn(). I
 *              removed the test "if(have_profile) { remove it }" because
 *              this was removing the profile info from the master anytime
 *              an instance differed. This was leading to crashes when the
 *              master tried to dump its info. I didn't understand the
 *              reasoning behind this test as I think it's no less valid
 *              for a master and an instance to have differing profile data
 *              then it is for any other property to differ between master
 *              and instance.
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

extern bool al_end_text;    /* set when <End Text> seen */

/**--------------------------------------------------------------------------**/
#define ALIGN		1  /* keep these matched with case statement */
#define ATTNEXT		2
#define ATTPRIOR	3
#define BMARGIN		4
#define CENTERTAB	5
#define DECIMALTAB	6
#define FILL		7
#define FONT		8
#define HEIGHT		9
#define INDENT		10
#define LEADING		11
#define LMARGIN		12
#define LEFTTAB		13
/*#define CLASS_NAME	14    GONE */
#define NEWPAGE		15
#define ORPH_CNTL	16
#define RMARGIN		17
#define RIGHTTAB	18
#define SELECTED	19
#define TMARGIN		20
#define UNBREAK		21
#define WID_CNTL	22
#define BRKAFTER	23
#define BRKWITHIN	24
#define CMPN_HYPH	25
#define STRADDLE	26
#define ANLEVEL		27
#define ANSTREAM	28
#define TOC_DOC_NAME	29
#define SPACE_TRACK	30
#define INDENT_COUNT	31
#define WORDSPACE_MIN	32
#define WORDSPACE_NOM	33
#define WORDSPACE_MAX	34
#define LETTERSPACE_MAX	35
#define WORDSPACING	36
#define LETTERSPACING	37
#define LEFT_PROFILE	38
#define RIGHT_PROFILE	39
#define PROFILING	40
#define SUBCMPN		41
#define CMPN_CONTENTS	42
#define CMPN_TEXT	43
#define COLUMN_BREAK	44
#define DECCOMMATAB	45
#define TBL_FOOTER	46
#define TBL_HEADER	47
#define FORM_SPACE	48
#define ALCMPN_APAGE	49	/** not yet used.  BT **/
#define ALCMPN_HIDDEN	50	/** added for dmg on 10/4/88  BT **/

struct keytab al_component_tokens[] = {  /* this list must remain sorted */
    {"Align",ALIGN},
    {"Alignment",ALIGN},
    {"Allow Break After", BRKAFTER},
    {"Allow Break Within", BRKWITHIN},
    {"Allow Page Break After", BRKAFTER},
    {"Allow Page Break Within", BRKWITHIN},
    {"Attach Next",ATTNEXT},
    {"Attach Previous",ATTPRIOR},
    {"Autonumber Level",ANLEVEL},
    {"Autonumber Name",ANSTREAM},
    {"Begin New Column", COLUMN_BREAK},
    {"Begin New Page",NEWPAGE},
    {"Bottom Margin",BMARGIN},
    {"Centered Tab",CENTERTAB},
    {"Center Tab",CENTERTAB},		/* note order */
    {"Column Break", COLUMN_BREAK},
    {"Composition", FORM_SPACE},
    {"Contents", CMPN_CONTENTS},
    {"Decimal Comma Tab",DECCOMMATAB},
    {"Decimal Tab",DECIMALTAB},
    {"Fill",FILL},
    {"First Indent",INDENT},
    {"Font",FONT},
    {"Footer",TBL_FOOTER},		/* table row only */
    {"Header",TBL_HEADER},		/* table row only */
    {"Height",HEIGHT},
    {"Hidden", ALCMPN_HIDDEN},
    {"Hyphenation", CMPN_HYPH},
    {"Indent",INDENT},
    {"Indentation",INDENT},    
    {"Indent Count",INDENT_COUNT},	/* note order */
    {"Inline",SUBCMPN},
    {"Inline Component",SUBCMPN},
    {"Leading",LEADING},
    {"Left Margin",LMARGIN},
    {"Left Profile",LEFT_PROFILE},
    {"Left Tab",LEFTTAB},
    {"Letterspace Max",LETTERSPACE_MAX},
    {"Letterspacing",LETTERSPACING},
    {"Line Leading",LEADING},
    {"Line Spacing",LEADING},
    {"New Column", COLUMN_BREAK},
    {"New Page",NEWPAGE},
    {"Orphan Control",ORPH_CNTL},
    {"Profiling",PROFILING},
    {"Right Margin",RMARGIN},
    {"Right Profile",RIGHT_PROFILE},
    {"Right Tab",RIGHTTAB},
/** {"Selected",SELECTED}, not used in 4.0 docs.   BT **/
    {"Straddle",STRADDLE},
    {"Straddle Columns",STRADDLE},
    {"Subcmpn",SUBCMPN},
    {"Subcomponent",SUBCMPN},
/** {"Text",CMPN_TEXT}, not used in 4.0 docs.   BT **/
    {"TOC Doc Name",TOC_DOC_NAME},
    {"TOC Document",TOC_DOC_NAME},
    {"TOC Document Name",TOC_DOC_NAME},
    {"Top Margin",TMARGIN},
    {"Track Kern Spaces",SPACE_TRACK},
    {"Unbreakable",UNBREAK},
    {"Widow Control",WID_CNTL},
    {"Wordspace Max",WORDSPACE_MAX},
    {"Wordspace Min",WORDSPACE_MIN},
    {"Wordspace Nom",WORDSPACE_NOM},
    {"Wordspacing",WORDSPACING}
};

struct keytab al_hyph_tokens[] = {   /* this list must remain sorted! */
    {"Full",   AR_MAX_HYPHEN},
    {"Normal", AR_HYPHEN_DEFAULT},
    {"Off",    0},
    {"On",     AR_HYPHEN_DEFAULT}
};

struct keytab al_content_tokens[] = {   /* this list must remain sorted! */
    {"Init",		tok_BUF_PRIVATE},
    {"Initial",		tok_BUF_PRIVATE},
    {"Master",		tok_BUF_SHARED},
    {"Prefix",		tok_BUF_PREFIX},
    {"Prefix Shared",	tok_BUF_PREFIX_SHARED},
    {"Private",		tok_BUF_PRIVATE},
    {"Shared",		tok_BUF_SHARED},
};

/**--------------------------------------------------------------------------**/

/**	The 3define values used to be here, but I had to move them into      **/
/** 	ar5private.h.	BT						     **/

/** This comment is from TPS.  BT **/
/* NOTE: since adMain uses this list to decide which names to
 * quote on output, you must use care in REMOVING items from the
 * list, so that backward compatibility is maintained.
 * There has already been a problem with Chart and Diagram, since
 * OPS 2.5 recognizes these as commands. */

struct keytab
command_names[] = { 		/* These are the reserved words */
				/* this list must remain sorted! */
				/*  Note that blanks are missing from this
                                 * list so that the EXACT match will work
                                 * after GetWord has eliminated blanks */
    {"Autonum", AUTO_NUM},
    {"Cell", AL_CELL},
    {"Comment",COMMENT},
    {"Data",FUTURE_EXPANSION},
    {"EndFrame",END_FRAME},
    {"EndInline",END_SUB},
    {"EndSub",END_SUB},
    {"EndTable",END_TABLE},
    {"EndText",END_TEXT},
    {"FirstPageFooter",FFOOTER},
    {"FirstPageHeader",FHEADER},
    {"FJ",FLUSH_RIGHT},		/* note order */
    {"Frame",LD_FRAME},
    {"HR",HARD_RETURN},
    {"Index",INDEX_MARKUP},
    {"Language",FUTURE_EXPANSION},
    {"LeftPageFooter",LFOOTER},
    {"LeftPageHeader",LHEADER},
    {"Mail",FUTURE_EXPANSION},
    {"Master Row",AL_MASTER_ROW},
    {"NoOp",AL_NO_OP},
    {"Note",FUTURE_EXPANSION},
    {"PageFooter",FOOTER},
    {"PageHeader",HEADER},
    {"Query",FUTURE_EXPANSION},
    {"Ref",REF_MARKUP},
    {"Rev",FUTURE_EXPANSION},
    {"RightPageFooter",RFOOTER},
    {"RightPageHeader",RHEADER},
    {"Row",AL_ROW},
    {"SP",HARD_SPACE},
    {"SR",AL_SOFT_RETURN},
    {"Tab",TAB},
    {"Tab-",TAB_DASH},
    {"Tab.",TAB_DOT},
    {"Table",AL_TABLE},		/* note order! */
    {"Tab_",TAB_HYPH},
    {"|",LD_PAGEBR}		/* page break (ignored) */
};
/**--------------------------------------------------------------------------**/
/**--------------------------------------------------------------------------**/
static Cmpn_t *
ArCmpnName2Master(sP, masterP)
  shar *sP;
  Cmpn_t *masterP;	/** list of masters. **/
{
    for ( ; masterP != NULL; masterP = (Cmpn_t *)masterP->nextP) {
	if (ArShrcmp(sP, masterP->cmpn_name) == 0) {
	    break;
	}
    }
    return( masterP );
}
/**--------------------------------------------------------------------------**/
/**	cmP is a Cmpn_t allocated and initialized by the caller.  For a	     **//**	master, initialized means basics defaults.  For an instance, it	     **/
/**	means basic defaults + master properties.			     **/

Cmpn_t *ArParseCmpn(fiP, cmP)
  File_info_t *fiP;
  Cmpn_t *cmP;
{
    int c;
    int token;
    short tempshort;
    int tempint, tempint1;
    Rsu temprsu;
    bool first_profile = TRUE;

    cmP->attrvalsP = NULL;	/** always true at start of parse. **/
    ArTablineStart();

    while ((c = ArGetchNW(fiP)) != AR_CMD_TERM && c != EOF) {
	if (c == '@') {		/* indicates a component attribute */
	    arParseUserAttr(fiP, &cmP->attrvalsP);
	    continue;
	}
	else {
	    ArUnGetch(c, fiP);
	}
        switch(token = ArGetTokenEq(fiP, al_component_tokens,
				    NUM_OF(al_component_tokens))) {
	case NO_EQUALS:
	    break;
        case ALIGN :
	    if (ArGetAlign(fiP, &tempshort) != NO) {
		switch ( tempshort ) {
		case CJST:	cmP->alignment = d_BOTH;	break;
		case CCEN:	cmP->alignment = d_CENTER;	break;
		case CLFT:	cmP->alignment = d_LEFT;	break;
		case CRGT:	cmP->alignment = d_RIGHT;	break;
		default:	break;	/** could give an error **/
		}
	    }
	    break;
	case ATTNEXT:
	    ArGetYesNoShort(fiP, &cmP->att_next);
	    break;
	case ATTPRIOR:
	    ArGetYesNoShort(fiP, &cmP->att_prior);
	    /** if(al_no_att_prev) ArTellError(ERR_ATT_PREV); BT **/
	    break;
	case BMARGIN:
	    ArGetPZRsu(fiP, &cmP->bottom_margin);
#if 0	/** BT **/
	    ArGetPZRsu((type == P_ROW || type == P_MROW) ?
		       &cmpn->rbm : &cmpn->cbm);
#endif	/** BT **/
	    break;
	case CENTERTAB:
	    ArGetTab(fiP, tab_CENTER, cmP);
	    break;
	case DECIMALTAB:
	    ArGetTab(fiP, tab_DECIMAL, cmP);
	    break;
	case DECCOMMATAB:
	    ArGetTab(fiP, tab_DECIMAL_COMMA, cmP);
	    break;
	case FILL:
	    if (ArGetYesNoShort(fiP, &tempshort)) {
		if (tempshort == NO) cmP->fill = fill_OFF;
		else                 cmP->fill = fill_ON;
	    }
	    else {
		/* anything but yes or no ==>  blank */
		cmP->fill = fill_BLANK;	
	    }
	    break;
	case FONT:
	    { Font_value_t my_val;

		if (ArFontGetValue(fiP, &my_val) == YES) {
		    cmP->font_value = my_val; 
		}
		ArSkipPastComma(fiP);	/** always do this. **/
	    }
	    break;
	case HEIGHT:
	{   Rsu dummy;
	    
	    ArGetPRsu(fiP, &dummy);    /* only meaningful in diagrams/charts */
	    break;
	}
	case INDENT:
	    ArGetRsu(fiP, &cmP->first_indent);
	    break;
	case INDENT_COUNT:
	    if (ArGetInt(fiP, &tempint, YES, NO) != NO && 
		tempint >= 0 && tempint < AR_MAX_INDCNT)
	    {
		cmP->indent_count = tempint;
	    }
	    else {
	      ArTellError(ERR_INDENT_COUNT_VAL);
	      break;
	    }
	    break;
	case LEADING:
	    ArGetLines(fiP, &cmP->line_spacing);
	    break;
	case LMARGIN:
	    ArGetRsu(fiP, &cmP->left_margin);
	    /** note TPS checks this against the page left margin. **/
	    /** we accept whatever is in the document.		   **/
	    break;
	case LEFTTAB:
	    ArGetTab(fiP, tab_LEFT, cmP);
	    break;
	case NEWPAGE:
	    ArGetYesNoShort(fiP, &cmP->new_page);
	    break;
	case COLUMN_BREAK:
	    ArGetYesNoShort(fiP, &cmP->new_column);
	    break;
	case RMARGIN :
	    ArGetRsu(fiP, &cmP->right_margin);
	    /** note TPS checks this against the page left margin. **/
	    /** we accept whatever is in the document.		   **/
	    break;
	case RIGHTTAB:
	    ArGetTab(fiP, tab_RIGHT, cmP);
	    break;
        case TMARGIN :
	    ArGetPZRsu(fiP, &cmP->top_margin);
#if 0	/** BT **/
	    ArGetPZRsu((type == P_ROW || type == P_MROW) ?
		       &cmpn->rtm : &cmpn->ctm);
#endif	/** BT **/
	    break;
	case UNBREAK:
	    ArGetYesNoShortInv(fiP, &cmP->pbreak_within);
	    break;
	case WID_CNTL:
	    if (ArGetInt(fiP, &tempint, YES, NO) != NO && 
		tempint >= 0 && tempint < AR_MAX_WIDORPH) {
		    if(tempint == 0) tempint = 1;
		    cmP->widow_ctrl = tempint;
	    }
	    else {
		    ArTellError(ERR_WID_VAL);
		    ArSkipPastComma(fiP);
	    }
	    break;
	case ORPH_CNTL:
	    if (ArGetInt(fiP, &tempint, YES, NO) != NO && 
		tempint >= 0 && tempint < AR_MAX_WIDORPH) {
		    if(tempint == 0) tempint = 1;
		    cmP->orphan_ctrl = tempint;
	    }
	    else {
		    ArTellError(ERR_ORPH_VAL);
		    ArSkipPastComma(fiP);
	    }
	    break;
	case BRKAFTER:
	    ArGetYesNoShortInv(fiP, &cmP->att_next);
	    break;
	case BRKWITHIN:
	    ArGetYesNoShort(fiP, &cmP->pbreak_within);
	    break;
	case CMPN_HYPH:
	    if(ArGetInt(fiP, &tempint, YES, NO) == NO) {
		ArGetWordShrC(fiP, NOCHAR, al_token_shr, AR_MAX_WORD-1, COMMA);
		tempint = ArTokenShr(fiP, al_token_shr, al_hyph_tokens,
				     NUM_OF(al_hyph_tokens), LOOSE);
	    }
	    if(tempint < 0 || tempint > AR_MAX_HYPHEN){
		ArTellError(ERR_CMPN_HYPH);
		tempint = AR_HYPHEN_DEFAULT;
	    }
	    cmP->hyphenation = tempint;
	    break;
	case STRADDLE:
	    ArGetYesNoShort(fiP, &cmP->straddle);
	    break;
	case ANLEVEL:
	    if (ArGetInt(fiP, &tempint, YES, NO) != NO && 
		tempint >= AR_MIN_ANLEVELS && tempint < AR_MAX_ANLEVELS) {
		    /** not used in TPS yet.   BT **/
		    /** cmpn->strmlevel = (char)(tempint - 1); BT **/
	    }
	    else {
		ArTellError(ERR_AN_LEVELS);
	    }
	    break;
	case ANSTREAM:
	    if(ArGetWordShrC(fiP,NOCHAR,al_token_shr,AR_MAX_ANTAGSIZE-1,COMMA)){
		/** not used in TPS yet.   BT **/
		/** ArShrcpy( cmpn->stream, al_token_shr); BT **/
	    }
	    break;
	case SPACE_TRACK:
	    ArGetYesNoShort(fiP, &cmP->kern_space);
	    break;
	case TOC_DOC_NAME:
	    ArGetWordShrC(fiP, NOCHAR,  cmP->toc_name, AR_MAX_TOCNAME-1, COMMA);
	    break;
	case LETTERSPACING:
	    ArGetYesNoShort(fiP, &cmP->letter_spacing);
	    break;
	case LETTERSPACE_MAX:
	    cmP->letter_spacing = YES;
	    ArGetFloat(fiP, &cmP->letter_sp_max);
	    ArSkipPastComma(fiP);
	    break;
	case WORDSPACING:
	    /* the only form is "Wordspacing = Default" */
	    cmP->word_spacing = NO;
	    cmP->word_sp_max = active_default_cmpnP->word_sp_max;
	    cmP->word_sp_min = active_default_cmpnP->word_sp_min;
	    cmP->word_sp_nom = active_default_cmpnP->word_sp_nom;
	    ArSkipPastComma(fiP);
	    break;
	case WORDSPACE_MAX:
	    cmP->word_spacing = YES;
	    ArGetFloat(fiP, &cmP->word_sp_max);
	    ArSkipPastComma(fiP);
	    break;
	case WORDSPACE_MIN:
	    cmP->word_spacing = YES;
	    ArGetFloat(fiP, &cmP->word_sp_min);
	    ArSkipPastComma(fiP);
	    break;
	case WORDSPACE_NOM:
	    cmP->word_spacing = YES;
	    ArGetFloat(fiP, &cmP->word_sp_nom);
	    ArSkipPastComma(fiP);
	    break;
	case LEFT_PROFILE:
	case RIGHT_PROFILE:
	    if (!ArGetInt(fiP,&tempint,NO,NO) || tempint <= 0 || tempint>32767){
	      ArTellError(ERR_PROFILE_START);
	      ArSkipPastComma(fiP);
	    }
	    else if ((ArGetchNW(fiP) != ';') || !ArGetInt(fiP,&tempint1,NO,NO)||
	         tempint1 <= 0 || tempint1 > 32767) {
	      ArTellError(ERR_PROFILE_COUNT);
	      ArSkipPastComma(fiP);
	    }
	    else if ((ArGetchNW(fiP) != ';') || !ArGetRsu(fiP, &temprsu)) {
	      ArTellError(ERR_PROFILE_INDENT);
	    }
	    else {
	      register Cmpn_profile_t *thisP, *trailP;
	      if (first_profile) {
		  first_profile = FALSE;
		  cmP->profileP = NULL;
	      }
	      /** add this entry on end of list. **/
	      trailP = NEW(Cmpn_profile_t);
	      trailP->start_line = tempint;
	      trailP->line_count = tempint1;
	      trailP->place	= temprsu;
	      trailP->side	= (token==LEFT_PROFILE? d_LEFT : d_RIGHT);
	      trailP->nextP	= NULL;
	      if (cmP->profileP == NULL) {
		  cmP->profileP = trailP;
	      }
	      else {
		  for(thisP=cmP->profileP;thisP->nextP!=NULL;thisP=thisP->nextP)
		      { /** advance to the last one on the list. **/ }
		  thisP->nextP = trailP;
	      }
	    }
	    break;
	case PROFILING:
	    /* The only form is "Profiling = None" */
	    ArSkipPastComma(fiP);
	    cmP->profileP = NULL;
	    break;
	case SUBCMPN:
	    tempshort = FALSE;
	    ArGetYesNoShort(fiP, &cmP->subcmpn);
	    break;
	case CMPN_CONTENTS:
	    ArGetWordShrC(fiP, NOCHAR,  al_token_shr, AR_MAX_WORD-1, COMMA);
	    switch ( tempint = ArTokenShr(fiP,  al_token_shr, al_content_tokens,
					   NUM_OF(al_content_tokens), LOOSE) ){
	    case tok_BUF_PRIVATE:
	    case tok_BUF_PREFIX:
	    case tok_BUF_SHARED:
	    case tok_BUF_PREFIX_SHARED:
		break;
	    default:
		ArTellError( ERR_CMPN_CONTENTS);
		tempint = tok_BUF_PRIVATE;
		break;
	    }
	    cmP->content_type = tempint;
	    break;
#if 0	/** BT **/
	case CMPN_TEXT:
	    /* For a short time, when a component master (i.e. a Class) had 
	     * contents, it was dumped in a form like this:
	     <!Class, foo,
	         Left Margin = ...,
		 ...,
		 Text = Here are the contents of the master component.<HR>
		 The end.
		 <End Text> >
	     * There probably are no documents around with this, since it
	     * soon was changed. */

	    if(type == P_DEFAULT) {
		/* ArTellError(); */
		break;
	    }
	    {
		CMPN* temp = al_cmpn;
		CMPN* stemp = al_subcmpn;
		al_cmpn = cmpn;
		ArCmpnInit();
		(void)ArMainLoop(TRUE);
		al_cmpn = temp;
		al_subcmpn = stemp;
	    }
	    break;
These are relevant only when loading a Table Cell command, which uses
the same keyword table.
	case TBL_HEADER:
	    ArGetYesNoShort(fiP, &cmP->table_header);
	    break;
	case TBL_FOOTER:
	    ArGetYesNoShort(fiP, &cmP->table_footer);
	    break;
#endif	/** BT **/
	case FORM_SPACE:
	    ArGetFormSpace(fiP, &cmP->composition);
	    break;
	case ALCMPN_HIDDEN:
	    ArGetYesNoShort(fiP, &cmP->hidden);
	    break;
        default      :
	    cmP->unkP = 
		(Unknown_t *)ArBuildUnknownPropEq(fiP, al_token_shr, cmP->unkP);
	    ArSkipPastComma(fiP);
	    break;
	}
    }

    ArTablineFinish(&cmP->tabs);

    return( cmP );
}
/**--------------------------------------------------------------------------**/
bool
ArBufGetCmd(fiP, topLevel, ssP, ilP)
  File_info_t *fiP;
  bool topLevel;
  Ssave_t *ssP;
  Il_state_t *ilP;
{
  int tempint;
  NullObj_t *thingP;
  Token_t *tok_cmP;

    tempint = ArGetWordShrC(fiP, NOCHAR, al_token_shr, AR_MAX_WORD-1, 0);
    if (tempint == 0) {
	/** failure **/
	ArSkipPastTerm(fiP);
	return( NO );
    }
    else if (tempint == AR_QUOTED) {
	/** a component name.  must be a subcmpn. **/
	thingP = ArBuildCmpnName(fiP, al_token_shr, topLevel, NULL, ilP);
	if (thingP->id == ID_Cmpn) {
	    if (((Cmpn_t *)thingP)->subcmpn == NO) {
		/** this WAS just a sanity check. **/
		/** ArFprintf(stderr,
			"ArBufGetCmd: subcmpn confusion. Get help\n"); BT **/
		arObjectPushBack( thingP );
		return( NO );
	    }
	    tok_cmP = NEW(Token_t);
	    tok_cmP->token = TOKEN_SET(tok_Cmpn);
	    tok_cmP->u.subcmpnP = (Cmpn_t *)thingP;
	    /** make caller's buffer refer to this item. **/
	    BufEnter(tok_cmP, ssP);
	    return(YES);
	}
	else {
	  shar c;
	    ArFprintf(stderr, "ArBufGetCmd confusion.  Get help.\n");
	    ArFprintf(stderr, "Here is the rest of the input file:\n");
	    while ((c = ArGetch(fiP)) != EOF) {
		ArFprintf(stderr, "%#C", c);
	    }
	    exit( -1 );
	}
    }
    /** else its an unquoted cmpn name or a  normal command like <Tab>  **/
    /**      or an abnormal command like <PageHeader>		        **/
    if ((tempint = ArTokenShr(fiP, 
				al_token_shr, 
				command_names, 
				NUM_OF(command_names), 
			        LOOSE)) == -1) {
	/** a component name.  must be a subcmpn. **/
	thingP = ArBuildCmpnName(fiP, al_token_shr, topLevel, NULL, ilP);
	if (thingP->id == ID_Cmpn) {
	    if (((Cmpn_t *)thingP)->subcmpn == NO) {
		/** this WAS just a sanity check. **/
		/** ArFprintf(stderr,
			"ArBufGetCmd: subcmpn confusion. Get help\n"); BT **/
		arObjectPushBack( thingP );
		return( NO );
	    }
	    tok_cmP = NEW(Token_t);
	    tok_cmP->token = TOKEN_SET(tok_Cmpn);
	    tok_cmP->u.subcmpnP = (Cmpn_t *)thingP;
	    /** make caller's buffer refer to this item. **/
	    BufEnter(tok_cmP, ssP);
	    return(YES);
	}
    }
    /** finally, it is a plain command.  build its token. **/
    switch( tempint ) {
	default: 
	    ArFprintf(stderr,
		"ArBufGetCmd(): unknown token type %d, al_token_shr == %#S\n",
		tempint, al_token_shr);
	    break;
	case FN:
	    /** al_font_inherit and al_attrs were set globally by ArTokenShr **/
	    /** ArFprintf(stderr, "Getting a font value.\n"); BT **/
	    return( arBufGetFontCmd(fiP, ssP) );
	    break;
	case  AUTO_NUM:
	    return( arBufGetAutonum(fiP, ssP) );
	    break;
	case  LD_FRAME:
	    return( arBufGetFrame(fiP, ssP) );
	    break;
	/** I diverge from TPS usage here.  TPS overloads END_TEXT for 	     **/
	/** subcmpn endings AND for the end of a microdoc.  This is a	     **/
	/** problem for me, to distinguish 1) a cmpn in a microdoc, that is  **/
	/** empty, followed by an <ENdText> to rterminate the microdoc, from **/
	/** 2) a subcmpn that has its tag, a blank line, and an <EndSub> to  **/
	/** terminate the subcmpn, regardless of its surrounding context.    **/
	case END_SUB:
	    ArSkipPastTerm(fiP);/** should be right there. **/
	    return( NO );	/** normal end of component body. **/
	    break;
	case END_TEXT:
	    ArUnGetshr(al_token_shr, fiP);
	    ArUnGetch(AR_CMD_CHAR, fiP);
	    return( NO );  /** end of component body, which may be empty **/
	    break;
	case TAB:
	case TAB_DASH:
	case TAB_DOT:
	case TAB_HYPH:
	    return( arBufGetTab(fiP, tempint, ssP) );
	    break;
	case LD_PAGEBR:
	    return( arBufGetPageBreak(fiP, ssP) );
	    break;
	case COMMENT:
	    return( arBufGetComment(fiP, ssP) );
	    break;
	case FLUSH_RIGHT:
	    return( arBufGetFJ(fiP, ssP) );
	    break;
	case HARD_RETURN:
	    return( arBufGetHR(fiP, ssP) );
	    break;
	case HARD_SPACE:
	    return( arBufGetSP(fiP, ssP) );
	    break;
	case AL_SOFT_RETURN:
	    return( arBufGetSR(fiP, ssP) );
	    break;
	case AL_NO_OP:
	    return( arBufGetNoOp(fiP, ssP) );
	    break;
	case INDEX_MARKUP:
	    return( arBufGetIndex(fiP, ssP) );
	    break;
	case REF_MARKUP:
	    return( arBufGetRef(fiP, ssP) );
	    break;
	/**-----------------------------------**/
	case FFOOTER:
	case FHEADER:
	case LFOOTER:
	case LHEADER:
	case FOOTER:
	case HEADER:
	case RFOOTER:
	case RHEADER:
	case AL_TABLE:
	case AL_CELL:
	case END_TABLE:
	case AL_MASTER_ROW:
	case AL_ROW:
	case END_FRAME:		/**  obsolete **/
	case FUTURE_EXPANSION:  /** might as well treat it as a terminator.  **/
	    ArUnGetshr(al_token_shr, fiP);
	    ArUnGetch(AR_CMD_CHAR, fiP);
	    return( NO );
	    break;
	/**-----------------------------------**/
#if 0	/** BT **/
	case AL_TABLE:
	    /** a table is a top-level object, 		 **/
	    /** therefore the end of the current thing.  **/
	    { NullObj_t *tableP;
		tableP = ArBuildTable(fiP, NO);
		if (tableP != NULL) {
		    arObjectPushBack(tableP);
		}
	    }
	    return( NO );
	    break;
	/**-----------------------------------**/
	case AL_CELL:
	case END_TABLE:
	case AL_MASTER_ROW:
	case AL_ROW:
	case END_FRAME:		/**  obsolete **/
	case FUTURE_EXPANSION:
  char *err_msg = "ArBufGetCmd: markup type %d not supported yet.\n";
	    ArFprintf(stderr,err_msg,tempint);
	    ArSkipPastTerm(fiP);
	    break;
#endif	/** BT **/
    }
    return( NO ); /** if we get here, we're in trouble. **/
}
/**--------------------------------------------------------------------------**/
Ssave_t ArCmpnGetBody(fiP, topLevel, tkP, cmpn_fill, ilP)
  File_info_t *fiP;
  bool         topLevel;
  Token_t     *tkP;
  bool         cmpn_fill;
  Il_state_t  *ilP;
{
  Ssave_t buf;
  shar c;
  int nl_count;

    nl_count = 0;
    buf = SsInitEmpty();

    if (tkP != NULL) {
	/** save a look-ahead Token_t that somebody built for us. **/
	BufEnter(tkP, &buf);
    }

#ifdef BAD
    c = ArGetchNW(fiP);	   /** advance to first text unit in the cmpn body. **/
#else
    while(c = ArGetch(fiP)) {
      if(c == EOF || ald_chtype[c] != WHT)
	break;
    }
#endif

    ArUnGetch(c, fiP);
    while ((c = ArGetch(fiP)) != EOF) {
	if (c == AR_CMD_CHAR) {
	    nl_count = 0;
	    if ((c = ArGetch(fiP)) == AR_CMD_CHAR) {
		/** it is '<<' **/
		/** does it need a token of its own?  check ArFprintf. **/
		if (SsSaveS(AR_CMD_CHAR, &buf) == NO) {
		    return( buf );
		}
	    }
	    else if (c == AR_DECL_CHAR) {
		/** we have gone beyond the master-content into the next **/
		/** declaration.					 **/
		ArUnGetch(AR_DECL_CHAR, fiP);
		ArUnGetch(AR_CMD_CHAR, fiP);
		break;
	    }
	    else if (c == AR_HEX_CHAR) {
		tkP = NEW(Token_t);
		tkP->token = TOKEN_SET(tok_CharSpecial);
		tkP->u.token_data = ArDoHex(fiP);  /** note ArDoHex eats '>' **/
		BufEnter(tkP, &buf);
		nl_count = 0;
	    }
	    else {
		/** it is a Header or Footer, a cmpn instance, (quoted or not)
		    or a markup command. **/
		ArUnGetch(c, fiP);
		if (ArBufGetCmd(fiP, topLevel, &buf, ilP) == NO) {
		    /** the end of the cmpn body, or failure of some sort. **/
		    break;
		}
	    }
	}
	else if (c == '\n') {
	    /** two sequential newlines terminates the component. **/
	    /** don't ever save a newline directly in the buffer. **/
	    if (++nl_count == 2) {
		break;
	    }
	    else {
		if (cmpn_fill == fill_BLANK) {
		    if (SsSaveS(' ', &buf) == NO) {
			return( buf );
		    }
		}
		/** else discard it.  **/
	    }
	}
	else {
	    /** it is a plain text character **/
	    nl_count = 0;
	    if (c < 0x20 || c > 0x7e) {
		/** call it special; it is outside the printable range **/
		tkP = NEW(Token_t);
		tkP->token = tok_CharSpecial;
		tkP->u.token_data = c;
		BufEnter(tkP, &buf);
		ArSkipPastTerm(fiP);
	    }
	    else /** it is as ordinary as any character can be. **/
	    if (SsSaveS(c, &buf) == NO) {
		return( buf );
	    }
	}
    }
    return( buf );
}
/**--------------------------------------------------------------------------**/
NullObj_t *
ArBuildClass(fiP, ilP)
  File_info_t *fiP;
  Il_state_t *ilP;
{
  Cmpn_t *cmP = NEW(Cmpn_t);

    ArCmpnInit(cmP);	/** set defaults, std. or metric **/
    SET_NullObjFields(cmP, ID_MasterCmpn);

    if (ArGetWordShrC(fiP,NOCHAR,cmP->cmpn_name,AR_MAX_WORD-1,COMMA)==0){
	ArCmpnFree(cmP);
	ArFprintf(stderr, "ArBuildClass(): error reading cmpn name.\n");
	return( NULL );
    }
    if (ArParseCmpn(fiP, cmP) != cmP) {
	ArCmpnFree(cmP);
	ArFprintf(stderr, "ArBuildClass(): error parsing component.\n");
	return( NULL );
    }
    if (ARdebug) {
      Attr_t *listP;
      int length = 0;
	for (listP = cmP->attrvalsP;listP&&listP->nextP;listP=listP->nextP){
	    /** advance to end of list **/
	    if (ARdebug) {
		ArFprintf(stderr, 
		    "ArBuildClass[%Q]: length %d, nameP %Q, stringP%Q\n",
		    cmP->cmpn_name, length, listP->nameP, listP->stringP);
	    }
	    /**/length++;
	}
    }
    cmP->body = ArCmpnGetBody(fiP, NO, NULL, cmP->fill, ilP);

    /** ArFprintf(stderr, "Adding Class %Q.\n", cmP->cmpn_name); BT **/
    return( (NullObj_t *)cmP );
}
/**--------------------------------------------------------------------------**/
NullObj_t *
ArBuildClassDef(fiP, ilP)
  File_info_t *fiP;
  Il_state_t *ilP;
{ 
  Cmpn_t *cmP = NEW(Cmpn_t);

    ArCmpnInit(cmP);	/** set defaults, std. or metric **/
    SET_NullObjFields(cmP, ID_ClassDef);

    if (ArParseCmpn(fiP, cmP) != cmP) {
	ArCmpnFree(cmP);
	ArFprintf(stderr, "ArBuildClassDef(): error parsing component.\n");
	return( NULL );
    }
    /** slightly controversial **/
    cmP->body = ArCmpnGetBody(fiP, NO, NULL, cmP->fill, ilP);
    ArCmpnSetDefaults(cmP);

    return( (NullObj_t *)cmP );
}
/**--------------------------------------------------------------------------**/
/**	this function is called with a '<' as the next NW character in the   **/
/**	input stream, beginning the instance tag for a component.	     **/
#if 0	/** BT **/
NullObj_t *
ArBuildCmpn(fiP, ilP)
  File_info_t *fiP;
  Il_state_t *ilP;
{
  shar c;
  Cmpn_t *cmP;
  Cmpn_t *masterP;

    if ((c = ArGetchNW(fiP)) != Lmarkup) {
	ArUnGetch(c, fiP);
	ArFprintf(stderr, "ArBuildCmpn(): expected '<', got %#C.\n", c);
	return( NULL );
    }
    cmP = NEW(Cmpn_t);
    ArCmpnInit(cmP);	/** set defaults, std. or metric **/
    if (ArGetWordShrC(fiP, NOCHAR, cmP->cmpn_name, AR_MAX_CMPNNAME-1, COMMA)
	== NO) {
	    ArCmpnFree(cmP);
	    ArFprintf(stderr, "ArBuildCmpn(): error reading cmpn name.\n");
	    return( NULL );
    }
    if((masterP=ArCmpnName2Master(cmP->cmpn_name,(ilP?ilP->cmpnP:NULL)))!=NULL){
	*cmP = *masterP;	/** set properties from the master **/
    }
    SET_NullObjFields(cmP, ID_Cmpn);
    if (ArParseCmpn(fiP, cmP) != cmP) {
	ArCmpnFree(cmP);
	ArFprintf(stderr, "ArBuildCmpn(): error parsing component.\n");
	return( NULL );
    }
    cmP->body = ArCmpnGetBody(fiP, NO, NULL, cmP->fill, ilP);

    /** ArFprintf(stderr, "Adding Cmpn %Q.\n", cmP->cmpn_name); BT **/
    return( (NullObj_t *)cmP );
}
#endif	/** BT **/
/**--------------------------------------------------------------------------**/
NullObj_t *
ArBuildCmpnName(fiP, sP, topLevel, prevP, ilP)
 File_info_t *fiP;
 shar *sP;
 bool topLevel;
 Cmpn_t *prevP;
 Il_state_t *ilP;
{
  shar     c;
  Cmpn_t  *cmP;
  Cmpn_t  *masterP;
  Cmpn_t  *subcmpnP;
  Token_t *tkP;
  char *err_msg = "ArBuildCmpnName(%#S): subcomponent confusion.  Get help.\n";

    cmP = NEW(Cmpn_t);
    ArCmpnInit(cmP);	/** set defaults, std. or metric **/
    if ((masterP = ArCmpnName2Master(sP, (ilP? ilP->cmpnP : NULL))) != NULL) {
	*cmP = *masterP;	/** set properties from the master. **/
    }
    ArShrcpy(cmP->cmpn_name, sP);
    SET_NullObjFields(cmP, ID_Cmpn);
#if 0	/** BT **/
    ArSkipPastComma(fiP);	/** should be right on top of it. **/
#else
    if ((c = ArGetchNW(fiP)) != COMMA) {
	ArUnGetch(c, fiP);
    }
#endif	/** BT **/
    if (ArParseCmpn(fiP, cmP) != cmP) {
	ArCmpnFree(cmP);
	ArFprintf(stderr, "ArBuildCmpnName(): error parsing component.\n");
	return( NULL );
    }

    /** now, we recover from the exquisitely bad design of IAM.  BT **/
    if (topLevel == YES && cmP->subcmpn == YES) {
	/**  Shuffle things and continue, with topLevel == NO.   **/
	subcmpnP = cmP;
	subcmpnP->body = ArCmpnGetBody(fiP, NO, NULL, subcmpnP->fill, ilP);
	tkP = NEW(Token_t);
	tkP->token = TOKEN_SET(tok_Cmpn);
	tkP->u.subcmpnP = subcmpnP;
	cmP = NEW(Cmpn_t);
	if (prevP == NULL) {
	    ArFprintf(stderr, err_msg, sP); /** we're screwed. **/
	    ArFprintf(stderr, "Here is the rest of the input file:\n");
	    while ((c = ArGetch(fiP)) != EOF) {
		fputc(c, stderr);
	    }
	    exit(-1);
	}
	else {
	    *cmP = *prevP;
	}
	/** this is the only call to ArCmpnGetBody with a non-NULL tokenP **/
	cmP->body = ArCmpnGetBody(fiP, NO, tkP, cmP->fill, ilP);
    }
    else {
	/** note that in this body, it is no longer at topLevel. **/
	cmP->body = ArCmpnGetBody(fiP, NO, NULL, cmP->fill, ilP);
    }

    /** ArFprintf(stderr, "Adding Cmpn %Q.\n", cmP->cmpn_name); BT **/
    return( (NullObj_t *)cmP);
}
/**--------------------------------------------------------------------------**/
NullObj_t *
ArBuildCmpnShar(fiP, s, prevP, ilP)
 File_info_t *fiP;
 shar s;
 Cmpn_t *prevP;
 Il_state_t *ilP;
{
  Cmpn_t *cmP = NEW(Cmpn_t);

    if (prevP != NULL) {
	*cmP = *prevP;		/** set properties from predecessor **/
    }
    else {
	ArCmpnInit(cmP);
    }
    SET_NullObjFields(cmP, ID_Cmpn);
    ArUnGetch(s, fiP);	/** easy way out; push back the starting shar. **/
    cmP->body = ArCmpnGetBody(fiP, NO, NULL, cmP->fill, ilP);
    /** ArFprintf(stderr, "Adding Cmpn %Q.\n", cmP->cmpn_name); BT **/
    return( (NullObj_t *)cmP);
}
/**--------------------------------------------------------------------------**/
NullObj_t *
ArBuildCmpnCmd(fiP, cmdP, prevP, ilP)
 File_info_t *fiP;
 short *cmdP;
 Cmpn_t *prevP;
 Il_state_t *ilP;
{ 
  Cmpn_t *cmP = NEW(Cmpn_t);

    if (prevP != NULL) {
	*cmP = *prevP;		/** set properties from predecessor **/
    }
    else {
	ArCmpnInit(cmP);
    }
    SET_NullObjFields(cmP, ID_Cmpn);

    /** these two lines are the easy way out for now. **/
    ArUnGetshr(cmdP, fiP);
    ArUnGetch('<', fiP);

    cmP->body = ArCmpnGetBody(fiP, NO, NULL, cmP->fill, ilP);
    /** ArFprintf(stderr, "Adding Cmpn %Q.\n", cmP->cmpn_name); BT **/
    return( (NullObj_t *)cmP);
}
/**--------------------------------------------------------------------------**/
Microdoc_t *
ArBuildMicrodoc(fiP)
  File_info_t *fiP;
{
  Microdoc_t *mdP = NEW(Microdoc_t);
  register NullObj_t *thingP;
  register NullObj_t **trailPP;
  Il_state_t *ilP = AR_il_stateP;
  Cmpn_t *Save_prevP;
  shar c;
  bool   done;
  int get_ret;

    Save_prevP      = AR_prev_cmpnP;
    AR_prev_cmpnP   = NULL;
    ArPageInfoSetContext(pg_MICROCONTEXT);

    thingP = ArNextObject(fiP, ilP);
    if (Identify(thingP) == ID_Page) {
	/** save the microdoc page properties. **/
	mdP->micro_page = *(Page_info_t *)thingP;
	/** build the list of contents. **/
	trailPP = &mdP->contentsP;
	while ( (thingP = arObjectPreFetch()) != NULL ){
	   *trailPP = thingP;
	   trailPP = &thingP->nextP;
	}
	if ( al_end_text ){
	}else{
	   done = FALSE;
	   do {
	      while ((thingP = ArNextObject(fiP, ilP)) != NULL ){
	         *trailPP = thingP;
	         trailPP = &thingP->nextP;
	         while ( (thingP = arObjectPreFetch()) != NULL ){
	            *trailPP = thingP;
	            trailPP = &thingP->nextP;
		 }
	         if ( al_end_text ){
		    done = TRUE;
		    break;
	         }else{
	            /* have to hack our way out of text back into diagramming.*/
	            c = ArGetchNW(fiP);
	            if ( c == AR_CMD_CHAR ){
		       get_ret = ArGetWordShrC(fiP, NOCHAR,al_token_shr,
					       AR_MAX_WORD-1, NOCHAR);
		       if ( ArShrstrcmp(al_token_shr, "EndText") == 0 ){
		          /** this is the end of the microdoc. **/
		          ArSkipPastTerm(fiP);/** should be right there. **/
		          done = TRUE;
		          break;
		       }else{
			  if(get_ret == AR_QUOTED)      /* don't lose quotes. CK */
			    ArUnGetch('"', fiP);
		          ArUnGetshr(al_token_shr, fiP);
			  if(get_ret == AR_QUOTED)      /* don't lose quotes. CK */
			    ArUnGetch('"', fiP);
		          ArUnGetch(AR_CMD_CHAR, fiP);
		       }
	            }else{
		       ArUnGetch(c, fiP);
	            }
	         }
	      }
	   }while ( !done );
	}

#ifdef NEVER
	while ((thingP = ArNextObject(fiP, ilP)) != NULL) {
	    *trailPP = thingP;
	    trailPP = &thingP->nextP;
	    /** have to hack our way out of text back into diagramming. **/
	    c = ArGetchNW(fiP);
	    if (c == AR_CMD_CHAR) {
		ArGetWordShrC(fiP, NOCHAR, al_token_shr, AR_MAX_WORD-1, NOCHAR);
		if (ArShrstrcmp(al_token_shr, "EndText") == 0) {
		    /** this is the end of the microdoc. **/
		    ArSkipPastTerm(fiP);	/** should be right there. **/
		    break;
		}
		else {
		    ArUnGetshr(al_token_shr, fiP);
		    ArUnGetch(AR_CMD_CHAR, fiP);
		    continue;
		}
	    }
	    else {
		ArUnGetch(c, fiP);
	    }
	}
#endif

    }else{
       free( mdP );
       mdP = NULL;
    }
    AR_prev_cmpnP = Save_prevP;
    ArPageInfoSetContext(pg_MACROCONTEXT);
    /** ArFprintf(stderr, "Adding Microdoc\n"); BT **/
    return( mdP );
}
/**--------------------------------------------------------------------------**/
/**--------------------------------------------------------------------------**/
int al_tabcount;		    /* number of tabs in arrays below */
				    /* -1 means no tabs mentioned
				     *  0 means empty set of tabs */
Tab_type_t al_tabtypes[AR_MAX_TABS];
Rsu        al_tabstops[AR_MAX_TABS];


/*  tab syntax:
 *    Left Tab = 1 inch,
 *    Right Tab = 1/2/3/4/5 inches,
 *    Decimal Tab = 1/1*4 inches, ( = 1/2/3/4/5)
 *    Center Tab = 1*4 ( = 0/1/2/3/4)
 *    Left Tab = , (This is used when there are NO tabs. If nothing
 *		    is mentioned at all, that means default set of tabs.)
 */

/**--------------------------------------------------------------------------**/
/* This ADDS some tabstops to the arrays. ArTabline puts the set
 * into a cmpn and resets al_tabcount. */

ArGetTab(fiP, ttype, cmP)
  File_info_t *fiP;
  Tab_type_t ttype;
  Cmpn_t *cmP;
{
    register int k;
    register int i;
    register int j;
    register int n;
    int c;
    int nstar;
    Rsu stop;
    int units;
    float step;
    float rawtab[AR_MAX_TABS]; /* before units conversion */
    
    if(al_tabcount == -1) al_tabcount = 0;
    for(n=0; n<AR_MAX_TABS; ) {
	if(!ArGetFloat(fiP, &rawtab[n])) {
	    /* The "no tabs" case comes here. We have already
	     * set al_tabcount to zero. */
	    ArSkipPastComma(fiP);
	    return;
	}
	switch(c = ArGetchNW(fiP)) {
	case '/': ++n; continue;
	case '*': {
	    if((step = rawtab[n]) < 0.01 
		|| !ArGetInt(fiP, &nstar, TRUE, NO) 
		|| nstar < 1) {
		ArSkipPastComma(fiP);
		return;
	    }
	    if(n == 0)rawtab[n++] = 0.0;
	    while(nstar--) {
		if(n >= AR_MAX_TABS) goto toomany_tabs;
		rawtab[n] = rawtab[n-1] + step;
		++n;
	    }
	    if((c = ArGetchNW(fiP)) == '/') break;
	    --n;
	}
	default : ArUnGetch(c, fiP); goto getunits;
	}
    }

toomany_tabs:
    ArSkipPastTab(fiP);

getunits:
    if((units = ArGetUnitMeas(fiP)) == -1){
	return;
    }
    
    for(j=0; j<=n; ++j) {
	stop = ArCvtUnits(units, rawtab[j]);
	for(i=0; i<al_tabcount; ++i) { /* we sort list as we build */
	    if(stop <= al_tabstops[i]) break;
	}
	if(i < al_tabcount && stop == al_tabstops[i]) {
	    /* diff tab at same stop? */
	    continue;
	}
	if(al_tabcount >= AR_MAX_TABS) {
	    return;
	}
	for(k=al_tabcount; k>i; --k) {
	    al_tabstops[k] = al_tabstops[k-1];
	    al_tabtypes[k] = al_tabtypes[k-1];
	}
	al_tabstops[i] = stop;
	al_tabtypes[i] = ttype;
	++al_tabcount;
    }
}

/**--------------------------------------------------------------------------**/
ArSkipPastTab(fiP)
  File_info_t *fiP;
{
    register int c;
    
    while(c = ArGetchNW(fiP)) {
	if(c >= '0' && c <= '9') continue;
	switch(c) {
	case '-':
	case '.':
	case '/':
	case '*':
	    break;
	default :
	    ArUnGetch(c, fiP);
	    return;
	}
    }
}

/**--------------------------------------------------------------------------**/
ArTablineStart()
{
    al_tabcount = -1;
}

/**--------------------------------------------------------------------------**/
/* Output the accumulated tabline */

char* ad_tab_types[4] = {
",\n\tLeft Tab =\t\t",
",\n\tCenter Tab =\t\t",
",\n\tRight Tab =\t\t",
",\n\tDecimal Tab =\t\t"
};

/**--------------------------------------------------------------------------**/
/*
 *  Note that we currently always output 2 decimal places
 *  for tabs, though we could lose a hair when the tabs
 *  were input (from ascii only) not as 0.01 multiples.
 */
ArTablineFinish(tlP)
  register Tabline_t *tlP;
{
  register int    i;
    
    if(al_tabcount >= 0) {
	tlP->num_stops = al_tabcount;
	for(i = 0; i < al_tabcount; ++i) {  
	    /* loop for all tabs */
	    tlP->entries[i].stop = al_tabstops[i];
	    tlP->entries[i].type = al_tabtypes[i];
	}
    }
    /**	else no explicit tabs, setting from the master should prvail. **/
}

/**--------------------------------------------------------------------------**/
NullObj_t *
ArBuildFromCmd(fiP, cmd, word, ilP)
  File_info_t *fiP;
  short cmd;
  shar *word;
  Il_state_t *ilP;
{
  Cmpn_t *my_cmP;

    switch( cmd ) {
    case  END_TEXT:
          al_end_text = TRUE;
	  return(NULL);
    case  HEADER: case FHEADER: case LHEADER: case RHEADER:
    case  FOOTER: case FFOOTER: case LFOOTER: case RFOOTER:
	/** Header & footer are "commands", even though they occur **/
	/** once in a document.  Build it and return it. **/
	return( ArBuildHeaderFooter(fiP, cmd) );
	break;
    case AL_TABLE:
	/**it's a table, which is a top-level object. build it and return it.**/
	return( (NullObj_t *)ArBuildTable(fiP, ilP, NO) );
	break;
    default:
	/** it's a component with a command at its beginning,   **/
	/** e.g. <Tab>.  Build it and return it.		**/
	my_cmP=(Cmpn_t*)ArBuildCmpnCmd(fiP, word, AR_prev_cmpnP, ilP);
	/** ArFprintf(stderr, "Adding Cmpn %Q.\n", my_cmP->cmpn_name); BT **/
	return( (NullObj_t *)my_cmP );
	break;
    }
    /* NOTREACHED */
}
/**--------------------------------------------------------------------------**/
int
ArIsCommandName(fiP, word, precision)
  File_info_t *fiP;
  shar *word;
  int precision;
{
    return(ArTokenShr(fiP,word,command_names,NUM_OF(command_names),precision));
}
/**--------------------------------------------------------------------------**/
static void ArCmpnProfileFree(profP)
  Cmpn_profile_t *profP;
{
  Cmpn_profile_t *nextP;

    for ( ; profP != NULL; ) {
	nextP = profP->nextP;
	FREE((char *)profP);
	profP = nextP;
    }
}
/**--------------------------------------------------------------------------**/
#if 0	/** BT **/
static void ArCmpnTableFree(tblP)
  Table_t *tblP;
{
    if (tblP != NULL) {
	ArFprintf(stderr, "ArCmpnTableFree(): non-NULL argument. Get help.\n");
	exit( -1 );
    }
}
#endif	/** BT **/
/**--------------------------------------------------------------------------**/
bool ArBodyFree(bodyP)
  Ssave_t *bodyP;
{
  int i;
  shar *sP;

    if (bodyP == NULL) {
	return( YES );
    }
    for (i = SsAmount(bodyP), sP = SsBufStart(bodyP); i > 0; ) {
	if (IS_TOKEN(*sP) && !IS_PADDING(*sP)) {
	    if (ArTokenFree(sP) == NO) {
		return( NO );
	    }
	    sP += sizeof(Token_t)/sizeof(shar);/**bump pointer past token**/
	    i  -= sizeof(Token_t)/sizeof(shar);/**and update count	     **/
	    continue;
	}
	else {
	    /** there is nothing to free. **/
	    i--;
	    sP++;
	    continue;
	}
    }
    SsFree(bodyP);
    return( YES );
}
/**--------------------------------------------------------------------------**/
bool ArCmpnFree(cmP) 
  Cmpn_t *cmP;
{ 
    if (cmP == NULL) {
	return( YES );
    }
    /** extra security check. **/
    switch (Identify(cmP)) {
	case ID_MasterCmpn:
	case ID_ClassDef:
	case ID_Cmpn:
	    /** it's cool. **/
	    break;
	default:
	    return( NO );
	    break;
    }
    if (ArObjectFree(cmP->nextP) == YES) {
	ArUnknownFree(cmP->unkP);
	ArAttrFree(cmP->attrvalsP);
	ArCmpnProfileFree(cmP->profileP);
	/** ArCmpnTableFree(cmP->tableP); BT **/
	if (ArBodyFree(&cmP->body) == YES) {
	    FREE((char *)cmP);
	    return( YES );
	}
    }
    return( NO );
}
/**--------------------------------------------------------------------------**/
