
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
/*	ar5init.c                               */
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
#define ATTRCHAR    '@'
#define TOGGLECHAR  '~'
#define CAPCHAR     'C'
/**--------------------------------------------------------------------------**/
#define AT_SIGN	    '@'
#define UNDERCHAR   'U'
#define STRIKECHAR  'S'
#define SUPERCHAR   'T'
#define SUBCHAR     'B'
#define REVCHAR	    'R'
#define PAIRCHAR    'P'
#define TRACKCHAR   'K'
#define OVERCHAR    'O'
#define DBLCHAR	    'D'
#define COLORCHAR   'Z'
#define LANGCHAR    'L'
#define INHERCHAR   'i'
#define TILDA	    '~'
#define LESSCHAR    '-'
#define MORECHAR    '+'
#define BOLDCHAR    'E'
#define ITALCHAR    'I'
#define SIZECHAR    'H'
#define FAMCHAR	    'F'
#define ALLCHAR	    '*'

#define FQSIZE 16   /* font quack size */

#define DEFAULT_SIZE  10
#define FONTNAME_MAX  60    /* longest font name (in chars.) */
/**--------------------------------------------------------------------------**/
struct keytab ar_lang_names[] =		/** this list must remain sorted! **/
{
    {"AM", lang_AM},
    {"BR", lang_BR},
    {"CA", lang_CA},
    {"DA", lang_DA},
    {"DU", lang_DU},
    {"FI", lang_FI},
    {"FR", lang_FR},
    {"GE", lang_GE},
    {"IT", lang_IT},
    {"NL", lang_NL},	/** the 'no language' case. **/
    {"NO", lang_NO},
    {"PO", lang_PO},
    {"SP", lang_SP},
    {"SW", lang_SW},
};

/**--------------------------------------------------------------------------**/
Font_attr_t EcInherFontUnder(){fprintf(stderr, "stub called.n"); exit(-1);}
Font_attr_t EcInherFontStrike(){fprintf(stderr, "stub called.n"); exit(-1);}
Font_attr_t EcInherFontRev(){fprintf(stderr, "stub called.n"); exit(-1);}
Font_attr_t EcInherFontSuper(){fprintf(stderr, "stub called.n"); exit(-1);}
Font_attr_t EcInherFontSub(){fprintf(stderr, "stub called.n"); exit(-1);}
Font_attr_t EcInherFontOver(){fprintf(stderr, "stub called.n"); exit(-1);}
Font_attr_t EcInherFontCaps(){fprintf(stderr, "stub called.n"); exit(-1);}
Font_attr_t EcInherFontPair(){fprintf(stderr, "stub called.n"); exit(-1);}
Font_attr_t EcInherFontTrack(){fprintf(stderr, "stub called.n"); exit(-1);}
Font_attr_t EcInherFontColor(){fprintf(stderr, "stub called.n"); exit(-1);}
Font_attr_t EcInherFontLang(){fprintf(stderr, "stub called.n"); exit(-1);}
int         EcInherFontBold(){fprintf(stderr, "stub called.n"); exit(-1);}
int         EcInherFontItalic(){fprintf(stderr, "stub called.n"); exit(-1);}
int         EcInherFontSize(){fprintf(stderr, "stub called.n"); exit(-1);}
Font_attr_t EcInheritAlloc(){fprintf(stderr, "stub called.n"); exit(-1);}
/**--------------------------------------------------------------------------**/
    
/*  Face change macros.  For use with FnFindClosestMatch.
 */
#define fn_BOLD_FACE_SAME	0L
#define fn_BOLD_FACE_TOGGLE	1L
#define fn_BOLD_FACE_ON		2L
#define fn_BOLD_FACE_OFF	3L
#define fn_BOLD_FACE_INVERT	4L	/* Like TOGGLE, but high precedence. */
#define fn_BOLD_FACE_OP		0x7L	/* == SAME | TOGGLE | ON | OFF */

#define fn_ITALIC_FACE_SAME	0L
#define fn_ITALIC_FACE_TOGGLE	(1L<<3)
#define fn_ITALIC_FACE_ON	(2L<<3)
#define fn_ITALIC_FACE_OFF	(3L<<3)
#define fn_ITALIC_FACE_INVERT	(4L<<3)	/* Like TOGGLE, but high precedence. */
#define fn_ITALIC_FACE_OP	(0x7L<<3)/* == SAME | TOGGLE | ON | OFF */

#define fn_UNDER_FACE_SAME	0L
#define fn_UNDER_FACE_TOGGLE	(1L<<6)
#define fn_UNDER_FACE_ON	(2L<<6)
#define fn_UNDER_FACE_OFF	(3L<<6)
#define fn_UNDER_FACE_INVERT	(4L<<6)	/* Like TOGGLE, but high precedence */
#define fn_UNDER_FACE_OP	(0x7L<<6) /* == SAME | TOGGLE | ON | OFF */

#define fn_STRIKE_FACE_SAME	0L
#define fn_STRIKE_FACE_TOGGLE	(1L<<9)
#define fn_STRIKE_FACE_ON	(2L<<9)
#define fn_STRIKE_FACE_OFF	(3L<<9)
#define fn_STRIKE_FACE_INVERT	(4L<<9)	/* Like TOGGLE, but high precedence */
#define fn_STRIKE_FACE_OP	(0x7L<<9)/* == SAME | TOGGLE | ON | OFF */


#define fn_SAME_FACE		(fn_BOLD_FACE_SAME | fn_ITALIC_FACE_SAME)
#define fn_SET_FACE_ROMAN	(fn_BOLD_FACE_OFF | fn_ITALIC_FACE_OFF)
#define fn_SET_FACE_ITALIC	(fn_BOLD_FACE_OFF | fn_ITALIC_FACE_ON)
#define fn_SET_FACE_BOLD	(fn_BOLD_FACE_ON | fn_ITALIC_FACE_OFF)
#define fn_SET_FACE_BOLD_ITALIC	(fn_BOLD_FACE_ON | fnITALIC_FACE_ON)

