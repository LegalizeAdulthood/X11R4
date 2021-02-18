
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/tkerr1.c,v 1.2 89/09/29 11:41:44 mss Exp Locker: jr $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/tkerr1.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	tkerr1.c,v $
 * Revision 1.2  89/09/29  11:41:44  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:41:06  mss
 * Initial revision
 * 
 * Revision 1.20  89/08/29  13:12:42  annm
 * added cast to a constant in tkerr init
 * 
 * Revision 1.19  89/07/19  22:31:46  mss
 * Added new warning facility
 * 
 * Revision 1.18  89/01/24  12:18:17  mss
 * Made forward decls consistent with actual declations (w.r.t. static)
 * 
 * Revision 1.17  88/12/15  12:05:39  mss
 * Get parity right on DONT_FORMAT_ERRORS
 * 
 * Revision 1.16  88/12/15  11:42:56  mss
 * Made compilation of formatting procedures
 * dependent on DONT_FORMAT_ERRORS switch
 * 
 * Revision 1.15  88/09/16  13:59:59  jr
 * Empty routine for
 * TKERR_FormatStringType
 * 
 * Revision 1.14  88/09/06  13:14:29  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.13  88/09/06  12:03:07  annm
 * no change
 * 
 * Revision 1.12  88/09/02  11:42:47  jr
 * Some cleanup
 * 
 * Revision 1.11  88/09/02  09:12:23  jr
 * Oops...missing ()
 * 
 * Revision 1.10  88/09/02  09:10:45  jr
 * Forgot to declare routine.
 * 
 * Revision 1.9  88/08/31  20:09:29  jr
 * Imple TKERR_FormatInt
 * 
 * Revision 1.8  88/08/31  14:42:58  jr
 * Include io module.
 * 
 * Revision 1.7  88/08/31  13:57:04  jr
 * Fix up #define/#include ordering.
 * 
 * Revision 1.6  88/08/31  12:27:30  jr
 * 1st routines for internal error
 * handling
 * 
 * Revision 1.5  88/06/15  17:25:06  jr
 * Make calls to TKERR_Init
 * 
 * Revision 1.4  88/06/08  16:58:47  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.3  88/06/08  14:37:54  annm
 * added Copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/tkerr1.c,v $";
#endif

#define TK_IMPL_INIT
#define	TK_IMPL_TKERR
#define TK_TKI_IO
#define TK_GVARS_TKERR1
#define TK_TKI_FMT

#include <lcsite.h>
#include <init.h>
#include <tkerr.h>
#include <io.h>
#include <fmt.h>

#undef TK_GVARS_TKERR1
#undef TK_TKI_IO
#undef TK_IMPL_TKERR
#undef TK_IMPL_INIT
#undef TK_TKI_FMT

/* Index into error message buffer (TKErrorMesg) */
static INT_type	ErrorIndex;

/* Marker indicating buffer overrun */
static CHAR_type TruncMarker[] = { '.', '.', '.' };

/* Forward declarations */
static INT_type	AddChars();
static BOOL_type	FormattingErrors();

