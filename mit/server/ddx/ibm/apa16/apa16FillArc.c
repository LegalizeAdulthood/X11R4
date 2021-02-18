/************************************************************
Copyright 1989 by The Massachusetts Institute of Technology

Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and without
fee is hereby granted, provided that the above copyright
notice appear in all copies and that both that copyright
notice and this permission notice appear in supporting
documentation, and that the name of MIT not be used in
advertising or publicity pertaining to distribution of the
software without specific prior written permission.
M.I.T. makes no representation about the suitability of
this software for any purpose. It is provided "as is"
without any express or implied warranty.

********************************************************/

/* $XConsortium: mfbfillarc.c,v 5.5 89/11/05 15:15:12 rws Exp $ */

#include "X.h"
#include "Xprotostr.h"
#include "miscstruct.h"
#include "gcstruct.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "mfb.h"
#include "maskbits.h"
#include "mifillarc.h"

#include "ibmTrace.h"
#include "apa16Hdwr.h"

extern void miPolyFillArc();

static void
apa16FillEllipseSolid(pDraw, arc, rop)
    DrawablePtr pDraw;
    xArc *arc;
    int rop;
{
    int iscircle;
    int x, y, e, ex;
    int yk, xk, ym, xm, dx, dy, xorg, yorg;
    int slw;
    miFillArcRec info;
    register int xpos;
    unsigned cmd;
    int ylt, ylb;

    miFillArcSetup(arc, &info);
    MIFILLARCSETUP();
    xorg += pDraw->x;
    yorg += pDraw->y;
    ylt = yorg - y;
    ylb = yorg + y + dy;
    iscircle = (arc->width == arc->height);

    APA16_GET_CMD(ROP_VECTOR,rop,cmd);

    while (y)
    {
	ylt++;
	ylb--;

	if (iscircle)
	{
	    MIFILLCIRCSTEP(slw);
	}
	else
	{
	    MIFILLELLSTEP(slw);
	    if (!slw)
		continue;
	}
	xpos = xorg - x;

	if(slw) {
	    DRAW_VECTOR(cmd,xpos,ylt,xpos+slw,ylt);
	    if(miFillArcLower(slw)) DRAW_VECTOR(cmd,xpos,ylb,xpos+slw,ylb);
	}
    }
}

#define FILLSPAN(xl,xr,yad) \
    if (xr >= xl) \
    { \
	width = xr - xl + 1; \
	DRAW_VECTOR(cmd,xl,yad,xr+1,yad); \
    }

#define FILLSLICESPANS(flip,yad) \
    if (!flip) \
    { \
	FILLSPAN(xl, xr, yad); \
    } \
    else \
    { \
	xc = xorg - x; \
	FILLSPAN(xc, xr, yad); \
	xc += slw - 1; \
	FILLSPAN(xl, xc, yad); \
    }

static void
apa16FillArcSliceSolidCopy(pDraw, pGC, arc, rop)
    DrawablePtr pDraw;
    GCPtr pGC;
    xArc *arc;
    int rop;
{
    int yk, xk, ym, xm, dx, dy, xorg, yorg, slw;
    register int x, y, e, ex;
    miFillArcRec info;
    miArcSliceRec slice;
    int xl, xr, xc;
    int iscircle;
    unsigned cmd;
    int ylt, ylb;

    register int width;
    
    miFillArcSetup(arc, &info);
    miFillArcSliceSetup(arc, &slice, pGC);
    MIFILLARCSETUP();
    iscircle = (arc->width == arc->height);
    xorg += pDraw->x;
    yorg += pDraw->y;
    ylt = yorg - y;
    ylb = yorg + y + dy;
    slice.edge1.x += pDraw->x;
    slice.edge2.x += pDraw->x;

    APA16_GET_CMD(ROP_VECTOR,rop,cmd);

    while (y > 0)
    {
	ylt++;
	ylb--;
	if (iscircle)
	{
	    MIFILLCIRCSTEP(slw);
	}
	else
	{
	    MIFILLELLSTEP(slw);
	}
	MIARCSLICESTEP(slice.edge1);
	MIARCSLICESTEP(slice.edge2);
	if (miFillSliceUpper(slice))
	{
	    MIARCSLICEUPPER(xl, xr, slice, slw);
	    FILLSLICESPANS(slice.flip_top, ylt);
	}
	if (miFillSliceLower(slice))
	{
	    MIARCSLICELOWER(xl, xr, slice, slw);
	    FILLSLICESPANS(slice.flip_bot, ylb);
	}
    }
}

void
apa16PolyFillArcSolid(pDraw, pGC, narcs, parcs)
    DrawablePtr	pDraw;
    GCPtr	pGC;
    int		narcs;
    xArc	*parcs;
{
    mfbPrivGC *priv;
    register xArc *arc;
    register int i;
    BoxRec box;
    RegionPtr cclip;
    int rop;

    priv = (mfbPrivGC *) pGC->devPrivates[mfbGCPrivateIndex].ptr;
    rop = priv->rop;
    if ((rop == RROP_NOP) || !(pGC->planemask & 1))
	return;

    if(pDraw->type != DRAWABLE_WINDOW)
    {
	QUEUE_WAIT();
	mfbPolyFillArcSolid(pDraw, pGC, narcs, parcs);
	return;
    }

    cclip = priv->pCompositeClip;
    for (arc = parcs, i = narcs; --i >= 0; arc++)
    {
	if (miFillArcEmpty(arc))
	    continue;
	if (miCanFillArc(arc))
	{
	    box.x1 = arc->x + pDraw->x;
	    box.y1 = arc->y + pDraw->y;
	    box.x2 = box.x1 + (int)arc->width + 1;
	    box.y2 = box.y1 + (int)arc->height + 1;
	    if ((*pDraw->pScreen->RectIn)(cclip, &box) == rgnIN)
	    {
		if ((arc->angle2 >= FULLCIRCLE) ||
		    (arc->angle2 <= -FULLCIRCLE))
		    apa16FillEllipseSolid(pDraw, arc, rop);
		else
		    apa16FillArcSliceSolidCopy(pDraw, pGC, arc, rop);
		continue;
	    }
	}
	miPolyFillArc(pDraw, pGC, 1, arc);
    }
}
