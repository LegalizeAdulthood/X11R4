/***********************************************************************
*  File:   xlib.c
*  Author: Philip Thompson
*  $Date: 89/11/02 16:19:30 $
*  $Revision: 1.2 $
*  Purpose: General xim libray of X related utililities
*  Copyright (c) 1988  Philip R. Thompson  (phils@athena.mit.edu)
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
*   I provide this software freely as a public service, but not as
*   public domain.  It is not a commercial product, and therefore is
*   not subject to an an implied warranty of merchantability or fitness
*   for a particular purpose.  I provide it as is, without warranty.
*   Philip R. Thompson (phils@athena.mit.edu)
***********************************************************************/
#if (!defined(lint) && !defined(SABER))
static char xlib_rcs_id[] =
    "$Header: /mit/phils/X/Lib/RCS/ximxlib.c,v 1.2 89/11/02 16:19:30 phils Exp Locker: phils $";
#endif

#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "XimHeader.h"

extern  debug_flag;
char  *malloc(), *calloc(), *realloc();
u_long  blackpixel, whitepixel;


/* Return the most suitable Visual for representing numcolors colors
*/
XVisualInfo  *FindVisual(dpy, numcolors)
Display *dpy;
int numcolors;
{
    int  i, num_vis, big_map = 0, best_color_size[6];
    XVisualInfo *vlist, *v, *big_vis = NULL, *the_vis = NULL;
    XVisualInfo *best_vis[6];        /* Best Visual per Visual class */
    static best_class[] = { TrueColor, DirectColor, PseudoColor,
                     StaticColor, GrayScale, StaticGray };

    for (i=0; i < 6; i++) {
        best_color_size[i] = 0;
        best_vis[i] = NULL;
    }
    /* Get descriptions of all the visuals */
    vlist = XGetVisualInfo(dpy, VisualNoMask, NULL, &num_vis);
    /* Scan the list examining the colormap size */
    for (v = vlist; v < (vlist + num_vis); v++) {
        /* Remember the biggest colormap among all classes */
        if (v->colormap_size > big_map) {
            big_map = v->colormap_size;
            big_vis = v;
        }
        /* If the colormap is big enough,  remember it */
        if (v->colormap_size > numcolors) {
            if ((best_color_size[v->class] == 0) ||
                    (v->colormap_size < best_color_size[v->class])) {
                /* Smallest so far that'll do */
                best_color_size[v->class] = v->colormap_size;
                best_vis[v->class] = v;
            }
        }
    }
    if (debug_flag)
        fprintf(stderr, "Visual(s) found: %d\n", num_vis);
    /* In decreasing order of usability,  look at each class */
    for (i=0; i < 6; i++)
        if (best_color_size[best_class[i]] > numcolors) {
            /* This class can represent enough colors */
            the_vis = best_vis[best_class[i]];
            break;
        }
    /* We'll have to make do with a Visual that's too small */
    the_vis = big_vis;
    if (debug_flag)
        Print_visual_info(the_vis);
    return(the_vis);
}

/* took this out of xdpyinfo.c */
Print_visual_info(vip)
XVisualInfo *vip;
{
    char errorbuf[40], *class = NULL, *sprintf();   /* for printing */

    switch (vip->class) {
      case StaticGray:      class = "StaticGray"; break;
      case GrayScale:       class = "GrayScale"; break;
      case StaticColor:     class = "StaticColor"; break;
      case PseudoColor:     class = "PseudoColor"; break;
      case TrueColor:       class = "TrueColor"; break;
      case DirectColor:     class = "DirectColor"; break;
      default:
        (void)sprintf(errorbuf, "unknown class %d", vip->class);
        class = errorbuf;
        break;
    }
    printf ("  visual selected:\n");
    printf ("       id:    0x%lx\n", vip->visualid);
    printf ("    class:    %s\n", class);
    printf ("    depth:    %d plane%s\n", vip->depth,
        vip->depth == 1 ? "" : "s");
    printf ("    colormap size:   %d entries\n", vip->colormap_size);
    printf ("    red, green, blue masks:    0x%lx, 0x%lx, 0x%lx\n",
        vip->red_mask, vip->green_mask, vip->blue_mask);
    printf ("    significant bits r/g/b:    %d bits\n",
        vip->bits_per_rgb);
    return;
}


