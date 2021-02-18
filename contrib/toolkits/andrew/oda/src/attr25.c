
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr25.c,v 1.3 89/10/31 22:13:49 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr25.c,v $

(c) Copyright 1988 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	attr25.c,v $
 * Revision 1.3  89/10/31  22:13:49  mss
 * Moved casting of sizeof to a reasonable place
 * 
 * Revision 1.2  89/10/31  13:51:21  jr
 * Casting problems fixed.
 * 
 * Revision 1.1  89/09/21  06:21:30  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/12  09:08:50  mss
 * Removed #
 * 
 * Revision 1.1  89/04/11  15:38:31  annm
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr25.c,v $";
#endif



#define TK_IMPL_ATTR
#define TK_GVARS_ATTR25
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
#undef TK_GVARS_ATTR25
#undef TK_TKI_TKERR
#undef TK_TKI_SEQ
#undef TK_IMPL_CONST
#undef TK_TKI_MM


/*

    This file cntains the canonical aunion.h operations for the following 
	types:


	STRING_EXPR_type

*/



INT_type STRING_EXPR_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type destination;
{

    INT_type rval;

    SEQUENCE_type Source;

    SEQUENCE_type Destination;

    INT_type i;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    Source = (SEQUENCE_type) source;
    Destination = (SEQUENCE_type) destination;

    if( Source->sequence_tag != SEQUENCE_ATOMIC_STR_EXPR_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[STRING_EXPR_Copy]: source does not have a SEQUENCE_ATOMIC_STR_EXPR_tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( Destination->sequence_tag != SEQUENCE_ATOMIC_STR_EXPR_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[STRING_EXPR_Copy]: destination does not have a SEQUENCE_ATOMIC_STR_EXPR_tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < Source->length; i++ ){

	SlaveRval = ATTR_CopyAtomicStrExpr( 
		    &Source->sequence_value.atomic_string_exprs[i],
		    &Destination->sequence_value.atomic_string_exprs[i] );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[STRING_EXPR_Copy]: the above error occurred in copying sequence element " );
	    dummy = TKERR_FormatInt( "%d", i );
	    dummy = TKERR_EndErrMsg();
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



INT_type ATTR_CopyAtomicStrExpr( source, destination )
    IN ATOMIC_STRING_EXPR_type *source;
    OUT ATOMIC_STRING_EXPR_type *destination;
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

    switch( source->atomic_string_expr_tag ){

    case STRING_LITERAL_tag:
	dummy_mask = (PARM_MASK_type) 0;
	SlaveRval = STRING_Copy( 

			(POINTER_type)
			source->atomic_string_value.string,

			/* source mask */ ALL_PARMS,
			/* wanted mask */ ALL_PARMS,
			/* dest mask */ &dummy_mask,

			(POINTER_type) 
			destination->atomic_string_value.string );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CopyAtomicStrExpr]:  can not copy the string literal in an atomic string expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case REF_BINDING_VALUE_tag:
	SlaveRval = ATTR_CopyRefBindingValue(
		    &source->atomic_string_value.value,
		    &destination->atomic_string_value.value );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CopyAtomicStrExpr]:  the above message occurred while copying a ref binding value in an atomic string expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case STRING_FUNCTION_tag:
	SlaveRval = ATTR_CopyStringFunction(
		    &source->atomic_string_value.function,
		    &destination->atomic_string_value.function );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CopyAtomicStrExpr]:  the above message occurred while copying a string function in an atomic string expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyAtomicStrExpr]: bad source tag seen - got " );
	dummy = TKERR_FormatInt( "%d", 
		    (INT_type) source->atomic_string_expr_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    destination->atomic_string_expr_tag = source->atomic_string_expr_tag;    

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_CopyRefBindingValue( source, destination )
    IN REF_BINDING_VALUE_type *source;
    OUT REF_BINDING_VALUE_type *destination;
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

    SlaveRval = ATTR_CopyObjectRef( 
		    &source->object_ref,
		    &destination->object_ref );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyRefBindingValue]: the above error occurred while copying the object ref in a ref binding value" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    destination->binding_identifier = source->binding_identifier;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_CopyObjectRef( source, destination )
    IN OBJECT_REF_type *source;
    OUT OBJECT_REF_type *destination;
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

    switch( source->obj_ref_tag ){

    case OBJECT_ID_tag:
	destination->object_ref_value.object_id = 
	    source->object_ref_value.object_id;
	break;

    case OBJECT_ID_EXPR_tag:
	SlaveRval = ATTR_CopyObjectIdExpr(
		    &source->object_ref_value.object_id_expr,
		    &destination->object_ref_value.object_id_expr );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CopyObjectRef]:  the above error occurred while copying an object id expr in an object ref" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyObjRef]: bad source obj ref tag seen - got " );
	dummy = TKERR_FormatInt( "%d", (INT_type) source->obj_ref_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    destination->obj_ref_tag = source->obj_ref_tag;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_CopyObjectIdExpr( source, destination )
    IN OBJECT_ID_EXPR_type *source;
    OUT OBJECT_ID_EXPR_type *destination;
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

    switch( source->object_selection_function_tag ){

    case CURRENT_OBJECT_tag:
	/* no parameters - so no copying!!! */
	break;

    case CURRENT_INSTANCE_tag:
	SlaveRval = ATTR_CopyCurrInstanceParms(
		    &source->parameters.curr_inst_parms,
		    &destination->parameters.curr_inst_parms );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CopyObjectIdExpr]:  the above error occurred while copying current instance parms in an object id expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case SUPERIOR_OBJECT_tag:
    case PRECEDING_OBJECT_tag:
	SlaveRval = ATTR_CopyObjectIdExpr( 
		    source->parameters.sup_or_pre_parms,
		    destination->parameters.sup_or_pre_parms );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CopyObjectIdExpr]:  the above error occurred while copying sup or pre parameters in an object id expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyObjectIdExpr]: bad source tag seen - got " );
	dummy = TKERR_FormatInt( "%d", 
		    (INT_type) source->object_selection_function_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
	
    }

    destination->object_selection_function_tag = 
	source->object_selection_function_tag;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_CopyCurrInstanceParms( source, destination )
    IN CURR_INSTANCE_PARMS_type *source;
    OUT CURR_INSTANCE_PARMS_type *destination;
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

    switch( source->parm1_tag ){

    case OBJECT_CLASS_ID_tag:
	destination->parm1.object_class_id = source->parm1.object_class_id;
	break;

    case OBJECT_TYPE_tag:
	destination->parm1.object_type = source->parm1.object_type;
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyCurrInstanceParms]: bad source parm1 tag seen while copying a curr instantce parms - got " );
	dummy = TKERR_FormatInt( "%d",
		    (INT_type) source->parm1_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    destination->parm1_tag = source->parm1_tag;

    SlaveRval = ATTR_CopyObjectRef( source->parm2, destination->parm2 );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyCurrInstanceParms]: the above error occurred while copying the parm2 of a curr instance parms" );
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



