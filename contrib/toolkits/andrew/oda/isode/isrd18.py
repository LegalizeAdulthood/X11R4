
ISRD18 DEFINITIONS ::=

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


-- EXPORTS  Font-Reference;


BEGIN


DECODER parse


-- IS09541-FONTS DEFINITIONS

Font-Reference [[ P struct isode2toolkit_t * ]]
::= SEQUENCE  {
default-name-prefix		[0]  IMPLICIT Default-Name-Prefix OPTIONAL,
font-name			[1]  IMPLICIT Structured-Name OPTIONAL,
device-independent-font-name	[2]  IMPLICIT Structured-Name OPTIONAL,
proprietary-data-indicator		[3]  IMPLICIT INTEGER OPTIONAL,
typeface-name			[4]  IMPLICIT Structured-Name OPTIONAL,
family-name			[5]  IMPLICIT Structured-Name OPTIONAL,
posture				[6]  IMPLICIT INTEGER OPTIONAL,
weight				[7]  IMPLICIT INTEGER OPTIONAL,
proportionate-width		[8]  IMPLICIT INTEGER OPTIONAL,
character-complement		[9]  IMPLICIT Character-Complement OPTIONAL,
number-of-characters		[10] IMPLICIT INTEGER OPTIONAL,
ligature-table			[11] IMPLICIT Ligature-Table OPTIONAL,
design-size			[12] IMPLICIT Real OPTIONAL,
minimum-size			[13] IMPLICIT Real OPTIONAL,
maximum-size			[14] IMPLICIT Real OPTIONAL,
design-classification		[15] IMPLICIT SEQUENCE OF INTEGER OPTIONAL,
structure				[16] IMPLICIT INTEGER OPTIONAL,
minimum-feature-size		[17] IMPLICIT Real OPTIONAL,
writing-modes			[18] IMPLICIT Writing-Modes OPTIONAL,
writing-mode			[19] IMPLICIT Structured-Name OPTIONAL,
character-shape-technologies	[20] IMPLICIT Character-Shape-Technologies OPTIONAL,
non-ISO-attributes			[21] IMPLICIT SET OF Non-ISO-Attribute OPTIONAL}


Character-Complement		::= SEQUENCE {
   default-name-prefix	              [0] IMPLICIT Default-Name-Prefix OPTIONAL,
included-character-collections	[1] IMPLICIT SET OF Structured-Name OPTIONAL,
excluded-character-collections	[2] IMPLICIT SET OF Structured-Name OPTIONAL,
included-characters		[3] IMPLICIT SET OF Structured-Name OPTIONAL,
excluded-characters		[4] IMPLICIT SET OF Structured-Name OPTIONAL}

Ligature-Table			::= SEQUENCE {
default-name-prefix		[0] IMPLICIT Default-Name-Prefix OPTIONAL,
ligatures				[1] IMPLICIT SET OF Ligature }

Ligature				::= SEQUENCE {
default-name-prefix		[0] IMPLICIT Default-Name-Prefix OPTIONAL,
ligature-character			[1] IMPLICIT Structured-Name ,
equivalent-characters		[2] IMPLICIT SEQUENCE OF Structured-Name }

Writing-Modes			::=SEQUENCE {
default-name-prefix		[0] IMPLICIT Default-Name-Prefix OPTIONAL,
iso-0				[1] IMPLICIT Modal-Font-Attributes OPTIONAL,
iso-180				[2] IMPLICIT Modal-Font-Attributes OPTIONAL,
iso-270				[3] IMPLICIT Modal-Font-Attributes OPTIONAL,
non-ISO-modes			[4] IMPLICIT SET OF SEQUENCE  {

mode-name			[0] IMPLICIT Structured-Name ,
mode-attributes			[1] IMPLICIT Modal-Font-Attributes }
                                                                                                                 OPTIONAL}

