
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/prior1.c,v 1.1 89/09/21 06:35:23 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/prior1.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	prior1.c,v $
 * Revision 1.1  89/09/21  06:35:23  mss
 * Initial revision
 * 
 * Revision 1.12  89/09/12  09:14:29  mss
 * Removed #
 * 
 * Revision 1.11  88/09/06  13:12:58  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.10  88/09/06  12:02:48  annm
 * no change
 * 
 * Revision 1.9  88/06/08  16:57:56  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.8  88/06/08  14:37:03  annm
 * added Copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/prior1.c,v $";
#endif

/* Put .c template comment here */



#define TK_IMPL_PRIOR
#define TK_GVARS_PRIOR1
#include <prior.h>
#undef TK_IMPL_PRIOR
#undef TK_GVARS_PRIOR1


/* put #define's and declarations local to this file here */



INT_type AttrDocProfPriority(set, attribute, priority)
    IN BOOL_type    set;
    IN INT_type	    attribute;
    IN OUT INT_type *priority;
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



INT_type ComponentAttrPriority(set, type, class, attribute, priority)
    IN BOOL_type	    set;
    IN at_OBJECT_TYPE_type  type;
    IN BOOL_type	    class;
    IN INT_type		    attribute;
    IN OUT INT_type	    *priority;
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



INT_type ContentAttrPriority(set, type, attribute, priority)
    IN BOOL_type	set;
    IN INT_type		type;	    
    IN INT_type		attribute;
    IN OUT INT_type     *priority;
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



INT_type StyleAttrPriority(set, type, attribute, priority)
    IN BOOL_type    set;
    IN VALUE_type   type;	
    IN INT_type	    attribute;
    IN OUT INT_type *priority;
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




INT_type PRIOR_Final()
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



INT_type PRIOR_Init()
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


