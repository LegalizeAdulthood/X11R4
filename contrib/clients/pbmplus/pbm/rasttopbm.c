/* rasttopbm.c - read a Sun raster file and produce a portable bitmap
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
#include "rast.h"

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    struct rasterfile header;
    struct pixrect *pr, *pr_load_image();
    register bit *bitrow, *bP;
    int rows, cols, row, col, mask;
    int linesize;
    unsigned char *data, *byteP;

    pm_progname = argv[0];

    if ( argc > 2 )
	pm_usage( "[rastfile]" );

    if ( argc == 2 )
	ifd = pm_openr( argv[1] );
    else
	ifd = stdin;

    /* Read in the raster file.  First the header. */
    if ( pr_load_header( ifd, &header ) != 0 )
	pm_error( "unable to read in raster file header", 0,0,0,0,0 );

    /* PBM can only handle monochrome bitmaps. */
    if ( header.ras_depth != 1 )
	pm_error(
	    "invalid depth %d - PBM can only handle depth 1",
	    header.ras_depth, 0,0,0,0 );

    cols = header.ras_width;
    rows = header.ras_height;
    if ( cols <= 0 )
	pm_error( "invalid cols: %d", cols, 0,0,0,0 );
    if ( rows <= 0 )
	pm_error( "invalid rows: %d", rows, 0,0,0,0 );

    /* If there is a color map (there shouldn't be), skip over it. */
    if ( header.ras_maptype != RMT_NONE || header.ras_maplength != 0 )
	{
	if ( pr_load_colormap( ifd, &header, NULL ) != 0 )
	    pm_error( "unable to skip colormap data", 0,0,0,0,0 );
	}

    /* Now load the data.  The pixrect returned is a memory pixrect. */
    if ( (pr = pr_load_image( ifd, &header, NULL )) == NULL )
	pm_error(
	    "unable to read in the image from the raster file", 0,0,0,0,0 );

    linesize = ( (struct mpr_data *) pr->pr_data )->md_linebytes;
    data = ( (struct mpr_data *) pr->pr_data )->md_image;

    pm_close( ifd );

    /* Now write out the PBM. */
    pbm_writepbminit( stdout, cols, rows );
    bitrow = pbm_allocrow( cols );

    for ( row = 0; row < rows; row++ )
	{
	byteP = data;
	mask = 0x80;
	for ( col = 0, bP = bitrow; col < cols; col++, bP++ )
	    {
	    if ( mask == 0 )
		{
		byteP++;
		mask = 0x80;
		}
	    *bP = ( *byteP & mask ) ? PBM_BLACK : PBM_WHITE;
	    mask = mask >> 1;
	    }
	data += linesize;
	pbm_writepbmrow( stdout, bitrow, cols );
	}

    exit( 0 );
    }
