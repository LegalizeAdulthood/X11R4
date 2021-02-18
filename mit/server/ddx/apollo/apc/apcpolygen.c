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

/*
 * apcFillGPRExactPoly - Driver internal code
 *      Fill a possibly self-intersecting polygon by using GPR render-exact mode.
 */
static void
apcFillGPRExactPoly(pPrivScreen, pRegion, frule, count, ptsIn)
    apcPrivScreenPtr    pPrivScreen;
    RegionPtr           pRegion;
    int                 frule;
    int                 count;
    DDXPointPtr         ptsIn;
{
    status_$t                       status;
    gpr_$triangle_fill_criteria_t   fill_criteria;
    gpr_$coordinate_t               x_first, y_first;
    gpr_$coordinate_t              *xPtrTemp;
    gpr_$coordinate_t              *yPtrTemp;
    BoxPtr                          pClip, pClipDone;
    int                             nClip;
    gpr_$window_t                   window;
    short                           i;

    if (frule == EvenOddRule)
        /* In GPR this translates to use gpr_$parity fill */
        fill_criteria.wind_type = gpr_$parity;
    else
        /* default to WindingNumber */
        fill_criteria.wind_type = gpr_$nonzero;
    fill_criteria.winding_no = 1;

    SET_TRIANGLE_FILL_CRITERIA (fill_criteria, pPrivScreen);
    SET_PGON_DECOMP_TECHNIQUE (gpr_$render_exact, pPrivScreen);

    x_first = (gpr_$coordinate_t)(ptsIn->x);
    y_first = (gpr_$coordinate_t)(ptsIn->y);

    if (count > APCData.polygons.n_points)
    {
        APCData.polygons.x_pts_ptr = (gpr_$coordinate_t *)
                                     (xrealloc (APCData.polygons.x_pts_ptr,
                                                (count*sizeof(gpr_$coordinate_t))));
        if (!APCData.polygons.x_pts_ptr)
        {
            if (APCData.polygons.y_pts_ptr) xfree(APCData.polygons.y_pts_ptr);
            APCData.polygons.n_points = 0;
            return;
        }
        APCData.polygons.y_pts_ptr = (gpr_$coordinate_t *)
                                     (xrealloc (APCData.polygons.y_pts_ptr,
                                                (count*sizeof(gpr_$coordinate_t))));
        if (!APCData.polygons.y_pts_ptr)
        {
            if (APCData.polygons.x_pts_ptr) xfree(APCData.polygons.x_pts_ptr);
            APCData.polygons.n_points = 0;
            return;
        }
        APCData.polygons.n_points = count;
    }

    xPtrTemp = APCData.polygons.x_pts_ptr;
    yPtrTemp = APCData.polygons.y_pts_ptr;
    for (i=0; i<(count-1); i++)
    {
        ptsIn++;
        *xPtrTemp++ = (gpr_$coordinate_t)(ptsIn->x);
        *yPtrTemp++ = (gpr_$coordinate_t)(ptsIn->y);
    }

    nClip = REGION_NUM_RECTS(pRegion);
    if (nClip > 1)
    {
        pClip = REGION_RECTS(pRegion);
        for (pClipDone = pClip + nClip; pClip < pClipDone; pClip++)
        {
            window.window_size.x_size = pClip->x2 -
                                        (window.window_base.x_coord = pClip->x1);
            window.window_size.y_size = pClip->y2 -
                                        (window.window_base.y_coord = pClip->y1);

            SET_CLIP_WINDOW (window, pPrivScreen);

            gpr_$start_pgon (x_first, y_first, &status);
            gpr_$pgon_polyline (APCData.polygons.x_pts_ptr,
                                APCData.polygons.y_pts_ptr,
                                (short)(count-1), &status);
            gpr_$close_fill_pgon (&status);
        }
    }
    else
    {
        gpr_$start_pgon (x_first, y_first, &status);
        gpr_$pgon_polyline (APCData.polygons.x_pts_ptr,
                            APCData.polygons.y_pts_ptr,
                            (short)(count-1), &status);
        gpr_$close_fill_pgon (&status);
    }
}

