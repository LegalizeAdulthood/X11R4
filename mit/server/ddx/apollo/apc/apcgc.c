/******************************************************************
Copyright 1987 by Apollo Computer Inc., Chelmsford, Massachusetts.
Copyright 1989 by Hewlett-Packard Company.

                        All Rights Reserved

Permission to use, duplicate, change, and distribute this software and
its documentation for any purpose and without fee is granted, provided
that the above copyright notice appear in such copy and that this
copyright notice appear in all supporting documentation, and that the
names of Apollo Computer Inc., the Hewlett-Packard Company, or MIT not
be used in advertising or publicity pertaining to distribution of the
software without written prior permission.

HEWLETT-PACKARD MAKES NO WARRANTY OF ANY KIND WITH REGARD
TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  Hewlett-Packard shall not be liable for errors
contained herein or direct, indirect, special, incidental or
consequential damages in connection with the furnishing,
performance, or use of this material.

This software is not subject to any license of the American
Telephone and Telegraph Company or of the Regents of the
University of California.
******************************************************************/

/* $XConsortium: apcgc.c,v 1.4 90/03/21 09:58:39 rws Exp $ */

#include "apc.h"
#include "Xmd.h"
#include "dixfontstr.h"
#include "fontstruct.h"
#include "mistruct.h"
#include "apctext.h"


static void apcValidateGC();
static void apcChangeGC();
static void apcCopyGC();
static void apcDestroyGC();
static void apcChangeClip();
static void apcDestroyClip();
static void apcCopyClip();

/* Macro saying whether a GCOps is dynamically allocated.
   Right now it always is, if not null. */

#define APC_OPS_DYNAMIC(pOps) (pOps)


static GCOps apcDefaultOps =
{
    apcSolidFS,
    apcSetSpans,
    apcPutImage,
    apcCopyArea,
    apcCopyPlane,
    apcPolyPoint,
    apcZeroLine,
    apcZeroSegment,
    miPolyRectangle,
    miPolyArc,
    apcFillPolygon,
    apcNaturalPFR,
    miPolyFillArc,
    miPolyText8,
    miPolyText16,
    miImageText8,
    miImageText16,
    miImageGlyphBlt,
    miPolyGlyphBlt,
    apcPushPixelsSolid,
    miMiter,
    {NULL}
};

static GCFuncs apcFuncs =
{
    apcValidateGC,
    apcChangeGC,
    apcCopyGC,
    apcDestroyGC,
    apcChangeClip,
    apcDestroyClip,
    apcCopyClip
};


/*
 * apcCreateGC -- DDX interface (screen)
 *      Fill in a new GC.
 */
Bool
apcCreateGC(pGC)
    GCPtr   pGC;
{
    apcPrivGCPtr    pPrivGC;

    pGC->clientClip = NULL;
    pGC->clientClipType = CT_NONE;

    pGC->ops = NULL;
    pGC->funcs = &apcFuncs;

    /* apc wants to translate before scan conversion */
    pGC->miTranslate = TRUE;

    pPrivGC = (apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr);

    pPrivGC->fExpose = TRUE;
    pPrivGC->freeCompClip = FALSE;
    pPrivGC->pCompositeClip = NULL;
    pPrivGC->gprTile = gpr_$nil_bitmap_desc;
    pPrivGC->pdataTile = NULL;
    pPrivGC->gprOpaqueStipple = gpr_$nil_bitmap_desc;
    pPrivGC->pdataOpaqueStipple = NULL;

    return TRUE;
}

/*
 * apcChangeGC -- DDX interface (GC "funcs")
 *      Not interested in GC changes until validation.
 */
static void
apcChangeGC(pGC, mask)
    GCPtr   pGC;
    BITS32  mask;
{
}

/*
 * apcCopyGC -- DDX interface (GC "funcs")
 *	Nothing to do.
 */
static void
apcCopyGC (pGCSrc, changes, pGCDst)
    GCPtr   pGCSrc;
    Mask    changes;
    GCPtr   pGCDst;
{
}

/*
 * apcDestroyGC -- DDX interface (GC "funcs")
 *      Clear currentGC and free composite clip when freeing GC.
 *      Also free tile and opaque stipple if any.
 *      If the ops record is dynamically allocated, free it too.
 */
static void
apcDestroyGC(pGC)
    GCPtr   pGC;
{
    apcPrivScreenPtr    pPrivScreen;
    apcPrivGCPtr        pPrivGC;

    /* clear cache */
    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    if (pGC == pPrivScreen->currentGC)
        pPrivScreen->currentGC = NULL;

    pPrivGC = (apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr);
    if ((pPrivGC->freeCompClip) && (pPrivGC->pCompositeClip))
        (*pGC->pScreen->RegionDestroy)(pPrivGC->pCompositeClip);

    if (pPrivGC->gprTile)
    {
        xfree (pPrivGC->pdataTile);
        apDeallocateBitmap (pPrivGC->gprTile);
    }

    if (pPrivGC->gprOpaqueStipple)
    {
        xfree (pPrivGC->pdataOpaqueStipple);
        apDeallocateBitmap (pPrivGC->gprOpaqueStipple);
    }

    if (APC_OPS_DYNAMIC(pGC->ops))
        xfree(pGC->ops);
}

