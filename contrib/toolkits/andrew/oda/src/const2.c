
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/const2.c,v 1.2 89/10/31 14:21:19 annm Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/const2.c,v $

(c) Copyright 1988 Carnegie Mellon University

	 See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
		 CMU-ITC-071) for full text & permitted uses.

$Log:	const2.c,v $
 * Revision 1.2  89/10/31  14:21:19  annm
 * no change
 * 
 * Revision 1.1  89/09/21  06:27:11  mss
 * Initial revision
 * 
 * Revision 1.26  89/04/11  15:32:18  annm
 * minor tweek
 * 
 * Revision 1.25  89/03/22  15:50:57  annm
 * minor tweek to TKERR msg
 * 
 * Revision 1.24  89/03/10  13:00:58  annm
 * changes for the new DAPD stuff
 * 
 * Revision 1.23  89/02/24  18:21:53  annm
 * changes for the new MakeComponent
 * 
 * Revision 1.22  89/01/17  16:57:32  annm
 * changes for DVL defaulting
 * 
 * Revision 1.21  88/09/21  15:48:07  annm
 * added tkerr stuff
 * 
 * Revision 1.20  88/09/06  13:11:01  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.19  88/09/06  12:01:47  annm
 * no change
 * 
 * Revision 1.18  88/08/25  16:12:49  annm
 * added remove from document routines for support
 * for fill in constituent, collaspe content portions
 * and delete constituent
 * 
 * Revision 1.17  88/08/15  18:17:54  annm
 * no change
 * 
 * Revision 1.16  88/08/03  13:58:11  annm
 * no change
 * 
 * Revision 1.15  88/07/29  11:02:32  mss
 * Removed references to comp_basic_page
 * 
 * Revision 1.14  88/07/25  14:02:36  annm
 * removed undef of TS_GVARS_CONST1
 * 
 * Revision 1.13  88/06/15  17:24:58  jr
 * Make calls to TKERR_Init
 * 
 * Revision 1.12  88/06/08  16:55:50  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.11  88/06/08  14:34:56  annm
 * added Copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/const2.c,v $";
#endif

#define TK_IMPL_CONST
#define TK_GVARS_CONST2
#define TK_IMPL_DOC
#define TK_IMPL_ATTR
#define TK_TKI_MM
#define TK_TKI_ATTR
#define TK_TKI_TKERR

#include <acd.h>
#include <mm.h>
#include <tkerr.h>

#undef TK_IMPL_CONST
#undef TK_GVARS_CONST2
#undef TK_IMPL_DOC
#define TK_IMPL_ATTR
#undef TK_TKI_MM
#undef TK_TKI_ATTR
#undef TK_TKI_TKERR

#include <values.h>

/* put #define's and declarations local to this file here */



