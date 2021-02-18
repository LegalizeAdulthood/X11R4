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

/* $Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelCmap.c,v 6.5 88/11/03 22:42:26 paul Exp $ */
/* $Source: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelCmap.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/R3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelCmap.c,v 6.5 88/11/03 22:42:26 paul Exp $" ;
#endif

#include "X.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "colormapst.h"
#include "cursor.h"
#include "resource.h"

#include "OScompiler.h"

#include "ppc.h"

#include "ibmScreen.h"

#include "mpelHdwr.h"

#include "ibmTrace.h"

typedef struct _mpelColorTable
	{
	unsigned short 	start ;
	unsigned short 	count ;
	unsigned int 	rgb[256] ;
	} mpelColorTable, *mpelColorTablePtr ;

#if defined(AIXrt)	/* no comment */
static unsigned long tableptr = 0x1008400 ;
#else
static unsigned long tableptr = mpelAddr(MPEL_COLOR_TABLE) ;
#endif

/***==================================================================***/

/*ARGSUSED*/
void
mpelSetColor( number, red, green, blue, pVisual )
    register unsigned long int number ;
    register unsigned short int red, green, blue ;
    VisualPtr pVisual ;
{
    register mpelColorTable *pmap ;

    TRACE(("mpelSetColor(%d,%d,%d,%d,0x%x)\n",number,red,green,blue,pVisual)) ;

/* we don't get the screen pointer anymore
    if ( ibmScreenState( pVisual->screen ) == SCREEN_ACTIVE ) {
*/
	pmap = (mpelColorTable *) (MPEL_COLOR_TABLE ) ;
	pmap->start = number ;
	pmap->count = 1 ;	/* was 0 */
	pmap->rgb[0] = ( ( red & 0xff00 ) << 8 )
		       | ( green & 0xff00 )
		       | ( ( blue & 0xff00 ) >> 8 ) ;
	mpelicmd( MPELCMD_LOAD_CMAP, &tableptr, 2 ) ;
/*
    }
*/
    return ;
}

/***==================================================================***/

/* MegaPel Colormap Installation
   Does not use PPC because SetColor is so slow.
   For an entire map, should do it this way.  Duplicates PPC function,
   but better keep up to date with PPC if InstallColormap responsibilities
   change   - paquin 10/12/87
 */

void
mpelInstallColormap( pColormap )
    register ColormapPtr pColormap ;
{
    register int i ;
    register mpelColorTablePtr pmap ;
    ScreenPtr pScreen = pColormap->pScreen ;
    ppcScrnPrivPtr devPriv = (ppcScrnPriv *) pScreen->devPrivate ;

    TRACE(("mpelInstallColormap(pColormap=0x%x)\n",pColormap)) ;

    if ( ibmScreenState( pScreen->myNum ) == SCREEN_ACTIVE ) {
	pmap = (mpelColorTable *) (MPEL_COLOR_TABLE) ;
	pmap->start = 0 ;
	pmap->count = pColormap->pVisual->ColormapEntries ;

        for ( i = 0 ; i < pColormap->pVisual->ColormapEntries ; i++ ) {
	    pmap->rgb[i] =
	    ( ( ( pColormap->red[i].co.local.red & 0xFFFF ) << 8 )
	    | ( ( pColormap->red[i].co.local.green & 0xFFFF ) )
	    | ( ( pColormap->red[i].co.local.blue & 0xFFFF ) >> 8 ) ) ;
        }
	mpelicmd( MPELCMD_LOAD_CMAP, &tableptr, 2 ) ;
    }

    devPriv->InstalledColormap = pColormap ;

    WalkTree( pScreen,TellGainedMap,&(pColormap->mid) ) ;

    (* devPriv->RecolorCursor)( pColormap ) ;
    return ;
}
