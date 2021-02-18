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
/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Curs.c,v 3.0 89/11/02 15:18:16 probe Exp $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Curs.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Curs.c,v 3.0 89/11/02 15:18:16 probe Exp $";
#endif

#include "X.h"
#include "Xmd.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursorstr.h"

#include "mfb.h"
#include "maskbits.h"

#include "OScompiler.h"

#if defined(BSDrt)
#include "bsdIO.h"
#else
#include "OSio.h"
	******** ERROR ******** Currently busted for AIXrt
#endif

#include "ibmScreen.h"
#include "ibmTrace.h"
#include "apa16Hdwr.h"

/***============================================================***/

	/*
	 * NOTES ON THE APA16 HARDWARE CURSOR:
	 * The apa16 supports a 48x64 hardware cursor which 'AND's and
	 * 'XOR's two different patterns simultaneously on the screen.
	 * The cursor patterns are stored at (0,784) and (48,784) on
	 * the apa16 adapter.
	 *
	 * Writes to the adapter registers (CURSOR_X and CURSOR_Y) move
	 * the cursor on the screen.
	 * 
	 * The CURSOR_Y register may be set to any value -- the cursor
	 * is not displayed if CURSOR_Y is <0 or >830 (767+64).
	 * A partial cursor is displayed for CURSOR_Y<64 or CURSOR_Y>767.
	 *
	 * The CURSOR_X register must be in the range 0<=CURSOR_X<=976
	 * (1024-48).   Values outside of this range distort the screen
	 * image.   The apa16 does *NOT* display a partial cursor image
	 * for a cursor that falls off of the left or right of the screen
	 * we have slide the cursor image(s) around in the reserved
	 * hardware area to simulate the cursor moving off of the screen.
	 *
	 * NOTES ON THE RT/PC X11 SERVER CURSOR SUPPORT (../librt/rtcursor.c):
	 * The X emulator in the kernel normally tries to track the cursor.
	 * Unfortunately, the kernel apa16 locator code uses the apa16 rasterop
	 * processor to copy and clear cursor images.  If the X server is
	 * trying to use the apa16 hardware at the same time, it can get
	 * pretty messy.  This means that we have to track the cursor in the
	 * server.   
	 *
	 * The area to the right of the active cursor area on the adapter
	 * (the rectangle with corners at (96,767) and (1023,847) is used 
	 * to store additional cursor patterns.   When a cursor is "realized" 
	 * we allocate space (if available) in this rectangle and copy 
	 * the image into it.
	 * When a "realized" cursor is displayed, we can use the apa16
	 * rasterop processor to copy the image from the reserved area (on
	 * the adapter) into the active cursor area.
	 */

/***============================================================***/

	/*
	 * all cursor images are between scan-lines 784 and 847 (inclusive),
	 * and consist of two 48x64 masks.
	 * For each "realized" cursor we need to know the X coordinate
	 * of the left edge of the masks.
	 *
	 * We use "next" to keep track of the next free space for the cursor.
	 *
	 * MAXCURSORS is the number of cursor images we have reserved space
	 * for on the adapter.
	 *
	 * XOR_EDGE and AND_EDGE calculate the right edge of the masks
	 * for a cursor.  We need the right edge because the apa16 hardware
	 * uses the lower right corner of the area to be copied.
	 */

typedef struct A16CURSOR {
	struct	A16CURSOR	*next;
	short			edge;
} apa16Cursor;

#define ACTIVE_CURSOR	(&cursors[0])
#define	TEMP_CURSOR	(&cursors[1])

#define XOR_EDGE(c)	((c)->edge+2*CURSOR_WIDTH)
#define AND_EDGE(c)	((c)->edge+CURSOR_WIDTH)

/***============================================================***/

	/*
	 * "cursors" is an array of structures describing the available
	 * cursor images.
	 * "nextFree" is structure describing the next area to be allocated.
	 * "currentCursor" is the most recently displayed cursor.
	 */

