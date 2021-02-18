
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
/*	ar5io.c                                  */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  07/88    BT	Original revision from AR 3.5 io.c
 */

#ifdef INTERLEAF_INCLUDE_FILES
#include "cdefs.h"
#include "sysstdio.h"
#include "sysctype.h"
#else
#ifndef AR_DEFINES
#define AR_DEFINES
typedef short bool;
typedef short rsu;
typedef long Rsu;
typedef short shar;
typedef unsigned short ushar;
#endif
#include <stdio.h>
#include <ctype.h>
#endif
#include "ar5defs.h"
#include "ar5private.h"

/**--------------------------------------------------------------------------**/
/** used in debugging. **/
void    show_inqueue();
#define SHOW(str)

/**--------------------------------------------------------------------------**/
void ArIoReset( fiP )
  register File_info_t *fiP;
{
    SsFree(&fiP->inqueue);
    fiP->saved_char = 0;
}

/**--------------------------------------------------------------------------**/
shar
ArGetch(fiP)
  register File_info_t *fiP;
{
  register shar retval;

    SHOW("Before ArGetch()");
    /** if there are put-back characters ... **/
    if (SsAmount(&fiP->inqueue) > 0) {
	/** use the next put-back character. **/
	retval = SsGetS(&fiP->inqueue);
    } else {
	/** put-back chars all gone! so reset the buffer, use the file **/
	SsReset(&fiP->inqueue);
	retval = getc(fiP->ifile);	/** note EOF is possible. **/
    }
    SHOW("After  ArGetch()");
    return( retval );
}

/**--------------------------------------------------------------------------**/
shar
ArGetchNW( fiP )			/* return next non-white character */
 register File_info_t *fiP;		/* also skips -- type comments	   */
{
    register shar s;
    
    for ( ;; ){
	s = ArGetch(fiP);
	if(s == 0) continue;
	if(s == AR_CMNT_CHAR){
	    if((s = ArGetch(fiP)) == AR_CMNT_CHAR) { /* saw -- comment */
		while((s = ArGetch(fiP)) != EOF) {  /* gobble until another -- */
		    if(s == AR_CMNT_CHAR && (s = ArGetch(fiP)) == AR_CMNT_CHAR){
			break;
		    }
		    if(s == AR_CMD_TERM) {	       /* ...or > */
			ArUnGetch(s, fiP);
			break;
		    }
		}
		continue;
	    } else {  /* oops, only saw single - */
		ArUnGetch(s, fiP);
		return(AR_CMNT_CHAR);
	    }
	}
        if(s == EOF || ald_chtype[s] != WHT) {
	    return(s);
	}
    }
    /* NOTREACHED */
}

/**--------------------------------------------------------------------------**/
#if 0	/** BT **/
shar ArCmpnGetch()
{
  register shar s, s2, s3;
  bool quoted;
  shar buf[AR_MAXQSTR];
  shar *sP;

    if (saved_char != 0) {
	/** this is the second char in '<<' from the input. **/
	s = saved_char;
	saved_char = 0;
	return( s );
    }
    s = ArGetch();
    if (s == EOF) {
	return( EOF );
    }
    else
    if (s == '\n') {
	/** check for two newlines **/
	if ((s2 = ArGetch()) == '\n') {
	    /** found a new component following this one. **/
	    /** must set g_newline_same_cmpn within the scope of this if stmt. **/
	    /** and in all cases, return EOF				       **/
	    if ((s3 = ArGetch()) == Lmarkup) {
		/** check if it is a cmpn tag, or just in-line markup at the **/
		/** beginning of an identical-propertied following cmpn. **/
		/** look ahead. **/
		quoted = ArGetWord(buf, sizeof(buf), 0);
		if (quoted == 0) {
		    /** failure of some kind. **/
		    g_newline_same_cmpn = NO;	/** ??? is this right? **/
		    ArSkipPastTerm();		/** ??? is this right? **/
		    return( EOF );		/** ??? is this right? **/
		}
		else
		if (quoted == AR_QUOTED || !ArIsMarkupCmd(buf)) {
		    /** it's a component tag. **/
		    /** always quote the push-back cmpn name.  It is easy to do,**/
		    /** doesn't hurt, and makes the next recognition easier. **/
		    ArUnGetch('"');
		    sP = ArQuoteShring(buf, '"');
		    ArUnGetshr(sP);
		    ArUnGetch('"');
		    ArUnGetch( Lmarkup );
		    g_newline_same_cmpn = NO;
		    return( EOF );	/** the end of the current component. **/
		}
		else {
		    /** it's in-line markup command. **/
		    ArUnGetshr(buf);
		    ArUnGetch( Lmarkup );
		    g_newline_same_cmpn = YES;
		    return( EOF );
		}
	    }
	    else
	    if (s3 == EOF) {
		g_newline_same_cmpn = NO;
		return( EOF );
	    }
	    else {
		/** plain-text char implies that this is an identical-prop'ed **/
		/** cmpn to the previous one.  pass that fact back up to      **/
		/** ArDoCommand, and return the end of this compnent to the   **/
		/** client function.  Whew!					**/
		ArUnGetch(s3);
		g_newline_same_cmpn = YES;
		return( EOF );
	    }
	}
	else {
	    if (s2 == EOF) {
		/** paranoia: no OPS-saved document will have a single         **/
		/** newline followed by EOF, but one of my test documents did. **/
		g_newline_same_cmpn = NO;
	    }
	    else {
		ArUnGetch(s2);
	    }
	    return( s );
	}
    }
    else if (s == Lmarkup) {
	if ((s2 = ArGetch()) == Lmarkup) {
	    /** found '<<' **/
	    saved_char = Lmarkup;
	    return( Lmarkup );
	}
	else {
	    ArUnGetch(s2);
	}
	s2 = ArGetchNW();
	ArUnGetch(s2);		/** NOTE it gets put back here. **/
	if (s2 == '#' || s2 == '-' || s2 == '|' || s2 == '!') {
	    return( s );
	}
	else {
	    /** check for in-line markup command, or component tag. **/
	    quoted = ArGetWord(buf, sizeof(buf), 0);
	    if (quoted == 0) {
		/** failure of some kind. **/
		ArSkipPastTerm();
		return( EOF );		/** ??? is this right? **/
	    }
	    else
	    if (quoted == AR_QUOTED || !ArIsMarkupCmd(buf)) {
		/** it's a component tag. **/
		/** always quote the push-back cmpn name.  It is easy to do, **/
		/** doesn't hurt, and makes the next recognition easier. **/
		ArUnGetch('"');
		sP = ArQuoteShring(buf, '"');
		ArUnGetshr(sP);
		ArUnGetch('"');
		ArUnGetch( Lmarkup );
		return( EOF );		/** the end of the current component. **/
	    }
	    else {
		/** it's in-line markup command. **/
		ArUnGetshr(buf);
		return( Lmarkup );
	    }
	}
    }
    else {
	/** it's a plain character. **/
	return( s );
    }
}