INT_type ATTR_CopyStringFunction( source, destination )
    IN STRING_FUNCTION_type *source;
    OUT STRING_FUNCTION_type *destination;
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

    switch( source->function_tag ){

    case MAKE_STRING_tag:
    case UPPER_ALPHA_tag:
    case LOWER_ALPHA_tag:
    case UPPER_ROMAN_tag:
    case LOWER_ROMAN_tag:
	SlaveRval = ATTR_CopyNumericExpr( &source->arg,
			&destination->arg );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CopyStringFunction]:  the above error occurred while copying the arg in a string function" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyStringFunction]: bad source tag seen in string function - got " );
	dummy = TKERR_FormatInt( "%d",
		    (INT_type) source->function_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    destination->function_tag = source->function_tag;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
    

}



INT_type ATTR_CopyNumericExpr( source, destination )
    IN NUMERIC_EXPR_type *source;
    OUT NUMERIC_EXPR_type *destination;
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

    switch( source->numeric_expr_tag ){

    case NUMERIC_LITERAL_tag:
	destination->numeric_expr_value.numeric_literal =
	    source->numeric_expr_value.numeric_literal;
	break;

    case REF_BINDING_VALUE_tag:
	SlaveRval = ATTR_CopyRefBindingValue(
		    &source->numeric_expr_value.binding_value,
		    &destination->numeric_expr_value.binding_value );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CopyNumericExpr]:  the above error occurred while copying a ref binding value in a numeric expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case NUMERIC_FUNCTION_tag:
	SlaveRval = ATTR_CopyNumericFunction(
		    &source->numeric_expr_value.function,
		    &destination->numeric_expr_value.function );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CopyNumericExpr]:  the above error occurred while copying a numeric function in a numeric expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyNumericExpr]: bad source tag seen in a numeric expression - got " );
	dummy = TKERR_FormatInt( "%d",
		    (INT_type) source->numeric_expr_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    destination->numeric_expr_tag = source->numeric_expr_tag;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_CopyNumericFunction( source, destination )
    IN NUMERIC_FUNCTION_type *source;
    OUT NUMERIC_FUNCTION_type *destination;
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

    switch( source->function_tag ){

    case INCREMENT_tag:
    case DECREMENT_tag:
	SlaveRval = ATTR_CopyNumericExpr(
		    source->numeric_function_value.incr_decr_parameter,
		    destination->numeric_function_value.incr_decr_parameter );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CopyNumericFunction]:  the above error occurred while copying the incr/decr paramter of a numeric function" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case ORDINAL_tag:
	SlaveRval = ATTR_CopyObjectRef(
		    &source->numeric_function_value.ord_parameter,
		    &destination->numeric_function_value.ord_parameter );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CopyNumericFunction]:  the above error occurred while copying the ordinal in a numeric function" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyNumericFunction]: bad source tag seen in numeric function - got " );
	dummy = TKERR_FormatInt( "%d",
		    (INT_type) source->function_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    
    }

    destination->function_tag = source->function_tag;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



