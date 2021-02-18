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
#include "apctext.h"
#include "fontstruct.h"
#include "dixfontstr.h"

/*
 * These routines all make internal copies of the text, of maximum size MAXCHRS, and
 * then they process larger strings in batches.
 */
#define MAXCHRS 128

static gpr_$window_t    nocrect = { 0, 0, 0, 0 };

/*
 * rep_dfltch -- Driver internal code
 *      Examine each glyph for (a) existence, and (b) width.  Return overall width, plus
 *      a new string in which missing characters are replaced by chDefault.  This code is
 *      cloned from (dix)getGlyphs which doesn't do what is needed.
 */
static int
rep_dfltch(pGC, fontEncoding, chars, cpp, count)
    GCPtr           pGC;
    FontEncoding    fontEncoding;
    unsigned char  *chars, *cpp;
    int             count;
{
    CharInfoPtr     pCI = pGC->font->pCI;
    FontInfoPtr     pFI = pGC->font->pFI;
    unsigned int    firstCol = pFI->firstCol;
    unsigned int    numCols = pFI->lastCol - firstCol + 1;
    unsigned int    firstRow = pFI->firstRow;
    unsigned int    numRows = pFI->lastRow - firstRow + 1;
    unsigned int    chDefault = pFI->chDefault;
    unsigned int    c;
    int             i;
    int             width;

    if (fontEncoding == Linear16Bit && pFI->lastRow > 0)
        fontEncoding = TwoD16Bit;

    width = 0;

    switch (fontEncoding)
    {
    case Linear8Bit:
    case TwoD8Bit:
        for (i=0; i < count; i++)
        {
            c = *chars - firstCol;
            if (c < numCols && pCI[c].exists)
            {
                *cpp++ = *chars++;
                width += pCI[c].metrics.characterWidth;
            }
            else
            {
                *cpp++ = chDefault;
                chars++;
                c = chDefault - firstCol;
                if (c < numCols && pCI[c].exists)
                    width += pCI[c].metrics.characterWidth;
            }
        }
        break;

    case Linear16Bit:
        for (i=0; i < count; i++)
        {
            if (*chars++)
                c = chDefault;
            else
                c = *chars++;
            *cpp++ = 0;

            *cpp = c;
            c -= firstCol;
            if (c < numCols && pCI[c].exists)
            {
                cpp++;
                width += pCI[c].metrics.characterWidth;
            }
            else
            {
                *cpp++ = chDefault;
                c = chDefault - firstCol;
                if (c < numCols && pCI[c].exists)
                    width += pCI[c].metrics.characterWidth;
            }
        }
        break;

    case TwoD16Bit:
        for (i=0; i < count; i++)
        {
            unsigned int    row;
            unsigned int    col;

            row = (*chars) - firstRow;
            col = (*(chars+1)) - firstCol;
            if ((row < numRows) && (col < numCols))
            {
                c = row*numCols + col;
                if (pCI[c].exists)
                {
                    *cpp++ = *chars++;
                    *cpp++ = *chars++;
                    width += pCI[c].metrics.characterWidth;
                    continue;
                }
            }

            *cpp++ = chDefault >> 8;
            *cpp++ = chDefault;
            chars += 2;

            row = (chDefault >> 8)-firstRow;
            col = (chDefault & 0xff)-firstCol;
            if ((row < numRows) && (col < numCols))
            {
                c = row*numCols + col;
                if (pCI[c].exists)
                    width += pCI[c].metrics.characterWidth;
            }
        }
        break;
    }
    return width;
}

/*
 * gprText8 -- Driver internal code
 *      Does the work for both gprPolyText8 and gprImageText8 -- they differ only
 *      in the colors they use.
 *      Returns new x, screen-relative.
 */
