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

/* $XConsortium: apcscrinit.c,v 1.3 90/02/22 10:38:44 rws Exp $ */

#include "apc.h"
#include "Xmd.h"
#include "servermd.h"
#include "mistruct.h"
#include "mibstore.h"

/*
 * apc driver data record goes here.
 */
apcDataRec  APCData;

/*
 * Vector of mi backing store funcs.
 */
static miBSFuncRec apcBSFuncRec = {
    apcSaveAreas,
    apcRestoreAreas,
    (void (*)()) 0,
    (PixmapPtr (*)()) 0,
    (PixmapPtr (*)()) 0,
};


/*
 * Variables controlling allocation of apc screen- and GC-privates.
 */
int                     apcScreenIndex;
int                     apcGCPrivateIndex;
static unsigned long    apcGeneration = 0;


/*
 * apcCloseScreen -- DDX interface (screen)
 *      Close the screen.  Free private resources.
 */
static Bool
apcCloseScreen(index, pScreen)
    int         index;
    ScreenPtr   pScreen;
{
    apcPrivScreenPtr        pPrivScreen;

    xfree (APCData.polygons.x_pts_ptr);
    xfree (APCData.polygons.y_pts_ptr);
    xfree (APCData.polygons.t_list_ptr);

    pPrivScreen = (apcPrivScreenPtr)pScreen->devPrivates[apcScreenIndex].ptr;
    xfree (pPrivScreen);
}

/*
 * apcScreenInit -- Driver internal code.
 *      Initialize the apc driver.
 *      This involves making our screen- and GC-privates,
 *      filling in the vectors in the ScreenRec, and initializing
 *      mi backing store.
 */