POINTER_type STRING_EXPR_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

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

    OriginalSequence = (SEQUENCE_type) value;

    if( OriginalSequence->sequence_tag != SEQUENCE_ATOMIC_STR_EXPR_tag ){
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[STRING_EXPR_Alloc]: value does not have a SEQUENCE_ATOMIC_STR_EXPR_tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    NewSequence = MakeSequence( SEQUENCE_ATOMIC_STR_EXPR_tag,
		    OriginalSequence->length );
    if( NewSequence == ERROR_SEQUENCE ){
	rval = ERROR_POINTER;
	goto LEAVE;
    }

    rval = (POINTER_type) NewSequence;

    for( i = (INT_type) 0; i < OriginalSequence->length; i++ ){

	SlaveRval = ATTR_AllocAtomicStrExpr( 
			/* original */
		    &OriginalSequence->sequence_value.atomic_string_exprs[i],
			/* new */
		    &NewSequence->sequence_value.atomic_string_exprs[i] );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_POINTER;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[STRING_EXPR_Alloc]: the above error occurred in allocating sequence element " );
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



INT_type ATTR_AllocAtomicStrExpr( original, new )
    IN ATOMIC_STRING_EXPR_type *original;
    OUT ATOMIC_STRING_EXPR_type *new;
{

    INT_type rval;

    INT_type SlaveRval;

    POINTER_type SlavePtr;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( original->atomic_string_expr_tag ){

    case STRING_LITERAL_tag:
	SlavePtr = STRING_Alloc( 
			(POINTER_type)
			original->atomic_string_value.string );	
	if( SlavePtr == ERROR_POINTER ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocAtomicStrExpr]:  can not get space for the string literal" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	new->atomic_string_value.string = (STRING_type) SlavePtr;
	break;

    case REF_BINDING_VALUE_tag:
	SlaveRval = ATTR_AllocRefBindingValue(
			/* original */
		    &original->atomic_string_value.value,
			/* new */
		    &new->atomic_string_value.value );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocAtomicStrExpr]:  the above message occurred in allocating space for a ref binding value in an atomic string expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case STRING_FUNCTION_tag:
	SlaveRval = ATTR_AllocStringFunction(
			/* original */
		    &original->atomic_string_value.function,
			/* new */
		    &new->atomic_string_value.function );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocAtomicStrExpr]:  the above message occurred in allocating space for a string function in an atomic string expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocAtomicStrExpr]: bad original tag seen - got " );
	dummy = TKERR_FormatInt( "%d", 
		    (INT_type) original->atomic_string_expr_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    new->atomic_string_expr_tag = original->atomic_string_expr_tag;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_AllocRefBindingValue( original, new )
    IN REF_BINDING_VALUE_type *original;
    OUT REF_BINDING_VALUE_type *new;
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

    SlaveRval = ATTR_AllocObjectRef( 
			/* original */
		    &original->object_ref,
			/* destination */
		    &new->object_ref );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocRefBindingValue]: the above error occurred while allocating the object ref in a ref binding value" );
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



INT_type ATTR_AllocObjectRef( original, new )
    IN OBJECT_REF_type *original;
    OUT OBJECT_REF_type *new;
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

    switch( original->obj_ref_tag ){

    case OBJECT_ID_tag:
	/* its just a constituent so nothing needs to be allocated */
	break;

    case OBJECT_ID_EXPR_tag:
	SlaveRval = ATTR_AllocObjectIdExpr(
			/* original */
		    &original->object_ref_value.object_id_expr,
			/* new */
		    &new->object_ref_value.object_id_expr );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocObjectRef]:  the above error occurred while allocating an object id expr in an object ref" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocObjRef]: bad obj ref tag seen - got " );
	dummy = TKERR_FormatInt( "%d", (INT_type) original->obj_ref_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    new->obj_ref_tag = original->obj_ref_tag;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_AllocObjectIdExpr( original, new )
    IN OBJECT_ID_EXPR_type *original;
    OUT OBJECT_ID_EXPR_type *new;
{

    INT_type rval;

    INT_type SlaveRval;

    INT_type dummy;

    POINTER_type ptr;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( original->object_selection_function_tag ){

    case CURRENT_OBJECT_tag:
	/* no parameters !!! */
	break;

    case CURRENT_INSTANCE_tag:
	SlaveRval = ATTR_AllocCurrInstanceParms(
			/* original */
		    &original->parameters.curr_inst_parms,
			/* new */
		    &new->parameters.curr_inst_parms );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocObjectIdExpr]:  the above error occurred while allocating current instance parms in an object id expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case SUPERIOR_OBJECT_tag:
    case PRECEDING_OBJECT_tag:
	ptr = MM_Malloc( (INT_type) sizeof( struct OBJECT_ID_EXPR_struct ) );
	if( ptr == ERROR_POINTER ){
	    rval = ERROR_INT;
	    TKError = NOMEM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocObjectIdExpr]:  can not get case for an OBJECT_ID_EXPR_struct for a superior or preceding object" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	new->parameters.sup_or_pre_parms = (struct OBJECT_ID_EXPR_struct *) ptr;
	SlaveRval = ATTR_AllocObjectIdExpr( 
			/* original */
		    original->parameters.sup_or_pre_parms,
			/* new */
		    new->parameters.sup_or_pre_parms );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocObjectIdExpr]:  the above error occurred while allocating sup or pre parameters" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocObjectIdExpr]: bad original tag seen - got " );
	dummy = TKERR_FormatInt( "%d", 
		    (INT_type) original->object_selection_function_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
	
    }

    new->object_selection_function_tag = 
	original->object_selection_function_tag;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_AllocCurrInstanceParms( original, new )
    IN CURR_INSTANCE_PARMS_type *original;
    OUT CURR_INSTANCE_PARMS_type *new;
{
    
    INT_type rval;

    POINTER_type ptr;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( original->parm1_tag ){

    case OBJECT_CLASS_ID_tag:
	/* the object class id is a constituent */
	break;

    case OBJECT_TYPE_tag:
	/* the object type is a VALUE_type */
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocCurrInstanceParms]: bad original parm1 tag seen while allocating a curr instantce parms - got " );
	dummy = TKERR_FormatInt( "%d",
		    (INT_type) original->parm1_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    new->parm1_tag = original->parm1_tag;

    ptr = MM_Malloc( (INT_type) sizeof( struct OBJECT_REF_struct ) );
    if( ptr == ERROR_POINTER ){
	rval = ERROR_INT;
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocCurrInstanceParms]: can not get memory for the parm2 in a curr instance parms" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    new->parm2 = (struct OBJECT_REF_struct *) ptr;

    SlaveRval = ATTR_AllocObjectRef (
		    /* original */
		original->parm2,
		    /* new */
		new->parm2 );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocCurrInstanceParms]: the above error occurred in allocating the parm2 of a curr instance parms" );
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



INT_type ATTR_AllocStringFunction( original, new )
    IN STRING_FUNCTION_type *original;
    OUT STRING_FUNCTION_type *new;
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

    switch( original->function_tag ){

    case MAKE_STRING_tag:
    case UPPER_ALPHA_tag:
    case LOWER_ALPHA_tag:
    case UPPER_ROMAN_tag:
    case LOWER_ROMAN_tag:
	SlaveRval = ATTR_AllocNumericExpr(
			/* original */
		    &original->arg,
			/* new */
		    &new->arg );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocStringFunction]:  the above error occurred in allocating the arg in a string function" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocStringFunction]: bad tag seen in string function - got " );
	dummy = TKERR_FormatInt( "%d",
		    (INT_type) original->function_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    new->function_tag = original->function_tag;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
    

}



