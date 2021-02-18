/* psidtopgm.c - convert PostScript "image" data into a portable graymap
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
    int rows, cols, bitspersample;
    char *usage = "<width> <height> <bits/sample> [imagedata]";

    pm_progname = argv[0];

    argn = 1;

    if ( argn + 3 > argc )
	pm_usage( usage );

    cols = atoi( argv[argn++] );
    rows = atoi( argv[argn++] );
    bitspersample = atoi( argv[argn++] );
    if ( cols <= 0 || rows <= 0 || bitspersample <= 0 )
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

    maxval = ( 1 << bitspersample ) - 1;
    if ( maxval > PGM_MAXMAXVAL )
	pm_error( "bits/sample is too large - try recompiling with a larger gray type" );

    pgm_writepgminit( stdout, cols, rows, (gray) maxval );
    grayrow = pgm_allocrow( ( cols + 7 ) / 8 * 8 );
    for ( row = 0; row < rows; row++)
	{
	for ( col = 0, gP = grayrow; col < cols; )
	    {
	    val = gethexit( ifd ) << 4;
	    val += gethexit( ifd );
	    switch ( bitspersample )
		{
		case 1:
		*gP++ = val >> 7;
		*gP++ = ( val >> 6 ) & 0x1;
		*gP++ = ( val >> 5 ) & 0x1;
		*gP++ = ( val >> 4 ) & 0x1;
		*gP++ = ( val >> 3 ) & 0x1;
		*gP++ = ( val >> 2 ) & 0x1;
		*gP++ = ( val >> 1 ) & 0x1;
		*gP++ = val & 0x1;
		col += 8;
		break;

		case 2:
		*gP++ = val >> 6;
		*gP++ = ( val >> 4 ) & 0x3;
		*gP++ = ( val >> 2 ) & 0x3;
		*gP++ = val & 0x3;
		col += 4;
		break;

		case 4:
		*gP++ = val >> 4;
		*gP++ = val & 0xf;
		col += 2;
		break;

		case 8:
		*gP++ = val;
		col++;
		break;

		default:
		pm_error(
		    "bitspersample of %d not supported", bitspersample,
		    0,0,0,0 );
		}
	    }
	pgm_writepgmrow( stdout, grayrow, cols, (gray) maxval );
	}
    pm_close( ifd );

    exit( 0 );
    }

int
gethexit( ifd )
FILE *ifd;
    {
    register int i;
    register char c;

    for ( ; ; )
	{
	i = getc( ifd );
	if ( i == EOF )
	    pm_error( "premature EOF", 0,0,0,0,0 );
	c = (char) i;
	if ( c >= '0' && c <= '9' )
	    return c - '0';
	else if ( c >= 'A' && c <= 'F' )
	    return c - 'A' + 10;
	else if ( c >= 'a' && c <= 'f' )
	    return c - 'a' + 10;
	/* Else ignore - whitespace. */
	}
    }
