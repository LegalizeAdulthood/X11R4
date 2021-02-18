
ISRD6 DEFINITIONS ::=


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


-- EXPORTS Presentation-Style-Descriptor, Presentation-Attributes,
--	Content-Type, Content-Architecture-Class,
--        Layout-Style-Descripto,Fill-Order, Block-Alignment;


IMPORTS Object-or-Class-Identifier, Style-Identifier,
	Layout-Category-Name, Object-Id-Expression
        FROM ISRD3
        Comment-String, Transparency, Colour, Border,
	Layout-Object-Type
        FROM ISRD4
	Character-Attributes
	FROM ISRD9
	Raster-Graphics-Attributes
	FROM ISRD12
	Geometric-Graphics-Attributes
	FROM ISRD15;


DECODER parse


Presentation-Style-Descriptor [[ P struct isode2toolkit_t * ]]

%{

    CHAR_type *PresentStyleIDChars;
    STRING_type PresentStyleIDString;

    CHAR_type *CommentChars;
    INT_type CommentLength;
    STRING_type CommentString;

    CHAR_type *NameChars;
    INT_type NameLength;
    STRING_type NameString;

%}

::=

%{
    parm->constituent = MakeStyle( parm->document, PRESENTATION_STYLE);
    if ( parm->constituent == ERROR_CONSTITUENT ) {
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[parse_ISRD6_Presentation__Style__Descriptor]: can not make style");
	dummy = TKERR_EndErrMsg();
	return( NOTOK );
    }
%}