static	apa16Cursor	cursors[MAXCURSORS]; 
static	apa16Cursor	*nextFree;
static	apa16Cursor	*currentCursor;

/***============================================================***/

#if defined(DEBUG) && !defined(NDEBUG)
void
copyoutcursor()
{
unsigned cmd;

    APA16_GET_CMD(ROP_RECT_COPY,GXcopy,cmd);
    COPY_RECT(cmd,300,	300,
		  XOR_EDGE(ACTIVE_CURSOR),	CURSOR_AREA_BOTTOM,
		  2*CURSOR_WIDTH,	CURSOR_HEIGHT);
    return;
}
#endif

/***============================================================***/

	/*
	 * Clear the image of the cursor area described by "Curs."
	 * To clear a cursor image, we want to set the AND mask to
	 * all 1's and the XOR mask to all 0's.
	 */

static void
apa16ClearCursor(Curs)
apa16Cursor	*Curs;
{
unsigned cmd;

    TRACE(("apa16ClearCursor( Curs= 0x%x )\n",Curs));

    APA16_GET_CMD(ROP_RECT_FILL,RROP_WHITE,cmd);
    FILL_RECT(cmd,AND_EDGE(Curs),CURSOR_AREA_BOTTOM,CURSOR_WIDTH,CURSOR_HEIGHT);
    APA16_GET_CMD(ROP_RECT_FILL,RROP_BLACK,cmd);
    FILL_RECT(cmd,XOR_EDGE(Curs),CURSOR_AREA_BOTTOM,CURSOR_WIDTH,CURSOR_HEIGHT);
    return ;
}
/***============================================================***/

static void
apa16CopyCursor(srcCurs,dstCurs)
apa16Cursor	*srcCurs,*dstCurs;
{
unsigned cmd;

    TRACE(("apa16CopyCursor( srcCurs= 0x%x, dstCurs= 0x%x)\n",srcCurs,dstCurs));

    APA16_GET_CMD(ROP_RECT_COPY,GXcopy,cmd);
    COPY_RECT(cmd,XOR_EDGE(dstCurs),	CURSOR_AREA_BOTTOM,
		  XOR_EDGE(srcCurs),	CURSOR_AREA_BOTTOM,
		  2*CURSOR_WIDTH,	CURSOR_HEIGHT);
    return ;
}

/***============================================================***/

	/*
	 * The RT/PC server will take care of updating the cursor location
	 * (through ibmCursorX and ibmCursorY) for most of the screen.
	 * The apa16 hardware is brain-damaged and can't handle the cursor
	 * around the left and right edges of the screen, so we have to 
	 * slide the cursor image around without moving the box to simulate
	 * the cursor going off of the screen.   apa16ShowCursor() takes
	 * care of this.
	 * Keep in mind that the COPY_RECT uses the <<lower right>> corner
	 * of the source and destination areas.
	 *
	 */

