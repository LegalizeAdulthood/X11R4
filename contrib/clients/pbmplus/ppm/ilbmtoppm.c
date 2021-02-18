/* ilbmtoppm.c - read an Amiga IFF ILBM file and produce a portable pixmap
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
#include "ilbm.h"

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    pixel *pixelrow, *colormap = 0;
    int argn, colors, i, j, r, g, b, byte, bytes;
    short rows, cols = 0, row, col;
    int maxval;
    char iffid[5];
    unsigned char *body = 0, *bp, *ubp, *rawrow, *runbuf;
    long formsize, bytesread, chunksize, viewportmodes = 0;
    int nPlanes, masking, compression, xAsp, yAsp, ham, hammask, allPlanes;
    unsigned char get_byte();

    pm_progname = argv[0];

    argn = 1;

    if ( argn < argc )
	{
	ifd = pm_openr( argv[argn] );
	argn++;
	}
    else
	ifd = stdin;

    if ( argn != argc )
	pm_usage( "[ilbmfile]" );

    /* Read in the ILBM file. */
    iffid[4] = '\0';
    getfourchars( ifd, iffid );
    if ( strcmp( iffid, "FORM" ) != 0 )
	pm_error( "input is not a FORM type IFF file", 0,0,0,0,0 );
    if ( pm_readbiglong( ifd, &formsize ) == -1 )
	pm_perror( 0 );
    getfourchars( ifd, iffid );
    if ( strcmp( iffid, "ILBM" ) != 0 )
	pm_error( "input is not an ILBM type FORM IFF file", 0,0,0,0,0 );
    bytesread = 12;

    /* Main loop, parsing the IFF FORM. */
    while ( bytesread < formsize )
	{
	getfourchars( ifd, iffid );
	if ( pm_readbiglong( ifd, &chunksize ) == -1 )
	    pm_perror( 0 );
	bytesread += 8;

	if ( body != 0 )
	    {
	    fprintf(
		stderr, "%s: \"%s\" chunk found after BODY chunk -- skipping\n",
		argv[0], iffid );
	    for ( i = 0; i < chunksize; i++ )
		(void) get_byte( ifd );
	    }
	else if ( strcmp( iffid, "BMHD" ) == 0 )
	    {
	    short junk;
	    if ( pm_readbigshort( ifd, &cols ) == -1 )
		pm_perror( 0 );
	    if ( pm_readbigshort( ifd, &rows ) == -1 )
		pm_perror( 0 );
	    if ( pm_readbigshort( ifd, &junk ) == -1 )
		pm_perror( 0 );
	    if ( pm_readbigshort( ifd, &junk ) == -1 )
		pm_perror( 0 );
	    nPlanes = get_byte( ifd );
	    masking = get_byte( ifd );
	    compression = get_byte( ifd );
	    (void) get_byte( ifd );	/* pad1 */
	    if ( pm_readbigshort( ifd, &junk ) == -1 )	/* transparentColor */
		pm_perror( 0 );
	    xAsp = get_byte( ifd );
	    yAsp = get_byte( ifd );
	    if ( pm_readbigshort( ifd, &junk ) == -1 )	/* pageWidth */
		pm_perror( 0 );
	    if ( pm_readbigshort( ifd, &junk ) == -1 )	/* pageHeight */
		pm_perror( 0 );
	    }
	else if ( strcmp( iffid, "CMAP" ) == 0 )
	    {
	    colors = chunksize / 3;
	    if ( colors > 0 )
		{
		colormap = ppm_allocrow( colors );
		for ( i = 0; i < colors; i++ )
		    {
		    r = get_byte( ifd );
		    g = get_byte( ifd );
		    b = get_byte( ifd );
		    PPM_ASSIGN( colormap[i], r, g, b );
		    }
		if ( colors * 3 != chunksize )
		    (void) get_byte( ifd );
		}
	    }
	else if ( strcmp( iffid, "CAMG" ) == 0 )
	    {
	    if ( pm_readbiglong( ifd, &viewportmodes ) == -1 )
		pm_perror( 0 );
	    }
	else if ( strcmp( iffid, "BODY" ) == 0 )
	    {
	    body = (unsigned char *) malloc( chunksize );
	    if ( body == 0 )
		pm_error( "out of memory", 0,0,0,0,0 );
	    if ( fread( body, 1, chunksize, ifd ) != chunksize )
		pm_error( "premature EOF reading BODY chunk", 0,0,0,0,0 );
	    }
	else if ( strcmp( iffid, "GRAB" ) == 0 ||
	          strcmp( iffid, "DEST" ) == 0 ||
	          strcmp( iffid, "SPRT" ) == 0 ||
	          strcmp( iffid, "CRNG" ) == 0 ||
	          strcmp( iffid, "CCRT" ) == 0 ||
	          strcmp( iffid, "DPPV" ) == 0 )
	    {
	    for ( i = 0; i < chunksize; i++ )
		(void) get_byte( ifd );
	    }
	else
	    {
	    fprintf(
		stderr, "%s: unknown chunk type \"%s\" -- skipping\n",
		argv[0], iffid );
	    for ( i = 0; i < chunksize; i++ )
		(void) get_byte( ifd );
	    }

	bytesread += chunksize;
	}

    pm_close( ifd );

    /* Done reading.  Now interpret what we got. */
    if ( cols == 0 )
	pm_error( "no BMHD chunk found", 0,0,0,0,0 );
    if ( body == 0 )
	pm_error( "no BODY chunk found", 0,0,0,0,0 );
    if ( xAsp != yAsp )
	fprintf(
	    stderr,
	    "(Warning: non-square pixels; to fix do a 'ppmscale -%cscale %g'.)\n",
	    xAsp > yAsp ? 'x' : 'y',
	    xAsp > yAsp ? (float) xAsp / yAsp : (float) yAsp / xAsp );
    if ( viewportmodes & vmHAM )
	{
	ham = 1;
	hammask = ( 1 << ( nPlanes - 2 ) ) - 1;
	maxval = ( 1 << ( nPlanes - 2 ) ) - 1;
	if ( maxval > PPM_MAXMAXVAL )
	    pm_error(
	     "nPlanes is too large - try recompiling with a larger pixval type",
		0,0,0,0,0 );
	if ( colormap != 0 )
	    for ( i = 0; i < colors; i++ )
		{
		r = PPM_GETR( colormap[i] ) >> ( 10 - nPlanes );
		g = PPM_GETG( colormap[i] ) >> ( 10 - nPlanes );
		b = PPM_GETB( colormap[i] ) >> ( 10 - nPlanes );
		PPM_ASSIGN( colormap[i], r, g, b );
		}
	}
    else
	{
	ham = 0;
	if ( colormap != 0 )
	    maxval = 255;		/* colormap contains bytes */
	else
	    maxval = ( 1 << nPlanes ) - 1;
	if ( maxval > PPM_MAXMAXVAL )
	    pm_error( "nPlanes is too large - try recompiling with a larger pixval type" );
	}
    if ( viewportmodes & vmEXTRA_HALFBRITE )
	{
	pixel *tempcolormap;
	
	tempcolormap = ppm_allocrow( colors * 2 );
	for ( i = 0; i < colors; i++ )
	    {
	    tempcolormap[i] = colormap[i];
	    PPM_ASSIGN(
		tempcolormap[colors + i], PPM_GETR(colormap[i]) / 2,
		PPM_GETG(colormap[i]) / 2, PPM_GETB(colormap[i]) / 2 );
	    }
	ppm_freerow( colormap );
	colormap = tempcolormap;
	colors *= 2;
	}
    if ( colormap == 0 )
	fprintf(
	    stderr, "(No colormap -- interpreting values as grayscale.)\n" );
    allPlanes = nPlanes + ( masking == mskHasMask ? 1 : 0 );

    ppm_writeppminit( stdout, cols, rows, (pixval) maxval );
    pixelrow = ppm_allocrow( cols );
    rawrow = (unsigned char *) malloc( cols );
    if ( rawrow == 0 )
	pm_error( "out of memory", 0,0,0,0,0 );
    runbuf = (unsigned char *) malloc( RowBytes( cols ) );
    if ( runbuf == 0 )
	pm_error( "out of memory", 0,0,0,0,0 );

    bp = body;
    for ( row = 0; row < rows; row++ )
	{
	/* Extract rawrow from the image. */
	for ( col = 0; col < cols; col++ )
	    rawrow[col] = 0;
	for ( i = 0; i < allPlanes; i++ )
	    {
	    switch ( compression )
		{
		case cmpNone:
		ubp = bp;
		bp += RowBytes( cols );
		break;

		case cmpByteRun1:
		ubp = runbuf;
		bytes = RowBytes( cols );
		do
		    {
		    byte = *bp++;
		    if ( byte <= 127 )
			for ( j = byte, bytes -= j + 1; j >= 0; j-- )
			    *ubp++ = *bp++;
		    else if ( byte != 128 )
			for ( j = 256 - byte, bytes -= j + 1, byte = *bp++;
			      j >= 0; j-- )
			    *ubp++ = byte;
		    }
		while ( bytes > 0 );
		if ( bytes < 0 )
		    pm_error( "error doing ByteRun decompression", 0,0,0,0,0 );
		ubp = runbuf;
		break;

		default:
		pm_error( "unknown compression type", 0,0,0,0,0 );
		}

	    if ( i >= nPlanes )
		continue;	/* ignore mask plane */

	    for ( col = 0; col < cols; col++ )
		if ( ubp[col / 8] & ( 128 >> ( col % 8 ) ) )
		    rawrow[col] |= 1 << i;
	    }

	/* Interpret rawrow into pixels. */
	r = g = b = 0;
	for ( col = 0; col < cols; col++ )
	    if ( ham )
		{ /* HAM mode. */
		switch ( ( rawrow[col] >> nPlanes - 2 ) & 0x3 )
		    {
		    case 0:
		    if ( colormap != 0 && colors >= maxval )
			pixelrow[col] = colormap[rawrow[col] & hammask];
		    else
			PPM_ASSIGN(
			    pixelrow[col], rawrow[col] & hammask,
			    rawrow[col] & hammask, rawrow[col] & hammask );
		    r = PPM_GETR( pixelrow[col] );
		    g = PPM_GETG( pixelrow[col] );
		    b = PPM_GETB( pixelrow[col] );
		    break;

		    case 1:
		    b = rawrow[col] & hammask;
		    PPM_ASSIGN( pixelrow[col], r, g, b );
		    break;

		    case 2:
		    r = rawrow[col] & hammask;
		    PPM_ASSIGN( pixelrow[col], r, g, b );
		    break;

		    case 3:
		    g = rawrow[col] & hammask;
		    PPM_ASSIGN( pixelrow[col], r, g, b );
		    break;

		    default:
		    pm_error( "impossible HAM code", 0,0,0,0,0 );
		    }
		}
	    else if ( colormap != 0 )
		/* Non-HAM colormapped. */
		pixelrow[col] = colormap[rawrow[col]];
	    else
		/* Non-HAM direct -- weird. */
		PPM_ASSIGN(
		    pixelrow[col], rawrow[col], rawrow[col], rawrow[col] );

	/* And write out the row. */
	ppm_writeppmrow( stdout, pixelrow, cols, (pixval) maxval );
	}

    exit( 0 );
    }

unsigned char
get_byte( f )
FILE *f;
    {
    int i;

    i = getc( f );
    if ( i == EOF )
	pm_error( "premature EOF", 0,0,0,0,0 );

    return (unsigned char) i;
    }

getfourchars( f, fourchars )
FILE *f;
char fourchars[4];
    {
    fourchars[0] = get_byte( f );
    fourchars[1] = get_byte( f );
    fourchars[2] = get_byte( f );
    fourchars[3] = get_byte( f );
    }
