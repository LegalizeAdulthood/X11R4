
ISRD15 DEFINITIONS ::=


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

#include <odatk.h>

#define TK_IMPL_ISRD

#include <isrd.h>

#undef TK_IMPL_ISRD

#define PEPYPARM struct isode2toolkit_t *

%}


BEGIN


EXPORTS Geometric-Graphics-Attributes, Encoding-Announcer,
	Line-Rendition, Marker-Rendition,Text-Rendition,
	Filled-Area-Rendition,Edge-Rendition,Colour-Representations,
        Transparency-Specification,Transformation-Specification,
	Region-Of-Interest, VDC-Pair, Picture-Orientation, Picture-Dimensions;


DECODER parse


Geometric-Graphics-Attributes ::=    SET{
     encoding-announcer              [0] IMPLICIT Encoding-Announcer OPTIONAL,
     line-rendition                  [1] IMPLICIT Line-Rendition OPTIONAL,
     marker-rendition                [2] IMPLICIT Marker-Rendition OPTIONAL,
     text-rendition                  [3] IMPLICIT Text-Rendition OPTIONAL,
     filled-area-rendition           [4] IMPLICIT Filled-Area-Rendition 
								OPTIONAL,
     edge-rendition                  [5] IMPLICIT Edge-Rendition OPTIONAL,
     colour-representations          [6] IMPLICIT Colour-Representations 
								OPTIONAL,
     transparency-specification      [7] IMPLICIT Transparency-Specification 
								OPTIONAL,
     transformation-specification    [8] IMPLICIT Transformation-Specification 
								OPTIONAL,
     region-of-interest              [9]  Region-Of-Interest OPTIONAL,
     picture-orientation            [10] IMPLICIT Picture-Orientation OPTIONAL,
     picture-dimensions             [11]  Picture-Dimensions OPTIONAL}

Encoding-Announcer ::=            OCTET STRING

Line-Rendition ::=                SEQUENCE {
     individual-part                 [0] IMPLICIT OCTET STRING OPTIONAL,
     asf-part                        [1] IMPLICIT SEQUENCE {
        line-type-asf        	         ASF-Type,
	line-width-asf           	  ASF-Type,
	line-colour-asf           	  ASF-Type
   							} OPTIONAL,

     bundle-part                     [2] IMPLICIT SEQUENCE OF
      						       SEQUENCE {
        bundle-index                      INTEGER,
        bundle-representation             OCTET STRING } OPTIONAL
                                       }

Marker-Rendition ::=              SEQUENCE {
     individual-part                 [0] IMPLICIT OCTET STRING OPTIONAL,
     asf-part                        [1] IMPLICIT SEQUENCE {
       marker-type-asf        	          ASF-Type,
       marker-width-asf           	  ASF-Type,
       marker-colour-asf           	  ASF-Type
   							} OPTIONAL,

     bundle-part                     [2] IMPLICIT SEQUENCE OF
      						       SEQUENCE {
        bundle-index                      INTEGER,
        bundle-representation             OCTET STRING } OPTIONAL
					}

Text-Rendition ::=                SEQUENCE {
     individual-part                 [0] IMPLICIT OCTET STRING OPTIONAL,
     asf-part                        [1] IMPLICIT SEQUENCE {
        text-type-asf        	          ASF-Type,
	text-precision-asf           	  ASF-Type,
        character-expansion-factor-asf    ASF-Type,
        character-spacing-asf  	          ASF-Type,
	text-colour-asf           	  ASF-Type
   							} OPTIONAL,

     bundle-part                     [2] IMPLICIT SEQUENCE OF
      						       SEQUENCE {
        bundle-index                      INTEGER,
        bundle-representation             OCTET STRING } OPTIONAL
                                       }

Filled-Area-Rendition ::=         SEQUENCE {
     individual-part                 [0] IMPLICIT OCTET STRING OPTIONAL,
     pattern-table-part              [1] IMPLICIT SEQUENCE OF 
						OCTET STRING OPTIONAL,
     asf-part                        [2] IMPLICIT SEQUENCE {
        interior-style-asf    	          ASF-Type,
	fill-colour-asf           	  ASF-Type,
	hatch-index-asf           	  ASF-Type,
	pattern-index-asf           	  ASF-Type
   							} OPTIONAL,

     bundle-part                     [3] IMPLICIT SEQUENCE OF
      						       SEQUENCE {
        bundle-index                      INTEGER,
        bundle-representation             OCTET STRING } OPTIONAL
                                       }

Edge-Rendition ::=                SEQUENCE {
     individual-part                 [0] IMPLICIT OCTET STRING OPTIONAL,
     asf-part                        [1] IMPLICIT SEQUENCE {
        edge-type-asf        	          ASF-Type,
	edge-width-asf           	  ASF-Type,
	edge-colour-asf           	  ASF-Type
   							} OPTIONAL,

     bundle-part                     [2] IMPLICIT SEQUENCE OF
      						       SEQUENCE {
        bundle-index                      INTEGER,
        bundle-representation             OCTET STRING } OPTIONAL
                                       }

ASF-Type ::= 			 INTEGER { bundled(0), individual(1)}

Colour-Representations ::=         SEQUENCE{
     background-colour               [0] IMPLICIT OCTET STRING OPTIONAL,
     colour-table-part               [1] IMPLICIT SEQUENCE OF
						OCTET STRING OPTIONAL
					}

Transparency-Specification ::=    OCTET STRING 

Transformation-Specification ::=  OCTET STRING

Region-Of-Interest ::=            CHOICE{
      automatic                      [0] IMPLICIT NULL,
      rectangle                      [1] IMPLICIT SEQUENCE {VDC-Pair,VDC-Pair}
                                         }

VDC-Pair ::=                      OCTET STRING

Picture-Orientation ::=           One-Of-Four-Angles

One-Of-Four-Angles ::=            INTEGER{    d0(0),
                                               d90(1),
                                              d180(2),
                                              d270(3)  }

Picture-Dimensions ::=            CHOICE{
     picture-width                   [0] IMPLICIT SEQUENCE{
        w-mimimum-width                               INTEGER,
        w-preferred-width                             INTEGER},
     picture-height                  [1] IMPLICIT SEQUENCE{
        h-mimimum-height                             INTEGER,
        h-preferred-height                           INTEGER},
     picture-size                    [2] IMPLICIT SEQUENCE{
        s-mimimum-width                              INTEGER,
        s-preferred-width                            INTEGER,
        s-mimimum-height                             INTEGER,
        s-preferred-height                           INTEGER,
        s-aspect-ratio-flag                          INTEGER{ fixed(0), variable(1)   }},
       automatic 		     [3] IMPLICIT NULL
						}


Geo-Gr-Presentation-Attribute 		::= CHOICE{ INTEGER  }


END
