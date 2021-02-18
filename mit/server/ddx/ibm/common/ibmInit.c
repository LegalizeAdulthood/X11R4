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
/* $Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/common/RCS/ibmInit.c,v 30.2 89/06/14 18:32:00 jeff Exp $ */
/* $Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/common/RCS/ibmInit.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/common/RCS/ibmInit.c,v 30.2 89/06/14 18:32:00 jeff Exp $" ;
#endif

#include "X.h"
#include "servermd.h"
#include "misc.h"
#include "miscstruct.h"
#include "input.h"
#include "opaque.h"
#include "scrnintstr.h"
#include "cursorstr.h"

#include "ibmScreen.h"

#include "ibmKeybd.h"
#include "ibmMouse.h"

#include "ibmCmdArgs.h"

#include "OSio.h"

#include "ibmTrace.h"

extern void ibmInfoMsg() ;

#define NUMDEVICES 2

/* Command-Line Globals */
int ibmOpenAllScreens = FALSE ;
int ibmScreensWanted = 0 ;

static Bool
ibmFormatExists( screenInfo, newFmt )
ScreenInfo *screenInfo ;
PixmapFormatPtr newFmt ;
{
    PixmapFormatPtr oldFmt ;
    int ndx ;

    TRACE( ("ibmFormatExisits(screenInfo=0x%x,newFmt=0x%x(%d,%d,%d))\n",
	   screenInfo, newFmt, newFmt->depth,
	   newFmt->bitsPerPixel, newFmt->scanlinePad ) ) ;

    for ( ndx = 0 ; ndx < screenInfo->numPixmapFormats ; ndx++ ) {
	oldFmt = &screenInfo->formats[ndx] ;
	if ( ( newFmt->depth == oldFmt->depth )
	  && ( newFmt->bitsPerPixel == oldFmt->bitsPerPixel )
	  && ( newFmt->scanlinePad == oldFmt->scanlinePad ) )
		return TRUE ;
    }
    return FALSE ;
}

/***==================================================================***/

static int
ibmFindSomeScreens()
{
register ibmPerScreenInfo **scrPtr = ibmPossibleScreens ;
register ibmPerScreenInfo *scr ;
extern void OS_GetDefaultScreens() ;

    TRACE(("ibmFindSomeScreens()\n")) ;
    OS_GetDefaultScreens() ;
    if ( !ibmNumScreens ) {
	while ( ( scr = *scrPtr++ ) && scr->ibm_ScreenFlag ) {
	    if ( ( scr->ibm_ScreenFD = (* scr->ibm_ProbeFunc)() ) >= 0 ) {
		ibmScreens[ibmNumScreens++]= scr ;
		if ( !ibmOpenAllScreens )
		    return TRUE ;
	    }
	    else
		scr->ibm_ScreenFD = -1 ;
	}
    }
    else
	return TRUE ;

    return ( ibmNumScreens > 0 ) && ibmOpenAllScreens ;
}

/***==================================================================***/

static void
ibmAddScreens( screenInfo, argc, argv )
ScreenInfo *screenInfo ;
int argc ;
char *argv[] ;
{
    PixmapFormatPtr newFmt, oldFmt ;
    int ndx, fmtNdx ;
    static int been_here ;

    TRACE( ( "ibmAddScreens(screenInfo=0x%x,argc= %d,argv=x%x)\n",
	   screenInfo, argc, argv ) ) ;

    for ( ndx = 0 ; ndx < ibmNumScreens ; ndx++ ) {
	if ( !been_here ) {
	    if ( ibmScreens[ndx]->ibm_Wanted ) {
		ErrorF( "Multiple requests for screen '%s'  -- ignored\n",
			ibmScreenFlag( ndx ) ) ;
		continue ;
	    }
	    ibmScreens[ndx]->ibm_Wanted = TRUE ;
#if !defined(AIXrt) && !defined(IBM_OS_HAS_HFT)
	    if ( ndx > 0 ) {
		ibmScreenMinX( ndx ) += ibmScreenMaxX( ndx - 1 ) ;
		ibmScreenMaxX( ndx ) += ibmScreenMaxX( ndx - 1 ) ;
	    }
#endif /* AIX/HFT */
	}

	for ( fmtNdx = 0 ; fmtNdx < ibmNumFormats( ndx ) ; fmtNdx++ ) {
	    if ( !ibmFormatExists( screenInfo,
				   &ibmScreenFormats( ndx )[ fmtNdx ] ) ) {
		newFmt = &ibmScreenFormats( ndx )[ fmtNdx ] ;
		oldFmt = &screenInfo->formats[ screenInfo->numPixmapFormats++] ;
		oldFmt->depth = newFmt->depth ;
		oldFmt->bitsPerPixel = newFmt->bitsPerPixel ;
		oldFmt->scanlinePad = newFmt->scanlinePad ;
		if ( screenInfo->numPixmapFormats > MAXFORMATS ) {
		    ErrorF( "WSGO!! Too many formats! Exiting\n" ) ;
		    exit( 1 ) ;
		}
	    }
	}
	ibmSetScreenState( ndx, SCREEN_ACTIVE ) ;
	AddScreen( ibmScreenInit( ndx ), argc, argv ) ;
    }
    been_here = TRUE ;
    return ;
}

