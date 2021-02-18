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
 * $XConsortium: eventdebug.c,v 6.1 88/10/11 18:10:28 jim Exp $ 
 * $Locker:  $ 
 */
#ifdef DEBUG

#include "resource.h"

void ShowButtonMask(x10mask, x11mask)
	unsigned long	x10mask, x11mask;
{
	if (! debugged(XButtonMaskDebug))
		return;
	bugout("X10 button mask(0x%x): ", x10mask);
	if (X10ControlMask & x10mask)	   bugout("Control ");
	if (X10MetaMask & x10mask)	   bugout("Meta ");
	if (X10ShiftMask & x10mask)	   bugout("Shift ");
	if (X10ShiftLockMask & x10mask)    bugout("ShiftLock ");
	if (X10LeftMask & x10mask)	   bugout("Left ");
	if (X10MiddleMask & x10mask)	   bugout("Middle ");
	if (X10RightMask & x10mask)	   bugout("Right ");
	if (X10ValueMask & x10mask)	   bugout("Value ");

	bugout("\n  -->X11: ");
	if (ShiftMask & x11mask)
		bugout("Shift ");
	if (LockMask & x11mask)
		bugout("Lock ");
	if (ControlMask & x11mask)
		bugout("Control ");
	if (Mod1Mask & x11mask)
		bugout("Mod1 ");
	if (Mod2Mask & x11mask)
		bugout("Mod2 ");
	if (Mod3Mask & x11mask)
		bugout("Mod3 ");
	if (Mod4Mask & x11mask)
		bugout("Mod4 ");
	if (Mod5Mask & x11mask)
		bugout("Mod5 ");
	bugout("\n");
}

void ShowEventMask(x10mask, x11mask)
	unsigned long	x10mask, x11mask;
{
	if (! debugged(XEventMaskDebug))
		return;
	bugout("X10 event mask(0x%x): ", x10mask);
	if (X10KeyPressed & x10mask)	   bugout("KeyPressed ");
	if (X10KeyReleased & x10mask)	   bugout("KeyReleased ");
	if (X10ButtonPressed & x10mask)	   bugout("ButtonPressed ");
	if (X10ButtonReleased & x10mask)   bugout("ButtonReleased ");
	if (X10EnterWindow & x10mask)	   bugout("EnterWindow ");
	if (X10LeaveWindow & x10mask)	   bugout("LeaveWindow ");
	if (X10MouseMoved & x10mask)	   bugout("MouseMoved ");
	if (X10RightDownMotion & x10mask)  bugout("RightDownMotion ");
	if (X10MiddleDownMotion & x10mask) bugout("MiddleDownMotion ");
	if (X10LeftDownMotion & x10mask)   bugout("LeftDownMotion ");
	if (X10ExposeWindow & x10mask)	   bugout("ExposeWindow ");
	if (X10ExposeRegion & x10mask)	   bugout("ExposeRegion ");
	if (X10ExposeCopy & x10mask)	   bugout("ExposeCopy ");
	if (X10FocusChange & x10mask)	   bugout("FocusChange ");
	if (X10UnmapWindow & x10mask)	   bugout("UnmapWindow ");

	bugout("\n  -->X11: ");
	if (KeyPressMask & x11mask)
		bugout("KeyPress ");
	if (KeyReleaseMask & x11mask)
		bugout("KeyRelease ");
	if (ButtonPressMask & x11mask)
		bugout("ButtonPress ");
	if (ButtonReleaseMask & x11mask)
		bugout("ButtonRelease ");
	if (EnterWindowMask & x11mask)
		bugout("EnterWindow ");
	if (LeaveWindowMask & x11mask)
		bugout("LeaveWindow ");
	if (PointerMotionMask & x11mask)
		bugout("PointerMotion ");
	if (PointerMotionHintMask & x11mask)
		bugout("PointerMotionHint ");
	if (Button1MotionMask & x11mask)
		bugout("Button1Motion ");
	if (Button2MotionMask & x11mask)
		bugout("Button2Motion ");
	if (Button3MotionMask & x11mask)
		bugout("Button3Motion ");
	if (Button4MotionMask & x11mask)
		bugout("Button4Motion ");
	if (Button5MotionMask & x11mask)
		bugout("Button5Motion ");
	if (ButtonMotionMask & x11mask)
		bugout("ButtonMotion ");
	if (KeymapStateMask & x11mask)
		bugout("KeymapState ");
	if (ExposureMask & x11mask)
		bugout("Exposure ");
	if (VisibilityChangeMask & x11mask)
		bugout("VisibilityChange ");
	if (StructureNotifyMask & x11mask)
		bugout("StructureNotify ");
	if (ResizeRedirectMask & x11mask)
		bugout("ResizeRedirect ");
	if (SubstructureNotifyMask & x11mask) 
		bugout("SubstructureNotify ");
	if (SubstructureRedirectMask & x11mask)
		bugout("SubstructureRedirect ");
	if (FocusChangeMask & x11mask)
		bugout("FocusChange ");
	if (PropertyChangeMask & x11mask)
		bugout("PropertyChange ");
	if (ColormapChangeMask & x11mask)
		bugout("ColormapChange ");
	if (OwnerGrabButtonMask & x11mask)
		bugout("OwnerGrabButton ");
	bugout("\n");
}

