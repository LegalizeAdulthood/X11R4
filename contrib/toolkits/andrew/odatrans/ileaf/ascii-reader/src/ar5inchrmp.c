
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
/*	ar5incharmap.c                               */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  01/89    BT	Original AR version, derived from AR 3.5 unreleased version
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
#ifndef Lbrace
#define Lbrace	('{')
#define Rbrace	('}')
#endif
#define Lparen	('(')
#define Rparen	(')')
#define Dquote	('"')
#define Squote	('\'')
#define Oquote	(0x7f)
#define Comma	(',')
#define Qmark	('?')
#define Minus	('-')
/**--------------------------------------------------------------------------**/
#if 0	/** BT **/
static void dump_inmap(mapP)
  register In_char_map_t *mapP;
{
  shar *sP;
  int i;
  int length;

  for ( ; mapP != NULL; mapP = mapP->nextP) {
    length = entry_length(mapP);
    if (length == 0) {
	ArFprintf(stderr, "dump_inmap(%#x) has length 0!\n", mapP);
	exit( -1 );
    }
    ArFprintf(stderr, "dump_inmap(%#x)\n", mapP);
    ArFprintf(stderr, "  nextP    == %#x\n", mapP->nextP);
    ArFprintf(stderr, "  patternP == %#x, length == %d\n", 
		mapP->patternP, entry_length(mapP) );
    for (sP = mapP->patternP; 
	 (unsigned long)*sP != (unsigned long)AR_PATTERN_END; ) {
	ArFprintf(stderr, "       pat ==  %#x '%#C'\n", 
	    *sP, *sP);
	sP++;
    }
    ArFprintf(stderr, "  type     == %s\n", (mapP->type == r_TEXT)?
						"Text" : "Function");
    ArFprintf(stderr, "  replacement count == %d\n", mapP->replacement.count);
    for (i = mapP->replacement.count, sP = mapP->replacement.sP; 
	 i > 0; 
	 i--, sP++) {
	ArFprintf(stderr, "       rep ==  %#x '%#C'\n", *sP, *sP);
    }
    ArFprintf(stderr, "  repfnP   == %#x\n", mapP->repfnP);
  }
}
#endif	/** BT **/
/**--------------------------------------------------------------------------**/
/**	Return the next plain text shar from a text body buffer.  Ignores    **/
/**	tokens, and reduce e.g. tok_HR to its plain character form, '\n'.    **/
/**	This function does NOT descend into subcomponents, or 		     **/
/**	frames/microdocs.						     **/
/**	Destructively modifies the ssP it is given; it is presumed to be     **/
/**	a copy of the 'real' one.					     **/

shar ArTextReadch(ssP)
  Ssave_t *ssP;
{
  register shar s;
  Token_t *tkP = NULL;

    for ( ; (s = SsGetS(ssP)) != EOF; ) {
	if (IS_PADDING(s)) {
	    continue;	/** skip it. **/
	}
	else if (IS_TOKEN(s)) {
	    if (TOKEN_RESET(s) == tok_CharSpecial) {
	       tkP = (Token_t *)(SsBufStart(ssP)-1); /** cheating! **/
	    }
	    /** go past the token structure **/
	    { int i = sizeof(Token_t)/sizeof(shar);
		while (--i > 0) {
		    SsGetS(ssP);	/* skip past the token structure. **/
		}
	    }
	    switch (TOKEN_RESET(s)) {
	    case tok_CharSpecial:
		s = tkP->u.token_data;
		break;
	    case tok_SP:
		s = ' ';
		break;
	    case tok_FJ:
	    case tok_HR:
	    case tok_SR:
		s = '\n';
		break;
	    case tok_Tab:
		s = '\t';
		break;
	    case tok_TabDot:
		s = '\t';
		break;
	    case tok_TabDash:
		s = '\t';
		break;
	    default:
		/** go round the loop again - this token isn't text. **/
		continue;
		break;
	    }
	    break;	/** we've got a real character is s now. **/
	}
	else {
	    /** it's a plain shar character.  return it. **/
	    break;
	}
    }
    return( s );
}
/**--------------------------------------------------------------------------**/

