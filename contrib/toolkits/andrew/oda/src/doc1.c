
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/doc1.c,v 1.2 89/10/31 14:44:47 annm Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/doc1.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	doc1.c,v $
 * Revision 1.2  89/10/31  14:44:47  annm
 * fixing lint problems
 * 
 * Revision 1.1  89/09/21  06:29:01  mss
 * Initial revision
 * 
 * Revision 1.30  89/09/12  09:13:39  mss
 * Removed #
 * 
 * Revision 1.29  89/04/24  14:24:45  mss
 * Made list heads more consistent (filled in "document" fields
 * of dummy structures).
 * 
 * Revision 1.28  89/04/17  14:48:43  annm
 * implemented DeleteDocument
 * 
 * Revision 1.27  88/09/30  15:41:10  annm
 * fixing VMS bugs
 * 
 * Revision 1.26  88/09/12  15:56:50  annm
 * fixed bugs in tkerr calls and added dependency on <tkerr.h>
 * 
 * Revision 1.25  88/09/12  12:57:48  annm
 * added TKError message stuff
 * 
 * Revision 1.24  88/09/06  13:11:29  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.23  88/09/06  12:02:09  annm
 * no change
 * 
 * Revision 1.22  88/06/13  13:55:14  annm
 * If malloc fails in MakeDocument code
 * now sets TKError to NOMEM_err
 * 
 * Revision 1.21  88/06/08  16:56:20  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.20  88/06/08  14:35:26  annm
 * added Copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/doc1.c,v $";
#endif

/* Put .c template comment here */

#define TK_IMPL_DOC
#define TK_GVARS_DOC1
#define TK_IMPL_CONST
#define TK_TKI_MM
#define TK_TKI_TKERR

#include <acd.h>
#include <mm.h>
#include <tkerr.h>

#undef TK_IMPL_DOC
#undef TK_GVARS_DOC1
#undef TK_IMPL_CONST
#undef TK_TKI_MM
#undef TK_TKI_TKERR


/* put #define's and declarations local to this file here */

static INT_type next_document_id;   /* Next id to use for document */



