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
 *  File:   iog4.h
 *  Author: Andrew Werth
 *
 *  Header info for Group 4 input/output routines.
 *
 */

#ifndef TK_INCLUDED_iog4
#define TK_INCLUDED_iog4

#ifdef TK_TKI_g4
#define TK_g4
#else
#ifdef TK_IMPL_g4
#define	TK_g4
#else
#define AP_g4
#endif
#endif


/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must
	  also call with -DTK_IMPL_g4.

*/



/* 
    EVERYONE SEES THIS
*/

typedef struct {
    VALUE_type	msb;	    /* Which bit is MSB in byte   */
    INT_type	pad_size;   /* Optional pad size for FaxB */
    INT_type	width;	    /* Width of raster		  */
} FORMAT_GROUP4_type;

/* 
    INSIDE THE TOOLKIT SEES THIS, ONLY!
    (TK_TKI or TK_IMPL/TK_GVARS)

*/

#ifdef  TK_g4

#define UNKNOWN	    ((VALUE_type) 0)
#define LEFTBIT	    ((VALUE_type) 1)
#define RIGHTBIT    ((VALUE_type) 2)

#define BLACK	    ((BYTE_type) 1)
#define WHITE	    ((BYTE_type) 0)

#define INTERNAL    ((BYTE_type) 0)
#define WHTERM	    ((BYTE_type) 1)
#define MAKE	    ((BYTE_type) 2)
#define EOLN	    ((BYTE_type) 3)
#define BLTERM	    ((BYTE_type) 4)
#define EDFB	    (9)

#define MOD8	    ((INT_type) 7)
#define DIV8	    ((INT_type) 3)
#define MOD64	    ((BYTE_type) 63)
#define DIV64	    ((BYTE_type) 6)

RASTER_type	*G4_ReadRaster();
RUN_t		*G4_NextRun();
DecodeWord_t	*G4_Traverse();
BYTE_type	G4_GetBit();

#endif

/*
    ONLY MODULE IMPLEMENTORS SEE THIS!
    (TK_IMPL_)

*/

#ifdef TK_IMPL_g4

static ROW_t	    *Decode4();
static INT_type	    UndoPassMode();
static INT_type	    UndoHorizMode();
static INT_type	    UndoVertMode();
static INT_type	    PutRun();
static BYTE_type    RowDetectChange();
static INT_type	    RowGetBit();
static INT_type	    RowSetBit();

#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/



#endif


/*  End of file   */
