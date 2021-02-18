
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr5.c,v 1.2 89/10/31 14:04:52 annm Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr5.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr5.c,v $
 * Revision 1.2  89/10/31  14:04:52  annm
 * fixing lint problmes
 * 
 * Revision 1.1  89/09/21  06:23:17  mss
 * Initial revision
 * 
 * Revision 1.14  89/09/12  09:10:22  mss
 * Removed #
 * 
 * Revision 1.13  88/12/13  20:45:29  annm
 * crumb character at top of file
 * 
 * Revision 1.12  88/12/13  19:55:38  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.11  88/09/23  15:12:56  annm
 * Free of construction expr now does a DeleteSequenceDeeply for the construction variants
 * 
 * Revision 1.10  88/09/21  15:47:00  annm
 * fixed bug in ckval for constr expr
 * 
 * Revision 1.9  88/09/19  16:57:13  annm
 * added error message formatting, canonical free and ckval routines
 * 
 * Revision 1.8  88/09/06  13:09:42  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.7  88/08/22  19:44:19  annm
 * no change
 * 
 * Revision 1.6  88/08/10  15:42:42  annm
 * added inheritance from object/style, objectclass/sytle and iso 8613 defaults for the currently implemented aunion.h types
 * 
 * Revision 1.5  88/08/03  13:57:12  annm
 * added setting of destination_mask in the copy's to get the return to the user to work right
 * 
 * Revision 1.4  88/08/01  18:45:26  annm
 * OH BOY WHAT A MESS - THE DASTERLY -I BUG STRIKES AGAIN (AND, OH BOY, IS IT HARD TO FIND... THANK GOD FOR CC -E !!!)
 * 
 * Revision 1.3  88/07/21  18:43:46  annm
 * typo
 * 
 * Revision 1.2  88/07/21  17:40:09  annm
 * added defaults in switches to catch errors, and fixed core leak
 * 
 * Revision 1.1  88/07/21  16:36:42  annm
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr5.c,v $";
#endif


/*

    This file constaint the canonical procedures for the following
    aunion.h types:

	CONSTRUCTION_EXPR_type (good ol' Generator for Subordinates)

    At present, only the Copy, Alloc, Set and Get canonical procedures
    are implemented. The others are still in stubs.c.

*/



#define	TK_IMPL_ATTR
#define TK_GVARS_ATTR5
#define TK_IMPL_CONST
#define TK_TKI_SEQ
#define TK_TKI_MM
#define TK_TKI_TKERR

#include <rnames.h>
#include <apityp.h>
#include <acd.h>
#include <seq.h>
#include <mm.h>
#include <parms.h>
#include <tkerr.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR5
#undef TK_IMPL_CONST
#undef TK_TKI_SEQ
#undef TK_TKI_MM
#undef TK_TKI_TKERR



