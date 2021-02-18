
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
/*	ar5index.c                               */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  08/88    BT	Original version
 */

/**--------------------------------------------------------------------------**/
/**	These routines build an index token to put in a Cmpn_t buffer.	     **/
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
#define DOC_KEYWORD		0
#define SORT_STRING_KEYWORD	1
#define TYPEFACE_KEYWORD	2
#define THIS_PAGE_KEYWORD	3
#define TO_NEXT_KEYWORD		4
#define COUNT_KEYWORD		5
#define NAMED_CMPN_KEYWORD	6
#define SEE_KEYWORD		7
#define IGNORE_TO_EQUAL		8
#define DICT_KEYWORD		9

static struct keytab	index_inst_keywords[] = {
    { "Count",		COUNT_KEYWORD },
    { "Dictionary",	DICT_KEYWORD },
    { "Doc",		DOC_KEYWORD },
    { "Range",		IGNORE_TO_EQUAL },
    { "See",		SEE_KEYWORD },
    { "See Also",	SEE_KEYWORD },
    { "Sort",		SORT_STRING_KEYWORD },
    { "Sort String",	SORT_STRING_KEYWORD },
    { "This Page",	THIS_PAGE_KEYWORD },
    { "To Named",	NAMED_CMPN_KEYWORD },
    { "To Next",	TO_NEXT_KEYWORD },
    { "Typeface",	TYPEFACE_KEYWORD },
};


#define ROMAN_KEYWORD		0
#define ITALIC_KEYWORD		1
#define BOLD_KEYWORD		2

static struct keytab	index_typeface_keywords[] = {
    { "Bold",		BOLD_KEYWORD },
    { "Italic",		ITALIC_KEYWORD },
    { "Italics",	ITALIC_KEYWORD },
    { "Normal",		ROMAN_KEYWORD },
    { "Roman",		ROMAN_KEYWORD },
};


/**--------------------------------------------------------------------------**/
/**	We assume that each heading level is immediately followed by its     **/
/**	sort string entry *if it has one*, and so the two counters will	     **/
/**	correctly increment, independent of each other.			     **/
/**	There can never be more sort strings than headings.		     **/

bool
arBufGetIndex(fiP, ssP)
  File_info_t *fiP;
  Ssave_t *ssP;
{
  Token_t *tkP;
  Index_t *inP;
  register int	keyword;
  short		sbuf[AR_MAX_QSTR];
  int		cur_level       = 0;
  int		cur_sort_string = 0;
  short 	cmpn_count      = 0;


    tkP		  = NEW(Token_t);
    tkP->token    = TOKEN_SET(tok_Index);
    inP           = NEW(Index_t);
    tkP->u.indexP = inP;
    *inP = default_index_entry;

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
	    inP->headings[cur_level] = 
		    (shar *)ArSafeMalloc(sizeof(shar)*(1+ArShrlen(sbuf)));
	    ArShrcpy(inP->headings[cur_level], sbuf);
	    cur_level++;
	    continue;
	case AL_TEXT_OK:
	    break;
	}
