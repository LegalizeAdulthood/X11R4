
ISPR13 DEFINITIONS ::=


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


EXPORTS Raster-Gr-Coding-Attributes, Compression;


PRINTER print


Raster-Gr-Coding-Attributes	::= SET {
	number-of-pels-per-line		[0] IMPLICIT INTEGER DEFAULT 0,
	number-of-lines			[1] IMPLICIT INTEGER DEFAULT 0,
	compression			[2] IMPLICIT Compression OPTIONAL,
	number-of-discarded-pels	[3] IMPLICIT INTEGER DEFAULT 0}

Compression			::= INTEGER { uncompressed (0),
					      compressed   (1)}
END
