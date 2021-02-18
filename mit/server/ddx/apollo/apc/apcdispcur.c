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
#include "cursorstr.h"
#include "mipointer.h"
#include "misprite.h"

/* per-screen private data */

static int              apcDCScreenIndex;
static unsigned long    apcDCGeneration = 0;

static Bool     apcDCCloseScreen();

typedef struct {
    gpr_$bitmap_desc_t  saveArea;   /* saveunders for cursor */
    gpr_$bitmap_desc_t  tempArea;   /* temp area for MoveCursor */
    unsigned long      *pDataSave;  /* pointer to bits for saveArea */
    unsigned long      *pDataTemp;  /* pointer to bits for tempArea */
    int                 saveWidth;  /* width of saveArea */
    int                 saveHeight; /* height of saveArea */
    int                 tempWidth;  /* width of tempArea */
    int                 tempHeight; /* height of tempArea */
    Bool                (*CloseScreen)();
} apcDCScreenRec, *apcDCScreenPtr;

/* per-cursor per-screen private data */
typedef struct {
    gpr_$bitmap_desc_t  fgBitmap;   /* bits of cursor which are foreground pixels */
    gpr_$bitmap_desc_t  bgBitmap;   /* bits of cursor which are background pixels */
    unsigned long      *pDataFg;    /* pointer to bits for fgBitmap */
    unsigned long      *pDataBg;    /* pointer to bits for bgBitmap */
    unsigned long       fgPixel;    /* pixel value we are currently drawing as foreground */
    unsigned long       bgPixel;    /* pixel value we are currently drawing as background */
    gpr_$raster_op_t    fgRops[MAX_DEPTH_SUPPORTED];    /* gpr rop vector to make foreground color */
    gpr_$raster_op_t    bgRops[MAX_DEPTH_SUPPORTED];    /* gpr rop vector to make background color */
} apcDCCursorRec, *apcDCCursorPtr;

/*
 * Values used in precalculation of a rop per plane to accomplish
 * fg and bg colors.
 * Since cursor alu is always GXcopy, we can choose one of two
 * constant rops based on whether we're drawing zeroes or ones in
 * this plane.
 *
 *      alu_bit0 = 4 | (((pGC->alu) >> 2) & 0x03);
 *      alu_bit1 = 4 | ((pGC->alu) & 0x03);
 *
 *   Using constant 0x03 (GXcopy) instead of pGC->alu yields
 *
 *      alu_bit0 = 4
 *      alu_bit1 = 7
 */
#define ALU_BIT0    4
#define ALU_BIT1    7

/*
 * Vector of miSprite cursor funcs.
 */

static Bool apcDCRealizeCursor();
static Bool apcDCUnrealizeCursor();
static Bool apcDCPutUpCursor();
static Bool apcDCSaveUnderCursor();
static Bool apcDCRestoreUnderCursor();
static Bool apcDCMoveCursor();
static Bool apcDCChangeSave();

static miSpriteCursorFuncRec apcDCFuncs = {
    apcDCRealizeCursor,
    apcDCUnrealizeCursor,
    apcDCPutUpCursor,
    apcDCSaveUnderCursor,
    apcDCRestoreUnderCursor,
    apcDCMoveCursor,
    apcDCChangeSave
};


/*
 * apcDCInitialize -- Driver internal code
 *      Set up to do cursors at the lowest (DispCur) layer.
 *      Test for an apc implementation of the next higher layer (Sprite);
 *      use it if available, else use the mi implementation.
 */
