
ISWR17 DEFINITIONS ::=


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

#define TK_IMPL_ISWR

#include <iswr.h>

#undef TK_IMPL_ISWR

#define PEPYPARM struct isode2toolkit_t *

%}


BEGIN


EXPORTS Geo-Gr-Content-Defaults, Geo-Gr-Presentation-Features;


IMPORTS Encoding-Announcer,
	Line-Rendition, Marker-Rendition,Text-Rendition,
	Filled-Area-Rendition,Edge-Rendition,Colour-Representations,
        Transparency-Specification,Transformation-Specification,
	Region-Of-Interest, Picture-Orientation, Picture-Dimensions
        FROM ISWR15;


ENCODER build


Geo-Gr-Presentation-Features 		::=NULL 

Geo-Gr-Content-Defaults			::= SET {
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
END
