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
 *  File:   ioetc.h
 *  Author: Andrew Werth
 *
 *  Header info for raster reader of unknown formats.
 *
 */

#ifndef TK_INCLUDED_ioetc
#define	TK_INCLUDED_ioetc

#ifdef TK_TKI_etc
#define TK_etc
#else
#ifdef TK_IMPL_etc
#define	TK_etc
#else
#define AP_etc
#endif
#endif


/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must
	  also call with -DTK_IMPL_etc.

*/



/* 
    EVERYONE SEES THIS
*/



/* 
    INSIDE THE TOOLKIT SEES THIS, ONLY!
    (TK_TKI or TK_IMPL/TK_GVARS)

*/

#ifdef  TK_etc

RASTER_type *ETC_ReadRaster();
INT_type    ETC_GetNumber();

#endif

/*
    ONLY MODULE IMPLEMENTORS SEE THIS!
    (TK_IMPL_)

*/

#ifdef TK_IMPL_etc

#define MAX_INT_LENGTH (16)

#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/



#endif

/*  End of file   */
