/*
 * $XConsortium: xperfmon.c,v 1.6 89/09/19 14:44:02 jim Exp $
 * 
 * xperfmon
 * X Window System -based Unix Performance Monitor.
 * Simple graphical performance monitor for system-wide data.
 *
 * Copyright 1985, Massachusetts Institute of Technology
 * Copyright 1989, PCS Computer Systeme GmbH, West Germany
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of PCS not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  PCS makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL PCS
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * This original version was completed by Emanuel Jay Berkenbilt,
 * MIT Project Athena on 1/21/1988.  The program was built directly
 * on the Xlib.
 *
 * The current version was modified from the original by Thomas Bagli,
 * PCS GmbH on 8/8/89.  It is X Toolkit and Athena Widget -based.
 *
 * xperfmon is composed of the following files:
 *	xperfmon.c	xperfmon.bm	xperfmon.man	(the program)
 *	Perfmon.c	Perfmon.h	PerfmonP.h	(the widget)
 *	system.c	system.h	graph.h
 *	Imakefile	Makefile
 *
 */

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include "Perfmon.h"
#include "xperfmon.bm"

extern void exit();

static XrmOptionDescRec options[] = {
    { "-st",        "*perfmon.stepSize",  XrmoptionSepArg,    NULL },
    { "-stepsize",  "*perfmon.stepSize",  XrmoptionSepArg,    NULL },
    { "-up",        "*perfmon.update",    XrmoptionSepArg,    NULL },
    { "-update",    "*perfmon.update",    XrmoptionSepArg,    NULL },
    { "-usercpu",   "*perfmon.userCpu",   XrmoptionNoArg,     "TRUE" },
    { "-systemcpu", "*perfmon.systemCpu", XrmoptionNoArg,     "TRUE" },
    { "-idlecpu",   "*perfmon.idleCpu",   XrmoptionNoArg,     "TRUE" },
    { "-freemem",   "*perfmon.freeMem",   XrmoptionNoArg,     "TRUE" },
};

void
usage()
{
    fprintf(stderr, "\nusage: xperfmon option option option .....\n");
    fprintf(stderr, "options:\n");
    fprintf(stderr, "    [-display [{host}]:[{vs}]]\n");
    fprintf(stderr, "				(to specify display)\n");
    fprintf(stderr,
	"    [-geometry [{width}][x{height}][{+-}{xoff}[{+-}{yoff}]]]\n");
    fprintf(stderr, "				(to set window geometry)\n");
    fprintf(stderr, "    [-rv]			(black on white)\n");
    fprintf(stderr, "    [-bw {pixels}]		(border width)\n");
    fprintf(stderr, "    [-fn {font}]		(font)\n");
    fprintf(stderr, "    [-bd {color}]		(border color)\n");
    fprintf(stderr, "    [-fg {color}]		(foreground color)\n");
    fprintf(stderr, "    [-bg {color}]		(background color)\n");
    fprintf(stderr, "    [-update {seconds}]		(update interval)\n");
    fprintf(stderr, "    [-stepsize {integer}]	(stepsize in pixels)\n");
    fprintf(stderr,
	"    [-usercpu]			(display user cpu percentage)\n");
    fprintf(stderr,
	"    [-systemcpu]		(display system cpu percentage)\n");
    fprintf(stderr,
	"    [-idlecpu]			(display idle cpu percentage)\n");
    fprintf(stderr,
	"    [-freemem]			(display free memory)\n");
    exit(1);
}

void
main(argc, argv)
    int argc;
    char **argv;
{
    Arg arg;
    Pixmap icon_pixmap = None;
    Widget toplevel, perfmon;

    toplevel = XtInitialize("main", "XPerfmon", options,
	XtNumber(options), &argc, argv);
    if (argc != 1) usage();

    XtSetArg(arg, XtNiconPixmap, &icon_pixmap);
    XtGetValues(toplevel, &arg, 1);
    if (icon_pixmap == None) {
	XtSetArg(arg, XtNiconPixmap,
	    XCreateBitmapFromData(XtDisplay(toplevel),
		XtScreen(toplevel)->root, xperfmon_bits,
		xperfmon_width, xperfmon_height));
	XtSetValues(toplevel, &arg, 1);
    }

    perfmon = XtCreateManagedWidget("perfmon", perfmonWidgetClass,
	toplevel, NULL, 0);

    XtRealizeWidget(toplevel);
    XtMainLoop();
}
