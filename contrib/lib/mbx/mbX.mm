.de iX
.am xX
@@eindexentry{\$1;\n%}
\..
..
.de dX
.if \nx{\
.br
.nf
INDEX_XEDNI_INDEX_XEDNI_INDEX_XEDNI_INDEX_XEDNI
.xX
XEDNI_INDEX_XEDNI_INDEX_XEDNI_INDEX_XEDNI_INDEX
.}
..
.em dX
.AF ""
.MT 4
.P
.rs
.DS C O
.sp 1i
.B
\&MultiByte X Proposal
.R
.sp 1i
.sp 2
.sp 1i
\&F. Rojas
\&AIX Systems Development
\&Asia/Pacific Technical Operations
\&IBM Japan, Ltd.
.sp 0.5i
.DE
.SK
.bp
/***********************************************************
Copyright International Business Machines Corporation 1989

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
.bp

.bp
.H 1 "The MultibyteX Proposal"
.P
\c
.B
Acknowledgments\c
.R \c
\&
.P
The following persons should be acknowledged for contributing
to the design of this proposal:
.BL
.LI
Dick Verburg and Ann Leonard for defining the original
algorithm to map multibyte codepoints to glyph indexes.
.LI
Akio Kido and Gary Miller for their expertise on NLS and
code set processing.  Its a true art.
.LI
Eiji Tosaka for being the first user of the MultibyteX.
Jackie Drane for her support of a code set independent designs.
.LI
Makoto Nishino for starting the entire concept of 
code set independence input under AIX and providing the
original drafts for the AIX Input Methods.  Thanks.
.LI
The XMLTALK FORUM which provided a knowledge far beyond 
any single person.  And to its participants who are the 
source of that knowledge.
.LE 1
.bp
.H 2 "Overview"
.P
The MultibyteX proposal defines a set of interfaces that 
provide encoding mapping from the
system's locale code set to X font indexes.  
Its goals are to :
.AL
.LI
Support mapping from single & multibyte character strings 
to a fonts code set.
.LI
Provide mapping from a single code set to multiple fonts.  
MultibyteX will incorporate the concept of character set (CS) 
identifiers with each identifier being associated with a
font. No further knowledge of CS is defined by MultibyteX.
.LI
Support XLFD standard CHARSET_ENCODING that are registered.
(in future releases)
.LI
Conform to existing multibyte functions found in ANSI-C.
.LE 1
.P
MultibyteX will use the LC_CTYPE category (used by the setlocale())
to affect the behavior of the MultibyteX functions.
For any given codepoint within a LC_CTYPE locale the MultibyteX 
functions will identify the 
character set identifier and font index.  The character set identifier will
be mapped to a particular font file under X.
The number of CS identifiers and association to font code set is
locale specific.
There shall be no limit placed on the number of character set identifiers 
supported by a particular locale ( As in the case of the Taiwan SEED
project they are defining 16 planes of double octet code
points.)
Character set identifiers will be simply numbered sequentially,
CS0/CS1/CS2/..., with CS0 always being assumed to be ASCII.
.P
The current version, 1.1, is a prototype version that has its
encoding tables linked in with the program and has some 
setlocale stub code included for systems that don't have such
functions.  Future work should be considered for:
.ML \& 1
.LI
Placing the encoding tables under some NLS directory.
I suggest /usr/lib/X11/nls/$LC_CTYPE.mbX.
.LI
Perform XLFD validation of encoding information in the fonts.
Plus more convenience functions, eg. mbXDrawLineUndeline.
.LE
p.
.P
.H 2 "Functional Description"
.P
The following define the interfaces provided.  In general, all MultibyteX
functions perform simiiar to the Xlib function it is dependent on.
Refer to Xlib Reference manual for list of Errors.
.P
.bp
\c
.B
Name\c
.R \c
\&
.DS L
  mbXLoadQueryFont - load(s) font and fill structure
.DE
\c
.B
Synopsis\c
.R \c
\&
.DS L
  XFontStruct **mbXLoadQueryFont( display, fontlist)
	Display *display;
  	char *fontlist;
.DE
\c
.B
Arguments\c
.R \c
\&
.DS L
  fontlist	Specifies the font(s) to be associated with the current
		LC_CTYPE locale.

		The format of the fontlist is:
			"font [font|-]..."
.DE
\c
.B
Description\c
.R \c
\&
.P
mfXLoadQueryFont loads the necessary files and
verifies that the fonts are valid for a the particular
locale.  Verification is implementation specific but the
following is suggested.
.P
Each locale will contain a set of encoding it supports, these
will be matched with the CHARSET_ENCODING of the particular
font.
If the locale does not match the CHARSET_ENCODING
of a particular font mfXLoadQueryFont will fail.
.P
If successful, mfXLoadQueryFont returns a pointer to a table of
XFontStruct pointers which are used by other MultibyteX
functions.  The size of the table returned will be 
determine by the
number of CS identifiers associated with the locale.  If less
fonts are specified then needed the first font becomes the
default for the particular CS.  If any error occurs then NULL
is returned.
The return value should be freed using XtFree
when it is no longer needed.  Refer to XLoadQueryFont.
.P
Each font is assigned sequentially according to it's
relative position in the fontlist string.  (NOTE: fontlist
should conform to XLFD naming conventions in the future. Since
XLFD allows spaces, "font" needs to be defined better.)
.P
A '-' in the fontlist is used as a place holder and
causes the first font in the font list to be used
as a default.
.bp
\c
.B
Name\c
.R \c
\&
.DS L
  mbXsetlocal - resets the MultibyteX functions to a new locale
.DE
\c
.B
Synopsis\c
.R \c
\&
.DS L
  char *mbXsetlocale( locale )
  	char *locale;
.DE
\c
.B
Arguments\c
.R \c
\&
.DS L
  locale        Specifies the new locale affecting the
		MultibyteX functions.
.DE
\c
.B
Description\c
.R \c
\&
.P
All MultibyteX functions execute under a single locale
until mbXsetlocale is called.  The default locale for the MultibyteX
functions is defined by LC_CTYPE at the time the first call is made.
Setting the locale for multibyte encoding does not change the
system locale.  User still need to call setlocale to do this.
.P
The following call sets the multibyte encoding to
the system define default:
.DS L
    mbXsetlocale(setlocale(LC_CTYPE, NULL));
.DE
.P
mbXsetlocale will return the value of the current mbXlocale.
Also note that no validation of loaded fonts is performed.
.bp
\c
.B
Name\c
.R \c
\&
.DS L
  mbXFreeFont - free fonts associated with a fontlist.
.DE
\c
.B
Synopsis\c
.R \c
\&
.DS L
  mbXFreeFont( display, fontlist)
	Display *display;
  	XFontStruct **fontlist;
.DE
\c
.B
Arguments\c
.R \c
\&
.DS L
  fontlist	Specifies a pointer to a list of XFontStruct
		structures returned from mbXLoadQueryFont.
.DE
\c
.B
Description\c
.R \c
\&
.P
mbXFreeFont will free each of the fonts associated with the 
fontlist.  Refer to XFreeFont.
.bp
\c
.B
Name\c
.R \c
\&
.DS L
  mbXDrawString - draw a multibyte character string
  mbXDrawImageString - draw a multibyte character image string
.DE
\c
.B
Synopsis\c
.R \c
\&
.DS L
  mbXDrawString( display, drawable, gc, x, y, string, length, fontlist)
  mbXDrawImageString( display, drawable, gc, x, y, string, length, fontlist)
	Display *display;
	Drawable drawable;
	GC gc;
	int x, y;
	char *string;
	int length;  /* in bytes */
  	XFontStruct **fontlist;
