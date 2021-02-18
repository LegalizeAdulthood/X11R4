
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/expr1.c,v 1.1 89/09/21 06:29:34 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/expr1.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	expr1.c,v $
 * Revision 1.1  89/09/21  06:29:34  mss
 * Initial revision
 * 
 * Revision 1.18  89/09/12  09:13:55  mss
 * Removed #
 * 
 * Revision 1.17  88/09/06  13:11:47  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.16  88/09/06  12:02:14  annm
 * no change
 * 
 * Revision 1.15  88/07/25  14:03:35  annm
 * fixed typo in undef of the TK_GVARS_EXPR1 switch
 * 
 * Revision 1.14  88/06/08  16:56:35  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.13  88/06/08  14:35:39  annm
 * added Copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/expr1.c,v $";
#endif

/* Put .c template comment here */



#define TK_IMPL_EXPR
#define TK_GVARS_EXPR1
#define TK_TKI_STR
#define TK_IMPL_CONST

#include <acd.h>
#include <str.h>
#include <expr.h>

#undef TK_IMPL_EXPR
#undef TK_GVARS_EXPR1
#undef TK_TKI_STR
#undef TK_IMPL_CONST


#include <apirt.h>
#include <errors.h>



/* put #define's and declarations local to this file here */



INT_type EvalNumExpr(expr, value)
    IN NUMERIC_EXPR_type    *expr;
    OUT INT_type	    *value;
{

    INT_type	rval;	    /* the returned value */

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



CONSTITUENT EvalObjIdExpr(expr)
    IN OBJECT_ID_EXPR_type    *expr;
{

    CONSTITUENT	rval;	    /* the returned value */

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = ERROR_CONSTITUENT;
    TKError = UNIMPL_err;


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type EvalRefBind(refbind, string, integer)
    IN REF_BINDING_VALUE_type	*refbind;
    OUT STRING_type		*string;
    OUT INT_type		*integer;
{

    INT_type	rval;	    /* the returned value */

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



STRING_type EvalStringExpr(expr)
    IN STRING_EXPR_type	*expr;
{

    STRING_type	rval;	    /* the returned value */

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = ERROR_STRING;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}




INT_type EXPR_Final()
{

    INT_type	rval;	    /* the returned value */

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type EXPR_Init()
{

    INT_type	rval;	    /* the returned value */

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}
