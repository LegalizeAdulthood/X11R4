
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr26.c,v 1.1 89/09/21 06:22:11 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr26.c,v $

(c) Copyright 1988 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	attr26.c,v $
 * Revision 1.1  89/09/21  06:22:11  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/12  09:09:23  mss
 * Removed #
 * 
 * Revision 1.1  89/04/11  15:39:03  annm
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr26.c,v $";
#endif



#define TK_IMPL_ATTR
#define TK_GVARS_ATTR26
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
#include <parms.h>

#include <attr0.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR26
#undef TK_TKI_TKERR
#undef TK_TKI_SEQ
#undef TK_IMPL_CONST
#undef TK_TKI_MM


/*

    This file cntains the canonical aunion.h operations for the following 
	types:

	SEQUENCE_BINDING_PAIR_type

*/

INT_type SEQUENCE_BINDING_PAIR_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type destination;
{

    INT_type rval;

    SEQUENCE_type SourceSequence;
    SEQUENCE_type DestinationSequence;

    INT_type SlaveRval;

    INT_type i;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    SourceSequence = (SEQUENCE_type) source;
    DestinationSequence = (SEQUENCE_type) destination;

    if( SourceSequence->sequence_tag != SEQUENCE_BINDING_PAIR_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_BINDING_PAIR_Copy]: the source does not have SEQUENCE_BINDING_PAIR_tag - got " );
	dummy = TKERR_FormatInt( "%d", 
		    (INT_type) SourceSequence->sequence_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( DestinationSequence->sequence_tag != SEQUENCE_BINDING_PAIR_tag ){
	rval = ERROR_INT;
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_BINDING_PAIR_Copy]: the destination dose not have SEQUENCE_BINDING_PAIR_tag - got " );
	dummy = TKERR_FormatInt( "%d", 
		    (INT_type) DestinationSequence->sequence_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < SourceSequence->length; i++ ){

	SlaveRval = ATTR_CopyBindingPair( 
			&SourceSequence->sequence_value.binding_pairs[i],
			&DestinationSequence->sequence_value.binding_pairs[i] );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SEQUENCE_BINDING_PAIR_Copy]:  The above error occurred while copying binding pair number - " );
	    dummy = TKERR_FormatInt( "%d", i );
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



INT_type ATTR_CopyBindingPair( source, destination )
    IN BINDING_PAIR_type *source;
    OUT BINDING_PAIR_type *destination;
{

    INT_type rval;
	
    INT_type SlaveRval;

    INT_type dummy;

    PARM_MASK_type dummy_mask;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( source->value.binding_pair_tag ){

    case STRING_EXPR_tag:
	SlaveRval = ATTR_CopyStringExpr( 
			source->value.binding_pair_value.string_expr,
			destination->value.binding_pair_value.string_expr );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CopyBindingPair]:  the above error occurred while copying a numeric expr in a binding value" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case NUMERIC_EXPR_tag:
	SlaveRval = ATTR_CopyNumericExpr(
			&source->value.binding_pair_value.numeric_expr,
			&destination->value.binding_pair_value.numeric_expr );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CopyBindingPair]:  the above error occurred while copy a numeric expr in a binding value" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case OBJECT_ID_EXPR_tag:
	SlaveRval = ATTR_CopyObjectIdExpr(
			&source->value.binding_pair_value.object_id_expr,
			&destination->value.binding_pair_value.object_id_expr );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CopyBindingPair]:  the above error occurred while copying an object id expr in a binding value" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case INTEGER_tag:
	destination->value.binding_pair_value.integer = 
		source->value.binding_pair_value.integer;
	break;

    case STRING_tag:
	dummy_mask = (PARM_MASK_type) 0;
	SlaveRval = STRING_Copy( 
			/* source */
			(POINTER_type) source->value.binding_pair_value.string,
			/* source mask */ ALL_PARMS,
			/* wanted mask */ ALL_PARMS,
			/* dest mask */ &dummy_mask,
			/* destination */
			(POINTER_type) destination->value.binding_pair_value.string );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CopyBindingPair]:  the above error occurred while copying the string in a binding pair" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case CONSTITUENT_tag:
	destination->value.binding_pair_value.constituent = 
		source->value.binding_pair_value.constituent;
	break;
	
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyBindingPair]: bad source binding pair tag seen - got " );
	dummy = TKERR_FormatInt( "%d", 
		    (INT_type) source->value.binding_pair_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
	
    }

    dummy_mask = (PARM_MASK_type) 0;
    SlaveRval = STRING_Copy( 
		    /* source */
		    (POINTER_type) source->binding_identifier,
		    /* source mask */ ALL_PARMS,
		    /* wanted mask */ ALL_PARMS,
		    /* dest mask */ &dummy_mask,
		    /* destination */
		    (POINTER_type) destination->binding_identifier );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyBindingPair]: the above error occurred while copying the binding identifier in a binding pair" );
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



