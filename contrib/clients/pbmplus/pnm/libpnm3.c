/* libpnm3.c - pnm utility library part 3
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

#ifdef PPM
#include "ppm.h"
#include "libppm.h"
#endif /*PPM*/

#ifdef PGM
#include "pgm.h"
#include "libpgm.h"
#endif /*PGM*/

#ifdef PBM
#include "pbm.h"
#include "libpbm.h"
#endif /*PBM*/

xel
pnm_backgroundxel( xels, cols, rows, maxval, format )
xel **xels;
xelval maxval;
int cols, rows, format;
    {
    xel bgxel, ul, ur, ll, lr;

    /* Guess a good background value. */
    ul = xels[0][0];
    ur = xels[0][cols-1];
    ll = xels[rows-1][0];
    lr = xels[rows-1][cols-1];

    /* First check for three corners equal. */
    if ( PNM_EQUAL( ul, ur ) && PNM_EQUAL( ur, ll ) )
	bgxel = ul;
    else if ( PNM_EQUAL( ul, ur ) && PNM_EQUAL( ur, lr ) )
	bgxel = ul;
    else if ( PNM_EQUAL( ul, ll ) && PNM_EQUAL( ll, lr ) )
	bgxel = ul;
    else if ( PNM_EQUAL( ur, ll ) && PNM_EQUAL( ll, lr ) )
	bgxel = ur;
    /* Nope, check for two corners equal. */
    else if ( PNM_EQUAL( ul,  ur ) || PNM_EQUAL( ul,  ll ) ||
	      PNM_EQUAL( ul,  lr ) )
	bgxel = ul;
    else if ( PNM_EQUAL( ur,  ll ) || PNM_EQUAL( ul,  lr ) )
	bgxel = ur;
    else if ( PNM_EQUAL( ll,  lr ) )
	bgxel = ll;
    else
	{
	/* Nope, we have to average the four corners.  This breaks the
	** rules of pnm, but oh well.  Let's try to do it portably. */
	switch ( format )
	    {
#ifdef PPM
	    case PPM_FORMAT:
	    case RPPM_FORMAT:
	    PPM_ASSIGN( bgxel,
		PPM_GETR(ul) + PPM_GETR(ur) + PPM_GETR(ll) + PPM_GETR(lr) / 4,
		PPM_GETG(ul) + PPM_GETG(ur) + PPM_GETG(ll) + PPM_GETG(lr) / 4,
		PPM_GETB(ul) + PPM_GETB(ur) + PPM_GETB(ll) + PPM_GETB(lr) / 4 );
	    break;
#endif /*PPM*/

#ifdef PGM
	    case PGM_FORMAT:
	    case RPGM_FORMAT:
	    {
	    gray gul, gur, gll, glr;
	    gul = (gray) PNM_GET1( ul );
	    gur = (gray) PNM_GET1( ur );
	    gll = (gray) PNM_GET1( ll );
	    glr = (gray) PNM_GET1( lr );
	    PNM_ASSIGN1( bgxel, ( ( gul + gur + gll + glr ) / 4 ) );
	    break;
	    }
#endif /*PGM*/

#ifdef PBM
	    case PBM_FORMAT:
	    case RPBM_FORMAT:
	    pm_error(
		"pnm_backgroundxel: four bits no two of which equal each other??",
		0,0,0,0,0 );
#endif /*PBM*/

	    default:
	    pm_error( "can't happen", 0,0,0,0,0 );
	    }
	}

    return bgxel;
    }

xel
pnm_backgroundxelrow( xelrow, cols, maxval, format )
xel *xelrow;
xelval maxval;
int cols, format;
    {
    xel bgxel, l, r;

    /* Guess a good background value. */
    l = xelrow[0];
    r = xelrow[cols-1];

    /* First check for both corners equal. */
    if ( PNM_EQUAL( l, r ) )
	bgxel = l;
    else
	{
	/* Nope, we have to average the two corners.  This breaks the
	** rules of pnm, but oh well.  Let's try to do it portably. */
	switch ( format )
	    {
#ifdef PPM
	    case PPM_FORMAT:
	    case RPPM_FORMAT:
	    PPM_ASSIGN( bgxel, PPM_GETR(l) + PPM_GETR(r) / 2,
		PPM_GETG(l) + PPM_GETG(r) / 2, PPM_GETB(l) + PPM_GETB(r) / 2 );
	    break;
#endif /*PPM*/

#ifdef PGM
	    case PGM_FORMAT:
	    case RPGM_FORMAT:
	    {
	    gray gl, gr;
	    gl = (gray) PNM_GET1( l );
	    gr = (gray) PNM_GET1( r );
	    PNM_ASSIGN1( bgxel, ( ( gl + gr ) / 2 ) );
	    break;
	    }
#endif /*PGM*/

#ifdef PBM
	    case PBM_FORMAT:
	    case RPBM_FORMAT:
	    {
	    int col, blacks;
	    bit b;
	    /* One black, one white.  Gotta count. */
	    for ( col = 0, blacks = 0; col < cols; ++col )
		{
		b = (bit) PNM_GET1( xelrow[col] );
		if ( b == PBM_BLACK )
		    ++blacks;
		}
	    if ( blacks >= cols / 2 )
		PNM_ASSIGN1( bgxel, PBM_BLACK );
	    else
		PNM_ASSIGN1( bgxel, PBM_WHITE );
	    break;
	    }
#endif /*PBM*/

	    default:
	    pm_error( "can't happen", 0,0,0,0,0 );
	    }
	}

    return bgxel;
    }

