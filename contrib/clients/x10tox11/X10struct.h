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
 * $XConsortium: X10struct.h,v 6.1 88/10/11 18:10:22 jim Exp $ 
 * $Locker:  $ 
 */
#include <X11/copyright.h>

/* Definitions for the X10 window system needed for translator */

#define UBPS (sizeof(short)/2) /* useful bytes per short */
/* size in bytes of a bitmap */
#define X10BitmapSize(width, height) \
	(((((width) + 15) >> 3) &~ 1) * (height) * UBPS)
/* size in bytes of a pixmap in XYFormat */
#define X10XYPixmapSize(width, height, planes) \
	((((width)+15 >> 3) & ~1) * (height) * UBPS * (planes))
/* size in bytes of a pizmap in ZFormat for 9 to 16 bit planes */
#define X10WZPixmapSize(width, height) (((width) * (height)) << 1)
/* size in bytes of a pixmap in ZFormat for 2 to 8 bit planes */
#define X10BZPixmapSize(width, height) ((width) * (height))

#define X10_XYFormat		0
#define X10_ZFormat		1

#define X10BytePad(n)	(((n) + 3) & ~3)
#define X10WordPad(n)	(((n) + 3) & ~3)

#define	X10_PROTOCOL		10
#define	XDEV_X10TRANSLATOR	8	/* Allocated from XDEV in X10 */
/* Request structure */

#ifdef BIGSHORTS
#define B16 :16
#define psizeof(x) ( 2*sizeof(x) / sizeof(short) )
typedef struct _short_p {
	short left:16, right:16;
} short_p;
typedef struct _ushort_p {
	unsigned short left:16, right:16;
} ushort_p;
#else
#define psizeof sizeof
#define B16
#endif

typedef struct _ColorDef {
	unsigned short pixel B16;
	unsigned short red B16, green B16, blue B16;
} X10ColorDef;

typedef struct _X10Req {
	unsigned char code;
	unsigned char func;
	unsigned short mask B16;
	Window windowId;
	union {
	    long l[4];
#ifdef BIGSHORTS
	    short_p s[4];
	    ushort_p u[4];
#else
	    short s[8];
	    unsigned short u[8];
#endif BIGSHORTS
	    char b[16];
	} param;
} X10Req;

/* Reply structure */

typedef struct _X10Rep {
	long code;
	union {
	    long l[5];
#ifdef BIGSHORTS
	    short_p s[5];
	    ushort_p u[5];
#else
	    short s[10];
	    unsigned short u[10];
#endif BIGSHORTS
	    char b[20];
	} param;
} X10Rep;

/* Used in X_Draw and X_DrawFilled */

typedef struct _X10Vertex {
	short x, y;
	unsigned short flags;
} X10Vertex;

/* The meanings of the flag bits.  If the bit is 1 the predicate is true */

#define X10VertexRelative	0x0001		/* else absolute */
#define X10VertexDontDraw	0x0002		/* else draw */
#define X10VertexCurved		0x0004		/* else straight */
#define X10VertexStartClosed	0x0008		/* else not */
#define X10VertexEndClosed	0x0010		/* else not */
#define X10VertexDrawLastPoint	0x0020		/* else don't */

#ifdef BIGSHORTS
#define params0 param.s[0].left
#define params1 param.s[0].right
#define params2 param.s[1].left
#define params3 param.s[1].right
#define params4 param.s[2].left
#define params5 param.s[2].right
#define params6 param.s[3].left
#define params7 param.s[3].right
#define params8 param.s[4].left
#define params9 param.s[4].right
#define paramu0 param.u[0].left
#define paramu1 param.u[0].right
#define paramu2 param.u[1].left
#define paramu3 param.u[1].right
#define paramu4 param.u[2].left
#define paramu5 param.u[2].right
#define paramu6 param.u[3].left
#define paramu7 param.u[3].right
#define paramu8 param.u[4].left
#define paramu9 param.u[4].right
#else
#define params0 param.s[0]
#define params1 param.s[1]
#define params2 param.s[2]
#define params3 param.s[3]
#define params4 param.s[4]
#define params5 param.s[5]
#define params6 param.s[6]
#define params7 param.s[7]
#define params8 param.s[8]
#define params9 param.s[9]
#define paramu0 param.u[0]
#define paramu1 param.u[1]
#define paramu2 param.u[2]
#define paramu3 param.u[3]
#define paramu4 param.u[4]
#define paramu5 param.u[5]
#define paramu6 param.u[6]
#define paramu7 param.u[7]
#define paramu8 param.u[8]
#define paramu9 param.u[9]
#endif BIGSHORTS

/*
 * X10 Bad returns
 */
#define X10BadRequest	1		/* bad request code */
#define X10BadValue	2		/* int parameter out of range */
#define X10BadWindow	3		/* parameter not a Window */
#define X10BadPixmap	4		/* parameter not a Pixmap */
#define X10BadBitmap	5		/* parameter not a Bitmap */
#define X10BadCursor	6		/* parameter not a Cursor */
#define X10BadFont	7		/* parameter not a Font */
#define X10BadMatch	8		/* parameter mismatch */
#define X10BadTile	9		/* Pixmap shape invalid for tiling */
#define X10BadGrab	10		/* mouse/button already grabbed */
#define X10BadAccess	11		/* access control violation */
#define X10BadAlloc	12		/* insufficient resources */
#define X10BadColor	13		/* no such color */

