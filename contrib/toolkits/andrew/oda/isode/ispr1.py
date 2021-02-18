
ISPR1 DEFINITIONS ::=


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


EXPORTS  Interchange-Data-Element;


IMPORTS Document-Profile-Descriptor
     FROM ISPR2
     Layout-Class-Descriptor, Layout-Object-Descriptor
     FROM ISPR4
     Logical-Class-Descriptor, Logical-Object-Descriptor
     FROM ISPR5
     Presentation-Style-Descriptor, Layout-Style-Descriptor
     FROM ISPR6
     Text-Unit FROM ISPR8 ;


PRINTER print


Interchange-Data-Element
::= CHOICE {
     document-profile               [0] IMPLICIT Document-Profile-Descriptor,
     layout-object-class            [1] IMPLICIT Layout-Class-Descriptor,
     layout-object                  [2] IMPLICIT Layout-Object-Descriptor,
     content-portion                [3] IMPLICIT Text-Unit,
     logical-object-class           [5] IMPLICIT Logical-Class-Descriptor,
     logical-object                 [6] IMPLICIT 
					Logical-Object-Descriptor,
     presentation-style             [7] IMPLICIT Presentation-Style-Descriptor,
     layout-style                   [8] IMPLICIT Layout-Style-Descriptor}
END
