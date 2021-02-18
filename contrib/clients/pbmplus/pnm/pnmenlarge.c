/* pnmenlarge.c - read a portable anymap and enlarge it N times
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
#include "pnm.h"

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    register xel *xelrow, *newxelrow, *xP;
    int argn, n, rows, cols, format, row, col, subrow, subcol;
    xelval maxval;
    char *usage = "N [pnmfile]";

    pm_progname = argv[0];

    argn = 1;

    if ( argn == argc )
	pm_usage( usage );
    if ( sscanf( argv[argn], "%d", &n ) != 1 )
	pm_usage( usage );
    if ( n < 2 )
	pm_error( "N must be greater than 1", 0,0,0,0,0 );
    argn++;

    if ( argn != argc )
	{
	ifd = pm_openr( argv[argn] );
	argn++;
	}
    else
	ifd = stdin;

    if ( argn != argc )
	pm_usage( usage );

    pnm_readpnminit( ifd, &cols, &rows, &maxval, &format );
    xelrow = pnm_allocrow( cols );
    pnm_writepnminit( stdout, cols * n, rows * n, maxval, format );
    newxelrow = pnm_allocrow( cols * n );

    for ( row = 0; row < rows; row++ )
	{
	pnm_readpnmrow( ifd, xelrow, cols, maxval, format );
	for ( subrow = 0; subrow < n; subrow++ )
	    {
	    for ( col = 0, xP = xelrow; col < cols; col++, xP++ )
		{
		for ( subcol = 0; subcol < n; subcol++ )
		    newxelrow[col * n + subcol] = *xP;
		}
	    pnm_writepnmrow( stdout, newxelrow, cols * n, maxval, format );
	    }
	}

    pm_close( ifd );

    exit( 0 );
    }
