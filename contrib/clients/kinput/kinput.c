/*
 *	kinput.c -- a Japanese conversion server for X11
 *
 *		ishisone@sra.co.jp
 */

/*
 * Copyright (C) 1988 by Software Research Associates, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Software Research
 * Associates makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * Author:  Makoto Ishisone, Software Research Associates, Inc., Japan
 *		ishisone@sra.co.jp
 */

#ifndef lint
static char	*rcsid = "$Header: /private/usr/mmb/ishisone/job/x11/kinput/RCS/kinput.c,v 3.2 89/10/27 17:23:50 ishisone Rel $";
#endif

#include <stdio.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#ifdef CHECK_PROTOTYPE
#include <X11/Xlib.p>
#include <X11/Intrinsic.p>
#endif
#include <X11/Shell.h>
#ifdef HEADER_IS_HERE
#include "WStr.h"
#include "XWStr.h"
#else
#include <WStr.h>
#include <X11/XWStr.h>
#endif
#include "jslib.h"
#include "jclib.h"
#include "jilib.h"
#include "cconv.h"
#include "KanjiInput.h"

/*#define DEBUG*/
#ifdef DEBUG
#define PRINT(a)	debugprint a
#else
#define PRINT(a)
#endif

typedef struct {
	Position	x, y;
	Dimension	width, height;
} Geo;

typedef struct _convclient {
	Widget	kkshell;	/* popup transient shell */
	Widget	kkconv;		/* kanji conversion widget */
	Window	probe;		/* event probe window */
	XEvent	convreq;	/* CONVERSION_REQUEST event */
	Window	reqwin;		/* conversion requestor window */
	Window	rootwin;	/* root window of reqwin */
	Geo	geometry;	/* geometry information of reqwin */
	struct _convclient	*next;
} ConvClient;

static ConvClient	*convclients;

/* Atoms */
static Atom	convAtom;	/* "JAPANESE_CONVERSION" */
static Atom	propAtom;	/* "CONVERSION_STRING" */
static Atom	ctextAtom;	/* "COMPOUND_TEXT" */
static Atom	reqAtom;	/* "CONVERSION_REQUEST" */
static Atom	notifyAtom;	/* "CONVERSION_NOTIFY" */
static Atom	endAtom;	/* "CONVERSION_END" */
static Atom	endReqAtom;	/* "CONVERSION_END_REQUEST" */

static XrmOptionDescRec	options[] = {
	{"-jserver",	"*conv.jserver",	XrmoptionSepArg,	NULL},
	{"-ccdef",	"*conv.ccdef",		XrmoptionSepArg,	NULL},
	{"-wnnenvname",	"*conv.wnnEnvname",	XrmoptionSepArg,	NULL},
	{"-wnnenvrc",	"*conv.wnnEnvrc",	XrmoptionSepArg,	NULL},
	{"-nlines",	"*conv.nlines",		XrmoptionSepArg,	NULL},
	{"-font",	"*conv.font",		XrmoptionSepArg,	NULL},
	{"-kanjifont",	"*conv.kanjiFont",	XrmoptionSepArg,	NULL},
};


#if __STDC__ == 1
static ConvClient	*findConverter(Window);
static ConvClient	*getFreeConverter(Widget, Window, Window);
static ConvClient	*getPopup(Widget, Window);
static Widget	createConvWidget(ConvClient *, char *, Widget, Screen *);
static void	makeInitialConverter(Widget, int);
static void	addConverter(ConvClient *);
static Boolean	setConvSelection(Widget);
static void	selReq(Widget, XEvent *);
static void	selClr(Widget, XEvent *);
static void	clMsg(Widget, XEvent *);
static void	probeEvent(XEvent *);
static void	sendConvNotify(XEvent *, Atom, Window);
static void	fixProc(Widget, caddr_t, wchar_t *);
static void	endProc(Widget, caddr_t);
static void	fatalError(char *);
static int	dicConfirm(Widget, int, char *);
#else
static ConvClient	*findConverter();
static ConvClient	*getFreeConverter();
static ConvClient	*getPopup();
static Widget	createConvWidget();
static void	makeInitialConverter();
static void	addConverter();
static Boolean	setConvSelection();
static void	selReq();
static void	selClr();
static void	clMsg();
static void	getPlace();
static void	probeEvent();
static void	sendConvNotify();
static void	fixProc();
static void	endProc();
static void	fatalError();
static int	dicConfirm();
#endif

