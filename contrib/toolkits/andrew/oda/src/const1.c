
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/const1.c,v 1.2 89/10/31 14:23:13 annm Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/const1.c,v $

(c) Copyright 1988 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	const1.c,v $
 * Revision 1.2  89/10/31  14:23:13  annm
 * fixing lint problems
 * 
 * Revision 1.1  89/09/21  06:26:49  mss
 * Initial revision
 * 
 * Revision 1.44  89/09/01  15:38:02  annm
 * fixing USE_LONG_NAMES problems
 * 
 * Revision 1.43  89/04/11  15:32:30  annm
 * implemented CopyConstituent
 * 
 * Revision 1.42  89/03/22  15:50:34  annm
 * fixed dump bug in MakeComponent
 * 
 * Revision 1.41  89/03/10  13:00:36  annm
 * changes for the new DAPD stuff
 * 
 * Revision 1.40  89/02/24  18:06:20  annm
 * new MakeComponent interface
 * 
 * Revision 1.39  89/01/19  18:01:16  annm
 * no change
 * 
 * Revision 1.38  89/01/17  16:57:19  annm
 * changes for DVL defaulting
 * 
 * Revision 1.37  88/12/13  19:56:01  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.36  88/09/12  15:56:39  annm
 * fixed bugs in tkerr calls and added dependency on <tkerr.h>
 * 
 * Revision 1.35  88/09/12  12:57:53  annm
 * added TKError message stuff
 * 
 * Revision 1.34  88/09/06  13:10:53  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.33  88/09/06  12:01:39  annm
 * no change
 * 
 * Revision 1.32  88/08/25  16:12:33  annm
 * wrote delete constituent
 * 
 * Revision 1.31  88/08/03  13:57:49  annm
 * no change
 * 
 * Revision 1.30  88/07/29  11:02:19  mss
 * Removed references to comp_basic_page
 * 
 * Revision 1.29  88/07/19  16:19:42  annm
 * MakeComponent now sets the value of at_OBJECT_TYPE
 * 
 * Revision 1.28  88/06/15  17:24:11  jr
 * Make calls to TKERR_Init
 * 
 * Revision 1.27  88/06/08  16:55:40  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.26  88/06/08  14:34:44  annm
 * added Copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/const1.c,v $";
#endif



#define TK_IMPL_CONST
#define TK_GVARS_CONST1
#define TK_IMPL_DOC
#define TK_IMPL_ATTR
#define TK_TKI_MM
#define TK_TKI_TKERR

#include <acd.h>
#include <mm.h>
#include <attr0.h>
#include <tkerr.h>

#undef TK_IMPL_CONST
#undef TK_GVARS_CONST1
#undef TK_IMPL_DOC
#define TK_IMPL_ATTR
#undef TK_TKI_MM
#undef TK_TKI_TKERR

#include <values.h>


