
ISRD8 DEFINITIONS ::=


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

#define TK_IMPL_ISRD

#include <isrd.h>

#undef TK_IMPL_ISRD

#define PEPYPARM struct isode2toolkit_t *

INT_type dummy;

%}


BEGIN


EXPORTS Text-Unit, Type-Of-Coding;


IMPORTS  Content-Portion-Identifier
        FROM ISRD3
	Character-Coding-Attributes
	FROM ISRD10
	Raster-Gr-Coding-Attributes
	FROM ISRD13
	Geo-Gr-Coding-Attributes
	FROM ISRD16;


DECODER parse


Text-Unit [[ P struct isode2toolkit_t * ]]

%{
    SEQUENCE_INT_type TypeOfCoding;

    STRING_type ContentLayoutIDString;
    STRING_type ContentLogicalIDString;

    INT_type ContentInfoLength;
    CHAR_type *ContentInfoChars;
    STRING_type ContentInfoString;
    SEQUENCE_BYTE_type ContentInfo;

    BOOL_type NoContentLayoutID;
    BOOL_type NoContentLogicalID;

    struct hash_t *HashStructure;

    CONSTITUENT_type TheConstituent;
%}

::= SEQUENCE {

    content-portion-attributes

	Content-Portion-Attributes [[ p parm ]]

	OPTIONAL,

    content-information

	Content-Information [[ o ContentInfoChars $ ContentInfoLength ]]

	%{
	    ContentInfoString = MakeString(ContentInfoLength);
	    if ( ContentInfoString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD8_Text__Unit]: can not make string for attribute at_CONTENT_INFORMATION" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    ContentInfoString->sequence_value.chars = ContentInfoChars;
	    ContentInfo = String2Bytes(ContentInfoString);
	    if ( ContentInfo == ERROR_SEQUENCE) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD8_Text__Unit]: can not make byte sequence for attribute at_CONTENT_INFORMATION" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    if ( SetAttr( parm->constituent,
			  at_CONTENT_INFORMATION,
			  (POINTER_type) ContentInfo,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD8_Text__Unit]: can not set attribute at_CONTENT_INFORMATION" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

}

%{
    if ( GetAttr( parm->constituent,
		  at_CONTENT_ID_LAYOUT,
		  (POINTER_type) &ContentLayoutIDString,
		  /* default */ BOOL_false,
		  /* mask */ (PARM_MASK_type *) 0
	        ) != ERROR_INT ) {
	NoContentLayoutID = BOOL_false;
	if ( ISODE_FillInConstituent ( parm->constituent,
				       ContentLayoutIDString
				     ) == ERROR_INT ) {
	    return ( NOTOK );
	}
    } else {
	NoContentLayoutID = BOOL_true;
    }
    if ( GetAttr( parm->constituent,
		  at_CONTENT_ID_LOGICAL,
		  (POINTER_type) &ContentLogicalIDString,
		  /* default */ BOOL_false,
		  /* mask */ (PARM_MASK_type *) 0
	        ) != ERROR_INT ) {
	NoContentLogicalID = BOOL_false;
	if ( NoContentLayoutID ) {
	    TheConstituent = parm->constituent;
	} else {
	    HashStructure =
		ISODE_GetHashStructure(ContentLayoutIDString,parm->document);
	    TheConstituent = HashStructure->constituent;
	}
	if( ISODE_AddToHash( TheConstituent,
			     ContentLogicalIDString
			   ) == ERROR_INT ) {
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[parse_ISRD8_Text__Unit]: can not add content portion under the logical id found" );
	    dummy = TKERR_EndErrMsg();
	    return ( NOTOK);
	}
    } else {
	NoContentLogicalID = BOOL_true;
    }
    if ( NoContentLayoutID && NoContentLogicalID) {
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[parse_ISRD8_Text__Unit]: can not find content logical or layout id" );
	dummy = TKERR_EndErrMsg();
	return ( NOTOK );
    }
%}


Content-Portion-Attributes [[ P struct isode2toolkit_t * ]]

%{
    CHAR_type *ContentLayoutIDChars;
    STRING_type ContentLayoutIDString;

    CHAR_type *ContentLogicalIDChars;
    STRING_type ContentLogicalIDString;

    struct OIDentifier *TypeOfCodingObjectID;
    SEQUENCE_INT_type TypeOfCoding;
    VALUE_type ContentType;

    INT_type AltRepLength;
    CHAR_type *AltRepChars;
    STRING_type AltRepString;

    BOOL_type ContentLayoutIDFound;
    BOOL_type ContentLogicalIDFound;
    BOOL_type TypeOfCodingFound;
    BOOL_type AltRepFound;
%}

::=

%{
    ContentLayoutIDFound = BOOL_false;
    ContentLogicalIDFound = BOOL_false;
    TypeOfCodingFound = BOOL_false;
    AltRepFound = BOOL_false;
%}