INT_type DeleteDocument(document, delete)
    IN DOCUMENT_type	document;
    IN BOOL_type	delete;
{

    /*
	Strategy:
	    1) check the document to make sure that it is not
		ERROR_DOCUMENT or (DOCUMENT_type) 0
	    2) check the delete flag to ensure that it
		is BOOL_true or BOOL_false
	    3) if delete is BOOL_true (=> constituents are to
		be deleted) then call service routine to delete them
	    4) the document is removed  from the document list
	    5) free the document structure
    */

    INT_type	rval;	    /* the returned value */

    INT_type dummy;

    INTERNAL_DOCUMENT_type TheDocument;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    /* check the document */
    if( document == ERROR_DOCUMENT || document == (DOCUMENT_type) 0 ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[DeleteDocument]: the document arguement is either ERROR_DOCUMENT or (DOCUMENT_type) 0" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* check the delete arguement */
    if( delete != BOOL_true && delete != BOOL_false ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[DeleteDocument]: the delete arguement is neither BOOL_true or BOOL_false" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* delete the constituents if needed */

    TheDocument = (INTERNAL_DOCUMENT_type) document;
    if( delete == BOOL_true ){
	dummy = DOC_ZapConstituents( TheDocument );
    }

    /* remove the document from the document list */

    TheDocument->forward->back = TheDocument->back;
	/* the forward document points back at my back neighbor */

    TheDocument->back->forward = TheDocument->forward;
	/* the back document points forward at my forward neighbor */

    dummy = MM_Free( (POINTER_type) document );

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type DOC_ZapConstituents( TheDocument )
    IN INTERNAL_DOCUMENT_type TheDocument;
{

    /*
	Strategy:
	    1) DeleteConstituent the 'special' constituents 
		if they are present in the document
	    2) walk the constituent_list delete-ing
		merrily along
	    3) walk the layout styles list deleting as
		we go
	    4) walk the presentation styles list deleting
		as we go
    */

    INT_type rval;

    INTERNAL_CONSTITUENT SomeConstituent;

    INT_type SlaveRval;

    INTERNAL_CONSTITUENT TempConstituent;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    /* first delete the 'special' constituents if they exist */

    if( TheDocument->document_profile != NULL_CONSTITUENT ){
	SlaveRval = DeleteConstituent( TheDocument->document_profile );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[DOC_ZapConstituents]: the DeleteConstituent for the document profile failed - aborting delete" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    if( TheDocument->specific_logical_root != NULL_CONSTITUENT ){
	SlaveRval = DeleteConstituent( TheDocument->specific_logical_root );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[DOC_ZapConstituents]: the DeleteConstituent for the specific logical root failed - aborting delete" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    if( TheDocument->specific_layout_root != NULL_CONSTITUENT ){
	SlaveRval = DeleteConstituent( TheDocument->specific_layout_root );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[DOC_ZapConstituents]: the DeleteConstituent for the specific layout root failed - aborting delete" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    if( TheDocument->generic_logical_root != NULL_CONSTITUENT ){
	SlaveRval = DeleteConstituent( TheDocument->generic_logical_root );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[DOC_ZapConstituents]: the DeleteConstituent for the generic logical root failed - aborting delete" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    if( TheDocument->generic_layout_root != NULL_CONSTITUENT ){
	SlaveRval = DeleteConstituent( TheDocument->generic_layout_root );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[DOC_ZapConstituents]: the DeleteConstituent for the generic layout root failed - aborting delete" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    /* delete the constituents */
    for( SomeConstituent = TheDocument->constituent_list.forward;
	 SomeConstituent != &TheDocument->constituent_list;
	  ){

	TempConstituent = SomeConstituent->forward;

	SlaveRval = DeleteConstituent( (CONSTITUENT) SomeConstituent );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[DOC_ZapConstituents]: the above error occurred while attempting to delete a constituent - delete of constituents aborted" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	SomeConstituent = TempConstituent;
    }

    /* delete the layout styles */
    for( SomeConstituent = TheDocument->layout_styles_list.forward;
	 SomeConstituent != &TheDocument->layout_styles_list;
	  ){

	TempConstituent = SomeConstituent->forward;

	SlaveRval = DeleteConstituent( (CONSTITUENT) SomeConstituent );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[DOC_ZapConstituents]: the above error occurred while attempting to delete a layout style - delete of constituents aborted" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	SomeConstituent = TempConstituent;
    }

    /* delete the presentation styles */
    for( SomeConstituent = TheDocument->presentation_styles_list.forward;
	 SomeConstituent != &TheDocument->presentation_styles_list;
	  ){

	TempConstituent = SomeConstituent->forward;

	SlaveRval = DeleteConstituent( (CONSTITUENT) SomeConstituent );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[DOC_ZapConstituents]: the above error occurred while attempting to delete a presentation style - delete of constituents aborted" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	SomeConstituent = TempConstituent;
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );


}



DOCUMENT_type MakeDocument()
{

    /*
	Outline of code:
	    1) allocate a document structure
		(if this fails an error return is made)
	    2) initialize the structure as empty
		(this can not fail)
	    2a) generate an id for the document & initialize
		constituent id counter
	    3) link this document structure into the
		document list (this can not fail.)
    */


    DOCUMENT_type	rval;	    /* the returned value */
    POINTER_type	p;	    /* Temporary memory pointer */
    INTERNAL_DOCUMENT_type TheDocument;

    INT_type		dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /*
	Step 1: Allocate a document structure.
    */
    p = MM_Malloc( (INT_type) sizeof( struct DOCUMENT_struct ) );
    if( p == ERROR_POINTER ){
	rval = ERROR_DOCUMENT;
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( 
	    "[MakeDocument]: Can't get memory for the document" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    rval = (DOCUMENT_type) p;
    TheDocument = (INTERNAL_DOCUMENT_type) p;

    /* 
	Step 2: make the document look empty.
    */
    TheDocument->document_profile = NULL_CONSTITUENT;
    
    TheDocument->specific_logical_root = NULL_CONSTITUENT;    
    TheDocument->specific_layout_root = NULL_CONSTITUENT;    
    TheDocument->generic_logical_root = NULL_CONSTITUENT;    
    TheDocument->generic_layout_root = NULL_CONSTITUENT;    

    /*
	constituent and styles lists to look empty -
	doubly linked so point at the document
	struct.
    */

    TheDocument->layout_styles_list.forward = &(TheDocument->layout_styles_list);
    TheDocument->layout_styles_list.back = &(TheDocument->layout_styles_list);
    TheDocument->layout_styles_list.document = TheDocument;
    TheDocument->layout_styles_list.what_kind = LOUT_STYLE;

    TheDocument->presentation_styles_list.forward = &(TheDocument->presentation_styles_list);
    TheDocument->presentation_styles_list.back = &(TheDocument->presentation_styles_list);
    TheDocument->presentation_styles_list.document = TheDocument;
    TheDocument->presentation_styles_list.what_kind = PRESENT_STYLE;

    TheDocument->constituent_list.forward = &(TheDocument->constituent_list);
    TheDocument->constituent_list.back = &(TheDocument->constituent_list);
    TheDocument->constituent_list.document = TheDocument;

    /*
	Step 2a: Generate document id & initialize constituent id counter
    */
    TheDocument -> id = next_document_id++;
    TheDocument -> next_constituent_id = 1;

    /*
	Step 3:add this doucment struct to the front of the document
	list. Add to the front of the document list.
    */
    TheDocument->forward = DOC_DocumentList.forward;
	/* new guy points forward at current first guy on list */
    TheDocument->back = &DOC_DocumentList;
	/* new guy points back at doc list head */

    (DOC_DocumentList.forward)->back = TheDocument;
	/* current first guy on list points back at new guy */
    DOC_DocumentList.forward = TheDocument;
	/* doc list head points forward at new guy */

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type DOC_Final()
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

    dummy = TKERR_StartErrMsg();
    dummy = TKERR_Format( "[DOC_Final]: Not Implemented" );
    dummy = TKERR_EndErrMsg();


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type DOC_Init()
{

    INT_type	rval;	    /* the returned value */

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* assume that all works */
    rval = SUCCESS_INT;

    /* Initialize document id counter */
    next_document_id = 1;

    /* set up an empty document list */
    DOC_DocumentList.forward = &DOC_DocumentList;
    DOC_DocumentList.back = &DOC_DocumentList;

    /* set all other fields to something (hopefully) benign */
    DOC_DocumentList.document_profile = NULL_CONSTITUENT;
    DOC_DocumentList.specific_logical_root = NULL_CONSTITUENT;
    DOC_DocumentList.specific_layout_root = NULL_CONSTITUENT;
    DOC_DocumentList.generic_logical_root = NULL_CONSTITUENT;
    DOC_DocumentList.generic_layout_root  = NULL_CONSTITUENT;
    
    /*
	 initialize style list and constituent list as empty
	circular doubly linked lists.
    */
    DOC_DocumentList.layout_styles_list.forward = 
	&(DOC_DocumentList.layout_styles_list);
    DOC_DocumentList.layout_styles_list.back = 
	&(DOC_DocumentList.layout_styles_list);
    DOC_DocumentList.presentation_styles_list.forward = 
	&(DOC_DocumentList.presentation_styles_list);
    DOC_DocumentList.presentation_styles_list.back = 
	&(DOC_DocumentList.presentation_styles_list);

    DOC_DocumentList.constituent_list.forward = 
	&(DOC_DocumentList.constituent_list);
    DOC_DocumentList.constituent_list.back =
 	&(DOC_DocumentList.constituent_list);

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}