void
apa16ShowCursor(x,y)
register short x,y;
{
register unsigned cmd;
register int	offset;
static	int	obscured= TRUE;

    TRACE(("apa16ShowCursor( x= %d, y= %d )\n",x,y));

    if ( ibmScreenState(ibmCurrentScreen)!= SCREEN_ACTIVE) {
	return;
    }

    APA16_GET_CMD(ROP_RECT_COPY,GXcopy,cmd);
    if (x<ibmCursorHotX(ibmCurrentScreen)) {
	offset= ibmCursorHotX(ibmCurrentScreen)-x;
	apa16ClearCursor(ACTIVE_CURSOR);
	if (offset < CURSOR_WIDTH) {
	    COPY_RECT(cmd, AND_EDGE(ACTIVE_CURSOR)-offset,  CURSOR_AREA_BOTTOM,
		           AND_EDGE(currentCursor),	    CURSOR_AREA_BOTTOM,
			   CURSOR_WIDTH-offset,	    	    CURSOR_HEIGHT);
	    COPY_RECT(cmd, XOR_EDGE(ACTIVE_CURSOR)-offset,  CURSOR_AREA_BOTTOM,
		           XOR_EDGE(currentCursor),	    CURSOR_AREA_BOTTOM,
		           CURSOR_WIDTH-offset,	    	    CURSOR_HEIGHT);
	}
	CURSOR_X= 0;
	CURSOR_Y= y-ibmCursorHotY(ibmCurrentScreen);
	obscured= TRUE;
    }
    else if (x>APA16_WIDTH-CURSOR_WIDTH+ibmCursorHotX(ibmCurrentScreen)) {
	offset= x-APA16_WIDTH-ibmCursorHotX(ibmCurrentScreen)+CURSOR_WIDTH;
	apa16ClearCursor(ACTIVE_CURSOR);
	if (offset < CURSOR_WIDTH) {
	    COPY_RECT(cmd, AND_EDGE(ACTIVE_CURSOR),    	    CURSOR_AREA_BOTTOM,
		           AND_EDGE(currentCursor)-offset,  CURSOR_AREA_BOTTOM,
		           CURSOR_WIDTH-offset,		    CURSOR_HEIGHT);
	    COPY_RECT(cmd, XOR_EDGE(ACTIVE_CURSOR),	    CURSOR_AREA_BOTTOM,
		           XOR_EDGE(currentCursor)-offset,  CURSOR_AREA_BOTTOM,
		           CURSOR_WIDTH-offset,		    CURSOR_HEIGHT);
	}
	CURSOR_X= APA16_WIDTH-48;
	CURSOR_Y= y-ibmCursorHotY(ibmCurrentScreen);
	obscured= TRUE;
    }
    else {
	if (obscured) {
	    apa16CopyCursor(currentCursor,ACTIVE_CURSOR);
	    obscured= FALSE;
	}
	CURSOR_X= x-ibmCursorHotX(ibmCurrentScreen);
	CURSOR_Y= y-ibmCursorHotY(ibmCurrentScreen);
    }
    return;
}

/***============================================================***/

    /*
     * Copy the upper left (48x64) rectangle of mask and source
     * onto reserved space on the apa16 adapter.
     * (server pads cursor images to longword boundaries)
     *
     * The code to copy treats each mask as three 16 bit wide
     * vertical bands.  SET_BAND sets band 'n' in both destination
     * masks (sD,mD) to the values in band 'n' of the source masks (sS,mS).
     * 
     * We copy the X "source" onto the XOR area and the X "mask" onto the
     * "AND" area.
     */

#define SERVER_PAD(w)	((((w)+31)/32)*4)

#define	SET_BAND(ndx,fg,w,sS,mS,aD,xD)\
	if ((w)>=((ndx+1)*16)) {\
	    (aD)[(ndx)]=	~(mS)[(ndx)];\
    	    (xD)[(ndx)]=	((mS)[(ndx)])&((fg)^((sS)[(ndx)]));\
	}

