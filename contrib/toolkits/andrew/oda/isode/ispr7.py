
ISPR7 DEFINITIONS ::=


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


EXPORTS Default-Value-Lists-Logical,Default-Value-Lists-Layout;


IMPORTS  Style-Identifier, Layout-Category-Name
        FROM ISPR3
        Measure-Pair, One-Of-Four-Angles, Medium-Type,
	 Transparency, Colour, Border
        FROM ISPR4
	Protection FROM ISPR5
        Presentation-Attributes
        FROM ISPR6;


PRINTER print


Default-Value-Lists-Layout      ::= SET {
    page-attributes                 [2] IMPLICIT Page-Attributes OPTIONAL,
    frame-attributes                [3] IMPLICIT Frame-Attributes OPTIONAL,
    block-attributes                [4] IMPLICIT Block-Attributes OPTIONAL}

Default-Value-Lists-Logical     ::= SET {
    composite-logical-attributes    [5] IMPLICIT Composite-Logical-Attributes
                                                                      OPTIONAL,
    basic-logical-attributes        [6] IMPLICIT Basic-Logical-Attributes
                                                                      OPTIONAL}

Page-Attributes                 ::= SET {
    dimensions                      < Attribute OPTIONAL,
    transparency                    < Attribute OPTIONAL,
    presentation-attributes         < Attribute OPTIONAL,
    layout-path                     < Attribute OPTIONAL,
    page-position                   < Attribute OPTIONAL,
    medium-type                     < Attribute OPTIONAL,
    presentation-style              < Attribute OPTIONAL,
    colour                          < Attribute OPTIONAL}

Frame-Attributes                ::= SET {
    position                        < Attribute OPTIONAL,
    dimensions                      < Attribute OPTIONAL,
    transparency                    < Attribute OPTIONAL,
    layout-path                     < Attribute OPTIONAL,
    permitted-categories            < Attribute OPTIONAL,
    colour                          < Attribute OPTIONAL,
    border                          < Attribute OPTIONAL}

Block-Attributes                ::= SET {
    position                        < Attribute OPTIONAL,
    dimensions                      < Attribute OPTIONAL,
    transparency                    < Attribute OPTIONAL,
    presentation-attributes         < Attribute OPTIONAL,
    presentation-style              < Attribute OPTIONAL,
    colour                          < Attribute OPTIONAL,
    border                          < Attribute OPTIONAL}

Composite-Logical-Attributes    ::= SET {
    protection             	    < Attribute OPTIONAL,
    layout-style                    < Attribute OPTIONAL}

Basic-Logical-Attributes        ::= SET {
    presentation-attributes         < Attribute OPTIONAL,
    protection             	    < Attribute OPTIONAL,
    presentation-style              < Attribute OPTIONAL,
    layout-style                    < Attribute OPTIONAL}

Attribute                       ::= CHOICE {
    position                        [0] IMPLICIT Measure-Pair,
    dimensions                      [1] IMPLICIT Measure-Pair,
    transparency                    [2] IMPLICIT Transparency,
    presentation-attributes         [3] IMPLICIT Presentation-Attributes,
    layout-path                     [4] IMPLICIT One-Of-Four-Angles,
    page-position                   [5] IMPLICIT Measure-Pair,
    medium-type                     [6] IMPLICIT Medium-Type,
    permitted-categories            [7] IMPLICIT SET OF Layout-Category-Name,
    protection             	    [8] IMPLICIT Protection,
    presentation-style              [9] IMPLICIT Style-Identifier,
    layout-style                    [10] IMPLICIT Style-Identifier,
    colour                          [11] IMPLICIT Colour,
    border                          [12] IMPLICIT Border}


END
