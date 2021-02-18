/* ppmtoxwd.c - read a portable pixmap and produce a color X11 window dump
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
#include "ppmcmap.h"
#include "x11wd.h"

#define MAXCOLORS 256

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    register pixel **pixels, *pP;
    int argn, rows, cols, colors, depth, morecolors, i, row, col;
    pixval maxval;
    long lmaxval;
    colorhash_table cht;
    colorhist_vector chv;
    X11WDFileHeader h11;
    X11XColor color;

    pm_progname = argv[0];

    argn = 1;

    if ( argn < argc )
	{
	ifd = pm_openr( argv[1] );
	argn++;
	}
    else
	ifd = stdin;
    
    if ( argn != argc )
	pm_usage( "[ppmfile]" );

    pixels = ppm_readppm( ifd, &cols, &rows, &maxval );
    lmaxval = (long) maxval;

    pm_close( ifd );
    
    /* Figure out the colormap. */
    fprintf( stderr, "(Computing colormap..." );
    fflush( stderr );
    chv = ppm_computecolorhist( pixels, cols, rows, MAXCOLORS, &colors );
    if ( chv == (colorhist_vector) 0 )
	{
	fprintf(
	    stderr,
	    "  Too many colors!  Try running the pixmap through 'ppmquant 256'.\n" );
	exit( 1 );
	}
    fprintf( stderr, "  Done.  %d colors found.)\n", colors );
    /* Make a hash table for fast color lookup. */
    cht = ppm_colorhisttocolorhash( chv, colors );
    depth = colorstobpp( colors );
    /* Round depth up to the next byte, because xwud is stupid. */
    depth = ( depth + 7 ) / 8 * 8;
    morecolors = 1 << depth;

    /* Write out the header. */
    h11.header_size = sizeof(h11);
    h11.file_version = X11WD_FILE_VERSION;
    h11.pixmap_format = ZPixmap;
    h11.pixmap_depth = depth;
    h11.pixmap_width = cols;
    h11.pixmap_height = rows;
    h11.xoffset = 0;
    h11.byte_order = LSBFirst;
    h11.bitmap_unit = 8;
    h11.bitmap_bit_order = LSBFirst;
    h11.bitmap_pad = 8;
    h11.bits_per_pixel = 8;
    h11.bytes_per_line = cols;
    h11.visual_class = PseudoColor;
    h11.red_mask = 0;
    h11.green_mask = 0;
    h11.blue_mask = 0;
    h11.bits_per_rgb = h11.pixmap_depth;
    h11.colormap_entries = morecolors;
    h11.ncolors = morecolors;
    h11.window_width = cols;
    h11.window_height = rows;
    h11.window_x = 0;
    h11.window_y = 0;
    h11.window_bdrwidth = 0;
    fwrite( &h11, sizeof(h11), 1, stdout );

    /* Write out the colormap. */
    color.flags = 7;
    color.pad = 0;
    for ( i = 0; i < colors; i++ )
	{
	color.pixel = i;
	color.red = PPM_GETR( chv[i].color );
	color.green = PPM_GETG( chv[i].color );
	color.blue = PPM_GETB( chv[i].color );
	if ( lmaxval != 65535 )
	    {
	    color.red = (long) color.red * 65535 / lmaxval;
	    color.green = (long) color.green * 65535 / lmaxval;
	    color.blue = (long) color.blue * 65535 / lmaxval;
	    }
	fwrite( &color, sizeof(color), 1, stdout );
	}
    for ( ; i < morecolors; i++ )
	{
	color.pixel = i;
	color.red = 0;
	color.green = 0;
	color.blue = 0;
	fwrite( &color, sizeof(color), 1, stdout );
	}

    /* Finally, write out the data. */
    for ( row = 0; row < rows; row++ )
        for ( col = 0, pP = pixels[row]; col < cols; col++, pP++ )
	    putchar( ppm_lookupcolor( cht, *pP ) );

    exit( 0 );
    }

int
colorstobpp( colors )
int colors;
    {
    int bpp;

    if ( colors <= 2 )
	bpp = 1;
    else if ( colors <= 4 )
	bpp = 2;
    else if ( colors <= 8 )
	bpp = 3;
    else if ( colors <= 16 )
	bpp = 4;
    else if ( colors <= 32 )
	bpp = 5;
    else if ( colors <= 64 )
	bpp = 6;
    else if ( colors <= 128 )
	bpp = 7;
    else if ( colors <= 256 )
	bpp = 8;
    else
	pm_error( "can't happen", 0,0,0,0,0 );

    return bpp;
    }
