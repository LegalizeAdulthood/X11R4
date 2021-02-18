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
#ifdef DEBUG
/* 
 * $XConsortium: debug.c,v 6.3 88/10/11 18:10:25 jim Exp $ 
 * $Locker:  $ 
 */
#include "resource.h"
#include "X11/X.h"
#include <stdio.h>
#include <ctype.h>

char *X10ProtoNames[] = {
	"unimplemented",
	"CreateWindow",
	"CreateTransparency",
	"DestroyWindow",
	"DestroySubwindows",
	"MapWindow",
	"MapSubwindows",
	"UnmapWindow",
	"UnmapSubwindows",
	"UnmapTransparent",
	"RaiseWindow",
	"LowerWindow",
	"CircWindowUp",
	"MoveWindow",
	"ChangeWindow",
	"ConfigureWindow",
	"ChangeBackground",
	"ChangeBorder",
	"TileMode",
	"ClipMode",
	"QueryWindow",
	"StoreName",
	"FetchName",
	"SetIconWindow",
	"SetResizeHint",
	"GetResizeHint",
	"DefineCursor",
	"SelectInput",
	"GrabMouse",
	"GrabButton",
	"QueryMouse",
	"InterpretLocator",
	"WarpMouse",
	"FocusKeyboard",
	"CircWindowDown",
	"QueryTree",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"Clear",
	"PixFill",
	"TileFill",
	"PixmapPut",
	"PixmapBitsPut",
	"BitmapBitsPut",
	"CopyArea",
	"Text",
	"TextMask",
	"Line",
	"Draw",
	"DrawFilled",
	"PixmapSave",
	"PixmapGet",
	"StippleFill",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"unimplemented",
	"SetUp",
	"UngrabMouse",
	"UngrabButton",
	"GetColor",
	"GetColorCells",
	"FreeColors",
	"StoreColors",
	"QueryColor",
	"GetFont",
	"FreeFont",
	"QueryFont",
	"CharWidths",
	"StringWidth",
	"FontWidths",
	"StoreBitmap",
	"FreeBitmap",
	"CharBitmap",
	"StorePixmap",
	"FreePixmap",
	"MakePixmap",
	"QueryShape",
	"StoreCursor",
	"FreeCursor",
	"MouseControl",
	"FeepControl",
	"Feep",
	"ShiftLock",
	"KeyClick",
	"AutoRepeat",
	"ScreenSaver",
	"StoreBytes",
	"FetchBytes",
	"RotateCuts",
	"AddHost",
	"RemoveHost",
	"GetHosts",
	"GrabServer",
	"UngrabServer",
	"LookupColor",
	"AppendBytes",
};

/*
 * Incomming X11 events.
 */
char *X11EventNames[] = {
	"type 0?",
	"type 1?",
	"KeyPress",
	"KeyRelease",
	"ButtonPress",
	"ButtonRelease",
	"MotionNotify",
	"EnterNotify",
	"LeaveNotify",
	"FocusIn",
	"FocusOut",
	"KeymapNotify",
	"Expose",
	"GraphicsExpose",
	"NoExpose",
	"VisibilityNotify",
	"CreateNotify",
	"DestroyNotify",
	"UnmapNotify",
	"MapNotify",
	"MapRequest",
	"ReparentNotify",
	"ConfigureNotify",
	"ConfigureRequest",
	"GravityNotify",
	"ResizeRequest",
	"CirculateNotify",
	"CirculateRequest",
	"PropertyNotify",
	"SelectionClear",
	"SelectionRequest",
	"SelectionNotify",
	"ColormapNotify",
	"ClientMessage"
};

/*
 * Our own pseudo codes start here for debugging other sorts of things.
 */
char *PseudoRequests[] = {
	"EventMasks",
	"ButtonMasks",
	"Bitmaps",
	"Unimplemented",
	"WinTree",
	"WinTreeMappedOnly",
	"PromptForEachRequest",
	"PromptForEachEvent",
	"LineCoordinates",
	"IO",
	"NameOnly",
	"Initialize",
	"Sequence"
};

u_char	debugbits[ XLastDebug ];

ForceShowReq(client, req)
	int	client;
	register X10Req *req;
{
	int	code = req->code;

	if (debugged(code))
		return;
	SetDebugbit(code);
	ShowReq(client, req);
	ClearDebugbit(code);
}

#define	TypeToMask(type)	(1<<sizeof(type))
#define Tchar	TypeToMask(char)
#define Tshort	TypeToMask(short)
#define Tlong	TypeToMask(long)
#define TWindow	TypeToMask(Window)

StaticFunction Bool ChangeElem(pvalue, name, psize, possible)
	char	*pvalue;
	char	*name;
	int	*psize;
	int	possible;
{
	static char	buffer[ BUFSIZ ];
	int	value;

	switch(1<<(*psize)) {
	case Tchar:	value = *((unsigned char *)pvalue); break;
	case Tshort:	value = *((unsigned short *)pvalue); break;
	case Tlong:	value = *((long *)pvalue); break;
	}

	bugout("%s=%d(0x%x): ", name, value, value);
	if (gets(buffer)) {
	    switch (*buffer) {
	    case '\0':
		break;
	    case 'b':
		if (possible & Tchar)
		    *psize = sizeof(char);
		else
		    goto badsize;
		break;
	    case 's':
		if (possible & Tshort)
		    *psize = sizeof(short);
		else
		    goto badsize;
		break;
	    case 'l':
		if (possible & Tlong)
		    *psize = sizeof(short);
		else
		    goto badsize;
		break;
	    case '0': case '1': case '2': case '3': case '4':
	    case '5': case '6': case '7': case '8': case '9':
		if (buffer[0] == '0' && buffer[1] == 'x')
		    sscanf(buffer, "0x%x", &value);
		else
		    sscanf(buffer, "%d", &value);

		switch(1<<(*psize)) {
		case Tchar:	*((unsigned char *)pvalue) = value; break;
		case Tshort:	*((unsigned short *)pvalue) = value; break;
		case Tlong:	*((long *)pvalue) = value; break;
		}

		break;
	    case -1:	/* impossible case */
	    badsize:
		bugout("can't change to that size... ");
		break;
	    default:
		bugout("huh?  ");
	    }
	}

	return (*buffer != '\0');
}
		
	
StaticFunction void EditRequest(req)
	register X10Req *req;
{
#define	ElementAddr(elem)	((int)(((char *)(&((X10Req *)NULL)->elem)) \
					- ((char *)NULL)))
