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
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

			All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
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

/* $Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelDash.c,v 1.4 88/11/03 20:29:57 paul Exp $ */
/* $Source: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelDash.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelDash.c,v 1.4 88/11/03 20:29:57 paul Exp $" ;
#endif

#include "X.h"
#include "Xproto.h"
#include "misc.h"
#include "font.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "region.h"

#include "mfb.h"

#include "mistruct.h"

#include "ppc.h"
#include "ppcProcs.h"

#include "ibmTrace.h"

#include "mpel.h"
#include "mpelProcs.h"

#define barrel_shift16(v, s) \
	(v = (v >> s) | (v << (16 - s)))

void
mpel_do_dashline_gc( pGC )
register GCPtr pGC ;
{
	register unsigned int len = 0 ;
	register unsigned int n ;
       	mpelPrivGCPtr mpelPriv =
		(mpelPrivGCPtr) ( (ppcPrivGCPtr) pGC->devPrivates[mfbGCPrivateIndex].ptr )->devPriv;

	/* check for dash pattern useable to mpel */
	for ( n = 0 ; n < pGC->numInDashList ; n++ )
		len += pGC->dash[n] ;

	if ( ( len < 16 && !( len & -len ) )
	  || ( len == 16 && !( pGC->numInDashList & 1 ) ) ) {
		register int bit ;
		register int offset = 0 ;
		register unsigned char *dp = &pGC->dash[0] ;
		mpelUserLineStyle ULineStyle ;

		ULineStyle.factor = 0 ;
		ULineStyle.mask = 0 ;
		/* encode dash pattern into bitmask */
		while ( n-- > 0 && *dp ) {
			ULineStyle.mask |= ( ( ( 1 << *dp ) - 1 ) << offset ) ;
			offset += *dp++ ;
			if ( n-- > 0 )
				offset += *dp++ ;
		}
		/* replicate pattern over full 16 bits */
		for ( bit = offset ; bit < 16 ; bit++ ) {
			ULineStyle.mask |=
				( ULineStyle.mask & ( 1 << bit - offset ) )
					<< offset ;
		}
		/* shift pattern to implement offset - mod 16 */
		/* is it legal for offset > length ?? */
		if ( pGC->dashOffset )
			barrel_shift16( ULineStyle.mask, pGC->dashOffset % 16 );

    		pGC->ops->PolySegment = mpelPolySegment ;
		pGC->ops->Polylines = mpelZeroLine ;
		pGC->ops->PolyArc = mpelPolyArc ;
		mpelPriv->LineType = MPEL_USERLINESTYLE_CONT ;
		mpelPriv->LineStyle = ULineStyle ;
	}
	else {
    		pGC->ops->PolySegment = miPolySegment ;
		pGC->ops->Polylines = ppcScrnZeroDash ;
		pGC->ops->PolyArc = miPolyArc ;
		mpelPriv->LineType = MPEL_SOLIDLINE ;
	}
	return ;
}
