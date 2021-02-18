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

#include "apc.h"

/*
 * apcPushPixelsSolid -- DDX interface (GC "op" PushPixels)
 *      Squeegees the foreground color of pGC through pBitMap into pDraw.
 *
 *      Strategy:  just use gpr_$additive_blt from the stencil to the drawable, with
 *      clever use of raster ops to get the foreground pixel value where the stencil
 *      is 1, and leave the drawable unchanged where the stencil is 0.
 */
void
apcPushPixelsSolid(pGC, pBitMap, pDraw, dx, dy, xOrg, yOrg)
    GCPtr       pGC;
    PixmapPtr   pBitMap;
    DrawablePtr pDraw;
    int         dx, dy, xOrg, yOrg;
{
    RegionPtr           pClip;
    int                 numClipRects;
    BoxPtr              clipRects;
    int                 xMin, xMax, yMin, yMax;
    int                 i;
    BoxPtr              pbox;
    short               alu_fg0, alu_fg1;
    gpr_$bitmap_desc_t  srcBitmap;
    gpr_$window_t       src_win;
    gpr_$position_t     dest_origin;
    gpr_$raster_op_t    rop;
    status_$t           status;
    apcPrivScreenPtr    pPrivScreen;

    /* If the destination isn't realized, this is easy */
    if (pDraw->type == DRAWABLE_WINDOW &&
        !((WindowPtr)pDraw)->realized)
        return;

    pClip = ((apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr))->pCompositeClip;
    if (REGION_NIL(pClip))
        return;

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    numClipRects = REGION_NUM_RECTS(pClip);
    clipRects = REGION_RECTS(pClip);

    srcBitmap = ((apcPrivPMPtr)(((PixmapPtr)pBitMap)->devPrivate.ptr))->bitmap_desc;

    /*
     * Select per-plane raster ops which will apply pGC->alu to pGC->fgPixel
     * where the stencil is 1, and leave the destination alone where the
     * stencil is 0.  I.e., where src==0, use dst, therefore the raster op
     * will be 4, 5, 6 or 7 (binary 01ab).  ab will be the high-order two bits
     * of pGC->alu if the bit of pGC->fgPixel for this plane is a 0, or the
     * low-order two bits if it is a 1.
     *
     *  src dst     0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
     *----------------------------------------------------------------
     *   0   0      0  0  0  0  0  0  0  0  1  1  1  1  1  1  1  1
     *   0   1      0  0  0  0  1  1  1  1  0  0  0  0  1  1  1  1
     *   1   0      0  0  1  1  0  0  1  1  0  0  1  1  0  0  1  1
     *   1   1      0  1  0  1  0  1  0  1  0  1  0  1  0  1  0  1
     */

    alu_fg0 = 4 | (((pGC->alu) >> 2) & 0x03);
    alu_fg1 = 4 | ((pGC->alu) & 0x03);

    /* Set a rop per plane to accomplish fg/transparent bg effect */
    for (i = 0; i < pGC->depth; i++)
    {
        rop = ((pGC->fgPixel >> i) & 0x01) ? alu_fg1 : alu_fg0;
        SET_RASTER_OP (i, rop, pPrivScreen);
    }

    if (numClipRects > 1)
        SET_CLIPPING_ACTIVE (false, pPrivScreen);

    for (i = 0; i < numClipRects; i++)
    {
        pbox = &clipRects[i];
        xMin = max(xOrg, pbox->x1);
        xMax = min(xOrg+dx, pbox->x2);
        yMin = max(yOrg, pbox->y1);
        yMax = min(yOrg+dy, pbox->y2);
        /* is there anything visible here? */
        if (xMax <= xMin || yMax <= yMin)
            continue;

        src_win.window_base.x_coord = xMin-xOrg;
        src_win.window_base.y_coord = yMin-yOrg;
        src_win.window_size.x_size = xMax-xMin;
        src_win.window_size.y_size = yMax-yMin;

        dest_origin.x_coord = xMin;
        dest_origin.y_coord = yMin;

        gpr_$additive_blt (srcBitmap, src_win, 0, dest_origin, &status);
    }
}

