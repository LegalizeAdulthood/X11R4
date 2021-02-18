
ISRD5 DEFINITIONS ::=


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
#define TK_TKI_MM

#include <odatk.h>

#undef TK_TKI_TKERR
#undef TK_TKI_APIRT
#undef TK_TKI_MM

#define TK_IMPL_ISRD

#include <isrd.h>

#undef TK_IMPL_ISRD

#define PEPYPARM struct isode2toolkit_t *

INT_type dummy;

%}


BEGIN


EXPORTS Logical-Object-Descriptor, Logical-Class-Descriptor,
	Logical-Object-Type, Protection;


IMPORTS Object-or-Class-Identifier, Style-Identifier,
	Resource-Name, Construction-Expression,String-Expression
        FROM ISRD3
	Comment-String, Binding-Pair
        FROM ISRD4 
        Presentation-Attributes
	FROM ISRD6
        Default-Value-Lists-Logical
        FROM ISRD7;


DECODER parse


Logical-Object-Descriptor [[ P struct isode2toolkit_t * ]]

%{
    INT_type AnInt;
    VALUE_type ObjectType;

    STRING_type ObjectIDString;
%}

::= SEQUENCE {

    object-type

	Logical-Object-Type [[ i AnInt ]]

	%{
	    switch( AnInt ){
		case 0:
		    ObjectType = at_OBJECT_TYPE_doc_logical_root;
		    break;
		case 1:
		    ObjectType = at_OBJECT_TYPE_comp_logical_obj;
		    break;
		case 2:
		    ObjectType = at_OBJECT_TYPE_bas_logical_obj;
		    break;
	    }
	    parm->constituent =
		    MakeComponent( parm->document, 
				   ObjectType,
				   SPECIFIC_COMPONENT );
	    if( parm->constituent == ERROR_CONSTITUENT ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor]: can not make the component" );
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
	    }
	%}

	OPTIONAL,

    descriptor-body

	Logical-Object-Descriptor-Body [[ p parm ]]

	OPTIONAL

}

%{
    if ( GetAttr( parm->constituent,
		  at_OBJECT_IDENTIFIER,
		  (POINTER_type) &ObjectIDString,
		  /* default */ BOOL_false,
		  /* mask */ (PARM_MASK_type *) 0
	        ) == ERROR_INT ) {
	return ( NOTOK );
    } else {
	if ( ISODE_FillInConstituent ( parm->constituent,
				       ObjectIDString
				     ) == ERROR_INT ) {
	    return ( NOTOK );
	}
    }
%}


Logical-Object-Type

::= INTEGER {

    document-logical-root (0),
    composite-logical-object (1),
    basic-logical-object (2)

}


Logical-Object-Descriptor-Body [[ P struct isode2toolkit_t * ]]

%{
    CHAR_type *ObjectIDChars;
    STRING_type ObjectIDString;

    CHAR_type *CommentChars;
    INT_type CommentLength;
    STRING_type CommentString;

    CHAR_type *NameChars;
    INT_type NameLength;
    STRING_type NameString;

    CHAR_type *ObjectClassIDChars;
    STRING_type ObjectClassIDString;
    struct hash_t *ObjectClassBucket;
    CONSTITUENT_type ObjectClass;

    CHAR_type *PresentStyleIDChars;
    STRING_type PresentStyleIDString;
    struct hash_t *PresentStyleBucket;
    CONSTITUENT_type PresentStyle;

    CHAR_type *LayoutStyleIDChars;
    STRING_type LayoutStyleIDString;
    struct hash_t *LayoutStyleBucket;
    CONSTITUENT_type LayoutStyle;

    INT_type Protection;
    VALUE_type ProtectionValue;

    STRING_type ParentID;

    CHAR_type *SubordinatesIDChars;
    STRING_type SubordinatesIDString;
    SEQUENCE_STRING_type SubordinatesIDSequence;
    SEQUENCE_CONSTITUENT_type Subordinates;

    CHAR_type *ContentIDChars;
    STRING_type ContentIDString;
    SEQUENCE_STRING_type ContentIDSequence;
    SEQUENCE_CONSTITUENT_type ContentPortions;

    INT_type ApplCommentLength;
    CHAR_type *ApplCommentChars;
    STRING_type ApplCommentString;
    SEQUENCE_BYTE_type ApplComment;
%}

