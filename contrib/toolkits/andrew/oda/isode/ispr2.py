
ISPR2 DEFINITIONS ::=


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


EXPORTS Document-Profile-Descriptor;


IMPORTS Resource-Name, Object-or-Class-Identifier
        FROM ISPR3
        Measure-Pair, Transparency,Colour,One-Of-Four-Angles,
        Border, Medium-Type FROM ISPR4
        Protection FROM ISPR5
        Content-Architecture-Class, Content-Type, Block-Alignment, Fill-Order
        FROM ISPR6

        Type-of-Coding FROM  ISPR8

	Character-Coding-Attributes
	FROM ISPR10
        Character-Content-Defaults, Char-Presentation-Features
        FROM ISPR11

	Raster-Gr-Coding-Attributes
	FROM ISPR13
        Raster-Gr-Content-Defaults, Ra-Gr-Presentation-Features
        FROM ISPR14

	Geo-Gr-Coding-Attributes
	FROM ISPR16        
	Geo-Gr-Content-Defaults, Geo-Gr-Presentation-Features
	FROM ISPR17

	Font-Reference
	FROM ISPR18;


PRINTER print


Document-Profile-Descriptor      ::= SET {
     generic-layout-structure       [0] IMPLICIT NumericString OPTIONAL,
     specific-layout-structure      [1] IMPLICIT NumericString OPTIONAL,
     generic-logical-structure      [4] IMPLICIT NumericString OPTIONAL,
     specific-logical-structure     [5] IMPLICIT NumericString OPTIONAL,
     presentation-styles            [6] IMPLICIT NumericString OPTIONAL,
     layout-styles                  [7] IMPLICIT NumericString OPTIONAL,

        -- each of theae numeric strings consists of a single character;
        -- 'partial' is represented by "O", and 'complete' or 'present'
        -- is represented by "1".

     external-document-class        [9] Document-Reference OPTIONAL,
     resource-document              [10] Document-Reference OPTIONAL,
     resources                      [11] IMPLICIT SET OF SET {
        resource-identifier             Resource-Name,
        object-class-identifier         Object-or-Class-Identifier} OPTIONAL,
     document-characteristics       [2] IMPLICIT Document-Characteristics
                                                                       OPTIONAL,
     document-management-attributes [3] IMPLICIT Document-Management-Attributes
                                                                       OPTIONAL}
Document-Characteristics        ::= SET {
    document-application-profile    CHOICE{
                                    [0] IMPLICIT INTEGER {
                                        group-4-facsimile (2)},
                                    [4] IMPLICIT OBJECT IDENTIFIER} OPTIONAL, 
    doc-appl-profile-defaults       [10] IMPLICIT Doc-Appl-Profile-Defaults
                                                                      OPTIONAL,
    document-architecture-class     [1] IMPLICIT INTEGER{ 
					formatted (0), 
					processable (1), 
					formatted-processable (2)} DEFAULT 2,
    content-architecture-classes    [5] IMPLICIT SET OF OBJECT IDENTIFIER
                                                                      OPTIONAL,
    interchange-format-class        [6] IMPLICIT INTEGER { 
					if-a (0), 
					if-b (1)} DEFAULT 1,
    oda-version                     [8] IMPLICIT SEQUENCE {
	standard-or-recommendation      Character-Data,
	publication-date		Date-and-Time} OPTIONAL,
    non-basic-doc-characteristics   [2] IMPLICIT Non-Basic-Doc-Characteristics
                                                                      OPTIONAL,
    non-basic-struc-characteristics [3] IMPLICIT Non-Basic-Struc-Characteristics
                                                                      OPTIONAL,
    additional-doc-characteristics  [9] IMPLICIT Additional-Doc-Characteristics
                                                                      OPTIONAL}


