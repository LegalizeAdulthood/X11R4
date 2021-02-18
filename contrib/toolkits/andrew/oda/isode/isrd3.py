
ISRD3 DEFINITIONS ::=


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


-- EXPORTS
--	Content-Portion-Identifier, Object-or-Class-Identifier,
--      Style-Identifier, Layout-Category-Name,
--      Resource-Name, Binding-Name,
--	Construction-Expression, Object-Id-Expression,
--	Numeric-Expression, String-Expresion;


IMPORTS 
	Layout-Object-Type
	FROM ISRD4
	Logical-Object-Type
        FROM ISRD5;


DECODER parse


Content-Portion-Identifier

::= [APPLICATION 0] IMPLICIT PrintableString

         -- only digits and space are used in the present version
         -- of the standard; other characters are reserved for extensions


Object-or-Class-Identifier

::= [APPLICATION 1] IMPLICIT PrintableString

         -- only digits and space are used in the present version
         -- of the standard; other characters are reserved for extensions;
         -- a "null" value is represented by an empty string


Style-Identifier

::= [APPLICATION 5] IMPLICIT PrintableString

         -- only digits and space are used in the present version
         -- of the standard; other characters are reserved for extensions
         -- a "null" value is represented by an empty string


Layout-Category-Name

::= PrintableString

         -- a "null" value is represented by empty string


Resource-Name

::= PrintableString


Binding-Name

::= PrintableString


Construction-Expression [[ P CONSTRUCTION_EXPR_type * ]]

::=

CHOICE {

    construction-type

	Construction-Type [[ p parm ]],

    single-term-construction

	[3] Construction-Term [[ p &(parm->construction_value.term) ]]

	%{
	    parm->construction_tag = TERM_tag;
	%}

}


Construction-Type [[ P CONSTRUCTION_EXPR_type * ]]

::=

%{

    parm->construction_value.construction =
	MakeSequence( SEQUENCE_CONSTRUCTION_TERM_tag, (INT_type) 0 );
    if ( parm->construction_value.construction == ERROR_SEQUENCE ) {
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[parse_ISRD3_Construction__Type]: can not make sequence for construction terms" );
	dummy = TKERR_EndErrMsg();
	return( NOTOK );
    }
%}

CHOICE {

    sequence-construction

	[0] IMPLICIT Term-Sequence
	    [[ p parm->construction_value.construction ]]

	%{
	    parm->construction_tag = SEQUENCE_tag;
	%},

    aggregate-construction

	[1] IMPLICIT Term-Sequence
	    [[ p parm->construction_value.construction ]]

	%{
	    parm->construction_tag = AGGREGATE_tag;
	%},

    choice-construction

	[2] IMPLICIT Term-Sequence
	    [[ p parm->construction_value.construction ]]

	%{
	    parm->construction_tag = CHOICE_tag;
	%}

}


Term-Sequence [[ P SEQUENCE_CONSTRUCTION_TERM_type ]]

%{
    POINTER_type memp;
    CONSTRUCTION_TERM_type *ConstructionTerm;
%}

::= SEQUENCE OF

    %{
	memp = MM_Malloc(sizeof(CONSTRUCTION_TERM_type));
	if ( memp == ERROR_POINTER ) {
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[parse_ISRD3_Term__Sequence]: out of memory" );
	    dummy = TKERR_EndErrMsg();
	    return( NOTOK );
	}
	ConstructionTerm = (CONSTRUCTION_TERM_type *) memp;
    %}

    Construction-Term [[ p ConstructionTerm ]]

    %{
	if ( ExpandSequence( parm, (INT_type) 1 ) == ERROR_INT ) {
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[parse_ISRD3_Term__Sequence]: can not expand sequence of construction terms" );
	    dummy = TKERR_EndErrMsg();
	    dummy = MM_Free( ConstructionTerm );
	    return( NOTOK );
	}
	parm->sequence_value.construction_terms[parm->length-1] =
	    *ConstructionTerm;
	dummy = MM_Free( ConstructionTerm );
    %}


Construction-Term [[ P CONSTRUCTION_TERM_type * ]]

