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
		Copyright IBM Corporation 1987

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

/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedFillSp.c,v 1.2 89/11/19 20:24:49 jfc Exp $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedFillSp.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedFillSp.c,v 1.2 89/11/19 20:24:49 jfc Exp $";
#endif

#include "X.h"
#include "Xmd.h"

#include "misc.h"
#include "gcstruct.h"
#include "pixmapstr.h"
#include "windowstr.h"

#include "mfb.h"

#include "ibmTrace.h"
#include "aedHdwr.h"

/* scanline filling for monochrome frame buffer
   written by drewry, oct 1986

   these routines all clip.  they assume that anything that has called
them has already translated the points (i.e. pGC->miTranslate is
non-zero, which is howit gets set in mfbCreateGC().)

   the number of new scnalines created by clipping ==
MaxRectsPerBand * nSpans.

    FillSolid is overloaded to be used for OpaqueStipple as well,
if fgPixel == bgPixel.  
Note that for solids, PrivGC.rop == PrivGC.ropOpStip


    FillTiled is overloaded to be used for OpaqueStipple, if
fgPixel != bgPixel.  based on the fill style, it uses
{RotatedTile, gc.alu} or {RotatedStipple, PrivGC.ropOpStip}
*/


void aedSolidFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int		nInit;			/* number of spans to fill */
    DDXPointPtr pptInit;		/* pointer to list of start points */
    int		*pwidthInit;		/* pointer to list of n widths */
    int 	fSorted;
{
				/* next three parameters are post-clip */
    int n;			/* number of spans to fill */
    register DDXPointPtr ppt;	/* pointer to list of start points */
    register int *pwidth;	/* pointer to list of n widths */
    int rop;			/* reduced rasterop */
    int *pwidthFree;		/* copies of the pointers to free */
    DDXPointPtr pptFree;

    TRACE(("aedSolidFS(pDrawable= 0x%x, pGC= 0x%x, nInit= %d, ppt= %d, pwidth= 0x%x, fSorted= %d)\n", pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted));

    if (!(pGC->planemask & 1))
	return;

    n = nInit * miFindMaxBand(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip);
    pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
	DEALLOCATE_LOCAL(ppt);
	DEALLOCATE_LOCAL(pwidth);
	return;
    }
    pwidthFree = pwidth;
    pptFree = ppt;
    n = miClipSpans(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip,
		     pptInit, pwidthInit, nInit,
		     ppt, pwidth, fSorted);

    rop = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->rop;
/*************************************************************************/
    /* XXXXX
     * code to fill the spans.  At this point the following is true:
     *		rop will be either RROP_BLACK, RROP_WHITE,
     * 			or RROP_INVERT.  
     *		n = # of spans
     *		ppt points to list of starting points
     *		pwidth points to list of widths
     */

{

short i;
unsigned short x0;
vforce();
clear(9);
switch(rop)
	{
	case RROP_BLACK:   vikint[ORMERGE] = 0; break;
	case RROP_WHITE:   vikint[ORMERGE] =15; break;
	case RROP_INVERT:  vikint[ORMERGE] = 5; break;
	default: ErrorF("aedSolidFS: bad GXalu\n");
		 vikint[ORMERGE] = 15;
		 break;
	}
TRACE(("filling %d spans after clipping\n",n));
if( n <= 0 )
	return;

vikint[ORXPOSN] =  x0 = (unsigned short) ppt->x;	/* move to x0 */
vikint[ORYPOSN] =  (unsigned short) ppt->y;		/* move to y0 */
vikint[ORCLIPHX] = x0 + (unsigned short) *pwidth++ -1 ;
ppt++;

vikint[vikoff++] = 10;	  		/* tile order */
vikint[vikoff++] = 1024;		/* rectangle width */
vikint[vikoff++] = 1;	  		/* rectangle height */
vikint[vikoff++] = 1;	  		/* tile width */
vikint[vikoff++] = 1;	 	 	/* tile height */
vikint[vikoff++] = -1;	  		/* tile (all ones) */

vforce();

for(i=1; i<n; i++)
	{
	vikint[ORXPOSN] = x0 = (unsigned short) ppt->x;	/* move to x0 */
	vikint[ORYPOSN] = (unsigned short) ppt->y;	/* move to y0 */
	ppt++;
	vikint[ORCLIPHX] = x0 + *pwidth++ -1 ;
	vikint[VIKCMD] 	= 2; 		/* reprocess orders */ 
	command(ORDATA);
	}
clear(2);
}
/*************************************************************************/
    DEALLOCATE_LOCAL(pptFree);
    DEALLOCATE_LOCAL(pwidthFree);
}


