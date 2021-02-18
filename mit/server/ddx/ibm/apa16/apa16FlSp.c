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
/* $XConsortium: apa16FlSp.c,v 1.2 90/03/05 13:53:30 swick Exp $ */
#include "X.h"
#include "Xmd.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "mfb.h"
#include "maskbits.h"

#include "servermd.h"

#include "OScompiler.h"
#include "apa16Hdwr.h"
#include "ibmTrace.h"

/* scanline filling for monochrome frame buffer
   written by drewry, oct 1986
   modified for apa16 by erik, may 1987

   these routines all clip.  they assume that anything that has called
them has already translated the points (i.e. pGC->miTranslate is
non-zero, which is howit gets set in mfbCreateGC().)

   the number of new scnalines created by clipping ==
MaxRectsPerBand * nSpans.

    FillSolid is overloaded to be used for OpaqueStipple as well,
if fgPixel == bgPixel.  


    FillTiled is overloaded to be used for OpaqueStipple, if
fgPixel != bgPixel.  based on the fill style, it uses
{RotatedPixmap, gc.alu} or {RotatedPixmap, PrivGC.ropOpStip}
*/
void
apa16SolidFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
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
    unsigned cmd,startmask;
    int	waitForQueue= TRUE;

    TRACE(("apa16SolidFS(pDrawable= 0x%x, pGC= 0x%x, nInit= %d, pptInit= 0x%x, pwidthInit= 0x%x, fSorted= %d)\n",
			pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted));

    if (!(pGC->planemask & 1))
	return;

    rop = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->rop;
    if (rop==RROP_NOP)	return;

    if (pDrawable->type!=DRAWABLE_WINDOW) {
	if (rop==RROP_WHITE)
	    mfbWhiteSolidFS(pDrawable,pGC,nInit,pptInit,pwidthInit,fSorted);
	else if (rop==RROP_BLACK)
	    mfbBlackSolidFS(pDrawable,pGC,nInit,pptInit,pwidthInit,fSorted);
	else if (rop==RROP_INVERT)
	    mfbInvertSolidFS(pDrawable,pGC,nInit,pptInit,pwidthInit,fSorted);
	return;
    }

    n = nInit * miFindMaxBand(((mfbPrivGC *)
	         (pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip);
    pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
        if(ppt) DEALLOCATE_LOCAL(ppt);
	if(pwidth) DEALLOCATE_LOCAL(pwidth);
	return;
    }
    pwidthFree = pwidth;
    pptFree = ppt;
    n = miClipSpans(((mfbPrivGC *)
		  (pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip,
		     pptInit, pwidthInit, nInit,
		     ppt, pwidth, fSorted);

    APA16_GET_CMD(ROP_NULL_VECTOR,rop,cmd);

    while (n--)
    {
		/* all bits inside same longword */
	if ( ((ppt->x & 0x1f) + *pwidth) < 32) {
	    volatile unsigned int  *pAddr;
	    
	    pAddr= ((volatile unsigned int *)SCREEN_ADDR(ppt->x,ppt->y));
	    maskpartialbits(ppt->x, *pwidth, startmask);

	    if (waitForQueue) {
		waitForQueue= FALSE;
		QUEUE_WAIT();
	    }
	    
	    if		(rop==RROP_WHITE)	*pAddr|= startmask;
	    else if	(rop==RROP_BLACK)	*pAddr&=~startmask;
	    else if	(rop==RROP_INVERT)	*pAddr^= startmask;
    	}
	else if (*pwidth) {
	    int	endx;
	    endx= ppt->x+*pwidth;
	    endx= MIN(endx,APA16_WIDTH-1);
	    DRAW_VECTOR(cmd,ppt->x,ppt->y,endx,ppt->y);
	}
	pwidth++;
	ppt++;
    }
    DEALLOCATE_LOCAL(pptFree);
    DEALLOCATE_LOCAL(pwidthFree);
    return ;
}

/* this works with tiles of width == 32 */
#define FILLSPAN32(ROP) \
    while (n--) \
    { \
	if (*pwidth) \
	{ \
            addrl = addrlBase + (ppt->y * nlwidth) + (ppt->x >> 5); \
	    src = psrc[ppt->y % tileHeight]; \
            if ( ((ppt->x & 0x1f) + *pwidth) < 32) \
            { \
	        maskpartialbits(ppt->x, *pwidth, startmask); \
	        *addrl = (*addrl & ~startmask) | \
		         (ROP(src, *addrl) & startmask); \
            } \
            else \
            { \
	        maskbits(ppt->x, *pwidth, startmask, endmask, nlmiddle); \
	        if (startmask) \
	        { \
	            *addrl = (*addrl & ~startmask) | \
			     (ROP(src, *addrl) & startmask); \
		    addrl++; \
	        } \
	        while (nlmiddle--) \
	        { \
		    *addrl = ROP(src, *addrl); \
		    addrl++; \
	        } \
	        if (endmask) \
	            *addrl = (*addrl & ~endmask) | \
			     (ROP(src, *addrl) & endmask); \
            } \
	} \
	pwidth++; \
	ppt++; \
    }




void
apa16TileFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
DrawablePtr pDrawable;
GC *pGC;
int nInit;			/* number of spans to fill */
DDXPointPtr pptInit;		/* pointer to list of start points */
int *pwidthInit;		/* pointer to list of n widths */
int fSorted;
{
				/* next three parameters are post-clip */
    int n;			/* number of spans to fill */
    register DDXPointPtr ppt;	/* pointer to list of start points */
    register int *pwidth;	/* pointer to list of n widths */
    int *addrlBase;		/* pointer to start of bitmap */
    int nlwidth;		/* width in longwords of bitmap */
    register int *addrl;	/* pointer to current longword in bitmap */
    register int startmask;
    register int endmask;
    register int nlmiddle;
    PixmapPtr pTile;
    int *psrc;
    register int src;
    int tileHeight;
    int rop;
    int *pwidthFree;		/* copies of the pointers to free */
    DDXPointPtr pptFree;

    if (!(pGC->planemask & 1))
	return;

    n = nInit * miFindMaxBand(((mfbPrivGC *)
	      (pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip);
    pwidthFree = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    pptFree = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!pptFree || !pwidthFree)
    {
	if (pptFree) DEALLOCATE_LOCAL(pptFree);
	if (pwidthFree) DEALLOCATE_LOCAL(pwidthFree);
	return;
    }
    pwidth = pwidthFree;
    ppt = pptFree;
    n = miClipSpans(((mfbPrivGC *)
		    (pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip,
		    pptInit, pwidthInit, nInit, 
		    ppt, pwidth, fSorted);

    if (pDrawable->type == DRAWABLE_WINDOW)
    {
	addrlBase = (int *)
	    (((PixmapPtr)(pDrawable->pScreen->devPrivate))->devPrivate.ptr);
	nlwidth = (int)
	    (((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind) >> 2;
	QUEUE_WAIT();
    }
    else
    {
	addrlBase = (int *)(((PixmapPtr)pDrawable)->devPrivate.ptr);
	nlwidth = (int)(((PixmapPtr)pDrawable)->devKind) >> 2;
    }

    pTile = ((mfbPrivGC*)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pRotatedPixmap;
    tileHeight = pTile->drawable.height;
    psrc = (int *)(pTile->devPrivate.ptr);
    if (pGC->fillStyle == FillTiled)
	rop = pGC->alu;
    else
    {
	rop = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->ropOpStip;
    }

    switch(rop)
    {
      case GXclear:
	FILLSPAN32(fnCLEAR)
	break;
      case GXand:
	FILLSPAN32(fnAND)
	break;
      case GXandReverse:
	FILLSPAN32(fnANDREVERSE)
	break;
      case GXcopy:
	FILLSPAN32(fnCOPY)
	break;
      case GXandInverted:
	FILLSPAN32(fnANDINVERTED)
	break;
      case GXnoop:
	break;
      case GXxor:
	FILLSPAN32(fnXOR)
	break;
      case GXor:
	FILLSPAN32(fnOR)
	break;
      case GXnor:
	FILLSPAN32(fnNOR)
	break;
      case GXequiv:
	FILLSPAN32(fnEQUIV)
	break;
      case GXinvert:
	FILLSPAN32(fnINVERT)
	break;
      case GXorReverse:
	FILLSPAN32(fnORREVERSE)
	break;
      case GXcopyInverted:
	FILLSPAN32(fnCOPYINVERTED)
	break;
      case GXorInverted:
	FILLSPAN32(fnORINVERTED)
	break;
      case GXnand:
	FILLSPAN32(fnNAND)
	break;
      case GXset:
	FILLSPAN32(fnSET)
	break;
    }
    DEALLOCATE_LOCAL(pptFree);
    DEALLOCATE_LOCAL(pwidthFree);
    return ;
}



/* Fill spans with tiles that aren't 32 bits wide */
void
apa16UnnaturalTileFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
DrawablePtr pDrawable;
GC		*pGC;
int		nInit;		/* number of spans to fill */
DDXPointPtr pptInit;		/* pointer to list of start points */
int *pwidthInit;		/* pointer to list of n widths */
int fSorted;
{
    int		iline;		/* first line of tile to use */
				/* next three parameters are post-clip */
    int n;			/* number of spans to fill */
    register DDXPointPtr ppt;	/* pointer to list of start points */
    register int *pwidth;	/* pointer to list of n widths */
    unsigned int *addrlBase;	/* pointer to start of bitmap */
    int		 nlwidth;	/* width in longwords of bitmap */
    register unsigned int *pdst;/* pointer to current word in bitmap */
    register unsigned int *psrc;/* pointer to current word in tile */
    register unsigned int startmask;
    register int nlMiddle;
    PixmapPtr	pTile;		/* pointer to tile we want to fill with */
    int		w, width, x, xSrc, ySrc, srcStartOver, nstart, nend;
    int 	tlwidth, rem, tileWidth, tileHeight, endinc, rop;
    unsigned int      tmpSrc, endmask, *psrcT;
    int *pwidthFree;		/* copies of the pointers to free */
    DDXPointPtr pptFree;

    if (!(pGC->planemask & 1))
	return;

    n = nInit *
      miFindMaxBand(((mfbPrivGC *)
		     (pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip);
    pwidthFree = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    pptFree = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!pptFree || !pwidthFree)
    {
	if (pptFree) DEALLOCATE_LOCAL(pptFree);
	if (pwidthFree) DEALLOCATE_LOCAL(pwidthFree);
	return;
    }
    pwidth = pwidthFree;
    ppt = pptFree;
    n = miClipSpans(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip,
		    pptInit, pwidthInit, nInit, 
		    ppt, pwidth, fSorted);

    if (pGC->fillStyle == FillTiled)
    {
	pTile = pGC->tile.pixmap;
	tlwidth = pTile->devKind >> 2;
	rop = pGC->alu;
    }
    else
    {
	pTile = pGC->stipple;
	tlwidth = pTile->devKind >> 2;
	rop = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->ropOpStip;
    }

    if (rop == GXnoop) {
	DEALLOCATE_LOCAL(pptFree);
	DEALLOCATE_LOCAL(pwidthFree);
	return;
      }

    xSrc = pDrawable->x;
    ySrc = pDrawable->y;

    if (pDrawable->type == DRAWABLE_WINDOW)
    {
	addrlBase = (unsigned int *)
		(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devPrivate.ptr);
	nlwidth = (int)
		  (((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind) >> 2;
	QUEUE_WAIT();
    }
    else
    {
	addrlBase = (unsigned int *)(((PixmapPtr)pDrawable)->devPrivate.ptr);
	nlwidth = (int)(((PixmapPtr)pDrawable)->devKind) >> 2;
    }

    tileWidth = pTile->drawable.width;
    tileHeight = pTile->drawable.height;

    /* this replaces rotating the tile. Instead we just adjust the offset
     * at which we start grabbing bits from the tile.
     * Ensure that ppt->x - xSrc >= 0 and ppt->y - ySrc >= 0,
     * so that iline and rem always stay within the tile bounds.
     */
    xSrc += (pGC->patOrg.x % tileWidth) - tileWidth;
    ySrc += (pGC->patOrg.y % tileHeight) - tileHeight;

    while (n--)
    {
	iline = (ppt->y - ySrc) % tileHeight;
        pdst = addrlBase + (ppt->y * nlwidth) + (ppt->x >> 5);
        psrcT = (unsigned int *) pTile->devPrivate.ptr + (iline * tlwidth);
	x = ppt->x;

	if (*pwidth)
	{
	    width = *pwidth;
	    while(width > 0)
	    {
		psrc = psrcT;
	        w = MIN(tileWidth, width);
		if((rem = (x - xSrc)  % tileWidth) != 0)
		{
		    /* if we're in the middle of the tile, get
		       as many bits as will finish the span, or
		       as many as will get to the left edge of the tile,
		       or a longword worth, starting at the appropriate
		       offset in the tile.
		    */
		    w = MIN(MIN(tileWidth - rem, width), BITMAP_SCANLINE_UNIT);
		    endinc = rem / BITMAP_SCANLINE_UNIT;
		    getandputrop((psrc+endinc), (rem&0x1f), (x & 0x1f), w, pdst, rop);
		    if((x & 0x1f) + w >= 0x20)
			pdst++;
		}
		else if(((x & 0x1f) + w) < 32)
		{
		    /* doing < 32 bits is easy, and worth special-casing */
		    putbitsrop(*psrc, x & 0x1f, w, pdst, rop);
		}
		else
		{
		    /* start at the left edge of the tile,
		       and put down as much as we can
		    */
		    maskbits(x, w, startmask, endmask, nlMiddle);

	            if (startmask)
		        nstart = 32 - (x & 0x1f);
	            else
		        nstart = 0;
	            if (endmask)
	                nend = (x + w)  & 0x1f;
	            else
		        nend = 0;

	            srcStartOver = nstart > 31;

		    if(startmask)
		    {
			putbitsrop(*psrc, (x & 0x1f), nstart, pdst, rop);
			pdst++;
			if(srcStartOver)
			    psrc++;
		    }
		     
		    while(nlMiddle--)
		    {
			    getandputrop0(psrc, nstart, 32, pdst, rop);
			    pdst++;
			    psrc++;
		    }
		    if(endmask)
		    {
			getandputrop0(psrc, nstart, nend, pdst, rop);
		    }
		 }
		 x += w;
		 width -= w;
	    }
	}
	ppt++;
	pwidth++;
    }
    DEALLOCATE_LOCAL(pptFree);
    DEALLOCATE_LOCAL(pwidthFree);
}


/* Fill spans with stipples that aren't 32 bits wide */
void
apa16UnnaturalStippleFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
DrawablePtr pDrawable;
GC		*pGC;
int		nInit;		/* number of spans to fill */
DDXPointPtr pptInit;		/* pointer to list of start points */
int *pwidthInit;		/* pointer to list of n widths */
int fSorted;
{
				/* next three parameters are post-clip */
    int n;			/* number of spans to fill */
    register DDXPointPtr ppt;	/* pointer to list of start points */
    register int *pwidth;	/* pointer to list of n widths */
    int		iline;		/* first line of tile to use */
    int		*addrlBase;	/* pointer to start of bitmap */
    int		 nlwidth;	/* width in longwords of bitmap */
    register int *pdst;		/* pointer to current word in bitmap */
    register int *psrc;		/* pointer to current word in tile */
    register int startmask;
    register int nlMiddle;
    PixmapPtr	pTile;		/* pointer to tile we want to fill with */
    int		w, width,  x, xSrc, ySrc, tmpSrc, srcStartOver, nstart, nend;
    int 	endmask, tlwidth, rem, tileWidth, *psrcT, endinc, rop;
    int		tileHeight;
    int *pwidthFree;		/* copies of the pointers to free */
    DDXPointPtr pptFree;

    if (!(pGC->planemask & 1))
	return;

    n = nInit * miFindMaxBand(((mfbPrivGC *)
			       (pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip);
    pwidthFree = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    pptFree = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!pptFree || !pwidthFree)
    {
	if (pptFree) DEALLOCATE_LOCAL(pptFree);
	if (pwidthFree) DEALLOCATE_LOCAL(pwidthFree);
	return;
    }
    pwidth = pwidthFree;
    ppt = pptFree;
    n = miClipSpans(((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->pCompositeClip,
		    pptInit, pwidthInit, nInit, 
		    ppt, pwidth, fSorted);

    pTile = pGC->stipple;
    rop = ((mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr))->rop;
    if (rop == GXnoop) 
      {
	DEALLOCATE_LOCAL(pptFree);
	DEALLOCATE_LOCAL(pwidthFree);
	return;
      }
    tlwidth = pTile->devKind >> 2;
    xSrc = pDrawable->x;
    ySrc = pDrawable->y;
    if (pDrawable->type == DRAWABLE_WINDOW)
    {
	addrlBase = (int *)
		(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devPrivate.ptr);
	nlwidth = (int)
		  (((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind) >> 2;
	QUEUE_WAIT();
    }
    else
    {
	addrlBase = (int *)(((PixmapPtr)pDrawable)->devPrivate.ptr);
	nlwidth = (int)(((PixmapPtr)pDrawable)->devKind) >> 2;
    }

    tileWidth = pTile->drawable.width;
    tileHeight = pTile->drawable.height;

    /* this replaces rotating the stipple.  Instead, we just adjust the offset
     * at which we start grabbing bits from the stipple.
     * Ensure that ppt->x - xSrc >= 0 and ppt->y - ySrc >= 0,
     * so that iline and rem always stay within the tile bounds.
     */
    xSrc += (pGC->patOrg.x % tileWidth) - tileWidth;
    ySrc += (pGC->patOrg.y % tileHeight) - tileHeight;
    while (n--)
    {
	iline = (ppt->y - ySrc) % tileHeight;
        pdst = addrlBase + (ppt->y * nlwidth) + (ppt->x >> 5);
        psrcT = (int *) pTile->devPrivate.ptr + (iline * tlwidth);
	x = ppt->x;

	if (*pwidth)
	{
	    width = *pwidth;
	    while(width > 0)
	    {
		psrc = psrcT;
	        w = MIN(tileWidth, width);
		if((rem = (x - xSrc) % tileWidth) != 0)
		{
		    /* if we're in the middle of the tile, get
		       as many bits as will finish the span, or
		       as many as will get to the left edge of the tile,
		       or a longword worth, starting at the appropriate
		       offset in the tile.
		    */
		    w = MIN(MIN(tileWidth - rem, width), BITMAP_SCANLINE_UNIT);
		    endinc = rem / BITMAP_SCANLINE_UNIT;
		    getandputrrop((psrc + endinc), (rem & 0x1f), (x & 0x1f),
				 w, pdst, rop)
		    if((x & 0x1f) + w >= 0x20)
			pdst++;
		}

		else if(((x & 0x1f) + w) < 32)
		{
		    /* doing < 32 bits is easy, and worth special-casing */
		    putbitsrrop(*psrc, x & 0x1f, w, pdst, rop);
		}
		else
		{
		    /* start at the left edge of the tile,
		       and put down as much as we can
		    */
		    maskbits(x, w, startmask, endmask, nlMiddle);

	            if (startmask)
		        nstart = 32 - (x & 0x1f);
	            else
		        nstart = 0;
	            if (endmask)
	                nend = (x + w)  & 0x1f;
	            else
		        nend = 0;

	            srcStartOver = nstart > 31;

		    if(startmask)
		    {
			putbitsrrop(*psrc, (x & 0x1f), nstart, pdst, rop);
			pdst++;
			if(srcStartOver)
			    psrc++;
		    }
		     
		    while(nlMiddle--)
		    {
			    getandputrrop0(psrc, nstart, 32, pdst, rop);
			    pdst++;
			    psrc++;
		    }
		    if(endmask)
		    {
			getandputrrop0(psrc, nstart, nend, pdst, rop);
		    }
		 }
		 x += w;
		 width -= w;
	    }
	}
	ppt++;
	pwidth++;
    }
    DEALLOCATE_LOCAL(pptFree);
    DEALLOCATE_LOCAL(pwidthFree);
}
