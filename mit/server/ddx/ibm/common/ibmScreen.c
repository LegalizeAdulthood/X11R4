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

/* $Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/common/RCS/ibmScreen.c,v 9.3 89/06/16 11:06:41 jeff Exp $ */
/* $Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/common/RCS/ibmScreen.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/common/RCS/ibmScreen.c,v 9.3 89/06/16 11:06:41 jeff Exp $";
#endif

#include "X.h"
#include "misc.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursorstr.h"

#include "ibmScreen.h"

/* SERVER GLOBALS */
ibmPerScreenInfo *ibmScreens[MAXSCREENS];
int		 ibmNumScreens=		0;
int		 ibmXWrapScreen= 	FALSE;
int		 ibmYWrapScreen= 	FALSE;
int		 ibmCurrentScreen= 	0;
int		 ibmUseHardware=	1;
int		 ibmAllowBackingStore=	1;

#ifdef IBM_VGA
extern ibmPerScreenInfo vgaScreenInfoStruct ;
#endif /* IBM_VGA */
#ifdef IBM_8514
extern ibmPerScreenInfo ibm8514ScreenInfoStruct ;
#endif /* IBM_8514 */
#ifdef IBM_MPEL
extern ibmPerScreenInfo mpelScreenInfoStruct ;
#endif /* IBM_MPEL */
#ifdef IBM_EGA
extern ibmPerScreenInfo egaScreenInfoStruct ;
#endif /* IBM_EGA */
#ifdef IBM_APA8c
extern ibmPerScreenInfo apa8cScreenInfoStruct ;
#endif /* IBM_APA8c */
#ifdef IBM_APA16
extern ibmPerScreenInfo apa16ScreenInfoStruct ;
#endif /* IBM_APA16 */
#ifdef IBM_AED
extern ibmPerScreenInfo aedScreenInfoStruct ;
#endif /* IBM_AED */
#ifdef IBM_PLX
extern ibmPerScreenInfo plxScreenInfoStruct ;
#endif /* IBM_PLX */

ibmPerScreenInfo *ibmPossibleScreens[] = {
#ifdef IBM_8514
	&ibm8514ScreenInfoStruct,
#endif /* IBM_8514 */
#ifdef IBM_VGA
	&vgaScreenInfoStruct,
#endif /* IBM_VGA */
#ifdef IBM_MPEL
	&mpelScreenInfoStruct,
#endif /* IBM_MPEL */
#ifdef IBM_EGA
	&egaScreenInfoStruct,
#endif /* IBM_EGA */
#ifdef IBM_APA8c
	&apa8cScreenInfoStruct,
#endif /* IBM_APA8c */
#ifdef IBM_APA16
	&apa16ScreenInfoStruct,
#endif /* IBM_APA16 */
#ifdef IBM_AED
	&aedScreenInfoStruct,
#endif /* IBM_AED */
#ifdef IBM_PLX
	&plxScreenInfoStruct,
#endif /* IBM_PLX */
	0
} ;