#define	ElementCase(elem, possible)					\
	case ElementAddr(elem) + (sizeof(req->elem)<<8):		\
	    if (! ChangeElem((char *)&req->elem,			\
			     "elem", &cursize, possible))		\
		    offset += cursize;					\
	    break;
			
	int	offset = ElementAddr(code),
		cursize = sizeof(req->code);

	bugout("b,s,l for byte,short,long; enter new value for change.\n");

	while (offset < sizeof(X10Req)) {
		switch(offset + (cursize<<8)) {
		ElementCase(code, Tchar)
		ElementCase(func, Tchar)
		ElementCase(mask, Tshort)
		ElementCase(windowId, TWindow)
		ElementCase(param.l[0], Tchar|Tshort|Tlong)
		ElementCase(param.l[1], Tchar|Tshort|Tlong)
		ElementCase(param.l[2], Tchar|Tshort|Tlong)
		ElementCase(param.l[3], Tchar|Tshort|Tlong)
		ElementCase(param.s[0], Tchar|Tshort|Tlong)
		ElementCase(param.s[1], Tchar|Tshort|Tlong)
		ElementCase(param.s[2], Tchar|Tshort|Tlong)
		ElementCase(param.s[3], Tchar|Tshort|Tlong)
		ElementCase(param.s[4], Tchar|Tshort|Tlong)
		ElementCase(param.s[5], Tchar|Tshort|Tlong)
		ElementCase(param.s[6], Tchar|Tshort|Tlong)
		ElementCase(param.s[7], Tchar|Tshort|Tlong)
		ElementCase(param.b[0], Tchar|Tshort|Tlong)
		ElementCase(param.b[1], Tchar|Tshort|Tlong)
		ElementCase(param.b[2], Tchar|Tshort|Tlong)
		ElementCase(param.b[3], Tchar|Tshort|Tlong)
		ElementCase(param.b[4], Tchar|Tshort|Tlong)
		ElementCase(param.b[5], Tchar|Tshort|Tlong)
		ElementCase(param.b[6], Tchar|Tshort|Tlong)
		ElementCase(param.b[7], Tchar|Tshort|Tlong)
		ElementCase(param.b[8], Tchar|Tshort|Tlong)
		ElementCase(param.b[9], Tchar|Tshort|Tlong)
		ElementCase(param.b[10], Tchar|Tshort|Tlong)
		ElementCase(param.b[11], Tchar|Tshort|Tlong)
		ElementCase(param.b[12], Tchar|Tshort|Tlong)
		ElementCase(param.b[13], Tchar|Tshort|Tlong)
		ElementCase(param.b[14], Tchar|Tshort|Tlong)
		ElementCase(param.b[15], Tchar|Tshort|Tlong)
		default:
			if (cursize < sizeof(short))
				cursize = sizeof(short);
			else if (cursize < sizeof(long))
				cursize = sizeof(long);
			else if (offset < ElementAddr(param.l[0]))
				offset--;
			else
				offset++;
		}
	}
}