/* translation table for toplevel widget */
static char	overridetrans[] =
	"<Message>:	client-message()\n\
	 <SelReq>:	selection-request()\n\
	 <SelClr>:	selection-clear()";

static XtActionsRec	actions[] = {
{"selection-request",	selReq},
{"selection-clear",	selClr},
{"client-message",	clMsg},
};

Widget		toplevel;

extern int	doConfirm();

void
main(ac, av)
int ac;
char **av;
{
	XtTranslations	trans;
	int		i;
	XEvent		event;
	Arg		args[20];
	Display		*disp;

	toplevel = XtInitialize("main", "XJconv", options, XtNumber(options),
				&ac,av);
	disp = XtDisplay(toplevel);

	XtAddActions(actions, XtNumber(actions));
	trans = XtParseTranslationTable(overridetrans);

	/*
	 * don't map toplevel window.
	 * but must specify the dimension, or you will get error.
	 */
	i = 0;
	XtSetArg(args[i], XtNmappedWhenManaged, False); i++;
	XtSetArg(args[i], XtNwidth, 10); i++;
	XtSetArg(args[i], XtNheight, 10); i++;
	XtSetArg(args[i], XtNtranslations, trans); i++;
	XtSetValues(toplevel, args, i);

	/* create Atoms */
	convAtom = XInternAtom(disp, "JAPANESE_CONVERSION", False);
	propAtom = XInternAtom(disp, "CONVERSION_STRING", False);
	ctextAtom = XInternAtom(disp, "COMPOUND_TEXT", False);
	reqAtom = XInternAtom(disp, "CONVERSION_REQUEST", False);
	notifyAtom = XInternAtom(disp, "CONVERSION_NOTIFY", False);
	endAtom = XInternAtom(disp, "CONVERSION_END", False);
	endReqAtom = XInternAtom(disp, "CONVERSION_END_REQUEST", False);

	/* set error handler */
	XtSetErrorHandler(fatalError);

	XtRealizeWidget(toplevel);
	makeInitialConverter(toplevel, 1);

	/* set Selection */
	if (setConvSelection(toplevel) == False) {
		/* can't own selection */
		XtError("can't own selection");
	}

	/* main loop */
	for (;;) {
		XtNextEvent(&event);
		probeEvent(&event);
		XtDispatchEvent(&event);
	}
}

static void
makeInitialConverter(toplevel, n)
Widget toplevel;
int n;
{
	int		i;
	ConvClient	*ccp;
	Window		root = RootWindowOfScreen(XtScreen(toplevel));

	for (i = 0; i < n; i++) {
		ccp = XtNew(ConvClient);
		createConvWidget(ccp, "kanjiInput", toplevel, XtScreen(toplevel));
		ccp->probe = None;
		ccp->reqwin = None;
		ccp->rootwin = root;
		addConverter(ccp);
	}

	PRINT(("makeInitialConverter: create new\n"));
}
                                   
static void
addConverter(ccp)
ConvClient *ccp;
{
	ConvClient	*tmp = convclients;

	convclients = ccp;
	ccp->next = tmp;
}

static ConvClient *
findConverter(win)
Window win;
{
	ConvClient	*ccp;

	for (ccp = convclients; ccp; ccp = ccp->next) {
		if (ccp->reqwin == win && ccp->probe != None) {
			return ccp;
		}
	}
	return NULL;
}

static ConvClient *
getFreeConverter(widget, win, root)
Widget widget;
Window win;
Window root;
{
	Display		*dpy = XtDisplay(widget);
	Screen		*screen;
	ConvClient	*ccp;
	int		i;

	/*
	 * find an entry that has same root and same window
	 */
	for (ccp = convclients; ccp; ccp = ccp->next) {
		if (ccp->rootwin == root && ccp->reqwin == win) {
			if (ccp->probe != None) {
				/* 使用中 */
				PRINT(("getFreeConverter: in use\n"));
				return NULL;
			} else {
				PRINT(("getFreeConverter: same window found (%d)\n", i));
				return ccp;
			}
		}
	}

	/*
	 * if failed, then find an entry that has same root (ie same screen)
	 */
	for (ccp = convclients; ccp; ccp = ccp->next) {
		if (ccp->rootwin == root && ccp->probe == None) {
			PRINT(("getFreeConverter: same root found (%d)\n", i));
			return ccp;
		}
	}

	/* get screen */
	screen = NULL;
	for (i = 0; i < ScreenCount(dpy); i++) {
		if (root == RootWindow(dpy, i)) {
			screen = ScreenOfDisplay(dpy, i);
			break;
		}
	}
	if (screen == NULL) {
		PRINT(("getFreeConverter: screen not found for root %d\n", root));
		return NULL;
	}

	ccp = XtNew(ConvClient);
	createConvWidget(ccp, "kanjiInput", widget, screen);
	ccp->probe = None;
	ccp->reqwin = None;
	ccp->rootwin = root;
	addConverter(ccp);

	PRINT(("getFreeConverter: create new\n"));
	return ccp;
}

