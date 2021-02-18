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
/* $XConsortium: mfbgc.c,v 5.6 89/07/19 09:30:40 rws Exp $ */
#include "X.h"
#include "Xmd.h"
#include "Xproto.h"
#include "dixfontstr.h"
#include "fontstruct.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "region.h"

#include "mfb.h"
#include "mistruct.h"

#include "maskbits.h"

#include "ibmTrace.h"
#include "apa16Hdwr.h"
#include "apa16Decls.h"


static void
	apa16DestroyOps(),

	apa16ValidateGC(),
	apa16ChangeGC(),
	apa16CopyGC(),
	apa16DestroyGC(),
	apa16ChangeClip(),
	apa16DestroyClip(),
	apa16CopyClip();




static GCFuncs	mfbFuncs = {
	apa16ValidateGC,
	apa16ChangeGC,
	apa16CopyGC,
	apa16DestroyGC,
	apa16ChangeClip,
	apa16DestroyClip,
	apa16CopyClip,
};

static GCOps	whiteTECopyOps = {
	apa16SolidFS,
	apa16SetSpans,
	apa16PutImage,
	apa16CopyArea,
	apa16CopyPlane,
	apa16PolyPoint,
 	apa16LineSS,
	apa16PolySegment,
	miPolyRectangle,
	apa16ZeroPolyArcSS,
	miFillPolygon,
	mfbPolyFillRect,
	apa16PolyFillArcSolid,
	apa16PolyText8,
	apa16PolyText16,
	apa16ImageText8,
	apa16ImageText16,
	apa16ImageGlyphBlt,
	apa16PolyGlyphBlt,
	apa16PushPixels,
	NULL,
};

static GCOps	blackTECopyOps = {
	apa16SolidFS,
	apa16SetSpans,
	apa16PutImage,
	apa16CopyArea,
	apa16CopyPlane,
	apa16PolyPoint,
	apa16LineSS,
	apa16PolySegment,
	miPolyRectangle,
	apa16ZeroPolyArcSS,
	miFillPolygon,
	mfbPolyFillRect,
	apa16PolyFillArcSolid,
	apa16PolyText8,
	apa16PolyText16,
	apa16ImageText8,
	apa16ImageText16,
	apa16ImageGlyphBlt,
	apa16PolyGlyphBlt,
	apa16PushPixels,
	NULL,
};

static GCOps	invertWhiteTECopyOps = {
	apa16SolidFS,
	apa16SetSpans,
	apa16PutImage,
	apa16CopyArea,
	apa16CopyPlane,
	apa16PolyPoint,
	apa16LineSS,
	apa16PolySegment,
	miPolyRectangle,
	apa16ZeroPolyArcSS,
	miFillPolygon,
	mfbPolyFillRect,
	apa16PolyFillArcSolid,
	apa16PolyText8,
	apa16PolyText16,
	apa16ImageText8,
	apa16ImageText16,
	apa16ImageGlyphBlt,
	apa16PolyGlyphBlt,
	apa16PushPixels,
	NULL,
};

static GCOps	invertBlackTECopyOps = {
	apa16SolidFS,
	apa16SetSpans,
	apa16PutImage,
	apa16CopyArea,
	apa16CopyPlane,
	apa16PolyPoint,
	apa16LineSS,
	apa16PolySegment,
	miPolyRectangle,
	apa16ZeroPolyArcSS,
	miFillPolygon,
	mfbPolyFillRect,
	apa16PolyFillArcSolid,
	apa16PolyText8,
	apa16PolyText16,
	apa16ImageText8,
	apa16ImageText16,
	apa16ImageGlyphBlt,
	apa16PolyGlyphBlt,
	apa16PushPixels,
	NULL,
};

static GCOps	whiteCopyOps = {
	apa16SolidFS,
	apa16SetSpans,
	apa16PutImage,
	apa16CopyArea,
	apa16CopyPlane,
	apa16PolyPoint,
	apa16LineSS,
	apa16PolySegment,
	miPolyRectangle,
	apa16ZeroPolyArcSS,
	miFillPolygon,
	mfbPolyFillRect,
	apa16PolyFillArcSolid,
	apa16PolyText8,
	apa16PolyText16,
	apa16ImageText8,
	apa16ImageText16,
	apa16ImageGlyphBlt,
	apa16PolyGlyphBlt,
	apa16PushPixels,
	NULL,
};

