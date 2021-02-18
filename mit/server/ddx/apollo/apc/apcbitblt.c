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
#include "mi.h"

/*
 * Sortrects -- Driver internal code
 *      Sort list of clip rectangles, depending on blt direction.
 */
static void
Sortrects(order, pClip, dstx, dsty, Box)
    unsigned int   *order;
    RegionPtr       pClip;
    int             dstx, dsty;
    BoxRec          Box;
{
    int     xMax, i, j, y, yMin, yMax;
    int     clipNumRects;
    BoxPtr  clipRects;

    clipNumRects = REGION_NUM_RECTS(pClip);
    clipRects = REGION_RECTS(pClip);

    if (dsty <= Box.y1)
    {   /* Scroll up or stationary vertical.  Vertical order OK */
        if (dstx <= Box.x1)
            /* Scroll left or stationary horizontal.  Horizontal order OK as well */
            for (i=0; i < clipNumRects; i++)
                order[i] = i;
        else
        {   /* scroll right. must reverse horizontal banding of rects. */
            for (i=0, j=1, xMax=0; i < clipNumRects; j=i+1, xMax=i)
            {
                /* find extent of current horizontal band */
                y = clipRects[i].y1;    /* band has this y coordinate */
                while ((j < clipNumRects) && (clipRects[j].y1 == y))
                    j++;
                /* reverse the horizontal band in the output ordering */
                for (j-- ; j >= xMax; j--, i++)
                    order[i] = j;
            }
        }
    }
    else
    {   /* Scroll down. Must reverse vertical banding. */
        if (dstx < Box.x1)
        {   /* Scroll left. Horizontal order OK. */
            for (i=clipNumRects-1, j=i-1, yMin=i, yMax=0; i >= 0; j=i-1, yMin=i)
            {
                /* find extent of current horizontal band */
                y = clipRects[i].y1;    /* band has this y coordinate */
                while ((j >= 0) && (clipRects[j].y1 == y))
                    j--;
                /* reverse the horizontal band in the output ordering */
                for (j++ ; j <= yMin; j++, i--, yMax++)
                    order[yMax] = j;
            }
        }
        else
            /* Scroll right or horizontal stationary.
               Reverse horizontal order as well (if stationary, horizontal
               order can be swapped without penalty and this is faster
               to compute). */
            for (i=0, j=clipNumRects-1; i < clipNumRects; i++, j--)
                order[i] = j;
    }
}

/*
 * doCopy -- Driver internal code
 *      Do the real work for apcCopyArea, apcCopyPlane and the XY
 *      format case of apcPutImage.
 */
