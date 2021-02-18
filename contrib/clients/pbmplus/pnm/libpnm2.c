/* libpnm2.c - pnm utility library part 2
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
pnm_writepnminit( file, cols, rows, maxval, format )
FILE *file;
int cols, rows, format;
xelval maxval;
    {
    switch ( format )
	{
#ifdef PPM
	case PPM_FORMAT:
	case RPPM_FORMAT:
	ppm_writeppminit( file, cols, rows, (pixval) maxval );
	break;
#endif /*PPM*/

#ifdef PGM
	case PGM_FORMAT:
	case RPGM_FORMAT:
	pgm_writepgminit( file, cols, rows, (gray) maxval );
	grayrow = pgm_allocrow( cols );
	break;
#endif /*PGM*/

#ifdef PBM
	case PBM_FORMAT:
	case RPBM_FORMAT:
	pbm_writepbminit( file, cols, rows );
	bitrow = pbm_allocrow( cols );
	break;
#endif /*PBM*/

	default:
	pm_error( "can't happen", 0,0,0,0,0 );
	}
    }

void
pnm_writepnmrow( file, xelrow, cols, maxval, format )
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
	ppm_writeppmrow( file, (pixel *) xelrow, cols, (pixval) maxval );
	break;
#endif /*PPM*/

#ifdef PGM
	case PGM_FORMAT:
	case RPGM_FORMAT:
	for ( col = 0, gP = grayrow, xP = xelrow; col < cols; col++, gP++, xP++ )
	    *gP = PNM_GET1( *xP );
	pgm_writepgmrow( file, grayrow, cols, (gray) maxval );
	break;
#endif /*PGM*/

#ifdef PBM
	case PBM_FORMAT:
	case RPBM_FORMAT:
	for ( col = 0, bP = bitrow, xP = xelrow; col < cols; col++, bP++, xP++ )
	    *bP = PNM_GET1( *xP );
	pbm_writepbmrow( file, bitrow, cols );
	break;
#endif /*PBM*/

	default:
	pm_error( "can't happen", 0,0,0,0,0 );
	}
    }

void
pnm_writepnm( file, xels, cols, rows, maxval, format )
FILE *file;
xel **xels;
xelval maxval;
int cols, rows, format;
    {
    int row;

    pnm_writepnminit( file, cols, rows, maxval, format );

    for ( row = 0; row < rows; row++ )
	pnm_writepnmrow( file, xels[row], cols, maxval, format );
    }
