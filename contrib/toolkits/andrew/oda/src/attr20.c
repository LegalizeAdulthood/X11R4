
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr20.c,v 1.2 89/10/31 13:37:48 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr20.c,v $

(c) Copyright 1988 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	attr20.c,v $
 * Revision 1.2  89/10/31  13:37:48  jr
 * Casting problems.
 * 
 * Revision 1.1  89/09/21  06:19:52  mss
 * Initial revision
 * 
 * Revision 1.6  89/09/12  09:06:39  mss
 * Removed #
 * 
 * Revision 1.5  89/04/21  18:15:05  annm
 * chnages for deleteseq/deletesequencedeeply
 * 
 * Revision 1.4  89/03/09  15:50:39  annm
 * no change
 * 
 * Revision 1.3  89/02/23  20:37:57  annm
 * changes for new DVL stuff
 * 
 * Revision 1.2  89/01/26  18:18:28  annm
 * added canonical procedures for SEQUENCE_ASN1_OBJECT_ID_type
 * 
 * Revision 1.1  89/01/26  12:37:53  annm
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr20.c,v $";
#endif



#define TK_IMPL_ATTR
#define TK_GVARS_ATTR20
#define TK_TKI_TKERR
#define TK_TKI_SEQ
#define TK_IMPL_CONST
#define TK_TKI_MM

#include <rnames.h>
#include <acd.h>
#include <tkerr.h>
#include <seq.h>
#include <const.h>
#include <mm.h>
#include <values.h>
#include <init.h>

#include <attr0.h>
#include <parms.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR20
#undef TK_TKI_TKERR
#undef TK_TKI_SEQ
#undef TK_IMPL_CONST
#undef TK_TKI_MM

/*
    This file contains the canonical aunion.h operations for the following 
	types:

	SEQUENCE_VALUE_type
	at_MEDIUM_TYPE_type
	SEQUENCE_at_MEDIUM_TYPE_type
	SEQUENCE_ASN1_OBJECT_ID_type


*/



INT_type SEQUENCE_VALUE_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	IN POINTER_type destination;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    /*
	The source mask is ignored.

	The source is a sequence type.
    
	The destination is a sequence type.
    */

    rval = SEQ_CopySequenceElements( (SEQUENCE_type) source,
	    (SEQUENCE_type) destination );

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



