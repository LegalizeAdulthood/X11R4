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
 * fliff.c: FBM Library 0.9 (Beta test) 07-Mar-89  Michael Mauldin
 *
 * Copyright (C) 1989 by Michael Mauldin.  Permission is granted to
 * use this file in whole or in part provided that you do not sell it
 * for profit and that this copyright notice is retained unchanged.
 *
 * fliff.c: 
 *
 * CONTENTS
 *	write_iff (image, stream)
 *	read_iff (image, stream, mstr, mlen)
 *
 * EDITLOG
 *	LastEditDate = Mon Mar 20 18:54:08 1989 - Michael Mauldin
 *	LastFileName = /usr2/mlm/src/misc/fbm/fliff.c
 *
 * HISTORY
 * 07-Mar-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *	Beta release (version 0.9) mlm@cs.cmu.edu
 *
 * 04-Mar-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *	Read and Write support for Amiga IFF (except HAM mode)
 *****************************************************************/

#define TK_TKI_fbm
#define TK_IMPL_fbm
#define TK_GVARS_fliff

#include <fbm.h>
#include <rgsite.h>
#include <rgerr.h>

#undef TK_GVARS_fliff
#undef TK_IMPL_fbm
#undef TK_TKI_fbm

#ifndef USE_LONG_NAMES

#define UnPackRow	unpkrw
#define build_bmhd	blbmhd
#define build_body	blbody
#define build_cmap	blcmap
#define parse_form	prsfor
#define read_iff_body	rdifbd
#define RgStrNCmp	rgsncm

#endif

extern int (*fbm_GetByte)();
extern int (*fbm_PutByte)();
extern char *fbm_Parameter;


/****************************************************************
 * from iff.h
 ****************************************************************/

# define BOOL	int		/*  1 bit value			*/
# define SBYTE	char		/*  8 bits signend		*/
# define UBYTE	unsigned char	/*  8 bits unsigned		*/
# define WORD	short		/* 16 bits signed		*/
# define UWORD	unsigned short	/* 16 bits unsigned		*/
# define LONG	long		/* 32 bits signed		*/
# define ID	long
# define MakeID(a,b,c,d)  ( (LONG)(a)<<24L | (LONG)(b)<<16L | (c)<<8 | (d) )
# define FORM MakeID('F','O','R','M')
# define PROP MakeID('P','R','O','P')
# define LIST MakeID('L','I','S','T')
# define CAT  MakeID('C','A','T',' ')
# define FILLER MakeID(' ',' ',' ',' ')
# define NULL_CHUNK 0L	       /* No current chunk.*/
# define TRUE	1
# define FALSE	0


/* ---------- Chunk ----------------------------------------------*/

/* All chunks start with a type ID and a count of the data bytes
   that follow--the chunk's "logical size" or "data size". If that
   number is odd, a 0 pad byte is written, too.
 */

typedef struct {
    ID	  ckID;
    LONG  ckSize;
    } ChunkHeader;

typedef struct {
    ID	  ckID;
    LONG  ckSize;
    UBYTE ckData[ 1 /*REALLY: ckSize*/ ];
    } Chunk;

/*----------------------------------------------------------------*
 * ILBM.H  Definitions for InterLeaved BitMap raster image.     1/23/86
 *
 * By Jerry Morrison and Steve Shaw, Electronic Arts.
 * This software is in the public domain.
 *
 * This version for the Commodore-Amiga computer.
 *----------------------------------------------------------------*/

# define ID_ILBM MakeID('I','L','B','M')
# define ID_BMHD MakeID('B','M','H','D')
# define ID_CMAP MakeID('C','M','A','P')
# define ID_GRAB MakeID('G','R','A','B')
# define ID_DEST MakeID('D','E','S','T')
# define ID_SPRT MakeID('S','P','R','T')
# define ID_CAMG MakeID('C','A','M','G')
# define ID_BODY MakeID('B','O','D','Y')

/* ---------- BitMapHeader ---------------------------------------*/

typedef UBYTE Masking;		/* Choice of masking technique.*/
# define mskNone                 0
# define mskHasMask              1
# define mskHasTransparentColor  2
# define mskLasso                3