ShowReq(client, req)
	int	client;
	register X10Req *req;
{
	char	buffer[ BUFSIZ ];
	int	code = req->code;

	if (! debugged(code))
		return;

	if (code-1 > sizeof(X10ProtoNames)/sizeof(char *))
		code = 0;
	bugout("%d: %s ", client, X10ProtoNames[ code ]);

	if (! debugged(XNameOnlyDebug)) switch (code) {
	case X10_CreateWindow:
		bugout("bw=%d ", req->func);
		bugout("parent win=0x%x ", req->windowId);
		bugout("ht=%d ", req->param.s[0]);
		bugout("wid=%d ", req->param.s[1]);
		bugout("(%d,%d) ", req->param.s[2], req->param.s[3]);
		bugout("\n   border=0x%x ", req->param.l[2]);
		bugout("back=0x%x: ", req->param.l[3]);
		break;

	case X10_CreateTransparency:
		bugout("win=0x%x ", req->windowId);
		bugout("ht=%d ", req->param.s[0]);
		bugout("wid=%d ", req->param.s[1]);
		bugout("(%d,%d) ", req->param.s[2], req->param.s[3]);
		break;

	case X10_DestroyWindow:
	case X10_DestroySubwindows:
	case X10_MapWindow:
	case X10_MapSubwindows:
	case X10_UnmapWindow:
	case X10_UnmapSubwindows:
	case X10_UnmapTransparent:
	case X10_RaiseWindow:
	case X10_LowerWindow:
	case X10_CircWindowUp:
		bugout("window=0x%x ", req->windowId);
		break;

	case X10_MoveWindow:
		bugout("func=%d ", req->func);
		bugout("win=0x%x ", req->windowId);
		bugout("(%d,%d) ", req->param.s[0], req->param.s[1]);
		break;

	case X10_ChangeWindow:
		bugout("func=%d ", req->func);
		bugout("win=0x%x ", req->windowId);
		bugout("ht=%d ", req->param.s[0]);
		bugout("wid=%d ", req->param.s[1]);
		break;

	case X10_ConfigureWindow:
		bugout("func=%d ", req->func);
		bugout("win=0x%x ", req->windowId);
		bugout("ht=%d ", req->param.s[0]);
		bugout("wid=%d ", req->param.s[1]);
		bugout("(%d,%d) ", req->param.s[2], req->param.s[3]);
		break;

	case X10_ChangeBackground:
		bugout("win=0x%x ", req->windowId);
		bugout("bktile=0x%x ", req->param.l[0]);
		break;

	case X10_ChangeBorder:
		bugout("win=0x%x ", req->windowId);
		bugout("bdtile=0x%x ", req->param.l[0]);
		break;

	case X10_TileMode:
		switch (req->func) {
		case 0: bugout("TileModeAbsolute "); break;
		case 1: bugout("TileModeRelative "); break;
		default: bugout("TileMode? "); break;
		}
		bugout("win=0x%x ", req->windowId);
		break;

	case X10_ClipMode:
		switch (req->func) {
		case 0: bugout("ClipModeClipped "); break;
		case 1: bugout("ClipModeDrawThru "); break;
		default: bugout("ClipMode? "); break;
		}
		bugout("win=0x%x ", req->windowId);
		break;

	case X10_QueryWindow:
		bugout("win=0x%x ", req->windowId);
		break;

	case X10_StoreName:
		bugout("win=0x%x ", req->windowId);
		bugout("len=%d ", req->param.s[0]);
		break;

	case X10_FetchName:
		bugout("win=0x%x ", req->windowId);
		break;

	case X10_SetIconWindow:
		bugout("win=0x%x ", req->windowId);
		bugout("icon=%d ", req->param.l[0]);
		break;

	case X10_SetResizeHint:
		bugout("win=0x%x ", req->windowId);
		bugout("ht=%d ", req->param.s[0]);
		bugout("inc=%d ", req->param.s[1]);
		bugout("wid=%d ", req->param.s[2]);
		bugout("inc=%d ", req->param.s[3]);
		break;

	case X10_GetResizeHint:
		bugout("win=0x%x ", req->windowId);
		break;

	case X10_DefineCursor:
		bugout("win=0x%x ", req->windowId);
		bugout("cursor=0x%x ", req->param.l[0]);
		break;

	case X10_SelectInput:
		bugout("win=0x%x ", req->windowId);
		bugout("evmask=");

		if (req->param.l[0] & X10KeyPressed)
			bugout("KeyPressed,");
		if (req->param.l[0] & X10KeyReleased)
			bugout("KeyReleased,");
		if (req->param.l[0] & X10ButtonPressed)
			bugout("ButtonPressed,");
		if (req->param.l[0] & X10ButtonReleased)
			bugout("ButtonReleased,");
		if (req->param.l[0] & X10EnterWindow)
			bugout("EnterWindow,");
		if (req->param.l[0] & X10LeaveWindow)
			bugout("LeaveWindow,");
		if (req->param.l[0] & X10MouseMoved)
			bugout("MouseMoved,");
		if (req->param.l[0] & X10ExposeWindow)
			bugout("ExposeWindow,");
		if (req->param.l[0] & X10ExposeRegion)
			bugout("ExposeRegion,");
		if (req->param.l[0] & X10ExposeCopy)
			bugout("ExposeCopy,");
		if (req->param.l[0] & X10RightDownMotion)
			bugout("RightDownMotion,");
		if (req->param.l[0] & X10MiddleDownMotion)
			bugout("MiddleDownMotion,");
		if (req->param.l[0] & X10LeftDownMotion)
			bugout("LeftDownMotion,");
		if (req->param.l[0] & X10UnmapWindow)
			bugout("UnmapWindow,");
		if (req->param.l[0] & X10FocusChange)
			bugout("FocusChange,");
		break;

	case X10_GrabMouse:
		bugout("win=0x%x ", req->windowId);
		bugout("cursor=0x%x ", req->param.l[0]);
		bugout("evmask=0x%x ", req->param.l[1]);
		break;

	case X10_GrabButton:
		bugout("butmask=0x%x ", req->mask);
		bugout("win=0x%x ", req->windowId);
		bugout("cursor=0x%x ", req->param.l[0]);
		bugout("evmask=0x%x ", req->param.l[1]);
		break;

	case X10_QueryMouse:
		bugout("win=0x%x ", req->windowId);
		break;

	case X10_InterpretLocator:
		bugout("win=0x%x ", req->windowId);
		bugout("locator=%d,%d ", req->param.s[0], req->param.s[1]);
		break;

	case X10_WarpMouse:
		bugout("destwin=0x%x ", req->windowId);
		bugout("x=%d ", req->param.s[0]);
		bugout("y=%d ", req->param.s[1]);
		bugout("srcwin=0x%x ", req->param.l[1]);
		bugout("ht=%d ", req->param.s[4]);
		bugout("wid=%d ", req->param.s[5]);
		bugout("(%d,%d) ", req->param.s[6], req->param.s[7]);
		break;

	case X10_FocusKeyboard:
		bugout("func=%d ", req->func);
		bugout("win=0x%x ", req->windowId);
		break;

	case X10_CircWindowDown:
		bugout("win=0x%x ", req->windowId);
		break;

	case X10_QueryTree:
		bugout("win=0x%x ", req->windowId);
		break;

	case X10_Clear:
		bugout("win=0x%x ", req->windowId);
		break;

	case X10_PixFill:
		bugout("func=%d ", req->func);
		bugout("planemask=0x%x ", req->mask);
		bugout("win=0x%x ", req->windowId);
		bugout("ht=%d ", req->param.s[0]);
		bugout("wid=%d ", req->param.s[1]);
		bugout("(%d,%d) ", req->param.s[2], req->param.s[3]);
		bugout("pix=%d ", req->param.u[4]);
		bugout("bitmap=%x ", req->param.l[3]);
		break;

	case X10_TileFill:
		bugout("func=%d ", req->func);
		bugout("planemask=0x%x ", req->mask);
		bugout("win=0x%x ", req->windowId);
		bugout("ht=%d ", req->param.s[0]);
		bugout("wid=%d ", req->param.s[1]);
		bugout("(%d,%d) ", req->param.s[2], req->param.s[3]);
		bugout("pixmap=0x%x ", req->param.l[2]);
		bugout("bitmap=%x ", req->param.l[3]);
		break;

	case X10_PixmapPut:
		bugout("func=%d ", req->func);
		bugout("planemask=0x%x ", req->mask);
		bugout("win=0x%x ", req->windowId);
		bugout("ht=%d ", req->param.s[0]);
		bugout("wid=%d ", req->param.s[1]);
		bugout("src=(%d,%d) ", req->param.s[2], req->param.s[3]);
		bugout("pixmap=0x%x ", req->param.l[2]);
		bugout("dest=(%d,%d) ", req->param.s[6], req->param.s[7]);
		break;

	case X10_PixmapBitsPut:
		bugout("func=%d ", req->func);
		bugout("planemask=0x%x ", req->mask);
		bugout("win=0x%x ", req->windowId);
		bugout("ht=%d ", req->param.s[0]);
		bugout("wid=%d ", req->param.s[1]);
		bugout("(%d,%d) ", req->param.s[2], req->param.s[3]);
		switch (req->param.s[4]) {
		case 0: bugout("XYFormat "); break;
		case 1: bugout("ZFormat "); break;
		default: bugout("Format? "); break;
		}
		bugout("bitmap=%x ", req->param.l[3]);
		break;

	case X10_BitmapBitsPut:
		bugout("func=%d ", req->func);
		bugout("planemask=0x%x ", req->mask);
		bugout("win=0x%x ", req->windowId);
		bugout("ht=%d ", req->param.s[0]);
		bugout("wid=%d ", req->param.s[1]);
		bugout("(%d,%d)\n", req->param.s[2], req->param.s[3]);
		bugout("   forepix=%d ", req->param.u[4]);
		bugout("backpix=%d ", req->param.u[5]);
		bugout("bitmap=%x ", req->param.l[3]);
		break;

	case X10_CopyArea:
		bugout("func=%d ", req->func);
		bugout("planemask=0x%x ", req->mask);
		bugout("win=0x%x ", req->windowId);
		bugout("ht=%d ", req->param.s[0]);
		bugout("wid=%d ", req->param.s[1]);
		bugout("(%d,%d) ", req->param.s[2], req->param.s[3]);
		bugout("dest (%d,%d) ", req->param.s[6], req->param.s[7]);
		break;

	case X10_Text:
		bugout("func=%d ", req->func);
		bugout("planemask=0x%x ", req->mask);
		bugout("win=0x%x ", req->windowId);
		bugout("(%d,%d) ", req->param.s[0], req->param.s[1]);
		bugout("font=0x%x\n", req->param.l[1]);
		bugout("   forepix=%d ", req->param.u[4]);
		bugout("backpix=%d ", req->param.u[5]);
		bugout("chars=%d ", req->param.u[6]);
		bugout("pad=%d ", req->param.b[14]);
		bugout("space pad=%d ", req->param.b[15]);
		break;

	case X10_TextMask:
		bugout("func=%d ", req->func);
		bugout("planemask=0x%x ", req->mask);
		bugout("win=0x%x ", req->windowId);
		bugout("(%d,%d) ", req->param.s[0], req->param.s[1]);
		bugout("font=0x%x ", req->param.l[1]);
		bugout("srcpix=%d ", req->param.u[4]);
		bugout("chars=%d ", req->param.u[6]);
		bugout("pad=%d ", req->param.b[14]);
		bugout("spacepad=%d ", req->param.b[15]);
		break;

	case X10_Line:
		bugout("func=%d ", req->func);
		bugout("planemask=0x%x ", req->mask);
		bugout("win=0x%x ", req->windowId);
		bugout("x1=%d ", req->param.s[0]);
		bugout("y1=%d ", req->param.s[1]);
		bugout("x2=%d ", req->param.s[2]);
		bugout("y2=%d ", req->param.s[3]);
		bugout("srcpix=%d ", req->param.u[4]);
		bugout("ht=%d ", req->param.b[10]);
		bugout("wid=%d ", req->param.b[11]);
		break;

	case X10_Draw:
		bugout("func=%d ", req->func);
		bugout("planemask=0x%x ", req->mask);
		bugout("win=0x%x ", req->windowId);
		bugout("n=%d ", req->param.s[0]);
		bugout("srcpix=%d ", req->param.u[1]);
		bugout("ht=%d ", req->param.b[4]);
		bugout("wid=%d\n   ", req->param.b[5]);
		switch (req->param.s[3]) {
		case 0: bugout("DrawSolid "); break;
		case 1: bugout("DrawDashed "); break;
		case 2: bugout("DrawPatterned "); break;
		default: bugout("Draw? "); break;
		}
		bugout("altpix=%d ", req->param.u[4]);
		bugout("pat=%d ", req->param.s[5]);
		bugout("len=%d ", req->param.s[6]);
		bugout("mul=%d ", req->param.s[7]);
		break;

	case X10_DrawFilled:
		bugout("func=%d ", req->func);
		bugout("planemask=0x%x ", req->mask);
		bugout("win=0x%x ", req->windowId);
		bugout("n=%d ", req->param.s[0]);
		bugout("srcpix=%d ", req->param.u[1]);
		bugout("tile=0x%x ", req->param.l[1]);
		break;

	case X10_PixmapSave:
		bugout("win=0x%x ", req->windowId);
		bugout("ht=%d ", req->param.s[0]);
		bugout("wid=%d ", req->param.s[1]);
		bugout("(%d,%d) ", req->param.s[2], req->param.s[3]);
		break;

	case X10_PixmapGet:
		switch (req->func) {
		case 0: bugout("XYFormat "); break;
		case 1: bugout("ZFormat "); break;
		default: bugout("Format? "); break;
		}
		bugout("win=0x%x ", req->windowId);
		bugout("ht=%d ", req->param.s[0]);
		bugout("wid=%d ", req->param.s[1]);
		bugout("(%d,%d) ", req->param.s[2], req->param.s[3]);
		break;

	case X10_StippleFill:
		bugout("func=%d ", req->func);
		bugout("planemask=0x%x ", req->mask);
		bugout("win=0x%x ", req->windowId);
		bugout("ht=%d ", req->param.s[0]);
		bugout("wid=%d ", req->param.s[1]);
		bugout("(%d,%d) ", req->param.s[2], req->param.s[3]);
		bugout("srcpix=%d ", req->param.u[4]);
		bugout("bitmap=%x ", req->param.l[3]);
		break;

	case X10_UngrabButton:
		bugout("butmask=0x%x ", req->mask);
		break;

	case X10_GetColor:
		bugout("red=%d ", req->param.u[0]);
		bugout("green=%d ", req->param.u[1]);
		bugout("blue=%d ", req->param.u[2]);
		break;

	case X10_GetColorCells:
		if (req->func) bugout("planes must be contig. ");
		bugout("colors=%d ", req->param.s[0]);
		bugout("planes=0x%x ", req->param.s[1]);
		break;

	case X10_FreeColors:
		bugout("planemask=0x%x ", req->mask);
		bugout("n=%d ", req->param.s[0]);
		break;

	case X10_StoreColors:
		bugout("n=%d ", req->param.s[0]);
		break;

	case X10_QueryColor:
		bugout("pixel=%d ", req->param.u[0]);
		break;

	case X10_GetFont:
		bugout("font=0x%x ", req->param.s[0]);
		break;

	case X10_FreeFont:
		bugout("font=0x%x ", req->param.l[0]);
		break;

	case X10_QueryFont:
		bugout("font=0x%x ", req->param.l[0]);
		break;

	case X10_CharWidths:
		bugout("font=0x%x ", req->param.l[0]);
		bugout("n=%d ", req->param.s[2]);
		break;

	case X10_StringWidth:
		bugout("font=0x%x ", req->param.l[0]);
		bugout("n=%d ", req->param.s[2]);
		break;

	case X10_FontWidths:
		bugout("font=0x%x ", req->param.l[0]);
		break;

	case X10_StoreBitmap:
		bugout("ht=%d ", req->param.s[0]);
		bugout("wid=%d ", req->param.s[1]);
		break;

	case X10_FreeBitmap:
		bugout("bitmap=%x ", req->param.l[0]);
		break;

	case X10_CharBitmap:
		bugout("font=0x%x ", req->param.l[0]);
		bugout("char=%d ", req->param.s[2]);
		break;

	case X10_StorePixmap:
		switch (req->func) {
		case 0: bugout("XYFormat "); break;
		case 1: bugout("ZFormat "); break;
		default: bugout("Format? "); break;
		}
		bugout("ht=%d ", req->param.s[0]);
		bugout("wid=%d ", req->param.s[1]);
		break;

	case X10_FreePixmap:
		bugout("pixmap=0x%x ", req->param.l[0]);
		break;

	case X10_MakePixmap:
		bugout("bitmap=%x ", req->param.l[0]);
		bugout("forepix=%d ", req->param.u[2]);
		bugout("backpix=%d ", req->param.u[3]);
		break;

	case X10_QueryShape:
		switch (req->func) {
		case 0: bugout("CursorShape "); break;
		case 1: bugout("TileShape "); break;
		case 2: bugout("BrushShape "); break;
		default: bugout("Shape? "); break;
		}
		bugout("ht=%d ", req->param.s[0]);
		bugout("wid=%d ", req->param.s[1]);
		break;

	case X10_StoreCursor:
		bugout("func=%d ", req->func);
		bugout("cursor=0x%x ", req->param.l[0]);
		bugout("forepix=%d ", req->param.u[2]);
		bugout("backpix=%d ", req->param.u[3]);
		bugout("bm=0x%x ", req->param.l[2]);
		bugout("(%d,", req->param.s[6]);
		bugout("%d) ", req->param.s[7]);
		break;

	case X10_FreeCursor:
		bugout("cursor=0x%x ", req->param.l[0]);
		break;

	case X10_MouseControl:
		bugout("accel=%d ", req->param.s[0]);
		bugout("thresh=%d ", req->param.s[1]);
		break;

	case X10_FeepControl:
		bugout("vol=%d ", req->func);
		break;

	case X10_Feep:
		bugout("vol=%d ", req->param.s[0]);
		break;

	case X10_ShiftLock:
		switch (req->func) {
		case 0: bugout("LockUpDownMode "); break;
		case 1: bugout("LockToggleMode "); break;
		default: bugout("Mode? "); break;
		}
		break;

	case X10_KeyClick:
		bugout("vol=%d ", req->func);
		break;

	case X10_AutoRepeat:
		bugout(req->func == 0 ? "off" : "on");
		break;

	case X10_ScreenSaver:
		bugout(req->func == 0 ? "off" : "on");
		bugout("timeout=%d ", req->param.s[0]);
		bugout("interval=%d ", req->param.s[1]);
		break;

	case X10_StoreBytes:
		bugout("buf#=%d ", req->func);
		bugout("bytes=%d ", req->param.s[0]);
		break;

	case X10_FetchBytes:
		bugout("buf#=%d ", req->func);
		break;

	case X10_RotateCuts:
		bugout("count=%d ", req->func);
		break;

	case X10_AddHost:
		bugout("addr family=%d ", req->func);
		bugout("addr=%d ", req->param.l[0]);
		break;

	case X10_RemoveHost:
		bugout("addr family=%d ", req->func);
		bugout("addr=%d ", req->param.l[0]);
		break;

	case X10_GetHosts:
		bugout("addr family=%d ", req->func);
		break;

	case X10_LookupColor:
		bugout("len=%d ", req->param.s[0]);
		break;
	}

	if (debugged(XPromptForEachRequest)) {
		XFlush(clist[ client ]->dpy);
		do {
			bugout("hit e or CR: ");
			if (gets(buffer) == NULL)
				break;
			switch (*buffer) {
			case 'e':	/* edit the request */
				EditRequest(req);
				ShowReq(client, req);
			case '\n':
				*buffer = '\0';
			case '\0':
				break;
			default:
				bugout("what?  ");
				break;
			}
		} while (*buffer);
	}
	debug(XSequenceDebug, ("Seq %d", NextRequest(clist[ client ]->dpy)));
}

