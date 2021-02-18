/* pbmtoxbm.c - read a portable bitmap and produce an X11 bitmap file
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
    bit *bitrow;
    register bit *bP;
    int rows, cols, format, padright, row;
    register int col;
    char name[100], *cp;
    int itemsperline;
    register int bitsperitem;
    register int item;
    int firstitem;
    static char hexchar[] = "0123456789abcdef";

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
    
    /* Compute padding to round cols up to the nearest multiple of 8. */
    padright = ( ( cols + 7 ) / 8 ) * 8 - cols;

    printf( "#define %s_width %d\n", name, cols );
    printf( "#define %s_height %d\n", name, rows );
    printf( "static char %s_bits[] = {\n", name );

    itemsperline = 0;
    bitsperitem = 0;
    item = 0;
    firstitem = 1;

#define putitem() \
    { \
    if ( firstitem ) \
	firstitem = 0; \
    else \
	putchar( ',' ); \
    if ( itemsperline == 15 ) \
	{ \
	putchar( '\n' ); \
	itemsperline = 0; \
	} \
    if ( itemsperline == 0 ) \
	putchar( ' ' ); \
    itemsperline++; \
    putchar('0'); \
    putchar('x'); \
    putchar(hexchar[item >> 4]); \
    putchar(hexchar[item & 15]); \
    bitsperitem = 0; \
    item = 0; \
    }

#define putbit(b) \
    { \
    if ( bitsperitem == 8 ) \
	putitem( ); \
    if ( (b) == PBM_BLACK ) \
	item += 1 << bitsperitem; \
    bitsperitem++; \
    }

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

    if ( bitsperitem > 0 )
	putitem( );
    printf( "};\n" );

    exit( 0 );
    }