typedef UBYTE Compression;	/* Choice of compression algorithm applied to
     * each row of the source and mask planes. "cmpByteRun1" is the byte run
     * encoding generated by Mac's PackBits. See Packer.h . */
# define cmpNone      0
# define cmpByteRun1  1

/* Aspect ratios: The proper fraction xAspect/yAspect represents the pixel
 * aspect ratio pixel_width/pixel_height.
 *
 * For the 4 Amiga display modes:
 *   320 x 200: 10/11  (these pixels are taller than they are wide)
 *   320 x 400: 20/11
 *   640 x 200:  5/11
 *   640 x 400: 10/11		*/
# define x320x200Aspect 10
# define y320x200Aspect 11
# define x320x400Aspect 20
# define y320x400Aspect 11
# define x640x200Aspect  5
# define y640x200Aspect 11
# define x640x400Aspect 10
# define y640x400Aspect 11

/* A BitMapHeader is stored in a BMHD chunk. */
typedef struct {
    UWORD w, h;			/* raster width & height in pixels */
    WORD  x, y;			/* position for this image */
    UBYTE nPlanes;		/* # source bitplanes */
    Masking masking;		/* masking technique */
    Compression compression;	/* compression algorithm */
    UBYTE pad1;			/* UNUSED.  For consistency, put 0 here.*/
    UWORD transparentColor;	/* transparent "color number" */
    UBYTE xAspect, yAspect;	/* aspect ratio, a rational number x/y */
    WORD  pageWidth, pageHeight;  /* source "page" size in pixels */
    } BitMapHeader;

/* RowBytes computes the number of bytes in a row, from the width in pixels.*/
# define RowBytes(w)   (2 * (((w) + 15) / 16))


/* ---------- ColorRegister --------------------------------------*/
/* A CMAP chunk is a packed array of ColorRegisters (3 bytes each). */
typedef struct {
    UBYTE red, green, blue;   /* MUST be UBYTEs so ">> 4" won't sign extend.*/
    } ColorRegister;

/* Use this constant instead of sizeof(ColorRegister). */
# define sizeofColorRegister  3

typedef WORD Color4;	/* Amiga RAM version of a color-register,
			 * with 4 bits each RGB in low 12 bits.*/

# define swapword(X)	((((X) & 0xff) << 8) | (((X) & 0xff00) >> 8))
# define swaplong(X)	(((unsigned) ((X) & 0xff000000) >> 24) |	\
			 ((unsigned) ((X) & 0x00ff0000) >> 8) |	\
			 ((unsigned) ((X) & 0x0000ff00) << 8) |	\
			 ((unsigned) ((X) & 0x000000ff) << 24))

# define swapsize(X) ((machine_byte_order () == LITTLE) ? swaplong(X) : (X))

/* Maximum number of bitplanes in RAM. Current Amiga max w/dual playfield. */
# define MaxAmDepth 6

/* Chunks must be padded to align to even boundaries */
# define EVENALIGN(X) (((X) + 1) & ~1)

/* ---------- Point2D --------------------------------------------*/
/* A Point2D is stored in a GRAB chunk. */
typedef struct {
    WORD x, y;		/* coordinates (pixels) */
    } Point2D;

/* ---------- DestMerge ------------------------------------------*/
/* A DestMerge is stored in a DEST chunk. */
typedef struct {
    UBYTE depth;	/* # bitplanes in the original source */
    UBYTE pad1;		/* UNUSED; for consistency store 0 here */
    UWORD planePick;	/* how to scatter source bitplanes into destination */
    UWORD planeOnOff;	/* default bitplane data for planePick */
    UWORD planeMask;	/* selects which bitplanes to store into */
    } DestMerge;

/* ---------- SpritePrecedence -----------------------------------*/
/* A SpritePrecedence is stored in a SPRT chunk. */
typedef UWORD SpritePrecedence;

/* ---------- Viewport Mode --------------------------------------*/
/* A Commodore Amiga ViewPort->Modes is stored in a CAMG chunk. */
/* The chunk's content is declared as a LONG. */

