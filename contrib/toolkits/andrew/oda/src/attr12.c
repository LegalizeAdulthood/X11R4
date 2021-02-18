
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr12.c,v 1.3 89/11/01 15:46:33 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr12.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr12.c,v $
 * Revision 1.3  89/11/01  15:46:33  mss
 * Changed some casting and declarations for passing lint
 * 
 * Revision 1.2  89/10/31  13:08:19  jr
 * Lots of casting problems.
 * 
 * Revision 1.1  89/09/21  06:16:56  mss
 * Initial revision
 * 
 * Revision 1.14  89/09/12  09:04:23  mss
 * Removed #
 * 
 * Revision 1.13  89/03/09  15:50:26  annm
 * changes for new DAPD stuff
 * 
 * Revision 1.12  89/02/23  20:35:19  annm
 * changes for new DVL stuff
 * 
 * Revision 1.11  89/01/19  14:53:09  annm
 * primping the Check value code - tightened up restrictions on presentation attributes going on content dependent DVL/DAPD components. 
 * ALso tightened up on presentation attribute placement to account for the shared versus logical versus layout presentation attributes.
 * 
 * Revision 1.10  89/01/17  16:55:43  annm
 * changes for DVL defaulting
 * 
 * Revision 1.9  88/12/13  19:53:58  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.8  88/11/18  13:41:57  annm
 * no change
 * 
 * Revision 1.7  88/10/13  16:38:10  annm
 * minor fix in rg_PEl spacing ckval
 * 
 * Revision 1.6  88/09/30  15:05:02  annm
 * no change
 * 
 * Revision 1.5  88/09/21  15:45:38  annm
 * added ckval and free canonical procedures - also added the tkerr stuff
 * 
 * Revision 1.4  88/09/14  15:42:23  annm
 * added LINT to the ifndef's
 * 
 * Revision 1.3  88/09/12  12:59:12  annm
 * added support for rg_IMAGE_DIMENSIONS_type
 * 
 * Revision 1.2  88/09/08  15:34:21  annm
 * added support for rg_PEL_SPACING_type and rg_CLIPPING_type
 * 
 * Revision 1.1  88/09/07  18:34:40  annm
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr12.c,v $";
#endif

/* Put .c template comment here */

/*

    This file contains the canonical procedures fro the following
    aunion.h types;

	at_DIMENSIONS_type
	rg_CLIPPING_type
	rg_PEL_SPACING_type
	rg_IMAGE_DIMENSIONS_type

    At present, only the Copy, Alloc, Set, Get and iso default
    canonical procedures are implemented. The others
    are still in stubs.c
*/

#define	TK_IMPL_ATTR
#define TK_GVARS_ATTR12
#define TK_IMPL_CONST
#define TK_TKI_MM
#define TK_TKI_TKERR

#include <rnames.h>
#include <acd.h>
#include <mm.h>
#include <parms.h>
#include <tags.h>
#include <tkerr.h>
#include <attr0.h>
#include <values.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR12
#undef TK_IMPL_CONST
#undef TK_TKI_MM
#undef TK_TKI_TKERR


/* put #define's and declarations local to this file here */



