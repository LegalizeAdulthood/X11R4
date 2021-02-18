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

#include "X.h"
#include "windowstr.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "cursor.h"
#include "gcstruct.h"

#include "OScompiler.h"

#include "ibmScreen.h"

#include "ppcGCstr.h"
#include "ppcScrn.h"

#include "vgaVideo.h"
#include "vgaSave.h"

#define CURSOR_ONLY
#include "vgaProcs.h"

#if defined(IBM_OS_HAS_HFT) && !defined(IBM_OS_HAS_X_QUEUE)
#define HFT_HACKS "Oh no. The hack is here!"
#endif

#if !defined(HFT_HACKS)
extern WindowPtr ObscureEntireScreen() ;
#else
extern void ibmSaveScreenData() ;
extern void ibmRestoreScreenData() ;
extern void ppcSaveAreas() ;
extern void ppcRestoreAreas() ;
#endif

/* ************************************************************************** */

/* Data from vgaScrInfo.c */
extern ibmPerScreenInfo vgaScreenInfoStruct ;

/* Data from vgaData.c */
extern ppcScrnPriv vgaScrnPriv ;

/* Procedures from vgaOSD.c */
extern int vgaScreenInitHW() ;
extern int vgaCloseHW() ;

/* Procedures from vgaCurs.c */
extern void vgaRemoveCursor() ;
extern void vgaShowCursor() ;

/* Global Available For Anyone */
int vgaDisabled = 0 ; /* But only used in vgaOSD.c ! */

/* ************************************************************************** */

/* Private Saved State Variables */
static int CursorWasActive = 0 ;
static ppcScrnPriv savedvgaScrnPriv ;
#if !defined(HFT_HACKS)
static WindowPtr coveringWindow = (WindowPtr) 0 ;
#endif

static int
DoNaught()
{
return 0 ;
}

#if !defined(HFT_HACKS)
void
vgaSuspendScreenAndSave( index )
int index ;
#else /* erik's hft hack */
void
vgaSuspendScreenAndSave( pScreen, saveBits )
ScreenPtr pScreen ;
int saveBits ;
#endif
{
if ( vgaScreenInfoStruct.ibm_ScreenState == SCREEN_ACTIVE ) {
	/* Now Disable any further screen writes
	 * by preventing call though the screen-priv
	 */
	savedvgaScrnPriv = vgaScrnPriv ;
	CursorWasActive = !vgaCursorSemaphore
			 && vgaCheckCursor( 0, 0, MAX_ROW, MAX_COLUMN ) ;
	vgaCursorSemaphore++ ;

	/* Fields in the ibmPerScreenInfo Structure */
	vgaScreenInfoStruct.ibm_HideCursor = (void (*)()) DoNaught ;
	vgaScreenInfoStruct.ibm_CursorShow = (void (*)()) DoNaught ;

#if !defined(HFT_HACKS)
	/* Obscure The Screen */
	coveringWindow = ObscureEntireScreen( vgaScreenInfoStruct.ibm_Screen ) ;
#else /* erik's hft hack */
	if ( saveBits )
		ibmSaveScreenData( pScreen, ppcSaveAreas ) ;
#endif

	/* Fields in the ppcScrnPriv Structure */
	vgaScrnPriv.blit = (void (*)()) DoNaught ;
	vgaScrnPriv.solidFill = (void (*)()) DoNaught ;
	vgaScrnPriv.tileFill = (void (*)()) DoNaught ;
	vgaScrnPriv.stipFill = (void (*)()) DoNaught ;
	vgaScrnPriv.opstipFill = (void (*)()) DoNaught ;
	vgaScrnPriv.imageFill = (void (*)()) DoNaught ;
	vgaScrnPriv.imageRead = (void (*)()) DoNaught ;
	vgaScrnPriv.lineBres = (void (*)()) DoNaught ;
	vgaScrnPriv.lineHorz = (void (*)()) DoNaught ;
	vgaScrnPriv.lineVert = (void (*)()) DoNaught ;
	vgaScrnPriv.setColor = (void (*)()) DoNaught ;
	vgaScrnPriv.RecolorCursor = (void (*)()) DoNaught ;
	vgaScrnPriv.monoFill = (void (*)()) DoNaught ;
	vgaScrnPriv.glyphFill = (void (*)()) DoNaught ;
	vgaScrnPriv.planeRead = (unsigned long int *(*)()) DoNaught ;
	vgaScrnPriv.replicateArea = (void (*)()) DoNaught ;
	vgaScrnPriv.DestroyGCPriv = (void (*)()) DoNaught ;
	vgaScrnPriv.CheckCursor = (int (*)()) DoNaught ;
	vgaScrnPriv.ReplaceCursor = (void (*)()) DoNaught ;

	vgaDisabled = PhysicalVgaAccessDisabled ;
	vgaScreenInfoStruct.ibm_ScreenState = SCREEN_INACTIVE ;
}
else if ( vgaScreenInfoStruct.ibm_ScreenState == SCREEN_INACTIVE ) {
	ErrorF( "vgaSuspendScreenAndSave: vga State Is Already Inactive!" ) ;
}
else if ( vgaScreenInfoStruct.ibm_ScreenState == SCREEN_UNINITIALIZED ) {
	ErrorF( "vgaSuspendScreenAndSave: vga Still uninitialized!" ) ;
}

return ;
}