Bool
apcScreenInit(pScreen, pDisp, pClrPrivScreen)
    ScreenPtr           pScreen;
    apDisplayDataPtr    pDisp;
    apClrPrivScreenPtr  pClrPrivScreen;
{
    apcPrivScreenPtr                pPrivScreen;
    status_$t                       status;
    gpr_$window_t                   window;
    apcDrawPatternRec               draw_pattern;
    gpr_$triangle_fill_criteria_t   triangle_fill_criteria;
    unsigned short int              rop_set;
    VisualPtr                       pVis;
    VisualID                       *pVid;
    DepthPtr                        pDepth0, pDepth1;

    if (apcGeneration != serverGeneration)
    {
        if ((apcScreenIndex = AllocateScreenPrivateIndex()) < 0)
            return FALSE;
        apcGCPrivateIndex = AllocateGCPrivateIndex();
        apcGeneration = serverGeneration;
    }

    pPrivScreen = (apcPrivScreenPtr) xalloc(sizeof(apcPrivScreenRec));
    if (!pPrivScreen)
        return FALSE;
    pScreen->devPrivates[apcScreenIndex].ptr = (pointer) pPrivScreen;

    if (!AllocateGCPrivate (pScreen, apcGCPrivateIndex, sizeof (apcPrivGC)))
        return FALSE;

    pPrivScreen->attribute_block = gpr_$attribute_block (pDisp->display_bitmap, &status);

    /* initialize GPR attributes in attribute block for display and Pixmaps */
    apSetBitmap (pDisp->display_bitmap);
    pPrivScreen->curContext.bitmap = pDisp->display_bitmap;

    pPrivScreen->curContext.clipping_active = true;
    gpr_$set_clipping_active (true, &status);

    window.window_base.x_coord = 0;
    window.window_base.y_coord = 0;
    window.window_size.x_size = pScreen->width;
    window.window_size.y_size = pScreen->height;
    pPrivScreen->curContext.clip_window = window;
    gpr_$set_clip_window (window, &status);

    pPrivScreen->curContext.plane_mask = 0xFFFFFFFF;
    gpr_$set_plane_mask_32 ((gpr_$mask_32_t) 0xFFFFFFFF, &status);

    pPrivScreen->curContext.raster_op.rop_common = GXcopy;
    gpr_$set_raster_op_mask ((gpr_$mask_32_t) 0xFFFFFFFF, (gpr_$raster_op_t) GXcopy, &status);

    pPrivScreen->curContext.draw_value = 0xFFFFFFFF;
    gpr_$set_draw_value ((gpr_$pixel_value_t) 0xFFFFFFFF, &status);

    draw_pattern.pattern[0] = 0;
    draw_pattern.pattern[1] = 0;
    draw_pattern.pattern[2] = 0;
    draw_pattern.pattern[3] = 0;
    draw_pattern.repeat_count = 0;
    draw_pattern.length = 0;
    pPrivScreen->curContext.draw_pattern = draw_pattern;
    gpr_$set_draw_pattern (draw_pattern.repeat_count, draw_pattern.pattern,
                           draw_pattern.length, &status);

    pPrivScreen->curContext.fill_value = 0xFFFFFFFF;
    gpr_$set_fill_value ((gpr_$pixel_value_t) 0xFFFFFFFF, &status);

    pPrivScreen->curContext.fill_background_value = 0;
    gpr_$set_fill_background_value ((gpr_$pixel_value_t) 0, &status);

    pPrivScreen->curContext.fill_pattern = gpr_$nil_bitmap_desc;
    gpr_$set_fill_pattern (gpr_$nil_bitmap_desc, (short) 1, &status);

    triangle_fill_criteria.wind_type = gpr_$parity;
    triangle_fill_criteria.winding_no = 1;
    pPrivScreen->curContext.triangle_fill_criteria = triangle_fill_criteria;
    gpr_$set_triangle_fill_criteria (triangle_fill_criteria, &status);

    pPrivScreen->curContext.pgon_decomp_technique = gpr_$non_overlapping_tris;
    gpr_$pgon_decomp_technique (gpr_$non_overlapping_tris, &status);

    pPrivScreen->curContext.text_value = 0xFFFFFFFF;
    gpr_$set_text_value ((gpr_$pixel_value_t) 0xFFFFFFFF, &status);

    pPrivScreen->curContext.text_background_value = 0;
    gpr_$set_text_background_value ((gpr_$pixel_value_t) 0, &status);

    pPrivScreen->curContext.text_font = 0;
    gpr_$set_text_font (0, &status);

    /* HACK - setting prim set to ALL three members, fill, blt and line */
    rop_set = 7;
    gpr_$raster_op_prim_set (rop_set, &status);

    pPrivScreen->currentGC = (GCPtr)NULL;

    APCData.polygons.x_pts_ptr = (gpr_$coordinate_t *)(xalloc (INITIAL_N_POINTS*sizeof(gpr_$coordinate_t)));
    if (!APCData.polygons.x_pts_ptr)
        return FALSE;
    APCData.polygons.y_pts_ptr = (gpr_$coordinate_t *)(xalloc (INITIAL_N_POINTS*sizeof(gpr_$coordinate_t)));
    if (!APCData.polygons.y_pts_ptr)
        return FALSE;
    APCData.polygons.t_list_ptr = (gpr_$triangle_t *)(xalloc (INITIAL_N_TRIANGLES*sizeof(gpr_$triangle_t)));
    if (!APCData.polygons.t_list_ptr)
        return FALSE;
    APCData.polygons.n_points = INITIAL_N_POINTS;
    APCData.polygons.n_triangles = INITIAL_N_TRIANGLES;

    pScreen->width = pDisp->display_char.x_visible_size;
    pScreen->height = pDisp->display_char.y_visible_size;
    pScreen->mmWidth = (pScreen->width * 10) / pDisp->display_char.x_pixels_per_cm;
    pScreen->mmHeight = (pScreen->height * 10) / pDisp->display_char.y_pixels_per_cm;

    pScreen->backingStoreSupport = Always;
    pScreen->saveUnderSupport = NotUseful;

    /* anything that apc doesn't know about is assumed to be done
       elsewhere.  (we put in no-op only for things that we KNOW
       are really no-op.
    */
    pScreen->CreateWindow = apcCreateWindow;
    pScreen->DestroyWindow = apcDestroyWindow;
    pScreen->PositionWindow = apcPositionWindow;
    pScreen->ChangeWindowAttributes = apcChangeWindowAttributes;
    pScreen->RealizeWindow = apcMapWindow;
    pScreen->UnrealizeWindow = apcUnmapWindow;

    pScreen->RealizeFont = gprRealizeFont;
    pScreen->UnrealizeFont = gprUnrealizeFont;
    pScreen->CloseScreen = apcCloseScreen;
    pScreen->QueryBestSize = apcQueryBestSize;
    pScreen->GetImage = apcGetImage;
    pScreen->SourceValidate = (void (*)()) 0;
    pScreen->GetSpans = apcGetSpans;
    pScreen->CreateGC = apcCreateGC;
    pScreen->CreatePixmap = apcCreatePixmap;
    pScreen->DestroyPixmap = apcDestroyPixmap;
    pScreen->ValidateTree = miValidateTree;

    pScreen->RegionCreate = miRegionCreate;
    pScreen->RegionInit = miRegionInit;
    pScreen->RegionCopy = miRegionCopy;
    pScreen->RegionDestroy = miRegionDestroy;
    pScreen->RegionUninit = miRegionUninit;
    pScreen->Intersect = miIntersect;
    pScreen->Inverse = miInverse;
    pScreen->Union = miUnion;
    pScreen->Subtract = miSubtract;
    pScreen->RegionReset = miRegionReset;
    pScreen->TranslateRegion = miTranslateRegion;
    pScreen->RectIn = miRectIn;
    pScreen->PointInRegion = miPointInRegion;
    pScreen->WindowExposures = miWindowExposures;
    pScreen->PaintWindowBackground = apcPaintWindow;
    pScreen->PaintWindowBorder = apcPaintWindow;
    pScreen->CopyWindow = apcCopyWindow;
    pScreen->ClearToBackground = miClearToBackground;

    pScreen->RegionNotEmpty = miRegionNotEmpty;
    pScreen->RegionEmpty = miRegionEmpty;
    pScreen->RegionExtents = miRegionExtents;
    pScreen->RegionAppend = miRegionAppend;
    pScreen->RegionValidate = miRegionValidate;
    pScreen->BitmapToRegion = apcPixmapToRegion;
    pScreen->RectsToRegion = miRectsToRegion;
    pScreen->SendGraphicsExpose = miSendGraphicsExpose;

    pScreen->BlockHandler = NoopDDA;
    pScreen->WakeupHandler = NoopDDA;
    pScreen->blockData = (pointer)NULL;
    pScreen->wakeupData = (pointer)NULL;
    pScreen->devPrivate = NULL;

    /* create depth and visual data for screen */
    pVis = &pClrPrivScreen->visuals[0];
    pVid = &pClrPrivScreen->vids[0];
    pDepth0 = &pClrPrivScreen->depths[0];
    pDepth1 = &pClrPrivScreen->depths[1];

    pVis->vid = FakeClientID (0);
    if (pDisp->depth > 1)
    {
        /* only 1 visual: pseudocolor */
        pVis->class = PseudoColor;
        pVis->bitsPerRGBValue = pDisp->display_char.lut_width_per_primary;
    }
    else
    {
        /* only 1 visual: static gray */
        pVis->class = StaticGray;
        pVis->bitsPerRGBValue = 1;
    }
    pVis->ColormapEntries = 1 << (pDisp->depth);
    pVis->nplanes = pDisp->depth;
    pVis->redMask = pVis->greenMask = pVis->blueMask = 0;
    pVis->offsetRed = pVis->offsetGreen = pVis->offsetBlue = 0;
    *pVid = pVis->vid;

    if (pDisp->depth > 1)
    {
        /* only 2 depths: 1 and full-depth */
        pDepth0->depth = 1;
        pDepth0->numVids = 0;
        pDepth0->vids = NULL;
        pDepth1->depth = pDisp->depth;
        pDepth1->numVids = 1;
        pDepth1->vids = pVid;
        pScreen->numDepths = 2;
    }
    else
    {
        /* only 1 depth: 1  */
        pDepth0->depth = 1;
        pDepth0->numVids = 1;
        pDepth0->vids = pVid;
        pScreen->numDepths = 1;
    }

    pScreen->allowedDepths = pDepth0;
    pScreen->rootDepth = pDisp->depth;
    pScreen->numVisuals = 1;
    pScreen->visuals = pVis;
    pScreen->rootVisual = pVis->vid;

    miInitializeBackingStore (pScreen, &apcBSFuncRec);

#ifdef MITSHM
    ShmRegisterFbFuncs (pScreen);
#endif
    return TRUE;
}