shar ArTextReadchNW(ssP)
  Ssave_t *ssP;
{
  shar c;

    while ((c = ArTextReadch(ssP)) == ' ' || c == '\n' || c == '\t') {
	/** gobble. **/
    }
    return( c );
}
/**--------------------------------------------------------------------------**/
/**	Returns a character to storage (by modifying storage pointer)	     **/
/**	IF and ONLY IF it was the character previous in place there.	     **/
/**	The return value is YES or NO.					     **/
/**	This is CHEATING because it uses the insides of an Ssave_t.	     **/

bool ArTextReturn(s, ssP)
  shar s;
  Ssave_t *ssP;
{
    if (ssP != NULL && ssP->baseP != NULL && ssP->outP > ssP->baseP) {
	/** there is room. **/
	if (s == *--ssP->outP) {
	    /** it's cool. **/
	    return( YES );
	}
	else {
	    /** it's not cool. undo change, and get out. **/
	    ++ssP->outP;
	    return( NO );
	}
    }
    return( NO );
}
/**--------------------------------------------------------------------------**/
/* Get integer from input. Return FALSE if nothing. */
/* Eats prior white space.  May gobble trailing comma, if any */
/* May gobble trailing white space, if any */
/* Will take integers in decimal, 0... octal, or 0x...  hex values.        */

