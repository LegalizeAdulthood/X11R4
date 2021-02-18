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

#define AP_NPOINTS 512

/*
 * gprdrawlines -- Driver internal code
 *      Inner loop for drawing polylines via GPR.
 */
static void
gprdrawlines(pPrivScreen, nptInit, pptInit, prgn)
    apcPrivScreenPtr    pPrivScreen;
    int                 nptInit;    /* number of points */
    DDXPointRec        *pptInit;    /* points in the polyline */
    RegionPtr           prgn;       /* clip region */
{
    status_$t           status;
    gpr_$coordinate_t   x[AP_NPOINTS], y[AP_NPOINTS];
    short               i;
    DDXPointRec        *ppt;
    BoxPtr              pclip;
    int                 nrects;
    int                 irect, npt, n;
    Bool                copy = TRUE;
    Bool                start;

    pclip = REGION_RECTS(prgn);
    nrects = REGION_NUM_RECTS(prgn);
    for (irect = 0; irect < nrects; irect++)
    {
        if (nrects > 1)
        {
            gpr_$window_t gwin;
            gwin.window_base.x_coord = pclip->x1;
            gwin.window_base.y_coord = pclip->y1;
            gwin.window_size.x_size = pclip->x2 - gwin.window_base.x_coord;
            gwin.window_size.y_size = pclip->y2 - gwin.window_base.y_coord;
            SET_CLIP_WINDOW (gwin, pPrivScreen);
            pclip++;
        }

        start = TRUE;   /* beginning of polyline */
        npt = nptInit;
        ppt = pptInit;
        while (npt)
        {
            n = min(npt, AP_NPOINTS);
            npt -= n;

            if (copy)
            {
                for (i = 0; i < n; i++, ppt++)
                {
                    x[i] = ppt->x;
                    y[i] = ppt->y;
                }
                copy = nptInit > AP_NPOINTS;
            }

            if (start)
            {
                gpr_$move (x[0], y[0], &status);
                gpr_$polyline (&x[1], &y[1], (short)(i-1), &status);
                start = FALSE;
            }
            else
                gpr_$polyline (x, y, i, &status);
        }
    }
}

/*
 * gprdrawsegs -- Driver internal code
 *      Inner loop for drawing segments via GPR.
 */
static void
gprdrawsegs(pPrivScreen, nsegInit, psegInit, prgn)
    apcPrivScreenPtr    pPrivScreen;
    int                 nsegInit;
    xSegment           *psegInit;
    RegionPtr           prgn;
{
    status_$t           status;
    gpr_$coordinate_t   x[AP_NPOINTS], y[AP_NPOINTS];
    short               i;
    xSegment           *pseg;
    BoxPtr              pclip;
    int                 nrects;
    int                 irect, iseg, nseg, n;
    Bool                copy = TRUE;

    pclip = REGION_RECTS(prgn);
    nrects = REGION_NUM_RECTS(prgn);
    for (irect = 0; irect < nrects; irect++)
    {
        if (nrects > 1)
        {
            gpr_$window_t gwin;
            gwin.window_base.x_coord = pclip->x1;
            gwin.window_base.y_coord = pclip->y1;
            gwin.window_size.x_size = pclip->x2 - gwin.window_base.x_coord;
            gwin.window_size.y_size = pclip->y2 - gwin.window_base.y_coord;
            SET_CLIP_WINDOW (gwin, pPrivScreen);
            pclip++;
        }

        nseg = nsegInit;
        pseg = psegInit;
        while (nseg)
        {
            n = min(nseg, (AP_NPOINTS>>1));
            nseg -= n;

            if (copy)
            {
                i = 0;
                for (iseg = 0; iseg < n; iseg++, pseg++)
                {
                    x[i]   = pseg->x1;
                    y[i++] = pseg->y1;
                    x[i]   = pseg->x2;
                    y[i++] = pseg->y2;
                }
                copy = nsegInit > (AP_NPOINTS>>1);
            }
            gpr_$multiline (x, y, i, &status);
        }
    }
}

/*
 * apcZeroLine -- DDX interface (GC "op" Polylines)
 *      Invoke gprdrawlines to do a Polylines with GPR.
 */
void
apcZeroLine(pDraw, pGC, mode, npt, pptInit)
    DrawablePtr     pDraw;
    GCPtr           pGC;
    int             mode;
    int             npt;
    DDXPointRec    *pptInit;
{
    RegionPtr           prgn;
    int                 xorg, yorg;
    DDXPointRec        *ppt;
    int                 i;
    apcPrivScreenPtr    pPrivScreen;

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    prgn = ((apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr))->pCompositeClip;

    xorg = pDraw->x;
    yorg = pDraw->y;

    ppt = pptInit;
    if (mode == CoordModeOrigin)
    {
        for (i = 0; i<npt; i++)
        {
            ppt->x += xorg;
            ppt++->y += yorg;
        }
    }
    else
    {
        ppt->x += xorg;
        ppt++->y += yorg;
        for (i = 1; i<npt; i++)
        {
            ppt->x += (ppt-1)->x;
            ppt->y += (ppt-1)->y;
            ppt++;
        }
    }

    gprdrawlines (pPrivScreen, npt, pptInit, prgn);
}

