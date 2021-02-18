/*
 * Copyright 1987 by Tektronix Inc, Beaverton, Oregon.
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Tektronix not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 *
 *
 * TEKTRONIX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * TEKTRONIX BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/* 
 * $XConsortium: data.c,v 6.2 88/10/11 18:10:24 jim Exp $ 
 * $Locker:  $ 
 */
#include <sys/param.h>
#include "X11/Xlib.h"
#include "resource.h"

FontMap x10fontMap[] = {
 { "6x10", FONT_FIXED },
 { "6x13", FONT_FIXED },
 { "6x13p", FONT_FIXED },
 { "8x13", FONT_FIXED },
 { "9x15", FONT_FIXED },
 { "accord", FONT_FIXED },
 { "accord18", FONT_FIXED },
 { "accordb", FONT_FIXED },
 { "accordbfx", FONT_FIXED },
 { "accordbssx", FONT_FIXED },
 { "accordbsx", FONT_FIXED },
 { "accordfx", FONT_FIXED },
 { "accordssx", FONT_FIXED },
 { "accordsx", FONT_FIXED },
 { "b28botb", FONT_FIXED },
 { "cmi5", FONT_FIXED },
 { "cmi5fx", FONT_FIXED },
 { "cmi5ssx", FONT_FIXED },
 { "cmi5sx", FONT_FIXED },
 { "cmr7", FONT_FIXED },
 { "cmr7fx", FONT_FIXED },
 { "cmr7ssx", FONT_FIXED },
 { "cmr7sx", FONT_FIXED },
 { "cmsy7", FONT_FIXED },
 { "cnt57", FONT_FIXED },
 { "cor", FONT_FIXED },
 { "cor20", FONT_FIXED },
 { "cor20fx", FONT_FIXED },
 { "corfx", FONT_FIXED },
 { "ctl25", FONT_FIXED },
 { "ctl25fx", FONT_FIXED },
 { "ctl25ssx", FONT_FIXED },
 { "ctl25sx", FONT_FIXED },
 { "ctrlbar", FONT_FIXED },
 { "cyr30", FONT_FIXED },
 { "datapad", FONT_FIXED },
 { "delgat", FONT_FIXED },
 { "delgatfx", FONT_FIXED },
 { "dolby", FONT_FIXED },
 { "dotx1", FONT_FIXED },
 { "dotx1fx", FONT_FIXED },
 { "doty4", FONT_FIXED },
 { "doty4fx", FONT_FIXED },
 { "doty4ssx", FONT_FIXED },
 { "doty4sx", FONT_FIXED },
 { "doty5", FONT_FIXED },
 { "doty5fx", FONT_FIXED },
 { "doty5ssx", FONT_FIXED },
 { "doty5sx", FONT_FIXED },
 { "empire", FONT_FIXED },
 { "esch40", FONT_FIXED },
 { "esch80", FONT_FIXED },
 { "germ35", FONT_FIXED },
 { "helv10", FONT_FIXED },
 { "helv10b", FONT_FIXED },
 { "helv10bfx", FONT_FIXED },
 { "helv10bssx", FONT_FIXED },
 { "helv10bsx", FONT_FIXED },
 { "helv10fx", FONT_FIXED },
 { "helv10ssx", FONT_FIXED },
 { "helv10sx", FONT_FIXED },
 { "helv12", FONT_FIXED },
 { "helv12b", FONT_FIXED },
 { "helv12bfx", FONT_FIXED },
 { "helv12bssx", FONT_FIXED },
 { "helv12bsx", FONT_FIXED },
 { "helv12fx", FONT_FIXED },
 { "helv12i", FONT_FIXED },
 { "helv12ifx", FONT_FIXED },
 { "helv12issx", FONT_FIXED },
 { "helv12isx", FONT_FIXED },
 { "helv12ssx", FONT_FIXED },
 { "helv12sx", FONT_FIXED },
 { "hisym", FONT_FIXED },
 { "hisymbol", FONT_FIXED },
 { "host", FONT_FIXED },
 { "ice34", FONT_FIXED },
 { "icons", FONT_FIXED },
 { "k28bot", FONT_FIXED },
 { "k28botb", FONT_FIXED },
 { "k28botbu", FONT_FIXED },
 { "k28botu", FONT_FIXED },
 { "k28top", FONT_FIXED },
 { "k28topb", FONT_FIXED },
 { "k28topu", FONT_FIXED },
 { "kata30", FONT_FIXED },
 { "kbsym", FONT_FIXED },
 { "kilter", FONT_FIXED },
 { "kilter10", FONT_FIXED },
 { "kilter28", FONT_FIXED },
 { "kilter28b", FONT_FIXED },
 { "kilter28u", FONT_FIXED },
 { "kilterb", FONT_FIXED },
 { "kiltercrn", FONT_FIXED },
 { "kilterd", FONT_FIXED },
 { "kilterdb", FONT_FIXED },
 { "kiltr28bu", FONT_FIXED },
 { "leds", FONT_FIXED },
 { "math5", FONT_FIXED },
 { "micr25", FONT_FIXED },
 { "micr25fx", FONT_FIXED },
 { "micr25sx", FONT_FIXED },
 { "monitorbk", FONT_FIXED },
 { "monitorfr", FONT_FIXED },
 { "mouse", FONT_FIXED },
 { "nil2", FONT_FIXED },
 { "noecho", FONT_FIXED },
 { "noecho14", FONT_FIXED },
 { "nonb44", FONT_FIXED },
 { "nonb44fx", FONT_FIXED },
 { "nonb44ssx", FONT_FIXED },
 { "nonb44sx", FONT_FIXED },
 { "noni44", FONT_FIXED },
 { "noni44fx", FONT_FIXED },
 { "noni44ssx", FONT_FIXED },
 { "noni44sx", FONT_FIXED },
 { "oldeng", FONT_FIXED },
 { "oldengfx", FONT_FIXED },
 { "oldengssx", FONT_FIXED },
 { "oldengsx", FONT_FIXED },
 { "pointers", FONT_FIXED },
 { "rogue-b", FONT_FIXED },
 { "rogue-n", FONT_FIXED },
 { "rune32", FONT_FIXED },
 { "sbdi40", FONT_FIXED },
 { "sbdi40fx", FONT_FIXED },
 { "sbdi40ssx", FONT_FIXED },
 { "sbdi40sx", FONT_FIXED },
 { "sbdr40", FONT_FIXED },
 { "sbdr40fx", FONT_FIXED },
 { "sbdr40ssx", FONT_FIXED },
 { "sbdr40sx", FONT_FIXED },
 { "sbdr66", FONT_FIXED },
 { "sbdr66fx", FONT_FIXED },
 { "sbdr66ssx", FONT_FIXED },
 { "sbdr66sx", FONT_FIXED },
 { "symbol", FONT_FIXED },
 { "tekctrlbr", FONT_FIXED },
 { "teklwrbar", FONT_FIXED },
 { "timrom10", FONT_VARIABLE },
 { "timrom10b", FONT_VARIABLE },
 { "timrom10bfx", FONT_VARIABLE },
 { "timrom10bssx", FONT_VARIABLE },
 { "timrom10bsx", FONT_VARIABLE },
 { "timrom10fx", FONT_VARIABLE },
 { "timrom10i", FONT_VARIABLE },
 { "timrom10ifx", FONT_VARIABLE },
 { "timrom10issx", FONT_VARIABLE },
 { "timrom10isx", FONT_VARIABLE },
 { "timrom10ssx", FONT_VARIABLE },
 { "timrom10sx", FONT_VARIABLE },
 { "timrom12", FONT_VARIABLE },
 { "timrom12b", FONT_VARIABLE },
 { "timrom12bfx", FONT_VARIABLE },
 { "timrom12bssx", FONT_VARIABLE },
 { "timrom12bsx", FONT_VARIABLE },
 { "timrom12fx", FONT_VARIABLE },
 { "timrom12i", FONT_VARIABLE },
 { "timrom12ifx", FONT_VARIABLE },
 { "timrom12issx", FONT_VARIABLE },
 { "timrom12isx", FONT_VARIABLE },
 { "timrom12ssx", FONT_VARIABLE },
 { "timrom12sx", FONT_VARIABLE },
 { "tng40", FONT_FIXED },
 { "tng75", FONT_FIXED },
 { "troman10", FONT_VARIABLE },
 { "vs100", FONT_FIXED },
 { "vstadoc", FONT_FIXED },
 { "vstadocfx", FONT_FIXED },
 { "vstadocssx", FONT_FIXED },
 { "vstadocsx", FONT_FIXED },
 { "vtbold", FONT_FIXED },
 { "vtdhbot", FONT_FIXED },
 { "vtdhtop", FONT_FIXED },
 { "vtdwidth", FONT_FIXED },
 { "vtsingle", FONT_FIXED },
 { 0, 0 }
};