/***==================================================================***/

static DevicePtr keyboard ;
static DevicePtr mouse ;

void
InitInput()
{
extern DevicePtr OS_MouseProc(), OS_KeybdProc() ;
extern void OS_InitInput() ;
extern void OS_AddAndRegisterOtherDevices() ;

    TRACE( ( "InitInput()\n" ) ) ;

    OS_InitInput() ;
    mouse = AddInputDevice( OS_MouseProc, TRUE ) ;
    keyboard = AddInputDevice( OS_KeybdProc, TRUE ) ;

    RegisterPointerDevice( mouse ) ;
    RegisterKeyboardDevice( keyboard ) ;

    OS_AddAndRegisterOtherDevices() ;
    return ;
}

/***==================================================================***/

extern void ibmPrintBuildDate() ;
extern int ibmAllowBackingStore ;
extern Bool disableBackingStore ;

void
InitOutput( screenInfo, argc, argv )
    ScreenInfo *screenInfo ;
    int argc ;
    char *argv[] ;
{
    static int been_here = 0 ;
    extern void OS_PreScreenInit() ;	/* usually opens /dev/bus */
    extern void OS_PostScreenInit() ;

    TRACE( ( "InitOutput(screenInfo=0x%x)\n", screenInfo ) ) ;

    screenInfo->imageByteOrder = IMAGE_BYTE_ORDER ;
    screenInfo->bitmapScanlineUnit = BITMAP_SCANLINE_UNIT ;
    screenInfo->bitmapScanlinePad = BITMAP_SCANLINE_PAD ;
    screenInfo->bitmapBitOrder = BITMAP_BIT_ORDER ;

    screenInfo->numPixmapFormats = 1 ;
    screenInfo->formats[0].depth = 1 ;
    screenInfo->formats[0].bitsPerPixel = 1 ;
    screenInfo->formats[0].scanlinePad = BITMAP_SCANLINE_PAD ;

    if ( !been_here ) {
	been_here = TRUE ;

	/*
	 * let dix command parsing figure out if we do backing store.
	 */
	ibmAllowBackingStore = !disableBackingStore;

	if ( ibmNumScreens != ibmScreensWanted ) {
	    ErrorF( "Couldn't open all requested screens.\n" ) ;
	    exit( 1 ) ;
	}
	else if ( !ibmNumScreens && !ibmFindSomeScreens() ) {
	    ErrorF( "Couldn't open any screens.\n" ) ;
	    exit( 1 ) ;
	}

	/* Informational Messages */
	ibmPrintBuildDate() ;
	ibmInfoMsg(
  "X Window System protocol version %d, revision %d (vendor release %d)\n",
		X_PROTOCOL, X_PROTOCOL_REVISION, VENDOR_RELEASE ) ;

	OS_PreScreenInit() ;	/* usually opens /dev/bus */
	ibmInfoMsg( ibmUsePCKeys
		    ? "Using PC keyboard layout...\n"
		    : "Using RT keyboard layout...\n" ) ;
    }
    ibmAddScreens( screenInfo, argc, argv ) ;
    OS_PostScreenInit() ;
    return ;
}