INT_type CONST_SetComponentWhatKind(constituent, type, kind)
    IN OUT CONSTITUENT	constituent;
    IN VALUE_type	type;
    IN VALUE_type	kind;
{

    INT_type	rval;	    /* the returned value */

    INT_type dummy;

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* 
	This routine together with the CONST_DoSetWhatKind routine, sets 
	    the what kind field of the constituent
	    structure for Components ONLY (for other constituents
	    this is done in-line in the MakeMumble procedure in
	    const1.c).
	This routine switches to call CONST_DoSetWhatKind with the right
	    values. CONST_DoSetWhatKind actually sets the what kind field.
	    then CONST_DoSetWhatKind sets up the error return.

	Note that the third/fourth arguement passed to CONST_DoSetWhatKind is
	    (VALUE_type) 0 for the 'illegal' pairings for kind and type
	    (e.g. type can not indicate a page set if the kind indicates
	    DVL). Because of the consistency check made in MakeComponent
	    the inconsistent pairings will never be seen here - thus,
	    (VALUE_type) 0 is passed as the third 'dummy' arguement in
	    these cases.
    */

    switch( type ){
    case at_OBJECT_TYPE_doc_layout_root:
	rval = CONST_DoSetComponentWhatKind( constituent, kind, 
		DOC_LAY_ROOT_CL, DOC_LAY_ROOT_NC,
		(VALUE_type) 0, (VALUE_type) 0 );
	break;
    case at_OBJECT_TYPE_page_set:
	rval = CONST_DoSetComponentWhatKind( constituent, kind, 
		PAGE_SET_CL, PAGE_SET_NC, 
		(VALUE_type) 0, (VALUE_type) 0 );
	break;
    case at_OBJECT_TYPE_page:
	rval = CONST_DoSetComponentWhatKind( constituent, kind, 
		PAGE_CL, PAGE_NC, DVL_PAGE, (VALUE_type) 0 );
	break;
    case at_OBJECT_TYPE_frame:
	rval = CONST_DoSetComponentWhatKind( constituent, kind, 
		FRAME_CL, FRAME_NC, DVL_FRAME, (VALUE_type) 0 );
	break;
    case at_OBJECT_TYPE_block:
	rval = CONST_DoSetComponentWhatKind( constituent, kind, 
		BLOCK_CL, BLOCK_NC, DVL_BLOCK, DAPD_BLOCK );
	break;
    case at_OBJECT_TYPE_doc_logical_root:
	rval = CONST_DoSetComponentWhatKind( constituent, kind,
		DOC_LOG_ROOT_CL, DOC_LOG_ROOT_NC, 
		(VALUE_type) 0, (VALUE_type) 0 );
	break;
    case at_OBJECT_TYPE_comp_logical_obj:
	rval = CONST_DoSetComponentWhatKind( constituent, kind, 
		COMP_LOG_CL, COMP_LOG_NC, DVL_COMP, (VALUE_type) 0 );
	break;
    case at_OBJECT_TYPE_bas_logical_obj:
	rval = CONST_DoSetComponentWhatKind( constituent, kind, 
		BAS_LOG_CL, BAS_LOG_NC, DVL_BAS, DAPD_BAS );
	break;
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONST_SetComponentWhatKind]: constituent has a bad object type" ); 
	dummy = TKERR_EndErrMsg();
	break;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type CONST_DoSetComponentWhatKind(constituent, kind, 
    ValueIfClass, ValueIfNotClass, ValueIfDVL, ValueIfDAPD )
    IN OUT CONSTITUENT	constituent;
    IN VALUE_type	kind;
    IN VALUE_type	ValueIfClass;
    IN VALUE_type	ValueIfNotClass;
    IN VALUE_type	ValueIfDVL;
    IN VALUE_type	ValueIfDAPD;
{

    INT_type	rval;	    /* the returned value */

    INTERNAL_CONSTITUENT TheConstituent;

    INT_type dummy;

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* 
	This routine sets the what kind field of the constituent
	    structure for Components ONLY (for other constituents
	    this is done in-line in the MakeMumble procedure in
	    const1.c). 
	This routine is called from CONST_SetComponentWhatKind
	    NOT from MakeComponent.
    */

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( kind ){
    case GENERIC_COMPONENT:
	TheConstituent->what_kind = ValueIfClass;
	break;
    case SPECIFIC_COMPONENT:
	TheConstituent->what_kind = ValueIfNotClass;
	break;
    case DVL_COMPONENT:
	TheConstituent->what_kind = ValueIfDVL;
	break;
    case DAPD_COMPONENT:
	TheConstituent->what_kind = ValueIfDAPD;
	break;
    default:
	/* This shouldn't happen - but just in case... */
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt(
	    "[CONST_DoSetComponentWhatKind]: bad kind value of %d",
	    (INT_type) kind ); 
	dummy = TKERR_EndErrMsg();
	break;
    }


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}


INT_type CONST_SetOtherFields( document, constituent )
    IN DOCUMENT_type	document;
    OUT CONSTITUENT	constituent;
{

    INT_type	rval;	    /* the returned value */
    INT_type	i;	    /* loop index */

    INTERNAL_DOCUMENT_type TheDocument;
    INTERNAL_CONSTITUENT   TheConstituent;

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* 
	This routine sets commonly initialized constituent
	    structure fields for new constituents (i.e. those in the 
	    process of being made as a result of a call to one of the 
	    make routines).
    */

    rval = SUCCESS_INT;
    TheDocument = (INTERNAL_DOCUMENT_type) document;
    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    /* the attribute hash buckets to look empty */
    for( i = (INT_type) 0; i < NBUCKETS; i++){
	TheConstituent->attr_hash_buckets[i] = NULL_ATTRIBUTE;
    }

    /* make the which_attributes fields look empty */
    for( i = (INT_type) 0; i < NBYTES; i++){
	TheConstituent->which_attributes[i] = ( (BYTE_type) 0);
    }

    /* initialy there is no parent - this constituent is isolated */
    TheConstituent->parent = (struct CONSTITUENT_struct *) NULL_CONSTITUENT;

    /* Point to containing document */
    TheConstituent->document = TheDocument;

    /* Set up the debugging information */

    /* Assign id */
    TheConstituent->id = TheDocument->next_constituent_id++;

    /* Init debug counter */
    TheConstituent->debug_counter = (INT_type) 0;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type CONST_InsertConstituent( document, constituent )
    IN OUT DOCUMENT_type	document;
    IN OUT CONSTITUENT		constituent;
{

    INT_type	rval;	    /* the returned value */

    INTERNAL_DOCUMENT_type TheDocument;
    INTERNAL_CONSTITUENT   TheConstituent;

    INT_type dummy;

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* 
	This routine inserts the constituent into the document.

	Note: if there is an error, this routine must leave the
	document's pointers is a 'stable' state as the caller
	is liable to free the constituent !!!

	Note that the switch below is explicit to show what kind
	of constituent goes where in the document. This is done in
	the interest of writting 'self-documenting' code (what ever that
	truly is...), and in the interest of future use of this routine
	by folks other than the initial developer (sic!)

    */

    TheDocument = (INTERNAL_DOCUMENT_type) document;
    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( TheConstituent->what_kind ){
    case DOC_PROFILE:
	rval = CONST_AddSpecialConstituent( &TheDocument->document_profile, constituent );	
	break;
    case LOUT_STYLE:
	rval = CONST_AddDoublyLinked( &TheDocument->layout_styles_list, constituent);
	break;
    case PRESENT_STYLE:
	rval = CONST_AddDoublyLinked( &TheDocument->presentation_styles_list, constituent );
	break;
    case DOC_LOG_ROOT_NC:
	rval = CONST_AddSpecialConstituent( &TheDocument->specific_logical_root, constituent);
	break;
    case DOC_LOG_ROOT_CL:
	rval = CONST_AddSpecialConstituent( &TheDocument->generic_logical_root, constituent );
	break;
    case DOC_LAY_ROOT_NC:
	rval = CONST_AddSpecialConstituent( &TheDocument->specific_layout_root, constituent );
	break;
    case DOC_LAY_ROOT_CL:
	rval = CONST_AddSpecialConstituent( &TheDocument->generic_layout_root, constituent );
	break;
    case CP_CHAR_CONTENT:
    case CP_RAST_CONTENT:
    case CP_GEO_CONTENT:
    case COMP_LOG_NC:
    case COMP_LOG_CL:
    case BAS_LOG_NC:
    case BAS_LOG_CL:
    case PAGE_SET_NC:
    case PAGE_SET_CL:
    case PAGE_NC:
    case PAGE_CL:
    case FRAME_NC:
    case FRAME_CL:
    case BLOCK_NC:
    case BLOCK_CL:
    case EMPTY:

/* assuming no special DVL/DAPD iterators */
    case DVL_PAGE:
    case DVL_FRAME:
    case DVL_BLOCK:
    case DVL_COMP:
    case DVL_BAS:
    case DAPD_BLOCK:
    case DAPD_BAS:

	rval = CONST_AddDoublyLinked( &TheDocument->constituent_list, constituent );
	break;

    default:
	/* ASSERT: This shouldn't happen */
	rval = ERROR_INT;
	TKError = INTERNAL_err;	
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt(
	    "[CONST_InsertConstituent]: constituent has bad what_kind value of %d",
	    (INT_type)  TheConstituent->what_kind ); 
	dummy = TKERR_EndErrMsg();
	break;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}


INT_type CONST_AddSpecialConstituent( SpotInDocument, constituent )
    IN OUT CONSTITUENT	    *SpotInDocument;
    IN CONSTITUENT	    constituent;
{

    INT_type	rval;	    /* the returned value */

    INT_type dummy;

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* 
	This routine sets a 'special' constituent in the document
	structure PROVIDED that the SpotInDocument is NOT
	current 'occupied'. If 'occupied' the EXISTS_err results.
    */

    if( *SpotInDocument != NULL_CONSTITUENT ){
	/* Spot is current occupied so ERROR */
	rval = ERROR_INT;
	TKError = EXISTS_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONST_AddSpecialConstituent]: document already has special constituent" ); 
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* otherwise do the insert */
    *SpotInDocument = constituent;
    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}


INT_type CONST_AddDoublyLinked( SpotInDocument, constituent )
    IN OUT struct CONSTITUENT_struct	*SpotInDocument;
    IN CONSTITUENT			constituent;	    
{

    INT_type	rval;	    /* the returned value */

    INTERNAL_CONSTITUENT    TheConstituent;

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* 
	This routine inserts a constituent at the beginning of 
	one of the doubly linked list structures in the document.

	This routine can not fail.
    */
    
    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    TheConstituent->forward = SpotInDocument->forward;
	/* new constituent points forward at the current first on the list */
    
    TheConstituent->back = SpotInDocument;
	/* new constituent points back at the list head */

    SpotInDocument->forward->back = TheConstituent;
	/* current first on list points back at new constituent */

    SpotInDocument->forward = TheConstituent;
	/* list head points at the new constituent */

    rval = SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}


INT_type CONST_CleanUpConstituent( constituent )
    IN CONSTITUENT	constituent;
{

    INT_type	rval;	    /* the returned value */

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* 
	This routine is called whenever a 'fatal' error
	prevents the making of a constituent. The memory
	for the constituent is freed. Note that the routine
	calling this one is responsible for dealing with the
	error condition, and so, this routine simply
	frees the space - if an error occurs on the free, no
	error recovery is attempted here - presumably something
	is very wrong when you can't free memory.
    */

    rval = MM_Free( constituent );

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type CONST_RemoveConstituent( constituent )
    IN CONSTITUENT constituent;
{

    /*
	Remove the constituent from its place in the
	document.
    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    struct DOCUMENT_struct *TheDocument;

    INT_type dummy;


#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;
    TheDocument = TheConstituent->document;

    
    switch( TheConstituent->what_kind ){

    case DOC_PROFILE:
	rval = CONST_RemoveSpecialConstituent( &TheDocument->document_profile );	
	break;
    case LOUT_STYLE:
	rval = CONST_RemoveDoublyLinked( constituent );
	break;
    case PRESENT_STYLE:
	rval = CONST_RemoveDoublyLinked( constituent );
	break;
    case DOC_LOG_ROOT_NC:
	rval = CONST_RemoveSpecialConstituent( &TheDocument->specific_logical_root);
	break;
    case DOC_LOG_ROOT_CL:
	rval = CONST_RemoveSpecialConstituent( &TheDocument->generic_logical_root );
	break;
    case DOC_LAY_ROOT_NC:
	rval = CONST_RemoveSpecialConstituent( &TheDocument->specific_layout_root );
	break;
    case DOC_LAY_ROOT_CL:
	rval = CONST_RemoveSpecialConstituent( &TheDocument->generic_layout_root );
	break;
    case CP_CHAR_CONTENT:
    case CP_RAST_CONTENT:
    case CP_GEO_CONTENT:
    case COMP_LOG_NC:
    case COMP_LOG_CL:
    case BAS_LOG_NC:
    case BAS_LOG_CL:
    case PAGE_SET_NC:
    case PAGE_SET_CL:
    case PAGE_NC:
    case PAGE_CL:
    case FRAME_NC:
    case FRAME_CL:
    case BLOCK_NC:
    case BLOCK_CL:
    case EMPTY:

/* assuming no special DVL/DAPD iterators */
    case DVL_PAGE:
    case DVL_FRAME:
    case DVL_BLOCK:
    case DVL_COMP:
    case DVL_BAS:
    case DAPD_BLOCK:
    case DAPD_BAS:
	rval = CONST_RemoveDoublyLinked( constituent );
	break;
    default:
	/* ASSERT: This shouldn't happen */
	rval = ERROR_INT;
	TKError = INTERNAL_err;	
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt(
	    "[CONST_RemoveConstituent]: constituent has a bad what kind value of d",
	    (INT_type) TheConstituent->what_kind ); 
	dummy = TKERR_EndErrMsg();
	break;
    }


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type CONST_RemoveDoublyLinked( constituent )
    IN CONSTITUENT constituent;
{

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    /* make the previous node point at the next node */
    TheConstituent->back->forward = TheConstituent->forward;

    /* make the next node point back at the previous */
    TheConstituent->forward->back = TheConstituent->back;
    
LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type CONST_RemoveSpecialConstituent( SpotInDocument )
    OUT CONSTITUENT *SpotInDocument;
{

    INT_type rval;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    *SpotInDocument = NULL_CONSTITUENT;


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}
