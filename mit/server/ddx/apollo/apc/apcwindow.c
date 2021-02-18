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
#include "mistruct.h"

/*
 * apcPaintWindow -- DDX interface (screen)
 *      Detect and handle here the case where the given region is null,
 *      or where painting with a solid color.  If neither case, call miPaintWindow.
 */
void
apcPaintWindow (pWindow, pRegion, kind)
    WindowPtr   pWindow;
    RegionPtr   pRegion;
    int         kind;
{
    gpr_$pixel_value_t      pixel;
    status_$t               status;
    ScreenPtr               pScreen;
    apDisplayDataPtr        pDisp;
    apcPrivScreenPtr        pPrivScreen;
    BoxPtr                  pBox;
    BoxPtr                  pPastLastBox;
    gpr_$window_t           window;

    if (REGION_NIL(pRegion))
        return;

    switch (kind)
    {
    case PW_BACKGROUND:
        switch (pWindow->backgroundState)
        {
        case None:
            return;
        case ParentRelative:
            do
                pWindow = pWindow->parent;
            while (pWindow->backgroundState == ParentRelative);

            (*pWindow->drawable.pScreen->PaintWindowBackground) (pWindow, pRegion, kind);
            return;
        case BackgroundPixmap:
            miPaintWindow (pWindow, pRegion, kind);
            return;
        case BackgroundPixel:
            pixel = (gpr_$pixel_value_t) pWindow->background.pixel;
            break;
        }
        break;
    case PW_BORDER:
        if (pWindow->borderIsPixel)
            pixel = (gpr_$pixel_value_t) pWindow->border.pixel;
        else
        {
            miPaintWindow (pWindow, pRegion, kind);
            return;
        }
        break;
    }

    /*
     * Paint each box in the given region with the solid color in pixel,
     * copy mode, all planes.  Set the box as the clip window, use gpr_$clear.
     */
    pScreen = pWindow->drawable.pScreen;
    pPrivScreen = (apcPrivScreenPtr) pScreen->devPrivates[apcScreenIndex].ptr;
    pDisp = (apDisplayDataPtr) pScreen->devPrivates[apDisplayScreenIndex].ptr;
    SET_BITMAP (pDisp->display_bitmap, pPrivScreen);
    SET_CLIPPING_ACTIVE (true, pPrivScreen);
    SET_PLANE_MASK (0xFFFFFFFF, pPrivScreen);

    pPastLastBox = (pBox = REGION_RECTS(pRegion)) + REGION_NUM_RECTS(pRegion);
    while (pBox < pPastLastBox)
    {
        window.window_size.x_size = pBox->x2 - (window.window_base.x_coord = pBox->x1);
        window.window_size.y_size = pBox->y2 - (window.window_base.y_coord = pBox->y1);
        SET_CLIP_WINDOW (window, pPrivScreen);
        gpr_$clear (pixel, &status);
        pBox++;
    }
}

/*
 * apcCreateWindow -- DDX interface (screen)
 *      No driver-dependent work to do for window creation.
 */
Bool
apcCreateWindow(pWin)
    WindowPtr pWin;
{
    return TRUE;
}

/*
 * apcDestroyWindow -- DDX interface (screen)
 *      No driver-dependent work to do for window destruction.
 */
Bool
apcDestroyWindow(pWin)
    WindowPtr pWin;
{
    return TRUE;
}

/*
 * apcMapWindow -- DDX interface (screen)
 *      No driver-dependent work to do for window mapping.
 */
Bool
apcMapWindow(pWindow)
    WindowPtr pWindow;
{
    return TRUE;
}

/*
 * apcPositionWindow -- DDX interface (screen)
 *      No driver-dependent work to do for window positioning.
 */
Bool
apcPositionWindow(pWin, x, y)
    WindowPtr pWin;
    int x, y;
{
    return TRUE;
}

/*
 * apcUnmapWindow -- DDX interface (screen)
 *      No driver-dependent work to do for window unmapping.
 */
Bool
apcUnmapWindow(pWindow, x, y)
    WindowPtr pWindow;
    int x, y;
{
    return TRUE;
}

/*
 * apcCopyWindow -- DDX interface (screen)
 *      Do direct screen-to-screen blts to move a window.
 */
