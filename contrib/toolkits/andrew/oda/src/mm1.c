
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/mm1.c,v 1.1 89/09/21 06:34:33 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/mm1.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	mm1.c,v $
 * Revision 1.1  89/09/21  06:34:33  mss
 * Initial revision
 * 
 * Revision 1.7  89/08/31  18:21:01  mss
 * Fixed to use new LOCAL interface
 * 
 * Revision 1.6  88/09/30  15:41:21  annm
 * fixing VMS bugs
 * 
 * Revision 1.5  88/09/06  13:12:45  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.4  88/09/06  12:02:44  annm
 * no change
 * 
 * Revision 1.3  88/06/08  16:57:40  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.2  88/06/08  14:36:48  annm
 * added Copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/mm1.c,v $";
#endif

/* Put .c template comment here */

#define TK_IMPL_MM
#define TK_GVARS_MM
#define TK_TKI_TKERR

#include <mm.h>
#include <tkerr.h>

#undef TK_IMPL_MM
#undef TK_GVARS_MM
#undef TK_TKI_TKERR

/* put #define's and declarations local to this file here */

INT_type MM_Init()
{
    INT_type	rval;	    /* the returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type MM_Final()
{
    INT_type	rval;	    /* the returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

POINTER_type MM_Malloc(nbytes)
    IN INT_type nbytes;
{
    REGISTER POINTER_type rval;
    INT_type	dummy;	    /* For error messages */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = LOCAL_Malloc(nbytes);
    if (rval == LCL_ERROR_POINTER) {
	/* map from local to tk error */
	rval = ERROR_POINTER; 
	TKError = LCLError;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[MM_Malloc]: error returned ");
	dummy = TKERR_FormatInt("from LOCAL_Malloc for %d bytes",
				(INT_type) nbytes);
	dummy = TKERR_EndErrMsg();
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

POINTER_type MM_Realloc(old, newsize)
    IN POINTER_type old;
    IN INT_type	    newsize;
{
    REGISTER POINTER_type rval;
    INT_type	dummy;	    /* For error messages */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = LOCAL_Realloc(old, newsize);
    if (rval == LCL_ERROR_POINTER) {
	/* map from local to tk error */
	rval = ERROR_POINTER; 
	TKError = LCLError;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[MM_Realloc]: error returned ");
	dummy = TKERR_FormatInt("from LOCAL_Realloc for %d bytes",
				(INT_type) newsize);
	dummy = TKERR_EndErrMsg();
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type MM_Free(block)
    IN POINTER_type block;
{
    REGISTER INT_type rval;
    INT_type	dummy;	    /* For error messages */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = LOCAL_Free(block);
    if (rval == LCL_ERROR_INT) {
	/* map from local to tk error */
	rval = ERROR_INT; 
	TKError = LCLError;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[MM_Free]: error returned ");
	dummy = TKERR_Format("from LOCAL_Free");
	dummy = TKERR_EndErrMsg();
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}
