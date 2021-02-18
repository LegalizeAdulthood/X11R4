/* colordemo - show all the colors from rgb.txt
 * vix 04apr88 [written]
 */


#include <stdio.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Box.h>
#include <X11/Clock.h>
#include <X11/Command.h>
#include <X11/Dialog.h>
#include <X11/Label.h>
#include <X11/Load.h>
#include <X11/Scroll.h>
#include <X11/AsciiText.h>
#include <X11/Core.h>
#include <X11/Viewport.h>
#include <X11/VPaned.h>
#include <X11/Cardinals.h>
#include <X11/Shell.h>

#ifndef RGB_TXT
#define RGB_TXT "/usr/lib/X11/rgb.txt"
#endif

static XrmOptionDescRec options[] = {
	{"-label", XtNlabel, XrmoptionSepArg, NULL}
};


void
Syntax(call) char *call; {
	printf("Usage: %s\n", call);
	exit(0);
}


/* ARGSUSED */
void
Destroyed(widget, closure, callData)
	Widget widget;                   /* unused */
	caddr_t closure;               /* unused */
	caddr_t callData;               /* unused */
{
	exit(0);
}



/* ARGSUSED */
void
quitFunc(widget, closure, callData)
	Widget widget;                   /* unused */
	caddr_t closure;               /* widget to destroy */
	caddr_t callData;               /* unused */
{
	XtDestroyWidget((Widget)closure);
}



void
main(argc, argv)
	unsigned int argc;
	char **argv;
{
	static	XtCallbackRec	callback[2];    /* K&R: initialized to NULL */
	/**/	Widget		toplevel, outer, colors;
	/**/	Arg		arg[10];
	/**/	int	 	n;

	toplevel = XtInitialize(	"main", "colordemo",
					options, XtNumber(options),
					&argc, argv	);

	if (argc != 1) Syntax(argv[0]);

	n = 0;
	XtSetArg(arg[n], XtNwidth, 400); n++;
	outer = XtCreateManagedWidget(	NULL, vPanedWidgetClass,
					toplevel, arg, n	);
	XtAddCallback(outer, XtNdestroyCallback, Destroyed, NULL);

	n = 0;
	callback[0].callback = quitFunc;
	callback[0].closure = (caddr_t) outer;
	XtSetArg(arg[n], XtNcallback, callback);	n++;
	XtCreateManagedWidget(		"quit", commandWidgetClass,
					outer, arg, n		);

	n = 0;
	colors = XtCreateManagedWidget(	"colors", boxWidgetClass,
					outer, arg, n		);

	colordemo(colors);

	XtRealizeWidget(toplevel);

	XtMainLoop();
}


static
colordemo(parent)
	Widget parent;
{
	int r, g, b, prev_r, prev_g, prev_b, dups;
	char colorname[50], save_colorname[50];
	FILE *rgb;

	if (!(rgb = fopen(RGB_TXT, "r"))) {
		perror(RGB_TXT);
		exit(2);		/* vms will be with me forever */
	}

	dups = 0;
	prev_r = prev_g = prev_b = -1;
	save_colorname[0] = '\0';
	while (4 == fscanf(rgb, "%d %d %d %[^\n]\n", &r, &g, &b, colorname)) {
		if (r == prev_r && g == prev_g && b == prev_b) {
			dups++;
			strcpy(save_colorname, colorname);
		} else {
			dups = 0;
			if (save_colorname[0])
				do_color(parent, save_colorname);
			prev_r = r;
			prev_g = g;
			prev_b = b;
		}
	}
	if (dups)
		do_color(parent, colorname);
}


static Pixel
stringToPixel(w, colorname)
	Widget w;
	String colorname;
{
	XrmValue	in, out;

	in.addr = colorname;
	in.size = strlen(colorname) + 1;
	XtConvert(w, XtRString, &in, XtRPixel, &out);
	return *(Pixel*)out.addr;
}


static
do_color(parent, colorname)
	Widget parent;
	char *colorname;
{
	Arg		arg[10];
	int 		n;

	n = 0;
	XtSetArg(arg[n], XtNborderWidth, 10); n++;
	XtSetArg(arg[n], XtNborderColor, stringToPixel(parent, colorname));
									n++;
	XtCreateManagedWidget(colorname, labelWidgetClass, parent, arg, n);
}
