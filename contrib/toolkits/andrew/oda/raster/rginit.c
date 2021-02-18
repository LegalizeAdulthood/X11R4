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
 *  File:   init.c
 *  Author: Andrew Werth
 *
 *  RG Tool Kit Initialization/Finalization Routines
 *
 */

#define TK_TKI_rast
#define TK_TKI_init
#define TK_IMPL_init
#define TK_GVARS_init
#define TK_GVARS_rgerr

#include <rgsite.h>
#include <rginit.h>
#include <raster.h>

#undef TK_GVARS_init
#undef TK_IMPL_init
#undef TK_TKI_init
#undef TK_TKI_rast
#undef TK_GVARS_rgerr

#ifndef USE_LONG_NAMES

#define Initialized rgintz

#endif

extern INT_type		    ETC_MagicSize;

static BOOL_type    Initialized = BOOL_false;
INIT_RGPARMS_type   *INIT_RGParms;

/*\
 *  INITRGTOOLKIT:  Initialize the raster graphics tool kit.
 *		    Maintains a pointer to the initialization
 *		    parameters.
\*/

INT_type InitRGToolKit(parms)
    IN INIT_RGPARMS_type	*parms;
{
    INT_type	rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering();
    }
#endif
    
    if (Initialized != BOOL_false) {
	rval = RG_ERROR_INT;
	RGError = RG_NOFINAL_err;
	goto LEAVE;
    }

    INIT_RGParms = parms;

    ETC_MagicSize = 0;	    /* Clear the Magic # buffer */
    

    /* Set up list of rasters in use by toolkit (NULL list) */

    if (RAST_InitRasterList() == RG_ERROR_POINTER) {
	rval = RG_ERROR_INT;
	RGError = RG_NOMEM_err;
	goto LEAVE;
    }

    rval = RG_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving();
    }
#endif

    return(rval);
}


/*\
 *  FINALRGTOOLKIT:  Finalization routine for tool kit; 
 *		     deallocates all memory and "closes"
 *		     all i/o streams.
\*/

INT_type FinalRGToolKit()
{
    INT_type	    rval;

#ifdef DEBUG
    if (TraceRoutine(0)) {
	Entering();
    }
#endif

    if (Initialized == BOOL_false) {
	rval = RG_ERROR_INT;
	RGError = RG_NOFINAL_err;
	goto LEAVE;
    }

#ifdef RGTK_STANDALONE
    /* Close all I/O streams, only if NOT ODA Tool Kit */

#endif

    /* Finalize the local routines, if necessary */

#ifdef RGTK_STANDALONE
    if (LOCAL_Final() == LCL_ERROR_INT) {
	RGError = LCLError;
	rval = RG_ERROR_INT;
	goto LEAVE;
    }
#endif

    /* Free up memory associated with rasters, raster list */

    rval = RAST_FinalRasterList();

LEAVE:
#ifdef DEBUG
    if (TraceRoutine(0)) {
	Leaving();
    }
#endif

    return(rval);
}
