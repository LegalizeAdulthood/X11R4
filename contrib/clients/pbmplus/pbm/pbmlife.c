/* pbmlife.c - read a portable bitmap and apply Conway's rules of Life to it
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
    register bit **bits, *bitrow;
    int rows, cols, row, col, count;

    pm_progname = argv[0];

    if ( argc > 2 )
	pm_usage( "[pbmfile]" );

    if ( argc == 2 )
	ifd = pm_openr( argv[1] );
    else
	ifd = stdin;

    bits = pbm_readpbm( ifd, &cols, &rows );
    pbm_writepbminit( stdout, cols, rows );
    bitrow = pbm_allocrow( cols );

    pm_close( ifd );

    for ( row = 0; row < rows; row++ )
	{
        for ( col = 0; col < cols; col++ )
	    {
	    /* Check the neighborhood, with an unrolled double loop. */
	    count = 0;
	    if ( row - 1 >= 0 )
		{
		/* upper left */
		if ( col - 1 >= 0 && bits[row - 1][col - 1] )
		    count++;
		/* upper center */
		if ( bits[row - 1][col] )
		    count++;
		/* upper right */
		if ( col + 1 < cols && bits[row - 1][col + 1] )
		    count++;
		}
	    /* left */
	    if ( col - 1 >= 0 && bits[row][col - 1] )
		count++;
	    /* right */
	    if ( col + 1 < cols && bits[row][col + 1] )
		count++;
	    if ( row + 1 < rows )
		{
		/* lower left */
		if ( col - 1 >= 0 && bits[row + 1][col - 1] )
		    count++;
		/* lower center */
		if ( bits[row + 1][col] )
		    count++;
		/* lower right */
		if ( col + 1 < cols && bits[row + 1][col + 1] )
		    count++;
		}

	    /* And compute the new value. */
	    if ( bits[row][col] )
		if ( count == 2 || count == 3 )
		    bitrow[col] = 1;
		else
		    bitrow[col] = 0;
	    else
		if ( count == 3 )
		    bitrow[col] = 1;
		else
		    bitrow[col] = 0;
	    }
	pbm_writepbmrow( stdout, bitrow, cols );
	}

    exit( 0 );
    }
