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

#include "stedit.h"

static int true = True;
static int panel, PANEL;
static char *fname;

static XtResource application_resources[] = {
	{"panel", "Panel", XtRBoolean, sizeof(int),
		(Cardinal)&PANEL, XtRBoolean, (caddr_t)&true},
	{"fname", "Fname", XtRString, sizeof(char *),
		(Cardinal)&fname, XtRString, ""},
};

static XrmOptionDescRec options[] = {
	{"-ri", ".panel", XrmoptionNoArg, "True"},
	{"-right", ".panel", XrmoptionNoArg, "True"},
	{"-le", ".panel", XrmoptionNoArg, "False"},
	{"-left", ".panel", XrmoptionNoArg, "False"},
	{"-f", ".fname", XrmoptionSepArg, ""},
};

static String menutable[] = {
	"Select", "Attributes", "Write", "Read", "Read Image",
	"Undo", "Redraw", NULL
};

static StPictureFunction menufunc[] = {
	StpictureDrawEnd, StpictureAttributes, StpictureWrite, StpictureRead,
	StpictureImage, StpictureUndo, StpictureRedraw, StpictureEnd
};

typedef struct {
	Widget	buttonid;
	void	(* button_calbk)();
	char *	icon;
} IconDefs;

typedef struct {
	Widget buttonid;
	void	(* button_calbk)();
	char *	buttonname;
} ButtonDefs;

extern void RectC();
extern void CirC();
extern void EllipsC();
extern void LineC();
extern void ArcC();
extern void EllipsarcC();
extern void ArcpieC();
extern void EllipspieC();
extern void PlineC();
extern void PolygonC();
extern void FlineC();
extern void StringC();

IconDefs icondefs[] = {
	{(Widget)NULL, RectC, rect_bits},
	{(Widget)NULL, CirC, cir_bits},
	{(Widget)NULL, EllipsC, ellips_bits},
	{(Widget)NULL, LineC, line_bits},
	{(Widget)NULL, ArcC, arc_bits},
	{(Widget)NULL, EllipsarcC, ellipsarc_bits},
	{(Widget)NULL, ArcpieC, arcpie_bits},
	{(Widget)NULL, EllipspieC, ellipspie_bits},
	{(Widget)NULL, PlineC, pline_bits},
	{(Widget)NULL, PolygonC, polygon_bits},
	{(Widget)NULL, FlineC, fline_bits},
	{(Widget)NULL, StringC, string_bits}
};

extern void PSetgroup();
extern void PUnsetgroup();
extern void PTop();
extern void PBottom();
extern void PResize();
extern void PMove();
extern void PCopy();
extern void PToptobottom();
extern void PLefttoright();
extern void PRotate();
extern void POnedelete();
extern void PAlldelete();

ButtonDefs buttondefs[] = {
	{NULL, PSetgroup, "Set"},
	{NULL, PUnsetgroup, "Reset"},
	{NULL, PTop, "Raise"},
	{NULL, PBottom, "Lower"},
	{NULL, PResize, "Resize"},
	{NULL, PMove, "Move"},
	{NULL, PCopy, "Copy"},
	{NULL, PToptobottom, "Top-Bottom"},
	{NULL, PLefttoright, "Left-Right"},
	{NULL, PRotate, "Rotate"},
	{NULL, POnedelete, "DeleteOne"},
	{NULL, PAlldelete, "DeleteAll"}
};

extern void command_clbk();

XtActionsRec pvaction[] = {
	{"command-callback",command_clbk}
};

