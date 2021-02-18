
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/str1.c,v 1.1 89/09/21 06:39:50 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/str1.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	str1.c,v $
 * Revision 1.1  89/09/21  06:39:50  mss
 * Initial revision
 * 
 * Revision 1.17  89/08/31  18:20:47  mss
 * Fixed to use new LOCAL interface
 * 
 * Revision 1.16  89/04/21  18:17:24  annm
 * chnages for deleteseq/deletesequencedeeply
 * 
 * Revision 1.15  88/12/13  19:56:54  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.14  88/09/13  15:03:19  mz0k
 * Added the ConcatString.
 * 
 * Revision 1.13  88/09/06  13:13:49  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.12  88/09/06  12:03:02  annm
 * no change
 * 
 * Revision 1.11  88/07/01  12:04:16  jr
 * Make DeleteString return SUCCESS_INT
 * 
 * Revision 1.10  88/06/08  16:58:36  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.9  88/06/08  14:37:42  annm
 * added Copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/str1.c,v $";
#endif

/* Put .c template comment here */

#define TK_IMPL_STR
#define TK_GVARS_STR1
#define TK_TKI_MM
#define TK_TKI_SEQ

#include <mm.h>
#include <seq.h>
#include <str.h>

STRING_type Chars2String(str)
    IN CHAR_type *str;
{
	     STRING_type    rval;	/* The returned value */
    REGISTER CHAR_type	    *strptr;	/* Pointer for stepping through str */
    REGISTER CHAR_type	    *seqptr;	/* Pointer for stepping in seq value */
    REGISTER INT_type	    length;	/* How many non-\0 chars in string */
#ifdef USE_BCOPY
	     INT_type	    dummy;	/* Used to ignore return value when bcopy'ing */
#endif

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Find length of char string */
    for (length=0, strptr=str; *strptr!='\0'; strptr++, length++) {
	continue;
    }

    /* Allocate STRING_type of appropriate length */
    rval = MakeString(length);
    if (rval == ERROR_STRING) {
	goto LEAVE;
    }

    /* Copy characters into sequence */
    seqptr = rval->sequence_value.chars;
#ifdef USE_BCOPY
    if (LOCAL_Bcopy((POINTER_type) strptr, (POINTER_type) seqptr, length) == LCL_ERROR_INT) {
	TKError	= LCLError;	    /* Map errors */
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[Chars2String]: error returned ");
	dummy = TKERR_Format("from LOCAL_Bcopy");
	dummy = TKERR_EndErrMsg();
	dummy = DeleteString(rval); /* Only 1 error at a time */
	rval = ERROR_STRING;
	goto LEAVE;
    }
#else
    for (strptr=str; *strptr!='\0'; strptr++, seqptr++) {
	*seqptr = *strptr;
    }
#endif

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

STRING_type ConcatString(s1, s2)
    IN STRING_type  s1;
    IN STRING_type  s2;
{
    STRING_type	rval;	    /* the returned value */
    INT_type	i;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif
   
    rval = MakeString((INT_type) s1->length+s2->length);
    if (rval ==	ERROR_STRING) {  
	TKError = INTERNAL_err;
	goto LEAVE; 
    }


    for (i=0; i<=s1->length-1; i++) {
	rval->sequence_value.chars[i] = s1->sequence_value.chars[i];
    }
    for (i=s1->length; i<=s1->length+s2->length-1; i++) {
	rval->sequence_value.chars[i] = s2->sequence_value.chars[i-s1->length];
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}



INT_type DeleteString(string)
    IN STRING_type  string;
{

    /*
	Strategy:
	    call DeleteSequence
    */

    INT_type	rval;	    /* the returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = DeleteSequence( (SEQUENCE_type) string );

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}



STRING_type MakeString(length)
    IN INT_type	    length;
{
    STRING_type	rval;	    /* the returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = (STRING_type) MakeSequence(SEQUENCE_CHAR_tag, length);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type STR_Final()
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

INT_type STR_Init()
{
    INT_type	rval;	    /* the returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif
    
    /* Assume that all will work */
    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}
