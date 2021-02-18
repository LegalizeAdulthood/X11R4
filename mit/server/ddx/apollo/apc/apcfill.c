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

/* Calls to gpr to support X server fill operations */

#include "apc.h"

/*
 * apcNaturalPFR -- DDX interface (GC "op" PolyFillRect)
 *      Do a PolyFillRect using GPR rectangle fills.
 *      This works for solid, or natural tiles/stipples.
 */
void
apcNaturalPFR(pDraw, pGC, nrect, prectInit)
    DrawablePtr pDraw;
    GCPtr       pGC;
    int         nrect;      /* number of rectangles to fill */
    xRectangle *prectInit;  /* Pointer to first rectangle to fill */
{
    status_$t           status;
    gpr_$window_t       wndw;
    RegionPtr           prgn;
    xRectangle         *prect;
    BoxPtr              pbox;
    BoxRec              box;
    int                 irect, ibox;
    short               xorg, yorg;
    apcPrivScreenPtr    pPrivScreen;

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    prgn = ((apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr))->pCompositeClip;

    if (REGION_NUM_RECTS(prgn) > 1)
        SET_CLIPPING_ACTIVE (false, pPrivScreen);

    xorg = pDraw->x;
    yorg = pDraw->y;

    for (irect=0, prect=prectInit; irect<nrect; irect++, prect++)
    {
        box.x1 = prect->x + xorg;
        box.y1 = prect->y + yorg;
        box.x2 = box.x1 + prect->width;
        box.y2 = box.y1 + prect->height;
        for (ibox=0, pbox=REGION_RECTS(prgn); ibox<REGION_NUM_RECTS(prgn); ibox++, pbox++)
        {
            if (box.y2 <= pbox->y1)
                break;
            if ((box.y1 >= pbox->y2)
                || (box.x1 >= pbox->x2)
                || (box.x2 <= pbox->x1))
                continue;
            /* got something */
            wndw.window_base.x_coord = max(box.x1, pbox->x1);
            wndw.window_base.y_coord = max(box.y1, pbox->y1);
            wndw.window_size.x_size  = min(box.x2, pbox->x2) - wndw.window_base.x_coord;
            wndw.window_size.y_size  = min(box.y2, pbox->y2) - wndw.window_base.y_coord;
            gpr_$rectangle (wndw, &status);
        }
    }
}

/*
 * apcUnnaturalPFR -- DDX interface (GC "op" PolyFillRect)
 *      Do a PolyFillRect using GPR bitblts (the painful way).
 *      This works for solid, or natural tiles/stipples.
 */
