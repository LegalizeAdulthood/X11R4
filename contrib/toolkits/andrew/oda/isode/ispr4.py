
ISPR4 DEFINITIONS ::=


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

-- EXPORTS Layout-Object-Descriptor, Layout-Class-Descriptor,
--	Transparency, Comment-String, Layout-Object-Type,
--        Binding-Pair, One-Of-Four-Angles, Measure-Pair,
--	Medium-Type, Colour, Border;


IMPORTS Object-or-Class-Identifier, Style-Identifier,
	Layout-Category-Name, Resource-Name, Binding-Name,
	Construction-Expression, Object-Id-Expression,
        Numeric-Expression, String-Expression
        FROM ISPR3
        Presentation-Attributes
	FROM ISPR6
        Default-Value-Lists-Layout
        FROM ISPR7;


PRINTER print



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


Layout-Object-Descriptor        ::= SEQUENCE {
    object-type                     Layout-Object-Type OPTIONAL,
    descriptor-body                 Layout-Object-Descriptor-Body OPTIONAL}

Layout-Object-Type              ::= INTEGER {document-layout-root (0),
                                             page-set (1), page (2),
                                             frame (3), block (4)}
Layout-Object-Descriptor-Body   ::= SET {
    object-identifier               Object-or-Class-Identifier OPTIONAL,
    subordinates                    [0] IMPLICIT SEQUENCE OF NumericString
                                                                      OPTIONAL,
    content-portions                [1] IMPLICIT SEQUENCE OF NumericString
                                                                      OPTIONAL,
    object-class                    [2] IMPLICIT Object-or-Class-Identifier
                                                                      OPTIONAL,
    position                        [3] IMPLICIT Measure-Pair OPTIONAL,
    dimensions                      [4] IMPLICIT Dimension-Pair OPTIONAL,
    transparency                    [5] IMPLICIT Transparency OPTIONAL,
    presentation-attributes         [6] IMPLICIT Presentation-Attributes
                                                                      OPTIONAL,
    default-value-lists             [7] IMPLICIT Default-Value-Lists-Layout
                                                                      OPTIONAL,
    user-readable-comments          [8] IMPLICIT Comment-String OPTIONAL,
    bindings                        [9] IMPLICIT SET OF Binding-Pair OPTIONAL,
    layout-path                     [11] IMPLICIT One-Of-Four-Angles OPTIONAL,
    imaging-order                   [12] IMPLICIT SEQUENCE OF NumericString
                                                                      OPTIONAL,
    permitted-categories            [13] IMPLICIT SET OF Layout-Category-Name
                                                                      OPTIONAL,

         -- a "null" value is represented by an empty set

    user-visible-name               [14] IMPLICIT Comment-String OPTIONAL,
    page-position                   [15] IMPLICIT Measure-Pair OPTIONAL,
    medium-type                     [16] IMPLICIT Medium-Type OPTIONAL,
    presentation-style              [17] IMPLICIT Style-Identifier OPTIONAL,
    balance                         [21] IMPLICIT SET OF
                                            Object-or-Class-Identifier OPTIONAL,

         -- a "null" value is represented by an empty set

    colour                          [22] IMPLICIT Colour OPTIONAL,
    border                          [23] IMPLICIT Border OPTIONAL,
    application-comments	    [25] IMPLICIT OCTET STRING OPTIONAL}

Layout-Class-Descriptor          ::= SEQUENCE {
     object-type                    Layout-Object-Type,
     descriptor-body                Layout-Class-Descriptor-Body}

Layout-Class-Descriptor-Body     ::= SET {
     object-class-identifier        Object-or-Class-Identifier,
     generator-for-subordinates     [0] Construction-Expression OPTIONAL,
     content-portions               [1] IMPLICIT SEQUENCE OF NumericString
                                                                       OPTIONAL,
     position                       CHOICE {
     fixed-position		    [3] IMPLICIT Measure-Pair,
     variable-position		    [26] IMPLICIT Position-Spec} OPTIONAL,
     dimensions                     [4] IMPLICIT Dimension-Spec OPTIONAL,
     transparency                   [5] IMPLICIT Transparency OPTIONAL,
     presentation-attributes        [6] IMPLICIT Presentation-Attributes
                                                                       OPTIONAL,
     default-value-lists            [7] IMPLICIT Default-Value-Lists-Layout
                                                                       OPTIONAL,
     user-readable-comments         [8] IMPLICIT Comment-String OPTIONAL,  
     bindings                       [9] IMPLICIT SET OF Binding-Pair OPTIONAL,
     content-generator              [10] IMPLICIT String-Expression OPTIONAL,
     layout-path                    [11] IMPLICIT One-Of-Four-Angles OPTIONAL,
     permitted-categories           [13] IMPLICIT SET OF Layout-Category-Name
                                                                       OPTIONAL,

         -- a "null" value is represented by an empty set

     user-visible-name              [14] IMPLICIT Comment-String OPTIONAL,
     page-position                  [15] IMPLICIT Measure-Pair OPTIONAL,
     medium-type                    [16] IMPLICIT Medium-Type OPTIONAL,
     presentation-style             [17] IMPLICIT Style-Identifier OPTIONAL,
     logical-source                 [18] IMPLICIT Object-or-Class-Identifier 
                                                                       OPTIONAL,
     balance                        [21] IMPLICIT SET OF
                                                  Object-or-Class-Identifier OPTIONAL,

         -- a "null" value is represented by an empty set

     colour                         [22] IMPLICIT Colour OPTIONAL,
     border                         [23] IMPLICIT Border OPTIONAL,
     resource                       [24] IMPLICIT Resource-Name OPTIONAL,
     application-comments	    [25] IMPLICIT OCTET STRING OPTIONAL}

END
