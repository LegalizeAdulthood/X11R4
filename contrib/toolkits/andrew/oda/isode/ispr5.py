
ISPR5 DEFINITIONS ::=


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


EXPORTS Logical-Object-Descriptor, Logical-Class-Descriptor,
	Logical-Object-Type, Protection;


IMPORTS Object-or-Class-Identifier, Style-Identifier,
	Resource-Name, Construction-Expression,String-Expression
        FROM ISPR3
	Comment-String, Binding-Pair
        FROM ISPR4 
        Presentation-Attributes
	FROM ISPR6
        Default-Value-Lists-Logical
        FROM ISPR7;


PRINTER print


Logical-Object-Descriptor
::= SEQUENCE {
    object-type
	    Logical-Object-Type
	    OPTIONAL,
    descriptor-body                 Logical-Object-Descriptor-Body OPTIONAL}

Logical-Object-Type             ::= INTEGER {
					     document-logical-root (0),
                                             composite-logical-object (1),
                                             basic-logical-object (2)}

Logical-Object-Descriptor-Body ::=  SET {
    object-identifier               Object-or-Class-Identifier OPTIONAL,
    subordinates                    [0] IMPLICIT SEQUENCE OF NumericString
                                                                      OPTIONAL,
    content-portions                [1] IMPLICIT SEQUENCE OF NumericString
                                                                      OPTIONAL,
    object-class                    [2] IMPLICIT Object-or-Class-Identifier
                                                                      OPTIONAL,
    presentation-attributes         [6] IMPLICIT Presentation-Attributes
                                                                      OPTIONAL,
    default-value-lists             [7] IMPLICIT Default-Value-Lists-Logical
                                                                      OPTIONAL,
    user-readable-comments          [8] IMPLICIT Comment-String OPTIONAL,
    bindings                        [9] IMPLICIT SET OF Binding-Pair OPTIONAL,
    content-generator               [10] IMPLICIT String-Expression OPTIONAL,
    user-visible-name               [14] IMPLICIT Comment-String OPTIONAL,
    presentation-style              [17] IMPLICIT Style-Identifier OPTIONAL,
    layout-style                    [19] IMPLICIT Style-Identifier OPTIONAL,
    protection                      [20] IMPLICIT Protection OPTIONAL,
    application-comments	    [25] IMPLICIT OCTET STRING OPTIONAL}


Logical-Class-Descriptor        ::= SEQUENCE {
    object-type                     Logical-Object-Type,
    descriptor-body                 Logical-Class-Descriptor-Body}

Logical-Class-Descriptor-Body   ::= SET {
    object-class-identifier         Object-or-Class-Identifier,
    generator-for-subordinates      [0] Construction-Expression OPTIONAL,
    content-portions                [1] IMPLICIT SEQUENCE OF NumericString
                                                                      OPTIONAL,
    presentation-attributes         [6] IMPLICIT Presentation-Attributes
                                                                      OPTIONAL,
    default-value-lists             [7] IMPLICIT Default-Value-Lists-Logical
                                                                      OPTIONAL,
    user-readable-comments          [8] IMPLICIT Comment-String OPTIONAL,
    bindings                        [9] IMPLICIT SET OF Binding-Pair OPTIONAL,
    content-generator               [10] IMPLICIT String-Expression OPTIONAL,
    user-visible-name               [14] IMPLICIT Comment-String OPTIONAL,
    presentation-style              [17] IMPLICIT Style-Identifier OPTIONAL,
    layout-style                    [19] IMPLICIT Style-Identifier OPTIONAL,
    protection                      [20] IMPLICIT Protection OPTIONAL,
    resource                        [24] IMPLICIT Resource-Name OPTIONAL,
    application-comments	    [25] IMPLICIT OCTET STRING OPTIONAL}


Protection			 ::= INTEGER {
					unprotected (0),
					protected (1)}

END