void
apcUnnaturalPFR(pDraw, pGC, nrect, prectInit)
    DrawablePtr pDraw;
    GCPtr       pGC;
    int         nrect;      /* number of rectangles to fill */
    xRectangle *prectInit;  /* Pointer to first rectangle to fill */
{
    status_$t           status;
    gpr_$bitmap_desc_t  sbm, dbm; /* src,dst bitmaps */
    short               xorgPM, yorgPM, xorg, yorg;
    PixmapPtr           tsp;
    BoxRec              myrect;
    Bool                DoubleBltFlag;
    Bool                tileflag;
    int                 xalign, yalign;
    RegionPtr           prgn;
    xRectangle         *prect;
    BoxPtr              pbox;
    BoxRec              box;
    int                 irect, ibox;
    apcPrivScreenPtr    pPrivScreen;

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    /*
     * The Double Blt method can only be used if the Destination pixel
     * is not referenced by the rop and the entire image is defined
     * by the pixmap.  FillStipple is not acceptable because it doesn't
     * touch pixels marked by a zero in the bitmap.
     */
    DoubleBltFlag = (((pGC->fillStyle == FillOpaqueStippled)
                      || (pGC->fillStyle == FillTiled))
                     && ((pGC->alu == 0)  || (pGC->alu == 3)
                      || (pGC->alu == 12) || (pGC->alu == 15)));

    prgn = ((apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr))->pCompositeClip;

    if (REGION_NUM_RECTS(prgn) > 1)
        SET_CLIPPING_ACTIVE (false, pPrivScreen);

    /*
     * Convert fg/bg/rop values for a bitmap to a rop.  This techinque
     * is copied from DoCopy (apcbitblt.c).  If the fillstyle is
     * FillStippled then reset the background rop to a NOOP.
     * FillStippled only affects pixel set to one, in the bitmap.
     */
    tileflag = pGC->fillStyle == FillTiled;
    if (tileflag)
        tsp = pGC->tile.pixmap;
    else
    {
        int                 tfg, tbg;
        short               alu_bg0, alu_bg1, alu_fg0, alu_fg1;
        int                 i;
        gpr_$raster_op_t    rop;

        tsp = pGC->stipple;

        alu_bg0 = (pGC->alu) & 0x0c;
        alu_fg0 = alu_bg0 >> 2;
        alu_fg1 = (pGC->alu) & 0x03;
        alu_bg1 = alu_fg1 << 2;
        if (pGC->fillStyle == FillStippled)
            /* background transparent */
            alu_bg0 = alu_bg1 = 0x04;

        /* Set a rop per plane to accomplish fg/bg effect */
        for (i = 0; i < pGC->depth; i++)
        {
            tfg = (pGC->fgPixel >> i) & 0x01;
            tbg = (pGC->bgPixel >> i) & 0x01;
            rop = tbg ? alu_bg1 : alu_bg0;
            rop |= tfg ? alu_fg1 : alu_fg0;
            SET_RASTER_OP (i, rop, pPrivScreen);
        }
    }

    sbm = ((apcPrivPMPtr)tsp->devPrivate.ptr)->bitmap_desc;

    /*
     * Calculate the absolute origin of the window and tile/stipple.  The
     * origin of the window is used to convert the rectangle into abs
     * coordinates.  The abs location of tile/stipple is used to determine
     * if the tile must be aligned.
     */
    if (pDraw->type == DRAWABLE_WINDOW)
    {
        apDisplayDataPtr    pDisp;

        pDisp = (apDisplayDataPtr) pDraw->pScreen->devPrivates[apDisplayScreenIndex].ptr;
        dbm = pDisp->display_bitmap;
    }
    else
        dbm = ((apcPrivPMPtr)(((PixmapPtr)pDraw)->devPrivate.ptr))->bitmap_desc;

    xorg = pDraw->x;
    yorg = pDraw->y;
    xorgPM = xorg + pGC->patOrg.x;
    yorgPM = yorg + pGC->patOrg.y;

    for (irect=0, prect=prectInit; irect<nrect; irect++, prect++)
    {
        box.x1 = prect->x + xorg;
        box.y1 = prect->y + yorg;
        box.x2 = box.x1 + prect->width;
        box.y2 = box.y1 + prect->height;
        for (ibox=0, pbox=REGION_RECTS(prgn); ibox<REGION_NUM_RECTS(prgn); ibox++, pbox++)
        {
            if (box.y2 <= pbox->y1)
                break;
            if ((box.y1 >= pbox->y2)
                || (box.x1 >= pbox->x2)
                || (box.x2 <= pbox->x1))
                continue;
            /* got something */
            myrect.x1 = max(box.x1, pbox->x1);
            myrect.y1 = max(box.y1, pbox->y1);
            myrect.x2 = min(box.x2, pbox->x2);
            myrect.y2 = min(box.y2, pbox->y2);
            /*
             * Determine the tile alignment.
             */
            xalign = (myrect.x1 - xorgPM) % tsp->width;
            yalign = (myrect.y1 - yorgPM) % tsp->height;
            /* HACK ALERT require sign of remainder is sign of divisor */
            if (xalign < 0) xalign += tsp->width;
            if (yalign < 0) yalign += tsp->height;

            if (DoubleBltFlag)
                apcDoubleBlt (sbm, dbm, pDraw->depth, xalign, yalign, tsp->width, tsp->height,
                              &myrect, tileflag, pPrivScreen);
            else
                apcRepTileBlt (sbm, xalign, yalign, tsp->width, tsp->height,
                               &myrect, tileflag);
        }
    }
}

/*
 * apcRepTileBlt -- Driver internal code
 *      Fill a rectangle with a tile/stipple.  Nothing fancy, just loop
 *      in the x direction and y direction and blt the tile/stipple.
 *
 *      Uses GPR clipping, plane mask, raster op set by caller.
 */
