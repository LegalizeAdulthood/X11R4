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
 * $XConsortium: debug.h,v 6.2 88/10/11 18:10:26 jim Exp $ 
 * $Locker:  $ 
 */
#ifdef DEBUG

extern u_char	debugbits[];

#define	debugged(req)		(debugbits[ (req) ])
#define	debug(req, printfargs)	if (debugged(req)) { bugout printfargs; }
#define	SetDebugbit(req)	(debugbits[ (req) ] = 1)
#define	ClearDebugbit(req)	(debugbits[ (req) ] = 0)
#define	debugevent(printfargs)	debug(X10EventDebug, printfargs)

#define	X11_EventBase		120
#define	X11_Reply		(Reply+X11_EventBase)
#define	X11_Error		(Error+X11_EventBase)
#define	X11_KeyPress		(KeyPress+X11_EventBase)
#define	X11_KeyRelease		(KeyRelease+X11_EventBase)
#define	X11_ButtonPress		(ButtonPress+X11_EventBase)
#define	X11_ButtonRelease	(ButtonRelease+X11_EventBase)
#define	X11_MotionNotify	(MotionNotify+X11_EventBase)
#define	X11_EnterNotify		(EnterNotify+X11_EventBase)
#define	X11_LeaveNotify		(LeaveNotify+X11_EventBase)
#define	X11_FocusIn		(FocusIn+X11_EventBase)
#define	X11_FocusOut		(FocusOut+X11_EventBase)
#define	X11_KeymapNotify	(KeymapNotify+X11_EventBase)
#define	X11_Expose		(Expose+X11_EventBase)
#define	X11_GraphicsExpose	(GraphicsExpose+X11_EventBase)
#define	X11_NoExpose		(NoExpose+X11_EventBase)
#define	X11_VisibilityNotify	(VisibilityNotify+X11_EventBase)
#define	X11_CreateNotify	(CreateNotify+X11_EventBase)
#define	X11_DestroyNotify	(DestroyNotify+X11_EventBase)
#define	X11_UnmapNotify		(UnmapNotify+X11_EventBase)
#define	X11_MapNotify		(MapNotify+X11_EventBase)
#define	X11_MapRequest		(MapRequest+X11_EventBase)
#define	X11_ReparentNotify	(ReparentNotify+X11_EventBase)
#define	X11_ConfigureNotify	(ConfigureNotify+X11_EventBase)
#define	X11_ConfigureRequest	(ConfigureRequest+X11_EventBase)
#define	X11_GravityNotify	(GravityNotify+X11_EventBase)
#define	X11_ResizeRequest	(ResizeRequest+X11_EventBase)
#define	X11_CirculateNotify	(CirculateNotify+X11_EventBase)
#define	X11_CirculateRequest	(CirculateRequest+X11_EventBase)
#define	X11_PropertyNotify	(PropertyNotify+X11_EventBase)
#define	X11_SelectionClear	(SelectionClear+X11_EventBase)
#define	X11_SelectionRequest	(SelectionRequest+X11_EventBase)
#define	X11_SelectionNotify	(SelectionNotify+X11_EventBase)
#define	X11_ColormapNotify	(ColormapNotify+X11_EventBase)
#define	X11_ClientMessage	(ClientMessage+X11_EventBase)

#define	XPseudoBase		(X11_ClientMessage+1)
#define	XEventMaskDebug		(XPseudoBase+0)
#define	XButtonMaskDebug	(XPseudoBase+1)
#define	XBitmapDebug		(XPseudoBase+2)
#define	XUnimplementedDebug	(XPseudoBase+3)
#define	XWinTreeDebug		(XPseudoBase+4)
#define	XWinTreeMappedOnlyDebug	(XPseudoBase+5)
#define	XPromptForEachRequest	(XPseudoBase+6)
#define	XPromptForEachEvent	(XPseudoBase+7)
#define	XLineCoordinatesDebug	(XPseudoBase+8)
#define	XIODebug		(XPseudoBase+9)
#define	XNameOnlyDebug		(XPseudoBase+10)
#define	XInitializeDebug	(XPseudoBase+11)
#define	XSequenceDebug		(XPseudoBase+12)

#define	XLastDebug		(XSequenceDebug+1)

void	ShowX11AndX10Event();
void	ShowButtonMask();
void	ShowEventMask();

/*
 * libc declarations.
 */
char	*gets();

#else /* DEBUG */

#define	debugged(req)		(False)
#define	debug(req, printfargs)
#define	SetDebugbit(req)
#define	SetDebug(string)	(False)
#define	debugevent(printfargs)
#define ShowReq(req, rep)
#define ShowRep(code, reply, req, data1, data2)
#define ShowReqName(client, req)
#define	ForceShowReq(client, req)
#define	ShowX11AndX10Event(client, x11event, x10event, status)
#define ShowImage(name, image)
#define ShowButtonMask(x10mask, x11mask)
#define	ShowX10Bitmap(bitmap, width, height)
#define	ShowEventMask(x10mask, x11mask)
#define	ShowX10Line(npoints, vlist)

#endif /* DEBUG */