INT_type ATTR_AllocNumericExpr( original, new )
    IN NUMERIC_EXPR_type *original;
    OUT NUMERIC_EXPR_type *new;
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

    switch( original->numeric_expr_tag ){

    case NUMERIC_LITERAL_tag:
	/* its just an int */
	break;

    case REF_BINDING_VALUE_tag:
	SlaveRval = ATTR_AllocRefBindingValue(
			/* original */
		    &original->numeric_expr_value.binding_value,
			/* new */
		    &new->numeric_expr_value.binding_value );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocNumericExpr]:  the above error occurred while allocating a ref binding value in a numeric expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case NUMERIC_FUNCTION_tag:
	SlaveRval = ATTR_AllocNumericFunction(
			/* original */
		    &original->numeric_expr_value.function,
			/* new */
		    &new->numeric_expr_value.function );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocNumericExpr]:  the above error occurred while allocating a numeric function in a numeric expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocNumericExpr]: bad tag seen in a numeric expression - got " );
	dummy = TKERR_FormatInt( "%d",
		    (INT_type) original->numeric_expr_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    new->numeric_expr_tag = original->numeric_expr_tag;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type ATTR_AllocNumericFunction( original, new )
    IN NUMERIC_FUNCTION_type *original;
    OUT NUMERIC_FUNCTION_type *new;
{

    INT_type rval;

    POINTER_type ptr;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( original->function_tag ){

    case INCREMENT_tag:
    case DECREMENT_tag:
	ptr = MM_Malloc( (INT_type) sizeof( struct NUMERIC_EXPR_struct ) );
	if( ptr == ERROR_POINTER ){
	    rval = ERROR_INT;
	    TKError = NOMEM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocNumericFunction]:  can not get memory for the incr/decr parameter in a numeric function" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	new->numeric_function_value.incr_decr_parameter = 
	    (struct NUMERIC_EXPR_struct *) ptr;
	SlaveRval = ATTR_AllocNumericExpr(
			/* original */
		    original->numeric_function_value.incr_decr_parameter,
			/* new */
		    new->numeric_function_value.incr_decr_parameter );

	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocNumericFunction]:  the above error occurred while allocating the incr/decr paramter" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case ORDINAL_tag:
	SlaveRval = ATTR_AllocObjectRef(
			/* original */
		    &original->numeric_function_value.ord_parameter,
			/* new */
		    &new->numeric_function_value.ord_parameter );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocNumericFunction]:  the above error occurred while allocating the ordinal in a numeric function" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocNumericFunction]: bad tag seen in numeric function - got " );
	dummy = TKERR_FormatInt( "%d",
		    (INT_type) original->function_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    
    }

    new->function_tag = original->function_tag;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type STRING_EXPR_Free(storage)
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

    if( TheSequence->sequence_tag != SEQUENCE_ATOMIC_STR_EXPR_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[STRING_EXPR_Free]: value does not have a SEQUENCE_ATOMIC_STR_EXPR_tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < TheSequence->length; i++ ){

	SlaveRval = ATTR_FreeAtomicStrExpr( 
		    &TheSequence->sequence_value.atomic_string_exprs[i] );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[STRING_EXPR_Free]: the above error occurred in freeing sequence element " );
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



INT_type ATTR_FreeAtomicStrExpr( original )
    IN ATOMIC_STRING_EXPR_type *original;
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

    switch( original->atomic_string_expr_tag ){

    case STRING_LITERAL_tag:
	SlaveRval = STRING_Free( 
			(POINTER_type)
			original->atomic_string_value.string );	
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeAtomicStrExpr]:  can not free space for the string literal" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case REF_BINDING_VALUE_tag:
	SlaveRval = ATTR_FreeRefBindingValue(
		    &original->atomic_string_value.value );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeAtomicStrExpr]:  the above message occurred in freeing space for a ref binding value in an atomic string expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case STRING_FUNCTION_tag:
	SlaveRval = ATTR_FreeStringFunction(
		    &original->atomic_string_value.function );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeAtomicStrExpr]:  the above message occurred in freeing space for a string function in an atomic string expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_FreeAtomicStrExpr]: bad original tag seen - got " );
	dummy = TKERR_FormatInt( "%d", 
		    (INT_type) original->atomic_string_expr_tag );
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