void
apcRepTileBlt(sbm,xalign,yalign,width,height,prect,tileflag)
    gpr_$bitmap_desc_t  sbm;            /* Tile/Stipple bitmap */
    int                 xalign, yalign; /* align values for the bitmap */
    int                 width, height;  /* bitmap dimensions */
    BoxPtr              prect;          /* Rectangle to fill (abs coords) */
    Bool                tileflag;       /* 1 = tile, 0 = stipple */
{
    status_$t       status;
    gpr_$window_t   sw;
    gpr_$position_t dp;
    short           x0, y0, xi, yi, dx, dy;

    x0 = prect->x1 - xalign;
    y0 = prect->y1 - yalign;

    for (yi=y0; yi<prect->y2; yi+=height)
        for (xi=x0; xi<prect->x2; xi+=width)
        {
            sw.window_base.x_coord = 0;
            sw.window_base.y_coord = 0;
            sw.window_size.x_size = width;
            sw.window_size.y_size = height;
            dp.x_coord = xi;
            dp.y_coord = yi;

            dx = prect->x1 - xi;
            if (dx > 0)
            {
                sw.window_base.x_coord += dx;
                sw.window_size.x_size  -= dx;
                dp.x_coord += dx;
            }

            dy = prect->y1 - yi;
            if (dy > 0)
            {
                sw.window_base.y_coord += dy;
                sw.window_size.y_size  -= dy;
                dp.y_coord += dy;
            }

            dx = xi + width - prect->x2;
            if (dx > 0)
                sw.window_size.x_size -= dx;

            dy = yi + height - prect->y2;
            if (dy > 0)
                sw.window_size.y_size -= dy;

            if (tileflag)
                gpr_$pixel_blt (sbm, sw, dp, &status);
            else
                gpr_$additive_blt (sbm, sw, (gpr_$plane_t)0, dp, &status);
        }
}

/*
 * This macro is used by apcDoubleBlt.
 * It sets up the structures for gpr and calls the desired blt routine.
 */
#define doBltTS(sbm, sx1, sy1, swidth, sheight, dx1, dy1, dx2, dy2, tileflag) \
    if ((dy1 < dy2) && (dx1 < dx2) && (swidth > 0) && (sheight > 0)) \
    { \
        gpr_$window_t   swin; \
        gpr_$position_t dpos; \
        status_$t       status; \
        swin.window_base.x_coord = (short) sx1; \
        swin.window_base.y_coord = (short) sy1; \
        swin.window_size.x_size = min(dx1 + swidth,  dx2) - dx1; \
        swin.window_size.y_size = min(dy1 + sheight, dy2) - dy1; \
        dpos.x_coord = (short) dx1; \
        dpos.y_coord = (short) dy1; \
        if (tileflag) \
            gpr_$pixel_blt (sbm, swin, dpos, &status); \
        else  \
            gpr_$additive_blt (sbm, swin, (gpr_$plane_t)0, dpos, &status); \
    } \
    else /* ; */

/*
 * apcDoubleBlt -- Driver internal code
 *      Fill a rectangle with a tile/stipple.  Use doubling blts.
 *
 *      Uses GPR clipping, plane mask, raster op set by caller.
 *      (May change raster op to copy.)
 */