ShowRep(code, rep, req, data1)
	int	code;
	register X10Rep *rep;
	register X10Req *req;
	char *data1;
{
	int	i;
	short	*shortdata;
	X10ColorDef *x10colors;

	if (! debugged(code))
		return;
	if (debugged(XNameOnlyDebug)) {
		debug(code, ("\n"))
		return;
	}

	switch (code) {
	case X10_CreateWindow:
	case X10_CreateTransparency:
	case X10_QueryWindow:
	case X10_FetchName:
	case X10_GetResizeHint:
	case X10_QueryMouse:
	case X10_InterpretLocator:
	case X10_QueryTree:
	case X10_PixmapSave:
	case X10_PixmapGet:
	case X10_SetUp:
	case X10_GetColor:
	case X10_GetColorCells:
	case X10_QueryColor:
	case X10_GetFont:
	case X10_QueryFont:
	case X10_CharWidths:
	case X10_StringWidth:
	case X10_FontWidths:
	case X10_StoreBitmap:
	case X10_FreeBitmap:
	case X10_CharBitmap:
	case X10_StorePixmap:
	case X10_MakePixmap:
	case X10_QueryShape:
	case X10_StoreCursor:
	case X10_FetchBytes:
	case X10_GetHosts:
	case X10_LookupColor:
		bugout("reply: ");
		break;
	case X10_StoreColors:
		break;
	}

	switch (code) {
	case X10_CreateWindow:
		bugout("win=0x%x", rep->param.l[0]);
		break;

	case X10_CreateTransparency:
		bugout("win=0x%x ", rep->param.l[0]);
		break;

	case X10_QueryWindow:
		bugout("ht=%d ", rep->param.s[0]);
		bugout("wid=%d ", rep->param.s[1]);
		bugout("(%d,%d) ", rep->param.s[2], rep->param.s[3]);
		bugout("bdwid=%d ", rep->param.s[4]);

		switch (rep->param.b[10]) {
		case 0: bugout("IsUnmapped "); break;
		case 1: bugout("IsMapped "); break;
		case 2: bugout("IsInvisible "); break;
		default: bugout("MapIs? "); break;
		}
		switch (rep->param.b[11]) {
		case 0: bugout("IsTransparent "); break;
		case 1: bugout("IsOpaque "); break;
		case 2: bugout("IsIcon "); break;
		default: bugout("WinIs? "); break;
		}
		bugout("win=0x%x ", rep->param.l[3]);
		bugout("evmask=0x%x ", rep->param.l[4]);
		break;

	case X10_FetchName:
		bugout("len=%d ", rep->param.s[0]);
		break;

	case X10_GetResizeHint:
		bugout("ht=%d ", rep->param.s[0]);
		bugout("inc=%d ", rep->param.s[1]);
		bugout("wid=%d ", rep->param.s[2]);
		bugout("inc=%d ", rep->param.s[3]);
		break;

	case X10_QueryMouse:
		bugout("win=0x%x ", rep->param.l[0]);
		bugout("x=%d ", rep->param.s[2]);
		bugout("y=%d ", rep->param.s[3]);
		bugout("butmask=0x%x ", rep->param.s[4]);
		break;

	case X10_InterpretLocator:
		bugout("win=0x%x ", rep->param.l[0]);
		bugout("x=%d ", rep->param.s[2]);
		bugout("y=%d ", rep->param.s[3]);
		break;

	case X10_QueryTree:
		bugout("win=0x%x ", rep->param.l[0]);
		bugout("nwin=0x%x ", rep->param.l[1]);
		break;

	case X10_PixmapSave:
		bugout("pixmap=0x%x ", rep->param.l[0]);
		break;

	case X10_PixmapGet:
		bugout("n=%d ", rep->param.l[0]);
		break;

	case X10_SetUp:
		bugout("win=0x%x ", rep->param.l[0]);
		bugout("version=%d ", rep->param.s[2]);
		bugout("dev=%d ", rep->param.s[3]);
		bugout("planes=0x%x ", rep->param.s[4]);
		bugout("cells=%d ", rep->param.u[5]);
		break;

	case X10_GetColor:
		bugout("pixel=%d ", rep->param.u[0]);
		break;

	case X10_GetColorCells:
		bugout("planemask=0x%x ", rep->param.u[0]);
		shortdata = (short *) data1;
		if (req->param.s[0])
			bugout("pixels=");
		for (i=0; i < req->param.s[0]; i++)
			bugout("%s0x%x", i ? "," : "", shortdata[i]);
		break;

	case X10_QueryColor:
		bugout("red=%d ", rep->param.u[0]);
		bugout("green=%d ", rep->param.u[1]);
		bugout("blue=%d ", rep->param.u[2]);
		break;

	case X10_GetFont:
		bugout("font=0x%x ", rep->param.l[0]);
		break;

	case X10_QueryFont:
		bugout("ht=%d ", rep->param.s[0]);
		bugout("wid=%d ", rep->param.s[1]);
		bugout("first=%d ", rep->param.s[2]);
		bugout("last=%d ", rep->param.s[3]);
		bugout("bsline=%d ", rep->param.s[4]);
		bugout("%s ", rep->param.s[5] == 1 ? "fixed" : "variable");
		break;

	case X10_CharWidths:
		bugout("n=%d ", rep->param.l[0]);
		break;

	case X10_StringWidth:
		bugout("wid=%d ", rep->param.s[0]);
		break;

	case X10_FontWidths:
		bugout("n=%d ", rep->param.l[0]);
		break;

	case X10_StoreBitmap:
		bugout("bitmap=%x ", rep->param.l[0]);
		break;

	case X10_FreeBitmap:
		bugout("bitmap=%x ", rep->param.l[0]);
		break;

	case X10_CharBitmap:
		bugout("bitmap=%x ", rep->param.l[0]);
		break;

	case X10_StorePixmap:
		bugout("pixmap=0x%x ", rep->param.l[0]);
		break;

	case X10_MakePixmap:
		bugout("pixmap=0x%x ", rep->param.l[0]);
		break;

	case X10_QueryShape:
		bugout("ht=%d ", rep->param.s[0]);
		bugout("wid=%d ", rep->param.s[1]);
		break;

	case X10_StoreCursor:
		bugout("cursor=0x%x ", rep->param.l[0]);
		break;

	case X10_FetchBytes:
		bugout("bytes=%d ", rep->param.s[0]);
		break;

	case X10_GetHosts:
		bugout("bytes=%d ", rep->param.l[0]);
		break;

	case X10_LookupColor:
		bugout("red=%d ", rep->param.u[0]);
		bugout("green=%d ", rep->param.u[1]);
		bugout("blue=%d ", rep->param.u[2]);
		bugout("hdred=%d ", rep->param.u[3]);
		bugout("hdgreen=%d ", rep->param.u[4]);
		bugout("hdblue=%d ", rep->param.u[5]);
		break;
	case X10_StoreColors:
		x10colors = (X10ColorDef *)data1;
		for (i = 0; i < req->param.s[0]; i++) {
			bugout(" pix=0x%x(0x%x,0x%x,0x%x)",
				x10colors[i].pixel,
				x10colors[i].red,
				x10colors[i].green,
				x10colors[i].blue);
		}
		break;
	}
	bugout("\n");
}

