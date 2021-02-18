/* libpgm2.c - pgm utility library part 2
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
#include "libpgm.h"

void
pgm_writepgminit( file, cols, rows, maxval )
FILE *file;
int cols, rows;
gray maxval;
    {
#ifdef PBMPLUS_RAWBITS
    if ( maxval <= 255 )
	fprintf(
	    file, "%c%c\n%d %d\n%d\n", PGM_MAGIC1, RPGM_MAGIC2,
	    cols, rows, maxval );
    else
	fprintf(
	    file, "%c%c\n%d %d\n%d\n", PGM_MAGIC1, PGM_MAGIC2,
	    cols, rows, maxval );
#else /*PBMPLUS_RAWBITS*/
    fprintf(
	file, "%c%c\n%d %d\n%d\n", PGM_MAGIC1, PGM_MAGIC2,
	cols, rows, maxval );
#endif /*PBMPLUS_RAWBITS*/
    }

static int
putus( n, file )
unsigned short n;
FILE *file;
    {
    if ( n >= 10 )
	if ( putus( n / 10, file ) == EOF )
	    return EOF;
    return putc( n % 10 + '0', file );
    }

#ifdef PBMPLUS_RAWBITS
static void
pgm_writepgmrowraw( file, grayrow, cols, maxval )
FILE *file;
gray *grayrow;
int cols;
gray maxval;
    {
    register int col;
    register gray *gP;

    for ( col = 0, gP = grayrow; col < cols; col++, gP++ )
	{
#ifdef DEBUG
	if ( *gP > maxval )
	    pm_error( "value out of bounds (%u > %u)", *gP, maxval, 0,0,0 );
#endif /*DEBUG*/
	if ( putc( *gP, file ) == EOF )
	    pm_perror( 0 );
	}
    }
#endif /*PBMPLUS_RAWBITS*/

static void
pgm_writepgmrowplain( file, grayrow, cols, maxval )
FILE *file;
gray *grayrow;
int cols;
gray maxval;
    {
    register int col, charcount;
    register gray *gP;

    charcount = 0;
    for ( col = 0, gP = grayrow; col < cols; col++, gP++ )
	{
	if ( charcount >= 70 )
	    {
	    if ( putc( '\n', file ) == EOF )
		pm_perror( 0 );
	    charcount = 0;
	    }
#ifdef DEBUG
	if ( *gP > maxval )
	    pm_error( "value out of bounds (%u > %u)", *gP, maxval, 0,0,0 );
#endif /*DEBUG*/
	if ( putus( (unsigned long) *gP, file ) == EOF )
	    pm_perror( 0 );
	if ( putc( ' ', file ) == EOF )
	    pm_perror( 0 );
	charcount += 4;
	}
    if ( putc( '\n', file ) == EOF )
	pm_perror( 0 );
    }

void
pgm_writepgmrow( file, grayrow, cols, maxval )
FILE *file;
gray *grayrow;
int cols;
gray maxval;
    {
#ifdef PBMPLUS_RAWBITS
    if ( maxval <= 255 )
	pgm_writepgmrowraw( file, grayrow, cols, maxval );
    else
	pgm_writepgmrowplain( file, grayrow, cols, maxval );
#else /*PBMPLUS_RAWBITS*/
    pgm_writepgmrowplain( file, grayrow, cols, maxval );
#endif /*PBMPLUS_RAWBITS*/
    }

void
pgm_writepgm( file, grays, cols, rows, maxval )
FILE *file;
gray **grays;
int cols, rows;
gray maxval;
    {
    int row;

    pgm_writepgminit( file, cols, rows, maxval );

    for ( row = 0; row < rows; row++ )
	pgm_writepgmrow( file, grays[row], cols, maxval );
    }
