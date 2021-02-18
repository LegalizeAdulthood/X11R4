/* pnmcut.c - cut a rectangle out of a portable anymap
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
    register xel *xelrow;
    xelval maxval;
    int rows, cols, format, x, y, width, height, row;
    char *usage = "x y width height [pnmfile]";

    pm_progname = argv[0];

    if ( argc < 5 || argc > 6 )
	pm_usage( usage );

    if ( sscanf( argv[1], "%d", &x ) != 1 )
	pm_usage( usage );
    if ( sscanf( argv[2], "%d", &y ) != 1 )
	pm_usage( usage );
    if ( sscanf( argv[3], "%d", &width ) != 1 )
	pm_usage( usage );
    if ( sscanf( argv[4], "%d", &height ) != 1 )
	pm_usage( usage );

    if ( x < 0 )
	pm_error( "x is less than 0", 0,0,0,0,0 );
    if ( y < 0 )
	pm_error( "y is less than 0", 0,0,0,0,0 );
    if ( width < 1 )
	pm_error( "width is less than 1", 0,0,0,0,0 );
    if ( height < 1 )
	pm_error( "height is less than 1", 0,0,0,0,0 );

    if ( argc == 6 )
	ifd = pm_openr( argv[5] );
    else
	ifd = stdin;

    pnm_readpnminit( ifd, &cols, &rows, &maxval, &format );
    xelrow = pnm_allocrow( cols );

    if ( x >= cols )
	pm_error(
	    "x is too large -- the anymap has only %d cols", cols, 0,0,0,0 );
    if ( y >= rows )
	pm_error(
	    "y is too large -- the anymap has only %d rows", rows, 0,0,0,0 );
    if ( x + width > cols )
	pm_error(
	    "x + width is too large by %d xels", x + width - cols, 0,0,0,0 );
    if ( y + height > rows )
	pm_error(
	    "y + height is too large by %d xels", y + height - rows, 0,0,0,0 );

    pnm_writepnminit( stdout, width, height, maxval, format );
    for ( row = 0; row < y + height; row++ )
	{
	pnm_readpnmrow( ifd, xelrow, cols, maxval, format );
	if ( row >= y )
	    pnm_writepnmrow( stdout, &(xelrow[x]), width, maxval, format );
	}

    pm_close( ifd );

    exit( 0 );
    }
