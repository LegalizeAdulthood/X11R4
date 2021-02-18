/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        SashP.h
 **
 **   Project:     X Widgets 
 **
 **   Description: Private file for subclasses of sash or for widgets
 **                which need direct access to instance or class files.
 **
 *****************************************************************************
 **   
 **   Copyright (c) 1988 by Hewlett-Packard Company
 **   Copyright (c) 1987, 1988 by Digital Equipment Corporation, Maynard,
 **             Massachusetts, and the Massachusetts Institute of Technology,
 **             Cambridge, Massachusetts
 **   
 **   Permission to use, copy, modify, and distribute this software 
 **   and its documentation for any purpose and without fee is hereby 
 **   granted, provided that the above copyright notice appear in all 
 **   copies and that both that copyright notice and this permission 
 **   notice appear in supporting documentation, and that the names of 
 **   Hewlett-Packard, Digital or  M.I.T.  not be used in advertising or 
 **   publicity pertaining to distribution of the software without 
 **   written prior permission.
 **   
 **   DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 **   ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 **   DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 **   ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 **   WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 **   ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 **   SOFTWARE.
 **   
 *****************************************************************************
 *************************************<+>*************************************/

/*
 *  SashP.h - Private definitions for Sash widget (Used by VPane Widget)
 *
 */

#include <Xw/Sash.h>

/*****************************************************************************
 *
 * Sash Widget Private Data
 *
 *****************************************************************************/

/* New fields for the Sash widget class record */
typedef struct {int empty;} XwSashClassPart;

/* Full Class record declaration */
typedef struct _XwSashClassRec {
    CoreClassPart         core_class;
    XwPrimitiveClassPart  primitive_class;
    XwSashClassPart    sash_class;
} XwSashClassRec;

extern XwSashClassRec XwsashClassRec;

/* New fields for the Sash widget record */
typedef struct {
  XtCallbackList sash_action;
} XwSashPart;

/*****************************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************************/

typedef struct _XwSashRec {
   CorePart         core;
   XwPrimitivePart  primitive;
   XwSashPart       sash;
} XwSashRec;



