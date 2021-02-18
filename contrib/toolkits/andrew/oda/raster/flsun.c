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
 * flsun.c: FBM Library 0.94 (Beta test) 20-May-89  Michael Mauldin
 *
 * Copyright (C) 1989 by Michael Mauldin.  Permission is granted to
 * use this file in whole or in part provided that you do not sell it
 * for profit and that this copyright notice is retained unchanged.
 *
 * flsun.c: 
 *
 * CONTENTS
 *	read_sun (image, rfile, mstr, mlen)
 *	write_sun (image, wfile)
 *
 * EDITLOG
 *	LastEditDate = Sat May 20 19:08:32 1989 - Michael Mauldin
 *	LastFileName = /usr2/mlm/src/misc/fbm/flsun.c
 *
 * HISTORY
 * 20-May-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *	Fixed problem with odd length rows on reading
 *
 * 07-Mar-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *	Beta release (version 0.9) mlm@cs.cmu.edu
 *
 * 12-Nov-88  Michael Mauldin (mlm) at Carnegie-Mellon University
 *	Created.
 *****************************************************************/

#define TK_TKI_fbm
#define TK_IMPL_fbm
#define TK_GVARS_flsun

#include <fbm.h>
#include <rgsite.h>
#include <rgerr.h>

#undef TK_GVARS_flsun
#undef TK_IMPL_fbm
#undef TK_TKI_fbm


extern int (*fbm_GetByte)();
extern int (*fbm_PutByte)();
extern char *fbm_Parameter;

/****************************************************************
 * write_sun (image, wfile)
 ****************************************************************/

write_sun (image, PutByte, Parameter)
FBM *image;
int (*PutByte)();
char *Parameter;
{ 
  int rval;
  RASHDR rhdr;
  FILE *wfile;
  register int i, j, byte;
  unsigned char *bmp, *rp, *gp, *bp;
  int width, height, plnlen, clrlen, rowlen, depth, bits;

    wfile = (FILE *) 0;
    fbm_PutByte = PutByte;
    fbm_Parameter = Parameter;

  if (image->hdr.planes != 1 && image->hdr.planes != 3)
  { 
    rval = RG_ERROR_INT;
/*"Error, write_sun can only handle images with depth 1 or 3\n" */
    RGError = RG_FORMBAD_err;
    goto LEAVE;
  }

/*
  if (image->hdr.physbits != 8)
  {
    rval = RG_ERROR_INT;
    RGError = RG_FORMBAD_err;
    goto LEAVE;
/ *"Error, write_sun can only handle 8 physical bits per pixel\n"* /
  }
*/

  if (image->hdr.physbits == 1 && (image->hdr.rowlen % 16) != 0)
  {
/*"Error, 1 bit deep files must have rowlen (%d) divisible by 16" */
    rval = RG_ERROR_INT;
    RGError = RG_FORMBAD_err;
    goto LEAVE;
  }

  if (image->hdr.physbits == 8 && image->hdr.bits == 1)
    bits = 1;
  else
    bits = image->hdr.physbits > 1 ? 8 : 1;

/**************
# ifdef DEBUG
  fprintf (stderr, "write_sun: [%dx%d] rowlen %d, planes %d, bits %d, physbits %d, using %d\n",
	   image->hdr.cols,
	   image->hdr.rows,
	   image->hdr.rowlen,
	   image->hdr.planes,
	   image->hdr.bits,
	   image->hdr.physbits,
           bits);
# endif
**************/

  width = image->hdr.cols;
  height = image->hdr.rows;
  rowlen = image->hdr.rowlen;
  plnlen = image->hdr.plnlen;
  clrlen = image->hdr.clrlen;
  depth = bits * image->hdr.planes;

  /* Initialize Sun raster header */
  rhdr.ras_magic = SUN_MAGIC;
  rhdr.ras_width = width;
  rhdr.ras_height = height;
  rhdr.ras_depth = depth;
  rhdr.ras_length = plnlen * bits / 8;
  rhdr.ras_type = RT_STANDARD;
  rhdr.ras_maptype = depth > 8 ? RMT_RAW : clrlen ? RMT_EQUAL_RGB : RMT_NONE;
  rhdr.ras_maplength = clrlen;

  /* Write rasterfile header - note: use Sun byte order */
  put_long (rhdr.ras_magic, wfile, BIG);
  put_long (rhdr.ras_width, wfile, BIG);
  put_long (rhdr.ras_height, wfile, BIG);
  put_long (rhdr.ras_depth, wfile, BIG);
  put_long (rhdr.ras_length, wfile, BIG);
  put_long (rhdr.ras_type, wfile, BIG);
  put_long (rhdr.ras_maptype, wfile, BIG);
  put_long (rhdr.ras_maplength, wfile, BIG);
  
  /* Dump colormap if need be */
  if (clrlen > 0)
  { fbmwrite (image->cm, 1, clrlen, wfile); }
  
  /* Write bytes */
  switch (depth)
  { case 24:	rp = &image->bm[0];
		gp = rp + plnlen;
		bp = gp + plnlen;

		for (i=0; i<plnlen; i++)
		{ FBMPUTC (*rp++, wfile);
		  FBMPUTC (*gp++, wfile);
		  FBMPUTC (*bp++, wfile);
		}
		break;

    case 8:	fbmwrite (image->bm, 1, plnlen, wfile);
		break;

    case 1:	
/************
# ifdef DEBUG
fprintf (stderr, "Writing Sun 1bit file [%dx%d] rowlen %d\n",
		 width, height, rowlen);
# endif
************/
		for (j=0; j<height; j++)
		{ bmp = &(image->bm[j*rowlen]);
		  byte = 0;

		  for (i=0; i<rowlen; i++)
		  { byte = (byte << 1) | (*bmp++ ? 0 : 1);

		    if ((i & 7) == 7)
		    { FBMPUTC (byte, wfile); byte = 0; }
		  }
		}
		break;

    default:	rval = RG_ERROR_INT;
		RGError = RG_FORMBAD_err;
		goto LEAVE;
		/*"Error, write_sun given invalid depth %d bits\n"*/
  }
    rval = RG_SUCCESS_INT;
LEAVE:
  return (rval);
}

