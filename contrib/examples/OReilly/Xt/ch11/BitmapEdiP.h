/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */

/* 
 * BitmapEditP.h - Private definitions for BitmapEdit widget
 */

#ifndef _ORABitmapEditP_h
#define _ORABitmapEditP_h

/*
 * This include not needed unless the .c file includes IntrinsicP.h
 * after this file.   Anyway, it doesn't hurt.
 */
#include <X11/CoreP.h>

/*
 * This one is always needed!
 */
#include "BitmapEdit.h"

/* New fields for the BitmapEdit widget class record */

typedef struct {
	int make_compiler_happy;	/* keep compiler happy */
} BitmapEditClassPart;

/* Full class record declaration */
typedef struct _BitmapEditClassRec {
    CoreClassPart	core_class;
    BitmapEditClassPart	bitmapEdit_class;
} BitmapEditClassRec;

extern BitmapEditClassRec bitmapEditClassRec;

/* New fields for the BitmapEdit widget record */
typedef struct {
    /* resources */
    Pixel	foreground;
    XtCallbackList callback;	/* application installed callback function(s) */
    Dimension	pixmap_width_in_cells;
    Dimension	pixmap_height_in_cells;
    int cell_size_in_pixels;
    int cur_x, cur_y;  /* position of visible corner in big pixmap */
    char *cell;	/* array for keeping track of array of bits */

    /* private state */
    Dimension	pixmap_width_in_pixels;
    Dimension	pixmap_height_in_pixels;
    Pixmap big_picture;
    GC		draw_gc;	/* one plane, for drawing into pixmap */
    GC		undraw_gc;	/* one plane, for drawing into pixmap */
    GC		copy_gc;	/* defaultdepthofscreen, for copying pixmap into window */
} BitmapEditPart;

/*
 * Full instance record declaration
 */
typedef struct _BitmapEditRec {
    CorePart		core;
    BitmapEditPart	bitmapEdit;
} BitmapEditRec;

#endif /* _ORABitmapEditP_h */
