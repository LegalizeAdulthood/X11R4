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

extern aedCursorInit();
extern Bool aedRealizeCursor();
extern Bool aedUnrealizeCursor();
extern int aedDisplayCursor();
extern void aedHideCursor();
extern void aedShowCursor();
extern void aedSolidFS();
extern void aedTileFS();
extern Bool aedCreateGC();
extern Bool aedCreateWindow();
extern void aedDestroyGC();
extern void aedValidateGC();
extern void aedGetSpans();
extern Bool aedProbe();
extern Bool aedScreenInit();
extern aedScreenClose();
extern void aedPaintWindowSolid();
extern Bool aedInitScreen();
extern void aedSetSpans();
extern void aedCopyWindow();
extern Bool aedChangeWindowAttributes();
extern void aedSolidLine();
extern void aedDashLine();
extern void aedPaintWindowTile();
extern void aedImageGlyphBlt();
extern RegionPtr aedCopyArea();
extern Bool aedPositionWindow();
extern void aedPolyFillRect();
extern void aedSolidFillArea();
extern void aedStippleFillArea();
extern void aedPolySegment();
extern int aedSetCursorPosition();
extern void aedPushPixSolid();
extern void aedDrawImage();
extern Bool aedRealizeFont();
extern Bool aedUnrealizeFont();
extern void aedImageText8();
extern int aedPolyText8();
extern void aedPaintWindow();
extern void aedCopyRotatePixmap();
