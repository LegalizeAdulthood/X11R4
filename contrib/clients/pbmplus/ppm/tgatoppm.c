/* tgatoppm.c - read a TrueVision Targa file and write a portable pixmap
**
** Partially based on tga2rast, version 1.0, by Ian MacPhedran.
**
** Copyright (C) 1989 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
*/

#include <stdio.h>
#include "ppm.h"
#include "tga.h"
#ifdef SYSV
#include <string.h>
#else /*SYSV*/
#include <strings.h>
#endif /*SYSV*/

#define max(a,b) ((a) > (b) ? (a) : (b))

#define MAXCOLORS 16384

int mapped, rlencoded;

pixel ColorMap[MAXCOLORS];
int RLE_count = 0, RLE_flag = 0;

unsigned char getbyte( );

main( argc, argv )
int argc;
char *argv[];
    {
    struct ImageHeader tga_head;
    int i;
    unsigned int temp1, temp2;
    FILE *ifd;
    int argn, debug, rows, cols, row, col, realrow;
    int maxval;
    pixel **pixels;
    char *usage = " [-debug] [tgafile]";

    pm_progname = argv[0];

    argn = 1;
    debug = 0;

    if ( argn < argc && argv[argn][0] == '-' )
	{
	if ( strncmp( argv[argn], "-debug", max(strlen(argv[argn]),2) ) == 0 )
	    debug = 1;
	else
	    pm_usage( usage );
	argn++;
	}

    if ( argn != argc )
	{
	ifd = pm_openr( argv[argn] );
	argn++;
	}
    else
	ifd = stdin;

    if ( argn != argc )
	pm_usage( usage );

    /* Read the Targa file header. */
    readtga( ifd, &tga_head );
    if ( debug )
	{
	fprintf( stderr, "IDLength = %d\n", (int) tga_head.IDLength );
	fprintf( stderr, "CoMapType = %d\n", (int) tga_head.CoMapType );
	fprintf( stderr, "ImgType = %d\n", (int) tga_head.ImgType );
	fprintf( stderr, "Index_lo = %d\n", (int) tga_head.Index_lo );
	fprintf( stderr, "Index_hi = %d\n", (int) tga_head.Index_hi );
	fprintf( stderr, "Length_lo = %d\n", (int) tga_head.Length_lo );
	fprintf( stderr, "Length_hi = %d\n", (int) tga_head.Length_hi );
	fprintf( stderr, "CoSize = %d\n", (int) tga_head.CoSize );
	fprintf( stderr, "X_org_lo = %d\n", (int) tga_head.X_org_lo );
	fprintf( stderr, "X_org_hi = %d\n", (int) tga_head.X_org_hi );
	fprintf( stderr, "Y_org_lo = %d\n", (int) tga_head.Y_org_lo );
	fprintf( stderr, "Y_org_hi = %d\n", (int) tga_head.Y_org_hi );
	fprintf( stderr, "Width_lo = %d\n", (int) tga_head.Width_lo );
	fprintf( stderr, "Width_hi = %d\n", (int) tga_head.Width_hi );
	fprintf( stderr, "Height_lo = %d\n", (int) tga_head.Height_lo );
	fprintf( stderr, "Height_hi = %d\n", (int) tga_head.Height_hi );
	fprintf( stderr, "PixelSize = %d\n", (int) tga_head.PixelSize );
	fprintf( stderr, "AttBits = %d\n", (int) tga_head.AttBits );
	fprintf( stderr, "Rsrvd = %d\n", (int) tga_head.Rsrvd );
	fprintf( stderr, "OrgBit = %d\n", (int) tga_head.OrgBit );
	fprintf( stderr, "IntrLve = %d\n", (int) tga_head.IntrLve );
	}
    rows = ( (int) tga_head.Height_lo ) + ( (int) tga_head.Height_hi ) * 256;
    cols = ( (int) tga_head.Width_lo ) + ( (int) tga_head.Width_hi ) * 256;

    switch ( tga_head.ImgType )
	{
	case TGA_Map:
	case TGA_RGB:
	case TGA_Mono:
	case TGA_RLEMap:
	case TGA_RLERGB:
	case TGA_RLEMono:
	break;

	default:
	pm_error( "unknown Targa image type %d", tga_head.ImgType, 0,0,0,0 );
	}

    if ( tga_head.ImgType == TGA_Map ||
	 tga_head.ImgType == TGA_RLEMap ||
	 tga_head.ImgType == TGA_CompMap ||
	 tga_head.ImgType == TGA_CompMap4 )
	{ /* Color-mapped image */
	if ( tga_head.CoMapType != 1 )
	    pm_error( 
		"mapped image (type %d) with color map type != 1",
		tga_head.ImgType, 0,0,0,0 );
	mapped = 1;
	/* Figure maxval from CoSize. */
	switch ( tga_head.CoSize )
	    {
	    case 8:
	    case 24:
	    case 32:
	    maxval = 255;
	    break;

	    case 15:
	    case 16:
	    maxval = 31;
	    break;

	    default:
	    pm_error(
		"unknown colormap pixel size - %d", tga_head.CoSize, 0,0,0,0 );
	    }
	if ( maxval > PPM_MAXMAXVAL )
	    pm_error( "CoSize is too large - try recompiling with a larger pixval type" );
	}
    else
	{ /* Not colormap, so figure maxval from PixelSize. */
	mapped = 0;
	switch ( tga_head.PixelSize )
	    {
	    case 8:
	    case 24:
	    case 32:
	    maxval = 255;
	    break;

	    case 15:
	    case 16:
	    maxval = 31;
	    break;

	    default:
	    pm_error( "unknown pixel size - %d", tga_head.PixelSize, 0,0,0,0 );
	    }
	if ( maxval > PPM_MAXMAXVAL )
	    pm_error( "PixelSize is too large - try recompiling with a larger pixval type" );
	}

    /* If required, read the color map information. */
    if ( tga_head.CoMapType != 0 )
	{
	temp1 = tga_head.Index_lo + tga_head.Index_hi * 256;
	temp2 = tga_head.Length_lo + tga_head.Length_hi * 256;
	if ( ( temp1 + temp2 + 1 ) >= MAXCOLORS )
	    pm_error( "too many colors - %d", ( temp1 + temp2 + 1 ), 0,0,0,0 );
	for ( i = temp1; i < ( temp1 + temp2 ); i++ )
	    get_map_entry( ifd, &ColorMap[i], (int) tga_head.CoSize );
	}

    /* Check run-length encoding. */
    if ( tga_head.ImgType == TGA_RLEMap ||
	 tga_head.ImgType == TGA_RLERGB ||
	 tga_head.ImgType == TGA_RLEMono )
	rlencoded = 1;
    else
	rlencoded = 0;

    /* Read the Targa file body and convert to portable format. */
    pixels = ppm_allocarray( cols, rows );
    for ( row = 0; row < rows; row++ )
	{
	if ( tga_head.IntrLve == TGA_IL_Four )
	    {
	    if ( 4 * row < rows )
		realrow = 4 * row;
	    else if ( 2 * row < rows )
		{
		realrow = row - rows / 4;
		realrow = 4 * realrow + 1;
		}
	    else if ( 4 * row < 3 * rows )
		{
		realrow = row - rows / 2;
		realrow = 4 * realrow + 2;
		}
	    else
		{
		realrow = row - rows / 2 - rows / 4;
		realrow = 4 * realrow + 3;
		}
	    }
	else if ( tga_head.IntrLve == TGA_IL_Two )
	    {
	    if ( 2 * row < rows )
		realrow = 2 * row;
	    else
		{
		realrow = row - rows / 2;
		realrow = 2 * realrow + 1;
		}
	    }
	else
	    realrow = row;

	if ( tga_head.OrgBit == 0 )
	    realrow = rows - realrow - 1;

	for ( col = 0; col < cols; col++ )
	    get_pixel( ifd, &(pixels[realrow][col]), (int) tga_head.PixelSize );
	}

    pm_close( ifd );

    ppm_writeppm( stdout, pixels, cols, rows, (pixval) maxval );

    exit( 0 );
    }

