/*
* $XConsortium: ReversiP.h,v 1.1 89/12/14 16:00:51 keith Exp $
*/


/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* 
 * ReversiP.h - Private definitions for Reversi widget
 * 
 */

#ifndef _XawReversiP_h
#define _XawReversiP_h

/***********************************************************************
 *
 * Reversi Widget Private Data
 *
 ***********************************************************************/

#include "Reversi.h"
#include "transform.h"
#include <X11/Xaw/SimpleP.h>

/* New fields for the ReversiBoard widget class record */

typedef struct {int foo;} ReversiBoardClassPart;

/* Full class record declaration */
typedef struct _ReversiBoardClassRec {
    CoreClassPart	core_class;
    SimpleClassPart	simple_class;
    ReversiBoardClassPart	reversi_class;
} ReversiBoardClassRec;

extern ReversiBoardClassRec reversiBoardClassRec;

typedef enum _AnimateState { AnimateNone, AnimateA, AnimateB } AnimateState;

typedef struct _Animate {
    AnimateState    state;
    ReversiStone    A, B;
    ReversiBoardWidget   rw;
    int		    x, y;
    unsigned long   delay;
    int		    togo;
    XtIntervalId    timeout;
    int		    (*callback)();
} Animate;

#define BOARD_WIDTH	8
#define BOARD_HEIGHT	8

/* New fields for the ReversiBoard widget record */
typedef struct {
    /* resources */
    Pixel	    white;
    Pixel	    black;
    Pixel	    grid;
    XtCallbackList  callbacks;

    /* private state */
    GC		    white_GC;
    GC		    black_GC;
    GC		    grid_GC;
    ReversiStone    board[BOARD_WIDTH][BOARD_HEIGHT];
    Animate	    animate;
    Transform	    t;
} ReversiBoardPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _ReversiBoardRec {
    CorePart	core;
    SimplePart	simple;
    ReversiBoardPart	reversi;
} ReversiBoardRec;

#endif /* _XawReversiP_h */