/* Input Event Codes */

#define X10NoEvent	 0x0000
#define	X10KeyPressed	 0x0001		/* keyboard key pressed */
#define	X10KeyReleased	 0x0002		/* keyboard key released */
#define	X10ButtonPressed 0x0004		/* mouse button pressed */
#define	X10ButtonReleased 0x0008	/* mouse button released */
#define	X10EnterWindow	 0x0010		/* mouse entering window */
#define	X10LeaveWindow	 0x0020		/* mouse leaving window */
#define	X10MouseMoved	 0x0040		/* mouse moves within window */
#define	X10ExposeWindow	 0x0080		/* full window changed and/or exposed */
#define	X10ExposeRegion	 0x0100		/* region of window exposed */
#define	X10ExposeCopy	 0x0200		/* region exposed by X_CopyArea */
#define	X10RightDownMotion 0x0400	/* mouse moves with right button down */
#define	X10MiddleDownMotion 0x0800	/* mouse moves with middle button down */
#define	X10LeftDownMotion 0x1000	/* mouse moves with left button down */
#define	X10UnmapWindow	 0x2000		/* window is unmapped */
#define	X10FocusChange	 0x4000		/* keyboard focus changed */

/* Event detail bits */

#define X10ControlMask	0x4000		/* Control key */
#define X10MetaMask	0x2000		/* Meta (Symbol) key */
#define X10ShiftMask	0x1000		/* Shift key */
#define X10ShiftLockMask 0x0800		/* ShiftLock key */
#define X10LeftMask	0x0400		/* Left button */
#define X10MiddleMask	0x0200		/* Middle button */
#define X10RightMask	0x0100		/* Right button */
#define X10ValueMask	0x00ff		/* Key/button code */

/* Reply codes */

#define X10_Reply		0	/* Normal reply */
#define X10_Error		-1	/* Error */

/* Request codes */
#define X10_CreateWindow		1
#define X10_CreateTransparency		2
#define X10_DestroyWindow		3
#define X10_DestroySubwindows		4
#define X10_MapWindow			5
#define X10_MapSubwindows		6
#define X10_UnmapWindow			7
#define X10_UnmapSubwindows		8
#define X10_UnmapTransparent		9
#define X10_RaiseWindow			10
#define X10_LowerWindow			11
#define X10_CircWindowUp		12
#define X10_MoveWindow			13
#define X10_ChangeWindow		14
#define X10_ConfigureWindow		15
#define X10_ChangeBackground		16
#define X10_ChangeBorder		17
#define X10_TileMode			18
#define X10_ClipMode			19
#define X10_QueryWindow			20
#define X10_StoreName			21
#define X10_FetchName			22
#define X10_SetIconWindow		23
#define X10_SetResizeHint		24
#define X10_GetResizeHint		25
#define X10_DefineCursor	 	26
#define X10_SelectInput			27
#define X10_GrabMouse			28
#define X10_GrabButton			29
#define X10_QueryMouse			30
#define X10_InterpretLocator		31
#define X10_WarpMouse			32
#define X10_FocusKeyboard		33
#define X10_CircWindowDown		34
#define X10_QueryTree			35
#define X10_Clear			40
#define X10_PixFill			41
#define X10_TileFill			42
#define X10_PixmapPut			43
#define X10_PixmapBitsPut		44
#define X10_BitmapBitsPut		45
#define X10_CopyArea			46
#define X10_Text			47
#define X10_TextMask			48
#define X10_Line			49
#define X10_Draw			50
#define X10_DrawFilled			51
#define X10_PixmapSave			52
#define X10_PixmapGet			53
#define X10_StippleFill			54
#define X10_SetUp			80
#define X10_UngrabMouse			81
#define X10_UngrabButton		82
#define X10_GetColor			83
#define X10_GetColorCells		84
#define X10_FreeColors			85
#define X10_StoreColors			86
#define X10_QueryColor			87
#define X10_GetFont			88
#define X10_FreeFont			89
#define X10_QueryFont			90
#define X10_CharWidths			91
#define X10_StringWidth			92
#define X10_FontWidths			93
#define X10_StoreBitmap			94
#define X10_FreeBitmap			95
#define X10_CharBitmap			96
#define X10_StorePixmap			97
#define X10_FreePixmap			98
#define X10_MakePixmap			99
#define X10_QueryShape			100
#define X10_StoreCursor			101
#define X10_FreeCursor			102
#define X10_MouseControl		103
#define X10_FeepControl			104
#define X10_Feep			105
#define X10_ShiftLock			106
#define X10_KeyClick			107
#define X10_AutoRepeat			108
#define X10_ScreenSaver			109
#define X10_StoreBytes			110
#define X10_FetchBytes			111
#define X10_RotateCuts			112
#define X10_AddHost			113
#define X10_RemoveHost			114
#define X10_GetHosts			115
#define X10_GrabServer			116
#define X10_UngrabServer		117
#define X10_LookupColor			118
#define X10_AppendBytes			119

/* Used in X_AddHost, X_RemoveHost, and X_GetHosts */
#define X10AF_INET		2
#define X10AF_DECnet		12
