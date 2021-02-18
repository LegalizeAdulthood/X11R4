/* qrttoppm.c - read a QRT ray-tracer output file and produce a portable pixmap
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

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    register pixel *pixelrow;
    int rows, cols, row, col;
    pixval maxval;
    unsigned char *buf;

    pm_progname = argv[0];

    if ( argc > 2 )
	pm_usage( "[qrtfile]" );

    if ( argc == 2 )
	ifd = pm_openr( argv[1] );
    else
	ifd = stdin;

    /* Read in the QRT file.  First the header. */
    cols = getc( ifd );
    cols += getc( ifd ) << 8;
    rows = getc( ifd );
    rows += getc( ifd ) << 8;

    if ( cols <= 0 || rows <= 0 )
	pm_error( "invalid size: %d %d", cols, rows, 0,0,0 );
    maxval = 255;

    ppm_writeppminit( stdout, cols, rows, maxval );
    pixelrow = ppm_allocrow( cols );
    buf = (unsigned char *) malloc( 3 * cols );
    if ( buf == (unsigned char *) 0 )
	pm_error( "out of memory", 0,0,0,0,0 );

    for ( row = 0; row < rows; row++ )
	{
        (void) getc( ifd );	/* discard */
        (void) getc( ifd );	/* linenum */
	if ( fread( buf, 3 * cols, 1, ifd ) != 1 )
	    pm_error( "premature EOF", 0,0,0,0,0 );
	for ( col = 0; col < cols; col++ )
	    PPM_ASSIGN(
		pixelrow[col], buf[col], buf[cols + col], buf[2 * cols + col] );
	ppm_writeppmrow( stdout, pixelrow, cols, maxval );
	}

    pm_close( ifd );

    exit( 0 );
    }
