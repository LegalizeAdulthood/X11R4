
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
/*	ar5token.c                               */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  08/88    BT	Original version
 */

/**--------------------------------------------------------------------------**/
/**	These routines print and free the object associated with a token     **/
/**	from a Cmpn_t buffer.						     **/
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
void 
ArTokenWrite(tkP, ilP, ofile)
  Token_t *tkP;
  Il_state_t *ilP;
  FILE *ofile;
{
    switch(TOKEN_RESET(tkP->token)) {
    case tok_PaddingNOP:
	/** totally ignore it. **/
	break;
    case tok_Autonum:
	ArAutonumWrite(tkP->u.anumP, ofile);
	break;
    case tok_CharSpecial:
	ArFprintf(ofile, CHAR_SPECIAL_FMT, tkP->u.token_data);
	break;
    case tok_Cmpn:
	ArCmpnWrite(tkP->u.subcmpnP, ilP, ofile);
	break;
    case tok_Comment:
	ArCommentWrite(tkP->u.commentP, ofile);
	break;
    case tok_Font:
	ArFprintf(ofile, FONT_START_FMT);
	ArFontValueWrite(tkP->u.fontvalP, (ilP? ilP->fontP : NULL), ofile);
	ArFprintf(ofile, FONT_END_FMT);
	break;
    case tok_FJ:
	ArFprintf(ofile, CMD_PLAIN_FMT, "FJ");
	break;
    case tok_Frame:
	ArFrameWrite(tkP->u.frameP, ofile);
	break;
    case tok_HR:
	ArFprintf(ofile, CMD_PLAIN_FMT, "HR");
	break;
    case tok_Index:
	ArIndexWrite(tkP->u.indexP, ofile);
	break;
    case tok_NoOp:
	ArFprintf(ofile, CMD_PLAIN_FMT, "NOP");
	break;
    case tok_PageBreak:
	SsSaveS('\0', tkP->u.textP);	/** just to be sure **/
	ArFprintf(ofile, PAGE_BREAK_FMT, SsBufStart(tkP->u.textP) );
	break;
    case tok_Ref:
	ArRefWrite(tkP->u.refP, ofile);
	break;
    case tok_SP:
	ArFprintf(ofile, CMD_PLAIN_FMT, "SP", AR_CMD_CHAR, AR_CMD_TERM);
	break;
    case tok_SR:
	ArFprintf(ofile, CMD_PLAIN_FMT, "SR", AR_CMD_CHAR, AR_CMD_TERM);
	break;
    case tok_Tab:
	ArFprintf(ofile, TAB_PLAIN_FMT);
	break;
    case tok_TabDot:
    case tok_TabDash:
    case tok_TabUnder:
	ArFprintf(ofile, TAB_LEADER_FMT, tkP->u.token_data);
	break;
    /**--------------------------**/
    /** this will never happen in the normal case; H/F are attached	     **/
    /** to the Il_state_t, not tokens.					     **/
    case tok_PageHeader:
    case tok_FirstPageHeader:
    case tok_LeftPageHeader:
    case tok_RightPageHeader:
    case tok_PageFooter:
    case tok_FirstPageFooter:
    case tok_LeftPageFooter:
    case tok_RightPageFooter:
	ArHeaderFooterWrite(tkP->u.token_data, ofile);
	break;
    /**--------------------------**/
    /** these are hopeless.      **/
    case tok_Invalid:
    case tok_Cell:
    case tok_Data:
    case tok_EndFrame:
    case tok_EndInline:
    case tok_EndTable:
    case tok_EndText:
    case tok_Language: 
    case tok_Mail: 
    case tok_Master_Row: 
    case tok_Note: 
    case tok_Query: 
    case tok_Rev: 
    case tok_Row: 
    default:
	ArFprintf(ofile, DECL_START_FMT, "Comment");
	ArFprintf(ofile, ", ArTokenWrite: ignoring type %d, data %#x.",
		tkP->token, tkP->u.token_data);
	ArFprintf(ofile, DECL_END_FMT);
	break;
    }
}
/**--------------------------------------------------------------------------**/
/**	Free the secondary data structures associated with a token.          **/
/**	the token itself IS NOT free'd because we assume it is resident	     **/
/**	in a buffer belonging to a Cmpn_t somewhere and was not		     **/
/**	indivdually malloc()-ed.					     **/

bool ArTokenFree(tkP)
  Token_t *tkP;
{
    if (tkP == NULL) {
      return( YES );
    }
    switch(TOKEN_RESET(tkP->token)) {
    case tok_Autonum:
	ArAutonumFree(tkP->u.anumP);
	break;
    case tok_Cmpn:
	return( ArCmpnFree(tkP->u.subcmpnP) );
	break;
    case tok_Comment:
	SsFree(tkP->u.textP);
	break;
    case tok_Font:
	FREE(tkP->u.fontvalP);
	break;
    case tok_Frame:
	return( ArObjectFree(tkP->u.frameP) );
	break;
    case tok_Index:
	ArIndexFree(tkP->u.indexP);
	break;
    case tok_PageBreak:
	SsFree(tkP->u.textP);
	break;
    case tok_Ref:
	ArRefFree(tkP->u.refP);
	break;
    case tok_Unknown:
	ArUnknownFree(tkP->u.unkP);
	break;
    /**------------------------------------------**/
    /**  These have no secondary data structure. **/
    case tok_CharSpecial:
    case tok_FJ:
    case tok_HR:
    case tok_NoOp:
    case tok_SP:
    case tok_SR:
    case tok_Tab:
    case tok_TabDot:
    case tok_TabDash:
    case tok_TabUnder:
	break;
    /**--------------------------**/
    /** this will never happen in the normal case; H/F are attached	     **/
    /** to the Il_state_t, not tokens.					     **/
    case tok_PageHeader:
    case tok_FirstPageHeader:
    case tok_LeftPageHeader:
    case tok_RightPageHeader:
    case tok_PageFooter:
    case tok_FirstPageFooter:
    case tok_LeftPageFooter:
    case tok_RightPageFooter:
	break;
    /**--------------------------**/
    /** these are hopeless.      **/
    case tok_Invalid:
    case tok_PaddingNOP:
    case tok_Cell:
    case tok_Data:
    case tok_EndFrame:
    case tok_EndInline:
    case tok_EndTable:
    case tok_EndText:
    case tok_Language: 
    case tok_Mail: 
    case tok_Master_Row: 
    case tok_Note: 
    case tok_Query: 
    case tok_Rev: 
    case tok_Row: 
    default:
	break;
    }
    return( YES );
}
/**--------------------------------------------------------------------------**/