static GCOps	blackCopyOps = {
	apa16SolidFS,
	apa16SetSpans,
	apa16PutImage,
	apa16CopyArea,
	apa16CopyPlane,
	apa16PolyPoint,
	apa16LineSS,
	apa16PolySegment,
	miPolyRectangle,
	apa16ZeroPolyArcSS,
	miFillPolygon,
	mfbPolyFillRect,
	apa16PolyFillArcSolid,
	apa16PolyText8,
	apa16PolyText16,
	apa16ImageText8,
	apa16ImageText16,
	apa16ImageGlyphBlt,
	apa16PolyGlyphBlt,
	apa16PushPixels,
	NULL,
};

static GCOps	invertWhiteCopyOps = {
	apa16SolidFS,
	apa16SetSpans,
	apa16PutImage,
	apa16CopyArea,
	apa16CopyPlane,
	apa16PolyPoint,
	apa16LineSS,
	apa16PolySegment,
	miPolyRectangle,
	apa16ZeroPolyArcSS,
	miFillPolygon,
	mfbPolyFillRect,
	apa16PolyFillArcSolid,
	apa16PolyText8,
	apa16PolyText16,
	apa16ImageText8,
	apa16ImageText16,
	apa16ImageGlyphBlt,
	apa16PolyGlyphBlt,
	apa16PushPixels,
	NULL,
};

static GCOps	invertBlackCopyOps = {
	apa16SolidFS,
	apa16SetSpans,
	apa16PutImage,
	apa16CopyArea,
	apa16CopyPlane,
	apa16PolyPoint,
	apa16LineSS,
	apa16PolySegment,
	miPolyRectangle,
	apa16ZeroPolyArcSS,
	miFillPolygon,
	mfbPolyFillRect,
	apa16PolyFillArcSolid,
	apa16PolyText8,
	apa16PolyText16,
	apa16ImageText8,
	apa16ImageText16,
	apa16ImageGlyphBlt,
	apa16PolyGlyphBlt,
	apa16PushPixels,
	NULL,

};

struct commonOps {
    int		    fg, bg;
    int		    rrop;
    int		    terminalFont;
    GCOps	    *ops;
    void	    (*fillArea)();
};

static struct commonOps mfbCommonOps[] = {
    { 1, 0, RROP_WHITE, 1, &whiteTECopyOps, apa16SolidFillArea },
    { 0, 1, RROP_BLACK, 1, &blackTECopyOps, apa16SolidFillArea },
    { 1, 0, RROP_INVERT, 1, &invertWhiteTECopyOps, apa16SolidFillArea },
    { 1, 0, RROP_INVERT, 1, &invertBlackTECopyOps, apa16SolidFillArea },
    { 1, 0, RROP_WHITE, 0, &whiteCopyOps, apa16SolidFillArea },
    { 0, 1, RROP_BLACK, 0, &blackCopyOps, apa16SolidFillArea },
    { 1, 0, RROP_INVERT, 0, &invertWhiteCopyOps, apa16SolidFillArea },
    { 0, 1, RROP_INVERT, 0, &invertBlackCopyOps, apa16SolidFillArea },
};

#define numberCommonOps	(sizeof (mfbCommonOps) / sizeof (mfbCommonOps[0]))

