
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr22.c,v 1.2 89/10/31 13:44:55 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr22.c,v $

(c) Copyright 1988 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	attr22.c,v $
 * Revision 1.2  89/10/31  13:44:55  jr
 * Casting problems & others.
 * 
 * Revision 1.1  89/09/21  06:20:33  mss
 * Initial revision
 * 
 * Revision 1.10  89/09/12  09:07:30  mss
 * Removed #
 * 
 * Revision 1.9  89/04/21  18:16:07  annm
 * chnages for deleteseq/deletesequencedeeply
 * 
 * Revision 1.8  89/04/11  15:33:40  annm
 * I can't remember what I did here
 * 
 * Revision 1.7  89/03/28  13:33:30  mss
 * Fixed number of parameters for Tkerr call
 * 
 * Revision 1.6  89/03/09  15:51:17  annm
 * changes for the new DAPD stuff
 * 
 * Revision 1.5  89/02/23  20:38:12  annm
 * changes for new DVL stuff
 * 
 * Revision 1.4  89/02/09  17:43:31  annm
 * minor tweek
 * 
 * Revision 1.3  89/02/09  15:07:38  annm
 * added canonical procedures for dp_LOCAL_FILE_REFERENCE_type and at_BORDER_type
 * 
 * Revision 1.2  89/02/06  16:13:52  annm
 * added canonical procedures for SEQUENCE_STRING_OR_ID_type
 * 
 * Revision 1.1  89/02/02  16:46:58  annm
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr22.c,v $";
#endif



#define TK_IMPL_ATTR
#define TK_GVARS_ATTR22
#define TK_TKI_TKERR
#define TK_TKI_STR
#define TK_TKI_SEQ
#define TK_IMPL_CONST
#define TK_TKI_MM

#include <rnames.h>
#include <acd.h>
#include <tkerr.h>
#include <str.h>
#include <seq.h>
#include <const.h>
#include <mm.h>

#include <parms.h>
#include <values.h>

#include <attr0.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR22
#undef TK_TKI_TKERR
#undef TK_TKI_STR
#undef TK_TKI_SEQ
#undef TK_IMPL_CONST
#undef TK_TKI_MM

/*
    This file contains the canonical aunion.h operations for the following 
	types:

	STRING_OR_ID_type
	SEQUENCE_STRING_OR_ID_type
	dp_LOCAL_FILE_REFERENCE_type
	at_BORDER_type


*/



