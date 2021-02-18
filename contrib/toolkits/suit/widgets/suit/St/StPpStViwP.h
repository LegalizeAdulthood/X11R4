/*
 *	Original Source: ./lib/Xaw/ViewportP.h
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */

/*
 * $XConsortium: ViewportP.h,v 1.8 88/09/06 16:42:50 jim Exp $
 * Private declarations for StPopupSheetViewWidgetClass
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

/************************************************************
Copyright 1989 by Information-Technology Promotion Agency Japan (IPA),
Mitsubishi Research Institute, Inc. (MRI), and Fuji Xerox Co., Ltd. (FX)

                        All Rights Reserved

Permission to use, copy, modify and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IPA, MRI or FX not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IPA, MRI or FX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL IPA, MRI or FX BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

********************************************************/

#ifndef _StPopupSheetViewP_h
#define _StPopupSheetViewP_h

#include <St/StPpStViw.h>

typedef struct {int empty;} StPopupSheetViewClassPart;

typedef struct _StPopupSheetViewClassRec {
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    StPopupSheetViewClassPart	view_class;
} StPopupSheetViewClassRec;

extern StPopupSheetViewClassRec stPopupSheetViewClassRec;

typedef struct _StPopupSheetViewPart {
    /* resources */
    Dimension bottommargin;	/* Whether we should always display */
    Dimension topmargin;	/* Whether we should always display */
    Dimension leftmargin;	/* Whether we should always display */
    Dimension rightmargin;	/* Whether we should always display */
    Dimension maxheight;	/* Whether we should always display */
    Dimension maxwidth;		/* Whether we should always display */
    Dimension minheight;	/* Whether we should always display */
    Dimension minwidth;		/* Whether we should always display */
    String    titlename;	/* Whether we should always display */
    Pixel     titlebackground;	/* Whether we should always display */
    Pixel     titleforeground;	/* Whether we should always display */
    Pixmap    titlebackgroundpixmap;	/* Whether we should always display */
    Pixel     titlebordercolor;	/* Whether we should always display */
    Pixmap    titleborderpixmap;/* Whether we should always display */
    Dimension titleborderwidth; /* Whether we should always display */
    XFontStruct *titlefont;/* Whether we should always display */
    XtJustify titlejustify;		/* Whether we should always display */
    Boolean   windowable;	/* True iff windowable . */

    /* private state */
    Widget    childW;		/*  child widget */
    Widget    mainW;		/*  main flame widget */
    Dimension mainwidth;	/*  main widget currentry width */
    Dimension mainheight;	/*  main widget currentry height */
    Position  mainx;		/*  main widget currentry position */
    Position  mainy;		/*  main widget currentry position */
    Dimension mainborder_width; /*  main widget border width */
    Widget    titleW;		/*  title widget */
    Dimension titlewidth;	/*  title widget currentry width */
    Dimension titleheight;	/*  title widget currentry height */
    Position  titlex;		/*  title widget currentry position */
    Position  titley;		/*  title widget currentry position */
    Widget    marginW;		/*  margin widget */
    Dimension marginewidth;	/*  margine widget currentry width */
    Dimension margineheight;	/*  margine widget currentry height */
    Position  marginex;		/*  margine widget currentry position */
    Position  marginey;		/*  margine widget currentry position */
    Widget    windcomW;		/*  windcom widget */
    Dimension windcomwidth;	/*  windcom widget currentry width */
    Dimension windcomheight;	/*  windcom widget currentry height */
    Position  windcomx;		/*  windcom widget currentry position */
    Position  windcomy;		/*  windcom widget currentry position */
    Boolean   iswindow;		/* True if is window */
    Boolean   setval;		/* True if setvalued */
    Boolean   reparent; 	/* True if child has been re-parented */
    Dimension shadow_width;	/*  width of shadow window */
    GC	      grayGC;		/*  gray color GC */
    GC	      normalGC;		/*  normal color GC */
} StPopupSheetViewPart;

typedef struct _StPopupSheetViewRec {
    CorePart	core;
    CompositePart	composite;
    StPopupSheetViewPart	stpopupsheetview;
} StPopupSheetViewRec;



#endif _StPopupSheetViewP_h