INT_type ATTR_CopyStringExpr( source, destination )
    IN STRING_EXPR_type source;
    OUT STRING_EXPR_type destination;
{

    INT_type rval;

    SEQUENCE_type SourceSequence;
    SEQUENCE_type DestinationSequence;

    INT_type i;
    
    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    SourceSequence = (SEQUENCE_ATOMIC_STR_EXPR_type) source;
    DestinationSequence = (SEQUENCE_ATOMIC_STR_EXPR_type) destination;

    if( SourceSequence->sequence_tag != SEQUENCE_ATOMIC_STR_EXPR_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyStringExpr]: the source string expression (in a binding pair) does not have SEQUENCE_ATOMIC_STR_EXPR_tag - got " );
	dummy = TKERR_FormatInt( "%d", 
		    (INT_type) SourceSequence->sequence_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( DestinationSequence->sequence_tag != SEQUENCE_ATOMIC_STR_EXPR_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyStringExpr]: the destination string expression (in a binding pair) does not have SEQUENCE_ATOMIC_STR_EXPR_tag - got " );
	dummy = TKERR_FormatInt( "%d", 
		    (INT_type) DestinationSequence->sequence_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < SourceSequence->length; i++ ){

	SlaveRval = ATTR_CopyAtomicStrExpr(
		    &SourceSequence->sequence_value.atomic_string_exprs[i],
		    &DestinationSequence->sequence_value.atomic_string_exprs[i] );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CopyStringExpr]:  the above error occurred in copying an atomic string expr in a binding pair at element number " );
	    dummy = TKERR_FormatInt( "%d", i );
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



POINTER_type SEQUENCE_BINDING_PAIR_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    SEQUENCE_type OriginalSequence;

    SEQUENCE_type NewSequence;

    INT_type SlaveRval;

    INT_type i;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif


    OriginalSequence = (SEQUENCE_type) value;

    if( OriginalSequence->sequence_tag != SEQUENCE_BINDING_PAIR_tag ){
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_BINDING_PAIR_Alloc]: the value does not have SEQUENCE_BINDING_PAIR_tag - got " );
	dummy = TKERR_FormatInt( "%d", 
		    (INT_type) OriginalSequence->sequence_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    NewSequence = MakeSequence( SEQUENCE_BINDING_PAIR_tag,
		    OriginalSequence->length );
    if( NewSequence == ERROR_SEQUENCE ){
	rval = ERROR_POINTER;
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_BINDING_PAIR_Alloc]: can not get memory for the sequence binding pair" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    rval = (POINTER_type) NewSequence;

    for( i = (INT_type) 0; i < OriginalSequence->length; i++ ){

	SlaveRval = ATTR_AllocBindingPair( 
			&OriginalSequence->sequence_value.binding_pairs[i],
			&NewSequence->sequence_value.binding_pairs[i] );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_POINTER;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SEQUENCE_BINDING_PAIR_Alloc]:  The above error occurred while freeing binding pair number - " );
	    dummy = TKERR_FormatInt( "%d", i );
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



INT_type ATTR_AllocBindingPair( original, new )
    IN BINDING_PAIR_type *original;
    OUT BINDING_PAIR_type *new;
{

    INT_type rval;
	
    INT_type SlaveRval;

    POINTER_type SlavePtr;

    SEQUENCE_type NewSequence;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( original->value.binding_pair_tag ){

    case STRING_EXPR_tag:
	if( original->value.binding_pair_value.string_expr->sequence_tag 
		    != SEQUENCE_ATOMIC_STR_EXPR_tag ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocBindingPair: the original string expression (in a binding pair) does not have SEQUENCE_ATOMIC_STR_EXPR_tag - got " );
	    dummy = TKERR_FormatInt( "%d", 
		    (INT_type) 
		    original->value.binding_pair_value.string_expr->sequence_tag );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	NewSequence = MakeSequence( SEQUENCE_ATOMIC_STR_EXPR_tag,
			original->value.binding_pair_value.string_expr->length );
	if( NewSequence == ERROR_SEQUENCE ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocBindingPair]:  the above error occurred while MakeSequence'ing for a string expr in a binding pair" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	new->value.binding_pair_value.string_expr = (STRING_type) NewSequence;
	SlaveRval = ATTR_AllocStringExpr( 
			original->value.binding_pair_value.string_expr,
			new->value.binding_pair_value.string_expr );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocBindingPair]:  the above error occurred while allocating a numeric expr in a binding value" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case NUMERIC_EXPR_tag:
	SlaveRval = ATTR_AllocNumericExpr(
			&original->value.binding_pair_value.numeric_expr,
			&new->value.binding_pair_value.numeric_expr );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocBindingPair]:  the above error occurred while allocating a numeric expr in a binding value" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case OBJECT_ID_EXPR_tag:
	SlaveRval = ATTR_AllocObjectIdExpr(
			&original->value.binding_pair_value.object_id_expr,
			&new->value.binding_pair_value.object_id_expr );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocBindingPair]: the above error occurred while allocating an object id expr in a binding value" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case INTEGER_tag:
	break;

    case STRING_tag:
	SlavePtr = STRING_Alloc( (POINTER_type)
			original->value.binding_pair_value.string );
	if( SlavePtr == ERROR_POINTER ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocBindingPair]:  the above error occurred while allocating a string in a binding pair" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	new->value.binding_pair_value.string = (STRING_type) SlavePtr;
	break;

    case CONSTITUENT_tag:
	break;
	
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocBindingPair]: bad original binding pair tag seen - got " );
	dummy = TKERR_FormatInt( "%d", 
		    (INT_type) original->value.binding_pair_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
	
    }

    SlavePtr = STRING_Alloc( (POINTER_type)
		    original->binding_identifier );
    if( SlavePtr == ERROR_POINTER ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocBindingPair]: the above error occurred while allocating the binding identifier in a binding pair" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    new->binding_identifier = (STRING_type) SlavePtr;

    new->value.binding_pair_tag = original->value.binding_pair_tag;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_AllocStringExpr( original, new )
    IN STRING_EXPR_type original;
    OUT STRING_EXPR_type new;
{

    INT_type rval;

    SEQUENCE_type OriginalSequence;
    SEQUENCE_type NewSequence;

    INT_type i;
    
    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    OriginalSequence = (SEQUENCE_ATOMIC_STR_EXPR_type) original;
    NewSequence = (SEQUENCE_ATOMIC_STR_EXPR_type) new;

    for( i = (INT_type) 0; i < OriginalSequence->length; i++ ){

	SlaveRval = ATTR_AllocAtomicStrExpr(
		    &OriginalSequence->sequence_value.atomic_string_exprs[i],
		    &NewSequence->sequence_value.atomic_string_exprs[i] );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocStringExpr]:  the above error occurred in allocating an atomic string expr in a binding pair at element number " );
	    dummy = TKERR_FormatInt( "%d", i );
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



INT_type SEQUENCE_BINDING_PAIR_Free(storage)
	IN POINTER_type storage;
{

    INT_type rval;

    SEQUENCE_type TheSequence;

    INT_type SlaveRval;

    INT_type i;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSequence = (SEQUENCE_type) storage;

    if( TheSequence->sequence_tag != SEQUENCE_BINDING_PAIR_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_BINDING_PAIR_Free]: the value does not have SEQUENCE_BINDING_PAIR_tag - got " );
	dummy = TKERR_FormatInt( "%d", 
		    (INT_type) TheSequence->sequence_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < TheSequence->length; i++ ){

	SlaveRval = ATTR_FreeBindingPair( 
			&TheSequence->sequence_value.binding_pairs[i] );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SEQUENCE_BINDING_PAIR_Free]:  The above error occurred while freeing binding pair number - " );
	    dummy = TKERR_FormatInt( "%d", i );
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



INT_type ATTR_FreeBindingPair( BindingPair )
    IN BINDING_PAIR_type *BindingPair;
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

    switch( BindingPair->value.binding_pair_tag ){

    case STRING_EXPR_tag:
	SlaveRval = ATTR_FreeStringExpr( 
			BindingPair->value.binding_pair_value.string_expr );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeBindingPair]:  the above error occurred while freeing a numeric expr in a binding value" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case NUMERIC_EXPR_tag:
	SlaveRval = ATTR_FreeNumericExpr(
			&BindingPair->value.binding_pair_value.numeric_expr );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeBindingPair]:  the above error occurred while freeing a numeric expr in a binding value" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case OBJECT_ID_EXPR_tag:
	SlaveRval = ATTR_FreeObjectIdExpr(
			&BindingPair->value.binding_pair_value.object_id_expr );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeBindingPair]:  the above error occurred while freeing an object id expr in a binding value" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case INTEGER_tag:
	break;

    case STRING_tag:
	SlaveRval = STRING_Free( (POINTER_type)
			BindingPair->value.binding_pair_value.string );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeBindingPair]:  the above error occurred while freeing a string in a binding pair" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case CONSTITUENT_tag:
	break;
	
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_FreeBindingPair]: bad binding pair tag seen - got " );
	dummy = TKERR_FormatInt( "%d", 
		    (INT_type) BindingPair->value.binding_pair_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
	
    }

    SlaveRval = STRING_Free( (POINTER_type)
		    BindingPair->binding_identifier );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_FreeBindingPair]: the above error occurred while freeing the binding identifier in a binding pair" );
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



