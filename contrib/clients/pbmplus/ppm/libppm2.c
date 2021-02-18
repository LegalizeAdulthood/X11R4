/* libppm2.c - ppm utility library part 2
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
#include "libppm.h"

void
ppm_writeppminit( file, cols, rows, maxval )
FILE *file;
int cols, rows;
pixval maxval;
    {
#ifdef PBMPLUS_RAWBITS
    if ( maxval <= 255 )
	fprintf(
	    file, "%c%c\n%d %d\n%d\n", PPM_MAGIC1, RPPM_MAGIC2,
	    cols, rows, maxval );
    else
	fprintf(
	    file, "%c%c\n%d %d\n%d\n", PPM_MAGIC1, PPM_MAGIC2,
	    cols, rows, maxval );
#else /*PBMPLUS_RAWBITS*/
    fprintf(
	file, "%c%c\n%d %d\n%d\n", PPM_MAGIC1, PPM_MAGIC2,
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
ppm_writeppmrowraw( file, pixelrow, cols, maxval )
FILE *file;
pixel *pixelrow;
int cols;
pixval maxval;
    {
    register int col;
    register pixel *pP;
    register pixval val;

    for ( col = 0, pP = pixelrow; col < cols; col++, pP++ )
	{
	val = PPM_GETR( *pP );
#ifdef DEBUG
	if ( val > maxval )
	    pm_error( "r value out of bounds (%u > %u)", val, maxval, 0,0,0 );
#endif /*DEBUG*/
	if ( putc( val, file ) == EOF )
	    pm_perror( 0 );
	val = PPM_GETG( *pP );
#ifdef DEBUG
	if ( val > maxval )
	    pm_error( "g value out of bounds (%u > %u)", val, maxval, 0,0,0 );
#endif /*DEBUG*/
	if ( putc( val, file ) == EOF )
	    pm_perror( 0 );
	val = PPM_GETB( *pP );
#ifdef DEBUG
	if ( val > maxval )
	    pm_error( "b value out of bounds (%u > %u)", val, maxval, 0,0,0 );
#endif /*DEBUG*/
	if ( putc( val, file ) == EOF )
	    pm_perror( 0 );
        }
    }
#endif /*PBMPLUS_RAWBITS*/

static void
ppm_writeppmrowplain( file, pixelrow, cols, maxval )
FILE *file;
pixel *pixelrow;
int cols;
pixval maxval;
    {
    register int col, charcount;
    register pixel *pP;
    register pixval val;

    charcount = 0;
    for ( col = 0, pP = pixelrow; col < cols; col++, pP++ )
	{
	if ( charcount >= 70 )
	    {
	    if ( putc( '\n', file ) == EOF )
		pm_perror( 0 );
	    charcount = 0;
	    }
	val = PPM_GETR( *pP );
#ifdef DEBUG
	if ( val > maxval )
	    pm_error( "r value out of bounds (%u > %u)", val, maxval, 0,0,0 );
#endif /*DEBUG*/
	if ( putus( val, file ) == EOF )
	    pm_perror( 0 );
	if ( putc( ' ', file ) == EOF )
	    pm_perror( 0 );
	val = PPM_GETG( *pP );
#ifdef DEBUG
	if ( val > maxval )
	    pm_error( "g value out of bounds (%u > %u)", val, maxval, 0,0,0 );
#endif /*DEBUG*/
	if ( putus( val, file ) == EOF )
	    pm_perror( 0 );
	if ( putc( ' ', file ) == EOF )
	    pm_perror( 0 );
	val = PPM_GETB( *pP );
#ifdef DEBUG
	if ( val > maxval )
	    pm_error( "b value out of bounds (%u > %u)", val, maxval, 0,0,0 );
#endif /*DEBUG*/
	if ( putus( val, file ) == EOF )
	    pm_perror( 0 );
	if ( putc( ' ', file ) == EOF )
	    pm_perror( 0 );
	if ( putc( ' ', file ) == EOF )
	    pm_perror( 0 );
	charcount += 13;
	}
    if ( putc( '\n', file ) == EOF )
	pm_perror( 0 );
    }

void
ppm_writeppmrow( file, pixelrow, cols, maxval )
FILE *file;
pixel *pixelrow;
int cols;
pixval maxval;
    {
#ifdef PBMPLUS_RAWBITS
    if ( maxval <= 255 )
	ppm_writeppmrowraw( file, pixelrow, cols, maxval );
    else
	ppm_writeppmrowplain( file, pixelrow, cols, maxval );
#else /*PBMPLUS_RAWBITS*/
    ppm_writeppmrowplain( file, pixelrow, cols, maxval );
#endif /*PBMPLUS_RAWBITS*/
    }

void
ppm_writeppm( file, pixels, cols, rows, maxval )
FILE *file;
pixel **pixels;
int cols, rows;
pixval maxval;
    {
    int row;

    ppm_writeppminit( file, cols, rows, maxval );

    for ( row = 0; row < rows; row++ )
	ppm_writeppmrow( file, pixels[row], cols, maxval );
    }
