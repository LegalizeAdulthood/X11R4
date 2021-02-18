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

/* $Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelMono.c,v 6.4 88/11/03 21:38:56 paul Exp $ */
/* $Source: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelMono.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelMono.c,v 6.4 88/11/03 21:38:56 paul Exp $" ;
#endif

/*
 * Megapel bitmap fill with solid foreground and transparent background
 *
 * Paquin 10/87
 *
 */

#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "font.h"
#include "OScompiler.h"

#include "ibmTrace.h"

#include "mpel.h"
#include "mpelHdwr.h"
#include "mpelFifo.h"

/* Cursor Stuff */
extern int mpelcursorSemaphore ;
extern int mpelCheckCursor() ;
extern void mpelReplaceCursor() ;

/***==================================================================***/

void
mpelDrawMonoByteImage( data, x, y, w, h, fg, alu, planes )
    register unsigned char *data ;
    int			x ;
    int			y ;
    int			w ;
    register int	h ;
    unsigned long int	fg ;
    int			alu ;
    unsigned long int	planes ;
{
    register volatile unsigned char *dstPtr ;
    int			i ;
    int			cleanBytes, widthBytes ;
    unsigned char	lastbyteMask ;
    int			PaddedWidthInBits ;
    int cursor_saved ;
    mpelSrcBLTVPMWColorExpansion block ;
#if (GLYPHPADBYTES != 1)
    int 		extrabytesPerScanline ;
#endif

    void mpelUnnaturalMonoByteImage() ; /* Defined Below !! */

    TRACE(("mpelDrawMonoByteImage(0x%x,%d,%d,%d,%d,%d,%d,0x%x)\n",
		data,x,y,w,h,fg,alu,planes)) ;

    if ( w <= 0 || h <= 0 || alu == GXnoop )
	return ;

    widthBytes = ( w + 7 ) / 8 ; /* width of image, not data */
    if ( ( widthBytes * h ) > MPEL_BLIT_STAGE_SIZE ) {
	mpelUnnaturalMonoByteImage(data, x,y,w,h, fg, alu, planes ) ;
	return ;
    }

    /* If Cursor Is In The Way Remove It */
    cursor_saved = !mpelcursorSemaphore && mpelCheckCursor( x, y, w, h ) ;

    mpelSetPlaneMask( planes ) ;

    if ( i = ( w % 8 ) ) {
	lastbyteMask = ( ( ~0 ) << ( 8 - i ) ) ;
	PaddedWidthInBits = ( w & ~7 ) + 8 ;
    }
    else {
	lastbyteMask = ~0 ;
	PaddedWidthInBits = w ;
    }

    cleanBytes = widthBytes - 1 ;

#if (GLYPHPADBYTES != 1)
    extrabytesPerScanline = PADGLYPHWIDTHBYTES( w ) - widthBytes ;
#endif

    /* copy image TO adapter buffer area */
    block.color		= fg ;
    block.srcaddr     	= mpelAddr(MPEL_BLIT_STAGE) ;
    block.dest.lleft.x	= x ;
    block.dest.lleft.y	= MPEL_HEIGHT - ( y + h ) ;
    block.dest.uright.x	= x + PaddedWidthInBits - 1 ;
    block.dest.uright.y	= MPEL_HEIGHT - 1 - y ;
    block.alu		= alu + 1 ;

    /* If the 'SetPlaneMask' has been swallowed we can continue */
    MPELWaitFifo() ;

    
    for ( dstPtr = (volatile unsigned char *) MPEL_BLIT_STAGE ; h-- ; ) {
	    if ( cleanBytes ) {
		MOVE( data, dstPtr, cleanBytes) ;
		data += cleanBytes ;
		dstPtr += cleanBytes ;
	    }
	    /* mask last byte */
	    *dstPtr++ = *data++ & lastbyteMask ; 
#if (GLYPHPADBYTES != 1)
	    data += extrabytesPerScanline ;
#endif
    }

    MPELSrcBLTVPMWColorExpansionROP( &block ) ;

    if ( cursor_saved )
	mpelReplaceCursor() ;

    return ;
}

/***==================================================================***/

void
mpelUnnaturalMonoByteImage( data, x, y, w, h, fg, alu, planes )
    unsigned char	*data ;
    int			x, y, w, h ;
    unsigned long int	fg ;
    int			alu ;
    unsigned long int	planes ;
{
    int 	totwidthBytes ;
    int		linesPerBlock ;
    int		bytes ;
    int		bytesPerBlock ;

    TRACE(("mpelUnnaturalMonoByteImage(0x%x,%d,%d,%d,%d,%d,%d,0x%x)\n",
		data, x, y, w, h, fg, alu, planes ) ) ;

    totwidthBytes = ( ( w + 7 ) / 8 ) ;
    linesPerBlock = MPEL_BLIT_STAGE_SIZE / totwidthBytes ;

    /* the following *should* be impossible */
    if ( ( linesPerBlock * totwidthBytes ) > MPEL_BLIT_STAGE_SIZE ) {
	    ErrorF( "Fatal MonoByte: too big (internal)\n" ) ;
	    return ;
    }

    bytes = totwidthBytes * h ;
    bytesPerBlock = totwidthBytes * linesPerBlock ;

    while ( bytes > 0 ) {
	mpelDrawMonoByteImage( data, x, y, w, linesPerBlock, fg, alu, planes ) ;
	y += linesPerBlock ;
	data += bytesPerBlock ;
	bytes -= bytesPerBlock ;
    }
    return ;
}

