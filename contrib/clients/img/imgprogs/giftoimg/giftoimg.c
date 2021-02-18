/*
	This is a hack of an old copy of gif2ras, by Patrick J. Naughton
	of Sun Microsystems.  The original copyright notice follows
	after ISI's copyright notice.

	To convert a GIF file to a color mapped Img file, use:

		giftoimg gif_filename img_filename

	gif_filename should be the actual filename.
	img_filename may omit the .p[.Z] extension;
		if omitted, this will be supplied automatically
*/
/*

    Copyright (c) 1989 University of Southern California

	Paul Raveling
	USC Information Sciences Institute
	4676 Admiralty Way
	Marina del Rey, California 90292


	The authors grant without fee permission to use, copy, modify,
	distribute, and sell this software for any purpose, provided:

	--  The above copyright notice appears in all copies.

	--  This disclaimer appears in all source code copies.

	--  The names of ISI and USC are not used in advertising or
	    publicity  pertaining  to distribution  of the software
	    without prior specific written permission from ISI.

	ISI provides this software freely as a public service.  It is
	NOT a commercial product,  and therefore is not subject to an
	an implied  warranty  of  merchantability  or  fitness  for a
	particular purpose.  ISI provides it as is, without warranty.

	This software  is furnished  only on the basis that any party
	who receives it indemnifies  and holds harmless  the original
	authors and their sponsors  against any claims,  demands,  or
	liabilities  connected  with  using  it,   furnishing  it  to
	others, or providing it to a third party.


	The author  of this software is  Paul Raveling.   Development
	used the author's after-work hours and ISI's facilities.

	Thanks are due to ISI for allowing  such extracurricular work
	and  to Hewlett Packard,   whose  University  Grants  program
	provided  the 9000/370  workstation  and  supporting software
	which was the platform for this work.
*/

/*-
 * gif2ras.c - Converts from a Compuserve GIF (tm) image to a Sun Raster image.
 *
 * Copyright (c) 1988 by Patrick J. Naughton
 *
 * Author: Patrick J. Naughton
 * naughton@wind.sun.com
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * This file is provided AS IS with no warranties of any kind.  The author
 * shall have no liability with respect to the infringement of copyrights,
 * trade secrets or any patents by this file or any part thereof.  In no
 * event will the author be liable for any lost revenue or profits or
 * other special, indirect and consequential damages.
 *
 * Comments and additions should be sent to the author:
 *
 *                     Patrick J. Naughton
 *                     Sun Microsystems, Inc.
 *                     2550 Garcia Ave, MS 14-40
 *                     Mountain View, CA 94043
 *                     (415) 336-1080
 *
 * Revision History:
 * 28-Nov-89: Modified copyright notice to suit X Consortium standards
 * 27-Nov-89: Finalized for X11R4 by adding copyright notice comments (PR)
 * 27-Dec-88: Modified by Paul Raveling to output II pixmap files.
 * 28-Aug-88: Modified by Jef Poskanzer to output PBM instead of Sun raster.
 * 27-Jul-88: Updated to use libpixrect to fix 386i byteswapping problems.
 * 11-Apr-88: Converted to C and changed to write Sun rasterfiles.
 * 19-Jan-88: GIFSLOW.PAS posted to comp.graphics by Jim Briebel,
 *            a Turbo Pascal 4.0 program to painfully slowly display
 *            GIF images on an EGA equipped IBM-PC.
 *
 * Description:
 *   This program takes a Compuserve "Graphics Interchange Format" or "GIF"
 * file as input and writes a file known as a Sun rasterfile.  This datafile
 * can be loaded by the NeWS "readcanvas" operator and is of the same format
 * as the files in /usr/NeWS/smi/*.  Under X11R2 there is a program called
 * xraster to display these files.
 *
 * Portability:
 *   To make this program convert to some image format other than PBM
 * format simply seach for the tag "PBMS:" in the source and
 * replace these simple mechanisms with the appropriate ones for the
 * other output format.  I have marked all (six) PBM Specific pieces
 * of code with this comment.
 *
 * SS: compile with "cc -o gif2ras -O gif2ras.c -lpixrect"
 * PBMS: compile with "cc -o giftopbm -O giftopbm.c libpbm.a
 */

