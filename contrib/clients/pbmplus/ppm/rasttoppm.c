/* rasttoppm.c - read a Sun raster file and produce a portable pixmap
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
#include "rast.h"

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    struct rasterfile header;
    colormap_t pr_colormap;
    struct pixrect *pr, *pr_load_image( );
    register pixel *pixelrow, *pP;
    int argn, rows, cols, depth, row, col, mask;
    pixval maxval;
    pixel zero, one;
    int linesize;
    unsigned char *data, *byteP;

    pm_progname = argv[0];

    argn = 1;

    if ( argn != argc )
	{
	ifd = pm_openr( argv[argn] );
	argn++;
	}
    else
	ifd = stdin;

    if ( argn != argc )
	pm_usage( "[rastfile]" );

    /* Read in the raster file.  First the header. */
    if ( pr_load_header( ifd, &header ) != 0 )
	pm_error( "unable to read in raster file header", 0,0,0,0,0 );

    cols = header.ras_width;
    rows = header.ras_height;
    depth = header.ras_depth;

    if ( cols <= 0 )
	pm_error( "invalid cols: %d", cols, 0,0,0,0 );
    if ( rows <= 0 )
	pm_error( "invalid rows: %d", rows, 0,0,0,0);

    /* If there is a color map, read it. */
    if ( header.ras_maptype != RMT_NONE || header.ras_maplength != 0 )
	{
	if ( pr_load_colormap( ifd, &header, &pr_colormap ) != 0 )
	    pm_error( "unable to skip colormap data", 0,0,0,0,0 );
	}

    /* Check the depth and color map. */
    switch ( depth )
	{
	case 1:
	if ( header.ras_maptype == RMT_NONE && header.ras_maplength == 0 )
	    {
	    maxval = 255;
	    PPM_ASSIGN( zero, maxval, maxval, maxval );
	    PPM_ASSIGN( one, 0, 0, 0 );
	    }
	else if ( header.ras_maptype == RMT_EQUAL_RGB &&
		  header.ras_maplength == 6 )
	    {
	    maxval = 255;
	    PPM_ASSIGN(
		zero, pr_colormap.map[0][0], pr_colormap.map[1][0],
		pr_colormap.map[2][0] );
	    PPM_ASSIGN(
		one, pr_colormap.map[0][1], pr_colormap.map[1][1],
		pr_colormap.map[2][1] );
	    }
	else
	    pm_error(
		"this depth-1 rasterfile has a non-standard colormap - type %d length %d",
		header.ras_maptype, header.ras_maplength, 0,0,0 );
	break;

	case 8:
	if ( header.ras_maptype != RMT_EQUAL_RGB )
	    pm_error(
		"this depth-8 rasterfile has a non-standard colormap - type %d length %d",
		header.ras_maptype, header.ras_maplength, 0,0,0 );
	maxval = 255;
	break;

	case 24:
	if ( header.ras_maptype != RMT_NONE || header.ras_maplength != 0 )
	    pm_error(
		"this depth-24 rasterfile has a colormap - can't handle it",
		0,0,0,0,0 );
	maxval = 255;
	break;

	default:
	pm_error(
	    "invalid depth: %d.  PPM can only handle depth 1, 8, or 24.",
	    depth, 0,0,0,0 );
	}

    /* Now load the data.  The pixrect returned is a memory pixrect. */
    if ( ( pr = pr_load_image( ifd, &header, NULL ) ) == NULL )
	pm_error(
	    "unable to read in the image from the raster file", 0,0,0,0,0 );

    linesize = ( (struct mpr_data *) pr->pr_data )->md_linebytes;
    data = ( (struct mpr_data *) pr->pr_data )->md_image;

    pm_close( ifd );

    /* Now write out the PPM. */
    ppm_writeppminit( stdout, cols, rows, maxval );
    pixelrow = ppm_allocrow( cols );

    for ( row = 0; row < rows; row++ )
	{
	byteP = data;
	switch ( depth )
	    {
	    case 1:
	    mask = 0x80;
	    for ( col = 0, pP = pixelrow; col < cols; col++, pP++ )
		{
		if ( mask == 0 )
		    {
		    byteP++;
		    mask = 0x80;
		    }
		*pP = ( *byteP & mask ) ? one : zero;
		mask = mask >> 1;
		}
	    break;

	    case 8:
	    for ( col = 0, pP = pixelrow; col < cols; col++, pP++ )
		{
		PPM_ASSIGN(
		    *pP, pr_colormap.map[0][*byteP], pr_colormap.map[1][*byteP],
		    pr_colormap.map[2][*byteP] );
		byteP++;
		}
	    break;

	    case 24:
	    for ( col = 0, pP = pixelrow; col < cols; col++, pP++ )
		{
		register pixval r, g, b;

		r = *byteP++;
		g = *byteP++;
		b = *byteP++;
		PPM_ASSIGN( *pP, r, g, b );
		}
	    break;

	    default:
	    pm_error( "can't happen", 0,0,0,0,0 );
	    }
	data += linesize;
	ppm_writeppmrow( stdout, pixelrow, cols, maxval );
	}

    exit( 0 );
    }
