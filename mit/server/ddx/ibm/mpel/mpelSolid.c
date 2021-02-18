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

/* $Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelSolid.c,v 1.1 88/11/03 22:42:44 paul Exp $ */
/* $Source: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelSolid.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelSolid.c,v 1.1 88/11/03 22:42:44 paul Exp $" ;
#endif

#define BADREAD

#include "X.h"
#include "Xmd.h"
#include "misc.h"
#include "regionstr.h"
#include "miscstruct.h"

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
 * mpelFillSolid(color,alu,planes,x0,y0,lx,ly)
 */

void
mpelFillSolid( color, alu, mask, x, y, w, h )
unsigned long int color ;
int alu ;
unsigned long int mask ;
int x, y, w, h ;
{
    int cursor_saved ;
    mpelPoint points[4] ;
    mpelRectangle rect ;

    TRACE(("mpelFillSolid(%d,%d,0x%x,%d,%d,%d,%d)\n",color,alu,mask,x,y,w,h)) ;

    if ( w <= 0 || h <= 0 || alu == GXnoop )
	return ;

    /* If Cursor Is In The Way Remove It */
    cursor_saved = !mpelcursorSemaphore && mpelCheckCursor( x, y, w, h ) ;
    mpelSetPlaneMask( mask ) ;
    mpelSetALU( alu ) ;	/* logical operation p. 95 */

    if ( alu == GXcopy ) {
	MPELSetPgonInteriorColor(color) ;
	rect.lleft.x = x ;
	rect.lleft.y = MPEL_HEIGHT - ( y + h ) ;
	rect.uright.x = x + w - 1 ;
	rect.uright.y = ( MPEL_HEIGHT - 1 ) - y ;
	MPELFillRectangle( &rect ) ;
    }
    else if ( w == 1 ) {
	MPELSetLineType( 1 ) ;
	MPELSetPolylineColor( color ) ;
	points[0].x = x ;
	points[0].y = ( MPEL_HEIGHT - 1 ) - y ;
	points[1].x = x ;
	points[1].y = MPEL_HEIGHT - ( y + h ) ;
	MPELPolyline( 2, points ) ;
    }
    else if ( h == 1 ) {
	MPELSetLineType( 1 ) ;
	MPELSetPolylineColor( color ) ;
	points[0].x = x ;
	points[0].y = ( MPEL_HEIGHT - 1 ) - y ;
	points[1].x = x + w - 1 ;
	points[1].y = ( MPEL_HEIGHT - 1 ) - y ;
	MPELPolyline( 2, points ) ;
    }
    else {
	MPELSetPgonInteriorStyle( 2 ) ;
	MPELSetPgonInteriorColor( color ) ;
	MPELSetPgonEdgeFlag( 2 ) ;

	MPELBeginPgon() ;
	points[0].x =	x ;
	points[0].y =	( MPEL_HEIGHT - 1 ) - y ;
	points[1].x =	x + w - 1 ;
	points[1].y =	( MPEL_HEIGHT - 1 ) - y ;
	points[2].x =	x + w - 1 ;
	points[2].y =	MPEL_HEIGHT - ( y + h ) ;
	points[3].x =	x ;
	points[3].y =	MPEL_HEIGHT - ( y + h ) ;
	MPELPolyline( 4, points ) ;
	MPELEndPgon() ;	/* end polygon p. 115 */
    }
    if ( cursor_saved )
	mpelReplaceCursor() ;
    return ;
}
