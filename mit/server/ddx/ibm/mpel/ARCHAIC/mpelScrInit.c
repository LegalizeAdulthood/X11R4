/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/***********************************************************
		Copyright IBM Corporation 1987,1988

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelScrInit.c,v 6.2 88/11/01 17:35:50 paul Exp $ */
/* $Source: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelScrInit.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelScrInit.c,v 6.2 88/11/01 17:35:50 paul Exp $";
#endif

#include "X.h"
#include "Xproto.h"
#include "font.h"
#include "servermd.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "windowstr.h"
#include "resource.h"
#include "colormap.h"

#include "mfb.h"
#include "mistruct.h"

#include "OSio.h"

#include "ppc.h"
#include "ppcProcs.h"

#include "mpelProcs.h"
#ifdef AIXEXTENSIONS
extern Bool mpelCreateWindow();
extern Bool mpelDestroyWindow();
extern Bool mpelPositionWindow();
extern Bool mpelMapWindow();
extern Bool mpelUnmapWindow();
#endif

#include "ibmTrace.h"

Colormap mpelDefaultColormap;

static DepthRec depths[2] ;

extern void NoopDDA();

/***==================================================================***/

/*ARGSUSED*/
static void
mpelFreeVisual(p, id)
    register pointer p;
    register int id;
{
    /*$$$XXX are visuals the same (no allocs inside a priv or whatever?*/
    Xfree(p);
    return;
}

/***==================================================================***/

/* Declared in "mpelcurs.c" */
extern int mpelcursorSemaphore ;
extern int mpelCheckCursor() ;
extern void mpelReplaceCursor() ;

