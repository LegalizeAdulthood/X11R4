/* brushtopbm.c - read a doodle brush file and write a portable bitmap
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
    bit getbit();
    int rows, cols, padright, row, col;

    pm_progname = argv[0];

    if ( argc > 2 )
	pm_usage( "[brushfile]" );

    if ( argc == 2 )
	ifd = pm_openr( argv[1] );
    else
	ifd = stdin;

    getinit( ifd, &cols, &rows );

    pbm_writepbminit( stdout, cols, rows );
    bitrow = pbm_allocrow( cols );

    /* Compute padding to round cols up to the next multiple of 16. */
    padright = ( ( cols + 15 ) / 16 ) * 16 - cols;

    for ( row = 0; row < rows; row++ )
	{
	/* Get data. */
        for ( col = 0, bP = bitrow; col < cols; col++, bP++ )
	    *bP = getbit( ifd );
	/* Discard line padding. */
        for ( col = 0; col < padright; col++ )
	    (void) getbit( ifd );
	/* Write row. */
	pbm_writepbmrow( stdout, bitrow, cols );
	}

    pm_close( ifd );
    
    exit( 0 );
    }


int item, bitsperitem, bitshift;

getinit( file, colp, rowp )
FILE *file;
int *colp, *rowp;
    {
    int i;

    if ( getc( file ) != 1 )
	pm_error( "bad magic number 1", 0,0,0,0,0 );
    if ( getc( file ) != 0 )
	pm_error( "bad magic number 2", 0,0,0,0,0 );
    *colp = getc( file ) << 8;
    *colp += getc( file );
    *rowp = getc( file ) << 8;
    *rowp += getc( file );
    bitsperitem = 8;

    /* Junk rest of header. */
    for ( i = 0; i < 10; i++ )  /* 10 is just a guess at the header size */
	(void) getc( file );
    }

bit
getbit( file )
FILE *file;
    {
    bit b;

    if ( bitsperitem == 8 )
	{
	item = getc( file );
	bitsperitem = 0;
	bitshift = 7;
	}
    bitsperitem++;
    b = ( ( item >> bitshift) & 1 ) ? PBM_WHITE : PBM_BLACK;
    bitshift--;
    return b;
    }
