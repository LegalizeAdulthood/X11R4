
ISWR9 DEFINITIONS ::=


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


EXPORTS Character-Attributes,One-Of-Four-Angles, One-Of-Two-Angles,
	Alignment, Graphic-Rendition, Kering-Offset,
	Proportional-Line-Spacing,
        Pairwise-Kernings;


IMPORTS Measure-Pair
	FROM ISWR4;


ENCODER build


Character-Attributes		 ::= SET {
	character-path			[0] IMPLICIT One-Of-Four-Angles
								OPTIONAL,
	line-progression		[1] IMPLICIT One-Of-Two-Angles
								OPTIONAL,
	character-orientation 		[2] IMPLICIT One-Of-Four-Angles
								OPTIONAL,
	initial-offset			[3] IMPLICIT Measure-Pair
								OPTIONAL,
	character-spacing		[6] IMPLICIT INTEGER DEFAULT 120,
	line-spacing			[7] IMPLICIT INTEGER DEFAULT 200,
	alignment			[8] IMPLICIT Alignment OPTIONAL,
	line-layout-table		[9] IMPLICIT Layout-Table
								OPTIONAL,
	graphic-rendition		[10] IMPLICIT Graphic-Rendition
								OPTIONAL,
	formatting-indicator		[11] IMPLICIT Formatting-Indicator
								OPTIONAL,
	charcter-fonts			[12] IMPLICIT Character-Fonts
								OPTIONAL,
	graphic-char-subrepertoire	[13] IMPLICIT INTEGER DEFAULT 0,
	itemisation			[14] IMPLICIT Itemisation
								OPTIONAL,
	widow-size			[15] IMPLICIT INTEGER DEFAULT 1,
	orphan-size			[16] IMPLICIT INTEGER DEFAULT 1,
	graphic-character-sets		[17] IMPLICIT OCTET STRING OPTIONAL,
	indentation			[19] IMPLICIT INTEGER DEFAULT 0,
	kerning-offset			[20] IMPLICIT Kerning-Offset OPTIONAL,
	proportional-line-spacing	[21] IMPLICIT Proportional-Line-Spacing
								 OPTIONAL,
	pairwise-kerning		[22] IMPLICIT Pairwise-Kerning
								OPTIONAL,
	first-line-offset		[23] IMPLICIT INTEGER DEFAULT 0,
	code-extension-announcers	[24] IMPLICIT OCTET STRING 
								OPTIONAL}


One-Of-Four-Angles		::= INTEGER { d0	(0),
					      d90	(1),
					      d180	(2),
					      d270	(3) }

One-Of-Two-Angles		::= INTEGER { d90	(1),
					      d270	(3) }


Alignment			::= INTEGER { start-aligned (0),
					      end-aligned   (1),
					      centred       (2),
					      justified     (3) }



Layout-Table			::= SET OF Tabulation-Stop



Tabulation-Stop			::= SET {
	tabulation-reference		[0] IMPLICIT NumericString,
	tabulation-position		[1] IMPLICIT INTEGER,
	alignment			[2] IMPLICIT  INTEGER 
						{ start-aligned (0),
					      end-aligned   (1),
					      centred-1       (2),
					      round-aligned     (3) },
	alignment-character-string	[3] IMPLICIT OCTET STRING OPTIONAL}

				-- string of graphic characters
				-- from the set of graphic elements
				-- specified by the presentation
				-- attributes "graphic character
				-- sets" and "graphic character
				-- subrepertoire"



Graphic-Rendition		::= SET OF Graphic-Rendition-Aspect


Character-Fonts			::= SET {
	primary-font			[0] IMPLICIT Font-Type OPTIONAL,
	first-alternative-font		[1] IMPLICIT Font-Type OPTIONAL,
	second-alternative-font		[2] IMPLICIT Font-Type OPTIONAL,
	third-alternative-font		[3] IMPLICIT Font-Type OPTIONAL,
	fourth-alternative-font		[4] IMPLICIT Font-Type OPTIONAL,
	fifth-alternative-font		[5] IMPLICIT Font-Type OPTIONAL,
	sixth-alternative-font		[6] IMPLICIT Font-Type OPTIONAL,
	seventh-alternative-font	[7] IMPLICIT Font-Type OPTIONAL,
	eighth-alternative-font		[8] IMPLICIT Font-Type OPTIONAL,
	ninth-alternative-font		[9] IMPLICIT Font-Type OPTIONAL}



Font-Type			::= SET {
	font-size			[0] IMPLICIT INTEGER,
	font-identifier			[1] IMPLICIT INTEGER }


Graphic-Rendition-Aspect	::= INTEGER {
					cancel				(0),
					increased-intensity		(1),
					decreased-intensity		(2),
					italicized			(3),
					underlined			(4),
					slowly-blinking			(5),
					rapidly-blinking		(6),
					negative-image			(7),
					crossed-out			(9),
					primary-font			(10),
					first-alternative-font		(11),
					second-alternative-font		(12),
					third-alternative-font		(13),
					fourth-alternative-font		(14),
					fifth-alternative-font		(15),
					sixth-alternative-font		(16),
					seventh-alternative-font	(17),
					eighth-alternative-font		(18),
					ninth-alternative-font		(19),
					doubly-underlined		(21),
					normal-intensity		(22),
					not-italicized			(23),
					not-underlined			(24),
					steady				(25),
					variable-spacing		(26),
					positive-image			(27),
					not-crossed-out			(29) }


Formatting-Indicator		::= INTEGER { no (0), yes (1) }


Itemisation 			::= SET {
	identifier-alignment		[0] IMPLICIT INTEGER{
					no-itemisation (0), 
                                        start-aligned (1), 
					end-aligned (2)    },
	identifier-start-offset		[1] IMPLICIT INTEGER DEFAULT 0,
	identifier-end-offset		[2] IMPLICIT INTEGER DEFAULT 0 }

Kerning-Offset			::= SET {
	start-offset			[0] IMPLICIT INTEGER,
	end-offset			[1] IMPLICIT INTEGER }

Proportional-Line-Spacing	::= INTEGER { no (0) , yes (1) }

Pairwise-Kerning		::= INTEGER { no (0) , yes (1) }
END