static String TranslationTable = 
	"Ctrl<Key>D:	delete-character()\n\
	Ctrl<Key>H:	delete-previous-character()\n\
	Ctrl<Key>K:	erase-to-end-of-line()\n\
	Ctrl<Key>M:	newline()\n\
	Ctrl<Key>U:	erase-line()\n\
	<Key>Delete:	delete-previous-character()\n\
	<Key>BackSpace:	delete-previous-character()\n\
	<Key>Return:	newline()\n\
	<Key>:		insert-char()\n\
	Shift<Btn1Down>,Shift<Btn1Up>:	move-hand() \
					notify-before(move-hand) \
					notify-after(move-hand)\n\
	<Btn1Down>:	select-start()\n\
	<Btn1Motion>:	select-adjust()\n\
	<Btn1Up>:	select-end() notify-after(select-end) command-callback()\n\
        Shift<Btn2Down>,Shift<Btn2Up>:	pline-end() \
					notify-after(pline-end) \n\
	<Btn2Down>,<Btn2Up>:	main-popup()\n\
	<Btn3Down>:	extend-start()\n\
	<Btn3Motion>:	extend-adjust()\n\
	<Btn3Up>:	extend-end()\n\
	<Motion>:	motion()";

static XtTranslations compiletranslationtable;

void syntax()
{
	printf("usage:   stedit [-options ...]\n\n");
	printf("options include:\n");
	printf("    -right          or -ri display icon window to right\n");
	printf("    -left           or -le display icon window to left\n");
	printf("    -f [filename]   read file\n");
	exit(0);
}

main(argc,argv)
int argc;
char **argv;
{
	static Arg args[2];
	char *filename;
	int widht  = 750;
	int height = 750;
	static ac;

	filename = (char *)malloc(50);

	toplevel = XtInitialize("stpic", "Stpic", options, XtNumber(options),
				&argc, argv);

	if (argc!=1)
		syntax();

	XtGetApplicationResources(toplevel, 0, application_resources,
				XtNumber(application_resources), NULL, 0);

	ac = 0;
	XtSetArg(args[ac], XtNwidth, widht); ac++;
	XtSetArg(args[ac], XtNheight, height); ac++;
	XtSetValues(toplevel, args, ac);

	makeButtonsAndBoxes();

	XtRealizeWidget(toplevel);

	XDefineCursor(XtDisplay(toplevel), XtWindow(toplevel), None);

/*
	XDefineCursor(XtDisplay(toplevel), XtWindow(toplevel), XC_top_left_arrow);
*/

	SetupButton();

	if (strlen(fname)) {
		StPicReadFile(pic1, fname);
		StPicRedrawAll(pic1);
	}

	XtMainLoop();
}

void quitbutton(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
	XtDestroyWidget(toplevel);
	exit(0);
}

