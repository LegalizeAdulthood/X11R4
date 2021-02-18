/* Combined Purdue/PurduePlus patches, level 2.0, 1/17/89 */
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
/***********************************************************
Copyright 1989 by the Massachusetts Institute of Technology

                     All rights reserved.

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of the Massachusetts
Institute of Technology (M.I.T.) not be used in advertising or publicity
pertaining to distribution of the software without specific, written
prior permission.

M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* $XConsortium: mfbbitblt.c,v 5.5 89/07/16 10:45:08 rws Exp $ */
#include "X.h"
#include "Xprotostr.h"

#include "miscstruct.h"
#include "regionstr.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"

#include "mi.h"

#include "mfb.h"
#include "maskbits.h"

#include "OScompiler.h"
#include "apa16Hdwr.h"
#include "ibmTrace.h"

/* CopyArea and CopyPlane for a monchrome frame buffer


    clip the source rectangle to the source's available bits.  (this
avoids copying unnecessary pieces that will just get exposed anyway.)
this becomes the new shape of the destination.
    clip the destination region to the composite clip in the
GC.  this requires translating the destination region to (dstx, dsty).
    build a list of source points, one for each rectangle in the
destination.  this is a simple translation.
    go do the multiple rectangle copies
    do graphics exposures
*/
RegionPtr 
apa16CopyArea(pSrcDrawable, pDstDrawable,
	    			pGC, srcx, srcy, width, height, dstx, dsty)