/*
 * apcPushPixelsStip -- DDX interface (GC "op" PushPixels)
 *      Does a PushPixels for FillStyle either Stippled or OpaqueStippled.
 *      That is, for Stippled it squeegees the foreground color of pGC through pBitMap,
 *      intersected with the stipple bitmap, into pDraw.  For OpaqueStippled it does the
 *      above, then also squeegees the background color of pGC through pBitMap, intersected with
 *      the complement of the stipple bitmap, into pDraw.
 *
 *      Strategy:  make a temp copy of the stencil (we are about to do destructive things to it).
 *      Then, AND the stipple over it (taking care to align the stipple as it should be aligned
 *      w.r.t. the ultimate drawable, not the temp).  We do this by making a scratch GC with the
 *      same stipple, a function of AND, and fill style of OpaqueStipple, and using it to do a
 *      filled rectangle onto the temp.  We now have kept only the bits which are 1 in the stipple.
 *      Then, proceed as above for the Solid case.
 *
 *      If we are doing Stippled, we are now done.  If we are doing OpaqueStippled, continue
 *      by XORing the original stencil onto the temp, thus producing a screen which contains
 *      only the bits which are 0 in the stipple.  Then, do the work as in the Solid case
 *      again, only using the background rather than the foreground pixel value.
 */
