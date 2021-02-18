/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        CreateTile.c
 **
 **   Project:     X Widgets
 **
 **   Description: This file contains source for the tile creation
 **                routine used for patterned area fills, tiled
 **                text writes, etc...
 **
 *****************************************************************************
 **   
 **   Copyright (c) 1988 by Hewlett-Packard Company
 **   Copyright (c) 1988 by the Massachusetts Institute of Technology
 **   
 **   Permission to use, copy, modify, and distribute this software 
 **   and its documentation for any purpose and without fee is hereby 
 **   granted, provided that the above copyright notice appear in all 
 **   copies and that both that copyright notice and this permission 
 **   notice appear in supporting documentation, and that the names of 
 **   Hewlett-Packard or  M.I.T.  not be used in advertising or publicity 
 **   pertaining to distribution of the software without specific, written 
 **   prior permission.
 **   
 *****************************************************************************
 *************************************<+>*************************************/


#include <stdio.h>
#include <ctype.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <Xw/XwP.h>
#include <Xw/Xw.h>
#include <X11/StringDefs.h>


/*  Tile types that can be created  */

static unsigned char bitmaps [9][32] =
{
/* Solid Foreground */
{  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },

/* Solid Background */
{  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },

/* 25 percent */
{  0x88, 0x88, 0x22, 0x22, 0x88, 0x88, 0x22, 0x22, 0x88, 0x88, 0x22, 0x22,
   0x88, 0x88, 0x22, 0x22, 0x88, 0x88, 0x22, 0x22, 0x88, 0x88, 0x22, 0x22,
   0x88, 0x88, 0x22, 0x22, 0x88, 0x88, 0x22, 0x22 },

/* 50 percent */
{  0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa,
   0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa,
   0x55, 0x55, 0xaa, 0xaa, 0x55, 0x55, 0xaa, 0xaa },

/* 75 percent */
{  0x55, 0x55, 0xff, 0xff, 0xaa, 0xaa, 0xff, 0xff, 0x55, 0x55, 0xff, 0xff,
   0xaa, 0xaa, 0xff, 0xff, 0x55, 0x55, 0xff, 0xff, 0xaa, 0xaa, 0xff, 0xff,
   0x55, 0x55, 0xff, 0xff, 0xaa, 0xaa, 0xff, 0xff },

/* Vertical */
{  0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
   0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
   0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55 },

/* Horizontal */
{  0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
   0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
   0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00 },

/* Left Slant */
{  0xee, 0xee, 0xdd, 0xdd, 0xbb, 0xbb, 0x77, 0x77, 0xee, 0xee, 0xdd, 0xdd,
   0xbb, 0xbb, 0x77, 0x77, 0xee, 0xee, 0xdd, 0xdd, 0xbb, 0xbb, 0x77, 0x77,
   0xee, 0xee, 0xdd, 0xdd, 0xbb, 0xbb, 0x77, 0x77 },

/* Right Slant */
{  0x77, 0x77, 0xbb, 0xbb, 0xdd, 0xdd, 0xee, 0xee, 0x77, 0x77, 0xbb, 0xbb,
   0xdd, 0xdd, 0xee, 0xee, 0x77, 0x77, 0xbb, 0xbb, 0xdd, 0xdd, 0xee, 0xee,
   0x77, 0x77, 0xbb, 0xbb, 0xdd, 0xdd, 0xee, 0xee }
};


/*  Tile caching structure  */

typedef struct _PixmapCache
{
   Screen * screen;
   Pixel    foreground;
   Pixel    background;
   int      tileType;
   Pixmap   pixmap;
   struct _PixmapCache * next;
} CacheEntry;

static CacheEntry * pixmapCache = NULL;



/************************************************************************
 *
 *  XwCreateTile
 *	Create a tile of screen depth, using the foreground and 
 *      background colors specified.  Use the type parameter to
 *      select the particular tile to create.  Ensure that multiple
 *      tiles of the same attributes are not created by caching the
 *      tiles.
 *
 ************************************************************************/

Pixmap XwCreateTile (screen, foreground, background, tileType)
Screen * screen;
Pixel    foreground;
Pixel    background;
int      tileType;

{
   register Display * display = DisplayOfScreen(screen);
   XImage       image;
   CacheEntry * cachePtr;
   Pixmap       tile;
   GC gc;
   XGCValues gcValues;


    /*  Check for a matching pixmap  */

    for (cachePtr = pixmapCache; cachePtr; cachePtr = cachePtr -> next)
	if (cachePtr -> screen == screen         && 
            cachePtr -> foreground == foreground &&
            cachePtr -> background == background &&
            cachePtr -> tileType == tileType)
          return (cachePtr -> pixmap);


#ifdef SERVER_NOT_BROKEN
    /*  Fill out the image structure for the new tile and create the pixmap */

    image.height = 16;
    image.width = 16;
    image.xoffset = 0;
    image.format = XYBitmap;
    image.data = (char*) bitmaps[tileType];
    image.byte_order = ImageByteOrder(display);
    image.bitmap_pad = BitmapPad(display);
    image.bitmap_bit_order = BitmapBitOrder(display);
    image.bitmap_unit = BitmapUnit(display);
    image.depth = 1;
    image.bytes_per_line = 2;
    image.obdata = NULL;

    tile = XCreatePixmap (display, RootWindowOfScreen(screen), 
		          16, 16, DefaultDepthOfScreen (screen));
#else SERVER_NOT_BROKEN
    tile = XCreatePixmapFromBitmapData(display, RootWindowOfScreen(screen),
				       bitmaps+tileType,
				       16, 16, foreground, background,
				       DefaultDepthOfScreen(screen));
#endif SERVER_NOT_BROKEN


    /*  Insert it at the head of the cache  */

    cachePtr = XtNew (CacheEntry);
    cachePtr -> screen = screen;
    cachePtr -> foreground = foreground;
    cachePtr -> background = background;
    cachePtr -> tileType = tileType;
    cachePtr -> pixmap = tile;
    cachePtr -> next = pixmapCache;
    pixmapCache = cachePtr;


#ifdef SERVER_NOT_BROKEN
    /*  Store the image into the tile  */

    gcValues.foreground = foreground;
    gcValues.background = background;
    gc = XCreateGC (display, RootWindowOfScreen(screen),
		    GCForeground | GCBackground, &gcValues);

    XPutImage (display, tile, gc, &image, 0, 0, 0, 0, 16, 16);

    XFreeGC (display, gc);
#endif SERVER_NOT_BROKEN

    return (tile);
}






