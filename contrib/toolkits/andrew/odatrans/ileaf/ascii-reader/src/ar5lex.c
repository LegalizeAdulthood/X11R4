
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
/*	ar5lex.c                               */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  07/88    BT	Original revision from alLex.c
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
static bool ArGetYesNoShortOpt();
/**--------------------------------------------------------------------------**/
char al_token_word[AR_MAX_QSTR+1]; /* most all tokens are read in here */
short al_token_shr[AR_MAX_QSTR+1]; 	/* or here */

ArTokenShr(fiP, word, keytbl, length, precision)
File_info_t *fiP;
short* word;
struct keytab keytbl[];
int length;
int precision;
{
    struct keytab* kt;

    /* first check for F0, F1, etc. */
    if(ArFontTokenShr(fiP, word, precision, P_TEXT)) return (FN);
    /* then search list */
    if(kt = ArBinaryShr(word, keytbl, length, precision, FALSE))
	return(kt->keyval);
    else
	return(NO_MATCH);
}

/* Read a token followed by "=", look it up (with LOOSE match),
 * and return number. Word is left in al_token_shr (for error
 * reporting etc.) */

ArGetTokenEq(fiP, keytbl, length)
File_info_t *fiP;
struct keytab keytbl[];
int length;
{
  shar c;

    ArGetWordShrC(fiP, NOCHAR, al_token_shr, AR_MAX_QSTR-1, 0);
    if((c = ArGetchNW(fiP)) != '='){
	ArUnGetch(c, fiP);
	ArTellError(ERR_MISS_EQ);
	ArSkipPastComma(fiP);
	return(NO_EQUALS);
    }
    return(ArTokenShr(fiP, al_token_shr, keytbl, length, LOOSE));
}

/* Read a token followed by optional ",", look it up (with LOOSE match),
 * and return number. Word is left in al_token_shr (for error
 * reporting etc.) */

ArGetTokenComma(fiP, keytbl, length)
File_info_t *fiP;
struct keytab keytbl[];
int length;
{
    ArGetWordShrC(fiP, NOCHAR, al_token_shr, AR_MAX_QSTR-1, COMMA);
    return(ArTokenShr(fiP, al_token_shr, keytbl, length, LOOSE));
}

/* Read a string terminated by a non-alpha character, look it up
 * (with LOOSE match), and return number. Word is left in al_token_shr
 * (for error reporting etc.)
 */
ArGetTokenAlpha(fiP, keytbl, length)
File_info_t *fiP;
struct keytab keytbl[];
int length;
{
    ArGetAlphaWord(fiP, al_token_shr, AR_MAX_QSTR);
    if (al_token_shr[0] == 0)
	return(NO_MATCH);
    return(ArTokenShr(fiP, al_token_shr, keytbl, length, LOOSE));
}

/* Make sure that the various keyword tables are properly
 * sorted. Debugging mode only.
 */

#ifdef MD_DEBUG
ArCheckKeytbl(keytbl, length)
struct keytab keytbl[];
int length;
{
    register int i;
    for(i = 1; i < length; ++i) {
	if(ArStrncmp(keytbl[i-1].keyword, keytbl[i].keyword,
		     AR_MAX_QSTR, LOOSE) >= 0)
	    printf("ArCheckKeytbl (loose): %s >= %s\n\r",
		   keytbl[i-1].keyword, keytbl[i].keyword);
	if(ArStrncmp(keytbl[i-1].keyword, keytbl[i].keyword,
		     AR_MAX_QSTR, EXACT) >= 0)
	    printf("ArCheckKeytbl (exact): %s >= %s\n\r",
		   keytbl[i-1].keyword, keytbl[i].keyword);
    }
}
#endif

