/*****************************************************************************
 **
 **   File:        SRasterP.h
 **
 **   Project:     X Widgets
 **
 **   Description: Private include file for Static Raster class widget
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
 *****************************************************************************/


/***************************************************
 *
 *  New fields for the Static Raster widget class record
 *  and the full class record.
 *
 **************************************************/

typedef struct _SRasterClassPart
{
   int mumble;
} SRasterClassPart;

typedef struct _XwSRasterClassRec
{
   CoreClassPart      core_class;
   XwPrimitiveClassPart primitive_class;
   SRasterClassPart   sraster_class;
} SRasterClassRec;

/****************
 *
 *  Forward Declarations - I thought unix was smart!
 *
 ****************/
void    InitSRaster();
void    Realize();
void    Resize();
void    ShowSR();

/**************************************************
 *
 *  New fields for the Static Raster instance record and
 *  the full instance record.
 *
 ***************************************************/

typedef struct _SRasterPart
{
   XImage      * image;
   Boolean	 invert;
   Boolean	 poked;
   Boolean	 showme;
   GC            NormalGC;
   GC            InverseGC;
} SRasterPart;

typedef struct _XwSRasterRec
{
   CorePart       core;
   XwPrimitivePart  primitive;
   SRasterPart     sraster;
} SRasterRec;


