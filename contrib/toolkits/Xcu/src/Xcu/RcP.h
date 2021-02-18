/*
 * Copyright 1989 Cornell University
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Cornell U. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Cornell U. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * CORNELL UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORNELL UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Gene W. Dykes, Program of Computer Graphics
 *          120 Rand Hall, Cornell University, Ithaca, NY 14853
 *          (607) 255-6713   gwd@freedom.graphics.cornell.edu
 */

/* XcuRc widget private definitions */

#ifndef _Xcu_RcP_h
#define _Xcu_RcP_h

#include <Xcu/TblP.h>
#include <Xcu/Rc.h>

#define Dimension int
#define Cardinal int
#define Position int

typedef struct
    {
    int empty ;
    } XcuRcClassPart;

typedef struct _XcuRcClassRec
    {
    CoreClassPart	core_class;
    CompositeClassPart	composite_class;
    ConstraintClassPart	constraint_class;
    XcuSimpleClassPart  simple_class;
    XcuTblClassPart	tbl_class;
    XcuRcClassPart	rc_class;
    } XcuRcClassRec;

extern XcuRcClassRec xcuRcClassRec;

typedef struct _XcuRcPart
    {
    int empty ;
    } XcuRcPart;

typedef struct _XcuRcRec
    {
    CorePart		core;
    CompositePart	composite;
    ConstraintPart	constraint;
    XcuSimplePart	simple;
    XcuTblPart		tbl;
    XcuRcPart		rc ;
    } XcuRcRec;

typedef struct _XcuRcConstraintsPart
    {
    int	empty ;
    } XcuRcConstraintsPart;

typedef struct _XcuRcConstraintsRec
    {
    XcuTblConstraintsPart tbl ;
    XcuRcConstraintsPart rc ;
    } XcuRcConstraintsRec, *XcuRcConstraints;

/* Declarations of "Tbl Class" calls */

static String		MyString () ;

#endif _Xcu_RcP_h
