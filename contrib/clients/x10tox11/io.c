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
 * $XConsortium: io.c,v 6.1 88/10/11 18:10:33 jim Exp $ 
 * $Locker:  $ 
 */

/*
 * These routines maintain the x11 request sequence numbering of x10
 * requests.  A circular list (named 'list') is kept for each
 * "outstanding" x10 request.  An outstanding request is a request that we
 * have not received an x11 reply for (several X11 requests have no
 * reply).  An x10 request is no longer outstanding if an x11 reply has
 * been received whose sequence number is greater than or equal to the x10
 * request sequence number.
 */
#include "X11/Xlib.h"
#include "X11/Xproto.h"
#include "resource.h"
#include <stdio.h>
#include <setjmp.h>

static u_char SynchronousX11Request[] = {
	False,	/* CreateWindow */
	False,	/* ChangeWindowAttributes */
	True,	/* GetWindowAttributes */
	False,	/* DestroyWindow */
	False,	/* DestroySubwindows */
	False,	/* ChangeSaveSet */
	False,	/* ReparentWindow */
	False,	/* MapWindow */
	False,	/* MapSubwindows */
	False,	/* UnmapWindow */
	False,	/* UnmapSubwindows */
	False,	/* ConfigureWindow */
	False,	/* CirculateWindow */
	True,	/* GetGeometry */
	True,	/* QueryTree */
	True,	/* InternAtom */
	True,	/* GetAtomName */
	False,	/* ChangeProperty */
	False,	/* DeleteProperty */
	True,	/* GetProperty */
	True,	/* ListProperties */
	False,	/* SetSelectionOwner */
	True,	/* GetSelectionOwner */
	False,	/* ConvertSelection */
	False,	/* SendEvent */
	True,	/* GrabPointer */
	False,	/* UngrabPointer */
	False,	/* GrabButton */
	False,	/* UngrabButton */
	False,	/* ChangeActivePointerGrab */
	True,	/* GrabKeyboard */
	False,	/* UngrabKeyboard */
	False,	/* GrabKey */
	False,	/* UngrabKey */
	False,	/* AllowEvents */
	False,	/* GrabServer */
	False,	/* UngrabServer */
	True,	/* QueryPointer */
	True,	/* GetMotionEvents */
	True,	/* TranslateCoords */
	False,	/* WarpPointer */
	False,	/* SetInputFocus */
	True,	/* GetInputFocus */
	True,	/* QueryKeymap */
	False,	/* OpenFont */
	False,	/* CloseFont */
	True,	/* QueryFont */
	True,	/* QueryTextExtents */
	True,	/* ListFonts */
	True,	/* ListFontsWithInfo */
	False,	/* SetFontPath */
	True,	/* GetFontPath */
	False,	/* CreatePixmap */
	False,	/* FreePixmap */
	False,	/* CreateGC */
	False,	/* ChangeGC */
	False,	/* CopyGC */
	False,	/* SetDashes */
	False,	/* SetClipRectangles */
	False,	/* FreeGC */
	False,	/* ClearArea */
	False,	/* CopyArea */
	False,	/* CopyPlane */
	False,	/* PolyPoint */
	False,	/* PolyLine */
	False,	/* PolySegment */
	False,	/* PolyRectangle */
	False,	/* PolyArc */
	False,	/* FillPoly */
	False,	/* PolyFillRectangle */
	False,	/* PolyFillArc */
	False,	/* PutImage */
	True,	/* GetImage */
	False,	/* PolyText8 */
	False,	/* PolyText16 */
	False,	/* ImageText8 */
	False,	/* ImageText16 */
	False,	/* CreateColormap */
	False,	/* FreeColormap */
	False,	/* CopyColormapAndFree */
	False,	/* InstallColormap */
	False,	/* UninstallColormap */
	True,	/* ListInstalledColormaps */
	True,	/* AllocColor */
	True,	/* AllocNamedColor */
	True,	/* AllocColorCells */
	True,	/* AllocColorPlanes */
	False,	/* FreeColors */
	False,	/* StoreColors */
	False,	/* StoreNamedColor */
	True,	/* QueryColors */
	True,	/* LookupColor */
	False,	/* CreateCursor */
	False,	/* CreateGlyphCursor */
	False,	/* FreeCursor */
	False,	/* RecolorCursor */
	False,	/* QueryBestSize */
	True,	/* QueryExtension */
	True,	/* ListExtensions */
	False,	/* ChangeKeyboardMapping */
	True,	/* GetKeyboardMapping */
	False,	/* ChangeKeyboardControl */
	True,	/* GetKeyboardControl */
	False,	/* Bell */
	False,	/* ChangePointerControl */
	True,	/* GetPointerControl */
	False,	/* SetScreenSaver */
	True,	/* GetScreenSaver */
	False,	/* ChangeHosts */
	True,	/* ListHosts */
	False,	/* SetAccessControl */
	False,	/* SetCloseDownMode */
	False,	/* KillClient */
	False,	/* RotateProperties */
	False,	/* ForceScreenSaver */
	True,	/* SetPointerMapping */
	True,	/* GetPointerMapping */
	True,	/* SetModifierMapping */
	True,	/* GetModifierMapping */
};

