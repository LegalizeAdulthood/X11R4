/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        RCManagerP.h
 **
 **   Project:     X Widgets
 **
 **   Description: Private include file for the RCManager class
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


/*  The RC manager constraint structure  */

typedef struct _XwRCConstraintRec
{
   Dimension set_width;
   Dimension set_height;
} XwRCConstraintRec;


/*  Internally used layout structure  */

typedef struct _XwRCLayoutList
{
   Widget widget;
   int border;
   int x, y;
   Dimension width, height;
   Dimension set_width, set_height;
} XwRCLayoutList;


/*  New fields for the RowCol widget class record  */

typedef struct
{
   int mumble;   /* No new procedures */
} XwRowColClassPart;


/* Full class record declaration */

typedef struct _XwRowColClassRec
{
   CoreClassPart       core_class;
   CompositeClassPart  composite_class;
   ConstraintClassPart constraint_class;
   XwManagerClassPart  manager_class;
   XwRowColClassPart   row_col_class;
} XwRowColClassRec;

extern XwRowColClassRec XwrowColClassRec;


/* New fields for the RowCol widget record */

typedef struct
{
   Dimension  h_space, v_space;
   Widget     selectedChild;
   int        layout_type;
   int        columns;
   Boolean    force_size;
   Boolean    single_row;
   int        mode;
} XwRowColPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _XwRowColRec
{
    CorePart	   core;
    CompositePart  composite;
    ConstraintPart constraint;
    XwManagerPart  manager;
    XwRowColPart   row_col;
} XwRowColRec;

