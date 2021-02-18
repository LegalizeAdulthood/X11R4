/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        VPWP.h
 **
 **   Project:     X Widgets 
 **
 **   Description: Private file for subclasses of vertical paned window,
 **                or for widgets which need direct access to instance or 
 **                class files.
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

/*********************************************************************
 *
 * XwVPaned Widget Private Data
 *
 *********************************************************************/


/* New fields for the VPaned widget class record */

typedef struct _XwVPanedClassPart {
    int mumble;
} XwVPanedClassPart;


/* Full Class record declaration */

typedef struct _XwVPanedClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    ConstraintClassPart constraint_class;
    XwManagerClassPart  manager_class;
    XwVPanedClassPart     vpaned_class;
} XwVPanedClassRec;

extern XwVPanedClassRec XwvPanedClassRec;


/* VPaned constraint record */

typedef struct _XwVPanedConstraintsPart {
    Boolean     isPane;         /* true if constraint of pane, false if
				   constraint of sash */
    int		position;	/* position location in VPaned */
    Position	dy;		/* Desired Location */
    Position	olddy;		/* The last value of dy. */
    Dimension	min;		/* Minimum height */
    Dimension	max;		/* Maximum height */
    Boolean	allow_resize;	/* TRUE iff child resize requests are ok */
    Boolean	skip_adjust;	/* TRUE iff child's height should not be */
				/* changed without explicit user action. */
    Dimension	dheight;	/* Desired height */
    Widget	sash;		/* The sash for this child */
} XwVPanedConstraintsPart, *Pane;

typedef struct _XwVPanedConstraintsRec {
    XwVPanedConstraintsPart vpaned;
} XwVPanedConstraintsRec, *XwVPanedConstraints;


/* New Fields for the VPaned widget record */

typedef struct {
    /* resources */
    Position    sash_indent;               /* Location of sashs (offset	*/
                                           /*   from right margin)	*/
    Boolean     refiguremode;              /* Whether to refigure changes right now */
    int         padding;                   /* whitespace around panes */
    int         border_frame;              /* enforced pane border width */
    Boolean     framed;                    /* if true, leave some whitespace
					      around window, else leave none */

    /* private */
    Boolean     recursively_called;        /* For Changed Managed */
    int         starty;                    /* mouse origin when adjusting */
    Pane        top_pane;                  /* Pane closest to 0 index */
    Pane        bottom_pane;               /* Pane farthest away from 0 index*/
    GC          flipgc;                    /* GC to use when animating borders */
    int         pane_count;                /* number of managed panes */

} XwVPanedPart;

/**************************************************************************
 *
 * Full instance record declaration
 *
 **************************************************************************/

typedef struct _XwVPanedRec {
    CorePart       core;
    CompositePart  composite;
    ConstraintPart constraint;
    XwManagerPart  manager;
    XwVPanedPart   v_paned;
} XwVPanedRec;


