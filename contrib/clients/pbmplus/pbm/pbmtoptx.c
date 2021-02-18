/* pbmtoptx.c - read a portable bitmap and produce a Printronix printer file
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
    int rows, cols, format, row, col;
    char *usage = "[pbmfile]";

    pm_progname = argv[0];

    if ( argc > 2 )
	pm_usage( usage );

    if ( argc == 2 )
	ifd = pm_openr( argv[1] );
    else
	ifd = stdin;

    pbm_readpbminit( ifd, &cols, &rows, &format );
    bitrow = pbm_allocrow( cols );

    putinit( );
    for ( row = 0; row < rows; row++ )
	{
	pbm_readpbmrow( ifd, bitrow, cols, format );
        for ( col = 0, bP = bitrow; col < cols; col++, bP++ )
	    putbit( *bP );
	putrest( );
	putchar( 5 );
	putchar( '\n' );
        }

    pm_close( ifd );
    
    exit( 0 );
    }


char item;
int bitsperitem, bitshift;

putinit( )
    {
    bitsperitem = 0;
    item = 64;
    bitshift = 0;
    }

putbit( b )
bit b;
    {
    if ( bitsperitem == 6 )
	putitem( );
    if ( b == PBM_BLACK )
	item += 1 << bitshift;
    bitsperitem++;
    bitshift++;
    }

putrest( )
    {
    if ( bitsperitem > 0 )
	putitem( );
    }

putitem( )
    {
    putchar( item );
    bitsperitem = 0;
    item = 64;
    bitshift = 0;
    }