SET {

    style-identifier

	Style-Identifier [[ s PresentStyleIDChars ]]

	%{
	    PresentStyleIDString = Chars2String(PresentStyleIDChars);
	    if ( PresentStyleIDString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD6_Presentation__Style__Descriptor]: can not make string");
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    if ( SetAttr( parm->constituent,
			  at_PRESENTATION_STYLE_ID,
			  (POINTER_type) PresentStyleIDString,
			  /* mask */ (PARM_MASK_type *) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD6__Presentation__Style__Descriptor]: can not set attribute at_PRESENTATION_STYLE_ID" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%},

    user-readable-comments

	[0] IMPLICIT OCTET STRING [[ o CommentChars $ CommentLength ]]

	%{
	    CommentString = MakeString(CommentLength);
	    if ( CommentString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD6_Presentation__Style__Descriptor]: can not make string");
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    CommentString->sequence_value.chars = CommentChars;
	    if ( SetAttr( parm->constituent,
			  at_USER_READABLE_COMMENTS,
			  (POINTER_type) CommentString,
			  /* mask */ (PARM_MASK_type *) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD6_Presentation__Style__Descriptor]: can not set attribute at_USER_READABLE_COMMENTS" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    user-visible-name

	[1] IMPLICIT OCTET STRING [[ o NameChars $ NameLength ]]

	%{
	    NameString = MakeString(NameLength);
	    if ( NameString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD6_Presentation__Style__Descriptor]: can not make string");
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    NameString->sequence_value.chars = NameChars;
	    if ( SetAttr( parm->constituent,
			  at_USER_VISIBLE_NAME,
			  (POINTER_type) NameString,
			  /* mask */ (PARM_MASK_type *) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD6_Presentation_Style__Descriptor]: can not set attribute at_USER_VISIBLE_NAME" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    transparency

	[2] IMPLICIT Transparency

	OPTIONAL,

    presentation-attributes

	[3] IMPLICIT Presentation-Attributes

	OPTIONAL,

    colour

	[4] IMPLICIT Colour

	OPTIONAL,

    border

	[5] IMPLICIT Border

	OPTIONAL

}

%{
    if ( GetAttr( parm->constituent,
		  at_PRESENTATION_STYLE_ID,
		  (POINTER_type) &PresentStyleIDString,
		  /* default */ BOOL_false,
		  /* mask */ (PARM_MASK_type *) 0
	        ) == ERROR_INT ) {
	return ( NOTOK );
    } else {
	if ( ISODE_FillInConstituent ( parm->constituent,
				       PresentStyleIDString
				     ) == ERROR_INT ) {
	    return ( NOTOK );
	}
    }
%}


Presentation-Attributes         ::= SET {
    content-type                    Content-Type OPTIONAL,
    content-architecture-class      Content-Architecture-Class OPTIONAL,
    character-attributes            [0] IMPLICIT Character-Attributes OPTIONAL,
    raster-graphics-attributes      [1] IMPLICIT Raster-Graphics-Attributes
                                                                      OPTIONAL,
    geometric-graphics-attributes   [2] IMPLICIT Geometric-Graphics-Attributes
                                                                      OPTIONAL,

        -- the following tags are reserved for additional types
        -- of presentation attributes:
            -- [3] composite-graphics
            -- [4] audio
            -- [5] dynamic-graphics

ext-cont-arch-attributes            [6] IMPLICIT SEQUENCE OF EXTERNAL OPTIONAL}

Content-Type                    ::= [APPLICATION 2] IMPLICIT INTEGER {
                                        formatted-characters (0),
                                        formatted-raster-graphics (1)}

Content-Architecture-Class      ::= OBJECT IDENTIFIER

Layout-Style-Descriptor [[ P struct isode2toolkit_t * ]]

%{

    CHAR_type *LayoutStyleIDChars;
    STRING_type LayoutStyleIDString;

    CHAR_type *CommentChars;
    INT_type CommentLength;
    STRING_type CommentString;

    CHAR_type *NameChars;
    INT_type NameLength;
    STRING_type NameString;

%}

::=

%{
    parm->constituent = MakeStyle( parm->document, LAYOUT_STYLE );
    if ( parm->constituent == ERROR_CONSTITUENT ) {
	dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD6_Layout__Style__Descriptor]: can not make style");
	dummy = TKERR_EndErrMsg();
	return( NOTOK );
    }
%}

SET {

    style-identifier

	Style-Identifier [[ s LayoutStyleIDChars ]]

	%{
	    LayoutStyleIDString = Chars2String(LayoutStyleIDChars);
	    if ( LayoutStyleIDString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD6_Layout__Style__Descriptor]: can not make string");
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    if ( SetAttr( parm->constituent,
			  at_LAYOUT_STYLE_IDENTIFIER,
			  (POINTER_type) LayoutStyleIDString,
			  /* mask */ (PARM_MASK_type *) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD6_Layout__Style__Descriptor]: can not set attribute at_LAYOUT_STYLE_IDENTIFIER" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%},

    user-readable-comments

	[0] IMPLICIT OCTET STRING [[ o CommentChars $ CommentLength ]]

	%{
	    CommentString = MakeString(CommentLength);
	    if ( CommentString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD6_Layout__Style__Descriptor]: can not make string");
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    CommentString->sequence_value.chars = CommentChars;
	    if ( SetAttr( parm->constituent,
			  at_USER_READABLE_COMMENTS,
			  (POINTER_type) CommentString,
			  /* mask */ (PARM_MASK_type *) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD6_Layout__Style__Descriptor]: can not set attribute at_USER_READABLE_COMMENTS" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    user-visible-name

	[1] IMPLICIT OCTET STRING [[ o NameChars $ NameLength ]]

	%{
	    NameString = MakeString(NameLength);
	    if ( NameString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD6_Layout__Style__Descriptor]: can not make string");
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    NameString->sequence_value.chars = NameChars;
	    if ( SetAttr( parm->constituent,
			  at_USER_VISIBLE_NAME,
			  (POINTER_type) NameString,
			  /* mask */ (PARM_MASK_type *) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD6_Layout_Style__Descriptor]: can not set attribute at_USER_VISIBLE_NAME" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    layout-directives

	[4] IMPLICIT Layout-Directives

	OPTIONAL
}

%{
    if ( GetAttr( parm->constituent,
		  at_LAYOUT_STYLE_IDENTIFIER,
		  (POINTER_type) &LayoutStyleIDString,
		  /* default */ BOOL_false,
		  /* mask */ (PARM_MASK_type *) 0
	        ) == ERROR_INT ) {
	return ( NOTOK );
    } else {
	if ( ISODE_FillInConstituent ( parm->constituent,
				       LayoutStyleIDString
				     ) == ERROR_INT ) {
	    return ( NOTOK );
	}
    }
%}


Layout-Directives               ::= SET {
    indivisibility                  CHOICE{
        to-layout-object-class      [0] IMPLICIT Object-or-Class-Identifier,
        to-layout-category          [1] IMPLICIT Layout-Category-Name,
        to-layout-object-type       [2] IMPLICIT Layout-Object-Type,
        null                        [15] IMPLICIT NULL} OPTIONAL,
    separation                      [3] IMPLICIT Separation OPTIONAL,
    offset                          [4] IMPLICIT Offset OPTIONAL,
    fill-order                      [5] IMPLICIT Fill-Order OPTIONAL,
    concatenation                   [6] IMPLICIT Concatenation OPTIONAL,
    new-layout-object              CHOICE {
        to-layout-object-class      [7] IMPLICIT Object-or-Class-Identifier,
        to-layout-category          [8] IMPLICIT Layout-Category-Name,
        to-layout-object-type       [9] IMPLICIT Layout-Object-Type,
        null                        [16] IMPLICIT NULL} OPTIONAL,
    same-layout-object              [10] IMPLICIT Same-Layout-Object OPTIONAL,
    layout-object-class             [11] IMPLICIT Object-or-Class-Identifier
                                                                      OPTIONAL,
    layout-category                 [12] IMPLICIT Layout-Category-Name OPTIONAL,
    synchronization                 CHOICE {
                                    [13] IMPLICIT Object-or-Class-Identifier,
                                    [17] Object-Id-Expression, 
        			    [18] IMPLICIT NULL} OPTIONAL,
    block-alignment                 [14] IMPLICIT Block-Alignment OPTIONAL 
   }



 
Separation                      ::= SET {
    leading                         [0] IMPLICIT INTEGER DEFAULT 0,
    trailing                        [1] IMPLICIT INTEGER DEFAULT 0,
    centre                          [2] IMPLICIT INTEGER DEFAULT 0}

Offset                          ::= SET {
    right-hand                      [0] IMPLICIT INTEGER DEFAULT 0,
    left-hand                       [1] IMPLICIT INTEGER DEFAULT 0,
    trailing                        [2] IMPLICIT INTEGER DEFAULT 0,
    leading                         [3] IMPLICIT INTEGER DEFAULT 0}

Fill-Order                      ::= INTEGER {normal (0), reverse (1)}

Concatenation                   ::= INTEGER {non-concatenated (0),
                                                           concatenated (1)}

Same-Layout-Object              ::= SET {
    to-logical-object               CHOICE {
                                    [0] IMPLICIT Object-or-Class-Identifier,
                                    [4] Object-Id-Expression,
        			    [5] IMPLICIT NULL},
				    CHOICE {
    to-layout-object-class          [1] IMPLICIT Object-or-Class-Identifier,
    to-layout-category              [2] IMPLICIT Layout-Category-Name,
    to-layout-object-type           [3] IMPLICIT Layout-Object-Type} OPTIONAL}


Block-Alignment                 ::= INTEGER {
                                        right-hand (0), left-hand (1),
                                        centred (2), null (3)}


END 