makeButtonsAndBoxes()
{
	static Arg args[20];
	static ac;

	static XtCallbackRec callbacks1[] = {
		{quitbutton, NULL},
		{NULL, NULL}
	};

	topform = XtCreateManagedWidget("stpic", formWidgetClass, toplevel,
					NULL, NULL);

	if (PANEL) {
		ac = 0;
		XtSetArg(args[ac], XtNwidth, 597); ac++;
		XtSetArg(args[ac], XtNheight, 402); ac++;
		XtSetArg(args[ac], XtNlabel, ""); ac++;
		XtSetArg(args[ac], XtNborderWidth, (XtArgVal)2); ac++;
		XtSetArg(args[ac], StNallowProtrusion, True); ac++;
		XtSetArg(args[ac], XtNforceBars, True); ac++;
		XtSetArg(args[ac], XtNuseBottom, False); ac++;
		XtSetArg(args[ac], XtNuseRight, False); ac++;
		XtSetArg(args[ac], StNmenuEntries, menutable); ac++;
		XtSetArg(args[ac], StNmenuFunctions, menufunc); ac++;
		pic1 = XtCreateManagedWidget("pic1", stPictureWidgetClass,
					topform, args, ac);	

		ac = 0;
		XtSetArg(args[ac], XtNfromHoriz, (XtArgVal)pic1); ac++;
		XtSetArg(args[ac], XtNborderWidth, (XtArgVal)1); ac++;
		XtSetArg(args[ac], XtNhorizDistance, (XtArgVal)4); ac++;
		topiconbox = XtCreateManagedWidget("topiconbox",
				formWidgetClass, topform, args, ac);
	}
	else {
		ac = 0;
		XtSetArg(args[ac], XtNwidth, 30); ac++;
		XtSetArg(args[ac], XtNheight, 430); ac++;
		XtSetArg(args[ac], XtNborderWidth, 1); ac++;
		XtSetArg(args[ac], XtNhorizDistance, 6); ac++;
		topiconbox = XtCreateManagedWidget("topiconbox",
					formWidgetClass, topform, args, ac);

		ac = 0;
		XtSetArg(args[ac], XtNwidth, 597); ac++;
		XtSetArg(args[ac], XtNheight, 402); ac++;
		XtSetArg(args[ac], XtNlabel, ""); ac++;
		XtSetArg(args[ac], XtNfromHoriz, (XtArgVal)topiconbox); ac++;
		XtSetArg(args[ac], XtNborderWidth, (XtArgVal)2); ac++;
		XtSetArg(args[ac], StNallowProtrusion, True); ac++;
		XtSetArg(args[ac], XtNforceBars, True); ac++;
		XtSetArg(args[ac], XtNuseBottom, False); ac++;
		XtSetArg(args[ac], XtNuseRight, False); ac++;
		XtSetArg(args[ac], StNmenuEntries, menutable); ac++;
		XtSetArg(args[ac], StNmenuFunctions, menufunc); ac++;
		pic1 = XtCreateManagedWidget("pic1", stPictureWidgetClass,
					topform, args, ac);	
	}

	XtAddActions(pvaction, XtNumber(pvaction));

	compiletranslationtable = XtParseTranslationTable(TranslationTable);

	XtOverrideTranslations(pic1, compiletranslationtable);

	if (PANEL) {
		ac = 0;
		XtSetArg(args[ac], XtNwidth, (XtArgVal)580); ac++;
		XtSetArg(args[ac], XtNheight, (XtArgVal)30); ac++;
		XtSetArg(args[ac], XtNfromHoriz, NULL); ac++;
		XtSetArg(args[ac], XtNfromVert, (XtArgVal)pic1); ac++;
		XtSetArg(args[ac], XtNhorizDistance, 4); ac++;
	}
	else {
		ac = 0;
		XtSetArg(args[ac], XtNwidth, (XtArgVal)580); ac++;
		XtSetArg(args[ac], XtNheight, (XtArgVal)30); ac++;
		XtSetArg(args[ac], XtNfromHoriz, NULL); ac++;
		XtSetArg(args[ac], XtNfromVert, (XtArgVal)topiconbox); ac++;
		XtSetArg(args[ac], XtNhorizDistance, 60); ac++;
	}
	topbuttonbox = XtCreateManagedWidget("topbuttonbox",
					formWidgetClass, topform, args, ac);

	if (PANEL) {
		ac = 0;
		XtSetArg(args[ac], XtNwidth, 43); ac++;
		XtSetArg(args[ac], XtNheight, 32); ac++;
		XtSetArg(args[ac], XtNborderWidth, 3); ac++;
		XtSetArg(args[ac], XtNlabel, "quit"); ac++;
		XtSetArg(args[ac], XtNfromVert, (XtArgVal)topiconbox); ac++;
		XtSetArg(args[ac], XtNfromHoriz, (XtArgVal)topbuttonbox); ac++;
		XtSetArg(args[ac], XtNhorizDistance, 5); ac++;
	}
	else {
		ac = 0;
		XtSetArg(args[ac], XtNwidth, 43); ac++;
		XtSetArg(args[ac], XtNheight, 32); ac++;
		XtSetArg(args[ac], XtNborderWidth, 3); ac++;
		XtSetArg(args[ac], XtNlabel, "quit"); ac++;
		XtSetArg(args[ac], XtNfromVert, (XtArgVal)pic1); ac++;
		XtSetArg(args[ac], XtNhorizDistance, 6); ac++;
	}
		XtSetArg(args[ac], XtNcallback, (XtArgVal)callbacks1); ac++;
		quitbox = XtCreateManagedWidget("quit",commandWidgetClass,
					topform, args, ac);	

	InitButton();

}

