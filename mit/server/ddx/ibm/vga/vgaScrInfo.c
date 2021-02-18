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
#include "misc.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursorstr.h"

#include "ibmScreen.h"
#include "vgaVideo.h"

extern	Bool	vgaScreenInit();
extern	Bool	vgaProbe();
extern	void	vgaRemoveCursor();
extern	void	vgaShowCursor();
extern	void	vgaSuspendScreenAndSave() ;
extern	void	vgaRestoreScreenAndActivate() ;
extern	PixmapFormatRec  vgaFormats[];

ibmPerScreenInfo vgaScreenInfoStruct = {
	{ 0, 0, 639, 479 },	/* BoxRec	 ibm_ScreenBounds; */
	1,			/* int		 ibm_NumFormats; */
	vgaFormats,		/* PixmapFormatRec	*ibm_ScreenFormats; */
	vgaScreenInit,		/* Bool		(*ibm_InitFunc)(); */
	vgaProbe,		/* int		(*ibm_ProbeFunc)(); */
	vgaRemoveCursor,	/* void		(*ibm_HideCursor)(); */
	"-vga",			/* char		 *ibm_ScreenFlag; */
	"",			/* char		 *ibm_ScreenDevice; */
	"/dev/msvga",			/* char		 *ibm_ScreenPointer; */
	vgaSuspendScreenAndSave,/* void		(*ibm_SaveFunc)(); */
	vgaRestoreScreenAndActivate,/* void	(*ibm_RestoreFunc)(); */
	(ScreenPtr) 0,		/* ScreenPtr	  ibm_Screen */
	-1,			/* int		  ibm_ScreenFD */
	0,			/* int		  ibm_Wanted */
	0,			/* short	  ibm_CursorHotX */
	0,			/* short	  ibm_CursorHotY */
	vgaShowCursor,		/* void		 (*ibm_CursorShow)() */
	(CursorPtr) 0,		/* CursorPtr	  ibm_CurrentCursor */
	SCREEN_UNINITIALIZED	/* int		  ibm_ScreenState */
} ;