INT_type ATTR_FreeStringExpr( StringExpr )
    IN STRING_EXPR_type StringExpr;
{

    INT_type rval;

    SEQUENCE_type TheSequence;

    INT_type i;
    
    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSequence = (SEQUENCE_ATOMIC_STR_EXPR_type) StringExpr;

    if( TheSequence->sequence_tag != SEQUENCE_ATOMIC_STR_EXPR_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_FreeStringExpr]: the string expression (in a binding pair) does not have SEQUENCE_ATOMIC_STR_EXPR_tag - got " );
	dummy = TKERR_FormatInt( "%d", (INT_type) TheSequence->sequence_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < TheSequence->length; i++ ){

	SlaveRval = ATTR_FreeAtomicStrExpr(
			&TheSequence->sequence_value.atomic_string_exprs[i] );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeStringExpr]:  the above error occurred in freeing an atomic string expr in a binding pair at element number " );
	    dummy = TKERR_FormatInt( "%d", i );
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



INT_type SEQUENCE_BINDING_PAIR_Set(AttrStructPtr, TheStorage)
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

    AttrStructPtr->attribute_value.SEQUENCE_BINDING_PAIR_field =
	(SEQUENCE_BINDING_PAIR_type) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type SEQUENCE_BINDING_PAIR_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_SEQUENCE_BINDING_PAIR( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_BINDING_PAIR_CkVal(constituent, attribute, value, mask)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN POINTER_type value;
	IN PARM_MASK_type mask;
{

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    SEQUENCE_type TheSequence;

    INT_type SlaveRval;

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
    
    case at_BINDINGS:
	if( !( CONST_IsComponent( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SEQUENCE_BINDING_PAIR_CkVal]: Attribute ");
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format(" must go on a component" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_BINDING_PAIR_CkVal]: Atribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" does not have type SEQUENCE_BINDING_PAIR_type");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    TheSequence = (SEQUENCE_type) value;

    if( TheSequence->sequence_tag != SEQUENCE_BINDING_PAIR_tag ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_BINDING_PAIR_CkVal]: the value does not have SEQUENCE_BINDING_PAIR_tag - got " );
	dummy = TKERR_FormatInt( "%d", 
		    (INT_type) TheSequence->sequence_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < TheSequence->length; i++ ){

	SlaveRval = ATTR_CkValBindingPair( 
			&TheSequence->sequence_value.binding_pairs[i] );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SEQUENCE_BINDING_PAIR_CKVal]:  The above error occurred while checking binding pair number - " );
	    dummy = TKERR_FormatInt( "%d", i );
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



INT_type ATTR_CkValBindingPair( BindingPair )
    IN BINDING_PAIR_type *BindingPair;
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

    switch( BindingPair->value.binding_pair_tag ){

    case STRING_EXPR_tag:
	SlaveRval = ATTR_CheckStringExpr( 
			BindingPair->value.binding_pair_value.string_expr );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CkValBindingPair]:  the above error occurred while checking a numeric expr in a binding value" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case NUMERIC_EXPR_tag:
	SlaveRval = ATTR_CkValNumericExpr(
			&BindingPair->value.binding_pair_value.numeric_expr );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CkValBindingPair]:  the above error occurred while checking a numeric expr in a binding value" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case OBJECT_ID_EXPR_tag:
	SlaveRval = ATTR_CkValObjectIdExpr(
			&BindingPair->value.binding_pair_value.object_id_expr );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CkValBindingPair]:  the above error occurred while checking an object id expr in a binding value" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case INTEGER_tag:
	break;

    case STRING_tag:
	if( BindingPair->value.binding_pair_value.string->sequence_tag != 
	    SEQUENCE_CHAR_tag ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CkValBindingPair]: the string in a binding pair value does not have a SEQUENCE_CHAR_tag" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case CONSTITUENT_tag:
	break;
	
    default:
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CkValBindingPair]: bad binding pair tag seen - got " );
	dummy = TKERR_FormatInt( "%d", 
		    (INT_type) BindingPair->value.binding_pair_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
	
    }

    if( BindingPair->binding_identifier->sequence_tag != 
	    SEQUENCE_CHAR_tag ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CkValBindingPair]: the binding identifier in a binding pair is not a STRING" );
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



INT_type ATTR_CheckStringExpr( StringExpr )
    IN STRING_EXPR_type StringExpr;
{

    INT_type rval;

    SEQUENCE_type TheSequence;

    INT_type i;
    
    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSequence = (SEQUENCE_ATOMIC_STR_EXPR_type) StringExpr;

    if( TheSequence->sequence_tag != SEQUENCE_ATOMIC_STR_EXPR_tag ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CheckStringExpr]: the string expression (in a binding pair) does not have SEQUENCE_ATOMIC_STR_EXPR_tag - got " );
	dummy = TKERR_FormatInt( "%d", (INT_type) TheSequence->sequence_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < TheSequence->length; i++ ){

	SlaveRval = ATTR_CkValAtomicStrExpr(
			&TheSequence->sequence_value.atomic_string_exprs[i] );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CheckStringExpr]:  the above error occurred in checking an atomic string expr in a binding pair at element number " );
	    dummy = TKERR_FormatInt( "%d", i );
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



INT_type SEQUENCE_BINDING_PAIR_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type *destination;
{

    INT_type rval;

    SEQUENCE_type TheSequence;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( attribute ){

    /* default value of NULL_SEQUENCE */
    case at_BINDINGS:
	if( *destination == (POINTER_type) 0 ){
	    *destination = (POINTER_type) NULL_SEQUENCE;
	}
	else {
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[SEQUENCE_BINDING_PAIR_Deflt]: have space and shouldn't" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
		"[SEQUENCE_BINDING_PAIR_Deflt]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" does not have SEQUENCE_BINDING_PAIR_type" );
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
