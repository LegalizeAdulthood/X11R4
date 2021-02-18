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
/*
 * ppcFont.c - ppc font related routines
 */

#include "X.h"
#include "Xmd.h"
#include "Xproto.h"

#include "fontstruct.h"
#include "dixfontstr.h"
#include "scrnintstr.h"

#include "ppcCache.h"

Bool
ppcRealizeFont( pScreen, pFont )
	ScreenPtr pScreen;
	FontPtr pFont;
	{
	int		index = pScreen->myNum;
	FontInfoPtr	pfi = pFont->pFI;
	CharInfoPtr	minb = &pfi->minbounds;
	ppcBMInfoPtr	*bmtable;
	int		numGlyphs;

	ppcCacheInit(pScreen);

	if ( ( minb->metrics.descent + minb->metrics.ascent ) > 32 )
		{	/* this font has no cacheable characters */
		return(mfbRealizeFont( pScreen, pFont ));
		}

	numGlyphs = ( pfi->lastCol - pfi->firstCol + 1 ) *
			( pfi->lastRow - pfi->firstRow + 1 );

	
	if ( ( bmtable = (ppcBMInfoPtr *)Xalloc(sizeof(ppcBMInfoPtr)*numGlyphs ) ) == NULL )
		{
		return(FALSE);
		}

	/* initialize to null */
	bzero(bmtable, sizeof(ppcBMInfoPtr)*numGlyphs);

	pFont->devPriv[index] = (pointer)bmtable;

	return(TRUE);
	}

Bool
ppcUnrealizeFont( pScreen, pFont )
	ScreenPtr pScreen;
	FontPtr pFont;
	{
	int		index = pScreen->myNum;

	if ( (int)pFont->devPriv[index] > 20 )
		Xfree(pFont->devPriv[index]);
	return(TRUE);
	}