SetDebug(name)
	char	*name;
{
	int	i, start, end, delete = False;
	char	*p, *strchr();

	/*
	 * Check for names that cover ranges of requests.
	 */
	if (strcmp(name, "+all") == 0 || strcmp(name, "+All") == 0) {
		for (i=0; i<sizeof(debugbits); i++)
			debugbits[ i ] = 1;
		return(True);
	} else if (strcmp(name, "-all") == 0 || strcmp(name, "-All") == 0) {
		for (i=0; i<sizeof(debugbits); i++)
			debugbits[ i ] = 0;
		return(True);
	} else if (strcmp(name, "+Events") == 0) {
		for (i=X11_EventBase; i<XPseudoBase; i++)
			SetDebugbit(i);
		return(True);
	} else if (strcmp(name, "-Events") == 0) {
		for (i=X11_EventBase; i<XPseudoBase; i++)
			ClearDebugbit(i);
		return(True);
	} else if (strcmp(name, "+Requests") == 0) {
		for (i=0; i<X11_EventBase; i++)
			SetDebugbit(i);
		return(True);
	} else if (strcmp(name, "-Requests") == 0) {
		for (i=0; i<X11_EventBase; i++)
			ClearDebugbit(i);
		return(True);
	} else if (strcmp(name, "+Fonts") == 0) {
		SetDebugbit(X10_Text);
		SetDebugbit(X10_TextMask);
		SetDebugbit(X10_GetFont);
		SetDebugbit(X10_FreeFont);
		SetDebugbit(X10_QueryFont);
		SetDebugbit(X10_CharWidths);
		SetDebugbit(X10_StringWidth);
		SetDebugbit(X10_FontWidths);
		return(True);
	}

	/*
	 * If the request begins with a '-', then delete instead of add.
	 */
	if (*name == '-')
		delete = True;
	name++;

	if (p = strchr(name, '-'))
		*p++ = '\0';
	if (isanumber(name)) /* number */
		start = atoi(name);
	else
		start = FindReq(name);

	if (start < 0)
		return(False);
	if (p) {
		if (isanumber(p)) /* number */
			end = atoi(p);
		else
			end = FindReq(p);
		if (start < 0)
			return(False);
		for (i=start; i<=end; i++)
			delete ? ClearDebugbit(i) : SetDebugbit(start);
	} else
		delete ? ClearDebugbit(start) : SetDebugbit(start);
	return(True);
}

