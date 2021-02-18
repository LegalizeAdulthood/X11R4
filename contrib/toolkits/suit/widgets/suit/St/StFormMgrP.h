/*
 *	Original Source: ./lib/Xaw/FormP.h
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */
/* $XConsortium: FormP.h,v 1.12 88/09/06 16:41:26 jim Exp $ */
/* Copyright	Massachusetts Institute of Technology	1987 */
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

#ifndef _StFormManagerP_h
#define _StFormManagerP_h

#include <X11/FormP.h>
#include <St/StFormMgr.h>

typedef struct {int empty;} StFormManagerClassPart;

typedef struct _StFormManagerClassRec {
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    ConstraintClassPart	constraint_class;
    FormClassPart	form_class;
    StFormManagerClassPart  stform_manager_class;
} StFormManagerClassRec;

extern StFormManagerClassRec stFormManagerClassRec;

typedef struct _StFormManagerPart {
    Dimension	old_width, old_height; /* last known dimensions		 */
    int		default_spacing;    /* default distance between children */
    int		no_refigure;	    /* no re-layout while > 0		 */
    Boolean	needs_relayout;	    /* next time no_refigure == 0	 */
} StFormManagerPart;

typedef struct _StFormManagerRec {
    CorePart		core;
    CompositePart	composite;
    ConstraintPart	constraint;
    FormPart		form;
    StFormManagerPart	stform_manager;
} StFormManagerRec;

typedef struct _StFormManagerConstraintsPart {
	/* resources */
		
	/* private */
	int empty;
} StFormManagerConstraintsPart;

typedef struct _StFormManagerConstraintsRec {
    FormConstraintsPart	form;
    StFormManagerConstraintsPart stform_manager;
} StFormManagerConstraintsRec, *StFormManagerConstraints;

#endif _StFormManagerP_h
