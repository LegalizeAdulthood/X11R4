
ISPR12 DEFINITIONS ::=


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


EXPORTS  Raster-Graphics-Attributes,
        Pel-Transmission-Density,
	Clipping,
	Pel-Spacing,
        Spacing-Ratio,
	One-Of-Four-Angles,
	One-Of-Two-Angles,
	Image-Dimensions;


IMPORTS Measure-Pair
	FROM ISPR4;


PRINTER print


Raster-Graphics-Attributes	::= SET {
	pel-path			[0] IMPLICIT One-Of-Four-Angles
								OPTIONAL,
	line-progression		[1] IMPLICIT One-Of-Two-Angles
								OPTIONAL,
	pel-transmission-density	[2] IMPLICIT Pel-Transmission-Density
								OPTIONAL,
	initial-offset			[3] IMPLICIT Measure-Pair
								OPTIONAL,
	clipping			[4] IMPLICIT Clipping OPTIONAL,
	pel-spacing			[5]  Pel-Spacing
								OPTIONAL,
	spacing-ratio			[6] IMPLICIT Spacing-Ratio
								OPTIONAL,
	image-dimensions		[7] Image-Dimensions OPTIONAL }

One-Of-Four-Angles		::= INTEGER {
					d0 (0), -- 0 degrees
					d90 (1), -- 90 degrees
					d180 (2), -- 180 degrees
					d270 (3) -- 270 degrees
				    }

One-Of-Two-Angles		::= INTEGER {
					d90 (1), -- 90 degrees
					d270 (3) -- 270 degrees
				    }

Pel-Transmission-Density	::= INTEGER {
					p6 (1), -- 6 BMU
					p5 (2), -- 5 BMU
					p4 (3), -- 4 BMU
					p3 (4), -- 3 BMU
					p2 (5), -- 2 BMU
					p1 (6)  -- 1 BMU
								}


Clipping			::= SEQUENCE {
	first-coordinate-pair		[0] IMPLICIT Coordinate-Pair
								OPTIONAL,
	second-coordinate-pair		[1] IMPLICIT Coordinate-Pair}


Coordinate-Pair			::= SEQUENCE {
	x-coordinate			INTEGER,
	y-coordinate			INTEGER }


Pel-Spacing			::= CHOICE {
	spacing				[0] IMPLICIT 
						 SEQUENCE {
		length				INTEGER,
		pel-spaces			INTEGER },
	null				[1] IMPLICIT NULL }


Spacing-Ratio			::= SEQUENCE {
	line-spacing-value		INTEGER,
	pel-spacing-value		INTEGER }


Image-Dimensions		::= CHOICE {
	width-controlled		[0] IMPLICIT SEQUENCE {
		minimum-width			INTEGER,
		preferred-width			INTEGER },

	height-controlled		[1] IMPLICIT SEQUENCE {
		minimum-height			INTEGER,
		preferred-height			INTEGER },

	area-controlled			[2] IMPLICIT SEQUENCE {
		a-minimum-width			INTEGER,
		a-preferred-width		INTEGER,
		a-minimum-height		INTEGER,
		a-preferred-height		INTEGER,
		aspect-ratio-flag		INTEGER {
							fixed     (0),
							variable  (1) } },

	automatic			[3] IMPLICIT NULL }
END
