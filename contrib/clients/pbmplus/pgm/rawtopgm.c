/* rawtopgm.c - convert raw grayscale bytes into a portable graymap
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
    register gray *grayrow, *gP;
    int argn, row;
    register int col, val;
    int maxval;
    int rows, cols;
    char *usage = "<width> <height> [rawdata]";

    pm_progname = argv[0];

    argn = 1;

    if ( argn + 3 > argc )
	pm_usage( usage );

    cols = atoi( argv[argn++] );
    rows = atoi( argv[argn++] );
    if ( cols <= 0 || rows <= 0 )
	pm_usage( usage );

    if ( argn < argc )
	{
	ifd = pm_openr( argv[argn] );
	argn++;
	}
    else
	ifd = stdin;

    if ( argn != argc )
	pm_usage( usage );

    maxval = 255;
    if ( maxval > PGM_MAXMAXVAL )
	pm_error( "maxval of 255 is too large - try recompiling with a larger gray type" );

    pgm_writepgminit( stdout, cols, rows, (gray) maxval );
    grayrow = pgm_allocrow( ( cols + 7 ) / 8 * 8 );
    for ( row = 0; row < rows; row++)
	{
	for ( col = 0, gP = grayrow; col < cols; ++col )
	    {
	    val = getc( ifd );
	    if ( val == EOF )
		pm_error( "premature EOF", 0,0,0,0,0 );
	    *gP++ = val;
	    }
	pgm_writepgmrow( stdout, grayrow, cols, (gray) maxval );
	}
    pm_close( ifd );

    exit( 0 );
    }
