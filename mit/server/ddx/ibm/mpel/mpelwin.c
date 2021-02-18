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
#ifdef AIXEXTENSIONS
#include "X.h"
#include "Xmd.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "mfb.h"
#include "mistruct.h"
#include "regionstr.h"

#include "OScompiler.h"
#include "ibmTrace.h"


extern void ppcCopyWindow();

#include "AIXext.h"

void mpelWIProc()
{
};

Bool
mpelValidateWindow()
{
}

Bool
mpelDestroyWindowHeader(pWin,pWI)
WindowPtr pWin;
WInterestPtr pWI;
{
	if (pWI)
		xfree(pWI) ;
}

Bool mpelChangeExtWindowAttributes(pWin,mask,pPrivate)
register WindowPtr pWin;
register int mask;
register pointer pPrivate ; 
{
   aixPrivWin *pPrivWin;
   WInterestPtr pWI , pWInit , pWInext ;
   pPrivWin = (aixPrivWin *)(pWin->devPrivate) ;

   /* here we Map WindowInterest Stuff */
   /* replace the following codes by macro ??? */

    pWI = pPrivWin->pNextWInterest;
    pWInit = (WInterestPtr) &pPrivWin->pNextWInterest;

    do
    {
        pWInext = pWI->pNextWInterest;
	if(pWI->ChangeExtWindowAttributes)
	    (*pWI->ChangeExtWindowAttributes) (pWin,mask,pPrivate);
	pWI = pWInext;
    }
    while(pWI != pWInit);

}


extern WindowRec WindowTable[];

Bool mpelCreateWindow(pWin)
register WindowPtr pWin;
{
    register aixPrivWin *pPrivWin;

    WInterestPtr pWI;

    TRACE(("mpelCreateWindow( pWin= 0x%x )\n",pWin));

    pWin->ClearToBackground = miClearToBackground;
    pWin->PaintWindowBackground = mfbPaintWindowNone;
    pWin->PaintWindowBorder = mfbPaintWindowPR;
    pWin->CopyWindow = ppcCopyWindow;

    if(!(pPrivWin = (aixPrivWin *)Xalloc(sizeof(aixPrivWin))))
	 return (FALSE);

    pWin->devPrivate = (pointer)pPrivWin;
    pPrivWin->pRotatedBorder = NullPixmap;
    pPrivWin->pRotatedBackground = NullPixmap;
    pPrivWin->fastBackground = 0;
    pPrivWin->fastBorder = 0;


    /* here we initialize WindowInterest Stuff */
    /* create the head of WindowInterest */

    if (! (pWI = (WInterestPtr) xalloc(sizeof(WInterestRec))) ) {
	xfree(pPrivWin);
	return FALSE;
    }
     
    /*
     * Now link this into the WIqueue
     */

    pPrivWin->pNextWInterest = pWI;
    pPrivWin->pLastWInterest = pWI;

    pWI->pNextWInterest = (WInterestPtr) &pPrivWin->pNextWInterest;
    pWI->pLastWInterest = (WInterestPtr) &pPrivWin->pNextWInterest;
    pWI->length = sizeof(WInterestRec); 
    pWI->owner = 0;		/* server owns this */
    pWI->ValInterestMask = ~0;	/* interested in everything at validate time */
    pWI->ValidateWindow = mpelValidateWindow ;
    pWI->ChangeInterestMask = 0; /* interested in nothing at change time */
    pWI->ChangeExtWindowAttributes = (Bool (*)()) NULL ;
    pWI->MapWindow = (Bool (*)()) NULL ;
    pWI->UnmapWindow = (Bool (*)()) NULL ;
    pWI->CreateWindow = (Bool (*)()) NULL ;
    pWI->DestroyWindow = mpelDestroyWindowHeader ;
    pWI->PositionWindow = (Bool (*)()) NULL ;
    pWI->WIProc = mpelWIProc ;
    pWI->extPriv = 0 ;


    return (TRUE);
}


Bool 
mpelDestroyWindow(pWin)
    WindowPtr pWin;
{

    aixPrivWin *pPrivWin;

    WInterestPtr pWI , pWInit , pWInext ;

    pPrivWin = (aixPrivWin *)(pWin->devPrivate);


   /* here we Destroy WindowInterest Stuff */

    pWI = pPrivWin->pNextWInterest;
    pWInit = (WInterestPtr) &pPrivWin->pNextWInterest;

    do
    {
        pWInext = pWI->pNextWInterest;
	if(pWI->DestroyWindow)
	    (*pWI->DestroyWindow) (pWin, pWI);
	pWI = pWInext;
    }
    while(pWI != pWInit);


    /* keep it for a while  ! */

    if(pPrivWin->pRotatedBorder)
    	mfbDestroyPixmap(pPrivWin->pRotatedBorder);

    if(pPrivWin->pRotatedBackground)
    	mfbDestroyPixmap(pPrivWin->pRotatedBackground);

    Xfree(pWin->devPrivate);

    return (TRUE);
}


Bool mpelMapWindow(pWin)
WindowPtr pWin;
{

   aixPrivWin *pPrivWin;
   WInterestPtr pWI , pWInit , pWInext ;
   pPrivWin = (aixPrivWin *)(pWin->devPrivate);

   /* here we Map WindowInterest Stuff */
   /* replace the following codes by macro ??? */

    pWI = pPrivWin->pNextWInterest;
    pWInit = (WInterestPtr) &pPrivWin->pNextWInterest;

    do
    {
        pWInext = pWI->pNextWInterest;
	if(pWI->MapWindow)
	    (*pWI->MapWindow) (pWin);
	pWI = pWInext;
    }
    while(pWI != pWInit);


    mfbMapWindow(pWin) ;

    return (TRUE);
}


Bool mpelPositionWindow(pWin, x, y)
register WindowPtr pWin;
int x, y;
{

   register aixPrivWin *pPrivWin;
   WInterestPtr pWI , pWInit , pWInext ;

   pPrivWin = (aixPrivWin *)(pWin->devPrivate);

   /* here we Position WindowInterest Stuff */
   /* replace the following codes by macro ??? */

    pWI = pPrivWin->pNextWInterest;
    pWInit = (WInterestPtr) &pPrivWin->pNextWInterest;

    do
    {
        pWInext = pWI->pNextWInterest;
	if(pWI->PositionWindow)
	    (*pWI->PositionWindow) (pWin,x,y);
	pWI = pWInext;
    }
    while(pWI != pWInit);

    mfbPositionWindow(pWin,x,y);	

    return (TRUE);
}

Bool mpelUnmapWindow(pWin, x, y)
WindowPtr pWin;
int x, y;
{


   aixPrivWin *pPrivWin;
   WInterestPtr pWI , pWInit , pWInext ;
   pPrivWin = (aixPrivWin *)(pWin->devPrivate);

   /* here we Map WindowInterest Stuff */
   /* replace the following codes by macro ??? */

    pWI = pPrivWin->pNextWInterest;
    pWInit = (WInterestPtr) &pPrivWin->pNextWInterest;

    do
    {
        pWInext = pWI->pNextWInterest;
	if(pWI->UnmapWindow)
	    (*pWI->UnmapWindow) (pWin,x,y);
	pWI = pWInext;
    }
    while(pWI != pWInit);


    mfbUnmapWindow(pWin, x, y) ;

    return (TRUE);
}
#endif
