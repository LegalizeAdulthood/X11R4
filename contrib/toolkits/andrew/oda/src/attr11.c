
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr11.c,v 1.2 89/10/31 13:04:20 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr11.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr11.c,v $
 * Revision 1.2  89/10/31  13:04:20  jr
 * Casting for MM_Malloc.
 * 
 * Revision 1.1  89/09/21  06:16:37  mss
 * Initial revision
 * 
 * Revision 1.13  89/09/12  09:03:56  mss
 * Removed #
 * 
 * Revision 1.12  89/03/09  15:50:13  annm
 * changes for new DAPD stuff
 * 
 * Revision 1.11  89/02/23  20:35:03  annm
 * changes for new DVL stuff
 * 
 * Revision 1.10  89/01/19  14:42:50  annm
 * primping the Check value code - tightened up restrictions on presentation attributes going on content dependent DVL/DAPD components. 
 * ALso tightened up on presentation attribute placement to account for the shared versus logical versus layout presentation attributes.
 * 
 * Revision 1.9  89/01/17  16:55:22  annm
 * changes for DVL defaulting
 * 
 * Revision 1.8  88/12/13  19:53:35  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.7  88/09/30  15:04:50  annm
 * no change
 * 
 * Revision 1.6  88/09/21  15:44:58  annm
 * added ckval and free canonical procedures - also added the tkerr stuff
 * 
 * Revision 1.5  88/09/14  15:42:37  annm
 * added LINT to the ifndef's
 * 
 * Revision 1.4  88/09/06  13:08:31  annm
 * no change
 * 
 * Revision 1.3  88/09/01  14:13:53  annm
 * added canonical procedures for at_POSITION_type
 * 
 * Revision 1.2  88/08/30  15:12:07  annm
 * no change
 * 
 * Revision 1.1  88/08/26  16:29:01  annm
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr11.c,v $";
#endif

/* Put .c template comment here */

/*

    This file contains the canonical procedures fro the following
    aunion.h types;

	SEQUENCE_INT_PAIR_type
	dp_ODA_VERSION_type
	at_POSITION_type

    At present, only the Copy, Alloc, Set, Get and iso default
    canonical procedures are implemented. The others
    are still in stubs.c
*/

#define	TK_IMPL_ATTR
#define TK_GVARS_ATTR11
#define TK_IMPL_CONST
#define TK_TKI_SEQ
#define TK_TKI_STR
#define TK_TKI_MM
#define TK_TKI_TKERR

#include <rnames.h>
#include <acd.h>
#include <seq.h>
#include <mm.h>
#include <str.h>
#include <parms.h>
#include <values.h>
#include <tkerr.h>
#include <attr0.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR11
#undef TK_IMPL_CONST
#undef TK_TKI_SEQ
#undef TK_TKI_STR
#undef TK_TKI_MM
#undef TK_TKI_TKERR

/* put #define's and declarations local to this file here */



