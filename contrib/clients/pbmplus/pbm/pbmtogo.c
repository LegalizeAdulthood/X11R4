/* pbmtogo.c - read a portable bitmap and produce a GraphOn terminal raster file
**	
**	based on pbmtolj.c
**
**	Bo Thide', Swedish Institute of Space Physics, bt@irfu.se
**				   
**	misfeatures: 
**		No 2D compression, no positioning
**
** Copyright (C) 1988 by Jef Poskanzer, Michael Haberler, and Bo Thide'.
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
#ifdef SYSV
#include <string.h>
#else /*SYSV*/
#include <strings.h>
#endif /*SYSV*/

    
main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    register bit *bitrow, *bP;
    int argn, rows, cols, format, rucols, padright, row, col;
    char *usage = "[-c] [pbmfile]";

    pm_progname = argv[0];

    argn = 2;

    /* Check for flags. */
    if ( argc > argn )
	{
	if ( argv[argn][0] == '-' )
	    {
	    if ( strcmp( argv[argn], "-c" ) == 0 )
		pm_error( "compression not implemented yet", 0,0,0,0,0 );
	    argn++;
	    }
	else
	    pm_usage( usage );
	}

    if ( argc > argn + 1 )
	pm_usage( usage );

    if ( argc == argn )
	ifd = pm_openr( argv[argn-1] );
    else
	ifd = stdin;

    pbm_readpbminit( ifd, &cols, &rows, &format );
    bitrow = pbm_allocrow( cols );

    /* Round cols up to the nearest multiple of 8. */
    rucols = ( cols + 7 ) / 8;
    rucols = rucols * 8;
    padright = rucols - cols;

    putinit( );

    /* Star donwloading screen raster*/
    printf( "\033P0;0;1;4;101;%d;%d;101!R", rows + 100, rucols + 100 );
    for ( row = 0; row < rows; row++ )
	{
	pbm_readpbmrow( ifd, bitrow, cols, format );
	/* Transfer raster graphics */
	printf( "%d/",rucols / 8 );  /* No. of bytes per row */
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
    /* Enter graphics window */
    printf( "\0331" );

    /* Erase graphics window */
    printf( "\033\014" );

    /* Set graphics window in raster mode */
    printf( "\033r" );

    /* Select standard Tek coding **/
    printf( "\033[=11l" );

    itemsperline = 0;
    bitsperitem = 1;
    item = 0;
    bitshift = 7;
    firstitem = 1;
    }

putbit( b )
bit b;
    {
    if ( b == PBM_BLACK )
	item += 1 << bitshift;
    bitshift--;
    if ( bitsperitem == 8 ) {
	putitem( );
        bitshift = 7;
    }
    bitsperitem++;
    }

putrest( )
    {
    if ( bitsperitem > 1 )
	putitem( );

    /* end raster downloading */
    printf( "\033\134" );

    /* Exit raster mode */
    printf( "\033t" );

    /* Exit graphics window
    printf( "\0332" ); */
    }

putitem( )
    {
    putchar( item );
    bitsperitem = 0;
    item = 0;
    }