static int
gprText8(pPrivScreen, regionp, gfp, x, y, count, chars, fgcolor, bgcolor, crect)
    apcPrivScreenPtr    pPrivScreen;
    RegionPtr           regionp;
    gprFIDPtr           gfp;
    int                 x, y;
    int                 count;
    gpr_$string_t       chars;
    gpr_$pixel_value_t  fgcolor, bgcolor;
    gpr_$window_t      *crect;
{
    status_$t       status;
    short           clipx, cx;
    gpr_$window_t   gwin;
    BoxPtr          pclip;
    int             nclips;
    short           newx, newy;

    SET_TEXT_VALUE (fgcolor, pPrivScreen);
    SET_TEXT_BACKGROUND_VALUE (bgcolor, pPrivScreen);

    if (regionp)
    {
        pclip = REGION_RECTS(regionp);
        nclips = REGION_NUM_RECTS(regionp);
    }
    else
    {
        SET_CLIPPING_ACTIVE (false, pPrivScreen);
        nclips = 1;
    }

    for (clipx = 0; clipx < nclips; clipx++)
    {
        gpr_$move ((short) x, (short) (y - 1), &status);

        if (nclips != 1)
        {
            gpr_$window_t   gwin;

            gwin.window_base.x_coord = pclip->x1;
            gwin.window_base.y_coord = pclip->y1;
            gwin.window_size.x_size = pclip->x2 - gwin.window_base.x_coord;
            gwin.window_size.y_size = pclip->y2 - gwin.window_base.y_coord;
            SET_CLIP_WINDOW (gwin, pPrivScreen);
            pclip++;
        }

        if (crect->window_size.x_size)
            gpr_$rectangle (*crect, &status);

        gpr_$text (chars, (short) count, &status);
    }
    gpr_$inq_cp (&newx, &newy, &status);

    return newx;
}

/*
 * gprPolyText8 -- DDX interface (GC "op" PolyText8)
 *      Do a solid PolyText8 via the gprText8 routine.
 *      Returns new x, drawable-relative.
 */
int
gprPolyText8(pDraw, pGC, x, y, count, chars)
    DrawablePtr     pDraw;
    GCPtr           pGC;
    int             x, y;
    int             count;
    unsigned char  *chars;
{
    FontInfoPtr         pfi = pGC->font->pFI;
    int                 i;
    unsigned char       chrcopy[MAXCHRS];
    unsigned short      dflt16[MAXCHRS];
    RegionPtr           regionp;
    gprFIDPtr           gfp;
    int                 xorg, newx;
    apcPrivGCPtr        pPrivGC;
    apcPrivScreenPtr    pPrivScreen;

    if (count < 1)
        return x;

    /* handle long strings */
    while (count > MAXCHRS)
    {
        x = gprPolyText8 (pDraw, pGC, x, y, MAXCHRS, chars);
        count -= MAXCHRS;
        chars += MAXCHRS;
    }

    if (pfi->firstRow > 0)
    {
        /*
         * No 8-bit characters can be displayed in this font
         * at all.  Just build the appropriate 16-bit array of default
         * chrs, and give it to gprPolyText16.
         */
        for (i=0; i<count; i++)
            dflt16[i] = pfi->chDefault;
        return gprPolyText16 (pDraw, pGC, x, y, count, dflt16);
    }

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    /* replace missing characters with chDefault */
    if (pfi->allExist)
    {
        for (i=0; i<count; i++)
            if (chars[i] >= pfi->firstCol &&
                chars[i] <= pfi->lastCol)
                chrcopy[i] = chars[i];
            else
                chrcopy[i] = pfi->chDefault;
    }
    else
        rep_dfltch (pGC, Linear8Bit, chars, chrcopy, count);

    xorg = pDraw->x;
    x += xorg;
    y += pDraw->y;

    /* go actually draw the text */
    pPrivGC = (apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr);
    regionp = pPrivGC->pCompositeClip;
    gfp = (gprFIDPtr) (pGC->font->devPriv[pGC->pScreen->myNum]);
    newx = gprText8 (pPrivScreen, regionp, gfp, x, y, count, chrcopy,
                     pPrivGC->polyTextVal, gpr_$transparent, &nocrect);
    return (newx - xorg);
}

/*
 * gprPolyText8Stencil -- DDX interface (GC "op" PolyText8)
 *      Do a general PolyText8 via the gprText8 routine.
 *      Returns new x, drawable-relative.
 */
