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

#ifndef lint
static char *rcsid = "$Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Info.c,v 1.2 89/11/12 00:05:52 probe Exp $";
static char sccsid[] = "@(#)apa16Info.c	3.1 88/09/22 09:30:15";
#endif

#include "X.h"
#include "misc.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursorstr.h"

#include "ibmScreen.h"

extern	Bool	apa16ScreenInit();
extern	Bool	apa16Probe();
extern	void	apa16RemoveCursor();
extern	void	apa16ShowCursor();
extern	void	apa16SaveState();
extern	void	apa16RestoreState();

ibmPerScreenInfo apa16ScreenInfoStruct = {
	{ 0, 0, 1023, 767 },	/* BoxRec	  ibm_ScreenBounds */
	0,			/* int		 ibm_NumFormats ; */
	(PixmapFormatRec *) 0,	/* PixmapFormatRec	*ibm_ScreenFormats ; */
	apa16ScreenInit,	/* Bool		(*ibm_InitFunc)() ; */
	apa16Probe,		/* int		(*ibm_ProbeFunc)() ; */
	apa16RemoveCursor,	/* void		(*ibm_HideCursor)() ; */
	"-apa16",		/* char		 *ibm_ScreenFlag ; */
	"/dev/apa16",		/* char		 *ibm_ScreenDevice ; */
	"/dev/msapa16",		/* char		 *ibm_ScreenPointer ; */
	apa16SaveState,		/* void		(*ibm_SaveFunc)() ; */
	apa16RestoreState,	/* void	(*ibm_RestoreFunc)() ; */
	(ScreenPtr) 0,		/* ScreenPtr	  ibm_Screen */
	-1,			/* int		  ibm_ScreenFD */
	0,			/* int		  ibm_Wanted */
	0,			/* short	  ibm_CursorHotX */
	0,			/* short	  ibm_CursorHotY */
	apa16ShowCursor,	/* void		(*ibm_CursorShow)() */
	(CursorPtr) 0,		/* CursorPtr	  ibm_CurrentCursor */
	SCREEN_UNINITIALIZED	/* int		  ibm_ScreenState */
} ;