/*  ArGetWordShrC
 *	Get a word up to 'lim' characters long into array "w".
 *	('lim' does not include trailing zero byte.)
 *	Argument 'c' is the first character to put into the word, or
 *	NOCHAR (#defined as 0).
 *	Unless quotes are used, whitespace is ignored
 *	and , = < > and @ are terminators.  If quotes
 *	are used, they may be single or double, and embedded
 *	linefeeds etc. are ignored.
 *      If "term" is nonzero, it is a terminator to be gobbled.
 *	Always gives at least a null string.
 *	Returns FALSE if word is too long or only an EOF was seen.
 *	Returns QUOTED (2) if word was quoted.
 *	Returns TRUE otherwise.
 */

/**--------------------------------------------------------------------------**/
bool
ArGetWordShrC(fiP, c, w, lim, term)
File_info_t *fiP;
register int c;
register short* w;   /* must be at least lim+1 long */
register int lim;
char term;
{
    short* iw = w;
    register int t;
    int retval = TRUE;
    int temp, t2;

    if(c == 0) { c = ArGetchNW(fiP); }
    if(c == EOF) {
	*w = '\0';
	return(FALSE);
    }
    if(c == '"' || c == '\'') {  /* the quoted case */
	retval = AR_QUOTED;
	while(TRUE) {
	    t = ArGetch(fiP);
	    /* < is doubled in a quoted string. This may have been a mistake.
	     * <#xx> is allowed too. */
	    if(t == al_cmd_char) {
		temp = ArGetch(fiP);
		if(temp != al_cmd_char) {
		    if(temp == al_hex_char && al_qs_hex) {
			if((temp = ArDoHex(fiP)) >= 0) {
			    if(temp > 0xff) {
				ArTellError(ERR_HEX_HIGH);
				continue;
			    }
			    t = temp;
			} else continue;
		    } else {
			/* a lone "<": treat like << */
			ArUnGetch(temp, fiP);
		    }
		}
	    } else if(c <= ALD_CHTYP_MAX && ald_chtype[t] == WHT && t != ' ')
		continue;
	    *w++ = t;
	    if(t == EOF) break;
	    if(t == c) {
		t2 = ArGetch(fiP);
		if(c == t2) continue;   /* gobble second quote */
		c = t2;			/* get probable terminator */
		break;
	    }
	    if(--lim < 0) {
		/* At this point, we might gobble ahead (with some limit
		 * to the distance) until we hit a terminator (if nonzero).
		 * (Also, we could combine with same case below.) */
		ArTellError(ERR_NAME_TOOLONG);
		*--w = '\0';
		return(FALSE);
	    }
	}	/* end of while loop */
	
	*(w-1) = '\0';
    } else {		/* the unquoted case */
	do {
	    *w++ = c;
	    if(c <= ALD_CHTYP_MAX && ald_chtype[c] == SPC)
		break;
	    if(--lim < 0) {
		ArTellError(ERR_NAME_TOOLONG);
		*(w-1) = '\0';
		return(FALSE);
	    }
	} while((c = ArGetchNW(fiP)) != EOF);
	--w;
	while(w >= iw && *--w == ' '){}; /* throw out trailing blanks */
	*++w = '\0';
    }

    if(c != term) ArUnGetch(c, fiP); /* gobble terminator if any */
    return(retval);
}
/**--------------------------------------------------------------------------**/

/* Get an alphabetic word. Whitespace squeezed out. No quoting. */

ArGetAlphaWord(fiP, w, lim)
File_info_t *fiP;
register shar* w;   /* must be at least lim+1 long */
register int lim;
{
    register int c;

    if((c = ArGetchNW(fiP)) == EOF) {
	*w = 0;
	return(FALSE);
    }

    do {
	if(c > ALD_CHTYP_MAX)
	    break;
	if(ald_chtype[c] != LTR)
	    break;
	*w++ = c;
	if(--lim < 0) {
	    ArTellError(ERR_NAME_TOOLONG);
	    *(w-1) = 0;
	    return(FALSE);
	}
    } while((c = ArGetchNW(fiP)) != EOF);

    *w = 0;
    ArUnGetch(c, fiP);
    return(TRUE);
}