static ConvClient *
getPopup(widget, reqwin)
Widget widget;
Window reqwin;
{
	Display		*dpy = XtDisplay(widget);
	Window		root;
	Window		junkwin;
	int		reqx, reqy;
	unsigned int	reqwidth, reqheight, reqborder, junk;
	Dimension	dpyWidth, dpyHeight;
	ConvClient	*ccp;
	Dimension	kwidth, kheight;
	Position	kx, ky;
	Arg		args[2];

	/* get the position and the dimension of the requestor window */
	XGetGeometry(dpy, reqwin, &root, &reqx, &reqy,
		     &reqwidth, &reqheight, &reqborder, &junk);
	XTranslateCoordinates(dpy, reqwin, root,
			      0, 0, &reqx, &reqy, &junkwin);

	/* find an unused converter */
	if ((ccp = getFreeConverter(widget, reqwin, root)) == NULL) {
		return NULL;
	}
		
	ccp->reqwin = reqwin;

	/* create a event probe window  */
	ccp->probe = XCreateWindow(dpy, reqwin, 0, 0, 9999, 9999,
				   0, 0, InputOnly, CopyFromParent, 0, NULL);

	/* select KeyPress, KeyRelease and DestroyNotify event */
	XSelectInput(dpy, ccp->probe,
		     KeyPressMask|KeyReleaseMask|StructureNotifyMask);
	XMapWindow(dpy, ccp->probe);

	/* if requestor window's position and dimension is same as
	 * last used, don't reposition pop-up window.
	 */
	if (reqwin == ccp->reqwin &&
	    reqx == ccp->geometry.x &&
	    reqy == ccp->geometry.y &&
	    reqwidth == ccp->geometry.width &&
	    reqheight == ccp->geometry.height) {
		return ccp;
	}

	ccp->geometry.x = reqx;
	ccp->geometry.y = reqy;
	ccp->geometry.width = reqwidth;
	ccp->geometry.height = reqheight;

	/* get the dimension of the popup */
	XtSetArg(args[0], XtNwidth, &kwidth);
	XtSetArg(args[1], XtNheight, &kheight);
	XtGetValues(ccp->kkconv, args, 2);

	/* place the popup just below the requestor window */
	kx = reqx + reqwidth / 2 - kwidth / 2;
	ky = reqy + reqheight + 5;

	/* check if the popup is out of the screen */

	dpyWidth = WidthOfScreen(XtScreen(ccp->kkshell));
	dpyHeight = HeightOfScreen(XtScreen(ccp->kkshell));

	/* x-dimension */
	if (kx + kwidth > dpyWidth)
		kx = dpyWidth - kwidth;	/* move left */
	if (kx < 0)
		kx = 0;			/* move right */

	/* y-dimension */
	if (ky + kheight > dpyHeight) {
		/* try just above the requestor */
		if ((ky = reqy - kheight) < 0) {
			/* just above the screen bottom */
			ky = dpyHeight - kheight;
		}
	}

	/* move it */
	XtMoveWidget(ccp->kkshell, kx, ky);

	return ccp;
}

static Widget
createConvWidget(ccp, name, parent, screen)
ConvClient *ccp;
char *name;
Widget parent;
Screen *screen;
/* create popup conversion widget */
{
	Widget	transient, kinput;
	Arg	args[4];
	int	i;

	PRINT(("parent shell = 0x%08x\n", parent));
	i = 0;
	XtSetArg(args[i], XtNinput, True); i++;
	if (screen != XtScreen(parent)) {
		XtSetArg(args[i], XtNscreen, screen); i++;
		XtSetArg(args[i], XtNdepth, DefaultDepthOfScreen(screen)); i++;
		XtSetArg(args[i], XtNcolormap, DefaultColormapOfScreen(screen)); i++;
	}
	transient = XtCreatePopupShell(name, transientShellWidgetClass,
				       parent, args, i);

	XtSetArg(args[0], XtNconfirmFunc, dicConfirm);
	kinput = XtCreateManagedWidget("conv",
				       kanjiInputWidgetClass, transient,
				       args, 1);

	PRINT(("adding callback\n"));
	XtAddCallback(kinput, XtNfixNotify, fixProc, (caddr_t)ccp);
	XtAddCallback(kinput, XtNendNotify, endProc, (caddr_t)ccp);

	ccp->kkshell = transient;
	ccp->kkconv = kinput;
	ccp->reqwin = None;

	return transient;
}

