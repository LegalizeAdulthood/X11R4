/*
 * Copyright 1989 Dirk Grunwald
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  Dirk
 * Grunwald and M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Dr. Dirk Grunwald
 * 	Dept. of Computer Science
 * 	Campus Box 430
 * 	Univ. of Colorado, Boulder
 * 	Boulder, CO 80309
 * 
 * 	grunwald@colorado.edu
 * 	
 */ 

#ifndef _DviPageP_h
#define _DviPageP_h

#include "DviPage.h"
/* include superclass private header file */

#include <X11/CoreP.h>

typedef struct {
    int empty;
} DviPageClassPart;

typedef struct _DviPageClassRec {
    CoreClassPart	core_class;
    DviPageClassPart	dviPage_class;
} DviPageClassRec;

extern DviPageClassRec dviPageClassRec;

typedef struct {
	i32	stack_hh;
	i32	stack_vv;
	i32	h;		/* the saved h */
	i32	v;		/* the saved v */
	i32	w;		/* etc */
	i32	x;
	i32	y;
	i32	z;
} LocalDviStack;

typedef struct {
    Pixel	background;
    Pixel	foreground;

    int useBackingStore;	/* backing store type */
    Bool haveBackingStore;

    Bool reverseVideo;

    XFontStruct *font;

    /* private state */
    
    GC globalGC;

    char *page;

    Dimension pixelsWide;
    Dimension pixelsHigh;

    Dimension pixelsPerInchHoriz;
    Dimension pixelsPerInchVert;

    char *paperWidthString;
    char *paperHeightString;
    float paperWidth;
    float paperHeight;

    char *topOffsetString;
    char *leftOffsetString;
    float topOffset;
    float leftOffset;

    int maxDriftString;	/* in points */

    XFontStruct **xFonts;

    struct PostAmbleInfo *pai;
    DviFontInfo *dviFonts;

    int userMag;

    double dpi;
    char *dpiString;

    double dpiHoriz;
    char *dpiHorizString;

    double dpiVert;
    char *dpiVertString;

    long fastFromSpValueHoriz;
    long fastFromSpValueVert;

    Region updateRegion;

    LocalDviStack *dviStack;
    LocalDviStack *dviStackPointer;
    int dviStackSize;

    /* for tpic specials */
#define	MAXPOINTS	300	/* Max points in a path */
    int xx[MAXPOINTS];
    int yy[MAXPOINTS];
    int pathLen;
    int penSize;
    Bool whiten;
    Bool shade;
    Bool blacken;
} DviPagePart;

typedef struct _DviPageRec {
    CorePart		core;
    DviPagePart	dviPage;
} DviPageRec;


#define fastFromSpVert(w, x) ((x) / (w -> dviPage.fastFromSpValueVert))
#define fastFromSpHoriz(w, x) ((x) / (w -> dviPage.fastFromSpValueHoriz))

#endif  _DviPageP_h
