/***********************************************************************
*  File:   xxim.c
*  Author: Philip Thompson
*  $Date: 89/11/09 17:30:02 $
*  $Revision: 1.4 $
*  Purpose: To view a variety of images on X displays and.
*   "Xxim" displays images up to 24-bits deep with an 'ImageHeader'
*   on 8-bit color and bitmap displays.  Various dithering and
*   halftoning techniques are used to achieve this image portability.
*
*  Copyright (c) 1988  Philip R. Thompson
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
#if (!defined(lint) && !defined(SABER))
static char xxim_rcs_id[] =
    "$Header: /mit/phils/X/RCS/xxim.c,v 1.4 89/11/09 17:30:02 phils Exp Locker: phils $";
#endif lint

#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include "XimHeader.h"

#define ICONSIZE  100               /* minimum icon size */

extern int dm4[4][4];
int GreyToBitmap_fs(), GreyToBitmap_dither(), (*bw)();
char *progName, *malloc(), *calloc();
Display  *dpy;
int  screen;
Window  image_win, icon_win;
FILE  *in_fp = stdin;
int  debug_flag = False, enhance = 0;
int  newmap_flag = False, mono_flag = False, cdither_flag = False;
int  pclose(), fclose(), (*closefunc)() = fclose;

main(argc, argv)
int argc;
char **argv;
{
    register  i;
    XimImage  xim, icon;
    unsigned borderWidth = 5;
    int  icon_scale;
    char *win_name = NULL, *display_name = NULL;
    char *geometry = NULL, *icon_geom = NULL;
    Bool  inverse_flag = False, output_flag = False;
    FILE  *out_fp = stdout, *popen(), *fopen();
    XEvent  event;
    GC  gc;
    XGCValues  gc_val;
    XSetWindowAttributes  xswa;
    XImage  *image = NULL, *icon_image = NULL;
    XSizeHints  sizehints;
    XWMHints  wmhints;
    extern u_long  blackpixel, whitepixel;
    XVisualInfo  *visInfo = NULL, *FindVisual();
    XColor  colors[256], fg_color, bg_color;
    Colormap  colormap, GetXColormap();
    extern ditherFactor;

    progName = argv[0];
    for (i=1; i < argc; i++) {
        char *ptr = NULL;
        if (strncmp(argv[i], "-dis", 4) == 0) {
            display_name = argv[++i];
            continue;
        }
        if (strcmp(argv[i], "-in") == 0) {   /* compressed file ? */
            ptr = rindex(argv[++i], '.');
            if (ptr && strcmp(ptr, ".Z") == 0) {
                char popen_cmd[80];
                (void)strcpy(popen_cmd, "zcat ");
                (void)strcat(popen_cmd, argv[i]);
                in_fp = popen(popen_cmd, "r");
                closefunc = pclose;
            } else
                in_fp = fopen(argv[i], "r");
            if (in_fp == NULL)
                XimError("Can't open input file: %s ", argv[i]);
            win_name = argv[i];
            continue;
        }
        if (strncmp(argv[i], "-o", 2) == 0) {
            if ((out_fp = fopen(argv[++i], "w")) == NULL)
                XimError("Can't open %s\n", argv[i]);
            output_flag = True;
            continue;
        }
        if (strncmp(argv[i], "-ge", 3) == 0) {
            geometry = argv[++i];
            continue;
        }
        if (strncmp(argv[i], "-ic", 3) == 0) {
            icon_geom = argv[++i];
            continue;
        }
        if (strncmp(argv[i], "-fs", 3) == 0) {      /* default */
            bw = GreyToBitmap_fs;
            mono_flag = True;
            continue;
        }
        if (strncmp(argv[i], "-dit", 4) == 0) {
            if ((ptr=rindex(argv[i],'=')) != NULL) {
                (void)sscanf(ptr+1,"%d", &ditherFactor);
                /* dm = &(dm4[0][0]); */
            }
            bw = GreyToBitmap_dither;
            cdither_flag = True;
            continue;
        }
        if (strncmp(argv[i], "-en", 3) == 0) {
            if ((ptr=rindex(argv[i],'=')) != NULL)
                enhance = atoi(ptr+1);
            else
                enhance = 9;
            continue;
        }
        if (strncmp(argv[i], "-re", 3) == 0) {
            inverse_flag = True;
            continue;
        }
        if (strncmp(argv[i], "-mo", 3) == 0) {
            mono_flag = True;
            continue;
        }
        if (strncmp(argv[i], "-cm", 3) == 0) {
            newmap_flag = True;
            continue;
        }
        if (strncmp(argv[i], "-bw", 3) == 0) {
            borderWidth = atoi(argv[++i]);
            continue;
        }
        if (strncmp(argv[i], "-de", 3) == 0) {
            debug_flag = True;
            continue;
        }
        Usage();
    }

    /*  Open the display & set defualts */
    if ((dpy = XOpenDisplay(display_name)) == NULL)
        XimError("Can't open display '%s'", XDisplayName(display_name));
    screen = XDefaultScreen(dpy);
    
    blackpixel = XBlackPixel(dpy, screen);
    whitepixel = XWhitePixel(dpy, screen);
    if (!geometry)
        geometry = XGetDefault(dpy, progName, "geometry");

    /* Read header and verify file formats */
    if (!ReadXimHeader(in_fp, &xim))
        XimError("can't read xim header");
    if (xim.bits_channel == 1)
        mono_flag = True;

    if (mono_flag)
        visInfo = FindVisual(dpy, 1);  /* This needs fixing */
    else
        visInfo = FindVisual(dpy, 256);
    if (visInfo->class == StaticGray && visInfo->depth == 1)
        mono_flag = True;

    /* Check the flags and options here */
    if (cdither_flag) {
        if (mono_flag) {
            cdither_flag = False;              /* mono flag wins */
            newmap_flag = False;
            if ((ditherFactor != 4) && (ditherFactor != 8))
                XimError("Dither size must be 4 or 8 in monochrome.");
        } else
            if (ditherFactor < 2 || ditherFactor > 8)
                XimError("Dither level must be from 2 to 6 in color");
    }

    (void)fprintf(stderr,"Size: %d x %d   ", xim.width, xim.height);
    /* Get or make the color table.  */
    if (xim.bits_channel == 1 && xim.nchannels == 1)
        (void)fprintf(stderr,"Bitmap\n");
    else {
        if ((xim.nchannels >= 3 || cdither_flag) && !mono_flag) {
            (void)fprintf(stderr,"nchannels %d\n", xim.nchannels);
            if (!mono_flag && visInfo->colormap_size > 7)
                xim.ncolors = MakeXdithermap(ditherFactor, colors,
                    visInfo->colormap_size);
        } else if (xim.nchannels == 1) {    /* use provided colormap */
            (void)fprintf(stderr,"ncolors %d\n", xim.ncolors);
            for (i=0; i < xim.ncolors; i++) {
                colors[i].pixel = (u_long)i;
                colors[i].red = (u_short)(xim.colors[i].red * 257);
                colors[i].green = (u_short)(xim.colors[i].grn * 257);
                colors[i].blue = (u_short)(xim.colors[i].blu * 257);
                colors[i].flags = DoRed | DoGreen | DoBlue;
            }
        }
        if (inverse_flag && !mono_flag)
            for (i=0; i < xim.ncolors; i++) {
                colors[i].red = ~colors[i].red;
                colors[i].green = ~colors[i].green;
                colors[i].blue = ~colors[i].blue;
            }
    }

    /* read in the first picture of the image */
    if (!ReadXimImage(in_fp, &xim))
        XimError("can't read xim data");

    Make_icon(&wmhints, &xim, &icon, &icon_scale, icon_geom);
    if ((icon.data = (byte *)calloc(icon.datasize, 1)) == NULL)
        XimError("Can't malloc() icon buffer");

    /* process and store the image and icon. */
    ProcessImageAndIcon(&xim, &icon, icon_scale);
    if (mono_flag)
        colormap = XDefaultColormap(dpy, screen);
    else {
        colormap = GetXColormap(dpy, visInfo, colors, xim.ncolors,
                &newmap_flag);
        DataToXColorValues(&xim, colors);
        DataToXColorValues(&icon, colors);
    }
    image = XCreateImage(dpy, visInfo->visual, (mono_flag ? 1 : 8),
            (mono_flag ? XYBitmap : ZPixmap), 0, (char *)xim.data,
            xim.width, xim.height, 8, 0);
    icon_image = XCreateImage(dpy, visInfo->visual, (mono_flag ? 1: 8),
            (mono_flag ? XYBitmap : ZPixmap), 0, (char *)icon.data,
            icon.width, icon.height, 8, 0);
    if (mono_flag) {
        icon_image->bitmap_bit_order = LSBFirst;
        icon_image->byte_order = LSBFirst;
        image->bitmap_bit_order = LSBFirst;
        image->byte_order = LSBFirst;
    }
    if (output_flag) {
        SaveImageState(&xim, colormap, inverse_flag);
        (void)WriteXim(out_fp, &xim);
        Quit();
    }

    /* Get window attributes & create windows */
    xswa.colormap = colormap;
    xswa.bit_gravity = NorthWestGravity;        /* CenterGravity; */
    xswa.event_mask = ExposureMask | ButtonPressMask | KeyPressMask;
    xswa.background_pixel = blackpixel;
    xswa.border_pixel = whitepixel;
    xswa.cursor = XCreateFontCursor(dpy, XC_gumby);
    Set_sizehint(&sizehints, &xim, geometry);
    image_win = XCreateWindow(dpy, XRootWindow(dpy, screen),
            sizehints.x, sizehints.y, sizehints.width, sizehints.height,
            borderWidth, visInfo->depth, InputOutput, visInfo->visual,
            CWBackPixel | CWEventMask | CWCursor | CWBorderPixel |
            CWBitGravity | CWColormap, &xswa);
    xswa.event_mask = ExposureMask;
    icon_win = XCreateWindow(dpy, XRootWindow(dpy, screen), 0, 0,
            icon.width, icon.height, 1, visInfo->depth,
            InputOutput, visInfo->visual, CWEventMask | CWBorderPixel |
            CWColormap, &xswa);

    /* set window manager hints */
    XSetStandardProperties(dpy, image_win, "X Imager", win_name,
            None, argv, argc, &sizehints);
    wmhints.icon_window = icon_win;
    wmhints.flags |= IconWindowHint;
    XSetWMHints(dpy, image_win, &wmhints);

    /* Set up GCs and icon pixmap */
    if (inverse_flag) {
        gc_val.foreground = whitepixel;
        gc_val.background = blackpixel;
    } else {
        gc_val.foreground = blackpixel;
        gc_val.background = whitepixel;
    }
    gc = XCreateGC(dpy, image_win, GCForeground |GCBackground, &gc_val);

    if (newmap_flag && !mono_flag &&
            (xim.ncolors > XDisplayCells(dpy,screen))) {
        i = XDisplayCells(dpy,screen);
        bg_color.red = colors[i].red;
        bg_color.green = colors[i].green;
        bg_color.blue = colors[i].blue;     /* force the last two */
        fg_color.red = colors[++i].red;   /* colors and sacrifice */
        fg_color.green = colors[i].green;   /* the cursor. */
        fg_color.blue = colors[i].blue;     /* works on a Vax/GPX */
        XRecolorCursor(dpy, xswa.cursor, &fg_color, &bg_color);
    }
    XMapWindow(dpy, image_win);             /* Map the image window. */

    if (debug_flag)
       fprintf(stderr,"Processing events.\n");
    while (True) {          /* Set up a loop to maintain the image. */
        XNextEvent(dpy, &event);           /* Wait on input event. */
        switch((int)event.type) {
        int modulo;     /* Temporary var. for expose->x % 4 */
        case Expose: {
            XExposeEvent  *expose = (XExposeEvent *)&event;;
            if (expose->window == icon_win) {
                XPutImage(dpy, icon_win, gc, icon_image, 0, 0,
                    0, 0, icon.width, icon.height);
                break;
            }
            if ((modulo = expose->x % 4) != 0) {
                expose->x -= modulo;
                expose->width += modulo;
            }
            if (expose->width % 4 != 0)
                expose->width += 4 - (expose->width % 4);
            XPutImage(dpy, image_win, gc, image,
                expose->x, expose->y, expose->x, expose->y,
                expose->width, expose->height);
            }
            break;
        case ButtonPress:
            switch((int)event.xbutton.button) {
            case Button1:
                if (!ReadXimImage(in_fp, &xim)) {
                    fprintf(stderr,"No more pictures in file.\n");
                    break;
                }
                (void)DitherToDisplay(&xim, enhance, mono_flag,
                    cdither_flag);
                if (!mono_flag)
                    DataToXColorValues(&xim, colors);
                image->data = (char *)xim.data;
                XPutImage(dpy, image_win, gc, image, 0, 0, 0, 0,
                        image->width, image->height);
                break;
            case Button2:
                fprintf(stderr,"(x,y): %d %d\n", event.xbutton.x,
                        event.xbutton.y);
                break;
            case Button3:
                Quit();
            }
            break;
        case KeyPress: {
            XKeyEvent *kevent = (XKeyEvent *)&event;
            char kbuf[10];
            XLookupString(kevent, kbuf, sizeof kbuf, NULL, NULL);
            if (kbuf[0] == 'q' || kbuf[0] == 'Q' || kbuf[0] == '\003') {
                Quit();
            }
            }
            break;
        default:
             fprintf(stderr,"Bad X event.\n");
        }
    }
}  /* end main */