INT_type ATTR_FreeRefBindingValue( original )
    IN REF_BINDING_VALUE_type *original;
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

    SlaveRval = ATTR_FreeObjectRef( &original->object_ref );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_FreeRefBindingValue]: the above error occurred while freeing the object ref in a ref binding value" );
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



INT_type ATTR_FreeObjectRef( original )
    IN OBJECT_REF_type *original;
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

    switch( original->obj_ref_tag ){

    case OBJECT_ID_tag:
	/* its just a constituent so nothing needs to be freed */
	break;

    case OBJECT_ID_EXPR_tag:
	SlaveRval = ATTR_FreeObjectIdExpr(
		    &original->object_ref_value.object_id_expr );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeObjectRef]:  the above error occurred while freeing an object id expr in an object ref" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_FreeObjRef]: bad obj ref tag seen - got " );
	dummy = TKERR_FormatInt( "%d", (INT_type) original->obj_ref_tag );
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



INT_type ATTR_FreeObjectIdExpr( original )
    IN OBJECT_ID_EXPR_type *original;
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

    switch( original->object_selection_function_tag ){

    case CURRENT_OBJECT_tag:
	/* no parameters !!! */
	break;

    case CURRENT_INSTANCE_tag:
	SlaveRval = ATTR_FreeCurrInstanceParms(
		    &original->parameters.curr_inst_parms );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeObjectIdExpr]:  the above error occurred while freeing current instance parms in an object id expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case SUPERIOR_OBJECT_tag:
    case PRECEDING_OBJECT_tag:
	SlaveRval = ATTR_FreeObjectIdExpr( 
		    original->parameters.sup_or_pre_parms );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeObjectIdExpr]:  the above error occurred while freeing sup or pre parameters" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_FreeObjectIdExpr]: bad original tag seen - got " );
	dummy = TKERR_FormatInt( "%d", 
		    (INT_type) original->object_selection_function_tag );
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



