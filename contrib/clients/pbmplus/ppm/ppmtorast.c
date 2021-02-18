/* ppmtorast.c - read a portable pixmap and produce a Sun rasterfile
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
#include "ppmcmap.h"
#include "rast.h"

#define MAXCOLORS 256

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    pixel **pixels, *pixelrow, p;
    register pixel *pP;
    colorhist_vector chv;
    colorhash_table cht;
    int argn, pr_type, xflag, rows, cols, i;
    int depth, colors, linesize, row;
    register int col;
    pixval maxval;
    struct pixrect *pr, *mem_create();
    colormap_t *pr_colormapP;
    unsigned char *data;
    register unsigned char *byteP;
    colormap_t *make_pr_colormap();
    char *usage = "[-s] [-x] [ppmfile]";

    pm_progname = argv[0];

    argn = 1;
    pr_type = RT_BYTE_ENCODED;
    xflag = 0;

    while ( argn < argc && argv[argn][0] == '-' )
	{
	if ( ( argv[argn][1] == 's' || argv[argn][1] == 'S' ) &&
	     argv[argn][2] == '\0' )
	    {
	    pr_type = RT_STANDARD;
	    argn++;
	    }
	else if ( ( argv[argn][1] == 'x' || argv[argn][1] == 'X' ) &&
	     argv[argn][2] == '\0' )
	    {
	    xflag = 1;
	    argn++;
	    }
	else
	    pm_usage( usage );
	}

    if ( argn != argc )
	{
	ifd = pm_openr( argv[argn] );
	argn++;
	}
    else
	ifd = stdin;

    if ( argn != argc )
	pm_usage( usage );

    pixels = ppm_readppm( ifd, &cols, &rows, &maxval );

    pm_close( ifd );

    /* Figure out the proper depth and colormap. */
    fprintf( stderr, "(Computing colormap..." );
    fflush( stderr );
    chv = ppm_computecolorhist( pixels, cols, rows, MAXCOLORS, &colors );
    if ( chv == (colorhist_vector) 0 )
	{
	fprintf(
	    stderr, "  Too many colors - proceeding to write a 24-bit\n" );
	fprintf(
	    stderr, "non-mapped rasterfile.  If you want 8 bits, try running the\n" );
	fprintf( stderr, "pixmap through 'ppmquant 256'.)\n" );
	depth = 24;
	pr_colormapP = (colormap_t *) 0;
	}
    else
	{
	fprintf( stderr, "  Done.  %d colors found.)\n", colors );

	if ( maxval != 255 )
	    for ( i = 0; i < colors; i++ )
		PPM_CSCALE( chv[i].color, chv[i].color, maxval, 255 );

	if ( xflag )
	    {
	    /* Force black to slot 0 and white to slot 1, if possible. */
	    PPM_ASSIGN( p, 0, 0, 0 );
	    ppm_addtocolorhist( chv, &colors, MAXCOLORS, p, 0, 0 );
	    PPM_ASSIGN( p, 255, 255, 255 );
	    ppm_addtocolorhist( chv, &colors, MAXCOLORS, p, 0, 1 );
	    }
	else
	    {
	    /* Force white to slot 0 and black to slot 1, if possible. */
	    PPM_ASSIGN( p, 255, 255, 255 );
	    ppm_addtocolorhist( chv, &colors, MAXCOLORS, p, 0, 0 );
	    PPM_ASSIGN( p, 0, 0, 0 );
	    ppm_addtocolorhist( chv, &colors, MAXCOLORS, p, 0, 1 );
	    }

	/* Now turn the ppm colormap into the appropriate Sun colormap. */
	depth = 8;
	pr_colormapP = make_pr_colormap( chv, colors );
	cht = ppm_colorhisttocolorhash( chv, colors );
	ppm_freecolorhist( chv );
	}
    if ( maxval > 255 )
	fprintf(
	    stderr, "(Maxval is not 255 -- automatically rescaling colors.)\n");
    
    /* Allocate space for the Sun-format image. */
    if ( (pr = mem_create(cols, rows, depth)) == (struct pixrect *) 0 )
	pm_error( "unable to create new pixrect", 0,0,0,0,0 );
    data = ( (struct mpr_data *) pr->pr_data )->md_image;
    linesize = ( (struct mpr_data *) pr->pr_data )->md_linebytes;

    /* And compute the Sun image.  The variables at this point are:
    **   chv is null or not
    **   depth is 8, or 24
    */
    for ( row = 0; row < rows; row++ )
	{
	pixelrow = pixels[row];
	byteP = data;
	switch ( depth )
	    {
	    case 8:
	    /* If depth is 8, we have a valid cht (yes, t). */
	    for ( col = 0, pP = pixelrow; col < cols; col++, pP++ )
		{
		register int color;

		if ( maxval != 255 )
		    PPM_CSCALE( *pP, *pP, maxval, 255 );
		color = ppm_lookupcolor( cht, *pP );
		if ( color == -1 )
		    pm_error(
			"color not found?!?  row=%d col=%d  r=%d g=%d b=%d",
			row, col, PPM_GETR(*pP), PPM_GETG(*pP), PPM_GETB(*pP) );
		*byteP++ = color;
		}
	    break;

	    case 24:
	    /* If depth is 24, we do NOT have a valid cht. */
	    for ( col = 0, pP = pixelrow; col < cols; col++, pP++ )
		{
		if ( maxval != 255 )
		    PPM_CSCALE( *pP, *pP, maxval, 255 );
		*byteP++ = PPM_GETR( *pP );
		*byteP++ = PPM_GETG( *pP );
		*byteP++ = PPM_GETB( *pP );
		}
	    break;

	    default:
	    pm_error( "can't happen", 0,0,0,0,0 );
	    }
	data += linesize;
	}

    /* Finally, write the sucker out. */
    pr_dump( pr, stdout, pr_colormapP, pr_type, 0 );

    exit( 0 );
    }

