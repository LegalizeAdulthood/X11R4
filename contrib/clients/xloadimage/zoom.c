/* zoom.c:
 *
 * zoom an image
 *
 * jim frost 10.11.89
 *
 * Copyright 1989 Jim Frost.  See included file "copyright.h" for complete
 * copyright information.
 */

#include "copyright.h"
#include "image.h"

static unsigned int *buildIndex(width, zoom, rwidth)
     unsigned int  width;
     unsigned int  zoom;
     unsigned int *rwidth;
{ float         fzoom;
  unsigned int *index;
  unsigned int  a;

  if (!zoom) {
    fzoom= 100.0;
    *rwidth= width;
  }
  else {
    fzoom= (float)zoom / 100.0;
    *rwidth= fzoom * width;
  }
  index= (unsigned int *)lmalloc(sizeof(unsigned int) * *rwidth);
  for (a= 0; a < *rwidth; a++)
    if (zoom)
      *(index + a)= (float)a / fzoom;
    else
      *(index + a)= a;
  return(index);
}

Image *zoom(oimage, xzoom, yzoom, verbose)
     Image        *oimage;
     unsigned int  xzoom, yzoom;
{ char          buf[BUFSIZ];
  Image        *image;
  unsigned int *xindex, *yindex;
  unsigned int  xwidth, ywidth;
  unsigned int  x, y, xsrc, ysrc;
  unsigned int  pixlen;
  unsigned int  srclinelen;
  byte         *srcline, *srcptr;
  byte         *destptr;
  byte          srcmask, destmask, bit;
  Pixel         value;

  goodImage(oimage, "zoom");

  if (!xzoom && !yzoom) /* stupid user */
    return(NULL);

  if (verbose) {
    if (!xzoom) {
      printf("  Zooming image Y axis by %d%%...", yzoom);
      sprintf(buf, "%s (Y zoom %d%%)", oimage->title, yzoom);
    }
    else if (!yzoom) {
      printf("  Zooming image X axis by %d%%...", xzoom);
      sprintf(buf, "%s (X zoom %d%%)", oimage->title, xzoom);
    }
    else if (xzoom == yzoom) {
      printf("  Zooming image by %d%%...", xzoom);
      sprintf(buf, "%s (%d%% zoom)", oimage->title, xzoom);
    }
    else {
      printf("  Zooming image X axis by %d%% and Y axix by %d%%...",
	     xzoom, yzoom);
      sprintf(buf, "%s (X zoom %d%% Y zoom %d%%)", oimage->title,
	      xzoom, yzoom);
    }
    fflush(stdout);
  }

  xindex= buildIndex(oimage->width, xzoom, &xwidth);
  yindex= buildIndex(oimage->height, yzoom, &ywidth);

  switch (oimage->type) {
  case IBITMAP:
    image= newBitImage(xwidth, ywidth);
    for (x= 0; x < oimage->rgb.used; x++) {
      *(image->rgb.red + x)= *(oimage->rgb.red + x);
      *(image->rgb.green + x)= *(oimage->rgb.green + x);
      *(image->rgb.blue + x)= *(oimage->rgb.blue + x);
    }
    image->rgb.used= oimage->rgb.used;
    destptr= image->data;
    srcline= oimage->data;
    srclinelen= (oimage->width / 8) + (oimage->width % 8 ? 1 : 0);
    for (y= 0, ysrc= *(yindex + y); y < ywidth; y++) {
      while (ysrc != *(yindex + y)) {
	ysrc++;
	srcline += srclinelen;
      }
      srcptr= srcline;
      srcmask= 0x80;
      destmask= 0x80;
      bit= srcmask & *srcptr;
      for (x= 0, xsrc= *(xindex + x); x < xwidth; x++) {
	if (xsrc != *(xindex + x)) {
	  do {
	    xsrc++;
	    if (!(srcmask >>= 1)) {
	      srcmask= 0x80;
	      srcptr++;
	    }
	  } while (xsrc != *(xindex + x));
	  bit= srcmask & *srcptr;
	}
	if (bit)
	  *destptr |= destmask;
	if (!(destmask >>= 1)) {
	  destmask= 0x80;
	  destptr++;
	}
      }
    }
    break;

  case IRGB:
    image= newRGBImage(xwidth, ywidth, oimage->depth);
    for (x= 0; x < oimage->rgb.used; x++) {
      *(image->rgb.red + x)= *(oimage->rgb.red + x);
      *(image->rgb.green + x)= *(oimage->rgb.green + x);
      *(image->rgb.blue + x)= *(oimage->rgb.blue + x);
    }
    image->rgb.used= oimage->rgb.used;
    pixlen= oimage->pixlen;
    destptr= image->data;
    srcline= oimage->data;
    srclinelen= oimage->width * pixlen;
    for (y= 0, ysrc= *(yindex + y); y < ywidth; y++) {
      while (ysrc != *(yindex + y)) {
	ysrc++;
	srcline += srclinelen;
      }

      srcptr= srcline;
      value= memToVal(srcptr, pixlen);
      for (x= 0, xsrc= *(xindex + x); x < xwidth; x++) {
	if (xsrc != *(xindex + x)) {
	  do {
	    xsrc++;
	    srcptr += image->pixlen;
	  } while (xsrc != *(xindex + x));
	  value= memToVal(srcptr, pixlen);
	}
	valToMem(value, destptr++, pixlen);
      }
    }
    break;
  }

  image->title= dupString(buf);
  lfree(xindex);
  lfree(yindex);
  if (verbose)
    printf("done\n");
  return(image);
}
