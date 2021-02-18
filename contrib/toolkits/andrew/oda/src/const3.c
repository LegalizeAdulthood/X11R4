
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/const3.c,v 1.2 89/10/31 14:27:56 annm Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/const3.c,v $

(c) Copyright 1988 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	const3.c,v $
 * Revision 1.2  89/10/31  14:27:56  annm
 * fixing lint problems
 * 
 * 
 * Revision 1.1  89/09/21  06:27:31  mss
 * Initial revision
 * 
 * Revision 1.6  89/09/12  09:13:16  mss
 * Removed #
 * 
 * Revision 1.5  89/02/24  18:22:14  annm
 * no change
 * 
 * Revision 1.4  88/09/28  15:18:21  annm
 * added support for moving the old empty from one document list to another on a FillInConstituent
 * 
 * Revision 1.3  88/09/21  15:48:13  annm
 * added tkerr stuff
 * 
 * Revision 1.2  88/09/06  13:11:09  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.1  88/08/25  16:13:31  annm
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/const3.c,v $";
#endif



#define TK_IMPL_CONST
#define TK_GVARS_CONST3
#define TK_IMPL_DOC
#define TK_IMPL_ATTR
#define TK_TKI_MM
#define TK_TKI_TKERR

#include <acd.h>
#include <mm.h>
#include <tkerr.h>

#undef TK_IMPL_CONST
#undef TK_GVARS_CONST3
#undef TK_IMPL_DOC
#define TK_IMPL_ATTR
#undef TK_TKI_MM
#undef TK_TKI_TKERR



