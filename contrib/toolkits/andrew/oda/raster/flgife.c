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
 * flgife.c: FBM Library 0.9 (Beta test) 07-Mar-89  Michael Mauldin
 *
 * Portions of this code Copyright (C) 1989 by Michael Mauldin.
 * Permission is granted to use this file in whole or in part provided
 * that you do not sell it for profit and that this copyright notice
 * and the names of all authors are retained unchanged.
 *
 * flgife.c:
 *
 * CONTENTS
 *	GIFEncode( wfile, GHeight, GWidth, GInterlace, Background, 
 *		   BitsPerPixel, Red, Green, Blue, GetPixel )
 *
 * HISTORY
 * 07-Mar-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *	Beta release (version 0.9) mlm@cs.cmu.edu
 *
 * 19-Feb-89  Michael Mauldin (mlm) at Carnegie Mellon University
 *	Adapted to FBM package.  Now takes FILE pointer instead of
 *	character name for output file.
 *
 * 13-Feb-89  David Rowley (mgardi@watdcsu.waterloo.edu)
 *	Created (sent by mail on 2/13/89)
 *	original name: GIFENCODE.C - GIF Image compression interface
 *
 *****************************************************************************/
 
#define TK_IMPL_fbm
#define TK_GVARS_flgifc

#include <fbm.h>
#include <rgsite.h>
#include <rgreal.h>

#undef TK_GVARS_flgifc
#undef TK_IMPL_fbm

#ifndef USE_LONG_NAMES

#define BumpPixel   bmppix
#define CountDown   cntdwn
#define Interlace   inlace


#endif


/*
 * Pointer to function returning an int
 */
typedef int (* ifunptr)();

#define TRUE 1
#define FALSE 0

static int Width, Height;
static int curx, cury;
static long CountDown;
static int Pass = 0;
static int Interlace;

static int (*fbm_PutByte)();
static char *fbm_Parameter;


/*
 * Bump the 'curx' and 'cury' to point to the next pixel
 */
static BumpPixel()
{
	/*
	 * Bump the current X position
	 */
	curx++;

	/*
	 * If we are at the end of a scan line, set curx back to the beginning
	 * If we are interlaced, bump the cury to the appropriate spot,
	 * otherwise, just increment it.
	 */
	if( curx == Width ) {
		curx = 0;

	        if( !Interlace ) 
			cury++;
		else {
		     switch( Pass ) {
	     
	               case 0:
        	          cury += 8;
                	  if( cury >= Height ) {
		  		Pass++;
				cury = 4;
		  	  }
                          break;
		  
	               case 1:
        	          cury += 8;
                	  if( cury >= Height ) {
		  		Pass++;
				cury = 2;
		  	  }
			  break;
		  
	               case 2:
	                  cury += 4;
	                  if( cury >= Height ) {
	                     Pass++;
	                     cury = 1;
	                  }
	                  break;
			  
	               case 3:
	                  cury += 2;
	                  break;
			}
		}
	}
}

/*
 * Return the next pixel from the image
 */
GIFNextPixel( getpixel )
ifunptr getpixel;
{
	int r;

	if( CountDown == 0 )
		return RG_EOF;

	CountDown--;

	r = ( * getpixel )( curx, cury );

	BumpPixel();

	return r;
}

/* public */

GIFEncode( PutByte, Parameter, wfile, GWidth, GHeight, GInterlace,
	   Background, BitsPerPixel, Red, Green, Blue, GetPixel )
	 
int (*PutByte)();
char *Parameter;
FILE *wfile;
int GWidth, GHeight;
int GInterlace;
int Background;
int BitsPerPixel;
int Red[], Green[], Blue[];
ifunptr GetPixel;

{
	FILE *fp;
	int B;
	int RWidth, RHeight;
	int LeftOfs, TopOfs;
	int Resolution;
	int ColorMapSize;
	int InitCodeSize;
	int i;

	fp = (FILE *) 0;
	fbm_PutByte = PutByte;
	fbm_Parameter = Parameter;

	Interlace = GInterlace;
	
	ColorMapSize = 1 << BitsPerPixel;
	
	RWidth = Width = GWidth;
	RHeight = Height = GHeight;
	LeftOfs = TopOfs = 0;
	
	Resolution = BitsPerPixel;

	/*
	 * Calculate number of bits we are expecting
	 */
	CountDown = (long)Width * (long)Height;

	/*
	 * Indicate which pass we are on (if interlace)
	 */
	Pass = 0;

	/*
	 * The initial code size
	 */
	if( BitsPerPixel <= 1 )
		InitCodeSize = 2;
	else
		InitCodeSize = BitsPerPixel;

	/*
	 * Set up the current x and y position
	 */
	curx = cury = 0;

	/*
	 * Open the GIF file for binary write
	 */
/****
I took this out since no files are opened in RGTK.  -Andrew Werth


	fp = fopen( FName, "wb" );

	fp = wfile;	 *** Change for FBM - mlm 2/19/89 ***

	if( fp == (FILE *)0 ) {
		printf( "error: could not open output file\n" );
		return (0);
	}
****/

	/*
	 * Write the Magic header
	 */
	fbmwrite( "GIF87a", 1, 6, fp );

	/*
	 * Write out the screen width and height
	 */
	Putword( RWidth, fp );
	Putword( RHeight, fp );

	/*
	 * Indicate that there is a global colour map
	 */
	B = 0x80;	/* Yes, there is a color map */

	/*
	 * OR in the resolution
	 */
	B |= (Resolution - 1) << 5;

	/*
	 * OR in the Bits per Pixel
	 */
	B |= (BitsPerPixel - 1);

	/*
	 * Write it out
	 */
	FBMPUTC( B, fp );

	/*
	 * Write out the Background colour
	 */
	FBMPUTC( Background, fp );

	/*
	 * Byte of 0s (future expansion)
	 */
	FBMPUTC( 0, fp );

	/*
	 * Write out the Global Colour Map
	 */
     	for( i=0; i<ColorMapSize; i++ ) {
		FBMPUTC( Red[i], fp );
		FBMPUTC( Green[i], fp );
		FBMPUTC( Blue[i], fp );
	}

	/*
	 * Write an Image separator
	 */
	FBMPUTC( ',', fp );

	/*
	 * Write the Image header
	 */

	Putword( LeftOfs, fp );
	Putword( TopOfs, fp );
	Putword( Width, fp );
	Putword( Height, fp );

	/*
	 * Write out whether or not the image is interlaced
	 */
	if( Interlace )
		FBMPUTC( 0x40, fp );
	else
		FBMPUTC( 0x00, fp );

	/*
	 * Write out the initial code size
	 */
	FBMPUTC( InitCodeSize, fp );

	/*
	 * Go and actually compress the data
	 */
	compress( InitCodeSize+1, fp, GetPixel );

	/*
	 * Write out a Zero-length packet (to end the series)
	 */
	FBMPUTC( 0, fp );

	/*
	 * Write the GIF file terminator
	 */
	FBMPUTC( ';', fp );

	/*
	 * And close the file
	 */
	
	return (1);	/* success - mlm 2/19/89 */
}

/*
 * Write out a word to the GIF file
 */
static Putword( w, fp )
int w;
FILE *fp;
{
	FBMPUTC( w & 0xff, fp );
	FBMPUTC( (w / 256) & 0xff, fp );
}
