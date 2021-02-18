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
#include <St/StViewMgr.h>
#include <X11/Command.h>

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

void main(argc, argv)
	unsigned int argc;
	char **argv;
{
	Widget toplevel, box;
	static Arg shell_args[] = {
	    { XtNwidth, (XtArgVal)200},
	    { XtNheight, (XtArgVal)150},
	};
	static Arg com_args[] = {
	    { XtNwidth, (XtArgVal) 80},
	    { XtNheight, (XtArgVal) 80},
	    { XtNx, (XtArgVal) 0},
	    { XtNy, (XtArgVal) 0},
	    { NULL, NULL},
	};
	static Arg box_args[] = {
	    { XtNbackground, (XtArgVal) 1},
            { StNhorizontalScrollBar, (XtArgVal) True}, 
            { StNverticalScrollBar, (XtArgVal) True},
            { StNscrollBarOnTop, (XtArgVal) False},
            { StNscrollBarOnRight,  (XtArgVal) True}
	};
	Pixel lightBlue, skyBlue;

	toplevel = XtInitialize( NULL, "Demo", NULL, 0, &argc, argv);
	XtSetValues(toplevel, shell_args, XtNumber(shell_args));

	lightBlue = GetColor(toplevel, "light blue");
	skyBlue = GetColor(toplevel, "sky blue");

	XtSetArg(box_args[0], XtNbackground, lightBlue);
	box = XtCreateManagedWidget( "base-box", stViewManagerWidgetClass, 
		toplevel, box_args, XtNumber(box_args));

	XtSetArg(com_args[2], XtNx, (XtArgVal)0);
	XtSetArg(com_args[3], XtNy, (XtArgVal)0);
	XtSetArg(com_args[4], XtNbackground, skyBlue);
	XtCreateManagedWidget( "0@0", commandWidgetClass, box, com_args,
		XtNumber(com_args));

	XtSetArg(com_args[2], XtNx, (XtArgVal)100);
	XtSetArg(com_args[3], XtNy, (XtArgVal)0);
	XtCreateManagedWidget( "100@0", commandWidgetClass, box, com_args,
		XtNumber(com_args));

	XtSetArg(com_args[2], XtNx, (XtArgVal)0);
	XtSetArg(com_args[3], XtNy, (XtArgVal)100);
	XtCreateManagedWidget( "0@100", commandWidgetClass, box, com_args,
		XtNumber(com_args));

	XtSetArg(com_args[2], XtNx, (XtArgVal)200);
	XtSetArg(com_args[3], XtNy, (XtArgVal)200);
	XtCreateManagedWidget( "200@200", commandWidgetClass, box, com_args,
		XtNumber(com_args));

	XtRealizeWidget(toplevel);
	XtMainLoop();
}