/*
 * apcUpdateGPR -- Driver internal code
 *      Compare two GPR context records, one in the GC privates and the other
 *      in the Screen privates.  For each entry that differs, copy it from the
 *      GC privates to the Screen privates and set the GPR attribute.
 *      Then set the currentGC field in the Screen privates to the GC pointer.
 *
 *      This routine is called by the UPDATE_GPR macro which must be at the start
 *      of every GC op routine in the driver which draws with GPR calls.  It
 *      should be called only when currentGC does not already equal pGC.
 *
 *      Ideally we would like only to update the attributes the caller will
 *      actually use.  But then we couldn't just mark this GC, by use of the
 *      currentGC field in the Screen privates, as being necessarily completely
 *      up-to-date with the real GPR state.
 *
 *      What we would really like is some really fast way to always check and
 *      update the GPR attributes that will actually be used; then the currentGC
 *      cache could go away (would have to -- it would be incorrect).
 */
void
apcUpdateGPR (pGC, pPrivScreen)
    GCPtr               pGC;
    apcPrivScreenPtr    pPrivScreen;
{
    apcPrivGCPtr    pPrivGC;
    int             i;
    status_$t       status;

    pPrivGC = (apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr);

    /*
     * We must always call apSetBitmap, since if the current bitmap has just
     * been deallocated via apDeallocateBitmap, there will be a new current
     * bitmap, and only apSetBitmap knows this.
     */
    pPrivScreen->curContext.bitmap = pPrivGC->reqContext.bitmap;
    apSetBitmap (pPrivGC->reqContext.bitmap);

    /*
     * Compare the records all at once first.
     */
    if (pPrivGC->reqContext != pPrivScreen->curContext)
    {
        if (pPrivGC->reqContext.clipping_active != pPrivScreen->curContext.clipping_active)
        {
            pPrivScreen->curContext.clipping_active = pPrivGC->reqContext.clipping_active;
            gpr_$set_clipping_active (pPrivGC->reqContext.clipping_active, &status);
        }

        if (pPrivGC->reqContext.clip_window != pPrivScreen->curContext.clip_window)
        {
            pPrivScreen->curContext.clip_window = pPrivGC->reqContext.clip_window;
            gpr_$set_clip_window (pPrivGC->reqContext.clip_window, &status);
        }

        if (pPrivGC->reqContext.plane_mask != pPrivScreen->curContext.plane_mask)
        {
            pPrivScreen->curContext.plane_mask = pPrivGC->reqContext.plane_mask;
            gpr_$set_plane_mask_32 (pPrivGC->reqContext.plane_mask, &status);
        }

        if ((pGC->depth > pPrivScreen->curContext.raster_op.depth) ||
            (pPrivGC->reqContext.raster_op.rop_common !=
             pPrivScreen->curContext.raster_op.rop_common))
        {
            pPrivScreen->curContext.raster_op.rop_common = pPrivGC->reqContext.raster_op.rop_common;
            pPrivScreen->curContext.raster_op.depth = pGC->depth;
            gpr_$set_raster_op_mask ((gpr_$mask_32_t) 0xFFFFFFFF,
                                     pPrivGC->reqContext.raster_op.rop_common, &status);
        }

        if (pPrivGC->reqContext.draw_value != pPrivScreen->curContext.draw_value)
        {
            pPrivScreen->curContext.draw_value = pPrivGC->reqContext.draw_value;
            gpr_$set_draw_value (pPrivGC->reqContext.draw_value, &status);
        }

        if (pPrivGC->reqContext.draw_pattern != pPrivScreen->curContext.draw_pattern)
        {
            pPrivScreen->curContext.draw_pattern = pPrivGC->reqContext.draw_pattern;
            gpr_$set_draw_pattern (pPrivGC->reqContext.draw_pattern.repeat_count,
                                   pPrivGC->reqContext.draw_pattern.pattern,
                                   pPrivGC->reqContext.draw_pattern.length, &status);
        }

        if (pPrivGC->reqContext.fill_value != pPrivScreen->curContext.fill_value)
        {
            pPrivScreen->curContext.fill_value = pPrivGC->reqContext.fill_value;
            gpr_$set_fill_value (pPrivGC->reqContext.fill_value, &status);
        }

        if (pPrivGC->reqContext.fill_background_value != pPrivScreen->curContext.fill_background_value)
        {
            pPrivScreen->curContext.fill_background_value = pPrivGC->reqContext.fill_background_value;
            gpr_$set_fill_background_value (pPrivGC->reqContext.fill_background_value, &status);
        }

        if (pPrivGC->reqContext.fill_pattern != pPrivScreen->curContext.fill_pattern)
        {
            pPrivScreen->curContext.fill_pattern = pPrivGC->reqContext.fill_pattern;
            gpr_$set_fill_pattern (pPrivGC->reqContext.fill_pattern, (short) 1, &status);
        }

        if (pPrivGC->reqContext.triangle_fill_criteria != pPrivScreen->curContext.triangle_fill_criteria)
        {
            pPrivScreen->curContext.triangle_fill_criteria = pPrivGC->reqContext.triangle_fill_criteria;
            gpr_$set_triangle_fill_criteria (pPrivGC->reqContext.triangle_fill_criteria, &status);
        }

        if (pPrivGC->reqContext.pgon_decomp_technique != pPrivScreen->curContext.pgon_decomp_technique)
        {
            pPrivScreen->curContext.pgon_decomp_technique = pPrivGC->reqContext.pgon_decomp_technique;
            gpr_$pgon_decomp_technique (pPrivGC->reqContext.pgon_decomp_technique, &status);
        }

        if (pPrivGC->reqContext.text_value != pPrivScreen->curContext.text_value)
        {
            pPrivScreen->curContext.text_value = pPrivGC->reqContext.text_value;
            gpr_$set_text_value (pPrivGC->reqContext.text_value, &status);
        }

        if (pPrivGC->reqContext.text_background_value != pPrivScreen->curContext.text_background_value)
        {
            pPrivScreen->curContext.text_background_value = pPrivGC->reqContext.text_background_value;
            gpr_$set_text_background_value (pPrivGC->reqContext.text_background_value, &status);
        }

        if (pPrivGC->reqContext.text_font != pPrivScreen->curContext.text_font)
        {
            pPrivScreen->curContext.text_font = pPrivGC->reqContext.text_font;
            gpr_$set_text_font (pPrivGC->reqContext.text_font, &status);
        }
    }

    /*
     * Mark the screen as having the GPR state of this GC now.
     */
    pPrivScreen->currentGC = pGC;
}

