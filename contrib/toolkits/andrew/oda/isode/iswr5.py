
ISWR5 DEFINITIONS ::=


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


EXPORTS Logical-Object-Descriptor, Logical-Class-Descriptor,
	Logical-Object-Type, Protection;


IMPORTS Object-or-Class-Identifier, Style-Identifier,
	Resource-Name, Construction-Expression,String-Expression
        FROM ISWR3
	Comment-String, Binding-Pair
        FROM ISWR4 
        Presentation-Attributes
	FROM ISWR6
        Default-Value-Lists-Logical
        FROM ISWR7;


ENCODER build


Logical-Object-Descriptor  [[ P struct isode2toolkit_t * ]]

%{

    VALUE_type ObjectType;
    INT_type BigObjectType;

%}

::= SEQUENCE {

    object-type

	%{
	    switch( ObjectType ){
		case at_OBJECT_TYPE_doc_logical_root:
		    BigObjectType = (INT_type) 0;
		    break;
		case at_OBJECT_TYPE_comp_logical_obj:
		    BigObjectType = (INT_type) 1;
		    break;
		case at_OBJECT_TYPE_bas_logical_obj:
		    BigObjectType = (INT_type) 2;
		    break;
		default:
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR5__Logical__Object__Descriptor]: bad object type" );
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
	    }
	%}

	Logical-Object-Type [[ i BigObjectType ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_OBJECT_TYPE,
			(POINTER_type) &ObjectType,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    descriptor-body

	Logical-Object-Descriptor-Body [[ p parm ]]

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


Logical-Object-Type

::= INTEGER {

    document-logical-root (0),
    composite-logical-object (1),
    basic-logical-object (2)

}


Logical-Object-Descriptor-Body [[ P struct isode2toolkit_t * ]]

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

    CONSTITUENT_type LayoutStyle;
    STRING_type LayoutStyleIDString;
    CHAR_type *LayoutStyleIDChars;

    VALUE_type ProtectionValue;
    INT_type Protection;

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

    STRING_type ResourceNameString;
    CHAR_type *ResourceNameChars;
%}

::=  SET {

    object-identifier

	%{
	    ObjectIDChars = String2Chars(ObjectIDString);
	    if ( ObjectIDChars == ERROR_CHARS ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR5_Logical__Object__Descriptor__Body]: can not convert string to object-identifier chars" );
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
		    dummy = TKERR_Format("[build_ISWR5_Logical__Object__Descriptor__Body]: can not get attribute at_OBJECT_IDENTIFIER" );
		    dummy = TKERR_EndErrMsg();
		    return ( NOTOK );
		}
		LastSubordinatesIDDigits =
		    GetLastIDDigits(SubordinatesIDString);
		if ( LastSubordinatesIDDigits == ERROR_STRING ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR5_Logical__Object__Descriptor__Body]: can not get last ID digits");
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
		}
		SubordinatesIDChars = String2Chars(LastSubordinatesIDDigits);
		if ( SubordinatesIDChars == ERROR_CHARS ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR5_Logical__Object__Descriptor__Body]: can not convert string to subordinates id chars" );
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
		    dummy = TKERR_Format("[build_ISWR5_Logical__Object__Descriptor__Body]: can not get attribute CONTENT_ID_LOGICAL" );
		    dummy = TKERR_EndErrMsg();
		    return ( NOTOK );
		}
		LastContentIDDigits =
		    GetLastIDDigits(ContentIDString);
		if ( LastContentIDDigits == ERROR_STRING ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR5_Logical__Object__Descriptor__Body]: can not get last ID digits");
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
		}
		ContentIDChars = String2Chars(LastContentIDDigits);
		if ( ContentIDChars == ERROR_CHARS ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR5_Logical__Object__Descriptor__Body]: can not convert string to content id chars" );
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
		dummy = TKERR_Format("[build_ISWR5_Logical__Object__Descriptor__Body]: can not get attribute at_OBJECT_CLASS_IDENTIFIER" );
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    ObjectClassIDChars = String2Chars(ObjectClassIDString);
	    if ( ObjectClassIDChars == ERROR_CHARS ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR5_Logical__Object__Descriptor__Body]: can not convert string to object-class-identifier chars" );
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

    presentation-attributes

	%{

	%}

	[6] IMPLICIT Presentation-Attributes

	OPTIONAL,

    default-value-lists

	%{

	%}

	[7] IMPLICIT Default-Value-Lists-Logical

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

    bindings

	%{

	%}

	[9] IMPLICIT SET OF Binding-Pair

	OPTIONAL,

    content-generator

	%{

	%}

	[10] IMPLICIT String-Expression

	OPTIONAL,

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

    presentation-style

	%{
	    if ( GetAttr( PresentStyle,
			  at_PRESENTATION_STYLE_ID,
			  (POINTER_type) &PresentStyleIDString,
			  /* default */ BOOL_false,
			  /* mask */ (PARM_MASK_type *) 0
			) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR5_Logical__Object__Descriptor__Body]: can not get attribute at_PRESEMTATOPN_STYLE_ID" );
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    PresentStyleIDChars = String2Chars(PresentStyleIDString);
	    if ( PresentStyleIDChars == ERROR_CHARS ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR5_Logical__Object__Descriptor__Body]: can not convert string to presentation-style-identifier chars" );
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

    layout-style

	%{
	    if ( GetAttr( LayoutStyle,
			  at_LAYOUT_STYLE_IDENTIFIER,
			  (POINTER_type) &LayoutStyleIDString,
			  /* default */ BOOL_false,
			  /* mask */ (PARM_MASK_type *) 0
			) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR5_Logical__Object__Descriptor__Body]: can not get attribute at_LAYOUT_STYLE_ID" );
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    LayoutStyleIDChars = String2Chars(LayoutStyleIDString);
	    if ( LayoutStyleIDChars == ERROR_CHARS ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR5_Logical__Object__Descriptor__Body]: can not convert string to layout-style-identifier chars" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	[19] IMPLICIT Style-Identifier [[ s LayoutStyleIDChars ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_LAYOUT_STYLE,
			(POINTER_type) &LayoutStyle,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    protection

	%{
	    switch( ProtectionValue ) {
		case at_PROTECTION_unprotected:
		    Protection = (INT_type) 0;
		    break;
		case at_PROTECTION_protected:
		    Protection = (INT_type) 1;
		    break;
		default:
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR5__Logical__Object__Descriptor]: bad protection value" );
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
	    }
	%}

	[20] IMPLICIT Protection [[ i Protection ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_PROTECTION,
			(POINTER_type) &ProtectionValue,
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
		dummy = TKERR_Format("[build_ISWR5_Logical__Object__Descriptor__Body]: can not convert bytes to chars" );
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


Logical-Class-Descriptor  [[ P struct isode2toolkit_t * ]]

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
		dummy = TKERR_Format("[build_ISWR5_Logical__Class__Descriptor__Body]: can not get attribute at_OBJECT_TYPE" );
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    switch( ObjectType ){
		case at_OBJECT_TYPE_doc_logical_root:
		    BigObjectType = (INT_type) 0;
		    break;
		case at_OBJECT_TYPE_comp_logical_obj:
		    BigObjectType = (INT_type) 1;
		    break;
		case at_OBJECT_TYPE_bas_logical_obj:
		    BigObjectType = (INT_type) 2;
		    break;
		default:
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR5__Logical__Class__Descriptor]: bad object type" );
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
	    }
	%}

	Logical-Object-Type [[ i BigObjectType ]],

    descriptor-body

	Logical-Class-Descriptor-Body [[ p parm ]]

}


