/* libppm5.c - ppm utility library part 5
**
** This library module contains the ppmdraw routines.
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
#include "ppmdraw.h"


#define DDA_SCALE 8192
#define abs(x) ((x) < 0 ? -(x) : (x))
#define min(x,y) ((x) < (y) ? (x) : (y))
#define max(x,y) ((x) > (y) ? (x) : (y))


void
ppmd_point_drawproc( pixels, cols, rows, maxval, x, y, clientdata )
pixel **pixels;
int cols, rows, x, y;
pixval maxval;
char *clientdata;
    {
    if ( x >= 0 && x < cols && y >= 0 && y < rows )
	pixels[y][x] = *( (pixel *) clientdata );
    }


/* Simple fill routine. */

void
ppmd_filledrectangle( pixels, cols, rows, maxval, x, y, width, height, drawprocP, clientdata)
pixel **pixels;
int cols, rows, x, y, width, height;
pixval maxval;
void (*drawprocP)();
char *clientdata;
    {
    register cx, cy, cwidth, cheight, col, row;

    /* Clip. */
    cx = x;
    cy = y;
    cwidth = width;
    cheight = height;
    if ( cx < 0 )
	{
	cx = 0;
	cwidth += x;
	}
    if ( cy < 0 )
	{
	cy = 0;
	cheight += y;
	}
    if ( cx + cwidth > cols )
	cwidth = cols - cx;
    if ( cy + cheight > rows )
	cheight = rows - cy;

    /* Draw. */
    for ( row = cy; row < cy + cheight; row++ )
	for ( col = cx; col < cx + cwidth; col++ )
	    if ( drawprocP == PPMD_NULLDRAWPROC )
		pixels[row][col] = *( (pixel *) clientdata );
	    else
		(*drawprocP)(
		    pixels, cols, rows, maxval, col, row, clientdata );
    }


/* Outline drawing stuff. */

static int ppmd_linetype = PPMD_LINETYPE_NORMAL;

int
ppmd_setlinetype( type )
int type;
    {
    int old;

    old = ppmd_linetype;
    ppmd_linetype = type;
    return old;
    }

static int ppmd_lineclip = 1;

int
ppmd_setlineclip( clip )
int clip;
    {
    int old;

    old = ppmd_lineclip;
    ppmd_lineclip = clip;
    return old;
    }

