/***********************************************************

Copyright 1987 by the Regents of the University of California
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  
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

/* $Header: */

#include    "mfb.h"
#include    "X.h"
#include    "mibstore.h"
#include    "regionstr.h"
#include    "scrnintstr.h"
#include    "pixmapstr.h"
#include    "windowstr.h"
#include    "servermd.h"

#include    "aedHdwr.h"
#include    "OScompiler.h"
#include    "ibmTrace.h"

/*-
 *-----------------------------------------------------------------------
 * aedSaveAreas --
 *	Function called by miSaveAreas to actually fetch the areas to be
 *	saved into the backing pixmap. 
 *
 * Results:
 *	None.
 *
 * Side Effects:
 *	Data are copied from the screen into the pixmap.
 *
 *-----------------------------------------------------------------------
 */
void
aedSaveAreas(pPixmap, prgnSave, xorg, yorg)
    PixmapPtr	  	pPixmap;  	/* Backing pixmap */
    RegionPtr	  	prgnSave; 	/* Region to save (pixmap-relative) */
    int	    	  	xorg;	    	/* X origin of region */
    int	    	  	yorg;	    	/* Y origin of region */
{
    BoxPtr		pBox;
    int			nBox;
    int			*pdst, pw;

    TRACE(("aedSaveAreas(0x%x,0x%x,%d,%d)\n",pPixmap,prgnSave,xorg,yorg));
    TRACE(("\tpixmap: {w = %d, h = %d, ptr = %#x}\n", pPixmap->drawable.width,
	   pPixmap->drawable.height, pPixmap->devPrivate.ptr));
    
    if((nBox = REGION_NUM_RECTS(prgnSave)) == 0)
      return;
    pBox = REGION_RECTS(prgnSave);

    pdst = (int *)pPixmap->devPrivate.ptr;
    pw = (pPixmap->drawable.width + 31)/32;

    while(nBox--)
      {
	int y, inlen;
	int x = pBox->x1;
	int width;
	/* x = ((x + xorg)/32) * 32; */ 
	width = pBox->x2 - x;
	x += xorg;
	width = ((width + 31)/32)*32;
	vforce();
	clear(2);
	for(y=pBox->y1; y<pBox->y2; y++)
	  {
	    int *tmppdst = pdst + (pBox->x1>>5) + pw * y;

	    vikint[1] = 4;			/* read command */
	    vikint[2] = (short) x;		/* source x */
	    vikint[3] = (short) (y + yorg);	/* source y */
	    vikint[4] = (short) width;		/* slice width */
	    vikint[5] = 1;			/* slice height */
	    vikint[6] = 0;			/* window color */
	    
	    /* send command to Viking */
	    command(6);
	    
	    TRACE(("\tx = %d, y = %d, width = %d\n", x, (y+yorg), width));
	    inlen = width/16;
	    
	    vikwait(); /* wait until viking is finished reading */
	    
	    /* copy read-back data into user buffer */
	    VREAD(tmppdst,inlen,0X4002);
	  }
	pBox++;
      }
    clear(2);
    TRACE(("\treturn\n"));
    return;
}

/*-
 *-----------------------------------------------------------------------
 * aedRestoreAreas --
 *	Function called by miRestoreAreas to actually fetch the areas to be
 *	restored from the backing pixmap. 
 *
 * Results:
 *	None.
 *
 * Side Effects:
 *	Data are copied from the pixmap into the screen.
 *
 *-----------------------------------------------------------------------
 */
void
aedRestoreAreas(pPixmap, prgnRestore, xorg, yorg)
    PixmapPtr	  	pPixmap;  	/* Backing pixmap */
    RegionPtr	  	prgnRestore; 	/* Region to restore (screen-relative)*/
    int	    	  	xorg;	    	/* X origin of window */
    int	    	  	yorg;	    	/* Y origin of window */
{
    BoxPtr		pBox;
    int			nBox,srcWidth, srcHeight, srclen;
    int			nbits, dbits, i;

    TRACE(("aedRestoreAreas(0x%x,0x%x,%d,%d)\n",
	   pPixmap,prgnRestore,xorg,yorg));
    TRACE(("\tpixmap: {w = %d, h = %d, ptr = %#x}\n", pPixmap->drawable.width,
	   pPixmap->drawable.height, pPixmap->devPrivate.ptr));
    if((nBox = REGION_NUM_RECTS(prgnRestore)) == 0)
      return;
    pBox = REGION_RECTS(prgnRestore);

    srcWidth = pPixmap->drawable.width;
    srcHeight = pPixmap->drawable.height;
    srclen = ( srcWidth + 15 ) / 16;
    nbits = srclen * srcHeight;
    if(nbits > 2000)
      {
	srcHeight = 2000/srclen;
	dbits = srcHeight*srclen;
	TRACE(("\tsrcHeight changed to %d, nbits = %d, dbits = %d\n",
	       srcHeight, nbits, dbits));
      } else {
	dbits = nbits;
      }

    /* Process 2000 words at a time */
    for(i = 0;nbits > 0;nbits -= dbits,i++)
      {
	register BoxPtr pbox = pBox;
	register int nbox = nBox,j;
	int skip;
	register short *bits = (short *)pPixmap->devPrivate.ptr + i * dbits;

	clear(2);
	skip = (((srcWidth-1) & 0x1f) < 16);
	vikint[ORMERGE] = mergexlate[GXcopy];
	vikint[ORXPOSN] = xorg;
	vikint[ORYPOSN] = yorg + srcHeight * i;
	vikint[ORCLIPLX] = pbox->x1;
	vikint[ORCLIPLY] = pbox->y1;
	vikint[ORCLIPHX] = pbox->x2-1;
	vikint[ORCLIPHY] = pbox->y2-1;
	pbox++;
	nbox--;

	if(nbits < dbits)
	  srcHeight = nbits / srclen;
	TRACE(("\tx = %d, y = %d, skip = %d\n",xorg, yorg + srcHeight * i, skip));
	TRACE(("\tsrclen = %#x, bits = %d (%d)\n", bits, srclen,
	       ((short *)bits - (short *)pPixmap->devPrivate.ptr)/srclen));
	vikint[vikoff++] = 9;		/* draw image order */
	vikint[vikoff++] = srcWidth;
	vikint[vikoff++] = srcHeight;
	if ( skip )
	{
            for(j=0; j<srcHeight; j++)
            {
        	MOVE(bits, vikint+vikoff, srclen<<1 );
        	bits = bits + srclen + 1; /* skip the last 16 pad bits */
        	vikoff += srclen;
            }
	}
	else
	{
            MOVE(bits, vikint+vikoff, ( ( srclen<<1 ) * srcHeight ) );
            vikoff += ( srclen * srcHeight );
	}

	vforce();

	vikint[VIKCMD]      = 2;            /* reprocess orders */
	for(j=0; j<nbox; j++, pbox++)
	{
            vikint[ORCLIPLX] = pbox->x1;
            vikint[ORCLIPLY] = pbox->y1;
            vikint[ORCLIPHX] = pbox->x2-1;
            vikint[ORCLIPHY] = pbox->y2-1;
            command(ORDATA);
	}
      }
    clear(2);
    return;
}
