/* ppmcscale.c - scale the colors in a portable pixmap
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
#include "ppm.h"

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    register pixel *pixelrow, *pP;
    int argn, rows, cols, format, row;
    register int col;
    pixval maxval, newmaxval;
    int i;
    char *usage = "newmaxval [ppmfile]";

    pm_progname = argv[0];

    argn = 1;

    if ( argn == argc )
	pm_usage( usage );
    if ( sscanf( argv[argn], "%d", &i ) != 1 )
	pm_usage( usage );
    newmaxval = i;
    argn++;
    if ( newmaxval < 1 )
	pm_error( "newmaxval must be > 1", 0,0,0,0,0 );

    if ( argn != argc )
	{
	ifd = pm_openr( argv[argn] );
	argn++;
	}
    else
	ifd = stdin;

    if ( argn != argc )
	pm_usage( usage );

    ppm_readppminit( ifd, &cols, &rows, &maxval, &format );
    pixelrow = ppm_allocrow( cols );

    ppm_writeppminit( stdout, cols, rows, newmaxval );

    for ( row = 0; row < rows; row++ )
	{
	ppm_readppmrow( ifd, pixelrow, cols, maxval, format );

	for ( col = 0, pP = pixelrow; col < cols; col++, pP++ )
	    PPM_CSCALE( *pP, *pP, maxval, newmaxval );

	ppm_writeppmrow( stdout, pixelrow, cols, newmaxval );
	}

    pm_close( ifd );

    exit( 0 );
    }
