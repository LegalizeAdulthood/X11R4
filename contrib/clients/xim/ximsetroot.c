/* ximsetroot.c
*  Copyright (c) 1989, Massachusetts Institute of Technology
*                Philip R. Thompson
*                Computer Resource Laboratory (CRL)
*                Dept. of Architecture and Planning
*                M.I.T., Rm 9-526
*                Cambridge, MA  02139
*   This  software and its documentation may be used, copied, modified,
*   and distributed for any purpose without fee, provided:
*       --  The above copyright notice appears in all copies.
*       --  This disclaimer appears in all source code copies.
*       --  The names of M.I.T. and the CRL are not used in advertising
*           or publicity pertaining to distribution of the software
*           without prior specific written permission from me or CRL.
*   I provide this software freely as a public service.  It is NOT
*   public domain nor a commercial product, and therefore is not subject
*   to an an implied warranty of merchantability or fitness for a
*   particular purpose.  I provide it as is, without warranty.
*
*   This software is furnished  only on the basis that any party who
*   receives it indemnifies and holds harmless the parties who furnish
*   it against any claims, demands, or liabilities connected with using
*   it, furnishing it to others, or providing it to a third party.
*
*   Philip R. Thompson (phils@athena.mit.edu)
***********************************************************************/
#ifndef lint
static char  ximsetroot_rcsid[] =
    "$Header: /mit/phils/X/RCS/ximsetroot.c,v 1.3 89/10/23 22:46:26 phils Exp Locker: phils $";
#endif

#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "XimHeader.h"

char *progName, *calloc();
extern int debug_flag;

main(argc, argv)
int argc;
char **argv;
{
    Display *dpy;
    register i, scrn;
    Pixmap   pix;
    XimImage *xim;
    FILE *in_fp = stdin;
    int pclose(), fclose(), (*closefunc)() = fclose;
    int mvpixel_flag = False, bw_flag = False;
    int cdither_flag = False, perm_flag = False;
    XVisualInfo *visInfo, *FindVisual();
    XColor xcolors[256];
    Colormap xcmap, GetXColormap();
    char *display_name = NULL;
    XImage *ximage = NULL;
    GC  gc;
    XGCValues  gcval;
    Window window = (Window)0;
    XWindowAttributes wininfo;

    for (i=1; i < argc; i++) {
        if (strncmp(argv[i], "-dis", 4) == 0) {
            display_name = argv[++i];
            continue;
        }
        if (strncmp(argv[i], "-in", 3) == 0) {
            char *ptr = rindex(argv[++i], '.');
            if (ptr && strcmp(ptr, ".Z") == 0) {
                char popen_cmd[80];
                (void)strcpy(popen_cmd, "zcat ");
                (void)strcat(popen_cmd, argv[i]);
                in_fp = popen(popen_cmd, "r");
                closefunc = pclose;
            } else
                in_fp = fopen(argv[i], "r");
            if (in_fp == (FILE *)0)
                XimError("Can't open input file: %s ", argv[i]);
            continue;
        }
        if (strncmp(argv[i], "-id", 3) == 0) {
            if (sscanf(argv[++i], "0x%lx", &window) != 1)
                XimError("Invalid window id format: %s.", argv[i]);
            if (!window)
                if (sscanf(argv[i], "%ld", &window) != 1)
                    XimError("Invalid window id format: %s.", argv[i]);
            continue;
        }
        if (strncmp(argv[i], "-de", 3) == 0) {
            debug_flag = True;
            continue;
        }
        if (strncmp(argv[i], "-pe", 3) == 0) {
            perm_flag = True;
            continue;
        }
        XimError("Usage: %s [-in file] [-id win] [-perm]", progName);
    }

    if ((dpy = XOpenDisplay(display_name)) == NULL)
        XimError("Can't open display '%s'", XDisplayName(display_name));
    scrn = DefaultScreen(dpy);
    visInfo = FindVisual(dpy, 256);
    if (!window)
        window = RootWindow(dpy, scrn);
    if(!XGetWindowAttributes(dpy, window, &wininfo))
       XimError("Can't get target window attributes.");

    xim = (XimImage *)calloc(1, sizeof(XimImage));
    if (!ReadXim(in_fp, xim))
        XimError("Can't read image");
    (void)(*closefunc)(in_fp);
    
    if ((visInfo->class == StaticGray && visInfo->depth == 1) ||
            visInfo->colormap_size < 8)
        bw_flag = True;
    if (bw_flag || (xim->nchannels == 1 && xim->bits_channel == 1)) {
        xcmap = XDefaultColormap(dpy, scrn);
        bw_flag = True;
    } else {
        int ncolors, newmap_flag = False;
        ncolors = MakeXdithermap(6, xcolors, visInfo->colormap_size);
        xcmap = GetXColormap(dpy, visInfo, xcolors, ncolors,
            &newmap_flag);
        if (newmap_flag)
            XimError("Not enough free color slots in default colormap");
        mvpixel_flag = True;
        cdither_flag = True;
    }
    if (!(DitherToDisplay(xim, 0, bw_flag, cdither_flag)))
        XimError("can't  DitherToDisplay");
    if (!bw_flag && mvpixel_flag)
        if (!DataToXColorValues(xim, xcolors))
            XimError("can't move DataToXColorValues");

    gcval.foreground = WhitePixel(dpy, scrn);
    gcval.background = BlackPixel(dpy, scrn); 
    gc = XCreateGC(dpy, window, (GCForeground | GCBackground), &gcval);
    pix = XCreatePixmap(dpy, window, xim->width,
        xim->height, DefaultDepth(dpy, scrn));
    ximage = XCreateImage(dpy, visInfo->visual, (bw_flag ? 1 : 8),
       (bw_flag ? XYBitmap : ZPixmap), 0, (char *)xim->data,
        xim->width, xim->height, 8, 0);
    ximage->bitmap_bit_order = LSBFirst;
    ximage->byte_order = LSBFirst;
    XPutImage(dpy, pix, gc, ximage, 0,0, 0, 0, xim->width, xim->height);
    XSetWindowBackgroundPixmap(dpy, window, pix);
    XSetWindowColormap(dpy, window, xcmap);
    XClearWindow(dpy, window);
    XFlush(dpy);

    FreeXim(xim);
    XFreeGC(dpy, gc);
    ximage->data = NULL;
    XDestroyImage(ximage);
    XFreePixmap(dpy, pix);
    /* XFreeColormap(dpy, xcmap); */
    if (perm_flag)
        XSetCloseDownMode(dpy, RetainPermanent);
    XCloseDisplay(dpy);
    exit(0);
}

/*** end ximsetroot.c ***/
