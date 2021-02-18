/* pnminvert.c - read a portable anymap and invert it
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
    xelval maxval;
    register xel *xelrow, *xP;
    int rows, cols, format, row, col;

    pm_progname = argv[0];

    if ( argc > 2 )
	pm_usage( "[pnmfile]" );

    if ( argc == 2 )
	ifd = pm_openr( argv[1] );
    else
	ifd = stdin;

    pnm_readpnminit( ifd, &cols, &rows, &maxval, &format );
    pnm_writepnminit( stdout, cols, rows, maxval, format );
    xelrow = pnm_allocrow( cols );

    for ( row = 0; row < rows; row++ )
	{
	pnm_readpnmrow( ifd, xelrow, cols, maxval, format );
        for ( col = 0, xP = xelrow; col < cols; col++, xP++ )
	    *xP = pnm_invertxel( *xP, maxval, format );

	pnm_writepnmrow( stdout, xelrow, cols, maxval, format );
	}

    pm_close( ifd );

    exit( 0 );
    }