InitButton()
{
	static Arg args[10];
	static ac;
	static int i;
	static XtCallbackRec callbacks[1];

	ac = 0;
	callbacks[0].closure = NULL;
	XtSetArg(args[ac], XtNwidth, 35); ac++;
	XtSetArg(args[ac], XtNheight, 29); ac++;
	XtSetArg(args[ac], XtNlabel, ""); ac++;
	XtSetArg(args[ac], XtNborderWidth, 2); ac++;
	XtSetArg(args[ac], XtNvertDistance, 4); ac++;
	XtSetArg(args[ac], XtNfromVert, NULL); ac += 2;
	for (i = 0;i < 12;i++) {
		callbacks[0].callback = icondefs[i].button_calbk;
		callbacks[0].closure = (caddr_t)pic1;
		XtSetArg(args[ac - 1], XtNcallback, callbacks);
		icondefs[i].buttonid = XtCreateManagedWidget("child",
			commandWidgetClass, topiconbox, args, ac);
		XtSetArg(args[ac - 3], XtNvertDistance, 0);
		XtSetArg(args[ac - 2], XtNfromVert,
				(XtArgVal)icondefs[i].buttonid);
	}

	ac = 0;
	XtSetArg(args[ac], XtNwidth, 58); ac++;
	XtSetArg(args[ac], XtNheight, 10); ac++;
	XtSetArg(args[ac], XtNborderWidth, 2); ac++;
	XtSetArg(args[ac], XtNfromVert, NULL); ac++;
	XtSetArg(args[ac], XtNvertDistance, 4); ac++;
	XtSetArg(args[ac], XtNfromHoriz, NULL); ac += 2;
	for (i = 0;i < 12;i++) {
		if (i == 2 || i == 10) {
			XtSetArg(args[ac - 6], XtNheight, 10);
		}
		if (i == 1 || i == 3 || i == 11) {
			XtSetArg(args[ac - 6], XtNheight, 10);
			XtSetArg(args[ac - 4], XtNfromVert,
					buttondefs[i - 1].buttonid);
			XtSetArg(args[ac - 3], XtNvertDistance, NULL);
			if (i == 1) {
				XtSetArg(args[ac - 2], XtNfromHoriz, NULL);
			} 
			else {
				XtSetArg(args[ac - 2], XtNfromHoriz,
						buttondefs[i - 2].buttonid);
			}
		}
		callbacks[0].callback = buttondefs[i].button_calbk;
		callbacks[0].closure = (caddr_t)topbuttonbox;
		XtSetArg(args[ac - 1], XtNcallback, callbacks);
		buttondefs[i].buttonid = XtCreateManagedWidget(
			buttondefs[i].buttonname,
			commandWidgetClass, topbuttonbox, args, ac);
		XtSetArg(args[ac - 6], XtNheight, 24);
		XtSetArg(args[ac - 4], XtNfromVert, NULL);
		XtSetArg(args[ac - 3], XtNvertDistance, 4);
		XtSetArg(args[ac - 2], XtNfromHoriz,
				(XtArgVal)buttondefs[i].buttonid);
	}

}

SetupButton()
{
	static Arg args[10];
	int i;
	Pixmap p[12];
	GC cgc;
	XGCValues gcv;
	long bg, fg;
	static int ac;

	cgc = XCreateGC(XtDisplay(toplevel), XtWindow(icondefs[0].buttonid),
			GCBackground | GCForeground, &gcv);

	ac = 0;
	XtSetArg(args[ac], XtNbackground, &bg); ac++;
	XtSetArg(args[ac], XtNforeground, &fg); ac++;
	XtGetValues(icondefs[0].buttonid, args, (XtArgVal)2);

	ac = 1;
	for (i = 0;i < 12;i++){
		p[i] = XCreatePixmapFromBitmapData(XtDisplay(toplevel),
			XtWindow(icondefs[i].buttonid), icondefs[i].icon,
			41, 47, fg, bg,
			DefaultDepthOfScreen(XtScreen(icondefs[i].buttonid)));
		XtSetArg(args[ac - 1], XtNbackgroundPixmap, p[i]);
		XtSetValues(icondefs[i].buttonid, args, ac);
	}

	XFreeGC(XtDisplay(toplevel), cgc);
}
