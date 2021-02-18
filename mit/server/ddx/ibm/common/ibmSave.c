/*
 * Copyright IBM Corporation 1987,1988,1989
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
/***********************************************************
		Copyright IBM Corporation 1988

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

#if defined(IBM_OS_HAS_HFT) && !defined(IBM_OS_HAS_X_QUEUE)
#define HFT_HACKS "Oh !"
#endif

#if defined(HFT_HACKS)

#include <sys/types.h>
#include <sys/time.h>

#define NEED_EVENTS

#include "X.h"
#define NEED_REPLIES
#include "Xproto.h"
#include "misc.h"
#include "scrnintstr.h"
#include "os.h"
#include "regionstr.h"
#include "windowstr.h"
#include "input.h"
#include "resource.h"
#include "colormapst.h"
#include "cursorstr.h"
#include "dixstruct.h"
#include "gc.h"
#include "gcstruct.h"
#include "servermd.h"
#include "miscstruct.h"
#include "pixmapstr.h"
#include "colormap.h"

#include "ibmKeybd.h"
#include "ibmMouse.h"
#include "ibmCmdArgs.h"
#include "ibmScreen.h"

#include "OSio.h"
#include "OScompiler.h"

#else 

#include "X.h"
#include "scrnintstr.h"
#include "cursor.h"
#include "windowstr.h"
#include "window.h"

#endif /* defined(HFT_HACKS) */

#include "ibmTrace.h"

extern WindowPtr *WindowTable ;

/***====================================================================***/

WindowPtr
ObscureEntireScreen( pScreen )
register ScreenPtr pScreen ;
{
register WindowPtr pWin ;
static unsigned mask = CWSaveUnder ;
#if defined(DO_SCREEN_SAVE_UNDERS_ON_HOTKEY)
static XID attributes = 1 ;
#else
static XID attributes = 0 ;
#endif
int result ;

    TRACE( ( "ObscureEntireScreen(0x%X) %d\n", pScreen, attributes ) ) ;

    pWin =
	CreateWindow( FakeClientID( 0 ) ,		/* window id */
		      WindowTable[pScreen->myNum],	/* parent */
		      0, 0,				/* x, y */
		      pScreen->width,			/* width */
		      pScreen->height,			/* height */
		      0,				/* border width */
		      InputOutput,			/* class */
		      mask,
		      &attributes,			/* attributes */
		      0,				/* depth */
		      (ClientPtr) NULL,			/* client */
		      WindowTable[pScreen->myNum]->optional->visual,
							 /* visual */
		      &result ) ;			/* error */
    AddResource( pWin->drawable.id, RT_WINDOW, (pointer) pWin ) ;
    pWin->overrideRedirect = TRUE ;
    /* pWin->cursor = BlankCursor ; Should use special cursor !! */
    MapWindow( pWin, TRUE, FALSE, FALSE, (ClientPtr) NULL ) ;
    return pWin ;
}

#if defined(HFT_HACKS)

/***====================================================================***/

static	WindowPtr obscuringWins[MAXSCREENS];
extern	int	ScreenSaverTime,ScreenSaverInterval;
extern long defaultScreenSaverTime, defaultScreenSaverInterval;
static	int	realSaverTime = -1;
static	int	realSaverInterval = -1;
#ifdef AIXrt
	int	ibmRefreshOnActivate= FALSE;
#else
	int	ibmRefreshOnActivate= TRUE;
#endif

/***====================================================================***/

	/*
	 * 11/20/88 () -- do we want to turn on save unders here if
	 *	RefreshOnActivate is false?
	 */
void
ibmObscureScreen(pScreen)
ScreenPtr	pScreen;
{
    TRACE(("ibmObscureScreen()\n"));
    obscuringWins[pScreen->myNum]= ObscureEntireScreen( pScreen ) ;
    return;
}

/***====================================================================***/

void
ibmExposeScreen(pScreen)
ScreenPtr	pScreen;
{
WindowPtr	pWin= obscuringWins[pScreen->myNum];

    TRACE(("ibmExposeScreen(0x%x)\n",pScreen));
    FreeResource(pWin->drawable.id,RC_VANILLA);
    obscuringWins[pScreen->myNum]= NULL;
    return;
}

/***====================================================================***/

static	PixmapPtr saveMaps[MAXSCREENS];

