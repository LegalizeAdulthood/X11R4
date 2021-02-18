/*
 * Copyright 1989 Digital Equipment Corporation
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  Digital Equipment Corporation makes no representations
 * about the suitability of this software for any purpose.  It is
 * provided "as is" without express or implied warranty.
 *
 * DIGITAL EQUIPMENT CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL DIGITAL EQUIPMENT CORPORATION BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Terry Weissman
 *          weissman@decwrl.dec.com
 */

/* main.c -- create our windows and initialize things. */

#define MAIN
#include "roids.h"
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Paned.h>

static int width, height;		/* Size of window. */
static Pixel defaultfore;	/* Default foreground */
static Pixel defaultback;	/* Default background */
static Pixel scorepixel;	/* Color for score. */

static XrmOptionDescRec table[] = {
    {"-debug",	"*debug",	XrmoptionNoArg,	NULL},
};

static XtResource resources[] = {
    {XtNwidth, XtCWidth, XtRInt, sizeof(int),
	 (Cardinal)&width, XtRImmediate, (caddr_t) 500},
    {XtNheight, XtCHeight, XtRInt, sizeof(int),
	 (Cardinal)&height, XtRImmediate, (caddr_t) 800},
    {"debug", "Debug", XtRBoolean, sizeof(Boolean),
	 (Cardinal)&debug, XtRString, "off"},
    {"numdir", "NumDir", XtRInt, sizeof(int),
	 (Cardinal)&numdir, XtRImmediate, (caddr_t) 48},
    {"shipradius", "ShipRadius", XtRInt, sizeof(int),
	 (Cardinal)&shipradius, XtRImmediate, (caddr_t) 12},
    {"shipwait", "ShipWait", XtRInt, sizeof(int),
	 (Cardinal)&shipwait, XtRImmediate, (caddr_t) 10},
    {"rockwait", "RockWait", XtRInt, sizeof(int),
	 (Cardinal)&rockwait, XtRImmediate, (caddr_t) 30},
    {"shotwait", "ShotWait", XtRInt, sizeof(int),
	 (Cardinal)&shotwait, XtRImmediate, (caddr_t) 30},
    {"shipcolor", "ShipColor", XtRPixel, sizeof(Pixel),
	 (Cardinal)&shippixel, XtRString, "cyan"},
    {"rockcolor", "RockColor", XtRPixel, sizeof(Pixel),
	 (Cardinal)&rockpixel, XtRString, "#fa0"},
    {"shotcolor", "ShotColor", XtRPixel, sizeof(Pixel),
	 (Cardinal)&shotpixel, XtRString, "red"},
    {"scorecolor", "ScoreColor", XtRPixel, sizeof(Pixel),
	 (Cardinal)&scorepixel, XtRString, "green"},
    {"maxvelocity", "MaxVelocity", XtRFloat, sizeof(float),
	 (Cardinal)&maxv, XtRString, "3.0"},
    {"maxrockspeed", "MaxRockSpeed", XtRFloat, sizeof(float),
	 (Cardinal)&maxrockspeed, XtRString, "5.0"},
    {"accelerationPercentage", "AccelerationPercentage", XtRFloat,
	 sizeof(float), (Cardinal) &accper, XtRString, "0.1"},
    {"shotAcceleration", "ShotAcceleration", XtRFloat,
	 sizeof(float), (Cardinal) &shotacc, XtRString, "2.0"},
    {"maxshots", "MaxShots", XtRInt, sizeof(int),
	 (Cardinal) &maxshots, XtRImmediate, (caddr_t) 4},
    {"shotduration", "ShotDuration", XtRInt, sizeof(int),
	 (Cardinal) &shotduration, XtRImmediate, (caddr_t) 1000},
    {"defaultfore", "DefaultFore", XtRPixel, sizeof(Pixel),
	 (Cardinal) &defaultfore, XtRString, "white"},
    {"defaultback", "DefaultBack", XtRPixel, sizeof(Pixel),
	 (Cardinal) &defaultback, XtRString, "black"},
};


static void CvtStringToFloat(args, num_args, fromVal, toVal)
ArgList args;
Cardinal num_args;
XrmValue    *fromVal;
XrmValue    *toVal;
{
    static float  i;

    if (sscanf((char *)fromVal->addr, "%f", &i) == 1) {
	toVal->size = sizeof(float);
	toVal->addr = (caddr_t) &i;
    } else {
	toVal->size = 0;
	toVal->addr = NULL;
    }
}

static void AddResource(r, p)
char *r;
Pixel *p;
{
    XrmValue value;
    XrmDatabase db = XtDatabase(dpy);
    value.size = sizeof(Pixel);
    value.addr = (caddr_t) p;
    XrmPutResource(&db, r, XtRPixel, &value);
}

main(argc, argv)
Cardinal argc;
char **argv;
{
    static Arg args[] = {
	{XtNwidth, NULL},
	{XtNheight, NULL},
    };
    static Arg scoreargs[] = {
	{XtNforeground, NULL},
    };
    Widget pane;
    extern WidgetClass labelwidgetclass;
    srandom(time(0));
    toplevel = XtInitialize(argv[0], "Roids", table, XtNumber(table),
			    &argc, argv);
    dpy = XtDisplay(toplevel);
    XtAddConverter(XtRString, XtRFloat, CvtStringToFloat, NULL, 0);
    XtGetApplicationResources(toplevel, (caddr_t) NULL, 
			      resources, XtNumber(resources),
			      NULL, (Cardinal) 0);
    AddResource("*background", &defaultback);
    if (DisplayCells(dpy, DefaultScreen(dpy)) <= 2) {
	shippixel = defaultfore;
	rockpixel = defaultfore;
	shotpixel = defaultfore;
	scorepixel = defaultfore;
    }
    args[0].value = (XtArgVal) width;
    args[1].value = (XtArgVal) height;
    pane = XtCreateWidget("pane", vPanedWidgetClass, toplevel,
			  args, XtNumber(args));
    XtManageChild(pane);
    scoreargs[0].value = (XtArgVal) scorepixel;
    scorewidget = XtCreateWidget("score", labelWidgetClass, pane,
				 scoreargs, XtNumber(scoreargs));
    XtManageChild(scorewidget);
    gamewidget = (RoidsWidget)
	XtCreateWidget("field", roidsWidgetClass, pane, NULL, 0);
    XtManageChild(gamewidget);
    XtRealizeWidget(toplevel);
    XtMainLoop();
}


Punt(msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}
