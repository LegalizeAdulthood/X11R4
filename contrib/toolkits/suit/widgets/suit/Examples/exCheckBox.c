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
#include <St/StViewMgr.h>
#include <St/StButton.h>
#include <St/StLabel.h>
#include <St/StCkBxSw.h>

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
	    { XtNx, (XtArgVal)58},
	    { XtNy, (XtArgVal)115},
	    { XtNwidth, (XtArgVal) 40},
	    { XtNheight, (XtArgVal) 30},
	    { XtNbackground, (XtArgVal) 0},
	    { XtNborderWidth, (XtArgVal) 2},
	    { StNlabel, (XtArgVal)"NG"},
	    { StNfont, (XtArgVal)NULL},
	    { StNcursor, (XtArgVal)NULL},
	    { StNcallback, (XtArgVal)callbacks},
	};
	Pixel lightBlue;

	lightBlue = GetColor(w, "light blue");

	XtSetArg(btn_args[4], XtNbackground, (XtArgVal)lightBlue);
	XtSetArg(btn_args[8], StNcursor, (XtArgVal)cursor);
	XtSetArg(btn_args[7], StNfont, (XtArgVal)f);
	XtCreateManagedWidget( "NG", stButtonWidgetClass, w, btn_args,
	    XtNumber(btn_args));

	XtSetArg(btn_args[0], XtNx, (XtArgVal)100);
	XtSetArg(btn_args[6], StNlabel, (XtArgVal)"OK");
	XtCreateManagedWidget( "OK", stButtonWidgetClass, w, btn_args,
	    XtNumber(btn_args));
}

void main(argc, argv)
	unsigned int argc;
	char **argv;
{
	static Arg sw_args[] = {
	    { XtNx, (XtArgVal)5},
	    { XtNy, (XtArgVal)10},
	    { XtNwidth, (XtArgVal) 15},
	    { XtNheight, (XtArgVal) 15},
	    { XtNborderWidth, (XtArgVal) 0},
            { StNforeground, (XtArgVal)2},
	    { StNcursor, (XtArgVal) 146},
            { StNlabel, (XtArgVal)"RED"},
            { StNfont, (XtArgVal) NULL},
	    { XtNsensitive, (XtArgVal)TRUE},
            { XtNbackground, (XtArgVal)2},
	};
	static Arg label_args[] = {
	    { XtNx, (XtArgVal)35},
	    { XtNy, (XtArgVal)5},
	    { XtNwidth, (XtArgVal) 120},
	    { XtNheight, (XtArgVal) 25},
	    { XtNborderWidth, (XtArgVal) 0},
	    { StNcursor, (XtArgVal) 146},
            { StNlabel, (XtArgVal)"MELON SHAKE"},
            { StNfont, (XtArgVal) NULL},
	    { XtNsensitive, (XtArgVal)TRUE},
	    { StNjustify, (XtArgVal)XtJustifyLeft},
	};
	static Arg box_args[] = {
	    { XtNwidth, (XtArgVal)160},
	    { XtNheight, (XtArgVal) 140},
	};
	Pixel red, plum, blue, white;
	Widget toplevel, box;

	toplevel = XtInitialize( NULL, "Demo", NULL, 0, &argc, argv);

	red = GetColor(toplevel, "red");
	plum = GetColor(toplevel, "plum");
	blue = GetColor(toplevel, "blue");
	white = GetColor(toplevel, "white");
	cursor = XCreateFontCursor(XtDisplay(toplevel), 68);
	f = XLoadQueryFont(XtDisplay(toplevel), "variable");

	box = XtCreateManagedWidget("box", stViewManagerWidgetClass, toplevel, 
		box_args, XtNumber(box_args));

	XtSetArg(sw_args[5], StNforeground, (XtArgVal)red);
	XtSetArg(sw_args[6], StNcursor, (XtArgVal)cursor);
	XtSetArg(sw_args[8], StNfont, (XtArgVal)f);
	XtSetArg(sw_args[10], XtNbackground, (XtArgVal)plum);
	XtCreateManagedWidget( "sw1", stCheckBoxSwitchWidgetClass, box,
	    sw_args, XtNumber(sw_args));

	XtSetArg(sw_args[1], XtNy, (XtArgVal)35);
	XtCreateManagedWidget( "sw2", stCheckBoxSwitchWidgetClass, box,
	    sw_args, XtNumber(sw_args));

	XtSetArg(sw_args[1], XtNy, (XtArgVal)60);
	XtCreateManagedWidget( "sw3", stCheckBoxSwitchWidgetClass, box,
	    sw_args, XtNumber(sw_args));

	XtSetArg(sw_args[1], XtNy, (XtArgVal)85);
	XtSetArg(sw_args[9], XtNsensitive, (XtArgVal)FALSE);
	XtCreateManagedWidget( "sw4", stCheckBoxSwitchWidgetClass, box,
	    sw_args, XtNumber(sw_args));

	XtSetArg(label_args[5], StNcursor, (XtArgVal)cursor);
	XtSetArg(label_args[7], StNfont, (XtArgVal)f);
	XtCreateManagedWidget( "label1", stLabelWidgetClass, box,
	    label_args, XtNumber(label_args));

	XtSetArg(label_args[1], XtNy, (XtArgVal)30);
	XtSetArg(label_args[6], StNlabel, (XtArgVal)"BANANA SHAKE");
	XtCreateManagedWidget( "label2", stLabelWidgetClass, box,
	    label_args, XtNumber(label_args));

	XtSetArg(label_args[1], XtNy, (XtArgVal)55);
	XtSetArg(label_args[6], StNlabel, (XtArgVal)"PEACH SHAKE");
	XtCreateManagedWidget( "label3", stLabelWidgetClass, box,
	    label_args, XtNumber(label_args));

	XtSetArg(label_args[1], XtNy, (XtArgVal)80);
	XtSetArg(label_args[6], StNlabel, (XtArgVal)"ORANGE SHAKE");
	XtSetArg(label_args[8], XtNsensitive, (XtArgVal)FALSE);
	XtCreateManagedWidget( "label4", stLabelWidgetClass, box,
	    label_args, XtNumber(label_args));

	CreateOKButton(box);

	XtRealizeWidget(toplevel);
	XtMainLoop();
}