ProcessImageAndIcon(xim, icon, scale)
XimImage  *xim, *icon;
int scale;
{
    int i, j, k = 0, pixnum = 0;
    unsigned GreyValue();
    byte *icodata, *ximdata, *tmp = NULL;

    if (mono_flag) {               /* make a bitmap image, but first */
        if (xim->bits_channel == 1 && xim->nchannels == 1) {
        if (icon)       /* sample image for icon */
            for (i=0; i < icon->height; i++) {
                ximdata = xim->data + (i * xim->bytes_per_line *scale);
                icodata = icon->data + (i * icon->bytes_per_line);
                for (j=0; j < icon->bytes_per_line; j++)
                    for (k=0; k < 8; k++) {
                        pixnum = (j*8 + k) * scale;
                        if (pixnum < xim->width) {
                            tmp = ximdata + (pixnum / 8);
                            if (*tmp & (0x01 << (pixnum % 8)))
                                icodata[j] |= (0x01 << k);
                        }
                    }
            }
            return;
        }
        ColorToGrey(xim);
        if (bw == GreyToBitmap_dither)
            NormalizeDM(ditherFactor);
        if (icon)       /* sample image for icon */
            for (i=0, k=0; i < icon->height; i++)
                for (j=0; j < icon->width; j++, k++)
                    icon->data[k] = xim->data[(i*xim->width+j) *
                    scale];
        if (enhance) {
            if (!Edge_enhance(xim, enhance))
                return;
            if (!icon || !Edge_enhance(icon, enhance))
                return;
        }
        /* Translate pixmap into a bitmap for monochrome display. */
        if (icon)
            (*bw)(icon);
        (*bw)(xim);
    } else {                      /* use color image */
        if (xim->nchannels >= 3) {
            if (enhance)
                Edge_enhance(xim, enhance);
            Rgb_dither(xim);
        } else if (cdither_flag)
            Pixmap_dither(xim);
        if (icon) {
            for (i=0, k=0; i < icon->height; i++)
                for (j=0; j < icon->width; j++, k++)
                    icon->data[k] =
                            xim->data[(i*xim->width+j) * scale];
        }
    }
    if (xim->npics <= 1 && xim->nchannels == 3) {
        free((char *)xim->grn_data);
        free((char *)xim->blu_data);
    }
}


