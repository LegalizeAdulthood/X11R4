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

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Box.h>
#include <X11/Command.h>
#include <St/StPpStViw.h>


static void PopdownView(w, popup, call_data)
	Widget w;
	Widget popup;
	caddr_t call_data;
{
	XtPopdown(popup);
}

static void PopupView(w, closure, call_data)
	Widget w;
	caddr_t closure;
	caddr_t call_data;
{
	static XtCallbackRec popdown_callback[] = {
		{ PopdownView, NULL },
		{ NULL, NULL },
	};
	static Arg view_args[] = {
		{ XtNwidth, (XtArgVal) 110},
		{ XtNheight, (XtArgVal) 140},
		{ XtNborderWidth, (XtArgVal) 2},
		{ StNtitle, (XtArgVal) "Popup Sheet "},
		{ StNleftMargin, (XtArgVal) 10},
		{ StNrightMargin, (XtArgVal) 10},
		{ StNtopMargin, (XtArgVal) 10},
		{ StNbottomMargin, (XtArgVal) 10},
		{ StNwindowable, (XtArgVal)FALSE},
		{ NULL, NULL},
	};
	static Arg button1_args[] = {
		{ XtNcallback, (XtArgVal)popdown_callback },
		{ XtNlabel, (XtArgVal)"Popdown" },
		{ XtNx, (XtArgVal) 0},
		{ XtNy, (XtArgVal) 0},
		{ XtNwidth, (XtArgVal) 80},
		{ XtNheight, (XtArgVal) 80},
	};
	Widget popup, view, box;
	XFontStruct *f = XLoadQueryFont(XtDisplay(w), "variable");

	popup = XtCreatePopupShell("popup", topLevelShellWidgetClass, w, NULL, 0);
	XtSetArg(view_args[9], StNfont, f);
	view = XtCreateManagedWidget( "view", stPopupSheetViewWidgetClass, popup,
				view_args, XtNumber(view_args));

	box = XtCreateManagedWidget("box", boxWidgetClass, view, NULL, 0);

	popdown_callback[0].closure = (caddr_t)popup;
	XtCreateManagedWidget("popdown", commandWidgetClass, box,
				button1_args, XtNumber(button1_args) );

	XtPopup(popup, XtGrabNone);
}

void main(argc, argv)
    unsigned int argc;
    char **argv;
{
    static XtCallbackRec callbacks[] = {
      { PopupView, NULL },
      { NULL, NULL },
    };
    static Arg commandArgs[] = {
	{ XtNcallback, (XtArgVal)callbacks },
	{ XtNlabel, (XtArgVal)"POP" },
	{ XtNwidth, (XtArgVal) 50},
	{ XtNheight, (XtArgVal) 50},
	{ XtNborderWidth, (XtArgVal) 2},
    };
    Widget toplevel;

    toplevel = XtInitialize( NULL, "Demo", NULL, 0, &argc, argv);

    XtCreateManagedWidget( "popup-button",commandWidgetClass, 
			      toplevel, commandArgs, XtNumber(commandArgs) );

    XtRealizeWidget(toplevel);
    XtMainLoop();
}
