
/*

$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/fmt1.c,v 1.2 89/10/26 14:09:19 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/fmt1.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	fmt1.c,v $
 * Revision 1.2  89/10/26  14:09:19  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.1  89/09/21  06:30:00  mss
 * Initial revision
 * 
 * Revision 1.5  89/08/29  15:54:45  annm
 * added RCSID
 * added RCSID
 * 
 * Revision 1.4  89/01/24  12:16:01  mss
 * Made forward decls consistent with actual declations (w.r.t. static)
 * 
 * Revision 1.3  88/09/06  13:11:53  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.2  88/08/31  20:09:57  jr
 * Handle - numbers in FMT_FormatInt
 * 
 * Revision 1.1  88/07/05  16:57:32  jr
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/fmt1.c,v $";
#endif


#define TK_IMPL_FMT
#define TK_GVARS_FMT1

#include <errors.h>
#include <tkerr.h>
#include <fmt.h>

/* Forward declarations */
static INT_type FormatInt();
static INT_type RecursiveFormatInt();

INT_type FMT_FormatInt(where, len, format, value)
    IN OUT CHAR_type	*where;
    IN	   INT_type	len;
    IN	   CHAR_type	*format;
    IN	   INT_type	value;
{
	     INT_type	rval;	    /* The returned value */
    REGISTER CHAR_type	*next;	    /* Where to put next character */
    REGISTER CHAR_type	*c;	    /* Next format char */
    REGISTER INT_type	curlen;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = 0;
    if (len <= 0) {
	goto LEAVE;
    }
    next = where;
    curlen = len;

    for (c=format; rval!=ERROR_INT && *c!='\0' && curlen>0; c++) {
	if (*c != '%') {
	    *next++ = *c;
	    curlen--;
	    continue;
	}
	switch (*++c) {	/* ++ skips '%' */

	    case '%':	/* Escape for '%' */
		*next++ = '%';
		curlen--;
		break;

	    case 'd':
	    case 'D':	/* Decimal representation */
		rval = FormatInt(next, curlen, value, (INT_type) 10);
		break;

	    case 'x':
	    case 'X':	/* Hex representation */
		rval = FormatInt(next, curlen, value, (INT_type) 16);
		break;

	    default:	/* Oops... */
		rval = ERROR_INT;
		TKError = BADPARM_err;
		goto LEAVE;
	}

	/* Here if the format succeeded, then rval is the # of chars used */
	if (rval != ERROR_INT) {
	    next += rval;
	    curlen -= rval;
	}
    }

LEAVE:

    if (rval != ERROR_INT) {
	rval = next - where;	/* How many chars were written */
    }

#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

INT_type FMT_Init()
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

INT_type FMT_Final()
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

/********************\
* 		     *
* Internal routines  *
* 		     *
\********************/

static INT_type FormatInt(where, len, value, base)
    IN OUT CHAR_type	*where;
    IN	   INT_type	len;
    IN	   INT_type	value;
    IN	   INT_type	base;
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    /* Only up to hex */
    if (base > 16) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	goto LEAVE;
    }

    /* See if negative */
    if (value < 0) {
	*where = '-';
	/*
	   Should have check for 1's complement here in case
	   negating fails.
	*/
	rval = RecursiveFormatInt( (CHAR_type *) (where+1), (INT_type) (len-1),
				  (INT_type) (-value), base);
	if (rval != ERROR_INT) {
	    rval++;	/* Count '-' */
	}
    } else {
	rval = RecursiveFormatInt(where, len, value, base);
    }


LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static INT_type RecursiveFormatInt(where, len, value, base)
    IN OUT CHAR_type	*where;
    IN	   INT_type	len;
    IN	   INT_type	value;
    IN	   INT_type	base;
{
	   INT_type	rval;	    /* The returned value */
    static CHAR_type	digits[] = "0123456789ABCDEF";
#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    if (len <= 0) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	goto LEAVE;
    }

    /* Terminating case: 1 digit */
    if (value < base) {
	*where = digits[value];
	rval = 1;
	goto LEAVE;
    }

    rval = RecursiveFormatInt(where, len, value/base, base);
    if (rval != ERROR_INT) {
	if (len-rval <= 0) {
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    goto LEAVE;
	}
	*(where+rval) = digits[value%base];
	rval++;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}
