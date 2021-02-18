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
#include "apcmskbits.h"

#define AP_NPOINTS 1024
#define AP_NTRAPS 320

/*
 * apcSolidFS -- DDX interface (GC "op" FillSpans)
 *      Solid case of FillSpans.
 *      For each span fill it according to the contents of the GC.
 *      If fSorted is TRUE, the scanlines are in increasing Y order.
 *      Source bit lines are server scanline padded so that they always begin
 *      on a word boundary.
 */
void
apcSolidFS(pDraw, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDraw;
    GCPtr       pGC;
    int         nInit;      /* number of spans to fill */
    DDXPointPtr pptInit;    /* pointer to list of start points */
    int        *pwidthInit; /* pointer to list of n widths */
    int         fSorted;
{
    status_$t           status;
    gpr_$coordinate_t   x[AP_NPOINTS], y[AP_NPOINTS];
    short               i;
    RegionPtr           prgn;
    DDXPointPtr         pptLast;    /* These 5 parameters used to clip */
    BoxPtr              pbox, pboxLast;
    BoxPtr              pboxTest;
    int                 yMax;
    apcPrivScreenPtr    pPrivScreen;

    if (!(pGC->planemask))
        return;

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    prgn = ((apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr))->pCompositeClip;

    if (REGION_NUM_RECTS(prgn) > 1)
        SET_CLIPPING_ACTIVE (false, pPrivScreen);

    pptLast = pptInit + nInit;
    pboxTest = REGION_RECTS(prgn);
    pboxLast = pboxTest + REGION_NUM_RECTS(prgn);
    yMax = prgn->extents.y2;

    for (i = 0; pptInit < pptLast; pwidthInit++, pptInit++)
    {
        if (fSorted)
        {
            if (pptInit->y >= yMax)
                break;
            pbox = pboxTest;
        }
        else
        {
            if (pptInit->y >= yMax)
                continue;
            pbox = REGION_RECTS(prgn);
        }

        if (*pwidthInit == 0)
            continue;

        while (pbox < pboxLast)
        {
            if (pbox->y1 > pptInit->y)
                /* scanline is before clip box */
                break;
            else if (pbox->y2 <= pptInit->y)
            {
                /* clip box is before scanline */
                pboxTest = ++pbox;
                continue;
            }
            else if (pbox->x1 >= pptInit->x + *pwidthInit)
                /* clip box is to right of scanline */
                break;
            else if (pbox->x2 <= pptInit->x)
            {
                /* scanline is to right of clip box */
                pbox++;
                continue;
            }

            /* at least some of the scanline is in the current clip box */
            x[i] = max(pbox->x1, pptInit->x);
            y[i++] = pptInit->y;
            x[i] = min(pptInit->x + *pwidthInit, pbox->x2) - 1;
            y[i++] = pptInit->y;
            if (i > AP_NPOINTS - 2 )    /* each line requires 2 endpoints */
            {
                gpr_$multiline (x, y, i, &status);
                i = 0;
            }
            pbox++;
            if (pptInit->x + *pwidthInit <= pbox->x2)
                break;  /* We hit the end of the line */
        }   /* End of while loop */
    }   /* End of for loop */

    if (i > 0)      /* Process leftovers */
        gpr_$multiline (x, y, i, &status);
}

/*
 * apcNaturalFS -- DDX interface (GC "op" FillSpans)
 *      Natural tile/stipple case of FillSpans.
 *      For each span fill it according to the contents of the GC.
 *      If fSorted is TRUE, the scanlines are in increasing Y order.
 *      Source bit lines are server scanline padded so that they always begin
 *      on a word boundary.
 */
void
apcNaturalFS(pDraw, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDraw;
    GCPtr       pGC;
    int         nInit;      /* number of spans to fill */
    DDXPointPtr pptInit;    /* pointer to list of start points */
    int        *pwidthInit; /* pointer to list of n widths */
    int         fSorted;
{
    status_$t           status;
    gpr_$trap_t         buffer[AP_NTRAPS];
    short               i;
    Bool                gotone = FALSE;
    BoxRec              oldbox, newbox;
    RegionPtr           prgn;
    DDXPointPtr         pptLast;    /* These 5 parameters used to clip */
    BoxPtr              pbox, pboxLast;
    BoxPtr              pboxTest;
    int                 yMax;
    apcPrivScreenPtr    pPrivScreen;

    if (!(pGC->planemask))
        return;

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    prgn = ((apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr))->pCompositeClip;

    /* disable clipping - we always do it and traps are hard in general to clip */
    SET_CLIPPING_ACTIVE (false, pPrivScreen);
    /* multitrapezoid requires this setting */
    SET_PGON_DECOMP_TECHNIQUE (gpr_$fast_traps, pPrivScreen);

    pptLast = pptInit + nInit;
    pboxTest = REGION_RECTS(prgn);
    pboxLast = pboxTest + REGION_NUM_RECTS(prgn);
    yMax = prgn->extents.y2;

    for (i = 0; pptInit < pptLast; pwidthInit++, pptInit++)
    {
        if (fSorted)
        {
            if (pptInit->y >= yMax)
                break;
            pbox = pboxTest;
        }
        else
        {
            if (pptInit->y >= yMax)
                continue;
            pbox = REGION_RECTS(prgn);
        }

        if (*pwidthInit == 0)
            continue;

        while (pbox < pboxLast)
        {
            if (pbox->y1 > pptInit->y)
                /* scanline is before clip box */
                break;
            else if (pbox->y2 <= pptInit->y)
            {
                /* clip box is before scanline */
                pboxTest = ++pbox;
                continue;
            }
            else if (pbox->x1 >= pptInit->x + *pwidthInit)
                /* clip box is to right of scanline */
                break;
            else if (pbox->x2 <= pptInit->x)
            {
                /* scanline is to right of clip box */
                pbox++;
                continue;
            }

            /* at least some of the scanline is in the current clip box */
            newbox.x1 = max(pbox->x1, pptInit->x);
            newbox.y1 = pptInit->y;
            newbox.x2 = min(pptInit->x + *pwidthInit, pbox->x2);
            newbox.y2 = newbox.y1 + 1;
            pbox++;

            if (gotone)
            {
                if ( (newbox.y1 == oldbox.y2)
                     && (newbox.x1 == oldbox.x1)
                     && (newbox.x2 == oldbox.x2))
                    oldbox.y2++;
                else
                {
                    buffer[i].bot.x_coord_l = buffer[i].top.x_coord_l = oldbox.x1;
                    buffer[i].bot.x_coord_r = buffer[i].top.x_coord_r = oldbox.x2 - 1;
                    buffer[i].top.y_coord   = oldbox.y1;
                    buffer[i].bot.y_coord   = oldbox.y2 - 1;
                    i++;
                    if (i > AP_NTRAPS - 1)    /* each rect needs 1 trap */
                    {
                        gpr_$multitrapezoid (buffer, i, &status);
                        i = 0;
                    }
                    oldbox = newbox;
                }
            }
            else
            {
                oldbox = newbox;
                gotone = TRUE;
            }

            if (pptInit->x + *pwidthInit <= pbox->x2)
                break;    /* We hit the end of the line */
        }   /* End of while loop */
    }   /* End of for loop */

    if (gotone)
    {
        buffer[i].bot.x_coord_l = buffer[i].top.x_coord_l = oldbox.x1;
        buffer[i].bot.x_coord_r = buffer[i].top.x_coord_r = oldbox.x2 - 1;
        buffer[i].top.y_coord   = oldbox.y1;
        buffer[i].bot.y_coord   = oldbox.y2 - 1;
        i++;
    }

    if (i > 0)
        gpr_$multitrapezoid (buffer, i, &status);
}

/*
 * apcUnnaturalFS -- DDX interface (GC "op" FillSpans)
 *      Unnatural tile/stipple case of FillSpans.
 *      For each span fill it according to the contents of the GC.
 *      If fSorted is TRUE, the scanlines are in increasing Y order.
 *      Source bit lines are server scanline padded so that they always begin
 *      on a word boundary.
 */
void
apcUnnaturalFS(pDraw, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDraw;
    GCPtr       pGC;
    int         nInit;      /* number of spans to fill */
    DDXPointPtr pptInit;    /* pointer to list of start points */
    int        *pwidthInit; /* pointer to list of n widths */
    int         fSorted;
{
    status_$t           status;
    gpr_$bitmap_desc_t  sbm, dbm; /* src,dst bitmaps */
    PixmapPtr           tsp;
    int                 xorgPM, yorgPM;
    Bool                DoubleBltFlag;
    Bool                tileflag;
    Bool                gotone = FALSE;
    BoxRec              oldbox, newbox;
    int                 xalign, yalign;
    RegionPtr           prgn;
    DDXPointPtr         pptLast;    /* These 5 parameters used to clip */
    BoxPtr              pbox, pboxLast;
    BoxPtr              pboxTest;
    int                 yMax;
    apcPrivScreenPtr    pPrivScreen;

    if (!(pGC->planemask))
        return;

    /* draw solid lines if really opaque */
    if ((pGC->fillStyle == FillOpaqueStippled)
        && (pGC->fgPixel == pGC->bgPixel))
    {
        apcSolidFS(pDraw, pGC, nInit, pptInit, pwidthInit, fSorted);
        return;
    }

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    prgn = ((apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr))->pCompositeClip;

    /*
     * The Double Blt method can only be used if the Destination pixel
     * are not referenced by the rop and the entire image is defined
     * by the pixmap.  FillStipple is not acceptable because it doesn't
     * touch pixel marked by a zero in the bitmap.
     */
    DoubleBltFlag = (((pGC->fillStyle == FillOpaqueStippled)
                      || (pGC->fillStyle == FillTiled))
                     && ((pGC->alu == 0)  || (pGC->alu == 3)
                      || (pGC->alu == 12) || (pGC->alu == 15)));

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
     * Calculate the absolute origin of the tile/stipple. The origin
     * is used to determine if the tile must be aligned.
     */
    xorgPM = pGC->patOrg.x + pDraw->x;
    yorgPM = pGC->patOrg.y + pDraw->y;
    if (pDraw->type == DRAWABLE_WINDOW)
    {
        apDisplayDataPtr    pDisp;

        pDisp = (apDisplayDataPtr) pDraw->pScreen->devPrivates[apDisplayScreenIndex].ptr;
        dbm = pDisp->display_bitmap;
    }
    else
        dbm = ((apcPrivPMPtr)(((PixmapPtr)pDraw)->devPrivate.ptr))->bitmap_desc;

    pptLast = pptInit + nInit;
    pboxTest = REGION_RECTS(prgn);
    pboxLast = pboxTest + REGION_NUM_RECTS(prgn);
    yMax = prgn->extents.y2;

    for (; pptInit < pptLast; pwidthInit++, pptInit++)
    {
        if (fSorted)
        {
            if (pptInit->y >= yMax)
                break;
            pbox = pboxTest;
        }
        else
        {
            if (pptInit->y >= yMax)
                continue;
            pbox = REGION_RECTS(prgn);
        }

        if (*pwidthInit == 0)
            continue;

        while (pbox < pboxLast)
        {
            if (pbox->y1 > pptInit->y)
                /* scanline is before clip box */
                break;
            else if (pbox->y2 <= pptInit->y)
            {
                /* clip box is before scanline */
                pboxTest = ++pbox;
                continue;
            }
            else if (pbox->x1 >= pptInit->x + *pwidthInit)
                /* clip box is to right of scanline */
                break;
            else if (pbox->x2 <= pptInit->x)
            {
                /* scanline is to right of clip box */
                pbox++;
                continue;
            }

            /* at least some of the scanline is in the current clip box */
            newbox.x1 = max(pbox->x1, pptInit->x);
            newbox.y1 = pptInit->y;
            newbox.x2 = min(pptInit->x + *pwidthInit, pbox->x2);
            newbox.y2 = newbox.y1 + 1;
            pbox++;

            if (gotone)
            {
                if ( (newbox.y1 == oldbox.y2)
                     && (newbox.x1 == oldbox.x1)
                     && (newbox.x2 == oldbox.x2))
                    oldbox.y2++;
                else
                {
                    /*
                     * Determine the tile alignment.
                     */
                    xalign = (oldbox.x1 - xorgPM) % tsp->width;
                    yalign = (oldbox.y1 - yorgPM) % tsp->height;
                    /* HACK ALERT require sign of remainder is sign of divisor */
                    if (xalign < 0) xalign += tsp->width;
                    if (yalign < 0) yalign += tsp->height;

                    if (DoubleBltFlag)
                        apcDoubleBlt (sbm, dbm, xalign, yalign, tsp->width, tsp->height,
                                      &oldbox, tileflag, pPrivScreen);
                    else
                        apcRepTileBlt (sbm, xalign, yalign, tsp->width, tsp->height,
                                       &oldbox, tileflag);
                    oldbox = newbox;
                }
            }
            else
            {
                oldbox = newbox;
                gotone = TRUE;
            }

            if (pptInit->x + *pwidthInit <= pbox->x2)
                break;  /* We hit the end of the line */
        }   /* End of while loop */
    }   /* End of for loop */

    if (gotone)
    {
        /*
         * Determine the tile alignment.
         */
        xalign = (oldbox.x1 - xorgPM) % tsp->width;
        yalign = (oldbox.y1 - yorgPM) % tsp->height;
        /* HACK ALERT require sign of remainder is sign of divisor */
        if (xalign < 0) xalign += tsp->width;
        if (yalign < 0) yalign += tsp->height;

        if (DoubleBltFlag)
            apcDoubleBlt (sbm, dbm, xalign, yalign, tsp->width, tsp->height,
                          &oldbox, tileflag, pPrivScreen);
        else
            apcRepTileBlt (sbm, xalign, yalign, tsp->width, tsp->height,
                           &oldbox, tileflag);
    }
}