#include <stdio.h>
#ifdef notdefSS
#include <pixrect/pixrect_hs.h> /* SS: main Pixrect header file */
#endif notdefSS
#include <sys/types.h>
#include <imglib/imglib.h>

typedef int boolean;
#define True (1)
#define False (0)

#define NEXTSHORT (*ptr++ + (*ptr++ << 8))
/*	Compiler optimization may be shortcutting *ptr++ values...	*/
#define GETSHORT(var) \
	var = *ptr++; \
	var += *ptr++ << 8;
#define NEXTBYTE (*ptr++)
#define IMAGESEP 0x2c
#define INTERLACEMASK 0x40
#define COLORMAPMASK 0x80

int BitOffset = 0,		/* Bit Offset of next code */
    XC = 0, YC = 0,		/* Output X and Y coords of current pixel */
    Pass = 0,			/* Used by output routine if interlaced pic */
    OutCount = 0,		/* Decompressor output 'stack count' */
    RWidth, RHeight,		/* screen dimensions */
    Width, Height,		/* image dimensions */
    LeftOfs, TopOfs,		/* image offset */
    BitsPerPixel,		/* Bits per pixel, read from GIF header */
    ColorMapSize,		/* number of colors */
    CodeSize,			/* Code size, read from GIF header */
    InitCodeSize,		/* Starting code size, used during Clear */
    Code,			/* Value returned by ReadCode */
    MaxCode,			/* limiting value for current code size */
    ClearCode,			/* GIF clear code */
    EOFCode,			/* GIF end-of-information code */
    CurCode, OldCode, InCode,	/* Decompressor variables */
    FirstFree,			/* First free code, generated per GIF spec */
    FreeCode,			/* Decompressor, next free slot in hash table */
    FinChar,			/* Decompressor variable */
    BitMask,			/* AND mask for data size */
    ReadMask;			/* Code AND mask for current code size */

boolean Interlace, HasColormap;

#ifdef notdefSS
/* SS: defined in pixrect/pixrect_hs.h */
Pixrect *Output;		/* The Sun Pixrect */
colormap_t Colormap;		/* The Pixrect Colormap */
u_char *Image;			/* The result array */
#endif notdefSS

/*	Img definitions	*/

	Img	*newimg;	/*  Pointer to new image	*/
	Col	 colmap[256];	/*  Color map			*/
	int	 num_colors;
	Byte	*pixdata;	/*  Pointer to pixel data	*/
	int	 num_pixels, imgwidth, imgheight;

#if 0
/* PBMS: defined in pbm.h */
bit **bits;			/* The PBM bit array */
#endif

u_char *RawGIF;			/* The heap array to hold it, raw */
u_char *Raster;			/* The raster data stream, unblocked */

    /* The hash table used by the decompressor */
int Prefix[4096];
int Suffix[4096];

    /* An output array used by the decompressor */
int OutCode[1025];

    /* The color map, read from the GIF header */
u_char Red[256], Green[256], Blue[256];

char *id = "GIF87a";

char *pname;			/* program name (used for error messages) */
 
void
error(s1, s2)
char *s1, *s2;
{
    fprintf(stderr, s1, pname, s2);
    exit(1);
}


