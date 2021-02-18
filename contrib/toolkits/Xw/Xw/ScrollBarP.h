/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        ScrollBarP.h    
 **
 **   Project:     X Widgets
 **
 **   Description: Private include file for the ScrollBar class
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


/*  ScrollBar class structure  */

typedef struct _XwScrollBarClassPart
{
   int foo;	/*  No new fields needed  */
} XwScrollBarClassPart;


/*  Full class record declaration for ScrollBar class  */

typedef struct _XwScrollBarClassRec
{
   CoreClassPart        core_class;
   CompositeClassPart   composite_class;
   ConstraintClassPart  constraint_class;
   XwManagerClassPart   manager_class;
   XwScrollBarClassPart scrollbar_class;
} XwScrollBarClassRec;

extern XwScrollBarClassRec XwscrollbarClassRec;


/*  The ScrollBar instance record  */

typedef struct _XwScrollBarPart
{
   Pixel   highlight_color;
   int     highlight_tile;
   int     highlight_style;
   int     traversal_type;

   int min;		  /*  slider minimum coordiate position     */
   int max;		  /*  slider maximum coordiate position     */
   int origin;		  /*  slider edge location                  */
   int extent;		  /*  slider size                           */

   int orientation;       /*  vertical or horizontal scrolling      */

   int initial_delay;
   int repeat_rate;
   int granularity;

   XtCallbackList slider_moved;
   XtCallbackList slider_released;
   XtCallbackList area_selected;

   Boolean      origin_increasing;
   Boolean      timerProcessing;
   Boolean      initialProcessing;
   XtIntervalId timer;

   XwArrowWidget    left_or_up_arrow;
   XwArrowWidget    right_or_down_arrow;     
   XwValuatorWidget valuator;
} XwScrollBarPart;


/*  Full instance record declaration  */

typedef struct _XwScrollBarRec
{
   CorePart	    core;
   CompositePart    composite;
   ConstraintPart   constraint;
   XwManagerPart    manager;
   XwScrollBarPart  scrollbar;
} XwScrollBarRec;




