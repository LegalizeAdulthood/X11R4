/* pnmtile.c - replicate a portable anymap into a specified size
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
#include "pnm.h"

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    register xel **xels, *xelrow;
    xelval maxval;
    int rows, cols, format, width, height, row, col;
    char *usage = "width height [pnmfile]";

    pm_progname = argv[0];

    if ( argc < 3 || argc > 4 )
	pm_usage( usage );

    if ( sscanf( argv[1], "%d", &width ) != 1 )
	pm_usage( usage );
    if ( sscanf( argv[2], "%d", &height ) != 1 )
	pm_usage( usage );

    if ( width < 1 )
	pm_error( "width is less than 1", 0,0,0,0,0 );
    if ( height < 1 )
	pm_error( "height is less than 1", 0,0,0,0,0 );

    if ( argc == 4 )
	ifd = pm_openr( argv[3] );
    else
	ifd = stdin;

    xels = pnm_readpnm( ifd, &cols, &rows, &maxval, &format );
    pm_close( ifd );

    xelrow = pnm_allocrow( width );

    pnm_writepnminit( stdout, width, height, maxval, format );
    for ( row = 0; row < height; row++ )
	{
	for ( col = 0; col < width; col++ )
	    xelrow[col] = xels[row % rows][col % cols];
	pnm_writepnmrow( stdout, xelrow, width, maxval, format );
	}

    exit( 0 );
    }