TRY_AGAIN:
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
	keyword = ArTokenShr( fiP, sbuf, index_inst_keywords,
			     NUM_OF(index_inst_keywords), LOOSE);
	if ( keyword < 0 ){
	    /** there used to be some ecrit stuff here.  BT **/
	    continue;
	}
	/* Get the argument for those that need it. */
	switch ( keyword ){
	case DICT_KEYWORD:		
	case DOC_KEYWORD:		
	case SORT_STRING_KEYWORD:
	case SEE_KEYWORD:
	case TYPEFACE_KEYWORD:
	case IGNORE_TO_EQUAL:
	case COUNT_KEYWORD:
	case NAMED_CMPN_KEYWORD:
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
	}

	/* If changing the scope, dispose of any previous value. */
	switch ( keyword ){
	case THIS_PAGE_KEYWORD:
	case TO_NEXT_KEYWORD:
	case COUNT_KEYWORD:
	case NAMED_CMPN_KEYWORD:
	case SEE_KEYWORD:
	    /** IndexResetScope( inst); BT **/
	    break;
	}

	/* Finally, do the real work of handling the keyword. */
	switch ( keyword ){

	case DOC_KEYWORD:
	    ArShrcpy(inP->doc, sbuf);
	    break;

	case SORT_STRING_KEYWORD:
	    inP->sort_strings[cur_sort_string] = 
		    (shar *)ArSafeMalloc(sizeof(shar)*(1+ArShrlen(sbuf)));
	    ArShrcpy(inP->sort_strings[cur_sort_string++], sbuf);
	    break;

	case TYPEFACE_KEYWORD:
	    switch ( ArTokenShr( fiP, sbuf, index_typeface_keywords,
			      NUM_OF(index_typeface_keywords), LOOSE) ){
	    case ROMAN_KEYWORD:
		inP->typeface = face_PLAIN;
		break;
	    case ITALIC_KEYWORD:
		inP->typeface = face_ITALIC;
		break;
	    case BOLD_KEYWORD:
		inP->typeface = face_BOLD;
		break;
	    default:
		ArTellError2( ERR_NO_SUCH_CVAL, (long)sbuf, 0L);
		break;
	    }
	    break;

	case IGNORE_TO_EQUAL:
	    goto TRY_AGAIN;
	    break;
	case THIS_PAGE_KEYWORD:
	    inP->scope = index_SCOPE_LINE;
	    break;
	case TO_NEXT_KEYWORD:
	    inP->scope = index_SCOPE_CMPN;
	    break;
	case COUNT_KEYWORD:
	    inP->scope = index_SCOPE_COUNT;
	    (void)ArCvtIntShr( sbuf, &cmpn_count, FALSE);
	    if ( cmpn_count < 2 ){
		inP->scope = (( cmpn_count == 1 ) ?
			       index_SCOPE_CMPN : index_SCOPE_LINE);
	    }
	    else {
		/** if ( cmpn_count > 100 ) cmpn_count = 100; BT **/
		inP->scope = index_SCOPE_COUNT;
		inP->count = cmpn_count;
	    }
	    break;
	case NAMED_CMPN_KEYWORD:
	    inP->scope = index_SCOPE_NAMED_CMPN;
	    ArShrcpy(inP->to_named, sbuf);
	    break;
	case SEE_KEYWORD:
	    inP->scope = index_SCOPE_SEE_ALSO;
	    ArShrcpy(inP->see_string, sbuf);
	    break;
	case DICT_KEYWORD:
	    { int i;
		inP->dictionary = lang_DEFAULT;
		for (i = 0; i <= lang_SW; i++) {
		    if (ArShrstrcmp(sbuf, Language_names[i]) == 0) {
			inP->dictionary = i;
			break;
		    }
		}
	    }
	    break;

	default:
	    ArTellError2( ERR_NO_SUCH_CVAL, (long)sbuf, 0L);
	    break;
	}
    }
DONE_READING:

    BufEnter(tkP, ssP);
    /** ArSkipPastTerm(fiP);	 BT **/
    return( YES );
}
/**--------------------------------------------------------------------------**/
void
ArIndexWrite(inP, ofile)
  Index_t *inP;
  FILE *ofile;
{
  int i;

    if (inP != NULL && ofile != NULL) {
	ArFprintf(ofile, CMD_START_LINE_FMT, "Index");
	if(inP->doc[0] != '\0')
	  ArFprintf(ofile, ",\n    %s = \"%S\"", "Doc", inP->doc);
	if (inP->dictionary != lang_DEFAULT) {
	    ArFprintf(ofile, PROP_STRING_FMT, "Dictionary", 
			Language_names[inP->dictionary]);
	}
	for (i = 0; i < AR_MAX_IXLEVELS; i++) {
	    if (inP->headings[i] == NULL) {
		break;	/** can't be any following. **/
	    }
	    ArFprintf(ofile, INDEX_HEAD_FMT, inP->headings[i]);
	    if (inP->sort_strings[i] != NULL) {
		ArFprintf(ofile, PROP_NAME_FMT, "SortString", 
				inP->sort_strings[i]);
	    }
	}
    }
    ArFprintf(ofile, PROP_STRING_FMT, "Typeface", 
			FontFace_names[inP->typeface]);
    switch (inP->scope) {
    default:
    case index_SCOPE_LINE:
	break;
    case index_SCOPE_CMPN:
	ArFprintf(ofile, PROP_KEYWORD_FMT, "ToNext");
	break;
    case index_SCOPE_COUNT:
	ArFprintf(ofile, PROP_DECIMAL_FMT, "Count", inP->count);
	break;
    case index_SCOPE_NAMED_CMPN:
	ArFprintf(ofile, PROP_NAME_FMT, "ToNamed", inP->to_named);
	break;
    case index_SCOPE_SEE_ALSO:
	ArFprintf(ofile, PROP_NAME_FMT, "SeeAlso", inP->see_string);
	break;
    }
    ArFprintf(ofile, CMD_END_FMT);
}
/**--------------------------------------------------------------------------**/
void
ArIndexFree(inP)
  Index_t *inP;
{
  int i;

    if (inP != NULL) {
      for (i = 0; i < AR_MAX_IXLEVELS; i++) {
	  if (inP->headings[i] == NULL) {
	      break;	/** can't be any following. **/
	  }
	  FREE((char *)inP->headings    [i]);
	  if (inP->sort_strings[i] == NULL) {
	      continue;
	  }
	  FREE((char *)inP->sort_strings[i]);
      }
    }
    FREE((char *)inP);
}
/**--------------------------------------------------------------------------**/