Modal-Font-Attributes		::=SEQUENCE {
default-name-prefix		[0] IMPLICIT Default-Name-Prefix OPTIONAL,
escapement-direction		[1] IMPLICIT Real OPTIONAL,
escapement-class			[2] IMPLICIT INTEGER OPTIONAL,
average-escapement		[3] IMPLICIT Real OPTIONAL,
average-lower-case-escapement	[4] IMPLICIT Real OPTIONAL,
average-capital-escapement		[5] IMPLICIT Real OPTIONAL,
weighted-average-escapement	[6] IMPLICIT Real OPTIONAL,
tabular-escapement		[7] IMPLICIT SEQUENCE	 {
   x-component			   [0] IMPLICIT Real OPTIONAL,
   y-component			   [1] IMPLICIT Real OPTIONAL} OPTIONAL,
maximum-forward-extent		[8] IMPLICIT Real OPTIONAL,
maximum-left-extent		[9] IMPLICIT Real OPTIONAL,
maximum-backward-extent	             [10] IMPLICIT Real OPTIONAL,
maximum-right-extent	             [11] IMPLICIT Real OPTIONAL,
class-pairwise-esc-adjustments         [12] IMPLICIT SET OF SEQUENCE {
first-character-forward-class	[0] IMPLICIT INTEGER ,
second-character-backward-class	[1] IMPLICIT INTEGER  ,
x-adjustment			[2] IMPLICIT Real OPTIONAL,
y-adjustment			[3] IMPLICIT Real OPTIONAL} OPTIONAL,
minimum-space-amplification	[13] IMPLICIT Real OPTIONAL,
maximum-space-amplification	[14] IMPLICIT Real OPTIONAL,
space-adjustment-tracks	[15] IMPLICIT Space-Adjustment-Tracks OPTIONAL,
space-adjustment-track		[16] IMPLICIT Structured-Name OPTIONAL, 
scores				[17] IMPLICIT Scores OPTIONAL,
variant-scripts			[18] IMPLICIT Variant-Scripts OPTIONAL,
minimum-line-to-line-spacing	[19] IMPLICIT Real OPTIONAL,
minimum-anamorphic-scaling	[20] IMPLICIT Real OPTIONAL,
maximum-anamorphic-scaling	[21] IMPLICIT Real OPTIONAL,
modal-character-attributes		[22] IMPLICIT Modal-Character-Attributes OPTIONAL,
non-ISO-attributes			[23] IMPLICIT SET OF Non-ISO-Attribute OPTIONAL}

Space-Adjustment-Tracks		::= SEQUENCE {
default-name-prefix		[0] IMPLICIT Default-Name-Prefix OPTIONAL,
tight				[1] IMPLICIT Track OPTIONAL,
normal				[2] IMPLICIT Track OPTIONAL,
loose				[3] IMPLICIT Track OPTIONAL,
non-ISO-tracks			[4] IMPLICIT SET OF SEQUENCE {
track-name			[0] IMPLICIT Structured-Name ,
track				[1] IMPLICIT Track } OPTIONAL }

Track				::=SEQUENCE {
slope				[0] IMPLICIT Real OPTIONAL,
constant				[1] IMPLICIT Real OPTIONAL}

Scores				::=SEQUENCE {
default-name-prefix		[0] IMPLICIT Default-Name-Prefix OPTIONAL,
rightward			[1] IMPLICIT Score OPTIONAL,
leftward				[2] IMPLICIT Score OPTIONAL,
through				[3] IMPLICIT Score OPTIONAL,
non-ISO-scores			[4] IMPLICIT SET OF SEQUENCE {
score-name			[0] IMPLICIT Structured-Name ,
score				[1] IMPLICIT Score } OPTIONAL}

Score				::=SEQUENCE {
position				[0] IMPLICIT Real OPTIONAL,
thickness				[1] IMPLICIT Real OPTIONAL}

Variant-Scripts			::=SEQUENCE {
default-name-prefix		[0] IMPLICIT Default-Name-Prefix OPTIONAL,
leftward				[1] IMPLICIT Variant-Script OPTIONAL,
rightward			[2] IMPLICIT Variant-Script OPTIONAL,
non-ISO-scripts			[3] IMPLICIT SET OF SEQUENCE {
script-name			[0] IMPLICIT Structured-Name ,
script				[1] IMPLICIT Variant-Script } OPTIONAL}

Variant-Script			::= SEQUENCE {
x-offset				[0] IMPLICIT Real OPTIONAL,
y-offset				[1] IMPLICIT Real OPTIONAL,
x-scale				[2] IMPLICIT Real OPTIONAL,
y-scale				[3] IMPLICIT Real OPTIONAL}

