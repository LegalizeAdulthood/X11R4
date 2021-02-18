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

#ifndef lint
static char rcsid[] = "$Header: /home/reed/grunwald/Iptex/drivers/RCS/imagen1.c,v 1.10 88/07/07 23:25:49 grunwald Exp Locker: grunwald $";
#endif

#include <stdio.h>
#include <signal.h>
#include "libtex/dvistuff.h"
#include "libtex/imagen.h"
#include "libtex/imPcodes.h"

extern int errno;	/* from libc */

/*
 *	We use one of the per-glyph user flags to keep track of whether a
 *	glyph has been loaded into the Imagen.
 *
 *	Font shrinkign uses USR0
 */
#define	GF_LOADED	GF_USR1

int	NextFamilyNumber;	/* next available Imagen glyph-family index */

int PageReversal = 0;
int LandScape = 0;
int Legal = 0;
int Verbose = 1;
int ForcePrint = 0;
int Debug;			/* -D => debug flag */

int	ImHH;			/* Imagen horizontal position */
int	ImVV;			/* Imagen vertical position */
int	ImFamily;		/* Imagen current-font number */

char	*PrintEngine;		/* e.g., canon, ricoh */
struct fontinfo NoFont;		/* a fake font to help get things started */

char	*getenv(), *malloc();
double atof();

/* Absolute value */
#define ABS(n) ((n) >= 0 ? (n) : -(n))

/* Put a two-byte (word) value to the Imagen */
#define putword(w) (putchar((w) >> 8), putchar(w))

char CenterWidth = 0;
char CenterHeight = 0;

static struct OptionListStruct {
    char *name;
    char *def;
    char *meaning;
} OptionList[] = {
{"-p", "false", "Select page reversal (print last page first)"},
{"+p", "false", "Select no page reversal"},
{"-l", "false", "Print in landscape mode (rotates fonts)"},
{"-legal","false","Use legal (8 by 14.5) inch paper size"},
{"-lm <float>", "1.0", "Alias for ``-sm''"},
{"-sm <float>", "1.0", "Set width of side margin, in inches"},
{"-tm <float>", "1.0", "Set width of top margin, in inches"},
{"-cwd", "false", "Center the document width in the page"},
{"-cht", "false", "Center the document height in the page"},
{"-v", "true", "Describe the document"},
{"+v", "false", "Do not describe the document"},
{"-force", "false", "Force the document to print printed, even with errors"},
{"-m <integer>", "1000", "Select global document magnification"},
{"-r <integer>", "300", "Set printer resolution in Dots Per Inch"},
{"-d <integer>", "3", "Maximum drift from true DVI position"},
{"-e <string> ", "imagen", "Select the type of printer"},
{"-D", "false", "Turn on debugging information"},
{"-help", "", "This display"},
{0, 0, 0}
};

leave()
{
    dviFini();
    exit(1);
}

