/* send.c:
 *
 * send an Image to an X pixmap
 *
 * jim frost 10.02.89
 *
 * Copyright 1989 Jim Frost.  See included file "copyright.h" for complete
 * copyright information.
 */

#include "copyright.h"
#include "xloadimage.h"

unsigned int sendImageToX(disp, scrn, visual, image, pixmap, cmap, verbose)
     Display      *disp;
     int           scrn;
     Visual       *visual;
     Image        *image;
     Pixmap       *pixmap;
     Colormap     *cmap;
     unsigned int  verbose;
{ Pixel        *index;
  unsigned int  a, newmap, x, y;
  byte         *pixptr;
  XColor        xcolor;
  XGCValues     gcv;
  GC            gc;
  XImage       *ximage;

  goodImage(image, "sendImageToX");

  switch(visual->class) {
  case PseudoColor:
  case GrayScale:
  case StaticColor:
  case StaticGray:
    break;
  default:
    printf("sendImageToX: unsupported display visual\n");
    exit(1);
  }

  index= (Pixel *)lmalloc(sizeof(Pixel) * image->rgb.used);
  xcolor.flags= DoRed | DoGreen | DoBlue;

  /* get the colormap to use
   */

  if (visual == DefaultVisual(disp, scrn)) {
    *cmap= DefaultColormap(disp, scrn);
    newmap= 0;
  }
  else {
    if ((visual->class == PseudoColor) || (visual->class == GrayScale))
      *cmap= XCreateColormap(disp, scrn, AllocNone);
    else
      *cmap= XCreateColormap(disp, scrn, AllocAll);
    newmap= 1;
  }

  /* here's where we have fun; we try to build a reasonable colormap from
   * whatever X will give us.  ugh.  blech.  gag.  puke.  barf.
   */

  if ((visual->class == PseudoColor) || (visual->class == GrayScale)) {
    for (a= 0; a < image->rgb.used; a++) {
      if (! XAllocColorCells(disp, *cmap, False, NULL, 0, index + a, 1))
	if (newmap)
	  break;
	else {
	  *cmap= XCopyColormapAndFree(disp, *cmap);
	  newmap= 1;
	  a--;
	}
    }
    if (a < image->rgb.used)     /* can't get enough colors, so reduce */
      reduce(image, a, verbose); /* the colormap to fit what we have */
    for (a= 0; a < image->rgb.used; a++) {
      xcolor.pixel= *(index + a);
      xcolor.red= *(image->rgb.red + a);
      xcolor.green= *(image->rgb.green + a);
      xcolor.blue= *(image->rgb.blue + a);
      XStoreColor(disp, *cmap, &xcolor);
    }
  }
  else if ((visual->class == StaticColor) || (visual->class == StaticGray)) {
    for (a= 0; a < image->rgb.used; a++) {
      xcolor.red= *(image->rgb.red + a);
      xcolor.green= *(image->rgb.green + a);
      xcolor.blue= *(image->rgb.blue + a);
      if (! XAllocColor(disp, *cmap, &xcolor)) {
	printf("sendImageToX: XAllocColor failed on StaticGrey visual\n");
	return(0);
      }
      *(index + a)= xcolor.pixel;
    }
  }

  *pixmap= XCreatePixmap(disp, RootWindow(disp, scrn), image->width,
			 image->height, DefaultDepth(disp, scrn));

  /* blast the image across
   */

  switch (image->type) {
  case IBITMAP:
    gcv.function= GXcopy;
    gcv.foreground= *(index + 1);
    gcv.background= *index;
    gc= XCreateGC(disp, *pixmap, GCFunction | GCForeground | GCBackground,
		  &gcv);
    ximage= XCreateImage(disp, visual, image->depth, XYBitmap, 0, image->data,
			 image->width, image->height, 8, 0);
    ximage->bitmap_bit_order= MSBFirst;
    ximage->byte_order= MSBFirst;
    XPutImage(disp, *pixmap, gc, ximage, 0, 0, 0, 0,
	      image->width, image->height);
    XFreeGC(disp, gc);
    break;

  case IRGB:

    /* modify pixel values to fit the colormap
     */

    if (verbose) {
      printf("  Modifying image to conform to X colormap...");
      fflush(stdout);
    }
    pixptr= image->data;
    for (y= 0; y < image->height; y++)
      for (x= 0; x < image->width; x++) {
	valToMem(*(index + memToVal(pixptr, image->pixlen)),
		 pixptr, image->pixlen);
	pixptr += image->pixlen;
      }
    if (verbose)
      printf("done\n");

    gcv.function= GXcopy;
    gc= XCreateGC(disp, *pixmap, GCFunction, &gcv);
    ximage= XCreateImage(disp, visual, image->depth, ZPixmap, 0, image->data,
			 image->width, image->height, 8, 0);
    ximage->byte_order= MSBFirst; /* trust me, i know what i'm talking about */

    XPutImage(disp, *pixmap, gc, ximage, 0, 0,
	      0, 0, image->width, image->height);
    ximage->data= NULL;
    XDestroyImage(ximage); /* waste not want not */
    XFreeGC(disp, gc);
    break;

  default:
    printf("sendImageToX: bad image type\n");
    return(0);
  }
  lfree(index);
  return(1);
}