CONSTITUENT CopyConstituent(document, constituent)
    IN DOCUMENT_type	document;
    IN CONSTITUENT	constituent;
{

    /*
	Strategy:
	    1) check the consituent to ensure that it is not NULL_CONSTITEUNT,
		ERROR_CONSTITUENT, or (CONSTITUENT) 0
	    2) check the document to ensure that it is not ERROR_DOCUMENT,
		or (DOCUMENT_type) 0
	    3) allocate space for the new constituent
	    4) copy over fields in the INTERNAL_CONSTITUNET from the 
		'old' constituent to the 'new'; also initilize fields in the
		'new' constituent
	    5) run through the attribute lists of the 'old' constituent
		and build up the attribute lists for the 'new' constituent
		by calling the canonical alloc and copy routines. Also,
		establish the list structure needed to string the attributes
		together.
	
    */

    CONSTITUENT	rval;	    /* the returned value */

    INTERNAL_CONSTITUENT OldConstituent;
    INTERNAL_CONSTITUENT NewConstituent;

    INTERNAL_DOCUMENT_type TheDocument;

    POINTER_type ptr;

    INT_type i;

    struct ATTR_attribute_struct **AttachmentPoint;

    ATTR_attribute_t OldAttribute;
    ATTR_attribute_t NewAttribute;

    INT_type TypeIndex;

    POINTER_type OldStorage;
    POINTER_type NewStorage;

    PARM_MASK_type dummy_mask;

    INT_type SlaveRval;

    INT_type dummy;

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* check the constituent */    
    if( constituent == NULL_CONSTITUENT ||
	constituent == ERROR_CONSTITUENT ||
	constituent == (CONSTITUENT) 0){
	rval = ERROR_CONSTITUENT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[CopyConstituent]: constituent is either NULL_CONSTITUENT, ERROR_CONSTITUENT or (CONSTITUENT) 0" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* check the document */
    if( document == (DOCUMENT_type) ERROR_DOCUMENT ||
	document == (DOCUMENT_type) 0 ){
	rval = ERROR_CONSTITUENT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[CopyConstituent]: the doucment is either ERROR_DOCUMENT or (DOCUMENT_type) 0" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* get space for the new constituent */
    ptr = MM_Malloc( (INT_type) sizeof( struct CONSTITUENT_struct ) );
    if( ptr == ERROR_POINTER ){
	rval = ERROR_CONSTITUENT;
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[CopyConstituent]: can not get memory for the new constituent" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    rval = (CONSTITUENT) ptr;

    OldConstituent = (INTERNAL_CONSTITUENT) constituent;
    TheDocument = (INTERNAL_DOCUMENT_type) document;

    NewConstituent = (INTERNAL_CONSTITUENT) ptr;

    /* copy/initialize over the INTERNAL_CONSTITUENT fields */

    NewConstituent->what_kind = OldConstituent->what_kind;
    NewConstituent->document = OldConstituent->document;
    NewConstituent->parent = (struct CONSTITUENT_struct *) NULL_CONSTITUENT;
    NewConstituent->id = TheDocument->next_constituent_id++;
    NewConstituent->debug_counter = (INT_type) OldConstituent->debug_counter;

    /* copy over the attributes */
    for( i = (INT_type) 0; i < NBUCKETS; i++ ){

	AttachmentPoint = &NewConstituent->attr_hash_buckets[i];

	for( OldAttribute = OldConstituent->attr_hash_buckets[i];
	     OldAttribute != NULL_ATTRIBUTE;
	     OldAttribute = OldAttribute->next ){

	    TypeIndex = ATTR_IndexIntoTypes[OldAttribute->attribute_id];

	    OldStorage = (*ATTR_AttrTypeTbl[TypeIndex].get)(OldAttribute);

	    NewStorage = (*ATTR_AttrTypeTbl[TypeIndex].allocate)(OldStorage);
	    if( NewStorage == ERROR_POINTER ){
		/* 
		    there is a CORE LEAK here - should go free the 
		    already allocated attributes.
		*/
		rval = ERROR_CONSTITUENT;
		dummy = MM_Free( (POINTER_type) rval );
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[CopyConstituent]:  can not get memory for new attribute value" );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	    }
	    
	    dummy_mask = (PARM_MASK_type) 0;
	    SlaveRval = (*ATTR_AttrTypeTbl[TypeIndex].copy)(
			    /* source */ OldStorage,
			    /* source mask */ OldAttribute->parm_mask,
			    /* wanted wask */ OldAttribute->parm_mask,
			    /* dest mask */ &dummy_mask,
			    /* destination */ NewStorage );
	    if( SlaveRval == ERROR_INT ){
		/* 
		    there is a CORE LEAK here - should go free the 
		    already allocated attributes.
		*/
		rval = ERROR_CONSTITUENT;
		dummy = MM_Free( (POINTER_type) rval );
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[CopyConstituent]:  can not copy into the new attribute" );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	    }

	    ptr = MM_Malloc( (INT_type) sizeof( struct ATTR_attribute_struct ) );
	    if( ptr == ERROR_POINTER ){
		/* 
		    there is a CORE LEAK here - should go free the 
		    already allocated attributes.
		*/
		rval = ERROR_CONSTITUENT;
		dummy = MM_Free( (POINTER_type) rval );
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[CopyConstituent]:  can not get memory for new attribute structure" );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;		
	    }

	    NewAttribute = (struct ATTR_attribute_struct *) ptr;

	    NewAttribute->attribute_id = OldAttribute->attribute_id;
	    NewAttribute->parm_mask = OldAttribute->parm_mask;

	    SlaveRval = (*ATTR_AttrTypeTbl[TypeIndex].set)(NewAttribute,
			    NewStorage );
	    if( SlaveRval == ERROR_INT ){
		/* 
		    there is a CORE LEAK here - should go free the 
		    already allocated attributes.
		*/
		rval = ERROR_CONSTITUENT;
		dummy = MM_Free( (POINTER_type) rval );
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[CopyConstituent]:  can not do the canonical set for the attribute" );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;	
	    }

	    *AttachmentPoint = NewAttribute;

	    AttachmentPoint = &NewAttribute->next;

	    CONST_UnsafeSetAttr( NewConstituent, NewAttribute->attribute_id );
	    
	}

	*AttachmentPoint = NULL_ATTRIBUTE;
    
    }    
    
    /* put the NewConstituent in the document's list structure */
    if( CONST_InsertConstituent( document, rval ) == ERROR_INT ){
	/* Clean up and leave - ASSERT: lower level set TKError */
	/* CORE LEAK here - should free all the attributes */
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



INT_type DeleteConstituent(constituent)
    IN CONSTITUENT	constituent;
{

    /*
	Stragegy:
	    1) check the constituent to make sure that it
		isn't obviously in error
	    2) if the constituent has the at_SUBORDINATES
		attribute, then unthread the tree
	    3) delete the attribute (using the canonical aunion.h
		procedures)
	    4) remove the constituent for the its place in 
		the document 
	    5) free the constituent structure.
    */

    INT_type	rval;	    /* the returned value */

    INTERNAL_CONSTITUENT TheConstituent;

    INT_type SlaveRval;

    INT_type dummy;

    ATTR_attribute_t AttrStructPtr;

    INT_type i;

    INT_type TypeIndex;

    POINTER_type TheStorage;

    ATTR_attribute_t Temp;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* first check the constituent */
    if( constituent == NULL_CONSTITUENT || 
	constituent == ERROR_CONSTITUENT ||
	constituent == (CONSTITUENT) 0 ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( 
	    "[DeleteConstituent]: constituent parameter ERROR or NULL" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* if the constituent has subordinates the unthread the tree */
    SlaveRval = HasAttr( constituent, at_SUBORDINATES );
    if( SlaveRval == HAS_ATTR ){
	dummy = ATTR_UnThreadTree( constituent );
    }

    /* delete the attributes */

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    for( i = 0; i < NBUCKETS; i++ ){

	for( AttrStructPtr = TheConstituent->attr_hash_buckets[i];
	     AttrStructPtr != NULL_ATTRIBUTE; ){

	    TypeIndex = ATTR_IndexIntoTypes[AttrStructPtr->attribute_id];
		
	    TheStorage = 
		(*ATTR_AttrTypeTbl[TypeIndex].get)(AttrStructPtr);
	    SlaveRval = (*ATTR_AttrTypeTbl[TypeIndex].free)(TheStorage);

	    Temp = AttrStructPtr->next;

	    dummy = MM_Free( (POINTER_type) AttrStructPtr );

	    AttrStructPtr = Temp;
	    
	}
    }

    /* fourth remove the constituent from the document */

    dummy = CONST_RemoveConstituent( constituent );

    /* fifth: free the constituent structure */
    dummy = MM_Free( constituent );    

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}




CONSTITUENT MakeComponent(document, type, kind)
    IN DOCUMENT_type	document;
    IN VALUE_type	type;
    IN VALUE_type	kind;
{

    /*
	Strategy:
	    1) check the arguements:
		a) accept document as an act of faith
		b) type must be one of the values for the type
		     (c.f. 11.1.1.1 in the api.)
		c) kind must be legal
	    2) allocate space for the component - if this fails
		then return NOMEM_err
	    3) load values into the constituent structure.
	    4) SetValueAttr for at_OBJECT_TYPE
	    5) add the constituent structure to the document
		(N.B. all components go on the document's component
		list except the roots which have special pointers
		in the document structure.)

    */

    CONSTITUENT	rval;	    /* the returned value */
    POINTER_type p;	    /* temp pointer for the malloc */
    INT_type	 dummy;	    /* from clean up */

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* First: check the arguments */

    /* check of the type arguement */
    if( type != at_OBJECT_TYPE_doc_layout_root &&
	type != at_OBJECT_TYPE_page_set &&
	type != at_OBJECT_TYPE_page &&
	type != at_OBJECT_TYPE_frame &&
	type != at_OBJECT_TYPE_block &&
	type != at_OBJECT_TYPE_doc_logical_root &&
	type != at_OBJECT_TYPE_comp_logical_obj &&
	type != at_OBJECT_TYPE_bas_logical_obj 
    ){
	rval = 	ERROR_CONSTITUENT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt( 
		"[MakeComponent]: Bad type parameter  %d", (INT_type) type);
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* check of the kind arguement */
    if( kind != SPECIFIC_COMPONENT  && 
	kind != GENERIC_COMPONENT &&
	kind != DVL_COMPONENT &&
	kind != DAPD_COMPONENT ){
	rval = ERROR_CONSTITUENT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( 
		"[MakeComponent]: kind is not one of the legal defined values" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* check for consistency between kind and type */
    if( kind == DVL_COMPONENT &&
	!( type == at_OBJECT_TYPE_page ||
	    type == at_OBJECT_TYPE_frame ||
	    type == at_OBJECT_TYPE_block ||
	    type == at_OBJECT_TYPE_comp_logical_obj ||
	    type == at_OBJECT_TYPE_bas_logical_obj )
    ){
	rval = ERROR_CONSTITUENT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[MakeComponent]: kind and type are inconsistent - i.e. kind of dvl disallows doc log/layout roots and page set");
	dummy = TKERR_EndErrMsg();
	    goto LEAVE;
    }

    if( kind == DAPD_COMPONENT &&
	!( type == at_OBJECT_TYPE_block ||
	    type == at_OBJECT_TYPE_bas_logical_obj ) ){
	rval = ERROR_CONSTITUENT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[MakeComponent]: type and kind are inconsistent, i.e. kind of dapd disallows some types" );
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
		"[MakeComponent]: Can't get memory for the component" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    rval = (CONSTITUENT) p;

    /* Third: load up the constituent sturcture */

    /* Set what kind field */
    if( CONST_SetComponentWhatKind( rval, type, kind ) == ERROR_INT ){
	/* Clean up and leave */
	/* ASSERT: TKError got set */
	dummy = CONST_CleanUpConstituent( rval );
	rval = ERROR_CONSTITUENT;
	goto LEAVE;
    }

    /* Set the common fields */
    if( CONST_SetOtherFields( document, rval ) == ERROR_INT ){
	/* Clean up and leave */
	/* ASSERT: TKError got set */
	dummy =  CONST_CleanUpConstituent( rval );
	rval = ERROR_CONSTITUENT;
	goto LEAVE;
    }

    /* Fourth: Set the at_OBJECT_TYPE */
    if( kind == GENERIC_COMPONENT || kind == SPECIFIC_COMPONENT ){
	if( SetValueAttr( rval, at_OBJECT_TYPE, type) == ERROR_INT ){
	    /* ASSERT: SetValueAttr set TKError */
	    dummy = CONST_CleanUpConstituent( rval );
	    rval = ERROR_CONSTITUENT;
	    goto LEAVE;
	}
    }

    /* 
	Fifth: stick the constituent in the document
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


CONSTITUENT MakeContent(document, type)
    IN DOCUMENT_type	document;
    IN VALUE_type	type;
{

    /*
	Strategy:
	    1) check the arguements:
		a) document is accepted as an act of faith
		b) type must be per MakeContent documentation in the api
	      If type is not ok then return BADPARM_err.
	    2) allocate space for the new content portion -
		if this fails return NOMEM_err
	    3) load up the constituent structure
	    4) insert the new constituent in the document's
		constituent_list
    */

    CONSTITUENT	rval;	    /* the returned value */
    POINTER_type p;	    /* temp pointer for the malloc */
    INT_type	 dummy;	    /* return from the clean routine */

    INTERNAL_CONSTITUENT    TheConstituent;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif
    
    /* First check the type arguement */
    if( type != CHARACTER_CONTENT &&
	type != GEOMETRIC_GRAPHICS_CONTENT &&
	type != RASTER_GRAPHICS_CONTENT 
    ){
	rval = ERROR_CONSTITUENT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt( "[MakeContent]: Bad type of %d", 
		    (INT_type) type );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Second get space for the new constituent */
    p = MM_Malloc( (INT_type) sizeof( struct CONSTITUENT_struct ) );
    if( p == ERROR_POINTER ){
	rval = ERROR_CONSTITUENT;
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( 
		"[MakeContent]: Can't get memory for the content portion" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    rval = (CONSTITUENT) p;
    TheConstituent =  (INTERNAL_CONSTITUENT) p;

    /* Third: load up the new constituent */
    
    /* the what_kind field */
    if( type == CHARACTER_CONTENT ){
	TheConstituent->what_kind = CP_CHAR_CONTENT;
    }
    else if( type == GEOMETRIC_GRAPHICS_CONTENT ){
	TheConstituent->what_kind = CP_GEO_CONTENT;
    }
    else {
	/* given check at First must be Raster Content */
	TheConstituent->what_kind = CP_RAST_CONTENT;
    }

    /* Set the common fields */
    if( CONST_SetOtherFields( document, rval ) == ERROR_INT ){
	/* clean up and leave */
	/* ASSERT: TKError got set */
	dummy = CONST_CleanUpConstituent( rval );
	rval = ERROR_CONSTITUENT;
	goto LEAVE;
    }

    /* Fourth: add to the constituent_list in the document */
    if( CONST_InsertConstituent( document, rval ) == ERROR_INT ){
	/* clean up and leave - ASSERT: lower level set TKError */
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



CONSTITUENT MakeDocumentProfile(document)
    IN DOCUMENT_type	document;
{

    /*
	Bote: document arguement is accepted as an act of faith.

	Strategy:
	    1) allocate space for the new content portion -
		if this fails return NOMEM_err
	    2) load up the constituent structure
	    3) insert the new constituent in the document's
		constituent_list
    */

    CONSTITUENT	rval;	    /* the returned value */
    POINTER_type p;	    /* temp pointer for the malloc */
    INT_type     dummy;	    /* return from the clean up routine */

    INTERNAL_CONSTITUENT    TheConstituent;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* First get space for the new constituent */
    p = MM_Malloc( (INT_type) sizeof( struct CONSTITUENT_struct ) );
    if( p == ERROR_POINTER ){
	rval = ERROR_CONSTITUENT;
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( 
	    "[MakeDocumentProfile]: Can't get memory for the document profile");
	dummy = TKERR_EndErrMsg();

	goto LEAVE;
    }
    rval = (CONSTITUENT) p;
    TheConstituent = (INTERNAL_CONSTITUENT) p;

    /* Second: load up the new constituent */
    
    /* the what_kind field */
    TheConstituent->what_kind = DOC_PROFILE;

    /* Set the common fields */
    if( CONST_SetOtherFields( document, rval ) == ERROR_INT ){
	/* clean up and leave */
	dummy = CONST_CleanUpConstituent( rval );
	rval = ERROR_CONSTITUENT;
	goto LEAVE;
    }

    /* Third: add to the document */
    if( CONST_InsertConstituent( document, rval ) == ERROR_INT ){
	/* clean up and leave - ASSERT: lower level set TKError */
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




CONSTITUENT MakeStyle(document, kind)
    IN DOCUMENT_type	document;
    IN VALUE_type	kind;	 
{
    
    /*
	Strategy:
	    1) check the kind arguement:
		must be PRESENTATION_STYLE or LAYOUT_STYLE - if not return
		BADPARM_err.
		Accept document as an act of faith.
	    2) allocate storage for the style - if fails
		return NOMEM_err.
	    3) load up the constituent structure
	    4) add to the document
    */

    CONSTITUENT	rval;	    /* the returned value */
    POINTER_type p;	    /* temp pointer for the malloc */
    INT_type	 dummy;	    /* return from the clean up routine */

    INTERNAL_CONSTITUENT    TheConstituent;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* First: check the kind arguement */
    if( kind != PRESENTATION_STYLE &&
	kind != LAYOUT_STYLE ){
	rval = ERROR_CONSTITUENT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( 
		"[MakeStyle]: kind is not PRESENTATION_STYLE or LAYOUT_STYLE" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* Second: get space for the style */
    p = MM_Malloc( (INT_type) sizeof( struct CONSTITUENT_struct ) );
    if( p == ERROR_POINTER ){
	rval = ERROR_CONSTITUENT;
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( 
		"[MakeStyle]: Can't get memory for the style" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    rval = (CONSTITUENT) p;
    TheConstituent = (INTERNAL_CONSTITUENT) p;

    /* Third: load up the constituent structure */

    /*
	Set the what_kind field based on the kind arguement 
	N.B. this test assumes that there are only two legal
	values for kind. DANGER DANGER DANGER !!!!!!!!!!!!!!!!!!!!!!!!
    */
    
    if( kind == LAYOUT_STYLE ){
	TheConstituent->what_kind = LOUT_STYLE;
    }
    else {
	/* Given check done at First must be Presentation Style */
	TheConstituent->what_kind = PRESENT_STYLE;
    }

    /* Set the common fields */
    if( CONST_SetOtherFields( document, rval ) == ERROR_INT ){
	/* clean up and leave */
	dummy = CONST_CleanUpConstituent( rval );
	rval = ERROR_CONSTITUENT;
	goto LEAVE;
    }

    /* Fourth: add to the document */
    if( CONST_InsertConstituent( document, rval ) == ERROR_INT ){
	/* clean up and leave - ASSERT: lower level set TKError */
	dummy = CONST_CleanUpConstituent( rval );
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



INT_type CONST_Final()
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
    dummy = TKERR_Format( "[CONST_Final]: Not Implemented" );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type CONST_Init()
{

    INT_type	rval;	    /* the returned value */

#ifndef USE_LONG_NAMES

#define THE_ERROR_CONSTITUENT gacon
#define THE_NULL_CONSTITUENT gancon

#else

#define THE_ERROR_CONSTITUENT The_Error_Constituent
#define THE_NULL_CONSTITUENT The_Null_Constituent

#endif

    static CONSTITUENT THE_ERROR_CONSTITUENT;
    static CONSTITUENT THE_NULL_CONSTITUENT;    

    /* put local dcl's here */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Assume that all will work */
    rval = SUCCESS_INT;

    /* set up the ERROR_CONSTITUENT */
    ERROR_CONSTITUENT  =  (CONSTITUENT)&THE_ERROR_CONSTITUENT;
    NULL_CONSTITUENT = (CONSTITUENT) &THE_NULL_CONSTITUENT;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}
