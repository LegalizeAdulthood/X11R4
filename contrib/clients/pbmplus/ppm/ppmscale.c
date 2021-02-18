/* ppmscale.c - read a portable pixmap and scale it
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
#ifdef SYSV
#include <string.h>
#else /*SYSV*/
#include <strings.h>
#endif /*SYSV*/

#define SCALE 4096
#define HALFSCALE 2048

#define max(a,b) ((a) > (b) ? (a) : (b))

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    pixel *pixelrow, *temppixelrow, *newpixelrow;
    register pixel *pP, *npP;
    int argn, specxscale, specyscale, specxsize, specysize;
    int rows, cols, rowsread, format, newrows, newcols;
    register int row, col, needtoreadrow;
    pixval maxval;
    float xscale, yscale;
    long sxscale, syscale;
    register long fracrowtofill, fracrowleft;
    long *rs, *gs, *bs;
    char *usage = "<s> [ppmfile]\n            -xsize|width|-ysize|-height <s> [ppmfile]\n            -xscale|-yscale <s> [ppmfile]\n            -xscale|-xsize|-width <s> -yscale|-ysize|-height <s> [ppmfile]";

    pm_progname = argv[0];

    argn = 1;
    specxscale = specyscale = specxsize = specysize = 0;

    while ( argn + 1 < argc && argv[argn][0] == '-' )
	{
	if ( strncmp(argv[argn],"-xscale",max(strlen(argv[argn]),4)) == 0 )
	    {
	    if ( specxsize )
		pm_error(
		    "only one of -xsize/-width and -xscale may be specified",
		    0,0,0,0,0 );
	    if ( sscanf( argv[argn+1], "%g", &xscale ) != 1 )
		pm_usage( usage );
	    if ( xscale <= 0.0 )
		pm_error( "x scale must be greater than 0", 0,0,0,0,0 );
	    specxscale = 1;
	    }
	else if ( strncmp(argv[argn],"-yscale",max(strlen(argv[argn]),4)) == 0 )
	    {
	    if ( specysize )
		pm_error(
		    "only one of -ysize/-height and -yscale may be specified",
		    0,0,0,0,0 );
	    if ( sscanf( argv[argn+1], "%g", &yscale ) != 1 )
		pm_usage( usage );
	    if ( yscale <= 0.0 )
		pm_error( "y scale must be greater than 0", 0,0,0,0,0 );
	    specyscale = 1;
	    }
	else if ( strncmp(argv[argn],"-xsize",max(strlen(argv[argn]),4)) == 0 ||
	          strncmp(argv[argn],"-width",max(strlen(argv[argn]),2)) == 0 )
	    {
	    if ( specxscale )
		pm_error(
		    "only one of -xscale and -xsize/-width may be specified",
		    0,0,0,0,0 );
	    if ( sscanf( argv[argn+1], "%d", &newcols ) != 1 )
		pm_usage( usage );
	    if ( newcols <= 0 )
		pm_error( "new width must be greater than 0", 0,0,0,0,0 );
	    specxsize = 1;
	    }
	else if ( strncmp(argv[argn],"-ysize",max(strlen(argv[argn]),4)) == 0 ||
	          strncmp(argv[argn],"-height",max(strlen(argv[argn]),2)) == 0 )
	    {
	    if ( specyscale )
		pm_error(
		    "only one of -yscale and -ysize/-height may be specified",
		    0,0,0,0,0 );
	    if ( sscanf( argv[argn+1], "%d", &newrows ) != 1 )
		pm_usage( usage );
	    if ( newrows <= 0 )
		pm_error( "new height must be greater than 0", 0,0,0,0,0 );
	    specysize = 1;
	    }
	else
	    pm_usage( usage );
	argn += 2;
	}

    if ( ! ( specxscale || specyscale || specxsize || specysize ) )
	{
	/* No flags specified, so a single scale factor is required. */
	if ( argn == argc )
	    pm_usage( usage );
	if ( sscanf( argv[argn], "%g", &xscale ) != 1 )
	    pm_usage( usage );
	if ( xscale <= 0.0 )
	    pm_error( "scale must be greater than 0", 0,0,0,0,0 );
	argn++;
	yscale = xscale;
	specxscale = specyscale = 1;
	}

    /* Now get input file. */
    if ( argn != argc )
	{
	ifd = pm_openr( argv[argn] );
	argn++;
	}
    else
	ifd = stdin;

    if ( argn != argc )
	pm_usage( usage );

    ppm_pbmmaxval = 255;	/* use larger value for better results */
    ppm_readppminit( ifd, &cols, &rows, &maxval, &format );

    /* Compute all sizes and scales. */
    if ( specxsize )
	xscale = (float) newcols / (float) cols;
    else if ( specxscale )
	newcols = cols * xscale + 0.999;

    if ( specysize )
	yscale = (float) newrows / (float) rows;
    else if ( specyscale )
	newrows = rows * yscale + 0.999;
    else
	if ( specxsize )
	    {
	    yscale = xscale;
	    newrows = rows * yscale + 0.999;
	    }
	else
	    {
	    yscale = 1.0;
	    newrows = rows;
	    }
    
    if ( ! ( specxsize || specxscale ) )
	if ( specysize )
	    {
	    xscale = yscale;
	    newcols = cols * xscale + 0.999;
	    }
	else
	    {
	    xscale = 1.0;
	    newcols = cols;
	    }

    sxscale = xscale * SCALE;
    syscale = yscale * SCALE;

    pixelrow = ppm_allocrow( cols );
    if ( newrows == rows )	/* shortcut Y scaling if possible */
	temppixelrow = pixelrow;
    else
	temppixelrow = ppm_allocrow( cols );
    rs = (long *) pm_allocrow( cols, sizeof(long) );
    gs = (long *) pm_allocrow( cols, sizeof(long) );
    bs = (long *) pm_allocrow( cols, sizeof(long) );
    rowsread = 0;
    fracrowleft = syscale;
    needtoreadrow = 1;
    for ( col = 0; col < cols; col++ )
	rs[col] = gs[col] = bs[col] = HALFSCALE;
    fracrowtofill = SCALE;

    ppm_writeppminit( stdout, newcols, newrows, maxval );
    newpixelrow = ppm_allocrow( newcols );

    for ( row = 0; row < newrows; row++ )
	{
	/* First scale Y from pixelrow into temppixelrow. */
	if ( newrows == rows )	/* shortcut Y scaling if possible */
	    {
	    ppm_readppmrow( ifd, pixelrow, cols, maxval, format );
	    }
	else
	    {
	    while ( fracrowleft < fracrowtofill )
		{
		if ( needtoreadrow )
		    if ( rowsread < rows )
			{
			ppm_readppmrow( ifd, pixelrow, cols, maxval, format );
			rowsread++;
			/* needtoreadrow = 0; */
			}
		for ( col = 0, pP = pixelrow; col < cols; col++, pP++ )
		    {
		    rs[col] += fracrowleft * PPM_GETR( *pP );
		    gs[col] += fracrowleft * PPM_GETG( *pP );
		    bs[col] += fracrowleft * PPM_GETB( *pP );
		    }
		fracrowtofill -= fracrowleft;
		fracrowleft = syscale;
		needtoreadrow = 1;
		}
	    /* Now fracrowleft is >= fracrowtofill, so we can produce a row. */
	    if ( needtoreadrow )
		if ( rowsread < rows )
		    {
		    ppm_readppmrow( ifd, pixelrow, cols, maxval, format );
		    rowsread++;
		    needtoreadrow = 0;
		    }
	    for ( col = 0, pP = pixelrow, npP = temppixelrow;
		  col < cols; col++, pP++, npP++ )
		{
		register long r, g, b;

		r = rs[col] + fracrowtofill * PPM_GETR( *pP );
		g = gs[col] + fracrowtofill * PPM_GETG( *pP );
		b = bs[col] + fracrowtofill * PPM_GETB( *pP );
		r /= SCALE;
		if ( r > maxval ) r = maxval;
		g /= SCALE;
		if ( g > maxval ) g = maxval;
		b /= SCALE;
		if ( b > maxval ) b = maxval;
		PPM_ASSIGN( *npP, r, g, b );
		rs[col] = gs[col] = bs[col] = HALFSCALE;
		}
	    fracrowleft -= fracrowtofill;
	    if ( fracrowleft == 0 )
		{
		fracrowleft = syscale;
		needtoreadrow = 1;
		}
	    fracrowtofill = SCALE;
	    }

	/* Now scale X from temppixelrow into newpixelrow and write it out. */
	if ( newcols == cols )	/* shortcut X scaling if possible */
	    ppm_writeppmrow( stdout, temppixelrow, newcols, maxval );
	else
	    {
	    register long r, g, b;
	    register long fraccoltofill, fraccolleft;
	    register int needcol;

	    npP = newpixelrow;
	    fraccoltofill = SCALE;
	    r = g = b = HALFSCALE;
	    needcol = 0;
	    for ( col = 0, pP = temppixelrow; col < cols; col++, pP++ )
		{
		fraccolleft = sxscale;
		while ( fraccolleft >= fraccoltofill )
		    {
		    if ( needcol )
			{
			npP++;
			r = g = b = HALFSCALE;
			needcol = 0;
			}
		    r += fraccoltofill * PPM_GETR( *pP );
		    g += fraccoltofill * PPM_GETG( *pP );
		    b += fraccoltofill * PPM_GETB( *pP );
		    r /= SCALE;
		    if ( r > maxval ) r = maxval;
		    g /= SCALE;
		    if ( g > maxval ) g = maxval;
		    b /= SCALE;
		    if ( b > maxval ) b = maxval;
		    PPM_ASSIGN( *npP, r, g, b );
		    fraccolleft -= fraccoltofill;
		    fraccoltofill = SCALE;
		    needcol = 1;
		    }
		if ( fraccolleft > 0 )
		    {
		    if ( needcol )
			{
			npP++;
			r = g = b = HALFSCALE;
			needcol = 0;
			}
		    r += fraccolleft * PPM_GETR( *pP );
		    g += fraccolleft * PPM_GETG( *pP );
		    b += fraccolleft * PPM_GETB( *pP );
		    fraccoltofill -= fraccolleft;
		    }
		}
	    if ( fraccoltofill > 0 )
		{
		pP--;
		r += fraccoltofill * PPM_GETR( *pP );
		g += fraccoltofill * PPM_GETG( *pP );
		b += fraccoltofill * PPM_GETB( *pP );
		}
	    if ( ! needcol )
		{
		r /= SCALE;
		if ( r > maxval ) r = maxval;
		g /= SCALE;
		if ( g > maxval ) g = maxval;
		b /= SCALE;
		if ( b > maxval ) b = maxval;
		PPM_ASSIGN( *npP, r, g, b );
		}
	    ppm_writeppmrow( stdout, newpixelrow, newcols, maxval );
	    }
	}

    pm_close( ifd );

    exit( 0 );
    }
