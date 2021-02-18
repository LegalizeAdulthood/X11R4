/* pgmenhance.c - edge-enhance a portable graymap
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
    register gray **grays, *newgrayrow, *ngP;
    int argn, n, rows, cols, row, col;
    float phi, omphi;
    gray maxval, sum;
    long newval;
    char *usage = "[-N] [pgmfile]  ( 1 <= N <= 9, default = 9 )";

    pm_progname = argv[0];

    argn = 1;
    n = 9;

    if ( argn < argc )
	{
	if ( argv[argn][0] == '-' )
	    {
	    if ( sscanf( &(argv[argn][1]), "%d", &n ) != 1 )
		pm_usage( usage );
	    if ( n < 1 || n > 9 )
		pm_usage( usage );
	    argn++;
	    }
	}

    if ( argn != argc )
	{
	ifd = pm_openr( argv[argn] );
	argn++;
	}
    else
	ifd = stdin;

    if ( argn != argc )
	pm_usage( usage );

    pgm_pbmmaxval = 255;	/* use a larger value for better (?) results */
    grays = pgm_readpgm( ifd, &cols, &rows, &maxval );

    pgm_writepgminit( stdout, cols, rows, maxval );
    newgrayrow = pgm_allocrow( cols );

    /* The edge enhancing technique is taken from Philip R. Thompson's "xim"
    ** program, which in turn took it from section 6 of "Digital Halftones by
    ** Dot Diffusion", D. E. Knuth, ACM Transaction on Graphics Vol. 6, No. 4,
    ** October 1987, which in turn got it from two 1976 papers by J. F. Jarvis
    ** et. al.
    */
    phi = n / 10.0;
    omphi = 1.0 - phi;

    /* Row 0. */
    for ( col = 0, ngP = newgrayrow; col < cols; col++, ngP++ )
	*ngP = grays[0][col];
    pgm_writepgmrow( stdout, newgrayrow, cols, maxval );

    /* Other rows. */
    for ( row = 1; row < rows - 1; row++ )
	{
	ngP = newgrayrow;
	*ngP = grays[row][0];
	ngP++;
	for ( col = 1; col < cols - 1; col++, ngP++ )
	    {
	    /* Compute the sum of the neighborhood. */
	    sum =
		grays[row-1][col-1] + grays[row-1][col] + grays[row-1][col+1] +
		grays[row  ][col-1] + grays[row  ][col] + grays[row  ][col+1] +
		grays[row+1][col-1] + grays[row+1][col] + grays[row+1][col+1];
	    /* Now figure new value. */
	    newval =
		(long) ( ( grays[row][col] - phi * sum / 9 ) / omphi + 0.5 );
	    if ( newval < 0 )
		newgrayrow[col] = 0;
	    else if ( newval > maxval )
		newgrayrow[col] = maxval;
	    else
		newgrayrow[col] = newval;
	    }
	newgrayrow[cols - 1] = grays[row][cols - 1];
	pgm_writepgmrow( stdout, newgrayrow, cols, maxval );
	}

    /* Last row. */
    for ( col = 0, ngP = newgrayrow; col < cols; col++, ngP++ )
	*ngP = grays[rows - 1][col];
    pgm_writepgmrow( stdout, newgrayrow, cols, maxval );

    pm_close( ifd );

    exit( 0 );
    }