Bool
apcDCInitialize(pScreen, cursorFuncs)
    ScreenPtr               pScreen;
    miPointerCursorFuncPtr  cursorFuncs;
{
    apcDCScreenPtr  pDCScreenPriv;

    if (apcDCGeneration != serverGeneration)
    {
        apcDCScreenIndex = AllocateScreenPrivateIndex ();
        if (apcDCScreenIndex < 0)
            return FALSE;
        apcDCGeneration = serverGeneration;
    }

    pDCScreenPriv = (apcDCScreenPtr) xalloc (sizeof (apcDCScreenRec));
    if (!pDCScreenPriv)
        return FALSE;

    pDCScreenPriv->saveArea = pDCScreenPriv->tempArea = gpr_$nil_bitmap_desc;
    pDCScreenPriv->pDataSave = pDCScreenPriv->pDataTemp = NULL;
    pDCScreenPriv->saveWidth = pDCScreenPriv->saveHeight = 0;
    pDCScreenPriv->tempWidth = pDCScreenPriv->tempHeight = 0;

    pDCScreenPriv->CloseScreen = pScreen->CloseScreen;
    pScreen->CloseScreen = apcDCCloseScreen;

    pScreen->devPrivates[apcDCScreenIndex].ptr = (pointer) pDCScreenPriv;

    if (apcSpriteInitialize (pScreen, &apcDCFuncs, cursorFuncs))
        return TRUE;
    else if (miSpriteInitialize (pScreen, &apcDCFuncs, cursorFuncs))
        return TRUE;
    else
    {
        xfree ((pointer) pDCScreenPriv);
        return FALSE;
    }
}

/*
 * apcDCCloseScreen -- DDX interface (screen)
 *      Free resources and call wrapped CloseScreen.
 */
static Bool
apcDCCloseScreen (index, pScreen)
    ScreenPtr   pScreen;
{
    apcDCScreenPtr  pDCScreenPriv;

    pDCScreenPriv = (apcDCScreenPtr) pScreen->devPrivates[apcDCScreenIndex].ptr;
    pScreen->CloseScreen = pDCScreenPriv->CloseScreen;

    if (pDCScreenPriv->saveArea != gpr_$nil_bitmap_desc)
    {
        xfree (pDCScreenPriv->pDataSave);
        apDeallocateBitmap (pDCScreenPriv->saveArea);
    }
    if (pDCScreenPriv->tempArea != gpr_$nil_bitmap_desc)
    {
        xfree (pDCScreenPriv->pDataTemp);
        apDeallocateBitmap (pDCScreenPriv->tempArea);
    }
    xfree ((pointer) pDCScreenPriv);
    return (*pScreen->CloseScreen) (index, pScreen);
}

/*
 * apcDCRealizeCursor -- DDX interface (screen)
 *      Realize a DIX cursor for the screen.
 *      We defer the real work until the first use of the cursor.
 */
static Bool
apcDCRealizeCursor(pScreen, pCursor)
    ScreenPtr   pScreen;
    CursorPtr   pCursor;
{
    if (pCursor->bits->refcnt <= 1)
        pCursor->bits->devPriv[pScreen->myNum] = (pointer)NULL;
    return TRUE;
}

/*
 * apcDCRealizeCursor -- Driver internal code.
 *      Do the real work of realizing a DIX cursor for the screen.
 */
