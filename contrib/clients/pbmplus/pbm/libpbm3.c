/* libpbm3.c - pbm utility library part 3
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
#include "libpbm.h"

void
pbm_writepbminit( file, cols, rows )
FILE *file;
int cols, rows;
    {
#ifdef PBMPLUS_RAWBITS
    fprintf( file, "%c%c\n%d %d\n", PBM_MAGIC1, RPBM_MAGIC2, cols, rows );
#else /*PBMPLUS_RAWBITS*/
    fprintf( file, "%c%c\n%d %d\n", PBM_MAGIC1, PBM_MAGIC2, cols, rows );
#endif /*PBMPLUS_RAWBITS*/
    }

void
pbm_writepbmrow( file, bitrow, cols )
FILE *file;
bit *bitrow;
int cols;
    {
#ifdef PBMPLUS_RAWBITS
    register int col, bitshift;
    register unsigned char item;
    register bit *bP;

    bitshift = 7;
    item = 0;
    for ( col = 0, bP = bitrow; col < cols; col++, bP++ )
	{
	if ( *bP )
	    item += 1 << bitshift;
	bitshift--;
	if ( bitshift == -1 )
	    {
	    if ( putc( item, file ) == EOF )
		pm_perror( 0 );
	    bitshift = 7;
	    item = 0;
	    }
	}
    if ( bitshift != 7 )
	if ( putc( item, file ) == EOF )
	    pm_perror( 0 );
#else /*PBMPLUS_RAWBITS*/
    register int col, charcount;
    register bit *bP;

    charcount = 0;
    for ( col = 0, bP = bitrow; col < cols; col++, bP++ )
	{
	if ( charcount >= 70 )
	    {
	    if ( putc( '\n', file ) == EOF )
		pm_perror( 0 );
	    charcount = 0;
	    }
	if ( putc( *bP ? '1' : '0', file ) == EOF )
	    pm_perror( 0 );
	charcount++;
	}
    if ( putc( '\n', file ) == EOF )
	pm_perror( 0 );
#endif /*PBMPLUS_RAWBITS*/
    }

void
pbm_writepbm( file, bits, cols, rows )
FILE *file;
bit **bits;
int cols, rows;
    {
    int row;

    pbm_writepbminit( file, cols, rows );

    for ( row = 0; row < rows; row++ )
	pbm_writepbmrow( file, bits[row], cols );
    }