xel
pnm_whitexel( maxval, format )
xelval maxval;
int format;
    {
    xel x;

    switch ( format )
	{
#ifdef PPM
	case PPM_FORMAT:
	case RPPM_FORMAT:
	PPM_ASSIGN( x, maxval, maxval, maxval );
	break;
#endif /*PPM*/

#ifdef PGM
	case PGM_FORMAT:
	case RPGM_FORMAT:
	PNM_ASSIGN1( x, maxval );
	break;
#endif /*PGM*/

#ifdef PBM
	case PBM_FORMAT:
	case RPBM_FORMAT:
	PNM_ASSIGN1( x, (xelval) PBM_WHITE );
	break;
#endif /*PBM*/

	default:
	pm_error( "can't happen", 0,0,0,0,0 );
	}

    return x;
    }

xel
pnm_blackxel( maxval, format )
xelval maxval;
int format;
    {
    xel x;

    switch ( format )
	{
#ifdef PPM
	case PPM_FORMAT:
	case RPPM_FORMAT:
	PPM_ASSIGN( x, 0, 0, 0 );
	break;
#endif /*PPM*/

#ifdef PGM
	case PGM_FORMAT:
	case RPGM_FORMAT:
	PNM_ASSIGN1( x, (xelval) 0 );
	break;
#endif /*PGM*/

#ifdef PBM
	case PBM_FORMAT:
	case RPBM_FORMAT:
	PNM_ASSIGN1( x, (xelval) PBM_BLACK );
	break;
#endif /*PBM*/

	default:
	pm_error( "can't happen", 0,0,0,0,0 );
	}

    return x;
    }

xel
pnm_invertxel( x, maxval, format )
xel x;
xelval maxval;
int format;
    {
    switch ( format )
	{
#ifdef PPM
	case PPM_FORMAT:
	case RPPM_FORMAT:
	PPM_ASSIGN(
	    x, maxval - PPM_GETR( x ),
	    maxval - PPM_GETG( x ), maxval - PPM_GETB( x ) );
	break;
#endif /*PPM*/

#ifdef PGM
	case PGM_FORMAT:
	case RPGM_FORMAT:
	PNM_ASSIGN1( x, (gray) maxval - (gray) PNM_GET1( x ) );
	break;
#endif /*PGM*/

#ifdef PBM
	case PBM_FORMAT:
	case RPBM_FORMAT:
	PNM_ASSIGN1(
	    x, ( (bit) PNM_GET1( x ) == PBM_WHITE ) ? PBM_BLACK : PBM_WHITE );
	break;
#endif /*PBM*/

	default:
	pm_error( "can't happen", 0,0,0,0,0 );
	}
    return x;
    }

void
pnm_promoteformat( xels, cols, rows, maxval, format, newmaxval, newformat )
xel **xels;
xelval maxval, newmaxval;
int cols, rows, format, newformat;
    {
    int row;

    for ( row = 0; row < rows; ++row )
	pnm_promoteformatrow(
	    xels[row], cols, maxval, format, newmaxval, newformat );
    }

