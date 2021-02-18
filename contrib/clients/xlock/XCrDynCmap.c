#ifndef lint
static char sccsid[] = "@(#)XCrDynCmap.c 22.2 89/09/20";
#endif
/*-
 * XCrDynCmap.c - X11 library routine to create dynamic colormaps.
 *
 * Copyright (c) 1989 by Sun Microsystems, Inc.
 *
 * Author: Patrick J. Naughton
 * naughton@sun.com
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.
 *
 * This file is provided AS IS with no warranties of any kind.  The author
 * shall have no liability with respect to the infringement of copyrights,
 * trade secrets or any patents by this file or any part thereof.  In no
 * event will the author be liable for any lost revenue or profits or
 * other special, indirect and consequential damages.
 *
 */

#include <X11/X.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

Status
XCreateDynamicColormap(dsp, screen, cmap, visual, colors,
		       count, red, green, blue)
    Display    *dsp;
    int		screen;
    Colormap   *cmap;
    Visual    **visual;
    XColor     *colors;
    int		count;
    u_char     *red,
	       *green,
	       *blue;
{
    XVisualInfo vinfo;
    int		pixels[256];
    int		i,
		ncolors,
		planes;
    unsigned long pmasks;
    Status	allocReturn;

    planes = DisplayPlanes(dsp, screen);
    if (XMatchVisualInfo(dsp, screen, planes, PseudoColor, &vinfo)) {

	*visual = vinfo.visual;
	*cmap = XCreateColormap(dsp, RootWindow(dsp, screen),
				*visual, AllocNone);
	ncolors = vinfo.colormap_size;

	if (count > ncolors)
	    return BadValue;

	allocReturn = XAllocColorCells(dsp, *cmap,
				       False, &pmasks, 0,
				       pixels, count);

/*	This should return Success, but it doesn't... Xlib bug?
 *	(I'll ignore the return value for now...)
 */
#ifdef NOTDEF
	if (allocReturn != Success)
	    return allocReturn;
#endif				/* NOTDEF */

	for (i = 0; i < count; i++) {
	    colors[i].pixel = pixels[i];
	    colors[i].red = *red++ << 8;
	    colors[i].green = *green++ << 8;
	    colors[i].blue = *blue++ << 8;
	    colors[i].flags = DoRed | DoGreen | DoBlue;
	}
	XStoreColors(dsp, *cmap, colors, count);
	return Success;
    } else
	return BadMatch;
}