void
ppmd_line( pixels, cols, rows, maxval, x0, y0, x1, y1, drawprocP, clientdata )
pixel **pixels;
int cols, rows, x0, y0, x1, y1;
pixval maxval;
void (*drawprocP)();
char *clientdata;
    {
    register int cx0, cy0, cx1, cy1;

    /* Special case zero-length lines. */
    if ( x0 == x1 && y0 == y1 )
	{
	if ( drawprocP == PPMD_NULLDRAWPROC )
	    ppmd_point_drawproc(
		pixels, cols, rows, maxval, x0, y0, clientdata );
	else
	    (*drawprocP)( pixels, cols, rows, maxval, x0, y0, clientdata );
	return;
	}

    /* Clip. */
    cx0 = x0;
    cy0 = y0;
    cx1 = x1;
    cy1 = y1;
    if ( ppmd_lineclip )
	{
	if ( cx0 < 0 )
	    {
	    if ( cx1 < 0 ) return;
	    cy0 = cy0 + ( cy1 - cy0 ) * ( -cx0 ) / ( cx1 - cx0 );
	    cx0 = 0;
	    }
	else if ( cx0 >= cols )
	    {
	    if ( cx1 >= cols ) return;
	    cy0 = cy0 + ( cy1 - cy0 ) * ( cols - 1 - cx0 ) / ( cx1 - cx0 );
	    cx0 = cols - 1;
	    }
	if ( cy0 < 0 )
	    {
	    if ( cy1 < 0 ) return;
	    cx0 = cx0 + ( cx1 - cx0 ) * ( -cy0 ) / ( cy1 - cy0 );
	    cy0 = 0;
	    }
	else if ( cy0 >= rows )
	    {
	    if ( cy1 >= rows ) return;
	    cx0 = cx0 + ( cx1 - cx0 ) * ( rows - 1 - cy0 ) / ( cy1 - cy0 );
	    cy0 = rows - 1;
	    }
	if ( cx1 < 0 )
	    {
	    cy1 = cy1 + ( cy0 - cy1 ) * ( -cx1 ) / ( cx0 - cx1 );
	    cx1 = 0;
	    }
	else if ( cx1 >= cols )
	    {
	    cy1 = cy1 + ( cy0 - cy1 ) * ( cols - 1 - cx1 ) / ( cx0 - cx1 );
	    cx1 = cols - 1;
	    }
	if ( cy1 < 0 )
	    {
	    cx1 = cx1 + ( cx0 - cx1 ) * ( -cy1 ) / ( cy0 - cy1 );
	    cy1 = 0;
	    }
	else if ( cy1 >= rows )
	    {
	    cx1 = cx1 + ( cx0 - cx1 ) * ( rows - 1 - cy1 ) / ( cy0 - cy1 );
	    cy1 = rows - 1;
	    }

	/* Check again for zero-length lines. */
	if ( cx0 == cx1 && cy0 == cy1 )
	    {
	    if ( drawprocP == PPMD_NULLDRAWPROC )
		ppmd_point_drawproc(
		    pixels, cols, rows, maxval, cx0, cy0, clientdata );
	    else
		(*drawprocP)(
		    pixels, cols, rows, maxval, cx0, cy0, clientdata );
	    return;
	    }
	}

    /* Draw, using a simple DDA. */
    if ( abs( cx1 - cx0 ) > abs( cy1 - cy0 ) )
	{ /* Loop over X domain. */
	register long dy, srow;
	register int dx, col, row, prevrow;

	if ( cx1 > cx0 )
	    dx = 1;
	else
	    dx = -1;
	dy = ( cy1 - cy0 ) * DDA_SCALE / abs( cx1 - cx0 );
	prevrow = row = cy0;
	srow = row * DDA_SCALE + DDA_SCALE / 2;
	col = cx0;
	for ( ; ; )
	    {
	    if ( ppmd_linetype == PPMD_LINETYPE_NODIAGS && row != prevrow )
		{
		if ( drawprocP == PPMD_NULLDRAWPROC )
		    pixels[prevrow][col] = *( (pixel *) clientdata );
		else
		    (*drawprocP)(
		        pixels, cols, rows, maxval, col, prevrow, clientdata );
		prevrow = row;
		}
	    if ( drawprocP == PPMD_NULLDRAWPROC )
		pixels[row][col] = *( (pixel *) clientdata );
	    else
		(*drawprocP)(
		    pixels, cols, rows, maxval, col, row, clientdata );
	    if ( col == cx1 )
		break;
	    srow += dy;
	    row = srow / DDA_SCALE;
	    col += dx;
	    }
	}
    else
	{ /* Loop over Y domain. */
	register long dx, scol;
	register int dy, col, row, prevcol;

	if ( cy1 > cy0 )
	    dy = 1;
	else
	    dy = -1;
	dx = ( cx1 - cx0 ) * DDA_SCALE / abs( cy1 - cy0 );
	row = cy0;
	prevcol = col = cx0;
	scol = col * DDA_SCALE + DDA_SCALE / 2;
	for ( ; ; )
	    {
	    if ( ppmd_linetype == PPMD_LINETYPE_NODIAGS && col != prevcol )
		{
		if ( drawprocP == PPMD_NULLDRAWPROC )
		    pixels[row][prevcol] = *( (pixel *) clientdata );
		else
		    (*drawprocP)(
			pixels, cols, rows, maxval, prevcol, row, clientdata );
		prevcol = col;
		}
	    if ( drawprocP == PPMD_NULLDRAWPROC )
		pixels[row][col] = *( (pixel *) clientdata );
	    else
		(*drawprocP)(
		    pixels, cols, rows, maxval, col, row, clientdata );
	    if ( row == cy1 )
		break;
	    row += dy;
	    scol += dx;
	    col = scol / DDA_SCALE;
	    }
	}
    }

