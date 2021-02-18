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

#include "apollo.h"
#include "dixfont.h"
#include "pixmapstr.h"
#include "gcstruct.h"

/*
 * Screen routines for the apc driver:
 */
extern Bool apcCreateWindow();
extern Bool apcDestroyWindow();
extern Bool apcPositionWindow();
extern Bool apcChangeWindowAttributes();
extern Bool apcMapWindow();
extern Bool apcUnmapWindow();
extern void apcQueryBestSize();
extern void apcGetImage();
extern void apcGetSpans();
extern Bool apcCreateGC();
extern PixmapPtr apcCreatePixmap();
extern Bool apcDestroyPixmap();
extern void apcPaintWindow();
extern void apcCopyWindow();
extern RegionPtr apcPixmapToRegion();
extern void apcSaveAreas();
extern void apcRestoreAreas();

extern Bool gprRealizeFont();
extern Bool gprUnrealizeFont();

/*
 * GC "op" routines for the apc driver:
 */
extern void apcSolidFS();
extern void apcNaturalFS();
extern void apcUnnaturalFS();
extern void apcSetSpans();
extern void apcPutImage();
extern RegionPtr apcCopyArea();
extern RegionPtr apcCopyPlane();
extern void apcPolyPoint();
extern void apcZeroLine();
extern void apcWideLine();
extern void apcZeroDashLine();
extern void apcZeroSegment();
extern void apcZeroDashSeg();
extern void apcFillPolygon();
extern void apcNaturalPFR();
extern void apcUnnaturalPFR();
extern void apcPushPixelsSolid();
extern void apcPushPixelsStip();
extern void apcPushPixelsTile();

extern int gprImageText8();
extern int gprImageText16();
extern int gprPolyText8();
extern int gprPolyText8Stencil();
extern int gprPolyText16();
extern int gprPolyText16Stencil();
extern int nopText();

/*
 * apc cursor routines:
 */
extern Bool apcDCInitialize();
extern Bool apcSpriteInitialize();

/*
 * apc internal routines:
 */
extern void apcDoubleBlt();
extern void apcRepTileBlt();
extern PixmapPtr apcCreatePixmapFromData();


/* All tiles used by gpr are TILE_SIZE wide and high */
#define TILE_SIZE 32

/* The maximum length of a GPR dash pattern: */
#define MAX_DASH_SIZE (sizeof(gpr_$line_pattern_t)<<3)


/*
 * A record for keeping track of long-lasting buffers for use
 * in the polygon code.
 */
/* initial size for number of points in polygon definition list */
#define INITIAL_N_POINTS 25
/* initial size for number of triangles in a polygon decomposition */
#define INITIAL_N_TRIANGLES 50
typedef struct {
    short               n_points;
    short               n_triangles;
    gpr_$coordinate_t  *x_pts_ptr;
    gpr_$coordinate_t  *y_pts_ptr;
    gpr_$triangle_t    *t_list_ptr;
} apcPolygonRec;

/*
 * All driver-allocated memory that is not associated with a resource
 * (i.e. won't be deleted by Free/Destroy operations) should be collected
 * in APCData so it can be freed at CloseScreen (server reset) time.
 */
typedef struct {
    apcPolygonRec   polygons;
} apcDataRec;

extern apcDataRec APCData;


/*
 * Data structures which store a complete set of current GPR settings
 * as used by this driver.  apcGPRContextRec is the main structure.
 */

/*
 * Store all the parameter values to gpr_$set_draw_pattern in one record.
 * (The parameters must be passed individually, not as this record.)
 */
typedef struct {
    gpr_$line_pattern_t pattern;
    short               repeat_count;
    short               length;
} apcDrawPatternRec;

/*
 * Raster op strategy:  Usually we want the rops on all planes to be equal,
 * for which we use gpr_$set_raster_op_mask with an all-ones mask.  In this
 * case, we set rop_common to that value and ignore the rops array.  When we
 * are first setting an individual per-plane rop, we copy the rop_common value
 * to all elements of rops, then set the individual value and set rop_common
 * to BAD_ROP (out of the legal range of 0 - 15).  This is the signal that the
 * rops are not all identical, and must be set with individual gpr_$set_raster_op
 * calls.
 */
#define BAD_ROP 16
typedef struct {
    gpr_$raster_op_t    rop_common;
    gpr_$raster_op_t    rops[MAX_DEPTH_SUPPORTED];
    unsigned short      depth;
} apcRasterOpRec;