/****************************************************************
 * read_sun (image, rfile)
 ****************************************************************/

read_sun (image, GetByte, Parameter, mstr, mlen)
FBM *image;
int (*GetByte)();
char *Parameter;
char *mstr;
int mlen;
{ 
  int rval;
  RASHDR rhdr;
  FILE *rfile;
  int width, height, plnlen, rowlen, clrlen, depth;
  register int i, j, byte;
  unsigned char *bmp, *rp, *gp, *bp;
  int m1, m2, m3, m4;

    rfile = (FILE *) 0;
    fbm_GetByte = GetByte;
    fbm_Parameter = Parameter;

  m1 = NEXTMCH(rfile,mstr,mlen) & 0xff;
  m2 = NEXTMCH(rfile,mstr,mlen) & 0xff;
  m3 = NEXTMCH(rfile,mstr,mlen) & 0xff;
  m4 = NEXTMCH(rfile,mstr,mlen) & 0xff;

  rhdr.ras_magic = (m1 << 24) | (m2 << 16) | (m3 << 8)| (m4);

  if (rhdr.ras_magic != SUN_MAGIC)
  {
    rval = RG_ERROR_INT;
    RGError = RG_NOFORM_err;
    /* "Error, not a Sun raster file (bad magic %08x)\n" */
    goto LEAVE;
  }

  rhdr.ras_width = get_long (rfile, BIG);
  rhdr.ras_height = get_long (rfile, BIG);
  rhdr.ras_depth = get_long (rfile, BIG);
  rhdr.ras_length = get_long (rfile, BIG);
  rhdr.ras_type = get_long (rfile, BIG);
  rhdr.ras_maptype = get_long (rfile, BIG);
  rhdr.ras_maplength = get_long (rfile, BIG);

  /* Check for nonstandard rasterfile formats */
  if (rhdr.ras_type != RT_STANDARD)
  {
    rval = RG_ERROR_INT;
    RGError = RG_FORMBAD_err;
    goto LEAVE;
    /* "Error: rasterfile is not a Sun RT_STANDARD file\n" */
  }

  if (rhdr.ras_maplength > 0 && rhdr.ras_maptype != RMT_EQUAL_RGB)
  { 
    rval = RG_ERROR_INT;
    RGError = RG_FORMBAD_err;
    goto LEAVE;
    /* "Error: color rasterfile is not RMT_EQUAL_RGB\n"); */
  }
  
  if (rhdr.ras_maplength == 0 &&
      rhdr.ras_maptype != RMT_NONE &&
      rhdr.ras_maptype != RMT_RAW)
  { 
    rval = RG_ERROR_INT;
    RGError = RG_FORMBAD_err;
    goto LEAVE;
    /* "Error: black and white rasterfile is not RMT_NONE\n" */
  }

  if (rhdr.ras_depth != 24 && rhdr.ras_depth != 8 && rhdr.ras_depth != 1)
  { 
    rval = RG_ERROR_INT;
    RGError = RG_FORMBAD_err;
    goto LEAVE;
    /* "Error, bits per pixel (%d) must be 1, 8 or 24\n" */
  }

  /* Initialize and allocate input image */  
  width = rhdr.ras_width;
  height = rhdr.ras_height;
  depth = rhdr.ras_depth;
  clrlen = rhdr.ras_maplength;

  if (depth == 1)
  { rowlen = 16 * ((width + 15) / 16);
    plnlen = rowlen * height;
  }
  else
  { rowlen = width;
    if (rowlen & 1) rowlen++;
    
    plnlen = width * height;
  }
  
  /* Check for consitency between colormap and depth */
  if (depth > 8 && clrlen > 0)
  {
    rval = RG_ERROR_INT;
    RGError = RG_FORMBAD_err;
    goto LEAVE;
    /* "Error, input has cmap length %d, but %d bits/pixel\n" */
  }

  /* Initialize image header */
  image->hdr.cols = width;
  image->hdr.rows = height;
  image->hdr.planes = (depth == 24) ? 3 : 1;
  image->hdr.bits = (depth == 24) ? 8 : depth;
  image->hdr.physbits = 8;
  image->hdr.rowlen = rowlen;
  image->hdr.plnlen = plnlen;
  image->hdr.clrlen = clrlen;
  image->hdr.aspect = 1.0;
  image->hdr.title[0] = '\0';
  image->hdr.credits[0] = '\0';

  /* Allocate space */
  alloc_fbm (image);

  /* Read colormap if need be */
  if (clrlen > 0 && (fbmread (image->cm, 1, clrlen, rfile)) != clrlen) 
  {
    rval = RG_ERROR_INT;
    RGError = RG_FORMBAD_err;
    goto LEAVE;
    /* "Error: couldn't read colormap, read %d of %d bytes\n" */
  }

  /* Read bytes */
  switch (depth)
  { case 24:	rp = &image->bm[0];
		gp = rp + plnlen;
		bp = gp + plnlen;

		for (i=0; i<plnlen /* && !feof (rfile)*/ ; i++)
		{ *rp++ = FBMGETC(rfile);
		  *gp++ = FBMGETC(rfile);
		  *bp++ = FBMGETC(rfile);
		}
		
		if (i<plnlen)
		{ 
		/* "Error: %s %d of %d pixels (%d bytes)\n" */
		    rval = RG_ERROR_INT;
		    RGError = RG_FORMBAD_err;
		    goto LEAVE;
		}
		
		break;

    case 8:	if ((fbmread (image->bm, 1, plnlen, rfile)) != plnlen)
		{
		    rval = RG_ERROR_INT;
		    RGError = RG_FORMBAD_err;
		    goto LEAVE;
		}
		break;

    case 1:	for (j=0; j<height; j++)
		{ bmp = &(image->bm[j * rowlen]);
    
		  for (i=0; i<rowlen; i++)
		  { if ((i&7) == 0)
		    { if ((byte = FBMGETC(rfile)) == RG_EOF)
		      { 
			rval = RG_ERROR_INT;
			RGError = RG_FORMBAD_err;
			goto LEAVE;
		/* "Error: EOF on bitmap after %d of %d bytes\n",*/
		      }
		    }
        
		    *bmp++ = (byte & 0x80) ? BLACK : WHITE;
		    byte <<= 1;
		  }
		}
		break;
    default:	rval = RG_ERROR_INT;
		RGError = RG_FORMBAD_err;
		goto LEAVE;
		/* "Invalid depth %d bits\n" */
  }

    rval = RG_SUCCESS_INT;

LEAVE:
  return (rval);
}
