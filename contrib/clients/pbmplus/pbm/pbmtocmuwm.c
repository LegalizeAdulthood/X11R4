/* pbmtocmuwm.c - read a portable bitmap and produce a CMU window manager bitmap
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
#include "pbm.h"
#include "cmuwm.h"

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    register bit *bitrow, *bP;
    int rows, cols, format, padright, row, col;

    pm_progname = argv[0];

    if ( argc > 2 )
	pm_usage( "[pbmfile]" );

    if ( argc == 2 )
	ifd = pm_openr( argv[1] );
    else
	ifd = stdin;

    pbm_readpbminit( ifd, &cols, &rows, &format );
    bitrow = pbm_allocrow( cols );
    
    /* Round cols up to the nearest multiple of 8. */
    padright = ( ( cols + 7 ) / 8 ) * 8 - cols;

    putinit( rows, cols );
    for ( row = 0; row < rows; row++ )
	{
	pbm_readpbmrow( ifd, bitrow, cols, format );
        for ( col = 0, bP = bitrow; col < cols; col++, bP++ )
	    putbit( *bP );
	for ( col = 0; col < padright; col++ )
	    putbit( 0 );
        }

    pm_close( ifd );

    putrest( );

    exit( 0 );
    }


unsigned char item;
int bitsperitem, bitshift;

putinit( rows, cols )
int rows, cols;
    {
    if ( pm_writebiglong( stdout, CMUWM_MAGIC ) == -1 )
	pm_perror( 0 );
    if ( pm_writebiglong( stdout, cols ) == -1 )
	pm_perror( 0 );
    if ( pm_writebiglong( stdout, rows ) == -1 )
	pm_perror( 0 );
    if ( pm_writebigshort( stdout, (short) 1 ) == -1 )
	pm_perror( 0 );

    item = 0;
    bitsperitem = 0;
    bitshift = 7;
    }

putbit( b )
bit b;
    {
    if ( bitsperitem == 8 )
	putitem( );
    if ( b == PBM_WHITE )
	item += 1 << bitshift;
    bitsperitem++;
    bitshift--;
    }

putrest( )
    {
    if ( bitsperitem > 0 )
	putitem( );
    }

putitem( )
    {
    if ( putc( item, stdout ) == EOF )
	pm_perror( 0 );
    item = 0;
    bitsperitem = 0;
    bitshift = 7;
    }
