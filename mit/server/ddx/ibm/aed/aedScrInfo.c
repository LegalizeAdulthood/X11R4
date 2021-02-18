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

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/aed/RCS/aedScrInfo.c,v 1.3 89/11/19 20:45:18 jfc Exp $" ;
#endif

#include "X.h"
#include "misc.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursorstr.h"

#include "ibmScreen.h"

extern	Bool	aedScreenInit() ;
extern	Bool	aedProbe() ;
extern	void	aedRemoveCursor() ;
extern	void	aedShowCursor() ;

ibmPerScreenInfo aedScreenInfoStruct = {
	{ 0, 0, 1023, 799 },	/* BoxRec	  ibm_ScreenBounds */
	0,			/* int		  ibm_NumFormats ; */
	(PixmapFormatRec *) 0,	/* PixmapFormatRec	*ibm_ScreenFormats ; */
	aedScreenInit,		/* Bool		(*ibm_InitFunc)() ; */
	aedProbe,		/* int		(*ibm_ProbeFunc)() ; */
	aedRemoveCursor,	/* void		(*ibm_HideCursor)() ; */

	"-aed",			/* char		 *ibm_ScreenFlag ; */
	"/dev/aed",		/* char		 *ibm_ScreenDevice ; */
	"/dev/msaed",		/* char		 *ibm_ScreenPointer ; */

	(void (*)()) 0,		/* void		(*ibm_SaveFunc)() ; */
	(void (*)()) 0,		/* void		(*ibm_RestoreFunc)() ; */

	(ScreenPtr) 0,		/* ScreenPtr	  ibm_Screen */
	-1,			/* int		  ibm_ScreenFD */
	0,			/* int		  ibm_Wanted */

	0,			/* short	  ibm_CursorHotX */
	0,			/* short	  ibm_CursorHotY */
	aedShowCursor,		/* void		(*ibm_CursorShow)() */
	(CursorPtr) 0,		/* CursorPtr	  ibm_CurrentCursor */
	SCREEN_UNINITIALIZED,	/* int		  ibm_ScreenState */
};