/* ---------- CRange ---------------------------------------------*/
/* A CRange is store in a CRNG chunk. */
typedef struct {
    WORD  pad1;		/* reserved for future use; store 0 here */
    WORD  rate;		/* color cycling rate, 16384 = 60 steps/second */
    WORD  active;	/* nonzero means color cycling is turned on */
    UBYTE low, high;	/* lower and upper color registers selected */
    } CRange;

/*----------------------------------------------------------------*
 * PACKER.H  typedefs for Data-Compresser.  		        1/22/86
 *
 * This module implements the run compression algorithm "cmpByteRun1"; the
 * same encoding generated by Mac's PackBits.
 *
 * By Jerry Morrison and Steve Shaw, Electronic Arts.
 * This software is in the public domain.
 *
 * This version for the Commodore-Amiga computer.
 *----------------------------------------------------------------*/

/* This macro computes the worst case packed size of a "row" of bytes. */
# define MaxPackedSize(rowSize)  ( (rowSize) + ( ((rowSize)+127) >> 7 ) )

/*----------------------------------------------------------------*
 * unpacker.c Convert data from "cmpByteRun1" run compression. 11/15/85
 *
 * By Jerry Morrison and Steve Shaw, Electronic Arts.
 * This software is in the public domain.
 *
 *	control bytes:
 *	 [0..127]   : followed by n+1 bytes of data.
 *	 [-1..-127] : followed by byte to be repeated (-n)+1 times.
 *	 -128       : NOOP.
 *
 * This version for the Commodore-Amiga computer.
 *----------------------------------------------------------------*/

/*----------- UnPackRow ------------------------------------------*/

# define UGetByte()	(*source++)
# define UPutByte(c)	(*dest++ = (c))

/* Given POINTERS to POINTER variables, unpacks one row, updating the source
 * and destination pointers until it produces dstBytes bytes. */
static UnPackRow(pSource, pDest, srcBytes0, dstBytes0)
	char **pSource, **pDest;  int srcBytes0, dstBytes0; {
    char *source = *pSource;
    char *dest   = *pDest;
    register int n;
    register char c;
    int srcBytes = srcBytes0, dstBytes = dstBytes0;
    BOOL error = TRUE;	/* assume error until we make it through the loop */

    while( dstBytes > 0 )  {
	if ( (srcBytes -= 1) < 0 )  goto ErrorExit;
    	n = UGetByte() & 0x0ff;

    	if (n < 128) {
	    n += 1;
	    
	    if ( (srcBytes -= n) < 0 )  goto ErrorExit;
	    if ( (dstBytes -= n) < 0 )  goto ErrorExit;
	    do {  UPutByte(UGetByte());  } while (--n > 0);
	    }

    	else if (n != 128) {
	    n = 257 - n;
	    
	    if ( (srcBytes -= 1) < 0 )  goto ErrorExit;
	    if ( (dstBytes -= n) < 0 )  goto ErrorExit;
	    c = UGetByte();
	    do {  UPutByte(c);  } while (--n > 0);
	    }
	}
    error = FALSE;	/* success! */

  ErrorExit:
    *pSource = source;  *pDest = dest;
    
    if (error)
    { RGError = RG_FORMBAD_err;
      return(RG_ERROR_INT);
      /* "error in unpack, src %d, dst %d\n" */
    }
    
    return(error);
    }

/****************************************************************
 * read_iff: Read an Amiga format IFF Interleaved Bitmap
 ****************************************************************/

