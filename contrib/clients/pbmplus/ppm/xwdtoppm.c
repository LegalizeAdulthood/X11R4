/* xwdtoppm.c - read an X11 or X10 window dump file and write a portable pixmap
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
#include "x10wd.h"
#include "x11wd.h"

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    register pixel *pixrow, *pP;
#define MAXCOLORS 16384
#define X10MAXCOLORS 256
    static pixel colors[MAXCOLORS];
    int rows, cols, padright, row, col;
    long maxval;

    pm_progname = argv[0];

    if ( argc > 2 )
	pm_usage( "[xwdfile]" );

    if ( argc == 2 )
	ifd = pm_openr( argv[1] );
    else
	ifd = stdin;

    getinit( ifd, &cols, &rows, &padright, &maxval, colors );

    ppm_writeppminit( stdout, cols, rows, (pixval) maxval );
    pixrow = ppm_allocrow( cols );

    for ( row = 0; row < rows; row++ )
	{
        for ( col = 0, pP = pixrow; col < cols; col++, pP++ )
	    *pP = colors[getpixnum( ifd )];
        for ( col = 0; col < padright; col++ )
	    (void) getpixnum( ifd );
	ppm_writeppmrow( stdout, pixrow, cols, (pixval) maxval );
	}

    pm_close( ifd );

    exit( 0 );
    }


char buf[4];
char *byteP;
short *shortP;
long *longP;
int bits_per_item, bits_used, bit_shift, bits_per_pixel, pixel_mask;
int bit_order, byte_swap;

short bs_short();
int bs_int();
long bs_long();


getinit( file, colsP, rowsP, padrightP, maxvalP, colors )
FILE *file;
int *colsP, *rowsP, *padrightP;
long *maxvalP;
pixel colors[256];
    {
    /* Assume X11 headers are larger than X10 ones. */
    unsigned char header[sizeof(X11WDFileHeader)];
    X10WDFileHeader *h10P;
    X11WDFileHeader *h11P;
    char junk[10000];

    if ( PPM_MAXMAXVAL < 65535 )
	*maxvalP = PPM_MAXMAXVAL;
    else
	*maxvalP = 65535;

    h10P = (X10WDFileHeader *) header;
    h11P = (X11WDFileHeader *) header;

    if ( sizeof(*h10P) > sizeof(*h11P) )
	{
	pm_message(
	    "ARGH!  On this machine, X10 headers are larger than X11 headers!",
	    0,0,0,0,0 );
	pm_error(
	    "You will have to re-write xwdtopbm.", 0,0,0,0,0 );
	}

    /* Read an X10 header. */
    if ( fread( &header[0], sizeof(*h10P), 1, file ) != 1 )
	pm_error( "couldn't read XWD file header", 0,0,0,0,0 );

    if ( h10P->file_version == X10WD_FILE_VERSION ||
	 bs_int( h10P->file_version ) == X10WD_FILE_VERSION )
	{
	int i;
	X10Color x10col;

	if ( h10P->file_version != X10WD_FILE_VERSION )
	    {
	    byte_swap = 1;
	    h10P->header_size = bs_int( h10P->header_size );
	    h10P->file_version = bs_int( h10P->file_version );
	    h10P->display_type = bs_int( h10P->display_type );
	    h10P->display_planes = bs_int( h10P->display_planes );
	    h10P->pixmap_format = bs_int( h10P->pixmap_format );
	    h10P->pixmap_width = bs_int( h10P->pixmap_width );
	    h10P->pixmap_height = bs_int( h10P->pixmap_height );
	    h10P->window_width = bs_short( h10P->window_width );
	    h10P->window_height = bs_short( h10P->window_height );
	    h10P->window_x = bs_short( h10P->window_x );
	    h10P->window_y = bs_short( h10P->window_y );
	    h10P->window_bdrwidth = bs_short( h10P->window_bdrwidth );
	    h10P->window_ncolors = bs_short( h10P->window_ncolors );
	    }
	if ( fread( junk, 1, h10P->header_size - sizeof(*h10P), file ) != h10P->header_size - sizeof(*h10P) )
	    pm_error( "couldn't read rest of X10 XWD file header", 0,0,0,0,0 );

	/* Check whether we can handle this dump. */
	if ( h10P->window_ncolors > X10MAXCOLORS )
	    pm_error(
		"can't handle X10 window_ncolors > %d", X10MAXCOLORS, 0,0,0,0 );
	if ( h10P->pixmap_format != ZFormat )
	    pm_error(
		"can't handle X10 pixmap_format %d", h10P->pixmap_format,
		0,0,0,0 );

	/* Read X10 colormap. */
	for ( i = 0; i < h10P->window_ncolors; i++ )
	    {
	    if ( fread( &x10col, sizeof(X10Color), 1, file ) != 1 )
		pm_error( "couldn't read X10 XWD colormap", 0,0,0,0,0 );
	    if ( *maxvalP != 65535 )
		{
		x10col.red = (long) x10col.red * *maxvalP / 65535;
		x10col.green = (long) x10col.green * *maxvalP / 65535;
		x10col.blue = (long) x10col.blue * *maxvalP / 65535;
		}
	    PPM_ASSIGN(
		colors[x10col.pixel], x10col.red, x10col.green, x10col.blue);
	    }

	*colsP = h10P->pixmap_width;
	*rowsP = h10P->pixmap_height;
	if ( h10P->window_ncolors == 0 )
	    {
	    PPM_ASSIGN( colors[0], 0, 0, 0 );
	    PPM_ASSIGN( colors[1], *maxvalP, *maxvalP, *maxvalP );
	    *padrightP =
		( ( h10P->pixmap_width + 15 ) / 16 ) * 16 - h10P->pixmap_width;
	    bits_per_item = 16;
	    bits_used = bits_per_item;
	    bits_per_pixel = 1;
	    }
	else
	    {
	    *padrightP = h10P->pixmap_width & 1;
	    bits_per_item = 8;
	    bits_used = bits_per_item;
	    bits_per_pixel = 8;
	    }
	pixel_mask = ( 1 << bits_per_pixel ) - 1;
	bit_order = LSBFirst;
	}
    else if ( h11P->file_version == X11WD_FILE_VERSION ||
	     bs_long( h11P->file_version ) == X11WD_FILE_VERSION )
	{
	int i;
	X11XColor x11col;

	if ( fread( &header[sizeof(*h10P)], sizeof(*h11P) - sizeof(*h10P), 1, file ) != 1 )
	    pm_error( "couldn't read X11 XWD file header", 0,0,0,0,0 );
	if ( h11P->file_version != X11WD_FILE_VERSION )
	    {
	    byte_swap = 1;
	    h11P->header_size = bs_long( h11P->header_size );
	    h11P->file_version = bs_long( h11P->file_version );
	    h11P->pixmap_format = bs_long( h11P->pixmap_format );
	    h11P->pixmap_depth = bs_long( h11P->pixmap_depth );
	    h11P->pixmap_width = bs_long( h11P->pixmap_width );
	    h11P->pixmap_height = bs_long( h11P->pixmap_height );
	    h11P->xoffset = bs_long( h11P->xoffset );
	    h11P->byte_order = bs_long( h11P->byte_order );
	    h11P->bitmap_unit = bs_long( h11P->bitmap_unit );
	    h11P->bitmap_bit_order = bs_long( h11P->bitmap_bit_order );
	    h11P->bitmap_pad = bs_long( h11P->bitmap_pad );
	    h11P->bits_per_pixel = bs_long( h11P->bits_per_pixel );
	    h11P->bytes_per_line = bs_long( h11P->bytes_per_line );
	    h11P->visual_class = bs_long( h11P->visual_class );
	    h11P->red_mask = bs_long( h11P->red_mask );
	    h11P->green_mask = bs_long( h11P->green_mask );
	    h11P->blue_mask = bs_long( h11P->blue_mask );
	    h11P->bits_per_rgb = bs_long( h11P->bits_per_rgb );
	    h11P->colormap_entries = bs_long( h11P->colormap_entries );
	    h11P->ncolors = bs_long( h11P->ncolors );
	    h11P->window_width = bs_long( h11P->window_width );
	    h11P->window_height = bs_long( h11P->window_height );
	    h11P->window_x = bs_long( h11P->window_x );
	    h11P->window_y = bs_long( h11P->window_y );
	    h11P->window_bdrwidth = bs_long( h11P->window_bdrwidth );
	    }
	if ( fread( junk, 1, h11P->header_size - sizeof(*h11P), file ) != h11P->header_size - sizeof(*h11P) )
	    pm_error( "couldn't read rest of X11 XWD file header", 0,0,0,0,0 );

	/* Check whether we can handle this dump. */
	if ( h11P->pixmap_depth > 8 )
	    pm_error( "can't handle X11 pixmap_depth > 8", 0,0,0,0,0 );
	if ( h11P->bits_per_rgb > 8 )
	    pm_error( "can't handle X11 bits_per_rgb > 8", 0,0,0,0,0 );
	if ( h11P->ncolors > MAXCOLORS )
	    pm_error( "can't handle X11 ncolors > %d", MAXCOLORS, 0,0,0,0 );
	if ( h11P->pixmap_format != ZPixmap )
	    pm_error(
		"can't handle X11 pixmap_format %d", h11P->pixmap_format,
		0,0,0,0 );
#ifdef notdef
	if ( h11P->bitmap_unit != h11P->bitmap_pad )
	    pm_error(
		"X11 bitmap_unit (%d) != bitmap_pad (%d) - can't handle",
		h11P->bitmap_unit, h11P->bitmap_pad, 0,0,0 );
#endif /*notdef*/
	if ( h11P->bitmap_unit != 8 && h11P->bitmap_unit != 16 &&
	     h11P->bitmap_unit != 32 )
	    pm_error(
		"X11 bitmap_unit (%d) is non-standard - can't handle",
		h11P->bitmap_unit, 0,0,0,0 );

	/* Read X11 colormap. */
	for ( i = 0; i < h11P->colormap_entries; i++ )
	    {
	    if ( fread( &x11col, sizeof(X11XColor), 1, file ) != 1 )
		pm_error( "couldn't read X11 XWD colormap", 0,0,0,0,0 );
	    if ( *maxvalP != 65535 )
		{
		x11col.red = (long) x11col.red * *maxvalP / 65535;
		x11col.green = (long) x11col.green * *maxvalP / 65535;
		x11col.blue = (long) x11col.blue * *maxvalP / 65535;
		}
	    PPM_ASSIGN(
		colors[x11col.pixel], x11col.red, x11col.green, x11col.blue );
	    }

	*colsP = h11P->pixmap_width;
	*rowsP = h11P->pixmap_height;
	if ( h11P->colormap_entries == 0 )
	    {
	    PPM_ASSIGN( colors[0], *maxvalP, *maxvalP, *maxvalP );
	    PPM_ASSIGN( colors[1], 0, 0, 0 );
	    }
	*padrightP =
	    h11P->bytes_per_line * 8 / h11P->bits_per_pixel -
	    h11P->pixmap_width;
	bits_per_item = h11P->bitmap_unit;
	bits_used = bits_per_item;
	bits_per_pixel = h11P->bits_per_pixel;
	bit_order = h11P->bitmap_bit_order;
	pixel_mask = ( 1 << bits_per_pixel ) - 1;
	}
    else
	pm_error( "unknown XWD file version: %d", h11P->file_version, 0,0,0,0 );

    byteP = (char *) buf;
    shortP = (short *) buf;
    longP = (long *) buf;
    }