Doc-Appl-Profile-Defaults    ::= SET {
    document-architecture-defaults  [0] IMPLICIT Document-Architecture-Defaults
                                                                      OPTIONAL,
    character-content-defaults      [1] IMPLICIT Character-Content-Defaults
                                                                      OPTIONAL,
    raster-gr-content-defaults      [2] IMPLICIT Raster-Gr-Content-Defaults
                                                                      OPTIONAL,
    geo-gr-content-defaults         [3] IMPLICIT Geo-Gr-Content-Defaults
                                                                      OPTIONAL,

        -- Character-Content-Defaults is defined in part 6
        -- Raster-Gr-Content-Defaults is defined in part 7
        -- Geo-Gr-Content-Defaults is defined in part 8
        -- the following tags are reserved for additional types
        -- of content defaults:
            -- [4] composite-graphics
            -- [5] audio
            -- [6] dynamic-graphics
    external-content-architecture-defaults
				    [7] IMPLICIT SEQUENCE OF EXTERNAL OPTIONAL}

Document-Architecture-Defaults ::=  SET {
    content-architecture-class      [0] IMPLICIT Content-Architecture-Class
                                                                      OPTIONAL,
    content-type                    [1] IMPLICIT Content-Type OPTIONAL,
    page-dimensions                 [2] IMPLICIT Measure-Pair OPTIONAL,
    transparency                    [3] IMPLICIT Transparency OPTIONAL,
    colour                          [4] IMPLICIT Colour OPTIONAL,
    layout-path                     [5] IMPLICIT One-Of-Four-Angles OPTIONAL,
    medium-type                     [6] IMPLICIT Medium-Type OPTIONAL,
    block-alignment                 [7] IMPLICIT Block-Alignment OPTIONAL,
    border			    [8] IMPLICIT Border OPTIONAL,
    page-position		    [9] IMPLICIT Measure-Pair OPTIONAL,
    type-of-coding		    [10] Type-of-Coding OPTIONAL}


Non-Basic-Doc-Characteristics   ::= SET {
    profile-character-sets         [5] IMPLICIT OCTET STRING OPTIONAL,
    comments-character-sets        [1] IMPLICIT OCTET STRING OPTIONAL,
    alternative-reper-char-sets     [6] IMPLICIT OCTET STRING OPTIONAL,

        -- each of these octet strings represents a string of escape sequences

    page-dimensions                [2] IMPLICIT SET OF Measure-Pair OPTIONAL,
    medium-types                   [8] IMPLICIT SET OF Medium-Type OPTIONAL,
    layout-paths                   [21] IMPLICIT SET OF One-Of-Four-Angles
                                                                      OPTIONAL,
    transparencies                 [22] IMPLICIT SET OF Transparency OPTIONAL,
    protections                    [23] IMPLICIT SET OF Protection OPTIONAL,
    block-alignments               [24] IMPLICIT SET OF Block-Alignment
                                                                      OPTIONAL,
    fill-orders                    [25] IMPLICIT SET OF Fill-Order OPTIONAL,
    colours			   [26] IMPLICIT SET OF Colour OPTIONAL,
    borders			   [27] IMPLICIT SET OF Border OPTIONAL,
    page-positions		   [28] IMPLICIT SET OF Measure-Pair OPTIONAL,
    types-of-coding		   [29] IMPLICIT SET OF Type-of-Coding OPTIONAL,
    char-presentation-features     [9] IMPLICIT SET OF
                                         Char-Presentation-Features OPTIONAL,
    ra-gr-presentation-features    [4] IMPLICIT SET OF
                                         Ra-Gr-Presentation-Features OPTIONAL,
    geo-gr-presentation-features   [12] IMPLICIT SET OF
                                        Geo-Gr-Presentation-Features OPTIONAL,

        -- the following tags are reserved for additional types
        -- of presentation attributes:
            -- [13] composite-graphics
            -- [14] audio
            -- [15] dynamic-graphics

    character-coding-attributes    [16] IMPLICIT SET OF
                                           Character-Coding-Attributes OPTIONAL,
    ra-gr-coding-attributes        [3] IMPLICIT SET OF
                                               Raster-Gr-Coding-Attributes OPTIONAL,
    geo-gr-coding-attributes       [17] IMPLICIT SET OF
                                               Geo-Gr-Coding-Attributes OPTIONAL,

        -- Character-Coding-Attribute is defined in part 6
        -- Raster-Gr-Coding-Attribute is defined in part 7
        -- Geo-Gr-Coding-Attribute is defined in part 8
        -- the following tags are reserved for additional types
        -- of coding attributes:
            -- [18] composite-graphics
            -- [19] audio
            -- [20] dynamic-graphics
    ext-non-basic-pres-features     [10] IMPLICIT SEQUENCE OF EXTERNAL OPTIONAL,
    ext-non-basic-coding-attributes
				    [11] IMPLICIT SEQUENCE OF EXTERNAL OPTIONAL}


