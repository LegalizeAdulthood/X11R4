/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        ImageEditP.h    
 **
 **   Project:     X Widgets
 **
 **   Description: Private include file for the ImageEdit class
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


/*  ImageEdit class structure  */

typedef struct _XwImageEditClassPart
{
   int foo;	/*  No new fields needed  */
} XwImageEditClassPart;


/*  Full class record declaration for ImageEdit class  */

typedef struct _XwImageEditClassRec
{
   CoreClassPart         core_class;
   XwPrimitiveClassPart  primitive_class;
   XwImageEditClassPart  imageEdit_class;
} XwImageEditClassRec;

extern XwImageEditClassRec XwimageEditClassRec;


/*  The ImageEdit instance record  */

typedef struct _XwImageEditPart
{
   int          pixel_scale;
   int          grid_thickness;
   XImage     * image; 
   Pixel	draw_color;
   Pixel	erase_color;
   Boolean	erase_on;

   int          old_width;
   int          old_height;
   int          pt_x;
   int          pt_y;
   GC           imageEdit_GC;  /*  graphics context for imageEdit drawing   */
   GC           imageEditErase_GC;
   GC           draw_GC;
   Pixel	draw_Pixel;
} XwImageEditPart;


/*  Full instance record declaration  */

typedef struct _XwImageEditRec
{
   CorePart	   core;
   XwPrimitivePart primitive;
   XwImageEditPart imageEdit;
} XwImageEditRec;