#define SPLINE_THRESH 3
void
ppmd_spline3( pixels, cols, rows, maxval, x0, y0, x1, y1, x2, y2, drawprocP, clientdata )
pixel **pixels;
int cols, rows, x0, y0, x1, y1, x2, y2;
pixval maxval;
void (*drawprocP)();
char *clientdata;
    {
    register int xa, ya, xb, yb, xc, yc, xp, yp;

    xa = ( x0 + x1 ) / 2;
    ya = ( y0 + y1 ) / 2;
    xc = ( x1 + x2 ) / 2;
    yc = ( y1 + y2 ) / 2;
    xb = ( xa + xc ) / 2;
    yb = ( ya + yc ) / 2;

    xp = ( x0 + xb ) / 2;
    yp = ( y0 + yb ) / 2;
    if ( abs( xa - xp ) + abs( ya - yp ) > SPLINE_THRESH )
	ppmd_spline3(
	    pixels, cols, rows, maxval, x0, y0, xa, ya, xb, yb, drawprocP, clientdata );
    else
	ppmd_line(
	    pixels, cols, rows, maxval, x0, y0, xb, yb, drawprocP, clientdata );

    xp = ( x2 + xb ) / 2;
    yp = ( y2 + yb ) / 2;
    if ( abs( xc - xp ) + abs( yc - yp ) > SPLINE_THRESH )
	ppmd_spline3(
	    pixels, cols, rows, maxval, xb, yb, xc, yc, x2, y2, drawprocP,
	    clientdata );
    else
	ppmd_line(
	    pixels, cols, rows, maxval, xb, yb, x2, y2, drawprocP, clientdata );
    }

void
ppmd_polyspline( pixels, cols, rows, maxval, x0, y0, nc, xc, yc, x1, y1, drawprocP, clientdata )
pixel **pixels;
int cols, rows, x0, y0, nc, *xc, *yc, x1, y1;
pixval maxval;
void (*drawprocP)();
char *clientdata;
    {
    register int i, x, y, xn, yn;

    x = x0;
    y = y0;
    for ( i = 0; i < nc - 1; i++ )
	{
	xn = ( xc[i] + xc[i + 1] ) / 2;
	yn = ( yc[i] + yc[i + 1] ) / 2;
	ppmd_spline3(
	    pixels, cols, rows, maxval, x, y, xc[i], yc[i], xn, yn, drawprocP,
	    clientdata );
	x = xn;
	y = yn;
	}
    ppmd_spline3(
	pixels, cols, rows, maxval, x, y, xc[nc - 1], yc[nc - 1], x1, y1,
	drawprocP, clientdata );
    }

void
ppmd_circle( pixels, cols, rows, maxval, cx, cy, radius, drawprocP, clientdata )
pixel **pixels;
int cols, rows, cx, cy, radius;
pixval maxval;
void (*drawprocP)();
char *clientdata;
    {
    register int x0, y0, x, y, prevx, prevy, nopointsyet;
    register long sx, sy, e;

    x0 = x = radius;
    y0 = y = 0;
    sx = x * DDA_SCALE + DDA_SCALE / 2;
    sy = y * DDA_SCALE + DDA_SCALE / 2;
    e = DDA_SCALE / radius;
    if ( drawprocP == PPMD_NULLDRAWPROC )
	pixels[y + cy][x + cx] = *( (pixel *) clientdata );
    else
	(*drawprocP)( pixels, cols, rows, maxval, x + cx, y + cy, clientdata );
    nopointsyet = 1;
    do
	{
	prevx = x;
	prevy = y;
	sx += e * sy / DDA_SCALE;
	sy -= e * sx / DDA_SCALE;
	x = sx / DDA_SCALE;
	y = sy / DDA_SCALE;
	if ( x != prevx || y != prevy )
	    {
	    nopointsyet = 0;
	    if ( drawprocP == PPMD_NULLDRAWPROC )
		pixels[y + cy][x + cx] = *( (pixel *) clientdata );
	    else
		(*drawprocP)(
		    pixels, cols, rows, maxval, x + cx, y + cy, clientdata );
	    }
	}
    while ( nopointsyet || x != x0 || y != y0 );
    }


/* Arbitrary fill stuff. */

typedef struct
    {
    int x;
    int y;
    } coord;
typedef struct
    {
    int n;
    int size;
    coord *coords;
    } fillobj;

#define SOME 1000

static int oldclip;

char *
ppmd_fill_init( )
    {
    fillobj *fh;

    fh = (fillobj *) malloc( sizeof(fillobj) );
    if ( fh == 0 )
	pm_error( "out of memory allocating a fillhandle", 0,0,0,0,0 );
    fh->n = 0;
    fh->coords = (coord *) malloc( SOME * sizeof(coord) );
    if ( fh->coords == 0 )
	pm_error( "out of memory allocating a fillhandle", 0,0,0,0,0 );
    fh->coords[0].x = fh->coords[0].y = -27182;
    fh->size = SOME;

    /* Turn off line clipping. */
    oldclip = ppmd_setlineclip( 0 );
    
    return (char *) fh;
    }