Quit()
{
    (void)(*closefunc)(in_fp);
    if (newmap_flag)
        XInstallColormap(dpy, XDefaultColormap(dpy,screen));
    if (image_win) {
        XDestroyWindow(dpy, image_win);
        XDestroyWindow(dpy, icon_win);
    }
    XCloseDisplay(dpy);
    exit(0);
}


Set_sizehint(hintp, xim, geom)
XSizeHints *hintp;
XimImage *xim;
char *geom;
{
    int geom_result = NoValue;

    /* set the size hints, algorithm from xlib xbiff
    */
    hintp->width = hintp->max_width = xim->width;
    hintp->height = hintp->max_height = xim->height;
    hintp->flags = PMaxSize;
    hintp->x = hintp->y = 0;
    if (!geom)
        return;
    geom_result = XParseGeometry(geom, &hintp->x, &hintp->y,
            &hintp->width, &hintp->height);
    if (geom_result & (WidthValue|HeightValue)) {
        hintp->width = hintp->width > hintp->min_width ?
            hintp->width : hintp->min_width;
        hintp->height = hintp->height > hintp->min_height ?
            hintp->height : hintp->min_height;
        hintp->flags |= USSize;
        hintp->flags &= ~PSize;
    }
    if (geom_result & (XValue|YValue)) {
        hintp->flags |= USPosition;
        hintp->flags &= ~PPosition;
    }
    if (!(hintp->flags & USSize)) {
        hintp->width = xim->width;
        hintp->height = xim->height;
        hintp->flags |= PSize;
    }
    if (!(hintp->flags & USPosition)) {
        hintp->x = hintp->y = 0;
        hintp->flags |= PPosition;
    }
    if (geom_result & XNegative)
        hintp->x = XDisplayWidth(dpy, screen) + hintp->x - hintp->width;
    if (geom_result & YNegative)
        hintp->y = XDisplayHeight(dpy, screen) +hintp->y -hintp->height;
}


