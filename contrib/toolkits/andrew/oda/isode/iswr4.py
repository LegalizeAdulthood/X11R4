
ISWR4 DEFINITIONS ::=

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


-- EXPORTS Layout-Object-Descriptor, Layout-Class-Descriptor,
--	Transparency, Comment-String, Layout-Object-Type,
--	Binding-Pair, One-Of-Four-Angles, Measure-Pair,
--	Medium-Type, Colour, Border;


IMPORTS Object-or-Class-Identifier, Style-Identifier,
	Layout-Category-Name, Resource-Name, Binding-Name,
	Construction-Expression, Object-Id-Expression,
	Numeric-Expression, String-Expression
	FROM ISWR3
	Presentation-Attributes
	FROM ISWR6
	Default-Value-Lists-Layout
	FROM ISWR7;


ENCODER build


Position-Spec			 ::= SET {
   offset			    [0] IMPLICIT SET {
	leading				[0] IMPLICIT INTEGER DEFAULT 0,
	trailing			[1] IMPLICIT INTEGER DEFAULT 0,
	left-hand			[2] IMPLICIT INTEGER DEFAULT 0,
	right-hand			[3] IMPLICIT INTEGER DEFAULT 0} OPTIONAL,
   separation			    [1] IMPLICIT SET {
	leading				[0] IMPLICIT INTEGER DEFAULT 0,
	trailing			[1] IMPLICIT INTEGER DEFAULT 0,
	centre				[2] IMPLICIT INTEGER DEFAULT 0} OPTIONAL,
   alignment			    [2] IMPLICIT INTEGER { 
					      right-hand (0),
					      centred    (1),
					      left-hand  (2)} DEFAULT 0,
   fill-order			    [3] IMPLICIT INTEGER {
					      normal (0), 
					      reverse (1)} DEFAULT 0 }
	

Dimension-Pair                   ::= SEQUENCE {
    horizontal                      [0] IMPLICIT INTEGER,
    vertical                        CHOICE {
	fixed				[0] IMPLICIT INTEGER,
	variable			[1] IMPLICIT INTEGER }}


Dimension-Spec			 ::= SEQUENCE {
    horizontal			     Dimension,
    vertical			     Dimension}


Dimension                        ::= CHOICE  {
    fixed                           [0] IMPLICIT INTEGER,
    rule-a                          [1] IMPLICIT SET {
        minimum-a                        [0] IMPLICIT INTEGER DEFAULT 0,
        maximum-a                        [1] IMPLICIT INTEGER DEFAULT 0},
    rule-b                          [2] IMPLICIT SET {
        minimum-b                        [0] IMPLICIT INTEGER DEFAULT 0,
        maximum-b                        [1] IMPLICIT INTEGER DEFAULT 0},
    maximum-size		     [3] IMPLICIT NULL }

Transparency                     ::= INTEGER {transparent (0), opaque (1)}

Comment-String                   ::= OCTET STRING

        -- string of characters from the sets designated by
        -- the document profile attribute "comments character sets",
        -- plus carriage return and line feed

Binding-Pair                     ::= SET {
    binding-identifier              [0] IMPLICIT Binding-Name,
    binding-value                   CHOICE {
                                    [1] Object-Id-Expression,
                                    [2] Numeric-Expression,
                                    [3] String-Expression,
				     [4] IMPLICIT Object-or-Class-Identifier,
				     [5] IMPLICIT INTEGER,
				     [6] IMPLICIT OCTET STRING }}

One-Of-Four-Angles		::= INTEGER { d0 (0), d90 (1), d180 (2), d270 (3) }

Measure-Pair                    ::= SEQUENCE {
    horizontal                      [0] IMPLICIT INTEGER,
    vertical                        [0] IMPLICIT INTEGER}


Medium-Type                     ::= SEQUENCE {
    nominal-page-size               Measure-Pair OPTIONAL,
    side-of-sheet                   INTEGER {unspecified (0), recto (1),
                                            verso (2)} OPTIONAL}

Colour                          ::= INTEGER {colourless (0), white (1)}

Border                          ::= SET {
    left-hand-edge		    [0] IMPLICIT Border-Edge OPTIONAL,
    right-hand-edge		    [1] IMPLICIT Border-Edge OPTIONAL,
    trailing-edge		    [2] IMPLICIT Border-Edge OPTIONAL,
    leading-edge		    [3] IMPLICIT Border-Edge OPTIONAL}