bool
ArTextReadInt(ssP, numP)
  Ssave_t *ssP;
  int     *numP;    
{
    register int   ch;
    register long  n=0;
    register int   radix = 10;
    register int   digit_seen = FALSE;
    int            sign = FALSE;

    if ((ch = ArTextReadch(ssP)) == '-') sign = TRUE;
    else { if (ch != '+') ArTextReturn(ch, ssP);
    }

    ch = ArTextReadch(ssP);
    if (ch == '0') {
	/** get octal or hex radix. **/
	digit_seen = TRUE;
	ch = ArTextReadch(ssP);
	if (ch == 'x' || ch == 'X') {
	    /** radix is hex. **/
	    radix = 16;
	} else {
	    /** radix is octal. **/
	    radix = 8;
	    ArTextReturn(ch, ssP);
	}
    }
    else {
	ArTextReturn(ch, ssP);
    }
    while ((ch = ArTextReadch(ssP)) != EOF) {
	if(   ald_chtype[ch] == DIG 
	   || (((ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f')) 
	        && radix == 16)) {
	    n *= radix;
	    if (ch >= '0' && ch <= '9') {
		n += ch - '0';
	    }
	    else if (ch >= 'a' && ch <= 'f') {
		n += 10 + ch - 'a';
	    }
	    else if (ch >= 'A' && ch <= 'F') {
		n += 10 + ch - 'A';
	    }
            digit_seen = TRUE;
	    continue;
	}
	ArTextReturn(ch, ssP);
	break;
    }
    if(!digit_seen) return(FALSE);

    if (sign) *numP = -n;
    else *numP = n;
    /**	fprintf(stderr, "ArTextReadInt -->%d, before return\n", n);	**/
    return(TRUE);
}
/**--------------------------------------------------------------------------**/
static bool advance(match, ssP)
  register shar match;
  Ssave_t *ssP;
{
  register shar c;

    while ((c = ArTextReadch(ssP)) != EOF) {
	/* if (c == ' ' || c == '\t' | c == '\n') { */
	if (c == ' ' || c == '\t' ) {
	    /** it's not safe to use isspace(c), because c might be > 127 **/
	     continue;
	}
	else {
	    break;
	}
    }
    if(c == match) {
	/** fprintf(stderr, "advance to %#x '%c' succeeded.\n", c, c); BT **/
	return( YES );
    }
    else {
	/** fprintf(stderr, "advance to %#x '%c' failed at %#x '%c'.\n",  BT **/
		/** match, match, c, c); BT **/
	ArTextReturn(c, ssP);
	return( NO );
    }
}
/**--------------------------------------------------------------------------**/
static bool skip_middle(ssP)
  Ssave_t *ssP;
{
    if (advance(EQsign, ssP)) {
	if (advance(Lbracket, ssP)) {
	    ArTextReturn(Lbracket, ssP);
	    return( YES );
	}
	else 
	if (advance(Lbrace, ssP)) {
	    ArTextReturn(Lbrace, ssP);
	    return( YES );
	}
	else 
	if (advance(Dquote, ssP)) {
	    ArTextReturn(Dquote, ssP);
	    return( YES );
	}
	else 
	if (advance(Oquote, ssP)) {
	    ArTextReturn(Oquote, ssP);
	    return( YES );
	}
    }
    return( NO );
}

/**---------------------------------------------------------**/
static bool get_range(text_ssP, map_ssP)
  register Ssave_t *text_ssP;
  register Ssave_t *map_ssP;
{
  register shar c;
  shar low, high;

    SsSaveS(AR_RANGE_START, map_ssP);
    SsSaveS(0, map_ssP);
    while ((c = ArTextReadch(text_ssP)) != EOF) {
	if (c == Rbracket) {
	    break;
	}
	if (c == Bslash) {
	    c = ArTextReadch(text_ssP);
	}
	low = c;
	high = ArTextReadch(text_ssP);
	if (high == Minus) {
	    /** It is a range. **/
	    high = ArTextReadch(text_ssP);
	    if (high == Bslash) {
		high = ArTextReadch(text_ssP);
	    }
	    /**  check for low <= high, prevent user errors. **/
	    if (low > high) {
		ArFprintf(stderr, 
		    "Bad range values, low (%#C) greater than high (%#C)\n", 
		    low, high);
		continue;
	    }
	    SsSaveS(AR_RANGE_LOW, map_ssP);
	    SsSaveS(low, map_ssP);
	    SsSaveS(AR_RANGE_HIGH, map_ssP);
	    SsSaveS(high, map_ssP);
	}
	else {
	    /** it's a solitary character.  **/
	    ArTextReturn(high, text_ssP);
	    SsSaveS(AR_RANGE_LOW, map_ssP);
	    SsSaveS(low, map_ssP);
	    SsSaveS(AR_RANGE_HIGH, map_ssP);
	    SsSaveS(low, map_ssP);
	    continue;
	}
    }
    SsSaveS(AR_RANGE_END, map_ssP);
    SsSaveS(0, map_ssP);
    return( c != EOF );
}

/**---------------------------------------------------------**/
static bool get_lhs(text_ssP, mapP)
  Ssave_t *text_ssP;
  In_char_map_t *mapP;
{
  register shar c, c2;
  Ssave_t *ssP              = NULL;
  int intvalue;

    while ((c = ArTextReadch(text_ssP)) != Lbracket) {
	if (c == EOF) {
	    return( NO );
	}
    }
    ssP = SsNew();
    while ((c = ArTextReadch(text_ssP)) != EOF) {
	if (c == ' ' || c == '\t' || c == '\n') {
	    /** it's not safe to use isspace(c), because c might be > 127 **/
	    continue;
	}
	if (c == Rbracket) {
	    /** fprintf(stderr, "get_lhs: found closing ']'\n"); BT **/
	    break;
	}
	/** fprintf(stderr, "get_lhs: processing %#x '%c'\n", c, c); BT **/
	switch(c) {
	case Qmark:
	    SsSaveS(AR_ANY_SINGLE, ssP);
	    SsSaveS('\0', ssP);
	    break;
	case Squote:
	case Dquote:
	case Oquote:
	    if (c == Oquote) { c = Dquote; }
	    while ((c2 = ArTextReadch(text_ssP)) != EOF) {
		if (c2 == c) {
		    break;
		}
		/** fprintf(stderr, "get_lhs: quoting '%c'\n", c2); BT **/
		SsSaveS(AR_PLAIN_CHAR, ssP);
		SsSaveS(c2, ssP);
	    }
	    break;
	case Lparen:
	    if (( c = ArTextReadch(text_ssP))  == Bslash) {
		c = ArTextReadch(text_ssP);
	    }
	    if (( c2 = ArTextReadch(text_ssP)) == Bslash) {
		c2 = ArTextReadch(text_ssP);
	    }
	    if (c == EOF || c2 == EOF || ArTextReadch(text_ssP)  != Rparen) { 
		return( NO ); 
	    }
	    SsSaveS(AR_UNORDERED_PAIR, ssP);
	    SsSaveS(c, ssP);
	    SsSaveS(c2, ssP);
	    break;
	case Lbracket:
	    get_range(text_ssP, ssP);
	    break;
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	    ArTextReturn(c, text_ssP);
	    if (ArTextReadInt(text_ssP, &intvalue) == YES) {
		/** ArFprintf(stderr, "saving plain char %#x '%#C'.\n",  BT **/
		    /** intvalue, intvalue); BT **/
		SsSaveS(AR_PLAIN_CHAR, ssP);
		SsSaveS(intvalue, ssP);
	    }
	    else {
		if (ARdebug > 0) {
		  ArFprintf(stderr, 
		  "Error reading numeric Inchar map pattern. Please check it.\n"
		    );
		}
		return( NO );
	    }
	    break;
	default:
	    break;
	}
    }
    if (SsAmount(ssP) == 0) {
	/** there was no pattern there at all! **/
	free(ssP);
	return( NO );
    }
    SsSaveS(AR_PATTERN_END, ssP);
    mapP->patternP = ssP->baseP;
    { /** for debug only. **/
    }
    return( YES );
}

/**---------------------------------------------------------**/
static bool (*(fnLookup(sP, fnListP)))()
  shar *sP;
  FnList_t *fnListP;
{
  if (fnListP != NULL) {
      for ( ; fnListP->nameP != NULL; fnListP++) {
	  if (ArShrcmp(sP, fnListP->nameP) == 0) {
	      return( fnListP->funcP );
	  }
      }
  }
  return( NULL );
}
/**--------------------------------------------------------------------------**/
/**	Return the next plain text shar from a text body buffer.  Ignores    **/
/**	tokens, except *Font* and reduce e.g. tok_HR to its plain character  **/
/**	form, '\n'.							     **/
/**	This function does NOT descend into subcomponents, or 		     **/
/**	frames/microdocs.						     **/
/**	Destructively modifies the ssP it is given; it is presumed to be     **/
/**	a copy of the 'real' one.					     **/

shar ArTextReadlit(ssP,tkP)
  Ssave_t *ssP;
  Token_t **tkP;
{
  register shar s;

    for ( ; (s = SsGetS(ssP)) != EOF; ) {
	if (IS_PADDING(s)) {
	    continue;	/** skip it. **/
	}
	else if (IS_TOKEN(s)) {
	    /* if (TOKEN_RESET(s) == tok_CharSpecial) { */
	       *tkP = (Token_t *)(SsBufStart(ssP)-1); /** cheating! **/
	    /* } */
	    /** go past the token structure **/
	    { int i = sizeof(Token_t)/sizeof(shar);
		while (--i > 0) {
		    SsGetS(ssP);	/* skip past the token structure. **/
		}
	    }
	    switch (TOKEN_RESET(s)) {
	    case tok_CharSpecial:
		s = (*tkP) -> u.token_data;
		break;
	    case tok_SP:
		s = ' ';
		break;
	    case tok_Font:
		break;
	    case tok_FJ:
	    case tok_HR:
	    case tok_SR:
		s = '\n';
		break;
	    case tok_Tab:
		s = '\t';
		break;
	    case tok_TabDot:
		s = '\t';
		break;
	    case tok_TabDash:
		s = '\t';
		break;
	    default:
		/** go round the loop again - this token isn't text. **/
		continue;
		break;
	    }
	    break;	/** we've got a real character is s now. **/
	}
	else {
	    /** it's a plain shar character.  return it. **/
	    break;
	}
    }
    return( s );
}

get_literal(in_ssP,out_ssP,count)

   Ssave_t	*in_ssP;
   Ssave_t	*out_ssP;
   short	*count;

{
   register shar	c = 0;
   /* char			temp[80]; */
   Token_t		*tkP;

   while ( (c = ArTextReadlit(in_ssP,&tkP)) != EOF ){
      if ( IS_TOKEN(c) ){
	 switch ( TOKEN_RESET(c) ){
	    case tok_Font:
	       SsSaveS('<',out_ssP);
	       ArSsFontValueWrite(tkP -> u.fontvalP,NULL,out_ssP);
	       SsSaveS('>',out_ssP);
	       continue;
	    default:
	       continue;
	       break;
	 }
      }else{
         if ( c == Rbracket ){ 
            break; 
         }else{
            if ( c == Bslash ){
	       c = ArTextReadlit(in_ssP,&tkP);
	       if ( IS_TOKEN(c) ){
		  continue;
	       }
	    }
         }
         SsSaveS(c,out_ssP);
      }
   }
   *count = SsAmount(out_ssP);
   SsSaveS('\0',out_ssP);	/** nul-terminate it. **/
}

/**---------------------------------------------------------**/
static bool get_rhs(text_ssP, mapP, fnListP)
  register Ssave_t       *text_ssP;
  register In_char_map_t *mapP;
           FnList_t      *fnListP;
{
  register shar c           = 0;
  Ssave_t *rep_ssP          = NULL;
  Entry_e type              = r_TEXT;
  bool (*fnP)() 	    = NULL;
  short count		    = 0;

    c = ArTextReadchNW(text_ssP);
    if (c == Lbracket) {
	/** it is a literal text replacment entry. **/
	rep_ssP = SsNew();
	get_literal(text_ssP,rep_ssP,&count);
	/* ArFprintf(stderr,"rep string: {%#S}\n",SsBufStart(rep_ssP)); MS */
    }
    else
    if (c == Dquote || c == Squote || c == Oquote) {
      /** it is a function-call-by-name entry. **/
      shar c2;
	rep_ssP = SsNew();
	if (c == Oquote) { c = Dquote; }
	while ((c2 = ArTextReadch(text_ssP)) != c) {
	    /** ArFprintf(stderr, 
			"get_rhs: fn name %#x, '%#C'.\n", c2, c2); BT **/
	    SsSaveS(c2, rep_ssP);
	}
	count = SsAmount(rep_ssP);
	SsSaveS('\0', rep_ssP);
	type = r_FUNCTION;
	fnP = fnLookup(SsBufStart(rep_ssP), fnListP);
    }
    else 
    if (c == Lbrace) {
	/** it is a list of numeric replacement items. **/
	rep_ssP = SsNew();
	while((c = ArTextReadchNW(text_ssP)) != EOF) {
	    if (c == Rbrace) { 
		break; 
	    }
	    if (c == Bslash) {
		c = ArTextReadch(text_ssP);
	    }
	    if (c >= '0' && c <= '9') {
	      int n;
		ArTextReturn(c, text_ssP);
		ArTextReadInt(text_ssP, &n);
		SsSaveS((short)n, rep_ssP);
	    }
	    else {
		/** error. **/
	   } 
	}
	count = SsAmount(rep_ssP);
    }
    else {
	/** badly formed entry. skip the rest of the map. **/
	ArFprintf(stderr, 
	"Badly formed input character map entry starting at character '%#C'.\n",
	  c);
	ArFprintf(stderr, "Please check the template file.\n", c);
	while (ArTextReadch(text_ssP) != EOF) { /** gobble. **/ }
	if (rep_ssP != NULL) { SsFree(rep_ssP); }
	return( NO );
    }

    if (rep_ssP == NULL) {
	/** can't happen. **/
    }

    /** found an okay replacement.  make a new map entry, stuff it. **/
    /** the pattern field is not done here. **/
    mapP->replacement.count  = count;
    mapP->replacement.sP     = rep_ssP->baseP;
    mapP->type           = type;
    mapP->repfnP         = fnP;
    /** SsFree(rep_ssP); BT **/

    return( YES );
}

/**--------------------------------------------------------------------------**/
/**	count the length of a pattern.  since each pattern takes a definite  **/
/**	number of characters to match, this is well-defined.		     **/
static int entry_length(mapP)
  register In_char_map_t *mapP;
{
  register int length = 0;
  register shar *sP;

    if (mapP != NULL && mapP->patternP != NULL) {
	for (sP = mapP->patternP; *sP != AR_PATTERN_END; sP++) {
	    switch(*sP) {
	    case AR_PLAIN_CHAR:
	    case AR_ANY_SINGLE:
	    case AR_RANGE_START:
		 length++;
		 sP++;
		 break;
	    case AR_UNORDERED_PAIR:
		 length += 2;
		 sP += 2;
		 break;
	    case AR_RANGE_LOW:
	    case AR_RANGE_HIGH:
	    case AR_RANGE_END:
		 sP++;
		 break;
	    case AR_GROUP_START:
	    case AR_GROUP_END:
	    case AR_REPEAT_COUNT:
		 ArFprintf(stderr, 
		     "Internal error in pattern length. Get help.\n");
		 exit( -1 ) ;
		 break;
	    }
	}
    }
    return( length );
}
/**--------------------------------------------------------------------------**/
/**	add a single map to the linked list, in its right place. **/
/**	The right place is before any other map of smaller size. **/
static In_char_map_t *map_merge(newP, listP)
  In_char_map_t *newP, *listP;
{
  register In_char_map_t *prevP, *thisP;
  int new_len, this_len;

    newP->nextP = NULL;
    if (listP == NULL) {
	return( newP );
    }
    else {
        new_len = entry_length(newP);
	for (prevP = NULL, thisP = listP; 
	     thisP != NULL; 
	     prevP = thisP, thisP = thisP->nextP) {
		this_len = entry_length(thisP);
		if (new_len >= this_len) {
		    /** add it before the current one in the list. **/
		    if (prevP == NULL) {
			/** special case, add it at the very front. **/
			newP->nextP = thisP;
			return( newP );
		    }
		    else {
			prevP->nextP = newP;
			newP->nextP = thisP;
			return( listP );
		    }
		}
	}
	/** it goes in place at the very end of the existing list. **/
	prevP->nextP = newP;
	return( listP );
    }
}

/**---------------------------------------------------------**/
In_char_map_t * ArGetInCharMap(cmpn_nameP, ilP, fnListP)
  shar *cmpn_nameP;
  Il_state_t *ilP;
  FnList_t   *fnListP;
{
  register short c;
  Ssave_t                *text_ssP = NULL;
  register In_char_map_t *icmapP   = NULL;
  register In_char_map_t *my_mapP  = NULL;
  NullObj_t              *thingP   = NULL;
  short	map_count = 1;

    /** only the first time creates the map. **/
    for (thingP = ilP->bodyP; thingP != NULL; thingP = thingP->nextP) {
	if (Identify(thingP) == ID_Cmpn) {
	    if (ArShrcmp(cmpn_nameP, ((Cmpn_t *)thingP)->cmpn_name) == 0) {
		break;
	    }
	}
    }
    if (thingP == NULL) {
	return( NULL );	/* no map in this document. **/
    }
    text_ssP = NEW(Ssave_t);
    *text_ssP = ((Cmpn_t *)thingP)->body;

    /** interpret entries in the template file component. **/
    while ((c = ArTextReadch(text_ssP)) != EOF) {
	if (c == Lbracket) {
	    /** fprintf(stderr, "starting a map entry at '['.\n"); BT **/
	    my_mapP = (In_char_map_t *)ArSafeMalloc(sizeof(In_char_map_t));
	    ArTextReturn(c, text_ssP);
	    if ( get_lhs(text_ssP, my_mapP) == YES ){
	       if ( skip_middle(text_ssP) == YES ){
	          if ( get_rhs(text_ssP, my_mapP, fnListP) == YES) {
		     icmapP = map_merge(my_mapP, icmapP);
		     map_count++;
		  }else{
                     fprintf(stderr,"Error in right side of ");
		     fprintf(stderr,"input character map entry %d",map_count);
                     fprintf(stderr,".  Please check it.\n");
		     free(my_mapP);
		     return(NULL);
		  }
	       }else{
                  fprintf(stderr,"Error in reading the ");
		  fprintf(stderr,"input character map entry %d",map_count);
                  fprintf(stderr,".  Please check it.\n");
		  free(my_mapP);
		  return(NULL);
	       }
	    }else{
               fprintf(stderr,"Error in left side of ");
	       fprintf(stderr,"input character map entry %d",map_count);
               fprintf(stderr,".  Please check it.\n");
	       free(my_mapP);
	       return(NULL);
	    }
	}
	/** else ignore it. **/
    }
#if 0
    { /** for debug only **/
	dump_inmap(icmapP);
    }
#endif
    return( icmapP );
}

/**--------------------------------------------------------------------------**/
In_char_info_t *ArMapGetchInit(ifile, mapP)
  FILE *ifile;
  In_char_map_t *mapP;
{
  In_char_info_t *resultP=(In_char_info_t*)ArSafeMalloc(sizeof(In_char_info_t));
  int length;

    resultP->ifile           = ifile;
    resultP->mapP            = mapP;

    length = entry_length(mapP);
    if (length > 0) {
	resultP->in_text.baseP   = (shar*)ArSafeMalloc(sizeof(shar)*(length+1));
	resultP->in_text.outP    = resultP->in_text.baseP;
	resultP->in_text.inP     = resultP->in_text.baseP;
	resultP->in_text.limitP  = resultP->in_text.baseP + length + 2;
    }
    else {
	resultP->in_text = SsInitEmpty();
#if 0	/** BT **/
	resultP->in_text.baseP   = NULL;
	resultP->in_text.outP    = NULL;
	resultP->in_text.inP     = NULL;
	resultP->in_text.limitP  = NULL;
#endif	/** BT **/
    }

    resultP->rep_text = SsInitEmpty();
#if 0	/** BT **/
    resultP->rep_text.baseP  = NULL;
    resultP->rep_text.outP   = NULL;
    resultP->rep_text.inP    = NULL;
    resultP->rep_text.limitP = NULL;
#endif	/** BT **/
    return( resultP );
}

/**--------------------------------------------------------------------------**/
/**	Compare the input stream against one pattern, return the length of   **/
/**	the matched pattern or 0.					     **/
/**	The tricky part is it can mess with the internals of the Ssave_t.    **/

static int map_match(ciP, mapP)
  In_char_info_t *ciP;
  In_char_map_t *mapP;
{
#define PEEK (ssP->baseP[peek])
  shar c;
  int peek;
  Ssave_t *ssP;
  shar c1, c2;
  shar *testP;
  bool result;

    /** ArFprintf(stderr, "Entering map_match()\n"); BT **/
    if (ciP == NULL || mapP == NULL) {
	return( 0 );
    }
    ssP = &ciP->in_text;
    if (SsAmount(ssP) < 1) {
	/** get at least one character into the saved storage, 
	    just to make things easier to manage. **/
	if ((c = getc(ciP->ifile)) == EOF) {
	    /** ArFprintf(stderr, "map_match() returning clean EOF\n"); BT **/
	    return( 0 );
	};
	SsSaveS(c, ssP);
    }
    /** now, left-justify the input stream characters in the Ssave_t. **/
    /** (if any).							**/
    if (ssP->outP != ssP->baseP) {
      int i, n;
      shar *dstP, *srcP;
	n = SsAmount(ssP);
	for (dstP=ssP->baseP, srcP=ssP->outP, i=0; i++ < n; ) {
	    *dstP++ = *srcP++;
	}
	ssP->outP = ssP->baseP;
	ssP->inP  = ssP->baseP + n;
    }
    for ( peek = 0, testP = mapP->patternP; *testP != AR_PATTERN_END; ) {
	if (&PEEK == ssP->inP) {
	    /** have to get more input. **/
	    if ((c = getc(ciP->ifile)) == EOF) {
		/** ArFprintf(stderr, "map_match() returning start EOF\n"); 
		BT **/
		return( 0 );
	    };
	    SsSaveS(c, ssP);
	}
	/** ArFprintf(stderr, "map_match() input == %#C\n", PEEK); BT **/
	switch(*testP) {
	case AR_PLAIN_CHAR:
	     if (PEEK == *++testP) { 
		 peek++; 
		 testP++;
		 continue; 
	     }
	     else {
		 /** it's not cool. **/
		 return( 0 );
	     }
	     break;
	case AR_ANY_SINGLE:
	     peek++; 
	     testP += 2;
	     continue;
	     break;
	case AR_UNORDERED_PAIR:
	     c1 = PEEK;
	     peek++;
	     if (&PEEK == ssP->inP) {
		 /** have to get more input. **/
		 if ((c = getc(ciP->ifile)) == EOF) {
		     /** it's not cool. **/
		     /** ArFprintf(stderr, "map_match() returning pair EOF\n");
		     BT **/
		     return( 0 );
		 };
		 SsSaveS(c, ssP);
	     }
	     c2 = PEEK;
	     ++testP;
	     if (c1 == *testP && c2 == *(testP+1)  ||
		 c2 == *testP && c1 == *(testP+1) ) {
		     peek++;
		     testP += 2;
	     }
	     else {
		 /** it's not cool. **/
		 return( 0 );
	     }
	     break;
	case AR_RANGE_START:
		c1 = PEEK;
		testP += 2;
		for (result = NO; *testP != AR_RANGE_END; ) {
		    if (testP[0] == AR_RANGE_LOW   && 
			testP[2] == AR_RANGE_HIGH  && 
			      c1 >= testP[1]    &&
			      c1 <= testP[3]       ) {
			    result = YES;
			    break;
		    }
		    else {
			/** keep looking. **/
			testP += 4;
		    }
		}
		if (result == YES) {
		    /** found a match. **/
		    peek++;
		    while (*testP++ != AR_RANGE_END) { }
		    testP++;
		    continue;
		}
		else {
		    /** failure. **/
		    return( 0 );
		}
	     break;
	default:
	     ArFprintf(stderr,"Unknown case %d in map_match()! Get help.\n",
			*testP);
	     exit( -1 );
	     break;
	}
    }
    /** assert: we found a match. **/
    return( entry_length(mapP) );
}

/**--------------------------------------------------------------------------**/
shar ArMapGetch(ciP)
  register In_char_info_t *ciP;
{
  register In_char_map_t *mapP;
  int length, i, count;
  shar *sP;
  Ssave_t *ssP;

    /** ArFprintf(stderr, "ArMapGetch(): \n"); BT **/
    if (ciP == NULL) {
	ArFprintf(stderr, "ArMapGetch(): given NULL pointer.  Get help.\n");
	exit( -1 );
    }
    if (SsAmount(&ciP->rep_text) > 0) {
	/** use the already mapped text. **/
	/** ArFprintf(stderr, "Using replacement text.\n"); BT **/
	return( SsGetS(&ciP->rep_text) );
    }
    else {
	if (ciP->mapP == NULL) {
	    /** no map, therefore no mapping.  just do a getc(). **/
	    /** ArFprintf(stderr, "Using plain getc, no map.\n"); BT **/
	    if (SsAmount(&ciP->in_text) > 0) {
		return( SsGetS(&ciP->in_text) );
	    }
	    else {
		return( getc(ciP->ifile) );
	    }
	}
	SsReset(&ciP->rep_text);	/** it must be empty; re-use it all. **/
	for (mapP = ciP->mapP; mapP != NULL; mapP = mapP->nextP) {
	    if ((length = map_match(ciP, mapP)) > 0) {
		/** found it. **/
		if (mapP->type == r_TEXT) {
		    while (length-- > 0) {
			/** abandon chars that matched. **/
			SsGetS(&ciP->in_text);
		    }
		    sP = mapP->replacement.sP;
		    ssP = &ciP->rep_text;
		    for(i = 0, count = mapP->replacement.count; i++ < count;){
			SsSaveS(*sP++, ssP);
		    }
		    return( SsGetS(ssP) );
		}
		else if (mapP->type == r_FUNCTION) {
		    /** ArFprintf(stderr,  BT **/
		    /** "ArMapGetch(): function replacement named %Q\n", BT **/
		    /** mapP->replacement.sP); BT **/
		    if (  mapP->repfnP == NULL ||
			(*mapP->repfnP)(ciP,length,mapP->replacement.sP) == NO){
			    while (length-- > 0) {
				/** no function in place.  **/
				/** or the function rejects the input. **/
				/** use input text unchanged. **/
				SsSaveS( SsGetS(&ciP->in_text), &ciP->rep_text);
			    }
		    }
		    /** in any case, return the next replacement character. **/
		    return( SsGetS(&ciP->rep_text) );
		}
		else {
		    /** can't happen. **/
		}
	    }
	}
	/** input does not match any pattern.  return a raw character. **/
	/** ArFprintf(stderr, "No pattern matched. raw getc.\n"); BT **/
	if (SsAmount(&ciP->in_text) > 0) {
	    return( SsGetS(&ciP->in_text) );
	}
	else {
	    return( getc(ciP->ifile) );
	}
    }
}

/**--------------------------------------------------------------------------**/
shar ArMapUnGetch(c, ciP)
  register shar c;
  register In_char_info_t *ciP;
{
    return( SsSaveSFront(c, &ciP->in_text) == YES? c : EOF ); 
}
/**--------------------------------------------------------------------------**/
