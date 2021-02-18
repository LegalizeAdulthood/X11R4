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

#define ADDRECT(reg,r,fr,rx1,ry1,rx2,ry2)			\
if (((rx1) < (rx2)) && ((ry1) < (ry2)) &&			\
    (!((reg)->data->numRects &&					\
       ((r-1)->y1 == (ry1)) &&					\
       ((r-1)->y2 == (ry2)) &&					\
       ((r-1)->x1 <= (rx1)) &&					\
       ((r-1)->x2 >= (rx2)))))					\
{								\
    if ((reg)->data->numRects == (reg)->data->size)		\
    {								\
	miRectAlloc(reg, 1);					\
	fr = REGION_BOXPTR(reg);				\
	r = fr + (reg)->data->numRects;				\
    }								\
    r->x1 = (rx1);						\
    r->y1 = (ry1);						\
    r->x2 = (rx2);						\
    r->y2 = (ry2);						\
    (reg)->data->numRects++;					\
    if(r->x1 < (reg)->extents.x1)				\
	(reg)->extents.x1 = r->x1;				\
    if(r->x2 > (reg)->extents.x2)				\
	(reg)->extents.x2 = r->x2;				\
    r++;							\
}

/*
 * apcPixmapToRegion -- DDX interface (screen)
 *      Convert bitmap clip mask into clipping region.
 *      First, goes through each line and makes boxes by noting the transitions
 *      from 0 to 1 and 1 to 0.
 *      Then it coalesces the current line with the previous if they have boxes
 *      at the same X coordinates.
 */
RegionPtr
apcPixmapToRegion(pPix)
    PixmapPtr   pPix;
{
    RegionPtr       pReg;
    unsigned       *pw, w;
    int             ib;
    int             width, h, base, rx1, crects;
    unsigned int   *pwLineStart;
    int             irectPrevStart, irectLineStart;
    BoxPtr          prectO, prectN;
    BoxPtr          FirstRect, rects, prectLineStart;
    Bool            fInBox, fSame;
    unsigned        mask0 = mask[0];

    pReg = (*pPix->drawable.pScreen->RegionCreate)(NULL, 1);
    if(!pReg)
        return NullRegion;
    FirstRect = REGION_BOXPTR(pReg);
    rects = FirstRect;
    width = pPix->drawable.width;
    pReg->extents.x1 = width - 1;
    pReg->extents.x2 = 0;
    pw = (unsigned int  *)((apcPrivPMPtr)(pPix->devPrivate.ptr))->bitmap_ptr;
    irectPrevStart = -1;
    for(h = 0; h < ((int)pPix->drawable.height); h++)
    {
        irectLineStart = rects - FirstRect;
        pwLineStart = pw;
        /* If the Screen left most bit of the word is set, we're starting in
         * a box */
        if(*pw & mask0)
        {
            fInBox = TRUE;
            rx1 = 0;
        }
        else
            fInBox = FALSE;
        /* Process all words which are fully in the pixmap */
        while(pw  < pwLineStart + width/32)
        {
            base = (pw - pwLineStart) * 32;
            w = *pw++;
            if (fInBox)
            {
                if (w == 0xFFFFFFFF)
                    continue;
            }
            else
            {
                if (w == 0)
                    continue;
            }
            for(ib = 0; ib < 32; ib++)
            {
                /* If the Screen left most bit of the word is set, we're
                 * starting a box */
                if(w & mask0)
                {
                    if(!fInBox)
                    {
                        rx1 = base + ib;
                        /* start new box */
                        fInBox = TRUE;
                    }
                }
                else
                {
                    if(fInBox)
                    {
                        /* end box */
                        ADDRECT(pReg, rects, FirstRect,
                                rx1, h, base + ib, h + 1);
                        fInBox = FALSE;
                    }
                }
                /* Shift the word VISUALLY left one. */
                w = SCRLEFT(w, 1);
            }
        }
        if(width & 0x1F)
        {
            /* Process final partial word on line */
            base = (pw - pwLineStart) * 32;
            w = *pw++;
            for(ib = 0; ib < (width & 0x1F); ib++)
            {
                /* If the Screen left most bit of the word is set, we're
                 * starting a box */
                if(w & mask0)
                {
                    if(!fInBox)
                    {
                        rx1 = base + ib;
                        /* start new box */
                        fInBox = TRUE;
                    }
                }
                else
                {
                    if(fInBox)
                    {
                        /* end box */
                        ADDRECT(pReg, rects, FirstRect,
                                rx1, h, base + ib, h + 1);
                        fInBox = FALSE;
                    }
                }
                /* Shift the word VISUALLY left one. */
                w = SCRLEFT(w, 1);
            }
        }
        /* If scanline ended with last bit set, end the box */
        if(fInBox)
        {
            ADDRECT(pReg, rects, FirstRect, rx1, h, base + ib, h + 1);
        }
        /* if all rectangles on this line have the same x-coords as
         * those on the previous line, then add 1 to all the previous  y2s and
         * throw away all the rectangles from this line
         */
        fSame = FALSE;
        if(irectPrevStart != -1)
        {
            crects = irectLineStart - irectPrevStart;
            if(crects == ((rects - FirstRect) - irectLineStart))
            {
                prectO = FirstRect + irectPrevStart;
                prectN = prectLineStart = FirstRect + irectLineStart;
                fSame = TRUE;
                while(prectO < prectLineStart)
                {
                    if((prectO->x1 != prectN->x1) || (prectO->x2 != prectN->x2))
                    {
                          fSame = FALSE;
                          break;
                    }
                    prectO++;
                    prectN++;
                }
                if (fSame)
                {
                    prectO = FirstRect + irectPrevStart;
                    while(prectO < prectLineStart)
                    {
                        prectO->y2 += 1;
                        prectO++;
                    }
                    rects -= crects;
                    pReg->data->numRects -= crects;
                }
            }
        }
        if(!fSame)
            irectPrevStart = irectLineStart;
    }
    if (!pReg->data->numRects)
        pReg->extents.x1 = pReg->extents.x2 = 0;
    else
    {
        pReg->extents.y1 = REGION_BOXPTR(pReg)->y1;
        pReg->extents.y2 = REGION_END(pReg)->y2;
        if (pReg->data->numRects == 1)
        {
            xfree(pReg->data);
            pReg->data = (RegDataPtr)NULL;
        }
    }
#ifdef DEBUG
    if (!miValidRegion(pReg))
        FatalError("Assertion failed file %s, line %d: expr\n", __FILE__, __LINE__);
#endif
    return(pReg);
}
