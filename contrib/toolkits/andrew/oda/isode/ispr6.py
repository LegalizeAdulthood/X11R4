
ISPR6 DEFINITIONS ::=


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


EXPORTS Presentation-Style-Descriptor, Presentation-Attributes,
	Content-Type, Content-Architecture-Class,
        Layout-Style-Descripto,Fill-Order, Block-Alignment;


IMPORTS Object-or-Class-Identifier, Style-Identifier,
	Layout-Category-Name, Object-Id-Expression
        FROM ISPR3
        Comment-String, Transparency, Colour, Border,
	Layout-Object-Type
        FROM ISPR4
	Character-Attributes
	FROM ISPR9
	Raster-Graphics-Attributes
	FROM ISPR12
	Geometric-Graphics-Attributes
	FROM ISPR15;


PRINTER print


Presentation-Style-Descriptor   ::= SET {
    style-identifier                Style-Identifier,
    user-readable-comments          [0] IMPLICIT Comment-String OPTIONAL,
    user-visible-name               [1] IMPLICIT Comment-String OPTIONAL,
    transparency                    [2] IMPLICIT Transparency OPTIONAL,
    presentation-attributes         [3] IMPLICIT Presentation-Attributes
                                                                      OPTIONAL,
    colour                          [4] IMPLICIT Colour OPTIONAL,
    border                          [5] IMPLICIT Border OPTIONAL}

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

Layout-Style-Descriptor         ::= SET {
    style-identifier                Style-Identifier,
    user-readable-comments          [0] IMPLICIT Comment-String OPTIONAL,
    user-visible-name               [1] IMPLICIT Comment-String OPTIONAL,
    layout-directives               [4] IMPLICIT Layout-Directives OPTIONAL}

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