/*
 * apcFillGPRPoly - Driver internal code
 *      Fill a non-self-intersecting polygon by using GPR triangle decomposition.
 *      Gives wrong results for self-intersecting polygons.
 */
static void
apcFillGPRPoly(pPrivScreen, pRegion, count, ptsIn)
    apcPrivScreenPtr    pPrivScreen;
    RegionPtr           pRegion;
    int                 count;
    DDXPointPtr         ptsIn;
{
    status_$t           status;
    gpr_$coordinate_t   x_first, y_first;
    gpr_$coordinate_t  *xPtrTemp;
    gpr_$coordinate_t  *yPtrTemp;
    BoxPtr              pClip, pClipDone;
    int                 nClip;
    gpr_$window_t       window;
    short               i;
    short               list_size;
    short               n_tris;

    SET_PGON_DECOMP_TECHNIQUE (gpr_$render_exact, pPrivScreen);

    x_first = (gpr_$coordinate_t)(ptsIn->x);
    y_first = (gpr_$coordinate_t)(ptsIn->y);
    gpr_$start_pgon (x_first, y_first, &status);

    if (count > APCData.polygons.n_points)
    {
        APCData.polygons.x_pts_ptr = (gpr_$coordinate_t *)
                                     (xrealloc (APCData.polygons.x_pts_ptr,
                                                (count*sizeof(gpr_$coordinate_t))));
        if (!APCData.polygons.x_pts_ptr)
        {
            if (APCData.polygons.y_pts_ptr) xfree(APCData.polygons.y_pts_ptr);
            APCData.polygons.n_points = 0;
            return;
        }
        APCData.polygons.y_pts_ptr = (gpr_$coordinate_t *)
                                     (xrealloc (APCData.polygons.y_pts_ptr,
                                                (count*sizeof(gpr_$coordinate_t))));
        if (!APCData.polygons.y_pts_ptr)
        {
            if (APCData.polygons.x_pts_ptr) xfree(APCData.polygons.x_pts_ptr);
            APCData.polygons.n_points = 0;
            return;
        }
        APCData.polygons.n_points = count;
    }

    xPtrTemp = APCData.polygons.x_pts_ptr;
    yPtrTemp = APCData.polygons.y_pts_ptr;
    for (i=0; i<(count-1); i++)
    {
        ptsIn++;
        *xPtrTemp++ = (gpr_$coordinate_t)(ptsIn->x);
        *yPtrTemp++ = (gpr_$coordinate_t)(ptsIn->y);
    }

    gpr_$pgon_polyline (APCData.polygons.x_pts_ptr,
                        APCData.polygons.y_pts_ptr,
                        (short)(count-1), &status);

    nClip = REGION_NUM_RECTS(pRegion);
    if (nClip > 1)
    {
        gpr_$close_return_pgon_tri (APCData.polygons.n_triangles,
                                    APCData.polygons.t_list_ptr,
                                    &n_tris, &status);

        /* If the number of triangles is more than the current list size
         * then we need to alloc space and remake the call.  We NEED all
         * the triangles.  No sacrifices at this point.
         */
        if (n_tris > APCData.polygons.n_triangles)
        {
            APCData.polygons.t_list_ptr = (gpr_$triangle_t *)
                                          (xrealloc (APCData.polygons.t_list_ptr,
                                                     (n_tris*sizeof(gpr_$triangle_t))));
            if (!APCData.polygons.t_list_ptr)
            {
                APCData.polygons.n_triangles = 0;
                return;
            }
            APCData.polygons.n_triangles = n_tris;
            gpr_$start_pgon (x_first, y_first, &status);
            gpr_$pgon_polyline (APCData.polygons.x_pts_ptr,
                                APCData.polygons.y_pts_ptr,
                                (short)(count-1), &status);
            gpr_$close_return_pgon_tri (n_tris, APCData.polygons.t_list_ptr,
                                        &list_size, &status);
        }

        pClip = REGION_RECTS(pRegion);
        for (pClipDone = pClip + nClip; pClip < pClipDone; pClip++)
        {
            window.window_size.x_size = pClip->x2 -
                                        (window.window_base.x_coord = pClip->x1);
            window.window_size.y_size = pClip->y2 -
                                        (window.window_base.y_coord = pClip->y1);

            SET_CLIP_WINDOW (window, pPrivScreen);

            gpr_$multitriangle (APCData.polygons.t_list_ptr, n_tris, &status);
        }
    }
    else
        gpr_$close_fill_pgon (&status);
}
/*
 * apcFillSpokedPoly - Driver internal code
 *      Fill a convex polygon by spoking and filling triangles.
 *      (Actually works for all "pseudo-convex" polygons, which are those for which
 *      all the vertices are "visible" from the first vertex.  I.e., those for
 *      which the "spokes" you draw from the first vertex do not cross the polygon's
 *      boundary.)
 */
