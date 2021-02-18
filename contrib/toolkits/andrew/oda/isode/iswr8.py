
ISWR8 DEFINITIONS ::=


%{

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

#define TK_TKI_TKERR
#define TK_TKI_APIRT

#include <odatk.h>

#undef TK_TKI_TKERR
#undef TK_TKI_APIRT

#define TK_IMPL_ISWR

#include <iswr.h>

#undef TK_IMPL_ISWR

#define PEPYPARM struct isode2toolkit_t *

INT_type dummy;

%}


BEGIN


EXPORTS Text-Unit, Type-Of-Coding;


IMPORTS  Content-Portion-Identifier
        FROM ISWR3
	Character-Coding-Attributes
	FROM ISWR10
	Raster-Gr-Coding-Attributes
	FROM ISWR13
	Geo-Gr-Coding-Attributes
	FROM ISWR16;


ENCODER build


Text-Unit [[ P struct isode2toolkit_t * ]]

%{
    SEQUENCE_BYTE_type ContentInfoBytes;
    CHAR_type *ContentInfoChars;
    INT_type ContentInfoLength;
%}

::= SEQUENCE {

    content-portion-attributes

	Content-Portion-Attributes [[ p parm ]],

	-- While optional, this must be decided
	-- at the Content-Portion-Attributes
	-- definition below.  If all the elements
	-- of this definition are omitted, then this
	-- field shall be omitted also.

    content-information

	%{
	    if ( GetAttr( parm->constituent,
			  at_CONTENT_INFORMATION,
			  (POINTER_type) &ContentInfoBytes,
			  /* default */ BOOL_false,
			  /* mask */ (PARM_MASK_type *) 0
			) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR8_Text__Unit]: can not get attribute at_CONTENT_INFORMATION" );
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    if ( Bytes2Chars( ContentInfoBytes,
			      &ContentInfoChars,
			      &ContentInfoLength
			    ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR8_Text__Unit]: can not convert bytes to chars" );
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	%}


	Content-Information [[ o ContentInfoChars $ ContentInfoLength ]]

}


Content-Portion-Attributes [[ P struct isode2toolkit_t * ]]

%{
    STRING_type ContentLogicalIDString;
    CHAR_type *ContentLogicalIDChars;

    STRING_type ContentLayoutIDString;
    CHAR_type *ContentLayoutIDChars;

    SEQUENCE_INT_type TypeOfCoding;
    struct OIDentifier *TypeOfCodingObjectID;
    INT_type TypeOfCodingSelection;

    STRING_type AltRepString;
    CHAR_type *AltRepChars;
    INT_type AltRepLength;
%}

::= SET {

    content-identifier-layout

	%{
	    ContentLayoutIDChars = String2Chars(ContentLayoutIDString);
	    if ( ContentLayoutIDChars == ERROR_CHARS ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR8_Content__Portion__Attributes]: can not convert string to content-layout-id chars" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	Content-Portion-Identifier [[ s ContentLayoutIDChars ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_CONTENT_ID_LAYOUT,
			(POINTER_type) &ContentLayoutIDString,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    content-identifier-logical

	%{
	    ContentLogicalIDChars = String2Chars(ContentLogicalIDString);
	    if ( ContentLogicalIDChars == ERROR_CHARS ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR8_Content__Portion__Attributes]: can not convert string to content-logical-identifier chars" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	[4] IMPLICIT Content-Portion-Identifier [[ s ContentLogicalIDChars ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_CONTENT_ID_LOGICAL,
			(POINTER_type) &ContentLogicalIDString,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    type-of-coding

	%{
	    /* support only object identifier format for type of coding */
	    TypeOfCodingSelection = (INT_type) 2;
	    TypeOfCodingObjectID = Ints2OID(TypeOfCoding);
	    if ( TypeOfCodingObjectID == NULLOID ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR8_Content__Portion__Attributes]: can not convert integer sequence to an object identifier" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	CHOICE << TypeOfCodingSelection >> {

	    [0] IMPLICIT
		    INTEGER {
			iso2022 (0),
			t6 (1)
		    },

	    [6] IMPLICIT
		    OBJECT IDENTIFIER [[ O TypeOfCodingObjectID ]]

		%{
		    if ( oid_free(TypeOfCodingObjectID) == NOTOK ) {
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_Format("[build_ISWR8_Content__Portion__Attributes]: can not free object identifier");
			dummy = TKERR_EndErrMsg();
			return( NOTOK );
		    }
		%}

	}

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_TYPE_OF_CODING,
			(POINTER_type) &TypeOfCoding,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    coding-attributes

	CHOICE {

	    character-coding-attributes

		[1] IMPLICIT Character-Coding-Attributes,

	    raster-gr-coding-attributes

		[2] IMPLICIT Raster-Gr-Coding-Attributes,

	    geo-gr-coding-attributes

		[7] IMPLICIT Geo-Gr-Coding-Attributes,
         
	    -- the following tags are reserved for additional types
	    -- coding attributes:

	    --	[8] composite-graphics
	    --	[9] audio
	    --	[10] dynamic-graphics

	    ext-cont-arch-coding-attributes

		[11] IMPLICIT EXTERNAL

	}

	OPTIONAL,

    alternative-representation

	%{
	    AltRepLength = AltRepString->length;
	    AltRepChars = AltRepString->sequence_value.chars;
	%}

	[3] IMPLICIT Alternative-Representation
	    [[ o AltRepChars $ AltRepLength ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_ALT_REP,
			(POINTER_type) &AltRepString,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>

}


Content-Information

::= OCTET STRING


Type-of-Coding

::= CHOICE {

    [0] IMPLICIT
	    INTEGER {
		iso2022 (0),
		t6 (1)
	    },

    [6] IMPLICIT
	    OBJECT IDENTIFIER

}


Alternative-Representation

::= OCTET STRING

    -- string of characters from the sets designated by
    -- the document profile attribute "alternative representation 
    -- character sets", plus carriage return and line feed


END
