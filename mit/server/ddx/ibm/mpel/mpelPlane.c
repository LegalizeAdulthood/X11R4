/*
 * Copyright IBM Corporation 1987,1988,1989
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
/***********************************************************
		Copyright IBM Corporation 1987,1988

		      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $Header: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelPlane.c,v 1.4 88/11/14 18:30:38 paul Exp $ */
/* $Source: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelPlane.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelPlane.c,v 1.4 88/11/14 18:30:38 paul Exp $" ;
#endif

#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "os.h"

#include "OScompiler.h"

#include "ibmTrace.h"

/***==================================================================***/


/* Was MIGETPLANE -- gets a bitmap representing one plane of pDraw
 * A helper used for CopyPlane and XY format GetImage
 * No clever strategy here, we grab a scanline at a time, pull out the
 * bits and then stuff them in a 1 bit deep map.
 */
unsigned long *
mpelGetPlane( planeNum, sx, sy, w, h, result )
    int planeNum ;	/* number of the bitPlane */
    int sx, sy, w, h ;
    unsigned long *result ;
{
    int i, j, k ;
    register unsigned char *pp ;
    unsigned char *pline ;
    unsigned long int bit ;
#if BITMAP_SCANLINE_UNIT == 8
    register unsigned char tmpScanUnitOut ;
    unsigned char *pScanUnitOut = (unsigned char *) result ;
#endif
#if BITMAP_SCANLINE_UNIT == 16
    register CARD16 tmpScanUnitOut ;
    CARD16 *pScanUnitOut = (CARD16 *) result ;
#endif
#if BITMAP_SCANLINE_UNIT == 32
    register CARD32 tmpScanUnitOut ;
    CARD32 *pScanUnitOut = (CARD32 *) result ;
#endif

TRACE( ( "mpelGetPlane(%d,%d,%d,%d,%d,0x%X)\n",
       planeNum, sx, sy, w, h, result ) ) ;

    pline = (unsigned char *) ALLOCATE_LOCAL( w ) ;

    for ( i = sy ; i < sy + h ; i++ ) {
	/* Fetch the next line */
	mpelReadColorImage( sx, i, w, 1, pline, w ) ;
	for ( j = 0, pp = pline, tmpScanUnitOut = 0, k = 0 ;
	      j < w ;
	      j++, pp++ ) {
	    bit = (unsigned long int) ( ( *pp >> planeNum ) & 1 ) ;
	    /* Now insert that bit into a bitmap in XY format */
#if (BITMAP_BIT_ORDER == LSBFIRST)
	    bit >> = k ;
#else
	    bit <<= ( ( BITMAP_SCANLINE_UNIT - 1 ) - k ) ;
#endif
	    tmpScanUnitOut |= bit ;
	    if ( ++k == BITMAP_SCANLINE_UNIT ) {
		*pScanUnitOut++ = tmpScanUnitOut ;
		tmpScanUnitOut = 0 ;
		k = 0 ;
	    }
        }
	if ( k )
	  *pScanUnitOut++ = tmpScanUnitOut ;
    }
    DEALLOCATE_LOCAL( pline ) ;

    return result ;
}