static apcDCCursorPtr
apcDCRealize (pScreen, pCursor)
    ScreenPtr   pScreen;
    CursorPtr   pCursor;
{
    apcPrivScreenPtr    pPrivScreen;
    apcDCCursorPtr      pPriv;
    int                 i;
    int                 totalBytes;
    status_$t           status;
    gpr_$bitmap_desc_t  cursorSource, cursorMask;
    gpr_$window_t       src_win;
    gpr_$position_t     dst_org;

    pPrivScreen = (apcPrivScreenPtr) pScreen->devPrivates[apcScreenIndex].ptr;

    pPriv = (apcDCCursorPtr) xalloc (sizeof (apcDCCursorRec));
    if (!pPriv)
        return (apcDCCursorPtr)NULL;

    totalBytes = PixmapBytePad(pCursor->bits->width, 1) * pCursor->bits->height;

    pPriv->pDataFg = xalloc (totalBytes);
    if (!pPriv->pDataFg)
    {
        xfree ((pointer) pPriv);
        return (apcDCCursorPtr)NULL;
    }
    pPriv->fgBitmap = apMakeBitmapFromArray (pCursor->bits->width, pCursor->bits->height, 1,
                                             pPrivScreen->attribute_block, XYPixmap, pPriv->pDataFg);

    pPriv->pDataBg = xalloc (totalBytes);
    if (!pPriv->pDataBg)
    {
        xfree (pPriv->pDataFg);
        apDeallocateBitmap (pPriv->fgBitmap);
        xfree ((pointer) pPriv);
        return (apcDCCursorPtr)NULL;
    }
    pPriv->bgBitmap = apMakeBitmapFromArray (pCursor->bits->width, pCursor->bits->height, 1,
                                             pPrivScreen->attribute_block, XYPixmap, pPriv->pDataBg);

    pCursor->bits->devPriv[pScreen->myNum] = (pointer) pPriv;

    /* create the two sets of bits, clipping as appropriate */

    cursorSource = apMakeBitmapFromArray (pCursor->bits->width, pCursor->bits->height, 1,
                                          pPrivScreen->attribute_block, XYPixmap, pCursor->bits->source);
    cursorMask = apMakeBitmapFromArray (pCursor->bits->width, pCursor->bits->height, 1,
                                        pPrivScreen->attribute_block, XYPixmap, pCursor->bits->mask);

    src_win.window_base.x_coord = src_win.window_base.y_coord = 0;
    src_win.window_size.x_size = pCursor->bits->width;
    src_win.window_size.y_size = pCursor->bits->height;
    dst_org.x_coord = dst_org.y_coord = 0;

    /* foreground bits -- pCursor->source & pCursor->mask */

    SET_BITMAP (pPriv->fgBitmap, pPrivScreen);
    SET_CLIPPING_ACTIVE (false, pPrivScreen);
    SET_PLANE_MASK (0xFFFFFFFF, pPrivScreen);
    SET_RASTER_OP (0, GXcopy, pPrivScreen);
    gpr_$pixel_blt (cursorSource, src_win, dst_org, &status);
    SET_RASTER_OP (0, GXand, pPrivScreen);
    gpr_$pixel_blt (cursorMask, src_win, dst_org, &status);

    /* background bits -- pCursor->mask & ~pCursor->source */

    SET_BITMAP (pPriv->bgBitmap, pPrivScreen);
    SET_RASTER_OP (0, GXcopy, pPrivScreen);
    gpr_$pixel_blt (cursorMask, src_win, dst_org, &status);
    SET_RASTER_OP (0, GXandInverted, pPrivScreen);
    gpr_$pixel_blt (cursorSource, src_win, dst_org, &status);

    apDeallocateBitmap (cursorSource);
    apDeallocateBitmap (cursorMask);

    pPriv->fgPixel = pPriv->bgPixel = 0;
    for (i = 0; i < pScreen->rootDepth; i++)
        pPriv->fgRops[i] = pPriv->bgRops[i] = ALU_BIT0;

    return pPriv;
}

/*
 * apcDCUnrealizeCursor -- DDX interface (screen)
 *      Unrealize a DIX cursor for the screen.
 */
static Bool
apcDCUnrealizeCursor(pScreen, pCursor)
    ScreenPtr   pScreen;
    CursorPtr   pCursor;
{
    apcDCCursorPtr  pPriv;

    pPriv = (apcDCCursorPtr) pCursor->bits->devPriv[pScreen->myNum];
    if (pPriv && (pCursor->bits->refcnt <= 1))
    {
        xfree (pPriv->pDataFg);
        apDeallocateBitmap (pPriv->fgBitmap);
        xfree (pPriv->pDataBg);
        apDeallocateBitmap (pPriv->bgBitmap);
        xfree ((pointer) pPriv);
        pCursor->bits->devPriv[pScreen->myNum] = (pointer)NULL;
    }
    return TRUE;
}

/*
 * apcDCPutBits -- Driver internal code.
 *      Render the given cursor in the given drawable using the given colors.
 */
