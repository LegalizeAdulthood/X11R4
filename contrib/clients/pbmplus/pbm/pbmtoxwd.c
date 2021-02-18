/* pbmtoxwd.c - read a portable bitmap and produce an X11 window dump
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
#include "x11wd.h"
#ifdef SYSV
#include <string.h>
#define index strchr
#else /*SYSV*/
#include <strings.h>
#endif /*SYSV*/

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    register bit *bitrow, *bP;
    int rows, cols, format, padright, row, col;
    char name[100], *cp;

    pm_progname = argv[0];

    if ( argc > 2 )
	pm_usage( "[pbmfile]" );

    if ( argc == 2 )
	{
	ifd = pm_openr( argv[1] );
	strcpy( name, argv[1] );
	if ( strcmp( name, "-" ) == 0 )
	    strcpy( name, "noname" );

	if ( ( cp = index( name, '.' ) ) != 0 )
	    *cp = '\0';
	}
    else
	{
	ifd = stdin;
	strcpy( name, "noname" );
	}

    pbm_readpbminit( ifd, &cols, &rows, &format );
    bitrow = pbm_allocrow( cols );
    
    /* Compute padding to round cols up to the nearest multiple of 32. */
    padright = ( ( cols + 31 ) / 32 ) * 32 - cols;

    putinit( cols, rows, name );
    for ( row = 0; row < rows; row++ )
	{
	pbm_readpbmrow( ifd, bitrow, cols, format );
        for ( col = 0, bP = bitrow; col < cols; col++, bP++ )
	    putbit( *bP );
	for ( col = 0; col < padright; col++ )
	    putbit( 0 );
        }

    if ( ifd != stdin )
	fclose( ifd );

    putrest( );

    exit( 0 );
    }


long item;
int bitsperitem, bitshift;


putinit( cols, rows, name )
int cols, rows;
char *name;
    {
    X11WDFileHeader h11;
    X11XColor color;

    h11.header_size = sizeof(h11) + (strlen(name) + 1) * sizeof(char);
    h11.file_version = X11WD_FILE_VERSION;
    h11.pixmap_format = ZPixmap;
    h11.pixmap_depth = 1;
    h11.pixmap_width = cols;
    h11.pixmap_height = rows;
    h11.xoffset = 0;
    h11.byte_order = MSBFirst;
    h11.bitmap_unit = 32;
    h11.bitmap_bit_order = MSBFirst;
    h11.bitmap_pad = 32;
    h11.bits_per_pixel = 1;
    h11.bytes_per_line = ( ( cols + 31 ) / 32 ) * 4;
    h11.visual_class = StaticGray;
    h11.red_mask = 0;
    h11.green_mask = 0;
    h11.blue_mask = 0;
    h11.bits_per_rgb = 1;
    h11.colormap_entries = 2;
    h11.ncolors = 2;
    h11.window_width = cols;
    h11.window_height = rows;
    h11.window_x = 0;			/* arbitrary */
    h11.window_y = 0;			/* arbitrary */
    h11.window_bdrwidth = 0;
    fwrite( &h11, sizeof(h11), 1, stdout );

    fwrite( name, sizeof(char), strlen(name) + 1, stdout );

    color.pixel = 0;
    color.red = 65535;
    color.green = 65535;
    color.blue = 65535;
    color.flags = 7;
    color.pad = 0;
    fwrite( &color, sizeof(color), 1, stdout );
    color.pixel = 1;
    color.red = 0;
    color.green = 0;
    color.blue = 0;
    fwrite( &color, sizeof(color), 1, stdout );

    item = 0;
    bitsperitem = 0;
    bitshift = 31;
    }

putbit( b )
bit b;
    {
    if ( bitsperitem == 32 )
	putitem( );
    bitsperitem++;
    if ( b == PBM_BLACK )
	item += 1 << bitshift;
    bitshift--;
    }

putrest( )
    {
    if ( bitsperitem > 0 )
	putitem( );
    }

putitem( )
    {
    fwrite( &item, sizeof(item), 1, stdout );
    item = 0;
    bitsperitem = 0;
    bitshift = 31;
    }
