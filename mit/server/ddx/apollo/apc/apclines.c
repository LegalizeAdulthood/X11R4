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

/*
 * Derived from milines.c.  Data types changed from SppPointRec to
 * DDXPointRec, since this code is only used when lines are horizontal
 * or vertical, thus integer coords suffice.
 *
 * No actual Apollo dependencies in this code!
 */

#include "X.h"
#include "windowstr.h"
#include "Xprotostr.h"
#include "gcstruct.h"
#include "scrnintstr.h"
#include "miscstruct.h"
#include "pixmapstr.h"
#include "mifpoly.h"
#include "mi.h"

#define RECT_BUF_SIZE   512

#define ROUNDUP(a, b)  ( a/b + (((a/b)*b) != (a)) )

#define GCVALSMASK      (GCForeground|GCBackground)
static int gcvals[] = {1, 0};

/*
 * ExtendSegment -- Driver internal code
 *      Calcuate point p3 made by extending the segment (away from p2)
 *      by a length d/2.
 *      Assumes segment is horizontal or vertical.
 */
static DDXPointRec
ExtendSegment(p1, p2, d)
    DDXPointRec p1, p2;
    int         d;
{
    DDXPointRec p3;

    if (p1 == p2)
    {
        ErrorF("ExtendSegment called with coincident points.\n");
        p3 = p2;
        return p3;
    }

    if (p1.x == p2.x)
    {
        p3.x = p2.x;
        if (p1.y > p2.y)
            p3.y = p2.y - d/2;
        else
            p3.y = p2.y + d/2;
    }
    else
    {
        p3.y = p2.y;
        if (p1.x > p2.x)
            p3.x = p2.x - d/2;
        else
            p3.x = p2.x + d/2;
    }

    return p3;
}

/*
 * ShortenSegment -- Driver internal code
 *      Calcuate point p3 made by shortening the segment (away from p2)
 *      by a length d.
 *      Assumes segment is horizontal or vertical.
 */
static DDXPointRec
ShortenSegment(p1, p2, d)
    DDXPointRec p1, p2;
    int         d;
{
    DDXPointRec p3;

    if (p1 == p2)
    {
        ErrorF("ShortenSegment called with coincident points.\n");
        p3 = p2;
        return p3;
    }

    if (p1.x == p2.x)
    {
        p3.x = p2.x;
        if (p1.y > p2.y)
            p3.y = p2.y + d;
        else
            p3.y = p2.y - d;
    }
    else
    {
        p3.y = p2.y;
        if (p1.x > p2.x)
            p3.x = p2.x + d;
        else
            p3.x = p2.x - d;
    }

    return p3;
}

/*
 * GetCrnrPt -- Driver internal code
 *      Assumes lines are horizontal or vertical.  Used to fill out corners
 *      of joined line segments.
 *      Modify p2 in the line segment (p1,p2) by an amount depending on the
 *      direction of the line segment and the width of the line.  Horizontal
 *      and vertical lines of width>1 going to the right or down get extended
 *      by (ROUNDUP(width/2) -1), while ones going to the left or up get
 *      extended by (width/2).  Modified p2 is returned in p3.
 */
static DDXPointRec
GetCrnrPt(p1In, p2In, width)
    DDXPointRec p1In, p2In;
    int         width;
{

    DDXPointRec p3;

    p3 = p2In;              /* unless modified, return pt. will be p2In */

    if (p1In != p2In)
    {   /* line is not 1 point */
        if (p1In.x != p2In.x)
        {   /* horizontal line */
            if (p1In.x < p2In.x)
                /* going to the right */
                p3.x = (width/2) ? (p2In.x + ROUNDUP(width, 2) - 1) : p2In.x;
            else
                /* going to the left */
                p3.x -= width/2;
        }
        else
        {   /* vertical line */
            if (p1In.y < p2In.y)
                /* going down */
                p3.y = (width/2) ? (p2In.y + ROUNDUP(width, 2) - 1) : p2In.y;
            else
                /* going up */
                p3.y -= width/2;
        }
    }
    return p3;
}


/*
 * GetRect -- Driver internal code
 *      Get the corners of the rectangle formed for a line with the endpoints
 *      p1In, p2In and the given width.  Stuff the coordinates into rect.
 *      Assumes line is horizontal or vertical.
 */