Colormap GetXColormap(dpy, visInfo, colors, ncolors, newmap_flag)
Display *dpy;
XVisualInfo *visInfo;
int ncolors;
XColor  colors[];
Bool  *newmap_flag;
{
    register i;
    Colormap cmap, cmap2;
    XColor qcolor;
    u_long FindColorPixel();
    
    if (ncolors > visInfo->colormap_size)
        ncolors = visInfo->colormap_size;
    if (debug_flag)
        fprintf(stderr,"Color table size %d\n", ncolors);
    if (*newmap_flag) {
        cmap = XCreateColormap(dpy, DefaultRootWindow(dpy),
                visInfo->visual, AllocAll);
        XStoreColors(dpy, cmap, colors, ncolors);
    } else {
        cmap = DefaultColormap(dpy, DefaultScreen(dpy));
        for (i=0; i < ncolors; i++) {
            if (XAllocColor(dpy, cmap, &colors[i]) == 0) {
                if (debug_flag)
                    fprintf(stderr,"Too many colors %d.\n",i);
                cmap2 = XCopyColormapAndFree(dpy, cmap);
                *newmap_flag = True;
                for ( ; i < ncolors; i++)
                    if (XAllocColor(dpy, cmap2, &colors[i]) == 0)
                        fprintf(stderr,"Can't xalloc color %d.\n",i);
                cmap = cmap2;
                break;
            }
        }
    }
    if (*newmap_flag) {
        whitepixel = FindColorPixel(dpy, cmap, ncolors, 255, 255, 255);
        blackpixel = FindColorPixel(dpy, cmap, ncolors, 0, 0, 0);
    }
    if (debug_flag) {
        fprintf(stderr,"white %lu   black %lu\n",whitepixel,blackpixel);
        for (i=0; i < ncolors; i++) {
            qcolor.pixel = (u_long)i;
            XQueryColor(dpy, cmap, &qcolor);
            fprintf(stderr,"color[%3d]: pix %3u r= %5u g= %5u b= %5u\n",
                i, qcolor.pixel, qcolor.red, qcolor.green, qcolor.blue);
        }
    }
    return(cmap);
}


Colormap GetNewColormap(dpy, visInfo, colors, ncolors)
Display *dpy;
XVisualInfo *visInfo;
XColor  colors[];
int ncolors;
{
    int i;
    Colormap cmap;
    XColor qcolor;
    u_long FindColorPixel();
    
    if (ncolors > visInfo->colormap_size)
        ncolors = visInfo->colormap_size;
    if (debug_flag)
        fprintf(stderr,"Color table size %d\n", ncolors);
    cmap = XCreateColormap(dpy, DefaultRootWindow(dpy), visInfo->visual,
            AllocAll);
    XStoreColors(dpy, cmap, colors, ncolors);
    whitepixel = FindColorPixel(dpy, cmap, ncolors, 255, 255, 255);
    blackpixel = FindColorPixel(dpy, cmap, ncolors, 0, 0, 0);
    if (debug_flag) {
        fprintf(stderr,"white %lu   black %lu\n",whitepixel,blackpixel);
        for (i=0; i < ncolors; i++) {
            qcolor.pixel = (u_long)i;
            XQueryColor(dpy, cmap, &qcolor);
            fprintf(stderr,"color[%3d]: pix %3u r= %5u g= %5u b= %5u\n",
                i, qcolor.pixel, qcolor.red, qcolor.green, qcolor.blue);
        }
    }
    return(cmap);
}


DataToXColorValues(xim, colors)
XimImage *xim;
XColor colors[];
{
    register byte *bp, *ep;
    int i;
        
    if (xim->bits_channel != 8 && xim->nchannels != 1) {
        XimWarn("DataToXColorValues(): not a pixmap");
        return(0);
    }
    for (i=0; i < xim->npics; i++) {
        bp = xim->data + (i * xim->datasize);
        ep = xim->data + ((i+1) * xim->datasize);
        do {
            *bp = (byte)colors[*bp].pixel;
        } while (++bp < ep);
    }
    return(1);
}
    