CONSTITUENT CONST_MakeConstituent( document )
    IN DOCUMENT_type document;
{
    /*
	Strategy:
	    1) check the arguements:
		a) check the document for ERROR_DOCUMENT and
		    (DOCUMENT_type) 0
	      If there is a problem return BADPARM_err.
	    2) allocate space for the constituent - if this fails
		then return NOMEM_err
	    3) load values into the constituent structure.
	    4) add the constituent structure to the document
		(N.B. all empty constituents go on the constituent
		list in the document structure.)

    */

    CONSTITUENT	rval;	    /* the returned value */
    POINTER_type p;	    /* temp pointer for the malloc */
    INT_type	 dummy;	    /* from clean up */

    INTERNAL_CONSTITUENT    TheConstituent;

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* First: check the arguments */
    if( document == ERROR_DOCUMENT ||
	document == (DOCUMENT_type) 0 ){
	rval = ERROR_CONSTITUENT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt(
	    "[CONST_MakeConstituent]: document argument is bad - got value of %d",
	    (INT_type) document ); 
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* ASSERT: if get here then the args check ok */

    /* 
	Second: get space for the component 
	If this fails return NOMEM_err
    */
    p = MM_Malloc( (INT_type) sizeof( struct CONSTITUENT_struct ) );
    if( p == ERROR_POINTER ){
	rval = ERROR_CONSTITUENT;
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONST_MakeConstituent]: can not get memory for a constituent" ); 
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    rval = (CONSTITUENT) p;

    /* Third: load up the constituent sturcture */

    /* Set what kind field */
    TheConstituent = (INTERNAL_CONSTITUENT) rval;
    TheConstituent->what_kind = EMPTY;

    /* Set the common fields */
    if( CONST_SetOtherFields( document, rval ) == ERROR_INT ){
	/* Clean up and leave */
	dummy =  CONST_CleanUpConstituent( rval );
	rval = ERROR_CONSTITUENT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONST_MakeConstituent]: failure in setting other fields in the constituent structure" ); 
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* 
	Fourth: stick the constituent in the document
    */
    if( CONST_InsertConstituent( document, rval ) == ERROR_INT ){
	/* Clean up and leave - ASSERT: lower level set TKError */
	dummy =  CONST_CleanUpConstituent( rval );
	rval = ERROR_CONSTITUENT;
	goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );


}



INT_type CONST_FillInConstituent( EmptyConstituent, FullConstituent )
    IN OUT CONSTITUENT EmptyConstituent;
    IN OUT CONSTITUENT FullConstituent;
{

    /*
	Strategy:
	    1) checking:
		a) both constituents to checked to ensure that they
		    are not the NULL_CONSTITUENT, ERROR_CONSTITUENT or
		    (CONSTITUENT) 0
		b) The EmptyConstituent must have its what_kind field ==
		    EMPTY, and the FullConstituent must have its what_kind
		    field != EMPTY
		c) both constituent must be in the same document.
		d) The FullConstituent must not have a parent.

	    2) fields from the FullConstituent are copied to the empty
		Fields that are copied are what_kind, the attr_hash_buckets
		and the which_attributes.

	    3) the FullConstituent is removed from this document list system.

	    4) the full constituent is deallocated

	    5) see if the empty constituent needs to be moved from one list in
		the document to another - if so do the move
	
    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheFull;
    INTERNAL_CONSTITUENT TheEmpty;

    INT_type dummy;

    INT_type SlaveRval;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    /* First: perform the required checking */

    if( EmptyConstituent == ERROR_CONSTITUENT ||
	EmptyConstituent == NULL_CONSTITUENT ||
	EmptyConstituent == (CONSTITUENT) 0 ||
	FullConstituent == ERROR_CONSTITUENT ||
	FullConstituent == NULL_CONSTITUENT ||
	FullConstituent == (CONSTITUENT) 0 ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONST_FillInConstituent]: error in one of the constituent arguements" ); 
	dummy = TKERR_FormatInt(" - got %d for the full and ", 
		    (INT_type) FullConstituent);
	dummy = TKERR_FormatInt("%d for the empty", 
		    (INT_type) EmptyConstituent );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheFull = (INTERNAL_CONSTITUENT) FullConstituent;
    TheEmpty = (INTERNAL_CONSTITUENT) EmptyConstituent;

    if( CONST_IsEmpty( TheFull ) ||
	! CONST_IsEmpty( TheEmpty) ||
	TheFull->document != TheEmpty->document ||
	TheFull->parent != (struct CONSTITUENT_struct *) NULL_CONSTITUENT ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONST_FillInConstituent]: Consistency check on the two constituents failed" ); 
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Second: copy over fields */
    dummy = CONST_Chameleon( /* source */ FullConstituent, 
		/* chameleon */ EmptyConstituent );

    /* Third: remove the Full from the document lists */
    dummy = CONST_RemoveConstituent( FullConstituent );

    /* Fourth: deallocate the Full */
    dummy = MM_Free( (POINTER_type) FullConstituent );

    /* Fifth: see if should move 'empty' to another docuemnt list -
	if yes do the move */
    SlaveRval = CONST_MoveConstituent( EmptyConstituent );
    if( SlaveRval == ERROR_INT ){
	/* ASSERT: TKError got set */
	rval = ERROR_INT;
	goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type CONST_CollaspeContentPortions( orphan, c1, c2 )
    IN OUT CONSTITUENT orphan;
    IN OUT CONSTITUENT c1;
    IN OUT CONSTITUENT c2;
{

    /*
	Strategy:
	    1) check arguements:
		a) the argument constituents must not
		    be ERROR_CONSTITUENT, NULL_CONSTITUENT, or
		    (CONSTITUENT) 0.
		b) The ophan must be a content portion.
		c) c1 anc c2 must be empty.
		d) all three constituents must be in the same document.
		e) the orphan must not have a parent; c1 and c2 must
		    have parents
	    2) make c1 acquire the orphan's what_kind, attr_hash_buckets and
		which_attributes
	    3) move at_CONTENT_PORTIONS references in c2's parent.
	    4) remove c2 and the orphan from the document's lists
	    5) deallocate c2 and the orphan.
	
    */

    INT_type rval;

    INTERNAL_CONSTITUENT C1;
    INTERNAL_CONSTITUENT C2;
    INTERNAL_CONSTITUENT Orphan;

    INT_type dummy;

    INT_type SlaveRval;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    /* First perform the required checking */
    
    if( orphan == ERROR_CONSTITUENT ||
	orphan == NULL_CONSTITUENT ||
	orphan == (CONSTITUENT) 0 ||
	c1 == ERROR_CONSTITUENT ||
	c1 == NULL_CONSTITUENT ||
	c1 == (CONSTITUENT) 0 ||
	c2 == ERROR_CONSTITUENT ||
	c2 == NULL_CONSTITUENT ||
	c2 == (CONSTITUENT) 0 ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONST_CollaspeContentPortions]: one of the input consituents is error, null or 0" ); 
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    Orphan = (INTERNAL_CONSTITUENT) orphan;

    if( ! CONST_IsContent( Orphan ) ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONST_CollaspeContentPortions]: orphan is not a content portion" ); 
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    C1 = (INTERNAL_CONSTITUENT) c1;
    C2 = (INTERNAL_CONSTITUENT) c2;
    
    if( ! CONST_IsEmpty( C1 ) ||
	! CONST_IsEmpty( C2 ) ){
	rval = ERROR_INT;   
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONST_CollaspeContentPortions]: one of c1/c2 is not empty" ); 
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( Orphan->document != C1->document ||
	C1->document != C2->document ||
	Orphan->document != C2->document ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONST_CollaspeContentPortions]: orphan, c1 and c2 are not all in the same document" ); 
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( Orphan->parent != (struct CONSTITUENT_struct *) NULL_CONSTITUENT ||
	C1->parent == (struct CONSTITUENT_struct *) NULL_CONSTITUENT ||
	C2->parent == (struct CONSTITUENT_struct *) NULL_CONSTITUENT ){
	rval = ERROR_INT;   
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONST_CollaspeContentPortions]: one of orphan, c1, c2 has a bad parent" ); 
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Second: c1 acquires the orphan's what_kind etc. */
    dummy = CONST_Chameleon( /* source */ orphan,
		/* chameleon */ c1 );

    /* Third: change c2's parents' content portion ref to c1 */
    SlaveRval = CONST_ChangeContentPortionRefs( /* change to */ c1,
		    /* from */ c2 );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONST_CollaspeContentPortions]: Can not change content portions references" ); 
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Fourth: remove the orphan and c2 from the document */
    dummy = CONST_RemoveConstituent( orphan );    
    dummy = CONST_RemoveConstituent( c2 );    

    /* Fifth: deallocate c2 and c1 */
    dummy = MM_Free( (POINTER_type) c2 );
    dummy = MM_Free( (POINTER_type) orphan );

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type CONST_Chameleon( source, chameleon )
    IN CONSTITUENT source;
    OUT CONSTITUENT chameleon;
{

    /*
	Copy what_kind, attr_hash_buckets and which_attributes from
	the source to the chameleon.
    */
    
    INT_type rval;

    INTERNAL_CONSTITUENT Source;
    INTERNAL_CONSTITUENT Chameleon;

    INT_type i;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    Source = (INTERNAL_CONSTITUENT) source;
    Chameleon = (INTERNAL_CONSTITUENT) chameleon;


    Chameleon->what_kind = Source->what_kind;

    for( i = 0; i < NBUCKETS; i++){
	Chameleon->attr_hash_buckets[i] = Source->attr_hash_buckets[i];
    }

    for( i = 0; i < NBYTES; i++){
	Chameleon->which_attributes[i] = Source->which_attributes[i];
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type CONST_ChangeContentPortionRefs( ChangeTo, From )
    IN CONSTITUENT ChangeTo;
    IN CONSTITUENT From;
{

    /* 
	Strategy:
	    1) get From's parent's content portion attribute
	    2) rummage through the content portion attribute value
		looking for the reference to From and change it to ChangeTo
    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheFrom;
    INTERNAL_CONSTITUENT TheParent;

    ATTR_attribute_t AttrStructPtr;
    
    SEQUENCE_CONSTITUENT_type TheContentPortions;
    CONSTITUENT TheContent;

    INT_type i;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    TheFrom = (INTERNAL_CONSTITUENT) From;

    TheParent = TheFrom->parent;

    if( TheParent == (struct CONSTITUENT_struct *) NULL_CONSTITUENT ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONST_ChangeContentPortionRefs]: From's parent is NULL_CONSTITUENT" ); 
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    
    SlaveRval = HasAttr( (CONSTITUENT) TheParent, at_CONTENT_PORTIONS );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONST_ChangeContentPortionRefs]: has attr for from's parent's content portion attribute failed" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    if( SlaveRval == NO_ATTR ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONST_ChangeContentPortionRefs]: from's parent doesn't have the content portions attribute" ); 
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    
    AttrStructPtr = ATTR_FindAttr( (CONSTITUENT) TheParent, 
			at_CONTENT_PORTIONS );
    if( AttrStructPtr == ERROR_ATTRIBUTE ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONST_ChangeContentPortionRefs]: from's parent has the content portions attribute but can't find it" ); 
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheContentPortions = GET_SEQUENCE_CONSTITUENT( AttrStructPtr );

    for( i = 0; i < TheContentPortions->length; i++){

	TheContent = TheContentPortions->sequence_value.constituents[i];

	if( TheContent == From ){
	    TheContentPortions->sequence_value.constituents[i] = ChangeTo;
	    break;
	}
    }   

    if( i == TheContentPortions->length ){
	/* we didn't find From in the content portions - HORRORS !!! */
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONST_ChangeContentPortionRefs]: can't find from in from's parent's content portions list" ); 
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type CONST_MoveConstituent( constituent )
    IN OUT CONSTITUENT constituent;
{

    /*
	ASSERT: the constituent is a former empty that has
	    been filled in. When created as an empty, it was
	    placed in the document's constituent list. See
	    if it now needs to be move to somewhere else in the
	    document list structure - if so remove it
	    from the constituent list and insert it in the right
	    place.
    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( TheConstituent->what_kind ){
    
    case DOC_PROFILE:
    case LAYOUT_STYLE:
    case PRESENT_STYLE:
    case DOC_LOG_ROOT_NC:
    case DOC_LOG_ROOT_CL:
    case DOC_LAY_ROOT_NC:
    case DOC_LAY_ROOT_CL:
	/* first remove the the constituent from the constituent list */
	TheConstituent->back->forward = TheConstituent->forward;
	    /*
		 the back constituent points forward at the constituent's
		forward
	    */
	TheConstituent->forward->back = TheConstituent->back;
	    /*
		the forward constituent points back at the constituent's
		back
	    */
	/* now insert the constituent in the right place */
	SlaveRval = CONST_InsertConstituent( 
			(DOCUMENT_type) TheConstituent->document,
			constituent );
	if( SlaveRval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    rval = ERROR_INT;
	}
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
	/* no move necessary */
	break;
    
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONST_MoveConstituent]: bad what_kind in constituent being moved" );
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
