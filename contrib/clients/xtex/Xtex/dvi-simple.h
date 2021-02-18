/*
 * Copyright 1989 Dirk Grunwald
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  Dirk
 * Grunwald and M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Dr. Dirk Grunwald
 * 	Dept. of Computer Science
 * 	Campus Box 430
 * 	Univ. of Colorado, Boulder
 * 	Boulder, CO 80309
 * 
 * 	grunwald@colorado.edu
 * 	
 */ 

#include "libtex/types.h"
#include "libtex/dviclass.h"
#include "libtex/dvicodes.h"
#include "libtex/tfm.h"
#include "libtex/fio.h"
#include "libtex/postamble.h"

/*****************************************************************************
 *	Things we read from the DVI file
 */

extern struct PostAmbleInfo ThePostAmbleInfo;

typedef struct {
	struct PostAmbleFont paf;
	int *widths;
	i32 pspace;		/* boundary between `small & `large spaces
				   (for positive horizontal motion) */
	i32 nspace;		/* -4 * pspace, for negative motion */
	i32 vspace;		/* 5 * pspace, for vertical motion */
} DviFontInfo;

#define MAX_POSTAMBLE_FONTS 256
extern int RegisteredFonts;
extern DviFontInfo TheFontInfo[ MAX_POSTAMBLE_FONTS ];

/*****************************************************************************
/*
 *	Defaults and configuration for dvi-simple
 */

#define DEFAULT_DPI	300
#define DEFAULT_HHMARGIN	DEFAULT_DPI
#define DEFAULT_VVMARGIN	DEFAULT_DPI
#define DEFAULT_MAX_DRIFT	3

#define DEFAULT_BLACKNESS 3

#define DVI_COUNT	10	/* count0 .. count9 */

#define SP_PER_POINT (65536)
#define POINTS_PER_INCH (72.27)
#define SP_PER_INCH ( 4736286 )
#define INCH_TO_SP(_inches_) ( (_inches_) * (SP_PER_INCH) )

extern long	*DviCount[DVI_COUNT];
extern long	*DviPageOffsets;

extern int	DviHHMargin;
extern int	DviVVMargin;
extern int	DviDpi;
extern int	DviTallestPage;
extern int	DviWidestPage;
extern int	DviTotalPages;
extern int	DviCurrentPage;
extern int	DviMaxDrift;
extern char 	*DviPrintEngine;
extern int      DviBlackness;
extern int	DviUserMag;

extern FILE	*DviFile;
extern char	*DviFileName;

#ifdef __STDC__
  extern int	DviInit( char *, int );
  extern void	DviFini();
  extern char * DviFetchPage( int );
#else
  extern int	DviInit();
  extern void	DviFini();
  extern char * DviFetchPage();
#endif