/***==================================================================***/

void
mpelDrawMonoImage( data, x, y, w, h, fg, alu, planes )
    unsigned char	*data ;
    int			x, y, w, h ;
    unsigned long int	fg ;
    int			alu ;
    unsigned long int	planes ;
{
    register volatile unsigned char	*dstPtr ;
    int 				i ;
    int 				cleanBytes, PadBytes ;
    int 				totwidthBytes, realwidthBytes ;
    int 				lastbyteMask ;
    int 				PaddedWidthInBits ;
    int					cursor_saved ;
    mpelSrcBLTVPMWColorExpansion 	block ;

    void mpelUnnaturalMonoImage() ; /* Defined Below !! */

    TRACE(("mpelDrawMonoImage(0x%x,%d,%d,%d,%d,%d,%d,0x%x)\n",
	data,x,y,w,h,fg,alu,planes)) ;

    if ( w <= 0 || h <= 0 || alu == GXnoop )
	return ;

    totwidthBytes = ( ( w + 31 ) / 32 ) << 2 ;
    realwidthBytes = ( w + 7 ) / 8 ;
    if ( ( realwidthBytes * h ) > MPEL_BLIT_STAGE_SIZE ) {
	mpelUnnaturalMonoImage( data, x, y, w, h, fg, alu, planes ) ;
	return ;
    }

    /* If Cursor Is In The Way Remove It */
    cursor_saved = !mpelcursorSemaphore && mpelCheckCursor( x, y, w, h ) ;

    mpelSetPlaneMask( planes ) ;
    PadBytes = totwidthBytes - realwidthBytes ;

    if ( i = ( w % 8 ) ) {
	lastbyteMask = ( ( ~0 ) << ( 8 - i ) ) ;
	PaddedWidthInBits = ( w & ~7 ) + 8 ;
    }
    else {
	lastbyteMask = ~0 ;
	PaddedWidthInBits = w ;
    }

    block.color		= fg ;
    block.srcaddr     	= mpelAddr(MPEL_BLIT_STAGE) ;
    block.dest.lleft.x	= x ;
    block.dest.lleft.y	= MPEL_HEIGHT - ( y + h ) ;
    block.dest.uright.x	= x + PaddedWidthInBits - 1 ;
    block.dest.uright.y	= MPEL_HEIGHT - 1 - y ;
    block.alu		= alu + 1 ;

    cleanBytes = realwidthBytes - 1 ;

    /* copy image TO adapter buffer area */
    /* If the 'SetPlaneMask' has been swallowed we can continue */
    MPELWaitFifo() ;
    for ( dstPtr = (volatile unsigned char *) MPEL_BLIT_STAGE ; h-- ; ) {
	    if ( cleanBytes ) {
		MOVE( data, dstPtr, cleanBytes) ;
		data += cleanBytes ;
		dstPtr += cleanBytes ;
	    }
	    /* mask last byte */
	    *dstPtr++ = *data++ & lastbyteMask ; 

	    data += PadBytes ;
	}

    MPELSrcBLTVPMWColorExpansionROP( &block ) ;

    if ( cursor_saved )
	mpelReplaceCursor() ;

    return ;
}

/***==================================================================***/

void
mpelUnnaturalMonoImage( data, x, y, w, h, fg, alu, planes )
    unsigned char	*data ;
    int			x, y, w, h ;
    unsigned long int	fg ;
    int			alu ;
    unsigned long int	planes ;
{
    int totwidthBytes, linesPerBlock, bytes, bytesPerBlock ;

    TRACE( ( "mpelUnnaturalMonoImage(0x%x,%d,%d,%d,%d,%d,%d,0x%x)\n",
	   data, x, y, w, h, fg, alu, planes ) ) ;

    totwidthBytes = ( ( w + 31 ) / 32 ) << 2 ;
    linesPerBlock = MPEL_BLIT_STAGE_SIZE / totwidthBytes ;

    /* the following *should* be impossible */
    if ( ( linesPerBlock * totwidthBytes ) > MPEL_BLIT_STAGE_SIZE ) {
	ErrorF( "Fatal Mono: too big (internal)\n" ) ;
	return ;
    }

    bytes = totwidthBytes * h ;
    bytesPerBlock = totwidthBytes * linesPerBlock ;

    while ( bytes > 0 ) {
	mpelDrawMonoImage(data, x,y, w,linesPerBlock, fg, alu, planes) ;
	y += linesPerBlock ;
	data += bytesPerBlock ;
	bytes -= bytesPerBlock ;
    }
    return ;
}