static
apa16CopyXCursor(srcCurs,dstCurs)
CursorPtr	srcCurs;
apa16Cursor	*dstCurs;
{
    CARD16	*srcSrc,*mskSrc;
    CARD16	*andDst,*xorDst;
    CARD16	fg;
    int		srcWidth,dstWidth;
    unsigned	endbits,i,rightEdge;

    TRACE(("apa16CopyXCursor(srcCurs= 0x%x, dstCurs= 0x%x)\n",srcCurs,dstCurs));
    /* set up pointers to source and destination */
    mskSrc= 	(CARD16 *)srcCurs->bits->mask;
    srcSrc= 	(CARD16 *)srcCurs->bits->source;
    srcWidth=	SERVER_PAD(srcCurs->bits->width)/2;
    andDst= 	SCREEN_ADDR(dstCurs->edge,CURSOR_AREA_TOP);
    xorDst= 	SCREEN_ADDR((dstCurs->edge+48),CURSOR_AREA_TOP);
    dstWidth=	APA16_WIDTH/16;

    if ((srcCurs->foreRed+srcCurs->foreGreen+srcCurs->foreBlue)>
	(srcCurs->backRed+srcCurs->backGreen+srcCurs->backBlue)) {
	fg= 0;
    }
    else fg= 0xffff;

    apa16ClearCursor(dstCurs);
    QUEUE_WAIT();

		/* endtab is 32 bit masks, turn it into a 16 bit mask */
    endbits=   endtab[srcCurs->bits->width&0xf]>>16;
    rightEdge= srcCurs->bits->width/16;
    for (i=0;i<srcCurs->bits->height;i++) {
	SET_BAND(0,fg,srcCurs->bits->width,srcSrc,mskSrc,andDst,xorDst);
	SET_BAND(1,fg,srcCurs->bits->width,srcSrc,mskSrc,andDst,xorDst);
	SET_BAND(2,fg,srcCurs->bits->width,srcSrc,mskSrc,andDst,xorDst);
	if ((srcCurs->bits->width&0xf)&&(rightEdge<=2)) {
	    andDst[rightEdge]=	~(mskSrc[rightEdge]&endbits);
	    xorDst[rightEdge]= (mskSrc[rightEdge]&(fg^srcSrc[rightEdge]))&
									endbits;
	}
	srcSrc+= srcWidth; mskSrc+= srcWidth;
	andDst+= dstWidth; xorDst+= dstWidth;
    }
    return TRUE;
}

/***============================================================***/

/*ARGSUSED*/
void
apa16RemoveCursor(scrIndex)
    int	scrIndex;
{

    TRACE(("apa16RemoveCursor(scrIndex=%d)\n",scrIndex));
    if ( ibmScreenState( ibmCurrentScreen )==SCREEN_ACTIVE )
	apa16ClearCursor(ACTIVE_CURSOR);
    return;
}

/***============================================================***/

	/*
	 * Initialize the apa16cursor package.  Sets up data structures
	 * and such so the RT/PC specific code will track the cursor
	 * correctly.  Called by apa16ScreenInit().
	 *
	 * I had trouble convinving the kernel *not* to track the cursor
	 * automatically.  The magic sequence of ioctls below seems to
	 * do the trick.
	 */

apa16CursorInit(scrIndex)
    int	scrIndex;
{
int  i,col;

    TRACE(("apa16CursorInit( scrIndex=%d )\n",scrIndex));
    /* Initialize data structures */
    for (i=0,col=0;i<MAXCURSORS;i++,col+=2*CURSOR_WIDTH) {
	cursors[i].next= 	&cursors[i+1];
	cursors[i].edge= 	col;
    }
    cursors[MAXCURSORS-1].next=	NULL;
    nextFree= &cursors[2];

    if ( ibmScreenState( scrIndex ) == SCREEN_ACTIVE ) {
	apa16RemoveCursor(scrIndex);
	apa16ClearCursor(ACTIVE_CURSOR);
    }
    return TRUE;
}

/***============================================================***/

	/*
	 * Lots of cursors that will never actually be used get
	 * realized, so don't bother to save space for cursors until
	 * they are actually used.
	 * The ibmCurrentCursor stuff is a hack.  Right now, the DIX
	 * layer doesn't bother to display a cursor when we switch screens
	 * so there is no active cursor.   Easy enough to fix:
	 * the first realized cursor becomes the default unless another
	 * cursor is explicitly displayed.  Easy, but a hack.
	 */