Non-Basic-Struc-Characteristics ::= SET {
    number-of-objects-per-page      [0] IMPLICIT INTEGER OPTIONAL
						}

Additional-Doc-Characteristics ::=  SET {
    unit-scaling                    [3] IMPLICIT SEQUENCE {INTEGER,INTEGER}
                                                                      OPTIONAL,
    fonts-list                      [2] IMPLICIT Fonts-List OPTIONAL}

-- should be but pepy can't handle SET OF SET
--Fonts-List                      ::= SET OF SET {

Fonts-List                      ::= SET OF Fonts-List-Prime

Fonts-List-Prime                ::= SET {
    font-identifier                     INTEGER,
    font-reference                   Font-Reference}

Document-Management-Attributes ::=  SET {
    document-description            [7] IMPLICIT Document-Description OPTIONAL,
    dates-and-times                 [0] IMPLICIT Dates-and-Times OPTIONAL,
    originators                     [1] IMPLICIT Originators OPTIONAL,
    other-user-information          [2] IMPLICIT Other-User-Information
                                                                      OPTIONAL,
    external-references             [3] IMPLICIT External-References OPTIONAL,
    local-file-refernces            [4] IMPLICIT Local-File-References OPTIONAL,
    content-attributes              [5] IMPLICIT Content-Attributes OPTIONAL,
    security-information            [6] IMPLICIT Security-Information OPTIONAL}

Document-Description            ::= SET {
    title                           [0] IMPLICIT Character-Data OPTIONAL,
    subject                         [1] IMPLICIT Character-Data OPTIONAL,
    document-type                   [2] IMPLICIT Character-Data OPTIONAL,
    abstract                        [3] IMPLICIT Character-Data OPTIONAL,
    keywords			    [4] IMPLICIT SET OF Character-Data OPTIONAL,
    document-reference		    [5] Document-Reference OPTIONAL}

Character-Data                  ::= [APPLICATION 3] IMPLICIT OCTET STRING

        -- string of characters from the sets designated by the attribute
        -- "profile character sets", plus carriage return and line feed

Document-Reference		 ::= CHOICE {
   unique-reference		    OBJECT IDENTIFIER,
   descriptive-reference	    Character-Data}


Dates-and-Times                  ::= SET {
     document-date-and-time         [0] IMPLICIT Date-and-Time OPTIONAL,
     creation-date-and-time         [1] IMPLICIT Date-and-Time OPTIONAL,
     local-filing-date-and-time     [2] IMPLICIT SEQUENCE OF Date-and-Time
                                                                      OPTIONAL,
     expiry-date-and-time           [3] IMPLICIT Date-and-Time OPTIONAL,
     start-date-and-time            [4] IMPLICIT Date-and-Time OPTIONAL,
     purge-date-and-time            [5] IMPLICIT Date-and-Time OPTIONAL,
     release-date-and-time          [6] IMPLICIT Date-and-Time OPTIONAL,
     revision-history               [7] IMPLICIT SEQUENCE OF SET {
         revision-date-and-time         [0] IMPLICIT Date-and-Time OPTIONAL,
         version-number                 [1] IMPLICIT NumericString OPTIONAL,
         revisors                       [2] IMPLICIT SET OF SET {
             names                          [0] IMPLICIT SET OF Personal-Name
                                                                      OPTIONAL,
             position                       [1] IMPLICIT Character-Data
                                                                      OPTIONAL,
             organization                   [2] IMPLICIT Character-Data
                                                                      OPTIONAL}
                                                                      OPTIONAL,
         version-reference          [3] Document-Reference OPTIONAL,
         user-comments              [4] IMPLICIT Character-Data OPTIONAL}
                                                                      OPTIONAL}

Date-and-Time                    ::= [APPLICATION 4] IMPLICIT PrintableString

         -- string of characters representing a date and, optionally, a time
         -- in accordance with ISO 86O1

Originators                      ::= SET {
     organizations                  [0] IMPLICIT SET OF Character-Data OPTIONAL,
     preparers                      [1] IMPLICIT SEQUENCE OF SET {
         personal-name                  [0] IMPLICIT Personal-Name OPTIONAL,
         organization                   [1] IMPLICIT Character-Data OPTIONAL}
                                                                      OPTIONAL,
     owners                         [2] IMPLICIT SEQUENCE OF SET {
         personal-name                  [0] IMPLICIT Personal-Name OPTIONAL,
         organization                   [1] IMPLICIT Character-Data OPTIONAL}
                                                                      OPTIONAL,
     authors                        [3] IMPLICIT SEQUENCE OF SET {
         personal-name                  [0] IMPLICIT Personal-Name OPTIONAL,
         organization                   [1] IMPLICIT Character-Data OPTIONAL}
                                                                      OPTIONAL}

Personal-Name                    ::= [APPLICATION 6] IMPLICIT SET {
     surname                            [0] IMPLICIT Character-Data,
     givenname                          [1] IMPLICIT Character-Data OPTIONAL,
     initials                           [2] IMPLICIT Character-Data OPTIONAL,
     title                              [3] IMPLICIT Character-Data OPTIONAL}


Other-User-Information           ::= SET {
     copyright			    [0] IMPLICIT SET OF SET {
         copyright-information          [0] IMPLICIT SET OF Character-Data 
								OPTIONAL,
         copyright-dates                [1] IMPLICIT SET OF Character-Data 
								OPTIONAL}
								OPTIONAL,
     status                         [1] IMPLICIT Character-Data OPTIONAL,
     user-specific-codes            [2] IMPLICIT SET OF Character-Data OPTIONAL,
     distribution-list              [3] IMPLICIT SEQUENCE OF SET {
	 personal-name			[0] IMPLICIT Personal-Name OPTIONAL,
	 organization			[1] IMPLICIT Character-Data OPTIONAL}
								OPTIONAL,
     additional-information         [5] ANY OPTIONAL}

External-References              ::= SET {
     references-to-other-documents  [0] IMPLICIT SET OF Document-Reference
                                                                      OPTIONAL,
     superseded-documents           [1] IMPLICIT SET OF Document-Reference
                                                                      OPTIONAL}

Local-File-References            ::= SET OF SET {
     filename                       [0] IMPLICIT Character-Data OPTIONAL,
     location                       [1] IMPLICIT Character-Data OPTIONAL,
     user-comments                  [2] IMPLICIT Character-Data OPTIONAL}

Content-Attributes               ::= SET {
     document-size                  [1] IMPLICIT INTEGER DEFAULT 10,
     number-of-pages                [2] IMPLICIT INTEGER DEFAULT 100,
     languages                      [4] IMPLICIT SET OF Character-Data OPTIONAL}

Security-Information             ::= SET {
     authorization                   CHOICE {
         person                         [0] IMPLICIT Personal-Name,
         organization                   [4] IMPLICIT Character-Data} OPTIONAL,
     security-classification        [1] IMPLICIT Character-Data OPTIONAL,
     access-rights                  [2] IMPLICIT SET OF Character-Data OPTIONAL}

END