/*
 * The main record.  Arranged so that elements are naturally aligned.
 */
typedef struct {
/* the following are 4 bytes each */
    gpr_$bitmap_desc_t              bitmap;
    gpr_$window_t                   clip_window;
    gpr_$mask_32_t                  plane_mask;
    gpr_$pixel_value_t              draw_value;
    gpr_$pixel_value_t              fill_value;
    gpr_$pixel_value_t              fill_background_value;
    gpr_$pixel_value_t              text_value;
    gpr_$pixel_value_t              text_background_value;
    gpr_$bitmap_desc_t              fill_pattern;
/* this is 8 bytes */
    apcDrawPatternRec               draw_pattern;
/* this is some number of 2-byte elements */
    apcRasterOpRec                  raster_op;
/* the following are 2 bytes each */
    short                           text_font;
    gpr_$triangle_fill_criteria_t   triangle_fill_criteria;
    gpr_$decomp_technique_t         pgon_decomp_technique;
/* the following are 1 byte each */
    boolean                         clipping_active;
    char                            pad;
} apcGPRContextRec;

/*
 * Screen privates for apc driver.
 */
extern int              apcScreenIndex;
typedef struct {
    gpr_$attribute_desc_t   attribute_block;    /* gpr's attribute block */
    GCPtr                   currentGC;          /* pointer to GC whose GPR state is known to be current */
    apcGPRContextRec        curContext;         /* current GPR settings */
} apcPrivScreenRec, *apcPrivScreenPtr;

/*
 * GC privates for apc driver.
 */
extern int              apcGCPrivateIndex;
typedef struct {
    short               fExpose;            /* callexposure handling ? */
    short               freeCompClip;       /* Bool telling whether pCompositeClip points to a region owned by
                                               the apc driver, or not (thus should not be freed) */
    RegionPtr           pCompositeClip;     /* free this based on freeCompClip
                                               flag rather than NULLness */
    gpr_$pixel_value_t  polyTextVal;        /* pixel value for Polytext foreground */
    gpr_$bitmap_desc_t  gprTile;            /* replicated tile for use by GPR */
    unsigned char      *pdataTile;          /* pointer to the bits for above */
    gpr_$bitmap_desc_t  gprOpaqueStipple;   /* replicated opaque stipple for use by GPR */
    unsigned char      *pdataOpaqueStipple; /* pointer to the bits for above */
    apcGPRContextRec    reqContext;         /* GPR settings required */
    } apcPrivGC, *apcPrivGCPtr;

/*
 * Pixmap devPrivate points to one of these:
 */
typedef struct {
    int                    *bitmap_ptr;     /* pointer to the bitmap */
    gpr_$bitmap_desc_t      bitmap_desc;    /* gpr bitmap descriptor */
    int                     width;          /* width of bitmap in words */
    gpr_$offset_t           size;           /* bitmap x & y sizes */
    Bool                    freeData;       /* free data when destroy pixmap? */
} apcPrivPM, *apcPrivPMPtr;

/*
 * Macro which must be invoked at the start of every GC op routine which
 * uses GPR calls to draw with.
 * Updates GPR state and the curContext in the Screen privates to reflect
 * what the GC needs.
 */
#define UPDATE_GPR(pGC, pPrivScreen) \
  { if ((pPrivScreen)->currentGC != (pGC)) \
        apcUpdateGPR ((pGC), (pPrivScreen)); } 

/*
 * Macros which set GPR attributes, keeping the curContext in the
 * Screen privates consistent.
 */
#define SET_BITMAP(arg_bitmap, pPrivScreen) \
  { gpr_$bitmap_desc_t old_bitmap = apSetBitmap((arg_bitmap)); \
    if ((arg_bitmap) != (pPrivScreen)->curContext.bitmap) { \
        (pPrivScreen)->curContext.bitmap = (arg_bitmap); \
        (pPrivScreen)->currentGC = (GCPtr)NULL; }}

#define SET_CLIPPING_ACTIVE(arg_clipping_active, pPrivScreen) \
  { if ((arg_clipping_active) != (pPrivScreen)->curContext.clipping_active) { \
        status_$t   status; \
        (pPrivScreen)->curContext.clipping_active = (arg_clipping_active); \
        (pPrivScreen)->currentGC = (GCPtr)NULL; \
        gpr_$set_clipping_active ((arg_clipping_active), &status); }}

