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
/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedCursor.c,v 1.4 89/11/19 20:21:03 jfc Exp $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedCursor.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedCursor.c,v 1.4 89/11/19 20:21:03 jfc Exp $";
#endif

#include "X.h"
#include "Xmd.h"

#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursorstr.h"

#include "maskbits.h"

#include "ibmTrace.h"

#include "ibmScreen.h"

#include "OScompiler.h"
#include "aedHdwr.h"

/***============================================================***/

	/*
	 * NOTES ON THE RT/PC X11 SERVER CURSOR SUPPORT (../librt/rtcursor.c)
	 * The X emulator in the kernel normally tries to track the cursor.
	 * Unfortunately, the kernel aed locator code uses the aed library
	 * code to copy and clear cursor images.  If the X server is
	 * trying to talk to the aed microcode at the same time, it can get
	 * pretty messy.  This means that we have to track the cursor in the
	 * server.   The rt cursor support (in librt) tries to allow for
	 * the server to track the cursor in a fairly generic way.  If the
	 * variable rtSoftCursor is non-zero, every mouse event that gets
	 * reported is also used to update the cursor position.  If the
	 * motion event occurs INSIDE of the BOX rtCursorBounds, *rtCursorX and
	 * *rtCursorY (both are short *) are set to the coordinates of the
	 * event offset by a hotspot (rtCursorHotX and rtCursorHotY).
	 * If the event occurs outside of rtCursorBounds, the function
	 * rtCursorShow() is called with the coordinates of the event.
	 */

/***============================================================***/

static	int	aedCursHidden;

	/*
	 * Initialize the aedcursor package.  Sets up data structures
	 * and such so the RT/PC specific code will track the cursor
	 * correctly.  Called by aedScreenInit().
	 *
	 * I had trouble convinving the kernel *not* to track the cursor
	 * automatically.  The magic sequence of ioctls below seems to
	 * do the trick.
	 */

void
aedShowCursor(x,y)
    register short x, y;
{
    /* TRACE(("aedShowCursor(x=%d,y=%d)\n", x, y)); */
    vforce();
    vikint[1] = 10;	/* position cursor */
    vikint[2] = x;
    vikint[3] = y;
    command(3);
    clear(2);
    return;
}

/***============================================================***/

/*ARGSUSED*/
void
aedRemoveCursor( scrIndex )
    int scrIndex;
{

    aedCursHidden = TRUE;
    vforce();
    vikint[1] = 7;	/* define cursor */
    vikint[2] = 0;
    vikint[3] = 0;
    bzero((char *)&vikint[4], 1024); /* zeroes */
    command(515);
    clear(2);
    return;
}

/***============================================================***/

aedCursorInit(scrIndex)
    int scrIndex;
{
    TRACE(("aedCursorInit(scrIndex=%d)\n",scrIndex));

    ibmCursorShow( scrIndex ) = aedShowCursor;
    ibmCurrentCursor( scrIndex ) = NULL;

    aedRemoveCursor(scrIndex);

    vforce();
    vikint[1] = 8;	/* enable cursor command */
    command(1);
    clear(2);

    return TRUE;
}

/***============================================================***/

	/* 
	 * realize cursor for aed.
	 */

