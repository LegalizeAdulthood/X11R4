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

/* $Header: mpelGC.c,v 1.2 89/11/13 18:39:40 rws Exp $ */
/* $Source: /xsrc/mit/server/ddx/ibm/mpel/RCS/mpelGC.c,v $ */

#ifndef lint
static char *rcsid = "$Header: mpelGC.c,v 1.2 89/11/13 18:39:40 rws Exp $" ;
#endif

#include "X.h"
#include "Xproto.h"
#include "font.h"
#include "misc.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "region.h"

#include "mfb.h"

#include "mistruct.h"

#include "ppc.h"
#include "ppcProcs.h"

#include "mpel.h"
#include "mpelProcs.h"

#include "ibmTrace.h"

Mask
mpelChangeGCtype( pGC, devPriv )
register GC *pGC ;
register ppcPrivGCPtr devPriv ;
{
	if ( devPriv->lastDrawableType == DRAWABLE_PIXMAP ) {
	    devPriv->FillArea = mfbSolidInvertArea ;
	    pGC->ops->CopyArea = miCopyArea ;
	    pGC->ops->PolyFillRect = miPolyFillRect ;
	    pGC->ops->PushPixels = miPushPixels ;
	    pGC->ops->ImageText8 = miImageText8 ;
	    pGC->ops->ImageText16 = miImageText16 ;
	    pGC->ops->PolyText8 = miPolyText8 ;
	    pGC->ops->PolyText16 = miPolyText16 ;

	    /* Maybe set to special case by kbg's code */
	    pGC->ops->FillPolygon = miFillPolygon ;
	    pGC->ops->PolySegment = miPolySegment ;
	    pGC->ops->Polylines = miZeroLine ;
	    pGC->ops->PolyArc = miPolyArc ;
	}
	else {
	    devPriv->FillArea = ppcAreaFill ;
	    pGC->ops->CopyArea = ppcCopyArea ;
	    pGC->ops->PolyFillRect = ppcPolyFillRect ;
	    pGC->ops->PushPixels = ppcPushPixels ;
	    pGC->ops->ImageText8 = mpelImageText8 ;
	    pGC->ops->ImageText16 = mpelImageText16 ;
	    pGC->ops->PolyText8 = mpelPolyText8 ;
	    pGC->ops->PolyText16 = mpelPolyText16 ;

	    if ( ! devPriv->devPriv ) /* If First Time */
		devPriv->devPriv =
			(mpelPrivGC *) Xalloc( sizeof (mpelPrivGC) ) ;

	}
	return ;
}

