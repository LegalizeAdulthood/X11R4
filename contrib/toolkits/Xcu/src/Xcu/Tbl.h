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

#ifndef _Xcu_Tbl_h
#define _Xcu_Tbl_h

/*
 * Superclass Public Include File
 */

#include <X11/Constraint.h>
#include <Xcu/GravityR.h>
#include <Xcu/JustifyR.h>
#include <Xcu/ResizePR.h>

/*
 * New Resource Names
 */

#define XtNalignedColumns	"alignedColumns"
#define XtNclipOnShrink		"clipOnShrink"
#define XtNequalColumns		"equalColumns"
#define XtNequalRows		"equalRows"
#define XtNformatFile		"formatFile"
#define XtNformatString		"formatString"
#define XtNfillColumn		"fillColumn"
#define XtNinterWidth		"interWidth"
#define XtNinterHeight		"interHeight"
#define XtNinternalWidth	"internalWidth"
#define XtNinternalHeight	"internalHeight"

/*
 * Widget Type Definition
 */

typedef struct _XcuTblClassRec	*XcuTblWidgetClass;
typedef struct _XcuTblRec	*XcuTblWidget;

/*
 * Class Record Pointer
 */

extern WidgetClass xcuTblWidgetClass;
 
/*
 * Public Functions
 */

void XcuTblFormatString () ;
void XcuTblFormatFile () ;

#endif _Xcu_Tbl_h