Bool
aedRealizeCursor( pScr, pCurs )
    ScreenPtr	pScr;
    CursorPtr	pCurs;
{
    unsigned long int *pStart;
    register unsigned long int *pImage, *pMask;
    register unsigned long int *psrcImage, *psrcMask;
    register int srcHeight;
    register unsigned long int endbits;
    int srcWidth;
    int srcRealWidth;
    int i;

    TRACE(("aedRealizeCursor( pScr= 0x%x, pCurs= 0x%x)\n",pScr,pCurs));

    if( (pCurs->devPriv[ pScr->myNum ] = (pointer)Xalloc(1024)) == NULL )
	{
	ErrorF("aedRealizeCursor: can't malloc\n");
	return FALSE;
	}
    pStart = pMask = (unsigned long int *) pCurs->devPriv[pScr->myNum];
    pImage = pMask + 128;
    bzero((char *) pMask, 1024);
    psrcImage = (unsigned long *)pCurs->bits->source;
    psrcMask = (unsigned long *)pCurs->bits->mask;
    srcRealWidth = srcWidth = (pCurs->bits->width + 31 ) / 32;
    endbits = endtab[pCurs->bits->width & 0x1f];
    if ( endbits == 0 )
	endbits = 0xffffffff;
    if( srcWidth > 2 )
	{
	srcWidth = 2;
	endbits = ~0;
	}
    if ( ( srcHeight = pCurs->bits->height ) > 64 )
	srcHeight = 64;
    if ( srcWidth == 1 )
	while ( srcHeight-- )
	{
	    *pImage = (*psrcImage++) & endbits;
	    pImage += 2;
	    *pMask  = (*psrcMask++)  & endbits;
	    pMask  += 2 ;
	}
    else
	while ( srcHeight-- )
	{
	    *pImage++ = *psrcImage++ ;
	    *pMask++  = *psrcMask++ ;
	    *pImage++ = *psrcImage++ & endbits;
	    *pMask++  = *psrcMask++  & endbits;
	}

    pMask = pStart;
    pImage = pMask + 128;
    if ((pCurs->foreRed + pCurs->foreGreen + pCurs->foreBlue) >
	(pCurs->backRed + pCurs->backGreen + pCurs->backBlue))
	for (i = 0; i < 128; i++)
	{
	    unsigned long int mask = *pMask;
	    unsigned long int source = *pImage;

	    *pMask++ = mask & ~source;
	    *pImage++ = mask & source;
	}
    else
	for (i = 0; i < 128; i++)
	{
	    unsigned long int mask = *pMask;
	    unsigned long int source = *pImage;

	    *pMask++ = mask & source;
	    *pImage++ = mask & ~source;
	}

    if (!ibmCurrentCursor(pScr->myNum)) {
	ibmCurrentCursor(pScr->myNum)= pCurs;
    }
    return TRUE;
}

/***============================================================***/

	/*
	 * Free up the space reserved for 'pCurs'
	 */

Bool
aedUnrealizeCursor( pScr, pCurs)
    register ScreenPtr 	pScr;
    register CursorPtr 	pCurs;
{

    TRACE(("aedUnrealizeCursor( pScr= 0x%x, pCurs= 0x%x )\n",pScr,pCurs));

    Xfree( pCurs->devPriv[ pScr->myNum ]);
    return TRUE;
}

/***============================================================***/

	/*
	 *  Display (and track) the cursor described by "pCurs"
	 *  Copies the cursor image into the hardware active cursor
	 *  area.
	 *
	 *  If the cursor image has not already been copied into the
	 *  adapted off-screen memory (cursor is not "realized"), try
	 *  to realize it.  If the area reserved for cursor images is
	 *  full, print an error message and bail out.
	 *
	 *  After copying the cursor image, adjust ibmCursorHotX and 
	 *  ibmCursorHotY so that the cursor is displayed with it's
	 *  hot spot at the coordinates of mouse motion events.
	 *  Bearing in mind that we are tracking the cursor hot spot
	 *  (NOT the edges of the cursor), we have to adjust the
	 *  acceptable bounds (ibmCursorBounds) of the cursor so we bump 
	 *  the cursor image around at the right times).
	 */

int
aedDisplayCursor( pScr, pCurs )
    register ScreenPtr 	pScr;
    register CursorPtr 	pCurs;
{
    TRACE(("aedDisplayCursor( pScr= 0x%x, pCurs= 0x%x )\n",pScr,pCurs));

    ibmCursorHotX( pScr->myNum ) = pCurs->bits->xhot ;
    ibmCursorHotY( pScr->myNum ) = pCurs->bits->yhot ;
    ibmCurrentCursor( pScr->myNum ) = pCurs ;

    vforce();
    vikint[1] = 7;	/* define cursor */
    vikint[2] = (short) pCurs->bits->xhot;
    vikint[3] = (short) pCurs->bits->yhot;
    MOVE( (char *) pCurs->devPriv[pScr->myNum], (char *) &vikint[4], 1024 );
    command(515);
    clear(2);
    if (aedCursHidden) {
	vforce();
	vikint[1] = 8;	/* enable cursor command */
	command(1);
	clear(2);
	aedCursHidden = FALSE ;
    }
    return TRUE;
}
