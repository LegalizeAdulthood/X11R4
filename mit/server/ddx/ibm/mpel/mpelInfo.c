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

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/release/server/ddx/ibm/mpel/RCS/mpelInfo.c,v 1.4 89/04/29 21:34:37 jeff Exp $" ;
#endif

#include "X.h"
#include "misc.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursorstr.h"

#include "ibmScreen.h"

#include "mpel.h"

extern Bool mpelScreenInit() ;
extern Bool mpelProbe() ;
extern void mpelRemoveCursor() ;
extern void mpelShowCursor() ;
extern PixmapFormatRec mpelFormats[1] ;
extern void  mpelSaveState() ;
extern void  mpelRestoreState() ; 

ibmPerScreenInfo mpelScreenInfoStruct = {
	{ 0, 0, MPEL_WIDTH, MPEL_HEIGHT }, /* BoxRec	  ibm_ScreenBounds */
	1,			/* int		 ibm_NumFormats ; */
	mpelFormats,		/* PixmapFormatRec	*ibm_ScreenFormats ; */
	mpelScreenInit,		/* Bool		(*ibm_InitFunc)() ; */
	mpelProbe,		/* int		(*ibm_ProbeFunc)() ; */
	mpelRemoveCursor,	/* void		(*ibm_HideCursor)() ; */
	"-mpel",		/* char		 *ibm_ScreenFlag ; */
	"/dev/mpel",		/* char		 *ibm_ScreenDevice ; */
	"/dev/msmpel",		/* char		 *ibm_ScreenPointer ; */
	mpelSaveState,		/* void		(*ibm_SaveFunc)() ; */
	mpelRestoreState,	/* void		(*ibm_RestoreFunc)() ; */
	(ScreenPtr) 0,		/* ScreenPtr	  ibm_Screen */
	-1,			/* int		  ibm_ScreenFD */
	0,			/* int		  ibm_Wanted */
	0,			/* short	  ibm_CursorHotX */
	0,			/* short	  ibm_CursorHotY */
	mpelShowCursor,		/* void		(*ibm_CursorShow)() */
	(CursorPtr) 0,		/* CursorPtr	  ibm_CurrentCursor */
	SCREEN_UNINITIALIZED	/* int		  ibm_ScreenState */
} ;
