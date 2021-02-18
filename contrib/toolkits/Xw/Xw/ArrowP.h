/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        ArrowP.h    
 **
 **   Project:     X Widgets
 **
 **   Description: Private include file for the Arrow class
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


/*  Arrow class structure  */

typedef struct _XwArrowClassPart
{
   int foo;	/*  No new fields needed  */
} XwArrowClassPart;


/*  Full class record declaration for Arrow class  */

typedef struct _XwArrowClassRec
{
   CoreClassPart         core_class;
   XwPrimitiveClassPart  primitive_class;
   XwArrowClassPart      arrow_class;
} XwArrowClassRec;

extern XwArrowClassRec XwarrowClassRec;


/*  The Arrow instance record  */

typedef struct _XwArrowPart
{
   int     direction;	  /*  the direction the arrow is pointing  */
   Boolean selected;


   GC      arrow_GC;	  /*  graphics context for arrow drawing   */
} XwArrowPart;


/*  Full instance record declaration  */

typedef struct _XwArrowRec
{
   CorePart	     core;
   XwPrimitivePart   primitive;
   XwArrowPart       arrow;
} XwArrowRec;

