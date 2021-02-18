

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr4.c,v 1.3 89/10/31 13:55:54 annm Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr4.c,v $


(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr4.c,v $
 * Revision 1.3  89/10/31  13:55:54  annm
 * fixing link problems
 * fixing LINT problems not link
 * 
 * Revision 1.2  89/10/26  14:06:34  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.1  89/09/21  06:22:58  mss
 * Initial revision
 * 
 * Revision 1.22  89/04/21  18:16:50  annm
 * chnages for deleteseq/deletesequencedeeply
 * 
 * Revision 1.21  89/03/16  15:29:49  annm
 * changes for SEQUENCE_REVISION_HISTORY_type and at_SAME_LAYOUT_OBJECT_type
 * 
 * Revision 1.20  89/03/09  15:53:01  annm
 * no change
 * 
 * Revision 1.19  89/01/31  19:05:45  annm
 * 
 * 
 * Revision 1.18  89/01/19  14:53:59  annm
 * primping the Check value code - tightened up restrictions on presentation attributes going on content dependent DVL/DAPD components. 
 * ALso tightened up on presentation attribute placement to account for the shared versus logical versus layout presentation attributes.
 * 
 * Revision 1.17  89/01/17  16:56:32  annm
 * changes for DVL defaulting
 * 
 * Revision 1.16  88/12/13  19:55:15  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.15  88/11/18  13:56:43  annm
 * fixed problm in at_SEPARATION iso default canonical procedure - it is ok to have memory because at_SEPARATION has parameters
 * 
 * Revision 1.14  88/11/01  08:42:58  annm
 * fixed bug in offset default code - having memory already is NOT an error
 * 
 * Revision 1.13  88/09/19  16:56:02  annm
 * added error message formatting, canonical free and ckval routines
 * 
 * Revision 1.12  88/09/06  13:09:25  annm
 * no change
 * 
 * Revision 1.11  88/09/06  12:01:31  annm
 * no change
 * 
 * Revision 1.10  88/09/01  14:19:16  annm
 * changed blasted " to <>
 * 
 * Revision 1.9  88/08/22  19:43:50  annm
 * tweeking the blasted masks
 * 
 * Revision 1.8  88/08/15  18:16:38  annm
 * fidging with the iso default routines to get the blasted
 * parm mask set righ
 * 
 * Revision 1.7  88/08/10  15:42:33  annm
 * added inheritance from object/style, objectclass/sytle and iso 8613 defaults for the currently implemented aunion.h types
 * 
 * Revision 1.6  88/08/03  13:56:41  annm
 * added setting of destination_mask in the copy's to get the return to the user to work right
 * 
 * Revision 1.5  88/08/01  18:45:20  annm
 * OH BOY WHAT A MESS - THE DASTERLY -I BUG STRIKES AGAIN (AND, OH BOY, IS IT HARD TO FIND... THANK GOD FOR CC -E !!!)
 * 
 * Revision 1.4  88/07/29  16:11:52  annm
 * fixed bugs in alloc and copy for SEQUENCE_STRING
 * 
 * Revision 1.3  88/07/28  17:20:17  annm
 * added copy, alloc, get and set for SEQUENCE STRING
 * 
 * Revision 1.2  88/07/26  18:41:45  annm
 * added copy, allocate, get and set canonical procedures for at_OFFSET_type and at_SEPARATION_type
 * 
 * Revision 1.1  88/07/20  19:25:25  annm
 * Initial revision
 * 

*/


#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr4.c,v $";
#endif


/*

    This file constaint the canonical procedures for the following
    aunion.h types:

	LAYOUT_OBJECT_type
	at_OFFSET_type
	at_SEPARATION_type
	SEQUENCE_STRING_type

    At present, only the Copy, Alloc, Set and Get canonical procedures
    are implemented. The others are still in stubs.c.

*/



#define	TK_IMPL_ATTR
#define TK_GVARS_ATTR4
#define TK_IMPL_CONST
#define TK_TKI_SEQ
#define TK_TKI_MM
#define TK_TKI_TKERR

#include <rnames.h>
#include <acd.h>
#include <seq.h>
#include <mm.h>
#include <parms.h>
#include <attr0.h>
#include <tkerr.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR4
#undef TK_IMPL_CONST
#undef TK_TKI_SEQ
#undef TK_TKI_MM
#undef TK_TKI_TKERR



INT_type LAYOUT_OBJECT_Copy( source, source_mask, wanted_mask, 
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	IN POINTER_type destination;
{

    /* The mask is ignored. */

    /* because allocate is shallow, may have to alloc a sequence */

    INT_type rval;

    LAYOUT_OBJECT_type *TheSource;
    LAYOUT_OBJECT_type *TheDestination;

    SEQUENCE_type TheSequence;

    INT_type dummy;

#ifdef DEBUG
    If( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    /* co-erse the pointers */
    TheSource = (LAYOUT_OBJECT_type *) source;
    TheDestination = (LAYOUT_OBJECT_type *) destination;

    switch( TheSource->layout_object_tag ){
    case LAYOUT_OBJ_DESC_tag:
	    TheDestination->layout_object_tag = LAYOUT_OBJ_DESC_tag;
	    TheDestination->layout_object_value.constituent =
		TheSource->layout_object_value.constituent;
	    break;
    case LAYOUT_CATEGORY_tag:
	    TheDestination->layout_object_tag = LAYOUT_CATEGORY_tag;
	    TheSequence = CopySequence( (SEQUENCE_type) 
		    TheSource->layout_object_value.layout_category );
	    if( TheSequence == ERROR_SEQUENCE ){
		rval = ERROR_INT;
		TKError = NOMEM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format(
		    "[LAYOUT_OBJECT_Copy]: Can not copy a LAYOUT CATEGORY" );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	    }
	    TheDestination->layout_object_value.layout_category =
		(STRING_type) TheSequence;
	    break;
    case OBJECT_TYPE_PAGE_tag:
	    TheDestination->layout_object_tag = OBJECT_TYPE_PAGE_tag;
	    break;
    case NULL_tag:
	    TheDestination->layout_object_tag = NULL_tag;
	    break;
    default:
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[LAYOUT_OBJECT_Copy]: Bad tag seen" );
	    dummy = TKERR_EndErrMsg();
	    break;
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



POINTER_type LAYOUT_OBJECT_Alloc(value)
    IN POINTER_type value;
{

    POINTER_type rval;

    INT_type dummy;

#ifdef DEBUG
    If( TraceRoutine() ){
	Entering();
    }
#endif

    rval = MM_Malloc( (INT_type) sizeof( LAYOUT_OBJECT_type ) );
    if( rval == ERROR_POINTER ){
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[LAYOUT_OBJECT_Alloc]: Can not get memory" );
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



INT_type LAYOUT_OBJECT_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

    LAYOUT_OBJECT_type *TheLO;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    TheLO = (LAYOUT_OBJECT_type *) storage;

    switch( TheLO->layout_object_tag ){

    case LAYOUT_OBJ_DESC_tag:
	break;

    case LAYOUT_CATEGORY_tag:
	SlaveRval = STRING_Free( (POINTER_type) 
	    TheLO->layout_object_value.layout_category );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();   
	    dummy = TKERR_Format("[LAYOUT_OBJECT_Free]: Can not free layout category string" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case OBJECT_TYPE_PAGE_tag:
	break;

    case NULL_tag:
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[LAYOUT_OBJECT_Free]: bad layout object tag of " );
	dummy = TKERR_FormatInt("%d", (INT_type) TheLO->layout_object_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }    

    rval = MM_Free( storage );
    
LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type LAYOUT_OBJECT_Set(AttrStructPtr, TheStorage)
    IN OUT ATTR_attribute_t AttrStructPtr;
    IN	   POINTER_type	    TheStorage;
{

    INT_type rval;

#ifdef DEBUG
    If( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    AttrStructPtr->attribute_value.LAYOUT_OBJECT_field = 
	(LAYOUT_OBJECT_type *) TheStorage;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



POINTER_type LAYOUT_OBJECT_Get(AttrStructPtr)
    IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
    If( TraceRoutine() ){
	Entering();
    }
#endif

    rval = (POINTER_type) GET_LAYOUT_OBJECT( AttrStructPtr );


LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type LAYOUT_OBJECT_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{


    /*
	Attributes with type LAYOUT_OBJECT_type:
	    at_INDIVISIBILITY 
	    at_NEW_LAYOUT_OBJECT 

	The masks are irrelevent.
    */


    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    LAYOUT_OBJECT_type *TheValue;

    INT_type dummy;


#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){

    case at_INDIVISIBILITY:
    case at_NEW_LAYOUT_OBJECT:
	if( !( CONST_IsLayoutStyle( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[LAYOUT_OBJECT_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a layout style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[LAYOUT_OBJECT_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " doesn't have LAYOUT_OBJECT_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    /* now examie the actual value */

    TheValue = (LAYOUT_OBJECT_type *) value;
    
    if( TheValue->layout_object_tag == LAYOUT_CATEGORY_tag ){
	if( TheValue->layout_object_value.layout_category->sequence_tag !=
	    SEQUENCE_CHAR_tag ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[LAYOUT_OBJECT_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " layout category is not a sequence of chars" );
	dummy = TKERR_EndErrMsg();
	}
    }
    else if( TheValue->layout_object_tag != LAYOUT_OBJ_DESC_tag &&
	     TheValue->layout_object_tag != OBJECT_TYPE_PAGE_tag &&
	     TheValue->layout_object_tag != NULL_tag ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[LAYOUT_OBJECT_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " value has an illegal layout_object_tag" );
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



INT_type LAYOUT_OBJECT_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
{

    /*
	Attributes with type LAYOUT_OBJECT_type that do NOT default:

	Attributes with type LAYOUT_OBJECT_type that DO default:
	    at_INDIVISIBILITY (objects only)
	    at_NEW_LAYOUT_OBJECT (objects only)

	The masks are irrelevent.
    */

    INT_type rval;

    LAYOUT_OBJECT_type *TheLayoutObject;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;
    
    /* first get space */

    switch( attribute ){

    case at_INDIVISIBILITY:
    case at_NEW_LAYOUT_OBJECT:
	if( *destination == (POINTER_type) 0 ){
	    *destination = MM_Malloc( (INT_type) sizeof( LAYOUT_OBJECT_type ) );
	    if( *destination == ERROR_POINTER ){
		rval = ERROR_INT;
		TKError = NOMEM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[LAYOUT_OBJECT_Deflt]: can not get memory" );
		dummy = TKERR_EndErrMsg();

		goto LEAVE;
	    }
	}
	else {
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[LAYOUT_OBJECT_Deflt]: have memory and shouldn't" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[LAYOUT_OBJECT_Deftl]: Bad attribute" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    switch( attribute ){
    
    case at_INDIVISIBILITY:
    case at_NEW_LAYOUT_OBJECT:
	TheLayoutObject = (LAYOUT_OBJECT_type *) *destination;
	TheLayoutObject->layout_object_tag = NULL_tag;
	TheLayoutObject->layout_object_value.constituent = NULL_CONSTITUENT;
	break;
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



INT_type at_OFFSET_Copy(source, source_mask, wanted_mask, 
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type destination;
{

    /*
	NOTE: at_OFFSET has parameters that default independently.
    */

    INT_type rval;
    
    at_OFFSET_type  *TheSource;
    at_OFFSET_type  *TheDestination;

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
	    right away, without updating destination_mask.
	*/
	goto LEAVE;
    }

    TheSource = (at_OFFSET_type *) source;
    TheDestination = (at_OFFSET_type *) destination;

    if( source_mask & LEADING_OFFSET_parm  &&
	wanted_mask & LEADING_OFFSET_parm &&
	!( *destination_mask & LEADING_OFFSET_parm ) ){
	TheDestination->leading_offset = TheSource->leading_offset;
	*destination_mask |= LEADING_OFFSET_parm;
    }

    if( source_mask & TRAILING_OFFSET_parm &&
	wanted_mask & TRAILING_OFFSET_parm &&
	!( *destination_mask & TRAILING_OFFSET_parm ) ){
	TheDestination->trailing_offset = TheSource->trailing_offset;
	*destination_mask |= TRAILING_OFFSET_parm;
    }

    if( source_mask & LEFT_HAND_OFFSET_parm &&
	wanted_mask & LEFT_HAND_OFFSET_parm &&
	!( *destination_mask & LEFT_HAND_OFFSET_parm ) ){
	TheDestination->left_hand_offset = TheSource->left_hand_offset;
	*destination_mask |= LEFT_HAND_OFFSET_parm;
    }

    if( source_mask & RIGHT_HAND_OFFSET_parm &&
	wanted_mask & RIGHT_HAND_OFFSET_parm &&
	!( *destination_mask & RIGHT_HAND_OFFSET_parm ) ){
	TheDestination->right_hand_offset = TheSource->right_hand_offset;
	*destination_mask |= RIGHT_HAND_OFFSET_parm;
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type at_OFFSET_Alloc(value)
    IN POINTER_type value;
{

    POINTER_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Malloc( (INT_type) sizeof( at_OFFSET_type ) );
    if( rval == ERROR_POINTER ){
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[at_OFFSET_Alloc]: Can not get memory" );
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



INT_type at_OFFSET_Free(storage)
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



INT_type at_OFFSET_Set(AttrStructPtr, TheStorage)
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

    AttrStructPtr->attribute_value.at_OFFSET_field = 
	(at_OFFSET_type *) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type at_OFFSET_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_at_OFFSET( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type at_OFFSET_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    /*
	Attribute with type at_OFFSET_type
	    at_OFFSET 

	The wanted_mask determines which fields get set. The
	destination_mask gets updated.
    */


    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    at_OFFSET_type *TheValue;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){

    case at_OFFSET:
	if( !( CONST_IsLayoutStyle( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[at_OFFSET_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a layout style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[at_OFFSET_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " doesn't have at_OFFSET_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    /* now examie the actual value */

    TheValue = (at_OFFSET_type *) value;

    if( ( ( mask & LEADING_OFFSET_parm ) &&
	TheValue->leading_offset < (INT_type) 0  ) ||
	( ( mask & TRAILING_OFFSET_parm ) &&
	TheValue->trailing_offset < (INT_type ) 0 ) ||
	( ( mask & LEFT_HAND_OFFSET_parm ) &&
	TheValue->left_hand_offset < (INT_type ) 0 ) ||
	( ( mask & RIGHT_HAND_OFFSET_parm ) &&
	TheValue->right_hand_offset < (INT_type ) 0 ) ) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[at_OFFSET_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " all fields must be non negative ints" );
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



INT_type at_OFFSET_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
{

    /*
	Attribute with type at_OFFSET_type that do NOT default:

	Attribute with type at_OFFSET_type that DO default:
	    at_OFFSET (objects only)

	The wanted_mask determines which fields get set. The
	destination_mask gets updated.
    */

    INT_type rval;

    at_OFFSET_type *TheOffset;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( attribute ){

    /* all four fields default to (INT_type) zero */
    case at_OFFSET:
	if( *destination == (POINTER_type) 0 ){
	    *destination = MM_Malloc( (INT_type) sizeof( at_OFFSET_type ) );
	    if( *destination == ERROR_POINTER ){
		rval = ERROR_INT;
		TKError = NOMEM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format(
		    "[at_OFFSET_Deflt]: can not get memory" );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	    }
  	}

	TheOffset = (at_OFFSET_type *) *destination;
	if( ( wanted_mask & LEADING_OFFSET_parm ) != (PARM_MASK_type) 0 &&
	    ( *destination_mask & LEADING_OFFSET_parm ) == (PARM_MASK_type) 0 ){
	    TheOffset->leading_offset = (INT_type) 0;
	    *destination_mask |= LEADING_OFFSET_parm;
	}
	if( ( wanted_mask & TRAILING_OFFSET_parm ) != (PARM_MASK_type) 0 &&
	    ( *destination_mask & TRAILING_OFFSET_parm ) == (PARM_MASK_type) 0 ){
	    TheOffset->trailing_offset = (INT_type) 0;
	    *destination_mask |= TRAILING_OFFSET_parm;
	}
	if( ( wanted_mask & LEFT_HAND_OFFSET_parm ) != (PARM_MASK_type) 0 &&
	    ( *destination_mask & LEFT_HAND_OFFSET_parm ) == (PARM_MASK_type) 0 ){
	    TheOffset->left_hand_offset = (INT_type) 0;
	    *destination_mask |= LEFT_HAND_OFFSET_parm;    
	}
	if( ( wanted_mask & RIGHT_HAND_OFFSET_parm ) != (PARM_MASK_type) 0 &&
	    ( *destination_mask & RIGHT_HAND_OFFSET_parm ) == (PARM_MASK_type) 0 ){
	    TheOffset->right_hand_offset = (INT_type) 0;
	    *destination_mask |= RIGHT_HAND_OFFSET_parm;
	}
	break;
    
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[at_OFFSET_Deflt]: bad attribute" );
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



INT_type at_SEPARATION_Copy(source, source_mask, wanted_mask, 
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    /*
	at_SEPARATION defaults and the parameters default 
	independently.
    */

    INT_type rval;

    at_SEPARATION_type *TheSource;
    at_SEPARATION_type *TheDestination;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;
    
    if( source_mask == NO_PARMS ){
	/* don't update the destination mask because indep parms */
	goto LEAVE;
    }

    TheSource = (at_SEPARATION_type *) source;
    TheDestination = (at_SEPARATION_type *) destination;

    if( source_mask & LEADING_EDGE_parm &&
	wanted_mask & LEADING_EDGE_parm &&
	!( *destination_mask & LEADING_EDGE_parm ) ){
	TheDestination->leading_edge = TheSource->leading_edge;
	*destination_mask |= LEADING_EDGE_parm;
    }
    
    if( source_mask & TRAILING_EDGE_parm &&
	wanted_mask & TRAILING_EDGE_parm &&
	!( *destination_mask & TRAILING_EDGE_parm ) ){
	TheDestination->trailing_edge = TheSource->trailing_edge;
	*destination_mask |= TRAILING_EDGE_parm;
    }

    if( source_mask & CENTRE_SEPARATION_parm &&
	wanted_mask & CENTRE_SEPARATION_parm &&
	!( *destination_mask & CENTRE_SEPARATION_parm ) ){
	TheDestination->centre_separation = TheSource->centre_separation;
	*destination_mask |= CENTRE_SEPARATION_parm;
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type at_SEPARATION_Alloc(value)
    IN POINTER_type value;
{

    POINTER_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Malloc( (INT_type) sizeof( at_SEPARATION_type ) );
    if( rval == ERROR_POINTER ){
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[at_SEPARATION_Alloc]: can not get memory" );
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



INT_type at_SEPARATION_Free(storage)
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



INT_type at_SEPARATION_Set(AttrStructPtr, TheStorage)
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

    AttrStructPtr->attribute_value.at_SEPARATION_field =
	(at_SEPARATION_type *) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type at_SEPARATION_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_at_SEPARATION( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type at_SEPARATION_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    /*
	Attributes with type at_SEPARATION_type
	    at_SEPARATION 

	The mask are relevent.
    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    at_SEPARATION_type *TheValue;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){

    case at_SEPARATION:
	if( !( CONST_IsLayoutStyle( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[at_SEPARATION_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a layout style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[at_SEPARATION_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " doesn't have at_OFFSET_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    /* now examie the actual value */

    TheValue = (at_SEPARATION_type *) value;

    if( ( ( mask & LEADING_EDGE_parm ) &&
	TheValue->leading_edge < (INT_type) 0  ) ||
	( ( mask & TRAILING_EDGE_parm ) &&
	TheValue->trailing_edge < (INT_type ) 0 ) ||
	( ( mask & CENTRE_SEPARATION_parm ) &&
	TheValue->centre_separation < (INT_type ) 0 ) ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[at_SEPARATION_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " all fields must be non negative ints" );
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



INT_type at_SEPARATION_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
{

    /*
	Attributes with type at_SEPARATION_type that do NOT default:

	Attributes with type at_SEPARATION_type that DO default:
	    at_SEPARATION (objects only)

	The mask are relevent.
    */

    INT_type rval;

    at_SEPARATION_type *TheSeparation;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( attribute ){

    case at_SEPARATION:
	if( *destination == (POINTER_type) 0 ){
	    *destination = MM_Malloc( (INT_type) sizeof( at_SEPARATION_type ) );
	    if( *destination == ERROR_POINTER ){
		rval = ERROR_INT;
		TKError = NOMEM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format("[at_SEPARATION_Deflt]: can't get memory" );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	    }	
	}

	TheSeparation = (at_SEPARATION_type *) *destination;
	if( ( wanted_mask & LEADING_EDGE_parm ) != (PARM_MASK_type) 0 &&
	    ( *destination_mask & LEADING_EDGE_parm ) == (PARM_MASK_type) 0 ){
	    TheSeparation->leading_edge = (INT_type) 0;
	    *destination_mask |= LEADING_EDGE_parm;
	}
	if( ( wanted_mask & TRAILING_EDGE_parm ) != (PARM_MASK_type) 0 &&
	    ( *destination_mask & TRAILING_EDGE_parm ) == (PARM_MASK_type) 0 ){
	    TheSeparation->trailing_edge = (INT_type) 0;
	    *destination_mask |= TRAILING_EDGE_parm;
	}
	if( ( wanted_mask & CENTRE_SEPARATION_parm ) != (PARM_MASK_type) 0 &&
	    ( *destination_mask & CENTRE_SEPARATION_parm ) == (PARM_MASK_type) 0 ){
	    TheSeparation->centre_separation = (INT_type) 0;
	    *destination_mask |= CENTRE_SEPARATION_parm;
	}
	break;	

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[at_SEPARATION_Deflt]: bad attribute" );
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



INT_type SEQUENCE_STRING_Copy(source, source_mask, wanted_mask, 
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    INT_type rval;

    INT_type i;
    SEQUENCE_type TheSource;
    SEQUENCE_type TheDestination;

    SEQUENCE_type TheSourceString;
    SEQUENCE_type TheDestinationString;

    INT_type SlaveRval;

    INT_type dummy;

    PARM_MASK_type dummy_mask;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSource = (SEQUENCE_type) source;
    TheDestination = (SEQUENCE_type) destination;

    if( TheSource == NULL_SEQUENCE ){
	*destination_mask = source_mask;
	goto LEAVE;	
    }

    if( TheSource->sequence_tag != SEQUENCE_STRING_tag ||
	TheDestination ->sequence_tag != SEQUENCE_STRING_tag  ||
	TheSource->length != TheDestination->length  ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_STRING_Copy]: bad source sequence tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
	
    for( i = 0; i < TheSource->length; i++ ){

	TheSourceString = TheSource->sequence_value.strings[i];
	TheDestinationString = TheDestination->sequence_value.strings[i];

	SlaveRval = STRING_Copy( (POINTER_type) TheSourceString, 
			/* source mask */ (PARM_MASK_type) 0,
			/* wanted mask */ (PARM_MASK_type) 0,
			/* dest mask */ &dummy_mask,
			(POINTER_type) TheDestinationString );
	if( SlaveRval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    rval = ERROR_INT;
	    goto LEAVE;
	}
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



POINTER_type SEQUENCE_STRING_Alloc(value)
    IN POINTER_type value;
{

    POINTER_type rval;

    SEQUENCE_type SourceSequence;
    SEQUENCE_type DestinationSequence;
    
    SEQUENCE_type OldString;

    INT_type i;
    INT_type dummy;

    POINTER_type Ptr;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    SourceSequence = (SEQUENCE_type) value;

    if( SourceSequence == NULL_SEQUENCE ){
	rval = value;
	goto LEAVE;
    }

    if( SourceSequence->sequence_tag != SEQUENCE_STRING_tag ){
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_STRING_Alloc]: bad source tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    DestinationSequence = MakeSequence( SEQUENCE_STRING_tag,
		    SourceSequence->length );
    if( DestinationSequence == ERROR_SEQUENCE ){
	/* ASSERT: MakeSequence has set TKError */
	rval = ERROR_POINTER;
	goto LEAVE;
    } 

    /* now allocate the strings in the body */

    for( i = 0; i < SourceSequence->length; i++){

	OldString = SourceSequence->sequence_value.strings[i];

	Ptr = STRING_Alloc( (POINTER_type) OldString );
	if( Ptr == ERROR_POINTER ){
	    rval = ERROR_POINTER;
	    dummy =  ATTR_Free_Some_String( DestinationSequence, 
			(INT_type) (i - 1) );
	    goto LEAVE;
	}
    
	DestinationSequence->sequence_value.strings[i] =
		(STRING_type) Ptr;
    }

    rval = (POINTER_type) DestinationSequence;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type ATTR_Free_Some_String( TheSequence, HowMany )
    IN SEQUENCE_type TheSequence;
    IN INT_type HowMany;
{

    INT_type rval;

    INT_type i;

    STRING_type TheString;

    INT_type dummy;
 
#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    if( TheSequence->sequence_tag != SEQUENCE_STRING_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_Free_Some_String]: bad sequence tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < HowMany; i++ ){

	TheString = TheSequence->sequence_value.strings[i];
	dummy = STRING_Free( (POINTER_type) TheString );
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}




INT_type SEQUENCE_STRING_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = DeleteSequenceDeeply( (SEQUENCE_type) storage );


LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type ATTR_FreeStrings( TheString )
    IN STRING_type TheString;
{

    INT_type rval;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    SlaveRval = STRING_Free( (POINTER_type) TheString );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_FreeStrings]: can not free a string in a sequence of strings" );
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


INT_type SEQUENCE_STRING_Set(AttrStructPtr, TheStorage)
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

    AttrStructPtr->attribute_value.SEQUENCE_STRING_field =
	(SEQUENCE_type) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type SEQUENCE_STRING_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_SEQUENCE_STRING( AttrStructPtr );


LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_STRING_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    /*
	Attributes with type SEQUENCE STRING
	    dp_ORGANIZATIONS
	    dp_USER_SPECIFIC_CODES
	    dp_LANGUAGES
	    at_PERMITTED_CATEGORIES

	The masks are irrelevent.
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

    case dp_ORGANIZATIONS:
    case dp_USER_SPECIFIC_CODES:
    case dp_LANGUAGES:
	if( !( CONST_IsDocProf( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[SEQUENCE_STRINGS_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on the document profile" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case at_PERMITTED_CATEGORIES:
	if( !( CONST_IsFrame( TheConstituent ) ||
		CONST_IsDVLFrame( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( "[SEQUENCE_STRINGS_CkVal]: Attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on a frame" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[SEQUENCE_STRING_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " doesn't have SEQUENCE_STRING_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    /* Can't check the actual value */


LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_STRING_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
{
    /*
	Attributes with type SEQUENCE STRING that do NOT default:
	    dp_ORGANIZATIONS
	    dp_USER_SPECIFIC_CODES
	    dp_LANGUAGES

	Attributes with type SEQUENCE STRING that DO default:
	    at_PERMITTED_CATEGORIES (objects only)

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

    case at_PERMITTED_CATEGORIES:
	if( *destination == (POINTER_type) 0 ){
	    *destination = (POINTER_type) NULL_SEQUENCE;
	}
	else {
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SEQUENCE_STRING_Deflt]: have memory and shouldn't" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_STRING_Deflt]: Bad attribute" );
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