main(argc, argv)
int argc;
char *argv[];
{
FILE *fp;
char *infname = argv[1];
char *outfname = argv[2];
int filesize;
register u_char ch, ch1;
register u_char *ptr, *ptr1;
register int i;

/*    setbuf(stderr, NULL);	*/
    pname = argv[0];

    if (argc < 3)
	error("usage: %s GIFfile rasterfile\n", NULL);

    if (!(fp = fopen(infname, "r")))
	error("%s: %s not found.\n", infname);

    /* find the size of the file */

    fseek(fp, 0L, 2);
    filesize = ftell(fp);
    fseek(fp, 0L, 0);

    if (!(ptr = RawGIF = (u_char *) malloc(filesize)))
	error("%s: not enough memory to read gif file.\n", NULL);

/*
    if (!(Raster = (u_char *) malloc(filesize)))
	error("%s: not enough memory to read gif file.\n", NULL);
*/




    fread(ptr, filesize, 1, fp);

    if (strncmp(ptr, id, 6))
	error("%s: %s is not a GIF file.\n", infname);
    ptr += 6;

/* Get variables from the GIF screen descriptor */

#if 0
    RWidth = NEXTSHORT;		/* screen dimensions... not used. */
    RHeight = NEXTSHORT;
#else
    GETSHORT(RWidth)
    GETSHORT(RHeight)
#endif

    ch = NEXTBYTE;
    HasColormap = ((ch & COLORMAPMASK) ? True : False);


    BitsPerPixel = (ch & 7) + 1;
    ColorMapSize = 1 << BitsPerPixel;
    BitMask = ColorMapSize - 1;

    ch = NEXTBYTE;		/* background color... not used. */

    if (NEXTBYTE)		/* supposed to be NULL */
	error("%s: %s is a corrupt GIF file.\n", infname);

/* Read in global colormap. */

    if (HasColormap) {
	fprintf(stderr, "%s is %d bits per pixel, (%d colors).\n",
		infname, BitsPerPixel, ColorMapSize);
	for (i = 0; i < ColorMapSize; i++) {
	    Red[i] = NEXTBYTE;
	    Green[i] = NEXTBYTE;
	    Blue[i] = NEXTBYTE;

	    colmap[i].red   = Red  [i];
	    colmap[i].green = Green[i];
	    colmap[i].blue  = Blue [i];
	    colmap[i].freq  = 0;
	    colmap[i].pixel = 0;
	}

#ifdef notdefSS
/* SS: Fill in the Pixrect colormap struct */
	Colormap.type = RMT_EQUAL_RGB;
	Colormap.length = ColorMapSize;
	Colormap.map[0] = Red;
	Colormap.map[1] = Green;
	Colormap.map[2] = Blue;
#endif notdefSS

#if 0
	/* PBMS: PBM only handles bitmaps.  Reject any pixmaps here. */
	if (BitsPerPixel != 1)
	    error("%s: %s has more than one bit per pixel - it's not a bitmap.\n", infname);
#endif
    }
    else error("%s: %s does not have a colormap.\n", infname);


/* Check for image seperator */

    if (NEXTBYTE != IMAGESEP)
	error("%s: %s is a corrupt GIF file.\n", infname);

/* Now read in values from the image descriptor */

#if 0
    LeftOfs = NEXTSHORT;
    TopOfs = NEXTSHORT;
    Width = NEXTSHORT;
    Height = NEXTSHORT;
#else
    GETSHORT(LeftOfs)
    GETSHORT(TopOfs)
    GETSHORT(Width)
    GETSHORT(Height)
#endif
    Interlace = ((NEXTBYTE & INTERLACEMASK) ? True : False);

    fprintf(stderr, "Reading a %d by %d %sinterlaced image...\n",
        Width, Height, (Interlace) ? "" : "non-");
    

/* Note that I ignore the possible existence of a local color map.
 * I'm told there aren't many files around that use them, and the spec
 * says it's defined for future use.  This could lead to an error
 * reading some files. 
 */

/* Start reading the raster data. First we get the intial code size
 * and compute decompressor constant values, based on this code size.
 */

    CodeSize = NEXTBYTE;
    ClearCode = (1 << CodeSize);
    EOFCode = ClearCode + 1;
    FreeCode = FirstFree = ClearCode + 2;

/* The GIF spec has it that the code size is the code size used to
 * compute the above values is the code size given in the file, but the
 * code size used in compression/decompression is the code size given in
 * the file plus one. (thus the ++).
 */

    CodeSize++;
    InitCodeSize = CodeSize;
    MaxCode = (1 << CodeSize);
    ReadMask = MaxCode - 1;
    Raster = (u_char *) malloc ( Width*Height );
    if ( Raster == NULL )
	{
	fprintf ( stderr, "Insufficient memory\n" );
	exit (2);
	}

/* Read the raster data.  Here we just transpose it from the GIF array
 * to the Raster array, turning it from a series of blocks into one long
 * data stream, which makes life much easier for ReadCode().
 */

    ptr1 = Raster;
    do {
	ch1 = NEXTBYTE;
	ch = ch1;
	while (ch--) *ptr1++ = NEXTBYTE;
    } while(ch1);

    free(RawGIF);		/* We're done with the raw data now... */

    fprintf(stderr, "done.\n");
    fprintf(stderr, "Decompressing...");


#ifdef notdefSS
/* SS: Allocate the Sun Pixrect and make "Image" point to the image data. */
    Output = mem_create(Width, Height, 8);
    if (Output == (Pixrect *) NULL)
	error("%s: not enough memory for output data.\n", NULL);
    Image = (u_char *) mpr_d(Output)->md_image;
#endif notdefSS
#if 0
/* PBMS: Allocate the PBM bit array. */
    bits = pbm_allocarray(Width, Height);
#endif

    num_pixels = Width * Height;
    imgwidth   = Width;
    imgheight  = Height;
    pixdata    = (Byte *) malloc ( num_pixels );
    if ( pixdata == NULL )
	{
	fprintf ( stderr, "Insufficient memory\n" );
	exit (2);
	}


/* Decompress the file, continuing until you see the GIF EOF code.
 * One obvious enhancement is to add checking for corrupt files here.
 */

    Code = ReadCode();
    while (Code != EOFCode) {

/* Clear code sets everything back to its initial value, then reads the
 * immediately subsequent code as uncompressed data.
 */

	if (Code == ClearCode) {
	    CodeSize = InitCodeSize;
	    MaxCode = (1 << CodeSize);
	    ReadMask = MaxCode - 1;
	    FreeCode = FirstFree;
	    CurCode = OldCode = Code = ReadCode();
	    FinChar = CurCode & BitMask;
	    AddToPixel(FinChar);
	}
	else {

/* If not a clear code, then must be data: save same as CurCode and InCode */

	    CurCode = InCode = Code;

/* If greater or equal to FreeCode, not in the hash table yet;
 * repeat the last character decoded
 */

	    if (CurCode >= FreeCode) {
		CurCode = OldCode;
		OutCode[OutCount++] = FinChar;
	    }

/* Unless this code is raw data, pursue the chain pointed to by CurCode
 * through the hash table to its end; each code in the chain puts its
 * associated output code on the output queue.
 */

	    while (CurCode > BitMask) {
		OutCode[OutCount++] = Suffix[CurCode];
		CurCode = Prefix[CurCode];
	    } 

/* The last code in the chain is treated as raw data. */

	    FinChar = CurCode & BitMask;
	    OutCode[OutCount++] = FinChar;

/* Now we put the data out to the Output routine.
 * It's been stacked LIFO, so deal with it that way...
 */

	    for (i = OutCount - 1; i >= 0; i--)
		AddToPixel(OutCode[i]);
	    OutCount = 0;

/* Build the hash table on-the-fly. No table is stored in the file. */

	    Prefix[FreeCode] = OldCode;
	    Suffix[FreeCode] = FinChar;
	    OldCode = InCode;

/* Point to the next slot in the table.  If we exceed the current
 * MaxCode value, increment the code size unless it's already 12.  If it
 * is, do nothing: the next code decompressed better be CLEAR
 */

	    FreeCode++;
	    if (FreeCode >= MaxCode) {
		if (CodeSize < 12) {
		    CodeSize++;
		    MaxCode *= 2;
		    ReadMask = (1 << CodeSize) - 1;
		}
	    }
	}
	Code = ReadCode();
    }

    free(Raster);

    fprintf(stderr, "done.\n");

#if 0
    if (!(fp = fopen(outfname, "w")))
	error("%s: can't create %s.\n", outfname);
#endif

#ifdef notdefSS
/* SS: Pixrect Rasterfile output code. */
    fprintf(stderr, "Writing Sun Rasterfile: %s...", outfname);
    if (pr_dump(Output, fp, &Colormap, RT_STANDARD, 0) == PIX_ERR)
	error("%s: error writing Sun Rasterfile: %s\n", outfname);
#endif notdefSS
#if 0
/* PBMS: PBM output code. */
    pbm_writepbm(stdout, bits, Width, Height);

    fclose(fp);
#endif

    /*	Reduce color count if possible	*/

    i = 256;
    while ( --i >= 0 )
	{
	if ( colmap[i].freq != 0 )
	   break;
	}

    num_colors = i+1;
    fprintf ( stderr, "Image uses %d colors\n", num_colors );

    newimg = create_img ( mapped_image, imgwidth, imgheight, 0, NULL,
		      num_colors, colmap, pixdata );

    if ( newimg == NULL )
 	{
        fprintf ( stderr, "Couldn't ceate Img!\n" );
	exit(2);
	}

    fprintf(stderr, "Writing image file: %s...", outfname);
    write_img (outfname, newimg);

}


