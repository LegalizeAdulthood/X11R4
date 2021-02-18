
ISPR8 DEFINITIONS ::=


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


EXPORTS Text-Unit, Type-Of-Coding;


IMPORTS  Content-Portion-Identifier
        FROM ISPR3
	Character-Coding-Attributes
	FROM ISPR10
	Raster-Gr-Coding-Attributes
	FROM ISPR13
	Geo-Gr-Coding-Attributes
	FROM ISPR16;


PRINTER print


Text-Unit                        ::= SEQUENCE {
      content-portion-attributes     Content-Portion-Attributes OPTIONAL,
      content-information             Content-Information}

Content-Portion-Attributes       ::= SET {
      content-identifier-layout      Content-Portion-Identifier OPTIONAL,
      content-identifier-logical     [4] IMPLICIT Content-Portion-Identifier
                                                                       OPTIONAL,
      type-of-coding                 CHOICE {
                                     [0] IMPLICIT INTEGER {
                                         iso2022 (0), t6 (1)},
                                     [6] IMPLICIT OBJECT IDENTIFIER} OPTIONAL,
      coding-attributes              CHOICE {
         character-coding-attributes [1] IMPLICIT Character-Coding-Attributes,
         raster-gr-coding-attributes [2] IMPLICIT Raster-Gr-Coding-Attributes,
         geo-gr-coding-attributes    [7] IMPLICIT Geo-Gr-Coding-Attributes,
         
         -- the following tags are reserved for additional types
         -- coding attributes:
              -- [8] composite-graphics
              -- [9] audio
              -- [10] dynamic-graphics
     ext-cont-arch-coding-attributes  [11] IMPLICIT EXTERNAL} OPTIONAL,
      alternative-representation     [3] IMPLICIT Alternative-Representation
                                                                       OPTIONAL}


Content-Information              ::= OCTET STRING

Type-of-Coding                   ::= CHOICE {
                                     [0] IMPLICIT INTEGER {
                                         iso2022 (0), t6 (1)},
                                     [6] IMPLICIT OBJECT IDENTIFIER}

Alternative-Representation       ::= OCTET STRING

         -- string of characters from the sets designated by
         -- the document profile attribute "alternative representation 
	 -- character sets", plus carriage return and line feed

END
