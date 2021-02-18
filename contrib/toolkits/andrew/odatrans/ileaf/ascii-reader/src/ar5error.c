
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
/*	ar5error.c                               */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  07/88    BT	Original revision from alError.c
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

extern char hex_digits[];

	/*  ERROR HANDLING ROUTINES */
/*
 *  Do not use these routines except for error recovery, since they may
 *  eventually report what they skip
 */

ArTellError(errnum)
int errnum;
{
    /** AlTellError2(errnum, 0, 0); BT **/
    ArFprintf(stderr, "ArTellError(): errnum = %#x\n", errnum);
}

ArTellError2(errnum, arg1, arg2)
int errnum;
long arg1;
long arg2;
{
#if 1	/** BT **/
    ArFprintf(stderr, "ArTellError2(): errnum = %#x, arg1 = %#x, arg2 = %#x\n",
		errnum, arg1, arg2);
#else
    extern int no_display;

    AlReportError(errnum, arg1, arg2);

    if(no_display) return;
    
    if(++al_err_count < 10) delay(25);  /* dont keep waiting if many errors */
    else delay(2);

    if(KbdCancel()) al_cancel = TRUE;
    
    if((al_err_count % 100) == 25) {
	if(MsgChoice(Menmid, A_LD_CONF_ABORT_LOAD, 2, 0) != 0)
	    al_cancel = TRUE;
    }
#endif	/** BT **/
}


ArReportError(errnum, arg1, arg2)
int errnum;
long arg1;
long arg2;
{
#if 1	/** BT **/
    ArFprintf(stderr, "ArReportError(): errnum = %#x, arg1 = %#x, arg2 = %#x\n",
		errnum, arg1, arg2);
#else
    register short* frmptr;
    short mbuf[256];
        
    if(errnum == ERR_NO_CMPN && !al_markup) return;
    frmptr = ALoad_Errors[errnum];
    shrprintf(mbuf, frmptr, arg1, arg2);

    if(al_warn_line_no) {
	if(al_incl_cnt > 0)
	    Mn1Print(Menmid, alErrorMsgLoaderWarnLineFile, al_line_number,
		      al_filename[al_incl_cnt - 1], mbuf);
	else
	    Mn1Print(Menmid, alErrorMsgLoaderWarnLine, al_line_number,
		      mbuf);
    } else {
	Mn1Print(Menmid, alErrorMsgLoaderWarn, mbuf);
    }
#endif	/** BT **/
}


void 
ArSkipPastComma(fiP)		/* this will not go past terminator (>) */
  register File_info_t *fiP;
{
    register int c;
    while((c = ArGetchNW(fiP)) != COMMA)
        if( c == al_cmd_term || c == EOF) {
	    ArUnGetch(c, fiP);
	    return;
	}
}

void 
ArSkipPastTerm(fiP)		/* skip past > */
  register File_info_t *fiP;
{
    register int c;
    while((c = ArGetchNW(fiP)) != al_cmd_term && c != EOF) {
	if(c == '"' || c == '\'') {
	    short temp[AR_MAX_QSTR];
	    /* skip stuff like "...>..." */
	    ArGetWordShrC(fiP, c, temp, AR_MAX_QSTR-1, 0);
	}
    }
}

/*  gobble just the >, which should be next */

ArGobbleTerm(fiP)
  register File_info_t *fiP;
{
    if(ArGetchNW(fiP) != al_cmd_term) {
	/** AlTellError(ERR_EXPECTED_TERM); BT **/
	ArSkipPastTerm(fiP);
    }
}

/*  gobble just the comma, which should be next */

ArGobbleComma(fiP)
  register File_info_t *fiP;
{
    if(ArGetchNW(fiP) != COMMA) {
	/** AlTellError(ERR_EXPECTED_COMMA); BT **/
	ArSkipPastComma(fiP);
    }
}

/*  gobble everything up to and including > */
ArGobblePastTerm(fiP)
  register File_info_t *fiP;
{
    ArSkipPastTerm(fiP);
}


ArError(msg)
short* msg;
{
    /** Mn1Print( Menmid, alErrorMsgInternAsciiError, msg); BT **/
    /** delay(30); BT **/
    ArFprintf(stderr, "ArError(): msg = %Q\n", msg);
}