DataToXPixelValues(xim, pixels)
XimImage *xim;
unsigned long pixels[];
{
    register byte *bp, *ep;
    int i;
        
    if (xim->bits_channel != 8 && xim->nchannels != 1) {
        XimWarn("DataToXColorValues(): not a pixmap");
        return(0);
    }
    for (i=0; i < xim->npics; i++) {
        bp = xim->data + (i * xim->datasize);
        ep = xim->data + ((i+1) * xim->datasize);
        do {
            *bp = (byte)pixels[*bp];
        } while (++bp < ep);
    }
    return(1);
}
    
byte PixelToXColorValue(pix, colors)
byte pix;
XColor colors[];
{
    return((byte)colors[pix].pixel);
}


/* Find the the closest color in the colormap.
*/
u_long FindColorPixel(dpy, cmap, ncolors, red, green, blue)
Display *dpy;
Colormap cmap;
int ncolors, red, green, blue;
{
    register i, red2, blue2, green2;
    XColor *qcolors;
    u_long value = 0;
    int dist, least = 65536;
    
    qcolors = (XColor *)calloc((unsigned)ncolors, sizeof(XColor));
    for (i=0; i < ncolors; i++) {
        qcolors[i].pixel = (u_long)i;
        qcolors[i].flags = DoRed | DoGreen | DoBlue;
    }
    XQueryColors(dpy, cmap, qcolors, ncolors);
    for (i=0; i < ncolors; i++) {
        red2 = (int)qcolors[i].red / 256;   
        green2 = (int)qcolors[i].green / 256;   
        blue2 = (int)qcolors[i].blue / 256; 
        dist = (red2 - red) * (red2 - red) +
               (green2 - green) * (green2 - green) +
               (blue2 - blue) * (blue2 - blue);
        if (dist == 0) {
            value = qcolors[i].pixel;
            break;
        } else if (dist < least) {
            least = dist;
            value = qcolors[i].pixel;
        }
    }
    free((char *)qcolors);
    return(value);
}


/* Very similar to Refine_colors in xconvert.c, but looks at the colors
*  used on the screen.
*/
RefineXColors(dpy, screen, xim, colormap)
Display *dpy;
int screen;
XimImage *xim;
Colormap colormap;
{
    register i, j = 0;
    unsigned indices[256];
    register byte *bp, *ep;
    XColor *qcolors;
    int ncolors;

    ncolors = XDisplayCells(dpy, screen);
    qcolors = (XColor *)calloc((unsigned)ncolors, sizeof(XColor));
    for (i=0; i < ncolors; i++)
        qcolors[i].pixel = (u_long)i;
    XQueryColors(dpy, colormap, qcolors, ncolors);

    bzero((char *)indices, sizeof(indices));
    bp = xim->data;
    ep = xim->data + xim->datasize;
    do {
        indices[*bp]++;
    } while (++bp < ep);
    if (xim->colors)
        xim->colors = (Color *)realloc((char *)xim->colors,
                (unsigned)ncolors * sizeof(Color));
    else
        xim->colors = (Color *)calloc((unsigned)ncolors,
                sizeof(Color));
    for (i=0, j=0; i < xim->ncolors; i++) {
        if ((int)indices[i] > 0) {
            indices[(int)qcolors[i].pixel] = j;
            xim->colors[j].red = qcolors[i].red / 256;
            xim->colors[j].grn = qcolors[i].green / 256;
            xim->colors[j].blu = qcolors[i].blue / 256;
            j++;
        }
    }
    xim->ncolors = j;
    bp = xim->data;
    do {
        *bp = indices[*bp];
    } while (++bp < ep);
    free((char *)qcolors);
    return(1);
}


