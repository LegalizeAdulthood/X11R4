/* libppm4.c - ppm utility library part 4
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

pixel
ppm_backgroundpixel( pixels, cols, rows )
pixel **pixels;
int cols, rows;
    {
    pixel bgpixel, ul, ur, ll, lr;

    /* Guess a good background value. */
    ul = pixels[0][0];
    ur = pixels[0][cols-1];
    ll = pixels[rows-1][0];
    lr = pixels[rows-1][cols-1];

    /* First check for three corners equal. */
    if ( PPM_EQUAL( ul, ur ) && PPM_EQUAL( ur, ll ) )
	bgpixel = ul;
    else if ( PPM_EQUAL( ul, ur ) && PPM_EQUAL( ur, lr ) )
	bgpixel = ul;
    else if ( PPM_EQUAL( ul, ll ) && PPM_EQUAL( ll, lr ) )
	bgpixel = ul;
    else if ( PPM_EQUAL( ur, ll ) && PPM_EQUAL( ll, lr ) )
	bgpixel = ur;
    else
	/* Nope, so just average the four corners. */
	PPM_ASSIGN( bgpixel,
	    PPM_GETR(ul) + PPM_GETR(ur) + PPM_GETR(ll) + PPM_GETR(lr) / 4,
	    PPM_GETG(ul) + PPM_GETG(ur) + PPM_GETG(ll) + PPM_GETG(lr) / 4,
	    PPM_GETB(ul) + PPM_GETB(ur) + PPM_GETB(ll) + PPM_GETB(lr) / 4 );

    return bgpixel;
    }
