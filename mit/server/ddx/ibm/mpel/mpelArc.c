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

/* $Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelArc.c,v 6.4 88/11/03 22:42:10 paul Exp $ */
/* $Source: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelArc.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelArc.c,v 6.4 88/11/03 22:42:10 paul Exp $" ;
#endif

#include "X.h"
#include "misc.h"
#include "Xprotostr.h"
#include "gcstruct.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "regionstr.h"
#include "pixmapstr.h"

#include "OScompiler.h"

#include "ppc.h"

#include "ibmTrace.h"

#include "mpel.h"
#include "mpelHdwr.h"
#include "mpelFifo.h"

/* Cursor Stuff */
extern int mpelcursorSemaphore ;
extern int mpelCheckCursor() ;
extern void mpelReplaceCursor() ;

#define FULLCIRCLE (360*64 )

/* mpelPolyArc --
 * If the arc is drawn with zero width lines, we
 * don't have to worry about the rasterop or join styles.
 */
void
mpelPolyArc(pDraw, pGC, narcs, parcs )
    DrawablePtr	pDraw ;
    GCPtr	pGC ;
    int		narcs ;
    xArc	*parcs ;
{
    register int	i ;
    RegionPtr		pCC ;
    int			nbox ;
    BoxRec		*pbox,bbox ;
    mpelRectangle	scissor ;
    int			clockwise = 1 ;
    ppcPrivGCPtr pPriv = (ppcPrivGCPtr)pGC->devPrivates[mfbGCPrivateIndex].ptr ;
    mpelPrivGCPtr mpelPriv = (mpelPrivGCPtr)pPriv->devPriv ;
    int cursor_saved ;

    TRACE(("mpelPolyArc( pDraw=x%x, pGC=x%x, narcs=%d, parcs=x%x )\n",
						pDraw, pGC, narcs, parcs)) ;

    if ( !narcs )
	return ;

    if ( pDraw->type != DRAWABLE_WINDOW ) {
	miPolyArc( pDraw, pGC, narcs, parcs ) ;
	return ;
    }

    pCC = pPriv->pCompositeClip ;
    if ( !( nbox = REGION_NUM_RECTS(pCC) ) )
	return ;

    pbox = REGION_RECTS(pCC) ;
    bbox.x2 = bbox.y2 = 0 ;
    bbox.x1 = MPEL_WIDTH ;
    bbox.y1 = MPEL_HEIGHT ;

    /* prepare arcs for MPEL */

    for ( i = 0 ; i < narcs ;i++ ) {
	register xArc *parc ;

	parc = &parcs[i] ;
	parc->x += pDraw->x ;
	parc->y += pDraw->y ;
	bbox.x1 = MIN( bbox.x1, parc->x ) ;
	bbox.y1 = MIN( bbox.y1, parc->y ) ;
	bbox.x2 = MAX( bbox.x2, parc->x + parc->width ) ;
	bbox.y2 = MAX( bbox.y2, parc->y + parc->height ) ;

	/* Throw out multiples of 360 degrees. */

	if ( parc->angle1 < 0 ) {
		while ( parc->angle1 < -FULLCIRCLE )
			parc->angle1 += FULLCIRCLE ;
	}
	else {
		while ( parc->angle1 > FULLCIRCLE )
			parc->angle1 -= FULLCIRCLE ;
	}

	if ( parc->angle2 < 0 ) {
		while ( parc->angle2 < -FULLCIRCLE )
			parc->angle2 += FULLCIRCLE ;
	}
	else {
		while ( parc->angle2 > FULLCIRCLE )
			parc->angle2 -= FULLCIRCLE ;
	}

	/* now convert to mpel */
	if ( parc->angle1 < 0 )
		parc->angle1 = FULLCIRCLE + parc->angle1 ;
	if ( parc->angle2 < 0 )
		clockwise = 0 ;
	parc->angle2 += parc->angle1 ;
	if ( parc->angle2 > FULLCIRCLE )
		parc->angle2 -= FULLCIRCLE ;
	if ( !clockwise ) {
		int tmp ;
		tmp = parc->angle1 ;
		parc->angle1 = parc->angle2 ;
		parc->angle2 = tmp ;
	}
	/* make mpel numbers */
	parc->width = parc->width / 2 ;
	parc->height = parc->height / 2 ;
	parc->y = ( MPEL_HEIGHT - 1 ) - ( parc->y + parc->height ) ;
	parc->x = parc->x + parc->width ;
	parc->angle1 = ( parc->angle1 / 64 ) * 10 ;
	parc->angle2 = ( parc->angle2 / 64 ) * 10 ;
    }

    /* If Cursor Is In The Way Remove It */
    cursor_saved = !mpelcursorSemaphore
    	    && mpelCheckCursor( bbox.x1, bbox.y1,
				bbox.x2 - bbox.x1, bbox.y2 - bbox.y1 ) ;

    mpelSetALU( pGC->alu ) ;
    mpelSetPlaneMask( pGC->planemask ) ;
    if ( mpelPriv->LineType == MPEL_USERLINESTYLE_CONT )
    	MPELSetUserLineStyle( &mpelPriv->LineStyle ) ;
    MPELSetLineType( mpelPriv->LineType ) ;
    MPELSetPolylineColor( pGC->fgPixel ) ;

/* SCISSOR CODE -- THIS CODE SUCKS -- FIX SOMEDAY */
    for ( i = 0 ; i < nbox ; i++, pbox++ ) {
	register arci ;
	mpelEllipticalArc Arc ;
	register xArc *pArc = parcs ;

	scissor.uright.x = 	pbox->x2 - 1 ;
	scissor.uright.y = 	MPEL_HEIGHT - 1 - pbox->y1 ;
	scissor.lleft.x =  	pbox->x1 ;
	scissor.lleft.y =	MPEL_HEIGHT - pbox->y2 ;
	MPELSetScissor( &scissor ) ;
	for ( arci = 0 ; arci < narcs ; arci++ ) {
		Arc.ellipse.center.x	= pArc[arci].x ;
		Arc.ellipse.center.y	= pArc[arci].y ;
		Arc.ellipse.majax	= pArc[arci].width ;
		Arc.ellipse.minax	= pArc[arci].height ;
		Arc.ellipse.angle	= 0 ;
		Arc.start		= pArc[arci].angle1 ;
		Arc.end			= pArc[arci].angle2 ;
		MPELEllipticalArc( &Arc ) ;
	}
    }

    MPELResetScissor() ;

    if ( cursor_saved )
	mpelReplaceCursor() ;

    return ;
}
