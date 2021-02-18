/************************************************************
Copyright 1989 by Information-Technology Promotion Agency Japan (IPA),
Mitsubishi Research Institute, Inc. (MRI), and Fuji Xerox Co., Ltd. (FX)

                        All Rights Reserved

Permission to use, copy, modify and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IPA, MRI or FX not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IPA, MRI or FX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL IPA, MRI or FX BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

********************************************************/

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Box.h>
#include <St/StViewMgr.h>
#include <St/StButton.h>
#include <St/StSltBxSw.h>

Widget active;
Cursor cursor;
XFontStruct *f;

static Pixel GetColor(w, c)
	Widget w;
	char *c;
{
	Screen *screen;
	XColor visual, exact;

	screen = XtScreen(w);
	XAllocNamedColor((*screen).display, (*screen).cmap, c,
		&visual, &exact);
	return visual.pixel;
}

static void Activate(w, closure, call_data)
	Widget w;
	caddr_t closure;
	caddr_t call_data;
{
	static Arg args[] = {
	    {StNcontents, (XtArgVal)FALSE},
	};

	XtSetValues(active, args, XtNumber(args));
	active = w;
}

static void Exit(w, closure, call_data)
	Widget w;
	caddr_t closure;
	caddr_t call_data;
{
	exit(0);
}

static void CreateOKButton(w)
	Widget w;
{
	static XtCallbackRec callbacks[] = {
	    { Exit, NULL },
	    { NULL, NULL },
	};
	static Arg btn_args[] = {
	    { XtNx, (XtArgVal)148},
	    { XtNy, (XtArgVal)55},
	    { XtNwidth, (XtArgVal) 40},
	    { XtNheight, (XtArgVal) 30},
	    { XtNbackground, (XtArgVal) 0},
	    { XtNborderWidth, (XtArgVal) 2},
	    { StNlabel, (XtArgVal)"OK"},
	    { StNfont, (XtArgVal)NULL},
	    { StNcursor, (XtArgVal)NULL},
	    { StNcallback, (XtArgVal)callbacks},
	};
	Pixel lightBlue;

	lightBlue = GetColor(w, "light blue");

	XtSetArg(btn_args[4], XtNbackground, (XtArgVal)lightBlue);
	XtSetArg(btn_args[8], StNcursor, (XtArgVal)cursor);
	XtSetArg(btn_args[7], StNfont, (XtArgVal)f);
	XtCreateManagedWidget( "OK", stButtonWidgetClass, w, btn_args,
	    XtNumber(btn_args));
}

void main(argc, argv)
	unsigned int argc;
	char **argv;
{
	static XtCallbackRec callbacks[] = {
	    { Activate, NULL },
	    { NULL, NULL },
	};
	static Arg sw_args[] = {
	    { XtNx, (XtArgVal)5},
	    { XtNy, (XtArgVal)5},
	    { XtNwidth, (XtArgVal) 60},
	    { XtNheight, (XtArgVal) 30},
	    { XtNborderWidth, (XtArgVal) 0},
	    { XtNbackground, (XtArgVal) 2},
            { StNforeground, (XtArgVal)2},
	    { StNcursor, (XtArgVal) 146},
            { StNcontents, (XtArgVal)FALSE},
            { StNlabel, (XtArgVal)"RED"},
            { StNfont, (XtArgVal) NULL},
            { StNcallback, (XtArgVal)callbacks},
	};
	static Arg box_args[] = {
	    { XtNwidth, (XtArgVal)190},
	    { XtNheight, (XtArgVal) 40},
	};
	Pixel red, green, blue, white;
	Widget toplevel, box;

	toplevel = XtInitialize( NULL, "Demo", NULL, 0, &argc, argv);

	red = GetColor(toplevel, "red");
	green = GetColor(toplevel, "green");
	blue = GetColor(toplevel, "blue");
	white = GetColor(toplevel, "white");
	cursor = XCreateFontCursor(XtDisplay(toplevel), 68);
	f = XLoadQueryFont(XtDisplay(toplevel), "variable");

	box = XtCreateManagedWidget("box", stViewManagerWidgetClass, toplevel, 
		box_args, XtNumber(box_args));

	XtSetArg(sw_args[5], XtNbackground, (XtArgVal)white);
	XtSetArg(sw_args[6], StNforeground, (XtArgVal)red);
	XtSetArg(sw_args[7], StNcursor, (XtArgVal)cursor);
	XtSetArg(sw_args[10], StNfont, (XtArgVal)f);
	XtCreateManagedWidget( "red", stSelectBoxSwitchWidgetClass, box,
	    sw_args, XtNumber(sw_args));

	XtSetArg(sw_args[0], XtNx, (XtArgVal)65);
	XtSetArg(sw_args[6], StNforeground, (XtArgVal)green);
	XtSetArg(sw_args[9], StNlabel, (XtArgVal)"GREEN");
	XtSetArg(sw_args[8], StNcontents, (XtArgVal)TRUE);
	active = XtCreateManagedWidget( "green", stSelectBoxSwitchWidgetClass, 
	    box, sw_args, XtNumber(sw_args));

	XtSetArg(sw_args[0], XtNx, (XtArgVal)125);
	XtSetArg(sw_args[6], StNforeground, (XtArgVal)blue);
	XtSetArg(sw_args[9], StNlabel, (XtArgVal)"BLUE");
	XtSetArg(sw_args[8], StNcontents, (XtArgVal)FALSE);
	XtCreateManagedWidget( "blue", stSelectBoxSwitchWidgetClass, box,
	    sw_args, XtNumber(sw_args));

	CreateOKButton(box);

	XtRealizeWidget(toplevel);
	XtMainLoop();
}
