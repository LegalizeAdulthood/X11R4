
/* 
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr13.c,v 1.2 89/10/31 13:44:45 jaap Exp $
$source: $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr13.c,v $
 * Revision 1.2  89/10/31  13:44:45  jaap
 * *** empty log message ***
 * 
 * Revision 1.1  89/09/21  06:17:19  mss
 * Initial revision
 * 
 * Revision 1.7  89/09/12  09:04:46  mss
 * Removed #
 * 
 * Revision 1.6  89/04/21  18:14:47  annm
 * chnages for deleteseq/deletesequencedeeply
 * 
 * Revision 1.5  88/12/13  19:54:20  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.4  88/11/01  16:00:48  annm
 * fix of font monster alloc/copy bug for null sequence value in the sequence of ints field
 * 
 * Revision 1.3  88/10/31  12:54:42  mss
 * Explicitly make new font structure unspecified (needed
 * for sequences)
 * 
 * Revision 1.2  88/10/14  16:08:41  annm
 * 
 * 
 * Revision 1.1  88/09/30  15:44:44  annm
 * Initial revision
 *  

*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr13.c,v $";
#endif


/* Put .c template comment here */

/*

    This file contains the alloc, free, set and get canonical procedures 
    for the SEQUENCE_FONT_PAIR_type.


*/

#define	TK_IMPL_ATTR
#define TK_GVARS_ATTR13
#define TK_TKI_MM
#define TK_TKI_TKERR
#define TK_TKI_SEQ

#include <rnames.h>
#include <acd.h>
#include <mm.h>
#include <tkerr.h>
#include <seq.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR13
#undef TK_TKI_MM
#undef TK_TKI_TKERR
#undef TK_TKI_SEQ


/* put #define's and declarations local to this file here */