StaticFunction char *X10EventName(type)
	int	type;
{
	char	*name = "?";

	switch (type) {
	case X10KeyPressed:	  name = "KeyPressed"; break;
	case X10KeyReleased:	  name = "KeyReleased"; break;
	case X10ButtonPressed:	  name = "ButtonPressed"; break;
	case X10ButtonReleased:	  name = "ButtonReleased"; break;
	case X10EnterWindow:	  name = "EnterWindow"; break;
	case X10LeaveWindow:	  name = "LeaveWindow"; break;
	case X10MouseMoved:	  name = "MouseMoved"; break;
	case X10ExposeWindow:	  name = "ExposeWindow"; break;
	case X10ExposeRegion:	  name = "ExposeRegion"; break;
	case X10ExposeCopy:	  name = "ExposeCopy"; break;
	case X10RightDownMotion:  name = "RightDownMotion"; break;
	case X10MiddleDownMotion: name = "MiddleDownMotion"; break;
	case X10LeftDownMotion:	  name = "LeftDownMotion"; break;
	case X10UnmapWindow:	  name = "UnmapWindow"; break;
	case X10FocusChange:	  name = "FocusChange"; break;
	}
	return (name);
}

static char *statusStrings[] = {
	"????",
	"Ok",
	"not selected",
	"win missing",
	"child missing",
	"selected by child",
	"lock toggle mode",
	"not implemented",
	"no X10 equiv",
	"mode field",
	"no change",
	"suppressed",
	"parent not mapped",
	"already mapped",
	"unknown",
	"belongs to server"
};

