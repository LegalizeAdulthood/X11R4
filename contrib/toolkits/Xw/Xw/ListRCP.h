/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        ListRCP.h
 **
 **   Project:     X Widgets
 **
 **   Description: Private include file for ListRowCol widget
 **
 *****************************************************************************
 **   
 **   Copyright (c) 1988 by Hewlett-Packard Company
 **   Copyright (c) 1988 by the Massachusetts Institute of Technology
 **   
 **   Permission to use, copy, modify, and distribute this software 
 **   and its documentation for any purpose and without fee is hereby 
 **   granted, provided that the above copyright notice appear in all 
 **   copies and that both that copyright notice and this permission 
 **   notice appear in supporting documentation, and that the names of 
 **   Hewlett-Packard or  M.I.T.  not be used in advertising or publicity 
 **   pertaining to distribution of the software without specific, written 
 **   prior permission.
 **   
 *****************************************************************************
 *************************************<+>*************************************/

/* New fields for the RowCol widget class record */
typedef struct {
     int mumble;   /* No new procedures */
} XwRowColClassPart;

/* Full class record declaration */
typedef struct _XwListRowColClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    ConstraintClassPart constraint_class;
    XwManagerClassPart  manager_class;
    XwRowColClassPart	row_col_class;
} XwListRowColClassRec;

extern XwListRowColClassRec XwlistrowColClassRec;

/* New fields for the RowCol widget record */
typedef struct {
    Dimension   h_space, v_space;
    Widget      selectedChild;
    int         columns;
    int         mode;
} XwListRowColPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _XwListRowColRec {
    CorePart	    core;
    CompositePart   composite;
    ConstraintPart  constraint;
    XwManagerPart   manager;
    XwListRowColPart   row_col;
} XwListRowColRec;

