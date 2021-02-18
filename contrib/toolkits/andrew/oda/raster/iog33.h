/*\
 *
 *  (C) Copyright 1989 by Carnegie Mellon University
 *
 *  Permission to use, copy, modify, and distribute these programs
 *  and their documentation for any purpose and without fee is
 *  hereby granted, provided that this copyright and permission
 *  notice appear on all copies and supporting documentation, and
 *  that the name of Carnegie Mellon University not be used in
 *  advertising or publicity pertaining to distribution of the
 *  programs without specific prior permission and notice be given
 *  in supporting documentation that copying and distribution is
 *  by permission of Carnegie Mellon University.
 *
 *  Carnegie Mellon University makes no representations about the
 *  suitability of this software for any purpose.  It is provided
 *  as is, without express or implied warranty.
 *
 *  Software by Andrew Werth, Information Technology Center,
 *  Carnegie Mellon University, except where noted.
\*/

/*
 *  File:   iog33.h
 *  Author: Andrew Werth
 *
 *  Header info for input routines for Group 3 Two Dimensional
 *  rasters.
 *
 */

#ifndef TK_INCLUDED_iog33
#define TK_INCLUDED_iog33

#ifdef TK_TKI_g3
#define TK_g3
#else
#ifdef TK_IMPL_g3
#define	TK_g3
#else
#define AP_g3
#endif
#endif


/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must
	  also call with -DTK_IMPL_g3.

*/



/* 
    EVERYONE SEES THIS
*/

RASTER_type *G3_ReadBitmap2();


/* 
    INSIDE THE TOOLKIT SEES THIS, ONLY!
    (TK_TKI or TK_IMPL/TK_GVARS)

*/

#ifdef  TK_g3

#define P	0
#define H	1
#define	VL3	2
#define	VL2	3
#define	VL1	4
#define	V0	5
#define VR1	6
#define	VR2	7
#define	VR3	8
#define EOL2	9
#define COMP   10

#endif

/*
    ONLY MODULE IMPLEMENTORS SEE THIS!
    (TK_IMPL_)

*/

#ifdef TK_IMPL_g3

#define ONE_DIM	    ((VALUE_type) 1)
#define TWO_DIM	    ((VALUE_type) 2)

#define DEFAULT_ROW_SIZE    ((INT_type) 2048)
#define DEFAULT_BUFFER	    (DEFAULT_ROW_SIZE / 8)
#define DEFAULT_INCREMENT   512

static ROW_t    *DecodeTwo();
static INT_type	UndoPassMode();
static INT_type	UndoHorizMode();
static INT_type	UndoVertMode();
static INT_type RowGetBit();
static INT_type RowSetBit();
static INT_type HandleCompression();
static INT_type	PutRun();
static BYTE_type RowDetectChange();

#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/



#endif

/*  End of file   */
