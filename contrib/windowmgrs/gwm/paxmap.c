/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/* WARNING: THIS IS NOT MY CODE AND I AM NOT RESPONSIBLE OF THE STYLE!!!
 * -- colas
 */
/***************************************************************************\
* 									    *
* Constant used in paxfile library functions :				    *
* 									    *
* int XxWritePixmapFile(dpy, cmap, filename, pixmap, w, h)		    *
* int XxReadPixmapFile(dpy, cmap, d, depth, filename, w, h, pixmap)	    *
* Pixmap XxCreatePixmapFromData(dpy, cmap, d, depth, w, h, n, c, col, pix)  *
* 									    *
* Daniel Dardailler - Bull RC (88/12/13) e-mail: daniel@mirsa.inria.fr	    *
* 									    *
\***************************************************************************/


#define PAX_FORMAT 1

/* we keep the same codes as for Bitmap management */
#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "machine.h"

DECLARE_strchr;

typedef struct _Cixel {
    char            c1, c2;
}               Cixel;		/* 2 chars for one pixel value */

typedef struct _CmapEntry {
    Cixel           cixel;
    int             red, green, blue;
}               CmapEntry;


#define MAXPRINTABLE 93		/* number of printable ascii chars minus \ et
				 * " for string compat. */

static char    *printable = " .XoO+@#$%&*=-;:?>,<1234567890qwertyuipasdfghjklzxcvbnmMNBVCZASDFGHJKLPIUYTREWQ!~^/()_`'][{}|";

/* Look of PAX file : .. something like X11 'C includeable' format
 * 
 * #define drunk_paxformat 1
 * #define drunk_width 18
 * #define drunk_height 21
 * #define drunk_ncolors 4
 * #define drunk_chars_per_pixel 2
 * static  char * drunk_colors[] = {
 * "  " , "#FFFFFFFFFFFF",
 * ". " , "#A800A800A800",
 * "X " , "White",
 * "o " , "#540054005400"
 * } ;
 * static char * drunk_pixels[] = {
 * "                                    ",
 * "                                    ",
 * "            . . . . . . .           ",
 * "          X         . . . .         ",
 * "        X     X       . . . .       ",
 * "      o         X       . . .       ",
 * "    o o     X           . . . .     ",
 * "  o o o               . . . . .     ",
 * "o o o               . . . . . .     ",
 * "o o o                   . . . .     ",
 * "  X                 X   . . .       ",
 * "  X   X               . . . .       ",
 * "    X               . . . .         ",
 * "    X                 . .           ",
 * "      X                   X X X     ",
 * "        X X X               X   X   ",
 * "              X           X X       ",
 * "            X X X       X X         ",
 * "          X       X X X             ",
 * "      X X                           ",
 * "                                    "
 * } ;
 * 
 * 
 * 
 *  Version 1 can handle a variable number of chars per pixel
 *  - for each different color : n chars can represent the pixel value
 *    and is associed with red, green and blue intensity or colorname.
 */


/*------------------------------------------------------------*/
/*------------------------------------------------------------*/
/* following routines are used in XxReadPixmapFile() function */
/*------------------------------------------------------------*/

/*
 * read the next line and jump blank lines 
 */
static char    *
getline(s, pF)
char           *s;
FILE           *pF;
{
    s = fgets(s, 256, pF);

    while (s) {
	int             len = strlen(s);

	if (len && s[len - 1] == '\015')
	    s[--len] = '\0';
	if (len == 0)
	    s = fgets(s, 256, pF);
	else
	    break;
    }
    return (s);
}

/*
 *  Read a Pixmap file in a X Pixmap with specified depth and colormap
 * (THIS VERSION READ ONLY 2 CHARS PER PIXEL FORMAT : '_chars_per_pixel 2')
 */
