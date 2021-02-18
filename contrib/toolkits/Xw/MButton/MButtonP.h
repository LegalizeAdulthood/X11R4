/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        MButtonP.h
 **
 **   Project:     X Widgets 
 **
 **   Description: Private include file for widgets which are
 **                subclasses of multibutton or which need to
 **                access directly the instance and class fields
 **                of the multibutton widget.
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

/********************************************
 *
 *   No new fields need to be defined
 *   for the MultiButton widget class record
 *
 ********************************************/

typedef struct {int nada;} XwMultiButtonClassPart;


/****************************************************
 *
 * Full class record declaration for MultiButton class
 *
 ****************************************************/
typedef struct _XwMultiButtonClassRec {
    CoreClassPart	  core_class;
    XwPrimitiveClassPart  primitive_class;
    XwButtonClassPart     button_class;
    XwMultiButtonClassPart multi_button_class;
} XwMultiButtonClassRec;


extern XwMultiButtonClassRec XwmultiButtonClassRec;


/********************************************
 *
 * New fields needed for instance record
 *
 ********************************************/

typedef struct _XwMultiButtonPart{
     String  *labels;
     int     num_labels;
     int     state;
     Boolean invert_on_select;
} XwMultiButtonPart;



/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _XwMultiButtonRec {
    CorePart	     core;
    XwPrimitivePart  primitive;
    XwButtonPart     button;
    XwMultiButtonPart multi_button;
} XwMultiButtonRec;
