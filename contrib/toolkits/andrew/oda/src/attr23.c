
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr23.c,v 1.2 89/10/31 13:43:11 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr23.c,v $

(c) Copyright 1988 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	attr23.c,v $
 * Revision 1.2  89/10/31  13:43:11  jr
 * Casting errors.
 * 
 * Revision 1.1  89/09/21  06:20:53  mss
 * Initial revision
 * 
 * Revision 1.4  89/09/12  09:07:55  mss
 * Removed #
 * 
 * Revision 1.3  89/03/09  15:51:48  annm
 * added aunion.h canonical procedures for the new dp_DOC_APP_PROF_DEFAULTS_type
 * 
 * Revision 1.2  89/02/23  20:38:31  annm
 * support for the new at_DEFAULT_VALUE_LISTS_type
 * 
 * Revision 1.1  89/02/09  17:44:34  annm
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr23.c,v $";
#endif

#define TK_IMPL_ATTR
#define TK_GVARS_ATTR23
#define TK_TKI_TKERR
#define TK_IMPL_CONST
#define TK_TKI_MM
#define TK_IMPL_SEQ

#include <rnames.h>
#include <acd.h>
#include <tkerr.h>
#include <const.h>
#include <mm.h>
#include <seq.h>

#include <attr0.h>

#include <values.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR23
#undef TK_TKI_TKERR
#undef TK_IMPL_CONST
#undef TK_TKI_MM
#undef TK_IMPL_SEQ

/*
    This file contains the canonical aunion.h operations for the following 
	types:

	cc_ITEMISATION_type
	at_DEFAULT_VALUE_LISTS_type

*/



