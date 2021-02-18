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
/***********************************************************
Copyright 1989 by the Massachusetts Institute of Technology

                     All rights reserved.

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of the Massachusetts
Institute of Technology (M.I.T.) not be used in advertising or publicity
pertaining to distribution of the software without specific, written
prior permission.

M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Decls.h,v 3.1 89/11/06 10:34:07 epeisach Exp Locker: epeisach $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/apa16/RCS/apa16Decls.h,v $ */
/* "@(#)apa16decls.h	3.1 88/09/22 09:30:27" */

/* apa16bitblt.c */
extern	RegionPtr	apa16CopyArea();
extern	RegionPtr	apa16CopyPlane();
extern	int		apa16DoBitblt();

/* apa16cursor.c */
extern	int		apa16CursorInit();
extern	Bool		apa16RealizeCursor();
extern	Bool		apa16UnrealizeCursor();
extern	int		apa16DisplayCursor();
extern	void		apa16HideCursor();

/* apa16fillsp.c */
extern	void		apa16TileFS();
extern	void		apa16SolidFS();
extern	void		apa16StippleFS();
extern	void		apa16InvertSolidFS();
extern	void		apa16InvertStippleFS();
extern	void		apa16WhiteStippleFS();
extern	void		apa16BlackSolidFS();
extern	void		apa16WhiteSolidFS();
extern	void		apa16UnnaturalTileFS();
extern	void		apa16UnnaturalStippleFS();

/* apa16font.c */
extern	Bool		afRealizeFont();
extern	Bool		afUnrealizeFont();

/* apa16gc.c */
extern	Bool		apa16CreateGC();
extern	void		apa16ValidateGC();
extern	void		apa16DestroyGC();

/* apa16hdwr.c */
extern	int		apa16_qoffset;
extern	unsigned short int apa16_rop2stype[];

/* apa16imggblt.c */
extern	void		apa16ImageGlyphBlt();

/* apa16io.c */
extern	Bool		apa16Probe();
extern	Bool		apa16ScreenInit();
extern	Bool		apa16ScreenClose();

/* apa16line.c */
extern	void		apa16LineSS();
extern	void		apa16PolySegment();
extern	void		apa16DashLine();

/* apa16plygblt.c */
extern	void		apa16PolyGlyphBlt();

/* apa16pntwin.c */
extern	void		apa16PaintWindowSolid();
extern	void		apa16PaintWindow();
/* apa16pnta.c */
extern	void		apa16SolidFillArea();
extern	void		apa16StippleFillArea();

/* apa16reparea.c */
extern	void		apa16replicateArea();

/* apa16text.c */
extern	void		apa16ImageText8();
extern	void		apa16ImageText16();
extern	int		apa16PolyText8();
extern	int		apa16PolyText16();

/* apa16tile.c */
extern	void		apa16TileArea32();

/* apa16window.c */
extern	Bool		apa16CreateWindow();
extern	void		apa16CopyWindow();
extern	Bool		apa16ChangeWindowAttributes();

/* mfbwait.c */
extern	void		apa16PolyFillRect();
extern	void		apa16PolyPoint();
extern	void		apa16PushPixels();
extern	void		apa16PutImage();
extern	void		apa16SetSpans();
extern	void		apa16GetSpans();
extern	void		apa16GetImage();
extern	void		apa16ZeroPolyArcSS();

/* apa16FillArc.c */
extern	void		apa16PolyFillArcSolid();

