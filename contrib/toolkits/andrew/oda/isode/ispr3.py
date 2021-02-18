
ISPR3 DEFINITIONS ::=


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

%}


BEGIN


-- EXPORTS    Content-Portion-Identifier, Object-or-Class-Identifier,
--         Style-Identifier, Layout-Category-Name,
--        Resource-Name, Binding-Name,
--	  Construction-Expression, Object-Id-Expression,
--	  Numeric-Expression, String-Expresion;


IMPORTS 
	Layout-Object-Type
	FROM ISPR4

	Logical-Object-Type
        FROM ISPR5;


PRINTER print


Content-Portion-Identifier       ::= [APPLICATION 0] IMPLICIT PrintableString

         -- only digits and space are used in the present version
         -- of the standard; other characters are reserved for extensions

Object-or-Class-Identifier       ::= [APPLICATION 1] IMPLICIT PrintableString

         -- only digits and space are used in the present version
         -- of the standard; other characters are reserved for extensions;
         -- a "null" value is represented by an empty string

Style-Identifier                 ::= [APPLICATION 5] IMPLICIT PrintableString

         -- only digits and space are used in the present version
         -- of the standard; other characters are reserved for extensions
         -- a "null" value is represented by an empty string

Layout-Category-Name             ::= PrintableString

         -- a "null" value is represented by empty string

Resource-Name			 ::= PrintableString


Binding-Name			 ::= PrintableString


Construction-Expression          ::= CHOICE {
     construction-type		    Construction-Type,
     single-term-construction       [3] Construction-Term}

Construction-Type		 ::= CHOICE {
     sequence-construction          [0] IMPLICIT Term-Sequence,
     aggregate-construction         [1] IMPLICIT Term-Sequence,
     choice-construction            [2] IMPLICIT Term-Sequence}

Term-Sequence                    ::= SEQUENCE OF Construction-Term

Construction-Term                ::= CHOICE {
     required-construction-factor   [0] Construction-Factor,
     optional-construction-factor   [1] Construction-Factor,
     repetitive-construction-factor [2] Construction-Factor,
     optional-repetitive-factor     [3] Construction-Factor}

Construction-Factor              ::= CHOICE {
     object-class-identifier        Object-or-Class-Identifier,
     construction-expression        Construction-Expression}

Object-Id-Expression             ::= CHOICE {
     current-object-function        [0] IMPLICIT NULL,
     preceding-object-function      [1] Object-Id-Expression,
     superior-object-function       [3] Object-Id-Expression,
     current-instance-function      [4] Current-Instance-Function}


Numeric-Expression               ::= CHOICE {
     numeric-literal                [0] IMPLICIT INTEGER,
     increment-application          [1] Numeric-Expression,
     decrement-application          [2] Numeric-Expression,
     ordinal-application            [3] CHOICE {
         identifier                     Object-or-Class-Identifier,
         expression                     Object-Id-Expression},
     binding-reference              [4] IMPLICIT Binding-Reference}

Binding-Reference                ::= SET {
     object-reference               CHOICE {
         identifier                     Object-or-Class-Identifier,
         expression                     Binding-Selection-Function},
     binding-identifier             Binding-Name}


Binding-Selection-Function       ::= CHOICE {
     current-object-function        [0] IMPLICIT NULL,
     preceding-function             [1] Object-Id-Expression,
     superior-function              [3] Object-Id-Expression,
     current-instance-function      [4] Current-Instance-Function}

Current-Instance-Function        ::= SEQUENCE { 
     first-parameter                CHOICE {
         identifier                    [0] IMPLICIT Object-or-Class-Identifier,
         layout-object-type            [1] IMPLICIT Layout-Object-Type,
         logical-object-type           [2] IMPLICIT Logical-Object-Type},
     second-parameter               CHOICE {
         identifier                     Object-or-Class-Identifier,
         expression                     Object-Id-Expression}}

String-Expression                ::= SEQUENCE OF Atomic-String-Expression

Atomic-String-Expression         ::= CHOICE {
     string-literal                 [0] IMPLICIT OCTET STRING,
     binding-reference              [2] IMPLICIT Binding-Reference,
     make-string-application        [3] Numeric-Expression,
     upper-alpha-application        [4] Numeric-Expression,
     lower-alpha-application        [5] Numeric-Expression,
     upper-roman-application        [6] Numeric-Expression,
     lower-roman-application        [7] Numeric-Expression}



END
