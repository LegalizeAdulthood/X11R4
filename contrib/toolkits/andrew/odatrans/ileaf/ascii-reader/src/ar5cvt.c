
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
/*	ar5cvt.c                                */
/*	Copyright 1988, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  07/88    BT	Original AR version, derived from alCvt.c
 *  02/16/89 ram make ArGetFloat() more versatile by
 		making it ArGetDouble(), called by ArGetFloat()
		for compatibility. Also, ArGetDouble() can return
		the number of digits read.
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
bool
ArGetPRsu(fiP, num)  /* get Rsu value > 0 */
File_info_t *fiP;
Rsu* num;
{
    Rsu temp;

    if(ArGetRsu(fiP, &temp)) {
	if(temp <= 0) {
	    ArTellError(ERR_RSU_GT0);
	    return( NO );
	} else {
	    *num = temp;
	    return( YES );
	}
    }
    return( NO );
}
/**--------------------------------------------------------------------------**/
bool
ArGetPZRsu(fiP, num)  /* get Rsu value >= 0 */
File_info_t *fiP;
Rsu* num;
{
    Rsu temp;

    if(ArGetRsu(fiP, &temp)) {
	if(temp < 0) {
	    ArTellError(ERR_RSU_GE0);
	    return( NO );
	} else {
	    *num = temp;
	    return( YES );
	}
    }
    return( NO );
}
/**--------------------------------------------------------------------------**/
/*  When we get lines (for line leading only) we save the raw
 *  "floating" value and convert it to Rsu's only when writing
 *  the component header.  This is in case the font changes etc.
 *  The way we save is to scale it up by 1000 and cram it into an Rsu
 *  (long).
 */

bool
ArGetLines(fiP, num)
File_info_t *fiP;
Rsu* num;
{
    float number;
    
    if(!ArGetFloat(fiP, &number)) {
	ArTellError(ERR_UNITS_NUM);
	ArSkipPastComma(fiP);
	return FALSE;
    }
    if(ArGetUnitMeas(fiP) != LINES) {
	ArTellError(ERR_LINES_UNITS);
    }
    if(number < AR_MIN_LNSPC) {
	ArTellError(ERR_LINES_RANGE);
	number = AR_MIN_LNSPC;
    }
    if(number > AR_MAX_LNSPC) {
	ArTellError(ERR_LINES_RANGE);
	number = AR_MAX_LNSPC;
    }
    *num = (int)((number * 1000.) + 0.5);
    return TRUE;
}
/**--------------------------------------------------------------------------**/
static int
ftoapwr(n, e)
register int n, e;
{
    register int retval = 1;
    while(e-- > 0) retval *= n;  /* note lmul here */
    return(retval);
}

/* Get floating # from input. Return FALSE if nothing. */
/* Ignores white space.  See Atof in utils.c */

	/* since arithmetic and args done as double, probably
	   unnecessary to make seperate function for floats and
	   doubles, but this is conservative.... ram 2/6/89
	 */
bool
ArGetFloat(fiP, number)
File_info_t *fiP;
float* number;
{

    
    double d; short dummy; /* ArGetDouble has side effects on d and dummy */
    bool ArGetDouble();

    if ( ( ArGetDouble(fiP, &d, &dummy) == FALSE)) return (FALSE);
    *number = (float) d;
    return(TRUE);
}
    
    
bool
ArGetDouble(fiP, number, digits)  
File_info_t *fiP;
double* number;
short *digits;			/* after decmal point */
{
    register int   ch;
    register long  n=0;
    register int   fract = 0;
    register long  left = 0;
    register int   digit_seen = FALSE;
    int            sign = FALSE;

    if ((ch = ArGetchNW(fiP)) == '-') sign = TRUE;
    else { if (ch != '+') ArUnGetch(ch, fiP);
    }
    while ((ch = ArGetchNW(fiP)) != EOF) {
	if(Isdigit(ch)) {
	    if(fract < 10) {
		n *= 10;
		n += ch - '0';
		if(fract) fract++;
	    }
            digit_seen = TRUE;
	    continue;
	}
	if (fract == 0 && (ch == '.')) {
	    fract++;
	    left = n;
	    n = 0;
	    continue;
	}
	ArUnGetch(ch, fiP);
	break;
    }
    if(!digit_seen) return(FALSE);

    if(fract==0) {left = n; n = 0; } 
    *number = (double) left + ((double) n / (double) ftoapwr(10, --fract));
    *digits = fract < 0 ? 0 : fract;
    if (sign) *number = -*number;
     /** fprintf(stderr, "ArGetFloat() ==> %.3f\n", *number); BT **/
    return(TRUE);
}

