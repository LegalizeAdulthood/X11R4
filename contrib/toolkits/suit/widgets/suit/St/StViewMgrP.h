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
 * Private declarations for StViewManagerWidgetClass
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

#ifndef _StViewManagerP_h
#define _StViewManagerP_h

#include <St/StViewMgr.h>

typedef struct {int empty;} StViewManagerClassPart;

typedef struct _StViewManagerClassRec {
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    StViewManagerClassPart  stview_manager_class;
} StViewManagerClassRec;

extern StViewManagerClassRec stViewManagerClassRec;

typedef struct _StViewManagerPart {
    /* resources */
    Boolean horizontal_scrollbar;/* Whether we allow horizontal scrollbars. */
    Boolean vertical_scrollbar;	 /* Whether we allow vertical scrollbars. */
    Boolean scrollbar_on_top;	 /* True if horiz bars appear at top. */
    Boolean scrollbar_on_right;	 /* True if vert bars appear at right. */

    /* private state */
    Widget clip;		/* The clipping child widgets */
    Widget frame;		/* The scrolling child widgets */
    Widget horiz_bar, vert_bar; /* What scrollbars we currently have. */
    Dimension disp_x, disp_y;	/* displacement from core window */
    Dimension children_width, children_height; /* total size of all children */
} StViewmanagerPart;

typedef struct _StViewManagerRec {
    CorePart	core;
    CompositePart	composite;
    StViewmanagerPart	stview_manager;
} StViewManagerRec;

#endif _StViewManagerP_h