static void
apcDCPutBits (pPrivScreen, depth, bitmap_id, pPriv, x, y, w, h, fgPix, bgPix)
    apcPrivScreenPtr    pPrivScreen;
    int                 depth;
    gpr_$bitmap_desc_t  bitmap_id;
    int                 x, y;
    unsigned            w, h;
    apcDCCursorPtr      pPriv;
    unsigned long       fgPix, bgPix;
{
    gpr_$window_t       src_win;
    gpr_$position_t     dst_org;
    int                 i;
    status_$t           status;

    src_win.window_base.x_coord = 0;
    src_win.window_base.y_coord = 0;
    src_win.window_size.x_size = w;
    src_win.window_size.y_size = h;

    dst_org.x_coord = x;
    dst_org.y_coord = y;

    SET_BITMAP (bitmap_id, pPrivScreen);
    SET_CLIPPING_ACTIVE (false, pPrivScreen);
    SET_PLANE_MASK (0xFFFFFFFF, pPrivScreen);

    if (fgPix == pPriv->fgPixel)
        /* just set the same rops we've already calculated */
        for (i = 0; i < depth; i++)
        {
            SET_RASTER_OP (i, pPriv->fgRops[i], pPrivScreen);
        }
    else
    {
        /* calculate and save new rops for new value */
        pPriv->fgPixel = fgPix;
        for (i = 0; i < depth; i++)
        {
            pPriv->fgRops[i] = ((fgPix >> i) & 0x01) ? ALU_BIT1 : ALU_BIT0;
            SET_RASTER_OP (i, pPriv->fgRops[i], pPrivScreen);
        }
    }
    gpr_$additive_blt (pPriv->fgBitmap, src_win, 0, dst_org, &status);

    if (bgPix == pPriv->bgPixel)
        /* just set the same rops we've already calculated */
        for (i = 0; i < depth; i++)
        {
            SET_RASTER_OP (i, pPriv->bgRops[i], pPrivScreen);
        }
    else
    {
        /* calculate and save new rops for new value */
        pPriv->bgPixel = bgPix;
        for (i = 0; i < depth; i++)
        {
            pPriv->bgRops[i] = ((bgPix >> i) & 0x01) ? ALU_BIT1 : ALU_BIT0;
            SET_RASTER_OP (i, pPriv->bgRops[i], pPrivScreen);
        }
    }
    gpr_$additive_blt (pPriv->bgBitmap, src_win, 0, dst_org, &status);
}

/*
 * apcDCPutUpCursor -- DispCur cursor interface
 *      Put up the given cursor in the given colors at the given screen position.
 */
static Bool
apcDCPutUpCursor(pScreen, pCursor, x, y, fgPix, bgPix)
    ScreenPtr       pScreen;
    CursorPtr       pCursor;
    int             x, y;
    unsigned long   fgPix, bgPix;
{
    apcDCCursorPtr      pPriv;
    gpr_$bitmap_desc_t  disp_bitmap_id;
    apcPrivScreenPtr    pPrivScreen;

    pPrivScreen = (apcPrivScreenPtr) pScreen->devPrivates[apcScreenIndex].ptr;

    pPriv = (apcDCCursorPtr) pCursor->bits->devPriv[pScreen->myNum];
    if (!pPriv)
    {
        pPriv = apcDCRealize(pScreen, pCursor);
        if (!pPriv)
            return FALSE;
    }

    disp_bitmap_id = ((apDisplayDataPtr) pScreen->devPrivates[apDisplayScreenIndex].ptr)->display_bitmap;
    apcDCPutBits (pPrivScreen, pScreen->rootDepth, disp_bitmap_id, pPriv,
                  x, y, pCursor->bits->width, pCursor->bits->height,
                  fgPix, bgPix);
    return TRUE;
}

/*
 * apcDCCopyHelper -- Driver internal code.
 *      Do (effectively) a CopyArea for manipulating the cursor saveunder data.
 */
