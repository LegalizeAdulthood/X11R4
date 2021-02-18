/*\
 *
 *  (C) Copyright 1989 by Carnegie Mellon University
 *
 *  Permission to use, copy, modify, and distribute these programs
 *  and their documentation for any purpose and without fee is
 *  hereby granted, provided that this copyright and permission
 *  notice appear on all copies and supporting documentation, and
 *  that the name of Carnegie Mellon University not be used in
 *  advertising or publicity pertaining to distribution of the
 *  programs without specific prior permission and notice be given
 *  in supporting documentation that copying and distribution is
 *  by permission of Carnegie Mellon University.
 *
 *  Carnegie Mellon University makes no representations about the
 *  suitability of this software for any purpose.  It is provided
 *  as is, without express or implied warranty.
 *
 *  Software by Andrew Werth, Information Technology Center,
 *  Carnegie Mellon University, except where noted.
\*/

/*\
 *  This code has been taken from the Fuzzy Bitmap library, as
 *  noted below.  It has been modified in several ways:  Input/
 *  Output has been changed to match Raster Graphics Tool Kit
 *  specifications; debugging output has been removed; error
 *  handling has been modified for RGTK specs; procedures
 *  have been made static where appropriate; and minor changes
 *  to more closely resemble RGTK coding conventions.
 *
 *  Modifications by Andrew Werth.
 *
\*/

/*****************************************************************
 * flgifw.c: FBM Library 0.9 (Beta test) 07-Mar-89  Michael Mauldin
 *
 * Portions of this code Copyright (C) 1989 by Michael Mauldin.
 * Permission is granted to use this file in whole or in part provided
 * that you do not sell it for profit and that this copyright notice
 * and the names of all authors are retained unchanged.
 *
 * flgifw.c:
 *
 * CONTENTS
 *	write_gif (image, stream, mstr, mlen)
 *
 * HISTORY
 * 07-Mar-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *	Beta release (version 0.9) mlm@cs.cmu.edu
 *
 * 19-Feb-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *	Adapted to FBM package.
 *
 * 13-Feb-89  David Rowley (mgardi@watdcsu.waterloo.edu)
 *	GIF encoding modifications.
 *
 *	Based on: compress.c - File compression ala IEEE Computer, June 1984.
 *
 *	Spencer W. Thomas       (decvax!harpo!utah-cs!utah-gr!thomas)
 *	Jim McKie               (decvax!mcvax!jim)
 *	Steve Davies            (decvax!vax135!petsd!peora!srd)
 *	Ken Turkowski           (decvax!decwrl!turtlevax!ken)
 *	James A. Woods          (decvax!ihnp4!ames!jaw)
 *	Joe Orost               (decvax!vax135!petsd!joe)
 *
 *****************************************************************/

#define TK_TKI_fbm
#define TK_IMPL_fbm
#define TK_GVARS_flgifw

#include <fbm.h>
#include <rgsite.h>
#include <rgerr.h>
#include <rgreal.h>

#undef TK_GVARS_flgifw
#undef TK_IMPL_fbm
#undef TK_TKI_fbm

#ifndef USE_LONG_NAMES

#define GetGIFPixel ggifpx

#endif

static GetGIFPixel();

extern int (*fbm_PutByte)();
extern char *fbm_Parameter;

static unsigned char *pixels = (unsigned char *) 0;
static int rowlen = 0;

write_gif (image, PutByte, Parameter)
FBM *image;
int (*PutByte)();
char *Parameter;
{
  int rval;
  register int i, bits, sum;
  int bkg, clrs;
  unsigned char *red, *grn, *blu;
  int rint[256], gint[256], bint[256];
  FILE *wfile;
    
    wfile = (FILE *) 0;
    fbm_PutByte = PutByte;
    fbm_Parameter = Parameter;

  if (image->hdr.planes > 1 || image->hdr.clrlen == 0)
  {
    rval = RG_ERROR_INT;
    RGError = RG_FORMBAD_err;
    goto LEAVE;
    /* "write_gif can only handle mapped color images\n" */
  }

  clrs = image->hdr.clrlen / 3;
  
  /* Calculate bits per pixel in colormap */
  for (i=clrs, bits=1; i > 2; )
  { i >>= 1; bits++; }

  if (1 << bits != clrs)
  { 
    rval = RG_ERROR_INT;
    RGError = RG_FORMBAD_err;
    goto LEAVE;
    /* "Error, number of colors %d is not a power of 2\n" */
  }
  
  if (bits < 1 || bits > 8)
  {
    rval = RG_ERROR_INT;
    RGError = RG_FORMBAD_err;
    goto LEAVE;
    /* "Error, bits per pixel (%d) must be in range 1..8\n" */
  }
  
  red = image->cm;
  grn = red + clrs;
  blu = grn + clrs;

  pixels = image->bm;
  rowlen = image->hdr.rowlen;
  
    red[0] = 0;		    /* Black */
    grn[0] = 0;
    blu[0] = 0;

    red[1] = MAXIMUM_INT;   /* White */
    grn[1] = MAXIMUM_INT;
    blu[1] = MAXIMUM_INT;

  /* Copy colormap, and find darkest pixel for background */
  { bkg = 0; sum = 1e9;

    for (i=0; i<clrs; i++)
    { rint[i] = red[i];
      gint[i] = grn[i];
      bint[i] = blu[i];

      if (red[i] + grn[i] + blu[i] < sum)
      { bkg = i; sum = red[i] + grn[i] + blu[i]; }
    }
  }

/****************
  fprintf (stderr, "Writing GIF file [%dx%d], %d colors, %d bits, bkg %d\n",
	  image->hdr.cols, image->hdr.rows, clrs, bits, bkg);
# ifdef DEBUG
  fprintf (stderr, "\n\nColormap:\n");
  for (i=0; i<clrs; i++)
  { fprintf (stderr, "%5d: <%3d, %3d, %3d>\n",
	     i, rint[i], gint[i], bint[i]);
  }
# endif
****************/

  rval = (GIFEncode (
	      PutByte,
	      Parameter,
	      wfile,
	      image->hdr.cols,		/* width */
	      image->hdr.rows,		/* height */
	      0,			/* No interlacing */
	      bkg,			/* Index of Backgrounf */
	      bits,			/* Bits Per pixel */
	      rint,			/* Red colormap */
	      gint,			/* Green colormap */
	      bint,			/* Blue colormap */
	      GetGIFPixel ) );		/* Get Pixel value */

LEAVE:
    return(rval);
}

/* Returns value of next pixel */

static GetGIFPixel (x, y)
int x, y;
{
  return (pixels[y * rowlen + x]);
}