main(argc, argv)
    int argc;
    register char **argv;
{
    register int c;
    int needHelp = 0;
    double paperHeight;
    double paperWidth;
    int totalDviHeight;
    int totalDviWidth;
    
    dviUserMag = 1000;
    dviMaxDrift = DefaultMaxDrift;
    dviDPI = DefaultDPI;
    dviFileName = 0;
    dviPrintEngine = "canon";
    
    dviHHMargin = dviDPI;	/* stored as DEVs */
    dviVVMargin = dviDPI;	/* stored as DEVs */
    
/*
 *	The options processing has been switched from the getopt
 *	routine to a possibly nastier version which allows
 *	multi-letter options. This is needed for -sm and -tm
 */
    ProgName = argv[0];
    argv++;
    argc--;
    while(argc > 0) {
	if (**argv != '-' & **argv != '+') {
	    dviFileName = *argv;
	} else {
	    if (strcmp(*argv,"-d") == 0) {
		argv++; argc--;
		dviMaxDrift = atoi(*argv);
	    } else if (strcmp(*argv, "-e") == 0) {
		argv++; argc--;
		dviPrintEngine = *argv;
		break;
	    } else if (strcmp(*argv, "-l") == 0) { /* landscape */
		LandScape++;
		dviFontRotation = ROT_RIGHT;
	    } else if (strcmp(*argv, "-m") == 0) { /* magnification */
		argv++; argc--;
		dviUserMag = atoi(*argv);
	    } else if (strcmp(*argv, "-p") == 0) { /* no page reversal */
		PageReversal = 1;
	    } else if (strcmp(*argv, "+p") == 0) { /* no page reversal */
		PageReversal = 0;
	    } else if (strcmp(*argv, "-r") == 0) { /* resolution */
		argv++; argc--;
		dviDPI = atoi(*argv);
	    } else if (strcmp(*argv, "-force") == 0) {
		ForcePrint = 1;
	    } else if (strcmp(*argv, "-v") == 0) {
		Verbose = 1;
	    } else if (strcmp(*argv, "+v") == 0) {
		Verbose = 0;
	    } else if (strcmp(*argv, "-D") == 0) { /* debug */
		Debug++;
	    } else if (strcmp(*argv, "-tm") == 0){ /* top margin, inches */
		argv++; argc--;
		dviVVMargin = atof(*argv) * dviDPI;
	    } else if (strcmp(*argv, "-sm") == 0 || strcmp(*argv, "-lm") == 0){
		argv++; argc--;
		dviHHMargin = atof(*argv) * dviDPI;
	    } else if (strcmp(*argv, "-cwd") == 0){
		CenterWidth = 1;
	    } else if (strcmp(*argv, "-cht") == 0){
		CenterHeight = 1;
	    } else if (strcmp(*argv, "-legal") == 0){
		Legal = 1;
	    } else {
		needHelp = 1;
	    }
	}
	
	argv++; argc--;
    }
    
    if (needHelp) {
	int i;
	fprintf(stderr,"Usage: %s [options] file[.dvi]\n",
		ProgName);
	fprintf(stderr, "%-15s| %-7s| %s\n", "Option", "Default", "Meaning");
	fprintf(stderr, "-------------------------------------------------\n");
	for (i = 0; OptionList[i].name != 0; i++) {
	    fprintf(stderr, "%-15s| %-7s| %s\n",
		    OptionList[i].name,
		    OptionList[i].def,
		    OptionList[i].meaning);
	}
	fprintf(stderr,"\nArguments must be provided. The default value is\n");
	fprintf(stderr,"used if the option is not specified\n");
	(void) fflush(stderr);
	exit(1);
    }
    
    if (dviInit()) {
	if (dviFileName == 0) {
	    error(1, errno, "Error openning stdin");
	} else {
	    error(1, errno, "Error openning %s", dviFileName);
	}
	leave(1);
    }

/*
 *	Set up signal handlers to clean up.
 */
    signal(SIGHUP, leave);
    signal(SIGQUIT, leave);
    signal(SIGINT, leave);
    signal(SIGTERM, leave);

    if (Legal) {
	paperHeight = 14;
	paperWidth = 8.5;
    } else {
	paperHeight = 11;
	paperWidth = 8.5;
    }

    if (LandScape) {
	double x = paperHeight;
	paperHeight = paperWidth;
	paperWidth = x;
    }

    if (CenterWidth) {
	int margin = (paperWidth * dviDPI) - (dviWidestPage);
	if (margin > 0) {
	    dviHHMargin = margin / 2;
	}
    }

    if (CenterHeight) {
	int margin = (paperHeight * dviDPI) - (dviTallestPage);
	if (margin > 0) {
	    dviVVMargin = margin / 2;
	}
    }

/*
 *	Margins only occur on *one* side of the paper.
 */

    totalDviWidth  = dviWidestPage + 1 * dviHHMargin;
    totalDviHeight = dviTallestPage + 1 * dviVVMargin;

/*
 *	Tell a story about the document
 */
    if (Verbose) {
	fprintf(stderr,"Without margins, your document is %4.2f inches wide and %4.2f inches tall\n",
		(float) dviWidestPage / (float) dviDPI,
		(float) dviTallestPage / (float) dviDPI);
	fprintf(stderr,"You have margins of %4.2f inches on the left and %4.2f on the top\n",
		(float) dviHHMargin / (float) dviDPI,
		(float) dviVVMargin / (float) dviDPI);
    }

    if (totalDviHeight > (paperHeight * dviDPI) ||
	totalDviWidth > (paperWidth * dviDPI) ) {
	    fprintf(stderr,"WARNING: document (with these margins) bigger than paper (%4.2fw by %4.2fh)\n",
		    paperWidth, paperHeight);
	    if (!ForcePrint) {
		fprintf(stderr,"WARNING: rerun with -force\n");
		leave(1);
	    }
	}
/*
 *	Prepare @document commands for Imagen
 */

    printf("@document(language imPRESS, name \"%s\"", dviFileName);
    if (Legal) {
	printf(",paper legal, inputbin legal");
    }
    printf(")");
    
    if (LandScape) {
	putchar(imP_SetHVSystem);
	putchar(0x55);	/* origin=ulc, h:v=90, h:x=90 */
	putchar(imP_SetAdvDirs);
	putchar(0);	/* main=0 (degrees), secondary=90 */
    }

    if (PageReversal) {
	int i;
	for (i = dviTotalPages - 1; i >= 0; i--) {
	    BeginPage(i);
	}
    } else {
	int i;
	for (i = 0; i < dviTotalPages; i++) {
	    BeginPage(i);
	}
    }

    if (Verbose) {
	fprintf(stderr,"\n");
    }

    putchar(imP_EOF);

    dviFini();
    exit(0);
}

/*
 * Start a page (process a DVI_BOP).
 */