INT_type STRING_OR_ID_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type destination;
{

    INT_type rval;

    STRING_OR_ID_type *Source;
    STRING_OR_ID_type *Destination;

    INT_type SlaveRval;

    PARM_MASK_type dummy_mask;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    Source = (STRING_OR_ID_type *) source;
    Destination = (STRING_OR_ID_type *) destination;

    if( Source->string_or_id_tag != STRING_tag &&
	Source->string_or_id_tag != ID_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt("[STRING_OR_ID_Copy]: bad source tag of %d",
		(INT_type) Source->string_or_id_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( Destination->string_or_id_tag != STRING_tag &&
	Destination->string_or_id_tag != ID_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt("[STRING_OR_ID_Copy]: bad source tag of %d",
		(INT_type) Source->string_or_id_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( Source->string_or_id_tag != Destination->string_or_id_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt("[STRING_OR_ID_Copy]: source and destination tags do not match - source tag is %d", (INT_type) Source->string_or_id_tag );
	dummy = TKERR_FormatInt(" and destination tag is %d", 
		    (INT_type) Destination->string_or_id_tag);
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    switch( Source->string_or_id_tag ){
	
    case STRING_tag:
	SlaveRval = STRING_Copy( (POINTER_type) Source->string_or_id_value.string,
		    /* source mask */ (PARM_MASK_type) 0,
		    /* wanted mask */ (PARM_MASK_type) 0,
		    /* dest mask */	&dummy_mask,
		    (POINTER_type) Destination->string_or_id_value.id );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	break;

    case ID_tag:
	SlaveRval = SEQUENCE_INT_Copy( (POINTER_type) Source->string_or_id_value.string,
		    /* source mask */ (PARM_MASK_type) 0,
		    /* wanted mask */ (PARM_MASK_type) 0,
		    /* dest mask */	&dummy_mask,
		    (POINTER_type) Destination->string_or_id_value.id );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    goto LEAVE;
	}
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



POINTER_type STRING_OR_ID_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    INT_type dummy;

    STRING_OR_ID_type *OldValue;

    STRING_OR_ID_type *NewValue;

    INT_type SlaveRval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Malloc( (INT_type) sizeof( STRING_OR_ID_type) );
    if( rval == ERROR_POINTER ){
	rval = ERROR_POINTER;
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[STRING_OR_ID_Alloc]: can not get memory for the string or id structure" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    OldValue = (STRING_OR_ID_type *) value;
    NewValue = (STRING_OR_ID_type *) rval;

    SlaveRval = ATTR_Alloc_String_or_id_value( OldValue, NewValue );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_POINTER;
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



INT_type ATTR_Alloc_String_or_id_value( Source, Destination )
    IN STRING_OR_ID_type *Source;
    OUT STRING_OR_ID_type *Destination;
{

    INT_type rval;

    POINTER_type Ptr;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( Source->string_or_id_tag ){
    
    case STRING_tag:
	Ptr = STRING_Alloc( (POINTER_type) 
		    Source->string_or_id_value.string );
	if( Ptr == ERROR_POINTER ){
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	Destination->string_or_id_tag = STRING_tag;
	Destination->string_or_id_value.string = (STRING_type) Ptr;
	break;

    case ID_tag:
	Ptr = SEQUENCE_INT_Alloc( (POINTER_type)
		    Source->string_or_id_value.id );
	if( Ptr == ERROR_POINTER ){
	    rval = ERROR_INT;
	    goto LEAVE;
	}
	Destination->string_or_id_tag = ID_tag;
	Destination->string_or_id_value.id = (SEQUENCE_INT_type) Ptr;
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt("[ATTR_Alloc_String_or_id_value]: source with bad tag of %d",
	    (INT_type) Source->string_or_id_tag );
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



INT_type STRING_OR_ID_Free(storage)
	IN POINTER_type storage;
{

    INT_type rval;

    INT_type dummy;

    STRING_OR_ID_type *TheValue;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheValue = (STRING_OR_ID_type *) storage;

    dummy = ATTR_Free_String_or_id_value( TheValue );

    dummy = MM_Free((POINTER_type) TheValue);

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type ATTR_Free_String_or_id_value( TheValue )
    IN STRING_OR_ID_type *TheValue;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( TheValue->string_or_id_tag ){
    
    case STRING_tag:
	dummy = STRING_Free( (POINTER_type)
		    TheValue->string_or_id_value.string );
	break;

    case ID_tag:
	dummy = SEQUENCE_INT_Free( (POINTER_type)
		    TheValue->string_or_id_value.id );
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt("[ATTR_Free_String_of_id_value]: string of id with bad tag of %d",
	    (INT_type) TheValue->string_or_id_tag );
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



INT_type STRING_OR_ID_Set(AttrStructPtr, TheStorage)
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

    AttrStructPtr->attribute_value.STRING_OR_ID_field =
	(STRING_OR_ID_type *) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type STRING_OR_ID_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_STRING_OR_ID( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type STRING_OR_ID_CkVal(constituent, attribute, value, mask)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN POINTER_type value;
	IN PARM_MASK_type mask;
{

    /*
	Attributes with type STRING_OR_ID_type:
	    dp_DOCUMENT_REFERENCE
    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    INT_type SlaveRval;

    INT_type dummy;

    STRING_OR_ID_type *TheValue;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;
    
    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ) {
    
    case dp_DOCUMENT_REFERENCE:
	if( !( CONST_IsDocProf( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[STRING_OR_ID_CkVal]: ");
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
	dummy = TKERR_Format( "[STRING_OR_ID_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " doesn't have STRING_OR_ID_type" );
	dummy = TKERR_EndErrMsg();	
	goto LEAVE;
    }

    TheValue = (STRING_OR_ID_type *) value;
    
    SlaveRval = ATTR_Ck_String_or_id_value( attribute, TheValue );
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



INT_type ATTR_Ck_String_or_id_value( attribute, TheValue )
    IN INT_type attribute;
    IN STRING_OR_ID_type *TheValue;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( TheValue->string_or_id_tag ){
    
    case STRING_tag:
	if( TheValue->string_or_id_value.string->sequence_tag !=
		SEQUENCE_CHAR_tag ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_Ck_String_or_id_value]: the string is not a sequence of char" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case ID_tag:
	if( TheValue->string_or_id_value.id->sequence_tag !=
		SEQUENCE_INT_tag ){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_Ck_String_or_id_value]: the id is not a sequence of int" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_Ck_String_or_id_value]: " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_FormatInt( " value has a bad tag of %d",
		(INT_type) TheValue->string_or_id_tag );
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



INT_type STRING_OR_ID_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	INT_type attribute;
	PARM_MASK_type wanted_mask;
	PARM_MASK_type *destination_mask;
	POINTER_type *destination;
{

    /*
	Attributes with type STRING_OF_ID_type:
	    dp_DOCUMENT_REFERENCE
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
    dummy = TKERR_Format("[STRING_OR_ID_Deflt]: can not get iso 8613 default" );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_STRING_OR_ID_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type destination;
{

    INT_type rval;

    INT_type dummy;

    INT_type SlaveRval;

    SEQUENCE_type Source;
    SEQUENCE_type Destination;

    INT_type i;

    PARM_MASK_type dummy_mask;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    Source = (SEQUENCE_type) source;
    Destination = (SEQUENCE_type) destination;

    if( Source->sequence_tag != SEQUENCE_STRING_OR_ID_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt("[SEQUENCE_STRING_OF_ID_Copy]: bad source sequence tag of %d",
		(INT_type) Source->sequence_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( Destination->sequence_tag != SEQUENCE_STRING_OR_ID_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt("[SEQUENCE_STRING_OF_ID_Copy]: bad destination sequence tag of %d",
		(INT_type) Destination->sequence_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( Source->length != Destination->length ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt("[SEQUENCE_STRING_OR_ID_Copy]: source and destination length do not match - source length is %d", (INT_type) Source->length );
	dummy = TKERR_FormatInt(" and destination length is %d", 
		    Destination->length );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < Source->length; i++ ){
	
	SlaveRval = STRING_OR_ID_Copy( (POINTER_type)
	    &Source->sequence_value.string_or_ids[i],
	    /* source mask */ (PARM_MASK_type) 0,
	    /* wanted mask */ (PARM_MASK_type) 0,
	    /* dest mask */	&dummy_mask,
	    (POINTER_type)
	    &Destination->sequence_value.string_or_ids[i] );
	if( SlaveRval == ERROR_INT ){
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



POINTER_type SEQUENCE_STRING_OR_ID_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    SEQUENCE_type OldSequence;
    SEQUENCE_type NewSequence;

    INT_type dummy;

    INT_type SlaveRval;

    INT_type i;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    OldSequence = (SEQUENCE_type) value;

    if( OldSequence->sequence_tag != SEQUENCE_STRING_OR_ID_tag ){
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_FormatInt("[SEQUENCE_STRING_OR_ID_Alloc]: bad sequence tag of %d",
	    (INT_type) OldSequence->sequence_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    NewSequence = MakeSequence( SEQUENCE_STRING_OR_ID_tag,
		OldSequence->length );
    if( NewSequence == ERROR_SEQUENCE ){
	rval = ERROR_POINTER;
	goto LEAVE;
    }

    rval = (POINTER_type) NewSequence;

    for( i = (INT_type) 0; i < OldSequence->length; i++ ){
	
	SlaveRval = ATTR_Alloc_String_or_id_value( 
		&OldSequence->sequence_value.string_or_ids[i],
		&NewSequence->sequence_value.string_or_ids[i] );
	if( SlaveRval == ERROR_INT ){
	    /* CORE LEAK HERE - should free the stuff already made */
	    rval = ERROR_POINTER;
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



INT_type SEQUENCE_STRING_OR_ID_Free(storage)
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



INT_type SEQUENCE_STRING_OR_ID_Set(AttrStructPtr, TheStorage)
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

    AttrStructPtr->attribute_value.SEQUENCE_STRING_OR_ID_field =
	(SEQUENCE_STRING_OR_ID_type) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type SEQUENCE_STRING_OR_ID_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_SEQUENCE_STRING_OR_ID( AttrStructPtr );


LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_STRING_OR_ID_CkVal(constituent, attribute, value, mask)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN POINTER_type value;
	IN PARM_MASK_type mask;
{

    /*
	Attributes with type SEQUENCE_STRING_OR_ID_Deflt_type:
	    dp_SUPERSEDED_DOCUMENTS,
	    dp_REFS_TO_OTHER_DOCUMENTS
    */

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

    switch( attribute ) {
    
    case dp_SUPERSEDED_DOCUMENTS:
    case dp_REFS_TO_OTHER_DOCS:
	if( !( CONST_IsDocProf( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[SEQUENCE_STRING_OR_ID_CkVal]: ");
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
	dummy = TKERR_Format( "[SEQUENCE_STRING_OR_ID_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " doesn't have SEQUENCE_STRING_OR_ID_type" );
	dummy = TKERR_EndErrMsg();	
	goto LEAVE;
    }

    TheSequence = (SEQUENCE_STRING_OR_ID_type) value;

    if( TheSequence->sequence_tag != SEQUENCE_STRING_OR_ID_tag ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_STRING_OR_ID_CkVal]: " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_FormatInt( " have a bad sequence tag of %d",
		    (INT_type) TheSequence->sequence_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < TheSequence->length; i++ ){
	
	SlaveRval = ATTR_Ck_String_or_id_value( attribute, 
	(STRING_OR_ID_type *) &TheSequence->sequence_value.string_or_ids[i] );

	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
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



INT_type SEQUENCE_STRING_OR_ID_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type *destination;
{

    /*
	Attributes with type SEQUENCE_STRING_OR_ID_Deflt_type:
	    dp_SUPERSEDED_DOCUMENTS,
	    dp_REFS_TO_OTHER_DOCUMENTS
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
    dummy = TKERR_Format("[STRING_OR_ID_Deflt]: can not get iso 8613 default" );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type dp_LOCAL_FILE_REFERENCE_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
    	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type destination;
{

    INT_type rval;

    dp_LOCAL_FILE_REFERENCE_type *Source;
    dp_LOCAL_FILE_REFERENCE_type *Destination;

    INT_type SlaveRval;

    INT_type dummy;

    PARM_MASK_type dummy_mask;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    Source = (dp_LOCAL_FILE_REFERENCE_type *) source;
    Destination = (dp_LOCAL_FILE_REFERENCE_type *) destination;

    SlaveRval = STRING_Copy( (POINTER_type) Source->filename,
		    /* source mask */ (PARM_MASK_type) 0,
		    /* wanted mask */ (PARM_MASK_type) 0,
		    /* dest mask */ &dummy_mask,
		    (POINTER_type) Destination->filename );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[dp_LOCAL_FILE_REFERENCE_Copy]: can no copy the filename" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    SlaveRval = STRING_Copy( (POINTER_type) Source->location,
		    /* source mask */ (PARM_MASK_type) 0,
		    /* wanted mask */ (PARM_MASK_type) 0,
		    /* dest mask */ &dummy_mask,
		    (POINTER_type) Destination->location );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[dp_LOCAL_FILE_REFERENCE_Copy]: can no copy the location" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    SlaveRval = STRING_Copy( (POINTER_type) Source->user_comments,
		    /* source mask */ (PARM_MASK_type) 0,
		    /* wanted mask */ (PARM_MASK_type) 0,
		    /* dest mask */ &dummy_mask,
		    (POINTER_type) Destination->user_comments );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[dp_LOCAL_FILE_REFERENCE_Copy]: can no copy the user comments" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
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



POINTER_type dp_LOCAL_FILE_REFERENCE_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    POINTER_type SlaveRval;

    dp_LOCAL_FILE_REFERENCE_type *OldRef;
    dp_LOCAL_FILE_REFERENCE_type *NewRef;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    SlaveRval = MM_Malloc( (INT_type) sizeof( dp_LOCAL_FILE_REFERENCE_type ) );
    if( SlaveRval == ERROR_POINTER ){
	rval = ERROR_POINTER;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[dp_LOCAL_FILE_REFERENCE_Alloc]: can not get space for the dp_LOCAL_FILE_REFERENCE struct" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    OldRef = (dp_LOCAL_FILE_REFERENCE_type *) value;
    NewRef = (dp_LOCAL_FILE_REFERENCE_type *) SlaveRval;

    SlaveRval = STRING_Alloc( (POINTER_type) OldRef->filename );
    if( SlaveRval == ERROR_POINTER ){
	rval = ERROR_POINTER;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[dp_LOCAL_FILE_REFERENCE_Alloc]: can not allocate the filename" );
	dummy = TKERR_EndErrMsg();
	dummy = MM_Free( (POINTER_type) NewRef );
	goto LEAVE;
    }
    NewRef->filename = (STRING_type) SlaveRval;

    SlaveRval = STRING_Alloc( (POINTER_type) OldRef->location );
    if( SlaveRval == ERROR_POINTER ){
	rval = ERROR_POINTER;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[dp_LOCAL_FILE_REFERENCE_Alloc]: can not allocate the location" );
	dummy = TKERR_EndErrMsg();
	dummy = STRING_Free( (POINTER_type) NewRef->filename );
	dummy = MM_Free( (POINTER_type) NewRef );
	goto LEAVE;
    }
    NewRef->location = (STRING_type) SlaveRval;

    SlaveRval = STRING_Alloc( (POINTER_type) OldRef->user_comments );
    if( SlaveRval == ERROR_POINTER ){
	rval = ERROR_POINTER;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[dp_LOCAL_FILE_REFERENCE_Alloc]: can not allocate the user comments" );
	dummy = TKERR_EndErrMsg();
	dummy = STRING_Free( (POINTER_type) NewRef->filename );
	dummy = STRING_Free( (POINTER_type) NewRef->location );
	dummy = MM_Free( (POINTER_type) NewRef );
	goto LEAVE;
    }
    NewRef->user_comments = (STRING_type) SlaveRval;

    rval = (POINTER_type) NewRef;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type dp_LOCAL_FILE_REFERENCE_Free(storage)
	IN POINTER_type storage;
{

    INT_type rval;

    dp_LOCAL_FILE_REFERENCE_type *TheRef;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheRef = (dp_LOCAL_FILE_REFERENCE_type *) storage;

    dummy = DeleteString( TheRef->filename );

    dummy = DeleteString( TheRef->location );

    dummy = DeleteString( TheRef->user_comments );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type dp_LOCAL_FILE_REFERENCE_Set(AttrStructPtr, TheStorage)
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

    AttrStructPtr->attribute_value.dp_LOCAL_FILE_REFERENCE_field =
	(dp_LOCAL_FILE_REFERENCE_type *) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type dp_LOCAL_FILE_REFERENCE_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_dp_LOCAL_FILE_REFERENCE( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type dp_LOCAL_FILE_REFERENCE_CkVal(constituent, attribute, value, mask)
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

    switch( attribute ) {
    
    case dp_LOCAL_FILE_REFERENCE:
	if( !( CONST_IsDocProf( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[dp_LOCAL_FILE_REFERENCE_CkVal]: ");
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
	dummy = TKERR_Format( "[dp_LOCAL_FILE_REFERENCE_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " doesn't have dp_LOCAL_FILE_REFERENCE_type" );
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



INT_type dp_LOCAL_FILE_REFERENCE_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN PARM_MASK_type wanted_mask;
	IN PARM_MASK_type *destination_mask;
	IN POINTER_type *destination;
{

    /*
	Attributes with type dp_LOCAL_FILE_REFERENCE_type that do NOT default:
	    dp_LOCAL_FILE_REFERENCE
	    
	Attributes with type dp_LOCAL_FILE_REFERENCE that DO default:

	The masks are irrelevent;
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
    dummy = TKERR_Format("[dp_LOCAL_FILE_REFERENCE_Deflt]: there is no iso 8613 default" );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type at_BORDER_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type destination;
{

    INT_type rval;

    at_BORDER_type *Source;
    at_BORDER_type *Destination;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    if( source_mask == NO_PARMS ){
	/* 
	    Because of defaulting with related parms,
	    this stops the inheritance.
	*/
	*destination_mask = NO_PARMS;
	goto LEAVE;
    }

    Source = (at_BORDER_type *) source;
    Destination = (at_BORDER_type *) destination;

    if( source_mask & LEFT_HAND_EDGE_parm &&
	wanted_mask & LEFT_HAND_EDGE_parm &&
	!( *destination_mask & LEFT_HAND_EDGE_parm ) ){

	rval = ATTR_BORDER_INFO_Copy( &Source->left_hand_edge,
		&Destination->left_hand_edge );
	if( rval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    goto LEAVE;
	}

    	*destination_mask |= LEFT_HAND_EDGE_parm;
    }

    if( source_mask & RIGHT_HAND_EDGE_parm &&
	wanted_mask & RIGHT_HAND_EDGE_parm &&
	!( *destination_mask & RIGHT_HAND_EDGE_parm ) ){

	rval = ATTR_BORDER_INFO_Copy( &Source->right_hand_edge,
		    &Destination->right_hand_edge );
	if( rval == ERROR_INT ){
	    /* ASSERT: TKError got set */		    
	    goto LEAVE;
	}

	*destination_mask |= RIGHT_HAND_EDGE_parm;

    }

    if( source_mask & BORDER_TRAILING_EDGE_parm &&
	wanted_mask & BORDER_TRAILING_EDGE_parm &&
	!( *destination_mask & BORDER_TRAILING_EDGE_parm ) ){

	rval = ATTR_BORDER_INFO_Copy( &Source->trailing_edge,
		    &Destination->trailing_edge );
	if( rval == ERROR_INT ){
	    /* ASSERT: TKError got set */		    
	    goto LEAVE;
	}

	*destination_mask |= BORDER_TRAILING_EDGE_parm;

    }

    if( source_mask & BORDER_LEADING_EDGE_parm &&
	wanted_mask & BORDER_LEADING_EDGE_parm &&
	!( *destination_mask & BORDER_LEADING_EDGE_parm ) ){

	rval = ATTR_BORDER_INFO_Copy( &Source->leading_edge,
		    &Destination->leading_edge );
	if( rval == ERROR_INT ){
	    /* ASSERT: TKError got set */		    
	    goto LEAVE;
	}

	*destination_mask |= BORDER_LEADING_EDGE_parm;

    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type ATTR_BORDER_INFO_Copy( Source, Destination )
    IN BORDER_INFO_type *Source;
    IN BORDER_INFO_type *Destination;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    Destination->is_null = Source->is_null;

    if( Source->is_null ){
	goto LEAVE;
    }

    Destination->border_line_width = Source->border_line_width;
    Destination->border_line_type = Source->border_line_type;
    Destination->border_freespace_width = Source->border_freespace_width;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



POINTER_type at_BORDER_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Malloc( (INT_type) sizeof( at_BORDER_type) );
    if( rval == ERROR_POINTER ){
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[at_BORDER_Alloc]: can not get memory for the at_BORDER_type" );
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



INT_type at_BORDER_Free(storage)
	IN POINTER_type storage;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    dummy = MM_Free( storage );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type at_BORDER_Set(AttrStructPtr, TheStorage)
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

    AttrStructPtr->attribute_value.at_BORDER_field =
	(at_BORDER_type *) TheStorage;


LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type at_BORDER_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_at_BORDER( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type at_BORDER_CkVal(constituent, attribute, value, mask)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN POINTER_type value;
	IN PARM_MASK_type mask;
{

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    at_BORDER_type *TheValue;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){
    
    case at_BORDER:
	if( !( CONST_IsFrame( TheConstituent ) ||
		CONST_IsBlock( TheConstituent ) ||
		CONST_IsDVLFrame( TheConstituent ) ||
		CONST_IsDVLBlock( TheConstituent ) ||
		CONST_IsDAPDBlock( TheConstituent ) ||
		CONST_IsPresentStyle( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[at_BORDER_CkVal]: at_Border must go on a frame, block or presentation style" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format(
		"[at_BORDER_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" does not have at_BORDER_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheValue = (at_BORDER_type *) value;

    /* from this point on attribute is assumed to be at_BORDER */

    if( mask & BORDER_LEADING_EDGE_parm ){
	
	rval = ATTR_BORDER_INFO_CkVal( &TheValue->leading_edge );
	if( rval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[at_BORDER_CkVal]: The above error occurred in the leading edge" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    if( mask & LEFT_HAND_EDGE_parm ){
	
	rval = ATTR_BORDER_INFO_CkVal( &TheValue->left_hand_edge );
	if( rval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[at_BORDER_CkVal]: The above error occurred in the left hand edge" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    if( mask & RIGHT_HAND_EDGE_parm ){
	
	rval = ATTR_BORDER_INFO_CkVal( &TheValue->right_hand_edge );
	if( rval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[at_BORDER_CkVal]: The above error occurred in the right hand edge" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    if( mask & BORDER_TRAILING_EDGE_parm ){
	
	rval = ATTR_BORDER_INFO_CkVal( &TheValue->trailing_edge );
	if( rval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[at_BORDER_CkVal]: The above error occurred in the trailing edge" );
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



INT_type ATTR_BORDER_INFO_CkVal( Value )
    IN BORDER_INFO_type *Value;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    if( Value->is_null != BOOL_true &&
	Value->is_null != BOOL_false ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_BORDER_INFO_CkVal]: is_null field is neither BOOL_true or BOOL_false" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( Value->is_null == BOOL_true ){
	goto LEAVE;
    }

    if( Value->border_line_width < (INT_type) 0 ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_BORDER_INFO_CkVal]: border_line_width is negative" );
	dummy = TKERR_FormatInt(" - got %d", Value->border_line_width );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( Value->border_line_type != at_BORDER_solid &&
	Value->border_line_type != at_BORDER_dashed &&
	Value->border_line_type != at_BORDER_dot &&
	Value->border_line_type != at_BORDER_dash_dot &&
	Value->border_line_type != at_BORDER_dash_dot_dot &&
	Value->border_line_type != at_BORDER_invisible ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_BORDER_INFO_CkVal]: border_line_type is illegal" );
	dummy = TKERR_FormatInt(" - got %d", (INT_type) Value->border_line_type );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( Value->border_freespace_width < (INT_type) 0 ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_BORDER_INFO_CkVal]: border_freespace_width is negative" );
	dummy = TKERR_FormatInt(" - got %d", Value->border_freespace_width );
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



INT_type at_BORDER_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type *destination;
{

    INT_type rval;

    at_BORDER_type *Destination;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    if( *destination == (POINTER_type) 0 ){
	*destination = MM_Malloc( (INT_type) sizeof( at_BORDER_type ) );
	if( *destination == ERROR_POINTER ){
	    rval = ERROR_INT;
	    TKError = NOMEM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format(
		"[at_BORDER_Deflt]: can not get memory" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    Destination = (at_BORDER_type *) *destination;

    if( wanted_mask & LEFT_HAND_EDGE_parm &&
	!( *destination_mask & LEFT_HAND_EDGE_parm ) ){

	rval = ATTR_BORDER_INFO_Deflt( &Destination->left_hand_edge );
	if( rval == ERROR_INT ){
	    /* ASSERT: TKError got set */
	    goto LEAVE;
	}

    	*destination_mask |= LEFT_HAND_EDGE_parm;
    }

    if( wanted_mask & RIGHT_HAND_EDGE_parm &&
	!( *destination_mask & RIGHT_HAND_EDGE_parm ) ){

	rval = ATTR_BORDER_INFO_Deflt( &Destination->right_hand_edge );
	if( rval == ERROR_INT ){
	    /* ASSERT: TKError got set */		    
	    goto LEAVE;
	}

	*destination_mask |= RIGHT_HAND_EDGE_parm;

    }

    if( wanted_mask & BORDER_TRAILING_EDGE_parm &&
	!( *destination_mask & BORDER_TRAILING_EDGE_parm ) ){

	rval = ATTR_BORDER_INFO_Deflt( &Destination->trailing_edge );
	if( rval == ERROR_INT ){
	    /* ASSERT: TKError got set */		    
	    goto LEAVE;
	}

	*destination_mask |= BORDER_TRAILING_EDGE_parm;

    }

    if( wanted_mask & BORDER_LEADING_EDGE_parm &&
	!( *destination_mask & BORDER_LEADING_EDGE_parm ) ){

	rval = ATTR_BORDER_INFO_Deflt( &Destination->leading_edge );
	if( rval == ERROR_INT ){
	    /* ASSERT: TKError got set */		    
	    goto LEAVE;
	}

	*destination_mask |= BORDER_LEADING_EDGE_parm;

    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type ATTR_BORDER_INFO_Deflt( Destination )
    OUT BORDER_INFO_type *Destination;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    Destination->is_null = BOOL_false;
    Destination->border_line_width = (INT_type) 20;
    Destination->border_line_type = at_BORDER_solid;
    Destination->border_freespace_width = (INT_type) 0;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}
