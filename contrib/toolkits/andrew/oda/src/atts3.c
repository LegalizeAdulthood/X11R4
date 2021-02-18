
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts3.c,v 1.3 89/11/01 15:48:01 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts3.c,v $

$Log:	atts3.c,v $
 * Revision 1.3  89/11/01  15:48:01  mss
 * Changed some casting and declarations for passing lint
 * 
 * Revision 1.2  89/10/31  14:17:59  annm
 * fixing lint problmes
 * 
 * Revision 1.1  89/09/21  06:25:52  mss
 * Initial revision
 * 
 * Revision 1.5  89/09/12  09:12:22  mss
 * Removed #
 * 
 * Revision 1.4  89/08/26  13:38:19  mss
 * set switches, include files for proper nanme mapping
 * 
 * Revision 1.3  89/08/26  10:28:38  mss
 * Changed use of LOCAL in MM
 * 
 * Revision 1.2  89/07/19  22:15:48  mss
 * got rid of some debugging materials accidently left in
 * 
 * Revision 1.1  89/07/19  21:04:00  mss
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/atts3.c,v $";
#endif



#define TK_IMPL_ATTS
#define TK_GVARS_ATTS3
#define TK_TKI_TKERR
#define TK_TKI_MM

#include <apirt.h>
#include <atts.h>
#include <apityp.h>
#include <tags.h>
#include <mm.h>
#include <seq.h>
#include <tkerr.h>

#undef TK_IMPL_ATTS
#undef TK_GVARS_ATTS3
#undef TK_TKI_TKERR

/*

    This file contains some utility delete procedures for attribute structures

*/

/* Forward declarations */
static INT_type DeleteConstTerm();



INT_type DeleteConstExpr(construction_expression)
IN OUT CONSTRUCTION_EXPR_type * construction_expression;
{

    INT_type rval;
    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    if (construction_expression == (CONSTRUCTION_EXPR_type *) 0) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[DeleteConstExpr]: null pointer ");
	dummy = TKERR_Format("passed for construction expression");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    switch(construction_expression->construction_tag) {
	case TERM_tag:
	    rval = DeleteConstTerm(
		&construction_expression->construction_value.term);
	    break;
	case SEQUENCE_tag:
	case AGGREGATE_tag:
	case CHOICE_tag:
	    rval = DeleteSequenceDeeply( (SEQUENCE_type)
		    construction_expression->construction_value.construction);
	    break;
	default:
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[DeleteConstExpr]: unknown construction ");
	    dummy = TKERR_FormatInt("tag %d",
			  (INT_type)  construction_expression->construction_tag );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	   /* break; */
    }

    rval = MM_Free( (POINTER_type) construction_expression);

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}

static INT_type DeleteConstTerm(construction_term)
IN OUT CONSTRUCTION_TERM_type * construction_term;
{

    INT_type rval;
    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    if (construction_term == (CONSTRUCTION_TERM_type *) 0) {
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[DeleteConstTerm]: null pointer ");
	dummy = TKERR_Format("passed for construction term");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    switch(construction_term->factor.factor_tag) {
	case OBJECT_CLASS_ID_tag:
	    break;
	case EXPR_tag:
	    rval = DeleteConstExpr(
		construction_term->factor.factor_value.expr);
	    break;
	default:
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[DeleteConstTerm]: unknown factor ");
	    dummy = TKERR_FormatInt("tag %d",
			  (INT_type)  construction_term->factor.factor_tag );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	    /* break; */
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}
