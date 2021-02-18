/* ppmtops.c - read a portable pixmap and produce a PostScript file
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
#define index strchr
#else /*SYSV*/
#include <strings.h>
#endif /*SYSV*/

#define max(a,b) ((a) > (b) ? (a) : (b))

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    register pixel *pixrow, *pP;
    int argn, rows, cols, format, bps, padright, row, col;
    pixval maxval;
    float scale;
    char name[100], *cp;
    char *usage = "[-scale <x>] [ppmfile]";

    pm_progname = argv[0];

    argn = 1;
    scale = 1.0;

    /* Check for flags. */
    while ( argn < argc && argv[argn][0] == '-' )
	{
	if ( strncmp(argv[argn],"-scale",max(strlen(argv[argn]),2)) == 0 )
	    {
	    argn++;
	    if ( argn == argc || sscanf( argv[argn], "%f", &scale ) != 1 )
		pm_usage( usage );
	    }
	else
	    pm_usage( usage );
	argn++;
	}

    if ( argn < argc )
	{
	ifd = pm_openr( argv[argn] );
	strcpy( name, argv[argn] );
	if ( strcmp( name, "-" ) == 0 )
	    strcpy( name, "noname" );

	if ( ( cp = index( name, '.' ) ) != 0 )
	    *cp = '\0';
	argn++;
	}
    else
	{
	ifd = stdin;
	strcpy( name, "noname" );
	}

    if ( argn != argc )
	pm_usage( usage );

    ppm_readppminit( ifd, &cols, &rows, &maxval, &format );
    pixrow = ppm_allocrow( cols );

    /* Figure out bps. */
    bps = maxvaltobps( maxval );
    
    /* Compute padding to round cols * bps up to the nearest multiple of 8. */
    padright = ( ( cols * bps + 7 ) / 8 ) * 8 - cols * bps;

    putinit( name, cols, rows, bps, scale );
    for ( row = 0; row < rows; row++ )
	{
	ppm_readppmrow( ifd, pixrow, cols, maxval, format );
        for ( col = 0, pP = pixrow; col < cols; col++, pP++ )
	    putpix( *pP );
	for ( col = 0; col < padright; col++ )
	    putpix( maxval );
        }

    pm_close( ifd );

    putrest( );

    exit( 0 );
    }

int
maxvaltobps( maxval )
pixval maxval;
    {
    switch ( maxval )
	{
	case 1:
	return 1;

	case 3:
	return 2;

	case 15:
	return 4;

	case 255:
	return 8;

#ifdef notdef
	case 7:
	return 3;

	case 31:
	return 5;

	case 63:
	return 6;

	case 127:
	return 7;
#endif /*notdef*/

	default:
	pm_error( "maxval of %d is not supported", maxval, 0,0,0,0 );
	/* NOTREACHED */
	}
    }


int bitspersample, item, bitsperitem, bitshift, itemsperline, items;
#define HSBUFSIZ 384

putinit( name, cols, rows, bps, scale )
char *name;
int cols, rows, bps;
float scale;
    {
    int scols, srows, llx, lly;

    scols = cols * 0.96 + 0.5;	/*   0.96 is the multiple of   */
    srows = rows * 0.96 + 0.5;	/* 72/300 that is closest to 1 */
    llx = 300 - ( scols / 2 );
    lly = 400 - ( srows / 2 );

    printf( "%%!PS-Adobe-2.0 EPSF-2.)\n" );
    printf( "%%%%Creator: ppmtops\n" );
    printf( "%%%%Title: %s.ps\n", name );
    printf( "%%%%Pages: 1\n" );
    printf(
	"%%%%BoundingBox: %d %d %d %d\n", llx, lly, llx + scols, lly + srows );
    printf( "%%%%EndComments\n" );
    printf( "%%%%EndProlog\n" );
    printf( "%%%%Page 1 1\n" );
    printf( "/picstr %d string def\n", HSBUFSIZ );
    printf( "gsave\n" );
    printf( "%d %d translate\n", llx, lly );
    printf( "%g %g scale\n", scale, scale );
    printf( "%d %d scale\n", scols, srows );
    printf( "%d %d %d\n", cols, rows, bps );
    printf( "[ %d 0 0 -%d 0 %d ]\n", cols, rows, rows );
    printf( "{ currentfile picstr readhexstring pop }\n" );
    printf( "false 3\n" );
    printf( "colorimage\n" );

    bitspersample = bps;
    itemsperline = items = 0;
    item = 0;
    bitsperitem = 0;
    bitshift = 8 - bitspersample;
    }

putitem( )
    {
    char *hexits = "0123456789abcdef";

    if ( itemsperline == 30 )
	{
	putchar( '\n' );
	itemsperline = 0;
	}
    putchar( hexits[item >> 4] );
    putchar( hexits[item & 15] );
    itemsperline++;
    items++;
    item = 0;
    bitsperitem = 0;
    bitshift = 8 - bitspersample;
    }

putpix( p )
pixel p;
    {
    putgray( PPM_GETR( p ) );
    putgray( PPM_GETG( p ) );
    putgray( PPM_GETB( p ) );
    }

putgray( g )
pixval g;
    {
    if ( bitsperitem == 8 )
	putitem( );
    item += g << bitshift;
    bitsperitem += bitspersample;
    bitshift -= bitspersample;
    }

putrest( )
    {
    if ( bitsperitem > 0 )
	putitem( );
    while ( items % HSBUFSIZ != 0 )
	putitem( );
    printf( "\n" );
    printf( "grestore\n" );
    printf( "showpage\n" );
    printf( "%%%%Trailer\n" );
    }
