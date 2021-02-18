/* image.h:
 *
 * portable image type declarations
 *
 * jim frost 10.02.89
 *
 * Copyright 1989 Jim Frost.  See included file "copyright.h" for complete
 * copyright information.
 */

#include "copyright.h"
#include <stdio.h>

typedef unsigned long  Pixel;     /* what X thinks a pixel is */
typedef unsigned short Intensity; /* what X thinks an RGB intensity is */
typedef unsigned char  byte;      /* byte type */

typedef struct {
  unsigned int  type;
  FILE         *stream;
} ZFILE;

#define ZSTANDARD 0
#define ZPIPE     1

typedef struct rgbmap {
  unsigned int  size;  /* size of RGB map */
  unsigned int  used;  /* number of colors used in RGB map */
  Intensity    *red;   /* color values in X style */
  Intensity    *green;
  Intensity    *blue;
} RGBMap;

/* image structure
 */

typedef struct {
  char         *title;  /* name of image */
  unsigned int  type;   /* type of image */
  RGBMap        rgb;    /* RGB map of image if IRGB type */
  unsigned int  width;  /* width of image in pixels */
  unsigned int  height; /* height of image in pixels */
  unsigned int  depth;  /* depth of image in bits if IRGB type */
  unsigned int  pixlen; /* length of pixel if IRGB type */
  byte         *data;   /* data rounded to full byte for each row */
} Image;

#define IBITMAP 0 /* image is a bitmap */
#define IRGB    1 /* image is RGB */

#define BITMAPP(IMAGE) ((IMAGE)->type == IBITMAP)
#define RGBP(IMAGE)    ((IMAGE)->type == IRGB)

/* function declarations
 */

Image *clip(); /* clip.c */

Image *dither(); /* dither.c */

void fold(); /* fold.c */

Image *loadImage(); /* imagetypes.c */
void   identifyImage();
void   goodImage();

Image *mergeImages(); /* merge.c */

char  *dupString(); /* new.c */
Image *newBitImage();
Image *newRGBImage();
void   freeImage();
void   freeImageData();
void   newRGBMapData();
void   freeRGBMapData();
byte  *lcalloc();
byte  *lmalloc();
void   lfree();

void reduceRGBMap(); /* reduce.c */
void reduce();

unsigned long memToVal(); /* value.c */
void          valToMem();

Image *zoom(); /* zoom.c */
