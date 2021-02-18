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
 * PRPQ 5799-PFF (C) COPYRIGHT IBM CORPORATION 1987
 * LICENSED MATERIALS - PROPERTY OF IBM
 * REFER TO COPYRIGHT INSTRUCTIONS FORM NUMBER G120-2083
 */
/* $Header: /andrew/X11/r3src/release/server/ddx/ibm/vga/RCS/vgaProcs.h,v 6.4 89/04/29 21:41:54 jeff Exp $ */
/* $Source: /andrew/X11/r3src/release/server/ddx/ibm/vga/RCS/vgaProcs.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
/* $Header: /andrew/X11/r3src/release/server/ddx/ibm/vga/RCS/vgaProcs.h,v 6.4 89/04/29 21:41:54 jeff Exp $ */
#endif

#if !defined(NO_FUNCTION_PROTOTYPES)
/* Cursor Stuff */
/* vgaCurs.o */
extern int vgaCheckCursor(
	const int x,
	const int y,
	const int lx,
	const int ly ) ;

extern void vgaReplaceCursor(
	void ) ;

extern void vgaRemoveCursor(
	void ) ;

extern int vgaCursorSemaphore ;

#if !defined(CURSOR_ONLY)
extern void vgaShowCursor(
	const int x_loc,
	const int y_loc ) ;

extern void vgaCursorInit(
	int index ) ;

extern void vgaColorCursor(
	unsigned long int fg,
	unsigned long int bg ) ;

extern void vgaRecolorCursor(
	ColormapPtr cmap ) ;

extern int vgaDisplayCursor(
	const ScreenPtr pScr,
	const CursorPtr pCurs ) ;

/* vgaHWCntl.o */
extern void save_vga_state(
	struct vga_video_hardware_state *VS ) ;

extern void restore_vga_state(
	struct vga_video_hardware_state const *VS ) ;

extern void vgaSetColor(
	unsigned long int color,
	unsigned short int r,
	unsigned short int g,
	unsigned short int b ) ;

extern void set_graphics_mode(
	struct vga_video_hardware_state *VS ) ;

extern Bool vgaScreenClose(
	const int index,
	const ScreenPtr pScreen ) ;

extern Bool vgaScreenInit(
	const int index,
	const ScreenPtr pScreen,
	const int argc,
	char * const * const argv ) ;

/* vgaOSD.o */
extern int vgaCheckDisplay(
	int fd ) ;

extern int vgaProbe(
	void ) ;

extern int vgaScreenInitHW(
	int index ) ;

extern void vgaCloseHW(
	int index ) ;

/* vgaSuspScr.o */
extern void vgaSuspendScreenAndSave(
	void ) ;

extern void vgaRestoreScreenAndActivate(
	void ) ;

/*
 * Everything Below is "ppc" entry points
 */
/* vgaGC.o */
extern Mask vgaChangeGCtype(
	GC *pGC,
	ppcPrivGCPtr devPriv ) ;

extern Mask vgaChangeWindowGC(
	GC *pGC,
	Mask changes ) ;

/* vgaBitBlt.o */
extern void vgaBitBlt(
	const int alu,
	const int readplanes,
	const int writeplanes,
	int x0,
	int y0,
	int x1,
	int y1,
	int w,
	int h ) ;

/* vgaGlyph.o */
extern void vgaDrawGlyph(
	const unsigned char *data,
	int x,
	int y,
	int w,
	int h,
	unsigned long int fg,
	const int alu,
	const unsigned long int planes ) ;

/* vgaImages.o */
extern void vgaDrawColorImage(
	int x,
	int y,
	int w,
	int h,
	const unsigned char *data,
	int stride,
	const int alu,
	const unsigned int planes ) ;

extern void vgaReadColorImage(
	int x,
	int y,
	int lx,
	int ly,
	unsigned char *data,
	int stride ) ;

/* vgaLine.o */
extern void vgaHorzLine(
	unsigned long int color,
	int alu,
	unsigned long int mask,
	int x,
	int y,
	int w ) ;

extern void vgaVertLine(
	unsigned long int color,
	int alu,
	unsigned long int mask,
	int x,
	int y,
	int h ) ;

extern void vgaBresLine(
	unsigned long int color,
	int alu,
	unsigned long int planes,
	int signdx,
	int signdy,
	int axis,
	int x,
	int y,
	int et,
	int e1,
	int e2,
	int len ) ;

/* vgaStipple.o */
extern void vgaDrawMonoImage(
	unsigned char *data,
	int x,
	int y,
	int w,
	int h,
	unsigned long int fg,
	int alu,
	unsigned long int planes ) ;

extern void vgaFillStipple(
	const PixmapPtr pStipple,
	unsigned long int fg,
	const int alu,
	unsigned long int planes,
	int x,
	int y,
	int w,
	int h,
	const int xSrc,
	const int ySrc ) ;

/* vgaSolid.o */
extern void vgaFillSolid(
	unsigned long int color,
	const int alu,
	unsigned long int planes,
	int x0,
	const int y0,
	int lx,
	const int ly ) ;
#endif /* CURSOR_ONLY */
#else /* defined(NO_FUNCTION_PROTOTYPES) Brain-Damaged */
/* Cursor Stuff */
/* vgaCurs.o */
extern int vgaCheckCursor() ;
extern void vgaReplaceCursor() ;
extern int vgaCursorSemaphore ;
#if !defined(CURSOR_ONLY)
extern void vgaRemoveCursor() ;
extern void vgaShowCursor() ;
extern void vgaCursorInit() ;
extern void vgaColorCursor() ;
extern void vgaRecolorCursor() ;
extern int vgaDisplayCursor() ;

extern void save_vga_state() ;
extern void restore_vga_state() ;
extern void vgaSetColor() ;
extern void save_dac() ;
extern void restore_dac() ;
extern void set_graphics_mode() ;
extern Bool vgaScreenClose() ;
extern Bool vgaScreenInit() ;
/* vgaOSD.o */
extern int vgaCheckDisplay() ;
extern int vgaProbe() ;
extern int vgaScreenInitHW() ;
extern void vgaCloseHW() ;
extern void vgaSuspendScreenAndSave() ;
extern void vgaRestoreScreenAndActivate() ;
/*
 * Everything Below is "ppc" entry points
 */
/* vgaGC.o */
extern Mask vgaChangeGCtype() ;
extern Mask vgaChangeWindowGC() ;
/* vgaBitBlt.o */
extern void vgaBitBlt() ;
/* vgaGlyph.o */
extern void vgaDrawGlyph() ;
/* vgaImages.o */
extern void vgaDrawColorImage() ;
extern void vgaReadColorImage() ;
/* vgaLine.o */
extern void vgaHorzLine() ;
extern void vgaVertLine() ;
extern void vgaBresLine() ;
/* vgaStipple.o */
extern void vgaDrawMonoImage() ;
extern void vgaFillStipple() ;
/* vgaSolid.o */
extern void vgaFillSolid() ;

#endif /* CURSOR_ONLY */
#endif