static u_char BadMap[] = {
	0,		/* 0 */
	X10BadRequest,	/* BadRequest */
	X10BadValue,	/* BadValue */
	X10BadWindow,	/* BadWindow */
	X10BadPixmap,	/* BadPixmap */
	X10BadValue,	/* BadAtom */
	X10BadCursor,	/* BadCursor */
	X10BadFont,	/* BadFont */
	X10BadMatch,	/* BadMatch */
	X10BadTile,	/* BadDrawable */
	X10BadAccess,	/* BadAccess */
	X10BadAlloc,	/* BadAlloc */
	X10BadColor,	/* BadColor */
	X10BadWindow,	/* BadGC */
	X10BadValue,	/* BadIDChoice */
	X10BadValue,	/* BadName */
	X10BadValue,	/* BadLength */
	X10BadValue,	/* BadImplementation */
};

static jmp_buf closeEnv;
static int closingDisplay = -1;

X11IOErrorHandler(dpy)
        Display *dpy;
{
	int		fd = ConnectionNumber (dpy);
	int		client;
	static int	active;

	/*
	 * if primary connection gone, then turn out the lights...
	 */
	if (fd == ConnectionNumber (PrimaryConnection)) {
	    (void) close (fd);		/* just in case */
	    HangUp ();			/* blow away our connections */
	    exit (1);			/* our primary connection got nuked */
	}

	if (fd == closingDisplay) {	/* if already closing this display */
	    longjmp (closeEnv, 1);
	}

	if (active)
		return;

	active++;
	client = (int) clist[fd];
	Notice("IO error: shutting down client #%d\n", client);
	Close_down (client);
	clist[fd] = NULL;
	active = 0;

	/* check for lost connection */
	SyncWithServer (PrimaryConnection, True);

	RestartDispatcher();
}


X11CloseDisplay (dpy)
    Display *dpy;
{
    closingDisplay = ConnectionNumber (dpy);
    if (setjmp (closeEnv) == 0) {
        XCloseDisplay (dpy);
    }
    closingDisplay = -1;
    return;
}

#ifdef DEBUG
/*
 * A copy of the standard Xlib default Error... without the fatal error.
 */
StaticFunction void XDefaultError(dpy, event)
        Display *dpy;
        XErrorEvent *event;
{
	char buffer[BUFSIZ];
	char number[32];

	sprintf(number, "%d", event->request_code);
	XGetErrorDatabaseText(dpy, "XRequest", number, "", buffer, BUFSIZ);
	bugout("\nXError for request %s(%d): ", buffer, event->request_code);
	XGetErrorText(dpy, event->error_code, buffer, BUFSIZ);
	bugout("%s\n  Resource ID=0x%x, serial #%d\n",
		buffer, event->resourceid, event->serial);
}
#endif DEBUG