int
gprPolyText8Stencil(pDraw, pGC, x, y, count, chars)
    DrawablePtr     pDraw;
    GCPtr           pGC;
    int             x, y;
    int             count;
    unsigned char  *chars;
{
    FontInfoPtr         pfi = pGC->font->pFI;
    int                 i;
    unsigned char       chrcopy[MAXCHRS];
    unsigned short      dflt16[MAXCHRS];
    RegionPtr           regionp;
    gprFIDPtr           gfp;
    int                 xorg, newx;
    int                 temp_width, temp_height;
    int                 temp_x, temp_y;
    int                 dst_temp_x, dst_temp_y;
    PixmapPtr           pTempBitm;
    gpr_$bitmap_desc_t  temp_bitm;
    apcPrivScreenPtr    pPrivScreen;

    if (count < 1)
        return x;

    /* handle long strings */
    while (count > MAXCHRS)
    {
        x = gprPolyText8Stencil (pDraw, pGC, x, y, MAXCHRS, chars);
        count -= MAXCHRS;
        chars += MAXCHRS;
    }

    if (pfi->firstRow > 0)
    {
        /*
         * No 8-bit characters can be displayed in this font
         * at all.  Just build the appropriate 16-bit array of default
         * chrs, and give it to gprPolyText16Stencil.
         */
        for (i=0; i<count; i++)
            dflt16[i] = pfi->chDefault;
        return gprPolyText16Stencil (pDraw, pGC, x, y, count, dflt16);
    }

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    /* replace missing characters with chDefault */
    if (pfi->allExist)
    {
        for (i=0; i<count; i++)
            if (chars[i] >= pfi->firstCol &&
                chars[i] <= pfi->lastCol)
                chrcopy[i] = chars[i];
            else
                chrcopy[i] = pfi->chDefault;
    }
    else
        rep_dfltch (pGC, Linear8Bit, chars, chrcopy, count);

    xorg = pDraw->x;
    x += xorg;
    y += pDraw->y;

    temp_x = max (0, pfi->maxbounds.metrics.leftSideBearing);
    temp_width = temp_x + (count-1) * pfi->maxbounds.metrics.characterWidth +
                 max (pfi->maxbounds.metrics.characterWidth, pfi->maxbounds.metrics.rightSideBearing);
    temp_y = max (pfi->fontAscent, pfi->maxbounds.metrics.ascent);
    temp_height = temp_y + max (pfi->fontDescent, pfi->maxbounds.metrics.descent);

    dst_temp_x = x - temp_x;
    dst_temp_y = y - temp_y;

    pTempBitm = (* pDraw->pScreen->CreatePixmap)(pDraw->pScreen, temp_width, temp_height, 1);
    if (!pTempBitm)
        return x;
    temp_bitm = ((apcPrivPMPtr)(pTempBitm->devPrivate.ptr))->bitmap_desc;
    SET_BITMAP (temp_bitm, pPrivScreen);

    /* draw the text into the temp bitmap */
    regionp = NULL;
    gfp = (gprFIDPtr) (pGC->font->devPriv[pGC->pScreen->myNum]);
    newx = gprText8 (pPrivScreen, regionp, gfp, temp_x, temp_y, count, chrcopy,
                     1, gpr_$transparent, &nocrect);

    /* silk-screen the foreground color thru the temp bitmap into the drawable */
    UPDATE_GPR (pGC, pPrivScreen);
    (* pGC->ops->PushPixels)(pGC, pTempBitm, pDraw, temp_width, temp_height, dst_temp_x, dst_temp_y);

    (* pDraw->pScreen->DestroyPixmap)(pTempBitm);
    return (newx + dst_temp_x - xorg);
}

/*
 * gprImageText8 -- DDX interface (GC "op" ImageText8)
 *      Do an ImageText8 via the gprText8 routine.
 *      Returns new x, drawable-relative.
 */
