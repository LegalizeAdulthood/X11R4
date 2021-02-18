/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        ListP.h
 **
 **   Project:     X Widgets
 **
 **   Description: Private include file for List widget
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


/* List constraint record */
typedef struct _XwListConstraintRec {
    int		row,column;
    Boolean	selected;
    Boolean	managed;
    Boolean	highlighted;
    int		originalWidth,originalHeight;
    Pixel	fg,bg;
    int		lastHL;
} XwListConstraintRec;

/* New fields for the List class record */
typedef struct  {
    Boolean	InClassInit;
} XwListClassPart;

/* Full Class Record */
typedef struct _XwListClassRec {
   CoreClassPart        core_class;
   CompositeClassPart   composite_class;
   ConstraintClassPart  constraint_class;
   XwManagerClassPart   manager_class;
   XwSWindowClassPart   swindow_class;
   XwListClassPart      list_class;
} XwListClassRec;

extern XwListClassRec XwlistClassRec;

/****************
 *
 *  New fields for the List.
 *
 ****************/
typedef struct {
    int		numcolumns;		/* Number of columns */
    int		LastElementRow;		/* Last inserted element */
    int		LastElementColumn;	/* Last inserted element */
    int		Event;			/* Event mask */
    int		LastEnterX,LastEnterY;
    int		LastLeaveX,LastLeaveY;
    int		BBEnterX,BBEnterY;
    int		BBLeaveX,BBLeaveY;
    int		*MaxWidth,*MaxHeight;
    int		*RowNum;
    int		JumboHeight,JumboWidth;
    int		Selection;
    int		Method;
    int		Bias;
    int		ElementHeight;
    int		ColumnWidth;
    int		Style;
    int		NumSelected;
    int		DestroyMode;
    Boolean	AppendInProgress;
    Boolean	FromPadList;
    Boolean	InInit;
    Boolean	DummyDestroy;
    Boolean	InChangeManaged;
    Boolean	CMRemove;
    WidgetList	SelectedList;
    Widget	LastHLWidget;
    Widget	StartWidget;
    Widget	EndWidget;
    XwListRowColWidget	rcmanager;
    XtCallbackList	DoubleClick;
} XwListPart;

/*  The List instance record  */

typedef struct _XwListRec
{
   CorePart	    core;
   CompositePart    composite;
   ConstraintPart   constraint;
   XwManagerPart    manager;
   XwSWindowPart    swindow;
   XwListPart       list;
} XwListRec;

#define	DEFAULT_WIDTH 300
#define	DEFAULT_HEIGHT 300