/* dts * (inch/dot) * (25.4 mm / inch) = mm */
Bool
mpelInitScreen(index, pScreen, xsize, ysize, dpix, dpiy)
    int index;
    register ScreenPtr pScreen;
    int xsize, ysize;		/* in pixels */
    int dpix, dpiy;		/* dots per inch */
{
    VisualPtr	pVisual;
    unsigned long *pVids;
    register PixmapPtr pPixmap;
    ppcScrnPrivPtr devPriv;

    TRACE(("mpelInitScreen(%d,0x%x,%d,%d,%d,%d)\n",
			index, pScreen, xsize, ysize, dpix, dpiy));

    pScreen->myNum = index;
    pScreen->width = xsize;
    pScreen->height = ysize;
    pScreen->mmWidth = (xsize * 254) / (dpix * 10);
    pScreen->mmHeight = (ysize * 254) / (dpiy * 10);
    pScreen->numDepths = 1;
    depths[0].depth = 8;
    depths[0].numVids = 1;
    depths[0].vids = 0;
    pScreen->allowedDepths = depths;

    pScreen->rootDepth = 8;
    pScreen->rootVisual = FakeClientID(0);

    pScreen->defColormap = (Colormap) FakeClientID(0);
    pScreen->minInstalledCmaps = 0;
    pScreen->maxInstalledCmaps = 1;
/* Eventual BUG Here */
    pScreen->whitePixel = 1;
    pScreen->blackPixel = 0;
    pScreen->backingStoreSupport = Always;
    pScreen->saveUnderSupport = NotUseful;

    /* cursmin and cursmax are device specific */

    pScreen->numVisuals = 1;
    pScreen->visuals = pVisual = (VisualPtr) Xalloc(sizeof (VisualRec));

    pScreen->devPrivate = (pointer)
	( devPriv = (ppcScrnPriv *) Xalloc( sizeof (ppcScrnPriv) ) ) ;
    devPriv->blit = mpelBitBlt;
    devPriv->solidFill = mpelFillSolid;
    devPriv->tileFill = mpelTile;
    devPriv->stipFill = ppcStipple;
    devPriv->opstipFill = ppcOpaqueStipple;
    devPriv->lineBres = mpelLineBres;
    devPriv->imageFill = mpelDrawColorImage;
    devPriv->imageRead = mpelReadColorImage;
    devPriv->lineHorz = mpelLineHorz;
    devPriv->lineVert = mpelLineVert;
    devPriv->setColor = mpelSetColor;
    devPriv->RecolorCursor = NoopDDA;
    devPriv->monoFill = mpelDrawMonoImage;
    devPriv->glyphFill = mpelDrawMonoByteImage;
    devPriv->planeRead = mpelGetPlane;
    devPriv->replicateArea = ppcReplicateArea;
    devPriv->CursorSemaphore = &mpelcursorSemaphore ;
    devPriv->CheckCursor = mpelCheckCursor ;
    devPriv->ReplaceCursor = mpelReplaceCursor ;
    devPriv->DestroyGCPriv = mpelDestroyGCPriv;

    pPixmap = (PixmapPtr) pScreen->devPrivate;

    pPixmap->drawable.type = DRAWABLE_PIXMAP;
    pPixmap->drawable.depth = 8;
    pPixmap->drawable.pScreen = pScreen;
    pPixmap->drawable.serialNumber = 0;
    pPixmap->width = xsize;
    pPixmap->height = ysize;
    pPixmap->refcnt = 1;
    pPixmap->devPrivate = NULL;
    pPixmap->devKind = PixmapBytePad(xsize, 8);

    /* anything that mfb doesn't know about is assumed to be done
       elsewhere.  (we put in no-op only for things that we KNOW
       are really no-op.
    */
#ifdef AIXEXTENSIONS
    pScreen->CreateWindow = mpelCreateWindow;
    pScreen->DestroyWindow = mpelDestroyWindow;
    pScreen->PositionWindow = mpelPositionWindow;
    pScreen->ChangeWindowAttributes = ppcChangeWindowAttributes;
    pScreen->RealizeWindow = mpelMapWindow;
    pScreen->UnrealizeWindow = mpelUnmapWindow;
#else
    pScreen->CreateWindow = ppcCreateWindow;
    pScreen->DestroyWindow = ppcDestroyWindow;
    pScreen->PositionWindow = ppcPositionWindow;
    pScreen->ChangeWindowAttributes = ppcChangeWindowAttributes;
    pScreen->RealizeWindow = mfbMapWindow;
    pScreen->UnrealizeWindow = mfbUnmapWindow;
#endif

    pScreen->RealizeFont = mpelRealizeFont;
    pScreen->UnrealizeFont = mpelUnrealizeFont;
    pScreen->GetImage = ppcGetImage;
    pScreen->GetSpans = (unsigned int *(*)())ppcGetSpans;
    pScreen->CreateGC = mpelCreateGC;
    pScreen->CreatePixmap = ppcCreatePixmap;
    pScreen->DestroyPixmap = mfbDestroyPixmap;
    pScreen->ValidateTree = miValidateTree;

    pScreen->CreateColormap = NoopDDA;
    pScreen->DestroyColormap = NoopDDA;
    pScreen->InstallColormap = mpelInstallColormap;
    pScreen->UninstallColormap = mpelUninstallColormap;
    pScreen->ListInstalledColormaps = ppcListInstalledColormaps;
    pScreen->StoreColors = ppcStoreColors;
    pScreen->ResolveColor = ppcResolveColor;

    pScreen->RegionCreate = miRegionCreate;
    pScreen->RegionCopy = miRegionCopy;
    pScreen->RegionDestroy = miRegionDestroy;
    pScreen->Intersect = miIntersect;
    pScreen->Inverse = miInverse;
    pScreen->Union = miUnion;
    pScreen->Subtract = miSubtract;
    pScreen->RegionReset = miRegionReset;
    pScreen->TranslateRegion = miTranslateRegion;
    pScreen->RectIn = miRectIn;
    pScreen->PointInRegion = miPointInRegion;
    pScreen->WindowExposures = miWindowExposures;
    pScreen->RegionNotEmpty = miRegionNotEmpty;
    pScreen->RegionEmpty = miRegionEmpty;
    pScreen->RegionExtents = miRegionExtents;
    pScreen->SendGraphicsExpose = miSendGraphicsExpose;

    pScreen->BlockHandler = OS_BlockHandler;
    pScreen->WakeupHandler = OS_WakeupHandler;
    pScreen->blockData = (pointer)0;
    pScreen->wakeupData = (pointer)0;

    pVisual->vid = pScreen->rootVisual;
    pVisual->screen = index;
    pVisual->class = PseudoColor;
    pVisual->redMask = 0;
    pVisual->greenMask = 0; /* no use to PseudoColor */
    pVisual->blueMask = 0;
    pVisual->bitsPerRGBValue = 4;
    pVisual->ColormapEntries = 256;
    pVisual->nplanes = 8;

    depths[0].vids = pVids = (unsigned long *) Xalloc(sizeof (unsigned long));
    pVids[0] = pScreen->rootVisual;	/* our one and only visual */
    AddResource(
	pScreen->rootVisual, RT_VISUALID, pVisual, mpelFreeVisual, RC_CORE);

    return 1;
}