int
gprImageText8(pDraw, pGC, x, y, count, chars)
    DrawablePtr     pDraw;
    GCPtr           pGC;
    int             x, y;
    int             count;
    unsigned char  *chars;
{
    FontInfoPtr         pfi = pGC->font->pFI;
    int                 i;
    status_$t           status;
    gpr_$window_t       gwin;
    unsigned char       chrcopy[MAXCHRS];
    unsigned short      dflt16[MAXCHRS];
    RegionPtr           regionp;
    gprFIDPtr           gfp;
    int                 xorg, newx;
    apcPrivScreenPtr    pPrivScreen;

    if (count < 1)
        return x;

    /* handle long strings */
    while (count > MAXCHRS)
    {
        x = gprImageText8(pDraw, pGC, x, y, MAXCHRS, chars);
        count -= MAXCHRS;
        chars += MAXCHRS;
    }

    if (pfi->firstRow > 0)
    {
        /*
         * No 8-bit characters can be displayed in this font
         * at all.  Just build the appropriate 16-bit array of default
         * chrs, and give it to gprImageText16.
         */
        for (i=0; i<count; i++)
            dflt16[i] = pfi->chDefault;
        return gprImageText16( pDraw, pGC, x, y, count, dflt16);
    }

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    /* replace missing characters with chDefault */
    if (pfi->allExist && pfi->constantMetrics)
    {
        for (i=0; i<count; i++)
            if (chars[i] >= pfi->firstCol &&
                chars[i] <= pfi->lastCol)
                chrcopy[i] = chars[i];
            else
                chrcopy[i] = pfi->chDefault;
    }
    else
        gwin.window_size.x_size = rep_dfltch (pGC, Linear8Bit, chars, chrcopy, count);

    xorg = pDraw->x;
    x += xorg;
    y += pDraw->y;

    regionp = ((apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr))->pCompositeClip;
    gfp = (gprFIDPtr) (pGC->font->devPriv[pGC->pScreen->myNum]);

    if (pfi->terminalFont)
        newx = gprText8 (pPrivScreen, regionp, gfp, x, y, count, chrcopy,
                         pGC->fgPixel, pGC->bgPixel, &nocrect);
    else
    {   /* must fill background */
        if (pfi->constantMetrics)
            gwin.window_size.x_size = count * pfi->maxbounds.metrics.characterWidth;
        /* else width got computed above */

        gwin.window_base.x_coord = x;
        gwin.window_base.y_coord = y - pfi->fontAscent;
        gwin.window_size.y_size = pfi->fontAscent + pfi->fontDescent;
        SET_FILL_VALUE (pGC->bgPixel, pPrivScreen);

        newx = gprText8 (pPrivScreen, regionp, gfp, x, y, count, chrcopy,
                         pGC->fgPixel, gpr_$transparent, &gwin);
    }
    return (newx - xorg);
}

/*
 * gprText16 -- Driver internal code
 *      Does the work for both gprPolyText16 and gprImageText16 -- they differ only
 *      in the colors they use.
 *      Returns new x, screen-relative.
 */
static int
gprText16(pPrivScreen, regionp, gfp, pfi, x, y, count, chars, fgcolor, bgcolor, crect)
    apcPrivScreenPtr     pPrivScreen;
    RegionPtr            regionp;
    gprFIDPtr            gfp;
    FontInfoPtr          pfi;
    int                  x, y;
    int                  count;
    unsigned short      *chars;
    gpr_$pixel_value_t   fgcolor, bgcolor;
    gpr_$window_t       *crect;
{
    status_$t   status;
    short       clipx, cx;
    short       newx, newy;
    BoxPtr      pclip;
    int         nclips;

    SET_TEXT_VALUE (fgcolor, pPrivScreen);
    SET_TEXT_BACKGROUND_VALUE (bgcolor, pPrivScreen);

    if (regionp)
    {
        pclip = REGION_RECTS(regionp);
        nclips = REGION_NUM_RECTS(regionp);
    }
    else
    {
        SET_CLIPPING_ACTIVE (false, pPrivScreen);
        nclips = 1;
    }

    for (clipx = 0; clipx < nclips; clipx++)
    {
        gpr_$move ((short) x, (short) (y - 1), &status);

        if (nclips != 1)
        {
            gpr_$window_t   gwin;

            gwin.window_base.x_coord = pclip->x1;
            gwin.window_base.y_coord = pclip->y1;
            gwin.window_size.x_size = pclip->x2 - gwin.window_base.x_coord;
            gwin.window_size.y_size = pclip->y2 - gwin.window_base.y_coord;
            SET_CLIP_WINDOW (gwin, pPrivScreen);
            pclip++;
        }

        if (crect->window_size.x_size)
            gpr_$rectangle (*crect, &status);

#ifdef NO_TEXT16
        {
            short           charBase;
            int             fxCur, fxNext;
            int             i;
            unsigned short *pChars16;
            unsigned short *pChars16Last;
            char            chars8[MAXCHRS];
            Bool            done;

            charBase = pfi->firstRow * 256;
            pChars16 = chars;
            pChars16Last = chars + count;
            fxCur = (*pChars16 - charBase) >> 8;
            i = 0;
            do
            {
                chars8[i++] = *pChars16++ & 0xFF;
                done = (pChars16 == pChars16Last);
                if (!done)
                    fxNext = (*pChars16 - charBase) >> 8;
                if (done || (fxNext != fxCur))
                {
                    SET_TEXT_FONT (gfp->fontIds[fxCur], pPrivScreen);
                    fxCur = fxNext;
                    gpr_$text (chars8, (short) i, &status);
                    i = 0;
                }
            }
            while (!done);
       }
#else
        gpr_$text16 (chars, (short) count, &status);
#endif
    }
    gpr_$inq_cp (&newx, &newy, &status);

    return newx;
}

