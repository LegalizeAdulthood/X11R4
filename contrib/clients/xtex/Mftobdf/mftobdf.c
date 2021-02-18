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

#include <assert.h>
#include <stdio.h>
#include <ctype.h>

#include "libtex/types.h"
#include "libtex/font.h"

#include "libtex/conv.h"
#include "libtex/dvi.h"
#include "libtex/dviclass.h"
#include "libtex/dvicodes.h"
#include "libtex/postamble.h"
#include "libtex/search.h"
#include "libtex/fio.h"

/* X11 related files */

int	DviBlackness = -1;
int	DviDpi = 300;
int	DviMag = 1000;
int	DviDsz = 1000;
char	*ProgName;
int	ShowSnf = 0;
int	UserMag = 1000;

long Numerator = 25400000;
long Denomonator = 473628272;

int Debug = 0;

Conv Conversion;
float DviFontTfmScale;

#define	GF_SHRUNK	GF_USR0
#define	GLYPH_SHRUNK(g) ((g) -> g_flags & GF_SHRUNK)
#define SHRUNK_GLYPH_BYTES_WIDE(g) ((((g) -> g_width + 15) >> 4) * 2)
#define DEFAULT_BLACKNESS 3

#define TWOTO16	0x10000
#define TWOTO24 16777216
#define PTPI 72.27

main(argc, argv)
int argc;
char **argv;
{
  char *name = 0;
  char *dev = "canon";
  char *fname = 0;
  int arg;
  extern void processFont();
  
  struct font *theFont;
  
  ProgName = argv[0];
  
  for (arg = 1; arg < argc; arg++) {
    char *str = argv[arg];
    if ( strcmp(str, "-scaled") == 0 ) {
      arg++;
      DviMag = atoi( argv[arg] );
    }
    else  if ( strcmp(str, "-dpi") == 0 ) {
      arg++;
      DviDpi = atoi( argv[arg] );
    }
    else  if ( strcmp(str, "-D") == 0 ) {
      Debug = 1;
    }
    else  if ( strcmp(str, "-size") == 0 ) {
      arg++;
      DviDsz = atoi( argv[arg] );
    }
    else  if ( strcmp(str, "-dev") == 0 ) {
      arg++;
      dev = argv[arg];
    }
    else  if ( strcmp(str, "-show") == 0 ) {
      ShowSnf = !ShowSnf;
    }
    else  if ( strcmp(str, "-mag") == 0 ) {
      arg++;
      UserMag = atoi(argv[arg]);
    }
    else  if ( strcmp(str, "-b") == 0 ) {
      arg++;
      DviBlackness = atoi(argv[arg]);
    }
    else if ( str[0] != '-' ) {
      name = argv[arg];
      
      if ( name == 0 ) {
	fprintf(stderr,"%s: you must specify a font name\n", ProgName);
	exit(1);
      }
      else {
	int len = strlen(name);
	int thisMag = DviMag;
	
	/* determine if they've specificd a font file or a font name */
	
	if (strcmp(name + len - 2,"pk") == 0 || strcmp(name + len - 2,"gf")) {
	  
	  /* assume they've set DviDpi, and we just need to get dviMag */
	  
	  char *p = &name[len];
	  
	  while (p > name && *p != '.') p--;
	  if ( *p == '.' ) {
	    long value = atoi(p+1);

	    *p = 0;	/* end name*/
	    thisMag = ((value * 1000 * 1000) / DviDpi) / 1000;
	    
	    if ( Debug || value < (DviDpi * 0.25) ||  value > ( DviDpi * 5) ) {
	      fprintf(stderr, "%s: You have a large font number\n",
		      ProgName);
	      fprintf(stderr, "%s: I see a %d-dpi file magnified %d\n",
		      ProgName, DviDpi, thisMag);
	    }
	  }
	  
	  /* We read the font in at mag and then step it down to user mag, so
	     assume here that usermag is 1000 */
	  
	  SetConversion(DviDpi, 1000, Numerator, Denomonator, 1000);
	  
	  theFont = GetFont(name, thisMag, DviDsz, dev, &fname);
	  if ( theFont == 0 ) {
	    fprintf(stderr,"Unable to locate ``%s''\n", name);
	    continue;
	  }
	  
#ifdef DEBUG
	  if ( fname != 0 ) {
	    fprintf(stderr,"%s: returned name is %s\n", ProgName, fname);
	    fprintf(stderr,"%s: which lives in %s\n",
		    ProgName, theFont -> f_path);
	  }
#endif  /* DEBUG */
	  
	  DviFontTfmScale = ((double)theFont -> f_design_size);
	  DviFontTfmScale *= 1000;
	  DviFontTfmScale /= (double) TWOTO24;
	  
	  processFont( theFont, name, thisMag );
	}
      }
    }
    else {
      fprintf(stderr,"%s: don't grok ``%s''\n", ProgName, argv[arg]);
      exit(1);
    }
  }
  exit(0);
}

