/* libpnm1.c - pnm utility library part 1
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

#ifdef PPM
#include "ppm.h"
#include "libppm.h"
#endif /*PPM*/

#ifdef PGM
#include "pgm.h"
#include "libpgm.h"
static gray *grayrow;
#endif /*PGM*/

#ifdef PBM
#include "pbm.h"
#include "libpbm.h"
static bit *bitrow;
#endif /*PBM*/

void
pnm_readpnminit( file, colsP, rowsP, maxvalP, formatP )
FILE *file;
int *colsP, *rowsP, *formatP;
xelval *maxvalP;
    {
#ifdef PGM
    gray gmaxval;
#endif /*PGM*/

    /* Check magic number. */
    *formatP = pbm_readmagicnumber( file );
    switch ( *formatP )
	{
#ifdef PPM
	case PPM_FORMAT:
	case RPPM_FORMAT:
	ppm_readppminitrest( file, colsP, rowsP, maxvalP );
	break;
#endif /*PPM*/

#ifdef PGM
	case PGM_FORMAT:
	case RPGM_FORMAT:
	pgm_readpgminitrest( file, colsP, rowsP, &gmaxval );
	*maxvalP = (xelval) gmaxval;
	grayrow = pgm_allocrow( *colsP );
	break;
#endif /*PGM*/

#ifdef PBM
	case PBM_FORMAT:
	case RPBM_FORMAT:
	pbm_readpbminitrest( file, colsP, rowsP );
	bitrow = pbm_allocrow( *colsP );
	break;
#endif /*PBM*/

	default:
	pm_error( "bad magic number - not a ppm, pgm, or pbm file", 0,0,0,0,0 );
	}
    }

void
pnm_readpnmrow( file, xelrow, cols, maxval, format )
FILE *file;
xel *xelrow;
xelval maxval;
int cols, format;
    {
    register int col;
    register xel *xP;
#ifdef PGM
    register gray *gP;
#endif /*PGM*/
#ifdef PBM
    register bit *bP;
#endif /*PBM*/

    switch ( format )
	{
#ifdef PPM
	case PPM_FORMAT:
	case RPPM_FORMAT:
	ppm_readppmrow( file, (pixel *) xelrow, cols, (pixval) maxval, format );
	break;
#endif /*PPM*/

#ifdef PGM
	case PGM_FORMAT:
	case RPGM_FORMAT:
	pgm_readpgmrow( file, grayrow, cols, (gray) maxval, format );
	for ( col = 0, xP = xelrow, gP = grayrow; col < cols; col++, xP++, gP++ )
	    PNM_ASSIGN1( *xP, *gP );
	break;
#endif /*PGM*/

#ifdef PBM
	case PBM_FORMAT:
	case RPBM_FORMAT:
	pbm_readpbmrow( file, bitrow, cols, format );
	for ( col = 0, xP = xelrow, bP = bitrow; col < cols; col++, xP++, bP++ )
	    PNM_ASSIGN1( *xP, *bP );
	break;
#endif /*PBM*/

	default:
	pm_error( "can't happen", 0,0,0,0,0 );
	}
    }

xel **
pnm_readpnm( file, colsP, rowsP, maxvalP, formatP )
FILE *file;
int *colsP, *rowsP, *formatP;
xelval *maxvalP;
    {
    xel **xels;
    int row;

    pnm_readpnminit( file, colsP, rowsP, maxvalP, formatP );

    xels = pnm_allocarray( *colsP, *rowsP );

    for ( row = 0; row < *rowsP; row++ )
	pnm_readpnmrow( file, xels[row], *colsP, *maxvalP, *formatP );

    return xels;
    }