void
apcPushPixelsStip(pGC, pBitMap, pDraw, dx, dy, xOrg, yOrg)
    GCPtr       pGC;
    PixmapPtr   pBitMap;
    DrawablePtr pDraw;
    int         dx, dy, xOrg, yOrg;
{
    RegionPtr           pClip;
    int                 numClipRects;
    BoxPtr              clipRects;
    int                 xMin, xMax, yMin, yMax;
    int                 xrelOrg, yrelOrg;
    int                 i;
    int                 temp_signed;
    BoxPtr              pbox;
    short               alu_0, alu_1;
    gpr_$bitmap_desc_t  srcBitmap, tempBitmap;
    PixmapPtr           pTempPixmap;
    GCPtr               pGCTemp;
    xRectangle          whole_rect;
    gpr_$window_t       src_win;
    gpr_$position_t     dest_origin;
    gpr_$raster_op_t    rop;
    status_$t           status;
    apcPrivScreenPtr    pPrivScreen;

static XID gcvals[] = {
        GXand,              /* function */
        1,                  /* plane mask */
        1,                  /* foreground */
        0,                  /* background */
        FillOpaqueStippled, /* fill style */
        NULL,               /* stipple (set dynamically) */
        0,                  /* x pattern origin (set dynamically) */
        0                   /* y pattern origin (set dynamically) */
        };


    /* If the destination isn't realized, this is easy */
    if (pDraw->type == DRAWABLE_WINDOW &&
        !((WindowPtr)pDraw)->realized)
        return;

    pClip = ((apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr))->pCompositeClip;
    if (REGION_NIL(pClip))
        return;

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    numClipRects = REGION_NUM_RECTS(pClip);
    clipRects = REGION_RECTS(pClip);

    srcBitmap = ((apcPrivPMPtr)(((PixmapPtr)pBitMap)->devPrivate.ptr))->bitmap_desc;

    /* relative origin needed for deriving pattern origin */
    xrelOrg = xOrg - pDraw->x;
    yrelOrg = yOrg - pDraw->y;

    /* Make temp bitmap and do a quick-and-dirty validate */
    pTempPixmap = (* pGC->pScreen->CreatePixmap)(pGC->pScreen, dx, dy, 1);
    if (!pTempPixmap)
        return;
    pGCTemp = GetScratchGC (1, pGC->pScreen);
    if (!pGCTemp)
    {
        (* pGC->pScreen->DestroyPixmap)(pTempPixmap);
        return;
    }
    tempBitmap = ((apcPrivPMPtr)(pTempPixmap->devPrivate.ptr))->bitmap_desc;
    SET_BITMAP (tempBitmap, pPrivScreen);

    SET_CLIPPING_ACTIVE (false, pPrivScreen);
    SET_PLANE_MASK (1, pPrivScreen);
    SET_RASTER_OP (0, GXcopy, pPrivScreen);

    /* Copy input stencil into temp */
    src_win.window_base.x_coord = src_win.window_base.y_coord = 0;
    src_win.window_size.x_size = dx;
    src_win.window_size.y_size = dy;
    dest_origin.x_coord = dest_origin.y_coord = 0;
    gpr_$pixel_blt (srcBitmap, src_win, dest_origin, &status);

    /* AND the stipple onto the temp */
    gcvals[5] = (XID) pGC->stipple;
    temp_signed = (pGC->patOrg.x + xrelOrg) % (pGC->stipple->width);
    if (temp_signed < 0) temp_signed += pGC->stipple->width;
    gcvals[6] = temp_signed;
    temp_signed = (pGC->patOrg.y + yrelOrg) % (pGC->stipple->height);
    if (temp_signed < 0) temp_signed += pGC->stipple->height;
    gcvals[7] = temp_signed;
    DoChangeGC (pGCTemp,
                (GCFunction | GCPlaneMask | GCForeground | GCBackground |
                 GCFillStyle | GCStipple | GCTileStipXOrigin | GCTileStipYOrigin),
                gcvals, TRUE);  /* last arg TRUE because stipple is a pointer not a resource ID */
    ValidateGC (pTempPixmap, pGCTemp);
    whole_rect.x = whole_rect.y = 0;
    whole_rect.width = dx;
    whole_rect.height = dy;
    /* This had better not recurse! */
    (* pGCTemp->ops->PolyFillRect)(pTempPixmap, pGCTemp, 1, &whole_rect);
    FreeScratchGC (pGCTemp);

    /* Go back to drawable and put the foreground pixels up */
    UPDATE_GPR (pGC, pPrivScreen);
    SET_CLIPPING_ACTIVE (false, pPrivScreen);   /* disable again after UPDATE_GPR */

    alu_0 = 4 | (((pGC->alu) >> 2) & 0x03);
    alu_1 = 4 | ((pGC->alu) & 0x03);

    /* Set a rop per plane to accomplish fg/transparent bg effect */
    for (i = 0; i < pGC->depth; i++)
    {
        rop = ((pGC->fgPixel >> i) & 0x01) ? alu_1 : alu_0;
        SET_RASTER_OP (i, rop, pPrivScreen);
    }

    for (i = 0; i < numClipRects; i++)
    {
        pbox = &clipRects[i];
        xMin = max(xOrg, pbox->x1);
        xMax = min(xOrg+dx, pbox->x2);
        yMin = max(yOrg, pbox->y1);
        yMax = min(yOrg+dy, pbox->y2);
        /* is there anything visible here? */
        if (xMax <= xMin || yMax <= yMin)
            continue;

        src_win.window_base.x_coord = xMin-xOrg;
        src_win.window_base.y_coord = yMin-yOrg;
        src_win.window_size.x_size = xMax-xMin;
        src_win.window_size.y_size = yMax-yMin;

        dest_origin.x_coord = xMin;
        dest_origin.y_coord = yMin;

        gpr_$additive_blt (tempBitmap, src_win, 0, dest_origin, &status);
    }

    if ((pGC->fillStyle) == FillOpaqueStippled)
    {
        /* XOR the stencil onto the temp */
        SET_BITMAP (tempBitmap, pPrivScreen);
        SET_PLANE_MASK (1, pPrivScreen);
        SET_RASTER_OP (0, GXxor, pPrivScreen);
        src_win.window_base.x_coord = src_win.window_base.y_coord = 0;
        src_win.window_size.x_size = dx;
        src_win.window_size.y_size = dy;
        dest_origin.x_coord = dest_origin.y_coord = 0;
        gpr_$pixel_blt (srcBitmap, src_win, dest_origin, &status);

        /* Go back to drawable and put the background pixels up */
        UPDATE_GPR (pGC, pPrivScreen);
        SET_CLIPPING_ACTIVE (false, pPrivScreen);   /* disable again after UPDATE_GPR */

        alu_0 = 4 | (((pGC->alu) >> 2) & 0x03);
        alu_1 = 4 | ((pGC->alu) & 0x03);

        /* Set a rop per plane to accomplish fg/transparent bg effect */
        for (i = 0; i < pGC->depth; i++)
        {
            rop = ((pGC->bgPixel >> i) & 0x01) ? alu_1 : alu_0;
            SET_RASTER_OP (i, rop, pPrivScreen);
        }

        for (i = 0; i < numClipRects; i++)
        {
            pbox = &clipRects[i];
            xMin = max(xOrg, pbox->x1);
            xMax = min(xOrg+dx, pbox->x2);
            yMin = max(yOrg, pbox->y1);
            yMax = min(yOrg+dy, pbox->y2);
            /* is there anything visible here? */
            if (xMax <= xMin || yMax <= yMin)
                continue;

            src_win.window_base.x_coord = xMin-xOrg;
            src_win.window_base.y_coord = yMin-yOrg;
            src_win.window_size.x_size = xMax-xMin;
            src_win.window_size.y_size = yMax-yMin;

            dest_origin.x_coord = xMin;
            dest_origin.y_coord = yMin;

            gpr_$additive_blt (tempBitmap, src_win, 0, dest_origin, &status);
        }
    }

    (* pGC->pScreen->DestroyPixmap)(pTempPixmap);
}

