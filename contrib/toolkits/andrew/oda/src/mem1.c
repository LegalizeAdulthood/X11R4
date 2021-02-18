
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/mem1.c,v 1.3 89/10/16 15:36:33 annm Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/mem1.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	mem1.c,v $
 * Revision 1.3  89/10/16  15:36:33  annm
 * no change
 * 
 * Revision 1.2  89/09/29  11:40:45  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:34:04  mss
 * Initial revision
 * 
 * Revision 1.5  89/09/12  09:14:13  mss
 * Removed #
 * 
 * Revision 1.4  88/09/30  17:26:37  annm
 * looking for what turned out to be a very dumb bug (missing ;)
 * 
 * Revision 1.3  88/09/06  13:12:40  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.2  88/09/06  12:02:39  annm
 * no change
 * 
 * Revision 1.1  88/06/08  19:45:26  mss
 * Initial revision
 * 
 * Revision 1.12  88/06/08  16:57:22  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.11  88/06/08  14:36:29  annm
 * added Copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/mem1.c,v $";
#endif

/* Put .c template comment here */



#define TK_IMPL_MEM
#define TK_GVARS_MEM1
#define TK_IMPL_CONST

#include <lcsite.h>
#include <acd.h>
#include <mem.h>

#undef TK_IMPL_MEM
#undef TK_GVARS_MEM1
#undef TK_IMPL_CONST


/* put #define's and declarations local to this file here */



INT_type HoldAttr(constituent, attribute)
    IN CONSTITUENT	constituent;
    IN INT_type		attribute;
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



INT_type HoldConstituent(constituent)
    IN CONSTITUENT	constituent;
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



INT_type HoldString(string)
    IN STRING_type	string;
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



INT_type ReleaseAttr(attribute)
    IN INT_type	attribute;
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



INT_type ReleaseConstituent(constituent)
    IN CONSTITUENT  constituent;
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



INT_type ReleaseString(string)
    IN STRING_type  string;
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




INT_type MEM_Final()
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



INT_type MEM_Init()
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
