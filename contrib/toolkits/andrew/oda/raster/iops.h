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
 *  File:   iops.h
 *  Author: Andrew Werth
 *
 *  Header info for writing PostScript files to output
 *
 */

#ifndef TK_INCLUDED_iops
#define TK_INCLUDED_iops

#ifdef TK_TKI_ps
#define TK_ps
#else
#ifdef TK_IMPL_ps
#define	TK_ps
#else
#define AP_ps
#endif
#endif


/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must
	  also call with -DTK_IMPL_ps.

*/



/* 
    EVERYONE SEES THIS
*/

typedef struct {
    FLOAT_type	    scale;	    /* Scaling for output */
    CHAR_type	    *name;	    /* Name of raster */    
} FORMAT_POSTSCRIPT_type;

INT_type    PS_WriteRaster();


/* 
    INSIDE THE TOOLKIT SEES THIS, ONLY!
    (TK_TKI or TK_IMPL/TK_GVARS)

*/

#ifdef  TK_ps

#endif

/*
    ONLY MODULE IMPLEMENTORS SEE THIS!
    (TK_IMPL_)

*/

#ifdef TK_IMPL_ps

static INT_type PutInit();
static putrlitem( );
static putrlbuffer( );
static putitem( );
static putbit(  );
static putrest( );
static INT_type Writef();
static INT_type Writen();
static INT_type Writex();

#define MAX_DIGITS  7
#define MAX_NUMBER  9999999

#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/



#endif

/*  End of file   */