#define SET_CLIP_WINDOW(arg_clip_window, pPrivScreen) \
  { if ((arg_clip_window) != (pPrivScreen)->curContext.clip_window) { \
        status_$t   status; \
        (pPrivScreen)->curContext.clip_window = (arg_clip_window); \
        (pPrivScreen)->currentGC = (GCPtr)NULL; \
        gpr_$set_clip_window ((arg_clip_window), &status); }}

#define SET_PLANE_MASK(arg_plane_mask, pPrivScreen) \
  { if ((arg_plane_mask) != (pPrivScreen)->curContext.plane_mask) { \
        status_$t   status; \
        (pPrivScreen)->curContext.plane_mask = (arg_plane_mask); \
        (pPrivScreen)->currentGC = (GCPtr)NULL; \
        gpr_$set_plane_mask_32 ((gpr_$mask_32_t)(arg_plane_mask), &status); }}

#define SET_RASTER_OP(arg_plane, arg_raster_op, pPrivScreen) \
  { if ((pPrivScreen)->curContext.raster_op.rop_common != BAD_ROP) { \
        int i; \
        for (i=0;i<MAX_DEPTH_SUPPORTED;i++) \
            (pPrivScreen)->curContext.raster_op.rops[i] = (pPrivScreen)->curContext.raster_op.rop_common; \
        (pPrivScreen)->curContext.raster_op.rop_common = BAD_ROP; } \
    if ((arg_raster_op) != (pPrivScreen)->curContext.raster_op.rops[(arg_plane)]) { \
        status_$t   status; \
        (pPrivScreen)->curContext.raster_op.rops[(arg_plane)] = (arg_raster_op); \
        (pPrivScreen)->currentGC = (GCPtr)NULL; \
        gpr_$set_raster_op ((gpr_$rgb_plane_t)(arg_plane), (gpr_$raster_op_t)(arg_raster_op), &status); }}

#define SET_RASTER_OP_COMMON(arg_raster_op, pPrivScreen, drawable_depth) \
  { if ((drawable_depth) > (pPrivScreen)->curContext.raster_op.depth || \
        (arg_raster_op) != (pPrivScreen)->curContext.raster_op.rop_common) { \
        status_$t   status; \
        (pPrivScreen)->curContext.raster_op.rop_common = (arg_raster_op); \
        (pPrivScreen)->curContext.raster_op.depth = (drawable_depth); \
        (pPrivScreen)->currentGC = (GCPtr)NULL; \
        gpr_$set_raster_op_mask ((gpr_$mask_32_t) 0xFFFFFFFF, (gpr_$raster_op_t)(arg_raster_op), &status); }}

#define SET_DRAW_VALUE(arg_draw_value, pPrivScreen) \
  { if ((arg_draw_value) != (pPrivScreen)->curContext.draw_value) { \
        status_$t   status; \
        (pPrivScreen)->curContext.draw_value = (arg_draw_value); \
        (pPrivScreen)->currentGC = (GCPtr)NULL; \
        gpr_$set_draw_value ((arg_draw_value), &status); }}

#define SET_DRAW_PATTERN(arg_draw_pattern, pPrivScreen) \
  { if ((arg_draw_pattern) != (pPrivScreen)->curContext.draw_pattern) { \
        status_$t   status; \
        (pPrivScreen)->curContext.draw_pattern = (arg_draw_pattern); \
        (pPrivScreen)->currentGC = (GCPtr)NULL; \
        gpr_$set_draw_pattern ((arg_draw_pattern).repeat_count, (arg_draw_pattern).pattern, \
                               (arg_draw_pattern).length, &status); }}

#define SET_FILL_VALUE(arg_fill_value, pPrivScreen) \
  { if ((arg_fill_value) != (pPrivScreen)->curContext.fill_value) { \
        status_$t   status; \
        (pPrivScreen)->curContext.fill_value = (arg_fill_value); \
        (pPrivScreen)->currentGC = (GCPtr)NULL; \
        gpr_$set_fill_value ((arg_fill_value), &status); }}

#define SET_FILL_BACKGROUND_VALUE(arg_fill_background_value, pPrivScreen) \
  { if ((arg_fill_background_value) != (pPrivScreen)->curContext.fill_background_value) { \
        status_$t   status; \
        (pPrivScreen)->curContext.fill_background_value = (arg_fill_background_value); \
        (pPrivScreen)->currentGC = (GCPtr)NULL; \
        gpr_$set_fill_background_value ((arg_fill_background_value), &status); }}