void
apcCopyWindow(pWin, ptOldOrg, prgnSrc)
    WindowPtr   pWin;
    DDXPointRec ptOldOrg;
    RegionPtr   prgnSrc;
{
    ScreenPtr           pScreen;
    apDisplayDataPtr    pDisp;
    RegionPtr           prgnDst;
    BoxPtr              pbox;
    BoxPtr              pboxTmp, pboxNext, pboxBase;
    BoxPtr              pboxNew, pboxNew1, pboxNew2;
    int                 dx, dy;
    int                 i, nbox;
    gpr_$window_t       srcrect;
    gpr_$position_t     dst;
    status_$t           status;
    apcPrivScreenPtr    pPrivScreen;
    GCPtr               pGC;

    pScreen = pWin->drawable.pScreen;

    prgnDst = (* pScreen->RegionCreate)(NULL, REGION_NUM_RECTS(&pWin->borderClip));

    dx = ptOldOrg.x - pWin->drawable.x;
    dy = ptOldOrg.y - pWin->drawable.y;
    (* pWin->drawable.pScreen->TranslateRegion)(prgnSrc, -dx, -dy);
    (* pWin->drawable.pScreen->Intersect)(prgnDst, &pWin->borderClip, prgnSrc);

    pbox = REGION_RECTS(prgnDst);
    nbox = REGION_NUM_RECTS(prgnDst);

    pboxNew1 = pboxNew2 = NULL;

    if (dy < 0)
    {
        /* walk source bottom to top */

        if (nbox > 1)
        {
            /* keep ordering in each band, reverse order of bands */
            pboxNew = pboxNew1 = (BoxPtr)ALLOCATE_LOCAL(sizeof(BoxRec) * nbox);
            if (!pboxNew)
            {
                DEALLOCATE_LOCAL(pboxNew);
                return;
            }
            pboxBase = pboxNext = pbox+nbox-1;
            while (pboxBase >= pbox)
            {
                while ((pboxNext >= pbox) &&
                       (pboxBase->y1 == pboxNext->y1))
                    pboxNext--;
                pboxTmp = pboxNext+1;
                while (pboxTmp <= pboxBase)
                    *pboxNew++ = *pboxTmp++;
                pboxBase = pboxNext;
            }
            pboxNew -= nbox;
            pbox = pboxNew;
        }
    }

    if (dx < 0)
    {
        /* walk source right to left */

        if (nbox > 1)
        {
            /* reverse order of rects in each band */
            pboxNew = pboxNew2 = (BoxPtr)ALLOCATE_LOCAL(sizeof(BoxRec) * nbox);
            pboxBase = pboxNext = pbox;
            if(!pboxNew)
            {
                DEALLOCATE_LOCAL(pboxNew);
                return;
            }
            while (pboxBase < pbox+nbox)
            {
                while ((pboxNext < pbox+nbox) &&
                       (pboxNext->y1 == pboxBase->y1))
                    pboxNext++;
                pboxTmp = pboxNext;
                while (pboxTmp != pboxBase)
                    *pboxNew++ = *--pboxTmp;
                pboxBase = pboxNext;
            }
            pboxNew -= nbox;
            pbox = pboxNew;
        }
    }

    pDisp = (apDisplayDataPtr) pScreen->devPrivates[apDisplayScreenIndex].ptr;
    pPrivScreen = (apcPrivScreenPtr) pScreen->devPrivates[apcScreenIndex].ptr;
    SET_BITMAP (pDisp->display_bitmap, pPrivScreen);
    SET_CLIPPING_ACTIVE (false, pPrivScreen);
    SET_PLANE_MASK (0xFFFFFFFF, pPrivScreen);
    SET_RASTER_OP_COMMON (GXcopy, pPrivScreen, pScreen->rootDepth);

    for (i=0; i<nbox; i++, pbox++)
    {
        dst.x_coord = pbox->x1;
        dst.y_coord = pbox->y1;
        srcrect.window_base.x_coord = pbox->x1 + dx;
        srcrect.window_base.y_coord = pbox->y1 + dy;
        srcrect.window_size.x_size = pbox->x2 - pbox->x1;
        srcrect.window_size.y_size = pbox->y2 - pbox->y1;

        gpr_$pixel_blt (pDisp->display_bitmap, srcrect, dst, &status);
    }

    (* pWin->drawable.pScreen->RegionDestroy)(prgnDst);
    if (pboxNew1)
    {
        DEALLOCATE_LOCAL(pboxNew1);
    }
    if (pboxNew2)
    {
        DEALLOCATE_LOCAL(pboxNew2);
    }
}

/*
 * apcChangeWindowAttributes -- DDX interface (screen)
 *      No driver-dependent work to do for window attribute changes.
 */
Bool
apcChangeWindowAttributes(pWin, mask)
    WindowPtr pWin;
    int mask;
{
    return TRUE;
}