read_iff (image, GetByte, Parameter, mstr, mlen)
FBM *image;
int (*GetByte)();
char *Parameter;
char *mstr;
int mlen;
{
  int rval;
  char magic[8];
  long formsize, buflen;
  FILE *rfile;
  Chunk *form;

    rfile = (FILE *) 0;
    fbm_GetByte = GetByte;
    fbm_Parameter = Parameter;

  /* First word is magic number */
  magic[0] = NEXTMCH(rfile,mstr,mlen) & 0xff;
  magic[1] = NEXTMCH(rfile,mstr,mlen) & 0xff;
  magic[2] = NEXTMCH(rfile,mstr,mlen) & 0xff;
  magic[3] = NEXTMCH(rfile,mstr,mlen) & 0xff;
  magic[4] = '\0';

  /* If magic number is not FORM, lose */
  if (RgStrNCmp (magic, "FORM", (INT_type) 4) !=  (INT_type) 0)
  { if (RgStrNCmp (magic, "FOR", (INT_type) 3) == (INT_type) 0 ||
	RgStrNCmp (magic, "LIS", (INT_type) 3) == (INT_type) 0 ||
	RgStrNCmp (magic, "CAT", (INT_type) 3) == (INT_type) 0)
    {
	rval = RG_ERROR_INT;
	RGError = RG_FORMBAD_err;
	goto LEAVE;
	/* "Sorry, I only handle FORM type IFF files\n" */
    }
    
    rval = RG_ERROR_INT;
    RGError = RG_NOFORM_err;
    goto LEAVE;
   /* "read_iff: error, file not a FORM type IFF file" */
  }

  /* Second longword is length of data chunk */
  formsize = get_long (rfile, BIG);  
  
  form = (Chunk *) MEM_Malloc (formsize + 8);
  form->ckID = FORM;
  form->ckSize = formsize;

  /* Now read the rest of the chunk */
  if ((buflen = fbmread (form->ckData, 1, formsize, stdin)) < formsize)
  { if (buflen < 0)
    { 
	rval = RG_ERROR_INT;
	RGError = RG_FORMBAD_err;
	goto LEAVE;
    }
    else
    {
	rval = RG_ERROR_INT;
	RGError = RG_FORMBAD_err;
	goto LEAVE;
	/* "error: premature EOF in FORM after %d of %d bytes\n" */
    }
  }

  /* Recursively parse the FORM */  
  parse_form (image, form);

  /* Now we've read the image (or not) */  
  MEM_Free ( (POINTER_type) form);

    rval = RG_SUCCESS_INT;

LEAVE:
  return (rval);
}

/****************************************************************
 * parse_form: Parse an IFF form chunk
 *
 *	FORM       ::= "FORM" #{ FormType (LocalChunk | FORM | LIST | CAT)* }
 *	FormType   ::= ID
 *	LocalChunk ::= Property | Chunk
 ****************************************************************/

