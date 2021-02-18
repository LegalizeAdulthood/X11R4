
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


/* unix includes */
#include <stdio.h>

/* isode includes */
#include <psap.h>

/* toolkit includes */
#define TK_IMPL_ISWR
#define TK_GVARS_ISWR2
#define TK_TKI_TKERR
#define TK_TKI_STR
#define TK_TKI_MM

#include <apityp.h>
#include <iswr.h>
#include <tkerr.h>
#include <str.h>
#include <mm.h>

#undef TK_IMPL_ISWR
#undef TK_GVARS_ISWR2
#undef TK_TKI_TKERR
#undef TK_TKI_STR
#undef TK_TKI_MM

CHAR_type *String2Chars(string)
    IN STRING_type string;
{
    INT_type i;
    POINTER_type memp;

    CHAR_type *p1, *p2;
    CHAR_type *rval;

    INT_type dummy;

    memp = MM_Malloc(sizeof(CHAR_type)*(string->length+1));
    if ( memp == ERROR_POINTER ) {
	rval = ERROR_CHARS;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[String2Chars]:  Out of Memory");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    p1 = (CHAR_type *) memp;
    p2 = string->sequence_value.chars;
    for (i=1; i<=string->length; i++) {
	*p1++ = *p2++;
    }
    *p1 = '\0';
    rval = (CHAR_type *) memp;

LEAVE:
    return (rval);
}


INT_type Bytes2Chars(bytes,chars,len)
    IN SEQUENCE_BYTE_type bytes;
    OUT CHAR_type **chars;
    OUT INT_type *len;
{
    INT_type i;
    POINTER_type memp;

    CHAR_type *charPtr;
    BYTE_type *bytePtr;

    INT_type rval;

    INT_type dummy;

    rval = SUCCESS_INT;

    memp = MM_Malloc(sizeof(BYTE_type)*(bytes->length));
    if ( memp == ERROR_POINTER ) {
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[Bytes2Chars]:  Out of Memory");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    charPtr = (CHAR_type *) memp;
    bytePtr = bytes->sequence_value.bytes;
    for (i=1; i<=bytes->length; i++) {
	*charPtr++ = (CHAR_type) *bytePtr++;
    }

    *chars = (CHAR_type *) memp;
    *len = bytes->length;

LEAVE:
    return (rval);
}


STRING_type GetLastIDDigits(IDString)
    IN STRING_type IDString;
{
    INT_type len;
    INT_type len2;
    INT_type count;
    INT_type count2;
    INT_type last;
    INT_type pos;

    STRING_type rval;

    INT_type dummy;

    rval = ERROR_STRING;

    len = IDString->length;

    last = len - 1;
    for ( count=last; count>0; count-- ) {

	if ( IDString->sequence_value.chars[count] == (CHAR_type) ' ' ) {

	    /* get position of last id digits */
	    pos = count + 1;

	    /* get length of last id digits */
	    len2 = last - pos + 1;

	    /* generate string for last id digits */
	    rval = MakeString(len2);
	    if ( rval == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[GetLastIDDigits]: Can not make string");
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	    }

	    /* extract last id digits */
	    for ( count2=0; count2<len2; count2++ ) {
		rval->sequence_value.chars[count2] =
		    IDString->sequence_value.chars[count2+pos];
	    }

	    /* return last id digits */
	    goto LEAVE;

	}

    }

LEAVE:
    return ( rval );

}


struct OIDentifier *Ints2OID(ints)
    IN SEQUENCE_INT_type ints;
{
    INT_type i;
    POINTER_type memp;

    struct OIDentifier *rval;

    INT_type dummy;

    /* get memory for structure */

    memp = MM_Malloc(sizeof(OIDentifier));
    if ( memp == ERROR_POINTER ) {
	rval = NULLOID;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[Ints2OID]:  Out of Memory");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    rval = (struct OIDentifier *) memp;

    /* get memory for integers */

    memp = MM_Malloc(sizeof(INT_type)*(ints->length));
    if ( memp == ERROR_POINTER ) {
	rval = NULLOID;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[Ints2OID]:  Out of Memory");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    rval->oid_nelem = ints->length;
    rval->oid_elements = (unsigned int *) memp;

    /* copy integer sequence over to object identifier elements */

    for (i=0; i<ints->length; i++) {
	rval->oid_elements[i] = (unsigned int) ints->sequence_value.ints[i];
    }

LEAVE:
    return (rval);
}
