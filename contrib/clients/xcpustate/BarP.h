#include <X11/copyright.h>

/* $XConsortium: BarP.h,v 1.2 88/10/25 17:37:59 swick Exp $ */
/* Copyright	Massachusetts Institute of Technology	1987, 1988 */

#ifndef _BarP_h
#define _BarP_h

#include <X11/Xmu.h>
#include "Bar.h"
/* include superclass private header file */
#include <X11/CoreP.h>

/* define unique representation types not found in <X11/StringDefs.h> */

#define MAXGRAY 10

typedef struct {
    Pixmap *gray;
} BarClassPart;

typedef struct _BarClassRec {
    CoreClassPart	core_class;
    BarClassPart	bar_class;
} BarClassRec;

extern BarClassRec barClassRec;

typedef struct {
    /* private state */
    int *values;		/* Array of values displayed in bar */
    int nvalues;		/* Number of elements in values */
    XtOrientation orientation;	/* XtorientHorizontal or XtorientVertical */
    Dimension	  length;	/* either height or width */
    Dimension	  thickness;	/* either width or height */
    GC		  gc;		/* a gc */
} BarPart;

typedef struct _BarRec {
    CorePart	core;
    BarPart	bar;
} BarRec;

#endif  _BarP_h