/* Fill spans with tiles */
void
aedTileFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
DrawablePtr pDrawable;
GC		*pGC;
int		nInit;		/* number of spans to fill */
DDXPointPtr pptInit;		/* pointer to list of start points */
int *pwidthInit;		/* pointer to list of n widths */
int fSorted;
{
    int n;			/* number of spans to fill */
    register DDXPointPtr ppt;	/* pointer to list of start points */
    register int *pwidth;	/* pointer to list of n widths */
    PixmapPtr	pTile;		/* pointer to tile we want to fill with */
    int 	tileWidth, rop;
    int *pwidthFree;		/* copies of the pointers to free */
    DDXPointPtr pptFree;
    int tilelen,skip,tileHeight;
    short i,j;
    unsigned short x0;
    unsigned short *bits;

    TRACE(("aedTileFS(pDrawable= 0x%x [bg %d], pGC= 0x%x, nInit= %d, ppt= %d, pwidth= 0x%x, fSorted= %d)\n",
	   pDrawable, ((WindowPtr)pDrawable)->backgroundState, pGC, nInit, pptInit, pwidthInit, fSorted));

    if (!(pGC->planemask & 1))
	return;

    n = nInit * miFindMaxBand(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip);
    pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
	DEALLOCATE_LOCAL(ppt);
	DEALLOCATE_LOCAL(pwidth);
	return;
    }
    pwidthFree = pwidth;
    pptFree = ppt;
    n = miClipSpans(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip,
		     pptInit, pwidthInit, nInit, 
		     ppt, pwidth, fSorted);
    if ( n <= 0 )
	return;

    pTile = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pRotatedPixmap;
    TRACE(("pTile = %x, {w = %d, h = %d}\n", pTile, pTile->drawable.width,
	   pTile->drawable.height));

    if (pGC->fillStyle == FillTiled)
      {
	rop = pGC->alu;
	if (pTile == 0)
	  pTile = pGC->tile.pixmap;
      } else if ( pGC->fillStyle == FillStippled ) {
	switch (((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->rop)
	  {
	  case RROP_BLACK:   rop = GXandInverted; break;
	  case RROP_WHITE:   rop = GXor; break;
	  case RROP_INVERT:  rop = GXxor; break;
	  default: rop = GXnoop;
	  }
	if (pTile == 0)
	  pTile = pGC->stipple;
      } else {
	rop = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->ropOpStip;
	if (pTile == 0)
	  pTile = pGC->stipple;
      }
    /* XXXXX
     * code to fill the spans.  At this point the following is true:
     *		n = # of spans
     *		ppt points to list of starting points
     *		pwidth points to list of widths
     *		pTile points to the tile bits (pre-rotated)
     *		rop is the alu operation to perform between the tile 
     *			and the screen
     */

    tileWidth = pTile->drawable.width;
    tileHeight = pTile->drawable.height;
    tilelen = (tileWidth + 15 )/16;
    TRACE(("tileWidth = %d, tileHeight = %d\n", tileWidth, tileHeight));
    vforce();

    clear(5+(tilelen * tileHeight));

    vikint[ORMERGE] = mergexlate[rop];
    vikint[ORXPOSN] = x0 = (unsigned short) ppt->x;	/* move to x0 */
    vikint[ORYPOSN] = (unsigned short) ppt->y;	/* move to y0 */
    vikint[ORCLIPHX] = x0 + (unsigned short) *pwidth++ -1 ;
    ppt++;

    bits = (unsigned short *)pTile->devPrivate.ptr;

    if ((tileHeight * tilelen) > 2000) 
    {
	ErrorF("aedTileFS: tile too large\n");
	tileWidth = 1;
	tileHeight = 1;
	tilelen = 1;
    }
    skip = (((tileWidth-1) & 0x1f) < 16);
    TRACE(("skip = %d, bits = %#x\n", skip, bits));
	
    vikint[vikoff++] = 10;	  		/* tile order */
    vikint[vikoff++] = 1024;		/* rectangle width */
    vikint[vikoff++] = 1;	  		/* rectangle height */
    vikint[vikoff++] = tileWidth;		/* tile width */
    vikint[vikoff++] = tileHeight;	 	/* tile height */
    for(j=0; j<tileHeight; j++)
    {
	for (i=0; i<tilelen; i++) 
	    vikint[vikoff++] = *bits++;
	if (skip) 
	    bits++; /* skip the last 16 pad bits */
    }

    vforce();

    for(i=1; i<n; i++)
    {
	vikint[ORXPOSN] = x0 = (unsigned short) ppt->x;	/* move to x0 */
	vikint[ORYPOSN] = (unsigned short) ppt->y;	/* move to y0 */
	vikint[ORCLIPHX] = x0 + (unsigned short) *pwidth++ -1 ;
	vikint[VIKCMD] 	= 2; 		/* reprocess orders */ 
	ppt++;
	command(ORDATA);
    }
    clear(2);

    DEALLOCATE_LOCAL(pptFree);
    DEALLOCATE_LOCAL(pwidthFree);
}