X11ErrorHandler(dpy, event)
        Display *dpy;
        XErrorEvent *event;
{
	int		client = (int) clist[ ConnectionNumber(dpy) ],
			tail;
	Clientptr	cp = clist[ client ];
	extern char	*X10ProtoNames[];
	X10Req		*preq, dummy;

	tail = cp->seq.tail;
	if (tail == cp->seq.head) { /* there are no requests! */
		preq = &dummy;
		bzero(&dummy, sizeof(dummy));
	} else {
		SequenceListptr	seq = &cp->seq;

		do {
			preq = seq->req + tail;
			debug(XSequenceDebug, ("\nSeq #%d: %s ",
				seq->x11seq[ tail ],
				X10ProtoNames[ preq[ tail ].code ]));
			if (event->serial <= seq->x11seq[ tail ])
				break;
			tail = NextSeqPtr(seq->last, tail, 1);
		} while (tail != seq->head);
	}
#ifdef DEBUG
	if (debugged(preq->code)) {
		XDefaultError(dpy, event);
		bugout("  Perhaps occuring in\n    ");
		ShowReq(client, preq);
		bugout("\n");
	}
	debug(XSequenceDebug, ("Error on Seq %d:\n ", event->serial));
#endif DEBUG

	/*
	 * Clear outstanding requests before this.  This assignment may
	 * actually  turn out to be a no-op.
	 */
	cp->seq.tail = tail;

	if (event->request_code > sizeof(SynchronousX11Request)
	 || ! SynchronousX11Request[ event->request_code ]) {
		debug(preq->code, ("X11 protocol error: "));
		Xstatus = BadMap[ event->error_code ];
		Oops(client, preq);
	}

	return(0);
}

int MakeInitialX11Connection (displayname)
	char *displayname;
{
	/*
	 * The protocol converter looks like a "client" in the client
	 * structures so that we can receive events.
	 */
	PrimaryClient = fileno(stdout);
	if (! X11connect(displayname, PrimaryClient))
		Error ("%s:  unable to connect to display \"%s\".\n",
			program, displayname);
	PrimaryConnection = clist[ PrimaryClient ]->dpy;
	return ( ConnectionNumber(PrimaryConnection) );
}

AfterFunction(dpy)
	Display		*dpy;
{
#ifdef DEBUG
	int	eventCount = QLength(dpy);

	XSync(dpy, 0);
	if (debugged(XSequenceDebug)) {
		if (QLength(dpy) != eventCount)
			bugout(",%d events,#%d",
				QLength(dpy), NextRequest(dpy));
		else
			bugout(",#%d", NextRequest(dpy));
	}
#else DEBUG
	XSync(dpy, 0);
#endif DEBUG
}

X11Initialize()
{
	Display		*dpy = PrimaryConnection;
	WinInfoptr	wp;
	XGCValues	gcv;

	InitVisualAndColorMap(dpy);	/* must be first */

	(void) XQueryBestTile(dpy, DefaultRootWindow(dpy),
		32, 32, &BestWidth, &BestHeight);

	XSetErrorHandler(X11ErrorHandler);
	XSetIOErrorHandler(X11IOErrorHandler);
	if (DoSync)
		(void) XSetAfterFunction(dpy, AfterFunction);

	/*
	 * Create a window structure containing the root window.
	 */
	wp = AddWin(PrimaryClient,
		    DefaultRootWindow(dpy),
		    (WinInfoptr)NULL,
		    False);

	/*
	 * Get enterNotify and leaveNotify to update our server times.
	 */
	XSelectInput(dpy, DefaultRootWindow(dpy),
		EnterWindowMask | LeaveWindowMask);

	/*
	 * XXX The best we can do here is to create what we think
	 * is the root tile and stick it in here... but that
	 * doesn't allow for the root tile having been changed.  This
	 * is unfortunate, but it is not possible to read a window's
	 * tile.
	 */
	wp->tile = ParentRelative;

	/*
	 * The default for the rootwindow is ClipModeDrawThru....
	 * so set the gc accordingly.
	 */
	gcv.subwindow_mode = IncludeInferiors;
	wp->gc = XCreateGC(dpy, DefaultRootWindow(dpy), GCSubwindowMode, &gcv);
	wp->drawThrough = True;

	wp->height = DisplayHeight(dpy, DefaultScreen(dpy));
	wp->height = DisplayWidth(dpy, DefaultScreen(dpy));
	wp->mapped = True;
	wp->mapRequested = True;

	XFlush(dpy);
}
