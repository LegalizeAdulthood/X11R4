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

/* $Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/mpel/RCS/mpelIO.c,v 6.4 89/07/24 20:05:28 jeff Exp $ */
/* $Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/mpel/RCS/mpelIO.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/mpel/RCS/mpelIO.c,v 6.4 89/07/24 20:05:28 jeff Exp $" ;
#endif

#include "X.h"
#include "Xproto.h"
#include "screenint.h"
#include "scrnintstr.h"
#include "cursorstr.h"
#include "pixmapstr.h"
#include "miscstruct.h"
#include "colormapst.h"
#include "windowstr.h"
#include "resource.h"
#include "font.h"

#include "OScursor.h"

#include "ibmScreen.h"

#include "ibmTrace.h"

/* Global Variable */
extern ScreenRec mpelScreenRec ;

/***==================================================================***/

/*ARGSUSED*/
Bool
mpelScreenInit( index, pScreen, argc, argv )
register const int index ;
register ScreenPtr const pScreen ;
register int const argc ;		/* these two may NOT be changed */
register char * const * const argv ;
{
	static int been_here = 0 ;

	TRACE( ( "mpelScreenInit(index=%d,pScreen=0x%x,argc=%d,argv=0x%x)\n",
		index, pScreen, argc, argv ) ) ;

	if ( !been_here ) {
	    if ( ibmScreenFD( index ) == -1 ) {
		ErrorF( "mpel file descriptor is invalid\nExiting.\n" ) ;
		exit( 1 ) ;
		/*NOTREACHED*/
	    }
	    mpelLoaduCode() ;
	    been_here = TRUE ;
	}

	ppcCommonScreenInit( pScreen, index, &mpelScreenRec ) ;
	mpelCursorInit( index ) ;
	mpelInitPlaneMask() ;

	return 1 ;
}

/***==================================================================***/

/*ARGSUSED*/
Bool
mpelScreenClose( index, pScreen )
    register int	index ;
    register ScreenPtr	pScreen ;
{
    TRACE( ( "mpelScreenClose(index=%d,pScreen=0x%x)\n", index, pScreen ) ) ;

    return TRUE ;
}