/*  ArStrncmpShr
 *  If precision is EXACT, this is the similar to strncmp.
 *  If LOOSE, then upper and lower case letters will
 *  match and embedded blanks will be ignored.
 *  When blanks are ignored, "lim" is extended.
 *  NOTE:  in both cases, the value returned must be the same.
 *  This is important for binary search. The value returned ignores case:
 *  "Foo" comes after "bar".
 *  Note that:
 *      ArStrncmpShr("Foo", "foo", 10, LOOSE) == 0
 *      ArStrncmpShr("Foo", "foo", 10, EXACT) == 1
 */

ArStrncmpShr(s, t, lim, precision)
register short* s;
register char* t;
register int lim;
int precision;
{ 
    register int caps;
    register int capt;
    register int diff = 0;
        
    while (--lim >= 0) {
	if(*s == *t) {
	    if(*s == '\0') return (diff);
	    ++s; ++t; continue;
	}
	if(precision == EXACT)
	    ++diff;
	if(*s == ' '){
	    ++s; ++lim; continue;
	}
	if(*t == ' '){
	    ++t; ++lim; continue;
	}
	if((caps = ArForceCap(*s)) == (capt = ArForceCap(*t)))
	    { ++s; ++t; continue; }
	return(caps - capt);
    }
    return(diff);
}

ArShrncmpShr(s, t, lim, precision)
register short* s;
register short* t;
register int lim;
int precision;
{ 
    register int caps;
    register int capt;
    register int diff = 0;
        
    while (--lim >= 0) {
	if(*s == *t) {
	    if(*s == '\0') return (diff);
	    ++s; ++t; continue;
	}
	if(precision == EXACT)
	    ++diff;
	if(*s == ' '){
	    ++s; ++lim; continue;
	}
	if(*t == ' '){
	    ++t; ++lim; continue;
	}
	if((caps = ArForceCap(*s)) == (capt = ArForceCap(*t)))
	    { ++s; ++t; continue; }
	return(caps - capt);
    }
    return(diff);
}

#ifdef MD_DEBUG
ArStrncmp(s, t, lim, precision)
register char* s;
register char* t;
register int lim;
int precision;
{ 
    register int caps;
    register int capt;
    register int diff = 0;
        
    while (--lim >= 0) {
	if(*s == *t) {
	    if(*s == '\0') return (diff);
	    ++s; ++t; continue;
	}
	if(precision == EXACT)
	    ++diff;
	if(*s == ' '){
	    ++s; ++lim; continue;
	}
	if(*t == ' '){
	    ++t; ++lim; continue;
	}
	if((caps = ArForceCap(*s)) == (capt = ArForceCap(*t)))
	    { ++s; ++t; continue; }
	return(caps - capt);
    }
    return(diff);
}
#endif

/* Binary search of a 'keytab' structure. Return NULL on failure. */    

struct keytab*
ArBinaryShr(word, tab, tabsiz, precision, prefix)
short *word;
struct keytab tab[];
int tabsiz;
int precision;
bool prefix;		/* look for keytab word which is prefix of word */
{
    register int cond;
    register struct keytab *low = &tab[0];
    register struct keytab *high = &tab[tabsiz-1];
    register struct keytab *mid;

    if(tabsiz < 1)
	return(NULL);
    while (low <= high) {
	mid = low + (high - low) / 2;
	if((cond = ArStrncmpShr(word, mid->keyword,
				prefix ? strlen(mid->keyword) : AR_MAX_QSTR,
				precision)) < 0) {
	    high = mid - 1;
	} else {
	    if (cond > 0)
		low = mid + 1;
	    else
		return(mid);
	}
    }
    return(NULL);
}



ArForceCap(c)  /* change lower case char to cap */
register int c;
{
    return((c >= 'a' && c <= 'z') ? (c & 0x5F) : c);
}