static void
apcDCCopyHelper(pPrivScreen, depth, srcBitmap, dstBitmap, srcX, srcY, sizeX, sizeY, dstX, dstY)
    apcPrivScreenPtr    pPrivScreen;
    int                 depth;
    gpr_$bitmap_desc_t  srcBitmap;
    gpr_$bitmap_desc_t  dstBitmap;
    int                 srcX, srcY, sizeX, sizeY, dstX, dstY;
{
    int                 i;
    gpr_$window_t       src_win;
    gpr_$position_t     dst_org;
    status_$t           status;

    SET_BITMAP (dstBitmap, pPrivScreen);
    SET_CLIPPING_ACTIVE (false, pPrivScreen);
    SET_PLANE_MASK (0xFFFFFFFF, pPrivScreen);
    SET_RASTER_OP_COMMON (GXcopy, pPrivScreen, depth);

    /* Adjust sizes and origins to handle edge-straddling conditions */

    if (srcX < 0)
    {
        sizeX += srcX;
        dstX -= srcX;
        srcX = 0;
    }
    if (srcY < 0)
    {
        sizeY += srcY;
        dstY -= srcY;
        srcY = 0;
    }

    if (dstX < 0)
    {
        sizeX += dstX;
        srcX -= dstX;
        dstX = 0;
    }
    if (dstY < 0)
    {
        sizeY += dstY;
        srcY -= dstY;
        dstY = 0;
    }

    src_win.window_base.x_coord = srcX;
    src_win.window_base.y_coord = srcY;
    src_win.window_size.x_size = sizeX;
    src_win.window_size.y_size = sizeY;

    dst_org.x_coord = dstX;
    dst_org.y_coord = dstY;

    gpr_$pixel_blt (srcBitmap, src_win, dst_org, &status);
}

/*
 * apcDCSaveUnderCursor -- DispCur cursor interface
 *      Do a saveunder of the pixels about to be stepped on by the cursor.
 */
static Bool
apcDCSaveUnderCursor(pScreen, x, y, w, h)
    ScreenPtr   pScreen;
    int         x, y, w, h;
{
    apcDCScreenPtr      pDCScreenPriv;
    gpr_$bitmap_desc_t  saveArea;
    gpr_$bitmap_desc_t  disp_bitmap_id;
    apcPrivScreenPtr    pPrivScreen;

    pPrivScreen = (apcPrivScreenPtr) pScreen->devPrivates[apcScreenIndex].ptr;

    pDCScreenPriv = (apcDCScreenPtr) pScreen->devPrivates[apcDCScreenIndex].ptr;
    saveArea = pDCScreenPriv->saveArea;

    if ((saveArea == gpr_$nil_bitmap_desc) ||
        (pDCScreenPriv->saveWidth < w) ||
        (pDCScreenPriv->saveHeight < h))
    {
        if (saveArea != gpr_$nil_bitmap_desc)
        {
            apDeallocateBitmap (saveArea);
        }
        pDCScreenPriv->pDataSave = xrealloc (pDCScreenPriv->pDataSave,
                                             PixmapBytePad(w, 1) * h * pScreen->rootDepth);
        if (!pDCScreenPriv->pDataSave)
            return FALSE;
        pDCScreenPriv->saveArea = saveArea =
                apMakeBitmapFromArray (w, h, pScreen->rootDepth,
                                       pPrivScreen->attribute_block, XYPixmap, pDCScreenPriv->pDataSave);
        pDCScreenPriv->saveWidth = w;
        pDCScreenPriv->saveHeight = h;
    }

    disp_bitmap_id = ((apDisplayDataPtr) pScreen->devPrivates[apDisplayScreenIndex].ptr)->display_bitmap;
    apcDCCopyHelper (pPrivScreen, pScreen->rootDepth, disp_bitmap_id, saveArea,
                     x, y, w, h, 0, 0);
    return TRUE;
}

/*
 * apcDCRestoreUnderCursor -- DispCur cursor interface
 *      Put the saveunder pixels back, thus erasing the cursor.
 */