Border-Edge			 ::= SET {
    line-width                      [0] IMPLICIT INTEGER DEFAULT 10,
    line-type                       [1] IMPLICIT INTEGER { 
						invisible (0), 
						solid (1),
 						dashed (2), 
						dot (3), 
						dash-dot (4), 
						dash-dot-dot (5)} DEFAULT 1,
    freespace-width                 [2] IMPLICIT INTEGER DEFAULT 10}


Layout-Object-Descriptor [[ P struct isode2toolkit_t * ]]

%{
    VALUE_type ObjectType;
    INT_type BigObjectType;
%}

::= SEQUENCE {

    object-type

	%{
	    switch( ObjectType ){
		case at_OBJECT_TYPE_doc_layout_root:
		    BigObjectType = (INT_type) 0;
		    break;
		case at_OBJECT_TYPE_page_set:
		    BigObjectType = (INT_type) 1;
		    break;
		case at_OBJECT_TYPE_page:
		    BigObjectType = (INT_type) 2;
		    break;
		case at_OBJECT_TYPE_frame:
		    BigObjectType = (INT_type) 3;
		    break;
		case at_OBJECT_TYPE_block:
		    BigObjectType = (INT_type) 4;
		    break;
		default:
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR4__Layout__Object__Descriptor]: bad object type" );
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
	    }
	%}

	Layout-Object-Type [[ i BigObjectType ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_OBJECT_TYPE,
			(POINTER_type) &ObjectType,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    descriptor-body

	Layout-Object-Descriptor-Body [[ p parm ]]

	-- While the body can be optional,
	-- having an OPTIONAL clause here
	-- will cause the body to be overlooked
	-- entirely.  This clause is therefore
	-- left out here, and the OPTIONAL
	-- clauses for each part of the body
	-- are used instead.  This way, the body
	-- can still be optional while allowing
	-- fine control over how each part of the
	-- body can be optional.

}


Layout-Object-Type

::= INTEGER {

    document-layout-root (0),
    page-set (1),
    page (2),
    frame (3),
    block (4)

}


Layout-Object-Descriptor-Body [[ P struct isode2toolkit_t * ]]

%{

    STRING_type ObjectIDString;
    CHAR_type *ObjectIDChars;

    STRING_type CommentString;
    CHAR_type *CommentChars;
    INT_type CommentLength;

    STRING_type NameString;
    CHAR_type *NameChars;
    INT_type NameLength;

    CONSTITUENT_type ObjectClass;
    STRING_type ObjectClassIDString;
    CHAR_type *ObjectClassIDChars;

    CONSTITUENT_type PresentStyle;
    STRING_type PresentStyleIDString;
    CHAR_type *PresentStyleIDChars;

    INT_type count;

    SEQUENCE_CONSTITUENT_type Subordinates;
    STRING_type SubordinatesIDString;
    STRING_type LastSubordinatesIDDigits;
    CHAR_type *SubordinatesIDChars;

    SEQUENCE_CONSTITUENT_type ContentPortions;
    STRING_type ContentIDString;
    STRING_type LastContentIDDigits;
    CHAR_type *ContentIDChars;

    SEQUENCE_BYTE_type ApplCommentBytes;
    CHAR_type *ApplCommentChars;
    INT_type ApplCommentLength;
%}

::= SET {

    object-identifier

	%{
	    ObjectIDChars = String2Chars(ObjectIDString);
	    if ( ObjectIDChars == ERROR_CHARS ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR4_Layout__Object__Descriptor__Body]: can not convert string to object-identifier chars" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	Object-or-Class-Identifier [[ s ObjectIDChars ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_OBJECT_IDENTIFIER,
			(POINTER_type) &ObjectIDString,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    subordinates

	[0] IMPLICIT SEQUENCE OF

	    %{
		if ( GetAttr( Subordinates->sequence_value.constituents[count],
			      at_OBJECT_IDENTIFIER,
			      (POINTER_type) &SubordinatesIDString,
			      /* default */ BOOL_false,
			      /* mask */ (PARM_MASK_type *) 0
			    ) == ERROR_INT ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR4_Layout__Object__Descriptor__Body]: can not get attribute at_OBJECT_IDENTIFIER" );
		    dummy = TKERR_EndErrMsg();
		    return ( NOTOK );
		}
		LastSubordinatesIDDigits =
		    GetLastIDDigits(SubordinatesIDString);
		if ( LastSubordinatesIDDigits == ERROR_STRING ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR4_Layout__Object__Descriptor__Body]: can not get last ID digits");
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
		}
		SubordinatesIDChars = String2Chars(LastSubordinatesIDDigits);
		if ( SubordinatesIDChars == ERROR_CHARS ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR4_Layout__Object__Descriptor__Body]: can not convert string to subordinates id chars" );
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
		}
	    %}

	    << count=0; count<Subordinates->length; count++ >>

	NumericString [[ s SubordinatesIDChars ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_SUBORDINATES,
			(POINTER_type) &Subordinates,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    content-portions

	[1] IMPLICIT SEQUENCE OF

	    %{
		if ( GetAttr( ContentPortions ->
				  sequence_value.constituents[count],
			      at_CONTENT_ID_LOGICAL,
			      (POINTER_type) &ContentIDString,
			      /* default */ BOOL_false,
			      /* mask */ (PARM_MASK_type *) 0
			    ) == ERROR_INT ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR4_Layout__Object__Descriptor__Body]: can not get attribute CONTENT_ID_LOGICAL" );
		    dummy = TKERR_EndErrMsg();
		    return ( NOTOK );
		}
		LastContentIDDigits =
		    GetLastIDDigits(ContentIDString);
		if ( LastContentIDDigits == ERROR_STRING ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR4_Layout__Object__Descriptor__Body]: can not get last ID digits");
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
		}
		ContentIDChars = String2Chars(LastContentIDDigits);
		if ( ContentIDChars == ERROR_CHARS ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR4_Layout__Object__Descriptor__Body]: can not convert string to content id chars" );
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
		}
	    %}

	    << count=0; count<ContentPortions->length; count++ >>

	NumericString [[ s ContentIDChars ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_CONTENT_PORTIONS,
			(POINTER_type) &ContentPortions,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    object-class

	%{
	    if ( GetAttr( ObjectClass,
			  at_OBJECT_CLASS_IDENTIFIER,
			  (POINTER_type) &ObjectClassIDString,
			  /* default */ BOOL_false,
			  /* mask */ (PARM_MASK_type *) 0
			) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR4_Layout__Object__Descriptor__Body]: can not get attribute at_OBJECT_CLASS_IDENTIFIER" );
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    ObjectClassIDChars = String2Chars(ObjectClassIDString);
	    if ( ObjectClassIDChars == ERROR_CHARS ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR4_Layout__Object__Descriptor__Body]: can not convert string to object-class-identifier chars" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	[2] IMPLICIT Object-or-Class-Identifier [[ s ObjectClassIDChars ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_OBJECT_CLASS,
			(POINTER_type) &ObjectClass,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    position                        [3] IMPLICIT Measure-Pair OPTIONAL,
    dimensions                      [4] IMPLICIT Dimension-Pair OPTIONAL,
    transparency                    [5] IMPLICIT Transparency OPTIONAL,
    presentation-attributes         [6] IMPLICIT Presentation-Attributes
                                                                     OPTIONAL,
    default-value-lists             [7] IMPLICIT Default-Value-Lists-Layout
                                                                     OPTIONAL,

    user-readable-comments

	%{
	    CommentLength = CommentString->length;
	    CommentChars = CommentString->sequence_value.chars;
	%}

	[8] IMPLICIT Comment-String [[ o CommentChars $ CommentLength ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_USER_READABLE_COMMENTS,
			(POINTER_type) &CommentString,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    bindings                        [9] IMPLICIT SET OF Binding-Pair OPTIONAL,
    layout-path                     [11] IMPLICIT One-Of-Four-Angles OPTIONAL,
    imaging-order                   [12] IMPLICIT SEQUENCE OF NumericString
                                                                     OPTIONAL,
    permitted-categories            [13] IMPLICIT SET OF Layout-Category-Name
                                                                     OPTIONAL,

        -- a "null" value is represented by an empty set

    user-visible-name

	%{
	    NameLength = NameString->length;
	    NameChars = NameString->sequence_value.chars;
	%}

	[14] IMPLICIT Comment-String [[ o NameChars $ NameLength ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_USER_VISIBLE_NAME,
			(POINTER_type) &NameString,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    page-position                   [15] IMPLICIT Measure-Pair OPTIONAL,
    medium-type                     [16] IMPLICIT Medium-Type OPTIONAL,

    presentation-style

	%{
	    if ( GetAttr( PresentStyle,
			  at_PRESENTATION_STYLE_ID,
			  (POINTER_type) &PresentStyleIDString,
			  /* default */ BOOL_false,
			  /* mask */ (PARM_MASK_type *) 0
			) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR4_Layout__Object__Descriptor__Body]: can not get attribute at_PRESEMTATOPN_STYLE_ID" );
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    PresentStyleIDChars = String2Chars(PresentStyleIDString);
	    if ( PresentStyleIDChars == ERROR_CHARS ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR4_Layout__Object__Descriptor__Body]: can not convert string to presentation-style-identifier chars" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	[17] IMPLICIT Style-Identifier [[ s PresentStyleIDChars ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_PRESENTATION_STYLE,
			(POINTER_type) &PresentStyle,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    balance                         [21] IMPLICIT SET OF
                                           Object-or-Class-Identifier OPTIONAL,

        -- a "null" value is represented by an empty set

    colour                          [22] IMPLICIT Colour OPTIONAL,
    border                          [23] IMPLICIT Border OPTIONAL,

    application-comments

	%{
	    if ( Bytes2Chars( ApplCommentBytes,
			      &ApplCommentChars,
			      &ApplCommentLength
			    ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR4_Layout__Object__Descriptor__Body]: can not convert bytes to chars" );
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	%}

	[25] IMPLICIT Comment-String
	    [[ o ApplCommentChars $ ApplCommentLength ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_APPLICATION_COMMENTS,
			(POINTER_type) &ApplCommentBytes,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>

}


Layout-Class-Descriptor [[ P struct isode2toolkit_t * ]]

%{
    VALUE_type ObjectType;
    INT_type BigObjectType;
%}

::= SEQUENCE {

    object-type

	%{
	    if ( GetAttr( parm->constituent,
			  at_OBJECT_TYPE,
			  (POINTER_type) &ObjectType,
			  /* default */ BOOL_false,
			  /* mask */ (PARM_MASK_type *) 0
			) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR4_Layout__Class__Descriptor__Body]: can not get attribute at_OBJECT_TYPE" );
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    switch( ObjectType ){
		case at_OBJECT_TYPE_doc_layout_root:
		    BigObjectType = (INT_type) 0;
		    break;
		case at_OBJECT_TYPE_page_set:
		    BigObjectType = (INT_type) 1;
		    break;
		case at_OBJECT_TYPE_page:
		    BigObjectType = (INT_type) 2;
		    break;
		case at_OBJECT_TYPE_frame:
		    BigObjectType = (INT_type) 3;
		    break;
		case at_OBJECT_TYPE_block:
		    BigObjectType = (INT_type) 4;
		    break;
		default:
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR4__Layout__Class__Descriptor]: bad object type" );
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
	    }
	%}

	Layout-Object-Type [[ i BigObjectType ]],

    descriptor-body

	Layout-Class-Descriptor-Body [[ p parm ]]

}


Layout-Class-Descriptor-Body [[ P struct isode2toolkit_t * ]]

%{

    STRING_type CommentString;
    CHAR_type *CommentChars;
    INT_type CommentLength;

    STRING_type NameString;
    CHAR_type *NameChars;
    INT_type NameLength;

    STRING_type ObjectClassIDString;
    CHAR_type *ObjectClassIDChars;

    CONSTITUENT_type PresentStyle;
    STRING_type PresentStyleIDString;
    CHAR_type *PresentStyleIDChars;

    INT_type count;

    SEQUENCE_CONSTITUENT_type ContentPortions;
    STRING_type ContentIDString;
    STRING_type LastContentIDDigits;
    CHAR_type *ContentIDChars;

    CONSTRUCTION_EXPR_type *GenForSubordinates;

    SEQUENCE_BYTE_type ApplCommentBytes;
    CHAR_type *ApplCommentChars;
    INT_type ApplCommentLength;

    STRING_type ResourceNameString;
    CHAR_type *ResourceNameChars;
%}

::= SET {

    object-class-identifier

	%{
	    if ( GetAttr( parm->constituent,
			  at_OBJECT_CLASS_IDENTIFIER,
			  (POINTER_type) &ObjectClassIDString,
			  /* default */ BOOL_false,
			  /* mask */ (PARM_MASK_type *) 0
			) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR4_Layout__Class__Descriptor__Body]: can not get attribute at_OBJECT_CLASS_IDENTIFIER" );
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    ObjectClassIDChars = String2Chars(ObjectClassIDString);
	    if ( ObjectClassIDChars == ERROR_CHARS ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR4_Layout__Class__Descriptor__Body]: can not convert string to object-class-identifier chars" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	Object-or-Class-Identifier [[ s ObjectClassIDChars ]],

    generator-for-subordinates

	[0] Construction-Expression [[ p GenForSubordinates ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_GEN_FOR_SUBORDINATES,
			(POINTER_type) &GenForSubordinates,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    content-portions

	[1] IMPLICIT SEQUENCE OF

	    %{
		if ( GetAttr( ContentPortions ->
				  sequence_value.constituents[count],
			      at_CONTENT_ID_LOGICAL,
			      (POINTER_type) &ContentIDString,
			      /* default */ BOOL_false,
			      /* mask */ (PARM_MASK_type *) 0
			    ) == ERROR_INT ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR4_Layout__Class__Descriptor__Body]: can not get attribute CONTENT_ID_LOGICAL" );
		    dummy = TKERR_EndErrMsg();
		    return ( NOTOK );
		}
		LastContentIDDigits =
		    GetLastIDDigits(ContentIDString);
		if ( LastContentIDDigits == ERROR_STRING ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR4_Layout__Class__Descriptor__Body]: can not get last ID digits");
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
		}
		ContentIDChars = String2Chars(LastContentIDDigits);
		if ( ContentIDChars == ERROR_CHARS ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR4_Layout__Class__Descriptor__Body]: can not convert string to content id chars" );
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
		}
	    %}

	    << count=0; count<ContentPortions->length; count++ >>

	NumericString [[ s ContentIDChars ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_CONTENT_PORTIONS,
			(POINTER_type) &ContentPortions,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    position                       CHOICE {
    fixed-position		    [3] IMPLICIT Measure-Pair,
    variable-position		    [26] IMPLICIT Position-Spec} OPTIONAL,
    dimensions                     [4] IMPLICIT Dimension-Spec OPTIONAL,
    transparency                   [5] IMPLICIT Transparency OPTIONAL,
    presentation-attributes        [6] IMPLICIT Presentation-Attributes
                                                                      OPTIONAL,
    default-value-lists            [7] IMPLICIT Default-Value-Lists-Layout
                                                                      OPTIONAL,

    user-readable-comments

	%{
	    CommentLength = CommentString->length;
	    CommentChars = CommentString->sequence_value.chars;
	%}

	[8] IMPLICIT Comment-String [[ o CommentChars $ CommentLength ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_USER_READABLE_COMMENTS,
			(POINTER_type) &CommentString,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    bindings                       [9] IMPLICIT SET OF Binding-Pair OPTIONAL,
    content-generator              [10] IMPLICIT String-Expression OPTIONAL,
    layout-path                    [11] IMPLICIT One-Of-Four-Angles OPTIONAL,
    permitted-categories           [13] IMPLICIT SET OF Layout-Category-Name
                                                                      OPTIONAL,

        -- a "null" value is represented by an empty set

    user-visible-name

	%{
	    NameLength = NameString->length;
	    NameChars = NameString->sequence_value.chars;
	%}

	[14] IMPLICIT Comment-String [[ o NameChars $ NameLength ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_USER_VISIBLE_NAME,
			(POINTER_type) &NameString,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    page-position                  [15] IMPLICIT Measure-Pair OPTIONAL,
    medium-type                    [16] IMPLICIT Medium-Type OPTIONAL,

    presentation-style

	%{
	    if ( GetAttr( PresentStyle,
			  at_PRESENTATION_STYLE_ID,
			  (POINTER_type) &PresentStyleIDString,
			  /* default */ BOOL_false,
			  /* mask */ (PARM_MASK_type *) 0
			) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR4_Layout__Class__Descriptor__Body]: can not get attribute at_PRESENTATION_STYLE_ID" );
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    PresentStyleIDChars = String2Chars(PresentStyleIDString);
	    if ( PresentStyleIDChars == ERROR_CHARS ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR4_Layout__Class__Descriptor__Body]: can not convert string to presentation-style-identifier chars" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	[17] IMPLICIT Style-Identifier [[ s PresentStyleIDChars ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_PRESENTATION_STYLE,
			(POINTER_type) &PresentStyle,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    logical-source                 [18] IMPLICIT Object-or-Class-Identifier 
                                                                      OPTIONAL,
    balance                        [21] IMPLICIT SET OF
                                                 Object-or-Class-Identifier OPTIONAL,

        -- a "null" value is represented by an empty set

    colour                         [22] IMPLICIT Colour OPTIONAL,
    border                         [23] IMPLICIT Border OPTIONAL,

    resource

	%{
	    ResourceNameChars = String2Chars(ResourceNameString);
	    if ( ResourceNameChars == ERROR_CHARS ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR4_Layout__Class__Descriptor__Body]: can not convert string to resource-name chars" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	[24] IMPLICIT Resource-Name [[ s ResourceNameChars ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_RESOURCE,
			(POINTER_type) &ResourceNameString,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    application-comments

	%{
	    if ( Bytes2Chars( ApplCommentBytes,
			      &ApplCommentChars,
			      &ApplCommentLength
			    ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR4_Layout__Class__Descriptor__Body]: can not convert bytes to chars" );
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	%}

	[25] IMPLICIT Comment-String
	    [[ o ApplCommentChars $ ApplCommentLength ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_APPLICATION_COMMENTS,
			(POINTER_type) &ApplCommentBytes,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>

}


END