readtga( ifd, tgaP )
FILE *ifd;
struct ImageHeader *tgaP;
    {
    unsigned char flags;
    ImageIDField junk;

    tgaP->IDLength = getbyte( ifd );
    tgaP->CoMapType = getbyte( ifd );
    tgaP->ImgType = getbyte( ifd );
    tgaP->Index_lo = getbyte( ifd );
    tgaP->Index_hi = getbyte( ifd );
    tgaP->Length_lo = getbyte( ifd );
    tgaP->Length_hi = getbyte( ifd );
    tgaP->CoSize = getbyte( ifd );
    tgaP->X_org_lo = getbyte( ifd );
    tgaP->X_org_hi = getbyte( ifd );
    tgaP->Y_org_lo = getbyte( ifd );
    tgaP->Y_org_hi = getbyte( ifd );
    tgaP->Width_lo = getbyte( ifd );
    tgaP->Width_hi = getbyte( ifd );
    tgaP->Height_lo = getbyte( ifd );
    tgaP->Height_hi = getbyte( ifd );
    tgaP->PixelSize = getbyte( ifd );
    flags = getbyte( ifd );
    tgaP->AttBits = flags & 0xf;
    tgaP->Rsrvd = ( flags & 0x10 ) >> 4;
    tgaP->OrgBit = ( flags & 0x20 ) >> 5;
    tgaP->IntrLve = ( flags & 0xc0 ) >> 6;

    if ( tgaP->IDLength != 0 )
	fread( junk, 1, (int) tgaP->IDLength, ifd );
    }