Logical-Class-Descriptor-Body

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

    CONSTITUENT_type LayoutStyle;
    STRING_type LayoutStyleIDString;
    CHAR_type *LayoutStyleIDChars;

    VALUE_type ProtectionValue;
    INT_type Protection;

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
		dummy = TKERR_Format("[build_ISWR5_Logical__Class__Descriptor__Body]: can not get attribute at_OBJECT_CLASS_IDENTIFIER" );
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    ObjectClassIDChars = String2Chars(ObjectClassIDString);
	    if ( ObjectClassIDChars == ERROR_CHARS ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR5_Logical__Class__Descriptor__Body]: can not convert string to object-class-identifier chars" );
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
		    dummy = TKERR_Format("[build_ISWR5_Logical__Class__Descriptor__Body]: can not get attribute CONTENT_ID_LOGICAL" );
		    dummy = TKERR_EndErrMsg();
		    return ( NOTOK );
		}
		LastContentIDDigits =
		    GetLastIDDigits(ContentIDString);
		if ( LastContentIDDigits == ERROR_STRING ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR5_Logical__Class__Descriptor__Body]: can not get last ID digits");
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
		}
		ContentIDChars = String2Chars(LastContentIDDigits);
		if ( ContentIDChars == ERROR_CHARS ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR5_Logical__Class__Descriptor__Body]: can not convert string to content id chars" );
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

    presentation-attributes

	%{

	%}

	[6] IMPLICIT Presentation-Attributes

	OPTIONAL,

    default-value-lists

	%{

	%}

	[7] IMPLICIT Default-Value-Lists-Logical

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

    bindings

	%{

	%}

	[9] IMPLICIT SET OF Binding-Pair

	OPTIONAL,

    content-generator

	%{

	%}

	[10] IMPLICIT String-Expression

	OPTIONAL,

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

    presentation-style

	%{
	    if ( GetAttr( PresentStyle,
			  at_PRESENTATION_STYLE_ID,
			  (POINTER_type) &PresentStyleIDString,
			  /* default */ BOOL_false,
			  /* mask */ (PARM_MASK_type *) 0
			) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR5_Logical__Class__Descriptor__Body]: can not get attribute at_PRESENTATION_STYLE_ID" );
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    PresentStyleIDChars = String2Chars(PresentStyleIDString);
	    if ( PresentStyleIDChars == ERROR_CHARS ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR5_Logical__Class__Descriptor__Body]: can not convert string to presentation-style-identifier chars" );
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

    layout-style

	%{
	    if ( GetAttr( LayoutStyle,
			  at_LAYOUT_STYLE_IDENTIFIER,
			  (POINTER_type) &LayoutStyleIDString,
			  /* default */ BOOL_false,
			  /* mask */ (PARM_MASK_type *) 0
			) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR5_Logical__Class__Descriptor__Body]: can not get attribute at_LAYOUT_STYLE_IDENTIFIER" );
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    LayoutStyleIDChars = String2Chars(LayoutStyleIDString);
	    if ( LayoutStyleIDChars == ERROR_CHARS ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR5_Logical__Class__Descriptor__Body]: can not convert string to layout-style-identifier chars" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	[19] IMPLICIT Style-Identifier [[ s LayoutStyleIDChars ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_LAYOUT_STYLE,
			(POINTER_type) &LayoutStyle,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    protection

	%{
	    switch( ProtectionValue ) {
		case at_PROTECTION_unprotected:
		    Protection = (INT_type) 0;
		    break;
		case at_PROTECTION_protected:
		    Protection = (INT_type) 1;
		    break;
		default:
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[build_ISWR5__Logical_Class__Descriptor]: bad protection value" );
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
	    }
	%}

	[20] IMPLICIT Protection [[ i Protection ]]

	OPTIONAL
	    << GetAttr( parm->constituent,
			at_PROTECTION,
			(POINTER_type) &ProtectionValue,
			/* default */ BOOL_false,
			/* mask */ (PARM_MASK_type *) 0
		      ) != ERROR_INT >>,

    resource

	%{
	    ResourceNameChars = String2Chars(ResourceNameString);
	    if ( ResourceNameChars == ERROR_CHARS ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR5_Logical__Class__Descriptor__Body]: can not convert string to resource-name chars" );
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
		dummy = TKERR_Format("[build_ISWR5_Logical__Object__Descriptor__Body]: can not convert bytes to chars" );
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


Protection

::= INTEGER {

    unprotected (0),
    protected (1)

}


END