float al_unit_factor[] = {	/* the defines match the subscript values */
    DEF_RSUINCH/LINE_PER_INCH,	/* if someone illegally uses "lines",     */
				/* we use this */
    DEF_RSUINCH,
    DEF_RSUINCH/CM_PER_INCH,
    DEF_RSUINCH/MM_PER_INCH,
    DEF_RSUINCH/POINTS_PER_INCH,
    DEF_RSUINCH/PICAS_PER_INCH,
    DEF_RSUINCH/DIDOT_PER_INCH,
    DEF_RSUINCH/CICERO_PER_INCH
};

#define LD_LINES 0
#define INCHES     1
#define CM         2
#define MM         3
#define POINTS     4
#define PICAS      5
#define DIDOT	   6
#define CICERO     7
struct keytab al_unit_names[] = {     /* this list must remain sorted! */
    {"Cicero",CICERO},
    {"Ciceros",CICERO},
    {"CM",CM},
    {"Didot",DIDOT},
    {"Didots",DIDOT},
    {"In",INCHES},
    {"Inch",INCHES},
    {"Inches",INCHES},
    {"Line",LD_LINES},
    {"Lines",LD_LINES},
    {"MM",MM},
    {"Pica",PICAS},
    {"Picas",PICAS},
    {"Point",POINTS},
    {"Points",POINTS}
};
#define NUNITS (sizeof(al_unit_names) / sizeof(struct keytab))

/* gobbles trailing comma */    
int
ArGetUnitMeas(fiP)
File_info_t *fiP;
{
    return(ArGetTokenComma(fiP, al_unit_names, NUNITS));
}

Rsu
ArCvtUnits(units, measure)
int units;
float measure;
{
    register float frsu;
    
    frsu = al_unit_factor[units] * measure + (measure >= 0.0 ? 0.5 : -0.5) ;
    if(frsu > RSUMAX) {
	ArTellError(ERR_RSU_SIZE);
	frsu = RSUMAX;
    }
    if(frsu < -RSUMAX) {
	ArTellError(ERR_RSU_SIZE);
	frsu = -RSUMAX;
    }

    return((Rsu)frsu);
}


/* gobbles trailing comma */    
bool
ArGetRsu(fiP,num)      /* get distance in Rsu's */
File_info_t *fiP;
Rsu* num;
{
    float measure;
    int units;

    if(!ArGetFloat(fiP, &measure)) {
	ArTellError(ERR_UNITS_NUM);
	ArSkipPastComma(fiP);
	return(FALSE);
    }
    if((units = ArGetUnitMeas(fiP)) == -1){
	ArTellError(ERR_RSU_UNITS);
	return(FALSE);
    }
    
    if(units == LD_LINES)
	ArTellError(ERR_RSU_LINES);
    *num = ArCvtUnits(units, measure);
    return(TRUE);
}


/*  NOTE -- These two work with SHORTS, and only touch the bits
	    specified in MASK! */

/*  returns FALSE if garbage */

bool
ArGetYesNoShort(fiP,flagptr)
File_info_t *fiP;
short* flagptr;
{
    return(ArGetYesNoShortOpt(fiP,flagptr, ERR_YESNO, FALSE));
}

/*  returns FALSE if garbage */

bool
ArGetYesNoShortInv(fiP,flagptr)
File_info_t *fiP;
short* flagptr;
{
    return(ArGetYesNoShortOpt(fiP,flagptr, ERR_YESNO, TRUE));
}


struct keytab al_yes_no_names[] = {  /* this list must remain sorted! */
    {"False", NO},
    {"No", NO},
    {"Off", NO},
    {"On", YES},
    {"True", YES},
    {"Yes", YES}
};
#define NYESNOS (sizeof(al_yes_no_names) / sizeof(struct keytab))

/*  returns  NO if garbage */