/*
 * apcCreateOps -- Driver internal code
 *      Allocate an ops record for a GC.
 */
static GCOps *
apcCreateOps (prototype)
    GCOps  *prototype;
{
    GCOps      *ret;
    extern Bool Must_have_memory;

    /* XXX */ Must_have_memory = TRUE;
    ret = (GCOps *) xalloc (sizeof *ret);
    /* XXX */ Must_have_memory = FALSE;
    if (!ret)
        return 0;
    *ret = *prototype;
    return ret;
}

/*
 * apcValidateGC -- DDX interface (GC "funcs")
 *      Validate the GC.
 */
void
apcValidateGC(pGC, changes, pDraw)
    GCPtr       pGC;
    Mask        changes;
    DrawablePtr pDraw;
{
    WindowPtr           pWin;
    int                 mask;       /* stateChanges */
    int                 index;      /* used for stepping through bitfields */
    DDXPointRec         oldOrg;     /* origin of thing GC was last used with */
    Bool                win_moved;  /* window has moved since last time */
    ScreenPtr           pScreen;
    apDisplayDataPtr    pDisp;
    apcPrivScreenPtr    pPrivScreen;
    apcPrivGCPtr        pPrivGC;
    PixmapPtr           pPM;
    int                 new_clip, new_line, new_text, new_tile, new_pushpix;
    int                 new_dash, new_dashoffset, new_fill, new_fillrule, new_planemask, new_rop;
    int                 effective_planemask;
    int                 i, xscale, yscale;
    unsigned char      *dashptr;
    BoxPtr              pBox;
    gprFIDPtr           gfp;
    Bool                gprtext;
    int                 xorg, yorg;
    gpr_$bitmap_desc_t  bitmap_id;
    int                 ptext_type; /* flag saying how we will do polytext ops */

#define PTEXT_NOP 0
#define PTEXT_GPR 1
#define PTEXT_GPR_STENCIL 2

    pScreen = pGC->pScreen;
    pDisp = (apDisplayDataPtr) pScreen->devPrivates[apDisplayScreenIndex].ptr;
    pPrivScreen = (apcPrivScreenPtr) pScreen->devPrivates[apcScreenIndex].ptr;
    pPrivGC = (apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr);

    /*
     * Force checking of GPR attributes at next entry to any of our GC ops.
     */
    pPrivScreen->currentGC = (GCPtr) NULL;

    /*
     * For now we create our own copy of an ops vector on first validation.
     */
    if (!(pGC->ops))
        pGC->ops = apcCreateOps(&apcDefaultOps);

    oldOrg = pGC->lastWinOrg;
    pGC->lastWinOrg.x = pDraw->x;
    pGC->lastWinOrg.y = pDraw->y;
    win_moved = (oldOrg != pGC->lastWinOrg);

    if (pDraw->type == DRAWABLE_WINDOW)
    {
        pWin = (WindowPtr)pDraw;
        bitmap_id = pDisp->display_bitmap;
    }
    else
    {
        pWin = (WindowPtr)NULL;
        bitmap_id = ((apcPrivPMPtr)(((PixmapPtr)pDraw)->devPrivate.ptr))->bitmap_desc;
    }

    pPrivGC->reqContext.bitmap = bitmap_id;

    /*
     * Clipping conventions:
     *  if the drawable is a window
     *      CT_REGION ==> pCompositeClip really is the composite
     *      CT_other ==> pCompositeClip is the window clip region
     *  if the drawable is a pixmap
     *      CT_REGION ==> pCompositeClip is the translated client region
     *          clipped to the pixmap boundary
     *      CT_other ==> pCompositeClip is the pixmap bounding box
     */

    /*
     * If the client clip is different or moved OR the subwindowMode has
     * changed OR the window's clip has changed since the last validation
     * we need to recompute the composite clip.
     */
    new_clip = (changes & (GCClipXOrigin | GCClipYOrigin | GCClipMask | GCSubwindowMode)) ||
               (pDraw->serialNumber != (pGC->serialNumber & DRAWABLE_SERIAL_BITS));
    if (new_clip)
    {
        if (pWin)
        {
            Bool        freeTmpClip, freeCompClip;
            RegionPtr   pregWin;

            freeTmpClip = (pGC->subWindowMode == IncludeInferiors);
            if (freeTmpClip)
                pregWin = NotClippedByChildren(pWin);
            else
                pregWin = &pWin->clipList;

            freeCompClip = pPrivGC->freeCompClip;

            /*
             * If there is no client clip, we can get by with just keeping
             * the pointer we got, and remembering whether or not should
             * destroy (or maybe re-use) it later.  This way, we avoid
             * unnecessary copying of regions.  (This wins especially if
             * many clients clip by children and have no client clip.)
             */
            if (pGC->clientClipType == CT_NONE)
            {
                if (freeCompClip)
                    (*pScreen->RegionDestroy) (pPrivGC->pCompositeClip);
                pPrivGC->pCompositeClip = pregWin;
                pPrivGC->freeCompClip = freeTmpClip;
            }
            else
            {
                /* We need one 'real' region to put into the composite clip.
                 * If pregWin and the current composite clip are real,
                 *     we can get rid of one.
                 * If the current composite clip is real and pregWin isn't,
                 *     intersect the client clip and pregWin
                 *     into the existing composite clip.
                 * If pregWin is real and the current composite clip isn't,
                 *     intersect pregWin with the client clip
                 *     and replace the composite clip with it.
                 * If neither is real,
                 *     create a new region and do the intersection into it.
                 */
                (*pScreen->TranslateRegion)(pGC->clientClip,
                                            pDraw->x + pGC->clipOrg.x,
                                            pDraw->y + pGC->clipOrg.y);

                if (freeCompClip)
                {
                    (*pScreen->Intersect)(pPrivGC->pCompositeClip,
                                          pregWin, pGC->clientClip);
                    if (freeTmpClip)
                        (*pScreen->RegionDestroy)(pregWin);
                }
                else if (freeTmpClip)
                {
                    (*pScreen->Intersect)(pregWin, pregWin, pGC->clientClip);
                    pPrivGC->pCompositeClip = pregWin;
                }
                else
                {
                    pPrivGC->pCompositeClip = (*pScreen->RegionCreate)(NullBox, 0);
                    (*pScreen->Intersect)(pPrivGC->pCompositeClip,
                                          pregWin, pGC->clientClip);
                }
                pPrivGC->freeCompClip = TRUE;
                (*pScreen->TranslateRegion)(pGC->clientClip,
                                            -(pDraw->x + pGC->clipOrg.x),
                                            -(pDraw->y + pGC->clipOrg.y));

            }
        }   /* end of composite clip for a window */
        else
        {
            BoxRec      pixbounds;

            pixbounds.x1 = 0;
            pixbounds.y1 = 0;
            pixbounds.x2 = pDraw->width;
            pixbounds.y2 = pDraw->height;

            if (pPrivGC->freeCompClip)
                (*pScreen->RegionReset) (pPrivGC->pCompositeClip, &pixbounds);
            else
            {
                pPrivGC->freeCompClip = TRUE;
                pPrivGC->pCompositeClip = (*pScreen->RegionCreate) (&pixbounds, 1);
            }

	    if (pGC->clientClipType == CT_REGION)
	    {
		(*pScreen->TranslateRegion)(pPrivGC->pCompositeClip,
					    -pGC->clipOrg.x, -pGC->clipOrg.y);
		(*pScreen->Intersect)(pPrivGC->pCompositeClip,
				      pPrivGC->pCompositeClip,
				      pGC->clientClip);
		(*pScreen->TranslateRegion)(pPrivGC->pCompositeClip,
					    pGC->clipOrg.x, pGC->clipOrg.y);
	    }
        }   /* end of composite clip for pixmap */
    }

    new_line = FALSE;
    new_text = FALSE;
    new_tile = FALSE;
    new_pushpix = FALSE;
    new_dash = FALSE;
    new_dashoffset = FALSE;
    new_fill = FALSE;
    new_fillrule = FALSE;
    new_planemask = FALSE;
    new_rop = FALSE;

    mask = changes;
    while (mask)
    {
        index = lowbit (mask);
        mask &= ~index;

        /*
         * Here we accumulate a list of booleans which tell us what sets of
         * related GC "op" functions and GPR attributes need to be reset.
         */
        switch (index) {
        case GCFunction:
            new_rop = TRUE;
            new_text = TRUE;
            break;
        case GCForeground:
        case GCBackground:
            new_fill = TRUE;
            new_text = TRUE;
            break;
        case GCPlaneMask:
            new_planemask = TRUE;
            new_text = TRUE;
            break;
        case GCLineStyle:
        case GCLineWidth:
        case GCCapStyle:
        case GCJoinStyle:
            new_line = TRUE;
            break;
        case GCFillStyle:
            new_fill = TRUE;
            new_tile = TRUE;
            new_text = TRUE;
            new_pushpix = TRUE;
            new_line = TRUE;
            break;
        case GCTile:
        case GCStipple:
        case GCTileStipXOrigin:
        case GCTileStipYOrigin:
            new_tile = TRUE;
            break;
        case GCFont:
            new_text = TRUE;
            break;
        case GCDashOffset:
            new_dashoffset = TRUE;
            break;
        case GCDashList:
            new_dashoffset = TRUE;
            new_dash = TRUE;
            new_line = TRUE;
            break;
        case GCFillRule:
            new_fillrule = TRUE;
            break;
        case GCSubwindowMode:
        case GCGraphicsExposures:
        case GCClipXOrigin:
        case GCClipYOrigin:
        case GCClipMask:
        case GCArcMode:
        default:
            break;
        }
    }

    /*
     * If window has moved, make sure the tile gets revalidated.
     */
    if (win_moved)
        new_tile = TRUE;

    /*
     * Now deal with the changes we've collected.
     */
    if (new_clip)
    {
        /* if there is at least one clip window, give the first one to GPR */
        if (!REGION_NIL(pPrivGC->pCompositeClip))
        {
            pPrivGC->reqContext.clipping_active = true;
            pBox = REGION_RECTS(pPrivGC->pCompositeClip);
            pPrivGC->reqContext.clip_window.window_size.x_size = pBox->x2 -
                        (pPrivGC->reqContext.clip_window.window_base.x_coord = pBox->x1);
            pPrivGC->reqContext.clip_window.window_size.y_size = pBox->y2 -
                        (pPrivGC->reqContext.clip_window.window_base.y_coord = pBox->y1);
        }
    }

    if (new_fillrule)
    {
        switch (pGC->fillRule)
        {
        case EvenOddRule:
            pPrivGC->reqContext.triangle_fill_criteria.wind_type = gpr_$parity;
            break;
        case WindingRule:
            pPrivGC->reqContext.triangle_fill_criteria.wind_type = gpr_$nonzero;
            break;
        }
        pPrivGC->reqContext.triangle_fill_criteria.winding_no = 1;
    }

    if (new_fill)
    {
        pPrivGC->reqContext.draw_value = pGC->fgPixel;
        pPrivGC->reqContext.fill_value = pGC->fgPixel;
        pPrivGC->reqContext.fill_background_value = (pGC->fillStyle == FillStippled) ?
                                                        (gpr_$pixel_value_t)(-1) :
                                                        pGC->bgPixel;
        pPrivGC->reqContext.pgon_decomp_technique = gpr_$non_overlapping_tris;
    }

    if (new_rop)
        pPrivGC->reqContext.raster_op.rop_common = pGC->alu;

    effective_planemask = (pGC->planemask) & ((1 << (pGC->depth)) - 1);

    if (new_planemask)
        pPrivGC->reqContext.plane_mask = effective_planemask;

    if (new_dash)
    {
        pPrivGC->reqContext.draw_pattern.length = 0;
        dashptr = pGC->dash;
        for (i=0; i<pGC->numInDashList; i++)
            pPrivGC->reqContext.draw_pattern.length += (short)(*dashptr++);

        if (pPrivGC->reqContext.draw_pattern.length <= MAX_DASH_SIZE)
        {
            /* recalculate pattern */
            int             ele_length, temp, index, i, j;
            short           bit = 0;
            unsigned char  *data = pGC->dash;

            /* ZERO the pattern */
            for (i=0; i<(MAX_DASH_SIZE>>4); i++)
                pPrivGC->reqContext.draw_pattern.pattern[i] = 0;

            for (i=0, index=0; i<pGC->numInDashList; i++, data++)
            {
                ele_length = (short)(*data);
                bit = (bit+1) & 0x1;
                for (j=0; j<ele_length; j++)
                {
                    index++;
                    pPrivGC->reqContext.draw_pattern.pattern[index>>4] =
                            (pPrivGC->reqContext.draw_pattern.pattern[index>>4]<<1) | bit;
                }
            }
            temp = index>>4;
            while (temp == (index>>4))
            {
                pPrivGC->reqContext.draw_pattern.pattern[index>>4] =
                        (pPrivGC->reqContext.draw_pattern.pattern[index>>4]<<1);
                index++;
            }
        }
    }

    if (new_dashoffset && (pPrivGC->reqContext.draw_pattern.length <= MAX_DASH_SIZE))
    {
        /* shift the dash pattern accordingly */

        short           mask = 0xFFFF;
        unsigned short  save1, save2;
        int             offset = pGC->dashOffset;
        short           patt_temp;

        while (offset > 16)
        {
            patt_temp = pPrivGC->reqContext.draw_pattern.pattern[3];
            pPrivGC->reqContext.draw_pattern.pattern[3] = pPrivGC->reqContext.draw_pattern.pattern[2];
            pPrivGC->reqContext.draw_pattern.pattern[2] = pPrivGC->reqContext.draw_pattern.pattern[1];
            pPrivGC->reqContext.draw_pattern.pattern[1] = pPrivGC->reqContext.draw_pattern.pattern[0];
            pPrivGC->reqContext.draw_pattern.pattern[0] = patt_temp;
            offset -= 16;
        }

        mask = mask<<(16-offset);
        save1 = pPrivGC->reqContext.draw_pattern.pattern[0] | mask;
        for (i=3; i>-1; i--)
        {
            if (i & 0x1)
            {   /* element 1 or 3 */
                save2 = pPrivGC->reqContext.draw_pattern.pattern[i] | mask;
                save1 = save1>>(16-offset);
                pPrivGC->reqContext.draw_pattern.pattern[i] <= offset;
                pPrivGC->reqContext.draw_pattern.pattern[i] |= save1;
            }
            else
            {   /* element 0 or 2 */
                save1 = pPrivGC->reqContext.draw_pattern.pattern[i] | mask;
                save2 = save2>>(16-offset);
                pPrivGC->reqContext.draw_pattern.pattern[i] <= offset;
                pPrivGC->reqContext.draw_pattern.pattern[i] |= save2;
            }
        }
    }

    if (new_line)
    {
        if (pGC->lineStyle == LineSolid)
        {
            pPrivGC->reqContext.draw_pattern.repeat_count = 0;
            if (pGC->lineWidth == 0)
            {
                if (pGC->fillStyle == FillSolid)
                {
                    pGC->ops->PolySegment = apcZeroSegment;
                    pGC->ops->Polylines = apcZeroLine;
                }
                else
                {
                    pGC->ops->PolySegment = miPolySegment;
                    pGC->ops->Polylines = miZeroLine;
                }
            }
            else
            {
                pGC->ops->Polylines = apcWideLine;
                pGC->ops->PolySegment = miPolySegment;
            }
        }
        else if (pGC->lineWidth == 0)
        {
            /*
             * Since GPR can only accept line patterns of 64 bits,
             * we check here to see if the num of dash list elements
             * is greater than eight (each element is a char).
             */
            if (pPrivGC->reqContext.draw_pattern.length > MAX_DASH_SIZE)
            {
                pGC->ops->Polylines = miWideDash;
                pGC->ops->PolySegment = miPolySegment;
            }
            else
            {
                pPrivGC->reqContext.draw_pattern.repeat_count = 1;
                pGC->ops->Polylines = apcZeroDashLine;
                pGC->ops->PolySegment = apcZeroDashSeg;
            }
        }
        else
        {
            pGC->ops->Polylines = miWideDash;
            pGC->ops->PolySegment = miPolySegment;
        }

        switch (pGC->joinStyle)
        {
        case JoinMiter:
            pGC->ops->LineHelper = miMiter;
            break;
        case JoinRound:
        case JoinBevel:
            pGC->ops->LineHelper = miNotMiter;
        }
    }

    if (new_text)
    {
        /* first see if GPR fonts exist; if not, must use mi */
        if (pGC->font)
        {
            gfp = (gprFIDPtr) pGC->font->devPriv[pScreen->myNum];
#ifdef NO_TEXT16
            gprtext = gfp && (gfp->nGprFonts);
#else
            gprtext = gfp && (gfp->fontId);
#endif
        }
        else
            gprtext = FALSE;

        if (gprtext)
        {
#ifdef NO_TEXT16
            pPrivGC->reqContext.text_font = gfp->fontIds[0];
#else
            pPrivGC->reqContext.text_font = gfp->fontId;
#endif
            /* force a reload, since we may have a new font with an old id */
            pPrivScreen->curContext.text_font = ~(pPrivGC->reqContext.text_font);

            pPrivGC->reqContext.text_value = pGC->fgPixel;
            pPrivGC->reqContext.text_background_value = pGC->bgPixel;

            pGC->ops->ImageText8 = gprImageText8;
            pGC->ops->ImageText16 = gprImageText16;

            ptext_type = PTEXT_GPR_STENCIL; /* this type will always work if we can use GPR at all */

            if (pGC->fillStyle == FillSolid)
            {
                Bool    pix_subset_mask, pix_disjoint_mask;

                pPrivGC->polyTextVal = pGC->fgPixel;

                pix_subset_mask = !(pGC->fgPixel & ~effective_planemask);
                pix_disjoint_mask = !(pGC->fgPixel & effective_planemask);
                switch (pGC->alu)
                {
                case GXclear:
                    pPrivGC->polyTextVal = 0;
                    ptext_type = PTEXT_GPR;
                    break;
                case GXand:
                    if (pix_subset_mask)
                        ptext_type = PTEXT_NOP;
                    else if (pix_disjoint_mask)
                    {
                        pPrivGC->polyTextVal = 0;
                        ptext_type = PTEXT_GPR;
                    }
                    break;
                case GXandReverse:
                    if (pix_disjoint_mask)
                    {
                        pPrivGC->polyTextVal = 0;
                        ptext_type = PTEXT_GPR;
                    }
                    break;
                case GXcopy:
                    ptext_type = PTEXT_GPR;
                    break;
                case GXandInverted:
                    if (pix_disjoint_mask)
                        ptext_type = PTEXT_NOP;
                    else if (pix_subset_mask)
                    {
                        pPrivGC->polyTextVal = 0;
                        ptext_type = PTEXT_GPR;
                    }
                    break;
                case GXnoop:
                    ptext_type = PTEXT_NOP;
                    break;
                case GXxor:
                    if (pix_disjoint_mask)
                        ptext_type = PTEXT_NOP;
                    break;
                case GXor:
                    if (pix_disjoint_mask)
                        ptext_type = PTEXT_NOP;
                    else if (pix_subset_mask)
                    {
                        pPrivGC->polyTextVal = effective_planemask;
                        ptext_type = PTEXT_GPR;
                    }
                    break;
                case GXnor:
                    if (pix_subset_mask)
                    {
                        pPrivGC->polyTextVal = 0;
                        ptext_type = PTEXT_GPR;
                    }
                    break;
                case GXequiv:
                    if (pix_subset_mask)
                        ptext_type = PTEXT_NOP;
                    break;
                case GXinvert:
                    break;
                case GXorReverse:
                    if (pix_subset_mask)
                    {
                        pPrivGC->polyTextVal = effective_planemask;
                        ptext_type = PTEXT_GPR;
                    }
                    break;
                case GXcopyInverted:
                    pPrivGC->polyTextVal = ~pPrivGC->polyTextVal;
                    ptext_type = PTEXT_GPR;
                    break;
                case GXorInverted:
                    if (pix_subset_mask)
                        ptext_type = PTEXT_NOP;
                    else if (pix_disjoint_mask)
                    {
                        pPrivGC->polyTextVal = effective_planemask;
                        ptext_type = PTEXT_GPR;
                    }
                    break;
                case GXnand:
                    if (pix_disjoint_mask)
                    {
                        pPrivGC->polyTextVal = effective_planemask;
                        ptext_type = PTEXT_GPR;
                    }
                    break;
                case GXset:
                    pPrivGC->polyTextVal = effective_planemask;
                    ptext_type = PTEXT_GPR;
                }
            }

            switch (ptext_type)
            {
            case PTEXT_NOP:
                pGC->ops->PolyText8 = nopText;
                pGC->ops->PolyText16 = nopText;
                break;
            case PTEXT_GPR:
                pGC->ops->PolyText8 = gprPolyText8;
                pGC->ops->PolyText16 = gprPolyText16;
                break;
            case PTEXT_GPR_STENCIL:
                pGC->ops->PolyText8 = gprPolyText8Stencil;
                pGC->ops->PolyText16 = gprPolyText16Stencil;
            }
        }
        else
        {   /* must fall back to mi */
            pGC->ops->ImageText8 = miImageText8;
            pGC->ops->ImageText16 = miImageText16;
            pGC->ops->PolyText8 = miPolyText8;
            pGC->ops->PolyText16 = miPolyText16;
        }
    }

    if (new_pushpix)
    {
        switch (pGC->fillStyle)
        {
        case FillSolid:
            pGC->ops->PushPixels = apcPushPixelsSolid;
            break;
        case FillStippled:
        case FillOpaqueStippled:
            pGC->ops->PushPixels = apcPushPixelsStip;
            break;
        case FillTiled:
            pGC->ops->PushPixels = apcPushPixelsTile;
        }
    }

    if (new_tile)
    {
        switch (pGC->fillStyle)
        {
        case FillSolid:
            pPrivGC->reqContext.fill_pattern = gpr_$nil_bitmap_desc;
            pGC->ops->PolyFillRect = apcNaturalPFR;
            pGC->ops->FillSpans = apcSolidFS;
            pGC->ops->FillPolygon = apcFillPolygon;
            break;
        case FillTiled:
        case FillOpaqueStippled:
        case FillStippled:
            /*
             * If the given tile is power-of-2 <= 32 in both dimensions,
             * we can use it in GPR.  Otherwise must resort to messier alternatives.
             */

            pPM = (pGC->fillStyle==FillTiled) ? pGC->tile.pixmap : pGC->stipple;

            xscale = TILE_SIZE / pPM->width;
            yscale = TILE_SIZE / pPM->height;
            if ((pPM->width * xscale != TILE_SIZE) ||
                (pPM->height * yscale != TILE_SIZE))
            {
                pGC->ops->PolyFillRect = apcUnnaturalPFR;
                pGC->ops->FillSpans = apcUnnaturalFS;
                pGC->ops->FillPolygon = miFillPolygon;
            }
            else
            {
                gpr_$bitmap_desc_t  gprSource;
                gpr_$bitmap_desc_t  gprTileStip;
                int                 xalign, yalign;
                int                 bitm_depth;
                BoxRec              box;

                if (pGC->fillStyle == FillTiled)
                {
                        bitm_depth = pGC->depth;
                        if (bitm_depth == 1)
                            /* no single-plane tiles, lest GPR stipple */
                            bitm_depth = 2;

                    if (pPrivGC->gprTile == gpr_$nil_bitmap_desc)
                    {
                        int             totalsize;
                        unsigned char  *pdata;

                        totalsize = PixmapBytePad(TILE_SIZE, 1) * TILE_SIZE * bitm_depth;
                        pPrivGC->pdataTile = (unsigned char *) xalloc (totalsize);
                        if (pPrivGC->pdataTile)
                        {
                            pPrivGC->gprTile =
                                apMakeBitmapFromArray (TILE_SIZE, TILE_SIZE, bitm_depth,
                                                       pPrivScreen->attribute_block,
                                                       XYPixmap, pPrivGC->pdataTile);
                        }
                    }
                    gprTileStip = pPrivGC->gprTile;
                }
                else /* stipple */
                {
                    bitm_depth = 1;

                    if (pPrivGC->gprOpaqueStipple == gpr_$nil_bitmap_desc)
                    {
                        int             totalsize;
                        unsigned char  *pdata;

                        totalsize = PixmapBytePad(TILE_SIZE, 1) * TILE_SIZE;
                        pPrivGC->pdataOpaqueStipple = (unsigned char *) xalloc (totalsize);
                        if (pPrivGC->pdataOpaqueStipple)
                        {
                            pPrivGC->gprOpaqueStipple =
                                apMakeBitmapFromArray (TILE_SIZE, TILE_SIZE, bitm_depth,
                                                       pPrivScreen->attribute_block,
                                                       XYPixmap, pPrivGC->pdataOpaqueStipple);
                        }
                    }
                    gprTileStip = pPrivGC->gprOpaqueStipple;
                }

                xorg = pGC->patOrg.x + pDraw->x;
                yorg = pGC->patOrg.y + pDraw->y;
                xalign = (-xorg) % pPM->width;
                yalign = (-yorg) % pPM->height;
                /* HACK ALERT require sign of remainder is sign of divisor */
                if (xalign < 0)
                    xalign += pPM->width;
                if (yalign < 0)
                    yalign += pPM->height;

                SET_BITMAP (gprTileStip, pPrivScreen);
                SET_CLIPPING_ACTIVE (false, pPrivScreen);
                SET_PLANE_MASK (0xFFFFFFFF, pPrivScreen);
                SET_RASTER_OP_COMMON (GXcopy, pPrivScreen, bitm_depth);

                gprSource = ((apcPrivPMPtr) (pPM->devPrivate.ptr))->bitmap_desc;
                box.x1 = box.y1 = 0;
                box.x2 = box.y2 = TILE_SIZE;
                apcDoubleBlt (gprSource, gprTileStip, bitm_depth, xalign, yalign,
                              pPM->width, pPM->height, &box, TRUE, pPrivScreen);

                pPrivGC->reqContext.fill_pattern = gprTileStip;

                if (pPrivScreen->curContext.fill_pattern == gprTileStip)
                    /* force a reload, since we have changed the contents but not the
                   identity of the tile/stipple bitmap */
                pPrivScreen->curContext.fill_pattern = gpr_$nil_bitmap_desc;

                pGC->ops->PolyFillRect = apcNaturalPFR;
                pGC->ops->FillSpans = apcNaturalFS;
                pGC->ops->FillPolygon = apcFillPolygon;
            }
            break;
        }
    }
}