FindReq(name)
	char	*name;
{
	int	i;

	for (i=0; i<sizeof(X10ProtoNames)/sizeof(char *); i++) {
		if (strcmp(name, X10ProtoNames[ i ]) == 0) {
			return(i);
		}
	}
	for (i=0; i<sizeof(X11EventNames)/sizeof(char *); i++) {
		if (strcmp(name, X11EventNames[ i ]) == 0) {
			return(i+X11_EventBase);
		}
	}
	for (i=0; i<sizeof(PseudoRequests)/sizeof(char *); i++) {
		if (strcmp(name, PseudoRequests[ i ]) == 0) {
			return(i+XPseudoBase);
		}
	}
	return(-1);
}

isanumber(name)
	char	*name;
{
	while (*name)
		if (! isdigit(*name++))
			return(0);
	return(1);
}

CommandInterp()
{
	char	buf[ BUFSIZ ];

	printf("\n");
	while (1) {
	    printf("command: "); fflush(stdout);
	    if (fgets(buf, BUFSIZ, stdin) == NULL) {
		printf("EOF received.\n");
		return;
	    }

	    buf[ strlen(buf) - 1 ] = '\0';
	    if (SetFlags(buf))
		continue;
	    if (strcmp(buf, "Quit") == 0)
		exit(0);

	    if (strcmp(buf, "ResetCache") == 0)
		ResetCache();
	    else if (strcmp(buf, "ShowCachePerformance") == 0)
		ShowCachePerformance();
	    else if (strncmp(buf, "SetCacheSize=", 13) == 0)
		SetCacheSize(atoi(buf+13));
	    else if (strncmp(buf, "WinTree", 7) == 0) {
		Bool		debugOn = debugged(XWinTreeDebug);
		WinInfoptr	wp = NULL;
		Window		win;
		char		*p;

		for (p = buf+7; *p; p++) {
		    if (*p == '0' && *(p+1) == 'x')
			*(++p) = '\t';
		    if (*p != ' ' && *p != '\t') {
			sscanf(p, "%x", &win);
			wp = WinLookup(-1, win);
			if (wp == NULL)
			    bugout("window 0x%x not found\n", win);
			break;
		    }
		}
		SetDebugbit(XWinTreeDebug);
		ShowWinTree(wp, 0);
		if (! debugOn)
		    ClearDebugbit(XWinTreeDebug);
	    }
	    else if (strcmp(buf, "Continue") == 0)
		return;
	    else
	        bugout("unknown command=%s\n", buf);
	}
}

