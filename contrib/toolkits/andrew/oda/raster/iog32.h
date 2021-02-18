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
 *  File:   iog32.h
 *  Author: Andrew Werth
 *
 *  Header info for Two Dimensional Group 3 Raster I/O Routines.
 *
 */

#ifndef TK_INCLUDED_iog32
#define TK_INCLUDED_iog32

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

INT_type	    G3_WriteBitmap2();
INT_type	    G4_WriteBitmap();

/* 
    INSIDE THE TOOLKIT SEES THIS, ONLY!
    (TK_TKI or TK_IMPL/TK_GVARS)

*/

#ifdef  TK_g3

#endif

/*
    ONLY MODULE IMPLEMENTORS SEE THIS!
    (TK_IMPL_)

*/

#ifdef TK_IMPL_g3

static INT_type	    EncodeTwo();
static INT_type	    DetectChange();
static INT_type	    DetectA1();
static INT_type	    DetectA2();
static INT_type	    DetectB1();
static INT_type	    DetectB2();
static INT_type	    PassMode();
static INT_type	    VerticalMode();
static INT_type	    HorizontalMode();

#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/



#endif

/*  End of file   */