static void
apcFillSpokedPoly(pPrivScreen, pRegion, count, pPts)
    apcPrivScreenPtr    pPrivScreen;
    RegionPtr           pRegion;
    int                 count;
    DDXPointPtr         pPts;
{
    gpr_$window_t       window;
    gpr_$coordinate_t   x_anchor, y_anchor;
    gpr_$triangle_t    *Triangles;
    BoxPtr              pClip, pClipDone;
    int                 nClip;
    int                 i;
    status_$t           status;

    if ((count-2) > APCData.polygons.n_triangles )
    {
        APCData.polygons.t_list_ptr = (gpr_$triangle_t *)
                                      (xrealloc(APCData.polygons.t_list_ptr,
                                                ((count-2)*sizeof(gpr_$triangle_t))));
        if (!APCData.polygons.t_list_ptr)
        {
            APCData.polygons.n_triangles = 0;
            return;
        }
        APCData.polygons.n_triangles = count-2;
    }

    Triangles = APCData.polygons.t_list_ptr;
    x_anchor = (gpr_$coordinate_t)(pPts->x);
    y_anchor = (gpr_$coordinate_t)(pPts++->y);
    for (i=0; i<count-2; i++, Triangles++)
    {
        Triangles->winding_no = 1;
        Triangles->p1.x_coord = x_anchor;
        Triangles->p1.y_coord = y_anchor;
        Triangles->p2.x_coord = (gpr_$coordinate_t)(pPts->x);
        Triangles->p2.y_coord = (gpr_$coordinate_t)(pPts++->y);
        Triangles->p3.x_coord = (gpr_$coordinate_t)(pPts->x);
        Triangles->p3.y_coord = (gpr_$coordinate_t)(pPts->y);
    }

    nClip = REGION_NUM_RECTS(pRegion);
    if (nClip > 1)
    {
        pClip = REGION_RECTS(pRegion);
        for (pClipDone = pClip + nClip; pClip < pClipDone; pClip++)
        {
            window.window_size.x_size = pClip->x2 -
                                        (window.window_base.x_coord = pClip->x1);
            window.window_size.y_size = pClip->y2 -
                                        (window.window_base.y_coord = pClip->y1);

            SET_CLIP_WINDOW (window, pPrivScreen);
            gpr_$multitriangle (APCData.polygons.t_list_ptr, (short)(count-2), &status);
            }
        }
    else
        gpr_$multitriangle (APCData.polygons.t_list_ptr, (short)(count-2), &status);
}

/*
 * apcFillPolygon - DDX interface (GC "op" FillPolygon)
 *      Choose a method for filling the polygon based on its nature.
 */
