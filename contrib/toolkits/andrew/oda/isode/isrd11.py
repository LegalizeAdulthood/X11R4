
ISRD11 DEFINITIONS ::=


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


EXPORTS  Char-Presentation-Features, 
        Character-Content-Defaults;


IMPORTS One-Of-Four-Angles, One-Of-Two-Angles,
	Alignment, Graphic-Rendition, Kerning-Offset,
	Proportional-Line-Spacing,
        Pairwise-Kerning
        FROM ISRD9;


DECODER parse


Char-Presentation-Features	::= CHOICE {
	character-path			[0] IMPLICIT One-Of-Four-Angles,
	line-progression		[1] IMPLICIT One-Of-Two-Angles,
	character-orientation		[2] IMPLICIT One-Of-Four-Angles,
	character-spacing		[6] IMPLICIT INTEGER,
	line-spacing			[7] IMPLICIT INTEGER,
	alignment			[8] IMPLICIT Alignment,
	graphic-rendition		[10] IMPLICIT Graphic-Rendition,
	graphic-char-subrepertoire	[13] IMPLICIT INTEGER,
	graphic-character-sets		[17] IMPLICIT OCTET STRING }

Character-Content-Defaults		::= SET {
	character-path			[0] IMPLICIT One-Of-Four-Angles
								OPTIONAL,
	line-progression		[1] IMPLICIT One-Of-Two-Angles
								OPTIONAL,
	character-orientation		[2] IMPLICIT One-Of-Four-Angles
								OPTIONAL,
	character-spacing		[6] IMPLICIT INTEGER DEFAULT 120,
	line-spacing			[7] IMPLICIT INTEGER DEFAULT 200,
	alignment			[8] IMPLICIT Alignment OPTIONAL,
	graphic-rendition		[10] IMPLICIT Graphic-Rendition
								OPTIONAL,
	graphic-char-subrepertoire	[13] IMPLICIT INTEGER DEFAULT 0,
	widow-size			[15] IMPLICIT INTEGER DEFAULT 1,
	orphan-size			[16] IMPLICIT INTEGER DEFAULT 1,
	graphic-character-sets		[17] IMPLICIT OCTET STRING
								OPTIONAL,
	indentation			[19] IMPLICIT INTEGER DEFAULT 0,
	kerning-offset			[20] IMPLICIT Kerning-Offset OPTIONAL,
	proportional-line-spacing	[21] IMPLICIT Proportional-Line-Spacing
								 OPTIONAL,
	pairwise-kerning		[22] IMPLICIT Pairwise-Kerning
								 OPTIONAL }
END
