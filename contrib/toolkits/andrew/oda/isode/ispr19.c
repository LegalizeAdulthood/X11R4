
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/ispr19.c,v 1.1 89/09/21 05:27:23 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/ispr19.c,v $

 *
 * (C) Copyright 1989 by Carnegie Mellon University
 *
 * Permission to use, copy, modify, and distribute these programs
 * and their documentation for any purpose and without fee is
 * hereby granted, provided that this copyright and permission
 * notice appear on all copies and supporting documentation, and
 * that the name of Carnegie Mellon University not be used in
 * advertising or publicity pertaining to distribution of the
 * programs without specific prior permission and notice be given
 * in supporting documentation that copying and distribution is
 * by permission of Carnegie Mellon University.
 *
 * Carnegie Mellon University makes no representations about the
 * suitability of this software for any purpose.  It is provided
 * as is, without express or implied warranty.
 *
 * Software by Ann Marks and James T. Lui,
 * Information Technology Center, Carnegie Mellon University,
 * except where noted.
 *

$Log:	ispr19.c,v $
 * Revision 1.1  89/09/21  05:27:23  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:30:53  mss
 * Initial revision
 * 
 * Revision 1.5  89/09/12  09:34:48  mss
 * Removed #
 * 
 * Revision 1.4  89/08/28  16:54:51  jl3p
 * added copyright notice
 * 
 * Revision 1.3  89/08/15  17:19:31  mss
 * Added ispr include files
 * 
 * Revision 1.2  89/08/01  08:42:23  mss
 * Provided further renaming of modules (from ODIF to ISPR) to prevent future comflicts
 * 
 * Revision 1.1  89/07/25  09:43:07  mss
 * Initial revision
 * 
 * Revision 1.1  89/07/11  15:29:25  annm
 * Initial revision
 * 
*/

#ifdef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/ispr19.c,v $";
#endif

/* toolkit incluces */
#define TK_IMPL_ISPR
#define TK_GVARS_ISPR19
#define TK_IMPL_INIT
#define TK_TKI_TKERR

#include <values.h>
#include <apityp.h>
#include <init.h>
#include <ispr.h>
#include <tkerr.h>

#undef TK_IMPL_ISPR
#undef TK_GVARS_ISPR19
#undef TK_IMPL_INIT
#undef TK_TKI_TKERR

/* UNIX includes */
#include <stdio.h>

/* isode includes */
#include <psap.h>



INT_type isode_PrintODIF(InFile) 
    IN FILE		*InFile;
{

    INT_type	rval;	/* The returned value */

    INT_type		dummy;

    INT_type		SlaveRval;

    PS			ps;

    PE			pe;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    rval = SUCCESS_INT;


    ps = ps_alloc(std_open);
    if( ps == NULLPS ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[isode_ReadODIF]: can not allocate the ps stream" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    SlaveRval = std_setup( ps, InFile );
    if( SlaveRval != OK ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[isode_PrintODIF]: can not perform the ps standard setup" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    while( (pe = ps2pe(ps) ) != NULLPE ){
    
	SlaveRval = print_ISPR1_Interchange__Data__Element(pe, 1, NULLIP,
			NULLVP, NULLCP );
	if( SlaveRval == NOTOK ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[isode_PrintODIF]: can not parse a pe" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	pe_free( pe );
    
    }

    ps_free( ps );

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}