INT_type SEQUENCE_INT_PAIR_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    /*
	The source mask is ignored.

	The source is a sequence type.
    
	The destination is a sequence type.

	ASSERT: wanted_mask is ALL_PARMS.
    */

    INT_type rval;

    SEQUENCE_type Source;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    Source = (SEQUENCE_type) source;

    if( Source == NULL_SEQUENCE ){
	rval = SUCCESS_INT;
    }
    else {
	rval = SEQ_CopySequenceElements( (SEQUENCE_type) source,
	    (SEQUENCE_type) destination );
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



POINTER_type SEQUENCE_INT_PAIR_Alloc(value)
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

    if( TheSource == NULL_SEQUENCE ){
	rval = value;
	goto LEAVE;
    }

    if( TheSource->sequence_tag != SEQUENCE_INT_PAIR_tag ){
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[SEQUENCE_INT_PAIR_Alloc]: bad sequence tag in source" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheSequence = MakeSequence( SEQUENCE_INT_PAIR_tag,
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



INT_type SEQUENCE_INT_PAIR_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

    SEQUENCE_type TheSequence;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    TheSequence = (SEQUENCE_type) storage;

    if( TheSequence == NULL_SEQUENCE ){
	rval = SUCCESS_INT;
	goto LEAVE;
    }

    rval = DeleteSequence( (SEQUENCE_type) storage );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_INT_PAIR_Set(AttrStructPtr, TheStorage)
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

    AttrStructPtr->attribute_value.SEQUENCE_INT_PAIR_field =
	(SEQUENCE_INT_PAIR_type) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type SEQUENCE_INT_PAIR_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_SEQUENCE_INT_PAIR( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



/* maximum length for a cc character fonts int pair sequence */
#define CC_CHARACTER_FONTS_MAX	( (INT_type) 10)

INT_type SEQUENCE_INT_PAIR_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    /*
	Attributes with type SEQUENCE INT PAIR
	    dp_PAGE_DIMENSIONS
	    dp_PAGE_POSITION
	    cc_CHARACTER_FONTS

	The masks are irrelevent.
    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    SEQUENCE_type TheValue;

    INT_type i;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){

    case cc_CHARACTER_FONTS:
	if( !( CONST_IsBlock( TheConstituent ) ||
		CONST_IsPresentStyle( TheConstituent) ||
		CONST_IsDVLBlock( TheConstituent ) ||
		CONST_IsDAPDBlock( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SEQUENCE_INT_PAIR_Ckval]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format(" must go on a block or presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case dp_PAGE_DIMENSIONS:
    case dp_PAGE_POSITIONS:
	if( !( CONST_IsDocProf( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SEQUENCE_INT_PAIR_Ckval]: Attribute " );
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
	dummy = TKERR_Format(
	    "[SEQUENCE_INT_PAIR_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" does not have sequence int pair type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    TheValue = (SEQUENCE_type) value;

    if( attribute == cc_CHARACTER_FONTS &&
	TheValue != NULL_SEQUENCE &&
	( TheValue->sequence_tag != SEQUENCE_INT_PAIR_tag ||
	TheValue->length > CC_CHARACTER_FONTS_MAX ) ) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_INT_PAIR_CkVal]: " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " value must be a sequence of int pairs or NULL_SEQUENCE or length too big" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    else if( attribute == cc_CHARACTER_FONTS &&
	TheValue == NULL_SEQUENCE ){
	rval = SUCCESS_INT;
	goto LEAVE;
    }
    else if( attribute != cc_CHARACTER_FONTS &&
	    TheValue->sequence_tag != SEQUENCE_INT_PAIR_tag ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[SEQUENCE_INT_PAIR_CkVal]: Value's sequence tag is not SEQUENCE_INT_PAIR_tag " );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;	
    }

    for( i = (INT_type) 0; i < TheValue->length; i++ ){

	switch( attribute ){
    
	case cc_CHARACTER_FONTS:
	    if( TheValue->sequence_value.int_pairs[i].first <= (INT_type) 0 ||
		TheValue->sequence_value.int_pairs[i].second <= (INT_type) 0 ){
		rval = ERROR_INT;
		TKError = BADPARM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format(
		    "[SEQUENCE_INT_PAIR_CkVal]: For attribute " );
		dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
		dummy = TKERR_FormatInt(" value of %d-th int pair is bad - ", i );
		dummy = TKERR_FormatInt("got first = %d and ", 
		    TheValue->sequence_value.int_pairs[i].first );
		dummy = TKERR_FormatInt("and second = %d and ", 
		    TheValue->sequence_value.int_pairs[i].second );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	    }
	    break;

	case dp_PAGE_DIMENSIONS:
	    break;

	case dp_PAGE_POSITIONS:
	    if( TheValue->sequence_value.int_pairs[i].first < (INT_type) 0 ||
		TheValue->sequence_value.int_pairs[i].second < (INT_type) 0 ){
		rval = ERROR_INT;
		TKError = BADPARM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format(
		    "[SEQUENCE_INT_PAIR_CkVal]: For attribute " );
		dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
		dummy = TKERR_FormatInt(" value of %d-th int pair is bad - ", i );
		dummy = TKERR_FormatInt("got first = %d and ", 
		    TheValue->sequence_value.int_pairs[i].first );
		dummy = TKERR_FormatInt("and second = %d and ", 
		    TheValue->sequence_value.int_pairs[i].second );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;	
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



INT_type SEQUENCE_INT_PAIR_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
{

    /*
	Attributes with type SEQUENCE INT PAIR that do NOT default:
	    dp_PAGE_DIMENSIONS
	    dp_PAGE_POSITION
	
	Attributes with type SEQUENCE INT PAIR that DO default:
	    cc_CHARACTER_FONTS

	The masks are irrelevent.
    */

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

   rval = SUCCESS_INT;

    switch( attribute ){

    /* default value is sequence with length zero */
    case cc_CHARACTER_FONTS:

	if( *destination == (POINTER_type) 0 ){
	    *destination = (POINTER_type) NULL_SEQUENCE;
	    *destination_mask = wanted_mask;
	}
	else{
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[SEQUENCE_INT_PAIR_Deflt]: have storage and shouldn't" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_INT_PAIR_Deflt]: Bad attribute" );
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



INT_type dp_ODA_VERSION_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    /* 
	NOTE: that because allocator is shallow, copy must allocate
	the two strings in the body of the dp_ODA_VERSION_type
	structure.
    */

    INT_type rval;

    dp_ODA_VERSION_type *TheSource;
    dp_ODA_VERSION_type *TheDestination;

    STRING_type TheStandard;
    STRING_type TheDate;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSource = (dp_ODA_VERSION_type *) source;

    if( TheSource->standard->sequence_tag != SEQUENCE_CHAR_tag ||
	TheSource->date->sequence_tag != SEQUENCE_CHAR_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[dp_ODA_VERSION_Copy]: source and destination tags don't match" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheStandard = MakeString( TheSource->standard->length );
    if( TheStandard == ERROR_STRING ){
	/* ASSERT: TKError got set */
	rval = ERROR_INT;
	goto LEAVE;
    }
    
    TheDate = MakeString( TheSource->date->length );
    if( TheDate == ERROR_STRING ){
	/* ASSERT: TkError got set */
	dummy = DeleteSequenceDeeply( TheStandard );
	rval = ERROR_INT;
	goto LEAVE;
    }
    
    rval = SEQ_CopySequenceElements( TheSource->standard, TheStandard );
    if( rval == ERROR_INT ){
	/* ASSET: TKError got set */
	dummy = DeleteSequenceDeeply( TheStandard );
	dummy = DeleteSequenceDeeply( TheDate );
	rval = ERROR_INT;
	goto LEAVE;
    }

    rval = SEQ_CopySequenceElements( TheSource->date, TheDate );
    if( rval == ERROR_INT ){
	/* ASSET: TKError got set */
	dummy = DeleteSequenceDeeply( TheStandard );
	dummy = DeleteSequenceDeeply( TheDate );
	rval = ERROR_INT;
	goto LEAVE;
    }

    TheDestination = (dp_ODA_VERSION_type *) destination;

    TheDestination->standard = TheStandard;
    TheDestination->date = TheDate;

    *destination_mask = source_mask;
    
LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type dp_ODA_VERSION_Alloc(value)
	IN POINTER_type value;
{

    /*
	Shallow allocate of a dp_ODA_VERSION_type structure.
    */

    POINTER_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Malloc( (INT_type) sizeof( dp_ODA_VERSION_type ) );
    if( rval == ERROR_POINTER ){
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[dp_ODA_VERSION_Alloc]: can not get memory" );
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




INT_type dp_ODA_VERSION_Free(storage)
	POINTER_type storage;
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



INT_type dp_ODA_VERSION_Set(AttrStructPtr, TheStorage)
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

    AttrStructPtr->attribute_value.dp_ODA_VERSION_field =
	(dp_ODA_VERSION_type *) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type dp_ODA_VERSION_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_dp_ODA_VERSION( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type dp_ODA_VERSION_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    dp_ODA_VERSION_type *TheValue;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){

    case dp_ODA_VERSION:
	if( !( CONST_IsDocProf( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[dp_ODA_VERSION_CkVal]: Attribute " );
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
	dummy = TKERR_Format(
	    "[dp_ODA_VERSION_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" does not have dp_ODA_VERSION_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    TheValue = (dp_ODA_VERSION_type *) value;

    switch( attribute ){
    
    case dp_ODA_VERSION:
	/* might want to check the date for compilance with iso 8601 */
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



INT_type dp_ODA_VERSION_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
{

    /*
	The only attribute with this type is dp_ODA_VERSION which can NOT
	default because it is a document profile attribute.
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
    dummy = TKERR_Format(
	"[dp_ODA_VERSION_Deflt]: doesn't have an iso 8613 default" );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type at_POSITION_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    INT_type rval;

    at_POSITION_type *Source;
    at_POSITION_type *Destination;

    PARM_MASK_type TheMask;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    Source = (at_POSITION_type *) source;
    Destination = (at_POSITION_type *) destination;

    if( Source->position_tag != FIXED_POSITION_tag &&
	Source->position_tag != VARIABLE_POSITION_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[at_POSITION_Copy]: source tag bad" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    Destination->position_tag = Source->position_tag;

    if( source_mask == NO_PARMS ){
	/*
	    Because of defaulting with related parms, 
	    this stops the inheritance 
	*/
	*destination_mask = NO_PARMS;
	goto LEAVE;
    }

    /* just to be safe - clean up the mask */
    if( Source->position_tag == FIXED_POSITION_tag ){
	TheMask = source_mask & 
		( HORIZONTAL_POSITION_parm | VERTICAL_POSITION_parm );
    }
    else {
	TheMask = source_mask & 
		( POS_LEADING_OFFSET_parm | POS_TRAILING_OFFSET_parm |
		  POS_LEFT_HAND_OFFSET_parm | POS_RIGHT_HAND_OFFSET_parm |
		  POS_LEADING_EDGE_parm | POS_TRAILING_EDGE_parm |
		  POS_CENTRE_SEPARATION_parm |
		  POS_ALIGNMENT_parm | POS_FILL_ORDER_parm );
    }

    if( ( TheMask & HORIZONTAL_POSITION_parm ) &&
	( *destination_mask & HORIZONTAL_POSITION_parm ) == (PARM_MASK_type) 0){
	Destination->position_body.fixed_position.horizontal_position =
	Source->position_body.fixed_position.horizontal_position;
	*destination_mask |= HORIZONTAL_POSITION_parm;
    }

    if( ( TheMask & VERTICAL_POSITION_parm ) &&
	( *destination_mask & VERTICAL_POSITION_parm ) == (PARM_MASK_type) 0){
	Destination->position_body.fixed_position.vertical_position =
	Source->position_body.fixed_position.vertical_position;
	*destination_mask |= VERTICAL_POSITION_parm;
    }

    if( ( TheMask & POS_LEADING_OFFSET_parm ) &&
	( *destination_mask & POS_LEADING_OFFSET_parm ) == (PARM_MASK_type) 0){
	Destination->position_body.variable_position.offset.leading_offset =
	Source->position_body.variable_position.offset.leading_offset;
	*destination_mask |= POS_LEADING_OFFSET_parm;
    }

    if( ( TheMask & POS_TRAILING_OFFSET_parm ) &&
	( *destination_mask & POS_TRAILING_OFFSET_parm )== (PARM_MASK_type) 0){
	Destination->position_body.variable_position.offset.trailing_offset =
	Source->position_body.variable_position.offset.trailing_offset;
	*destination_mask |= POS_TRAILING_OFFSET_parm;
    }

    if( ( TheMask & POS_LEFT_HAND_OFFSET_parm ) &&
	( *destination_mask & POS_LEFT_HAND_OFFSET_parm ) == (PARM_MASK_type) 0){
	Destination->position_body.variable_position.offset.left_hand_offset =
	Source->position_body.variable_position.offset.left_hand_offset;
	*destination_mask |= POS_LEFT_HAND_OFFSET_parm;
    }

    if( ( TheMask & POS_RIGHT_HAND_OFFSET_parm ) &&
	( *destination_mask & POS_RIGHT_HAND_OFFSET_parm ) == (PARM_MASK_type) 0){
	Destination->position_body.variable_position.offset.right_hand_offset =
	Source->position_body.variable_position.offset.right_hand_offset;
	*destination_mask |= POS_RIGHT_HAND_OFFSET_parm;
    }

    if( ( TheMask & POS_LEADING_EDGE_parm ) &&
	( *destination_mask & POS_LEADING_EDGE_parm ) == (PARM_MASK_type) 0){
	Destination->position_body.variable_position.separation.leading_edge =
	Source->position_body.variable_position.separation.leading_edge;
	*destination_mask |= POS_LEADING_EDGE_parm;
    }

    if( ( TheMask & POS_TRAILING_EDGE_parm ) &&
	( *destination_mask & POS_TRAILING_EDGE_parm ) == (PARM_MASK_type) 0){
	Destination->position_body.variable_position.separation.trailing_edge =
	Source->position_body.variable_position.separation.trailing_edge;
	*destination_mask |= POS_TRAILING_EDGE_parm;
    }

    if( ( TheMask & POS_CENTRE_SEPARATION_parm ) && 
	( *destination_mask & POS_CENTRE_SEPARATION_parm ) == (PARM_MASK_type) 0){
	Destination->position_body.variable_position.separation.centre_separation =
	Source->position_body.variable_position.separation.centre_separation;
	*destination_mask |= POS_CENTRE_SEPARATION_parm;
    }

    if( ( TheMask & POS_ALIGNMENT_parm ) &&
	( *destination_mask & POS_ALIGNMENT_parm ) == (PARM_MASK_type) 0){
	Destination->position_body.variable_position.alignment =
	Source->position_body.variable_position.alignment;
	*destination_mask |= POS_ALIGNMENT_parm;
    }

    if( ( TheMask & POS_FILL_ORDER_parm ) &&
	( *destination_mask & POS_FILL_ORDER_parm ) == (PARM_MASK_type) 0){
	Destination->position_body.variable_position.fill_order =
	Source->position_body.variable_position.fill_order;
	*destination_mask |= POS_FILL_ORDER_parm;
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type at_POSITION_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Malloc( (INT_type) sizeof( at_POSITION_type ) );
    if( rval == ERROR_POINTER ){
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[at_POSITION_Alloc]: can not get memory" );
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



INT_type at_POSITION_Free(storage)
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



INT_type at_POSITION_Set(AttrStructPtr, TheStorage)
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

    AttrStructPtr->attribute_value.at_POSITION_field =
	(at_POSITION_type *) TheStorage;


LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type at_POSITION_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_at_POSITION( AttrStructPtr );


LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type at_POSITION_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    at_POSITION_type *TheValue;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){

    case at_POSITION:
	if( !( CONST_IsFrame( TheConstituent ) ||
		CONST_IsBlock( TheConstituent ) ||
		CONST_IsDVLFrame( TheConstituent ) ||
		CONST_IsDVLBlock( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[at_POSITION_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format(" must go on a block or a frame" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[at_POSITIION_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" does not have at_POSITION_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    TheValue = (at_POSITION_type *) value;

    /* code that follows assumes that the attribute is at_POSITION */

    switch( TheValue->position_tag ){

    case FIXED_POSITION_tag:
	if( ( mask & HORIZONTAL_POSITION_parm &&
	    TheValue->position_body.fixed_position.horizontal_position 
		< (INT_type) 0 ) ||
	    ( mask & VERTICAL_POSITION_parm &&
	    TheValue->position_body.fixed_position.vertical_position
		< (INT_type) 0 ) ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[at_POSITION_CkVal]: Fixed position value for attribute at_POSITION is illegal" );
	    dummy = TKERR_FormatInt(" - got horizontal position = %d and", 
		TheValue->position_body.fixed_position.horizontal_position );
	    dummy = TKERR_FormatInt(" and vertical position = %d", 
		TheValue->position_body.fixed_position.vertical_position );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    case VARIABLE_POSITION_tag:
	if( mask & POS_LEADING_OFFSET_parm &&
	    TheValue->position_body.variable_position.offset.leading_offset 
		< (INT_type) 0 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[at_POSITION_CkVal]: Variable position value for attribute at_POSITION is illegal" );
	    dummy = TKERR_FormatInt(" - got leading offset = %d", 
		TheValue->position_body.variable_position.offset.leading_offset );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	if( mask & POS_TRAILING_OFFSET_parm &&
	    TheValue->position_body.variable_position.offset.trailing_offset
		< (INT_type) 0 ) {
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[at_POSITION_CkVal]: Variable position value for attribute at_POSITION is illegal" );
	    dummy = TKERR_FormatInt(" - got trailing offset = %d", 
		TheValue->position_body.variable_position.offset.trailing_offset );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	if( mask & POS_LEFT_HAND_OFFSET_parm &&
	    TheValue->position_body.variable_position.offset.left_hand_offset
		< (INT_type) 0 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[at_POSITION_CkVal]: Variable position value for attribute at_POSITION is illegal" );
	    dummy = TKERR_FormatInt(" - got left hand offset = %d", 
		TheValue->position_body.variable_position.offset.left_hand_offset );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	if( mask & POS_RIGHT_HAND_OFFSET_parm &&
	    TheValue->position_body.variable_position.offset.right_hand_offset
		< (INT_type) 0 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[at_POSITION_CkVal]: Variable position value for attribute at_POSITION is illegal" );
	    dummy = TKERR_FormatInt(" - got right hand offset = %d", 
		TheValue->position_body.variable_position.offset.right_hand_offset );
	    dummy = TKERR_EndErrMsg();
	}
	if( mask & POS_LEADING_EDGE_parm &&
	    TheValue->position_body.variable_position.separation.leading_edge
		< (INT_type) 0 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[at_POSITION_CkVal]: Variable position value for attribute at_POSITION is illegal" );
	    dummy = TKERR_FormatInt(" - got leading edge separation = %d", 
		TheValue->position_body.variable_position.separation.leading_edge );
	    dummy = TKERR_EndErrMsg();
	}	
	if( mask & POS_TRAILING_EDGE_parm &&
	    TheValue->position_body.variable_position.separation.trailing_edge
		< (INT_type) 0 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[at_POSITION_CkVal]: Variable position value for attribute at_POSITION is illegal" );
	    dummy = TKERR_FormatInt(" - got trailing edge separation = %d", 
		TheValue->position_body.variable_position.separation.trailing_edge );
	    dummy = TKERR_EndErrMsg();
	}	
	if( mask & POS_CENTRE_SEPARATION_parm  &&
	    TheValue->position_body.variable_position.separation.centre_separation
		< (INT_type) 0 ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[at_POSITION_CkVal]: Variable position value for attribute at_POSITION is illegal" );
	    dummy = TKERR_FormatInt(" - got centre separation = %d", 
		TheValue->position_body.variable_position.separation.centre_separation );
	    dummy = TKERR_EndErrMsg();
	}
	if( mask & POS_ALIGNMENT_parm  &&
	    TheValue->position_body.variable_position.alignment
		!= at_POSITION_right_hand_aligned &&
	    TheValue->position_body.variable_position.alignment
		!= at_POSITION_centred &&
	    TheValue->position_body.variable_position.alignment
		!= at_POSITION_left_hand_aligned  ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[at_POSITION_CkVal]: Variable position value for attribute at_POSITION is illegal" );
	    dummy = TKERR_FormatInt(" - got alignment = %d and", 
		(INT_type) TheValue->position_body.variable_position.alignment );
	    dummy = TKERR_EndErrMsg();
	}	
	if( mask & POS_FILL_ORDER_parm  &&
	    TheValue->position_body.variable_position.fill_order
		!= at_POSITION_normal_order &&
	    TheValue->position_body.variable_position.fill_order
		!= at_POSITION_reverse_order ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[at_POSITION_CkVal]: Variable position value for attribute at_POSITION is illegal" );
	    dummy = TKERR_FormatInt(" - got fill order = %d and", 
		(INT_type) TheValue->position_body.variable_position.fill_order );
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



INT_type at_POSITION_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
{

    INT_type rval;

    at_POSITION_type *Destination;

    PARM_MASK_type TheMask;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    if( *destination == (POINTER_type) 0 ){
	*destination = MM_Malloc( (INT_type) sizeof( at_POSITION_type ) );
	if( *destination == ERROR_POINTER ){
	    rval = ERROR_INT;
	    TKError = NOMEM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[at_POSITION_Deflt]: can not get memory" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	/* OUR GUESS - that if we haven't found anything return fixed */
	Destination = (at_POSITION_type *) *destination;
	Destination->position_tag = FIXED_POSITION_tag;
    }
    else {
	Destination = (at_POSITION_type *) *destination;
    }

    if( Destination->position_tag == FIXED_POSITION_tag ){
	TheMask = wanted_mask & 
		( HORIZONTAL_POSITION_parm | VERTICAL_POSITION_parm );
    }
    else {
	TheMask = wanted_mask & 
		( POS_LEADING_OFFSET_parm | POS_TRAILING_OFFSET_parm |
		  POS_LEFT_HAND_OFFSET_parm | POS_RIGHT_HAND_OFFSET_parm |
		  POS_LEADING_EDGE_parm | POS_TRAILING_EDGE_parm |
		  POS_CENTRE_SEPARATION_parm |
		  POS_ALIGNMENT_parm | POS_FILL_ORDER_parm );
    }

    if( ( TheMask & HORIZONTAL_POSITION_parm ) &&
	( *destination_mask & HORIZONTAL_POSITION_parm ) == (PARM_MASK_type) 0){
	Destination->position_body.fixed_position.horizontal_position =
	    (INT_type) 0;
	*destination_mask |= HORIZONTAL_POSITION_parm;
    }

    if( ( TheMask & VERTICAL_POSITION_parm ) &&
	( *destination_mask & VERTICAL_POSITION_parm ) == (PARM_MASK_type) 0 ){
	Destination->position_body.fixed_position.vertical_position =
	    (INT_type) 0;
	*destination_mask |= VERTICAL_POSITION_parm;
    }

    if( ( TheMask & POS_LEADING_OFFSET_parm ) &&
	( *destination_mask * POS_LEADING_OFFSET_parm ) == (PARM_MASK_type) 0 ){
	Destination->position_body.variable_position.offset.leading_offset =
	    (INT_type) 0;
	*destination_mask |= POS_LEADING_OFFSET_parm;
    }

    if( ( TheMask & POS_TRAILING_OFFSET_parm ) &&
	( *destination_mask & POS_TRAILING_OFFSET_parm ) == (PARM_MASK_type) 0){
	Destination->position_body.variable_position.offset.trailing_offset =
	    (INT_type) 0;
	*destination_mask |= POS_TRAILING_OFFSET_parm;
    }

    if( ( TheMask & POS_LEFT_HAND_OFFSET_parm ) &&
	( *destination_mask & POS_LEFT_HAND_OFFSET_parm ) == (PARM_MASK_type) 0){
	Destination->position_body.variable_position.offset.left_hand_offset =
	    (INT_type) 0;
	*destination_mask |= POS_LEFT_HAND_OFFSET_parm;
    }

    if( ( TheMask & POS_RIGHT_HAND_OFFSET_parm ) &&
	( *destination_mask & POS_RIGHT_HAND_OFFSET_parm ) == (PARM_MASK_type) 0 ){
	Destination->position_body.variable_position.offset.right_hand_offset =
	    (INT_type) 0;
	*destination_mask |= POS_RIGHT_HAND_OFFSET_parm;
    }

    if( ( TheMask & POS_LEADING_EDGE_parm ) &&
	( *destination_mask & POS_LEADING_EDGE_parm ) == (PARM_MASK_type) 0 ){
	Destination->position_body.variable_position.separation.leading_edge =
	    (INT_type) 0;
	*destination_mask |= POS_LEADING_EDGE_parm;
    }

    if( ( TheMask & POS_TRAILING_EDGE_parm ) &&
	( *destination_mask & POS_TRAILING_EDGE_parm ) == (PARM_MASK_type) 0 ){
	Destination->position_body.variable_position.separation.trailing_edge =
	    (INT_type) 0;
	*destination_mask |= POS_TRAILING_EDGE_parm;
    }

    if( ( TheMask & POS_CENTRE_SEPARATION_parm ) &&
	( *destination_mask & POS_CENTRE_SEPARATION_parm ) == (PARM_MASK_type) 0 ){
	Destination->position_body.variable_position.separation.centre_separation =
	    (INT_type) 0;
	*destination_mask |= POS_CENTRE_SEPARATION_parm;
    }

    if( ( TheMask & POS_ALIGNMENT_parm ) &&
	( *destination_mask & POS_ALIGNMENT_parm ) == (PARM_MASK_type) 0 ){
	Destination->position_body.variable_position.alignment =
	    at_POSITION_right_hand_aligned;
	*destination_mask |= POS_ALIGNMENT_parm;
    }

    if( ( TheMask & POS_FILL_ORDER_parm ) &&
	( *destination_mask & POS_FILL_ORDER_parm ) == (PARM_MASK_type) 0){
	Destination->position_body.variable_position.fill_order =
	    at_POSITION_normal_order;
	*destination_mask |= POS_FILL_ORDER_parm;
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}