static GCOps *
matchCommon (pGC)
    GCPtr   pGC;
{
    int	i;
    struct commonOps	*cop;
    mfbPrivGC		*priv;

    if (pGC->lineWidth != 0)
	return 0;
    if (pGC->lineStyle != LineSolid)
	return 0;
    if (pGC->fillStyle != FillSolid)
	return 0;
    if (!pGC->font ||
        pGC->font->pFI->maxbounds.metrics.rightSideBearing -
	pGC->font->pFI->minbounds.metrics.leftSideBearing > 32)
	return 0;
    for (i = 0; i < numberCommonOps; i++) {
	cop = &mfbCommonOps[i];
	if (pGC->fgPixel != cop->fg)
	    continue;
	if (pGC->bgPixel != cop->bg)
	    continue;
	priv = (mfbPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr;
	if (priv->rop != cop->rrop)
	    continue;
	if (cop->terminalFont != pGC->font->pFI->terminalFont)
	    continue;
	priv->FillArea = cop->fillArea;
	return cop->ops;
    }
    return 0;
}

Bool
apa16CreateGC(pGC)
    register GCPtr pGC;
{
    mfbPrivGC 	*pPriv;

    TRACE(("apa16CreateGC( pGC= 0x%x)\n",pGC));

    pGC->clientClip = NULL;
    pGC->clientClipType = CT_NONE;
    
    /* some of the output primitives aren't really necessary, since
       they will be filled in validategc because of dix/creategc()
       setting all the change bits.  others are necessary because although
       they depend on being a monochrome frame buffer, they don't change 
    */

    pGC->ops = &whiteCopyOps;
    pGC->funcs = &mfbFuncs;

    /* mfb wants to translate before scan convesion */
    pGC->miTranslate = 1;

    /* xxx -- warning: the next line is "special" */
    pPriv = (mfbPrivGC *) (pGC->devPrivates[mfbGCPrivateIndex].ptr);
    pPriv->rop = apa16ReduceRop(pGC->alu, pGC->fgPixel);
    pPriv->fExpose = TRUE;
    pPriv->pRotatedPixmap = NullPixmap;

    pPriv->freeCompClip = FALSE;
    pPriv->FillArea = apa16SolidFillArea;
    return TRUE;
}

/*ARGSUSED*/
static void
apa16ChangeGC(pGC, mask)
    GC	    *pGC;
    BITS32  mask;
{
    return;
}

/*ARGSUSED*/
static void
apa16CopyGC (pGCSrc, changes, pGCDst)
    GCPtr	pGCSrc;
    Mask 	changes;
    GCPtr	pGCDst;
{
    RegionPtr		pClip;

    if(changes & GCClipMask)
    {
	if(pGCDst->clientClipType == CT_PIXMAP)
	{
	    ((PixmapPtr)pGCDst->clientClip)->refcnt++;
	}
	else if(pGCDst->clientClipType == CT_REGION)
	{
	    pClip = (RegionPtr) pGCDst->clientClip;
	    pGCDst->clientClip =
	        (pointer)(* pGCDst->pScreen->RegionCreate)(NULL, 1);
	    (* pGCDst->pScreen->RegionCopy)(pGCDst->clientClip, pClip);
	}
    }
}

static void
apa16DestroyGC(pGC)
    GC 			*pGC;
{
    mfbPrivGC *pPriv;

    pPriv = (mfbPrivGC *)(pGC->devPrivates[mfbGCPrivateIndex].ptr);
    if (pPriv->pRotatedPixmap)
	mfbDestroyPixmap(pPriv->pRotatedPixmap);
    if (pPriv->freeCompClip)
	(*pGC->pScreen->RegionDestroy)(pPriv->pCompositeClip);
    apa16DestroyOps (pGC->ops);
}

/*
 * create a private op array for a gc
 */

static GCOps *
apa16CreateOps (prototype)
    GCOps	*prototype;
{
    GCOps	*ret;
    extern Bool	Must_have_memory;

    /* XXX */ Must_have_memory = TRUE;
    ret = (GCOps *) xalloc (sizeof *ret);
    /* XXX */ Must_have_memory = FALSE;
    if (!ret)
	return 0;
    *ret = *prototype;
    ret->devPrivate.val = 1;
    return ret;
}

static void
apa16DestroyOps (ops)
    GCOps   *ops;
{
    if (ops->devPrivate.val)
	xfree (ops);
}

/* Clipping conventions
	if the drawable is a window
	    CT_REGION ==> pCompositeClip really is the composite
	    CT_other ==> pCompositeClip is the window clip region
	if the drawable is a pixmap
	    CT_REGION ==> pCompositeClip is the translated client region
		clipped to the pixmap boundary
	    CT_other ==> pCompositeClip is the pixmap bounding box
*/

/*ARGSUSED*/
static void
apa16ValidateGC(pGC, changes, pDrawable)
    register GCPtr 	pGC;
    Mask 		changes;
    DrawablePtr 	pDrawable;
{
    register mfbPrivGCPtr	devPriv;
    WindowPtr pWin;
    int mask;			/* stateChanges */
    int index;			/* used for stepping through bitfields */
    int	xrot, yrot;		/* rotations for tile and stipple pattern */
    int rrop;			/* reduced rasterop */
				/* flags for changing the proc vector 
				   and updating things in devPriv
				*/
    int new_rotate, new_rrop,  new_line, new_text, new_fill;
    DDXPointRec	oldOrg;		/* origin of thing GC was last used with */

    TRACE(("apa16ValidateGC(pGC=0x%x,changes=0x%x,pDrawable=0x%x)\n",
						pGC,changes,pDrawable));

    oldOrg = pGC->lastWinOrg;

    pGC->lastWinOrg.x = pDrawable->x;
    pGC->lastWinOrg.y = pDrawable->y;
    if (pDrawable->type == DRAWABLE_WINDOW)
	pWin = (WindowPtr)pDrawable;
    else
	pWin = (WindowPtr)NULL;

    /* we need to re-rotate the tile if the previous window/pixmap
       origin (oldOrg) differs from the new window/pixmap origin
       (pGC->lastWinOrg)
    */
    new_rotate = (oldOrg.x != pGC->lastWinOrg.x) ||
		 (oldOrg.y != pGC->lastWinOrg.y);

    /* XXX -- hewre is more of the index stuff! */
    devPriv = ((mfbPrivGCPtr) (pGC->devPrivates[mfbGCPrivateIndex].ptr));

    /*
	if the client clip is different or moved OR
	the subwindowMode has changed OR
	the window's clip has changed since the last validation
	we need to recompute the composite clip
    */
    if ((changes & (GCClipXOrigin|GCClipYOrigin|GCClipMask)) ||
	(changes & GCSubwindowMode) ||
	(pDrawable->serialNumber != (pGC->serialNumber & DRAWABLE_SERIAL_BITS))
       )
    {
	ScreenPtr pScreen = pGC->pScreen;

	if (pWin)
	{
	    Bool freeTmpClip, freeCompClip;
	    RegionPtr pregWin;		/* clip for this window, without
					   client clip */

	    if (pGC->subWindowMode == IncludeInferiors)
	    {
	        pregWin = NotClippedByChildren(pWin);
		freeTmpClip = TRUE;
	    }
	    else
	    {
	        pregWin = &pWin->clipList;
		freeTmpClip = FALSE;
	    }
	    freeCompClip = devPriv->freeCompClip;

	    /* if there is no client clip, we can get by with
	       just keeping the pointer we got, and remembering
	       whether or not should destroy (or maybe re-use)
	       it later.  this way, we avoid unnecessary copying
	       of regions.  (this wins especially if many clients clip
	       by children and have no client clip.)
	    */
	    if (pGC->clientClipType == CT_NONE)
	    {
	        if(freeCompClip) 
		    (*pScreen->RegionDestroy) (devPriv->pCompositeClip);
	        devPriv->pCompositeClip = pregWin;
	        devPriv->freeCompClip = freeTmpClip;
	    }
	    else
	    {
		/* we need one 'real' region to put into the composite
		   clip.
			if pregWin and the current composite clip 
		   are real, we can get rid of one.
			if the current composite clip is real and
		   pregWin isn't, intersect the client clip and
		   pregWin into the existing composite clip.
			if pregWin is real and the current composite
		   clip isn't, intersect pregWin with the client clip
		   and replace the composite clip with it.
			if neither is real, create a new region and
		   do the intersection into it.
		*/

		(*pScreen->TranslateRegion)(pGC->clientClip,
					    pDrawable->x + pGC->clipOrg.x,
					    pDrawable->y + pGC->clipOrg.y);
						  
		if (freeCompClip)
		{
		    (*pScreen->Intersect)(devPriv->pCompositeClip,
					  pregWin, pGC->clientClip);
		    if (freeTmpClip)
			(*pScreen->RegionDestroy)(pregWin);
		}
		else if (freeTmpClip)
		{
		    (*pScreen->Intersect)(pregWin, pregWin, pGC->clientClip);
		    devPriv->pCompositeClip = pregWin;
		}
		else
		{
		    devPriv->pCompositeClip = (*pScreen->RegionCreate)(NullBox,
								       0);
		    (*pScreen->Intersect)(devPriv->pCompositeClip,
					  pregWin, pGC->clientClip);
		}
		devPriv->freeCompClip = TRUE;
		(*pScreen->TranslateRegion)(pGC->clientClip,
					    -(pDrawable->x + pGC->clipOrg.x),
					    -(pDrawable->y + pGC->clipOrg.y));
						  
	    }
	} /* end of composite clip for a window */
	else
	{
	    BoxRec pixbounds;

	    pixbounds.x1 = 0;
	    pixbounds.y1 = 0;
	    pixbounds.x2 = pDrawable->width;
	    pixbounds.y2 = pDrawable->height;

	    if (devPriv->freeCompClip)
	        (*pScreen->RegionReset)(devPriv->pCompositeClip, &pixbounds);
	    else
	    {
		devPriv->freeCompClip = TRUE;
		devPriv->pCompositeClip = (*pScreen->RegionCreate)(&pixbounds,
								   1);
	    }

	    if (pGC->clientClipType == CT_REGION)
		(*pScreen->Intersect)(devPriv->pCompositeClip, 
				      devPriv->pCompositeClip,
				      pGC->clientClip);
	} /* end of composite clip for pixmap */
    }

    new_rrop = FALSE;
    new_line = FALSE;
    new_text = FALSE;
    new_fill = FALSE;

    mask = changes;
    mask &= ~(GCPlaneMask | GCCapStyle | GCFillRule |
	      GCSubwindowMode | GCGraphicsExposures | GCClipXOrigin |
	      GCClipYOrigin | GCClipMask | GCDashOffset | GCDashList |
	      GCArcMode);
    while (mask)
    {
	index = lowbit (mask);
	mask &= ~index;

	/* this switch acculmulates a list of which procedures
	   might have to change due to changes in the GC.  in
	   some cases (e.g. changing one 16 bit tile for another)
	   we might not really need a change, but the code is
	   being paranoid.
	   this sort of batching wins if, for example, the alu
	   and the font have been changed, or any other pair
	   of items that both change the same thing.
	*/
	switch (index)
	{
	  case GCFunction:
	  case GCForeground:
	    new_rrop = TRUE;
	    break;
	  case GCBackground:
	    new_rrop = TRUE;	/* for opaque stipples */
	    break;
	  case GCLineStyle:
	  case GCLineWidth:
	  case GCJoinStyle:
	    new_line = TRUE;
	    break;
	  case GCFillStyle:
	    new_fill = TRUE;
	    break;
	  case GCTile:
	    if(pGC->tileIsPixel)
		break;
	    new_rotate = TRUE;
	    new_fill = TRUE;
	    break;

	  case GCStipple:
	    if(pGC->stipple == (PixmapPtr)NULL)
		break;
	    new_rotate = TRUE;
	    new_fill = TRUE;
	    break;

	  case GCTileStipXOrigin:
	    new_rotate = TRUE;
	    break;

	  case GCTileStipYOrigin:
	    new_rotate = TRUE;
	    break;

	  case GCFont:
	    new_text = TRUE;
	    break;
	  default:
	    break;
	}
    }

    /* deal with the changes we've collected .
       new_rrop must be done first because subsequent things
       depend on it.
    */

    if(new_rotate || new_fill)
    {
      extern unsigned int *apa16CurrentTileBits;
      Bool new_pix = FALSE;

	/* figure out how much to rotate */
	xrot = pGC->patOrg.x;
	yrot = pGC->patOrg.y;
	xrot += pDrawable->x;
	yrot += pDrawable->y;

	switch (pGC->fillStyle)
	{
	case FillTiled:
	    /* copy current tile and stipple */
	    if (!pGC->tileIsPixel && (pGC->tile.pixmap->drawable.width <= 32) &&
	    	!(pGC->tile.pixmap->drawable.width & (pGC->tile.pixmap->drawable.width - 1)))
	    {
	    	mfbCopyRotatePixmap(pGC->tile.pixmap, 
				    &devPriv->pRotatedPixmap, xrot, yrot);
		new_pix = TRUE;
	    }
	    break;
	case FillStippled:
	case FillOpaqueStippled:
	    if (pGC->stipple && (pGC->stipple->drawable.width <= 32) &&
	    	!(pGC->stipple->drawable.width & (pGC->stipple->drawable.width - 1)))
	    {
	    	if (pGC->stipple == pGC->pScreen->PixmapPerDepth[0])
	    	{
		    if (pGC->stipple->drawable.width != 32)
			mfbPadPixmap(pGC->stipple);
		    if (devPriv->pRotatedPixmap)
		        mfbDestroyPixmap(devPriv->pRotatedPixmap);
		    devPriv->pRotatedPixmap = pGC->stipple;
		    ++devPriv->pRotatedPixmap->refcnt;
	    	}
	    	else
	    	{
		    mfbCopyRotatePixmap(pGC->stipple,
					&devPriv->pRotatedPixmap, xrot, yrot);
	    	}
		new_pix = TRUE;
	    }
	}
/* Destroy any previously rotate tile or stipple */
      if (!new_pix && devPriv->pRotatedPixmap) 
	{
	  mfbDestroyPixmap(devPriv->pRotatedPixmap);
	  devPriv->pRotatedPixmap = (PixmapPtr)NULL;
	}
/* For an apa16 - if we are destroying old rotate pixmap or have a new, 
   clear the off screen cache 
 */
      apa16CurrentTileBits=	NULL;
    }

    /*
     * duck out here when the GC is unchanged
     */

    if (!changes)
	return;

    if (new_rrop || new_fill)
    {
	rrop = apa16ReduceRop(pGC->alu, pGC->fgPixel);
	devPriv->rop = rrop;
	new_fill = TRUE;
	/* FillArea raster op is GC's for tile filling,
	   and the reduced rop for solid and stipple
	*/
	if (pGC->fillStyle == FillTiled)
	    devPriv->ropFillArea = pGC->alu;
	else
	    devPriv->ropFillArea = rrop;

	/* opaque stipples:
	   fg	bg	ropOpStip	fill style
	   1	0	alu		tile
	   0	1	inverseAlu	tile
	   1	1	rrop(fg, alu)	solid
	   0	0	rrop(fg, alu)	solid
	Note that rrop(fg, alu) == mfbPrivGC.rop, so we don't really need to
	compute it.
	*/
        if (pGC->fillStyle == FillOpaqueStippled)
        {
	    if (pGC->fgPixel != pGC->bgPixel)
	    {
	        if (pGC->fgPixel)
		    devPriv->ropOpStip = pGC->alu;
	        else
		    devPriv->ropOpStip = InverseAlu[pGC->alu];
	    }
	    else
	        devPriv->ropOpStip = rrop;
	    devPriv->ropFillArea = devPriv->ropOpStip;
        }
    }
    else
	rrop = devPriv->rop;

    if (new_line || new_fill || new_text)
    {
	GCOps	*newops;

	if (newops = matchCommon (pGC))
 	{
	    if (pGC->ops->devPrivate.val)
		apa16DestroyOps (pGC->ops);
	    pGC->ops = newops;
	    new_line = new_fill = new_text = 0;
	}
 	else
 	{
	    if (!pGC->ops->devPrivate.val)
 	    {
		pGC->ops = apa16CreateOps (pGC->ops);
		pGC->ops->devPrivate.val = 1;
	    }
	}
    }

    if (new_line || new_fill)
    {
        if (pGC->lineWidth == 0)
        {
            if ((pGC->lineStyle == LineSolid) && (pGC->fillStyle == FillSolid)
                && ((rrop == RROP_WHITE) || (rrop == RROP_BLACK)))
                pGC->ops->PolyArc = apa16ZeroPolyArcSS;
            else
                pGC->ops->PolyArc = miZeroPolyArc;
        }
        else
            pGC->ops->PolyArc = miPolyArc;
	if (pGC->lineStyle == LineSolid)
	{
	    if(pGC->lineWidth == 0)
	    {
	        if (pGC->fillStyle == FillSolid)
		    pGC->ops->Polylines = apa16LineSS;
	        else
		    pGC->ops->Polylines = miZeroLine;
	    }
	    else
	    {
		pGC->ops->Polylines = miWideLine;
	    }
	}
	else
	    if(pGC->lineWidth == 0)
	        pGC->ops->Polylines = apa16DashLine;
	    else
	        pGC->ops->Polylines = miWideDash;
    }

    if (new_text || new_fill)
    {
	pGC->ops->ImageGlyphBlt = apa16ImageGlyphBlt;

	if (pGC->fillStyle == FillSolid ||
	    (pGC->fillStyle == FillOpaqueStippled &&
	     pGC->fgPixel == pGC->bgPixel))
	{
	    pGC->ops->PolyGlyphBlt	= apa16PolyGlyphBlt;
	    pGC->ops->PolyText8		= apa16PolyText8;
	    pGC->ops->PolyText16	= apa16PolyText16;
	    pGC->ops->ImageText8	= apa16ImageText8;
	    pGC->ops->ImageText16	= apa16ImageText16;
	}
	else
	{
	    pGC->ops->PolyGlyphBlt	= apa16PolyGlyphBlt;
	    pGC->ops->PolyText8		= apa16PolyText8;
	    pGC->ops->PolyText16	= apa16PolyText16;
	}
    }

    if (new_fill)
    {
	/* install a suitable fillspans */
	if ((pGC->fillStyle == FillSolid) ||
	    ((pGC->fillStyle == FillOpaqueStippled) &&
	     (pGC->fgPixel == pGC->bgPixel)))
	{
	    switch(devPriv->rop)
	    {
	      case RROP_WHITE:
	      case RROP_BLACK:
	      case RROP_INVERT:
		pGC->ops->FillSpans = apa16SolidFS;
		break;
	      case RROP_NOP:
		pGC->ops->FillSpans = NoopDDA;
		break;
	    }
	}
	/* beyond this point, opaqueStippled ==> fg != bg */
	else if (((pGC->fillStyle == FillTiled) ||
		 (pGC->fillStyle == FillOpaqueStippled)) &&
		 !devPriv->pRotatedPixmap)
	{
	    pGC->ops->FillSpans = apa16UnnaturalTileFS;
	}
	else if ((pGC->fillStyle == FillStippled) && !devPriv->pRotatedPixmap)
	{
	    pGC->ops->FillSpans = apa16UnnaturalStippleFS;
	}
	else if (pGC->fillStyle == FillStippled)
	{
	    switch(devPriv->rop)
	    {
	      case RROP_WHITE:
	      case RROP_INVERT:
	      case RROP_BLACK:
		pGC->ops->FillSpans = apa16StippleFS;
		break;
	      case RROP_NOP:
		pGC->ops->FillSpans = NoopDDA;
		break;
	    }
	}
	else /* overload tiles to do parti-colored opaque stipples */
	{
	    pGC->ops->FillSpans = apa16TileFS;
	}
        if (pGC->fillStyle == FillSolid)
            pGC->ops->PolyFillArc = apa16PolyFillArcSolid;
        else
            pGC->ops->PolyFillArc = miPolyFillArc;
	/* the rectangle code doesn't deal with opaque stipples that
	   are two colors -- we can fool it for fg==bg, though
	 */
	if ((((pGC->fillStyle == FillTiled) ||
	    (pGC->fillStyle == FillStippled)) && 
	     !devPriv->pRotatedPixmap) ||
	    ((pGC->fillStyle == FillOpaqueStippled) &&
	     (pGC->fgPixel != pGC->bgPixel))
	   )
	{
	    pGC->ops->PolyFillRect = miPolyFillRect;
	}
	else /* deal with solids and natural stipples and tiles */
	{
	    pGC->ops->PolyFillRect = mfbPolyFillRect;

	    if ((pGC->fillStyle == FillSolid) ||
		((pGC->fillStyle == FillOpaqueStippled) &&
		 (pGC->fgPixel == pGC->bgPixel)))
	    {
		switch(devPriv->rop)
		{
		  case RROP_WHITE:
		  case RROP_BLACK:
		  case RROP_INVERT:
		    devPriv->FillArea = apa16SolidFillArea;
		    break;
		  case RROP_NOP:
		    devPriv->FillArea = NoopDDA;
		    break;
		}
	    }
	    else if (pGC->fillStyle == FillStippled)
	    {
		switch(devPriv->rop)
		{
		  case RROP_WHITE:
		  case RROP_BLACK:
		  case RROP_INVERT:
		    devPriv->FillArea = apa16StippleFillArea;
		    break;
		  case RROP_NOP:
		    devPriv->FillArea = NoopDDA;
		    break;
		}
	    }
	    else /* deal with tiles */
	    {
		devPriv->FillArea = apa16TileArea32;
	    }
	} /* end of natural rectangles */
    } /* end of new_fill */
}

/* table to map alu(src, dst) to alu(~src, dst) 
int InverseAlu[16] = {
	GXclear,
	GXandInverted,
	GXnor,
	GXcopyInverted,
	GXand,
	GXnoop,
	GXequiv,
	GXorInverted,
	GXandReverse,
	GXxor,
	GXinvert,
	GXnand,
	GXcopy,
	GXor,
	GXorReverse,
	GXset
};
*/
int
apa16ReduceRop(alu, src)
    register unsigned char alu;
    register Pixel src;
{
    int rop;
    if (src == 0)	/* src is black */
    {
	switch(alu)
	{
	  case GXclear:
	    rop = RROP_BLACK;
	    break;

	  case GXand:
	    rop = RROP_BLACK;
	    break;
	  case GXandReverse:
	    rop = RROP_BLACK;
	    break;
	  case GXcopy:
	    rop = RROP_BLACK;
	    break;
	  case GXandInverted:
	    rop = RROP_NOP;
	    break;
	  case GXnoop:
	    rop = RROP_NOP;
	    break;
	  case GXxor:
	    rop = RROP_NOP;
	    break;
	  case GXor:
	    rop = RROP_NOP;
	    break;
	  case GXnor:
	    rop = RROP_INVERT;
	    break;
	  case GXequiv:
	    rop = RROP_INVERT;
	    break;
	  case GXinvert:
	    rop = RROP_INVERT;
	    break;
	  case GXorReverse:
	    rop = RROP_INVERT;
	    break;
	  case GXcopyInverted:
	    rop = RROP_WHITE;
	    break;
	  case GXorInverted:
	    rop = RROP_WHITE;
	    break;
	  case GXnand:
	    rop = RROP_WHITE;
	    break;
	  case GXset:
	    rop = RROP_WHITE;
	    break;
	}
    }
    else /* src is white */
    {
	switch(alu)
	{
	  case GXclear:
	    rop = RROP_BLACK;
	    break;
	  case GXand:
	    rop = RROP_NOP;
	    break;
	  case GXandReverse:
	    rop = RROP_INVERT;
	    break;
	  case GXcopy:
	    rop = RROP_WHITE;
	    break;
	  case GXandInverted:
	    rop = RROP_BLACK;
	    break;
	  case GXnoop:
	    rop = RROP_NOP;
	    break;
	  case GXxor:
	    rop = RROP_INVERT;
	    break;
	  case GXor:
	    rop = RROP_WHITE;
	    break;
	  case GXnor:
	    rop = RROP_BLACK;
	    break;
	  case GXequiv:
	    rop = RROP_NOP;
	    break;
	  case GXinvert:
	    rop = RROP_INVERT;
	    break;
	  case GXorReverse:
	    rop = RROP_WHITE;
	    break;
	  case GXcopyInverted:
	    rop = RROP_BLACK;
	    break;
	  case GXorInverted:
	    rop = RROP_NOP;
	    break;
	  case GXnand:
	    rop = RROP_INVERT;
	    break;
	  case GXset:
	    rop = RROP_WHITE;
	    break;
	}
    }
    return rop;
}

static void
apa16DestroyClip(pGC)
    GCPtr	pGC;
{
    if(pGC->clientClipType == CT_NONE)
	return;
    else if (pGC->clientClipType == CT_PIXMAP)
    {
	mfbDestroyPixmap((PixmapPtr)(pGC->clientClip));
    }
    else
    {
	/* we know we'll never have a list of rectangles, since
	   ChangeClip immediately turns them into a region 
	*/
        (*pGC->pScreen->RegionDestroy)(pGC->clientClip);
    }
    pGC->clientClip = NULL;
    pGC->clientClipType = CT_NONE;
}

static void
apa16ChangeClip(pGC, type, pvalue, nrects)
    GCPtr	    pGC;
    unsigned int    type;
    pointer	    pvalue;
    int		    nrects;
{
    apa16DestroyClip(pGC);
    if(type == CT_PIXMAP)
    {
	/* convert the pixmap to a region */
	pGC->clientClip = (pointer) (*pGC->pScreen->BitmapToRegion)((PixmapPtr)pvalue);
	/* you wouldn't do this if you were leaving the pixmap in
	   rather than converting it.
	*/
	(*pGC->pScreen->DestroyPixmap)(pvalue);
    }
    else if (type == CT_REGION)
    {
	/* stuff the region in the GC */
	pGC->clientClip = pvalue;
    }
    else if (type != CT_NONE)
    {
	pGC->clientClip = (pointer) (*pGC->pScreen->RectsToRegion)(nrects,
						    (xRectangle *)pvalue,
						    type);
	xfree(pvalue);
    }
    pGC->clientClipType = (type != CT_NONE && pGC->clientClip) ? CT_REGION :
        CT_NONE;
    pGC->stateChanges |= GCClipMask;
}

static void
apa16CopyClip (pgcDst, pgcSrc)
    GCPtr pgcDst, pgcSrc;
{
    RegionPtr prgnNew;

    switch(pgcSrc->clientClipType)
    {
      case CT_PIXMAP:
	((PixmapPtr) pgcSrc->clientClip)->refcnt++;
	/* Fall through !! */
      case CT_NONE:
	apa16ChangeClip(pgcDst, pgcSrc->clientClipType, pgcSrc->clientClip, 0);
	break;
      case CT_REGION:
	prgnNew = (*pgcSrc->pScreen->RegionCreate)(NULL, 1);
	(*pgcSrc->pScreen->RegionCopy)(prgnNew,
				       (RegionPtr)(pgcSrc->clientClip));
	apa16ChangeClip(pgcDst, CT_REGION, (pointer)prgnNew, 0);
	break;
    }
}