/* gobbles trailing comma */    
static bool
ArGetYesNoShortOpt(fiP,flagptr, msg, inv)
File_info_t *fiP;
short* flagptr;
int msg;
int inv;
{
    int value = ArGetTokenComma(fiP,al_yes_no_names, NYESNOS);

    if( value == -1) {
	if(msg >= 0) ArTellError(msg);
	return( NO);
    } else {
	if(inv) {
	    value = YES - value;
	}
	*flagptr = value;
    }
    return( YES);
}

struct keytab al_align_names[] = {   /* this list must remain sorted! */
    {"Both",CJST},
    {"Center",CCEN},
    {"Centered",CCEN},
    {"Justified",CJST},
    {"Justify",CJST},
    {"Left",CLFT},
    {"Right",CRGT}
};
#define NANAMES (sizeof(al_align_names) / sizeof(struct keytab))

/* gobbles trailing comma */    
bool
ArGetAlign(fiP,flagptr)
File_info_t *fiP;
short* flagptr;
{
    int value = ArGetTokenComma(fiP,al_align_names, NANAMES);

    if(value == -1) {
	ArTellError(ERR_ALIGN);
	return( NO );
    }
    else {
	*flagptr = value;
	return( YES );
    }
}

struct keytab al_orient_names[] = {   /* this list must remain sorted! */
    {"Landscape",d_LANDSCAPE},
    {"Portrait", d_PORTRAIT}
};
#define NORNAMES (sizeof(al_orient_names) / sizeof(struct keytab))


bool
ArGetOrient(fiP,flagptr)
File_info_t *fiP;
unsigned int* flagptr;
{
    int value = ArGetTokenComma(fiP,al_orient_names, NORNAMES);

    if(value == -1){
	ArTellError(ERR_ORIENT);
	return( NO );
    } 
    else {
	*flagptr = value;
	return( YES );
    }
}

struct keytab al_formspace_names[] =  /* this list must remain sorted */
  { { "Optimum", FSPC_OPT },
    { "Overset", FSPC_OVR } };

# define NFSNAMES (sizeof(al_formspace_names)/sizeof(struct keytab))

bool
ArGetFormSpace(fiP, flagptr)
File_info_t *fiP;
unsigned short * flagptr;
{
  int value = ArGetTokenComma(fiP,al_formspace_names,NFSNAMES);
  if (value == -1) {
    ArTellError(ERR_FORMSPACE);
    return( NO );
  }
  else {
      *flagptr = value;
      return( YES );
  }
}



struct keytab l_r_names[] = {   /* this list must remain sorted! */
    {"Left", d_LEFT},
    {"Right", d_RIGHT}
};
#define NLRNAMES (sizeof(l_r_names) / sizeof(struct keytab))


/*  returns: -1 = error, 0 = left, 1 = right */
int
ArGetLR(fiP)
File_info_t *fiP;
{
    return(ArGetTokenComma(fiP,l_r_names, NLRNAMES));
}

#define IN_TEXT		0
#define IN_WHITE	1
#define IN_COMMENT	2