Modal-Character-Attributes		::=SEQUENCE {
default-name-prefix		[0] IMPLICIT Default-Name-Prefix OPTIONAL,

characters			[1] IMPLICIT SET OF SEQUENCE {
default-name-prefix		[0] IMPLICIT Default-Name-Prefix OPTIONAL,
character-name			[1] IMPLICIT Structured-Name ,
position-x			[2] IMPLICIT Real OPTIONAL,
position-y			[3] IMPLICIT Real OPTIONAL,
escapement-x			[4] IMPLICIT Real OPTIONAL,
escapement-y			[5] IMPLICIT Real OPTIONAL,
forward-extent			[6] IMPLICIT Real OPTIONAL,
left-extent			[7] IMPLICIT Real OPTIONAL,
backward-extent			[8] IMPLICIT Real OPTIONAL,
right-extent			[9] IMPLICIT Real OPTIONAL,
char-pair-esc-adjusts		[10] IMPLICIT Char-Pair-Esc-Adjusts OPTIONAL,
forward-class-pairwise-class	[11] IMPLICIT INTEGER OPTIONAL,
backward-class-pairwise-class	[12] IMPLICIT INTEGER OPTIONAL,
forward-sector-space-adjusts	[13] IMPLICIT SEQUENCE OF Real OPTIONAL,
backward-sector-space-adjusts	[14] IMPLICIT SEQUENCE OF Real OPTIONAL,
amplification			[15] IMPLICIT INTEGER OPTIONAL,
correction			[16] IMPLICIT INTEGER OPTIONAL,
tracked-space-adjust-indicator	[17] IMPLICIT INTEGER OPTIONAL,
frequency-of-use			[18] IMPLICIT Real OPTIONAL,
non-ISO-attributes			[19] IMPLICIT SET OF Non-ISO-Attribute OPTIONAL}
								          }

Char-Pair-Esc-Adjusts		::= SEQUENCE  {
default-name-prefix		[0] IMPLICIT Default-Name-Prefix OPTIONAL,
successor-characters		[1] IMPLICIT SET OF SEQUENCE {
successor-character-name		[0] IMPLICIT Structured-Name ,
adjustment-x			[1] IMPLICIT Real OPTIONAL,
adjustment-y			[2] IMPLICIT Real OPTIONAL} }

Character-Shape-Technologies	::= SEQUENCE  {
default-name-prefix		[0] IMPLICIT Default-Name-Prefix OPTIONAL,
bitmap				[1] IMPLICIT Technology-Attributes OPTIONAL,
straight-line-outline			[2] IMPLICIT Technology-Attributes OPTIONAL,
circular-outline			[3] IMPLICIT Technology-Attributes OPTIONAL,
conic-outline			[4] IMPLICIT Technology-Attributes OPTIONAL,
bezier-outline			[5] IMPLICIT Technology-Attributes OPTIONAL,
mixed				[6] IMPLICIT Technology-Attributes OPTIONAL,
non-ISO-technologies		[7] IMPLICIT SET OF SEQUENCE  {
technology-name			[0] IMPLICIT Structured-Name ,
technology-attributes		[1] IMPLICIT Technology-Attributes }
								OPTIONAL}

Technology-Attributes		::= SEQUENCE  {
default-name-prefix		[0] IMPLICIT Default-Name-Prefix OPTIONAL,
fast-scan-direction			[1] IMPLICIT Real OPTIONAL,
slow-scan-direction		[2] IMPLICIT INTEGER OPTIONAL,
fast-scan-resolution		[3] IMPLICIT Real OPTIONAL,
slow-scan-resolution		[4] IMPLICIT Real OPTIONAL,
non-ISO-Attributes		[5] IMPLICIT SET OF Non-ISO-Attribute OPTIONAL}

Non-ISO-Attribute			::= SEQUENCE  {
attribute-name			[0] IMPLICIT Structured-Name ,
attribute-value			[1] IMPLICIT ANY }

Structured-Name			::= CHOICE  {
complete-structured-name		[0] IMPLICIT SEQUENCE OF Identifier,
abbreviated-structured-name	[1] IMPLICIT Identifier}  --to which
				      -- is logically prepended
				      -- the nearest enclosing 
				      -- default-name-prefix

Default-Name-Prefix		::= SEQUENCE OF Identifier
				     -- which is logically prepended to any
 				     -- enclosed abbreviated-structured-name

Identifier				::= CHOICE {
integer				[0] IMPLICIT INTEGER,
character-sequence		[1] IMPLICIT OCTET STRING}
				     --character set as per
				     --Part 2 of this standard,
				     -- characters encoded per ISO 646

Real				::= SEQUENCE {
numerator 			[0] IMPLICIT INTEGER ,
denominator			[1] IMPLICIT INTEGER OPTIONAL}
					--denominator defaults to one

END



	

