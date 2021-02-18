/* pgmhist.c - print a histogram of the values in a portable graymap
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
#include "pgm.h"

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    gray maxval, *grayrow;
    register gray *gP;
    int argn, rows, cols, format, row;
    int i, *hist, *rcount, count, size;
    register int col;
    char *usage = "[pgmfile]";

    pm_progname = argv[0];

    argn = 1;

    if ( argn < argc )
	{
	ifd = pm_openr( argv[argn] );
	argn++;
	}
    else
	ifd = stdin;

    if ( argn != argc )
	pm_usage( usage );

    pgm_readpgminit( ifd, &cols, &rows, &maxval, &format );
    grayrow = pgm_allocrow( cols );

    /* Build histogram. */
    hist = (int *) malloc( ( maxval + 1 ) * sizeof(int) );
    rcount = (int *) malloc( ( maxval + 1 ) * sizeof(int) );
    if ( hist == (int *) 0 || rcount == (int *) 0 )
	pm_error( "out of memory", 0,0,0,0,0 );
    for ( i = 0; i <= maxval; i++ )
	hist[i] = 0;
    for ( row = 0; row < rows; row++ )
	{
	pgm_readpgmrow( ifd, grayrow, cols, maxval, format );
        for ( col = 0, gP = grayrow; col < cols; col++, gP++ )
	    hist[(int) *gP]++;
	}

    pm_close( ifd );

    /* Compute count-down */
    count = 0;
    for ( i = maxval; i >= 0; i-- )
	{
	count += hist[i];
	rcount[i] = count;
	}

    /* And print it. */
    printf( "value\tcount\tb%%\tw%%\n" );
    printf( "-----\t-----\t--\t--\n" );
    count = 0;
    size = rows * cols;
    for ( i = 0; i <= maxval; i++ )
	if ( hist[i] > 0 )
	    {
	    count += hist[i];
	    printf(
		"%d\t%d\t%5.3g%%\t%5.3g%%\n", i, hist[i],
		(float) count * 100.0 / size, (float) rcount[i] * 100.0 / size );
	    }

    exit( 0 );
    }
