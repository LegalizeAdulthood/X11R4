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

#define AP_NPOINTS 1024

/*
 * apcPolyPoint -- DDX interface (GC "op" PolyPoint)
 *      Use gpr_$multiline to do a PolyPoint.
 */
void
apcPolyPoint(pDraw, pGC, mode, npt, pptInit)
    DrawablePtr     pDraw;
    GCPtr           pGC;
    int             mode;
    int             npt;
    xPoint         *pptInit;
{
    status_$t           status;
    gpr_$coordinate_t   x[AP_NPOINTS], y[AP_NPOINTS];
    short               i = 0;
    RegionPtr           prgn;
    BoxPtr              pboxInit;
    int                 nboxInit;
    BoxPtr              pbox;
    int                 nbox;
    int                 xorg;
    int                 yorg;
    int                 nptTmp;
    xPoint             *ppt;
    int                 xi;
    int                 yi;
    apcPrivScreenPtr    pPrivScreen;

    if (!pGC->planemask)
        return;

    pPrivScreen = (apcPrivScreenPtr) pDraw->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    xorg = pDraw->x;
    yorg = pDraw->y;

    prgn = ((apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr))->pCompositeClip;
    pboxInit = REGION_RECTS(prgn);
    nboxInit = REGION_NUM_RECTS(prgn);

    if (nboxInit > 1)
        SET_CLIPPING_ACTIVE (false, pPrivScreen);

    /* translate the point list
       do this here rather than in the loop because there are
       two cases to deal with
    */
    ppt = pptInit;
    nptTmp = npt;
    if (mode == CoordModeOrigin)
    {
        while (nptTmp--)
        {
            ppt->x += xorg;
            ppt++->y += yorg;
        }
    }
    else
    {
        ppt->x += xorg;
        ppt->y += yorg;
        nptTmp--;
        while (nptTmp--)
        {
            ppt++;
            ppt->x += (ppt-1)->x;
            ppt->y += (ppt-1)->y;
        }
    }

    ppt = pptInit;
    while (npt--)
    {
        nbox = nboxInit;
        pbox = pboxInit;
        xi = ppt->x;
        yi = ppt++->y;

        while (nbox--)
        {
            if (yi < pbox->y1) /* scanline is before clip box */
                break;

            if ((xi < pbox->x2) && (yi < pbox->y2)) /* now or never */
            {
                if (xi >= pbox->x1)
                {
                    x[i] = xi;
                    y[i++] = yi;
                    x[i] = xi;
                    y[i++] = yi;
                    if (i > AP_NPOINTS - 2 )    /* each line requires 2 endpoints */
                    {
                        gpr_$multiline (x, y, i, &status);
                        i = 0;
                    }
                }
                break;
            }
            else
                pbox++;
        }
    }

    if (i > 0)  /* process leftovers */
        gpr_$multiline (x, y, i, &status);
}
