
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
/*	ar5autonum.c                             */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  08/88    BT	Original version
 */

/**--------------------------------------------------------------------------**/
/**	These routines build an autonum instance and return a pointer to     **/
/**	its structure.  the structure is used either in a numbered frame     **/
/**	or in a cmpn buffer.						     **/
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
#define AN_RESTART 1
#define AN_FIRST 2
#define AN_TAG 3
#define AN_VALUE 4

struct keytab al_anumtok_tokens[] = 
{ /* this list must remain sorted */
    {"First", AN_FIRST},
    {"Restart", AN_RESTART},
    {"Tag", AN_TAG},
    {"Tagname", AN_TAG},
    {"Value", AN_VALUE}
};

#define NUMATOKTS (sizeof(al_anumtok_tokens) / sizeof(struct keytab))

/**--------------------------------------------------------------------------**/
/** 	this routine may be called with the input starting at 		     **/
/**	'<Autonum, name, ...' 	or at  ', name, ...' or at 'name, ...'	     **/
/**	Be broad-minded and gobble those various precursors!		     **/

Autonum_t *
ArBuildAutonum(fiP)
  File_info_t *fiP;
{
  Autonum_t *anP = NEW(Autonum_t);
  shar c;
  int levelnum;

    /** init it in place. **/
    anP->name[0]  = '\0';
    anP->tag[0]   = '\0';
    anP->value[0] = '\0';
    anP->first    = NO;
    anP->restart  = NO;
    anP->level    = 0;

    if ((c = ArGetchNW(fiP)) == AR_CMD_CHAR) {
	/** assume it is 'Autonum' **/
	ArGetWordShrC(fiP, NOCHAR, al_token_shr, AR_MAX_WORD-1, COMMA);
	c = NOCHAR;
    }
    else if (c == COMMA) {
	c = NOCHAR;
    }
    ArGetWordShrC(fiP, c, anP->name, NUM_OF(anP->name)-1, COMMA);
    
    if(!ArGetInt(fiP, &levelnum, YES, NO) || 
	levelnum <= 0 || levelnum > AR_MAX_ANLEVELS) {
	ArTellError(ERR_AN_LEVELS);
	levelnum = AR_MAX_ANLEVELS;
    }
    anP->level = levelnum;
    
    while((c = ArGetchNW(fiP)) != AR_CMD_TERM && c != EOF){
	ArUnGetch(c, fiP);
	switch(ArGetTokenEq(fiP, al_anumtok_tokens, NUMATOKTS)){
	case NO_EQUALS:
	    ArFprintf(stderr,"ArBuildAutonum: Syntax error. Please check it\n");
	    ArSkipPastTerm(fiP);
	    free(anP);
	    return(NULL);
	    break;
	case AN_RESTART:
	    ArGetYesNoShort(fiP, &anP->restart);
	    break;
	case AN_FIRST:
	    ArGetYesNoShort(fiP, &anP->first);
	    break;
	case AN_TAG:
	    ArGetWordShrC(fiP, NOCHAR, anP->tag, NUM_OF(anP->tag), COMMA);
	    break;
	case AN_VALUE:
	    ArGetWordShrC(fiP, NOCHAR, anP->value, NUM_OF(anP->value),COMMA);
	    break;
	default:
	    ArTellError2(ERR_NO_SUCH_CVAL, al_token_shr, 0);
	    ArSkipPastComma(fiP);
	    break;
	}
    }
    return( anP );
}
/**--------------------------------------------------------------------------**/
/**	this function should always use %Q, not %#Q, because it gets called  **/
/**	from ArFrameWrite(), which surrounds it with single quotes.	     **/
void
ArAutonumWrite(anP, ofile)
  Autonum_t *anP;
  FILE *ofile;
{
    if (anP != NULL && ofile != NULL) {
	ArFprintf(ofile, CMD_START_LINE_FMT, "Autonum");
	ArFprintf(ofile, OBJ_NAME_FMT, anP->name);
	ArFprintf(ofile, AN_LEVELS_FMT, anP->level);
	if (anP->first != NO) {
	    ArFprintf(ofile, PROP_YESNO_FMT, "First", "Yes");
	}
	if (anP->restart != NO) {
	    ArFprintf(ofile, PROP_YESNO_FMT, "Restart", "Yes");
	}
	if (anP->tag[0] != '\0') {
	    ArFprintf(ofile, PROP_NAME_FMT, "Tag", anP->tag);
	}
	if (anP->value[0] != '\0') {
	    ArFprintf(ofile, PROP_NAME_FMT, "Value", anP->value);
	}
	ArFprintf(ofile, CMD_END_FMT);
    }
}
/**--------------------------------------------------------------------------**/
void
ArAutonumFree(anP)
  Autonum_t *anP;
{
    /** there's nothing to this.  maybe take it out, replace w/ a macro. **/
    FREE((char *)anP);
}
/**--------------------------------------------------------------------------**/