/*
 * gprPolyText16 -- DDX interface (GC "op" PolyText16)
 *      Do a solid PolyText16 via the gprText16 routine.
 *      Returns new x, drawable-relative.
 */
int
gprPolyText16(pDraw, pGC, x, y, count, chars)
    DrawablePtr     pDraw;
    GCPtr           pGC;
    int             x, y;
    int             count;
    unsigned short *chars;
{
    FontInfoPtr         pfi = pGC->font->pFI;
    BoxRec              cursorBox;
    unsigned short      chrcopy[MAXCHRS];
    int                 i;
    RegionPtr           regionp;
    gprFIDPtr           gfp;
    int                 xorg, newx;
    apcPrivGCPtr        pPrivGC;
    apcPrivScreenPtr    pPrivScreen;

    if (count < 1)
        return x;

    /* handle long strings */
    while (count > MAXCHRS)
    {
        x = gprPolyText16 (pDraw, pGC, x, y, MAXCHRS, chars);
        count -= MAXCHRS;
        chars += MAXCHRS;
    }

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    /* replace missing characters with chDefault */
    if (pfi->allExist && pfi->lastRow==0)
    {
        for (i=0; i<count; i++)
            if (chars[i] >= pfi->firstCol &&
                chars[i] <= pfi->lastCol)
                chrcopy[i] = chars[i];
            else
                chrcopy[i] = pfi->chDefault;
    }
    else
        rep_dfltch (pGC, Linear16Bit, chars, chrcopy, count);

    xorg = pDraw->x;
    x += xorg;
    y += pDraw->y;

    /* go actually draw the text */
    pPrivGC = (apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr);
    regionp = pPrivGC->pCompositeClip;
    gfp = (gprFIDPtr) (pGC->font->devPriv[pGC->pScreen->myNum]);
    newx = gprText16 (pPrivScreen, regionp, gfp, pfi, x, y, count, chrcopy,
                      pPrivGC->polyTextVal, gpr_$transparent, &nocrect);
    return (newx - xorg);
}

/*
 * gprPolyText16Stencil -- DDX interface (GC "op" PolyText16)
 *      Do a general PolyText16 via the gprText16 routine.
 *      Returns new x, drawable-relative.
 */
int
gprPolyText16Stencil(pDraw, pGC, x, y, count, chars)
    DrawablePtr     pDraw;
    GCPtr           pGC;
    int             x, y;
    int             count;
    unsigned short *chars;
{
    FontInfoPtr         pfi = pGC->font->pFI;
    int                 i;
    unsigned short      chrcopy[MAXCHRS];
    RegionPtr           regionp;
    gprFIDPtr           gfp;
    int                 xorg, newx;
    int                 temp_width, temp_height;
    int                 temp_x, temp_y;
    int                 dst_temp_x, dst_temp_y;
    PixmapPtr           pTempBitm;
    gpr_$bitmap_desc_t  temp_bitm;
    apcPrivScreenPtr    pPrivScreen;

    if (count < 1)
        return x;

    /* handle long strings */
    while (count > MAXCHRS)
    {
        x = gprPolyText16Stencil (pDraw, pGC, x, y, MAXCHRS, chars);
        count -= MAXCHRS;
        chars += MAXCHRS;
    }

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    /* replace missing characters with chDefault */
    if (pfi->allExist && pfi->lastRow==0)
    {
        for (i=0; i<count; i++)
            if (chars[i] >= pfi->firstCol &&
                chars[i] <= pfi->lastCol)
                chrcopy[i] = chars[i];
            else
                chrcopy[i] = pfi->chDefault;
    }
    else
        rep_dfltch (pGC, Linear16Bit, chars, chrcopy, count);

    xorg = pDraw->x;
    x += xorg;
    y += pDraw->y;

    temp_x = max (0, pfi->maxbounds.metrics.leftSideBearing);
    temp_width = temp_x + (count-1) * pfi->maxbounds.metrics.characterWidth +
                 max (pfi->maxbounds.metrics.characterWidth, pfi->maxbounds.metrics.rightSideBearing);
    temp_y = max (pfi->fontAscent, pfi->maxbounds.metrics.ascent);
    temp_height = temp_y + max (pfi->fontDescent, pfi->maxbounds.metrics.descent);

    dst_temp_x = x - temp_x;
    dst_temp_y = y - temp_y;

    pTempBitm = (* pDraw->pScreen->CreatePixmap)(pDraw->pScreen, temp_width, temp_height, 1);
    if (!pTempBitm)
        return x;
    temp_bitm = ((apcPrivPMPtr)(pTempBitm->devPrivate.ptr))->bitmap_desc;
    SET_BITMAP (temp_bitm, pPrivScreen);

    /* draw the text into the temp bitmap */
    regionp = NULL;
    gfp = (gprFIDPtr) (pGC->font->devPriv[pGC->pScreen->myNum]);
    newx = gprText16 (pPrivScreen, regionp, gfp, pfi, temp_x, temp_y, count, chrcopy,
                      1, gpr_$transparent, &nocrect);

    /* silk-screen the foreground color thru the temp bitmap into the drawable */
    UPDATE_GPR (pGC, pPrivScreen);
    (* pGC->ops->PushPixels)(pGC, pTempBitm, pDraw, temp_width, temp_height, dst_temp_x, dst_temp_y);

    (* pDraw->pScreen->DestroyPixmap)(pTempBitm);
    return (newx + dst_temp_x - xorg);
}