Make_icon(wmhintp, xim, icon, scale, geom)
XWMHints  *wmhintp;
XimImage  *xim, *icon;
int *scale;
char *geom;
{
    int x, y, geom_result = NoValue;

    if (!geom) {
        wmhintp->flags &= ~IconPositionHint;
        wmhintp->icon_x = wmhintp->icon_y = 0;
        *scale = ICONSIZE;
    } else {
        geom_result = XParseGeometry(geom, &x, &y, &icon->width,
            &icon->height);
        if (geom_result & (WidthValue | HeightValue)) {
                *scale = icon->width > icon->height ? icon->width :
                        icon->height;
                *scale = *scale > ICONSIZE ? *scale : ICONSIZE;
        }
    }
    *scale = rnd(xim->height / *scale) > rnd(xim->width / *scale)
            ? rnd(xim->height / *scale) : rnd(xim->width / *scale);
    if (*scale <= 0)
        *scale = 1;
    if ((icon->width = xim->width / *scale) % 2)
        icon->width -= 1;
    icon->height = xim->height / *scale;
    icon->bits_channel = xim->bits_channel;
    if (icon->bits_channel == 1 && icon->nchannels == 1)
        icon->bytes_per_line = (icon->width+7) / 8;
    else
        icon->bytes_per_line = icon->width;
    icon->datasize = icon->height * icon->bytes_per_line;
    if (debug_flag)
        fprintf(stderr,"\nIcon width %d  height %d  scale %d:1\n",
            icon->width, icon->height, *scale);
    if (geom && (geom_result & (XValue | YValue))) {
        if (geom_result & XNegative)
            x = XDisplayWidth(dpy, screen) + x - icon->width;
        if (geom_result & YNegative)
            y = XDisplayHeight(dpy, screen) + y - icon->height;
        wmhintp->flags |= IconPositionHint;
        wmhintp->icon_x = x;
        wmhintp->icon_y = y;
        wmhintp->initial_state = IconicState;
        wmhintp->flags |= StateHint;
    }
}