static void
GetRect(p1In, p2In, rect, width, xOrg, yOrg)
    DDXPointRec p1In, p2In;
    xRectangle *rect;
    int         width, xOrg, yOrg;
{
    if (p1In == p2In)
    {   /* Bizarre answer, but it's what the spec calls for */
        rect->x = (short)(p1In.x - width/2);
        rect->y = p1In.y;
        rect->width = width;
        rect->height = 1;
    }
    else
    {
        if (p1In.x != p2In.x)
        {
            if (p1In.x < p2In.x)
            {
                rect->x = p1In.x;
                rect->y = (short)(p1In.y - width/2);
                rect->width = (short)(p2In.x - p1In.x) + 1;
                rect->height = width;
            }
            else
            {
                rect->width = (short)(p1In.x - p2In.x) + 1;
                rect->x = p1In.x - rect->width + 1;
                rect->y = (short)(p2In.y - width/2);
                rect->height = width;
            }
        }
        else
        {
            if (p1In.y < p2In.y)
            {
                rect->x = (short)(p1In.x - width/2);
                rect->y = p1In.y;
                rect->width = width;
                rect->height = (short)(p2In.y - p1In.y) + 1;
            }
            else
            {
                rect->height = (short)(p1In.y - p2In.y) + 1;
                rect->x = (short)(p2In.x - width/2);
                rect->y = p1In.y - rect->height + 1;
                rect->width = width;
            }
        }
    }
    rect->x -= xOrg;
    rect->y -= yOrg;
}

/*
 * OneSegWide -- Driver internal code
 *      Draw wide line which only has 1 segment.  This is so easy that it's worth
 *      special casing. Regardless of the raster op, this can't self-intersect.
 *      Also handles fluke cases where we're called with only 1 point
 */
static void
OneSegWide(pDraw, pGC, npt, pPts)
    DrawablePtr pDraw;
    GCPtr       pGC;
    int         npt;
    DDXPointPtr pPts;
{
    xRectangle  rect;
    DDXPointRec tmpPts[2];
    int         width = (pGC->lineWidth ? pGC->lineWidth : 1);

    tmpPts[0] = pPts[0];
    tmpPts[1] = pPts[1];
    if (npt == 1)
    {
        tmpPts[1] = pPts[0];
        pPts = tmpPts;
    }
    if (pGC->capStyle == CapProjecting)
    {
        if (PtEqual(pPts[0], pPts[1]))
        {
            tmpPts[0].y -= width/2;
            tmpPts[1].y += width/2;
        }
        else
        {
            tmpPts[1] = ExtendSegment (pPts[0], pPts[1], width);
            tmpPts[0] = ExtendSegment (pPts[1], pPts[0], width);
        }
    }
    else
    {
        if (width == 1)
            if (pGC->capStyle == CapNotLast)
                tmpPts[1] = ShortenSegment (pPts[0], pPts[1], 1);
    }

    GetRect (tmpPts[0], tmpPts[1], &rect, width, 0, 0);

    (* pGC->ops->PolyFillRect) (pDraw, pGC, (int) 1, &rect);
}

/*
 * apcWideLine -- DDX interface (GC "op" Polylines)
 *      Test the Polyline for any diagonal lines.  If so, just punt
 *      by calling miWideLine; else we can do it much faster via
 *      filling rectangles.
 */