static Boolean
setConvSelection(w)
Widget w;
/* own Selection */
{
	Boolean	res;

	XSetSelectionOwner(XtDisplay(w), convAtom, XtWindow(w), CurrentTime);
	if (XGetSelectionOwner(XtDisplay(w), convAtom) != XtWindow(w))
		res = False;
	else
		res = True;
	PRINT(("selection owner = %d\n", XtWindow(w)));
	return res;
}

/*ARGSUSED*/
static void
selReq(w, event)
Widget w;
XEvent *event;
/* this funcsion is called when SelectionRequest event is received.
 * not support Selection
 */
{
	XSelectionRequestEvent	*ev = &(event->xselectionrequest);
	XEvent			repl;

	repl.xselection.type = SelectionNotify;
	repl.xselection.requestor = ev->requestor;
	repl.xselection.selection = ev->selection;
	repl.xselection.target = ev->target;
	repl.xselection.property = None;
	repl.xselection.time = ev->time;

	XtWarning("got SelectionRequest. ignored.");
	XSendEvent(ev->display, ev->requestor, False, NoEventMask, &repl);
}

static void
clMsg(w, event)
Widget w;
XEvent *event;
/* ClientMessage event handler */
{
	XClientMessageEvent	*ev = &event->xclient;
	ConvClient	*ccp;

	PRINT(("got ClientMessage\n"));
	/* is it a correct event? */
	if (ev->window != XtWindow(w) ||
	    (ev->message_type != reqAtom && ev->message_type != endReqAtom) ||
	    ev->format != 32 ||
	    ev->data.l[0] != convAtom) {
		/*ignore */
		PRINT(("got invalid clientmessage event.\n"));
		return;
	}

	if (ev->message_type == endReqAtom) {
		if ((ccp = findConverter(ev->data.l[1])) == NULL) {
			/* 知らないクライアントから送られてきたので
			 * 無視する
			 */
			PRINT(("got conversion end request from unknown window\n"));
			return;
		}
		KIFixString(ccp->kkconv);
		endProc((Widget)ccp->kkconv, (caddr_t)ccp);
		return;
	}

	if ((ccp = getPopup(w, ev->data.l[1])) == NULL) {
		sendConvNotify(event, None, None);
		return;
	}
	ccp->convreq = *event;

	/* check target type */
#ifdef notdef
	if (ev->data.l[2] == ctextAtom) {
		/* OK */
	} 
#endif
	ccp->convreq.xclient.data.l[2] = ctextAtom;

	/* use default property if not specified */
	if (ev->data.l[3] == None)
		ccp->convreq.xclient.data.l[3] = propAtom;

	/* popup the conversion window */
	XtPopup(ccp->kkshell, XtGrabNone);

	/* send ConversionNotify to the client */
	sendConvNotify(&(ccp->convreq), ccp->convreq.xclient.data.l[3],
		       XtWindow(ccp->kkconv));
}

/* ARGSUSED */
static void
selClr(w, event)
Widget w;
XEvent *event;
{
	ConvClient	*ccp;
	/* Selection owner changed. kill myself */

	/*
	 * send ConversionEnd event to the clients before exit
	 */
	for (ccp = convclients; ccp; ccp = ccp->next) {
		if (ccp->probe != None)
			endProc(w, (caddr_t)ccp);
	}

	XtError("Selection Owner Changed");
	exit(0);
}

