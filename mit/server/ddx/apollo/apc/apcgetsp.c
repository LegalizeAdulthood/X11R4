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

/*
 * apcGetSpans -- DDX interface (Screen)
 *      For each span, gets bits from drawable starting at ppt[i]
 *      and continuing for pwidth[i] bits
 *      Each scanline returned will be server scanline padded, i.e., it will come
 *      out to an integral number of words.
 *
 *  HACK ALERT
 *      THIS IS CODED ASSUMING 8 BITS PER PIXEL, even though it
 *      looks up the bits per pixel in the screen info record.
 *      The pdst pointer is an unsigned char pointer.
 */
void
apcGetSpans(pDraw, wMax, ppt, pwidth, nspans, pdstStart)
    DrawablePtr     pDraw;      /* drawable from which to get bits */
    int             wMax;       /* largest value of all *pwidths */
    DDXPointPtr     ppt;        /* points to start copying from */
    int            *pwidth;     /* list of number of bits to copy */
    int             nspans;     /* number of scanlines to copy */
    unsigned int   *pdstStart;  /* where to put the bits */
{
    int                *psrc;       /* where to get the bits */
    int                 tmpSrc;     /* scratch buffer for bits */
    int                *psrcBase;   /* start of src bitmap */
    int                 widthSrc;   /* width of pixmap in bytes */
    int                 xEnd;       /* last pixel to copy from */
    int                 nstart;
    int                 nend;
    int                 srcStartOver;
    int                 startmask, endmask, nlMiddle, srcBit;
    int                 w;
    unsigned int       *pdstNext;
    int                *BaseBase;
    short               width;
    DDXPointPtr         ppttemp;
    int                *pwidthtemp;
    int                 pdstoffset, nspanstemp, mainmemoffset;
    gpr_$offset_t       size;
    gpr_$rgb_plane_t    depth;
    gpr_$bitmap_desc_t  src_bitmap;
    apcPrivScreenPtr    pPrivScreen;
    status_$t           status;

    depth = (gpr_$rgb_plane_t)pDraw->depth;

    if (pDraw->type == DRAWABLE_WINDOW)
    {
        apDisplayDataPtr    pDisp;

        pDisp = (apDisplayDataPtr) pDraw->pScreen->devPrivates[apDisplayScreenIndex].ptr;
        src_bitmap = pDisp->display_bitmap;
        psrcBase = (int *)pDisp->bitmap_ptr;
        widthSrc = pDisp->words_per_line >> 1;
    }
    else
    {
        apcPrivPMPtr    pPrivPM;

        pPrivPM = (apcPrivPMPtr)(((PixmapPtr)pDraw)->devPrivate.ptr);
        src_bitmap = pPrivPM->bitmap_desc;
        psrcBase = BaseBase = pPrivPM->bitmap_ptr;
        widthSrc = pPrivPM->width >> 1;
        mainmemoffset = widthSrc * pPrivPM->size.y_size;
    }

    pPrivScreen = (apcPrivScreenPtr) pDraw->pScreen->devPrivates[apcScreenIndex].ptr;
    SET_BITMAP (src_bitmap, pPrivScreen);

    gpr_$enable_direct_access (&status);
    if (depth == 1)
    {
        DDXPointPtr     pptLast;        /* one past last point to get */
        int             srcStartOver;
        unsigned int   *pdst;           /* where to put the bits */

        pptLast = ppt + nspans;

        pdstNext = pdstStart;

        while (ppt < pptLast)
        {
            pdst = pdstNext;
            pdstNext += (PixmapBytePad(wMax, 1)>>2);
            xEnd = min(ppt->x + *pwidth, widthSrc << 5);
            pwidth++;
            psrc = psrcBase + (ppt->y * widthSrc) + (ppt->x >> 5);
            w = xEnd - ppt->x;
            srcBit = ppt->x & 0x1f;

            if (srcBit + w <= 32)
            {
                getbits(psrc, srcBit, w, tmpSrc);
                putbits(tmpSrc, 0, w, pdst);
            }
            else
            {
                maskbits(ppt->x, w, startmask, endmask, nlMiddle);
                if (startmask)
                    nstart = 32 - srcBit;
                else
                    nstart = 0;
                if (endmask)
                    nend = xEnd & 0x1f;
                srcStartOver = srcBit + nstart > 31;
                if (startmask)
                {
                    getbits(psrc, srcBit, nstart, tmpSrc);
                    putbits(tmpSrc, 0, nstart, pdst);
                    if (srcStartOver)
                        psrc++;
                }
                while (nlMiddle--)
                {
                    tmpSrc = *psrc;
                    putbits(tmpSrc, nstart, 32, pdst);
                    psrc++;
                    pdst++;
                }
                if (endmask)
                {
                    getbits(psrc, 0, nend, tmpSrc);
                    putbits(tmpSrc, nstart, nend, pdst);
                }
            }
            ppt++;
        }
    }
    else
    {
        char           *pdst;   /* where to put the bits */
        unsigned long   nbytes;

        pdstoffset = PixmapBytePad(wMax, depth);
        nbytes = nspans * pdstoffset;
        pdstoffset = pdstoffset >> 2;

        /* clear out unused depth */
        if (depth <= gpr_$highest_plane)
            bzero (pdstStart, nbytes);

        pwidthtemp = pwidth;
        ppttemp = ppt;

        while (depth--)
        {
            if (pDraw->type == DRAWABLE_WINDOW)
                gpr_$remap_color_memory (depth, &status);
            else
                psrcBase = (int *)(mainmemoffset*depth+BaseBase);

            pwidth = pwidthtemp;
            ppt = ppttemp;
            nspanstemp = nspans;
            pdstNext = pdstStart;

            while (nspanstemp--)
            {
                xEnd = min(ppt->x + *pwidth, widthSrc << 5);
                psrc = psrcBase + ppt->y * widthSrc + (ppt->x >> 5);
                w = xEnd - ppt->x;
                srcBit = ppt->x & 0x1f;

                pdst = (char *)pdstNext;
                pdstNext = pdstNext + pdstoffset;

                if (srcBit + w <= 32)
                {
                    getbits(psrc, srcBit, w, tmpSrc);
                    putbitsapc(depth, tmpSrc, w, pdst);
                }
                else
                {
                    maskbits(ppt->x, w, startmask, endmask, nlMiddle);
                    if (startmask)
                        nstart = 32 - srcBit;
                    else
                        nstart = 0;
                    if (endmask)
                        nend = xEnd & 0x1f;
                    srcStartOver = srcBit + nstart > 31;
                    if (startmask)
                    {
                       getbits(psrc, srcBit, nstart, tmpSrc);
                       putbitsapc(depth, tmpSrc, nstart, pdst);
                       if (srcStartOver)
                           psrc++;
                    }
                    while (nlMiddle--)
                    {
                        tmpSrc = *psrc++;
                        putbitsapc(depth, tmpSrc, 32, pdst);
                    }
                    if (endmask)
                    {
                        getbits(psrc, 0, nend, tmpSrc);
                        putbitsapc(depth, tmpSrc, nend, pdst);
                    }
                    /* assuming no need to zero remainder of span memory */
                }
                ppt++;
                pwidth++;
            }
        }
    }
}