Mask
mpelChangeWindowGC( pGC, changes )
register GC *pGC ;
register Mask changes ;
{
register ppcPrivGCPtr devPriv = (ppcPrivGCPtr) (pGC->devPrivates[mfbGCPrivateIndex].ptr) ;
register unsigned long int index ; /* used for stepping through bitfields */

#define LOWBIT( x ) ( x & - x ) /* Two's complement */
    while ( index = LOWBIT( changes ) ) {
	switch ( index ) {

	  case GCLineStyle:
	  case GCLineWidth:
	    if ( pGC->lineWidth ) {
	    	pGC->ops->PolySegment = miPolySegment ;
#if !defined(AIXrt)
		pGC->ops->Polylines =
			( pGC->lineStyle == LineSolid )
			? miWideLine : miWideDash ;
#else
		if ( pGC->lineStyle == LineSolid )
			pGC->ops->Polylines = miWideLine ;
		else
			pGC->ops->Polylines = miWideDash ;
#endif
		pGC->ops->PolyArc = miPolyArc ;
	    }
	    else {
		if ( pGC->lineStyle != LineSolid ) {
			void mpel_do_dashline_gc() ;

			mpel_do_dashline_gc(pGC) ;
		}
		else {
	    		pGC->ops->PolySegment = mpelPolySegment ;
			pGC->ops->Polylines =  mpelZeroLine ;
			pGC->ops->PolyArc = mpelPolyArc ;
			((mpelPrivGCPtr) (devPriv->devPriv))->LineType
				= MPEL_SOLIDLINE ;
		}
	    }
	    /*
	     * If these are just square boxes with no funny business
	     * going on we can call the fast routine that draws
	     * rectangles without floating point.
	     */
	    if ( ( pGC->lineStyle == LineSolid ) 
			&& ( pGC->joinStyle == JoinMiter ) 
			&& ( pGC->lineWidth != 0 ) )
		pGC->ops->PolyRectangle = ppcPolyRectangle;
	    else
		pGC->ops->PolyRectangle = miPolyRectangle;
	    changes &= ~( GCLineStyle | GCLineWidth ) ;
	    break ;
	  case GCJoinStyle:
#if !defined(AIXrt)
	    pGC->ops->LineHelper =
		( pGC->joinStyle == JoinMiter ) ? miMiter : miNotMiter ;
#else
	if ( pGC->joinStyle == JoinMiter )
	    pGC->ops->LineHelper = miMiter ;
	else
	    pGC->ops->LineHelper = miNotMiter ;
#endif
	    /*
	     * If these are just square boxes with no funny business
	     * going on we can call the fast routine that draws
	     * rectangles without floating point.
	     */
	    if ( ( pGC->lineStyle == LineSolid ) 
			&& ( pGC->joinStyle == JoinMiter ) 
			&& ( pGC->lineWidth != 0 ) )
		pGC->ops->PolyRectangle = ppcPolyRectangle;
	    else
		pGC->ops->PolyRectangle = miPolyRectangle;
	    changes &= ~ index ; /* i.e. changes &= ~ GCJoinStyle */
	    break ;

	  case GCBackground:
	    if ( pGC->fillStyle != FillOpaqueStippled ) {
		changes &= ~ index ; /* i.e. changes &= ~GCBackground */
		break ;
	    } /* else Fall Through */
	  case GCForeground:
	    if ( pGC->fillStyle == FillTiled ) {
		changes &= ~ index ; /* i.e. changes &= ~GCForeground */
		break ;
	    } /* else Fall Through */
	  case GCFunction:
	  case GCPlaneMask:
	  case GCFillStyle:
	    { /* new_fill */
		int fillStyle = devPriv->colorRrop.fillStyle ;
		/* install a suitable fillspans */
		if ( fillStyle == FillSolid ) {
		    pGC->ops->FillSpans = ppcSolidWindowFS ;
		    pGC->ops->FillPolygon = mpelFillPolygon ;
		}
		else if ( fillStyle == FillStippled ) {
		    pGC->ops->FillSpans = ppcStippleWindowFS ;
		    pGC->ops->FillPolygon = miFillPolygon ;
		}
		else if ( fillStyle == FillOpaqueStippled ) {
		    pGC->ops->FillSpans = ppcOpStippleWindowFS ;
		    pGC->ops->FillPolygon = miFillPolygon ;
		}
		else { /*  fillStyle == FillTiled */
			register temp = pGC->tile.pixmap->drawable.width ;
			if ( temp <= 16 && !( temp & - temp ) ) {
				temp = pGC->tile.pixmap->drawable.height ;
				if ( temp <= 16 && !( temp & - temp ) )
					pGC->ops->FillPolygon = mpelTilePolygon ;
				else
					pGC->ops->FillPolygon = miFillPolygon ;
			}
			else
				pGC->ops->FillPolygon = miFillPolygon ;
		    pGC->ops->FillSpans = ppcTileWindowFS ;
		}
	    } /* end of new_fill */
	    changes &= ~( GCBackground | GCForeground | GCFunction
		     | GCPlaneMask | GCFillStyle ) ;
	    break ;

	default:
	    ErrorF( "mpelChangeWindowGC: Unexpected GC Change\n" ) ;
	    changes &= ~ index ; /* Remove it anyway */
	    break ;
	}
    }
    return ;
}

/*
 * mpelDestroyGCPriv - do whatever is necessary to clean up
 *	the devPriv field in the ppcGC structure.
 * NOTE: the field is filled in at changeType time above
 */

void
mpelDestroyGCPriv( pPriv )
pointer pPriv ;
{
	Xfree( pPriv ) ;
	return ;
}
