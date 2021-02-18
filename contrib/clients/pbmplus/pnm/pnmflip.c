/* pnmflip.c - perform one or more flip operations on a portable anymap
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
#include "pnm.h"
#ifdef SYSV
#include <string.h>
#else /*SYSV*/
#include <strings.h>
#endif /*SYSV*/

#define max(a,b) ((a) > (b) ? (a) : (b))

main( argc, argv )
int argc;
char *argv[];
    {
    FILE *ifd;
    xel *xelrow, **newxels;
    register xel *xP;
    int argn, cols, rows, format, newrows, newcols;
    int a, b, c, d, e, f;
    register int row, col, newrow, newcol;
    xelval maxval;
    void leftright(), topbottom(), transpose();
    char *usage = "[-leftright|-lr] [-topbottom|-tb] [-transpose|-xy]\n            [-rotate90|-r90|-ccw] [-rotate270|r270|-cw]\n            [-rotate180|-r180] [pnmfile]";

    pm_progname = argv[0];

    argn = 1;

    /* Just check the validity of arguments here. */
    while ( argn < argc && argv[argn][0] == '-' )
	{
	if ( strncmp(argv[argn],"-lr",max(strlen(argv[argn]),2)) == 0 ||
	     strncmp(argv[argn],"-leftright",max(strlen(argv[argn]),2)) == 0 )
	    { }
	else if ( strncmp(argv[argn],"-tb",max(strlen(argv[argn]),3)) == 0 ||
	     strncmp(argv[argn],"-topbottom",max(strlen(argv[argn]),3)) == 0 )
	    { }
	else if ( strncmp(argv[argn],"-xy",max(strlen(argv[argn]),2)) == 0 ||
	     strncmp(argv[argn],"-transpose",max(strlen(argv[argn]),3)) == 0 )
	    { }
	else if ( strncmp(argv[argn],"-r90",max(strlen(argv[argn]),3)) == 0 ||
	     strncmp(argv[argn],"-rotate90",max(strlen(argv[argn]),8)) == 0 ||
	     strncmp(argv[argn],"-ccw",max(strlen(argv[argn]),3)) == 0 )
	    { }
	else if ( strncmp(argv[argn],"-r270",max(strlen(argv[argn]),3)) == 0 ||
	     strncmp(argv[argn],"-rotate270",max(strlen(argv[argn]),8)) == 0 ||
	     strncmp(argv[argn],"-cw",max(strlen(argv[argn]),3)) == 0 )
	    { }
	else if ( strncmp(argv[argn],"-r180",max(strlen(argv[argn]),3)) == 0 ||
	     strncmp(argv[argn],"-rotate180",max(strlen(argv[argn]),8)) == 0 )
	    { }
	else
	    pm_usage( usage );
	argn++;
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

    pnm_readpnminit( ifd, &cols, &rows, &maxval, &format );
    xelrow = pnm_allocrow( cols );

    /* Now go through the flags again, this time accumulating transforms. */
    a = 1; b = 0;
    c = 0; d = 1;
    e = 0; f = 0;
    argn = 1;
    while ( argn < argc && argv[argn][0] == '-' )
	{
	if ( strncmp(argv[argn],"-lr",max(strlen(argv[argn]),2)) == 0 ||
	     strncmp(argv[argn],"-leftright",max(strlen(argv[argn]),2)) == 0 )
	    leftright( &a, &b, &c, &d, &e, &f );
	else if ( strncmp(argv[argn],"-tb",max(strlen(argv[argn]),3)) == 0 ||
	     strncmp(argv[argn],"-topbottom",max(strlen(argv[argn]),3)) == 0 )
	    topbottom( &a, &b, &c, &d, &e, &f );
	else if ( strncmp(argv[argn],"-xy",max(strlen(argv[argn]),2)) == 0 ||
	     strncmp(argv[argn],"-transpose",max(strlen(argv[argn]),3)) == 0 )
	    transpose( &a, &b, &c, &d, &e, &f );
	else if ( strncmp(argv[argn],"-r90",max(strlen(argv[argn]),3)) == 0 ||
	     strncmp(argv[argn],"-rotate90",max(strlen(argv[argn]),8)) == 0 ||
	     strncmp(argv[argn],"-ccw",max(strlen(argv[argn]),3)) == 0 )
	    {
	    transpose( &a, &b, &c, &d, &e, &f );
	    topbottom( &a, &b, &c, &d, &e, &f );
	    }
	else if ( strncmp(argv[argn],"-r270",max(strlen(argv[argn]),3)) == 0 ||
	     strncmp(argv[argn],"-rotate270",max(strlen(argv[argn]),8)) == 0 ||
	     strncmp(argv[argn],"-cw",max(strlen(argv[argn]),3)) == 0 )
	    {
	    transpose( &a, &b, &c, &d, &e, &f );
	    leftright( &a, &b, &c, &d, &e, &f );
	    }
	else if ( strncmp(argv[argn],"-r180",max(strlen(argv[argn]),3)) == 0 ||
	     strncmp(argv[argn],"-rotate180",max(strlen(argv[argn]),8)) == 0 )
	    {
	    leftright( &a, &b, &c, &d, &e, &f );
	    topbottom( &a, &b, &c, &d, &e, &f );
	    }
	else
	    pm_error( "shouldn't happen!", 0,0,0,0,0 );
	argn++;
	}

    /* Okay, we've got a matrix.  Now read in the anymap a line at a time
    ** and transform it into an in-memory array.
    */

    newcols = abs( a ) * cols + abs( c ) * rows;
    newrows = abs( b ) * cols + abs( d ) * rows;
    newxels = pnm_allocarray( newcols, newrows );

    for ( row = 0; row < rows; ++row )
	{
	pnm_readpnmrow( ifd, xelrow, cols, maxval, format );
	for ( col = 0, xP = xelrow; col < cols; ++col, ++xP )
	    {
	    /* Transform a point:
	    **
	    **            [ a b 0 ]
	    **  [ x y 1 ] [ c d 0 ] = [ x2 y2 1 ]
	    **            [ e f 1 ]
	    */
	    newcol = a * col + c * row + e * ( newcols - 1 );
	    newrow = b * col + d * row + f * ( newrows - 1 );
	    newxels[newrow][newcol] = *xP;
	    }
	}

    pm_close( ifd );

    pnm_writepnm( stdout, newxels, newcols, newrows, maxval, format );

    exit( 0 );
    }

void
leftright( aP, bP, cP, dP, eP, fP )
int *aP, *bP, *cP, *dP, *eP, *fP;
    {
    *aP = - *aP;
    *cP = - *cP;
    *eP = - *eP + 1;
    }

void
topbottom( aP, bP, cP, dP, eP, fP )
int *aP, *bP, *cP, *dP, *eP, *fP;
    {
    *bP = - *bP;
    *dP = - *dP;
    *fP = - *fP + 1;
    }

void
transpose( aP, bP, cP, dP, eP, fP )
int *aP, *bP, *cP, *dP, *eP, *fP;
    {
    register int t;

    t = *aP;
    *aP = *bP;
    *bP = t;
    t = *cP;
    *cP = *dP;
    *dP = t;
    t = *eP;
    *eP = *fP;
    *fP = t;
    }