static void
probeEvent(event)
XEvent *event;
{
	ConvClient	*ccp;
	Window		win = event->xany.window;

	for (ccp = convclients; ccp; ccp = ccp->next) {
		if (ccp->probe == win) {
			PRINT(("probeEvent: probe window event\n"));
			switch (event->type) {
			case DestroyNotify:
				PRINT(("probewindow destroyed\n"));
				/* destroy probe window */
				ccp->probe = None;
				ccp->reqwin = None;
				/* popdown converter */
				XtPopdown(ccp->kkshell);
				break;
			case KeyPress:
			case KeyRelease:
				/* ugly?  well... yes. */
				/* but this is the easiest way to
				 * fake the toolkit.
				 */
				event->xany.window = XtWindow(ccp->kkconv);
				break;
			}
			return;
		}
	}
}

static void
sendConvNotify(eventp, property, convwin)
XEvent *eventp;
Atom property;
Window convwin;
{
	XClientMessageEvent	*ev = &(eventp->xclient);
	XEvent		event;

	event.xclient.type = ClientMessage;
	event.xclient.window = ev->data.l[1];
	event.xclient.message_type = notifyAtom;
	event.xclient.format = 32;
	event.xclient.data.l[0] = convAtom;
	event.xclient.data.l[1] = ev->data.l[2];
	event.xclient.data.l[2] = property;
	event.xclient.data.l[3] = convwin;

	PRINT(("sending Conversion Notify to Window %d\n", ev->data.l[1]));
	XSendEvent(ev->display, ev->data.l[1], False, NoEventMask, &event);
}

static void
fixProc(w, client, kstring)
Widget w;
caddr_t client;
wchar_t *kstring;
{
	Atom	proptype;
	int	propsize;
	int	propformat;
	caddr_t	propvalue;
	ConvClient	*ccp = (ConvClient *)client;
	XClientMessageEvent	*ev = &(ccp->convreq.xclient);

	/*
	 * データをクライアントがリクエストした型に変換する
	 */
	/*if (ev->data.l[2] == ctextAtom) */{
		int	c;
		wchar_t	*p = kstring;
		/* compound text に変換する */
		/* ただし comound text では \r が送れないので これを \n に
		 * 変換しておく
		 */
		while (c = *p) {
			if (c == '\r') *p = '\n';
			p++;
		}
		proptype = ctextAtom;
		propformat = 8;
		propsize = convJWStoCT(kstring, NULL, 0);
		if (propsize <= 0)
			return;
		propvalue = (caddr_t)XtMalloc(propsize);
		(void)convJWStoCT(kstring, (unsigned char *)propvalue, 0);
	}

	/* Property に結果をセットする */
	XChangeProperty(XtDisplay(w), (Window)ev->data.l[1],
			(Atom)ev->data.l[3], proptype, propformat,
			PropModeAppend, propvalue, propsize);
	XtFree(propvalue);
}

static void
endProc(w, client)
Widget w;
caddr_t client;
{
	ConvClient	*ccp = (ConvClient *)client;
	XClientMessageEvent	*ev = &(ccp->convreq.xclient);
	XEvent	event;

	/* プローブウィンドウを消す */
	XDestroyWindow(XtDisplay(w), ccp->probe);
	ccp->probe = None;

	/* 変換ウィンドウをポップダウンする */
	XtPopdown(ccp->kkshell);

	/* クライアントに ClientMessage を送って知らせる
	 * イベントのフォーマットは、
	 *	window:		requestor window
	 *	message_type:	"CONVERSION_END"
	 *	format:		32
	 *	data.l[0]:	selection
	 *	data.l[1]:	selection-owner window
	 */
	event.xclient.type = ClientMessage;
	event.xclient.window = ev->data.l[1];
	event.xclient.message_type = endAtom;
	event.xclient.format = 32;
	event.xclient.data.l[0] = convAtom;
	event.xclient.data.l[1] = XtWindow(w);
	XSendEvent(XtDisplay(w), ev->data.l[1], False, NoEventMask, &event);
}

static void
fatalError(s)
char	*s;
{
	fprintf(stderr, "X Toolkit Error: %s\n", s);
	exit(1);
}

/* ARGSUSED */
static int
dicConfirm(w, type, file)
Widget w;
int type;
char *file;
{
	char	*title;
	char	line[512];

	if (type == TYPE_DIC) {
		title = "Create Dictionary:";
	} else {
		title = "Create Hindo-file:";
	}

	(void)strcpy(line, "\"");
	(void)strcat(line, file);
	(void)strcat(line, "\" ?");
	return doConfirm(toplevel, title, line);
}

#ifdef DEBUG
debugprint(char *fmt, int a1, int a2, int a3, int a4, int a5)
{
	fprintf(stderr, fmt, a1, a2, a3, a4, a5);
}
#endif
