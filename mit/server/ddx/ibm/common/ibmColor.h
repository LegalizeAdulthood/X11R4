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
/* $Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/common/RCS/ibmColor.h,v 9.3 89/06/09 11:55:46 jeff Exp $ */
/* $Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/common/RCS/ibmColor.h,v $ */

#if !defined(lint) && !defined(LOCORE)  && defined(RCS_HDRS)
static char *rcsidibmColor = "$Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/common/RCS/ibmColor.h,v 9.3 89/06/09 11:55:46 jeff Exp $";
#endif

#if !defined(NO_FUNCTION_PROTOTYPES)

extern void ibmAllocBlackAndWhitePixels(
	ColormapPtr const pCmap);

extern void ibmResolveColorMono(
	unsigned short int *pred,
	unsigned short int *pgreen,
	unsigned short int *pblue,
	VisualPtr pVisual);

extern Bool ibmCreateStaticGrayColormap(
	ColormapPtr const pCmap);

extern void ibmDestroyColormapMono(
	ColormapPtr const pCmap);

extern unsigned long ibmFindColor(
	ColormapPtr cmap,
	unsigned short int r,
	unsigned short int g,
	unsigned short int b);

extern void ibmDefineDefaultColormapColors(
	VisualPtr pVisual,
	ColormapPtr pCmap);

#else

extern void ibmAllocBlackAndWhitePixels() ;
extern void ibmResolveColorMono() ;
extern Bool ibmCreateStaticGrayColormap() ;
extern void ibmDestroyColormapMono() ;
extern unsigned long ibmFindColor() ;
extern void ibmDefineDefaultColormapColors();

#endif

extern char *ibmWhitePixelText ;
extern char *ibmBlackPixelText ;