SET {

    content-identifier-layout

	Content-Portion-Identifier [[ s ContentLayoutIDChars ]]

	%{
	    ContentLayoutIDString = Chars2String(ContentLayoutIDChars);
	    if ( ContentLayoutIDString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD8_Content__Portion__Attributes]: can not convert content layout id to a string" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    ContentLayoutIDFound = BOOL_true;
	%}

	OPTIONAL,

    content-identifier-logical

	[4] IMPLICIT Content-Portion-Identifier [[ s ContentLogicalIDChars ]]

	%{
	    ContentLogicalIDString = Chars2String(ContentLogicalIDChars);
	    if ( ContentLogicalIDString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD8_Content__Portion__Attributes]: can not convert content logical id to a string" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    ContentLogicalIDFound = BOOL_true;
	%}

	OPTIONAL,

    type-of-coding

	CHOICE {

	    [0] IMPLICIT
		    INTEGER {
			iso2022 (0),
			t6 (1)
		    }

		%{
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[parse_ISRD8_Content__Portion__Attributes]: This type of coding format not supported");
		    dummy = TKERR_Format("[parse_ISRD8_Content__Portion__Attributes]: Type of coding must be an object identifier");
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
		%},

	    [6] IMPLICIT
		    OBJECT IDENTIFIER [[ O TypeOfCodingObjectID ]]

		%{
		    TypeOfCoding = OID2Ints(TypeOfCodingObjectID);
		    if ( TypeOfCoding == ERROR_SEQUENCE ) {
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_Format("[parse_ISRD8_Content__Portion__Attributes]: can not convert type of coding to integer sequence" );
			dummy = TKERR_EndErrMsg();
			return( NOTOK );
		    }
		    if ( MatchInts(TypeOfCoding,at_TYPE_OF_CODING_cc) ) {
			ContentType = CHARACTER_CONTENT;
		    } else if ( MatchInts(TypeOfCoding,at_TYPE_OF_CODING_gg) ) {
			ContentType = GEOMETRIC_GRAPHICS_CONTENT;
		    } else if ( (MatchInts(TypeOfCoding,
					   at_TYPE_OF_CODING_T6)) ||
				(MatchInts(TypeOfCoding,
					   at_TYPE_OF_CODING_T4_1)) ||
				(MatchInts(TypeOfCoding,
					   at_TYPE_OF_CODING_T4_2)) ||
				(MatchInts(TypeOfCoding,
					   at_TYPE_OF_CODING_bitmap)) ) {
			ContentType = RASTER_GRAPHICS_CONTENT;
		    } else {
			dummy = TKERR_StartErrMsg();
			dummy = TKERR_Format("[parse_ISRD8_Content__Portion__Attributes]: Type of coding invalid" );
			dummy = TKERR_EndErrMsg();
			return( NOTOK );
		    }
		    TypeOfCodingFound = BOOL_true;
		%}

	}

	OPTIONAL,

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

	[3] IMPLICIT Alternative-Representation
	    [[ o AltRepChars $ AltRepLength ]]

	%{
	    AltRepString = MakeString(AltRepLength);
	    if ( AltRepString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD8_Content__Portion__Attributes]: can not make string" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    AltRepString->sequence_value.chars = AltRepChars;
	    AltRepFound = BOOL_true;
	%}

	OPTIONAL

}

%{
    if ( TypeOfCodingFound ) {
	parm->constituent = MakeContent(parm->document,ContentType);
	if ( parm->constituent == ERROR_CONSTITUENT ) {
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[parse_ISRD8_Content__Portion__Attributes]: can not make content portion" );
	    dummy = TKERR_EndErrMsg();
	    return( NOTOK );
	}
	if ( SetAttr( parm->constituent,
		      at_TYPE_OF_CODING,
		      (POINTER_type) TypeOfCoding,
		      /* mask */ (PARM_MASK_type) 0
		    ) == ERROR_INT ) {
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[parse_ISRD8_Content__Portion__Attributes]: can not set attribute at_TYPE_OF_CODING" );
	    dummy = TKERR_EndErrMsg();
	    return( NOTOK );
	}
    } else {
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[parse_ISRD8_Text__Unit]: can not find attribute at_TYPE_OF_CODING");
	dummy = TKERR_Format("[parse_ISRD8_Text__Unit]: Assuming CHARACTER_CONTENT by default");
	dummy = TKERR_EndErrMsg();
	parm->constituent =
	    MakeContent(parm->document, CHARACTER_CONTENT);
	if ( parm->constituent == ERROR_CONSTITUENT ) {
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[parse_ISRD8_Content__Portion__Attributes]: can not make content portion" );
	    dummy = TKERR_EndErrMsg();
	    return( NOTOK );
	}
    }
    if ( ContentLayoutIDFound ) {
	if ( SetAttr( parm->constituent,
		      at_CONTENT_ID_LOGICAL,
		      (POINTER_type) ContentLogicalIDString,
		      /* mask */ (PARM_MASK_type) 0
		    ) == ERROR_INT ) {
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[parse_ISRD8_Content__Portion__Attributes]: can not set attribute at_CONTENT_ID_LOGICAL" );
	    dummy = TKERR_EndErrMsg();
	    return( NOTOK );
	}
    }
    if ( ContentLogicalIDFound ) {
	if ( SetAttr( parm->constituent,
		      at_CONTENT_ID_LAYOUT,
		      (POINTER_type) ContentLayoutIDString,
		      /* mask */ (PARM_MASK_type) 0
		    ) == ERROR_INT ) {
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[parse_ISRD8_Content__Portion__Attributes]: can not set attribute at_CONTENT_ID_LAYOUT" );
	    dummy = TKERR_EndErrMsg();
	    return( NOTOK );
	}
    }
    if ( AltRepFound ) {
	if ( SetAttr( parm->constituent,
		      at_ALT_REP,
		      (POINTER_type) AltRepString,
		      /* mask */ (PARM_MASK_type) 0
		    ) == ERROR_INT ) {
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[parse_ISRD8_Content__Portion__Attributes]: can not set attribute at_ALT_REP" );
	    dummy = TKERR_EndErrMsg();
	    return( NOTOK );
	}
    }
%}


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