INT_type at_DIMENSIONS_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    INT_type rval;

    at_DIMENSIONS_type *TheSource;
    at_DIMENSIONS_type *TheDestination;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    if( source_mask == NO_PARMS ){
	/* 
	    because of independent defaulting, we keep
	    attempting to inherit. Therefore, get out
	    right away, without updating the destination_mask.
	*/
	goto LEAVE;
    }

    TheSource = (at_DIMENSIONS_type *) source;
    TheDestination = (at_DIMENSIONS_type *) destination;

    if( source_mask & HORIZONTAL_DIMENSION_parm &&
	wanted_mask & HORIZONTAL_DIMENSION_parm &&
	!( *destination_mask & HORIZONTAL_DIMENSION_parm ) ){

	if( TheSource->horizontal_dimension_is_null == BOOL_false ){
	    rval = DIMENSION_type_Copy( &TheSource->horizontal_dimension,
		    &TheDestination->horizontal_dimension );
	    if( rval == ERROR_INT ){
		/* ASSERT: TKError got set */
		goto LEAVE;
	    }
	}

    	*destination_mask |= HORIZONTAL_DIMENSION_parm;
	TheDestination->horizontal_dimension_is_null = 
	    TheSource->horizontal_dimension_is_null;

    }

    if( source_mask & VERTICAL_DIMENSION_parm &&
	wanted_mask & VERTICAL_DIMENSION_parm &&
	!( *destination_mask & VERTICAL_DIMENSION_parm ) ){

	if( TheSource->vertical_dimension_is_null == BOOL_false ){
	    rval = DIMENSION_type_Copy( &TheSource->vertical_dimension,
		    &TheDestination->vertical_dimension );
	    if( rval == ERROR_INT ){
		/* ASSERT: TKError got set */		    
		goto LEAVE;
	    }
	}

	*destination_mask |= VERTICAL_DIMENSION_parm;
	TheDestination->vertical_dimension_is_null = 
	    TheSource->vertical_dimension_is_null;
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type DIMENSION_type_Copy( TheSource, TheDestination )
    IN DIMENSION_type *TheSource;
    OUT DIMENSION_type *TheDestination;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( TheSource->dimensions_tag ){

    case FIXED_DIMENSIONS_tag:
	TheDestination->dimensions_body.fixed_dimension =
	    TheSource->dimensions_body.fixed_dimension;
	break;

    case VARIABLE_PAGE_HEIGHT_tag:
	TheDestination->dimensions_body.variable_page_height =
	    TheSource->dimensions_body.variable_page_height;
	break;

    case RULE_A_tag:
    case RULE_B_tag:
	TheDestination->dimensions_body.rule.minimum_dimension =
	    TheSource->dimensions_body.rule.minimum_dimension;
	TheDestination->dimensions_body.rule.maximum_dimension =
	    TheSource->dimensions_body.rule.maximum_dimension;
	break;

    case MAX_SIZE_tag:
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt(
	    "[DIMENSION_type_Copy]: bad dimensions_tag of %d",
	    (INT_type) TheSource->dimensions_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheDestination->dimensions_tag = TheSource->dimensions_tag;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



POINTER_type at_DIMENSIONS_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Malloc( (INT_type) sizeof( at_DIMENSIONS_type ) );
    if( rval == ERROR_POINTER ){
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[at_DIMENSIONS_Alloc]: can not get memory" );
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



INT_type at_DIMENSIONS_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Free(storage);

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type at_DIMENSIONS_Set(AttrStructPtr, TheStorage)
	ATTR_attribute_t AttrStructPtr;
	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    AttrStructPtr->attribute_value.at_DIMENSIONS_field =
	(at_DIMENSIONS_type *) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type at_DIMENSIONS_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_at_DIMENSIONS( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type at_DIMENSIONS_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    at_DIMENSIONS_type *TheValue;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){
    
    case at_DIMENSIONS:
	if( !( CONST_IsPage( TheConstituent) ||
		CONST_IsFrame( TheConstituent ) ||
		CONST_IsBlock( TheConstituent ) ||
		CONST_IsDVLPage( TheConstituent ) ||
		CONST_IsDVLFrame( TheConstituent ) ||
		CONST_IsDVLBlock( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[at_DIMENSIONS_CkVal]: at_Dimensions must go on a page, frame or block" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
		"[at_DIMENSIONS_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" does not have at_DIMENSIONS_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheValue = (at_DIMENSIONS_type *) value;

    /* from this point on attribute is assumed to be at_DIMENSIONS */

    if( mask & HORIZONTAL_DIMENSION_parm ){
	
	if( TheValue->horizontal_dimension_is_null == BOOL_false ){

	    rval = DIMENSION_type_CkVal( &TheValue->horizontal_dimension );
	    if( rval == ERROR_INT ){
		/* ASSERT: TKError got set */
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format(
		    "[at_DIMENSIONS_CkVal]: The above error occurred in the horizontal dimension" );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	    }
	}
	else if (TheValue->horizontal_dimension_is_null != BOOL_true) {
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_FormatInt(
		"[at_DIMENSIONS_CkVal]: bad horizontal_dimension_is null - got %d",
		(INT_type) TheValue->horizontal_dimension_is_null);
	    dummy = TKERR_EndErrMsg();
	}
    }

    if( mask & VERTICAL_DIMENSION_parm ){
	
	if( TheValue->vertical_dimension_is_null == BOOL_false ){

	    rval = DIMENSION_type_CkVal( &TheValue->vertical_dimension );
	    if( rval == ERROR_INT ){
		/* ASSERT: TKError got set */
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format(
		    "[at_DIMENSIONS_CkVal]: The above error occurred in the vertical dimension" );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	    }
	}
	else if( TheValue->vertical_dimension_is_null != BOOL_true ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_FormatInt(
		"[at_DIMENSIONS_CkVal]: bad vertical_dimension_is null - got %d", 
		(INT_type) TheValue->vertical_dimension_is_null );
	    dummy = TKERR_EndErrMsg();
	}
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type DIMENSION_type_CkVal( TheDimension )
    IN DIMENSION_type *TheDimension;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( TheDimension->dimensions_tag ){

    case FIXED_DIMENSIONS_tag:
	if( TheDimension->dimensions_body.fixed_dimension <=
	    (INT_type) 0 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_FormatInt(
		"[DIMENSION_type_CkVal]: bad fixed dimensions value of %d",
		TheDimension->dimensions_body.fixed_dimension );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case VARIABLE_PAGE_HEIGHT_tag:
	/* Given that we DO NOT support basic pages this is an error */
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[DIMENSION_type_CkVal]: CMU ODA toolkit doesn't support basic pages so VARIABLE_PAGE_HEIGHT is illegal" );
	dummy = TKERR_EndErrMsg();
	break;

    case RULE_A_tag:
    case RULE_B_tag:
	if( TheDimension->dimensions_body.rule.minimum_dimension <=
	    (INT_type) 0 ||
	TheDimension->dimensions_body.rule.maximum_dimension <=
	    (INT_type) 0 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[DIMENSION_type_CkVal]: Bad rule a/b value -" );
	    dummy = TKERR_FormatInt(" got min = %d", 
		TheDimension->dimensions_body.rule.minimum_dimension );
	    dummy = TKERR_FormatInt(" and max = %d",
		TheDimension->dimensions_body.rule.maximum_dimension );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case MAX_SIZE_tag:
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt("[DIMENSION_type_CkVal]: Bad dimension tag of %d",
				(INT_type) (TheDimension->dimensions_tag) );
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



INT_type at_DIMENSIONS_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
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
    dummy = TKERR_Format("[at_DIMENSIONS_Deflt]: not implemented" );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type rg_CLIPPING_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    /*
	Parameters are related for purposes of defaulting.
    */

    INT_type rval;

    INT_type SlaveRval;

    rg_CLIPPING_type *TheSource;
    rg_CLIPPING_type *TheDestination;

    PARM_MASK_type DummyMask;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    if( source_mask == NO_PARMS ){
	/*
	    Because of defaulting with related parms, 
	    this stops the inheritance 
	*/
	*destination_mask = NO_PARMS;
	goto LEAVE;
    }

    TheSource = (rg_CLIPPING_type *) source;
    TheDestination  = (rg_CLIPPING_type *) destination;

    if( source_mask & COORD1_parm &&
	wanted_mask & COORD1_parm &&
	!(*destination_mask & COORD1_parm ) ){
	SlaveRval = INT_PAIR_Copy( (POINTER_type) &TheSource->coord1, 
			source_mask, wanted_mask, &DummyMask, 
			(POINTER_type) &TheDestination->coord1 );
	if( SlaveRval == ERROR_INT ){
	    /* ASSERT: TKError got set*/
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	*destination_mask |= COORD1_parm;
    }

    if( source_mask & COORD2_parm &&
	wanted_mask & COORD2_parm &&
	!(*destination_mask & COORD2_parm ) ){
	SlaveRval = INT_PAIR_Copy( (POINTER_type) &TheSource->coord2, 
			source_mask, wanted_mask, &DummyMask, 
			(POINTER_type) &TheDestination->coord2 );
	if( SlaveRval == ERROR_INT ){
	    /* ASSERT: TKError got set*/
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	*destination_mask |= COORD2_parm;
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type rg_CLIPPING_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Malloc( (INT_type) sizeof( rg_CLIPPING_type ) );
    if( rval == ERROR_POINTER ){
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[rg_CLIPPING_Alloc]: can not get memory" );
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



INT_type rg_CLIPPING_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Free(storage);
    
LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type rg_CLIPPING_Set(AttrStructPtr, TheStorage)
	ATTR_attribute_t AttrStructPtr;
	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    AttrStructPtr->attribute_value.rg_CLIPPING_field =
	    (rg_CLIPPING_type *) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type rg_CLIPPING_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_rg_CLIPPING( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type rg_CLIPPING_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    rg_CLIPPING_type *TheValue;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){

    case rg_CLIPPING:
	if( !( CONST_IsBlock( TheConstituent ) ||
		CONST_IsPresentStyle( TheConstituent ) ||
		CONST_IsDVLBlock( TheConstituent )  ||
		CONST_IsDAPDBlock( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[rg_CLIPPING_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format(" must go on a block or a presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[rg_CLIPPING_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" does not have rg_CLIPPING_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    TheValue = (rg_CLIPPING_type *) value;

    switch( attribute ){
    
    case rg_CLIPPING:

	if( mask & COORD1_parm ){
	    if(  TheValue->coord1.first < (INT_type) 0 ||
		TheValue->coord1.second < (INT_type) 0 ){
		rval = ERROR_INT;
		TKError = BADPARM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt(
		    "[rg_CLIPPING_CkVal]: bad rg_CLIPPING value = got coord1.first = %d",
		    TheValue->coord1.first );
		dummy = TKERR_FormatInt( " coord1.second = %d", 
		    TheValue->coord1.second );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	    }
	}

	if( mask & COORD2_parm ){
	    if( TheValue->coord2.first < (INT_type) 0  ||
		TheValue->coord2.second < (INT_type) 0 ){
		rval = ERROR_INT;
		TKError = BADPARM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt( " coord2.first = %d", 
		    TheValue->coord2.first );
		dummy = TKERR_FormatInt( " coord2.second = %d", 
		    TheValue->coord2.second );
		dummy = TKERR_EndErrMsg();
	    }
	}
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



INT_type rg_CLIPPING_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
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
    dummy = TKERR_Format("[rg_CLIPPING_Deflt]: not implemented" );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type rg_PEL_SPACING_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    INT_type rval;

    rg_PEL_SPACING_type *TheSource;
    rg_PEL_SPACING_type *TheDestination;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSource = (rg_PEL_SPACING_type *) source;
    TheDestination = (rg_PEL_SPACING_type *) destination;

    if( TheSource->null == BOOL_true ){
	TheDestination->null = BOOL_true;
    }
    else {
	TheDestination->length = TheSource->length;
	TheDestination->pel_spaces = TheSource->pel_spaces;
    }

    *destination_mask = source_mask;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type rg_PEL_SPACING_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Malloc( (INT_type) sizeof( rg_PEL_SPACING_type ) );
    if( rval == ERROR_POINTER ){
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[rg_PEL_SPACING_Alloc]: can not get memory" );
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



INT_type rg_PEL_SPACING_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Free(storage);

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type rg_PEL_SPACING_Set(AttrStructPtr, TheStorage)
	ATTR_attribute_t AttrStructPtr;
	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    AttrStructPtr->attribute_value.rg_PEL_SPACING_field =
	(rg_PEL_SPACING_type *) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type rg_PEL_SPACING_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_rg_PEL_SPACING( AttrStructPtr );;
  
LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type rg_PEL_SPACING_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    rg_PEL_SPACING_type *TheValue;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){

    case rg_PEL_SPACING:
	if( !( CONST_IsPresentStyle( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[rg_PEL_SPACING_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format(" must go on a block or a presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[rg_PEL_SPACING_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" does not have rg_PEL_SPACING_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    TheValue = (rg_PEL_SPACING_type *) value;

    switch( attribute ){
    
    case rg_PEL_SPACING:
	if( TheValue->null == BOOL_false ){
	    if( TheValue->length <= (INT_type) 0 ||
	      TheValue->pel_spaces <= (INT_type) 0 ) {
		rval = ERROR_INT;
		TKError = BADPARM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_FormatInt(
		    "[rg_PEL_SPACING_CkVal]: bad rg_PEL_SPACING value = got length = %d",
		    TheValue->length );

		dummy = TKERR_FormatInt( " and pel spaces = %d", 
		    TheValue->pel_spaces );
		dummy = TKERR_EndErrMsg();
	      }
	}
	else if( TheValue->null != BOOL_true ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[rg_PEL_SPACING_CkVal]: bad rg_PEL_SPACING value - null field is neither BOOL_true or BOOL_false" );
	    dummy = TKERR_EndErrMsg();
	}
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



INT_type rg_PEL_SPACING_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
{

    INT_type rval;

    rg_PEL_SPACING_type *TheDestination;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    if( *destination == (POINTER_type) 0 ){
	*destination = MM_Malloc( (INT_type) sizeof( rg_PEL_SPACING_type ) );
	if( *destination == ERROR_POINTER ){
	    rval = ERROR_INT;
	    TKError = NOMEM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[rg_PEL_SPACING_Deflt]: can not get memory" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }
    else {
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[rg_PEL_SPACING_Deflt]: have memory and shouldn't" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheDestination = (rg_PEL_SPACING_type *) *destination;

    TheDestination->null = BOOL_false;
    TheDestination->length = (INT_type) 4;
    TheDestination->pel_spaces = (INT_type) 1;

    *destination_mask = wanted_mask;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type rg_IMAGE_DIMENSIONS_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    INT_type rval;

    rg_IMAGE_DIMENSIONS_type *TheSource;
    rg_IMAGE_DIMENSIONS_type *TheDestination;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSource = (rg_IMAGE_DIMENSIONS_type *) source;
    TheDestination = (rg_IMAGE_DIMENSIONS_type *) destination;

    switch( TheSource->image_dimensions_tag ){
    
    case WIDTH_CONTROLLED_tag:
	TheDestination->image_dimensions_value.width_controlled.first =
	TheSource->image_dimensions_value.width_controlled.first;

	TheDestination->image_dimensions_value.width_controlled.second =
	TheSource->image_dimensions_value.width_controlled.second;
	break;
    
    case HEIGHT_CONTROLLED_tag:
	TheDestination->image_dimensions_value.height_controlled.first =
	TheSource->image_dimensions_value.height_controlled.first;

	TheDestination->image_dimensions_value.height_controlled.second =
	TheSource->image_dimensions_value.height_controlled.second;
	break;

    case AREA_CONTROLLED_tag:
	TheDestination->image_dimensions_value.area_controlled.minimum_width =
	TheSource->image_dimensions_value.area_controlled.minimum_width;

	TheDestination->image_dimensions_value.area_controlled.preferred_width =
	TheSource->image_dimensions_value.area_controlled.preferred_width;

	TheDestination->image_dimensions_value.area_controlled.minimum_height =
	TheSource->image_dimensions_value.area_controlled.minimum_height;

	TheDestination->image_dimensions_value.area_controlled.preferred_height =
	TheSource->image_dimensions_value.area_controlled.preferred_height;
	

	TheDestination->image_dimensions_value.area_controlled.aspect_ratio_flag =
	TheSource->image_dimensions_value.area_controlled.aspect_ratio_flag;
	break;

    case AUTOMATIC_tag:
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt(
	    "[rg_IMAGE_DIMENSIONS_Copy]: bad image_dimensions_tag of %d",
	    (INT_type) TheSource->image_dimensions_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
	
    }

    TheDestination->image_dimensions_tag = TheSource->image_dimensions_tag;

    *destination_mask = source_mask;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type rg_IMAGE_DIMENSIONS_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Malloc( (INT_type) sizeof( rg_IMAGE_DIMENSIONS_type ) );
    if( rval == ERROR_POINTER ){
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[rg_IMAGE_DIMENSIONS_Alloc]: Can not get memory" );
	dummy = TKERR_EndErrMsg();
    }


LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type rg_IMAGE_DIMENSIONS_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Free(storage);

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type rg_IMAGE_DIMENSIONS_Set(AttrStructPtr, TheStorage)
	ATTR_attribute_t AttrStructPtr;
	POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    AttrStructPtr->attribute_value.rg_IMAGE_DIMENSIONS_field =
	(rg_IMAGE_DIMENSIONS_type *) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type rg_IMAGE_DIMENSIONS_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_rg_IMAGE_DIMENSIONS( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type rg_IMAGE_DIMENSIONS_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    INT_type rval;
    INTERNAL_CONSTITUENT TheConstituent;

    rg_IMAGE_DIMENSIONS_type *TheValue;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){

    case rg_IMAGE_DIMENSIONS:
	if( !( CONST_IsPresentStyle( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[rg_IMAGE_DIMENSIONS_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format(" must go on a block or a presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[rg_IMAGE_DIMENSIONS_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" does not have rg_IMAGE_DIMENSIONS_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    TheValue = (rg_IMAGE_DIMENSIONS_type *) value;

    /* from here on code assumes that attribute is rg_IMAGE_DIMENSIONS */

    switch( TheValue->image_dimensions_tag ){
    
    case WIDTH_CONTROLLED_tag:
	if( TheValue->image_dimensions_value.width_controlled.first <
		(INT_type) 0 ||
	    TheValue->image_dimensions_value.width_controlled.second <
		(INT_type) 0 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_FormatInt("[rg_IMAGE_DIMENSIONS_CkVal]: bad width controled value - got first = %d",
		TheValue->image_dimensions_value.width_controlled.first );
	    dummy = TKERR_FormatInt(" and second = %d",
		TheValue->image_dimensions_value.width_controlled.second );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;
    
    case HEIGHT_CONTROLLED_tag:
	if( TheValue->image_dimensions_value.height_controlled.first <
		(INT_type) 0 ||
	    TheValue->image_dimensions_value.height_controlled.second <
		(INT_type) 0 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_FormatInt("[rg_IMAGE_DIMENSIONS_CkVal]: bad height controled value - got first = %d",
		TheValue->image_dimensions_value.height_controlled.first );
	    dummy = TKERR_FormatInt(" and second = %d",
		TheValue->image_dimensions_value.height_controlled.second );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case AREA_CONTROLLED_tag:
	if( TheValue->image_dimensions_value.area_controlled.minimum_width <
		(INT_type) 0 ||
	    TheValue->image_dimensions_value.area_controlled.preferred_width <
		(INT_type) 0 ||
	    TheValue->image_dimensions_value.area_controlled.minimum_height <
		(INT_type) 0 ||
	    TheValue->image_dimensions_value.area_controlled.preferred_height <
		(INT_type) 0 ||
	    ( TheValue->image_dimensions_value.area_controlled.aspect_ratio_flag
		!= rg_IMAGE_DIMENSIONS_fixed &&
	      TheValue->image_dimensions_value.area_controlled.aspect_ratio_flag
		!= rg_IMAGE_DIMENSIONS_variable ) ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_FormatInt("[rg_IMAGE_DIMENSIONS_CkVal]: bad area controlled value - got minimum width = %d",
		TheValue->image_dimensions_value.area_controlled.minimum_width );
	    dummy = TKERR_FormatInt(" preferred width  = %d",
		TheValue->image_dimensions_value.area_controlled.preferred_width );
	    dummy = TKERR_FormatInt(" minimum height   = %d",
		TheValue->image_dimensions_value.area_controlled.minimum_height );
	    dummy = TKERR_FormatInt(" preferred height  = %d",
		TheValue->image_dimensions_value.area_controlled.preferred_height );
	    dummy = TKERR_FormatInt(" and aspect ratio flag of %d",
		(INT_type) 
		TheValue->image_dimensions_value.area_controlled.aspect_ratio_flag );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case AUTOMATIC_tag:
	/* there is nothing to check */
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt(
	    "[rg_IMAGE_DIMENSIONS_CkVal]: bad image_dimensions_tag of %d",
	    (INT_type) TheValue->image_dimensions_tag );
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



INT_type rg_IMAGE_DIMENSIONS_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
{

    INT_type rval;

    rg_IMAGE_DIMENSIONS_type *TheDestination;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    if( *destination == (POINTER_type) 0 ){
	*destination = MM_Malloc( (INT_type) sizeof( rg_IMAGE_DIMENSIONS_type ) );
	if( *destination == ERROR_POINTER ){
	    rval = ERROR_INT;
	    TKError = NOMEM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[rg_IMAGE_DIEMNSIONS_Deflt]: can not get memory" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	TheDestination = (rg_IMAGE_DIMENSIONS_type *) *destination;
	TheDestination->image_dimensions_tag = AUTOMATIC_tag;
    
    }
    else {
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[rg_IMAGE_DIMENSIONS_Deflt]: have memory and shouldn't" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    *destination_mask = wanted_mask;    

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}
