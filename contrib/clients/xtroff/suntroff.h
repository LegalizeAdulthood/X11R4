/*
 *	SunTroff -  A program to display the output of Device Independent
 *		Troff as a window on a Sun Workstation.
 *
 *	Suntroff.h - A number of defines and structures that are needed
 *		by the rest of the code.
 *
 *	Authors - The original version of this program was written by
 *			Richard L. Hyde (Purdue)
 *			David Slattengren (Berkeley)
 *		It was nearly all rewritten, cleaned up and a more elegant
 *		user interface installed by
 *			Malcolm Slaney (Schlumberger Palo Alto Research)
 *
 *	Legalese -  This command was developed as an independent project
 *		to satisfy a need of the author.  This program may contain
 *		bugs and the user is cautioned to independently verify that
 *		the program is suitable for the user's intended purpose.
 *		The program is made available on an ``as is'' basis with
 *		all faults and without any implied or expressed warranties
 *		or support from either the author, Malcolm Slaney, or
 *		Schlumberger Palo Alto Research Laboratory.
 *
 *		I am putting this program in the Unix domain.  You are
 *		free to use it as you wish.  In return I ask two things.
 *		First, that you do not remove the names of the authors
 *		from this work.  Secondly, if you make changes or
 *		improvements to this program that you pass these back to
 *		the author so that everybody can benefit from the
 *		improvements.
 */

#include	<ctype.h>
#include	<stdio.h>
#include	<string.h>
#ifndef SUNTOOLS
#include	<X11/Xlib.h>	/* Needed for definition of Font */
#include	<X11/Xos.h>	/* To try and make things more portable */
#else	/* SUNTOOLS */
#include	<sys/types.h>	/* Xos.h includes it for X */
#endif /* SUNTOOLS */

#include	"dev.h"

					/* TYPESETTER DEFINES */
					/* The first four defines are probably
					 * the only ones that need to be 
					 * customized for a site.
					 */

					/* Home of all the troff information */
					/* Hint: the standard AT&T location is
					 * /usr/dwb/lib/font
					 */
#ifndef FONTDIR
#define	FONTDIR		"/usr/lib/font"
#endif
					/* Home of all the font bits */
#ifndef BITDIR
#define	BITDIR		"/usr/local/lib/suntroff"
#endif

#ifdef SUNTOOLS
					/* Note the SunTroff program will
					 * append to the end of this string
					 * the printer name.
					 */
# define	LPRCOMMAND	"/usr/ucb/lpr -n -P"
#endif /* SUNTOOLS */

#define	FULLPAGE	0		/* Start up the tool with a width 
					 * that is as large as a page. */

#define	NFONTS		60		/* Number of Allowed Fonts */

#define	MAXNAME		20		/* Length of maximum typesetter name */

#define	MAXSTATE	10		/* Number of states to remember */

					/* Needs to be no larger than
					 * than the size of the
					 * intname character string
					 * in the dev.h/struct font
					 * structure.
					 */
#define	MaxFontName 10

#ifndef	DEFAULT_TYPESETTER
#ifdef SUNTOOLS
#define DEFAULT_TYPESETTER "sun"
#else /* ! SUNTOOLS */
#define	DEFAULT_TYPESETTER "X11"
#endif /* SUNTOOLS */
#endif	DEFAULT_TYPESETTER

#define	WARNING		0		/* Print Message and Return */
#define	FATAL		1		/* Print Message and Leave */

#define		PAGE_WIDTH		8.5
#define		PAGE_HEIGHT		11
#define		PAGE_PIXEL_WIDTH	(int)(SUNRES*PAGE_WIDTH)
#define		PAGE_PIXEL_HEIGHT	(int)(SUNRES*PAGE_HEIGHT)

/*
 * Fast comparison of strings - will often save us a procedure call at the
 * cost of a few extra instructions. Macro has SIDE-EFFECTS
 */
#define STREQ(a, b)	((*(a) == *(b)) && (strcmp(a, b) == 0))

struct FontBitStruct {
	char		Name[MaxFontName];
	int		Size;
#ifdef SUNTOOLS
	struct pixfont	*Bits;
#else SUNTOOLS
	XFontStruct	*Bits;
#endif
	char		*FileName;
	struct FontBitStruct *NextFont;
	struct FontBitStruct *NextSize;
};

struct Typesetter {
	char	Name[MAXNAME];		/* Name of this typesetter */

	struct dev Device;		/* Ditroff Typesetter description */

	short	*PointSizeTable;	/* Map Pointsize number to real # */

	short	*SpecialCharacterNumber;/* Map special character number into
					   an index for Name Array */
	char	*SpecialCharacterName;	/* Names of each special character
					   */
	char	**WidthTable;		/* Width of each character
					   (indexed by font and character) */
	char	**CodeTable;		/* Typesetter code for each char
					   (indexed by font and character) */
	char	**AsciiTable;		/* Mapping between ascii (or special
					   character number) and index into
					   width and code tables above. */
	struct Font *FontInformation[NFONTS];

	struct FontBitStruct FontBitHead; /* All the bits for all the fonts
					     that are known for this
					     typesetter. */
	struct Typesetter *NextTypesetter;
};

extern int	size, font, linmod, linethickness, HorizontalPosition;
extern int	VerticalPosition, DeviceResolution, LineNumber;
extern float	ditsiz;
/*extern int	SUNRES			/* Dots per inch on Sun Screen */

extern char	DeviceName[];

extern struct FontBitStruct *CurrentFont, *VirtualLoadFont();

extern struct Typesetter *UserTypesetter, *OutputTypesetter, *LoadDevice();
