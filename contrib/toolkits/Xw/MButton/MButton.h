/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        MButton.h
 **
 **   Project:     X Widgets 
 **
 **   Description: Public include file for applications using the
 **                multibutton widget.
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

/***********************************************************************
 *
 * MultiButton Widget
 *
 ***********************************************************************/


/*  Resources for MultiButton  */

#define XtNlabels	"labels"
#define XtCLabels       "Labels"
#define XtRLabels       "Labels"
#define XtNinvertOnSelect "invertOnSelect"
#define XtCInvertOnSelect "InvertOnSelect"
#define XtNnumLabels    "numLabels"
#define XtCNumLabels    "NumLabels"

extern WidgetClass XwmultiButtonWidgetClass;

typedef struct _XwMultiButtonClassRec *XwMultiButtonWidgetClass;
typedef struct _XwMultiButtonRec      *XwMultiButtonWidget;