INT_type ATTR_FreeCurrInstanceParms( original )
    IN CURR_INSTANCE_PARMS_type *original;
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

    switch( original->parm1_tag ){

    case OBJECT_CLASS_ID_tag:
	/* the object class id is a constituent */
	break;

    case OBJECT_TYPE_tag:
	/* the object type is a VALUE_type */
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_FreeCurrInstanceParms]: bad original parm1 tag seen while freeing a curr instantce parms - got " );
	dummy = TKERR_FormatInt( "%d",
		    (INT_type) original->parm1_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    SlaveRval = ATTR_FreeObjectRef( original->parm2 );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_FreeCurrInstanceParms]: the above error occurred while freeing the parm2 of a curr instance parms" );
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



INT_type ATTR_FreeStringFunction( original )
    IN STRING_FUNCTION_type *original;
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

    switch( original->function_tag ){

    case MAKE_STRING_tag:
    case UPPER_ALPHA_tag:
    case LOWER_ALPHA_tag:
    case UPPER_ROMAN_tag:
    case LOWER_ROMAN_tag:
	SlaveRval = ATTR_FreeNumericExpr( &original->arg );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeStringFunction]:  the above error occurred while freeing the arg in a string function" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_FreeStringFunction]: bad tag seen in string function - got " );
	dummy = TKERR_FormatInt( "%d",
		    (INT_type) original->function_tag );
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



INT_type ATTR_FreeNumericExpr( original )
    IN NUMERIC_EXPR_type *original;
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

    switch( original->numeric_expr_tag ){

    case NUMERIC_LITERAL_tag:
	/* its just an int */
	break;

    case REF_BINDING_VALUE_tag:
	SlaveRval = ATTR_FreeRefBindingValue(
		    &original->numeric_expr_value.binding_value );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeNumericExpr]:  the above error occurred while freeing a ref binding value in a numeric expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case NUMERIC_FUNCTION_tag:
	SlaveRval = ATTR_FreeNumericFunction(
		    &original->numeric_expr_value.function );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeNumericExpr]:  the above error occurred while freeing a numeric function in a numeric expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_FreeNumericExpr]: bad tag seen in a numeric expression - got " );
	dummy = TKERR_FormatInt( "%d",
		    (INT_type) original->numeric_expr_tag );
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