/*
 * apcDestroyClip -- DDX interface (GC "funcs")
 *      Destroy client clip info.
 */
static void
apcDestroyClip(pGC)
    GCPtr   pGC;
{
    if (pGC->clientClipType == CT_NONE)
        return;
    (*pGC->pScreen->RegionDestroy)(pGC->clientClip);
    pGC->clientClip = NULL;
    pGC->clientClipType = CT_NONE;
}

/*
 * apcChangeClip -- DDX interface (GC "funcs")
 *      Change client clip info.
 */
static void
apcChangeClip(pGC, type, pvalue, nrects)
    GCPtr       pGC;
    int         type;
    pointer     pvalue;
    int         nrects;
{
    apcDestroyClip (pGC);
    if (type == CT_PIXMAP)
    {
        pGC->clientClip = (pointer) (*pGC->pScreen->BitmapToRegion)
                                        ((PixmapPtr)pvalue);
        (*pGC->pScreen->DestroyPixmap)(pvalue);
    }
    else if (type == CT_REGION)
    {
        pGC->clientClip = pvalue;
    }
    else if (type != CT_NONE)
    {
        pGC->clientClip = (pointer) (*pGC->pScreen->RectsToRegion)
                                        (nrects, (xRectangle *)pvalue, type);
        xfree(pvalue);
    }
    pGC->clientClipType = (type != CT_NONE && pGC->clientClip) ?
                               CT_REGION : CT_NONE;
    pGC->stateChanges |= GCClipMask;
}

/*
 * apcCopyClip -- DDX interface (GC "funcs")
 *      Copy client clip info.
 */
static void
apcCopyClip (pGCDst, pGCSrc)
    GCPtr   pGCDst;
    GCPtr   pGCSrc;
{
    RegionPtr   pRegion;

    switch (pGCSrc->clientClipType)
    {
    case CT_PIXMAP:
        ((PixmapPtr) pGCSrc->clientClip)->refcnt++;
        /* Fall through !! */
    case CT_NONE:
        apcChangeClip(pGCDst, pGCSrc->clientClipType, pGCSrc->clientClip, 0);
        break;
    case CT_REGION:
        pRegion = (*pGCSrc->pScreen->RegionCreate)(NULL, 1);
        (*pGCSrc->pScreen->RegionCopy)(pRegion, (RegionPtr)(pGCSrc->clientClip));
        apcChangeClip (pGCDst, CT_REGION, pRegion, 0);
        break;
    }
}
