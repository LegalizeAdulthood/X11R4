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
		Copyright IBM Corporation 1987

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

/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedScrInit.c,v 1.5 89/11/29 14:23:18 jfc Exp $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedScrInit.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedScrInit.c,v 1.5 89/11/29 14:23:18 jfc Exp $";
#endif

#include "X.h"
#include "Xproto.h"	/* for xColorItem */
#include "Xmd.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "resource.h"
#include "colormapst.h"
#include "mfb.h"
#include "mistruct.h"
#include "servermd.h"
#include "miscstruct.h"
#include "cursorstr.h"

#include "OScursor.h"

#include "ibmColor.h"
#include "ibmScreen.h"

#include "aedHdwr.h"
#include "aedProcs.h"
#include "ibmTrace.h"

extern mfbFreeVisual();

extern void NoopDDA();

ColormapPtr pColormap;
extern int aedWindowPrivateIndex, aedScreenPrivateIndex;
extern	void	miRecolorCursor();
void aedResolveColor();
extern ColormapPtr pColormap;
extern void ibmQueryBestSize() ;
extern void NoopDDA();
extern void aedSaveAreas(), aedRestoreAreas();

miBSFuncRec aedBSFuncRec = {
    aedSaveAreas,	/* Really dummy rountine */
    aedRestoreAreas,	/* Really dummy routine */
    (void (*)()) 0,
    (PixmapPtr (*)()) 0,
    (PixmapPtr (*)()) 0,
};

Bool
aedInitScreen(index, pScreen)
    int index;
    register ScreenPtr pScreen;
{
    TRACE(("aedInitScreen(index= %d, pScreen= 0x%x)\n", index, pScreen));
    pScreen->myNum = index;

    pScreen->backingStoreSupport =	NotUseful;

    /* cursmin and cursmax are device specific */

    /* anything that mfb doesn't know about is assumed to be done
       elsewhere.  (we put in no-op only for things that we KNOW
       are really no-op.
    */
    pScreen->PositionWindow = aedPositionWindow;
    pScreen->PaintWindowBackground = aedPaintWindow;
    pScreen->PaintWindowBorder = aedPaintWindow;
    pScreen->CopyWindow = aedCopyWindow;
    pScreen->ClearToBackground = miClearToBackground;
    
    pScreen->SaveScreen=	ibmSaveScreen;
    pScreen->RealizeCursor=	aedRealizeCursor;
    pScreen->UnrealizeCursor=	aedUnrealizeCursor;
    pScreen->DisplayCursor=	aedDisplayCursor;

    pScreen->SetCursorPosition=	OS_SetCursorPosition;
    pScreen->CursorLimits=	OS_CursorLimits;
    pScreen->PointerNonInterestBox= OS_PointerNonInterestBox;
    pScreen->ConstrainCursor=	OS_ConstrainCursor;

    pScreen->RecolorCursor=	miRecolorCursor;
    pScreen->QueryBestSize=	ibmQueryBestSize;
    pScreen->CreateGC=		aedCreateGC;
    pScreen->CreateWindow=	aedCreateWindow;
    pScreen->ChangeWindowAttributes=	aedChangeWindowAttributes;
    pScreen->ResolveColor = 	ibmResolveColorMono;
    pScreen->CreateColormap = 	ibmCreateStaticGrayColormap;
    pScreen->DestroyColormap = 	NoopDDA;
    pScreen->InstallColormap = mfbInstallColormap;
    pScreen->UninstallColormap = NoopDDA;
    pScreen->StoreColors = NoopDDA;

    pScreen->RealizeFont = aedRealizeFont;
    pScreen->UnrealizeFont = aedUnrealizeFont;

    pScreen->BlockHandler = NoopDDA;
    pScreen->WakeupHandler = NoopDDA;
    pScreen->blockData = (pointer)0;
    pScreen->wakeupData = (pointer)0;

/* The folowing is done to prevent an infinit loop in the
   initializing of backingstopre. 
*/

    pScreen->CloseScreen = mfbCloseScreen;
    pScreen->GetImage = mfbGetImage;
    pScreen->GetSpans = aedGetSpans;
    pScreen->ChangeWindowAttributes = aedChangeWindowAttributes;
    pScreen->CreateGC = aedCreateGC;
    pScreen->DestroyWindow = mfbDestroyWindow;

    miInitializeBackingStore (pScreen, &aedBSFuncRec);


#ifdef	VIKING
    pScreen->ResetViking = aedResetViking;
#endif	VIKING

    /* we MIGHT return 0 if we had been keeping track of potential
       allocation failures.  one day someone will clean this up.
    */
    return 1;
}