static parse_form (image, chunk)
FBM *image;
Chunk *chunk;
{
  int rval;  
  UBYTE *data, *tail;
  int clrlen, colors;
  BitMapHeader *bmh;
  register int i, bits;
  long formtype;
  int found_bmhd=0, found_cmap=0, found_body=0;
  Chunk *part;

  data = chunk->ckData;
  tail = data + chunk->ckSize;  
  
  formtype = MakeID(data[0], data[1], data[2], data[3]);
  data += 4;
  
  if (formtype != ID_ILBM)
  {
    rval = RG_ERROR_INT;
    RGError = RG_FORMBAD_err;
    goto LEAVE;
    /* "this FORM doesn't start with ILBM, but %4.4s, sorry.\n" */
  }
  
  while (data < tail)
  { part = (Chunk *) data;
    part->ckID   = swapsize (part->ckID);
    part->ckSize = swapsize (part->ckSize);
    data += ( EVENALIGN (part->ckSize) + 8 );
    
/****************
# ifdef DEBUG
    fprintf (stderr, "Found %c%c%c%c, size %ld\n",
		(part->ckID & 0xff000000) >> 24,
		(part->ckID & 0x00ff0000) >> 16,
		(part->ckID & 0x0000ff00) >>  8,
		(part->ckID & 0x000000ff),
		part->ckSize);
# endif
****************/

    if (part->ckID == ID_BMHD)
    { found_bmhd++;
      bmh = (BitMapHeader *) part->ckData;
      
      /* IFF uses BIG byte order, swap if necessary */
      if (machine_byte_order () == LITTLE)
      { bmh->w = swapword (bmh->w);
        bmh->h = swapword (bmh->h);
	bmh->x = swapword (bmh->x);
	bmh->y = swapword (bmh->y);
	bmh->transparentColor = swapword (bmh->transparentColor);
	bmh->pageWidth = swapword (bmh->pageWidth);
	bmh->pageHeight = swapword (bmh->pageHeight);
      }
      
      image->hdr.rows = bmh->h;
      image->hdr.cols = bmh->w;
      image->hdr.planes = 1;
      image->hdr.bits = bmh->nPlanes;
      image->hdr.physbits = 8;
      image->hdr.rowlen = 16 * ((image->hdr.cols + 15) / 16);
      image->hdr.plnlen = image->hdr.rowlen * image->hdr.rows;
      image->hdr.clrlen = 0;
      image->hdr.aspect = (double) bmh->yAspect / bmh->xAspect;
      image->hdr.title[0] = '\0';
      image->hdr.credits[0] = '\0';
    }
    else if (part->ckID == ID_CMAP)
    { image->hdr.clrlen = part->ckSize;

      alloc_fbm (image);

      clrlen = image->hdr.clrlen;
      colors = clrlen / 3;

      for (i=0; i<image->hdr.clrlen; i++)
      { image->cm[(colors * (i%3)) + i/3] = part->ckData[i]; }

      /* Compute number of bits in colormap */
      for (i=colors, bits=1; i > 2; i /= 2, bits++) ;
      
      if (bits < image->hdr.bits) image->hdr.bits = bits;

      found_cmap++;
    }
    else if (part->ckID == ID_BODY)
    { if (!found_bmhd)
      {
	rval = RG_ERROR_INT;
	RGError = RG_FORMBAD_err;
	goto LEAVE;
	/* "Error, BODY found with no BMHD header\n" */
      }
      
      if (found_cmap == 0)
      { alloc_fbm (image); }
      
      found_body++;

#ifdef DEBUG      
      /* Decode body */
/**************
      fprintf (stderr, "Reading IFF [%dx%dx%d], %d physbits, %1.3lf aspect",
		image->hdr.cols, image->hdr.rows,
		image->hdr.bits, image->hdr.physbits,
		image->hdr.aspect);
      if (image->hdr.planes > 1)
      { fprintf (stderr, ", %d planes", image->hdr.planes); }
      if (image->hdr.clrlen > 1)
      { fprintf (stderr, ", %d colors", image->hdr.clrlen / 3); }
      if (bmh->compression)
      { fprintf (stderr, ", compressed"); }
      if (bmh->masking == mskHasMask)
      { fprintf (stderr, ", with mask"); }
      fprintf (stderr, "\n");

# ifdef DEBUG
      fprintf (stderr,
	       "masking %d, compression %d, transparent %d, page [%dx%d]\n",
	       bmh->masking, bmh->compression, bmh->transparentColor,
	       bmh->pageWidth, bmh->pageHeight);

	for (i=0; i<colors; i++)
	{ fprintf (stderr, "    color %3d:  <%3d, %3d, %3d>\n", i,
		   image->cm[i], image->cm[i+colors], image->cm[i+colors*2]);
	}
# endif
**************/
#endif
      
      rval = read_iff_body (image, bmh, part);
      goto LEAVE;
    }
  }
  
LEAVE:

  return (rval);
}

/****************************************************************
 * read_iff_body: Read the bits in the ILBM body into the FBM image
 ****************************************************************/

static read_iff_body (image, bmh, body)
FBM *image;
BitMapHeader *bmh;
Chunk *body;
{
  int rval;
  register int r, c, k;
  int pmask, byte, bit;
  unsigned char *row, *bp, *buf, *obm, *tail;
  int bytlen = image->hdr.cols / 8;
  int planes =  bmh->nPlanes + ((bmh->masking == mskHasMask) ? 1 : 0);
  
  buf = (unsigned char *) MEM_Malloc (bytlen);
  
  bp = body->ckData;
  tail = bp + body->ckSize;

  /* Each iteration reads one scan line */
  for (r=0; r<image->hdr.rows; r++)
  {
    if (bp > tail)
    {
	rval = RG_ERROR_INT;
	RGError = RG_FORMBAD_err;
	goto LEAVE;
	/* "Ran out of data in body after %d of %d rows\n" */
    }

    obm = &(image->bm[r * image->hdr.rowlen]);

    /* Clear the output row of pixels */
    for (c=0; c<image->hdr.cols; c++)
    { obm[c] = 0; }

    /* Each loop reads one plane of this scan line */    
    for (k=0; k<planes; k++)
    {
      /* First get pointer to data packed 8 bits per byte */
      if (bmh->compression == 0)
      { row = bp; bp += RowBytes (bmh->w); }
      else
      { row = buf;
        if (UnPackRow (&bp, &row, (int) (tail-bp), RowBytes (bmh->w)) != 0)
        { 
	    rval = RG_ERROR_INT;
	    RGError = RG_FORMBAD_err;
	    goto LEAVE;
	    /* "Error in UnPackRow" */
	}
	row = buf;
      }

      /* Ignore extra planes (including the mask if any) */
      if (k >= image->hdr.bits)
        continue;

      /* Now OR in these bits into the output pixels */
      pmask = 1 << k;

      for (c=0; c<image->hdr.cols; c++)
      { byte = c >> 3;
        bit = 7 - (c & 7);
	bit = row[byte] & (1 << bit);

	obm[c] |= bit ? pmask : 0;
      }
    }
  }

  if (tail-bp > 1)
  {
/*
 * "Warning, %d bytes of data unread\n", tail - bp
 */
  }

  rval = RG_SUCCESS_INT;

LEAVE:
  return (rval);
}