int	x10fontMapSize = (sizeof(x10fontMap) / sizeof(FontMap)) - 1;

FontMap x11fontMap[] = {
	{ "fixed", 0	/* FONT_FIXED */ },
	{ "variable", 0	/* FONT_VARIABLE */ },
	{ 0, 0 },
};

/*
 * Inverts the graphics function if black and white are not 0 and 1.
 * Note that we only have to invert those that take both source and
 * destination.  Esentially,
 *    - compliment both the source and destination
 *    - compliment the final result
 *    - reduce, using boolean algebra
 */
u_char	GXFunctionInvert[16] = {
	GXclear,	/* <== GXclear (0x0) */
	GXor,		/* <== GXand (0x1) */
	GXorInverted,	/* <== GXandReverse (0x2) */
	GXcopy,		/* <== GXcopy (0x3) */
	GXorReverse,	/* <== GXandInverted (0x4) */
	GXnoop,		/* <== GXnoop (0x5) */
	GXequiv,	/* <== GXxor (0x6) */
	GXand,		/* <== GXor (0x7) */
	GXnand,		/* <== GXnor (0x8) */
	GXxor,		/* <== GXequiv (0x9) */
	GXinvert,	/* <== GXinvert (0xa) */
	GXandInverted,	/* <== GXorReverse (0xb) */
	GXcopyInverted,	/* <== GXcopyInverted (0xc) */
	GXandReverse,	/* <== GXorInverted (0xd) */
	GXnor,		/* <== GXnand (0xe) */
	GXset		/* <== GXset (0xf) */
};