static Bool
apcDCRestoreUnderCursor(pScreen, x, y, w, h)
    ScreenPtr   pScreen;
    int         x, y, w, h;
{
    apcDCScreenPtr      pDCScreenPriv;
    gpr_$bitmap_desc_t  saveArea;
    gpr_$bitmap_desc_t  disp_bitmap_id;
    apcPrivScreenPtr    pPrivScreen;

    pPrivScreen = (apcPrivScreenPtr) pScreen->devPrivates[apcScreenIndex].ptr;

    pDCScreenPriv = (apcDCScreenPtr) pScreen->devPrivates[apcDCScreenIndex].ptr;
    saveArea = pDCScreenPriv->saveArea;
    if (saveArea == gpr_$nil_bitmap_desc)
        return FALSE;

    disp_bitmap_id = ((apDisplayDataPtr) pScreen->devPrivates[apDisplayScreenIndex].ptr)->display_bitmap;
    apcDCCopyHelper (pPrivScreen, pScreen->rootDepth, saveArea, disp_bitmap_id,
                     0, 0, w, h, x, y);
    return TRUE;
}

/*
 * apcDCChangeSave -- DispCur cursor interface
 *      Update the saveunder pixels, as we are about to move the cursor within its
 *      "padded cell".
 */
static Bool
apcDCChangeSave(pScreen, x, y, w, h, dx, dy)
    ScreenPtr   pScreen;
    int         x, y, w, h, dx, dy;
{
    apcDCScreenPtr      pDCScreenPriv;
    gpr_$bitmap_desc_t  saveArea;
    gpr_$bitmap_desc_t  disp_bitmap_id;
    int                 sourcex, sourcey, destx, desty, copyw, copyh;
    apcPrivScreenPtr    pPrivScreen;

    pPrivScreen = (apcPrivScreenPtr) pScreen->devPrivates[apcScreenIndex].ptr;

    pDCScreenPriv = (apcDCScreenPtr) pScreen->devPrivates[apcDCScreenIndex].ptr;
    saveArea = pDCScreenPriv->saveArea;
    if (saveArea == gpr_$nil_bitmap_desc)
        return FALSE;

    disp_bitmap_id = ((apDisplayDataPtr) pScreen->devPrivates[apDisplayScreenIndex].ptr)->display_bitmap;

    /*
     * copy the old bits to the screen.
     */

    if (dy > 0)
        apcDCCopyHelper (pPrivScreen, pScreen->rootDepth, saveArea, disp_bitmap_id,
                         0, h - dy, w, dy, x + dx, y + h);
    else if (dy < 0)
        apcDCCopyHelper (pPrivScreen, pScreen->rootDepth, saveArea, disp_bitmap_id,
                         0, 0, w, -dy, x + dx, y + dy);

    if (dy >= 0)
    {
        desty = y + dy;
        sourcey = 0;
        copyh = h - dy;
    }
    else
    {
        desty = y;
        sourcey = - dy;
        copyh = h + dy;
    }

    if (dx > 0)
        apcDCCopyHelper (pPrivScreen, pScreen->rootDepth, saveArea, disp_bitmap_id,
                         w - dx, sourcey, dx, copyh, x + w, desty);
    else if (dx < 0)
        apcDCCopyHelper (pPrivScreen, pScreen->rootDepth, saveArea, disp_bitmap_id,
                         0, sourcey, -dx, copyh, x + dx, desty);

    /*
     * move the bits that are still valid within the pixmap
     */

    if (dx >= 0)
    {
        sourcex = 0;
        destx = dx;
        copyw = w - dx;
    }
    else
    {
        destx = 0;
        sourcex = - dx;
        copyw = w + dx;
    }
    if (dy >= 0)
    {
	sourcey = 0;
	desty = dy;
	copyh = h - dy;
    }
    else
    {
	desty = 0;
	sourcey = -dy;
	copyh = h + dy;
    }

    apcDCCopyHelper (pPrivScreen, pScreen->rootDepth, saveArea, saveArea,
                     sourcex, sourcey, copyw, copyh, destx, desty);

    /*
     * copy the new bits from the screen into the remaining areas of the
     * pixmap
     */

    if (dy > 0)
        apcDCCopyHelper (pPrivScreen, pScreen->rootDepth, disp_bitmap_id, saveArea,
                         x, y, w, dy, 0, 0);
    else if (dy < 0)
        apcDCCopyHelper (pPrivScreen, pScreen->rootDepth, disp_bitmap_id, saveArea,
                         x, y + h + dy, w, -dy, 0, h + dy);

    if (dy >= 0)
    {
        desty = dy;
        sourcey = y + dy;
        copyh = h - dy;
    }
    else
    {
        desty = 0;
        sourcey = y;
        copyh = h + dy;
    }

    if (dx > 0)
        apcDCCopyHelper (pPrivScreen, pScreen->rootDepth, disp_bitmap_id, saveArea,
                         x, sourcey, dx, copyh, 0, desty);
    else if (dx < 0)
        apcDCCopyHelper (pPrivScreen, pScreen->rootDepth, disp_bitmap_id, saveArea,
                         x + w + dx, sourcey, -dx, copyh, w + dx, desty);

    return TRUE;
}