BeginPage(pageNumber)
int pageNumber;
{
    putchar(imP_Page);
    ImHH = 0;
    ImVV = 0;
    ImFamily = -1; /* imP_SetFamily command */
    dviPreparePage(pageNumber);
    if (Verbose) {
	int i;
	(void) fprintf(stderr, "[%ld", dviCount[0][dviCurrentPage]);
	for (i = 1; i < DVI_COUNT; i++) {
	    long v = dviCount[i][dviCurrentPage];
	    if (v == 0) {
		break;
	    }
	    fprintf(stderr,".%ld", v);
	}
	fprintf(stderr, "]");
	(void) fflush(stderr);
    }
    putchar(imP_EndPage);
}

DviFont *
applicationNewFont(f, key)
DviFont *f;
int key;
{
    if (key < 0 || key > MAX_FONTFAMILY) {
	fprintf(stderr,"[%s] bogus key in Newfont = %d\n",
		ProgName, key);
	leave();
    }
    return(f);
}

void
applicationResetFont()
{
}

void
applicationPutChar(hh, vv, charCode)
    int hh;
    int vv;
    int charCode;
{
    register struct glyph *g;
    int x,y;
    int key;

    key = dviCurrentFont -> family;
    g = GLYPH(dviCurrentFont -> f, charCode);
    if ((g->g_flags & GF_LOADED) == 0) {
	ImDownLoadGlyph(charCode, g);
    }
    /* BEGIN INLINE EXPANSION OF ImSetPosition */
    if (ImHH != hh) {
	if (ImHH == hh - 1)
	    putchar(imP_Forw);
	else if (ImHH == hh + 1)
	    putchar(imP_Backw);
	else {
	    putchar(imP_SetHAbs);
	    putword(hh);
	}
	ImHH = hh;
    }
    if (ImVV != vv) {
	putchar(imP_SetVAbs);
	putword(vv);
	ImVV = vv;
    }
    /* END INLINE EXPANSION OF ImSetPosition */

    if (ImFamily != dviCurrentFont->family) {
	putchar(imP_SetFamily);
	putchar(dviCurrentFont->family);
	ImFamily = dviCurrentFont->family;
    }
    putchar(charCode);
    ImHH += g->g_pixwidth;
}

void
applicationDoSpecial(cp)
char *cp;
{
    ImSetPosition(dviHH, dviVV);
    newDoSpecial(cp);
}

/*
 * Draw a rule at the current (hh,vv) position. 
 */
void
applicationSetRule(hh, vv, h, w)
int hh, vv;
int h, w;
{
    /* put the rule out */
    if (ImHH != hh || ImVV != vv)
	ImSetPosition(hh, vv);
    putchar(imP_Rule);
    putword(w);
    putword(h);
    putword(-h + 1);
}

/*
* Download the character c/g in the current font.
*/
ImDownLoadGlyph(c, g)
    int c;
    register struct glyph *g;
{
    register char *p;
    register int i, j, w;
    
    g->g_pixwidth = fromSP(g->g_tfmwidth);
    g->g_flags |= GF_LOADED;
    if (!HASRASTER(g))	/* never load dull glyphs */
	return;
    
    if (dviFontRotation == ROT_NORM) {
	w = 0;
	p = RASTER(g, dviCurrentFont->f, ROT_NORM);
    } else {
	w = 1 << 14;
	p = RASTER(g, dviCurrentFont->f, ROT_RIGHT);
    }
    
    w |= (dviCurrentFont->family << 7) | c;
    
    /* Define the character */
    putchar(imP_DefGlyph);	/* a.k.a. BGLY */
    putword(w);		/* rotation, family, member */
    putword(g->g_pixwidth);	/* advance */
    putword(g->g_width);	/* width */
    putword(g->g_xorigin);	/* left offset */
    putword(g->g_height);	/* height */
    putword(g->g_yorigin);	/* top-offset */
    
/*
 * Now put out the bitmap
 */
    w = (g->g_width + 7) >> 3;
    for (i = g->g_height; --i >= 0;){
	for (j = w; --j >= 0;) {
	    (void) putchar(*p++);
	}
    }

#ifdef UNDEF    
    if (g->g_raster) {	/* XXX */
	free(g->g_raster);
	g->g_raster = NULL;
    }
#endif
    
}

/*
 * Set the Imagen's h & v positions.  It is currently at ImHH, ImVV.
 */
ImSetPosition(h, v)
	register int h, v;
{
    
    if (ImHH != h) {
	if (ImHH == h - 1)
	    putchar(imP_Forw);
	else if (ImHH == h + 1)
	    putchar(imP_Backw);
	else {
	    putchar(imP_SetHAbs);
	    putword(h);
	}
	ImHH = h;
    }
    if (ImVV != v) {
	putchar(imP_SetVAbs);
	putword(v);
	ImVV = v;
    }
}