int 
XReadPaxmapFile(display, colormap, d, depth, filename, width, height, pixmap)
/*-----------------*/
Display        *display;
Colormap        colormap;
Drawable        d;
int             depth;
char           *filename;
unsigned int   *width, *height;	/* RETURNED */
Pixmap         *pixmap;		/* RETURNED */
{
    GC              Gc;
    XGCValues       xgcv;

    FILE           *fstream;	/* handle on file  */
    char            linebuf[256];
    char            namebuf[256];
    char            type[256];
    char            name[256];

    int             ncolors;

    CmapEntry      *cmap;
    int            *Tpixel;

    char            c1, c2, c;
    int             red, green, blue;
    XColor          xcolor;
    int             i, j, p;


    if ((fstream = fopen(filename, "r")) == NULL) {
	return BitmapOpenFailed;
    }
    getline(linebuf, fstream);
    if ((sscanf(linebuf, "#define %[^_]%s %d", namebuf, type, &p) != 3)
	|| (strcmp("_paxformat", type))
	|| (p != PAX_FORMAT)) {
	fclose(fstream);
	return BitmapFileInvalid;
    } else
	strcpy(name, namebuf);

    getline(linebuf, fstream);
    if ((sscanf(linebuf, "#define %[^_]%s %d", namebuf, type, width) != 3)
	|| (strcmp(name, namebuf))
	|| (strcmp("_width", type))) {
	fclose(fstream);
	return BitmapFileInvalid;
    }
    getline(linebuf, fstream);
    if ((sscanf(linebuf, "#define %[^_]%s %d", namebuf, type, height) != 3)
	|| (strcmp(name, namebuf))
	|| (strcmp("_height", type))) {
	fclose(fstream);
	return BitmapFileInvalid;
    }
    getline(linebuf, fstream);
    if ((sscanf(linebuf, "#define %[^_]%s %d", namebuf, type, &ncolors) != 3)
	|| (strcmp(name, namebuf))
	|| (strcmp("_ncolors", type))) {
	fclose(fstream);
	return BitmapFileInvalid;
    }
    if (ncolors > (MAXPRINTABLE * MAXPRINTABLE)) {
	fclose(fstream);
	return BitmapFileInvalid;
    }
    getline(linebuf, fstream);
    if ((sscanf(linebuf, "#define %[^_]%s %d", namebuf, type, &p) != 3)
	|| (strcmp(name, namebuf)) || (p != 2)
	|| (strcmp("_chars_per_pixel", type))) {
	fclose(fstream);
	return BitmapFileInvalid;
    }
    getline(linebuf, fstream);
    if ((sscanf(linebuf, "static char * %[^_]%s = {", namebuf, type) != 2)
	|| (strcmp(name, namebuf))
	|| (strcmp("_colors[]", type))) {
	fclose(fstream);
	return BitmapFileInvalid;
    }
    cmap = (CmapEntry *) malloc(ncolors * sizeof(CmapEntry));
    Tpixel = (int *) malloc(ncolors * sizeof(int));
    if ((cmap == NULL) || (Tpixel == NULL)) {
	fclose(fstream);
	return BitmapNoMemory;
    }
    getline(linebuf, fstream);
    for (i = 0; i < ncolors; i++) {
	if ((sscanf(linebuf, "\"%c%c\" , \"%[^\"]%s",
		    &c1, &c2,
		    namebuf, type) != 4)) {
	    fclose(fstream);
	    return BitmapFileInvalid;
	}
	if ((strchr(printable, c1)) &&
	    (strchr(printable, c2))) {
	    cmap[i].cixel.c1 = c1;
	    cmap[i].cixel.c2 = c2;
	    if (!XParseColor(display, colormap, namebuf, &xcolor)) {
		fclose(fstream);
		return BitmapFileInvalid;
	    }
	    XAllocColor(display, colormap, &xcolor);
	    Tpixel[i] = xcolor.pixel;
	} else {
	    fclose(fstream);
	    return BitmapFileInvalid;
	}

	getline(linebuf, fstream);
    }

    if (strncmp(linebuf, "} ;", 3)) {
	fclose(fstream);
	return BitmapFileInvalid;
    }
    getline(linebuf, fstream);
    if ((sscanf(linebuf, "static char * %[^_]%s = {", namebuf, type) != 2)
	|| (strcmp(name, namebuf))
	|| (strcmp("_pixels[]", type))) {
	fclose(fstream);
	return BitmapFileInvalid;
    }
    *pixmap = XCreatePixmap(display, d, *width, *height, depth);
    Gc = XCreateGC(display, *pixmap, 0, &xgcv);

    getline(linebuf, fstream);
    j = 0;
    while ((j < *height) && strncmp(linebuf, "} ;", 3)) {
	if (strlen(linebuf) < (2 * (*width) + 2)) {
	    fclose(fstream);
	    return BitmapFileInvalid;
	}
	for (i = 1; i < (2 * (*width)); i += 2) {
	    c1 = linebuf[i];
	    c2 = linebuf[i + 1];
	    for (p = 0; p < ncolors; p++)
		if ((cmap[p].cixel.c1 == c1) && (cmap[p].cixel.c2 == c2))
		    break;
	    if (p != ncolors)
		XSetForeground(display, Gc, Tpixel[p]);
	    else {
		fclose(fstream);
		return BitmapFileInvalid;
	    }
	    XDrawPoint(display, *pixmap, Gc, i / 2, j);
	}
	j++;
	getline(linebuf, fstream);
    }

    if (strncmp(linebuf, "} ;", 3)) {
	fclose(fstream);
	return BitmapFileInvalid;
    }
    if (j != *height) {
	fclose(fstream);
	return BitmapFileInvalid;
    }
    free(cmap);
    free(Tpixel);
    fclose(fstream);
    return BitmapSuccess;
}
