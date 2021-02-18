/* pbmmask.c - create a mask bitmap from a portable bitmap
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

bit **bits, **mask, backcolor;
int rows, cols;

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    register int row, col, wcount;

    pm_progname = argv[0];

    if ( argc > 2 )
	pm_usage( "[pbmfile]" );

    if ( argc == 2 )
	ifd = pm_openr( argv[1] );
    else
	ifd = stdin;

    bits = pbm_readpbm( ifd, &cols, &rows );
    pm_close( ifd );
    mask = pbm_allocarray( cols, rows );

    /* Clear out the mask. */
    for ( row = 0; row < rows; row++ )
        for ( col = 0; col < cols; col++ )
	    mask[row][col] = PBM_BLACK;

    /* Figure out the background color, by counting along the edge. */
    wcount = 0;
    for ( row = 0; row < rows; row++ )
	{
	if ( bits[row][0] == PBM_WHITE )
	    wcount++;
	if ( bits[row][cols - 1] == PBM_WHITE )
	    wcount++;
	}
    for ( col = 1; col < cols - 1; col++ )
	{
	if ( bits[0][col] == PBM_WHITE )
	    wcount++;
	if ( bits[rows - 1][col] == PBM_WHITE )
	    wcount++;
	}
    if ( wcount >= rows + cols - 2 )
	backcolor = PBM_WHITE;
    else
	backcolor = PBM_BLACK;

    /* Flood the entire edge.  Probably the first call will be enough, but
    ** might as well be sure. */
    for ( col = cols - 3; col >= 2; col -= 2 )
	{
	addflood( col, rows - 1 );
	addflood( col, 0 );
	}
    for ( row = rows - 1; row >= 0; row -= 2 )
	{
	addflood( cols - 1, row );
	addflood( 0, row );
	}
    flood( );

    /* All done. */
    pbm_writepbm( stdout, mask, cols, rows );

    exit( 0 );
    }

#define FLOODSTACKSIZE 50000
short fcols[FLOODSTACKSIZE], frows[FLOODSTACKSIZE];
int fstackp = 0;

addflood( col, row )
int col, row;
    {
    if ( bits[row][col] == backcolor && mask[row][col] == PBM_BLACK )
	{
	if ( fstackp >= FLOODSTACKSIZE )
	    pm_error( "flood stack overflow", 0,0,0,0,0 );
	fcols[fstackp] = col;
	frows[fstackp] = row;
	fstackp++;
	}
    }

flood( )
    {
    register int col, row, c;

    while ( fstackp > 0 )
	{
	fstackp--;
	col = fcols[fstackp];
	row = frows[fstackp];
	if ( bits[row][col] == backcolor && mask[row][col] == PBM_BLACK )
	    {
	    mask[row][col] = PBM_WHITE;
	    if ( row - 1 >= 0 )
		addflood( col, row - 1 );
	    if ( row + 1 < rows )
		addflood( col, row + 1 );
	    for ( c = col + 1; c < cols; c++ )
		{
		if ( bits[row][c] == backcolor && mask[row][c] == PBM_BLACK )
		    {
		    mask[row][c] = PBM_WHITE;
		    if ( row - 1 >= 0 && ( bits[row - 1][c - 1] != backcolor || mask[row - 1][c - 1] != PBM_BLACK ) )
			addflood( c, row - 1 );
		    if ( row + 1 < rows && ( bits[row + 1][c - 1] != backcolor || mask[row + 1][c - 1] != PBM_BLACK ) )
			addflood( c, row + 1 );
		    }
		else
		    break;
		}
	    for ( c = col - 1; c >= 0; c-- )
		{
		if ( bits[row][c] == backcolor && mask[row][c] == PBM_BLACK )
		    {
		    mask[row][c] = PBM_WHITE;
		    if ( row - 1 >= 0 && ( bits[row - 1][c + 1] != backcolor || mask[row - 1][c + 1] != PBM_BLACK ) )
			addflood( c, row - 1 );
		    if ( row + 1 < rows && ( bits[row + 1][c + 1] != backcolor || mask[row + 1][c + 1] != PBM_BLACK ) )
			addflood( c, row + 1 );
		    }
		else
		    break;
		}
	    }
	}
    }
