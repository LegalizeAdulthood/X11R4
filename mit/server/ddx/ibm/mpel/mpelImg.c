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

/* $Header: /andrew/X11/r3src/release/server/ddx/ibm/mpel/RCS/mpelImg.c,v 6.7 89/04/29 21:38:31 jeff Exp $ */
/* $Source: /andrew/X11/r3src/release/server/ddx/ibm/mpel/RCS/mpelImg.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/release/server/ddx/ibm/mpel/RCS/mpelImg.c,v 6.7 89/04/29 21:38:31 jeff Exp $" ;
#endif

#include "X.h"
/*
#include "scrnintstr.h"
#include "colormapst.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "windowstr.h"

#include "ppc.h"
*/

#include "OScompiler.h"

#include "mpel.h"
#include "mpelHdwr.h"
#include "mpelFifo.h"

#include "ibmTrace.h"

/* Cursor Stuff */
extern int mpelcursorSemaphore ;
extern int mpelCheckCursor() ;
extern void mpelReplaceCursor() ;

/***==================================================================***/

/*
 * mpelDrawColorImage(x,y,w,h,data,alu,planes)
 */

void
mpelDrawColorImage( x, y, w, h, data, RowIncrement, alu, mask )
    int	x ;
    int	y ;
    int w ;
    register int h ;
    register unsigned char *data ;
    int	RowIncrement ;
    int	alu ;
    unsigned long int mask ;
{
    int maxrows ;
    mpelSrcBLTVPM blt ;
    int cursor_saved ;

    TRACE( ( "mpelDrawColorImage(%d,%d,%d,%d,0x%x,%d,0x%x,0x%x)\n",
	   x, y, w, h, data, RowIncrement, alu, mask ) ) ;

    if ( w <= 0 || h <= 0 || alu == GXnoop )
	return ;

    /* If Cursor Is In The Way Remove It */
    cursor_saved = !mpelcursorSemaphore && mpelCheckCursor( x, y, w, h ) ;

    mpelSetPlaneMask( mask ) ;
    /* So we KNOW the stage is NOT in use when we wait later */

    blt.srcaddr =	mpelAddr( MPEL_BLIT_STAGE ) ;
    blt.dest.lleft.x =	x ;
    blt.dest.uright.x =	x + w - 1 ;
    blt.bpixel =	0x0008 ;
    blt.alu =		alu + 1 ;

    if ( h <= ( maxrows = MPEL_BLIT_STAGE_SIZE / w ) ) {
	blt.dest.lleft.y =	MPEL_HEIGHT - ( y + h ) ;
	blt.dest.uright.y =	MPEL_HEIGHT - 1 - y ;

	/* copy image TO adapter buffer area */
	MPELWaitFifo() ;
	if ( RowIncrement != w ) {
	    register volatile unsigned char *daptr =
		    (volatile unsigned char *) MPEL_BLIT_STAGE ;
	    while ( h-- ) {
	        MOVE( data, daptr, w ) ;
	        daptr += w ;
		data += RowIncrement ;
	    }
	}
	else
	    MOVE( data, (volatile unsigned char *) MPEL_BLIT_STAGE, w * h ) ;

	MPELSrcBLTVPM( &blt ) ;
    } 
    else { /* do multiple blits */
	register int nh ;

	while ( h ) {
	    /* update values */
	    blt.dest.uright.y =	MPEL_HEIGHT - 1 - y ;
	    nh = MIN( h, maxrows ) ;
	    blt.dest.lleft.y =	MPEL_HEIGHT - ( y += nh ) ;
	    h -= nh ;

	    /* copy image TO adapter buffer area */
	    MPELWaitFifo() ;
	    if ( RowIncrement != w ) {
	        register volatile unsigned char *daptr =
			(volatile unsigned char *) MPEL_BLIT_STAGE ;
	        while ( nh-- ) {
	    	    MOVE( data, daptr, w ) ;
		    daptr += w ;
	    	    data += RowIncrement ;
	        }	
	    }
	    else
		{
	        MOVE( data, (volatile unsigned char *) MPEL_BLIT_STAGE, w*nh) ;
		data += nh*RowIncrement;
		}

	    MPELSrcBLTVPM( &blt ) ;
	    MPELNoOp( 2, &MPELNoOpData ) ;
	} /* end while */
    } /* end else */

    if ( cursor_saved )
	mpelReplaceCursor() ;

    return ;
}

/*
 * mpelReadColorImage(x,y,lx,ly,data)
 * 
 * BUG: the first 32-bit word is broken !!!
 */

void
mpelReadColorImage( x, y, w, h, data, RowIncrement )
    int		x ;
    int		y ;
    int		w ;
    register int h ;
    register unsigned char *data ;
    int	RowIncrement ;
{
    int maxrows ;
    mpelVPMBLTDestination blt ;
    int cursor_saved ;

    TRACE( ( "mpelReadColorImage(%d,%d,%d,%d,0x%x,%d)\n",
	   x, y, w, h, data, RowIncrement ) ) ;

    if ( w <= 0 || h <= 0 )
	return ;

    cursor_saved = !mpelcursorSemaphore && mpelCheckCursor( x, y, w, h ) ;

    blt.destaddr =		mpelAddr( MPEL_BLIT_STAGE ) ;
    blt.source.lleft.x =	x ;
    blt.source.uright.x =	x + w - 1 ;
    blt.comp =			0x0001 ;	/* no color compression */

    /* copy image TO adapter buffer area */
    if ( h <= ( maxrows = MPEL_BLIT_STAGE_SIZE / w ) ) {
	blt.source.lleft.y =	MPEL_HEIGHT - ( y + h ) ;
	blt.source.uright.y =	MPEL_HEIGHT - 1 - y ;
	/* do a DestBlt VPM */
	MPELVPMBLTDest( &blt ) ;
	/* Make Sure The Transfer is complete */
	MPELNoOp( 2, &MPELNoOpData ) ;
	MPELWaitFifo() ;
	/* NOW, read the data from the Stage */
	if ( RowIncrement != w ) {
	    register volatile unsigned char *ptr =
		(volatile unsigned char *) MPEL_BLIT_STAGE ;
	    while ( h-- ) {
		    MOVE( ptr, data, w ) ;
		    ptr += w ;
		    data += RowIncrement ;
	    }	
	}
	else
	    MOVE( (volatile unsigned char *) MPEL_BLIT_STAGE, data, w * h ) ;
    } 
    else { /* do multiple blits */
	register int nh ;

	while ( h ) {
	    nh = MIN( h, maxrows ) ;

	    /* update values */
	    blt.source.uright.y =	MPEL_HEIGHT - 1 - y ;
	    blt.source.lleft.y =	MPEL_HEIGHT - ( y += nh ) ;

	    /* do a DestBlt VPM */
	    MPELVPMBLTDest( &blt ) ;
	    /* Make Sure The Transfer is complete */
	    MPELNoOp( 2, &MPELNoOpData ) ;

	    h -= nh ;

	    MPELWaitFifo() ;
	    /* NOW, read the data from the Stage */
	    if ( RowIncrement != w ) {
	        register volatile unsigned char *ptr =
		    (volatile unsigned char *) MPEL_BLIT_STAGE ;
	        while ( nh-- ) {
		    MOVE( ptr, data, w ) ;
		    ptr += w ;
		    data += RowIncrement ;
	        }
	    }
	    else
		{
		MOVE( (volatile unsigned char *) MPEL_BLIT_STAGE, data, w*nh ) ;
		data += nh*RowIncrement;
		}
	}
    } /* end ELSE */

    if ( cursor_saved )
	mpelReplaceCursor() ;

    return ;
}