static RegionPtr
doCopy(pDrawSrc, pDraw, pGC, pPrivScreen,
       xIn, yIn, widthSrc, heightSrc, xOut, yOut,
       bitPlane, doExposures, x_to_gpr)
    DrawablePtr         pDrawSrc;
    DrawablePtr         pDraw;
    GCPtr               pGC;
    apcPrivScreenPtr    pPrivScreen;
    int                 xIn, yIn;
    int                 widthSrc, heightSrc;
    int                 xOut, yOut;
    unsigned long       bitPlane;       /* 0 means do CopyArea, else do
                                           CopyPlane from plane given by
                                           single bit set in this value */
    Bool                doExposures;    /* true if CopyArea or CopyPlane,
                                           false if PutImage */
    int                 x_to_gpr;       /* 0 means do not convert,
                                           else convert the XY plane order
                                           from X to gpr. If non-zero,
                                           x_to_gpr will contain the depth. */
{
    BoxRec              srcBox, *prect;
                        /* may be a new region, or just a copy */
    RegionPtr           prgnSrcClip, prgnDstClip;
                        /* non-0 if we've created a src clip */
    int                 realSrcClip = 0;
    int                 srcNumRects, dstNumRects;
    BoxPtr              srcRects, dstRects;
    RegionPtr           prgnExposed;
    int                 srcx, srcy, dstx, dsty, i, j, width, height,
                        xMin, xMax, yMin, yMax;
    unsigned int        *srcordering, *dstordering;
    int                 x1,x2,y1,y2;
    BoxPtr              pbox;
    gpr_$bitmap_desc_t  srcBitmap;
    gpr_$window_t       src_win;
    gpr_$position_t     dest_origin;
    status_$t           status;
    int                 tfg, tbg;
    short               alu_bg0, alu_bg1, alu_fg0, alu_fg1;
    gpr_$raster_op_t    rop;
    gpr_$rgb_plane_t    plane_num;
    gpr_$window_t       window;

    /* clip the left and top edges of the source */
    if (xIn < 0)
    {
        widthSrc += xIn;
        xOut -= xIn;
        srcx = 0;
    }
    else
        srcx = xIn;
    if (yIn < 0)
    {
        heightSrc += yIn;
        yOut -= yIn;
        srcy = 0;
    }
    else
        srcy = yIn;


    /* If the destination isn't realized, this is easy */
    if (pDraw->type == DRAWABLE_WINDOW &&
        !((WindowPtr)pDraw)->realized)
        return NULL;
    else
    {
        /* clip the source */
        if (pDrawSrc->type == DRAWABLE_PIXMAP)
        {
            BoxRec  box;

            box.x1 = 0;
            box.y1 = 0;
            box.x2 = pDrawSrc->width;
            box.y2 = pDrawSrc->height;

            prgnSrcClip = (*pGC->pScreen->RegionCreate)(&box, 1);
            realSrcClip = 1;
            srcBitmap = ((apcPrivPMPtr)(((PixmapPtr)pDrawSrc)->devPrivate.ptr))->bitmap_desc;
        }
        else
        {
            apDisplayDataPtr    pDisp;

            pDisp = (apDisplayDataPtr) pDrawSrc->pScreen->devPrivates[apDisplayScreenIndex].ptr;
            srcBitmap = pDisp->display_bitmap;
            srcx += pDrawSrc->x;
            srcy += pDrawSrc->y;
            if (pGC->subWindowMode == IncludeInferiors)
            {
                prgnSrcClip = NotClippedByChildren((WindowPtr)pDrawSrc);
                realSrcClip = 1;
            }
            else
            {
                prgnSrcClip = &((WindowPtr)pDrawSrc)->clipList;
            }
        }

        prgnDstClip = ((apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr))->pCompositeClip;

        if (REGION_NIL(prgnDstClip) || REGION_NIL(prgnSrcClip))
            goto cleanup;

        /* If the src drawable is a window, we need to translate the srcBox so
         * that we can compare it with the window's clip region later on. */
        srcBox.x1 = srcx;
        srcBox.y1 = srcy;
        srcBox.x2 = srcx + widthSrc;
        srcBox.y2 = srcy + heightSrc;

        dstx = xOut + pDraw->x;
        dsty = yOut + pDraw->y;

        srcordering = NULL; /* assume source is in order */
        dstordering = NULL; /* assume destination is in order */

        srcNumRects = REGION_NUM_RECTS(prgnSrcClip);
        dstNumRects = REGION_NUM_RECTS(prgnDstClip);

        /* If not the same drawable then order of move doesn't matter.
           Following assumes that prgnSrcClip->rects are sorted from top
           to bottom and left to right.
        */
        if (pDrawSrc == pDraw)
        {
            if (REGION_RECTS(prgnSrcClip) != REGION_RECTS(prgnDstClip))
            {
                /* different list of clip rects for source and dest.
                   Must sort both of these. Do dest here, and default
                   to always sorting source.*/

                if (dstNumRects > 1)
                {
                    dstordering = (unsigned int *)
                        ALLOCATE_LOCAL(dstNumRects * sizeof(unsigned int));
                    Sortrects (dstordering, prgnDstClip, dstx, dsty, srcBox);
                }
            }

            if (srcNumRects > 1)
            {
                srcordering = (unsigned int *)
                    ALLOCATE_LOCAL(srcNumRects * sizeof(unsigned int));
                Sortrects (srcordering, prgnSrcClip, dstx, dsty, srcBox);
            }
        }

        if (dstNumRects > 1)
            SET_CLIPPING_ACTIVE (false, pPrivScreen);

        /*
         * Set raster-op for each plane to allow an additive_blt from a
         * one deep XYbitmap to N deep pixmap/screen, taking into account
         * foreground and background.  The first step is to separate the
         * raster-op into two sections, one which affects a 0 source value and
         * the other with affects a 1 source value (Raster Table).  Each
         * value is then shifted to create the other two values.  The next
         * step is to combine the ops created in step 1 into the a new op
         * depending on the foreground and background value.  The
         * table below list the options for the source pixel.
         *
         *      fg      bg      Result  Action      Technique
         *      0       1       0 -> 1  !src        Swith the bytes
         *                      1 -> 0
         *      1       0       1 -> 1  Identity    Same raster-op
         *                      0 -> 0
         *      1       1       1       Replace     copy low byte (src = 1)
         *                             src with 1   over high byte (src = 0)
         *      0       0       0       Replace     copy high byte (src = 0)
         *                             src with 0   over low byte (src = 1)
         *
         *                      Raster Table
         *
         *  src dst     0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
         *----------------------------------------------------------------
         *   0   0      0  0  0  0  0  0  0  0  1  1  1  1  1  1  1  1
         *   0   1      0  0  0  0  1  1  1  1  0  0  0  0  1  1  1  1
         *   1   0      0  0  1  1  0  0  1  1  0  0  1  1  0  0  1  1
         *   1   1      0  1  0  1  0  1  0  1  0  1  0  1  0  1  0  1
         *
         */

        if (bitPlane)
        {
            plane_num = (gpr_$rgb_plane_t) ffs(bitPlane) - 1;
            alu_bg0 = (pGC->alu) & 0x0c;
            alu_fg0 = alu_bg0 >> 2;
            alu_fg1 = (pGC->alu) & 0x03;
            alu_bg1 = alu_fg1 << 2;

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

        srcRects = REGION_RECTS(prgnSrcClip);
        dstRects = REGION_RECTS(prgnDstClip);

        for (i = 0; i < srcNumRects; i++)
        {
            if (srcordering == NULL) prect = &srcRects[i];
            else prect = &srcRects[srcordering[i]];

            x1 = max(prect->x1, srcBox.x1);
            x2 = min(prect->x2, srcBox.x2);
            y1 = max(prect->y1, srcBox.y1);
            y2 = min(prect->y2, srcBox.y2);
            /* is there anything visible here? */
            if ((x2 > x1) && (y2 > y1))
            {
                for (j=0; j < dstNumRects; j++)
                {
                    if ((dstordering == NULL) && (srcordering == NULL)) pbox = &dstRects[j];
                    else if (dstordering == NULL) pbox = &dstRects[srcordering[j]];
                    else pbox = &dstRects[dstordering[j]];
                    xMin = max(x1-(srcx-dstx), pbox->x1);
                    xMax = min(x2-(srcx-dstx), pbox->x2);
                    yMin = max(y1-(srcy-dsty), pbox->y1);
                    yMax = min(y2-(srcy-dsty), pbox->y2);
                    /* is there anything visible here? */
                    if (xMax <= xMin || yMax <= yMin)
                        continue;

                    src_win.window_base.x_coord = (xMin-(dstx-srcx));
                    src_win.window_base.y_coord = (yMin-(dsty-srcy));
                    src_win.window_size.x_size = (xMax-xMin);
                    src_win.window_size.y_size = (yMax-yMin);

                    dest_origin.x_coord = (xMin);
                    dest_origin.y_coord = (yMin);

                    if (bitPlane)
                        gpr_$additive_blt (srcBitmap, src_win, plane_num,
                                           dest_origin, &status);
                    else if (x_to_gpr)
                    {
                        gpr_$rgb_plane_t    i, j;
                        for (i = 0, j = (gpr_$rgb_plane_t) x_to_gpr - 1;
                             i < x_to_gpr; i++, j--)
                            gpr_$bit_blt (srcBitmap, src_win, i, dest_origin,
                                          j, &status);
                    }
                    else
                        gpr_$pixel_blt (srcBitmap, src_win, dest_origin,
                                        &status);
                }
            }
        }

        if (srcordering) DEALLOCATE_LOCAL(srcordering);
        if (dstordering) DEALLOCATE_LOCAL(dstordering);

cleanup:
        if (doExposures)
            prgnExposed = miHandleExposures(pDrawSrc, pDraw, pGC, xIn, yIn,
                                            widthSrc, heightSrc, xOut, yOut, 0);
        else
            prgnExposed = NULL;

        if (realSrcClip)
            (*pGC->pScreen->RegionDestroy)(prgnSrcClip);

        return prgnExposed;
    }
}

/*
 * apcCopyArea -- DDX interface (GC "op" CopyArea)
 *      Invoke doCopy to do a CopyArea with GPR.
 */
RegionPtr
apcCopyArea(pDrawSrc, pDraw,
            pGC, xIn, yIn, widthSrc, heightSrc, xOut, yOut)
    DrawablePtr pDrawSrc;
    DrawablePtr pDraw;
    GCPtr       pGC;
    int         xIn, yIn;
    int         widthSrc, heightSrc;
    int         xOut, yOut;
{
    apcPrivScreenPtr    pPrivScreen;

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    return doCopy(pDrawSrc, pDraw, pGC, pPrivScreen,
                  xIn, yIn, widthSrc, heightSrc, xOut, yOut,
                  (unsigned long)0, TRUE, 0);
}

/*
 * apcCopyPlane -- DDX interface (GC "op" CopyPlane)
 *      Invoke doCopy to do a CopyPlane with GPR.
 */
RegionPtr
apcCopyPlane(pDrawSrc, pDraw,
             pGC, xIn, yIn, widthSrc, heightSrc, xOut, yOut, bitPlane)
    DrawablePtr     pDrawSrc;
    DrawablePtr     pDraw;
    GCPtr           pGC;
    int             xIn, yIn;
    int             widthSrc, heightSrc;
    int             xOut, yOut;
    unsigned long   bitPlane;
{
    apcPrivScreenPtr    pPrivScreen;

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    return doCopy(pDrawSrc, pDraw, pGC, pPrivScreen,
                  xIn, yIn, widthSrc, heightSrc, xOut, yOut,
                  bitPlane, TRUE, 0);
}

/*
 * apcPutImage -- DDX interface (GC "op" PutImage)
 *      Invoke doCopy to do a PutImage with GPR.
 */
void
apcPutImage(pDraw, pGC, depth, x, y, w, h, leftPad, format, pImage)
    DrawablePtr     pDraw;
    GCPtr           pGC;
    int             depth, x, y, w, h, leftPad;
    unsigned int    format;
    unsigned char  *pImage;
{
    int                 bitPlane;
    int                 x_to_gpr;
    PixmapPtr           pTmpPixmap;
    apcPrivScreenPtr    pPrivScreen;

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    pTmpPixmap = apcCreatePixmapFromData (pDraw->pScreen, (w + leftPad),
                                          h, depth, format, pImage);
    if (!pTmpPixmap)
    {
        ErrorF( "apcPutImage can't create temp pixmap\n");
        return;
    }

    bitPlane = (format == XYBitmap);
    x_to_gpr = (format == ZPixmap) ? FALSE : depth;
    doCopy (pTmpPixmap, pDraw, pGC, pPrivScreen,
            leftPad, 0, w, h, x, y,
            bitPlane, FALSE, x_to_gpr);

    (*pDraw->pScreen->DestroyPixmap)(pTmpPixmap);
}

/*
 * apcGetImage -- DDX interface (Screen)
 *      Do a GetImage with GPR.
 */
void
apcGetImage(pDraw, sx, sy, w, h, format, planeMask, pdstLine)
    DrawablePtr     pDraw;
    int             sx, sy, w, h;
    unsigned int    format;
    unsigned long   planeMask;
    pointer         pdstLine;
{
    status_$t           status;
    gpr_$bitmap_desc_t  src_bitmap, dst_bitmap;
    gpr_$window_t       wndw;
    gpr_$position_t     org;
    apcPrivScreenPtr    pPrivScreen;
    PixmapPtr           pTmpPixmap;
    int                 d;      /* depth of returned image */

    d = (format == ZPixmap) ? pDraw->depth : 1;
    pTmpPixmap = apcCreatePixmapFromData (pDraw->pScreen, w, h, d, format, pdstLine);
    if (!pTmpPixmap)
    {
        ErrorF ("apcGetImage can't create temp pixmap\n");
        return;
    }

    pPrivScreen = (apcPrivScreenPtr) pDraw->pScreen->devPrivates[apcScreenIndex].ptr;
    SET_BITMAP (((apcPrivPMPtr)pTmpPixmap->devPrivate.ptr)->bitmap_desc, pPrivScreen);
    SET_CLIPPING_ACTIVE (false, pPrivScreen);
    SET_PLANE_MASK (0xFFFFFFFF, pPrivScreen);
    SET_RASTER_OP_COMMON (GXcopy, pPrivScreen, d);

    if (pDraw->type == DRAWABLE_WINDOW)
    {
        apDisplayDataPtr    pDisp;

        pDisp = (apDisplayDataPtr) pDraw->pScreen->devPrivates[apDisplayScreenIndex].ptr;
        src_bitmap = pDisp->display_bitmap;
    }
    else
        src_bitmap = ((apcPrivPMPtr)((PixmapPtr)pDraw)->devPrivate.ptr)->bitmap_desc;

    wndw.window_base.x_coord = sx + pDraw->x;
    wndw.window_base.y_coord = sy + pDraw->y;
    wndw.window_size.x_size = w;
    wndw.window_size.y_size = h;
    org.x_coord = org.y_coord = 0;

    if (format == ZPixmap)
    {
        if (((1 << d) - 1) & ~planeMask)
        {   /* clear masked planes */
            gpr_$clear ((gpr_$pixel_value_t) 0, &status);
            SET_PLANE_MASK (planeMask, pPrivScreen);
        }
        gpr_$pixel_blt (src_bitmap, wndw, org, &status);
    }
    else
        gpr_$bit_blt (src_bitmap, wndw, (gpr_$rgb_plane_t)(ffs(planeMask)-1),
                      org, (gpr_$rgb_plane_t)0, &status);

    (*pDraw->pScreen->DestroyPixmap)(pTmpPixmap);
}