Bool
apa16RealizeCursor( pScr, pCurs)
    ScreenPtr	pScr;
    CursorPtr	pCurs;	/* a SERVER-DEPENDENT cursor */
{
    TRACE(("apa16RealizeCursor( pScr= 0x%x, pCurs= 0x%x )\n",pScr,pCurs));
    pCurs->devPriv[ pScr->myNum ]= (pointer)NULL;
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
apa16UnrealizeCursor( pScr, pCurs)
    ScreenPtr 	pScr;
    CursorPtr 	pCurs;
{
apa16Cursor *cur;

    TRACE(("apa16UnrealizeCursor( pScr= 0x%x, pCurs= 0x%x )\n",pScr,pCurs));
    if (pCurs->devPriv[ pScr->myNum ]) {

	cur= (apa16Cursor *)pCurs->devPriv[ pScr->myNum ];
	if (cur&&(!cur->next)) {
	    cur->next=	nextFree;
	    nextFree=	cur;
	    pCurs->devPriv[ pScr->myNum ]= NULL;
	}
    }
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
apa16DisplayCursor( pScr, pCurs )
    ScreenPtr 	pScr;
    CursorPtr 	pCurs;
{
apa16Cursor	*aCurs;

    TRACE(("apa16DisplayCursor( pScr= 0x%x, pCurs= 0x%x )\n",pScr,pCurs));
    if ( ibmScreenState( pScr->myNum )==SCREEN_ACTIVE ) {
	if (!(aCurs= (apa16Cursor*)pCurs->devPriv[ pScr->myNum ])) {
	    /* actually find someplace to store the cursor */
	    if (!(aCurs=nextFree)) { /* nothing free */
		aCurs= TEMP_CURSOR;
	    }
	    else {
		nextFree=	   nextFree->next;
		aCurs->next= NULL;
		pCurs->devPriv[ pScr->myNum ]= (pointer)aCurs;
	    }
	    apa16CopyXCursor(pCurs,aCurs);
	}
	apa16CopyCursor(aCurs,ACTIVE_CURSOR);
    }

    ibmCursorHotX(pScr->myNum)=		pCurs->bits->xhot;
    ibmCursorHotY(pScr->myNum)=		pCurs->bits->yhot-CURSOR_HEIGHT;
    ibmCurrentCursor(pScr->myNum)=	pCurs;
    currentCursor= aCurs;

    apa16ShowCursor(CURRENT_X(),CURRENT_Y());
    return TRUE;
}

/***====================================================================***/

#define	CURS_SAVE_WIDTH	(2*CURSOR_WIDTH*MAXCURSORS)
#define	CURS_SV_WIDTH_WDS	((CURS_SAVE_WIDTH)/16)

static	unsigned short	apa16CursSave[CURS_SV_WIDTH_WDS*CURSOR_HEIGHT];

void
apa16InvalidateCursor()
{
register apa16MemoryPtr	pSrc,pDst;
register int		 x,y;

    TRACE(("apa16InvalidateCursor()\n"));
    pDst= apa16CursSave;
    pSrc= SCREEN_ADDR(0,CURSOR_AREA_TOP);
    QUEUE_WAIT();
    y= CURSOR_HEIGHT;
    while (y-->0) {
	x= CURS_SV_WIDTH_WDS;
	while (x-->0) {
	    *pDst= *pSrc;
	    pDst++; pSrc++;
	}
	pSrc+= ((APA16_WIDTH-CURS_SAVE_WIDTH)/16);
    }
    return;
}

/***====================================================================***/

void
apa16ReinitializeCursor()
{
register apa16MemoryPtr	pSrc,pDst;
register int		 x,y;

    TRACE(("apa16ReinitializeCursor()\n"));
    pDst= SCREEN_ADDR(0,CURSOR_AREA_TOP);
    pSrc= apa16CursSave;
    QUEUE_WAIT();
    y= CURSOR_HEIGHT;
    while (y--) {
	x= CURS_SV_WIDTH_WDS;
	while (x--) {
	    *pDst++= *pSrc++;
	}
	pDst+= ((APA16_WIDTH-CURS_SAVE_WIDTH)/16);
    }
    apa16ShowCursor(CURRENT_X(),CURRENT_Y());
    return;
}