register DrawablePtr pSrcDrawable;
register DrawablePtr pDstDrawable;
GC *pGC;
int srcx, srcy;
int width, height;
int dstx, dsty;
{
    BoxRec srcBox;
    RegionPtr prgnSrcClip;	/* may be a new region, or just a copy */
    int realSrcClip = 0;	/* non-0 if we've created a src clip */

    RegionPtr prgnExposed;
    RegionRec rgnDst;
    DDXPointPtr pptSrc;
    register DDXPointPtr ppt;
    register BoxPtr pbox;
    int i;
    register int dx;
    register int dy;
    xRectangle origSource;
    DDXPointRec origDest;
    int numRects;

    TRACE(("apa16CopyArea(pSrcDrawable= 0x%x, pDstDrawable= 0x%x, pGC= 0x%x, srcx= %d, srcy= %d, width= %d, height= %d, dstx= %d, dsty= %d)\n",pSrcDrawable,pDstDrawable,pGC,srcx,srcy,width,height,dstx,dsty));

    if ((pSrcDrawable->type!=DRAWABLE_WINDOW)||
	(pDstDrawable->type!=DRAWABLE_WINDOW)) {
	if ((pSrcDrawable->type == DRAWABLE_WINDOW) ||
	    (pDstDrawable->type == DRAWABLE_WINDOW))
	    QUEUE_WAIT();
	return(mfbCopyArea(pSrcDrawable,pDstDrawable,pGC,srcx,srcy,
						width,height,dstx,dsty));
    }

    prgnExposed= NULL;
    origSource.x = srcx;
    origSource.y = srcy;
    origSource.width = width;
    origSource.height = height;
    origDest.x = dstx;
    origDest.y = dsty;

    /*
       clip the left and top edges of the source - Is this still needed? XXX
    */
    if (srcx < 0) {
        width += srcx;
        srcx = 0;
    }
    if (srcy < 0) {
        height += srcy;
        srcy = 0;
    }

    /* clip the source */

    srcx += pSrcDrawable->x;
    srcy += pSrcDrawable->y;
    if (pGC->subWindowMode == IncludeInferiors)
    {
	if ((pSrcDrawable == pDstDrawable) &&
	    (pGC->clientClipType == CT_NONE))
	{
	    prgnSrcClip = ((mfbPrivGC *)(pGC->
					 devPrivates[mfbGCPrivateIndex].ptr))->
					   pCompositeClip;
	}
	else
	{
	    prgnSrcClip = NotClippedByChildren((WindowPtr)pSrcDrawable);
	    realSrcClip = TRUE;
	}
    }
    else
    {
	prgnSrcClip = &((WindowPtr)pSrcDrawable)->clipList;
    }

    srcBox.x1 = srcx;
    srcBox.y1 = srcy;
    srcBox.x2 = srcx + width;
    srcBox.y2 = srcy + height;

    (*pGC->pScreen->RegionInit)(&rgnDst, &srcBox, 1);
    (*pGC->pScreen->Intersect)(&rgnDst, &rgnDst, prgnSrcClip);

    if (!((WindowPtr)pDstDrawable)->realized) {
	goto freeRegions;
    }
    dstx += pDstDrawable->x;
    dsty += pDstDrawable->y;

    dx = srcx - dstx;
    dy = srcy - dsty;

    /* clip the shape of the dst to the destination composite clip */
    (*pGC->pScreen->TranslateRegion)(&rgnDst, -dx, -dy);
    (*pGC->pScreen->Intersect)(&rgnDst,
		&rgnDst,
		((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->
			       pCompositeClip);

    numRects = REGION_NUM_RECTS(&rgnDst);
    if (!numRects)
    {
	goto freeRegions;
    }
    if(!(pptSrc = (DDXPointPtr)ALLOCATE_LOCAL( numRects *
					      sizeof(DDXPointRec))))
      {
	goto freeRegions;
      }
    pbox = REGION_RECTS(&rgnDst);
    ppt = pptSrc;
    for (i=numRects; --i >= 0; pbox++, ppt++)
    {
	ppt->x = pbox->x1 + dx;
	ppt->y = pbox->y1 + dy;
    }

    if (pGC->planemask&1)
	apa16DoBitblt(pSrcDrawable, pDstDrawable, pGC->alu, &rgnDst, pptSrc);

    if (((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->fExpose) {
        prgnExposed= miHandleExposures(pSrcDrawable, pDstDrawable, pGC,
		          	origSource.x, origSource.y,
		          	(int) origSource.width, (int) origSource.height,
                                origDest.x, origDest.y, (unsigned long) 0);
    }
		
    DEALLOCATE_LOCAL(pptSrc);
freeRegions:
    (*pGC->pScreen->RegionUninit)(&rgnDst);
    if (realSrcClip)
      (*pGC->pScreen->RegionDestroy)(prgnSrcClip);
    return(prgnExposed);
}


/*
 * Macro to help make apa16DoBitblt comprehensible.
 */

#define	RECTS_LEFT()	nbox--

#define	SET_UP_FOR_BLT() \
	w= pbox->x2-pbox->x1;\
	h= pbox->y2-pbox->y1;\
	pptSrc->x+= w;\
	pptSrc->y+= h;

#define	NEXT_RECT() \
	pbox++;\
	pptSrc++;

/* 
 *DoBitblt() does multiple rectangle moves into the rectangles
 */

apa16DoBitblt(pSrcDrawable, pDstDrawable, alu, prgnDst, pptSrc)
DrawablePtr pSrcDrawable;
DrawablePtr pDstDrawable;
int alu;
RegionPtr 	 prgnDst;
register DDXPointPtr pptSrc;
{
    int widthSrc, widthDst;	/* add to get to same position in next line */

    register BoxPtr pbox;
    int nbox;

    BoxPtr pboxTmp, pboxNext, pboxBase, pboxNew1, pboxNew2;
				/* temporaries for shuffling rectangles */
    DDXPointPtr pptTmp, pptNew1,pptNew2;
				/* shuffling boxes entails shuffling the
				   source points too */

    register unsigned 	cmd;	/* apa16 rasterop command word */

    TRACE(("apa16DoBitblt(pSrcDrawable= 0x%x, pDstDrawable= 0%x, alu= 0x%x, prgnDst= 0x%x, pptSrc= 0x%x)\n",pSrcDrawable,pDstDrawable,alu,prgnDst,pptSrc));

    if ((pSrcDrawable->type != DRAWABLE_WINDOW)||
	(pDstDrawable->type != DRAWABLE_WINDOW)||
        (alu==GXorReverse)||(alu==GXequiv)) {
        if ((pSrcDrawable->type == DRAWABLE_WINDOW) ||
	    (pDstDrawable->type == DRAWABLE_WINDOW))
	    QUEUE_WAIT();
	mfbDoBitblt(pSrcDrawable,pDstDrawable,alu,prgnDst,pptSrc);
	return;
    }
    if (alu==GXnoop)	return;

    widthSrc = (int)
		((PixmapPtr)(pSrcDrawable->pScreen->devPrivate))->devKind>>2;
    widthDst = (int)
		((PixmapPtr)(pDstDrawable->pScreen->devPrivate))->devKind>>2;

    pbox = REGION_RECTS(prgnDst);
    nbox = REGION_NUM_RECTS(prgnDst);

    pboxNew1 = NULL;
    pptNew1 = NULL;
    pboxNew2 = NULL;
    pptNew2 = NULL;
    if (pptSrc->y < pbox->y1) 
    {
        /* walk source botttom to top */
	widthSrc = -widthSrc;
	widthDst = -widthDst;

	if (nbox > 1)
	{
	    /* keep ordering in each band, reverse order of bands */
	    pboxNew1 = (BoxPtr)ALLOCATE_LOCAL(sizeof(BoxRec) * nbox);
	    if (!pboxNew1)	
		return;
	    pptNew1 = (DDXPointPtr)ALLOCATE_LOCAL(sizeof(DDXPointRec) * nbox);
	    if(!pptNew1)
	    {
	        DEALLOCATE_LOCAL(pboxNew1);
	        return;
	    }
	    pboxBase = pboxNext = pbox+nbox-1;
	    while (pboxBase >= pbox)
	    {
	        while ((pboxNext >= pbox) && 
		       (pboxBase->y1 == pboxNext->y1))
		    pboxNext--;
	        pboxTmp = pboxNext+1;
	        pptTmp = pptSrc + (pboxTmp - pbox);
	        while (pboxTmp <= pboxBase)
	        {
		    *pboxNew1++ = *pboxTmp++;
		    *pptNew1++ = *pptTmp++;
	        }
	        pboxBase = pboxNext;
	    }
	    pboxNew1 -= nbox;
	    pbox = pboxNew1;
	    pptNew1 -= nbox;
	    pptSrc = pptNew1;
        }
    }

    if (pptSrc->x < pbox->x1)
    {
	if (nbox > 1)
	{
	    /* reverse order of rects in each band */
	    pboxNew2 = (BoxPtr)ALLOCATE_LOCAL(sizeof(BoxRec) * nbox);
	    pptNew2 = (DDXPointPtr)ALLOCATE_LOCAL(sizeof(DDXPointRec) * nbox);
	    pboxBase = pboxNext = pbox;
	    if(!pboxNew2 || !pptNew2)
	    {
	        if (pptNew2) DEALLOCATE_LOCAL(pptNew2);
	        if (pboxNew2) DEALLOCATE_LOCAL(pboxNew2);
		if (pboxNew1)
		{
		    DEALLOCATE_LOCAL(pptNew1);
		    DEALLOCATE_LOCAL(pboxNew1);
		}
	        return;
	    }
	    while (pboxBase < pbox+nbox)
	    {
	        while ((pboxNext < pbox+nbox) &&
		       (pboxNext->y1 == pboxBase->y1))
		    pboxNext++;
	        pboxTmp = pboxNext;
	        pptTmp = pptSrc + (pboxTmp - pbox);
	        while (pboxTmp != pboxBase)
	        {
		    *(pboxNew2++) = *(--pboxTmp);
		    *(pptNew2++) = *(--pptTmp);
	        }
	        pboxBase = pboxNext;
	    }
	    pboxNew2 -= nbox;
	    pbox = pboxNew2;
	    pptNew2 -= nbox;
	    pptSrc = pptNew2;
	}
    }

    APA16_GET_CMD(ROP_RECT_COPY,alu,cmd);
    if (cmd&STYPE_MASK) { /* hardware can handle copy */
	while (RECTS_LEFT()) {
	    register int w,h;
	    SET_UP_FOR_BLT();
	    COPY_RECT(cmd,pbox->x2,pbox->y2,pptSrc->x,pptSrc->y,w,h);
	    NEXT_RECT();
	}
    }
    else { /* special case, lend a hand */
	switch (alu) {
	    case GXcopyInverted: /* copy and invert. simple, no? */
		{ register unsigned cmd2;
		APA16_GET_CMD(ROP_RECT_COPY,GXcopy,cmd);
		APA16_GET_CMD(ROP_RECT_FILL,GXinvert,cmd2);
		while (RECTS_LEFT()) {
		    register int w,h;
		    SET_UP_FOR_BLT();
		    COPY_RECT(cmd,pbox->x2,pbox->y2,pptSrc->x,pptSrc->y,w,h);
		    FILL_RECT(cmd2,pbox->x2,pbox->y2,w,h);
		    NEXT_RECT();
		}
		break;
		}
			/* really rectangle fill! */
	    case GXclear:
	    case GXinvert:
	    case GXset:	
		APA16_GET_CMD(ROP_RECT_FILL,alu,cmd);
		while (RECTS_LEFT()) {
		    register int w,h;
		    SET_UP_FOR_BLT();
		    FILL_RECT(cmd,pbox->x2,pbox->y2,w,h);
		    NEXT_RECT();
		}
		break;
       }
    }
    if (pboxNew2)
    {
	DEALLOCATE_LOCAL(pptNew2);
	DEALLOCATE_LOCAL(pboxNew2);
    }
    if (pboxNew1)
    {
	DEALLOCATE_LOCAL(pptNew1);
	DEALLOCATE_LOCAL(pboxNew1);
    }
}


/*
    if fg == 1 and bg ==0, we can do an ordinary CopyArea.
    if fg == bg, we can do a CopyArea with alu = mfbReduceRop(alu, fg)
    if fg == 0 and bg == 1, we use the same rasterop, with
	source operand inverted.

    CopyArea deals with all of the graphics exposure events.
    This code depends on knowing that we can change the
alu in the GC without having to call ValidateGC() before calling
CopyArea().

*/

RegionPtr
apa16CopyPlane(pSrcDrawable, pDstDrawable,
	       pGC, srcx, srcy, width, height, dstx, dsty, plane)
DrawablePtr pSrcDrawable, pDstDrawable;
register GC *pGC;
int srcx, srcy;
int width, height;
int dstx, dsty;
unsigned long plane;
{
    int alu;
    RegionPtr	prgnExposed;

    /* XXX a deeper screen ought to wrap ValidateGC to get around this */
    if (pSrcDrawable->depth != 1)
	return miCopyPlane(pSrcDrawable, pDstDrawable,
			   pGC, srcx, srcy, width, height, dstx, dsty, plane);
    if (plane != 1)
	return NULL;

    if ((pGC->fgPixel == 1) && (pGC->bgPixel == 0))
    {
	prgnExposed = (*pGC->ops->CopyArea)(pSrcDrawable, pDstDrawable,
			 pGC, srcx, srcy, width, height, dstx, dsty);
    }
    else if (pGC->fgPixel == pGC->bgPixel)
    {
	alu = pGC->alu;
	pGC->alu = apa16ReduceRop(pGC->alu, pGC->fgPixel);
	prgnExposed = (*pGC->ops->CopyArea)(pSrcDrawable, pDstDrawable,
			 pGC, srcx, srcy, width, height, dstx, dsty);
	pGC->alu = alu;
    }
    else /* need to invert the src */
    {
	alu = pGC->alu;
	pGC->alu = InverseAlu[alu];
	prgnExposed = (*pGC->ops->CopyArea)(pSrcDrawable, pDstDrawable,
			 pGC, srcx, srcy, width, height, dstx, dsty);
	pGC->alu = alu;
    }
    return prgnExposed;
}





