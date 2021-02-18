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
#ifndef MPEL_PROCS_SEEN
#define	MPEL_PROCS_SEEN	1
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

/* $Header: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelProcs.h,v 6.4 88/11/15 14:17:42 paul Exp $ */
/* $Source: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelProcs.h,v $ */

#ifndef lint
static char *rcsidmpelprocs = "$Header: /andrew/X11/r3src/earlyRelease/server/ddx/ibm/mpel/RCS/mpelProcs.h,v 6.4 88/11/15 14:17:42 paul Exp $";
#endif

#if !defined(NO_FUNCTION_PROTOTYPES)
/* mpelCursor.o */

extern int mpelcursorSemaphore ;

extern int mpelCheckCursor(
	const int x,
	const int y,
	const int lx,
	const int ly ) ;

extern void mpelReplaceCursor( void ) ;

#if !defined(CURSOR_ONLY)
extern int mpelRemoveCursor( void ) ;

extern void mpelShowCursor(
	short int x,
	short int y ) ;

extern void mpelRecolorCursor(
	ColormapPtr cmap ) ;

extern void mpelCursorInit(
	int index ) ;

extern Bool mpelRealizeCursor(
	ScreenPtr pScr,
	CursorPtr pCurs ) ;

extern Bool mpelUnrealizeCursor(
	ScreenPtr pScr,
	CursorPtr pCurs ) ;

extern void mpelColorCursor(
	unsigned long int fg,
	unsigned long int bg ) ;

extern Bool mpelDisplayCursor(
	ScreenPtr pScr,
	CursorPtr pCurs ) ;

extern void mpelRevalidateCursor( void ) ;

/* mpelALU.o */

extern void mpelInitPlaneMask( void ) ;

/* mpelArc.o */

extern void mpelPolyArc(
	DrawablePtr pDraw,
	GCPtr pGC,
	int narcs,
	xArc *parcs ) ;

/* mpelBlit.o */

extern void mpelBitBlt(
	int alu,
	unsigned long int rmask,
	unsigned long int wmask,
	int x0,
	int y0,
	int x1,
	int y1,
	int w,
	int h ) ;

/* mpelCmap.o */

extern void mpelSetColor(
	unsigned long int number,
	unsigned short int red,
	unsigned short int green,
	unsigned short int blue,
	VisualPtr pVisual ) ;

extern void mpelInstallColormap(
	ColormapPtr pColormap ) ;


/* mpelDash.o */

extern void mpel_do_dashline_gc(
	GCPtr pGC ) ;

/* mpelFifoOut.o */

/* PLACE_HOLDER */

/* mpelFont.o */

extern void mpelSaveFonts( void ) ;

extern void mpelRestoreFonts( void ) ;

extern void mpelInitFonts( void ) ;

extern Bool mpelRealizeFont(
	ScreenPtr pScr,
	FontPtr pFont ) ;

extern Bool mpelUnrealizeFont(
	ScreenPtr pScr,
	FontPtr pFont ) ;

/* mpelGC.o */

extern Mask mpelChangeGCtype(
	GC *pGC,
	ppcPrivGCPtr devPriv ) ;

extern Mask mpelChangeWindowGC(
	GC *pGC,
	Mask changes ) ;

extern void mpelDestroyGCPriv(
	pointer pPriv ) ;

/* mpelImg.o */

extern void mpelDrawColorImage(
	int x,
	int y,
	int w,
	int h,
	unsigned char *data,
	int RowIncrement,
	int alu,
	unsigned long int mask ) ;

extern void mpelReadColorImage(
	int x,
	int y,
	int w,
	int h,
	unsigned char *data,
	int RowIncrement ) ;

/* mpelInfo.o */
/* mpelIO.o */

extern Bool mpelScreenInit(
	const int index,
	ScreenPtr const *pScreen,
	const int argc,
	char const *const *argv ) ;

extern Bool mpelScreenClose(
	int index,
	ScreenPtr pScreen ) ;

/* mpelLine.o */

extern void mpelPolySegment(
	DrawablePtr pDraw,
	GCPtr pGC,
	int nseg,
	xSegment *pSegs ) ;

