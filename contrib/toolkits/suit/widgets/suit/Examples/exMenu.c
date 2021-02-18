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
#include <X11/Command.h>
#include <St/StPpStViw.h>
#include <St/StMenu.h>


Widget popup;

static void menu_action(w, closure, call_data)
	Widget w;
	caddr_t closure;
	caddr_t call_data;
{
	printf("user %d called !!\n", closure);
	XtPopdown(popup);
}

static void PopupView(w, closure, call_data)
	Widget w;
	caddr_t closure;
	caddr_t call_data;
{
	XtPopup(popup, XtGrabNone/*XtGrabExclusive*/);
}

static void CreateMenu(parent, w)
	Widget parent;
	Widget w;
{
	XFontStruct *f;
	Cursor cursor;
	static Arg shell_args[] = {
		{ XtNx, (XtArgVal) 150},
		{ XtNy, (XtArgVal) 180},
	};
	static Arg view_args[] = {
		{ XtNwidth, (XtArgVal) 100},
		{ XtNheight, (XtArgVal) 150},
		{ StNtitle, (XtArgVal) "MENU EXAMPLE"},
		{ StNleftMargin, (XtArgVal) 0},
		{ StNrightMargin, (XtArgVal) 0},
		{ StNtopMargin, (XtArgVal) 0},
		{ StNbottomMargin, (XtArgVal) 0},
	};
	static StMenuItem menu_label[] = {
		{ "menu 1", menu_action, False , 20},
		{ "menu 2", menu_action, False , 20},
		{ "menu 3", menu_action, False , 20},
		{ "menu 4", menu_action, False , 20},
		{ "menu 5", menu_action, True , 20},
		{ "menu 6", menu_action, False , 20},
		{ "menu 7", menu_action, False , 20},
		{ "menu 8", menu_action, False , 20},
		{ "menu 9", menu_action, False , 20},
		{ "menu 10", menu_action, True , 20},
		{ "menu 11", menu_action, False , 20},
		{ NULL, NULL, False , 20}
	};
	static Arg menu_args[] = {
		{ StNmenuFont, 	(XtArgVal) NULL},
		{ StNmenuCursor, (XtArgVal) NULL},
		{ StNmenuItems, (XtArgVal) menu_label}, 
		{ StNselect, 	(XtArgVal) "<Btn1Up>"},
		{ XtNwidth, 	(XtArgVal) 100 },
		{ XtNheight, 	(XtArgVal) 150 },
		/*{ StNverticalScrollBar, (XtArgVal) True},
		{ StNscrollBarOnRight, (XtArgVal) True},*/
	};
	Widget view;

	popup = XtCreatePopupShell("popup", overrideShellWidgetClass, w, 
			shell_args, XtNumber(shell_args));

	f = XLoadQueryFont(XtDisplay(popup), "variable");
	cursor = XCreateFontCursor(XtDisplay(popup), 68);

	view = XtCreateManagedWidget("view", stPopupSheetViewWidgetClass, popup,
			view_args, XtNumber(view_args));

	XtSetArg(menu_args[0], StNmenuFont, f);
	XtSetArg(menu_args[1], StNmenuCursor, cursor);
	XtCreateManagedWidget("menu", stMenuWidgetClass, view,
		menu_args, XtNumber(menu_args) );
}

void main(argc, argv)
	unsigned int argc;
	char **argv;
{
	static XtCallbackRec callbacks[] = {
	    { PopupView, NULL },
	    { NULL, NULL },
	};
	static Arg com_args[] = {
	    { XtNcallback, (XtArgVal)callbacks },
	    { XtNlabel, (XtArgVal)"PRESS ME !!" },
	    { XtNwidth, (XtArgVal) 60},
	    { XtNheight, (XtArgVal) 40},
	    { XtNborderWidth, (XtArgVal) 3},
	};
	Widget toplevel, button;

	toplevel = XtInitialize( NULL, "Demo", NULL, 0, &argc, argv);

	button = XtCreateManagedWidget( "popup-button",commandWidgetClass, 
			      toplevel, com_args, XtNumber(com_args));

	CreateMenu(NULL, button);

	XtRealizeWidget(toplevel);
	XtMainLoop();
}
