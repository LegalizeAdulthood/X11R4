/* pbmtoicon.c - read a portable bitmap and produce a Sun icon file
**
** Copyright (C) 1988 by Jef Poskanzer.
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

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    register bit *bitrow, *bP;
    int rows, cols, format, pad, padleft, padright, row, col;

    pm_progname = argv[0];

    if ( argc > 2 )
	pm_usage( "[pbmfile]" );

    if ( argc == 2 )
	ifd = pm_openr( argv[1] );
    else
	ifd = stdin;

    pbm_readpbminit( ifd, &cols, &rows, &format );
    bitrow = pbm_allocrow( cols );
    
    /* Round cols up to the nearest multiple of 16. */
    pad = ( ( cols + 15 ) / 16 ) * 16 - cols;
    padleft = pad / 2;
    padright = pad - padleft;

    printf( "/* Format_version=1, Width=%d, Height=%d", cols + pad, rows );
    printf( ", Depth=1, Valid_bits_per_item=16\n */\n" );

    putinit( );
    for ( row = 0; row < rows; row++ )
	{
	pbm_readpbmrow( ifd, bitrow, cols, format );
	for ( col = 0; col < padleft; col++ )
	    putbit( 0 );
        for ( col = 0, bP = bitrow; col < cols; col++, bP++ )
	    putbit( *bP );
	for ( col = 0; col < padright; col++ )
	    putbit( 0 );
        }

    pm_close( ifd );

    putrest( );

    exit( 0 );
    }


int item, bitsperitem, bitshift, itemsperline, firstitem;

putinit( )
    {
    itemsperline = 0;
    bitsperitem = 0;
    item = 0;
    bitshift = 15;
    firstitem = 1;
    }

putbit( b )
bit b;
    {
    if ( bitsperitem == 16 )
	putitem( );
    bitsperitem++;
    if ( b == PBM_BLACK )
	item += 1 << bitshift;
    bitshift--;
    }

putrest( )
    {
    if ( bitsperitem > 0 )
	putitem( );
    putchar( '\n' );
    }

putitem( )
    {
    char *hexits = "0123456789abcdef";

    if ( firstitem )
	firstitem = 0;
    else
	putchar( ',' );
    if ( itemsperline == 8 )
	{
	putchar( '\n' );
	itemsperline = 0;
	}
    if ( itemsperline == 0 )
	putchar( '\t' );
    putchar( hexits[item >> 12] );
    putchar( hexits[( item >> 8 ) & 15] );
    putchar( hexits[( item >> 4 ) & 15] );
    putchar( hexits[item & 15] );
    itemsperline++;
    bitsperitem = 0;
    item = 0;
    bitshift = 15;
    }