get_map_entry( ifd, Value, Size )
FILE *ifd;
pixel *Value;
int Size;
    {
    unsigned char j, k, r, g, b;

    /* Read appropriate number of bytes, break into rgb & put in map. */
    switch ( Size )
	{
	case 8:				/* Grey scale, read and triplicate. */
	r = g = b = getbyte( ifd );
	break;

	case 16:			/* 5 bits each of red green and blue. */
	case 15:			/* Watch for byte order. */
	j = getbyte( ifd );
	k = getbyte( ifd );
	r = ( k & 0x7C ) >> 2;
	g = ( ( k & 0x03 ) << 3 ) + ( ( j & 0xE0 ) >> 5 );
	b = j & 0x1F;
	break;

	case 32:
	case 24:			/* 8 bits each of blue green and red. */
	b = getbyte( ifd );
	g = getbyte( ifd );
	r = getbyte( ifd );
	if ( Size == 32 )
	    (void) getbyte( ifd );	/* Read alpha byte & throw away. */
	break;

	default:
	pm_error( "unknown colormap pixel size (#2) - %d", Size, 0,0,0,0 );
	}
    PPM_ASSIGN( *Value, r, g, b );
    }

get_pixel( ifd, dest, Size )
FILE *ifd;
pixel *dest;
int Size;
    {
    static pixval Red, Grn, Blu;
    unsigned char j, k;
    static unsigned int l;

    /* Check if run length encoded. */
    if ( rlencoded )
	{
	if ( RLE_count == 0 )
	    { /* Have to restart run. */
	    unsigned char i;
	    i = getbyte( ifd );
	    RLE_flag = ( i & 0x80 ) >> 7;
	    if ( RLE_flag == 0 )
		/* Stream of unencoded pixels. */
		RLE_count = i + 1;
	    else
		/* Single pixel replicated. */
		RLE_count = i - 127;
	    /* Decrement count & get pixel. */
	    RLE_count--;
	    }
	else
	    { /* Have already read count & (at least) first pixel. */
	    RLE_count--;
	    if ( RLE_flag != 0 )
		/* Replicated pixels. */
		goto PixEncode;
	    }
	}
    /* Read appropriate number of bytes, break into RGB. */
    switch ( Size )
	{
	case 8:				/* Grey scale, read and triplicate. */
	Red = Grn = Blu = l = getbyte( ifd );
	break;

	case 16:			/* 5 bits each of red green and blue. */
	case 15:			/* Watch byte order. */
	j = getbyte( ifd );
	k = getbyte( ifd );
	l = (unsigned int) k << 8 + j;
	Red = ( k & 0x7C ) >> 2;
	Grn = ( ( k & 0x03 ) << 3 ) + ( ( j & 0xE0 ) >> 5 );
	Blu = j & 0x1F;
	break;

	case 32:
	case 24:			/* 8 bits each of blue green and red. */
	Blu = getbyte( ifd );
	Grn = getbyte( ifd );
	Red = getbyte( ifd );
	if ( Size == 32 )
	    (void) getbyte( ifd );	/* Read alpha byte & throw away. */
	l = 0;
	break;

	default:
	pm_error( "unknown pixel size (#2) - %d", Size, 0,0,0,0 );
	}

PixEncode:
    if ( mapped )
	*dest = ColorMap[l];
    else
	PPM_ASSIGN( *dest, Red, Grn, Blu );
    }

unsigned char
getbyte( ifd )
FILE *ifd;
    {
    unsigned char c;

    if ( fread( (char *) &c, 1, 1, ifd ) != 1 )
	pm_error( "premature EOF", 0,0,0,0,0 );

    return c;
    }
