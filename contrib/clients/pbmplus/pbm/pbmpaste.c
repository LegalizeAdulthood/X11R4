/* pbmpaste.c - paste a rectangle into a portable bitmap
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
    FILE *ifd1, *ifd2;
    register bit *bitrow1, **bits2, *b1P, *b2P;
    int argn, rows1, cols1, format1, x, y, rows2, cols2, row, col;
    char function;
    char *usage = "[-replace|-or|-and|-xor] frompbmfile x y [intopbmfile]";

    pm_progname = argv[0];

    argn = 1;
    function = 'r';

    /* Check for flags. */
    if ( argn < argc && argv[argn][0] == '-' )
	{
	if ( strncmp(argv[argn],"-replace",max(strlen(argv[argn]),2)) == 0 )
	    function = 'r';
	else if ( strncmp(argv[argn],"-or",max(strlen(argv[argn]),2)) == 0 )
	    function = 'o';
	else if ( strncmp(argv[argn],"-and",max(strlen(argv[argn]),2)) == 0 )
	    function = 'a';
	else if ( strncmp(argv[argn],"-xor",max(strlen(argv[argn]),2)) == 0 )
	    function = 'x';
	else
	    pm_usage( usage );
	argn++;
	}

    if ( argn == argc )
	pm_usage( usage );
    ifd1 = pm_openr( argv[argn] );
    pbm_readpbminit( ifd1, &cols1, &rows1, &format1 );
    bitrow1 = pbm_allocrow( cols1 );
    argn++;

    if ( argn == argc )
	pm_usage( usage );
    if ( sscanf( argv[argn], "%d", &x ) != 1 )
	pm_usage( usage );
    argn++;
    if ( argn == argc )
	pm_usage( usage );
    if ( sscanf( argv[argn], "%d", &y ) != 1 )
	pm_usage( usage );
    argn++;

    if ( argn == argc )
	ifd2 = stdin;
    else
	{
	ifd2 = pm_openr( argv[argn] );
	argn++;
	}
    bits2 = pbm_readpbm( ifd2, &cols2, &rows2 );
    pm_close( ifd2 );

    if ( x <= -cols2 )
	pm_error(
	    "x is too negative -- the second bitmap has only %d cols",
	    cols2, 0,0,0,0 );
    if ( y <= -rows2 )
	pm_error(
	    "y is too negative -- the second bitmap has only %d rows",
	    rows2, 0,0,0,0 );
    if ( x < 0 )
	x = cols2 - x;
    if ( y < 0 )
	y = rows2 - y;

    if ( x >= cols2 )
	pm_error(
	    "x is too large -- the second bitmap has only %d cols",
	    cols2, 0,0,0,0 );
    if ( y >= rows2 )
	pm_error(
	    "y is too large -- the second bitmap has only %d rows",
	    rows2, 0,0,0,0 );
    if ( x + cols1 > cols2 )
	pm_error(
	    "x + width is too large by %d pixels", x + cols1 - cols2, 0,0,0,0 );
    if ( y + rows1 > rows2 )
	pm_error(
	    "y + height is too large by %d pixels", y + rows1 - rows2, 0,0,0,0);

    if ( argn != argc )
	pm_usage( usage );

    for ( row = 0; row < rows1; row++ )
	{
	pbm_readpbmrow( ifd1, bitrow1, cols1, format1 );
        for ( col = 0, b1P = bitrow1, b2P = &(bits2[row+y][x]); col < cols1; col++, b1P++, b2P++ )
	    switch ( function )
		{
		case 'r':
		*b2P = *b1P;
		break;

		case 'o':
		*b2P = ( *b1P == PBM_WHITE || *b2P == PBM_WHITE ?
			 PBM_WHITE : PBM_BLACK );
		break;

		case 'a':
		*b2P = ( *b1P == PBM_WHITE && *b2P == PBM_WHITE ?
			 PBM_WHITE : PBM_BLACK );
		break;

		case 'x':
		*b2P = ( ( *b1P == PBM_WHITE && *b2P != PBM_WHITE ) ||
			 ( *b1P != PBM_WHITE && *b2P == PBM_WHITE ) ?
			 PBM_WHITE : PBM_BLACK );
		break;

		default:
		pm_error( "can't happen", 0,0,0,0,0 );
		}
	}

    pm_close( ifd1 );

    pbm_writepbm( stdout, bits2, cols2, rows2 );

    exit( 0 );
    }