void
apcWideLine(pDraw, pGC, mode, npt, pPts)
    DrawablePtr pDraw;
    GCPtr       pGC;
    int         mode;
    int         npt;
    DDXPointPtr pPts;
{
    int         xOrg, yOrg, i, fTricky, rect_idx;
    int         capStyle = pGC->capStyle;
    DDXPointRec mod_pt, first_pt;
    DDXPointPtr ppt;
    xRectangle  rect_buf[RECT_BUF_SIZE];
    int         width = (pGC->lineWidth ? pGC->lineWidth : 1);
    int         xMin, xMax, yMin, yMax, dx, dy;
    DrawablePtr pDrawTo;
    GCPtr       pGCTo;

    /* Check GC capStyle and joinStyle,
     * If capstyle is CapRound, use miWideLine
     * If joinstyle is not JoinMiter, use miWideLine
     */
    if ((pGC->capStyle == CapRound) ||
        (pGC->joinStyle != JoinMiter))
    {
        miWideLine (pDraw, pGC, mode, npt, pPts);
        return;
    }

    /* Loop through all line segments - if any are not horizontal or
     * vertical, use miWideLine for whole set of points
     */
    for (i=0, ppt = pPts ; i < npt-1 ; i++)
    {
        if (mode == CoordModeOrigin)
        {
            if ((ppt[i].x == ppt[i+1].x) || (ppt[i].y == ppt[i+1].y))
                continue;
            else
            {
                miWideLine (pDraw, pGC, mode, npt, pPts);
                return;
            }
        }
        else
        {   /* CoordModePrevious */
            if ((ppt[i+1].x == 0) || (ppt[i+1].y == 0))
                continue;
            else
            {
                miWideLine (pDraw, pGC, mode, npt, pPts);
                return;
            }
        }
    }

    ppt = pPts;

    if (mode == CoordModePrevious)
        for (ppt++,i = 1; i<npt; i++)
        {
            ppt->x += (ppt-1)->x;
            ppt->y += (ppt-1)->y;
            ppt++;
        }

    /*
     * special case length 1 polyline
     */
    if (npt <= 2)
    {
        OneSegWide (pDraw, pGC, npt, pPts);
        return;
    }

    /* Determine whether to use pixmap or to go directly to screen, */
    /* based on GC->alu - allocate pixmap, scratch GC if needed */
    switch (pGC->alu)
    {
    case GXclear:           /* 0 */
    case GXcopy:            /* src */
    case GXcopyInverted:    /* NOT src */
    case GXset:             /* 1 */
        fTricky = FALSE;
        xOrg = yOrg = 0;
        pDrawTo = pDraw;
        pGCTo = pGC;
        break;
    case GXand:             /* src AND dst */
    case GXandReverse:      /* src AND NOT dst */
    case GXandInverted:     /* NOT src AND dst */
    case GXnoop:            /* dst */
    case GXxor:             /* src XOR dst */
    case GXor :             /* src OR dst */
    case GXnor:             /* NOT src AND NOT dst */
    case GXequiv:           /* NOT src XOR dst */
    case GXinvert:          /* NOT dst */
    case GXorReverse:       /* src OR NOT dst */
    case GXorInverted:      /* NOT src OR dst */
    case GXnand:            /* NOT src OR NOT dst */
        fTricky = TRUE;
        yMin = yMax = pPts[0].y;
        xMin = xMax = pPts[0].x;

        for (i = 1; i < npt; i++)
        {
            xMin = min(xMin, pPts[i].x);
            xMax = max(xMax, pPts[i].x);
            yMin = min(yMin, pPts[i].y);
            yMax = max(yMax, pPts[i].y);
        }
        xOrg = xMin - D2SECANT * width;
        yOrg = yMin - D2SECANT * width;
        dx = xMax - xMin + D2SECANT * width * 2 + 1;
        dy = yMax - yMin + D2SECANT * width * 2 + 1;
        pDrawTo = (DrawablePtr) (*pDraw->pScreen->CreatePixmap)
                                    (pDraw->pScreen, dx, dy, 1);
        if (!pDrawTo)
            return;
        pGCTo = GetScratchGC (1, pDraw->pScreen);
        if (!pGCTo)
        {
            (*pDraw->pScreen->DestroyPixmap) (pDrawTo);
            return;
        }
        DoChangeGC (pGCTo, GCVALSMASK, gcvals, 0);
        ValidateGC (pDrawTo, pGCTo);
        miClearDrawable (pDrawTo, pGCTo);
    }

    rect_idx = 0;

    /*  Extend beginning of 1st line segment if capStyle is CapProjecting
     */
    first_pt = pPts[0];
    if (capStyle == CapProjecting)
        first_pt = ExtendSegment (pPts[1], pPts[0], width);

    /* Modify 2nd point of 1st line segment to extend segment to cover corner
     */
    mod_pt = GetCrnrPt (pPts[0], pPts[1], width);

    /*
     * build a rectangle out of the first edge;
     * buffer the rectangle coordinates
     */
    GetRect (first_pt, mod_pt, &rect_buf[rect_idx], width, xOrg, yOrg);
    rect_idx++;

    for (i = 1 ; i < (npt - 1) ; i++)
    {
        /* Modify 2nd point to extend segment to cover corner
         */
        mod_pt = GetCrnrPt (pPts[i], pPts[i+1], width);

        /*
         * find the next edge
         * build a rectangle out of the next edge
         * buffer the rectangle coordinates
         * bump to next
         */
        GetRect (pPts[i], mod_pt, &rect_buf[rect_idx], width, xOrg, yOrg);

        if (rect_idx == RECT_BUF_SIZE)
        {
            (* pGCTo->ops->PolyFillRect) (pDrawTo, pGCTo, rect_idx+1, rect_buf);
            rect_idx = 0;
        }
        else
            rect_idx++;
    }

    /*
     *  If first and last points are different, last seg may need
     *  to be modified.
     */
    mod_pt = pPts[npt-1];
    if (!PtEqual(pPts[0], pPts[npt-1]))
    {
        if (capStyle == CapProjecting)
            mod_pt = ExtendSegment (pPts[npt-2], pPts[npt-1], width);
        else
            if (width == 1)
            {
                if (capStyle == CapNotLast)
                    mod_pt = ShortenSegment (pPts[npt-2], pPts[npt-1], 1);
            }
    }
    else
        /*  If first and last points are not different, need to cover corner
         */
        mod_pt = GetCrnrPt (pPts[npt-2], pPts[npt-1], width);

    /* Build a rectangle from the last edge coordinates */
    GetRect (pPts[npt-2], mod_pt, &rect_buf[rect_idx], width, xOrg, yOrg);

    (* pGCTo->ops->PolyFillRect) (pDrawTo, pGCTo, rect_idx+1, rect_buf);

    /* Push Pixels if using pixmap */
    if (fTricky)
    {
        xOrg += pDraw->x;
        yOrg += pDraw->y;

        if ((pGC->serialNumber) != (pDraw->serialNumber))
            ValidateGC(pDraw, pGC);

        (*pGC->ops->PushPixels) (pGC, pDrawTo, pDraw, dx, dy, xOrg, yOrg);
        (*pDraw->pScreen->DestroyPixmap) (pDrawTo);
        FreeScratchGC (pGCTo);
    }
}
