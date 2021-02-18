
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/io1.c,v 1.4 89/10/31 14:51:46 annm Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/io1.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	io1.c,v $
 * Revision 1.4  89/10/31  14:51:46  annm
 * fixing lint problmes
 * 
 * Revision 1.3  89/10/26  14:10:29  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.2  89/09/29  11:40:28  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:31:26  mss
 * Initial revision
 * 
 * Revision 1.21  89/08/31  18:21:07  mss
 * Fixed to use new LOCAL interface
 * 
 * Revision 1.20  89/05/07  19:36:12  mss
 * Added required file descriptor parameter to call
 * of IO_WriteChar
 * 
 * Revision 1.19  89/03/24  15:18:41  mss
 * Added error messages to all error conditions
 * 
 * Revision 1.18  89/01/24  12:16:32  mss
 * Made forward decls consistent with actual declations (w.r.t. static)
 * 
 * Revision 1.17  88/10/26  12:05:04  jr
 * IO_PrintInt now works for
 * negative integers.
 * 
 * Revision 1.16  88/10/25  09:17:54  jr
 * Pass -1 through in IO_WriteBytes
 * 
 * Revision 1.15  88/09/13  14:38:06  mz0k
 * Added the ConcatString.
 * 
 * Revision 1.14  88/09/07  14:46:03  mz0k
 * ReadBytes etc.
 * 
 * Revision 1.13  88/09/02  13:58:12  mz0k
 * Changed the IO_ReadByte and IO_ReadChar
 * 
 * 
 * Revision 1.12  88/08/03  13:15:26  jr
 * Fix up forward defn's.
 * 
 * Revision 1.11  88/07/29  16:10:29  mz0k
 * Added the case of that there are less number of bytes in the resource
 * file than required
 * 
 * Revision 1.10  88/07/26  13:27:29  mz0k
 * Added IO_ReadByte(s),  IO_ReadChar(s)
 * 
 * Revision 1.9  88/06/08  16:56:58  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.8  88/06/08  14:36:05  annm
 * added Copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/io1.c,v $";
#endif

/* Put .c template comment here */

#define TK_IMPL_IO
#define TK_GVARS_IO1
#define TK_TKI_TKERR

#include <lcsite.h>
#include <errors.h>
#include <tkerr.h>
#include <local.h>
#include <io.h>

#undef TK_IMPL_IO
#undef TK_GVARS_IO1
#undef TK_TKI_TKERR

/* Forward declarations */
static INT_type	PrintInt();
static INT_type	StrLength();