StaticFunction void ShowX11Event(client, pe, eventStatus)
	int	client;
	XEvent	*pe;
	int	eventStatus;
{
	if (client != PrimaryClient && eventStatus != EventOK) {
		if (eventStatus <= 0
		 || eventStatus > sizeof(statusStrings) / sizeof(char *))
			Error("eventStatus=%d\n", eventStatus);
		bugout("dropped (%s):\n ", statusStrings[ eventStatus ]);
	}

	bugout("%d: %s(%d) seq %d",
		client, X11EventNames[ pe->type ], pe->type, pe->xany.serial);
	if (!debugged(XNameOnlyDebug)) switch (pe->type) {
	case KeyPress:
	case KeyRelease:
		bugout(" w=0x%x,root=0x%x,subw=0x%x",
			pe->xkey.window,
			pe->xkey.root,
			pe->xkey.subwindow);
		bugout("\n    time=0x%x,(%d,%d),root=(%d,%d),",
			pe->xkey.time,
			pe->xkey.x,
			pe->xkey.y,
			pe->xkey.y_root,
			pe->xkey.x_root);
		bugout("st=0x%x,key=0x%x,ss=%d",
			pe->xkey.state,
			pe->xkey.keycode,
			pe->xkey.same_screen);
		break;

	case ButtonPress:
	case ButtonRelease:
		bugout(" w=0x%x,root=0x%x,subw=0x%x",
			pe->xbutton.window,
			pe->xbutton.root,
			pe->xbutton.subwindow);
		bugout("\n    time=0x%x,(%d,%d),root=(%d,%d),",
			pe->xbutton.time,
			pe->xbutton.x,
			pe->xbutton.y,
			pe->xbutton.y_root,
			pe->xbutton.x_root);
		bugout("st=0x%xbut=0x%x,ss=%d",
			pe->xbutton.state,
			pe->xbutton.button,
			pe->xbutton.same_screen);
		break;

	case MotionNotify:
		bugout(" w=0x%x,root=0x%x,subw=0x%x",
			pe->xmotion.window,
			pe->xmotion.root,
			pe->xmotion.subwindow);
		bugout("\n    time=0x%x,(%d,%d),root=(%d,%d),",
			pe->xmotion.time,
			pe->xmotion.x,
			pe->xmotion.y,
			pe->xmotion.y_root,
			pe->xmotion.x_root);
		bugout("st=0x%ishint=%d,ss=%d",
			pe->xmotion.state,
			pe->xmotion.is_hint,
			pe->xmotion.same_screen);
		break;

	case EnterNotify:
	case LeaveNotify:
		bugout(" w=0x%x,root=0x%x,subw=0x%x",
			pe->xcrossing.window,
			pe->xcrossing.root,
			pe->xcrossing.subwindow);
		bugout("\n    time=0x%x,(%d,%d),root=(%d,%d),",
			pe->xcrossing.time,
			pe->xcrossing.x,
			pe->xcrossing.y,
			pe->xcrossing.y_root,
			pe->xcrossing.x_root);
		bugout("mode=");
		switch(pe->xcrossing.mode) {
		case NotifyNormal:	bugout("Normal,"); break;
		case NotifyGrab:	bugout("Grab,"); break;
		case NotifyUngrab:	bugout("Ungrab,"); break;
		case NotifyWhileGrabbed:bugout("WhileGrabbed,"); break;
		}
		bugout("\n    detail=");
		switch(pe->xcrossing.detail) {
		case NotifyAncestor:	bugout("Ancestor,"); break;
		case NotifyVirtual:	bugout("Virtual,"); break;
		case NotifyInferior:	bugout("Inferior,"); break;
		case NotifyNonlinear:	bugout("Nonlinear,"); break;
		case NotifyNonlinearVirtual:bugout("NonlinearVirtual,"); break;
		case NotifyPointer:	bugout("Pointer,"); break;
		case NotifyPointerRoot:	bugout("PointerRoot,"); break;
		case NotifyDetailNone:	bugout("DetailNone,"); break;
		}
		bugout("%sScreen,focus=%d,state=0x%x",
			pe->xcrossing.same_screen ? "Same" : "Diff",
			pe->xcrossing.focus,
			pe->xcrossing.state);
		break;

	case FocusIn:
	case FocusOut:
	case KeymapNotify:
		break;
	case Expose:
		bugout(" win=0x%x,(%d,%d),%dX%d,count=%d",
			pe->xexpose.window,
			pe->xexpose.x,
			pe->xexpose.y,
			pe->xexpose.width,
			pe->xexpose.height,
			pe->xexpose.count);
		break;

	case GraphicsExpose:
		bugout(" drawable=0x%x,(%d,%d),%dX%d,",
			pe->xgraphicsexpose.drawable,
			pe->xgraphicsexpose.x,
			pe->xgraphicsexpose.y,
			pe->xgraphicsexpose.width,
			pe->xgraphicsexpose.height);
		bugout("count=%d,maj=%d,min=%d",
			pe->xgraphicsexpose.count,
			pe->xgraphicsexpose.major_code,
			pe->xgraphicsexpose.minor_code);
		break;

	case NoExpose:
		bugout(" drawable=0x%x,maj=%d,min=%d",
			pe->xnoexpose.drawable,
			pe->xnoexpose.major_code,
			pe->xnoexpose.minor_code);
		break;

	case VisibilityNotify:
	case CreateNotify:
		break;

	case DestroyNotify:
		bugout(" evwin=0x%x,win=0x%x,fromconfig=%d",
			pe->xdestroywindow.event,
			pe->xdestroywindow.window);
		break;

	case UnmapNotify:
		bugout(" evwin=0x%x,win=0x%x,fromconfig=%d",
			pe->xunmap.event,
			pe->xunmap.window,
			pe->xunmap.from_configure);
		break;

	case MapNotify:
		bugout(" evwin=0x%x,win=0x%x,override=%d",
			pe->xmap.event,
			pe->xmap.window,
			pe->xmap.override_redirect);
		break;

	case MapRequest:
		bugout(" parent=0x%x,win=0x%x",
			pe->xmaprequest.parent,
			pe->xmaprequest.window);
		break;

	case ReparentNotify:
		bugout(" event=0x%x,win=0x%x,parent=0x%x,(%d,%d),",
			pe->xreparent.event,
			pe->xreparent.window,
			pe->xreparent.parent,
			pe->xreparent.x,
			pe->xreparent.y);
		bugout("override=%d", pe->xreparent.override_redirect);
		break;

	case ConfigureNotify:
		bugout(" event=0x%x,win=0x%x,(%d,%d),%dX%d\n",
			pe->xconfigure.event,
			pe->xconfigure.window,
			pe->xconfigure.x,
			pe->xconfigure.y,
			pe->xconfigure.width,
			pe->xconfigure.height);
		bugout("    bw=%d,above=0x%x,override=%d",
			pe->xconfigure.border_width,
			pe->xconfigure.above,
			pe->xconfigure.override_redirect);
		break;

	case ConfigureRequest:
	case GravityNotify:
	case ResizeRequest:
	case CirculateNotify:
	case CirculateRequest:
	case PropertyNotify:
	case SelectionClear:
	case SelectionRequest:
	case SelectionNotify:
	case ColormapNotify:
	case ClientMessage:
		break;
	}
	if (eventStatus != EventOK || !debugged(XNameOnlyDebug))
		bugout ("\n");
}

