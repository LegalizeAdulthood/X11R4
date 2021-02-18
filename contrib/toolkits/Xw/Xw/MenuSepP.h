/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        MenuSepP.h    
 **
 **   Project:     X Widgets
 **
 **   Description: Private include file for the MenuSep class
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


/*  MenuSep class structure  */

typedef struct _XwMenuSepClassPart
{
   int foo;	/*  No new fields needed  */
} XwMenuSepClassPart;


/*  Full class record declaration for MenuSep class  */

typedef struct _XwMenuSepClassRec
{
   CoreClassPart         core_class;
   XwPrimitiveClassPart  primitive_class;
   XwButtonClassPart     button_class;
   XwMenuButtonClassPart menubutton_class;
   XwMenuSepClassPart    menuSep_class;
} XwMenuSepClassRec;

extern XwMenuSepClassRec XwmenuSepClassRec;


/*  The MenuSep instance record  */

typedef struct _XwMenuSepPart
{
   int separator_type;
   GC  separator_GC;
} XwMenuSepPart;


/*  Full instance record declaration  */

typedef struct _XwMenuSepRec
{
   CorePart	    core;
   XwPrimitivePart  primitive;
   XwButtonPart     button;
   XwMenuButtonPart menubutton;
   XwMenuSepPart    menuSep;
} XwMenuSepRec;

