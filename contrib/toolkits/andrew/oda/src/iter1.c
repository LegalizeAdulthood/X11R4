
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter1.c,v 1.1 89/09/21 06:31:53 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter1.c,v $

		(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	iter1.c,v $
 * Revision 1.1  89/09/21  06:31:53  mss
 * Initial revision
 * 
 * Revision 1.24  89/04/24  14:28:52  mss
 * Moved style iterator into iter4.[ch]
 * 
 * Revision 1.23  89/01/09  13:52:04  jr
 * Align with new API.
 * 
 * Revision 1.22  88/12/23  16:51:26  mss
 * Cleaned up an error msg
 * (added checking for 0 document)
 * 
 * Revision 1.21  88/12/15  10:43:05  mss
 * Removed explicit use of DONT_FORMAT_ERROR
 * flag surrounding error code.
 * 
 * Revision 1.20  88/10/27  15:54:04  mss
 * Implemented FindDocumentProfile
 * 
 * Revision 1.19  88/09/17  16:12:14  mss
 * Got rid of most lines > 71 characters
 * 
 * Revision 1.18  88/09/08  08:19:05  mss
 * Added include of tkerr.h
 * 
 * Revision 1.17  88/09/07  12:11:39  mss
 * Added error messages
 * 
 * Revision 1.16  88/09/06  13:12:08  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.15  88/09/06  12:02:25  annm
 * no change
 * 
 * Revision 1.14  88/08/25  15:11:04  mss
 * Removed make, delete and nextconstituent procs
 * 
 * Revision 1.13  88/07/26  14:11:53  mss
 * Implemented FindDocumentRoot
 * 
 * Revision 1.12  88/06/08  16:57:10  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.11  88/06/08  14:36:18  annm
 * added Copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/iter1.c,v $";
#endif

#define TK_IMPL_ITER
#define TK_GVARS_ITER1
#define TK_IMPL_DOC
#define TK_IMPL_CONST
#define TK_TKI_TKERR

#include <iter.h>
#include <tkerr.h>

#undef TK_IMPL_ITER
#undef TK_GVARS_ITER1
#undef TK_IMPL_DOC
#undef TK_IMPL_CONST
#undef TK_TKI_TKERR

CONSTITUENT FindDocumentProfile(document)
    IN DOCUMENT_type	document;
{
    CONSTITUENT	rval;	    /* the returned value */
    INTERNAL_DOCUMENT_type TheDocument;
    INT_type	dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Does the document parameter look OK? */
    if ( ((DOCUMENT_type) ERROR_CONSTITUENT == document) ||
	 ((DOCUMENT_type) NULL_CONSTITUENT == document) ||
	 ((DOCUMENT_type) 0 == document) ||
	 (ERROR_DOCUMENT == document)) {
	
	rval = ERROR_CONSTITUENT;
	dummy = TKERR_StartErrMsg();
	dummy =
	  TKERR_Format("[FindDocumentProfile]: illegal parameter value");
	dummy =
	  TKERR_Format(" ERROR_DOCUMENT or ERROR_CONSTITUENT.");
	dummy = TKERR_EndErrMsg();
	TKError = BADPARM_err;
	goto LEAVE;
    }

    TheDocument = (INTERNAL_DOCUMENT_type) document;
    if(NULL_CONSTITUENT == TheDocument->document_profile) {
	rval = ERROR_CONSTITUENT;
	TKError = NOPROF_err;
	dummy = TKERR_StartErrMsg();
	dummy =
	  TKERR_Format("[FindDocumentProfile]: No document profile in document.");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    rval = TheDocument->document_profile;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

/* Test to see if a root is legal or still undefined */
static CONSTITUENT DOC_CheckRoot(testRoot)
    IN CONSTITUENT	testRoot;	/* Root to check for NULL */
{
    CONSTITUENT rval;
    INT_type	dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    if(testRoot == NULL_CONSTITUENT) {
	rval = ERROR_CONSTITUENT;
	TKError = NOROOT_err;
	TKERR_StartErrMsg();
	dummy = TKERR_Format("[DOC_CheckRoot]: No such root exists");
	TKERR_EndErrMsg();

    }
    else {
	rval = testRoot;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

CONSTITUENT FindDocumentRoot(document, kind)
    IN DOCUMENT_type	document;
    IN VALUE_type	kind;
{
    CONSTITUENT	rval;	    /* the returned value */
    INTERNAL_DOCUMENT_type TheDocument;
    INT_type	dummy;
 
#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Does the document parameter look OK? */
    if (((DOCUMENT_type) 0 == document) || (ERROR_DOCUMENT == document)) {
	rval = ERROR_CONSTITUENT;
	TKError = BADPARM_err;
	TKERR_StartErrMsg();
	dummy = TKERR_Format(
	     "[FindDocumentRoot]: NULL or ERROR document parameter passed");
	TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheDocument = (INTERNAL_DOCUMENT_type) document;

    /* Check to see if something reasonable came in for root choices */
    switch (kind) {
	case SPECIFIC_DOC_LOGICAL_ROOT:
	    rval = DOC_CheckRoot(TheDocument->specific_logical_root);
	    break;

	case GENERIC_DOC_LOGICAL_ROOT:
	    rval = DOC_CheckRoot(TheDocument->generic_logical_root);
	    break;

	case SPECIFIC_DOC_LAYOUT_ROOT:
	    rval = DOC_CheckRoot(TheDocument->specific_layout_root);
	    break;

	case GENERIC_DOC_LAYOUT_ROOT:
	    rval = DOC_CheckRoot(TheDocument->generic_layout_root);
	    break;

	default:
	    /* Bogus values for root selectors, return error */
	    rval = ERROR_CONSTITUENT;
	    TKError = BADPARM_err;
	    TKERR_StartErrMsg();
	    dummy =
		TKERR_FormatInt("[FindDocumentRoot]: Unknown kind: kind=>%d",
				(INT_type) kind);
	    TKERR_EndErrMsg();
    }

LEAVE:

#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}




INT_type ITER_Final()
{

    INT_type	rval;	    /* the returned value */
    INT_type	dummy;

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;
    TKERR_StartErrMsg();
    dummy = TKERR_Format("[ITER_Final]: Unimplemented procedure");
    TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type ITER_Init()
{

    INT_type	rval;	    /* the returned value */
    INT_type	dummy;

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;
    TKERR_StartErrMsg();
    dummy = TKERR_Format("[ITER_Init]: Unimplemented procedure");
    TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}