::= CHOICE {

    required-construction-factor

	[0] Construction-Factor [[ p &(parm->factor) ]]

	%{
	    parm->term_tag = REQ_tag;
	%},

    optional-construction-factor

	[1] Construction-Factor [[ p &(parm->factor) ]]

	%{
	    parm->term_tag = OPT_tag;
	%},

    repetitive-construction-factor

	[2] Construction-Factor [[ p &(parm->factor) ]]

	%{
	    parm->term_tag = REP_tag;
	%},

    optional-repetitive-factor

	[3] Construction-Factor [[ p &(parm->factor) ]]

	%{
	    parm->term_tag = OPT_REP_tag;
	%}

}


Construction-Factor [[ P CONSTRUCTION_FACTOR_type * ]]

%{
    POINTER_type memp;

    CHAR_type *ObjectClassIDChars;
    STRING_type ObjectClassIDString;
    struct hash_t *ObjectClassBucket;
    CONSTITUENT_type ObjectClass;
    DOCUMENT_type Document;
%}

::= CHOICE {

    object-class-identifier

	Object-or-Class-Identifier [[ s ObjectClassIDChars ]]

	%{
	    parm->factor_tag = OBJECT_CLASS_ID_tag;
	    ObjectClassIDString = Chars2String(ObjectClassIDChars);
	    if ( ObjectClassIDString == ERROR_STRING ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD3_Construction__Factor]: can not convert object-class-identifier to a string" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    Document = GetSavedDocument();
	    if ( Document == ERROR_DOCUMENT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD3_Construction__Factor]:  could not get the document" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    ObjectClassBucket =
		ISODE_GetHashStructure(ObjectClassIDString,Document);
	    if ( ObjectClassBucket == ERROR_BUCKET ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD3_Construction__Factor]: can not get hash structure" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    ObjectClass = ObjectClassBucket->constituent;
	    parm->factor_value.object_class_id = ObjectClass;
	%},

    construction-expression

	%{
	    memp = MM_Malloc(sizeof(CONSTRUCTION_EXPR_type));
	    if ( memp == ERROR_POINTER ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[parse_ISRD3_Construction__Factor]: out of memory" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	    parm->factor_value.expr = (CONSTRUCTION_EXPR_type *) memp;
	%}

	Construction-Expression [[ p parm->factor_value.expr ]]

	%{
	    parm->factor_tag = EXPR_tag;
	%}

}


Object-Id-Expression

::= CHOICE {

    current-object-function

	[0] IMPLICIT NULL,

    preceding-object-function

	[1] Object-Id-Expression,

    superior-object-function

	[3] Object-Id-Expression,

    current-instance-function

	[4] Current-Instance-Function

}


Numeric-Expression

::= CHOICE {

    numeric-literal

	[0] IMPLICIT INTEGER,

    increment-application

	[1] Numeric-Expression,

    decrement-application

	[2] Numeric-Expression,

    ordinal-application

	[3] CHOICE {

	    identifier
		Object-or-Class-Identifier,

	    expression
		Object-Id-Expression

	},

    binding-reference

	[4] IMPLICIT Binding-Reference

}


Binding-Reference

::= SET {

    object-reference

	CHOICE {

         identifier
	    Object-or-Class-Identifier,

         expression
	    Binding-Selection-Function

	},

    binding-identifier

	Binding-Name

}


Binding-Selection-Function

::= CHOICE {

     current-object-function

	[0] IMPLICIT NULL,

     preceding-function

	[1] Object-Id-Expression,

     superior-function

	[3] Object-Id-Expression,

     current-instance-function

	[4] Current-Instance-Function

}


Current-Instance-Function

::= SEQUENCE { 

     first-parameter

	CHOICE {

	    identifier
		[0] IMPLICIT Object-or-Class-Identifier,

	    layout-object-type
		[1] IMPLICIT Layout-Object-Type,

	    logical-object-type
		[2] IMPLICIT Logical-Object-Type

	},

     second-parameter

	CHOICE {

	    identifier
		Object-or-Class-Identifier,

	    expression
		Object-Id-Expression

	}

}


String-Expression

::= SEQUENCE OF Atomic-String-Expression


Atomic-String-Expression

::= CHOICE {

    string-literal

	[0] IMPLICIT OCTET STRING,

    binding-reference

	[2] IMPLICIT Binding-Reference,

    make-string-application

	[3] Numeric-Expression,

    upper-alpha-application

	[4] Numeric-Expression,

    lower-alpha-application

	[5] Numeric-Expression,

    upper-roman-application

	[6] Numeric-Expression,

    lower-roman-application

	[7] Numeric-Expression

}


END