POINTER_type SEQUENCE_FONT_PAIR_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    SEQUENCE_type TheSequence;

    SEQUENCE_type NewSequence;

    INT_type SlaveRval;

    INT_type i;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    TheSequence = (SEQUENCE_type) value;

    if( TheSequence == ERROR_SEQUENCE ||
	TheSequence == NULL_SEQUENCE ||
	TheSequence == (SEQUENCE_type) 0 ){
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[SEQUENCE_FONT_PAIR_Alloc]: error, null or 0 valued sequence" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( TheSequence->sequence_tag != SEQUENCE_FONT_PAIR_tag ){
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[SEQUENCE_FONT_PAIR_Alloc]: bad sequence tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    NewSequence = MakeSequence( SEQUENCE_FONT_PAIR_tag,
	    TheSequence->length );
    if( NewSequence == ERROR_SEQUENCE ){
	/* ASSERT: TKError got set */
	rval = ERROR_POINTER;
	goto LEAVE;
    }    

    /* assume that all goes well */
    rval = (POINTER_type) NewSequence;

    for( i = (INT_type) 0; i < TheSequence->length; i++ ){
    
	SlaveRval = ATTR_AllocFontPairBody( 
	    /* prototype */
	    &TheSequence->sequence_value.font_pairs[i],
	    /* clone */
	    &NewSequence->sequence_value.font_pairs[i] );
	if( SlaveRval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    rval = ERROR_POINTER;
	    dummy = DeleteSequence( NewSequence );
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



INT_type ATTR_AllocFontPairBody( prototype, clone )
    IN FONT_PAIR_type *prototype;
    OUT FONT_PAIR_type *clone;
{

    INT_type rval;

    FONT_REFERENCE_type *ProtoFR;
    FONT_REFERENCE_type *CloneFR;

    INT_type SlaveRval;
    SEQUENCE_type SlaveSequence;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    ProtoFR = &prototype->font_reference;
    CloneFR = &clone->font_reference;

    if( ProtoFR->dsclass != NULL_SEQUENCE ){
	if( ProtoFR->dsclass->sequence_tag != SEQUENCE_INT_tag ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[ATTR_AllocFontPairBody]: bad prototype dsclass" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	SlaveSequence = MakeSequence( SEQUENCE_INT_tag,
			    ProtoFR->dsclass->length );
	if( SlaveSequence == ERROR_SEQUENCE ){
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	CloneFR->dsclass = SlaveSequence;
    }
    else {
	CloneFR->dsclass = NULL_SEQUENCE;
    }

    SlaveRval = ATTR_AllocStructuredName( /* ProtoFR */
		    &ProtoFR->fontname,
		    /* CloneFR */ &CloneFR->fontname );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	goto LEAVE;
    }

    SlaveRval = ATTR_AllocStructuredName( /* ProtoFR */
		    &ProtoFR->difntnam,
		    /* CloneFR */ &CloneFR->difntnam );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	goto LEAVE;
    }

    SlaveRval = ATTR_AllocStructuredName( /* ProtoFR */
		    &ProtoFR->typeface,
		    /* CloneFR */ &CloneFR->typeface );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	goto LEAVE;
    }

    SlaveRval = ATTR_AllocStructuredName( /* ProtoFR */
		    &ProtoFR->family,
		    /* CloneFR */ &CloneFR->family );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	goto LEAVE;
    }

    SlaveRval = ATTR_AllocStructuredName( /* ProtoFR */
		    &ProtoFR->mode,
		    /* CloneFR */ &CloneFR->mode );
    if( SlaveRval == ERROR_INT ){
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



INT_type ATTR_AllocStructuredName( prototype, clone )
    IN STRUCTURED_NAME_type *prototype;
    OUT STRUCTURED_NAME_type *clone;
{

    INT_type rval;
    
    INT_type dummy;

    SEQUENCE_type SlaveSequence;

    INT_type SlaveRval;

    INT_type i;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( prototype->structured_name_tag ){

    case STRUCTURED_NAME_ID_tag:
	rval = ATTR_AllocISO9541Id( &prototype->structured_name_value.id,
		&clone->structured_name_value.id );
	clone->structured_name_tag = STRUCTURED_NAME_ID_tag;
	break;

    case STRUCTURED_NAME_SEQ_tag:
	if( prototype->structured_name_value.idseq->sequence_tag !=
	    SEQUENCE_ISO9541_ID_tag ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[ATTR_AllocStructuredName]: bad idseq sequence tag" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	SlaveSequence = MakeSequence( SEQUENCE_ISO9541_ID_tag,
			prototype->structured_name_value.idseq->length );
	if( SlaveSequence == ERROR_SEQUENCE ){
	    rval = ERROR_INT;
	    goto LEAVE;
	}

	clone->structured_name_value.idseq = SlaveSequence;
	clone->structured_name_tag = STRUCTURED_NAME_SEQ_tag;

	for( i = (INT_type) 0; 
	     i < prototype->structured_name_value.idseq->length;
	     i++ ){
	    
	    SlaveRval = ATTR_AllocISO9541Id(
		    &prototype->structured_name_value.idseq->sequence_value.iso9541_ids[i],
		    &clone->structured_name_value.idseq->sequence_value.iso9541_ids[i] );
	    if( SlaveRval == ERROR_INT ){
		rval = ERROR_INT;
		goto LEAVE;
	    } 
	    
	}
	break;

    case STRUCTURED_NAME_UNSPEC_tag:
	clone->structured_name_tag = STRUCTURED_NAME_UNSPEC_tag;
	break;
    
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
	    "[ATTR_AllocStructuredName]: bad structured name tag" );
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



INT_type ATTR_AllocISO9541Id( prototype, clone )
    IN ISO9541_ID_type *prototype;
    OUT ISO9541_ID_type *clone;
{

    INT_type rval;

    INT_type dummy;

    SEQUENCE_type SlaveSequence;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( prototype->iso9541_id_tag ){

    case ID_INT_tag:
	clone->iso9541_id_tag = ID_INT_tag;
	break;

    case ID_STR_tag:

	if( prototype->iso9541_id_value.octet_string->sequence_tag
	    != SEQUENCE_BYTE_tag ){
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		    "[ATTR_AllocISO9541Id]: octet string has bad sequence tag" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}

	SlaveSequence = MakeSequence( SEQUENCE_BYTE_tag,
			prototype->iso9541_id_value.octet_string->length );
	if( SlaveSequence == ERROR_SEQUENCE ){
	    rval = ERROR_INT;
	    goto LEAVE;
	}

	clone->iso9541_id_tag = ID_STR_tag;	
	clone->iso9541_id_value.octet_string = SlaveSequence;
    
	break;
    
    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
		"[ATTR_AllocISO9541Id]: bad iso9541_id_tag" );
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



INT_type SEQUENCE_FONT_PAIR_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = ERROR_INT;
    TKError = UNIMPL_err;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_FONT_PAIR_Set(AttrStructPtr, TheStorage)
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

    AttrStructPtr->attribute_value.SEQUENCE_FONT_PAIR_field = 
	    (SEQUENCE_FONT_PAIR_type) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type SEQUENCE_FONT_PAIR_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_SEQUENCE_FONT_PAIR(AttrStructPtr);

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}