extern void mpelHorzLine(
	unsigned long int color,
	int alu,
	unsigned long int mask,
	int x,
	int y,
	int w ) ;

extern void mpelVertLine(
	unsigned long int color,
	int alu,
	unsigned long int mask,
	int x,
	int y,
	int h ) ;

extern void mpelBresLine(
	unsigned long int color,
	int alu,
	unsigned long int mask,
	int signdx,
	int signdy,
	int axis,
	int x1,
	int y1,
	int e,
	int e1,
	int e2,
	int len ) ;

extern void mpelZeroLine(
	DrawablePtr dst,
	GCPtr pGC,
	int mode,
	int n,
	DDXPointPtr pPoint ) ;

/* mpelMono.o */

extern void mpelDrawMonoByteImage(
	unsigned char *data,
	int x,
	int y,
	int w,
	int h,
	unsigned long int fg,
	int alu,
	unsigned long int planes ) ;

extern void mpelUnnaturalMonoByteImage(
	unsigned char *data,
	int x,
	int y,
	int w,
	int h,
	unsigned long int fg,
	int alu,
	unsigned long int planes ) ;

extern void mpelDrawMonoImage(
	unsigned char *data,
	int x,
	int y,
	int w,
	int h,
	unsigned long int fg,
	int alu,
	unsigned long int planes ) ;

extern void mpelUnnaturalMonoImage(
	unsigned char *data,
	int x,
	int y,
	int w,
	int h,
	unsigned long int fg,
	int alu,
	unsigned long int planes ) ;

/* mpelOSD.o */

extern int mpelProbe( void ) ;

/* mpelPlane.o */

extern unsigned long int *mpelGetPlane(
	int planeNum,
	int sx,
	int sy,
	int w,
	int h,
	unsigned long int *result ) ;

/* mpelPoly.o */

extern void mpelFillPolygon(
	DrawablePtr pDraw,
	GCPtr pGC,
	int shape,
	int mode,
	int count,
	DDXPointPtr pPts ) ;

extern void mpelTilePolygon(
	DrawablePtr pDraw,
	GCPtr pGC,
	int shape,
	int mode,
	int count,
	DDXPointPtr pPts ) ;

/* mpelPolyPt.o */

extern void mpelPolyPoint(
	DrawablePtr pDrawable,
	GCPtr pGC,
	int mode,
	int npt,
	xPoint *pptInit ) ;

/* mpelSave.o */

extern void mpelSaveState( void ) ;

extern void mpelRestoreState( void ) ;

/* mpelSolid.o */

extern void mpelFillSolid(
	unsigned long int color,
	int alu,
	unsigned long int mask,
	int x,
	int y,
	int w,
	int h ) ;

/* mpelText.o */

extern int mpelImageText(
	DrawablePtr pDraw,
	GCPtr pGC,
	int x,
	int y,
	int count,
	char *chars,
	FontEncoding fontEncoding ) ;

extern int mpelPolyText(
	DrawablePtr pDraw,
	GCPtr pGC,
	int x,
	int y,
	int count,
	char *chars,
	FontEncoding fontEncoding ) ;

extern int mpelPolyText8(
	DrawablePtr pDraw,
	GCPtr pGC,
	int x,
	int y,
	int count,
	char *chars ) ;

extern int mpelPolyText16(
	DrawablePtr pDraw,
	GCPtr pGC,
	int x,
	int y,
	int count,
	char *chars ) ;

extern void mpelImageText8(
	DrawablePtr pDraw,
	GCPtr pGC,
	int x,
	int y,
	int count,
	char *chars ) ;

extern void mpelImageText16(
	DrawablePtr pDraw,
	GCPtr pGC,
	int x,
	int y,
	int count,
	char *chars ) ;

/* mpelTile.o */

extern void mpelTile(
	PixmapPtr pTile,
	int alu,
	unsigned long int planemask,
	int x,
	int y,
	int w,
	int h,
	int xSrc,
	int ySrc ) ;

/* mpeluCode.o */

extern void mpelLoad(
	char *file,
	volatile unsigned short int *addr ) ;

extern void mpelLoaduCode( void ) ;

/* mpelUtil.o */

extern void mpelicmd2(
	unsigned short int command_code,
	unsigned short int parameters[],
	int parm_count ) ;

