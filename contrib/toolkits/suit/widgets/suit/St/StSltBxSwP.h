/*
 *      Original Source: ./lib/Xaw/CommandP.h
 *
 * Changes
 * Modified by Fuji Xerox Co., Ltd.
 *
 *
 */
/*
* $XConsortium: CommandP.h,v 1.21 88/09/27 11:19:53 swick Exp $
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

#ifndef _StSelectBoxSwitchP_h
#define _StSelectBoxSwitchP_h

#include <St/StSltBxSw.h>
#include <St/StSimplSwP.h>

typedef struct _StSelectBoxSwitchClass {int yui;} StSelectBoxSwitchClassPart;

typedef struct _StSelectBoxSwitchClassRec {
    CoreClassPart		core_class;
    StPrimitiveClassPart	stPrimitive_class;
    StLabelClassPart		stLabel_class;
    StSimpleSwitchClassPart	stSimpSw_class;
    StSelectBoxSwitchClassPart	stSBoxSw_class;
} StSelectBoxSwitchClassRec;

extern StSelectBoxSwitchClassRec stSelectBoxSwitchClassRec;

typedef struct {int nanno;} StSelectBoxSwitchPart;

typedef struct _StSelectBoxSwitchRec {
    CorePart			core;
    StPrimitivePart		stPrimitive;
    StLabelPart			stLabel;
    StSimpleSwitchPart		stSimpSw;
    StSelectBoxSwitchPart	stSBoxSw;
} StSelectBoxSwitchRec;

#endif _StSelectBoxSwitchP_h
