
ISWR14 DEFINITIONS ::=


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


EXPORTS  Ra-Gr-Presentation-Attributes,
        Ra-Gr-Content-Defaults;


IMPORTS One-Of-Four-Angles,
        One-Of-Two-Angles,
        Pel-Transmission-Density,
	Measure-Pair,
	Clipping,
	Pel-Spacing,
        Spacing-Ratio,
	Image-Dimensions
        FROM ISWR12
         Compression
        FROM ISWR13;


ENCODER build


Ra-Gr-Presentation-Features	::= CHOICE {
	pel-path			[0] IMPLICIT One-Of-Four-Angles,
	line-progression		[1] IMPLICIT One-Of-Two-Angles,
	pel-transmission-density	[2] IMPLICIT Pel-Transmission-Density }


Raster-Gr-Content-Defaults::= SET {
	pel-path			[0] IMPLICIT One-Of-Four-Angles 
								OPTIONAL,
	line-progression		[1] IMPLICIT One-Of-Two-Angles
								OPTIONAL,
	pel-transmission-density	[2] IMPLICIT Pel-Transmission-Density
								OPTIONAL,
	pel-spacing			[5]  Pel-Spacing OPTIONAL,
	spacing-ratio			[6] IMPLICIT Spacing-Ratio OPTIONAL,
	compression			[8] IMPLICIT Compression OPTIONAL}


END