/**--------------------------------------------------------------------------**/
shar
ArCmpnGetchNW()	/* return next non-white character */
		/* also skips -- type comments	   */
{
    register shar s;
    
    while(s = ArCmpnGetch()) {
	if(s == 0) continue;
	if(s == AR_CMNT_CHAR){
	    if((s = ArCmpnGetch()) == AR_CMNT_CHAR) { /* saw -- comment */
		while((s = ArCmpnGetch()) != EOF) {/* gobble until another -- */
		    if(s == AR_CMNT_CHAR && (s = ArCmpnGetch()) == AR_CMNT_CHAR){
			break;
		    }
		    if(s == AR_CMD_TERM) {	       /* ...or > */
			ArUnGetch(s);
			break;
		    }
		}
		continue;
	    } else {  /* oops, only saw single - */
		ArUnGetch(s);
		return(AR_CMNT_CHAR);
	    }
	}
        if(s == EOF || ald_chtype[s] != WHT) return(s);
    }
    /* NOTREACHED */
}
#endif	/** BT **/

/**--------------------------------------------------------------------------**/
shar
ArUnGetch(s, fiP)
  register shar s;
  register File_info_t *fiP;
{
    SHOW("Before ArUnGetch()");
    return( SsSaveSFront(s, &fiP->inqueue) == YES? s : EOF ); 
}

/**--------------------------------------------------------------------------**/
shar *
ArUnGetshr(sP, fiP)
  register shar *sP;
  register File_info_t *fiP;
{
  register shar *lastsP;

    if (sP != NULL) {
	lastsP = sP + ArShrlen(sP) - 1;
	while (sP <= lastsP) {
	    ArUnGetch(*lastsP--, fiP);
	}
    }
    return( sP );
}

/**--------------------------------------------------------------------------**/
void
show_inqueue(fiP, cP)
  register File_info_t *fiP;
  register char *cP;
{
  register shar *sP;

    if (SsAmount(&fiP->inqueue) > 0) {
	fprintf(stderr, "%s: show_inqueue:",
	    cP, fiP->inqueue.baseP, fiP->inqueue.outP, fiP->inqueue.inP, fiP->inqueue.limitP);
	fprintf(stderr, "[");
	for (sP = fiP->inqueue.baseP; sP < fiP->inqueue.inP; sP++) {
	    fprintf(stderr, "%c", *sP);
	}
	fprintf(stderr, "]\n");
    }
#if 1
    else {
	fprintf(stderr, 
	    "%s: show_inqueue: baseP==%#x, outP==%#x, inP==%#x, limitP==%#x\n",
	    cP, fiP->inqueue.baseP, fiP->inqueue.outP, fiP->inqueue.inP, fiP->inqueue.limitP);
    }
#endif
}
/**--------------------------------------------------------------------------**/
void ArEmitChar(uc, mapP, ofile)
  register unsigned char  uc;
  register Char_map_t    *mapP;
  register FILE          *ofile;
{
    /**	fprintf(stderr, "emit_shar(%#C, %#x, %#x\n", uc, mapP, ofile);	**/
    if (ofile != NULL) {
	if (ArCharIsMapped(uc, mapP)) {
	    ArFprintf(ofile, "%#S", mapP[uc]);
	}
	else {
	    /** no map entry, just print it. **/
	    if (uc == '<' && AR_init_info.double_markup == YES) { 
		/** double '<<' in output. **/
		ArFprintf(ofile, "<<");
	    }
	    else {
		ArFprintf(ofile, "%#C", uc);
	    }
	}
    }
}
/**--------------------------------------------------------------------------**/