int
ArGetTextShr(fiP, buf, buf_size)
File_info_t *fiP;
short*		buf;
register int	buf_size;
{
    register short*  dest;
    register int    c;
    register long   size;
    register int    state;
    register short*  end;
    int temp;

    switch ( c = ArGetchNW(fiP) ){
    case EOF:
	buf[0] = '\0';
	return ( EOF );
    case '"':
    case '\'':
	ArGetWordShrC(fiP,c,  buf, buf_size-1, 0);
	return ( AL_TEXT_QUOTED );
    default:
	for ( end = dest = buf, size = buf_size, state = IN_TEXT;
	      c != EOF && (c > ALD_CHTYP_MAX || ald_chtype[c] != SPC);
	      ++dest, c = ArGetch(fiP) ){
	    switch ( state ){
	    case IN_TEXT:
		if ( c <= ALD_CHTYP_MAX && ald_chtype[c] == WHT ){
		    state = IN_WHITE;
		    end = dest;
		}
		else if ( c == al_com_char ){
		    if ( (temp = ArGetch(fiP)) == c ){
			if ( --size > 0 ) *dest = c;
			state = IN_COMMENT;
			end = dest;
			++dest;
		    }
		    else
			ArUnGetch(temp, fiP);
		}
		break;
	    case IN_WHITE:
		if ( c > ALD_CHTYP_MAX || ald_chtype[c] != WHT ){
		    if ( c != al_com_char )
			state = IN_TEXT;
		    else if ( (temp = ArGetch(fiP)) == c ){
			if ( --size > 0 ) *dest = c;
			state = IN_COMMENT;
			++dest;
		    }
		    else
			ArUnGetch(c, fiP);
		}
		break;
	    case IN_COMMENT:
		if ( c == al_com_char ){
		    if ( (temp = ArGetch(fiP)) == c ){
			if ( --size > 0 ) *dest = c;
			state = IN_WHITE;
			++dest;
		    }
		    else
			ArUnGetch(c, fiP);
		}
		break;
	    }
	    if ( --size > 0 ) *dest = c;
	}
	if ( c != EOF ) ArUnGetch(c, fiP);
	switch ( state ){
	case IN_TEXT:
	    end = dest;
	    break;
	case IN_WHITE:
	    break;
	case IN_COMMENT:
	    if ( c != al_cmd_term )
		end = dest;
	    break;
	}
	if ( end < buf + buf_size ){
	    *end = '\0';
	    return ( AL_TEXT_OK );
	}
	else {
	    ArTellError( ERR_NAME_TOOLONG);
	    buf[buf_size-1] = '\0';
	    return ( AL_TEXT_ERROR );
	}
    }
}

/* 	ArTokenPrefix
 * Looks for a prefix from the list in al_token_shr. If found,
 * it is stripped (along with trailing blanks) from al_token_shr 
 * and it's code returned.
 */
ArTokenPrefix(fiP, keytbl, length)
File_info_t *fiP;
struct keytab keytbl[];
int length;
{
    struct keytab* kt;
    shar* s;

    if(kt = ArBinaryShr(al_token_shr, keytbl, length, LOOSE, TRUE)) {
	for(s = al_token_shr + strlen(kt->keyword); *s == ' '; ++s);
	ArShrcpy(al_token_shr, s);
	return(kt->keyval);
    } else {
	return(NO_MATCH);
    }
}

struct keytab al_table_lr_prefixes[] = {  /* this list must remain sorted */
    {"Bottom", LD_BOTTOM},
    {"Footer", LD_FOOTER},
    {"Header", LD_HEADER},
    {"Left", LD_LEFT},
    {"Right", LD_RIGHT},
    {"Top", LD_TOP}
};

/*	ArLRTBPrefix
 * Check for Left/Right/Top/Bottom prefix. If any return code and
 * strip prefix from al_token_shr (where the name must be upon calling.)
 * If no prefix return -1.
 */

ArLRTBPrefix(fiP)
File_info_t *fiP;
{
    return(ArTokenPrefix(fiP, al_table_lr_prefixes,
			 NUM_OF(al_table_lr_prefixes)));
}
/**--------------------------------------------------------------------------**/
struct keytab al_color_proc_names[] = {   /* this list must remain sorted! */
    {"Off",	spot_color_sep_OFF},
    {"Screened",spot_color_sep_SCREENED},
    {"Solid",	spot_color_sep_SOLID}
};
#define NCOLORPROCNAMES (sizeof(al_color_proc_names) / sizeof(struct keytab))


bool
ArGetColorSep(fiP, sP)
File_info_t *fiP;
unsigned short* sP;
{
    int value = ArGetTokenComma(fiP, al_color_proc_names, NCOLORPROCNAMES);

    if(value == -1) {
	ArTellError(ERR_COLOR_PROC);
	return( NO );
    } else {
	*sP = value;
	return( YES );
    }
}

/**--------------------------------------------------------------------------**/
