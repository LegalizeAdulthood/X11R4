
ISRD1 DEFINITIONS ::=


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

-- EXPORTS  Interchange-Data-Element;


IMPORTS Document-Profile-Descriptor
     FROM ISRD2
     Layout-Class-Descriptor, Layout-Object-Descriptor
     FROM ISRD4
     Logical-Class-Descriptor, Logical-Object-Descriptor
     FROM ISRD5
     Presentation-Style-Descriptor, Layout-Style-Descriptor
     FROM ISRD6
     Text-Unit FROM ISRD8 ;

DECODER parse


Interchange-Data-Element [[ P struct isode2toolkit_t * ]]

::= CHOICE {

     document-profile

	[0] IMPLICIT Document-Profile-Descriptor [[ p parm ]],

     layout-object-class

	[1] IMPLICIT Layout-Class-Descriptor [[ p parm ]],

     layout-object

	[2] IMPLICIT Layout-Object-Descriptor [[ p parm ]],

     content-portion

	[3] IMPLICIT Text-Unit [[ p parm ]],

     logical-object-class

	[5] IMPLICIT Logical-Class-Descriptor [[ p parm ]],

     logical-object

	[6] IMPLICIT Logical-Object-Descriptor [[ p parm ]],

     presentation-style

	[7] IMPLICIT Presentation-Style-Descriptor [[ p parm ]],

     layout-style

	[8] IMPLICIT Layout-Style-Descriptor [[ p parm ]]

}


END