/*
 * gprImageText16 -- DDX interface (GC "op" ImageText16)
 *      Do an ImageText16 via the gprText16 routine.
 *      Returns new x, drawable-relative.
 */
int
gprImageText16(pDraw, pGC, x, y, count, chars)
    DrawablePtr     pDraw;
    GCPtr           pGC;
    int             x, y;
    int             count;
    unsigned short *chars;
{
    FontInfoPtr         pfi = pGC->font->pFI;
    int                 i;
    status_$t           status;
    gpr_$window_t       gwin;
    unsigned short      chrcopy[MAXCHRS];
    RegionPtr           regionp;
    gprFIDPtr           gfp;
    int                 xorg, newx;
    apcPrivScreenPtr    pPrivScreen;

    if (count < 1)
        return x;

    /* handle long strings */
    while (count > MAXCHRS)
    {
        x = gprImageText16 (pDraw, pGC, x, y, MAXCHRS, chars);
        count -= MAXCHRS;
        chars += MAXCHRS;
    }

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    /* replace missing characters with chDefault */
    if (pfi->allExist && pfi->constantMetrics && pfi->lastRow==0)
    {
        for (i=0; i<count; i++)
            if (chars[i] >= pfi->firstCol &&
                chars[i] <= pfi->lastCol)
                chrcopy[i] = chars[i];
            else
                chrcopy[i] = pfi->chDefault;
    }
    else
        gwin.window_size.x_size = rep_dfltch (pGC, Linear16Bit, chars, chrcopy, count);

    xorg = pDraw->x;
    x += xorg;
    y += pDraw->y;

    regionp = ((apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr))->pCompositeClip;
    gfp = (gprFIDPtr) (pGC->font->devPriv[pGC->pScreen->myNum]);

    if (pfi->terminalFont)
        newx = gprText16 (pPrivScreen, regionp, gfp, pfi, x, y, count, chrcopy,
                          pGC->fgPixel, pGC->bgPixel, &nocrect);

    else
    {   /* must fill background */
        if (pfi->constantMetrics)
            gwin.window_size.x_size = count * pfi->maxbounds.metrics.characterWidth;
        /* else width got computed above */

        gwin.window_base.x_coord = x;
        gwin.window_base.y_coord = y - pfi->fontAscent;
        gwin.window_size.y_size = pfi->fontAscent + pfi->fontDescent;
        SET_FILL_VALUE (pGC->bgPixel, pPrivScreen);

        newx = gprText16 (pPrivScreen, regionp, gfp, pfi, x, y, count, chrcopy,
                          pGC->fgPixel, gpr_$transparent, &gwin);
    }
    return (newx - xorg);
}

/*
 * nopText -- DDX interface (GC "op" [Poly/Image]Text[8/16])
 *      Do a text operation that actually boils down to a no-op,
 *      due to values of foreground, background, plane mask and alu.
 *      Return value doesn't really matter -- it's only used within
 *      dispatcher, never stored.
 */
int
nopText(pDraw, pGC, x, y, count, chars)
    DrawablePtr     pDraw;
    GCPtr           pGC;
    int             x, y;
    int             count;
    unsigned char  *chars;
{
    return x;
}