INT_type ATTR_FreeNumericFunction( original )
    IN NUMERIC_FUNCTION_type *original;
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

    switch( original->function_tag ){

    case INCREMENT_tag:
    case DECREMENT_tag:
	SlaveRval = ATTR_FreeNumericExpr(
		    original->numeric_function_value.incr_decr_parameter );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeNumericFunction]:  the above error occurred while freeing the incr/decr paramter" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case ORDINAL_tag:
	SlaveRval = ATTR_FreeObjectRef(
		    &original->numeric_function_value.ord_parameter );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeNumericFunction]:  the above error occurred while freeing the ordinal in a numeric function" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_FreeNumericFunction]: bad tag seen in numeric function - got " );
	dummy = TKERR_FormatInt( "%d",
		    (INT_type) original->function_tag );
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



INT_type STRING_EXPR_Set(AttrStructPtr, TheStorage)
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

    AttrStructPtr->attribute_value.STRING_EXPR_field =
	(STRING_EXPR_type *) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type STRING_EXPR_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_STRING_EXPR( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type STRING_EXPR_CkVal(constituent, attribute, value, mask)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN POINTER_type value;
	IN PARM_MASK_type mask;
{

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

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

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){
    
    case at_CONTENT_GENERATOR:
	if( !( CONST_IsBasLogNc( TheConstituent ) ||
		CONST_IsBasLogCl( TheConstituent ) ||
		CONST_IsBlockCl( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[STRING_EXPR_CkVal]: Attribute ");
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format(" must go on a basic object class or a basic logical object" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[STRING_EXPR_CkVal]: Atribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" does not have type STRING_EXPR_type");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    TheSequence = (SEQUENCE_type) value;

    if( TheSequence->sequence_tag != SEQUENCE_ATOMIC_STR_EXPR_tag ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[STRING_EXPR_CkVal]: value does not have a SEQUENCE_ATOMIC_STR_EXPR_tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < TheSequence->length; i++ ){

	SlaveRval = ATTR_CkValAtomicStrExpr( 
		    &TheSequence->sequence_value.atomic_string_exprs[i] );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[STRING_EXPR_CkVal]: the above error occurred in checking sequence element " );
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



INT_type ATTR_CkValAtomicStrExpr( original )
    IN ATOMIC_STRING_EXPR_type *original;
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

    switch( original->atomic_string_expr_tag ){

    case STRING_LITERAL_tag:
	if( original->atomic_string_value.string->sequence_tag !=
	    SEQUENCE_CHAR_tag ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CkValAtomicStrExrp]:  the sting literal in an atomic str expr does not have SEQUENCE_CHAR_tag got - " );
	    dummy = TKERR_FormatInt( "%d", (INT_type) 
			original->atomic_string_value.string->sequence_tag );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case REF_BINDING_VALUE_tag:
	SlaveRval = ATTR_CkValRefBindingValue(
		    &original->atomic_string_value.value );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CkValAtomicStrExpr]:  the above message occurred in checking a ref binding value in an atomic string expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case STRING_FUNCTION_tag:
	SlaveRval = ATTR_CkValStringFunction(
		    &original->atomic_string_value.function );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CkValAtomicStrExpr]:  the above message occurred while checking a string function in an atomic string expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CkValAtomicStrExpr]: bad atomic string expr tag seen - got " );
	dummy = TKERR_FormatInt( "%d", 
		    (INT_type) original->atomic_string_expr_tag );
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



INT_type ATTR_CkValRefBindingValue( original )
    IN REF_BINDING_VALUE_type *original;
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

    SlaveRval = ATTR_CkValObjectRef( &original->object_ref );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CkValRefBindingValue]: the above error occurred while ckecking an object ref in a ref binding value" );
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



INT_type ATTR_CkValObjectRef( original )
    IN OBJECT_REF_type *original;
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

    switch( original->obj_ref_tag ){

    case OBJECT_ID_tag:
	/* its just a constituent so nothing needs to be checked */
	break;

    case OBJECT_ID_EXPR_tag:
	SlaveRval = ATTR_CkValObjectIdExpr(
		    &original->object_ref_value.object_id_expr );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CkValObjectRef]:  the above error occurred while checking an object id expr in an object ref" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CkValObjRef]: bad obj ref tag seen - got " );
	dummy = TKERR_FormatInt( "%d", (INT_type) original->obj_ref_tag );
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



INT_type ATTR_CkValObjectIdExpr( original )
    IN OBJECT_ID_EXPR_type *original;
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

    switch( original->object_selection_function_tag ){

    case CURRENT_OBJECT_tag:
	/* no parameters !!! */
	break;

    case CURRENT_INSTANCE_tag:
	SlaveRval = ATTR_CkValCurrInstanceParms(
		    &original->parameters.curr_inst_parms );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CkValObjectIdExpr]:  the above error occurred while checking a current instance parms in an object id expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case SUPERIOR_OBJECT_tag:
    case PRECEDING_OBJECT_tag:
	SlaveRval = ATTR_CkValObjectIdExpr( 
		    original->parameters.sup_or_pre_parms );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CkValObjectIdExpr]:  the above error occurred while checking sup or pre parameters" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CkValObjectIdExpr]: bad object selection function tag seen - got " );
	dummy = TKERR_FormatInt( "%d", 
		    (INT_type) original->object_selection_function_tag );
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