Usage ()
{
    static char **cpp, *help_message[] = {
    "where options include:",
    "  -display host      host X server to contact",
    "  -in file           file to display, otherwise use stdin",
    "  -cmap              force creation of new colormap",
    "  -reverse           invert the colormap",
    "  -mono              force monochrome conversion",
    "  -dither[=#]        use dithering filter in mono and color",
    "  -mfs               use a modified floyd steinberg mono filter",
    "  -enhance[=1-9]     variable edge enhancement, (default = 9)",
    "  -geom geomstring   size and location of image",
    "  -icon geomstring   size and location of icon",
    "  -bw #              border width around image, default 5",
    "  -debug             print various debuging information",
    "Options may be abbreviated to the shortest unambiguous prefix.",
    "Exit with right button or 'q' in window.", NULL
    };

    fprintf (stderr, "usage:\n  %s [-options ...]\n\n", progName);
    for (cpp = help_message; *cpp; cpp++)
        fprintf (stderr, "%s\n", *cpp);
    exit(1);
}


SaveImageState(xim, colormap, inverse_flag)
XimImage *xim;
Colormap colormap;
Bool inverse_flag;
{
    register byte *bp, *ep;

    ep = xim->data + xim->datasize;
    if (xim->bits_channel == 1 && xim->nchannels == 1) {
        if (inverse_flag) {
            bp = xim->data;
            while (bp++ < ep)
                *bp = ~(*bp);
        }
        return;
    }
    (void)RefineXColors(dpy, screen, xim, colormap);
}

/* End of xxim.c */
