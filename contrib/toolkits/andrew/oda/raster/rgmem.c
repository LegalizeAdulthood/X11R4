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
 *  File:   rgmem.c
 *  Author: Andrew Werth
 *
 *  Handle memory allocation requests, calling appropriate swap
 *  routines to fit the requested blocks into memory.
 *
 */


#define TK_TKI_rast
#define TK_TKI_init
#define TK_TKI_mem
#define TK_IMPL_mem
#define TK_GVARS_rgmem

#include <rgsite.h>
#include <rginit.h>
#include <rgerr.h>
#include <raster.h>
#include <rgmem.h>

#undef TK_GVARS_rgmem
#undef TK_IMPL_mem
#undef TK_TKI_mem
#undef TK_TKI_init
#undef TK_TKI_rast

extern INIT_RGPARMS_type    *INIT_RGParms;

INT_type	    MEM_Alloced = 0;


/*\
 *  MEM_MALLOC:  Attempts to allocate a block of memory.  If the
 *		 allocation fails, then the routine will swap out
 *		 enough memory from other rasters until the new
 *		 block may be allocated.  If there is just not
 *		 enough room, then an error occurs.  All memory
 *		 allocated by MEM_Malloc should be freed by
 *		 MEM_Free so that the Tool Kit knows how much
 *		 memory is allocated.
\*/


POINTER_type MEM_Malloc(size)
    INT_type	    size;
{
    POINTER_type    rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    rval = LOCAL_Malloc(size);	/* Try normal allocation	*/
    if (rval != LCL_ERROR_POINTER) {/* If succeeds, skip the rest	*/
	goto LEAVE;
    }

    /* Normal allocation failed, so start swapping */

    if (INIT_RGParms->memsize > 0 &&
	size > INIT_RGParms->memsize - MEM_Alloced) {
	rval = RG_ERROR_POINTER;
	RGError = RG_NOMEM_err;
	goto LEAVE;
    }

    rval = RAST_SwapAlloc(size);
    if (rval != RG_ERROR_POINTER) {
	MEM_Alloced += size;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}

/*\
 *  MEM_REALLOC:  Reallocate a block of memory to a new size.
\*/

POINTER_type MEM_Realloc(old, newsize)
    POINTER_type    old;
    INT_type	    newsize;
{
    POINTER_type    rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    rval = LOCAL_Realloc(old, newsize);
    if (rval == LCL_ERROR_POINTER) {
	RGError = LCLError;
	rval = RG_ERROR_POINTER;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}



INT_type MEM_Free(memory)
    POINTER_type    memory;
{
    INT_type	    rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering("");
    }
#endif

    if ((rval = LOCAL_Free(memory)) == LCL_ERROR_INT) {
	RGError = LCLError;
	rval = RG_ERROR_INT;
	goto LEAVE;
    }
    MEM_Alloced -= 0;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving("");
    }
#endif

    return(rval);
}




/* End of file malloc.c */