/*
 * apcPushPixelsTile -- DDX interface (GC "op" PushPixels)
 *      Squeegees the tile pattern of pGC through pBitMap into pDraw.
 *
 *      Strategy:  make a temp Pixmap, size equal to the stencil and depth equal to the
 *      drawable.  Fill the temp Pixmap with the tile (as above, aligned properly w.r.t.
 *      the destination, not the temp).  Then BLT the destination area of the drawable into
 *      the temp, with a funny raster op computed from the GC's alu value (see below for
 *      justification; in the usual case of GXcopy, this raster op will be GXxor).  Now
 *      use the stencil to zero all areas of the temp Pixmap other than the desired shape
 *      (i.e. AND the stencil onto it).  Finally, BLT the temp Pixmap back onto the drawable
 *      using XOR.  This will (1) not modify pixels other than the desired shape; and (2)
 *      magically draw the desired shape, using the tile and applying the desired raster op.
 *
 *      The funny raster op is computed from the desired one by XORing it with 5, and then
 *      switching its middle 2 bits.  Why does this work?  Basically, when you modify a raster
 *      op by XORing it with 5, you are really using the given raster op, but inverting its
 *      effect wherever the destination bit is a 1.  Where the destination bit is a 0, the effect
 *      is the same as with the given raster op.  Then, permuting the middle 2 bits amounts to
 *      transposing the truth table represented by the raster op, so that the roles of source and
 *      destination are switched.  This functional composition means that using the resulting
 *      raster op amounts to, "first switch the source and destination, then use the inverse
 *      of the original function if the destination is a 1."
 *
 *      Since in our case the BLT in question uses the ultimate "destination" bits as its source
 *      and the tile (the actual "source" bits) as its destination, the reversal is necessary.
 *      Then, considering the XORing with 5, the result will produce the bits we eventually want
 *      to put back into the drawable where the destination had 0 bits, and the inverse of the
 *      bits we eventually want to put back where the destination had 1s.  So the XOR we finish
 *      off with will produce the desired bits everywhere.
 *
 *      One wrinkle:  A full-depth temp bitmap can be big.  So, we keep its size to a certain
 *      limit, but always allocating it full-depth and full-width, so that it may be shorter in
 *      height than needed.  Then we iterate the above procedure vertically down the drawable as
 *      many times as necessary.  We take care to make the height a multiple of the tile height;
 *      this means we don't have to revalidate our tile-filling GC each time.
 *
 *      One further wrinkle:  if the original raster op ignores source (i.e. the tile), none of
 *      this crap is necessary.  (GXclear, GXnoop, GXinvert, GXset.)  We can just compute a raster
 *      op equal to 01ab where ab is the low-order 2 bits of the original raster op, and use
 *      the stencil as source directly.
 */