INT_type CONSTRUCTION_EXPR_Copy(source, source_mask, wanted_mask, 
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	IN OUT POINTER_type destination;

{

    /* the mask is ignored */

    INT_type rval;

    INT_type SlaveRval;	    /* return from lower levels */
    INT_type i;		    /* index thru term sequence */

    CONSTRUCTION_EXPR_type *Source;
    CONSTRUCTION_EXPR_type *Destination;

    INT_type dummy;

#ifdef DEBUG
    If( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    Source = (CONSTRUCTION_EXPR_type *) source;
    Destination = (CONSTRUCTION_EXPR_type *) destination;

    if( Source->construction_tag != Destination->construction_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONSTRUCTION_EXPR_Copy]: source and destination have different tags" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    switch( Source->construction_tag ){
    case TERM_tag:
	SlaveRval = 
	CONSTRUCTION_TERM_Copy( &Source->construction_value.term, 
	    &Destination->construction_value.term );
	if( SlaveRval == ERROR_INT ){
	    /* assert: TKError got set at a lower level */
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	break;
    case SEQUENCE_tag:
    case AGGREGATE_tag:
    case CHOICE_tag:
	/* check tags and lengths of the sequences */
	if( Source->construction_value.construction->sequence_tag !=
		SEQUENCE_CONSTRUCTION_TERM_tag ||
	    Destination->construction_value.construction->sequence_tag !=
		SEQUENCE_CONSTRUCTION_TERM_tag ||
	    Source->construction_value.construction->length !=
	    Destination->construction_value.construction->length ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[CONSTRUCTION_EXPR_Copy]: SEQUENCE_CONSTRUCTION_TERM_tag's differ" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	for ( i=0; i < Source->construction_value.construction->length; i++ ){
	    SlaveRval = CONSTRUCTION_TERM_Copy( 
&Source->construction_value.construction->sequence_value.construction_terms[i],
&Destination->construction_value.construction->sequence_value.construction_terms[i] );
	    if( SlaveRval == ERROR_INT ){
		/* ASSERT: lower level set TKError */
		rval = ERROR_INT;
		goto LEAVE;
	    }
	}
	break;
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[CONSTRUCTION_EXPR_Copy]: bad source tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( destination_mask == (PARM_MASK_type *) 0 ){
	/* ASSERT: We are in a recursion - top level to set
	    the return destination mask 
	*/
    }
    else {
	/* ASSERT: We are the top level */
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



INT_type CONSTRUCTION_TERM_Copy( Source, Destination )
    IN CONSTRUCTION_TERM_type	    *Source;
    IN OUT CONSTRUCTION_TERM_type   *Destination;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
    If( TraceRoutine() ){
	Entering();
    }
#endif

    if( Source->term_tag != Destination->term_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( 
	    "[CONSTRUCTION_TERM_Copy]: source and destination tags don't match" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    
    switch( Source->term_tag ){
    case REQ_tag:
    case OPT_tag:
    case REP_tag:
    case OPT_REP_tag:
	break;
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[CONSTRUCTION_TERM_Copy]: bad tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    rval = CONSTRUCTION_FACTOR_Copy( &Source->factor, &Destination->factor );
    /* ASSERT: if error occurred TKError has been set */

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type CONSTRUCTION_FACTOR_Copy( Source, Destination )
    IN CONSTRUCTION_FACTOR_type *Source;
    IN OUT CONSTRUCTION_FACTOR_type *Destination;
{

    INT_type rval;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
    If( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    if( Source->factor_tag != Destination->factor_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[CONSTRUCTION_FACTOR_Copy]: tags don't match" );
	dummy = TKERR_EndErrMsg();
    
	goto LEAVE;
    }
    
    switch( Source->factor_tag ){
    case OBJECT_CLASS_ID_tag:
	Destination->factor_value.object_class_id =
	    Source->factor_value.object_class_id;
	break;
    case EXPR_tag:
	SlaveRval = CONSTRUCTION_EXPR_Copy( 
			(POINTER_type) Source->factor_value.expr, 
			(PARM_MASK_type) 0, (PARM_MASK_type) 0,
			(PARM_MASK_type *) 0,
			(POINTER_type) Destination->factor_value.expr );
	if( SlaveRval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	break;
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[CONSTRUCTION_FACTOR_Copy]: bad factor tag" );
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



POINTER_type CONSTRUCTION_EXPR_Alloc(value)
    IN POINTER_type value;

{

    CONSTRUCTION_EXPR_type *Source;
    CONSTRUCTION_EXPR_type *Destination;

    POINTER_type rval;

    INT_type SlaveRval;
    INT_type i;

    SEQUENCE_type TheSequence;

    INT_type dummy;

#ifdef DEBUG
    If( TraceRoutine() ){
	Entering();
    }
#endif

    rval = MM_Malloc( (INT_type) sizeof( struct CONSTRUCTION_EXPR_struct) );
    if( rval == ERROR_POINTER ){
	rval = ERROR_POINTER;
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[CONSTRUCTION_EXPR_Alloc]: can get memory" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    Destination = (CONSTRUCTION_EXPR_type *) rval;
    Source = (CONSTRUCTION_EXPR_type *) value;

    switch( Source->construction_tag ){
    case TERM_tag:
	Destination->construction_tag = TERM_tag;
	SlaveRval = 
	CONSTRUCTION_TERM_Alloc( &Source->construction_value.term, 
	    &Destination->construction_value.term );
	if( SlaveRval == ERROR_INT ){
	    /* assert: TKError got set at a lower level */
	    SlaveRval = MM_Free( rval );
	    rval = ERROR_POINTER;
	    goto LEAVE;
	}
	break;
    case SEQUENCE_tag:
	Destination->construction_tag = SEQUENCE_tag;		
	break;
    case AGGREGATE_tag:
	Destination->construction_tag = AGGREGATE_tag;
	break;
    case CHOICE_tag:
	Destination->construction_tag = CHOICE_tag;
	break;
    default:
	rval = ERROR_POINTER;
	TKError = BADPARM_err;
	SlaveRval = MM_Free( rval );
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[CONSTRUCTION_EXPR_Alloc]: bad tag seen" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    /* ASSERT: the construction_tag is legal */
    
    if( Source->construction_tag != TERM_tag ){
	/* tag must be SEQUENCE, AGGREGATE or CHOICE */
	/* allocate the sequence */
	if( Source->construction_value.construction->sequence_tag !=
	    SEQUENCE_CONSTRUCTION_TERM_tag ){
	    rval = ERROR_POINTER;
	    TKError = INTERNAL_err;
	    SlaveRval = MM_Free( rval );
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[CONSTRUCTION_EXPR_Alloc]: source tag not SEQUENCE_CONSTRUCITON_TERM_tag" );
	    dummy = TKERR_EndErrMsg();

	    goto LEAVE;
	}
	TheSequence = MakeSequence( SEQUENCE_CONSTRUCTION_TERM_tag,
			Source->construction_value.construction->length);
	if( TheSequence == ERROR_SEQUENCE ){
	    /* ASSERT: TKError has been set */
	    SlaveRval = MM_Free( rval );
	    rval = ERROR_POINTER;
	    goto LEAVE;
	}
	Destination->construction_value.construction = TheSequence;
	for( i=0; i < Source->construction_value.construction->length; i++ ){
	    SlaveRval = 
	    CONSTRUCTION_TERM_Alloc( 
&Source->construction_value.construction->sequence_value.construction_terms[i],
&Destination->construction_value.construction->sequence_value.construction_terms[i]
 );
	    if( SlaveRval == ERROR_INT ){
		/* ASSERT: TKError got set at a lower level */
		SlaveRval = DeleteSequence( TheSequence );
		SlaveRval = MM_Free( rval );
		rval = ERROR_POINTER;
		goto LEAVE;
	    }
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



INT_type CONSTRUCTION_TERM_Alloc( Source, Destination )
    IN CONSTRUCTION_TERM_type	    *Source;
    IN OUT CONSTRUCTION_TERM_type   *Destination;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
    If( TraceRoutine() ){
	Entering();
    }
#endif

    switch( Source->term_tag ){
    case REQ_tag:
	Destination->term_tag = REQ_tag;
	break;
    case OPT_tag:
	Destination->term_tag = OPT_tag;		
	break;
    case REP_tag:
	Destination->term_tag = REP_tag;
	break;
    case OPT_REP_tag:
	Destination->term_tag = OPT_REP_tag;
	break;
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[CONSTRUCTION_TERM_Alloc]: bad tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    rval = CONSTRUCTION_FACTOR_Alloc( &Source->factor, &Destination->factor );
    /* ASSERT: if error occurs TKError got set */

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type CONSTRUCTION_FACTOR_Alloc( Source, Destination )
    IN CONSTRUCTION_FACTOR_type	    *Source;
    IN OUT CONSTRUCTION_FACTOR_type *Destination;
{

    INT_type rval;
    
    POINTER_type MoreExpr;
	
    INT_type dummy;

#ifdef DEBUG
    If( TraceRoutine() ){
	Entering();
    }
#endif

    switch( Source->factor_tag ){
    case OBJECT_CLASS_ID_tag:
	Destination->factor_tag = OBJECT_CLASS_ID_tag;
	rval = SUCCESS_INT;
	break;
    case EXPR_tag:
	Destination->factor_tag = EXPR_tag;		
	MoreExpr = CONSTRUCTION_EXPR_Alloc( 
		    (POINTER_type) Source->factor_value.expr );
	if( MoreExpr == ERROR_POINTER ){
	    /* ASSERT: TKError has been set */
	    rval = ERROR_INT;
	}
	else {
	    /* save the returned value */
	    Destination->factor_value.expr = 
		(struct CONSTRUCTION_EXPR_struct *) MoreExpr;
	    rval = SUCCESS_INT;
	}
	break;
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONSTRUCTION_FACTOR_Alloc]: bad tag" );
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



INT_type CONSTRUCTION_EXPR_Free(storage)
	POINTER_type storage;
{

    CONSTRUCTION_EXPR_type *TheStorage;

    INT_type rval;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
    If( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    TheStorage = (CONSTRUCTION_EXPR_type *) storage;

    switch( TheStorage->construction_tag ){

    case TERM_tag:
	SlaveRval = 
	CONSTRUCTION_TERM_Free( &TheStorage->construction_value.term );
	if( SlaveRval == ERROR_INT ){
	    /* assert: TKError got set at a lower level */
	    rval = ERROR_INT;
	}
	break;

    case SEQUENCE_tag:
    case AGGREGATE_tag:
    case CHOICE_tag:
	SlaveRval = DeleteSequenceDeeply( (SEQUENCE_type) TheStorage->construction_value.construction );
	if( SlaveRval == ERROR_INT ){
	    /* ASSERT: TkError got set */
	    rval = ERROR_INT;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[CONSTRUCTION_EXPR_Free]: bad tag in construction expression" );
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



INT_type CONSTRUCTION_TERM_Free( TheTerm )
    IN CONSTRUCTION_TERM_type *TheTerm;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
    If( TraceRoutine() ){
	Entering();
    }
#endif

    switch( TheTerm->term_tag ){
    case REQ_tag:
    case OPT_tag:
    case REP_tag:
    case OPT_REP_tag:
	break;
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[CONSTRUCTION_TERM_Free]: bad term tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    rval = CONSTRUCTION_FACTOR_Free( &TheTerm->factor );
    /* ASSERT: if error occurs TKError got set */


LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type CONSTRUCTION_FACTOR_Free( TheFactor )
    IN CONSTRUCTION_FACTOR_type *TheFactor;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
    If( TraceRoutine() ){
	Entering();
    }
#endif

    switch( TheFactor->factor_tag ){
    case OBJECT_CLASS_ID_tag:
	rval = SUCCESS_INT;
	break;
    case EXPR_tag:
	rval = CONSTRUCTION_EXPR_Free( 
		    (POINTER_type) TheFactor->factor_value.expr );
	if( rval == ERROR_INT ){
	    /* ASSERT: TKError has been set */
	    rval = ERROR_INT;
	}
	else {
	    rval = SUCCESS_INT;
	}
	break;
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[CONSTRUCTION_FACTOR_Free]: bad factor tag" );
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



INT_type CONSTRUCTION_EXPR_Set(AttrStructPtr, TheStorage)
	IN OUT ATTR_attribute_t	AttrStructPtr;
	IN     POINTER_type	TheStorage;
{

    INT_type rval;

#ifdef DEBUG
    If( TraceRoutine() ){
	Entering();
    }
#endif

    rval = SUCCESS_INT;

    AttrStructPtr->attribute_value.CONSTRUCTION_EXPR_field =
	    (CONSTRUCTION_EXPR_type *) TheStorage;
LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



POINTER_type CONSTRUCTION_EXPR_Get(AttrStructPtr)
	IN ATTR_attribute_t	AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
    If( TraceRoutine() ){
	Entering();
    }
#endif

    rval = (POINTER_type) GET_CONSTRUCTION_EXPR(AttrStructPtr);
LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return( rval );

}



INT_type CONSTRUCTION_EXPR_CkVal(constituent, attribute, value, mask)
	CONSTITUENT constituent;
	INT_type attribute;
	POINTER_type value;
	PARM_MASK_type mask;
{

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    INT_type SlaveRval;

    INT_type i;

    INT_type dummy;

    CONSTRUCTION_EXPR_type *TheValue;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    if( !( CONST_IsComposite( TheConstituent ) &&
	    CONST_IsGeneric( TheConstituent ) ) ){
	rval = ERROR_INT;
	TKError = BADATTR_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
		"[CONSTRUCTION_EXPR_CkVal]: Construction expressions must go on composite object classes" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheValue = (CONSTRUCTION_EXPR_type *) value;

    switch( TheValue->construction_tag ){

    case TERM_tag:

	SlaveRval = CONSTRUCTION_TERM_CkVal( constituent, attribute,
		    &TheValue->construction_value.term, mask );
	if( SlaveRval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    rval = ERROR_INT;
	}
	break;

    case SEQUENCE_tag:
    case AGGREGATE_tag:
    case CHOICE_tag:

	for( i = 0; i < TheValue->construction_value.construction->length; i++ ){
	    SlaveRval = CONSTRUCTION_TERM_CkVal( constituent, attribute,
		&TheValue->construction_value.construction->sequence_value.construction_terms[i],
		mask );
	    if( SlaveRval == ERROR_INT ){
		/* ASSERT: TKError got set */
		rval = ERROR_INT;
		goto LEAVE;
	    }
	}

	break;

    default:
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[CONSTRUCTION_EXPR_CkVal]: bad tag in construction expression" );
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



INT_type CONSTRUCTION_TERM_CkVal( constituent, attribute, TheTerm, mask )
    IN CONSTITUENT constituent;
    IN INT_type attribute;
    IN CONSTRUCTION_TERM_type *TheTerm;
    IN PARM_MASK_type mask;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    switch( TheTerm->term_tag ){
    case REQ_tag:
    case OPT_tag:
    case REP_tag:
    case OPT_REP_tag:
	break;
    default:
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[CONSTRUCTION_TERM_CkVal]: bad term tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    rval = CONSTRUCTION_FACTOR_CkVal( constituent, attribute, 
		&TheTerm->factor, mask );
    /* ASSERT: if error occurs TKError got set */


LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type CONSTRUCTION_FACTOR_CkVal( constituent, attribute, TheFactor, mask )
    IN CONSTITUENT constituent;
    IN INT_type attribute;
    IN CONSTRUCTION_FACTOR_type *TheFactor;
    IN PARM_MASK_type mask;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
    If( TraceRoutine() ){
	Entering();
    }
#endif

    switch( TheFactor->factor_tag ){
    case OBJECT_CLASS_ID_tag:
	rval = SUCCESS_INT;
	break;
    case EXPR_tag:
	rval = CONSTRUCTION_EXPR_CkVal( constituent, attribute,
		    (POINTER_type) TheFactor->factor_value.expr, mask );
	if( rval == ERROR_INT ){
	    /* ASSERT: TKError has been set */
	    rval = ERROR_INT;
	}
	else {
	    rval = SUCCESS_INT;
	}
	break;
    default:
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[CONSTRUCTION_FACTOR_CkVal]: bad factor tag" );
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



INT_type CONSTRUCTION_EXPR_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
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
    TKError = INTERNAL_err;
    dummy = TKERR_StartErrMsg();
    dummy = TKERR_Format("[CONSTRUCTION_EXPR_Deflt]: don't have iso 8613 defaults" );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}