int
getpixnum( file )
FILE *file;
    {
    int p;

    if ( bits_used == bits_per_item )
	{
	if ( fread( buf, bits_per_item / 8, 1, file ) != 1 )
	    pm_error( "couldn't read bits", 0,0,0,0,0 );
	if ( byte_swap )
	    switch ( bits_per_item )
		{
		case 8:
		break;

		case 16:
		*shortP = bs_short( *shortP );
		break;

		case 32:
		*longP = bs_long( *longP );
		break;

		default:
		pm_error( "can't happen", 0,0,0,0,0 );
		}
	bits_used = 0;

	if ( bit_order == MSBFirst )
	    bit_shift = bits_per_item - bits_per_pixel;
	else
	    bit_shift = 0;
	}

    switch ( bits_per_item )
	{
	case 8:
	p = ( *byteP >> bit_shift) & pixel_mask;
	break;

	case 16:
	p = ( *shortP >> bit_shift) & pixel_mask;
	break;

	case 32:
	p = ( *longP >> bit_shift) & pixel_mask;
	break;

	default:
	pm_error( "can't happen", 0,0,0,0,0 );
	}

    if ( bit_order == MSBFirst )
	bit_shift -= bits_per_pixel;
    else
	bit_shift += bits_per_pixel;
    bits_used += bits_per_pixel;

    return p;
    }

short
bs_short( s )
short s;
    {
    short ss;
    unsigned char *bp, t;

    ss = s;
    bp = (unsigned char *) &ss;
    t = bp[0];
    bp[0] = bp[1];
    bp[1] = t;
    return ss;
    }

int
bs_int( i )
int i;
    {
    int ii;
    unsigned char *bp, t;

    ii = i;
    bp = (unsigned char *) &ii;
    t = bp[0];
    bp[0] = bp[3];
    bp[3] = t;
    t = bp[1];
    bp[1] = bp[2];
    bp[2] = t;
    return ii;
    }

long bs_long( l )
long l;
    {
    return bs_int( l );
    }
