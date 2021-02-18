
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
/*	ar5buf.c                               */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  08/88    BT	Original version
 */

/**--------------------------------------------------------------------------**/
/**	These routines build tokens to put in a Cmpn_t buffer.		     **/
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
bool
arBufGetComment(fiP, ssP)
  File_info_t *fiP;
  Ssave_t *ssP;
{
  shar c;
  /* Token_t *comP;		M. Saunders 2/9/89 here and below. */
  Token_t comP;
  register Ssave_t *my_ssP;
  long l;
  static char *err_msg = 
 "arBufGetComment(): unexpected markup in the document.  Please check it..\n";

    /* comP        = NEW(Token_t); */
    comP.token = TOKEN_SET(tok_Comment);
    comP.u.textP = my_ssP = SsNew();
    /** get the text of the comment into its own storage. **/
    ArSkipPastComma(fiP);	/** should be right on it. **/
    while ((c = ArGetch(fiP)) != EOF && c != AR_CMD_TERM) {
	/** have to check for '<<' and '<#xxxx>'  **/
	if (c == AR_CMD_CHAR) {
	    if ((c = ArGetch(fiP)) == AR_HEX_CHAR) {
		if (ArGetHex(fiP, &l, 4) == YES) {
		    c = l;
		}
		ArSkipPastTerm(fiP);
	    }
	    else if (c == AR_CMD_CHAR) {
		/** it's cool. **/
	    }
	    else {
		/** bad structure in the document. **/
		ArFprintf(stderr, err_msg);
		ArSkipPastTerm(fiP);
	    }
	}
	SsSaveS(c, my_ssP);
    }
    BufEnter(&comP, ssP);
    return( YES );
}
/**--------------------------------------------------------------------------**/
bool
arBufGetPageBreak(fiP, ssP)
  File_info_t *fiP;
  Ssave_t *ssP;
{
  shar c;
  Token_t pgP;
  register Ssave_t *my_ssP;
  long l;
  static char *err_msg = 
 "arBufGetPageBreak(): unexpected markup in the document.  Please check it..\n";

    /* pgP        = NEW(Token_t); */
    pgP.token = TOKEN_SET(tok_PageBreak);
    pgP.u.textP = my_ssP = SsNew();
    /** get the text of the page entry into its own storage. **/
    ArSkipPastComma(fiP);	/** should be right on it. **/
    while ((c = ArGetch(fiP)) != EOF && c != AR_CMD_TERM) {
	if (c == AR_CMD_CHAR) {
	    if ((c = ArGetch(fiP)) == AR_HEX_CHAR) {
		if (ArGetHex(fiP, &l, 4) == YES) {
		    c = l;
		}
		ArSkipPastTerm(fiP);
	    }
	    else if (c == AR_CMD_CHAR) {
		/** it's cool. **/
	    }
	    else {
		/** bad structure in the document. **/
		ArFprintf(stderr, err_msg);
		ArSkipPastTerm(fiP);
	    }
	}
	SsSaveS(c, my_ssP);
    }
    BufEnter(&pgP, ssP);
    return( YES );
}
/**--------------------------------------------------------------------------**/
bool
arBufGetHR(fiP, ssP)
  File_info_t *fiP;
  Ssave_t *ssP;
{
  Token_t      hrP;

    /* hrP               = NEW(Token_t); */
    hrP.token        = TOKEN_SET(tok_HR);
    hrP.u.token_data = 0;

    BufEnter(&hrP, ssP);
    ArSkipPastTerm(fiP);	/** should be right there. **/
    return( YES );
}
/**--------------------------------------------------------------------------**/
bool
arBufGetSR(fiP, ssP)
  File_info_t *fiP;
  Ssave_t *ssP;
{
  Token_t      srP;

    /* srP               = NEW(Token_t); */
    srP.token        = TOKEN_SET(tok_SR);
    srP.u.token_data = 0;

    BufEnter(&srP, ssP);
    ArSkipPastTerm(fiP);	/** should be right there. **/
    return( YES );
}
/**--------------------------------------------------------------------------**/
bool
arBufGetSP(fiP, ssP)
  File_info_t *fiP;
  Ssave_t *ssP;
{
  Token_t      spP;

    /* spP               = NEW(Token_t); */
    spP.token        = TOKEN_SET(tok_SP);
    spP.u.token_data = 0;

    BufEnter(&spP, ssP);
    ArSkipPastTerm(fiP);	/** should be right there. **/
    return( YES );
}
/**--------------------------------------------------------------------------**/
bool
arBufGetNoOp(fiP, ssP)
  File_info_t *fiP;
  Ssave_t *ssP;
{
  Token_t      NoOpP;

    /* NoOpP               = NEW(Token_t); */
    NoOpP.token        = TOKEN_SET(tok_NoOp);
    NoOpP.u.token_data = 0;

    BufEnter(&NoOpP, ssP);
    ArSkipPastTerm(fiP);	/** should be right there. **/
    return( YES );
}
/**--------------------------------------------------------------------------**/
bool
arBufGetFJ(fiP, ssP)
  File_info_t *fiP;
  Ssave_t *ssP;
{
  Token_t      fjP;

    /* fjP               = NEW(Token_t); */
    fjP.token        = TOKEN_SET(tok_FJ);
    fjP.u.token_data = 0;

    BufEnter(&fjP, ssP);
    ArSkipPastTerm(fiP);	/** should be right there. **/
    return( YES );
}
/**--------------------------------------------------------------------------**/
bool
arBufGetFontCmd(fiP, ssP)
  File_info_t *fiP;
  Ssave_t *ssP;
{
  Token_t      fnP;
  Font_value_t *fvP;

    /* fnP             = NEW(Token_t); */
    fnP.token      = TOKEN_SET(tok_Font);
    fvP             = NEW(Font_value_t);
    fnP.u.fontvalP = fvP;

    fvP->font_token   = al_font_number;
    fvP->font_inherit = al_font_inherit;
    fvP->font_attr    = al_attrs;
    BufEnter(&fnP, ssP);
    ArSkipPastTerm(fiP);	/** should be right there. **/
    return( YES );
}
/**--------------------------------------------------------------------------**/
bool
arBufGetFontValue(fiP, ssP)
  File_info_t *fiP;
  Ssave_t *ssP;
{
  Token_t      fnP;
  Font_value_t *fvP;

    /* fnP             = NEW(Token_t); */
    fnP.token      = TOKEN_SET(tok_Font);
    fvP             = NEW(Font_value_t);
    fnP.u.fontvalP = fvP;

    if (ArFontGetValue(fiP, fvP) == NO) {
	ArFprintf(stderr, "arBufGetFontValue: font error, using F0.\n");
	*fvP = ArFontValueInit();
    }
    BufEnter(&fnP, ssP);
    ArSkipPastTerm(fiP);	/** should be right there. **/
    return( YES );
}
/**--------------------------------------------------------------------------**/
/** note that this is a self-contained token, and does not point to any	     **/
/** subsidiary data.							     **/
bool
arBufGetTab(fiP, tempint, ssP)
  File_info_t *fiP;
  int tempint;
  Ssave_t *ssP;
{
  Token_t my_token;

    switch(tempint) {
    default:
    case TAB:
	my_token.token = TOKEN_SET(tok_Tab);
	my_token.u.token_data = 0;
	break;
    case TAB_DOT:
	my_token.token = TOKEN_SET(tok_TabDot);
	my_token.u.token_data = '.';
	break;
    case TAB_DASH:
	my_token.token = TOKEN_SET(tok_TabDash);
	my_token.u.token_data = '-';
	break;
    case TAB_HYPH:
	my_token.token = TOKEN_SET(tok_TabUnder); /** TAB_HYPH is a misnomer **/
	my_token.u.token_data = '_';
	break;
    }
    BufEnter(&my_token, ssP);
    ArSkipPastTerm(fiP);	/** should be right there. **/
    return( YES );
}
/**--------------------------------------------------------------------------**/
bool
arBufGetAutonum(fiP, ssP)
  File_info_t *fiP;
  Ssave_t *ssP;
{
  Token_t tkP;

    /* tkP          = NEW(Token_t); */
    tkP.token   = TOKEN_SET(tok_Autonum);
    tkP.u.anumP = ArBuildAutonum(fiP);
    BufEnter(&tkP, ssP);
    /** ArSkipPastTerm(fiP);	 BT **/
    return( YES );
}
/**--------------------------------------------------------------------------**/
bool
arBufGetFrame(fiP, ssP)
  File_info_t *fiP;
  Ssave_t *ssP;
{
  Token_t tkP;

    /* tkP          = NEW(Token_t); */
    tkP.token   = TOKEN_SET(tok_Frame);
    tkP.u.frameP = ArBuildFrame(fiP, NO, NO); /** not master or cell **/
    BufEnter(&tkP, ssP);
    /** ArSkipPastTerm(fiP);	 BT **/
    return( YES );
}
/**--------------------------------------------------------------------------**/