INT_type cc_ITEMISATION_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type destination;
{

    INT_type rval;

    cc_ITEMISATION_type *Source;
    cc_ITEMISATION_type *Destination;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    Source = (cc_ITEMISATION_type *) source;
    Destination = (cc_ITEMISATION_type *) destination;

    Destination->identifier_alignment = Source->identifier_alignment;
    Destination->identifier_start_offset = Source->identifier_start_offset;
    Destination->identifier_end_offset = Source->identifier_end_offset;

    *destination_mask = source_mask;
    
LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type cc_ITEMISATION_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Malloc( (INT_type) sizeof( cc_ITEMISATION_type ) );
    if( rval == ERROR_POINTER ){
	/* ASSERT: TkError got set */
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[cc_ITEMISATION_Alloc]: can not get memory for a cc_ITEMISATION_type" );
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



INT_type cc_ITEMISATION_Free(storage)
	IN POINTER_type storage;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Free( storage );
    if( rval == ERROR_INT ){
	/* ASSERT: TKError got set */
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[cc_ITEMISATION_Free]: can not free the cc_ITEMISATION_type" );
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

 

INT_type cc_ITEMISATION_Set(AttrStructPtr, TheStorage)
	OUT ATTR_attribute_t AttrStructPtr;
	IN POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    AttrStructPtr->attribute_value.cc_ITEMISATION_field =
	(cc_ITEMISATION_type *) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type cc_ITEMISATION_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_cc_ITEMISATION( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type cc_ITEMISATION_CkVal(constituent, attribute, value, mask)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN POINTER_type value;
	IN PARM_MASK_type mask;
{


    /*
	Attributes with type cc_ITEMISATION_type:
	    cc_ITEMISATION
    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    INT_type dummy;

    cc_ITEMISATION_type *TheValue;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;
    
    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ) {
    
    case cc_ITEMISATION:
	if( !( CONST_IsBlock( TheConstituent ) ||
		CONST_IsDVLBlock( TheConstituent ) ||
		CONST_IsDAPDBlock( TheConstituent ) ||
		CONST_IsPresentStyle( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[cc_ITEMISATION_CkVal]: ");
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format(" must go on a block or presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;
    
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[cc_ITEMISATION_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " doesn't have cc_ITEMISATION_type" );
	dummy = TKERR_EndErrMsg();	
	goto LEAVE;
    }

    TheValue = (cc_ITEMISATION_type *) value;
    
    if( TheValue->identifier_alignment != cc_ITEMISATION_no_itemisation &&
	TheValue->identifier_alignment != cc_ITEMISATION_start_aligned &&
	TheValue->identifier_alignment != cc_ITEMISATION_end_aligned ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[cc_ITEMISATION_CkVal]: identifier_alignment field has an illegal value" );
	dummy = TKERR_FormatInt(" - got %d", 
		    (INT_type) TheValue->identifier_alignment );
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



INT_type cc_ITEMISATION_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type *destination;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;
    dummy = TKERR_StartErrMsg();
    dummy = TKERR_Format("[cc_ITEMISATION_Deflt]: can not get the iso 8613 default" );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}


INT_type at_DEFAULT_VALUE_LISTS_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type destination;
{

    INT_type rval;

    at_DEFAULT_VALUE_LISTS_type *Source;
    at_DEFAULT_VALUE_LISTS_type *Destination;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    Source = (at_DEFAULT_VALUE_LISTS_type *) source;
    Destination = (at_DEFAULT_VALUE_LISTS_type *) destination;

    Destination->page_attributes = Source->page_attributes;
    Destination->frame_attributes = Source->frame_attributes;
    Destination->block_attributes = Source->block_attributes;
    Destination->composite_logical_attributes = Source->composite_logical_attributes;
    Destination->basic_logical_attributes = Source->basic_logical_attributes;

    *destination_mask = source_mask;
    
LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type at_DEFAULT_VALUE_LISTS_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Malloc( (INT_type) sizeof( at_DEFAULT_VALUE_LISTS_type ) );
    if( rval == ERROR_POINTER ){
	/* ASSERT: TkError got set */
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[at_DEFAULT_VALUE_LISTS_Alloc]: can not get memory for a at_DEFAULT_VALUE_LISTS_type" );
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



INT_type at_DEFAULT_VALUE_LISTS_Free(storage)
	IN POINTER_type storage;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Free( storage );
    if( rval == ERROR_INT ){
	/* ASSERT: TKError got set */
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[at_DEFAULT_VALUE_LISTS_Free]: can not free the at_DEFAULT_VALUE_LISTS_type" );
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

 

INT_type at_DEFAULT_VALUE_LISTS_Set(AttrStructPtr, TheStorage)
	OUT ATTR_attribute_t AttrStructPtr;
	IN POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    AttrStructPtr->attribute_value.at_DEFAULT_VALUE_LISTS_field =
	(at_DEFAULT_VALUE_LISTS_type *) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type at_DEFAULT_VALUE_LISTS_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_at_DEFAULT_VALUE_LISTS( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type at_DEFAULT_VALUE_LISTS_CkVal(constituent, attribute, value, mask)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN POINTER_type value;
	IN PARM_MASK_type mask;
{


    /*
	Attributes with type at_DEFAULT_VALUE_LISTS_type:
	    at_DEFAULT_VALUE_LISTS
    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;
    
    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ) {
    
    case at_DEFAULT_VALUE_LISTS:
	if( !( CONST_IsComposite( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[cc_ITEMISATION_CkVal]: ");
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format(" must go on a composite" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;
    
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[at_DEFAULT_VALUE_LISTS_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " doesn't have at_DEFAULT_VALUE_LISTS_type" );
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



INT_type at_DEFAULT_VALUE_LISTS_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type *destination;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = INTERNAL_err;
    dummy = TKERR_StartErrMsg();
    dummy = TKERR_Format("[at_DEFAULT_VALUE_LISTS_Deflt]: can not get the iso 8613 default" );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type dp_DOC_APP_PROF_DEFAULTS_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type destination;
{

    INT_type rval;

    dp_DOC_APP_PROF_DEFAULTS_type *TheSource;
    dp_DOC_APP_PROF_DEFAULTS_type *TheDestination;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSource = (dp_DOC_APP_PROF_DEFAULTS_type *) source;
    TheDestination = (dp_DOC_APP_PROF_DEFAULTS_type *) destination;

    if( TheSource->content_arch_class != NULL_SEQUENCE ){
	SlaveRval = SEQ_CopySequenceElements( TheSource->content_arch_class,
			TheDestination->content_arch_class );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[dp_DOC_APP_PROF_DEFAULTS_Copy]: can not copy the content_arch_class" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    if( TheSource->content_type_specified == BOOL_true ){
	TheDestination->content_type = TheSource->content_type;
	TheDestination->content_type_specified = BOOL_true;
    }
    else {
	TheDestination->content_type_specified = BOOL_false;
    }

    if( TheSource->dimensions_specified == BOOL_true ){
	SlaveRval = at_DIMENSIONS_Copy( (POINTER_type) &TheSource->dimensions,
			TheSource->dimensions_parm_mask,
			/* wanted mask */ TheSource->dimensions_parm_mask,
			&TheDestination->dimensions_parm_mask,
			(POINTER_type) &TheDestination->dimensions );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[dp_DOC_APP_PROF_DEFAULTS_Copy]: can not copy the dimensions" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	TheDestination->dimensions_specified = BOOL_true;
	TheDestination->dimensions_parm_mask = TheSource->dimensions_parm_mask;
    }
    else {
	TheDestination->dimensions_specified = BOOL_false;
    }

    if( TheSource->transparency_specified == BOOL_true ){
	TheDestination->transparency = TheSource->transparency;
	TheDestination->transparency_specified = BOOL_true;
    }
    else {
	TheDestination->transparency_specified = BOOL_false;
    }

    if( TheSource->colour_specified == BOOL_true ){
	TheDestination->colour = TheSource->colour;
	TheDestination->colour_specified = BOOL_true;
    }
    else {
	TheDestination->colour_specified = BOOL_false;
    }

    if( TheSource->border_specified == BOOL_true ){
	SlaveRval = at_BORDER_Copy( (POINTER_type) &TheSource->border,
			TheSource->border_parm_mask,
			/* wanted mask */ TheSource->border_parm_mask,
			&TheDestination->border_parm_mask,
			(POINTER_type) &TheDestination->border );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[dp_DOC_APP_PROF_DEFAULTS_Copy]: can not copy the border" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	TheDestination->border_specified = BOOL_true;
	TheDestination->border_parm_mask = TheSource->border_parm_mask;
    }
    else {
	TheDestination->border_specified = BOOL_false;
    }

    if( TheSource->layout_path_specified == BOOL_true ){
	TheDestination->layout_path = TheSource->layout_path;
	TheDestination->layout_path_specified = BOOL_true;
    }
    else {
	TheDestination->layout_path_specified = BOOL_false;
    }

    if( TheSource->page_position_specified == BOOL_true ){
	TheDestination->page_position.first = TheSource->page_position.first;
	TheDestination->page_position.second = TheSource->page_position.second;
    	TheDestination->page_position_specified = BOOL_true;
    }
    else {
	TheDestination->page_position_specified = BOOL_false;
    }

    if( TheSource->medium_type_specified == BOOL_true ){
	SlaveRval = at_MEDIUM_TYPE_Copy( (POINTER_type) &TheSource->medium_type,
			TheSource->medium_type_parm_mask,
			/* wanted mask */ TheSource->medium_type_parm_mask,
			&TheDestination->medium_type_parm_mask,
			(POINTER_type) &TheDestination->medium_type );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[dp_DOC_APP_PROF_DEFAULTS_Copy]: can not copy the medium type" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	TheDestination->medium_type_specified = BOOL_true;
	TheDestination->medium_type_parm_mask = 
	    TheSource->medium_type_parm_mask;
    }
    else {
	TheDestination->medium_type_specified = BOOL_false;
    }

    if( TheSource->block_alignment_specified == BOOL_true ){
	TheDestination->block_alignment = TheSource->block_alignment;
	TheDestination->block_alignment_specified = BOOL_true;
    }
    else {
	TheDestination->block_alignment_specified = BOOL_false;
    }

    if( TheSource->type_of_coding != NULL_SEQUENCE ){
	SlaveRval = SEQ_CopySequenceElements( TheSource->type_of_coding,
			TheDestination->type_of_coding );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[dp_DOC_APP_PROF_DEFAULTS_Copy]: can not copy the type_of_coding" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    TheDestination->cc_presentation = TheSource->cc_presentation;
    TheDestination->cc_coding = TheSource->cc_coding;
    TheDestination->rg_presentation = TheSource->rg_presentation;
    TheDestination->rg_coding = TheSource->rg_coding;
    TheDestination->gg_presentation = TheSource->gg_presentation;
    TheDestination->gg_coding = TheSource->gg_coding;

    if( rval != ERROR_INT ){
	*destination_mask = source_mask;
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type dp_DOC_APP_PROF_DEFAULTS_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    POINTER_type SlaveRval;

    INT_type dummy;

    dp_DOC_APP_PROF_DEFAULTS_type *NewValue;

    dp_DOC_APP_PROF_DEFAULTS_type *TheSource;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Malloc( (INT_type) sizeof( dp_DOC_APP_PROF_DEFAULTS_type ) );
    if( rval == ERROR_POINTER ){
	rval = ERROR_POINTER;
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[dp_DOC_APP_PROF_DEFAULTS_Alloc]: can not get memory" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    NewValue = ( dp_DOC_APP_PROF_DEFAULTS_type *) rval;
    TheSource = (dp_DOC_APP_PROF_DEFAULTS_type *) value;

    if( TheSource->content_arch_class != NULL_SEQUENCE ){
	SlaveRval = SEQUENCE_INT_Alloc( (POINTER_type) TheSource->content_arch_class );
	if( SlaveRval == ERROR_POINTER ){
	    rval = ERROR_POINTER;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[dp_DOC_APP_PROF_DEFAULTS]: can not allocate the content_arch_class" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	NewValue->content_arch_class = (SEQUENCE_type) SlaveRval;
    }
    else {
	NewValue->content_arch_class = NULL_SEQUENCE;
    }

    if( TheSource->type_of_coding != NULL_SEQUENCE ){
	SlaveRval = SEQUENCE_INT_Alloc( (POINTER_type) TheSource->type_of_coding );
	if( SlaveRval == ERROR_POINTER ){
	    rval = ERROR_POINTER;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[dp_DOC_APP_PROF_DEFAULTS]: can not allocate the type_of_coding" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	NewValue->type_of_coding = (SEQUENCE_type) SlaveRval;
    }
    else {
	NewValue->type_of_coding = NULL_SEQUENCE;
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type dp_DOC_APP_PROF_DEFAULTS_Free(storage)
	IN POINTER_type storage;
{

    INT_type rval;

    dp_DOC_APP_PROF_DEFAULTS_type *TheValue;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif


    rval = SUCCESS_INT;

    TheValue = (dp_DOC_APP_PROF_DEFAULTS_type *) storage;

    if( TheValue->content_arch_class != NULL_SEQUENCE ){
	dummy = DeleteSequence( TheValue->content_arch_class );
    }

    if( TheValue->type_of_coding != NULL_SEQUENCE ){
	dummy = DeleteSequence( TheValue->type_of_coding );
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type dp_DOC_APP_PROF_DEFAULTS_Set(AttrStructPtr, TheStorage)
	OUT ATTR_attribute_t AttrStructPtr;
	IN POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    AttrStructPtr->attribute_value.dp_DOC_APP_PROF_DEFAULTS_field =
	(dp_DOC_APP_PROF_DEFAULTS_type *) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type dp_DOC_APP_PROF_DEFAULTS_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_dp_DOC_APP_PROF_DEFAULTS( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type dp_DOC_APP_PROF_DEFAULTS_CkVal(constituent, attribute, value, mask)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN POINTER_type value;
	IN PARM_MASK_type mask;
{

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){

    case dp_DOC_APP_PROF_DEFAULTS:
	if( !( CONST_IsDocProf( TheConstituent ) ) ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[dp_DOC_APP_PROF_DEFAULTS_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " must go on the document profile" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[dp_DOC_APP_PROF_DEFAULTS_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " doesn't have type dp_DOC_APP_PROF_DEFAULTS" );
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



INT_type dp_DOC_APP_PROF_DEFAULTS_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN PARM_MASK_type wanted_mask;
	OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type *destination;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = INTERNAL_err;
    dummy = TKERR_StartErrMsg();
    dummy = TKERR_Format("[dp_DOC_APP_PROF_DEFAULTS_Deflt]: attribute dp_DOC_APP_PROF_DEFAULTS has not iso 8613 default" );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}