extern void mpelicmd(
	unsigned short int command_code,
	unsigned short int parameters[],
	int parm_count ) ;

extern void mpelPoke( void ) ;

extern void mpelInitPatterns( void ) ;

extern int mpelRectIntersect(
	BoxPtr prect1,
	BoxPtr prect2 ) ;

/* mpelXmorg.o */

extern void mpelTransmogrify(
	int w,
	int h,
	unsigned char *src,
	unsigned short int *dst ) ;

/* mpelData.o */

#endif /* ~ CURSOR_ONLY */
#else /* ~ NO_FUNCTION_PROTOTYPES */
/* mpelCursor.o */
extern int mpelcursorSemaphore ;
extern int mpelCheckCursor() ;
extern void mpelReplaceCursor() ;
#if !defined(CURSOR_ONLY)
extern int mpelRemoveCursor() ;
extern void mpelShowCursor() ;
extern void mpelRecolorCursor() ;
extern void mpelCursorInit() ;
extern Bool mpelRealizeCursor() ;
extern Bool mpelUnrealizeCursor() ;
extern void mpelColorCursor() ;
extern Bool mpelDisplayCursor() ;
extern void mpelRevalidateCursor() ;
/* mpelALU.o */
extern void mpelInitPlaneMask() ;
/* mpelArc.o */
extern void mpelPolyArc() ;
/* mpelBlit.o */
extern void mpelBitBlt() ;
/* mpelCmap.o */
extern void mpelSetColor() ;
extern void mpelInstallColormap() ;
/* mpelDash.o */
extern void mpel_do_dashline_gc() ;
/* mpelFifoOut.o */
/* PLACE_HOLDER */
/* mpelFont.o */
extern void mpelSaveFonts() ;
extern void mpelRestoreFonts() ;
extern void mpelInitFonts() ;
extern Bool mpelRealizeFont() ;
extern Bool mpelUnrealizeFont() ;
/* mpelGC.o */
extern Mask mpelChangeGCtype() ;
extern Mask mpelChangeWindowGC() ;
extern void mpelDestroyGCPriv() ;
/* mpelImg.o */
extern void mpelDrawColorImage() ;
extern void mpelReadColorImage() ;
/* mpelInfo.o */
/* mpelIO.o */
extern Bool mpelScreenInit() ;
extern Bool mpelScreenClose() ;
/* mpelLine.o */
extern void mpelPolySegment() ;
extern void mpelHorzLine() ;
extern void mpelVertLine() ;
extern void mpelBresLine() ;
extern void mpelZeroLine() ;
/* mpelMono.o */
extern void mpelDrawMonoByteImage() ;
extern void mpelUnnaturalMonoByteImage() ;
extern void mpelDrawMonoImage() ;
extern void mpelUnnaturalMonoImage() ;
/* mpelOSD.o */
extern int mpelProbe() ;
/* mpelPlane.o */
extern unsigned long int *mpelGetPlane() ;
/* mpelPoly.o */
extern void mpelFillPolygon() ;
extern void mpelTilePolygon() ;
/* mpelPolyPt.o */
extern void mpelPolyPoint() ;
/* mpelSave.o */
extern void mpelSaveState() ;
extern void mpelRestoreState() ;
/* mpelSolid.o */
extern void mpelFillSolid() ;
/* mpelText.o */
extern int mpelImageText() ;
extern int mpelPolyText() ;
extern int mpelPolyText8() ;
extern int mpelPolyText16() ;
extern void mpelImageText8() ;
extern void mpelImageText16() ;
/* mpelTile.o */
extern void mpelTile() ;
/* mpeluCode.o */
extern void mpelLoad() ;
extern void mpelLoaduCode() ;
/* mpelUtil.o */
extern void mpelicmd2() ;
extern void mpelicmd() ;
extern void mpelPoke() ;
extern void mpelInitPatterns() ;
extern int mpelRectIntersect() ;
/* mpelXmorg.o */
extern void mpelTransmogrify() ;
/* mpelData.o */
#endif /* ~ CURSOR_ONLY */
#endif /* NO_FUNCTION_PROTOTYPES */

#endif /* MPEL_PROCS_SEEN */