void
ppmd_fill_drawproc( pixels, cols, rows, maxval, x, y, clientdata )
pixel **pixels;
int cols, rows, x, y;
pixval maxval;
char *clientdata;
    {
    register fillobj *fh;
    register coord *cp;

    fh = (fillobj *) clientdata;
    cp = &(fh->coords[fh->n]);

    /* If these are the same coords we saved last time, don't bother. */
    if ( x == cp->x && y == cp->y )
	return;

    /* Ok, these are new; check if there's enough room. */
    if ( fh->n >= fh->size )
	{
	fh->size += SOME;
	fh->coords = (coord *) realloc(
	    (char *) fh->coords, fh->size * sizeof(coord) );
	if ( fh->coords == 0 )
	    pm_error( "out of memory enlarging a fillhandle", 0,0,0,0,0 );
	cp = &(fh->coords[fh->n]);
	}

    /* And save 'em. */
    cp->x = x;
    cp->y = y;
    fh->n++;
    }

void
ppmd_fill( pixels, cols, rows, maxval, fillhandle, drawprocP, clientdata )
pixel **pixels;
int cols, rows;
pixval maxval;
char *fillhandle;
void (*drawprocP)();
char *clientdata;
    {
    register fillobj *fh;
    register int i, on, px, py, col;
    register coord *cp;
    static int yxcompare();

    fh = (fillobj *) fillhandle;

    /* Restore clipping now. */
    (void) ppmd_setlineclip( oldclip );

    /* Sort the coords by Y, and secondarily by X. */
    qsort( (char *) fh->coords, fh->n, sizeof(coord), yxcompare );

    /* Ok, now run through the coords.  This code doesn't deal with
    ** inflection points too well, but it would not be hard to fix. */
    on = 0;
    px = py = -31415;
    for ( i = 0; i < fh->n; i++ )
	{
	cp = &(fh->coords[i]);
	if ( on )
	    {
	    if ( cp->y != py )
		{ /* Broken span.  Hmm.  Start a new one. */
		px = cp->x;
		py = cp->y;
		}
	    else
		if ( cp->x <= px + 1 )
		    { /* Not an end-of-span, but a continuation of the start. */
		    if ( py >= 0 && py < rows && px >= 0 && px < cols )
			if ( drawprocP == PPMD_NULLDRAWPROC )
			    pixels[py][px] = *( (pixel *) clientdata );
			else
			    (*drawprocP)(
				pixels, cols, rows, maxval, px, py,
				clientdata );
		    px = cp->x;
		    }
		else
		    { /* Got a span to fill.  But clip it first. */
		    if ( py >= 0 && py < rows )
			{
			if ( px < 0 )
			    px = 0;
			if ( cp->x >= cols )
			    cp->x = cols - 1;
			if ( px <= cp->x )
			    for ( col = px; col <= cp->x; col++ )
				if ( drawprocP == PPMD_NULLDRAWPROC )
				    pixels[py][col] = *( (pixel *) clientdata );
				else
				    (*drawprocP)(
					pixels, cols, rows, maxval, col, py,
					clientdata );
			}
		    px = cp->x;
		    on = 0;
		    }
	    }
	else
	    {
	    if ( cp->y == py && cp->x <= px + 1 )
		{ /* Not a start-of-span, but a continuation of the end. */
		px = cp->x;
		if ( py >= 0 && py < rows && px >= 0 && px < cols )
		    if ( drawprocP == PPMD_NULLDRAWPROC )
			pixels[py][px] = *( (pixel *) clientdata );
		    else
			(*drawprocP)(
			    pixels, cols, rows, maxval, px, py, clientdata );
		}
	    else
		{
		px = cp->x;
		py = cp->y;
		on = 1;
		}
	    }
	}

    /* All done.  Free up the fillhandle and leave. */
    free( fh->coords );
    free( fh );
    }

static int
yxcompare( c1, c2 )
coord *c1, *c2;
    {
    if ( c1->y > c2->y )
	return 1;
    if ( c1->y < c2->y )
	return -1;
    if ( c1->x > c2->x )
	return 1;
    if ( c1->x < c2->x )
	return -1;
    return 0;
    }