StaticFunction void ShowX10Event(ev)
	X10Rep	*ev;
{
	bugout("  ->%s", X10EventName((int)ev->code));
	switch (ev->code) {
	case X10KeyPressed:
	case X10KeyReleased:
	case X10ButtonPressed:
	case X10ButtonReleased:
	case X10EnterWindow:
	case X10LeaveWindow:
	case X10MouseMoved:
		bugout(" win=0x%x,time=0x%x,detail=0x%x,(%d,%d)\n",
			ev->param.l[0],
			ev->param.s[2],
			ev->param.s[3],
			ev->param.s[4],
			ev->param.s[5]);
		bugout("    subw=0x%x,locator=(%d,%d)",
			ev->param.l[3],
			ev->param.s[8],
			ev->param.s[9]);
		break;

	case X10ExposeWindow:
	case X10ExposeRegion:
		bugout(" win=0x%x,detail=%d,%dX%d,",
			ev->param.l[0],
			ev->param.s[3],
			ev->param.s[4],
			ev->param.s[5]);
		bugout("subw=0x%x,(top,left)=(%d,%d)",
			ev->param.l[3],
			ev->param.s[8],
			ev->param.s[9]);
		break;

	case X10UnmapWindow:
	case X10ExposeCopy:
		bugout(" win=0x%x,subw=0x%x",
			ev->param.l[0],
			ev->param.l[3]);
		break;

	case X10RightDownMotion:
	case X10MiddleDownMotion:
	case X10LeftDownMotion:
		bugout("");
		break;

	case X10FocusChange:
		bugout("win=0x%x,e or l=%d,subw=0x%x",
			ev->param.l[0],
			ev->param.s[3],
			ev->param.l[3]);
		break;
	}
}

void ShowX11AndX10Event(client, x11ev, ev, statusptr)
	int	client;
	XEvent	*x11ev;
	X10Rep	*ev;
	int	*statusptr;
{
	char	buffer[ BUFSIZ ];

	if (!debugged(x11ev->type + X11_EventBase))
		return;
	ShowX11Event(client,
		x11ev,
		statusptr == NULL ? EventSuppressed : *statusptr);

	if (ev == NULL)
		return;

	ShowX10Event(ev);
	if (*statusptr == EventOK && debugged(XPromptForEachEvent)) {
		bugout("hit return");
		if (gets(buffer) == NULL)
			return;
		if (*buffer == 's') {
			*statusptr = EventSuppressed;
			bugout("suppressed...\n");
		}
	} else
		bugout("\n");
}
#endif DEBUG