/****************************************************************
 * write_iff: Write AMIGA IFF format ILBM file
 *
 * Writes		FORM type ILBM
 *			    BMHD
 *			    CMAP (optional)
 *			    BODY (uncompressed)
 *
 ****************************************************************/

write_iff (image, PutByte, Parameter)
FBM *image;
int (*PutByte)();
char *Parameter;
{
  int rval;
  BitMapHeader bmhd;
  FILE *wfile;
  unsigned char *cmap, *body;
  int bodylen, cmaplen, bmhdlen, formlen, ilbmbits;

    wfile = (FILE *) 0;
    fbm_PutByte = PutByte;
    fbm_Parameter = Parameter;

  if (image->hdr.planes > 1)
  {
    rval = RG_ERROR_INT;
    RGError = RG_BADPARM_err;
    goto LEAVE;
    /* "Error, write_iff cannot handle multi-plane images\n" */
  }

  /* Determine number of bits in output */
  if (image->hdr.clrlen == 0)
  { ilbmbits = image->hdr.bits; }
  else
  { int colors = image->hdr.clrlen/3;
    for (ilbmbits=1; colors > 2; ilbmbits++, colors >>= 1) ;
  }
  
  if (ilbmbits > 5)
  {
/***
	fprintf (stderr, "%s\n%s\n%s\n",
	"Warning: most IFF ILBM displays cannot handle more than",
	"	 32 colors. You should probably run the image though",
	"	 'gray2clr -u | fbquant -c32' first.");
***/
  }

  /* Build BMHD, CMAP, and body chunks */
  bmhdlen = build_bmhd (image, &bmhd, ilbmbits) ;
  cmaplen = build_cmap (image, &cmap, ilbmbits);
  bodylen = build_body (image, &body, ilbmbits);
  
  /* Length of FORM is len(subparts) + 8 for header + 4 for type */
  formlen = bmhdlen + cmaplen + bodylen + 12;

  /*--------Write out FORM chunk header--------*/
  fbmprintf ( "FORM");
  put_long (formlen-8, wfile, BIG);
  fbmprintf ( "ILBM");

  /*----Write out BMHD chunk----*/
  fbmprintf ( "BMHD");
  put_long (bmhdlen-8, wfile, BIG);
  fbmwrite (&bmhd, bmhdlen-8, 1, wfile);

  /* No need to pad BMHD chunk, it must be even */
      
  /*----Write out CMAP chunk----*/
  if (cmaplen > 0)
  { fbmprintf ( "CMAP");
    put_long (cmaplen-8, wfile, BIG);
    fbmwrite (cmap, cmaplen-8, 1, wfile);
  
    /* Pad CMAP chunk if necessary */
    if (cmaplen & 1) FBMPUTC (0, wfile);
  }
      
  /*----Write out BODY chunk----*/
  fbmprintf ( "BODY");
  put_long (bodylen-8, wfile, BIG);
  fbmwrite (body, bodylen-8, 1, wfile);

  /* Pad BODY chunk if necessary */
  if (bodylen & 1) FBMPUTC (0, wfile);
      
  /*--------Free memory and return--------*/
  if (cmap)	MEM_Free ( (POINTER_type) cmap);
  if (body)	MEM_Free ( (POINTER_type) body);

    rval = RG_SUCCESS_INT;

LEAVE:
  return (rval);
}