/**--------------------------------------------------------------------------**/
short al_font_number;	/* Set in AlFontToken. Used in  AlGetFont,
			 * AlDoFontDefs  and AlDoFont.
			 * An "ASCII" font number. */
/*  values of al_font_number for <FI> and <FB> (must be < 0) */
#define BOLD_FONT     -1
#define ITALIC_FONT   -2
Font_attr_t al_attrs;			/* attributes with al_font_number */
bool al_pop_font;		/* set by alFontToken when /F seen */

short al_cur_font;		/* "ecrit" font */
short al_cmpn_dfont;		/* "ecrit" font (set in alCmpn) */
Font_inherit_t al_font_inherit;   /* font inheritance set by alfontToken */
long al_font_face_op;		/* used in conjunction w/inheritance */
short al_font_size;			/* used in conjunction w/inheritance */
short al_fam;				/* used in conjunction w/inheritance */

/*  "al_font_defs" is indexed by "ASCII" font number */
/*
 *  size == -1 means that the table entry is untouched.
 *  ecfno == -1 means that <!Fonts, Fn = > has been seen
 *	    but the font is not yet loaded. This can happen
 *	    because there can be fonts defined but never used
 *	    and we dont want to load them unnecessarily.
 *
 *  Note: this table is never de-allocated.
 */

#define UNUSED_FONT -1
#define NOT_LOADED -1
struct fontdes {
    short size;		/* size = -1 means unused font (must be short) */
    short weight;	/* this is already shifted into position */
    short family;
    short ecfno;	/* -1 means not font not loaded yet */
} *al_font_defs = NULL;

/**--------------------------------------------------------------------------**/
Font_inherit_t font_inherit_all = 
{ 
    fn_INHERIT_DIRECT,
    fn_INHERIT_DIRECT,
    fn_INHERIT_DIRECT,
    fn_INHERIT_DIRECT,
    fn_INHERIT_DIRECT,
    fn_INHERIT_DIRECT,
    fn_INHERIT_DIRECT,
    fn_INHERIT_DIRECT,
    fn_INHERIT_DIRECT,
    fn_INHERIT_DIRECT,
    fn_INHERIT_DIRECT,
    fn_INHERIT_DIRECT,
    fn_INHERIT_DIRECT,
    fn_INHERIT_DIRECT,
    fn_INHERIT_DIRECT 
};

Font_inherit_t font_inherit_nothing =
{ 
    fn_INHERIT_NONE,
    fn_INHERIT_NONE,
    fn_INHERIT_NONE,
    fn_INHERIT_NONE,
    fn_INHERIT_NONE,
    fn_INHERIT_NONE,
    fn_INHERIT_NONE,
    fn_INHERIT_NONE,
    fn_INHERIT_NONE,
    fn_INHERIT_NONE,
    fn_INHERIT_NONE,
    fn_INHERIT_NONE,
    fn_INHERIT_NONE,
    fn_INHERIT_NONE,
    fn_INHERIT_NONE 
};

/** pair kerning and black on by default. **/
Font_attr_t font_attr_nothing = { 0,0,0,0,0,0,YES,0,0,0,clr_BLACK,0 };

/**--------------------------------------------------------------------------**/
Font_info_t *
ArFontToken2Definition( font_token, fnP )
  register short font_token;
  register Font_info_t *fnP;
{
    for ( ; fnP != NULL; fnP = (Font_info_t *)fnP->nextP) {
	if (font_token == fnP->font_token) {
	    break;
	}
    }
    return( fnP );
}
/**--------------------------------------------------------------------------**/
bool
ArFontValueEqual(lfP, rfP)
  register Font_value_t *lfP, *rfP;
{
    return(
	lfP->font_token        == rfP->font_token		&&
	lfP->font_attr.str     == rfP->font_attr.str		&&
	lfP->font_attr.und     == rfP->font_attr.und		&&
	lfP->font_attr.lang    == rfP->font_attr.lang		&&
	lfP->font_attr.rev     == rfP->font_attr.rev		&&
	lfP->font_attr.sub     == rfP->font_attr.sub		&&
	lfP->font_attr.sup     == rfP->font_attr.sup		&&
	lfP->font_attr.kpr     == rfP->font_attr.kpr		&&
	lfP->font_attr.trk     == rfP->font_attr.trk		&&
	lfP->font_attr.dbl     == rfP->font_attr.dbl		&&
	lfP->font_attr.ovr     == rfP->font_attr.ovr		&&
	lfP->font_attr.clr     == rfP->font_attr.clr		&&
	lfP->font_attr.cap     == rfP->font_attr.cap		&&
	lfP->font_inherit.size == rfP->font_inherit.size	&&
	lfP->font_inherit.fam  == rfP->font_inherit.fam 	&&
	lfP->font_inherit.lang == rfP->font_inherit.lang	&&
	lfP->font_inherit.bold == rfP->font_inherit.bold	&&
	lfP->font_inherit.ital == rfP->font_inherit.ital	&&
	lfP->font_inherit.str  == rfP->font_inherit.str 	&&
	lfP->font_inherit.und  == rfP->font_inherit.und 	&&
	lfP->font_inherit.rev  == rfP->font_inherit.rev 	&&
	lfP->font_inherit.sup  == rfP->font_inherit.sup 	&&
	lfP->font_inherit.sub  == rfP->font_inherit.sub 	&&
	lfP->font_inherit.kpr  == rfP->font_inherit.kpr 	&&
	lfP->font_inherit.trk  == rfP->font_inherit.trk 	&&
	lfP->font_inherit.cap  == rfP->font_inherit.cap 	&&
	lfP->font_inherit.clr  == rfP->font_inherit.clr 	&&
	lfP->font_inherit.ovr  == rfP->font_inherit.ovr
    );
}
/**--------------------------------------------------------------------------**/
bool
ArFontValueInheritAll(fvP)
  register Font_value_t *fvP;
{
    return(
	fvP->font_inherit.size == fn_INHERIT_DIRECT	&&
	fvP->font_inherit.fam  == fn_INHERIT_DIRECT 	&&
	fvP->font_inherit.lang == fn_INHERIT_DIRECT	&&
	fvP->font_inherit.bold == fn_INHERIT_DIRECT	&&
	fvP->font_inherit.ital == fn_INHERIT_DIRECT	&&
	fvP->font_inherit.str  == fn_INHERIT_DIRECT 	&&
	fvP->font_inherit.und  == fn_INHERIT_DIRECT 	&&
	fvP->font_inherit.rev  == fn_INHERIT_DIRECT 	&&
	fvP->font_inherit.sup  == fn_INHERIT_DIRECT 	&&
	fvP->font_inherit.sub  == fn_INHERIT_DIRECT 	&&
	fvP->font_inherit.kpr  == fn_INHERIT_DIRECT 	&&
	fvP->font_inherit.trk  == fn_INHERIT_DIRECT 	&&
	fvP->font_inherit.cap  == fn_INHERIT_DIRECT 	&&
	fvP->font_inherit.clr  == fn_INHERIT_DIRECT 	&&
	fvP->font_inherit.ovr  == fn_INHERIT_DIRECT
    );
}
/**--------------------------------------------------------------------------**/
bool
ArFontGetValue(fiP, fvP)
  File_info_t *fiP;
  Font_value_t *fvP;
{
  shar my_buf[AR_MAX_WORD];

    if (ArGetWordShrC(fiP, NOCHAR, my_buf, NUM_OF(my_buf)-1, 0) == NO ||
        ArFontTokenShr(fiP, my_buf, LOOSE, P_DUMMY) == NO) {
	    return( NO );
    }
    else {
	fvP->font_token   = al_font_number;
	fvP->font_inherit = al_font_inherit;
	fvP->font_attr    = al_attrs;
	return( YES );
    }
}

