/* pbmtorast.c - read a portable bitmap and produce a Sun rasterfile
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
    register bit *bitrow, *bP;
    int argn, pr_type, linesize;
    int rows, cols, format, row, col;
    struct pixrect *pr, *mem_create();
    unsigned char *data, *byteP;
    int bitcount;
    char *usage = "[-s] [pbmfile]";

    pm_progname = argv[0];

    argn = 1;
    pr_type = RT_BYTE_ENCODED;

    if ( argc - argn >= 1 && argv[argn][0] == '-' )
	{
	if ( ( argv[argn][1] == 's' || argv[argn][1] == 'S' ) &&
	     argv[argn][2] == '\0' )
	    {
	    pr_type = RT_STANDARD;
	    argn++;
	    }
	else
	    pm_usage( usage );
	}

    if ( argc - argn > 1 )
	pm_usage( usage );

    if ( argc - argn == 1 )
	ifd = pm_openr( argv[argn] );
    else
	ifd = stdin;

    pbm_readpbminit( ifd, &cols, &rows, &format );
    bitrow = pbm_allocrow( cols );
    
    if ( (pr = mem_create(cols, rows, 1)) == NULL )
	pm_error( "unable to create new pixrect", 0,0,0,0,0 );

    data = ( (struct mpr_data *) pr->pr_data )->md_image;
    linesize = ( (struct mpr_data *) pr->pr_data )->md_linebytes;

    for ( row = 0; row < rows; row++ )
	{
	pbm_readpbmrow( ifd, bitrow, cols, format );
	byteP = data;
	*byteP = 0;
	bitcount = 7;
	for ( col = 0, bP = bitrow; col < cols; col++, bP++ )
	    {
	    if ( *bP == PBM_BLACK )
		*byteP |= 1 << bitcount;
	    bitcount--;
	    if ( bitcount < 0 )
		{
		byteP++;
		*byteP = 0;
		bitcount = 7;
		}
	    }
	data += linesize;
	}

    pm_close( ifd );

    pr_dump( pr, stdout, NULL, pr_type, 0 );

    exit( 0 );
    }
