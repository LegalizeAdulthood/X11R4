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

Cursor cursor;
XFontStruct *f;
Pixel red, blue, plum;
Boolean status;
Widget inActive;

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

static void Activate1(w, closure, call_data)
	Widget w;
	caddr_t closure;
	caddr_t call_data;
{
	static Arg args[2];

	if(status) {
	    XtSetArg(args[0], XtNbackground, (XtArgVal)blue);
	    XtSetArg(args[1], StNlabel, (XtArgVal)"?");
	} else {
	    XtSetArg(args[0], XtNbackground, (XtArgVal)red);
	    XtSetArg(args[1], StNlabel, (XtArgVal)"!");
	}
	XtSetValues(w, args, XtNumber(args));

	status = !status;
}

static void Activate2(w, closure, call_data)
	Widget w;
	caddr_t closure;
	caddr_t call_data;
{
	static Arg args[1];

	XtSetArg(args[0], XtNsensitive, (XtArgVal)TRUE);
	XtSetValues(inActive, args, XtNumber(args));

	XtSetArg(args[0], XtNsensitive, (XtArgVal)FALSE);
	XtSetValues(w, args, XtNumber(args));

	inActive = w;
}

static void Exit(w, closure, call_data)
	Widget w;
	caddr_t closure;
	caddr_t call_data;
{
	exit(0);
}

void main(argc, argv)
	unsigned int argc;
	char **argv;
{
	static XtCallbackRec callbacks1[] = {
	    { Activate1, NULL },
	    { NULL, NULL },
	};
	static XtCallbackRec callbacks2[] = {
	    { Activate2, NULL },
	    { NULL, NULL },
	};
	static XtCallbackRec callbacks3[] = {
	    { Exit, NULL },
	    { NULL, NULL },
	};
	static Arg btn_args[] = {
	    { XtNx, (XtArgVal)2},
	    { XtNy, (XtArgVal)2},
	    { XtNwidth, (XtArgVal) 50},
	    { XtNheight, (XtArgVal) 30},
	    { XtNbackground, (XtArgVal) 2},
	    { XtNborderWidth, (XtArgVal) 4},
	    { StNcursor, (XtArgVal) 146},
            { StNlabel, (XtArgVal)"test"},
            { StNfont, (XtArgVal) NULL},
            { StNcallback, (XtArgVal)callbacks1},
	    { XtNsensitive, (XtArgVal)TRUE},
	};
	static Arg box_args[] = {
	    { XtNwidth, (XtArgVal) 164},
	    { XtNheight, (XtArgVal) 86},
	};
	Widget toplevel, box;

	toplevel = XtInitialize( NULL, "Demo", NULL, 0, &argc, argv);

	blue = GetColor(toplevel, "light blue");
	red = GetColor(toplevel, "red");
	plum = GetColor(toplevel, "plum");

	cursor = XCreateFontCursor(XtDisplay(toplevel), 60);
	f = XLoadQueryFont(XtDisplay(toplevel), "variable");
	status = FALSE;

	box = XtCreateManagedWidget("box", stViewManagerWidgetClass, toplevel, 
		box_args, XtNumber(box_args));

	XtSetArg(btn_args[4], XtNbackground, (XtArgVal)blue);
	XtSetArg(btn_args[6], StNcursor, (XtArgVal)cursor);
	XtSetArg(btn_args[7], StNlabel, (XtArgVal)"?");
	XtSetArg(btn_args[8], StNfont, (XtArgVal)f);
	XtCreateManagedWidget( "button1", stButtonWidgetClass, box,
	   btn_args, XtNumber(btn_args));

	XtSetArg(btn_args[7], StNlabel, (XtArgVal)"OK");
	XtSetArg(btn_args[1], XtNy, (XtArgVal)54);
	XtSetArg(btn_args[9], StNcallback, (XtArgVal)callbacks3);
	XtCreateManagedWidget( "button2", stButtonWidgetClass, box,
	   btn_args, XtNumber(btn_args));

	XtSetArg(btn_args[0], XtNx, (XtArgVal)62);
	XtSetArg(btn_args[1], XtNy, (XtArgVal)2);
	XtSetArg(btn_args[3], XtNheight, (XtArgVal)40);
	XtSetArg(btn_args[4], XtNbackground, (XtArgVal)plum);
	XtSetArg(btn_args[2], XtNwidth, (XtArgVal)100);
	XtSetArg(btn_args[7], StNlabel, (XtArgVal)"SENSER");
	XtSetArg(btn_args[9], StNcallback, (XtArgVal)callbacks2);
	XtCreateManagedWidget( "button3", stButtonWidgetClass, box,
	   btn_args, XtNumber(btn_args));

	XtSetArg(btn_args[1], XtNy, (XtArgVal)44);
	XtSetArg(btn_args[10], XtNsensitive, (XtArgVal)FALSE);
	inActive = XtCreateManagedWidget( "button4", stButtonWidgetClass, box,
	   btn_args, XtNumber(btn_args));

	XtRealizeWidget(toplevel);
	XtMainLoop();
}