/* Create a color map and dithering matrix for the specified
*  intensity levels.
*/
int MakeXdithermap(factor, colors, maxcolors)
int  factor, maxcolors;
XColor colors[];
{
    float N, magicfact;
    register i, j, k, l, ncolors;
    extern int dm4[4][4], dm16[16][16], divN[256], modN[256];
    extern int levels, levelsq, ditherFactor;

    for (i = factor; i > 1; i--)
        if ((i*i*i) < maxcolors)
            break;
    ditherFactor = levels = i;
    levelsq = levels * levels;      /* squared */
    ncolors = levelsq * levels;     /* and cubed */
    if (levels < 2) {
       XimWarn("MakeXdithermap: can't work with less than 2 levels.");
       return(0);
    }
    N = 255.0 / (levels-1.0);       /* Get size of each step */

    /* Set up the color map entries.  */
    for (i=0; i < ncolors; i++) {
        colors[i].pixel = (u_long)i;
        colors[i].red = (u_short)(rnd((i%levels) * N) * 257);
        colors[i].green = (u_short)(rnd(((i/levels)%levels) * N) * 257);
        colors[i].blue = (u_short)(rnd(((i/levelsq)%levels) * N) * 257);
        colors[i].flags = DoRed | DoGreen | DoBlue;
    }
    for (i=0; i < 256; i++) {
        divN[i] = (int)(i / N);
        modN[i] = i - (int)(N * divN[i]);
    }
    magicfact = (N - 2.0) / 16.0;
    for (i=0; i < 4; i++)
        for (j=0; j < 4; j++)
            for (k=0; k < 4; k++)
                for (l=0; l < 4; l++)
                    dm16[4*k+i][4*l+j] = rnd(dm4[i][j] * magicfact +
                                        (dm4[k][l]/16.0) * magicfact);
    if (debug_flag) {
        fprintf(stderr,"Levels = %d  Ncolors = %d\n", levels, ncolors);
        for (i=0; i < 16; i++) {
            for (j=0; j < 16; j++)
                fprintf(stderr,"%4d", dm16[i][j]);
            fprintf(stderr,"\n");
        }
        fprintf(stderr,"\n");
    }
    return(ncolors);
}


XimDisplayImage(dpy, win, xim)
Display *dpy;
Window win;
XimImage *xim;
{
    int i, bw_flag = 0, enhance = 0;
    int cdither_flag = 0, ncolors = 0;
    int screen = DefaultScreen(dpy);
    XWindowAttributes xwa;
    Visual *vis;
    XColor *colors = NULL;
    XImage *image = NULL;

    fprintf(stderr, "XimDisplayImage() called.\n");
    XGetWindowAttributes(dpy, win, &xwa);
    vis = xwa.visual;
    if (vis->class == StaticGray && vis->map_entries <= 2)
        bw_flag = True;
    if (xim->data) {
        if (!bw_flag && xim->bits_channel == 8) {
            ncolors = vis->map_entries;
            cdither_flag = True;
            colors = (XColor *)calloc((unsigned)ncolors,sizeof(XColor));
            for (i=0; i < ncolors; i++)
                colors[i].pixel = (u_long)i;
            XQueryColors(dpy, xwa.colormap, colors, ncolors);
        }
        DitherToDisplay(xim, enhance, bw_flag, cdither_flag);
        if (!bw_flag && xim->bits_channel == 8)
            DataToXColorValues(xim, colors);
        image = XCreateImage(dpy, vis, (bw_flag ? 1 : 8),
            (bw_flag ? XYBitmap : ZPixmap), 0, (char *)xim->data,
            xim->width, xim->height, 8, 0);
        XPutImage(dpy, win, XDefaultGC(dpy, screen),image,
            0, 0, 0, 0, xim->width, xim->height);
        xim->other = (byte *)image->data;
        free((char *)colors);
    } else if (xim->other) {
        image = XCreateImage(dpy, vis, (bw_flag ? 1 : 8),
            (bw_flag ? XYBitmap : ZPixmap), 0, (char *)xim->other,
            xim->width, xim->height, 8, 0);
        XPutImage(dpy, win, XDefaultGC(dpy, screen), image,
            0, 0, 0, 0, xim->width, xim->height);
    }
    if (image->data) {
        image->data = NULL;
        XDestroyImage(image);
    }
    return(1);
}

/* xlib.c */