void
apcPushPixelsTile(pGC, pBitMap, pDraw, dx, dy, xOrg, yOrg)
    GCPtr       pGC;
    PixmapPtr   pBitMap;
    DrawablePtr pDraw;
    int         dx, dy, xOrg, yOrg;
{
    RegionPtr           pClip;
    int                 numClipRects;
    BoxPtr              clipRects;
    int                 xMin, xMax, yMin, yMax;
    int                 dy_temp, yOrg_temp;
    int                 xrelOrg, yrelOrg;
    int                 i;
    int                 temp_signed;
    BoxPtr              pbox;
    gpr_$bitmap_desc_t  srcBitmap, tempBitmap, dstBitmap;
    PixmapPtr           pTempPixmap;
    GCPtr               pGCTemp;
    xRectangle          whole_rect;
    gpr_$window_t       src_win;
    gpr_$position_t     dest_origin;
    gpr_$raster_op_t    rop;
    status_$t           status;
    apcPrivScreenPtr    pPrivScreen;

static XID gcvals[] = {
        GXcopy,     /* function */
        0,          /* plane mask (set dynamically) */
        FillTiled,  /* fill style */
        NULL,       /* tile (set dynamically) */
        0,          /* x pattern origin (set dynamically) */
        0           /* y pattern origin (set dynamically) */
        };


    /* If the destination isn't realized, this is easy */
    if (pDraw->type == DRAWABLE_WINDOW &&
        !((WindowPtr)pDraw)->realized)
        return;

    pClip = ((apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr))->pCompositeClip;
    if (REGION_NIL(pClip))
        return;

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    numClipRects = REGION_NUM_RECTS(pClip);
    clipRects = REGION_RECTS(pClip);

    srcBitmap = ((apcPrivPMPtr)(((PixmapPtr)pBitMap)->devPrivate.ptr))->bitmap_desc;

    switch (pGC->alu) {
    case GXclear:
    case GXnoop:
    case GXinvert:
    case GXset:
        rop = 4 | ((pGC->alu) & 0x03);
        SET_RASTER_OP_COMMON (rop, pPrivScreen, pDraw->depth);

        if (numClipRects > 1)
            SET_CLIPPING_ACTIVE (false, pPrivScreen);

        for (i = 0; i < numClipRects; i++)
        {
            pbox = &clipRects[i];
            xMin = max(xOrg, pbox->x1);
            xMax = min(xOrg+dx, pbox->x2);
            yMin = max(yOrg, pbox->y1);
            yMax = min(yOrg+dy, pbox->y2);
            /* is there anything visible here? */
            if (xMax <= xMin || yMax <= yMin)
                continue;

            src_win.window_base.x_coord = xMin-xOrg;
            src_win.window_base.y_coord = yMin-yOrg;
            src_win.window_size.x_size = xMax-xMin;
            src_win.window_size.y_size = yMax-yMin;

            dest_origin.x_coord = xMin;
            dest_origin.y_coord = yMin;

            gpr_$additive_blt (srcBitmap, src_win, 0, dest_origin, &status);
        }
        break;

    default:

        /* get relative origin and dest bitmap */
        xrelOrg = xOrg + pDraw->x;
        yrelOrg = yOrg + pDraw->y;
        if (pDraw->type == DRAWABLE_WINDOW)
        {
            apDisplayDataPtr    pDisp;

            pDisp = (apDisplayDataPtr) pDraw->pScreen->devPrivates[apDisplayScreenIndex].ptr;
            dstBitmap = pDisp->display_bitmap;
        }
        else
            dstBitmap = ((apcPrivPMPtr)(((PixmapPtr)pDraw)->devPrivate.ptr))->bitmap_desc;

#define BITSIZELIMIT 262144
        dy_temp = BITSIZELIMIT / (dx * (pGC->depth));
        /* Don't exceed total height */
        if (dy_temp > dy)
            dy_temp = dy;
        /* Be sure we cover at least one whole tile height */
        else if (dy_temp < pGC->tile.pixmap->height)
            dy_temp = pGC->tile.pixmap->height;
        /* Round down to even multiple of tile height */
        else
            dy_temp -= (dy_temp % pGC->tile.pixmap->height);

        /* Make temp bitmap and GC */
        pTempPixmap = (* pGC->pScreen->CreatePixmap)(pGC->pScreen, dx, dy_temp, pGC->depth);
        if (!pTempPixmap)
            return;
        tempBitmap = ((apcPrivPMPtr)(pTempPixmap->devPrivate.ptr))->bitmap_desc;
        pGCTemp = GetScratchGC (pGC->depth, pGC->pScreen);
        if (!pGCTemp)
        {
            (* pGC->pScreen->DestroyPixmap)(pTempPixmap);
            return;
        }

        /* Set up GC for tiling the temp */
        gcvals[1] = (XID) pGC->planemask;
        gcvals[3] = (XID) pGC->tile.pixmap;
        temp_signed = (pGC->patOrg.x + xrelOrg) % (pGC->tile.pixmap->width);
        if (temp_signed < 0) temp_signed += pGC->stipple->width;
        gcvals[4] = temp_signed;
        temp_signed = (pGC->patOrg.y + yrelOrg) % (pGC->tile.pixmap->height);
        if (temp_signed < 0) temp_signed += pGC->stipple->height;
        gcvals[5] = temp_signed;
        DoChangeGC (pGCTemp,
                    (GCFunction | GCPlaneMask | GCFillStyle | GCTile | GCTileStipXOrigin | GCTileStipYOrigin),
                    gcvals, TRUE);  /* last arg TRUE because tile is a pointer not a resource ID */
        ValidateGC (pTempPixmap, pGCTemp);
        whole_rect.x = whole_rect.y = 0;
        whole_rect.width = dx;

        /* Calculate bizarre transfrom of alu */
        rop = (pGC->alu) ^ 5;
        rop = (rop & 9) | ((rop & 4) >> 1) | ((rop & 2) << 1);

        yOrg_temp = yOrg;
        while (1)
        {
            UPDATE_GPR (pGCTemp, pPrivScreen);

            /* Fill the temp with the tile */
            whole_rect.height = dy_temp;
            (* pGCTemp->ops->PolyFillRect)(pTempPixmap, pGCTemp, 1, &whole_rect);

            /* Do the funny BLT of the destination onto the temp */
            SET_RASTER_OP_COMMON (rop, pPrivScreen, pGC->depth);

            for (i = 0; i < numClipRects; i++)
            {
                pbox = &clipRects[i];
                xMin = max(xOrg, pbox->x1);
                xMax = min(xOrg+dx, pbox->x2);
                yMin = max(yOrg_temp, pbox->y1);
                yMax = min(yOrg_temp+dy_temp, pbox->y2);
                /* is there anything visible here? */
                if (xMax <= xMin || yMax <= yMin)
                    continue;

                src_win.window_base.x_coord = xMin;
                src_win.window_base.y_coord = yMin;
                src_win.window_size.x_size = xMax-xMin;
                src_win.window_size.y_size = yMax-yMin;
                dest_origin.x_coord = xMin-xOrg;
                dest_origin.y_coord = yMin-yOrg;
                gpr_$pixel_blt (dstBitmap, src_win, dest_origin, &status);
            }

            /* Now AND the stencil onto the temp */
            SET_RASTER_OP_COMMON (GXand, pPrivScreen, pGC->depth);
            src_win.window_base.x_coord = 0;
            src_win.window_base.y_coord = yOrg_temp - yOrg;
            src_win.window_size.x_size = dx;
            src_win.window_size.y_size = dy_temp;
            dest_origin.x_coord = dest_origin.y_coord = 0;
            gpr_$additive_blt (srcBitmap, src_win, 0, dest_origin, &status);

            /* Now stomp this puppy onto the drawable using XOR */
            UPDATE_GPR (pGC, pPrivScreen);
            SET_CLIPPING_ACTIVE (false, pPrivScreen);
            SET_RASTER_OP_COMMON (GXxor, pPrivScreen, pGC->depth);
            for (i = 0; i < numClipRects; i++)
            {
                pbox = &clipRects[i];
                xMin = max(xOrg, pbox->x1);
                xMax = min(xOrg+dx, pbox->x2);
                yMin = max(yOrg_temp, pbox->y1);
                yMax = min(yOrg_temp+dy_temp, pbox->y2);
                /* is there anything visible here? */
                if (xMax <= xMin || yMax <= yMin)
                    continue;

                src_win.window_base.x_coord = xMin-xOrg;
                src_win.window_base.y_coord = yMin-yOrg_temp;
                src_win.window_size.x_size = xMax-xMin;
                src_win.window_size.y_size = yMax-yMin;
                dest_origin.x_coord = xMin;
                dest_origin.y_coord = yMin;
                gpr_$pixel_blt (tempBitmap, src_win, dest_origin, &status);
            }

            dy -= dy_temp;
            if (dy <= 0)
                break; /* done */

            yOrg_temp += dy_temp;
            if (dy_temp > dy)
                dy_temp = dy;
        }

        FreeScratchGC (pGCTemp);
        (* pGC->pScreen->DestroyPixmap)(pTempPixmap);
    }
}