#if !defined(HFT_HACKS)
void
vgaRestoreScreenAndActivate( index )
int index ;
#else /* erik's hft hack */
void
vgaRestoreScreenAndActivate( pScreen, restoreBits )
ScreenPtr pScreen ;
int restoreBits ;
#endif
{
ColormapPtr currentColormap ;

if ( vgaScreenInfoStruct.ibm_ScreenState == SCREEN_INACTIVE ) {

	/* Re-enable Cursor Fields in the ibmPerScreenInfo Structure */
	vgaScreenInfoStruct.ibm_HideCursor = vgaRemoveCursor ;
	vgaScreenInfoStruct.ibm_CursorShow = vgaShowCursor ;

	/* Now Re-Enable any screen writes by restoring the screen-priv */
	currentColormap = vgaScrnPriv.InstalledColormap ;
	vgaScrnPriv = savedvgaScrnPriv ;

	if ( !( vgaDisabled &= ~ ( PhysicalVgaAccessDisabled
				 | VirtualVgaHardwareOpenPending ) ) ) {
#if !defined(HFT_HACKS)
		vgaScreenInitHW( index ) ; /* Magic Hardware Initialization */
		vgaScreenInfoStruct.ibm_ScreenState = SCREEN_ACTIVE ;
		/* Now We Are Free to Draw Again !! */
		FreeResource( coveringWindow->drawable.id, RT_NONE ) ;
		coveringWindow = (WindowPtr) 0 ;
#else /* erik's hft hack */
		/* Magic Hardware Initialization */
		vgaScreenInitHW( pScreen->myNum ) ;
		vgaScreenInfoStruct.ibm_ScreenState = SCREEN_ACTIVE ;
		/* Now We Are Free to Draw Again !! */
		if ( restoreBits )
			ibmRestoreScreenData( pScreen, ppcRestoreAreas ) ;
#endif
		/* Now Reinstall the current Colormap !! */
		ppcInstallColormap( currentColormap ) ;

		if ( !--vgaCursorSemaphore && CursorWasActive )
			vgaReplaceCursor() ;
	}
	else { /* Close Request is pending */
		/* vgaDisabled == VirtualVgaHardwareClosePending */
		vgaDisabled = 0 ;
		/* Magic Hardware Shutdown Code */
#if !defined(HFT_HACKS)
		vgaCloseHW( index ) ;
#else /* erik's hft hack */
		vgaCloseHW( pScreen->myNum ) ;
#endif
	}
}
else if ( vgaScreenInfoStruct.ibm_ScreenState == SCREEN_ACTIVE ) {
	ErrorF( "vgaRestoreScreenAndActivate: vga State Is Still Active!\n" ) ;
	return ;
}
else if ( vgaScreenInfoStruct.ibm_ScreenState == SCREEN_UNINITIALIZED ) {
	ErrorF( "vgaRestoreScreenAndActivate: vga Still uninitialized!" ) ;
}

return ;
}