::=  SET {

    object-identifier

	Object-or-Class-Identifier [[ s ObjectIDChars ]]

	%{
	    ObjectIDString = Chars2String(ObjectIDChars);
	    if ( ObjectIDString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not convert object-identifier to a string" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    if ( SetAttr( parm->constituent,
			  at_OBJECT_IDENTIFIER,
			  (POINTER_type) ObjectIDString,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not set attribute at_OBJECT_IDENTIFIER" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    subordinates

	%{
	    SubordinatesIDSequence =
		MakeSequence( SEQUENCE_STRING_tag, (INT_type) 0 );
	    if ( SubordinatesIDSequence == ERROR_SEQUENCE ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not make sequence" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	[0] IMPLICIT SEQUENCE OF

	    NumericString [[ s SubordinatesIDChars ]]

	    %{
		SubordinatesIDString = Chars2String(SubordinatesIDChars);
		if ( SubordinatesIDString == ERROR_STRING ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not convert subordinates id chars to string" );
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
		}
		if ( ExpandSequence(SubordinatesIDSequence,
				    (INT_type) 1) == ERROR_INT ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not expand subordinates id string" );
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
		}
		SubordinatesIDSequence ->
		    sequence_value.strings[
		      SubordinatesIDSequence->length-1] = SubordinatesIDString;
	    %}

	%{
	    if ( GetAttr( parm->constituent,
			  at_OBJECT_IDENTIFIER,
			  (POINTER_type) &ParentID,
			  /* default */ BOOL_false,
			  /* mask */ (PARM_MASK_type *) 0
			) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not get parent id for subordinates");
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    Subordinates =
		ISODE_MakeSequenceConstituent( ParentID,
					       SubordinatesIDSequence,
					       parm->document );
	    if ( Subordinates == ERROR_SEQUENCE ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not convert subordinates ids to constituents" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    if ( SetAttr( parm->constituent,
			  at_SUBORDINATES,
			  (POINTER_type) Subordinates,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not set attribute at_SUBORDINATES" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    content-portions

	%{
	    ContentIDSequence =
		MakeSequence( SEQUENCE_STRING_tag, (INT_type) 0 );
	    if ( ContentIDSequence == ERROR_SEQUENCE ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not make sequence" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	[1] IMPLICIT SEQUENCE OF

	    NumericString [[ s ContentIDChars ]]

	    %{
		ContentIDString = Chars2String(ContentIDChars);
		if ( ContentIDString == ERROR_STRING ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not convert content id chars to string" );
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
		}
		if ( ExpandSequence(ContentIDSequence,
				    (INT_type) 1) == ERROR_INT ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not expand content id string" );
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
		}
		ContentIDSequence ->
		    sequence_value.strings[
		      ContentIDSequence->length-1] = ContentIDString;
	    %}

	%{
	    if ( GetAttr( parm->constituent,
			  at_OBJECT_IDENTIFIER,
			  (POINTER_type) &ParentID,
			  /* default */ BOOL_false,
			  /* mask */ (PARM_MASK_type *) 0
			) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not get parent id for subordinates");
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    ContentPortions =
		ISODE_MakeSequenceConstituent( ParentID,
					       ContentIDSequence,
					       parm->document );
	    if ( ContentPortions == ERROR_SEQUENCE ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not convert content ids to constituents" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    if ( SetAttr( parm->constituent,
			  at_CONTENT_PORTIONS,
			  (POINTER_type) ContentPortions,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not set attribute at_CONTENT_PORTIONS" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    object-class

	[2] IMPLICIT Object-or-Class-Identifier [[ s ObjectClassIDChars ]]

	%{
	    ObjectClassIDString = Chars2String(ObjectClassIDChars);
	    if ( ObjectClassIDString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not convert object-class-identifier to a string" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    ObjectClassBucket =
		ISODE_GetHashStructure(ObjectClassIDString,parm->document);
	    if ( ObjectClassBucket == ERROR_BUCKET ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not get hash structure" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    ObjectClass = ObjectClassBucket->constituent;
	    if ( SetAttr( parm->constituent,
			  at_OBJECT_CLASS,
			  (POINTER_type) ObjectClass,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not set attribute at_OBJECT_CLASS" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    presentation-attributes

	[6] IMPLICIT Presentation-Attributes

	%{

	%}

	OPTIONAL,

    default-value-lists

	[7] IMPLICIT Default-Value-Lists-Logical

	%{

	%}

	OPTIONAL,

    user-readable-comments

	[8] IMPLICIT Comment-String [[ o CommentChars $ CommentLength ]]

	%{
	    CommentString = MakeString(CommentLength);
	    if ( CommentString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor]: can not make string");
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    CommentString->sequence_value.chars = CommentChars;
	    if ( SetAttr( parm->constituent,
			  at_USER_READABLE_COMMENTS,
			  (POINTER_type) CommentString,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not set attribute at_USER_READABLE_COMMENTS" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    bindings

	[9] IMPLICIT SET OF Binding-Pair

	%{

	%}

	OPTIONAL,

    content-generator

	[10] IMPLICIT String-Expression

	%{

	%}

	OPTIONAL,

    user-visible-name

	[14] IMPLICIT Comment-String [[ o NameChars $ NameLength ]]

	%{
	    NameString = MakeString(NameLength);
	    if ( NameString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor]: can not make string");
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    NameString->sequence_value.chars = NameChars;
	    if ( SetAttr( parm->constituent,
			  at_USER_VISIBLE_NAME,
			  (POINTER_type) NameString,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not set attribute at_USER_VISIBLE_NAME" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    presentation-style

	[17] IMPLICIT Style-Identifier [[ s PresentStyleIDChars ]]

	%{
	    PresentStyleIDString = Chars2String(PresentStyleIDChars);
	    if ( PresentStyleIDString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not convert presentation-style-identifier to a string" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    PresentStyleBucket =
		ISODE_GetHashStructure(PresentStyleIDString,parm->document);
	    if ( PresentStyleBucket == ERROR_BUCKET ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not get hash structure" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    PresentStyle = PresentStyleBucket->constituent;
	    if ( SetAttr( parm->constituent,
			  at_PRESENTATION_STYLE,
			  (POINTER_type) PresentStyle,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not set attribute at_PRESENTATION_STYLE" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    layout-style

	[19] IMPLICIT Style-Identifier [[ s LayoutStyleIDChars ]]

	%{
	    LayoutStyleIDString = Chars2String(LayoutStyleIDChars);
	    if ( LayoutStyleIDString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not convert layout-style-identifier to a string" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    LayoutStyleBucket =
		ISODE_GetHashStructure(LayoutStyleIDString,parm->document);
	    if ( LayoutStyleBucket == ERROR_BUCKET ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not get hash structure" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    LayoutStyle = LayoutStyleBucket->constituent;
	    if ( SetAttr( parm->constituent,
			  at_LAYOUT_STYLE,
			  (POINTER_type) LayoutStyle,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not set attribute at_LAYOUT_STYLE" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    protection

	[20] IMPLICIT Protection [[ i Protection ]]

	%{
	    switch ( Protection ) {
		case 0:
		    ProtectionValue = at_PROTECTION_unprotected;
		    break;
		case 1:
		    ProtectionValue = at_PROTECTION_protected;
		    break;
	    }
	    if ( SetAttr( parm->constituent,
			  at_PROTECTION,
			  (POINTER_type) &ProtectionValue,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not set attribute at_PROTECTION" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    application-comments

	[25] IMPLICIT Comment-String
	    [[ o ApplCommentChars $ ApplCommentLength ]]

	%{
	    ApplCommentString = MakeString(ApplCommentLength);
	    if ( ApplCommentString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not make string for attribute at_APPLICATION_COMMENTS" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    ApplCommentString->sequence_value.chars = ApplCommentChars;
	    ApplComment = String2Bytes(ApplCommentString);
	    if ( ApplComment == ERROR_SEQUENCE) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not make byte sequence for attribute at_APPLICATION_COMMENTS" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    if ( SetAttr( parm->constituent,
			  at_APPLICATION_COMMENTS,
			  (POINTER_type) ApplComment,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Object__Descriptor__Body]: can not set attribute at_CONTENT_INFORMATION" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL

}


Logical-Class-Descriptor [[ P struct isode2toolkit_t * ]]

%{

    INT_type AnInt;
    VALUE_type ObjectType;

    STRING_type ObjectClassIDString;

%}

::= SEQUENCE {

    object-type

	Logical-Object-Type [[ i AnInt ]]

	%{
	    switch ( AnInt ) {
		case 0:
		    ObjectType = at_OBJECT_TYPE_doc_logical_root;
		    break;
		case 1:
		    ObjectType = at_OBJECT_TYPE_comp_logical_obj;
		    break;
		case 2:
		    ObjectType = at_OBJECT_TYPE_bas_logical_obj;
		    break;
	    }
	    parm->constituent =
		    MakeComponent( parm->document, 
				   ObjectType,
				   GENERIC_COMPONENT );
	    if( parm->constituent == ERROR_CONSTITUENT ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor]: can not make the component" );
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
	    }

	%},

    descriptor-body

	Logical-Class-Descriptor-Body [[ p parm ]]

}

%{
    if ( GetAttr( parm->constituent,
		  at_OBJECT_CLASS_IDENTIFIER,
		  (POINTER_type) &ObjectClassIDString,
		  /* default */ BOOL_false,
		  /* mask */ (PARM_MASK_type *) 0
	        ) == ERROR_INT ) {
	return ( NOTOK );
    } else {
	if ( ISODE_FillInConstituent ( parm->constituent,
				       ObjectClassIDString
				     ) == ERROR_INT ) {
	    return ( NOTOK );
	}
    }
%}


Logical-Class-Descriptor-Body [[ P struct isode2toolkit_t * ]]

%{
    POINTER_type memp;

    CHAR_type *ObjectClassIDChars;
    STRING_type ObjectClassIDString;
    struct hash_t *ObjectClassBucket;
    CONSTITUENT_type ObjectClass;

    CHAR_type *CommentChars;
    INT_type CommentLength;
    STRING_type CommentString;

    CHAR_type *NameChars;
    INT_type NameLength;
    STRING_type NameString;

    CHAR_type *PresentStyleIDChars;
    STRING_type PresentStyleIDString;
    struct hash_t *PresentStyleBucket;
    CONSTITUENT_type PresentStyle;

    CHAR_type *LayoutStyleIDChars;
    STRING_type LayoutStyleIDString;
    struct hash_t *LayoutStyleBucket;
    CONSTITUENT_type LayoutStyle;

    INT_type Protection;
    VALUE_type ProtectionValue;

    CONSTRUCTION_EXPR_type *ConstructionExpr;

    STRING_type ParentID;

    CHAR_type *ContentIDChars;
    STRING_type ContentIDString;
    SEQUENCE_STRING_type ContentIDSequence;
    SEQUENCE_CONSTITUENT_type ContentPortions;

    INT_type ApplCommentLength;
    CHAR_type *ApplCommentChars;
    STRING_type ApplCommentString;
    SEQUENCE_BYTE_type ApplComment;

    CHAR_type *ResourceNameChars;
    STRING_type ResourceNameString;
%}

::= SET {

    object-class-identifier

	Object-or-Class-Identifier [[ s ObjectClassIDChars ]]

	%{
	    ObjectClassIDString = Chars2String(ObjectClassIDChars);
	    if ( ObjectClassIDString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not convert object-identifier to a string" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    if ( SetAttr( parm->constituent,
			  at_OBJECT_CLASS_IDENTIFIER,
			  (POINTER_type) ObjectClassIDString,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not set attribute at_OBJECT_IDENTIFIER" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%},

    generator-for-subordinates

	%{
	    memp = MM_Malloc(sizeof(CONSTRUCTION_EXPR_type));
	    if ( memp == ERROR_POINTER ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: out of memory" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    ConstructionExpr = (CONSTRUCTION_EXPR_type *) memp;
	    SaveThisDocument( parm->document );
	%}

	[0] Construction-Expression [[ p ConstructionExpr ]]

	%{
	    if ( SetAttr( parm->constituent,
			  at_GEN_FOR_SUBORDINATES,
			  (POINTER_type) ConstructionExpr,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not set attribute at_GEN_FOR_SUBORDINATES" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    content-portions

	%{
	    ContentIDSequence =
		MakeSequence( SEQUENCE_STRING_tag, (INT_type) 0 );
	    if ( ContentIDSequence == ERROR_SEQUENCE ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not make sequence" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	[1] IMPLICIT SEQUENCE OF

	    NumericString [[ s ContentIDChars ]]

	    %{
		ContentIDString = Chars2String(ContentIDChars);
		if ( ContentIDString == ERROR_STRING ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not convert content id chars to string" );
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
		}
		if ( ExpandSequence(ContentIDSequence,
				    (INT_type) 1) == ERROR_INT ) {
		    dummy = TKERR_StartErrMsg();
		    dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not expand content id string" );
		    dummy = TKERR_EndErrMsg();
		    return( NOTOK );
		}
		ContentIDSequence ->
		    sequence_value.strings[
		      ContentIDSequence->length-1] = ContentIDString;
	    %}

	%{
	    if ( GetAttr( parm->constituent,
			  at_OBJECT_IDENTIFIER,
			  (POINTER_type) &ParentID,
			  /* default */ BOOL_false,
			  /* mask */ (PARM_MASK_type *) 0
			) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not get parent id for subordinates");
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    ContentPortions =
		ISODE_MakeSequenceConstituent( ParentID,
					       ContentIDSequence,
					       parm->document );
	    if ( ContentPortions == ERROR_SEQUENCE ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not convert content ids to constituents" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    if ( SetAttr( parm->constituent,
			  at_CONTENT_PORTIONS,
			  (POINTER_type) ContentPortions,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not set attribute at_CONTENT_PORTIONS" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    presentation-attributes

	[6] IMPLICIT Presentation-Attributes

	%{

	%}

	OPTIONAL,

    default-value-lists

	[7] IMPLICIT Default-Value-Lists-Logical

	%{

	%}

	OPTIONAL,

    user-readable-comments

	[8] IMPLICIT Comment-String [[ o CommentChars $ CommentLength ]]

	%{
	    CommentString = MakeString(CommentLength);
	    if ( CommentString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class_Descriptor]: can not make string");
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    CommentString->sequence_value.chars = CommentChars;
	    if ( SetAttr( parm->constituent,
			  at_USER_READABLE_COMMENTS,
			  (POINTER_type) CommentString,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not set attribute at_USER_READABLE_COMMENTS" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    bindings

	[9] IMPLICIT SET OF Binding-Pair

	%{

	%}

	OPTIONAL,

    content-generator

	[10] IMPLICIT String-Expression

	%{

	%}

	OPTIONAL,

    user-visible-name

	[14] IMPLICIT Comment-String [[ o NameChars $ NameLength ]]

	%{
	    NameString = MakeString(NameLength);
	    if ( CommentString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor]: can not make string");
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    NameString->sequence_value.chars = NameChars;
	    if ( SetAttr( parm->constituent,
			  at_USER_VISIBLE_NAME,
			  (POINTER_type) NameString,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not set attribute at_USER_VISIBLE_NAME" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    presentation-style

	[17] IMPLICIT Style-Identifier [[ s PresentStyleIDChars ]]

	%{
	    PresentStyleIDString = Chars2String(PresentStyleIDChars);
	    if ( PresentStyleIDString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not convert presentation-style-identifier to a string" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    PresentStyleBucket =
		ISODE_GetHashStructure(PresentStyleIDString,parm->document);
	    if ( PresentStyleBucket == ERROR_BUCKET ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not get hash structure" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    PresentStyle = PresentStyleBucket->constituent;
	    if ( SetAttr( parm->constituent,
			  at_PRESENTATION_STYLE,
			  (POINTER_type) PresentStyle,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not set attribute at_PRESENTATION_STYLE" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    layout-style

	[19] IMPLICIT Style-Identifier [[ s LayoutStyleIDChars ]]

	%{
	    LayoutStyleIDString = Chars2String(LayoutStyleIDChars);
	    if ( LayoutStyleIDString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not convert layout-style-identifier to a string" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    LayoutStyleBucket =
		ISODE_GetHashStructure(LayoutStyleIDString,parm->document);
	    if ( LayoutStyleBucket == ERROR_BUCKET ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not get hash structure" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    LayoutStyle = LayoutStyleBucket->constituent;
	    if ( SetAttr( parm->constituent,
			  at_LAYOUT_STYLE,
			  (POINTER_type) LayoutStyle,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not set attribute at_LAYOUT_STYLE" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    protection

	[20] IMPLICIT Protection [[ i Protection ]]

	%{
	    switch ( Protection ) {
		case 0:
		    ProtectionValue = at_PROTECTION_unprotected;
		    break;
		case 1:
		    ProtectionValue = at_PROTECTION_protected;
		    break;
	    }
	    if ( SetAttr( parm->constituent,
			  at_PROTECTION,
			  (POINTER_type) &ProtectionValue,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not set attribute at_PROTECTION" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    resource

	[24] IMPLICIT Resource-Name [[ s ResourceNameChars ]]

	%{
	    ResourceNameString = Chars2String(ResourceNameChars);
	    if ( ResourceNameString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not convert resource-name to a string" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    if ( SetAttr( parm->constituent,
			  at_RESOURCE,
			  (POINTER_type) ResourceNameString,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not set attribute at_RESOURCE" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL,

    application-comments

	[25] IMPLICIT Comment-String
	    [[ o ApplCommentChars $ ApplCommentLength ]]

	%{
	    ApplCommentString = MakeString(ApplCommentLength);
	    if ( ApplCommentString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not make string for attribute at_APPLICATION_COMMENTS" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    ApplCommentString->sequence_value.chars = ApplCommentChars;
	    ApplComment = String2Bytes(ApplCommentString);
	    if ( ApplComment == ERROR_SEQUENCE) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not make byte sequence for attribute at_APPLICATION_COMMENTS" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    if ( SetAttr( parm->constituent,
			  at_APPLICATION_COMMENTS,
			  (POINTER_type) ApplComment,
			  /* mask */ (PARM_MASK_type) 0
		        ) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD5_Logical__Class__Descriptor__Body]: can not set attribute at_CONTENT_INFORMATION" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	OPTIONAL

}


Protection

::= INTEGER {

    unprotected (0),
    protected (1)

}


END
