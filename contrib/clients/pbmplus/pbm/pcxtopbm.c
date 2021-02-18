/* pcxtopbm.c - convert PC paintbrush (.pcx) files to portable bitmaps
**
** Copyright (C) 1988 by Jef Poskanzer.
**
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted, provided
** that the above copyright notice appear in all copies and that both that
** copyright notice and this permission notice appear in supporting
** documentation.  This software is provided "as is" without express or
** implied warranty.
**
** This program is based on the pcx2rf program by:
**   Mike Macgirvin
**   Stanford Relativity Gyro Program GP-B
**   Stanford, Calif. 94503
**   ARPA: mike@relgyro.stanford.edu
*/

#include <stdio.h>
#include "pbm.h"

void read_pcx(), spread_byte();

main( argc, argv)
int argc;
char *argv[];
    {
    FILE *ifd;
    unsigned char pcxhd[128];
    int cnt, b;
    int xmin;
    int xmax;
    int ymin;
    int ymax;
    int bytes_per_row;
    register bit **bits;

    pm_progname = argv[0];

    if ( argc > 2 )
	pm_usage( "[pcxfile]" );

    /* Open input file. */
    if ( argc == 2 )
	ifd = pm_openr( argv[1] );
    else
	ifd = stdin;

    /* Read .pcx header. */
    for ( cnt = 0; cnt < 128; cnt++ )
	{
	b = getc( ifd );
	if ( b == EOF )
	    pm_error( "incomplete .pcx header", 0,0,0,0,0 );
	pcxhd[cnt] = b;
	}

    /* Calculate raster header and swap bytes. */
    xmin = pcxhd[4] + ( 256 * pcxhd[5] );
    ymin = pcxhd[6] + ( 256 * pcxhd[7] );
    xmax = pcxhd[8] + ( 256 * pcxhd[9] );
    ymax = pcxhd[10] + ( 256 * pcxhd[11] );
    xmax = xmax - xmin + 1;
    ymax = ymax - ymin + 1;
    bytes_per_row = pcxhd[66] + ( 256 * pcxhd[67] );

    /* Allocate pbm array. */
    bits = pbm_allocarray( xmax + 8, ymax );
    /* + 8 allows for slop turning bytes to bits; it's easier than checking. */

    /* Read compressed bitmap. */
    read_pcx( ifd, bytes_per_row, bits, ymax );
    pm_close( ifd );

    /* Write pbm. */
    pbm_writepbm( stdout, bits, xmax, ymax );

    /* All done. */
    exit( 0 );
    }


void
read_pcx( ifd, bytes_per_row, bits, rows )
FILE *ifd;
int bytes_per_row;
bit **bits;
int rows;
    {
    /* Goes like this:  Read a byte.  If the two high bits are set, then the
    ** low 6 bits contain a repeat count, and the byte to repeat is the next
    ** byte in the file.  If the two high bits are not set, then this is the
    ** byte to write.  
    */
    int row = 0;
    int col = 0;
    int bytes_this_row = 0;
    int b, i, cnt;

    while ( (b = fgetc( ifd )) != EOF )
	{
	if ( b & 0xC0 == 0xC0 )
	    {
	    cnt = b & 0x3F;
	    b = fgetc( ifd );
	    if ( b == EOF )
		pm_error( "unexpected end of file on input", 0,0,0,0,0 );
	    for ( i = 0; i < cnt; i++ )
		{
		if ( row >= rows )
		    {
		    fprintf( stderr, "junk in file after bitmap - ignoring\n" );
		    return;
		    }
		spread_byte( 255 - b, &(bits[row][col]) );
		col += 8;
		bytes_this_row++;
		if ( bytes_this_row == bytes_per_row )
		    {
		    row++;
		    col = 0;
		    bytes_this_row = 0;
		    }
		}
	    }
	else
	    {
	    if ( row >= rows )
		{
		fprintf( stderr, "junk in file after bitmap - ignoring\n" );
		return;
		}
	    spread_byte( 255 - b, &(bits[row][col]) );
	    col += 8;
	    bytes_this_row++;
	    if ( bytes_this_row == bytes_per_row )
		{
		row++;
		col = 0;
		bytes_this_row = 0;
		}
	    }
	}
    }


void
spread_byte( b, bits )
int b;
bit *bits;
    {
    int i, j;

    for ( i = 0, j = 7; i < 8; i++, j-- )
	bits[j] = ( b & ( 1 << i ) ) ? PBM_BLACK : PBM_WHITE;
    }