ShowX10Line(npoints, vlist)
	int	npoints;
	X10Vertex *vlist;
{
	int	i;

	if (!debugged(XLineCoordinatesDebug))
		return;
	bugout("\n");
	for (i = 0; i < npoints; i++) {
		bugout("   (%d,%d) ", vlist[i].x, vlist[i].y);
		if (vlist[i].flags & X10VertexRelative)
			bugout("Relative ");
		if (vlist[i].flags & X10VertexCurved)
			bugout("Curved ");
		if (vlist[i].flags & X10VertexStartClosed)
			bugout("StartClosed ");
		if (vlist[i].flags & X10VertexEndClosed)
			bugout("EndClosed ");
		if (vlist[i].flags & X10VertexDrawLastPoint)
			bugout("DrawLastPoint ");
		if (vlist[i].flags & X10VertexDontDraw)
			bugout("Dont");
		bugout("\n");
	}
	bugout(" ...");
}

/*VARARGS1*/
bugout(fmt,x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,xa,xb,xc,xd,xe,xf)
	char	*fmt;
{
	fprintf(stderr, fmt,x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,xa,xb,xc,xd,xe,xf);
	fflush (stderr);
}

ShowImage(name, image)
	char	*name;
	XImage	*image;
{
	int	depth, width, height, x, y;

	if (!debugged(XBitmapDebug))
		return;

	fprintf(stderr, "%s: ", name);
	depth = image->depth;
	width = image->width;
	height = image->height;
	fprintf(stderr, "%d X %d, depth %d, %s format",
	    width, height, depth,
	    image->format == XYBitmap ? "XYBitmap"
	      : image->format == XYPixmap ? "XYPixmap"
		:image->format == ZPixmap ? "ZPixmap" : "unknown");
	fprintf(stderr, ", %s byte-order\n",
	    image->byte_order == LSBFirst ? "LSBFirst"
	      : image->byte_order == MSBFirst ? "MSBFirst" : "unknown");
	fprintf(stderr, ", bitmap unit of %d", image->bitmap_unit);
	fprintf(stderr, ", %s bit-order\n",
	    image->bitmap_bit_order == LSBFirst ? "LSBFirst"
	      : image->bitmap_bit_order == MSBFirst ? "MSBFirst" : "unknown");
	
	for (y=0; y<height; y++) {
	    for (x = 0; x<width; x++) {
		if (depth > 4)
		    fprintf(stderr, " ");
		fprintf(stderr, "%x", XGetPixel(image, x, y));
	    }
	    fprintf(stderr, "\n");
	}
	fflush(stderr);
}
	
ShowX10Bitmap(bitmap, width, height)
	unsigned short	*bitmap;
	unsigned int	width, height;
{
	int	i, j;

	if (! debugged(XBitmapDebug))
		return;
	bugout("\n");
	for (j=0; j<height; j++) {
		for (i=0; i<width; i+=16)
			bugout(" %04x", *bitmap++);
		bugout("\n");
	}
}

#else DEBUG

/*
 * Do precisely nothing.
 */
bugout()
{
}

#endif /* DEBUG */