void
apcDoubleBlt(sbm, dbm, depth, xalign, yalign, width, height, prect, tileflag, pPrivScreen)
    gpr_$bitmap_desc_t  sbm;            /* source bitmap */
    gpr_$bitmap_desc_t  dbm;            /* destination bitmap */
    int                 depth;          /* depth (of both) */
    int                 xalign, yalign; /* alignment values for source bitmap */
    int                 width, height;  /* source bitmap dimensions */
    BoxPtr              prect;          /* box to fill in dbm */
    Bool                tileflag;       /* TRUE: tile; FALSE: opaque stipple */
    apcPrivScreenPtr    pPrivScreen;    /* used to change raster ops to copy */
{
    status_$t               status;
    gpr_$window_t           swin;
    gpr_$position_t         dpos;
    int                     dwidth, dheight;
    int                     i;

    /*
     * Align the source in the rectangle defined by prect. This will
     * become the seed which is replicated using doubling blts.
     *
     * blt tile to screen (Step 1).  Determine if the tile is aligned.
     * If it is, blt it to the screen.  If not, calculate the
     * new origin of the tile.  The tile is separated into four
     * sections and then copied to create the new tile.
     *
     *  Tile                    Destination
     *
     *  -------------           ------------
     *  |  1     2  |           |  4     3 |
     *  |     *     |           |          |
     *  |  3  ^  4  |           |  2     1 |
     *  ------|------           ------------
     *        |--------Start location for tile.
     */

    if (!xalign && !yalign)
        doBltTS(sbm, 0, 0, width, height,
                prect->x1, prect->y1,
                prect->x2, prect->y2, tileflag);
    else
    {
        /*
         * bottom right of tile to top left of dest
         */
        doBltTS(sbm, xalign, yalign,
                (width - xalign), (height - yalign),
                prect->x1, prect->y1,
                prect->x2, prect->y2, tileflag);
        /*
         * top right of tile to bottom left of dest
         */
        doBltTS(sbm, xalign, 0, (width - xalign), yalign,
                prect->x1, (prect->y1 + (height - yalign)),
                prect->x2, prect->y2, tileflag);
        /*
         * bottom left of tile to top right of dest
         */
        if (xalign)
            doBltTS(sbm, 0, yalign, xalign, (height - yalign),
                    (prect->x1 + (width - xalign)), prect->y1,
                    prect->x2, prect->y2, tileflag);
        /*
         * top left of tile to bottom right of dest
         */
        if (yalign)
            doBltTS(sbm, 0, 0, xalign, yalign,
                    (prect->x1 + (width - xalign)),
                    (prect->y1 + (height - yalign)),
                    prect->x2, prect->y2, tileflag);
    }

    dwidth  = prect->x2 - prect->x1;
    dheight = prect->y2 - prect->y1;

    if ((width >= dwidth) && (height >= dheight))
        return; /* The seed covered everything */

    /*
     * Use a doubling blt method to fill a rectangle.
     *  1)  Blt the first tile (top left corner) in the positive
     *      X direction.  Now blt the width of both the tiles, just
     *      created in the positive X direction.  Continue until
     *      the end of the visible rect is reached.
     *  2)  Use the technique described in step two except in the
     *      positive Y direction.
     * This code assumes that the pattern is correctly aligned in the
     * top left corner of the clip rectangle.
     */

    SET_RASTER_OP_COMMON (GXcopy, pPrivScreen, depth);

    /*
     * Initialize values for double blt
     */
    swin.window_base.x_coord = prect->x1;
    swin.window_base.y_coord = prect->y1;
    swin.window_size.x_size = min(width,  dwidth);
    swin.window_size.y_size = min(height, dheight);

    /*
     * duplicate the tile in the X direction until the end is reached
     */
    if (width < dwidth)
    {
        dpos.x_coord = swin.window_base.x_coord + swin.window_size.x_size;
        dpos.y_coord = prect->y1;
        while ((dpos.x_coord + swin.window_size.x_size) <= prect->x2)
        {
            gpr_$pixel_blt (dbm, swin, dpos, &status);
            swin.window_size.x_size <<= 1;
            dpos.x_coord = swin.window_base.x_coord + swin.window_size.x_size;
        }
        if (swin.window_size.x_size = prect->x2 - dpos.x_coord)
            gpr_$pixel_blt (dbm, swin, dpos, &status);
        swin.window_size.x_size = dwidth; /* now it is */
    }

    /*
     * duplicate the tile in the Y direction until the end is reached
     */
    if (height < dheight)
    {
        dpos.x_coord = prect->x1;
        dpos.y_coord = swin.window_base.y_coord + swin.window_size.y_size;
        while ((dpos.y_coord + swin.window_size.y_size) <= prect->y2)
        {
            gpr_$pixel_blt (dbm, swin, dpos, &status);
            swin.window_size.y_size <<= 1;
            dpos.y_coord = swin.window_base.y_coord + swin.window_size.y_size;
        }
        if (swin.window_size.y_size = prect->y2 - dpos.y_coord)
            gpr_$pixel_blt (dbm, swin, dpos, &status);
    }
}