void
pnm_promoteformatrow( xelrow, cols, maxval, format, newmaxval, newformat )
xel *xelrow;
xelval maxval, newmaxval;
int cols, format, newformat;
    {
    register int col;
    register xel *xP;

#ifdef PPM
    if ( ( ( format == PPM_FORMAT || format == RPPM_FORMAT ) &&
	   ( newformat == PGM_FORMAT || newformat == RPGM_FORMAT ||
	     newformat == PBM_FORMAT || newformat == RPBM_FORMAT ) ) ||
	 ( ( format == PGM_FORMAT || format == RPGM_FORMAT ) &&
	   ( newformat == PBM_FORMAT || newformat == RPBM_FORMAT ) ) )
	pm_error( "pnm_promoteformatrow: can't promote downwards!", 0,0,0,0,0 );
#else /*PPM*/
# ifdef PGM
    if ( ( ( format == PGM_FORMAT || format == RPGM_FORMAT ) &&
	   ( newformat == PBM_FORMAT || newformat == RPBM_FORMAT ) ) )
	pm_error( "pnm_promoteformatrow: can't promote downwards!", 0,0,0,0,0 );
# else /*PGM*/
#  ifdef PBM
    /* If only PBM is defined, we can just return. */
    return;
#  endif /*PBM*/
# endif /*PGM*/
#endif /*PPM*/

#ifdef PPM
    if ( ( ( format == PPM_FORMAT || format == RPPM_FORMAT ) &&
           ( newformat == PPM_FORMAT || newformat == RPPM_FORMAT ) ) ||
         ( ( format == PGM_FORMAT || format == RPGM_FORMAT ) &&
           ( newformat == PGM_FORMAT || newformat == RPGM_FORMAT ) ) ||
         ( ( format == PBM_FORMAT || format == RPBM_FORMAT ) &&
           ( newformat == PBM_FORMAT || newformat == RPBM_FORMAT ) ) )
#else /*PPM*/
# ifdef PGM
    if ( ( ( format == PGM_FORMAT || format == RPGM_FORMAT ) &&
           ( newformat == PGM_FORMAT || newformat == RPGM_FORMAT ) ) ||
         ( ( format == PBM_FORMAT || format == RPBM_FORMAT ) &&
           ( newformat == PBM_FORMAT || newformat == RPBM_FORMAT ) ) )
# else /*PGM*/
#  ifdef PBM
    if ( ( ( format == PBM_FORMAT || format == RPBM_FORMAT ) &&
           ( newformat == PBM_FORMAT || newformat == RPBM_FORMAT ) ) )
#  endif /*PBM*/
# endif /*PGM*/
#endif /*PPM*/
	{
#ifdef PBM
	if ( format == PBM_FORMAT || format == RPBM_FORMAT )
	    return;
#endif /*PBM*/
	if ( newmaxval < maxval )
	    pm_error(
		"pnm_promoteformatrow: can't decrease maxval -- try using ppmcscale",
		0,0,0,0,0 );
	if ( newmaxval == maxval )
	    return;
	/* Increase maxval. */
	switch ( format )
	    {
#ifdef PGM
	    case PGM_FORMAT:
	    case RPGM_FORMAT:
	    for ( col = 0, xP = xelrow; col < cols; col++, xP++ )
		PNM_ASSIGN1(
		    *xP, (int) PNM_GET1(*xP) * newmaxval / maxval );
	    break;
#endif /*PGM*/

#ifdef PPM
	    case PPM_FORMAT:
	    case RPPM_FORMAT:
	    for ( col = 0, xP = xelrow; col < cols; col++, xP++ )
		PPM_CSCALE( *xP, *xP, maxval, newmaxval );
	    break;
#endif /*PPM*/

	    default:
	    pm_error( "shouldn't happen", 0,0,0,0,0 );
	    }
	return;
	}

    switch ( format )
	{
#ifdef PBM
	case PBM_FORMAT:
	case RPBM_FORMAT:
	switch ( newformat )
	    {
#ifdef PGM
	    case PGM_FORMAT:
	    case RPGM_FORMAT:
	    for ( col = 0, xP = xelrow; col < cols; col++, xP++ )
		PNM_ASSIGN1(
		    *xP, ( PNM_GET1(*xP) == PBM_WHITE ) ? newmaxval : (xelval) 0 );
	    break;
#endif /*PGM*/

#ifdef PPM
	    case PPM_FORMAT:
	    case RPPM_FORMAT:
	    for ( col = 0, xP = xelrow; col < cols; col++, xP++ )
		if ( PNM_GET1(*xP) == PBM_WHITE )
		    PPM_ASSIGN( *xP, newmaxval, newmaxval, newmaxval );
		else
		    PPM_ASSIGN( *xP, 0, 0, 0 );
	    break;
#endif /*PPM*/

	    default:
	    pm_error( "can't happen", 0,0,0,0,0 );
	    }
	break;
#endif /*PBM*/

#ifdef PGM
	case PGM_FORMAT:
	case RPGM_FORMAT:
	switch ( newformat )
	    {
#ifdef PPM
	    case PPM_FORMAT:
	    case RPPM_FORMAT:
	    if ( newmaxval < maxval )
		pm_error(
		    "pnm_promoteformatrow: can't decrease maxval -- try using ppmcscale",
		    0,0,0,0,0 );
	    if ( newmaxval == maxval )
		{
		for ( col = 0, xP = xelrow; col < cols; col++, xP++ )
		    PPM_ASSIGN(
			*xP, PNM_GET1(*xP), PNM_GET1(*xP), PNM_GET1(*xP) );
		}
	    else
		{ /* Increase maxval. */
		for ( col = 0, xP = xelrow; col < cols; col++, xP++ )
		    PPM_ASSIGN(
			*xP, (int) PNM_GET1(*xP) * newmaxval / maxval,
			(int) PNM_GET1(*xP) * newmaxval / maxval,
			(int) PNM_GET1(*xP) * newmaxval / maxval );
		}
	    break;
#endif /*PPM*/

	    default:
	    pm_error( "can't happen", 0,0,0,0,0 );
	    }
	break;
#endif /*PGM*/

	default:
	pm_error( "can't happen", 0,0,0,0,0 );
	}
    }
