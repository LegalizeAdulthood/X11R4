/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        SWindow.h
 **
 **   Project:     X Widgets
 **
 **   Description: Scrolled Window manager widget public include file
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
 * Scrolled Window Widget (subclass of ManagerClass)
 *
 ***********************************************************************/

/* Class record constants */


extern WidgetClass XwswindowWidgetClass;

typedef struct _XwSWindowClassRec *XwSWindowWidgetClass;
typedef struct _XwSWindowRec      *XwSWindowWidget;


/* synonyms added for consistent naming conventions */

extern WidgetClass XwscrolledWindowWidgetClass;

typedef struct _XwSWindowClassRec *XwScrolledWindowWidgetClass;
typedef struct _XwSWindowRec      *XwScrolledWindowWidget;