colormap_t *
make_pr_colormap( chv, colors )
colorhist_vector chv;
int colors;
    {
    colormap_t *pr_colormapP;
    int i, morecolors;

    pr_colormapP = (colormap_t *) malloc( sizeof(colormap_t) );
    if ( pr_colormapP == 0 )
	pm_error( "out of memory", 0,0,0,0,0 );
    pr_colormapP->type = RMT_EQUAL_RGB;
    morecolors = 256;
    pr_colormapP->length = morecolors;
    pr_colormapP->map[0] =
	(unsigned char *) malloc( morecolors * sizeof(unsigned char) );
    pr_colormapP->map[1] =
	(unsigned char *) malloc( morecolors * sizeof(unsigned char) );
    pr_colormapP->map[2] =
	(unsigned char *) malloc( morecolors * sizeof(unsigned char) );
    if ( pr_colormapP->map[0] == 0 || pr_colormapP->map[1] == 0 ||
	 pr_colormapP->map[2] == 0 )
	pm_error( "out of memory", 0,0,0,0,0 );

    for ( i = 0; i < colors; i++ )
	{
	pr_colormapP->map[0][i] = PPM_GETR( chv[i].color );
	pr_colormapP->map[1][i] = PPM_GETG( chv[i].color );
	pr_colormapP->map[2][i] = PPM_GETB( chv[i].color );
	}
    for ( ; i < morecolors; i++ )
	pr_colormapP->map[0][i] = pr_colormapP->map[1][i] =
	    pr_colormapP->map[2][i] = 0;

    return pr_colormapP;
    }

#ifdef notdef
dump_map( chv, colors )
colorhist_vector chv;
int colors;
    {
    int i;

    for ( i = 0; i < colors; i++ )
	fprintf(
	    stderr, "%d: %d %d %d  (%d)\n", i, PPM_GETR(chv[i].color),
	    PPM_GETG(chv[i].color), PPM_GETB(chv[i].color), chv[i].count );
    }
#endif /*notdef*/
