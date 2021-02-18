/* cmuwmtopbm.c - read a CMU window manager bitmap and produce a portable bitmap
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
    int rows, cols, padright, row, col;
    short depth;
    bit getbit();

    pm_progname = argv[0];

    if ( argc > 2 )
	pm_usage( "[cmuwmfile]" );

    if ( argc == 2 )
	ifd = pm_openr( argv[1] );
    else
	ifd = stdin;

    getinit( ifd, &cols, &rows, &depth, &padright );
    if ( depth != 1 )
	pm_error(
	    "CMU window manager file has depth of %d, must be 1",
	    (int) depth, 0,0,0,0 );

    pbm_writepbminit( stdout, cols, rows );
    bitrow = pbm_allocrow( cols );

    for ( row = 0; row < rows; row++ )
	{
	/* Get data. */
        for ( col = 0, bP = bitrow; col < cols; col++, bP++ )
	    *bP = getbit( ifd );
	/* Discard line padding */
        for ( col = 0; col < padright; col ++ )
	    (void) getbit( ifd );
	pbm_writepbmrow( stdout, bitrow, cols );
	}

    pm_close( ifd );

    exit( 0 );
    }


int item;
int bitsperitem, bitshift;

getinit( file, colsP, rowsP, depthP, padrightP )
FILE *file;
int *colsP, *rowsP, *padrightP;
short *depthP;
    {
    long magic;

    if ( pm_readbiglong( file, &magic ) == -1 )
	pm_error( "premature EOF" );
    if ( magic != CMUWM_MAGIC )
	pm_error( "bad magic number in CMU window manager file", 0,0,0,0,0 );
    if ( pm_readbiglong( file, colsP ) == -1 )
	pm_error( "premature EOF" );
    if ( pm_readbiglong( file, rowsP ) == -1 )
	pm_error( "premature EOF" );
    if ( pm_readbigshort( file, depthP ) == -1 )
	pm_error( "premature EOF" );
    *padrightP = ( ( *colsP + 7 ) / 8 ) * 8 - *colsP;

    bitsperitem = 0;
    }

bit
getbit( file )
FILE *file;
    {
    bit b;

    if ( bitsperitem == 0 )
	{
	item = getc( file );
	if ( item == EOF )
	    pm_error( "premature EOF", 0,0,0,0,0 );
	bitsperitem = 8;
	bitshift = 7;
	}
    b = ( ( item >> bitshift) & 1 ) ? PBM_WHITE : PBM_BLACK;
    bitsperitem--;
    bitshift--;
    return b;
    }
