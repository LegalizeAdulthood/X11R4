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
/* $Header: mpelPolyPt.c,v 1.2 90/03/05 14:18:08 swick Exp $ */
/* $Source: /xsrc/mit/server/ddx/ibm/mpel/RCS/mpelPolyPt.c,v $ */

#ifndef lint
static char *rcsid = "$Header: mpelPolyPt.c,v 1.2 90/03/05 14:18:08 swick Exp $" ;
#endif

#include "X.h"
#include "misc.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "colormapst.h"
#include "scrnintstr.h"
#include "regionstr.h"
#include "region.h"

#include "OScompiler.h"

#include "ppc.h"

#include "mpel.h"
#include "mpelHdwr.h"
#include "mpelFifo.h"

#include "ibmTrace.h"

/* Cursor Stuff */
extern int mpelcursorSemaphore ;
extern int mpelCheckCursor() ;
extern void mpelReplaceCursor() ;

/* The maximum number of points in a Polymarker request. */
#define MAXRQPTS	(32760 / sizeof(mpelPoint))

void
mpelPolyPoint( pDrawable, pGC, mode, npt, pptInit )
DrawablePtr	pDrawable ;
GCPtr		pGC ;
int		mode ;				/* Origin or Previous */
int		npt ;
xPoint		*pptInit ;
{
	register xPoint *ppt ;
	ppcPrivGC *devPriv = (ppcPrivGC *) ( pGC->devPrivates[mfbGCPrivateIndex].ptr ) ;
	register RegionPtr pRegion = devPriv->pCompositeClip ;
	int	cursor_saved ;

	TRACE( ("mpelPolyPoint(0x%x,0x%x,\"%s\",%d,0x%x)\n",
		pDrawable, pGC, (mode == CoordModePrevious) ?
		"Relative" : "Absolute", npt, pptInit));

	if ( pGC->alu == GXnoop || REGION_NIL(pRegion) || npt == 0)
		return ;

	if ( pDrawable->type == DRAWABLE_PIXMAP ) {
		ppcPolyPoint( pDrawable, pGC, mode, npt, pptInit ) ;
		return ;
	}

	/* make pointlist origin relative
	 * AND adjust for the window org
	 * AND adjust for the Mpel's Backward Y coordinates
	 */
	{
	register const int xorg = pDrawable->x;
	register const int yorg = pDrawable->y;
	register int nptTmp = npt;
	register xPoint *mpt = pptInit;
	register int (* PointInRegion)() = pDrawable->pScreen->PointInRegion;
	BoxRec box;

	if ( mode == CoordModePrevious ) {
		ppt = pptInit;
		ppt->x += xorg;
		ppt->y += yorg;
		if((* PointInRegion)( pRegion, ppt->x, ppt->y, &box)) {
			mpt->x = ppt->x;
			mpt->y = (MPEL_HEIGHT - 1) - ppt->y;
			mpt++;
		}
		while (--nptTmp) {
			ppt++;
			ppt->x += xorg + (ppt-1)->x;
			ppt->y = yorg + ppt->y + (ppt-1)->y;
			if((* PointInRegion)( pRegion, ppt->x, ppt->y, &box)) {
				mpt->x = ppt->x;
				mpt->y = (MPEL_HEIGHT - 1) - ppt->y;
				mpt++;
			}
		}
		npt = mpt - pptInit;
	} else {
		for ( ppt = pptInit ; nptTmp-- ; ppt++ ) {
			ppt->x += xorg;
			ppt->y += yorg;
			if((* PointInRegion)( pRegion, ppt->x, ppt->y, &box)) {
				mpt->x = ppt->x;
				mpt->y = (MPEL_HEIGHT - 1) - ppt->y;
				mpt++;
			}
		}
		npt = mpt - pptInit;
	}
	}
	TRACE(("mpelPolyPoint: %d points\n", npt));
	if(!npt)	/* Nothing to do */
		return;
	/* If Cursor Is In The Way Remove It */
	cursor_saved = !mpelcursorSemaphore
		&& mpelCheckCursor(
			pRegion->extents.x1,
			pRegion->extents.y1,
			pRegion->extents.x2 - pRegion->extents.x1,
			pRegion->extents.y2 - pRegion->extents.y1
		) ;

	/* do the actual drawing */
	/* Check if the optimized rRop is valid */
	if ( devPriv->colorRrop.fillStyle == FillStippled
	  || devPriv->colorRrop.fillStyle == FillSolid ) {
		mpelSetALU( devPriv->colorRrop.alu ) ;
		mpelSetPlaneMask( devPriv->colorRrop.planemask ) ;
		MPELSetPolymarkerColor( devPriv->colorRrop.fgPixel ) ;
	}
	else {
		mpelSetALU( pGC->alu ) ;
		mpelSetPlaneMask( pGC->planemask ) ;
		MPELSetPolymarkerColor( pGC->fgPixel ) ;
	}
	MPELSetMarkerType( 1 ) ; /* Magic number for solid dots */
	/* Break into bite-sized pieces */
	{
	  register int rem;
	  /* MPELPolymarker does not evaluate any of its arguments
	     more than once. */
	  for(rem = npt;rem > 0;rem -= MAXRQPTS, pptInit+=MAXRQPTS)
	    MPELPolymarker((rem>MAXRQPTS)?MAXRQPTS:rem, pptInit);
	}
	if ( cursor_saved )
		mpelReplaceCursor() ;

	return ;
}
