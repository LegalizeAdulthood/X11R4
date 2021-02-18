/* ppmarith.c - perform arithmetic on two portable pixmaps
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
    FILE *ifd1, *ifd2;
    register pixel *pixrow1, *pixrow2, *p1P, *p2P;
    pixval maxval1, maxval2, maxval3;
    int argn, rows1, cols1, format1, rows2, cols2, format2, row, col;
    char function;
    char *usage = "-add|-subtract|-multiply ppmfile1 ppmfile2";

    pm_progname = argv[0];

    argn = 1;
    function = ' ';

    /* Check for flags. */
    if ( argn < argc && argv[argn][0] == '-' )
	{
	if ( strncmp(argv[argn],"-add",max(strlen(argv[argn]),2)) == 0 )
	    function = '+';
	else if ( strncmp(argv[argn],"-subtract",max(strlen(argv[argn]),2)) == 0 )
	    function = '-';
	else if ( strncmp(argv[argn],"-multiply",max(strlen(argv[argn]),2)) == 0 )
	    function = '*';
	else
	    pm_usage( usage );
	argn++;
	}

    if ( function == ' ' )
	pm_usage( usage );

    if ( argn == argc )
	pm_usage( usage );
    ifd1 = pm_openr( argv[argn] );
    argn++;

    if ( argn == argc )
	pm_usage( usage );
    ifd2 = pm_openr( argv[argn] );
    argn++;

    if ( argn != argc )
	pm_usage( usage );

    ppm_readppminit( ifd1, &cols1, &rows1, &maxval1, &format1 );
    pixrow1 = ppm_allocrow( cols1 );
    ppm_readppminit( ifd2, &cols2, &rows2, &maxval2, &format2 );
    if ( cols2 != cols1 || rows2 != rows1 )
	pm_error(
	    "the two pixmaps must be the same width and height", 0,0,0,0,0 );
    pixrow2 = ppm_allocrow( cols1 );

    maxval3 = max( maxval1, maxval2 );
    ppm_writeppminit( stdout, cols1, rows1, maxval3 );
    for ( row = 0; row < rows1; row++ )
	{
	ppm_readppmrow( ifd1, pixrow1, cols1, maxval1, format1 );
	ppm_readppmrow( ifd2, pixrow2, cols1, maxval2, format2 );
        for ( col = 0, p1P = pixrow1, p2P = pixrow2; col < cols1; col++, p1P++, p2P++ )
	    {
	    int r1, g1, b1, r2, g2, b2;

	    if ( maxval1 != maxval3 )
		PPM_CSCALE( *p1P, *p1P, maxval1, maxval3 );
	    r1 = PPM_GETR( *p1P );
	    g1 = PPM_GETG( *p1P );
	    b1 = PPM_GETB( *p1P );
	    if ( maxval2 != maxval3 )
		PPM_CSCALE( *p2P, *p2P, maxval2, maxval3 );
	    r2 = PPM_GETR( *p2P );
	    g2 = PPM_GETG( *p2P );
	    b2 = PPM_GETB( *p2P );
	    switch ( function )
		{
		case '+':
		r1 += r2;
		g1 += g2;
		b1 += b2;
		break;

		case '-':
		r1 -= r2;
		g1 -= g2;
		b1 -= b2;
		break;

		case '*':
		r1 = r1 * r2 / maxval3;
		g1 = g1 * g2 / maxval3;
		b1 = b1 * b2 / maxval3;
		break;

		default:
		pm_error( "can't happen", 0,0,0,0,0 );
		}
	    if ( r1 < 0 ) r1 = 0;
	    else if ( r1 > maxval3 ) r1 = maxval3;
	    if ( g1 < 0 ) g1 = 0;
	    else if ( g1 > maxval3 ) g1 = maxval3;
	    if ( b1 < 0 ) b1 = 0;
	    else if ( b1 > maxval3 ) b1 = maxval3;
	    PPM_ASSIGN( *p1P, r1, g1, b1 );
	    }
	ppm_writeppmrow( stdout, pixrow1, cols1, maxval3 );
	}

    pm_close( ifd1 );
    pm_close( ifd2 );

    exit( 0 );
    }