/**--------------------------------------------------------------------------**/
/**	Font values are sort of weird.  A master can set both attributes     **/
/**	and inheritance, because if used at top-level cmpn, tps uses the     **/
/**	attributes, and if used as in-line cmpn, tps uses the inheritance.   **/
/**	We don't have those semantic distinctions, and in fact don't         **/
/**	know from which context the caller wants this font value written.    **/
/**	The special cases are fnP == NULL, which must be for a master (or    **/
/**	a pathological document), and *not* inheriting the font family,      **/
/**	which means the family *must* be dumped and dumped before any other  **/
/**	font info.							     **/

/**	Note the function below; ArSsFontValueWrite(), which is similar to   **/
/**	ArFontValueWrite() except it prints to a Ssave_t.  If one function   **/
/**	is updated, most likely the other will have to be touched also.  MS  **/

static char direct[]       = "@%c";
static char inher_direct[] = "@i%c";
static char inher_with[]   = "@i%c%c";
static char value_str[]    = "@%c%02d";
static char value_str0[]    = "@%c%d";
#define out_direct(ofile, thing) ArFprintf(ofile, direct, thing)
#define out_inh_direct(ofile, thing) ArFprintf(ofile, inher_direct, thing)
#define out_inh_with(ofile, way, thing) ArFprintf(ofile, inher_with, way,thing)
#define out_value(ofile, way, val) ArFprintf(ofile, value_str, way, val)
#define out_value0(ofile, way, val) ArFprintf(ofile, value_str0, way, val)
void
ArFontValueWrite(fvP, fnP, ofile)
  Font_value_t *fvP;
  Font_info_t *fnP;
  FILE *ofile;
{
  bool	fam_dumped = NO;
  Font_info_t *my_fnP;

    if (fvP == NULL || ofile == NULL) { 
	return; 
    }
    /** special case where we must write the family info.	BT **/
    
    if (fnP == NULL || fvP->font_inherit.fam == fn_INHERIT_NONE) {
	ArFprintf(ofile, "F%d", fvP->font_token);
	fam_dumped = YES;
    }
    /**--------------------------**/
    /** first do font attributes.**/
    /**--------------------------**/
    if (fvP->font_attr.lang != lang_DEFAULT) {
	  ArFprintf(ofile, FONT_LANG_FMT, Language_names[fvP->font_attr.lang]);
    }
    if (fvP->font_attr.str) {
	  out_direct(ofile, STRIKECHAR);
    }
    if (fvP->font_attr.und == YES) {
	  if (fvP->font_attr.dbl == YES) {
	      out_direct(ofile, DBLCHAR);
	  }
	  else {
	      out_direct(ofile, UNDERCHAR);
	  }
    }
    if (fvP->font_attr.dbl == YES) {
	  out_direct(ofile, DBLCHAR);
    }
    if (fvP->font_attr.rev == YES) {
	  out_direct(ofile, REVCHAR);
    }
    if (fvP->font_attr.sup == YES) {
	  out_direct(ofile, SUPERCHAR);
    }
    if (fvP->font_attr.sub == YES) {
	  out_direct(ofile, SUBCHAR);
    }
    /** Note this is inverted from the others! **/
    if (fvP->font_attr.kpr == NO) {
	  out_direct(ofile, PAIRCHAR);
    }
    if (fvP->font_attr.trk != trk_OFF && fvP->font_attr.trk != trk_UNUSED) {
	out_value0(ofile, TRACKCHAR, fvP->font_attr.trk);
    }
    switch (fvP->font_attr.cap) {
      default:
      case cap_NORMAL:
	  break;
      case cap_ALL:
	  ArFprintf(ofile, "@C");
	  break;
      case cap_LOWSMALL:
	  ArFprintf(ofile, "@X");
	  break;
      case cap_ALLSMALL:
	  ArFprintf(ofile, "@A");
	  break;
    }
    if (fvP->font_attr.clr != clr_BLACK) {
	out_value0(ofile, COLORCHAR, fvP->font_attr.clr);
    }
    if (fvP->font_attr.ovr == YES) {
	out_direct(ofile, OVERCHAR);
    }
    /**--------------------------**/
    /** now do inheritance.      **/
    /**--------------------------**/
    switch (fvP->font_inherit.fam) {
      case fn_INHERIT_NONE:
	  if (fam_dumped == NO) {
	      ArFprintf(ofile, "F%d", fvP->font_token);
	      fam_dumped = YES;
	  }
	  break;
      case fn_INHERIT_DIRECT:
	  out_inh_direct(ofile, FAMCHAR);
	  break;
      default:
	  /** won't happen. **/
	  break;
    }
    switch (fvP->font_inherit.size) {
      case fn_INHERIT_NONE:
	  if (fam_dumped == YES) {
	      break;
	  }
	  my_fnP = ArFontToken2Definition(fvP->font_token, fnP);
	  out_value(ofile, SIZECHAR, ((my_fnP==NULL)? 0 : my_fnP->font_size));
	  break;
      case fn_INHERIT_LESS:
	  out_inh_with(ofile, LESSCHAR, SIZECHAR);
	  break;
      case fn_INHERIT_MORE:
	  out_inh_with(ofile, MORECHAR, SIZECHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  out_inh_direct(ofile, SIZECHAR);
      default:
	  break;
    }
    switch (fvP->font_inherit.bold) {
      case fn_INHERIT_NONE:
	  if (fam_dumped == YES) {
	      break;
	  }
	  my_fnP = ArFontToken2Definition(fvP->font_token, fnP);
	  if (my_fnP != NULL && 
	      (my_fnP->font_face==face_BOLD || 
	       my_fnP->font_face==face_BOLD_ITALIC)) {
		  out_direct(ofile, BOLDCHAR);
	  }
	  break;
      case fn_INHERIT_TOGGLE:
	  out_inh_with(ofile, TOGGLECHAR, BOLDCHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  out_inh_direct(ofile, BOLDCHAR);
      default:
	  break;
    }
    switch (fvP->font_inherit.ital) {
      case fn_INHERIT_NONE:
	  if (fam_dumped == YES) {
	      break;
	  }
	  my_fnP = ArFontToken2Definition(fvP->font_token, fnP);
	  if (my_fnP != NULL && 
	      (my_fnP->font_face==face_ITALIC || 
	       my_fnP->font_face==face_BOLD_ITALIC)) {
		  out_direct(ofile, ITALCHAR);
	  }
	  break;
      case fn_INHERIT_TOGGLE:
	  out_inh_with(ofile, TOGGLECHAR, ITALCHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  out_inh_direct(ofile, ITALCHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.lang) {
      case fn_INHERIT_DIRECT:
	  out_inh_direct(ofile, LANGCHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.str) {
      case fn_INHERIT_TOGGLE:
	  out_inh_with(ofile, TOGGLECHAR, STRIKECHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  out_inh_direct(ofile, STRIKECHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.und) {
      case fn_INHERIT_LESS:
	  out_inh_with(ofile, LESSCHAR, UNDERCHAR);
	  break;
      case fn_INHERIT_MORE:
	  out_inh_with(ofile, MORECHAR, UNDERCHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  out_inh_direct(ofile, UNDERCHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.rev) {
      case fn_INHERIT_TOGGLE:
	  out_inh_with(ofile, TOGGLECHAR, SIZECHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  out_inh_direct(ofile, REVCHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.sup) {
      case fn_INHERIT_TOGGLE:
	  out_inh_with(ofile, TOGGLECHAR, SUPERCHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  out_inh_direct(ofile, SUPERCHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.sub) {
      case fn_INHERIT_TOGGLE:
	  out_inh_with(ofile, TOGGLECHAR, SUBCHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  out_inh_direct(ofile, SUBCHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.kpr) {
      case fn_INHERIT_TOGGLE:
	  out_inh_with(ofile, TOGGLECHAR, PAIRCHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  out_inh_direct(ofile, PAIRCHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.trk) {
      case fn_INHERIT_LESS:
	  out_inh_with(ofile, LESSCHAR, TRACKCHAR);
	  break;
      case fn_INHERIT_MORE:
	  out_inh_with(ofile, MORECHAR, TRACKCHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  out_inh_direct(ofile, TRACKCHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.cap) {
      case fn_INHERIT_DIRECT:
	  out_inh_direct(ofile, CAPCHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.clr) {
      case fn_INHERIT_DIRECT:
	  out_inh_direct(ofile, COLORCHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.ovr) {
      case fn_INHERIT_TOGGLE:
	  out_inh_with(ofile, TOGGLECHAR, OVERCHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  out_inh_direct(ofile, OVERCHAR);
	  break;
      default:
	  break;
    }
}

#define Sout_direct(thing) sprintf(temp,direct,thing);SsSaveChr(temp,out_ssP)
#define Sout_inh_direct(thing) sprintf(temp,inher_direct,thing);SsSaveChr(temp,out_ssP)
#define Sout_inh_with(way,thing) sprintf(temp,inher_with,way,thing);SsSaveChr(temp,out_ssP)
#define Sout_value(way, val) sprintf(temp,value_str,way,val);SsSaveChr(temp,out_ssP)
#define Sout_value0(way, val) sprintf(temp,value_str,way,val);SsSaveChr(temp,out_ssP)
void
ArSsFontValueWrite(fvP, fnP, out_ssP)
  Font_value_t *fvP;
  Font_info_t *fnP;
  Ssave_t *out_ssP;
{
  bool	fam_dumped = NO;
  Font_info_t *my_fnP;
  char		temp[80];

    if (fvP == NULL || out_ssP == NULL) { 
	return; 
    }
    /** special case where we must write the family info.	BT **/
    
    if (fnP == NULL || fvP->font_inherit.fam == fn_INHERIT_NONE) {
	sprintf(temp, "F%d", fvP->font_token);
	SsSaveChr(temp,out_ssP);
	fam_dumped = YES;
    }
    /**--------------------------**/
    /** first do font attributes.**/
    /**--------------------------**/
    if (fvP->font_attr.lang != lang_DEFAULT) {
	  sprintf(temp, FONT_LANG_FMT, Language_names[fvP->font_attr.lang]);
	  SsSaveChr(temp,out_ssP);
    }
    if (fvP->font_attr.str) {
	  Sout_direct(STRIKECHAR);
    }
    if (fvP->font_attr.und == YES) {
	  if (fvP->font_attr.dbl == YES) {
	      Sout_direct(DBLCHAR);
	  }
	  else {
	      Sout_direct(UNDERCHAR);
	  }
    }
    if (fvP->font_attr.dbl == YES) {
	  Sout_direct(DBLCHAR);
    }
    if (fvP->font_attr.rev == YES) {
	  Sout_direct(REVCHAR);
    }
    if (fvP->font_attr.sup == YES) {
	  Sout_direct(SUPERCHAR);
    }
    if (fvP->font_attr.sub == YES) {
	  Sout_direct(SUBCHAR);
    }
    /** Note this is inverted from the others! **/
    if (fvP->font_attr.kpr == NO) {
	  Sout_direct(PAIRCHAR);
    }
    if (fvP->font_attr.trk != trk_OFF && fvP->font_attr.trk != trk_UNUSED) {
	Sout_value0(TRACKCHAR, fvP->font_attr.trk);
    }
    switch (fvP->font_attr.cap) {
      default:
      case cap_NORMAL:
	  break;
      case cap_ALL:
	  sprintf(temp,"@C");
	  SsSaveChr(temp,out_ssP);
	  break;
      case cap_LOWSMALL:
	  sprintf(temp,"@X");
	  SsSaveChr(temp,out_ssP);
	  break;
      case cap_ALLSMALL:
	  sprintf(temp,"@A");
	  SsSaveChr(temp,out_ssP);
	  break;
    }
    if (fvP->font_attr.clr != clr_BLACK) {
	Sout_value0(COLORCHAR, fvP->font_attr.clr);
    }
    if (fvP->font_attr.ovr == YES) {
	Sout_direct(OVERCHAR);
    }
    /**--------------------------**/
    /** now do inheritance.      **/
    /**--------------------------**/
    switch (fvP->font_inherit.fam) {
      case fn_INHERIT_NONE:
	  if (fam_dumped == NO) {
	      sprintf(temp,"F%d", fvP->font_token);
	      SsSaveChr(temp,out_ssP);
	      fam_dumped = YES;
	  }
	  break;
      case fn_INHERIT_DIRECT:
	  Sout_inh_direct(FAMCHAR);
	  break;
      default:
	  /** won't happen. **/
	  break;
    }
    switch (fvP->font_inherit.size) {
      case fn_INHERIT_NONE:
	  if (fam_dumped == YES) {
	      break;
	  }
	  my_fnP = ArFontToken2Definition(fvP->font_token, fnP);
	  Sout_value(SIZECHAR, ((my_fnP==NULL)? 0 : my_fnP->font_size));
	  break;
      case fn_INHERIT_LESS:
	  Sout_inh_with(LESSCHAR, SIZECHAR);
	  break;
      case fn_INHERIT_MORE:
	  Sout_inh_with(MORECHAR, SIZECHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  Sout_inh_direct(SIZECHAR);
      default:
	  break;
    }
    switch (fvP->font_inherit.bold) {
      case fn_INHERIT_NONE:
	  if (fam_dumped == YES) {
	      break;
	  }
	  my_fnP = ArFontToken2Definition(fvP->font_token, fnP);
	  if (my_fnP != NULL && 
	      (my_fnP->font_face==face_BOLD || 
	       my_fnP->font_face==face_BOLD_ITALIC)) {
		  Sout_direct(BOLDCHAR);
	  }
	  break;
      case fn_INHERIT_TOGGLE:
	  Sout_inh_with(TOGGLECHAR, BOLDCHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  Sout_inh_direct(BOLDCHAR);
      default:
	  break;
    }
    switch (fvP->font_inherit.ital) {
      case fn_INHERIT_NONE:
	  if (fam_dumped == YES) {
	      break;
	  }
	  my_fnP = ArFontToken2Definition(fvP->font_token, fnP);
	  if (my_fnP != NULL && 
	      (my_fnP->font_face==face_ITALIC || 
	       my_fnP->font_face==face_BOLD_ITALIC)) {
		  Sout_direct(ITALCHAR);
	  }
	  break;
      case fn_INHERIT_TOGGLE:
	  Sout_inh_with(TOGGLECHAR, ITALCHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  Sout_inh_direct(ITALCHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.lang) {
      case fn_INHERIT_DIRECT:
	  Sout_inh_direct(LANGCHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.str) {
      case fn_INHERIT_TOGGLE:
	  Sout_inh_with(TOGGLECHAR, STRIKECHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  Sout_inh_direct(STRIKECHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.und) {
      case fn_INHERIT_LESS:
	  Sout_inh_with(LESSCHAR, UNDERCHAR);
	  break;
      case fn_INHERIT_MORE:
	  Sout_inh_with(MORECHAR, UNDERCHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  Sout_inh_direct(UNDERCHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.rev) {
      case fn_INHERIT_TOGGLE:
	  Sout_inh_with(TOGGLECHAR, SIZECHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  Sout_inh_direct(REVCHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.sup) {
      case fn_INHERIT_TOGGLE:
	  Sout_inh_with(TOGGLECHAR, SUPERCHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  Sout_inh_direct(SUPERCHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.sub) {
      case fn_INHERIT_TOGGLE:
	  Sout_inh_with(TOGGLECHAR, SUBCHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  Sout_inh_direct(SUBCHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.kpr) {
      case fn_INHERIT_TOGGLE:
	  Sout_inh_with(TOGGLECHAR, PAIRCHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  Sout_inh_direct(PAIRCHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.trk) {
      case fn_INHERIT_LESS:
	  Sout_inh_with(LESSCHAR, TRACKCHAR);
	  break;
      case fn_INHERIT_MORE:
	  Sout_inh_with(MORECHAR, TRACKCHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  Sout_inh_direct(TRACKCHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.cap) {
      case fn_INHERIT_DIRECT:
	  Sout_inh_direct(CAPCHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.clr) {
      case fn_INHERIT_DIRECT:
	  Sout_inh_direct(COLORCHAR);
	  break;
      default:
	  break;
    }
    switch (fvP->font_inherit.ovr) {
      case fn_INHERIT_TOGGLE:
	  Sout_inh_with(TOGGLECHAR, OVERCHAR);
	  break;
      case fn_INHERIT_DIRECT:
	  Sout_inh_direct(OVERCHAR);
	  break;
      default:
	  break;
    }
}
/**--------------------------------------------------------------------------**/

/* Sets 'al_font_number' and 'al_attrs', which are used by
 * AlGetFont, AlDoFontDefs and AlDoFont. Sets al_pop_font if /F seen.
 * This is called in AlToken whenever any <...> is being checked for.
 * It is also called in AlGetFont for cases like Font = F5.
 * Note: this also calls AlGetch to get attribute characters! */
    
bool
ArFontTokenShr(fiP, word, precision, type)
File_info_t *fiP;
register short* word;	/* string: e.g. F5 (but see note above) */
int precision;		/* LOOSE or EXACT */
int type;
{
    register int c;
    shar lang_letters[3];
    bool inherit,toggle,more,less;
    
    al_attrs = font_attr_nothing;
    al_font_inherit = font_inherit_nothing;
    al_font_face_op = fn_FACE_SAME;
    al_font_size = 0;
    al_fam = 0;
    if(*word == '\0')
    { 
	if(type == P_CLASS)	/* possible inheritance case */
	    return(FALSE);
    }
    else
    {
	if(al_pop_font = (*word == al_cmd_end)) ++word; /* found </F> */
	if(precision == EXACT){
	    if(*word++ != 'F') return(FALSE);
	} else {
	    if(ArForceCap(*word++) != 'F') return(FALSE);
	}
	if(*(word + 1) == '\0') {
	    if((c = ArForceCap(*word)) == 'I') {
		al_font_number = ITALIC_FONT;
		return(TRUE);
	    } else if(c == 'B') {
		al_font_number = BOLD_FONT;
		return(TRUE);
	    }
	}
	/* Try to convert word to an integer, if failure return FALSE.  */
	if (!ArCvtIntShr(word, &al_font_number, TRUE)) return(FALSE);
    }
    /* If the next char in stream is an at-sign then get and interpret
       chars till a terminator is seen.  In either case push back last char. */
    if((c = ArGetchNW(fiP)) == AT_SIGN)
    {
	ArUnGetch(c, fiP);
	while((c = ArGetchNW(fiP)) == AT_SIGN) {
	    inherit = FALSE;
	    toggle = FALSE;
	    more = FALSE;
	    less = FALSE;
	    while (c != '>' && c != ',' && c != '=')
	    {
		switch(c = ArGetchNW(fiP)) 
		{
                case AT_SIGN:
		    break;
	        case INHERCHAR:
		    inherit = TRUE;
		    break;
		case TILDA:
		    toggle = TRUE;
		    break;
		case MORECHAR:
		    more = TRUE;
		    break;
		case LESSCHAR:
		    less = TRUE;
		    break;
		case UNDERCHAR:
		    if(!inherit)
		    {
			if(toggle)
			    al_attrs.und ^= YES;
			else
			    al_attrs.und = YES;
			al_attrs.dbl = NO;
			al_font_inherit.und = fn_INHERIT_NONE;
		    }
		    else
		    {
			if(more)
			    al_font_inherit.und = fn_INHERIT_MORE;
			else if (less)
			    al_font_inherit.und = fn_INHERIT_LESS;
			else
			    al_font_inherit.und = fn_INHERIT_DIRECT;
#if 0	/** BT **/
			if(type != P_CLASS)
			    al_attrs = EcInherFontUnder(al_cur_font,al_attrs,
							al_font_inherit.und);
#endif	/** BT **/
		    }
		    break;
		case STRIKECHAR:
		    if(!inherit)
		    {
			if(toggle)
			    al_attrs.str ^= YES;
			else
			    al_attrs.str = YES;
			al_font_inherit.str = fn_INHERIT_NONE;
		    }
		    else
		    {
			if(toggle)
			    al_font_inherit.str = fn_INHERIT_TOGGLE;
			else
			    al_font_inherit.str = fn_INHERIT_DIRECT;
#if 0	/** BT **/
			if(type != P_CLASS)
			    al_attrs = EcInherFontStrike(al_cur_font,al_attrs,
							 al_font_inherit.str);
#endif	/** BT **/
		    }
		    break;
		case REVCHAR:
		    if(!inherit)
		    {
			if(toggle)
			    al_attrs.rev ^= YES;
			else
			    al_attrs.rev = YES;
			al_font_inherit.rev = fn_INHERIT_NONE;
		    }
		    else
		    {
			if(toggle)
			    al_font_inherit.rev = fn_INHERIT_TOGGLE;
			else
			    al_font_inherit.rev = fn_INHERIT_DIRECT;
#if 0	/** BT **/
			if(type != P_CLASS)
			    al_attrs = EcInherFontRev(al_cur_font,al_attrs,
						      al_font_inherit.rev);
#endif	/** BT **/
		    }
		    break;
		case SUPERCHAR:
		    if(!inherit)
		    {
			if(toggle)
			    al_attrs.sup ^= YES;
			else
			{
			    al_attrs.sup = YES;
			    if(al_attrs.sub == YES)
			    {
				al_attrs.sub = NO;
				al_font_inherit.sub = fn_INHERIT_NONE;
			    }
			    al_font_inherit.sup = fn_INHERIT_NONE;
			}
		    }
		    else
		    {
			if(toggle)
			    al_font_inherit.sup = fn_INHERIT_TOGGLE;
			else
			    al_font_inherit.sup = fn_INHERIT_DIRECT;
#if 0	/** BT **/
			if(type != P_CLASS)
			{
			    al_attrs = EcInherFontSuper(al_cur_font,al_attrs,
							al_font_inherit.sup);
			    al_attrs.sub = NO;
			}
#endif	/** BT **/
		    }
		    break;
		case SUBCHAR:
		    if(!inherit)
		    {
			if(toggle)
			    al_attrs.sub ^= YES;
			else
			{
			    al_attrs.sub = YES;
			    if(al_attrs.sup == YES)
			    {
				al_attrs.sup = NO;
				al_font_inherit.sup = fn_INHERIT_NONE;
			    }
			    al_font_inherit.sub = fn_INHERIT_NONE;
			}
		    }
		    else
		    {
			if(toggle)
			    al_font_inherit.sub = fn_INHERIT_TOGGLE;
			else
			    al_font_inherit.sub = fn_INHERIT_DIRECT;
#if 0	/** BT **/
			if(type != P_CLASS)
			{
			    al_attrs = EcInherFontSub(al_cur_font,al_attrs,
						      al_font_inherit.sub);
			    al_attrs.sub = NO;
			}
#endif	/** BT **/
		    }
		    break;
		case OVERCHAR:
		    if(!inherit)
		    {
			if(toggle)
			    al_attrs.ovr ^= YES;
			else
			    al_attrs.ovr = YES;
			al_font_inherit.ovr = fn_INHERIT_NONE;
		    }
		    else
		    {
			if(toggle)
			    al_font_inherit.ovr = fn_INHERIT_TOGGLE;
			else 
			    al_font_inherit.ovr = fn_INHERIT_DIRECT;
#if 0	/** BT **/
			if(type != P_CLASS)
			    al_attrs = EcInherFontOver(al_cur_font,al_attrs,
						       al_font_inherit.ovr);
#endif	/** BT **/
		    }
		    break;
		case DBLCHAR:
		    al_attrs.dbl = YES;
		    al_attrs.und = NO;
		    al_font_inherit.und = fn_INHERIT_NONE;
		    break;
		case 'A':
		    al_attrs.cap = cap_ALLSMALL;
		    al_font_inherit.cap = fn_INHERIT_NONE;
		    break;
		case 'X':
		    al_attrs.cap = cap_LOWSMALL;
		    al_font_inherit.cap = fn_INHERIT_NONE;
		    break;
		case 'C':
		    if(!inherit)
		    {
			al_attrs.cap = cap_ALL;
			al_font_inherit.cap = fn_INHERIT_NONE;
		    }
		    else
		    {
			al_font_inherit.cap = fn_INHERIT_DIRECT;
#if 0	/** BT **/
			if(type != P_CLASS)
			    al_attrs = EcInherFontCaps(al_cur_font,al_attrs);
#endif	/** BT **/
		    }
		    break;
		/* pair kerning off is indicated by a @P, so the normal case
		 * is not dumped */
		case PAIRCHAR:
		    if(!inherit)
		    {
			if(toggle)
			    al_attrs.kpr = YES;
			else
			    al_attrs.kpr = NO;
			al_font_inherit.kpr = fn_INHERIT_NONE;
		    }
		    else
		    {
			if(toggle)
			    al_font_inherit.kpr = fn_INHERIT_TOGGLE;
			else
			    al_font_inherit.kpr = fn_INHERIT_DIRECT;
#if 0	/** BT **/
			if(type != P_CLASS)
			    al_attrs = EcInherFontPair(al_cur_font,al_attrs,
						       al_font_inherit.kpr);
#endif	/** BT **/
		    }
		    break;
		case TRACKCHAR:
		    if(!inherit)
		    {
			c = ArGetchNW(fiP);
			al_attrs.trk = c-'0';
			al_font_inherit.trk = fn_INHERIT_NONE;
		    }
		    else
		    {
			if(more)
			    al_font_inherit.trk = fn_INHERIT_MORE;
			else if(less)
			    al_font_inherit.trk = fn_INHERIT_LESS;
			else
			    al_font_inherit.trk = fn_INHERIT_DIRECT;
#if 0	/** BT **/
			if(type != P_CLASS)
			    al_attrs = EcInherFontTrack(al_cur_font,al_attrs,
							al_font_inherit.trk);
#endif	/** BT **/
		    }
		    break;
		case COLORCHAR:
		    if(!inherit)
		    {
			c = ArGetchNW(fiP);
			al_attrs.clr = c-'0';
			al_font_inherit.clr = fn_INHERIT_NONE;
		    }
		    else
		    {
			al_font_inherit.clr = fn_INHERIT_DIRECT;
#if 0	/** BT **/
			if(type != P_CLASS)
			    al_attrs = EcInherFontColor(al_cur_font,al_attrs);
#endif	/** BT **/
		    }
		    break;
		case LANGCHAR:
		    if(!inherit)
		    {
			struct keytab *ktP;
			c = ArGetchNW(fiP);
			lang_letters[0]=c;
			c = ArGetchNW(fiP);
			lang_letters[1]=c;
			lang_letters[2]='\0';
			ktP  = ArBinaryShr(lang_letters,
						ar_lang_names, 
						NUM_OF(ar_lang_names),
						LOOSE,
						NO);
			al_attrs.lang  = ((ktP == NULL)?lang_NL:ktP->keyval);
			al_font_inherit.lang = fn_INHERIT_NONE;
		    }
		    else
		    {
			al_font_inherit.lang = fn_INHERIT_DIRECT;
#if 0	/** BT **/
			if(type != P_CLASS)
			    al_attrs = EcInherFontLang(al_cur_font,al_attrs);
#endif	/** BT **/
		    }
		    break;
		case BOLDCHAR:
		    if(!inherit)
		    {
			if(toggle)
			    al_font_face_op = 
				((al_font_face_op & ~fn_BOLD_FACE_OP) |
				 fn_BOLD_FACE_ON);
			else 
			    al_font_face_op =
				((al_font_face_op &  ~fn_BOLD_FACE_OP) |
				 fn_BOLD_FACE_OFF);
			al_font_inherit.bold = fn_INHERIT_NONE;
		    }
		    else	
		    {
			if(toggle)
			    al_font_inherit.bold = fn_INHERIT_TOGGLE;
			else
			    al_font_inherit.bold = fn_INHERIT_DIRECT;
#if 0	/** BT **/
			if(type != P_CLASS)
			    al_font_face_op = 
				((al_font_face_op & ~fn_BOLD_FACE_OP) |
				 (int)EcInherFontBold(al_cur_font,
						 al_font_inherit.bold));
#endif	/** BT **/
		    }
		    break;
		case ITALCHAR:
		    if(!inherit)
		    {
			if(toggle)
			    al_font_face_op = 
				((al_font_face_op & ~fn_ITALIC_FACE_OP) |
				 fn_ITALIC_FACE_ON);
			else 
			    al_font_face_op =
				((al_font_face_op &  ~fn_ITALIC_FACE_OP) |
				 fn_BOLD_FACE_OFF);
			al_font_inherit.ital = fn_INHERIT_NONE;
		    }
		    else	
		    {
			if(toggle)
			    al_font_inherit.ital = fn_INHERIT_TOGGLE;
			else
			    al_font_inherit.ital = fn_INHERIT_DIRECT;
#if 0	/** BT **/
			if(type != P_CLASS)
			    al_font_face_op = 
				((al_font_face_op & ~fn_ITALIC_FACE_OP) |
				 (int)EcInherFontItalic(al_cur_font,
						   al_font_inherit.ital));
#endif	/** BT **/
		    }
		    break;
		case SIZECHAR:
		    if(!inherit)
		    {
			ArGetWordShrC(fiP,NOCHAR,al_token_shr,AR_MAX_WORD-1,0);
			ArCvtIntShr(al_token_shr,&al_font_size,TRUE);
			al_font_inherit.size = fn_INHERIT_NONE;
		    }
		    else
		    {
			if(more)
			    al_font_inherit.size = fn_INHERIT_MORE;
			else if(less)
			    al_font_inherit.size = fn_INHERIT_LESS;
			else
			    al_font_inherit.size = fn_INHERIT_DIRECT;
#if 0	/** BT **/
			if(type != P_CLASS)
			    al_font_size = 
				(int)EcInherFontSize(al_cur_font,
						al_font_inherit.size);
#endif	/** BT **/
		    }
		    break;
		case FAMCHAR:
		    if(!inherit)
		    {
			ArGetWordShrC(fiP, NOCHAR, al_token_shr, AR_MAX_WORD,0);
			/* assume F */
			ArCvtIntShr(&al_token_shr[1],&al_fam,TRUE);
			al_fam = al_font_defs[al_fam].family;
			al_font_inherit.fam = fn_INHERIT_NONE;
		    }
		    else
		    {
			al_font_inherit.fam = fn_INHERIT_DIRECT;
#if 0	/** BT **/
			if(type != P_CLASS)
			    al_fam = EcGetFontFamily(al_cur_font);
#endif	/** BT **/
		    }
		    break;
		case ALLCHAR:
		    /* error if inherit != TRUE */
		    al_font_inherit = font_inherit_all;
#if 0	/** BT **/
		    if(type != P_CLASS) {
			extern Font_attr_t ATTRS();
			al_attrs = ATTRS(al_cur_font);
		    }
#endif	/** BT **/
		    break;
		default:
		    break;
		}
		if(c == AT_SIGN)
		    break;
	    }
	    ArUnGetch(c, fiP);
	}
    }
    ArUnGetch(c, fiP);
    return(TRUE);
}

/* This is very similar to AlFontToken, but it does not get the 
 * attribute part or set the globals. It is used by the dumper
 * via AlCmmdToken.
 */
bool
ArFontTokenPreAttrShr(word)
register short* word;	/* string: e.g. F5 */
{
    register int c;
    short dummy;
    
    if(*word == al_cmd_end) ++word; /* found </F> */
    if(ArForceCap(*word++) != 'F') return(FALSE);
    if(*(word + 1) == '\0') {
	if((c = ArForceCap(*word)) == 'I') {
	    return(TRUE);
	} else if(c == 'B') {
	    return(TRUE);
	}
    }
    /* Try to convert word to an integer, if failure return FALSE.  */
    if (!ArCvtIntShr(word, &dummy, TRUE))
	return(FALSE);
    return (TRUE);
}