.DE
\c
.B
Arguments\c
.R \c
\&
.DS L
  string	Specifies a multibyte character string.
  fontlist	Specifies a pointer to a list of XFontStruct
		structures returned from mbXLoadQueryFont.
.DE
\c
.B
Description\c
.R \c
\&
.P
mbXDrawString parses the string and performs the XDrawString
and/or XDrawString16 operations to draw the multibyte character
sting.  The x position is handled across multiple
CS's and is dependent on the font metrics.  No
validation that all fonts conform to some width rule is
performed.  Refer to XDrawString and XDrawString16.
.P
mbDrawImageString is similar except it uses XDrawImageString(16).
Refer to XDrawImageString and XDrawImageString16.
.bp
\c
.B
Name\c
.R \c
\&
.DS L
  mbXTextWidth - get width in pixels of a multibyte character string
  mbXTextExtents - get sting metrics.
.DE

\c
.B
Synopsis\c
.R \c
\&
.DS L
  mbXTextWidth( fontlist, string, length)
  mbXTextExtents( fontlist, string, length, direction, 
			ascent, descent, overall)
  	XFontStruct **fontlist;
	char *string;
	int lenght;  /* in bytes */
.DE
\c
.B
Arguments\c
.R \c
\&
.DS L
  string	Specifies a multibyte character string.
  fontlist	Specifies a pointer to a list of XFontStruct
		structures returned from mbXLoadQueryFont.
.DE
\c
.B
Description\c
.R \c
\&
.P
mbXTextWidth returns the width in pixels using the
specified fontlist.  Refer to XTextWidth & XTextWidth16.
.P
mbTextExtents performs the same function as XTextExtents but
handles multibyte strings.  The ascent and descent returned
are the maximum found in the font list.  The overall structure
is handled appropriately for the multibyte character string.
Refer to XTextExtents and XTextExtents16.
.bp
\c
.B
Name\c
.R \c
\&
.DS L
  CSid - return CS identifier
.DE
\c
.B
Synopsis\c
.R \c
\&
.DS L
  #include <mbX.h>
  #include <Xlib.h>

  int CSid( string, len, fontindex16)
	char *string;
	int len;
	XChar2b **fontindex; /* RETURN */
.DE
\c
.B
Arguments\c
.R \c
\&
.DS L
  string	Specifies a multibyte character string.
  len		Specifies the length of the string in bytes
  fontindex16	Returns a pointer to a XChar2b index for this code point.
.DE
\c
.B
Description\c
.R \c
\&
.P
CSid returns the character set identifier of the next code point
pointed to by string.  If fontindex is NULL then no index
is returned.  The fontindex16 may used to perform X draw
operations.
.bp
.nr H1 0 1
.nr H2 0 1
.nr H3 0 1
.nr H4 0 1
.nr H5 0 1
.nr H6 0 1
.nr H7 0 1
.HM 1
.rm HX
.de HX
.SK
.ds }0 "
.ds }2 "
.nr ;0 2
.br
..
.nr H1 0 1
.nr H2 0 1
.nr H3 0 1
.nr H4 0 1
.nr H5 0 1
.nr H6 0 1
.nr H7 0 1
.HM 1
.rm HX
.de HX
.SK
.ds }0 "
.ds }2 "
.nr ;0 2
.br
..
.TC
