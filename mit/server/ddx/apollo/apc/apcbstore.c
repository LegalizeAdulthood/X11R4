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
#include "mibstore.h"

/*
 * apcDoBitblt -- Driver internal code
 *      Do a string of bitblts from one GPR bitmap to another.
 *      They are assumed to be different, so that order does not matter.
 */
static void
apcDoBitblt(pScreen, src, dst, depth, rop, prgn, xorg, yorg)
    ScreenPtr           pScreen;
    gpr_$bitmap_desc_t  src, dst;       /* source, destination bitmaps */
    int                 depth;          /* depth (of both) */
    gpr_$raster_op_t    rop;            /* raster op */
    RegionPtr           prgn;           /* destination-relative region */
    int                 xorg, yorg;     /* source offset */
{
    gpr_$window_t       win;            /* src window */
    gpr_$position_t     pos;            /* dst position */
    apcPrivScreenPtr    pPrivScreen;
    BoxPtr              pBox;
    int                 i;
    status_$t           status;

    pPrivScreen = (apcPrivScreenPtr) pScreen->devPrivates[apcScreenIndex].ptr;

    SET_BITMAP (dst, pPrivScreen);
    SET_CLIPPING_ACTIVE (false, pPrivScreen);
    SET_PLANE_MASK (0xFFFFFFFF, pPrivScreen);
    SET_RASTER_OP_COMMON (rop, pPrivScreen, depth);

    for (pBox = REGION_RECTS(prgn), i = REGION_NUM_RECTS(prgn); i > 0; i--, pBox++)
    {
        pos.x_coord = pBox->x1;                         /* dst x */
        pos.y_coord = pBox->y1;                         /* dst y */
        win.window_base.x_coord = pBox->x1 + xorg;      /* src x */
        win.window_base.y_coord = pBox->y1 + yorg;      /* src y */
        win.window_size.x_size = pBox->x2 - pBox->x1;   /* x size */
        win.window_size.y_size = pBox->y2 - pBox->y1;   /* y size */

        gpr_$pixel_blt (src, win, pos, &status);
    }
}

/*
 * apcSaveAreas -- DDX interface (mi backingstore)
 *      Copy window areas to be saved into the backing pixmap.
 */
void
apcSaveAreas(pPixmap, prgnSave, xorg, yorg)
    PixmapPtr   pPixmap;        /* Backing pixmap */
    RegionPtr   prgnSave;       /* Region to save (pixmap-relative) */
    int         xorg;           /* X origin of region */
    int         yorg;           /* Y origin of region */
{
    ScreenPtr           pScreen;
    apDisplayDataPtr    pDisp;
    gpr_$bitmap_desc_t  src, dst;

    pScreen = pPixmap->drawable.pScreen;
    pDisp = (apDisplayDataPtr) pScreen->devPrivates[apDisplayScreenIndex].ptr;

    src = pDisp->display_bitmap;
    dst = ((apcPrivPMPtr)(pPixmap->devPrivate.ptr))->bitmap_desc;

    apcDoBitblt(pScreen, src, dst, pScreen->rootDepth,
                (gpr_$raster_op_t)GXcopy, prgnSave, xorg, yorg);
}

/*
 * apcRestoreAreas -- DDX interface (mi backingstore)
 *      Copy window areas to be restored from the backing pixmap.
 */
void
apcRestoreAreas(pPixmap, prgnRestore, xorg, yorg)
    PixmapPtr   pPixmap;        /* Backing pixmap */
    RegionPtr   prgnRestore;    /* Region to restore (screen-relative) */
    int         xorg;           /* X origin of window */
    int         yorg;           /* Y origin of window */
{
    ScreenPtr           pScreen;
    apDisplayDataPtr    pDisp;
    gpr_$bitmap_desc_t  src, dst;

    pScreen = pPixmap->drawable.pScreen;
    pDisp = (apDisplayDataPtr) pScreen->devPrivates[apDisplayScreenIndex].ptr;

    src = ((apcPrivPMPtr)(pPixmap->devPrivate.ptr))->bitmap_desc;
    dst = pDisp->display_bitmap;

    apcDoBitblt(pScreen, src, dst, pScreen->rootDepth,
                (gpr_$raster_op_t)GXcopy, prgnRestore, -xorg, -yorg);
}