INT_type ATTR_CkValCurrInstanceParms( original )
    IN CURR_INSTANCE_PARMS_type *original;
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

    switch( original->parm1_tag ){

    case OBJECT_CLASS_ID_tag:
	/* the object class id is a constituent */
	break;

    case OBJECT_TYPE_tag:
	/* the object type is a VALUE_type */
	break;

    default:
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CkValCurrInstanceParms]: bad parm1 tag seen while checking a curr instantce parms - got " );
	dummy = TKERR_FormatInt( "%d",
		    (INT_type) original->parm1_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    SlaveRval = ATTR_CkValObjectRef( original->parm2 );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CkValCurrInstanceParms]: the above error occurred while checking the parm2 of a curr instance parms" );
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



INT_type ATTR_CkValStringFunction( original )
    IN STRING_FUNCTION_type *original;
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

    switch( original->function_tag ){

    case MAKE_STRING_tag:
    case UPPER_ALPHA_tag:
    case LOWER_ALPHA_tag:
    case UPPER_ROMAN_tag:
    case LOWER_ROMAN_tag:
	SlaveRval = ATTR_CkValNumericExpr( &original->arg );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CkValStringFunction]:  the above error occurred while checking the arg in a string function" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CkValStringFunction]: bad function_tag seen in string function - got " );
	dummy = TKERR_FormatInt( "%d",
		    (INT_type) original->function_tag );
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



INT_type ATTR_CkValNumericExpr( original )
    IN NUMERIC_EXPR_type *original;
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

    switch( original->numeric_expr_tag ){

    case NUMERIC_LITERAL_tag:
	/* its just an int */
	break;

    case REF_BINDING_VALUE_tag:
	SlaveRval = ATTR_CkValRefBindingValue(
		    &original->numeric_expr_value.binding_value );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CkValNumericExpr]:  the above error occurred while checking a ref binding value in a numeric expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case NUMERIC_FUNCTION_tag:
	SlaveRval = ATTR_CkValNumericFunction(
		    &original->numeric_expr_value.function );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CkValNumericExpr]:  the above error occurred while checking a numeric function in a numeric expr" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CkValNumericExpr]: bad numeric expr tag seen in a numeric expression - got " );
	dummy = TKERR_FormatInt( "%d",
		    (INT_type) original->numeric_expr_tag );
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



INT_type ATTR_CkValNumericFunction( original )
    IN NUMERIC_FUNCTION_type *original;
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

    switch( original->function_tag ){

    case INCREMENT_tag:
    case DECREMENT_tag:
	SlaveRval = ATTR_CkValNumericExpr(
		    original->numeric_function_value.incr_decr_parameter );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CkValNumericFunction]:  the above error occurred while checking the incr/decr paramter in a numeric function" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case ORDINAL_tag:
	SlaveRval = ATTR_CkValObjectRef(
		    &original->numeric_function_value.ord_parameter );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CkValNumericFunction]:  the above error occurred while checking the ordinal in a numeric function" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	dummy = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CkValNumericFunction]: bad function tag seen in numeric function - got " );
	dummy = TKERR_FormatInt( "%d",
		    (INT_type) original->function_tag );
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




INT_type STRING_EXPR_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
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
    dummy = TKERR_Format("[STRING_EXPR_Deflt]: STRING_EXPR_Deflt does not has an iso 8613 default" );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}
