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

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Cardinals.h>
#include <St/StPictureP.h>
#include "picture.bit"

extern void exit();

static XrmOptionDescRec options[] = {
{"-ap",		"*picture.allowProtrusion",	XrmoptionNoArg,	"TRUE"},
{"-np",		"*picture.allowProtrusion",	XrmoptionNoArg,	"FALSE"},
{"-scrollbar",	"*picture.forceBars",		XrmoptionNoArg,	"TRUE"},
{"-sb",		"*picture.forceBars",		XrmoptionNoArg,	"TRUE"},
{"-bottombar",  "*picture.useBottom",           XrmoptionNoArg, "TRUE"},
{"-bb",         "*picture.useBottom",           XrmoptionNoArg, "TRUE"},
{"-topbar",     "*picture.useBottom",           XrmoptionNoArg, "FALSE"},
{"-tb",         "*picture.useBottom",           XrmoptionNoArg, "FALSE"},
{"-rightbar",   "*picture.useRight",            XrmoptionNoArg, "TRUE"},
{"-rb",         "*picture.useRight",            XrmoptionNoArg, "TRUE"},
{"-leftbar",    "*picture.useRight",            XrmoptionNoArg, "FALSE"},
{"-lb",         "*picture.useRight",            XrmoptionNoArg, "FALSE"},
{"-f",          "*picture.file",                XrmoptionSepArg,""},
{"-file",       "*picture.file",                XrmoptionSepArg,""},
};

Syntax(call)
    char *call;
{
    (void)printf("Usage: %s [-sb] [-bb] [-tb] [-rb] [-lb]\n", call);
    (void)printf("  [-f <file_name>]\n");
    (void)printf("  [-fn <font_name>]\n");
    (void)printf("  [-fg <color>] [-bg <color>]\n");
    (void)printf("  [-bw <pixels>] [-bd <color>] [-rv]\n");
    (void)printf("  [-d [<host>]:[<vs>]]\n");
    (void)printf("  [-g [<width>][x<height>][<+-><xoff>[<+-><yoff]]]\n");
    exit(1);
}

void  Picexit(w,event,params,nparams)
Widget  w  ;
XEvent  *event  ;
String  *params  ;
Cardinal  nparams  ;
{
        exit(1)  ;
}

void main(argc, argv)
    int argc;
    char **argv;
{
    Widget toplevel, pic;
    Arg arg;
    Pixmap icon_pixmap = None;

static  XtActionsRec  add[]={
                {"exit",Picexit},
        }  ;
static  char  *overTranslations=
                  "Ctrl<Key>C    :  exit() "  ;

    toplevel = XtInitialize("main", "XPicture", options, XtNumber(options),
					&argc, argv);
    if(argc != 1) Syntax(argv[0]);

    XtAddActions(add,1)  ;

    XtSetArg(arg,XtNiconPixmap, &icon_pixmap);
    XtGetValues(toplevel, &arg, ONE);
    if(icon_pixmap == None) {
        arg.value = (XtArgVal)XCreateBitmapFromData(XtDisplay(toplevel),
		XtScreen(toplevel)->root,
		picture_bits, picture_width, picture_height);
        XtSetValues(toplevel, &arg, ONE);
    }

    pic = XtCreateManagedWidget("picture", stPictureWidgetClass, toplevel, NULL,
				ZERO);
    XtOverrideTranslations(pic, XtParseTranslationTable(overTranslations)) ;
    XtRealizeWidget(toplevel);
    XtMainLoop();
}