/*
 *	The following routine is used to shrink a glyph by some
 *	shrink factor (in the width & height dimension).
 *
 *	These shrunken glyphs are more appropriate for previewers.
 *
 *	To do this, we simple scan the original raster, counting the
 *	number of pixels within a given area. If the number of on pixels is
 *	at least twice the total number of pixels, we turn the pixel in
 *	the shrunken glyph ON, else OFF.
 *
 *	We use a lookup table to hasten the process of counting pixels.
 *
 *	The method used should be independent of byte-order (I think).
 *
 *	You need to define two types. One should be 32-bits long, and
 *	the other 16 bits long.
 */

typedef unsigned char BITS_8;
typedef unsigned short BITS_16;
typedef unsigned long BITS_32;

#define LOOKUP_BYTES	256
#define	LOOKUP_BITS	8
#define LOOKUP_MASK	0xff

static char dviLookUpTable[LOOKUP_BYTES];
static char tableNotInitialized = 1;

struct glyph *
dviShrinkGlyph(gly, font, rotation,shrinkH, shrinkW, blackness, killOld)
struct glyph *gly;
struct font *font;
int rotation;
int shrinkH;
int shrinkW;
int blackness;
int killOld;
{
    int shrunkHeight;
    int shrunkWidth;
    int glyphWide;
    int glyphHigh;
    int glyphAdvance;
    int shrunkAdvance;

    int bytesWide;
    int shrunkBytesWide;

    struct glyph *ngly;

    BITS_16 *shrunkRaster;
    int rasterSize;

    long width;
    double ddvimag;
    long dvimag;

    int x,y;
    BITS_8 *cp;
    BITS_8 *ptr;

    BITS_32 shrinkMask;

    int sampleSize;

    if (gly == 0 || !HASRASTER(gly) || (gly -> g_width < 0) ) {
	return(0);
    }

    if (gly -> g_raster == 0) {
	gly-> g_raster = RASTER(gly, font, rotation);
    }

/*
 *	Initialize the lookup table of the number of bits in a given byte
 */

    if (tableNotInitialized) {
	register int i;
	register int j;
	register int k;
	register int acc;
	for (i = 0; i < LOOKUP_BYTES; i++) {
	    j = i;
	    acc = 0;
	    for (k = 0; j != 0 && k < LOOKUP_BITS ; k++) {
		acc += (j & 1);
		j >>= 1;
	    }
	    dviLookUpTable[i] = acc;
	}
	tableNotInitialized = 0;
    }

    glyphHigh = gly -> g_height;
    glyphWide = gly -> g_width;

/*
 *	The following computes the ``escapement'' or the TFMwidth in
 *	pixels. This converted to the DWIDTH in the bdf files.
 *
 *	This is a screwy point with Toreks library. He changes
 *	the values of the design_size
 *
 */
    ddvimag = font -> f_design_size / 16;
    ddvimag *= font -> f_scaled;
    ddvimag /= 1000;
    dvimag = ROUND(ddvimag);
    width = ScaleOneWidth( gly -> g_rawtfmwidth, dvimag);
    gly -> g_pixwidth = fromSP( width );
    glyphAdvance = (gly -> g_pixwidth);

    shrunkHeight = (glyphHigh + shrinkH - 1) / shrinkH;
    shrunkWidth =  (glyphWide + shrinkW - 1) / shrinkW;
    shrunkAdvance =  (glyphAdvance + shrinkW - 1) / shrinkW;

    bytesWide = (gly -> g_width + 7) >> 3;

    shrunkBytesWide = ((shrunkWidth + 7) >> 3);

    rasterSize = (shrunkHeight + 1) * shrunkBytesWide;
    shrunkRaster = (BITS_16 *) malloc(rasterSize);
    bzero(shrunkRaster, rasterSize);
    ptr = (BITS_8 *) shrunkRaster;
    
    if (shrunkRaster == NULL) {
	fprintf(stderr, "Out of memory!\n");
	exit(1);
    }

    if (Debug) {
      fprintf(stderr, "%5d %10d %10d %10d %10d %10d %10d\n",
	      gly -> g_index,
	      gly -> g_width, gly -> g_height,
	      gly -> g_xorigin, gly -> g_yorigin,
	      shrunkWidth, shrunkHeight);
    }

    for (y = 0; y < glyphHigh; y+= shrinkH) {
	cp = (BITS_8 *) ptr;
	shrinkMask = 0x80;
	for (x = 0; x < glyphWide; x += shrinkW) {
	    int i;
	    int samples;
	    BITS_8 *baseP;
	    int upper;
	    register int thisShrinkW;

	    baseP = (BITS_8 *) gly -> g_raster + (y * bytesWide);

/*
 *	Set the upper limit on the height iteration so we dont count
 *	off the end of the raster
 */

	    upper = y + shrinkH;
	    if (upper > glyphHigh) {
		upper = glyphHigh;
	    }

	    if (x + shrinkW > glyphWide) {
		thisShrinkW = glyphWide - x;
	    } else {
		thisShrinkW = shrinkW;
	    }

	    samples = 0;
	    sampleSize = thisShrinkW * shrinkH;

	    for (i = y; i < upper; i++) {
		register int acc;
		register BITS_8 *p;
		register BITS_8 *ep;
/*
 *	Determine how many bytes our shrink window crosses (we might
 *	overlap on byte-edges)
 */

		p = baseP + (x >> 3);
		ep = baseP + ( (x + thisShrinkW - 1) >> 3);
		baseP += bytesWide;

/*
 *	stuff everything in the accumulator
 */

		acc = 0;
		while (p <= ep) {
		    acc = ((acc << 8) & 0xff) | *p;
		    p++;
		}

/*
 *	clean off the right hand-side of extra bits, then clean off
 *	the left hand side of extra bits, and then count them.
 */

		acc = acc >> ( 7 - ((x + thisShrinkW - 1) & 0x7));
		acc &= ~(-1 << thisShrinkW);
		while (acc != 0) {
		    samples += dviLookUpTable[ acc & LOOKUP_MASK ];
		    acc >>= LOOKUP_BITS;
		}
	    }
/*
 *	If at least 1/blackness of the bits are on, treat this entire sample as
 *	being on.
 */

	    if ((samples * blackness) >= sampleSize) {
		*ptr |= shrinkMask;
	    } else {
		*ptr &= ~ shrinkMask;
	    }
	    shrinkMask >>= 1;
	    if (shrinkMask == 0) {
		shrinkMask = 0x80;
		ptr ++;
	    }
	}
	ptr = (BITS_8 *) (cp + shrunkBytesWide);
    }

/*
 *	Build a new glyph from the shrunken raster
 */

    if (ShowSnf) {
      printf("Old glyph:\n");
      seeGlyph(gly -> g_raster, glyphHigh, bytesWide);
      printf("New glyph:\n");
      seeGlyph(shrunkRaster, shrunkHeight, shrunkBytesWide);
    }

    ngly = (struct glyph *) malloc(sizeof(struct glyph));
    bzero(ngly, sizeof(struct glyph));

    ngly -> g_raster = (char * ) shrunkRaster;
    ngly -> g_un.g_integer = shrunkBytesWide;
    ngly -> g_width = shrunkWidth;
    ngly -> g_pixwidth = shrunkAdvance;
    ngly -> g_height = shrunkHeight;

    ngly -> g_xorigin = gly -> g_xorigin / shrinkH;
    ngly -> g_yorigin = gly -> g_yorigin / shrinkW;

    ngly -> g_flags |= GF_SHRUNK;	/* yes, its been shrunk */

    if ( killOld && gly -> g_raster ) {
      free ( gly -> g_raster );
      gly -> g_raster = 0;
    }

    return(ngly);
}