/**--------------------------------------------------------------------------**/
/* Get integer from input. Return FALSE if nothing. */
/* Eats prior white space.  May gobble trailing comma, if any */
/* May gobble trailing white space, if any */
/* Will take integers in decimal, 0... octal, or 0x...  hex values.        */

bool
ArGetInt(fiP, number, gobble_comma, gobble_ws)
File_info_t *fiP;
int* number;    
bool gobble_comma;
bool gobble_ws;
{
    register int   ch;
    register long  n=0;
    register int   radix = 10;
    register int   digit_seen = FALSE;
    int            sign = FALSE;

    if ((ch = ArGetchNW(fiP)) == '-') sign = TRUE;
    else { if (ch != '+') ArUnGetch(ch, fiP);
    }

    ch = ArGetch(fiP);
    if (ch == '0') {
	/** get octal or hex radix. **/
	digit_seen = TRUE;
	ch = ArGetch(fiP);
	if (ch == 'x' || ch == 'X') {
	    /** radix is hex. **/
	    radix = 16;
	} else {
	    /** radix is octal. **/
	    radix = 8;
	    ArUnGetch(ch, fiP);
	}
    }
    else {
	ArUnGetch(ch, fiP);
    }
    while ((ch = ArGetch(fiP)) != EOF) {
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
	if(gobble_ws && (ch == '\n' || ch == ' ' || ch == '\t'))
	  continue;
	if(gobble_comma && ch == COMMA)
	  break;
	ArUnGetch(ch, fiP);
	break;
    }
    if(!digit_seen) return(FALSE);

    if (sign) *number = -n;
    else *number = n;
    /**	fprintf(stderr, "ArGetInt-->%d, before return\n", n);	**/
    return(TRUE);
}
/**--------------------------------------------------------------------------**/
bool
ArGetHex(fiP, num, nibbles)
File_info_t *fiP;
long* num;
int nibbles;
{
    register unsigned long n = 0;
    register int i;
    register int c;
    int c2;
    bool digit_seen = NO;
    
    for(i = 0; i < nibbles; i++) {
	c = ArGetch(fiP);
	if(c == EOF) {
	    break;
	}
	if(ald_chtype[c] == DIG){ c = c - '0'; digit_seen = YES; }
	else {
	    c2 = c;
	    c &= ~CHARCASEBIT;
	    if(c >= 'A' && c <= 'F') { 
		c = c - 'A' + 10; 
		digit_seen = YES; 
	    }
	    else {
		ArUnGetch(c2, fiP);
		break;
	    }
	}
	n = (n<<4) + c;
    }
    if (digit_seen) { *num = n; }
    /** fprintf(stderr, "ArGetHex() ==> %04x\n", *num); BT **/
    return( digit_seen );
}
/**--------------------------------------------------------------------------**/
/*  return -1 if error, otherwise hex byte value */
int ArDoHex(fiP)
  File_info_t *fiP;
{
    long n;
    int c;
    
    if(!ArGetHex(fiP, &n, 4)) {
	return(-1);
    }
    if((c = ArGetchNW(fiP)) != AR_CMD_TERM) {
	ArUnGetch(c, fiP);
	return(-1);
    }
    /** ArFprintf(stderr, "ArDoHex ==> %#x\n", n); BT **/
    return(n);
}

/**--------------------------------------------------------------------------**/
/* convert unsigned ASCII string to integer */
/*  if "term" is true, the number must end with zero byte */

ArCvtIntShr(shr, value, term)
int term;
register short* shr;
short* value;
{
    register int n = 0;
    register int c;

    while((c = *shr++) != 0) {
	if(c > ALD_CHTYP_MAX)
	    goto check_term;
	switch((int)ald_chtype[c]) {
	case DIG : n = n*10 + c - '0'; break;
	case WHT : break;
	default  : 
	  check_term:
	    if(term)
		return(FALSE);
	    else
		break;
	}
    }
    *value = n;
    return (TRUE);
}
/**--------------------------------------------------------------------------**/