POINTER_type SEQUENCE_VALUE_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    SEQUENCE_type TheSource;
    SEQUENCE_type TheSequence;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    TheSource = (SEQUENCE_type) value;
    if( TheSource->sequence_tag != SEQUENCE_VALUE_tag ){
	rval = ERROR_POINTER;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_VALUE_Alloc]: " );
	dummy = TKERR_Format( " value doesn't have SEQUENCE_VALUE_tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheSequence = MakeSequence( SEQUENCE_VALUE_tag, 
		TheSource->length );
    if( TheSequence == ERROR_SEQUENCE ){
	rval = ERROR_POINTER;
	goto LEAVE;
    }

    rval = (POINTER_type) TheSequence;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_VALUE_Free(storage)
	IN POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = DeleteSequence( (SEQUENCE_type) storage );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_VALUE_Set(AttrStructPtr, TheStorage)
	IN ATTR_attribute_t	AttrStructPtr;
	IN POINTER_type		TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    AttrStructPtr->attribute_value.SEQUENCE_VALUE_field =
	(SEQUENCE_VALUE_type) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type SEQUENCE_VALUE_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_SEQUENCE_INT( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_VALUE_CkVal(constituent, attribute, value, mask)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN POINTER_type value;
	IN PARM_MASK_type mask;
{

    /*
	Attributes that have type SEQUENCE_VALUE_type:
	    dp_LAYOUT_PATHS
	    dp_PROTECTIONS
	    dp_BLOCK_ALIGNMENTS
	    dp_FILL_ORDERS
	    dp_TRANSPARENCIES
	    dp_COLOURS
    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    SEQUENCE_type TheSequence;

    INT_type i;

    VALUE_type TheValue;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;
    
    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ) {

    case dp_LAYOUT_PATHS:
    case dp_PROTECTIONS:
    case dp_BLOCK_ALIGNMENTS:
    case dp_FILL_ORDERS:
    case dp_TRANSPARENCIES:
    case dp_COLOURS:
	if( !( CONST_IsDocProf( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[SEQUENCE_VALUE_CkVal]: ");
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format(" must go on the document profile" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;
    
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[SEQUENCE_INT_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " doesn't have SEQUENCE_CONSTITUENT_type" );
	dummy = TKERR_EndErrMsg();	
	goto LEAVE;
    }

    TheSequence = (SEQUENCE_type) value;

    if( TheSequence->sequence_tag != SEQUENCE_VALUE_tag ) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_VALUE_CkVal]: " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " value must be a sequence of INT_type's" );
	dummy = TKERR_EndErrMsg();
    }   

    for( i = (INT_type) 0; i < TheSequence->length; i++ ){

	TheValue = TheSequence->sequence_value.values[i];

	switch( attribute ){

	case dp_LAYOUT_PATHS:
	    if( TheValue != at_LAYOUT_PATH_type_0 &&
		TheValue != at_LAYOUT_PATH_type_90 &&
		TheValue != at_LAYOUT_PATH_type_180 &&
		TheValue != at_LAYOUT_PATH_type_270 ){
		rval = ERROR_INT;
		TKError = BADPARM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format( "[SEQUENCE_VALUE_CkVal]: Sequence element value of attribute " );
		dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
		dummy = TKERR_FormatInt( " at position %d", i );
		dummy = TKERR_FormatInt( 
		    " is %d which is illegal", 
		    (INT_type) TheValue );
		dummy = TKERR_EndErrMsg();
	    }
	    break;

	case dp_PROTECTIONS:
	    if( TheValue != at_PROTECTION_protected &&
		TheValue != at_PROTECTION_unprotected ){
		rval = ERROR_INT;
		TKError = BADPARM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format( "[SEQUENCE_VALUE_CkVal]: Value of attribute " );
		dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
		dummy = TKERR_FormatInt( "at position %d", i );
		dummy = TKERR_FormatInt( 
		    " is %d which is illegal", 
		    (INT_type) TheValue );
		dummy = TKERR_EndErrMsg();
	    }
	    break;

	case dp_BLOCK_ALIGNMENTS:
	    if( TheValue != at_BLK_ALGNM_right_hand_aligned &&
		TheValue != at_BLK_ALGNM_left_hand_aligned &&
		TheValue != at_BLK_ALGNM_centred &&
		TheValue != at_BLK_ALGNM_null ){
		rval = ERROR_INT;
		TKError = BADPARM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format( "[SEQUENCE_VALUE_CkVal]: Value of attribute " );
		dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
		dummy = TKERR_FormatInt( " at position %d", i );
		dummy = TKERR_FormatInt( 
		    " is %d which is illegal", 
		    (INT_type) TheValue );
		dummy = TKERR_EndErrMsg();
	    }
	    break;

	case dp_FILL_ORDERS:
	    if( TheValue != at_FILL_ORDER_normal_order &&
		TheValue != at_FILL_ORDER_reverse_order ){
		rval = ERROR_INT;
		TKError = BADPARM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format( "[SEQUENCE_VALUE_CkVal]: Value of attribute " );
		dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
		dummy = TKERR_FormatInt( " at position %d", i);
		dummy = TKERR_FormatInt( 
		    " is %d which is illegal", 
		    (INT_type) TheValue );
		dummy = TKERR_EndErrMsg();
	    }
	    break;

	case dp_TRANSPARENCIES:
	    if( TheValue != at_TRANSPARENCY_transparent &&
		TheValue != at_TRANSPARENCY_opaque ){
		rval = ERROR_INT;
		TKError = BADPARM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format( "[SEQUENCE_VALUE_CkVal]: Value of attribute " );
		dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
		dummy = TKERR_FormatInt( " at position %d", i);
		dummy = TKERR_FormatInt( 
		    " is %d which is illegal", 
		    (INT_type) TheValue );
		dummy = TKERR_EndErrMsg();
	    }
	    break;

	case dp_COLOURS:
	    if( TheValue != at_COLOUR_colourless &&
		TheValue != at_COLOUR_white ){
		rval = ERROR_INT;
		TKError = BADPARM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format( "[SEQUENCE_VALUE_CkVal]: Sequence element value of attribute " );
		dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
		dummy = TKERR_FormatInt( " at position %d", i );
		dummy = TKERR_FormatInt( 
		    " is %d which is illegal", 
		    (INT_type) TheValue );
		dummy = TKERR_EndErrMsg();
	    }	    
	    break;

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



INT_type SEQUENCE_VALUE_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
{

    /*
	Attributes that have type SEQUENCE_VALUE_type:
	    dp_LAYOUT_PATHS
	    dp_PROTECTIONS
	    dp_BLOCK_ALIGNMENTS
	    dp_FILL_ORDERS
	    dp_TRANSPARENCIES
	    dp_COLOURS
    */


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
    dummy = TKERR_Format("[SEQUENCE_VALUE_Deflt]: Should never try to get the iso 8613 default value" );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type at_MEDIUM_TYPE_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	IN POINTER_type destination;
{

    /*
	Medium type has two parameters: NOMINAL_PAGE_SIZE and 
	    SIDE_OF_SHEET.

	The parameters are INDEPENDENTLY DEFAULTABLE.
    */

    INT_type rval;

    at_MEDIUM_TYPE_type	*TheSource;
    at_MEDIUM_TYPE_type *TheDestination;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    if( source_mask == NO_PARMS ){
	/*
	    Because of independent defaulting, we keep 
	    attempting to inherit. Therefore, get out 
	    right away without updating destination_mask.
	*/
	goto LEAVE;
    }

    TheSource = (at_MEDIUM_TYPE_type *) source;
    TheDestination = (at_MEDIUM_TYPE_type *) destination;

    if( source_mask & NOMINAL_PAGE_SIZE_parm &&
	wanted_mask & NOMINAL_PAGE_SIZE_parm &&
	!( *destination_mask & NOMINAL_PAGE_SIZE_parm ) ){
	TheDestination->nominal_page_size.first =
	    TheSource->nominal_page_size.first;
	TheDestination->nominal_page_size.second =
	    TheSource->nominal_page_size.second;
	*destination_mask |= NOMINAL_PAGE_SIZE_parm;
    }

    if( source_mask & SIDE_OF_SHEET_parm &&
	wanted_mask & SIDE_OF_SHEET_parm &&
	!( *destination_mask & SIDE_OF_SHEET_parm ) ){
	TheDestination->side_of_sheet = TheSource->side_of_sheet;
	*destination_mask |= SIDE_OF_SHEET_parm;
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type at_MEDIUM_TYPE_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Malloc( (INT_type) sizeof( at_MEDIUM_TYPE_type ) );
    if( rval == ERROR_POINTER ){
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[at_MEDIUM_TYPE_Alloc]: Can not get memory" );
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



INT_type at_MEDIUM_TYPE_Free(storage)
	IN POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Free( storage );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type at_MEDIUM_TYPE_Set(AttrStructPtr, TheStorage)
	IN ATTR_attribute_t AttrStructPtr;
	IN POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    AttrStructPtr->attribute_value.at_MEDIUM_TYPE_field =
	(at_MEDIUM_TYPE_type *) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type at_MEDIUM_TYPE_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_at_MEDIUM_TYPE( AttrStructPtr );


LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type at_MEDIUM_TYPE_CkVal(constituent, attribute, value, mask)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN POINTER_type value;
	IN PARM_MASK_type mask;
{

    /*
	Attribute with type at_MEDIUM_TYPE_type:
	    at_MEDIUM_TYPE
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

    switch( attribute ){

    case at_MEDIUM_TYPE:
	if( !( CONST_IsPage( TheConstituent ) ||
		CONST_IsDVLPage( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[at_MEDIUM_TYPE_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a page" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[at_MEDIUM_TYPE_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " doesn't have at_MEDIUM_TYPE_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    rval = ATTR_Check_at_MEDIUM_TYPE( (at_MEDIUM_TYPE_type *) value,
	    mask, attribute );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}

INT_type ATTR_Check_at_MEDIUM_TYPE( TheMediumType, mask, attribute )
    IN at_MEDIUM_TYPE_type *TheMediumType;
    IN PARM_MASK_type mask;
    IN INT_type attribute;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    /* nominal page size must be a pair of positive integers */
    if( 
	(
	    ( mask & NOMINAL_PAGE_SIZE_parm ) &&
	    TheMediumType->nominal_page_size.first <= (INT_type) 0
	) ||
	(
	    ( mask & NOMINAL_PAGE_SIZE_parm ) &&
	    TheMediumType->nominal_page_size.second <= (INT_type) 0
	) 
    ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[ATTR_Check_at_MEDIUM_TYPE]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " - nominal page size must be a pair of positive ints" );
	dummy = TKERR_EndErrMsg();	
    }

    if(
	(
	    ( mask & SIDE_OF_SHEET_parm ) &&
	    TheMediumType->side_of_sheet != at_MEDIUM_TYPE_recto &&
	    TheMediumType->side_of_sheet != at_MEDIUM_TYPE_verso &&
	    TheMediumType->side_of_sheet != at_MEDIUM_TYPE_unspecified
	)
    ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[ATTR_Check_at_MEDIUM_TYPE]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_FormatInt( " - got bad side of sheet = %d", 
	    (INT_type) TheMediumType->side_of_sheet );
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



INT_type at_MEDIUM_TYPE_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN PARM_MASK_type wanted_mask;
	IN PARM_MASK_type *destination_mask;
	IN POINTER_type *destination;
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
    dummy = TKERR_Format("[at_MEDIUM_TYPE_Deflt]: iso 8613 default code unimplemented" );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_at_MEDIUM_TYPE_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	IN POINTER_type destination;
{

    /*
	The source mask is ignored.

	The source is a sequence type.

	The destination is a sequence type.

	ASSERT: wanted_mask is ALL_PARMS.
    */

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SEQ_CopySequenceElements( (SEQUENCE_type) source,
	    (SEQUENCE_type) destination );

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



POINTER_type SEQUENCE_at_MEDIUM_TYPE_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    SEQUENCE_type TheSequence;
    SEQUENCE_type TheSource;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    TheSource = (SEQUENCE_type) value;

    if( TheSource->sequence_tag != SEQUENCE_at_MEDIUM_TYPE_tag ){
	rval = ERROR_POINTER;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[SEQUENCE_at_MEDIUM_TYPE_Alloc]: value isn't SEQUENCE_at_MEDIUM_TYPE" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheSequence = MakeSequence( SEQUENCE_at_MEDIUM_TYPE_tag,
		    TheSource->length );
    if( TheSequence == ERROR_SEQUENCE ){
	rval = ERROR_POINTER;
	goto LEAVE;
    }

    rval = (POINTER_type) TheSequence;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_at_MEDIUM_TYPE_Free(storage)
	IN POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = DeleteSequence( (SEQUENCE_type) storage );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_at_MEDIUM_TYPE_Set(AttrStructPtr, TheStorage)
	IN ATTR_attribute_t AttrStructPtr;
	IN POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    AttrStructPtr->attribute_value.SEQUENCE_at_MEDIUM_TYPE_field =
	(SEQUENCE_at_MEDIUM_TYPE_type) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type SEQUENCE_at_MEDIUM_TYPE_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_SEQUENCE_at_MEDIUM_TYPE( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_at_MEDIUM_TYPE_CkVal(constituent, attribute, value, mask)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN POINTER_type value;
	IN PARM_MASK_type mask;
{

    /*
	Attributes with type SEQUENCE_at_MEDIUM_TYPE:
	    dp_MEDIUM_TYPES
    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    SEQUENCE_type TheSequence;

    INT_type i;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ) {

    case dp_MEDIUM_TYPES:
	if( !( CONST_IsDocProf( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[SEQUENCE_at_MEDIUM_TYPE_CkVal]: dp_MEDIUM_TYPES must go on the document profile" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[SEQUENCE_at_MEDIUM_TYPE_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " does not have type SEQUENCE_ANS1_OBJECT_ID_type" );
	dummy = TKERR_EndErrMsg();	
	goto LEAVE;
    }

    TheSequence = (SEQUENCE_type) value;

    if( TheSequence->sequence_tag != SEQUENCE_at_MEDIUM_TYPE_tag ) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_at_MEDIUM_TYPE_CkVal]: " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " value must be a sequence of at_MEIDUM_TYPE's" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }   

    for( i = (INT_type) 0; i < TheSequence->length; i++ ){

	rval = ATTR_Check_at_MEDIUM_TYPE( 
		(at_MEDIUM_TYPE_type *) &TheSequence->sequence_value.medium_types[i], 
		NOMINAL_PAGE_SIZE_parm | SIDE_OF_SHEET_parm, attribute );
	if( rval == ERROR_INT ){
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_FormatInt("[SEQUENCE_at_MEDIUM_TYPES_CkVal]: the above error occurred while checking sequence element %d", i );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
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



INT_type SEQUENCE_at_MEDIUM_TYPE_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN PARM_MASK_type wanted_mask;
	IN PARM_MASK_type *destination_mask;
	IN POINTER_type *destination;
{

    /*
	Attributes with type SEQUENCE_at_MEDIUM_TYPE_type:

	    dp_MEDIUM_TYPES (DOES NOT HAVE A DEFAULT VALUE !!!)
    */

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
    dummy = TKERR_Format("[SEQUENCE_at_MEDUIM_TYPE_Deflt]: can not get default value" );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_ASN1_OBJECT_ID_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	IN POINTER_type destination;
{

    /*
	The source mask is ignored.

	The source is a sequence type;

	The destination is a sequence type.

	ASSERT: wanted_mask is ALL_PARMS.
    */

    /*
	The astute code reader (<sic!>) might expect this routine
	to merely call SEQ_CopySequenceElements...

	but it can't to prevent a core leak.

	Basically, when SEQ_CopySequenceElements is called when 
	    doing a copy sequence: in this case the top level
	    copy sequence code will alloc the top level sequence
	    ( the sequence of asn1 object ids),
	    so the copy sequence elements loop must alloc the
	    contained sequence of int's

	If this routine merely called copy sequence elements, then the
	    subordinate sequence of ints would be allocated for a second
	    time as set/get attr has already called 
	    SEQUENCE_ASN1_OBJECT_ID_Alloc, which does a deep alloc, 
	    meaning that the contained sequence's of ints have already
	    been alloc'ed.

	So... after the obvious (redundant) check for sequence type, the
	routine loops copying each contained sequence of ints. The
	copy of these contained sequences of ints can use copy sequence
	elements as NO allocation is done for a sequence of ints.

	Whew !!! 
    */

    INT_type rval;

    SEQUENCE_type Source;
    SEQUENCE_type Destination;

    SEQUENCE_type SourceASN1Id;
    SEQUENCE_type DestinationASN1Id;

    INT_type i;

    INT_type dummy;

    INT_type SlaveRval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    Source = (SEQUENCE_type) source;

    if( Source->sequence_tag != SEQUENCE_ASN1_OBJECT_ID_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_ASN1_OBJECT_ID_Copy]: source not a sequence of asn1 object id's" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    Destination = (SEQUENCE_type) destination;

    if( Destination->sequence_tag != SEQUENCE_ASN1_OBJECT_ID_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_ASN1_OBJECT_ID_Copy]: destination not a sequence of asn1 object id's" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < Source->length; i++ ){

	SourceASN1Id = (SEQUENCE_type) 
	    Source->sequence_value.asn1_object_ids[i];
	DestinationASN1Id = (SEQUENCE_type)
	    Destination->sequence_value.asn1_object_ids[i];

	if( SourceASN1Id->sequence_tag != SEQUENCE_INT_tag ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_FormatInt("[SEQUENCE_ASN1_OBJECT_ID_Copy]: The %d-th source asn1 object id is not a sequence of ints", i );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	if( DestinationASN1Id->sequence_tag != SEQUENCE_INT_tag ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_FormatInt("[SEQUENCE_ASN1_OBJECT_ID_Copy]: The %d-th destination asn1 object id is not a sequence of ints", i );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	SlaveRval = SEQ_CopySequenceElements( SourceASN1Id, DestinationASN1Id );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    goto LEAVE;
	}

    }
    
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



POINTER_type SEQUENCE_ASN1_OBJECT_ID_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    SEQUENCE_type TheSequence;
    SEQUENCE_type TheSource;

    INT_type dummy;

    INT_type i;
    INT_type SlaveRval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    TheSource = (SEQUENCE_type) value;

    if( TheSource->sequence_tag != SEQUENCE_ASN1_OBJECT_ID_tag ){
	rval = ERROR_POINTER;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[SEQUENCE_ASN1_OBJECT_ID_Alloc]: value isn't SEQUENCE_ASN1_OBJECT_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheSequence = MakeSequence( SEQUENCE_ASN1_OBJECT_ID_tag,
		    TheSource->length );
    if( TheSequence == ERROR_SEQUENCE ){
	rval = ERROR_POINTER;
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < TheSource->length; i++ ){

	SlaveRval = ATTR_AllocASN1ObjectId( 
	    (ASN1_OBJECT_ID_type)
	    TheSource->sequence_value.asn1_object_ids[i],

	    (ASN1_OBJECT_ID_type *)
	    &TheSequence->sequence_value.asn1_object_ids[i] 
	);
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_POINTER;
	    goto LEAVE;
	}

    }

    rval = (POINTER_type) TheSequence;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type ATTR_AllocASN1ObjectId( Source, Destination )
    IN ASN1_OBJECT_ID_type Source;
    OUT ASN1_OBJECT_ID_type *Destination;
{

    INT_type rval;

    SEQUENCE_type NewSequence;
    
    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    if( Source->sequence_tag != SEQUENCE_INT_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocASN1ObjectId]: ANS1 object id seen that is not a sequence of ints" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    
    NewSequence = MakeSequence( SEQUENCE_INT_tag, Source->length );
    if( NewSequence == ERROR_SEQUENCE ){
	rval = ERROR_INT;
	goto LEAVE;
    }

    *Destination = NewSequence;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type SEQUENCE_ASN1_OBJECT_ID_Free(storage)
	IN POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = DeleteSequence( (SEQUENCE_type) storage );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_FreeAsn1ObjectId( storage )
    IN ASN1_OBJECT_ID_type storage;
{

    /*
	This routine is passed a particular ASN1 OBJECT ID to free.

	This ANS1 object ID is a sequence because an ASN1 object id is a 
	    sequence of ints.
    */

    INT_type rval;

    INT_type dummy;
    
#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    rval = DeleteSequence( (SEQUENCE_INT_type) storage );

    if( rval == ERROR_INT ){
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_ASN1_OBJECT_ID_Free]: can not free an asn1 object id" );
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



INT_type SEQUENCE_ASN1_OBJECT_ID_Set(AttrStructPtr, TheStorage)
	IN ATTR_attribute_t AttrStructPtr;
	IN POINTER_type TheStorage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    AttrStructPtr->attribute_value.SEQUENCE_ASN1_OBJECT_ID_field =
	(SEQUENCE_ASN1_OBJECT_ID_type) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type SEQUENCE_ASN1_OBJECT_ID_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_SEQUENCE_ASN1_OBJECT_ID( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_ASN1_OBJECT_ID_CkVal(constituent, attribute, value, mask)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN POINTER_type value;
	IN PARM_MASK_type mask;
{

    /*
	Attributes with type SEQUENCE_ASN1_OBJECT_ID_type:
	    dp_CONTENT_ARCH_CLASSES
	    dp_TYPES_OF_CODING
    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    SEQUENCE_type TheSequence;

    INT_type i;

    INT_type dummy;

    VALUE_type WhichASN1;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ) {

    case dp_CONTENT_ARCH_CLASSES:
    case dp_TYPES_OF_CODING:
	if( !( CONST_IsDocProf( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[SEQUENCE_ASN1_OBJECT_ID_CkVal]: Attribute " );
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
	dummy = TKERR_Format( "[SEQUENCE_ASN1_OBJECT_ID_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " bad attribute for SEQUENCE_ASN1_OBJECT_ID_type" );
	dummy = TKERR_EndErrMsg();	
	goto LEAVE;
    }

    TheSequence = (SEQUENCE_type) value;

    if( TheSequence->sequence_tag != SEQUENCE_ASN1_OBJECT_ID_tag ) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_ASN1_OBJECT_ID_CkVal]: " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " value must be a sequence of ASN1_OBJECT_ID's" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }   

    for( i = (INT_type) 0; i < TheSequence->length; i++ ){

	WhichASN1 = LookUpObjectId( 
	(ASN1_OBJECT_ID_type) TheSequence->sequence_value.asn1_object_ids[i] );

	if( WhichASN1 == UNKNOWN_id ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SEQUENCE_ASN1_OBJECT_ID_CkVal]: unknown ans1 object id" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	switch( attribute ){
	
	case dp_CONTENT_ARCH_CLASSES:
	    switch( WhichASN1 ){

	    case FORM_CC_id:
	    case PROC_CC_id:
	    case FP_CC_id:
	    case FORM_RG_id:
	    case FP_RG_id:
	    case FORM_GG_id:
		continue;
	    
	    default:
		rval = ERROR_INT;
		TKError = BADPARM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[SEQUENCE_ASN1_OBJECT_ID_CkVal]: illegal content arch class value for dp_CONTENT_ARCH_CLASSES" );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;

	    }
	    break;

	case dp_TYPES_OF_CODING:

	    switch( WhichASN1 ){

	    case CC_CODING_id:
	    case RG_T6_CODING_id:
	    case RG_T4_1_CODING_id:
	    case RG_T4_2_CODING_id:
	    case RG_BITMAP_CODING_id:
	    case GG_CODING_id:
		continue;
	    
	    default:
		rval = ERROR_INT;
		TKError = BADPARM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[SEQUENCE_ASN1_OBJECT_ID_CkVal]: illegal content arch class value for dp_TYPES_OF_CODING" );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;

	    }
	    break;

	} /* end of the switch on attribute */

    }	/* end of the for */

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_ASN1_OBJECT_ID_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN PARM_MASK_type wanted_mask;
	IN PARM_MASK_type *destination_mask;
	IN POINTER_type *destination;
{

    /*
	Attributes with type SEQUENCE_ASN1_OBJECT_ID_type:
	    dp_CONTENT_ARCH_CLASSES
	    dp_TYPES_OF_CODING
    */

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
    dummy = TKERR_Format("[SEQUENCE_ASN1_OBJECT_ID_Deflt]: Can not get iso 8613 default value" );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}
