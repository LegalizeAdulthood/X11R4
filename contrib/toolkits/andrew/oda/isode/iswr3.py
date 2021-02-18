
ISWR3 DEFINITIONS ::=


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


-- EXPORTS
--	Content-Portion-Identifier, Object-or-Class-Identifier,
--      Style-Identifier, Layout-Category-Name,
--      Resource-Name, Binding-Name,
--	Construction-Expression, Object-Id-Expression,
--	Numeric-Expression, String-Expresion;


IMPORTS 
	Layout-Object-Type
	FROM ISWR4
	Logical-Object-Type
        FROM ISWR5;

ENCODER build



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

%{
    INT_type selection;
%}

::=

%{
    switch (parm->construction_tag) {
	case TERM_tag:
	    selection = (INT_type) 2;
	    break;
	case SEQUENCE_tag:
	case AGGREGATE_tag:
	case CHOICE_tag:
	    selection = (INT_type) 1;
	    break;
	default:
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[build_ISWR3_Construction__Expression]: bad tag for construction expression" );
	    dummy = TKERR_EndErrMsg();
	    return( NOTOK );
    }
%}

CHOICE << selection >> {

    construction-type

	Construction-Type [[ p parm ]],

    single-term-construction

	[3] Construction-Term [[ p &(parm->construction_value.term) ]]

}


Construction-Type [[ P CONSTRUCTION_EXPR_type * ]]

%{
    INT_type selection;
%}

::=

%{
    switch (parm->construction_tag) {
	case SEQUENCE_tag:
	    selection = (INT_type) 1;
	    break;
	case AGGREGATE_tag:
	    selection = (INT_type) 2;
	    break;
	case CHOICE_tag:
	    selection = (INT_type) 3;
	    break;
    }
%}


CHOICE << selection >> {

    sequence-construction

	[0] IMPLICIT Term-Sequence
	    [[ p parm->construction_value.construction ]],

    aggregate-construction

	[1] IMPLICIT Term-Sequence
	    [[ p parm->construction_value.construction ]],

    choice-construction

	[2] IMPLICIT Term-Sequence
	    [[ p parm->construction_value.construction ]]

}


Term-Sequence [[ P SEQUENCE_CONSTRUCTION_TERM_type ]]

%{
    INT_type count;
%}

::= SEQUENCE OF << count=0; count<parm->length; count++ >>

	Construction-Term
	    [[ p &(parm->sequence_value.construction_terms[count]) ]]


Construction-Term [[ P CONSTRUCTION_TERM_type * ]]

%{
    INT_type selection;
%}

::=

%{
    switch (parm->term_tag) {
	case REQ_tag:
	    selection = (INT_type) 1;
	    break;
	case OPT_tag:
	    selection = (INT_type) 2;
	    break;
	case REP_tag:
	    selection = (INT_type) 3;
	    break;
	case OPT_REP_tag:
	    selection = (INT_type) 4;
	    break;
	default:
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[build_ISWR3_Construction__Term]: bad tag for construction term" );
	    dummy = TKERR_EndErrMsg();
	    return( NOTOK );
    }
%}

CHOICE << selection >> {

    required-construction-factor

	[0] Construction-Factor [[ p &(parm->factor) ]],

    optional-construction-factor

	[1] Construction-Factor [[ p &(parm->factor) ]],

    repetitive-construction-factor

	[2] Construction-Factor [[ p &(parm->factor) ]],

    optional-repetitive-factor

	[3] Construction-Factor [[ p &(parm->factor) ]]

}


Construction-Factor [[ P CONSTRUCTION_FACTOR_type * ]]

%{
    INT_type selection;

    STRING_type ObjectClassIDString;
    CHAR_type *ObjectClassIDChars;
%}

::=

%{
    switch (parm->factor_tag) {
	case OBJECT_CLASS_ID_tag:
	    selection = (INT_type) 1;
	    break;
	case EXPR_tag:
	    selection = (INT_type) 2;
	    break;
	default:
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[build_ISWR3_Construction__Term]: bad tag for construction factor" );
	    dummy = TKERR_EndErrMsg();
	    return( NOTOK );
    }
%}

CHOICE << selection >> {

    object-class-identifier

	%{
	    if ( GetAttr( parm->factor_value.object_class_id,
			  at_OBJECT_CLASS_IDENTIFIER,
			  (POINTER_type) &ObjectClassIDString,
			  /* default */ BOOL_false,
			  /* mask */ (PARM_MASK_type *) 0
			) == ERROR_INT ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR3_Construction__Factor]: can not get attribute at_OBJECT_CLASS_IDENTIFIER" );
		dummy = TKERR_EndErrMsg();
		return ( NOTOK );
	    }
	    ObjectClassIDChars = String2Chars(ObjectClassIDString);
	    if ( ObjectClassIDChars == ERROR_CHARS ) {
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[build_ISWR3_Construction__Factor]: can not convert string to object-class-identifier chars" );
		dummy = TKERR_EndErrMsg();
		return( NOTOK );
	    }
	%}

	Object-or-Class-Identifier [[ s ObjectClassIDChars ]],

    construction-expression

	Construction-Expression [[ p parm->factor_value.expr ]]

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
