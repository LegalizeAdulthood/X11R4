/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        ValuatorP.h    
 **
 **   Project:     X Widgets
 **
 **   Description: Private include file for the Valuator class
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


/*  Minimum slider width or height  */

#define MIN_SLIDER_SIZE	4


/*  Valuator class structure  */

typedef struct _XwValuatorClassPart
{
   int foo;	/*  No new fields needed  */
} XwValuatorClassPart;


/*  Full class record declaration for CheckBox class  */

typedef struct _XwValuatorClassRec
{
   CoreClassPart        core_class;
   XwPrimitiveClassPart primitive_class;
   XwValuatorClassPart  valuator_class;
} XwValuatorClassRec;

extern XwValuatorClassRec XwvaluatorClassRec;


/*  The Valuator instance record  */

typedef struct _XwValuatorPart
{
   int min;		  /*  slider minimum coordiate position     */
   int max;		  /*  slider maximum coordiate position     */
   int origin;		  /*  slider edge location                  */
   int extent;		  /*  slider size                           */

   int slider_tile;       /*  pattern to use for the slider         */
   int orientation;       /*  vertical or horizontal scrolling      */

   XtCallbackList slider_moved;
   XtCallbackList slider_released;
   XtCallbackList area_selected;

   GC slider_GC;
   GC slider_fill_GC;

   int sliderX;
   int sliderY;
   int sliderWidth;
   int sliderHeight;
   Drawable pixmap;
   Boolean sliding_on;

} XwValuatorPart;


/*  Full instance record declaration  */

typedef struct _XwValuatorRec
{
   CorePart	     core;
   XwPrimitivePart   primitive;
   XwValuatorPart    valuator;
} XwValuatorRec;