seeGlyph(c, h, w)
char *c;
int h;
int w;
{
    int i,j;

    for (i = 0; i < h; i++ ) {
	for (j = 0; j < w; j++) {
	    int k;
	    register int ch;
	    register int m;
	    char str[9];

	    ch = *(c++);
	    m = 0x80;
	    for (k = 0; k < 8; k++) {
		str[k] = ( (ch & m) ? '#' : '-' );
		m >>= 1;
	    }
	    str[8] = 0;
	    printf("%s", str);
	}
	printf("\n");
    }
}

printhex(f,n)
FILE *f;
int n;
{
  fputc("0123456789abcdef"[(n >> 4) & 0xf], f);
  fputc("0123456789abcdef"[n & 0xf], f);
}

/*	SNF related junk */

void
processFont(f, name, mag)
struct font *f;
char *name;
int mag;
{
  struct glyph *g;
  int c;
  int blackness;

  int maxOverlap;
  int nnonexistchars;

#define MAX_CHARACTER	127

  int glyphBytes[ MAX_CHARACTER + 1];
  struct glyph *shrunkGlyphs[ MAX_CHARACTER + 1];

  int totalGlyphs;
  int pointSize;
  int shrink;

  int min_width = TWOTO16;
  int max_width = -TWOTO16;

  int min_height = TWOTO16;
  int max_height = -TWOTO16;

  int min_xorigin = TWOTO16;
  int max_xorigin = -TWOTO16;

  int min_yorigin = TWOTO16;
  int max_yorigin = -TWOTO16;

#define MIN(x,y) ((x < y) ? (x) : (y))
#define MAX(x,y) ((x < y) ? (y) : (x))

#ifdef __STDC__
#define MINMAX(x,y) { min_##x = MIN(min_##x,y); max_##x = MAX(max_##x,y); }
#else
#define MINMAX(x,y) { min_/**/x = MIN(min_/**/x,y); max_/**/x = MAX(max_/**/x,y); }
#endif

  char newFileName[256];
  char newName[256];
  FILE *fontOut;
  int i;

  double suffMag = (double) mag / (double) DviDsz;
  int scaled = ROUND( (suffMag * 1000) );
  double dsuff = ((double) DviDpi * UserMag * mag) / ((double) 1000.0 * 1000.0) ;
  int suff = ROUND(dsuff);

  if ( f -> f_design_size == 0 ) {
    return;
  }
    
  shrink = ROUND(1000.0 / UserMag);
  if (shrink < 1 ) {
    shrink = 1;
  }
  else if ( shrink > 10 ) {
    shrink = 10;
  }
  
  blackness = DviBlackness;
  blackness = (blackness < 1) ? DEFAULT_BLACKNESS : blackness;

  sprintf(newName, "%s.%d", name, suff);
  sprintf(newFileName, "%s.bdf", newName);

  fontOut = fopen(newFileName,"w");

  if (! fontOut  ) {
    perror("fopen");
    exit(1);
  }

  totalGlyphs = 0;

  if ( Debug ) {
    fprintf(stderr,"Font runs from %d to %d\n", f -> f_lowch, f -> f_highch);
    fprintf(stderr, "%5s %10s %10s %10s %10s %10s %10s\n",
	    "Glyph", "Width", "Height", "Xorigin", "Yorigin",
	    "SWidth", "SHeight");
  }

  for( c = f -> f_lowch; c < f -> f_highch; c++ ) {
    g = GLYPH(f,c);
    shrunkGlyphs[c] = 0;

    if (GVALID(g) && HASRASTER(g)) {
	struct glyph *s;

	s = dviShrinkGlyph( g, f, ROT_NORM, shrink, shrink, blackness, 1);
	shrunkGlyphs[c] = s;

	if (s == 0 )
	  continue;

	totalGlyphs++;

	MINMAX(width, s->g_width);
	MINMAX(height, s->g_height);
	MINMAX(xorigin, s->g_xorigin);
	MINMAX(yorigin, s->g_yorigin);

    }
  }

  fprintf(fontOut, "STARTFONT 2.1\n");
  fprintf(fontOut,
	  "COMMENT This font image translated from Metafont\n");
  fprintf(fontOut, "COMMENT Input file was ``%s''\n", name);
  fprintf(fontOut, "COMMENT DPI was ``%d'', usermag ``%d'', blackness ``%d''\n",
	  DviDpi, UserMag, blackness);
  fprintf(fontOut, "COMMENT shrink was %d\n", shrink);

  fprintf(fontOut, "FONT %s\n", newName);

  pointSize = (f -> f_design_size) / (TWOTO16 * 16);
  fprintf(fontOut, "SIZE %d %d %d\n", pointSize, suff, suff);

  fprintf(fontOut, "FONTBOUNDINGBOX %d %d %d %d\n",
	  max_width + 1, max_height,
	  -max_xorigin, max_yorigin - max_height + 1);

  /* These follow the X conventions */

  fprintf(fontOut,"STARTPROPERTIES 3\n");
  fprintf(fontOut,"FONT_ASCENT %d\n",
	  max_height - min_yorigin);
  fprintf(fontOut,"FONT_DESCENT %d\n",
	  min_yorigin);
  fprintf(fontOut,"COPYRIGHT \"Public Domain\"\n");
  fprintf(fontOut,"ENDPROPERTIES\n");
  fprintf(fontOut,"CHARS %d\n", totalGlyphs);

  for( c = f -> f_lowch; c < f -> f_highch; c++ ) {
    if ( shrunkGlyphs[c] != 0 ) {
      int column;
      int width;
      float swidth;
      int bytes;
      int byte;
      char *p;
      struct glyph *s = shrunkGlyphs[c];

      if ( s -> g_raster == 0 ) {
	continue;	/* to next one */
      }

      fprintf(fontOut, "STARTCHAR C%03d\n",c);
      fprintf(fontOut, "ENCODING %d\n",c);

      width = s -> g_pixwidth;

      swidth = (double) ( width * 72000 ) / (double) ( pointSize * DviDpi );
      fprintf(fontOut, "SWIDTH %d %d\n", (int) swidth, 0);
      fprintf(fontOut, "DWIDTH %d %d\n", width, 0);

      fprintf(fontOut, "BBX %d %d %d %d\n",
	      s -> g_width,	/* width, height of bounding box */
	      s -> g_height,
	      -s -> g_xorigin,	/* offset of origin w.r.t. lower left corner*/
	      s -> g_yorigin - s -> g_height + 1);
      fprintf(fontOut, "BITMAP\n");

      column = 0;
      width = s -> g_un.g_integer;
      bytes = width * s -> g_height;
      for( p = s -> g_raster, byte = 0; byte < bytes; byte++, p++, column++ ) {
	if (column >= width) {
	  fprintf(fontOut,"\n");
	  column = 0;
	}
	printhex(fontOut,*p);
      }
      if (column == width) {
	fprintf(fontOut,"\n");
      }
      fprintf(fontOut, "ENDCHAR\n");

      free( s -> g_raster);
    }
  }
  fprintf(fontOut, "ENDFONT\n");
  fclose( fontOut );
}
