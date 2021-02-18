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
/***********************************************************
Copyright 1989 by the Massachusetts Institute of Technology

                     All rights reserved.

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of the Massachusetts
Institute of Technology (M.I.T.) not be used in advertising or publicity
pertaining to distribution of the software without specific, written
prior permission.

M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16IO.c,v 3.1 89/11/04 05:08:49 probe Exp Locker: epeisach $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16IO.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16IO.c,v 3.1 89/11/04 05:08:49 probe Exp Locker: epeisach $";
#endif

#include "X.h"
#include "screenint.h"
#include "scrnintstr.h"
#include "cursorstr.h"
#include "pixmapstr.h"
#include "miscstruct.h"
#include "input.h"
#include "colormapst.h"
#include "resource.h"
#include "mibstore.h"

#include "mfb.h"

#include "OScompiler.h"

#include "ibmColor.h"
#include "ibmScreen.h"
#include "ibmTrace.h"

#include "apa16Hdwr.h"
#include "apa16Decls.h"

#include "OSio.h"
#include "OScursor.h"


extern	void	miRecolorCursor();
extern	void	ibmQueryBestSize();

extern	void	apa16SaveAreas();
extern	void	apa16RestoreAreas();

miBSFuncRec apa16BSFuncRec = {
    mfbSaveAreas,
    mfbRestoreAreas,
    (void (*)()) 0,
    (PixmapPtr (*)()) 0,
    (PixmapPtr (*)()) 0,
};


/***==================================================================***/

Bool
apa16ScreenInit(index, pScreen, argc, argv)
    int index;
    ScreenPtr pScreen;
    int argc;		/* these two may NOT be changed */
    char **argv;
{
    Bool retval;
    static int been_here;

    extern apa16ScreenClose() ;
    extern mfbSaveScreen() ;
    extern mfbRealizeCursor() ;
    extern mfbUnrealizeCursor() ;
    extern mfbDisplayCursor() ;
    extern mfbResolveColorMono() ;
    extern mfbCreateDefColormap() ;
    extern mfbCreateStaticGrayColormap();


    TRACE(("apa16ScreenInit( index= %d, pScreen= 0x%x, argc= %d, argv= 0x%x )\n",index,pScreen,argc,argv));

    if (!been_here) {
	if ( ibmScreenFD(index) < 0)
	{
            ErrorF(  "apa16 file descriptor is invalid\n");
            return FALSE; 
	} 
	been_here= TRUE;
    }

    if (ibmScreenState(index)!=SCREEN_INACTIVE) {
	QUEUE_INIT();
	WHITE_ON_BLACK();
	SET_MERGE_MODE(MERGE_COPY);
    }

    retval = mfbScreenInit(pScreen, APA16_BASE, 
			   APA16_WIDTH, APA16_HEIGHT, 80,80, APA16_WIDTH);
    apa16CursorInit(index);

    /*
     * Re-initialze those items that mfbScreenInit() initializes and
     * have been re-located by miInitializeBackingStore() (which was
     * called from mfbScreenInit.)
     */
    pScreen->CloseScreen=		apa16ScreenClose ;
    pScreen->GetImage=			mfbGetImage;
    pScreen->GetSpans=			mfbGetSpans;
    pScreen->ChangeWindowAttributes=	mfbChangeWindowAttributes;
    pScreen->CreateGC=			mfbCreateGC;
    pScreen->DestroyWindow=		mfbDestroyWindow;

    /*
     * Re-initialize other routines to use IBM routines instead of the
     * mfb routines.
     */
    pScreen->SaveScreen=		ibmSaveScreen;
    pScreen->RealizeCursor=		apa16RealizeCursor;
    pScreen->UnrealizeCursor=		apa16UnrealizeCursor;
    pScreen->DisplayCursor=		apa16DisplayCursor;

    pScreen->SetCursorPosition=		OS_SetCursorPosition;
    pScreen->CursorLimits=		OS_CursorLimits;
    pScreen->PointerNonInterestBox=	OS_PointerNonInterestBox;
    pScreen->ConstrainCursor=		OS_ConstrainCursor;
    pScreen->RecolorCursor=		miRecolorCursor;

    pScreen->RealizeFont=		afRealizeFont;
    pScreen->UnrealizeFont=		afUnrealizeFont;

    pScreen->BlockHandler=		OS_BlockHandler;
    pScreen->WakeupHandler=		OS_WakeupHandler;


#ifdef	VIKING
    pScreen->ResetViking=	0;
#endif	VIKING

    if (ibmUseHardware) {
	pScreen->CreateGC=			apa16CreateGC;
    	pScreen->ChangeWindowAttributes=	apa16ChangeWindowAttributes;
	pScreen->PaintWindowBackground = 	apa16PaintWindow;
	pScreen->PaintWindowBorder=		apa16PaintWindow;
	pScreen->CopyWindow= 			apa16CopyWindow;
	pScreen->GetImage=			apa16GetImage;
	pScreen->GetSpans=			apa16GetSpans;

	apa16BSFuncRec.SaveAreas=			apa16SaveAreas;
	apa16BSFuncRec.RestoreAreas=		apa16RestoreAreas;
    } 

/*** XXX ***/
    /***
     *** The following should be re-done properly with argument parsing.
     ***/
    pScreen->blackPixel = 0;
    pScreen->whitePixel = 1;
/*** XXX ***/

    if (!ibmAllowBackingStore) 
	pScreen->backingStoreSupport =	NotUseful;
    
    miInitializeBackingStore(pScreen, &apa16BSFuncRec);

    mfbCreateDefColormap(pScreen);
    ibmScreen(index)= pScreen;
    return(retval);
}

apa16ScreenClose( index, pScreen )
int		index;
ScreenPtr	pScreen;
{
    TRACE(("apa16ScreenClose( index= 0x%x, pScreen= 0x%x )\n",index,pScreen));
    /* let mfb clean up anything it has to... */
    return mfbCloseScreen(pScreen);
}