/*
 * apcZeroSegment -- DDX interface (GC "op" PolySegment)
 *      Invoke gprdrawsegs to do a PolySegment with GPR.
 */
void
apcZeroSegment(pDraw, pGC, nseg, psegInit)
    DrawablePtr     pDraw;
    GCPtr           pGC;
    int             nseg;
    xSegment       *psegInit;
{
    RegionPtr       prgn;
    int             xorg, yorg;
    xSegment       *pseg;
    int             i;
    apcPrivScreenPtr    pPrivScreen;

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    prgn = ((apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr))->pCompositeClip;

    xorg = pDraw->x;
    yorg = pDraw->y;

    pseg = psegInit;
    for (i = 0; i<nseg; i++)
    {
        pseg->x1 += xorg;
        pseg->y1 += yorg;
        pseg->x2 += xorg;
        pseg++->y2 += yorg;
    }

    gprdrawsegs (pPrivScreen, nseg, psegInit, prgn);
}

/*
 * apcZeroDashLine -- DDX interface (GC "op" Polylines)
 *      Invoke gprdrawlines to do a Polylines with GPR,
 *      using GPR dashing.
 */
void
apcZeroDashLine(pDraw, pGC, mode, npt, pptInit)
    DrawablePtr     pDraw;
    GCPtr           pGC;
    int             mode;
    int             npt;
    DDXPointRec    *pptInit;
{
    RegionPtr           prgn;
    int                 xorg, yorg;
    DDXPointRec        *ppt;
    int                 i;
    apcDrawPatternRec   inverse_dashes;
    apcPrivGCPtr        pPrivGC;
    apcPrivScreenPtr    pPrivScreen;

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    prgn = ((apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr))->pCompositeClip;

    xorg = pDraw->x;
    yorg = pDraw->y;

    ppt = pptInit;
    if (mode == CoordModeOrigin)
    {
        for (i = 0; i<npt; i++)
        {
            ppt->x += xorg;
            ppt++->y += yorg;
        }
    }
    else
    {
        ppt->x += xorg;
        ppt++->y += yorg;
        for (i = 1; i<npt; i++)
        {
            ppt->x += (ppt-1)->x;
            ppt->y += (ppt-1)->y;
            ppt++;
        }
    }

    gprdrawlines (pPrivScreen, npt, pptInit, prgn);

    if (pGC->lineStyle == LineDoubleDash)
    {
        SET_DRAW_VALUE (pGC->bgPixel, pPrivScreen);

        pPrivGC = (apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr);
        inverse_dashes = pPrivGC->reqContext.draw_pattern;
        for (i = 0; i < (MAX_DASH_SIZE >> 4); i++)
            inverse_dashes.pattern[i] = ~inverse_dashes.pattern[i];
        SET_DRAW_PATTERN (inverse_dashes, pPrivScreen);

        gprdrawlines (pPrivScreen, npt, pptInit, prgn);
    }
}

/*
 * apcZeroDashSeg -- DDX interface (GC "op" PolySegment)
 *      Invoke gprdrawsegs to do a PolySegment with GPR,
 *      using GPR dashing.
 */
void
apcZeroDashSeg(pDraw, pGC, nseg, psegInit)
    DrawablePtr     pDraw;
    GCPtr           pGC;
    int             nseg;
    xSegment       *psegInit;
{
    RegionPtr           prgn;
    int                 xorg, yorg;
    xSegment           *pseg;
    int                 i;
    apcDrawPatternRec   inverse_dashes;
    apcPrivGCPtr        pPrivGC;
    apcPrivScreenPtr    pPrivScreen;

    pPrivScreen = (apcPrivScreenPtr) pDraw->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    prgn = ((apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr))->pCompositeClip;

    xorg = pDraw->x;
    yorg = pDraw->y;

    pseg = psegInit;
    for (i = 0; i<nseg; i++)
    {
        pseg->x1 += xorg;
        pseg->y1 += yorg;
        pseg->x2 += xorg;
        pseg++->y2 += yorg;
    }

    gprdrawsegs (pPrivScreen, nseg, psegInit, prgn);

    if (pGC->lineStyle == LineDoubleDash)
    {

        SET_DRAW_VALUE (pGC->bgPixel, pPrivScreen);

        pPrivGC = (apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr);
        inverse_dashes = pPrivGC->reqContext.draw_pattern;
        for (i = 0; i < (MAX_DASH_SIZE >> 4); i++)
            inverse_dashes.pattern[i] = ~inverse_dashes.pattern[i];
        SET_DRAW_PATTERN (inverse_dashes, pPrivScreen);

        gprdrawsegs (pPrivScreen, nseg, psegInit, prgn);
    }
}