#define SET_FILL_PATTERN(arg_fill_pattern, pPrivScreen) \
  { if ((arg_fill_pattern) != (pPrivScreen)->curContext.fill_pattern) { \
        status_$t   status; \
        (pPrivScreen)->curContext.fill_pattern = (arg_fill_pattern); \
        (pPrivScreen)->currentGC = (GCPtr)NULL; \
        gpr_$set_fill_pattern ((arg_fill_pattern), (short) 1, &status); }}

#define SET_TRIANGLE_FILL_CRITERIA(arg_triangle_fill_criteria, pPrivScreen) \
  { if ((arg_triangle_fill_criteria) != (pPrivScreen)->curContext.triangle_fill_criteria) { \
        status_$t   status; \
        (pPrivScreen)->curContext.triangle_fill_criteria = (arg_triangle_fill_criteria); \
        (pPrivScreen)->currentGC = (GCPtr)NULL; \
        gpr_$set_triangle_fill_criteria ((arg_triangle_fill_criteria), &status); }}

#define SET_PGON_DECOMP_TECHNIQUE(arg_pgon_decomp_technique, pPrivScreen) \
  { if ((arg_pgon_decomp_technique) != (pPrivScreen)->curContext.pgon_decomp_technique) { \
        status_$t   status; \
        (pPrivScreen)->curContext.pgon_decomp_technique = (arg_pgon_decomp_technique); \
        (pPrivScreen)->currentGC = (GCPtr)NULL; \
        gpr_$pgon_decomp_technique ((arg_pgon_decomp_technique), &status); }}

#define SET_TEXT_VALUE(arg_text_value, pPrivScreen) \
  { if ((arg_text_value) != (pPrivScreen)->curContext.text_value) { \
        status_$t   status; \
        (pPrivScreen)->curContext.text_value = (arg_text_value); \
        (pPrivScreen)->currentGC = (GCPtr)NULL; \
        gpr_$set_text_value ((arg_text_value), &status); }}

#define SET_TEXT_BACKGROUND_VALUE(arg_text_background_value, pPrivScreen) \
  { if ((arg_text_background_value) != (pPrivScreen)->curContext.text_background_value) { \
        status_$t   status; \
        (pPrivScreen)->curContext.text_background_value = (arg_text_background_value); \
        (pPrivScreen)->currentGC = (GCPtr)NULL; \
        gpr_$set_text_background_value ((arg_text_background_value), &status); }}

#define SET_TEXT_FONT(arg_text_font, pPrivScreen) \
  { if ((arg_text_font) != (pPrivScreen)->curContext.text_font) { \
        status_$t   status; \
        (pPrivScreen)->curContext.text_font = (arg_text_font); \
        (pPrivScreen)->currentGC = (GCPtr)NULL; \
        gpr_$set_text_font ((arg_text_font), &status); }}




/* reduced raster ops */
#define RROP_BLACK	GXclear
#define RROP_WHITE	GXset
#define RROP_NOP	GXnoop
#define RROP_INVERT	GXinvert

/* Binary search to figure out what to do for the raster op.  It may
 * do 5 comparisons, but at least it does no function calls
 * Special cases copy because it's so frequent
 */
#define DoRop(alu, src, dst) \
( ((alu) == GXcopy) ? (src) : \
    (((alu) >= GXnor) ? \
     (((alu) >= GXcopyInverted) ? \
       (((alu) >= GXnand) ? \
         (((alu) == GXnand) ? ~((src) & (dst)) : ~0) : \
         (((alu) == GXcopyInverted) ? ~(src) : (~(src) | (dst)))) : \
       (((alu) >= GXinvert) ? \
	 (((alu) == GXinvert) ? ~(dst) : ((src) | ~(dst))) : \
	 (((alu) == GXnor) ? ~((src) | (dst)) : (~(src) ^ (dst)))) ) : \
     (((alu) >= GXandInverted) ? \
       (((alu) >= GXxor) ? \
	 (((alu) == GXxor) ? ((src) ^ (dst)) : ((src) | (dst))) : \
	 (((alu) == GXnoop) ? (dst) : (~(src) & (dst)))) : \
       (((alu) >= GXandReverse) ? \
	 (((alu) == GXandReverse) ? ((src) & ~(dst)) : (src)) : \
	 (((alu) == GXand) ? ((src) & (dst)) : 0)))  ) )

#define DoRRop(alu, src, dst) \
(((alu) == RROP_BLACK) ? ((dst) & ~(src)) : \
 ((alu) == RROP_WHITE) ? ((dst) | (src)) : \
 ((alu) == RROP_INVERT) ? ((dst) ^ (src)) : \
  (dst))