INT_type IO_WriteBytes(od, buffer, length)
    IN INT_type	    od;
    IN BYTE_type    *buffer;
    IN INT_type	    length;
{
    INT_type	rval;	    /* The returned value */
    INT_type	left;	    /* Bytes remaining to write */
    INT_type	dummy;	    /* For error messages */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Check parameters */
    if (length < -1) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
        dummy = TKERR_Format("[IO_WriteBytes]: length too ");
	dummy = TKERR_FormatInt("small %d", (INT_type) length);
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Special case length==0 */
    if (length==0 || length==-1) {
	rval = LOCAL_WriteBytes(od, buffer, length);
	if (rval == LCL_ERROR_INT) {
	    /* map from local to tk error */
	    rval = ERROR_INT; 
	    TKError = LCLError;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[IO_WriteBytes]: error returned ");
	    dummy = TKERR_FormatInt("from LOCAL_WriteBytes for file %d,",
				    (INT_type) od);
	    dummy = TKERR_FormatInt("length %d",
				    (INT_type) length);
	    dummy = TKERR_EndErrMsg();
	}
	goto LEAVE;
    }

    /* Normal case: length > 0, loop until all bytes written */
    left = length;
    while (left > 0) {
	rval = LOCAL_WriteBytes(od, (BYTE_type *) (buffer+(length-left)), left);
	if (rval == LCL_ERROR_INT) {
	    /* map from local to tk error */
	    rval = ERROR_INT; 
	    TKError = LCLError;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[IO_WriteBytes]: error returned ");
	    dummy = TKERR_FormatInt("from LOCAL_WriteBytes for file %d, ",
				    (INT_type) od);
	    dummy = TKERR_FormatInt("length %d, ",
				    (INT_type) length);
	    dummy = TKERR_FormatInt("left %d",
				    (INT_type) left);
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	if (rval == 0 || rval > left /* This can't happen!! */) {
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[IO_WriteBytes]: error returned ");
	    dummy = TKERR_FormatInt("from LOCAL_WriteBytes, rval %d, ",
				    (INT_type) rval);
	    dummy = TKERR_FormatInt("left %d",
				    (INT_type) left);
	    dummy = TKERR_EndErrMsg();
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	left -= rval;
    }
    rval = length;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type IO_WriteChars(od, buffer, length)
    IN INT_type	    od;
    IN CHAR_type    *buffer;
    IN INT_type	    length;
{
    INT_type	rval;	    /* The returned value */
    INT_type	left;	    /* Chars remaining to write */
    INT_type	dummy;	    /* For error messages */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Check parameters */
    if (length < 0) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[IO_WriteChars]: length too small ");
	dummy = TKERR_FormatInt("%d",	(INT_type) length);
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Special case length==0 */
    if (length == 0) {
	rval = LOCAL_WriteChars(od, buffer, (INT_type) 0);
	if (rval == LCL_ERROR_INT) {
	    /* map from local to tk error */
	    rval = ERROR_INT; 
	    TKError = LCLError;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[IO_WriteChars]: error returned ");
	    dummy = TKERR_FormatInt("from LOCAL_WriteChars for file %d",
				    (INT_type) od);
	    dummy = TKERR_Format(" for length 0");
	    dummy = TKERR_EndErrMsg();
	}
	goto LEAVE;
    }

    /* Normal case: length > 0, loop until all chars written */
    left = length;
    while (left > 0) {
	rval = LOCAL_WriteChars(od, (CHAR_type *) (buffer+(length-left)), left);
	if (rval == LCL_ERROR_INT) {
	    /* map from local to tk error */
	    rval = ERROR_INT; 
	    TKError = LCLError;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[IO_WriteChars]: error returned ");
	    dummy = TKERR_FormatInt("from LOCAL_WriteChars for file %d, ",
				    (INT_type) od);
	    dummy = TKERR_FormatInt("length %d, ", (INT_type) length);
	    dummy = TKERR_FormatInt("left %d", (INT_type) left);
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	if (rval == 0 || rval > left /* This can't happen!! */) {
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[IO_WriteChars]: error returned ");
	    dummy = TKERR_FormatInt("from LOCAL_WriteChars, rval %d, ",
				    (INT_type) rval);
	    dummy = TKERR_FormatInt("left %d",
				    (INT_type) left);
	    dummy = TKERR_EndErrMsg();
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	left -= rval;
    }
    rval = length;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type IO_ReadBytes(od, buffer, length)
    IN INT_type	    od;
    IN BYTE_type    *buffer;
    IN INT_type	    length;
{   /* 'Rval < length' means that there are only less than required number of bytes
      in the resource file and that's the number returned.
     */

    INT_type	rval;	    /* The returned value */
    INT_type	left;	    /* Bytes remaining to read */
    INT_type	dummy;	    /* For error messages */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Check parameters */
    if (length < 0) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[IO_ReadBytes]: length too small ");
	dummy = TKERR_FormatInt("%d",	(INT_type) length);
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Special case length==0 */
    if (length == 0) {
	rval = LOCAL_ReadBytes(od, buffer, (INT_type) 0);
	if (rval == LCL_ERROR_INT) {
	    /* map from local to tk error */
	    rval = ERROR_INT; 
	    TKError = LCLError;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[IO_ReadBytes]: error returned ");
	    dummy = TKERR_FormatInt("from LOCAL_ReadBytes for file %d",
				    (INT_type) od);
	    dummy = TKERR_Format(" for length 0");
	    dummy = TKERR_EndErrMsg();
	}
	goto LEAVE;
    }

    /* Normal case: length > 0, loop until all bytes read */
    left = length;
    while (left > 0) {
	rval = LOCAL_ReadBytes(od, (BYTE_type *) (buffer+(length-left)), left);
	if (rval == LCL_ERROR_INT) {
	    /* map from local to tk error */
	    rval = ERROR_INT; 
	    TKError = LCLError;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[IO_ReadBytes]: error returned ");
	    dummy = TKERR_FormatInt("from LOCAL_ReadBytes for file %d, ",
				    (INT_type) od);
	    dummy = TKERR_FormatInt("length %d, ", (INT_type) length);
	    dummy = TKERR_FormatInt("left %d", (INT_type) left);
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	if ( rval > left /* This can't happen!! */) {
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[IO_ReadBytes]: error returned ");
	    dummy = TKERR_FormatInt("from LOCAL_ReadBytes, rval %d, ",
				    (INT_type) rval);
	    dummy = TKERR_FormatInt("left %d",
				    (INT_type) left);
	    dummy = TKERR_EndErrMsg();
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	if (rval == 0) /* The end of file */ {
	    rval = length - left;
	    goto LEAVE;
	}
	left -= rval;
    }
    rval = length;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type IO_ReadChars(od, buffer, length)
    IN INT_type	    od;
    IN CHAR_type    *buffer;
    IN INT_type	    length;
{
    INT_type	rval;	    /* The returned value */
    INT_type	left;	    /* Chars remaining to read */
    INT_type	dummy;	    /* For error messages */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Check parameters */
    if (length < 0) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[IO_ReadChars]: length too small ");
	dummy = TKERR_FormatInt("%d",	(INT_type) length);
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Special case length==0 */
    if (length == 0) {
	rval = LOCAL_ReadChars(od, buffer, (INT_type) 0);
	if (rval == LCL_ERROR_INT) {
	    /* map from local to tk error */
	    rval = ERROR_INT; 
	    TKError = LCLError;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[IO_ReadChars]: error returned ");
	    dummy = TKERR_FormatInt("from LOCAL_ReadChars for file %d",
				    (INT_type) od);
	    dummy = TKERR_Format(" for length 0");
	    dummy = TKERR_EndErrMsg();
	}
	goto LEAVE;
    }

    /* Normal case: length > 0, loop until all chars read */
    left = length;
    while (left > 0) {
	rval = LOCAL_ReadChars(od, (CHAR_type *) (buffer+(length-left)), left);
	if (rval == LCL_ERROR_INT) {
	    /* map from local to tk error */
	    rval = ERROR_INT; 
	    TKError = LCLError;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[IO_ReadChars]: error returned ");
	    dummy = TKERR_FormatInt("from LOCAL_ReadChars for file %d, ",
				    (INT_type) od);
	    dummy = TKERR_FormatInt("length %d, ", (INT_type) length);
	    dummy = TKERR_FormatInt("left %d", (INT_type) left);
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	if (rval == 0 || rval > left /* This can't happen!! */) {
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[IO_ReadChars]: error returned ");
	    dummy = TKERR_FormatInt("from LOCAL_ReadBytes, rval %d, ",
				    (INT_type) rval);
	    dummy = TKERR_FormatInt("left %d",
				    (INT_type) left);
	    dummy = TKERR_EndErrMsg();
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	left -= rval;
    }
    rval = length;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

/* Read a single char */
INT_type IO_ReadChar(od, c)
    IN INT_type	    od;
    IN CHAR_type    *c;
{
    INT_type	rval;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = IO_ReadChars(od, c, (INT_type) 1);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

/* Read a single byte */
INT_type IO_ReadByte(od, b)
    IN INT_type	    od;
    IN BYTE_type    *b;
{
    INT_type	rval;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = IO_ReadBytes(od, b, (INT_type) 1);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type IO_PrintInt(od, format, value)
    IN INT_type	    od;
    IN CHAR_type    *format;
    IN INT_type	    value;
{
    REGISTER INT_type	rval;	    /* The returned value */
    REGISTER INT_type	thevalue;   /* The value to print */
    REGISTER CHAR_type	*c;	    /* Pointer for stepping through format */
    INT_type		dummy;	    /* For error messages */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* See if negative */
    if (value < 0) {
	rval = IO_WriteChar(od, (CHAR_type) '-');
	thevalue = -value;
    } else {
	rval = SUCCESS_INT;
	thevalue = value;
    }

    for (c=format; rval!=ERROR_INT && *c!='\0'; c++) {
	if (*c != '%') {
	    rval = IO_WriteChar(od, *c);
	    continue;
	}
	switch (*++c) {	/* ++ skips % */

	    case '%':	/* Escape for '%' */
		rval = IO_WriteChar(od, (CHAR_type) '%');
		break;

	    case 'd':
	    case 'D':	/* Decimal representation */
		rval = PrintInt(od, thevalue, (INT_type) 10);
		break;

	    case 'x':
	    case 'X':	/* Hex representation */
		rval = PrintInt(od, thevalue, (INT_type) 16);
		break;

	    default:	/* Oops... */
		rval = ERROR_INT;
		TKError = BADPARM_err;
		/* Note: this can be tricky, since we are formatting an error
		   message while (presumably) formatting an error message. The hope
		   is that the original caller did the StartErrMsg and will do the
		   EndErrMsg (with enough of the original string intact) */
		/* dummy = TKERR_StartErrMsg(); */
		dummy = TKERR_Format("[IO_PrintInt]: unknown formatting character ");
/*
Last arg here needs to be a STRING_type.
		dummy = TKERR_FormatStringType("%s",c);
*/
		/* dummy = TKERR_EndErrMsg(); */
		goto LEAVE;
	}
    }

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type IO_PrintString(od, format, value)
    IN INT_type	    od;
    IN CHAR_type    *format;
    IN CHAR_type    *value;
{
    INT_type	rval;	    /* The returned value */
    INT_type	length;
    CHAR_type	*c;
    INT_type	dummy;	    /* For error messages */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;
    for (c=format; rval!=ERROR_INT && *c!='\0'; c++) {
	if (*c != '%') {
	    rval = IO_WriteChar(od, *c);
	    continue;
	}
	switch (*++c) {

	    case '%':	/* escape for '%' */
		rval = IO_WriteChar(od, (CHAR_type) '%');
		break;

	    case 's':
	    case 'S':	/* print string */
		length = StrLength(value);
		if (length == ERROR_INT) {
		    rval = ERROR_INT;
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[IO_PrintString]: illegal string length");
		    dummy = TKERR_EndErrMsg();
		    goto LEAVE;
		}
		rval = IO_WriteChars(od, value, length);
		break;
	    default:
		rval = ERROR_INT;
		TKError = BADPARM_err;
		/* Note: this can be tricky, since we are formatting an error
		   message while (presumably) formatting an error message. The hope
		   is that the original caller did the StartErrMsg and will do the
		   EndErrMsg (with enough of the original string intact) */
		/* dummy = TKERR_StartErrMsg(); */
		dummy = TKERR_Format("[IO_PrintString]: unknown formatting character ");
		dummy = TKERR_FormatInt("%d",(INT_type)*c);
		/* dummy = TKERR_EndErrMsg(); */
		goto LEAVE;
	}
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type IO_PrintChar(od, format, value)
    IN INT_type	    od;
    IN CHAR_type    *format;
    IN CHAR_type    value;
{
    INT_type	rval;	    /* the returned value */
    INT_type	dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;
    dummy = TKERR_StartErrMsg();
    dummy = TKERR_Format("[IO_PrintChar]: unimplemented operation");
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type IO_PrintByte(od, format, value)
    IN INT_type	    od;
    IN CHAR_type    *format;
    IN BYTE_type    value;
{
    INT_type	rval;	    /* the returned value */
    INT_type	dummy;	    /* For error messages */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;
    dummy = TKERR_StartErrMsg();
    dummy = TKERR_Format("[IO_PrintByte]: unimplemented operation");
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

/* Write a single char */
INT_type IO_WriteChar(od, c)
    IN INT_type	    od;
    IN CHAR_type    c;
{
    INT_type	rval;
    CHAR_type	buffer[1];

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    buffer[0] = c;
    rval = IO_WriteChars(od, buffer, (INT_type) 1);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

/* Write a single byte */
INT_type IO_WriteByte(od, b)
    IN INT_type	    od;
    IN BYTE_type    b;
{
    INT_type	rval;
    BYTE_type	buffer[1];

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    buffer[0] = b;
    rval = IO_WriteBytes(od, buffer, (INT_type) 1);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

/* Determine the length of a null-terminated C string */

static INT_type StrLength(s)
    CHAR_type *s;
{
	     INT_type	rval;
    REGISTER CHAR_type	*sptr;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    for (rval=0, sptr=s; *sptr!='\0'; sptr++) {
	rval++;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

/* Output an integer in specified base */

static INT_type PrintInt(od, value, base)
    IN INT_type	    od;
    IN INT_type	    value;
    IN INT_type	    base;
{
    INT_type	rval;
    INT_type	dummy;
    static CHAR_type digits[] = "0123456789ABCDEF";

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Only up to hex */
    if (base > 16) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt("[IO_PrintInt]: base too big %d", base);
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Special case single digit */
    if (value < base) {
	rval = IO_WriteChar(od, digits[value]);
	goto LEAVE;
    }

    rval = PrintInt(od, (INT_type) (value/base), base);
    if (rval != ERROR_INT) {
	rval = IO_WriteChar(od, digits[value%base]);
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}