char *VisualClassNames[] = {
	"StaticGray",
	"GrayScale",
	"StaticColor",
	"PseudoColor",
	"TrueColor",
	"DirectColor",
};

Clientptr	clist[ NOFILE ];
int		FileDescriptors = NOFILE;
Bool		NoTransform = True;
Bool		WindowsUnmappedTransparent;
Bool		DoSync = False;		/* whether we enforce syncronization */
Bool		GrabServerAllowed = True;/* true if we allow grabserver */
Bool		ServerGrabbed = False;	/* True if a grab is in progress */
Bool		GrabMouseAllowed = True;/* true if we allow grabmouse */
Bool		FocusKeyboardAllowed = True;/* true if we allow focuskeyboard */
Bool		Bigendian = True;	/* true if this is big endian machine */
Bool		LockToggleMode = True;	/* true if X10 LockToggleMode is on */
Bool		OverrideRedirect = True;/* used in MapWindow */
char		*program;
Display 	*PrimaryConnection;	/* private line to X11 server */
int		GrabUnviewable;		/* client# if grabmouse -> unviewable */
int		PrimaryClient;		/* we look like a client, too */
int		X11ServerFd;		/* Simple reference to x11 server */
Colormap 	ScreenColormap;		/* colormap to use for everything */
Visual		*ScreenVisual;		/* visual to use for everything */
unsigned long	ScreenWhitePixel;	/* White pixel */
unsigned long	ScreenBlackPixel;	/* Black pixel */
int		ScreenDepth;		/* depth of the visual we are using */
int		ScreenColorCells;	/* number of cells for our colormap */
unsigned int	BestWidth;
unsigned int	BestHeight;
Time		LastKnownServerTime;
