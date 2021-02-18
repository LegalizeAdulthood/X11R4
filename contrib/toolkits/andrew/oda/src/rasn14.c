/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rasn14.c,v 1.3 89/10/31 14:29:05 jaap Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rasn14.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	rasn14.c,v $
 * Revision 1.3  89/10/31  14:29:05  jaap
 * shut up lint
 * 
 * Revision 1.2  89/10/26  14:12:06  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.1  89/09/21  06:36:51  mss
 * Initial revision
 * 
 * Revision 1.3  89/01/24  12:17:24  mss
 * Made forward decls consistent with actual declations (w.r.t. static)
 * 
 * Revision 1.2  88/11/04  01:35:03  mz0k
 * Wrote the length octet for the 'Identifier' field 
 * of the Structured Name
 * 
 * Revision 1.1  88/10/26  18:58:23  mz0k
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/rasn14.c,v $";
#endif

#define TK_IMPL_RASN1
#define TK_GVARS_RASN14
#define TK_TKI_IO
#define TK_TKI_ATTR
#define TK_IMPL_CONST
#define TK_TKI_SEQ
#define TK_TKI_MM
#define TK_IMPL_INIT
#define TK_TKI_STR
#define TK_TKI_TKERR
#define TK_IMPL_RODIF
#define TK_TKI_FONT

#include <font.h>
#include <rasn1.h>
#include <rodif.h>

#undef TK_IMPL_RASN1
#undef TK_GVARS_RASN14
#undef TK_TKI_IO
#undef TK_TKI_ATTR
#undef TK_IMPL_CONST
#undef TK_TKI_SEQ
#undef TK_TKI_MM
#undef TK_IMPL_INIT
#undef TK_TKI_STR
#undef TK_TKI_TKERR
#undef TK_IMPL_RODIF
#undef TK_TKI_FONT

/* Forward declarations */
static INT_type RASN1_CompleteStructName();
static INT_type RASN1_Identifier();

INT_type RASN1_StructuredName(id, count, name, isimplicit, whichfield)
    IN INT_type		    id;
    OUT	INT_type	    *count;
    OUT	STRUCTURED_NAME_type	*name;
    IN BOOL_type	    isimplicit;
    IN INT_type	    whichfield; /* to indicate which of the CHOICE is present */
{   /* 
      */


	INT_type		rval;	
	INT_type		dummy;
	BOOL_type		isdeflen;
	BYTE_type		bytes[2];

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif


    /* If not IMPLICIT, read the tag byte and see what the nextbyte is */
    if (!isimplicit) {
	rval = RASN1_ReadByte(id, &nextbyte);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	count--;
    }
    else if (whichfield == (INT_type) 1) { /* the first field */
	nextbyte = (BYTE_type) 0xa0; /* fake it as the first field of EXPLICIT */
    }
    else if (whichfield	== (INT_type) 2) { /* the second field */
	nextbyte = (BYTE_type) 0x81;
    }

    /* Accoding to the tag byte,  decide what to do next */
    switch (nextbyte) {
	case 0xa0:
	    name->structured_name_tag = STRUCTURED_NAME_SEQ_tag;

	    /* Make a sequence for it */
	    name->structured_name_value.idseq = MakeSequence(SEQUENCE_ISO9541_ID_tag, (INT_type) 0);
	    if (name->structured_name_value.idseq == ERROR_SEQUENCE) {
		rval = ERROR_INT;
		goto LEAVE;
	    }

	    rval = RASN1_CompleteStructName(id, count, name->structured_name_value.idseq, BOOL_true);
	    break;
	case 0x81:
	    /* Make this to be EXPLICIT to avoid ambiguity */
	    name->structured_name_tag = STRUCTURED_NAME_ID_tag;
	    
	    /* Read the length octet for the primitive Identifier */
	    rval = RASN1_Length(id, count);
	    if (rval == ERROR_INT) {
		goto LEAVE;
	    }

	    isdeflen = ((rval == INDEFINITE_LENGTH)? BOOL_false:BOOL_true);

	    /* Then the Identifier itself. */
	    rval = RASN1_Identifier(id, count, &(name->structured_name_value.id), BOOL_false, (INT_type) 0);
	    
	    /* Read the EOC */
	    if (!isdeflen) {
		rval = RASN1_ReadBytes(id, bytes, (INT_type) 2);
		if (rval == ERROR_INT) {
		    goto LEAVE;
		}

		if (((INT_type) bytes[0] != 0x00) || ((INT_type) bytes[1] != 0x00)) {
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_StructuredName]:  Invalid byte 0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
		    goto LEAVE;
		}
	    }
	    break;
	default:
	    rval = ERROR_INT;
	    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_FormatInt( "[RASN1_StructuredName]:  Invalid byte 0x%x", (INT_type) nextbyte);
	    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
	    dummy = TKERR_EndErrMsg();
#endif
	    goto LEAVE;
    }
    


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}