/* Fetch the next code from the raster data stream.  The codes can be
 * any length from 3 to 12 bits, packed into 8-bit bytes, so we have to
 * maintain our location in the Raster array as a BIT Offset.  We compute
 * the byte Offset into the raster array by dividing this by 8, pick up
 * three bytes, compute the bit Offset into our 24-bit chunk, shift to
 * bring the desired code to the bottom, then mask it off and return it. 
 */
ReadCode()
{
int RawCode, ByteOffset;

    ByteOffset = BitOffset / 8;
    RawCode = Raster[ByteOffset] + (0x100 * Raster[ByteOffset + 1]);
    if (CodeSize >= 8)
	RawCode += (0x10000 * Raster[ByteOffset + 2]);
    RawCode >>= (BitOffset % 8);
    BitOffset += CodeSize;
    return(RawCode & ReadMask);
}


AddToPixel(Index)
u_char Index;
{
#ifdef notdefSS
    *(Image + YC * Width + XC) = Index;
#endif notdefSS
#if 0
/* PBMS: Store a pixel. */
    bits[YC][XC] = Index;
#endif

    pixdata[YC * Width + XC] = Index;
    ++colmap[Index].freq;


/* Update the X-coordinate, and if it overflows, update the Y-coordinate */

    if (++XC == Width) {

/* If a non-interlaced picture, just increment YC to the next scan line. 
 * If it's interlaced, deal with the interlace as described in the GIF
 * spec.  Put the decoded scan line out to the screen if we haven't gone
 * past the bottom of it
 */

	XC = 0;
	if (!Interlace) YC++;
	else {
	    switch (Pass) {
		case 0:
		    YC += 8;
		    if (YC >= Height) {
			Pass++;
			YC = 4;
		    }
		break;
		case 1:
		    YC += 8;
		    if (YC >= Height) {
			Pass++;
			YC = 2;
		    }
		break;
		case 2:
		    YC += 4;
		    if (YC >= Height) {
			Pass++;
			YC = 1;
		    }
		break;
		case 3:
		    YC += 2;
		break;
		default:
		break;
	    }
	}
    }
}
