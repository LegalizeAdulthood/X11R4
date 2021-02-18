/* pbmtolj.c - read a portable bitmap and produce a LaserJet bitmap file
**	
**	based on pbmtops.c
**
**	Michael Haberler HP Vienna mah@hpuviea.uucp
**				   mcvax!tuvie!mah
**	misfeatures: 
**		no positioning
**
**      Bug fix Dec 12, 1988 :
**              lines in putbit() reshuffled 
**              now runs OK on HP-UX 6.0 with X10R4 and HP Laserjet II
**      Bo Thide', Swedish Institute of Space Physics, Uppsala <bt@irfu.se>
**
** Copyright (C) 1988 by Jef Poskanzer and Michael Haberler.
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

int dpi = 75;
    
main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    register bit *bitrow, *bP;
    int argn, rows, cols, format, rucols, padright, row, col;
    char *usage = "[-r resolution] [pbmfile]\n\t\resolution = [75|100|150|300] (dpi)";

    pm_progname = argv[0];

    argn = 1;

    /* Check for flags. */
    if ( argc > argn )
	{
	if ( argv[argn][0] == '-' )
	    {
	    if ( strcmp( argv[argn], "-r" ) == 0 )
		{
		if ( argc == argn + 1 )
		    pm_usage( usage );
		if ( sscanf( argv[argn+1], "%d", &dpi ) != 1 )
		    pm_usage( usage );
		argn += 2;
		}
	    else
		pm_usage( usage );
	    }
	}

    if ( argc > argn + 1 )
	pm_usage( usage );

    if ( argc == argn + 1 )
	ifd = pm_openr( argv[argn] );
    else
	ifd = stdin;

    pbm_readpbminit( ifd, &cols, &rows, &format );
    bitrow = pbm_allocrow( cols );

    /* Round cols up to the nearest multiple of 8. */
    rucols = ( cols + 7 ) / 8;
    rucols = rucols * 8;
    padright = rucols - cols;

    putinit( );
    for ( row = 0; row < rows; row++ )
	{
	pbm_readpbmrow( ifd, bitrow, cols, format );
	/* Transfer raster graphics */
 	printf("\033*b%dW",rucols/8);
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
    /* Printer reset. */
    printf("\033E");

    /* Set raster graphics resolution */
    printf("\033*t%dR",dpi);

    /* Start raster graphics, relative adressing */
    printf("\033*r1A");

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

    /* end raster graphics */
    printf( "\033*rB" );

    /* Printer reset. */
    printf("\033E");
    }

putitem( )
    {
    putchar( item );
    bitsperitem = 0;
    item = 0;
    }
