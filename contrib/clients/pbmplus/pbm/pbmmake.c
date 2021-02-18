/* pbmmake.c - create a blank bitmap of a specified size
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

#define max(a,b) ((a) > (b) ? (a) : (b))

main( argc, argv )
int argc;
char *argv[];
    {
    register bit color, gray, rowcolor, *bitrow, *bP;
    int argn, rows, cols, row, col;
    char *usage = "[-white|-black|-gray] <width> <height>";

    pm_progname = argv[0];

    argn = 1;
    color = 0;
    gray = 0;

    /* Check for flags. */
    if ( argn < argc  && argv[argn][0] == '-' )
	{
	if ( strncmp(argv[argn],"-white",max(strlen(argv[argn]),2)) == 0 )
	    color = PBM_WHITE;
	else if ( strncmp(argv[argn],"-black",max(strlen(argv[argn]),2)) == 0 )
	    color = PBM_BLACK;
	else if ( strncmp(argv[argn],"-gray",max(strlen(argv[argn]),2)) == 0 ||
	          strncmp(argv[argn],"-grey",max(strlen(argv[argn]),2)) == 0 )
	    gray = 1;
	else
	    pm_usage( usage );
	argn++;
	}

    if ( argn == argc )
	pm_usage( usage );
    if ( sscanf( argv[argn], "%d", &cols ) != 1 )
	pm_usage( usage );
    argn++;
    if ( argn == argc )
	pm_usage( usage );
    if ( sscanf( argv[argn], "%d", &rows ) != 1 )
	pm_usage( usage );
    argn++;

    if ( argn != argc )
	pm_usage( usage );

    pbm_writepbminit( stdout, cols, rows );
    bitrow = pbm_allocrow( cols );

    if ( gray )
	rowcolor = PBM_WHITE;  /* arbitrarily make the corner white */
    for ( row = 0; row < rows; row++ )
	{
	if ( gray )
	    {
	    color = rowcolor;
	    rowcolor = 1 - rowcolor;
	    }
        for ( col = 0, bP = bitrow; col < cols; col++, bP++ )
	    {
	    *bP = color;
	    if ( gray )
		color = 1 - color;
	    }
	pbm_writepbmrow( stdout, bitrow, cols );
	}

    exit( 0 );
    }
