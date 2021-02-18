/* xbmtopbm.c - read an X bitmap file and produce a portable bitmap
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
#include <sys/types.h>
#include "pbm.h"

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    register bit *bitrow, *bP;
    int rows, cols, row, col, charcount;
    char *data, mask;

    pm_progname = argv[0];

    if ( argc > 2 )
	pm_usage( "[bitmapfile]" );
    
    if ( argc == 2 )
	ifd = pm_openr( argv[1] );
    else
	ifd = stdin;

    ReadBitmapFile( ifd, &cols, &rows, &data );

    pm_close( ifd );

    pbm_writepbminit( stdout, cols, rows );
    bitrow = pbm_allocrow( cols );

    for ( row = 0; row < rows; row++ )
	{
	charcount = 0;
	mask = 1;
	for ( col = 0, bP = bitrow; col < cols; col++, bP++ )
	    {
	    if ( charcount >= 8 )
		{
		data++;
		charcount = 0;
		mask = 1;
		}
	    *bP = ( *data & mask ) ? PBM_BLACK : PBM_WHITE;
	    charcount++;
	    mask = mask << 1;
	    }
	data++;
	pbm_writepbmrow( stdout, bitrow, cols );
	}

    exit( 0 );
    }

#ifdef SYSV
#include <string.h>
#define rindex strrchr
#else /*SYSV*/
#include <strings.h>
#endif /*SYSV*/

#define MAX_LINE 500

ReadBitmapFile( stream, widthP, heightP, dataP )
FILE *stream;
int *widthP, *heightP;
char **dataP;
    {
    char line[MAX_LINE], name_and_type[MAX_LINE];
    char *ptr, *t;
    int version10, raster_length, v;
    register int bytes, bytes_per_line, padding;
    register int c1, c2, value1, value2;
    static int hex_table[256];

    *widthP = *heightP = -1;

    for ( ; ; )
	{
	if ( fgets( line, MAX_LINE, stream ) == NULL )
	    pm_error( "premature EOF", 0,0,0,0,0 );
	if ( strlen( line ) == MAX_LINE - 1 )
	    pm_error( "line too long", 0,0,0,0,0 );

	if ( sscanf( line, "#define %s %d", name_and_type, &v ) == 2 )
	    {
	    if ( ! (t = rindex( name_and_type, '_' )) )
		t = name_and_type;
	    else
		t++;
	    if ( ! strcmp( "width", t ) )
		*widthP = v;
	    if ( ! strcmp( "height", t ) )
		*heightP = v;
	    continue;
	    }
	
	if ( sscanf( line, "static short %s = {", name_and_type ) == 1 )
	    {
	    version10 = 1;
	    break;
	    }
	else if ( sscanf( line, "static char %s = {", name_and_type ) == 1 )
	    {
	    version10 = 0;
	    break;
	    }
	else
	    continue;
	}
 
    if ( *widthP == -1 )
	pm_error( "invalid width", 0,0,0,0,0 );
    if ( *heightP == -1 )
	pm_error( "invalid height", 0,0,0,0,0 );

    padding = 0;
    if ( ((*widthP % 16) >= 1) && ((*widthP % 16) <= 8) && version10 )
	padding = 1;

    bytes_per_line = (*widthP+7)/8 + padding;
    
    raster_length =  bytes_per_line * *heightP;
    *dataP = (char *) malloc( raster_length );
    if ( *dataP == (char *) 0 )
	pm_error( "out of memory", 0,0,0,0,0 );

    /* Initialize hex_table. */
    for ( c1 = 0; c1 < 256; c1++ )
	hex_table[c1] = 256;
    hex_table['0'] = 0;
    hex_table['1'] = 1;
    hex_table['2'] = 2;
    hex_table['3'] = 3;
    hex_table['4'] = 4;
    hex_table['5'] = 5;
    hex_table['6'] = 6;
    hex_table['7'] = 7;
    hex_table['8'] = 8;
    hex_table['9'] = 9;
    hex_table['A'] = 10;
    hex_table['B'] = 11;
    hex_table['C'] = 12;
    hex_table['D'] = 13;
    hex_table['E'] = 14;
    hex_table['F'] = 15;
    hex_table['a'] = 10;
    hex_table['b'] = 11;
    hex_table['c'] = 12;
    hex_table['d'] = 13;
    hex_table['e'] = 14;
    hex_table['f'] = 15;

    if ( version10 )
	for ( bytes = 0, ptr = *dataP; bytes < raster_length; bytes += 2 )
	    {
	    while ( ( c1 = getc( stream ) ) != 'x' )
	        if ( c1 == EOF )
		    pm_error( "premature EOF", 0,0,0,0,0 );
	    c1 = getc( stream );
	    c2 = getc( stream );
	    if ( c1 == EOF || c2 == EOF )
		pm_error( "premature EOF", 0,0,0,0,0 );
	    value1 = ( hex_table[c1] << 4 ) + hex_table[c2];
	    if ( value1 >= 256 )
		pm_error( "syntax error", 0,0,0,0,0 );
	    c1 = getc( stream );
	    c2 = getc( stream );
	    if ( c1 == EOF || c2 == EOF )
		pm_error( "premature EOF", 0,0,0,0,0 );
	    value2 = ( hex_table[c1] << 4 ) + hex_table[c2];
	    if ( value2 >= 256 )
		pm_error( "syntax error", 0,0,0,0,0 );
	    *ptr++ = value2;
	    if ( ( ! padding ) || ( ( bytes + 2 ) % bytes_per_line ) )
	        *ptr++ = value1;
	    }
    else
	for ( bytes = 0, ptr = *dataP; bytes < raster_length; bytes++ )
	    {
	    while ( ( c1 = getc( stream ) ) != 'x' )
		if ( c1 == EOF )
		    pm_error( "premature EOF", 0,0,0,0,0 );
	    c1 = getc( stream );
	    c2 = getc( stream );
	    if ( c1 == EOF || c2 == EOF )
		pm_error( "premature EOF", 0,0,0,0,0 );
	    value1 = ( hex_table[c1] << 4 ) + hex_table[c2];
	    if ( value1 >= 256 )
		pm_error( "syntax error", 0,0,0,0,0 );
	    *ptr++ = value1;
	    }
    }