static INT_type RASN1_CompleteStructName(id, count, idseq, isimplicit)
    IN INT_type		    id;
    OUT	INT_type	    *count;
    OUT	SEQUENCE_ISO9541_ID_type	idseq;
    IN BOOL_type	    isimplicit;
{   /* 
      */

	INT_type		loccount;
	BOOL_type		isdeflen, go_on;
	INT_type		rval;	
	INT_type		dummy, which;
	ISO9541_ID_type		isoid;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif


    loccount = 0;
    go_on = BOOL_true;

    /* If not IMPLICIT, read the tag byte and, presumably it is 0x30 */
    if (!isimplicit) {
	rval = RASN1_ReadByte(id, &nextbyte);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	count--;
    }


    /* First thing is to read the length octet(s)   */
    rval = RASN1_Length(id, count);	
    if (rval == ERROR_INT) {
	goto LEAVE;
    }
    count -= rval;

    isdeflen = ((rval != INDEFINITE_LENGTH)? BOOL_true : BOOL_false);
    if (isdeflen)	 { loccount = rval; }


    while (go_on) {

	/* See if the end -- Exit 1: definite length becomes  0. */
	if ((isdeflen) && (loccount == 0)) {
	    rval = SUCCESS_INT;
	    goto LEAVE;
	}
    
    
	/* Fetch the nextbyte and see */
	rval = RASN1_ReadByte(id, &nextbyte);   
	loccount--;
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
    
	/* The nextbyte can only be one of the following:
	 80 -- integer;
	 81 -- character sequence;

	 Also 00 -- possible EOC.
	*/


	switch ((INT_type) nextbyte) {
		case 0x00:
		    /* Exit 2: indefinite length ... comes to the EOC. */
		    if (isdeflen) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_CompleteStructName]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    rval = RASN1_ReadByte(id, &nextbyte);	
		    if ((INT_type) nextbyte != 0x00) {
			rval = ERROR_INT;
			TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_FormatInt( "[RASN1_CompleteStructName]: Invalid byte:0x%x", (INT_type) nextbyte);
			dummy = TKERR_FormatInt( " at 0x%x", bytecount );
			dummy = TKERR_EndErrMsg();
#endif
			goto LEAVE;
		    }
		    go_on = BOOL_false;
		    break;
		case 0x80:
		case 0x81:
		    /* Notice this IMPLICIT situation.  The Tag has been read already,  as in an EXPLICIT case.  Thus pass in an IMPLICIT */
		    which = ((nextbyte == (BYTE_type) 0x80)? (INT_type) 1 : (INT_type) 2);
		    rval = RASN1_Identifier(id, &loccount, &isoid, BOOL_true, which);
		    if (rval == ERROR_INT) {
			goto LEAVE;
		    }
		    /* Append it to the sequence */
		    rval = ExpandSequence(idseq, (INT_type) 1);
		    (idseq)->sequence_value.iso9541_ids[(idseq)->length-1] = isoid;
		    break;
		default:
		    rval = ERROR_INT;
		    TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_FormatInt( "[RASN1_CompleteStructName]: Invalid byte:0x%x", (INT_type) nextbyte);
		    dummy = TKERR_FormatInt( " at 0x%x", bytecount );
		    dummy = TKERR_EndErrMsg();
#endif
		    goto LEAVE;
	}
    }
    



LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}


static INT_type RASN1_Identifier(id, count, isoid, isimplicit, whichfield)
    IN INT_type		    id;
    OUT	INT_type	    *count;
    OUT	ISO9541_ID_type	    *isoid;
    IN BOOL_type	    isimplicit;
    IN INT_type		    whichfield;
{   /* 
      */

	INT_type		rval;	
	INT_type		dummy;
	SEQUENCE_BYTE_type	bytes;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif


    /* If not IMPLICIT, read the tag byte and see */
    if (!isimplicit) {
	rval = RASN1_ReadByte(id, &nextbyte);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	*count--;
    }
    else if (whichfield == (INT_type) 1) {
	nextbyte = (BYTE_type) 0x80;
    }
    else if (whichfield == (INT_type) 2) {
	nextbyte = (BYTE_type) 0x81;
    }

    if (nextbyte == (BYTE_type) 0x80) {
	isoid->iso9541_id_tag = ID_INT_tag;
	rval = RASN1_GetInteger(id, count);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	isoid->iso9541_id_value.integer = rval;
    }
    else if (nextbyte == (BYTE_type) 0x81) {
	isoid->iso9541_id_tag = ID_STR_tag;

	/* Make an empty sequence first. */
	bytes = MakeSequence(SEQUENCE_BYTE_tag, (INT_type) 0);
	if (bytes == ERROR_SEQUENCE) {
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	rval = RODIF_GetSequenceByte(id, count, bytes);
	if (rval == ERROR_INT) {
	    goto LEAVE;
	}
	isoid->iso9541_id_value.octet_string = bytes; 
    }
    else {
	rval = ERROR_INT;
	TKError = ODIF_err;
#ifndef DONT_FORMAT_ERRORS
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt( "[RASN1_Identifier]: Invalid byte:0x%x", (INT_type) nextbyte);
	dummy = TKERR_FormatInt( " at 0x%x", bytecount );
	dummy = TKERR_EndErrMsg();
#endif
	goto LEAVE;
    }
  


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}