INT_type TKERR_Final()
{
    INT_type	rval;	    /* the returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type TKERR_Init()
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    ErrorIndex = (INT_type) 0;
    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}
/* The following routines are needed only if error
   messages are to be formatted */

#ifndef DONT_FORMAT_ERRORS


static INT_type Truncate()
{
	     INT_type	rval;	    /* The returned value */
    REGISTER INT_type	i;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = 0;
    if (ErrorIndex < ERROR_MESG_SIZE) {
	ErrorIndex = ERROR_MESG_SIZE - 3;
	for (i=0; i<sizeof(TruncMarker); i++) {
	    TKErrorMesg[ErrorIndex++] = TruncMarker[i];
	}
	TKErrorMesg[ErrorIndex] = '\0';
    }

    ErrorIndex = ERROR_MESG_SIZE;   /* Always indicate it's full */

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static BOOL_type FormattingErrors()
{
    BOOL_type	rval;	    /* The returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = InitParms->format_errors ? BOOL_true : BOOL_false;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type TKERR_StartErrMsg()
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    ErrorIndex = 0;
    rval = 0;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}


INT_type TKERR_StartWarnMsg()
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = TKERR_StartErrMsg();

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}


INT_type TKERR_EndErrMsg()
{
    INT_type	rval;	    /* The returned value */
    INT_type	save;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* See if formatting turned on and printing requested */
    if (InitParms->error_od == -1) {
	goto LEAVE;
    }

    save = TKError;

    /* Output message to descriptor */
    rval = IO_Print(InitParms->error_od, ">>> Tool Kit error <<<\n");
    rval = IO_WriteChars(InitParms->error_od, TKErrorMesg, ErrorIndex);
    rval = IO_WriteChar(InitParms->error_od, '\n');

    TKError = save;

LEAVE:

    rval = 0;

#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type TKERR_EndWarnMsg()
{
    INT_type	rval;	    /* The returned value */
    INT_type	save;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* See if formatting turned on and printing requested */
    if ((InitParms->error_od == -1) || 
        (InitParms->print_warnings != BOOL_true) ) {
	goto LEAVE;
    }

    save = TKError;

    /* Output message to descriptor */
    rval = IO_Print(InitParms->error_od, ">>> Tool Kit warning <<<\n");
    rval = IO_WriteChars(InitParms->error_od, TKErrorMesg, ErrorIndex);
    rval = IO_WriteChar(InitParms->error_od, '\n');

    TKError = save;

LEAVE:

    rval = 0;

#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type TKERR_Format(mesg)
    IN CHAR_type    *mesg;
{
    INT_type	rval;	    /* The returned value */
    INT_type	save;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    save = TKError;

    if (FormattingErrors()) {
	rval = AddChars(mesg);
	rval = 0;
    }

    TKError = save;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type TKERR_FormatInt(format, value)
    IN CHAR_type    *format;
    IN INT_type	    value;
{
    INT_type	rval;	    /* The returned value */
    INT_type	save;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    if (!FormattingErrors()) {
	goto LEAVE;
    }

    save = TKError;
    rval = FMT_FormatInt(TKErrorMesg+ErrorIndex,
			 ERROR_MESG_SIZE-ErrorIndex,
			 format, value);
    if (rval == ERROR_INT) {
	if (TKError == BADPARM_err) {
	    /* Must have run out of room */
	    rval = Truncate();
	} else {
	    /* Should do something here */
	}
	TKError = save;
	goto LEAVE;
    }
    ErrorIndex += rval;
    TKError = save;

LEAVE:

    rval = 0;	/* No error returns */

#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type TKERR_FormatStringType(format, string)
    IN CHAR_type    *format;
    IN STRING_type  string;
{
    INT_type	rval;	    /* The returned value */
    INT_type	save;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    if (!FormattingErrors()) {
	goto LEAVE;
    }

    save = TKError;
    TKError = save;

LEAVE:

    rval = 0;	/* No error returns */

#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type AddChars(chars)
    IN CHAR_type    *chars;
{
	     INT_type	rval;	    /* The returned value */
    REGISTER INT_type	i;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* See if buffer already overrun */
    if (ErrorIndex >= ERROR_MESG_SIZE-sizeof(TruncMarker)) {
	goto LEAVE;
    }

    for (i=0; chars[i]!='\0'; i++) {
	if (ErrorIndex == ERROR_MESG_SIZE-sizeof(TruncMarker)) {
	    rval = Truncate();
	    goto LEAVE;
	}

	/* Copy each character */
	TKErrorMesg[ErrorIndex++] = chars[i];
    }

    TKErrorMesg[ErrorIndex] = '\0';

LEAVE:

    rval = 0;

#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

/* end of ifdef for DONT_FORMAT_ERRORS */
#endif


