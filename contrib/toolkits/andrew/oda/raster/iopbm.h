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
 *  File:   iopbm.h
 *  Author: Andrew Werth
 *
 *  Header info for reading PBM formats.
 *
 */


#ifndef TK_INCLUDED_iopbm
#define	TK_INCLUDED_iopbm

#ifdef TK_TKI_pbm
#define TK_pbm
#else
#ifdef TK_IMPL_pbm
#define	TK_pbm
#else
#define AP_pbm
#endif
#endif


/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must
	  also call with -DTK_IMPL_<modulename>.

*/



/* 
    EVERYONE SEES THIS
*/

RASTER_type	    *PBM_ReadRaster();
INT_type	    PBM_WriteRaster();


/* 
    INSIDE THE TOOLKIT SEES THIS, ONLY!
    (TK_TKI or TK_IMPL/TK_GVARS)

*/

#ifdef  TK_pbm

#endif

/*
    ONLY MODULE IMPLEMENTORS SEE THIS!
    (TK_IMPL_)

*/

#ifdef TK_IMPL_pbm

static INT_type	    ReadMagic();
static INT_type	    ReadSize();
static RASTER_type  *ReadBitmap();

static INT_type	    WriteBitmap();

#define PBM_MAGIC1	('P')
#define PBM_MAGIC2	('1')

#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/



#endif