/*
 * apcDCMoveCursor -- DispCur cursor interface
 *      Move the cursor within its "padded cell".
 */
static Bool
apcDCMoveCursor(pScreen, pCursor, x, y, w, h, dx, dy, fgPix, bgPix)
    ScreenPtr       pScreen;
    CursorPtr       pCursor;
    int             x, y, w, h, dx, dy;
    unsigned long   fgPix, bgPix;
{
    apcDCCursorPtr      pPriv;
    apcDCScreenPtr      pDCScreenPriv;
    gpr_$bitmap_desc_t  tempArea;
    gpr_$bitmap_desc_t  disp_bitmap_id;
    apcPrivScreenPtr    pPrivScreen;

    pPrivScreen = (apcPrivScreenPtr) pScreen->devPrivates[apcScreenIndex].ptr;

    pPriv = (apcDCCursorPtr) pCursor->bits->devPriv[pScreen->myNum];
    if (!pPriv)
    {
        pPriv = apcDCRealize(pScreen, pCursor);
        if (!pPriv)
            return FALSE;
    }

    pDCScreenPriv = (apcDCScreenPtr) pScreen->devPrivates[apcDCScreenIndex].ptr;
    tempArea = pDCScreenPriv->tempArea;

    if ((tempArea == gpr_$nil_bitmap_desc) ||
        (pDCScreenPriv->tempWidth < w) ||
        (pDCScreenPriv->tempHeight < h))
    {
        if (tempArea != gpr_$nil_bitmap_desc)
        {
            apDeallocateBitmap (tempArea);
        }
        pDCScreenPriv->pDataTemp = xrealloc (pDCScreenPriv->pDataTemp,
                                             PixmapBytePad(w, 1) * h * pScreen->rootDepth);
        if (!pDCScreenPriv->pDataTemp)
            return FALSE;
        pDCScreenPriv->tempArea = tempArea =
                apMakeBitmapFromArray (w, h, pScreen->rootDepth,
                                       pPrivScreen->attribute_block, XYPixmap, pDCScreenPriv->pDataTemp);
        pDCScreenPriv->tempWidth = w;
        pDCScreenPriv->tempHeight = h;
    }

    disp_bitmap_id = ((apDisplayDataPtr) pScreen->devPrivates[apDisplayScreenIndex].ptr)->display_bitmap;

    /*
     * copy the saved area to a temporary pixmap
     */

    apcDCCopyHelper (pPrivScreen, pScreen->rootDepth, pDCScreenPriv->saveArea, tempArea,
                     0, 0, w, h, 0, 0);

    /*
     * draw the cursor in the temporary pixmap
     */

    apcDCPutBits (pPrivScreen, pScreen->rootDepth, tempArea, pPriv,
                  dx, dy, pCursor->bits->width, pCursor->bits->height,
                  fgPix, bgPix);

    /*
     * copy the temporary pixmap onto the screen
     */

    apcDCCopyHelper (pPrivScreen, pScreen->rootDepth, tempArea, disp_bitmap_id,
                     0, 0, w, h, x, y);

    return TRUE;
}