PixmapPtr
ibmSaveScreenData(pScreen,pSaveFunc)
ScreenPtr	pScreen;
void		(*pSaveFunc)();
{
RegionPtr	pRgn;
PixmapPtr	pPix;

    TRACE(("ibmSaveScreenData(0x%x)\n",pScreen));
    pRgn= &(WindowTable[pScreen->myNum]->winSize);
    if (!saveMaps[pScreen->myNum]) {
	saveMaps[pScreen->myNum]= (*pScreen->CreatePixmap)(pScreen,
						REGION_RECTS(pRgn)->x2,
						REGION_RECTS(pRgn)->y2,
						pScreen->rootDepth);
    }
    pPix= saveMaps[pScreen->myNum];
    (*pSaveFunc)(pPix,pRgn,0,0);
    return(pPix);
}

/***====================================================================***/

void
ibmRestoreScreenData(pScreen,pRestoreFunc)
ScreenPtr	pScreen;
void		(*pRestoreFunc)();
{
RegionPtr	pRgn;

    TRACE(("ibmRestoreScreenData(0x%x)\n",pScreen));
    pRgn= &(WindowTable[pScreen->myNum]->winSize);
    (*pRestoreFunc)(saveMaps[pScreen->myNum],pRgn,0,0);
    return;
}

/***====================================================================***/

void
ibmDeactivateScreens()
{
ScreenPtr 	pScreen;
void 		(*fnp)();
int		scrn;

    TRACE(("ibmDeactivateScreens()\n"));
#ifdef OS_SaveState
    OS_SaveState();
#endif
    /*
     * 11/20/88 (ef) -- XXX!  Temporary.  once we deal with the nuances
     *		of keeping the server alive when screens go away, we won't
     *		have to deal with the screen saver anymore.
     */
    realSaverTime= ScreenSaverTime;
    realSaverInterval= ScreenSaverInterval;
    ScreenSaverTime= ScreenSaverInterval= 0;
    for (scrn = 0; scrn < ibmNumScreens; scrn++) {

	pScreen = ibmScreen(scrn);
	if ((!pScreen)||(ibmScreenState(pScreen->myNum)!=SCREEN_ACTIVE))
	    continue;
	if (!pScreen) {
	    ErrorF("WSGO!!! trying to deactivate null screen\n");
	    ErrorF("to debug: ask Paquin what's going on here\n");
	    return ;
	}

	if (ibmRefreshOnActivate)	ibmObscureScreen(pScreen);

	/* find out what the screen wants to do vis-a-vis saving */
	fnp = ibmScreens[scrn]->ibm_SaveFunc;
	if (fnp) (*fnp)(pScreen,!ibmRefreshOnActivate);
	ibmSetScreenState(pScreen->myNum,SCREEN_INACTIVE);
    }
    /*
     * 11/20/88 (ef) -- XXX!!  Temporary.  We really want to keep the
     *       server alive but obscured.
     */
    DontListenToAnybody();
    return;
}

void
ibmReactivateScreens()
{
ScreenPtr 	pScreen;
void 		(*fnp)(); /* "fnp" means "FunctionPointer" */
int		scrn;
WindowPtr	pRoot;
ColormapPtr	pCmap;


    TRACE(("ibmReactivateScreens()\n"));
#ifdef OS_RestoreState
    OS_RestoreState();
#endif

    /*
     * 11/20/88 (ef) -- XXX! Temporary -- we really want to keep the server
     *		alive.  Once we do, we won't have to disable the screen
     *		saver anymore.
     */
    if ( realSaverTime == -1 )
	{
	realSaverTime = defaultScreenSaverTime;
	}
    if ( realSaverInterval == -1 )
	{
	realSaverInterval = defaultScreenSaverInterval;
	}
    ScreenSaverTime= realSaverTime;
    ScreenSaverInterval= realSaverInterval;;
    PayAttentionToClientsAgain();
    for (scrn = 0; scrn < ibmNumScreens; scrn++) {
	pScreen = ibmScreen(scrn);
	if ((!pScreen)||(ibmScreenState(pScreen->myNum)!=SCREEN_INACTIVE))
	    continue;

	/*
	 * 11/21/88 (ef) -- XXX!  PS/2 screens are setting their own
	 *		screen state, RT screens expect us to do it
	 *		for them.  It's not clear to me if it makes
	 *		all that much of a difference.   Leave it for
	 *		now, but we should figure this out someday.
	 */
#ifdef AIXrt
	ibmSetScreenState(pScreen->myNum,SCREEN_ACTIVE);
#endif

	fnp = ibmScreens[scrn]->ibm_RestoreFunc;
	if (fnp)
	    (*fnp)(pScreen,!ibmRefreshOnActivate);
	if (ibmRefreshOnActivate)	ibmExposeScreen(pScreen);
    }
    return;
}

#endif /* HFT_HACKS */