/****************************************************************
 * build_bmhd: Build a BitMapHeader, and byte swap it if necessary
 ****************************************************************/

static build_bmhd (image, bmh, bits)
FBM *image;
BitMapHeader *bmh;
int bits;
{
  bmh->w = image->hdr.cols;
  bmh->h = image->hdr.rows;
  bmh->x = 0;
  bmh->y = 0;
  bmh->nPlanes = bits;
  bmh->masking = 0;  
  bmh->compression = 0;
  bmh->pad1 = 0;
  bmh->transparentColor = 0;
  bmh->xAspect = 100;
  bmh->yAspect = (image->hdr.aspect * 100.0) + 0.5;
  bmh->pageWidth = bmh->w;
  bmh->pageHeight = bmh->h;

  /* IFF uses BIG byte order, swap if necessary */
  if (machine_byte_order () == LITTLE)
  { bmh->w = swapword (bmh->w);
    bmh->h = swapword (bmh->h);
    bmh->x = swapword (bmh->x);
    bmh->y = swapword (bmh->y);
    bmh->transparentColor = swapword (bmh->transparentColor);
    bmh->pageWidth = swapword (bmh->pageWidth);
    bmh->pageHeight = swapword (bmh->pageHeight);
  }

  return (sizeof (*bmh) + 8);
}

/****************************************************************
 * build_cmap: Convert an FBM format colormap to IFF format
 ****************************************************************/

static build_cmap (image, cmap, bits)
FBM *image;
unsigned char **cmap;
int bits;
{ register int len, i;
  unsigned char *r, *g, *b, *c;
  int colors;

  colors = image->hdr.clrlen / 3;
  
  r = image->cm;
  g = r + colors;
  b = g + colors;
  
  len = 3*colors;
  *cmap = (unsigned char *) MEM_Malloc (len);

  /* Now convert from three vectors to a vector of triples */
  for (i=0, c= *cmap; i<colors; i++)
  { *c++ = *r++;
    *c++ = *g++;
    *c++ = *b++;
  }
  
  /* Return length of chunk, just length of map plus 8 bytes chunk header */
  return (len + 8);
}

/****************************************************************
 * build_body: Interleave the bits for the byte plane
 ****************************************************************/

static build_body (image, body, bits)
FBM *image;
unsigned char **body;
int bits;
{ int bpr, size;
  unsigned char *obm, *bmp;
  register int r, c, k;
  int mask, byte, bit;

  bpr = RowBytes (image->hdr.cols);
  
  size = bpr * image->hdr.rows * bits;
  
  *body = (unsigned char *) MEM_Malloc (size);

  obm = *body;  

  for (r=0; r < image->hdr.rows; r++)
  { for (k=0; k<bits; k++)
    { mask = 1 << k;
      bmp = &(image->bm[r * image->hdr.rowlen]);

      for (c=0, byte=0; c<image->hdr.cols; c++)
      { bit = (*bmp++ & mask) ? 1 : 0;

	byte = (byte << 1) | bit;
        if ((c&7) == 7)
	{ *obm++ = byte;

	  byte=0;
        }
      }
      
      if ((c & 7) != 0)
      { while ((c&7) != 0)
        { c++; byte <<= 1; }
	*obm++ = byte;
      }
    }
  }
  
  return (size + 8);
  
}

static INT_type RgStrNCmp( first, second, length )
    IN char *first;
    IN char *second;
    IN INT_type length;
{

    char *fp;
    char *sp;

    INT_type i;

    /*
	Whimppy version of strncmp for rg toolkit.
	Returns 0 if first and second are equal;
	    otherwise returns (INT_type) -1.
    */

    for( fp = first, sp = second, i = (INT_type) 0 ; 
	 i < length && *fp == *sp ;
	 fp++, sp++, i++ ) ;
	/* N.B. test on i < length before look at memory - relys on
	    semantics of &&
	*/

    if( i == length ){
	return( (INT_type) 0 );
    }
    else {
	return( (INT_type) -1 );
    }



}
