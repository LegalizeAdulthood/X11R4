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
#include "X.h"
#include "resource.h"
#include "scrnintstr.h"

#include "OScompiler.h"

#include "vgaVideo.h"

#include "ibmTrace.h"

/* Global Variables */
extern int vgaDisplayTubeType ;
extern ScreenRec vgaScreenRec ;
extern VisualRec vgaVisuals[] ;

static int HardwareReady = 0 ;

Bool
vgaScreenInit( index, pScreen, argc, argv )
register const int index ;
register ScreenPtr const pScreen ;
register int const argc ;		/* these two may NOT be changed */
register char * const * const argv ;
{
	static int been_here = 0 ;

	TRACE( ( "vgaScreenInit(index=%d,pScreen=0x%x,argc=%d,argv=0x%x)\n",
		index, pScreen, argc, argv ) ) ;

	if ( !been_here ) {
/*		vgaInitFontCache() ;	MAYBE SOMEDAY */
		been_here = TRUE ;
	}
	if ( !HardwareReady )
		HardwareReady = vgaScreenInitHW( index ) ;

	vgaVisuals[0].class = ( vgaDisplayTubeType == COLOR_TUBE )
			    ? PseudoColor : GrayScale ;
	ppcCommonScreenInit( pScreen, index, &vgaScreenRec ) ;
	vgaCursorInit( index ) ;

	return HardwareReady ;
}

/*ARGSUSED*/
Bool
vgaScreenClose( index, pScreen )
register const int index ;
register ScreenPtr const pScreen ;
{
TRACE( ( "vgaScreenClose(index=%d,pScreen=0x%x)\n", index, pScreen ) ) ;
ppcCommonScreenClose( pScreen, index, &vgaScreenRec ) ;
if ( HardwareReady ) {
	vgaCloseHW( index ) ;
	HardwareReady = 0 ;
}

return 1 ;
}
