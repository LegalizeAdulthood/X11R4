/* 
**---------------------------------------------------------------------
** Tiff.h
**---------------------------------------------------------------------
**      Copyright 1988 by Paul J. Emerson 
**                                                                     
**      Permission to use, copy, modify, and distribute this software 
**      and its documentation for any purpose and without fee is        
**      hereby granted, provided that the above copyright notice        
**      appear in all copies and that both that copyright notice and    
**      this permission notice appear in supporting documentation, and  
**      that the name of Paul J. Emerson not be used in advertising or 
**      publicity pertaining to distribution of the software without 
**      specific, written prior permission.  Paul J. Emerson makes no 
**      representations about the suitability of this software for 
**      any purpose.  It is provided "as is" without express or implied 
**      warranty.          
**
**      Version 1.0 
**        Paul Emerson (ucf-cs!sdgsun!paul) December 1988
**
**---------------------------------------------------------------------
*/

/* Types as defined in the TIFF standard for SUN3 */

typedef unsigned char  BYTE;         /* 8-bits */
typedef unsigned char  ASCII;        /* 8-bits */
typedef unsigned short SHORT;        /* 16 bits */
typedef unsigned int   LONG;         /* 32 bits */
typedef unsigned long  RATIONAL;     /* 2 LONG's numerator : denominator

#define INFINITY  99999

/*
** Data Types
*/

#define TYPEBYTE     1      
#define TYPEASCII    2     
#define TYPESHORT    3    
#define TYPELONG     4   
#define TYPERATIONAL 5  

struct TiffHeader 
	   {
	   BYTE            ByteOrder[2];
	   SHORT           Version; 
	   LONG            IdfOffset;
	   };

struct IDF
	   {
	   SHORT    NumEntries;
	   struct   IDF_Entry *idfptr;	
	   };

struct IDF_Entry 
	   {
	   SHORT Tag;
	   SHORT Type;
	   LONG  Length;
	   LONG  ValueOffset;
	   };

struct  Tiffstruct 
     {
	 int SubFileType;
	 int ImageWidth;
	 int ImageLength;
	 int BitsPerSample;
	 int Compression;
	 int PhotoInterp;
	 int Threshold;
	 int CellWidth;
	 int CellLength;
	 int FillOrder;
	 char *DocName;
	 char *ImageDescript;
	 char *Make;
	 char *Model;
	 int StOffsetCnt;
	 LONG *StripOffset;
	 int Orientation;
	 int SamplesPixel;
	 int RowsStrip;
	 int StripByteCnt;
	 LONG *SBytesCntOffset;
	 int MinSampleValue;
	 int MaxSampleValue;
	 int Xres;
	 int Yres;
	 int PlanarConfig;
	 char *PageName;
	 int XPos; 
	 int YPos;
	 int FreeOffsets;
	 int FreeByteCount;
	 int GrayResUnit;
	 int GrayResCurve;
	 int Group3Option;
	 int Group4Option;
	 int ResolutionUnit;
	 int PageNumber;
	 int ColorResUnit;
	 int ColorResCurv;
	 };
