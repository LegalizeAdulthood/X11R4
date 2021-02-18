
/*
 *
 * (C) Copyright 1989 by Carnegie Mellon University
 *
 * Permission to use, copy, modify, and distribute these programs
 * and their documentation for any purpose and without fee is
 * hereby granted, provided that this copyright and permission
 * notice appear on all copies and supporting documentation, and
 * that the name of Carnegie Mellon University not be used in
 * advertising or publicity pertaining to distribution of the
 * programs without specific prior permission and notice be given
 * in supporting documentation that copying and distribution is
 * by permission of Carnegie Mellon University.
 *
 * Carnegie Mellon University makes no representations about the
 * suitability of this software for any purpose.  It is provided
 * as is, without express or implied warranty.
 *
 * Software by Ann Marks and James T. Lui,
 * Information Technology Center, Carnegie Mellon University,
 * except where noted.
 *
 */


/* toolkit includes */
#define TK_IMPL_ISRD
#define TK_GVARS_ISRD1
#define TK_IMPL_INIT
#define TK_TKI_TKERR
#define TK_IMPL_CONST
#define TK_TKI_MM

#include <values.h>
#include <apityp.h>
#include <init.h>
#include <acd.h>
#include <iter.h>
#include <isrd.h>
#include <tkerr.h>
#include <mm.h>

#undef TK_IMPL_ISRD
#undef TK_GVARS_ISRD1
#undef TK_IMPL_INIT
#undef TK_TKI_TKERR
#undef TK_IMPL_CONST
#undef TK_TKI_MM

/* UNIX includes */
#include <stdio.h>

/* isode includes */
#include <psap.h>

SEQUENCE_BYTE_type String2Bytes(string)
    IN STRING_type string;
{
    INT_type i;
    POINTER_type memp;

    CHAR_type *charPtr;
    BYTE_type *bytePtr;

    SEQUENCE_BYTE_type rval;

    INT_type dummy;

    memp = MM_Malloc(sizeof(BYTE_type)*(string->length));
    if ( memp == ERROR_POINTER ) {
	rval = ERROR_SEQUENCE;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[String2Bytes]:  Out of Memory");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    bytePtr = (BYTE_type *) memp;
    charPtr = string->sequence_value.chars;
    for (i=1; i<=string->length; i++) {
	*bytePtr++ = (BYTE_type) *charPtr++;
    }

    rval = MakeSequence(SEQUENCE_BYTE_tag,string->length);
    rval->sequence_value.bytes = (BYTE_type *) memp;

LEAVE:
    return (rval);
}


/* global variable for use with the next two routine in this module */

DOCUMENT_type SavedDoc = ERROR_DOCUMENT;


/*
   These two routines allow the saving and fetching of a document in
   a temporary location (ie. the above global variable SavedDoc.
*/

INT_type SaveThisDocument(doc)
    IN DOCUMENT_type doc;
{
    INT_type rval;

    rval = SUCCESS_INT;

    /* Save pointer to document in global variable */
    SavedDoc = doc;

LEAVE:
    return ( rval );
}


DOCUMENT_type GetSavedDocument()
{
    DOCUMENT_type rval;

    /* return globally stored document pointer */
    rval = SavedDoc;

LEAVE:
    return ( rval );
}


SEQUENCE_INT_type OID2Ints(oid)
    IN struct OIDentifier *oid;
{
    INT_type i;

    SEQUENCE_INT_type rval;

    INT_type dummy;

    /* make sequence as long as the number of integers in object id */

    rval = MakeSequence(SEQUENCE_INT_tag,oid->oid_nelem);
    if ( rval == ERROR_SEQUENCE ) {
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[OID2Ints]:  Out of Memory");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* copy integers in object id over to integer sequence */

    for (i=0; i<oid->oid_nelem; i++) {
	rval->sequence_value.ints[i] = (INT_type) oid->oid_elements[i];
    }

LEAVE:
    return (rval);
}


BOOL_type MatchInts(ints1,ints2)
    IN SEQUENCE_INT_type ints1;
    IN SEQUENCE_INT_type ints2;
{
    INT_type i;

    BOOL_type rval;

    INT_type dummy;

    if ( ints1->length != ints2->length ) {
	rval = BOOL_false;
	goto LEAVE;
    }

    for (i=0; i<ints1->length; i++) {
	if ( ints1->sequence_value.ints[i] != ints2->sequence_value.ints[i] ) {
	    rval = BOOL_false;
	    goto LEAVE;
	}
    }

    rval = BOOL_true;

LEAVE:
    return (rval);
}