void
apcFillPolygon(pDraw, pGC, shape, mode, count, pPts)
    DrawablePtr pDraw;
    GCPtr       pGC;
    int         shape, mode;
    int         count;
    DDXPointPtr pPts;
{
    apcPrivScreenPtr    pPrivScreen;
    RegionPtr           pRegion;
    DDXPointPtr         ppt, pptbeyond;
    int                 i;
    int                 xorg, yorg;
    int                 xmin, xmax, ymin, ymax, size;
    DDXPointRec         last_transpt, this_transpt, first_transpt;
    int                 x_first, y_first;
    int                 cur_area, first_area, areasign;
    int                 flag_180;

    if (count < 3)
        return;

    pRegion = ((apcPrivGCPtr)(pGC->devPrivates[apcGCPrivateIndex].ptr))->pCompositeClip;
    if (REGION_NIL(pRegion))
        return;

    pPrivScreen = (apcPrivScreenPtr) pGC->pScreen->devPrivates[apcScreenIndex].ptr;
    UPDATE_GPR (pGC, pPrivScreen);

    xorg = pDraw->x;
    yorg = pDraw->y;

    ppt = pPts;
    xmin = xmax = (ppt->x += xorg);
    ymin = ymax = (ppt->y += yorg);
    ppt++;
    if (mode == CoordModeOrigin)
        for (i = 1; i<count; i++, ppt++)
        {
            ppt->x += xorg;
            if (xmin > ppt->x) xmin = ppt->x;
            if (xmax < ppt->x) xmax = ppt->x;
            ppt->y += yorg;
            if (ymin > ppt->y) ymin = ppt->y;
            if (ymax < ppt->y) ymax = ppt->y;
        }
    else
        for (i = 1; i<count; i++, ppt++)
        {
            ppt->x += (ppt-1)->x;
            if (xmin > ppt->x) xmin = ppt->x;
            if (xmax < ppt->x) xmax = ppt->x;
            ppt->y += (ppt-1)->y;
            if (ymin > ppt->y) ymin = ppt->y;
            if (ymax < ppt->y) ymax = ppt->y;
        }

    size = (xmax - xmin) + (ymax - ymin);

    /*
     * Here we use various heuristics based on the polygon's size, number of
     * vertices, and given shape parameter to try to pick the fastest way to
     * draw it, either by spoking (apcFillSpokedPoly), by "normal" GPR
     * triangle decomposition (apcFillGPRPoly), or by "exact" GPR rasterization
     * (apcFillGPRExactPoly).  We can't waste too much time doing this.  On the
     * other hand, often the client says the polygon is "Complex" when in fact
     * it does not cross itself or may even be convex, just because it doesn't
     * really know.  So it's worth it to do a little work to find out something
     * about what the polygon is really like.
     *
     * If the polygon crosses itself, only the exact method gives correct results.
     * And if it's not convex, it may still be "pseudo-convex", meaning that
     * starting at the first vertex, you can still correctly draw it by spoking
     * it out into triangles from there.
     *
     * Finally, sometimes we decide to use a more general method than is seemingly
     * warranted.  (We can never use a LESS general method, that would give wrong
     * results.)  This is based on these performance considerations:
     *
     * 1.  Spoking out large polygons with many vertices wastes so much time
     *     calculating and rasterizing to all the internal slanted edges, that it
     *     would have been faster to let GPR decompose it.
     * 2.  For small polygons with many vertices, the exact method is faster than GPR
     *     decomposition because the cost of decomposition is fixed regardless of size
     *     (and fairly high), but the exact method calculates fewer intercept
     *     coordinates with small polygons.
     *
     * Some performance studies were done on a DN4000 with a Meerkat I (i.e. standard
     * 1024x800x8 bit) display to find the crossover points embodied in the code below.
     * These are likely to be approximately valid for all Otter-type graphics systems.
     * That is, as long as GPR performance for particular cases doesn't change.
     *
     * WARNING!!!  these choices are likely to be invalid for systems with graphics
     * processors such as the DN570/580/590 systems, Meekat III's, etc.
     * I'm just hoping that any resulting deoptimality is not too serious.
     */

    if (count == 3)
    {
        apcFillSpokedPoly (pPrivScreen, pRegion, count, pPts);
        /* that was easy */
        return;
    }

    if (shape != Convex)
    {
        /* Try to find out if it's really pseudo-convex.
         *
         * Do this by translating the polygon so that its first point is at the origin.
         * Then, start "sweeping" the polygon by considering the successive triangles
         * formed by the first point (now the origin) and each adjacent pair of vertices
         * following (say Pa, Pb).
         *
         * As long as the "signed area" of each triangle does not differ in sign from
         * all previous ones (ignoring those with zero area), we are OK.  "Signed area"
         * is the crossproduct of the vectors Pa and Pb.  In other words, the perimeter
         * of the polygon has to be moving consistently either clockwise or counterclockwise
         * w.r.t. the origin.  If it ever doubles back the other way, we don't have
         * pseudo-convexity; this will show up as a change of sign in the crossproduct.
         *
         * Fly in the ointment:  we could have a spiral-shaped polygon that crossed itself
         * without the perimeter ever changing direction.  This happens if the total sweep
         * of the perimeter exceeds 360 degrees.  We need to do extra crossproducts to test for
         * this, so we only test if the polygon is declared Complex (if it's really just
         * Nonconvex the problem can't happen), thus there are two copies of the loop below.
         * The test is to see if the signed area of the triangle formed by the first different
         * point after the initial point and the current point ever changes to the
         * opposite sign from the signed area of the individual perimeter triangles and then
         * back to the same sign.  When the sign changes to opposite, the sweep has exceeded
         * 180 degrees; if it reverts to the same sign, we have exceeded 360 degrees and
         * have a self-crossing polygon for which the spoking would give wrong results.
         */

        areasign = 0;
        x_first = pPts->x;
        y_first = pPts->y;
        ppt = pPts + 1;
        pptbeyond = pPts + count;
        this_transpt.x = ppt->x - x_first;
        this_transpt.y = ppt->y - y_first;
        if (shape == Nonconvex)
        {
            while ((++ppt) != pptbeyond)
            {
                last_transpt = this_transpt;
                this_transpt.x = ppt->x - x_first;
                this_transpt.y = ppt->y - y_first;
                cur_area = (last_transpt.x * this_transpt.y) -
                           (this_transpt.x * last_transpt.y);
                if (areasign)
                {   /* have a definite sign that must be matched */
                    if (cur_area)
                    {   /* but only if cur_area is nonzero */
                        if ((areasign^cur_area) < 0)    /* signs differ (yucky but fast) */
                            break;                      /* so give up */
                    }
                }
                else
                    /* no definite sign, not committed yet */
                    areasign = cur_area;
            }
        }
        else
        {
            /* shape is Complex, must test for > 360 degree sweep */
            while ((++ppt) != pptbeyond)
            {
                last_transpt = this_transpt;
                this_transpt.x = ppt->x - x_first;
                this_transpt.y = ppt->y - y_first;
                cur_area = (last_transpt.x * this_transpt.y) -
                           (this_transpt.x * last_transpt.y);
                if (areasign)
                {   /* have a definite sign that must be matched */
                    if (cur_area)
                    {
                        /* but only if cur_area is nonzero */
                        if ((areasign^cur_area) < 0)    /* signs differ (yucky but fast) */
                            break;                      /* so give up */
                        first_area = (first_transpt.x * this_transpt.y) -
                                     (this_transpt.x * first_transpt.y);
                        if (flag_180)
                        {   /* already passed 180 degrees */
                            if ((first_area^cur_area) >= 0)
                                break;      /* went past 360 degrees, that's too far */
                        }
                        else
                            /* not yet exceeded 180 degrees */
                            flag_180 = ((first_area^cur_area) < 0); /* set flag if we have now */
                    }
                }
                else
                    /* no definite sign, not committed yet */
                    if (cur_area)
                    {   /* found a sign, now start looking for > 180 degrees */
                        flag_180 = 0;
                        first_transpt = last_transpt;
                        areasign = cur_area;
                    }
            }
        }

        if (ppt == pptbeyond)
            /* established pseudo-convexity */
            shape = Convex;
    }

    switch (shape)
    {
    case Convex:
        /* if spoking is not too expensive, do it; else use GPR */
        if ( (count <  8) ||
            ((count < 11) && (size < 400)) ||
            ((count < 15) && (size < 200)) ||
            ((count < 50) && (size < 150)) )
        {
            apcFillSpokedPoly (pPrivScreen, pRegion, count, pPts);
            break;
        } /* else fall thru */
    case Nonconvex:
        /* if regular GPR is not too expensive, do it; else use exact GPR */
        if ( (count < 11) ||
            ((count < 15) && (size >  50)) ||
            ((count < 50) && (size > 100)) ||
             (size > 200) )
        {
            apcFillGPRPoly (pPrivScreen, pRegion, count, pPts);
            break;
        } /* else fall thru */
    case Complex:
        apcFillGPRExactPoly (pPrivScreen, pRegion, pGC->fillRule, count, pPts);
        break;
    }
}
