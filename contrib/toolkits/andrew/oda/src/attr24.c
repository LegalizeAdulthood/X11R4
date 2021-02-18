
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr24.c,v 1.2 89/10/31 13:49:58 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr24.c,v $

(c) Copyright 1988 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	attr24.c,v $
 * Revision 1.2  89/10/31  13:49:58  jr
 * Casting problems & others.
 * 
 * Revision 1.1  89/09/21  06:21:11  mss
 * Initial revision
 * 
 * Revision 1.3  89/09/12  09:08:19  mss
 * Removed #
 * 
 * Revision 1.2  89/04/21  18:16:28  annm
 * chnages for deleteseq/deletesequencedeeply
 * 
 * Revision 1.1  89/03/16  15:31:44  annm
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr24.c,v $";
#endif



#define TK_IMPL_ATTR
#define TK_GVARS_ATTR24
#define TK_TKI_TKERR
#define TK_TKI_SEQ
#define TK_IMPL_CONST
#define TK_TKI_MM

#include <rnames.h>
#include <acd.h>
#include <tkerr.h>
#include <seq.h>
#include <str.h>
#include <const.h>
#include <mm.h>

#include <attr0.h>
#include <parms.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR24
#undef TK_TKI_TKERR
#undef TK_TKI_SEQ
#undef TK_IMPL_CONST
#undef TK_TKI_MM

/*

    This file cntains the canonical aunion.h operations for the following 
	types:

	    SEQUENCE_REVISION_HISTORY_type

    This file also contains alloc/copy support beeded by CopySequenceElements
	for:

	SEQUENCE_REVISERS_type
	SEQUENCE_PERSONAL_NAME_type

*/



INT_type SEQUENCE_REVISION_HISTORY_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	POINTER_type source;
	PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	POINTER_type destination;
{

    INT_type rval;

    SEQUENCE_REVISION_HISTORY_type SourceSequence;
    SEQUENCE_REVISION_HISTORY_type DestinationSequence;

    INT_type i;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    SourceSequence = (SEQUENCE_REVISION_HISTORY_type) source;
    
    if( SourceSequence->sequence_tag != SEQUENCE_REVISION_HISTORY_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_REVISION_HISTORY_Copy]: source does not have SEQUENCE_REVISION_HISTORY_tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    DestinationSequence = (SEQUENCE_REVISION_HISTORY_type) destination;

    if( DestinationSequence->sequence_tag != SEQUENCE_REVISION_HISTORY_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_SEQUENCE_REIVSION_HISTORY_Copy]: destination does not have sequence revision history tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < SourceSequence->length; i++ ){

	SlaveRval = ATTR_CopyRevisionHistory( 
		    /* original */ 
		    &SourceSequence->sequence_value.revision_histories[i],
		    /* new */ 
		    &DestinationSequence->sequence_value.revision_histories[i] );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SEQUENCE_REVISION_HISTORY_Copy]: The above error occurred in copying sequence element " );
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



INT_type ATTR_CopyRevisionHistory( original, new )
    IN REVISION_HISTORY_type *original;
    OUT REVISION_HISTORY_type *new;
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

    dummy_mask = (PARM_MASK_type) 0;

    SlaveRval = STRING_Copy( 
	    /* source */ (POINTER_type) original->revision_date_and_time,
	    /* source mask*/ ALL_PARMS,
	    /* wanted mask */ ALL_PARMS,
	    /* dest mask */ &dummy_mask,
	    /* destination */ (POINTER_type) new->revision_date_and_time );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyRevisionHistory]: The above error occurred in copying the revision date and time" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    SlaveRval = STRING_Copy( 
	    /* source */ (POINTER_type) original->version_number,
	    /* source maks */ ALL_PARMS,
	    /* wanted mask */ ALL_PARMS,
	    /* dest mask */ &dummy_mask,
	    /* destination */ (POINTER_type) new->version_number );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyRevisionHistory]: The above error occurred in copying the version number" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    SlaveRval = ATTR_CopySequenceRevisers( 
		/* source */ original->revisers,
		/* destination */ new->revisers );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyRevisionHistory]:tThe above error occurred in copying the revisers");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    SlaveRval = STRING_OR_ID_Copy(
		/* source */ (POINTER_type) &original->version_reference,
		/* source mask */ ALL_PARMS,
		/* wanted mask */ ALL_PARMS,
		/* dest mask */ &dummy_mask,
		/* destination */ (POINTER_type) &new->version_reference );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyRevisionHistory]: can not copy the version reference" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    SlaveRval = SEQUENCE_BYTE_Copy( 
		/* source */ (POINTER_type) original->user_comments,
		/* source mask */ ALL_PARMS,
		/* wanted mask */ ALL_PARMS,
		/* dest mask */ &dummy_mask,
		/* destination */ (POINTER_type) new->user_comments );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyRevisionHistory]: The above error occurred in copying the user comments" );
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



INT_type ATTR_CopySequenceRevisers( OriginalSequence, NewSequence )
    IN SEQUENCE_REVISER_type OriginalSequence;
    OUT SEQUENCE_REVISER_type NewSequence;
{

    INT_type rval;

    INT_type i;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    if( OriginalSequence == NULL_SEQUENCE ){
	goto LEAVE;
    }

    if( OriginalSequence->sequence_tag != SEQUENCE_REVISER_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopySequenceRevisers]: The original does not have SEQUENCE_REVISERS_tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( NewSequence->sequence_tag != SEQUENCE_REVISER_tag ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopySequenceRevisers]: destination does not have SEQUENCE_REVISER_tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;	
    }

    for( i = (INT_type) 0; i < OriginalSequence->length; i++ ){

	SlaveRval = ATTR_CopyReviser(
		    /* original */
		    &OriginalSequence->sequence_value.revisers[i],
		    /* new */
		    &NewSequence->sequence_value.revisers[i] );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CopySequenceRevisers]: The above error occurred in copying reviser " );
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



INT_type ATTR_CopyReviser( original, new )
    IN REVISER_type *original;
    OUT REVISER_type *new;
{
    INT_type rval;

    INT_type dummy;

    INT_type SlaveRval;

    PARM_MASK_type dummy_mask;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    dummy_mask = (PARM_MASK_type) 0;

    SlaveRval = ATTR_CopySequencePersonalNames( 
		    original->names, new->names );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyReviser]: The above error occurred in copying the names" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    
    SlaveRval = SEQUENCE_BYTE_Copy( 
		/* source */ (POINTER_type) original->position,
		/* source mask */ ALL_PARMS,
		/* wanted mask */ ALL_PARMS,
		/* dest mask */ &dummy_mask,
		/* destination */ (POINTER_type) new->position );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyReviser]: The above error occurred in copying the position" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    SlaveRval = SEQUENCE_BYTE_Copy( 
		/* source */ (POINTER_type) original->organization,
		/* source mask */ ALL_PARMS,
		/* wanted mask */ ALL_PARMS,
		/* dest mask */ &dummy_mask,
		/* destinatin */ (POINTER_type) new->organization );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopyReviser]: The above error occurred in copying the organization" );
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



INT_type  ATTR_CopySequencePersonalNames( OriginalSequence, NewSequence )
    IN SEQUENCE_PERSONAL_NAME_type OriginalSequence;
    OUT SEQUENCE_PERSONAL_NAME_type NewSequence;
{

    INT_type rval;

    INT_type SlaveRval;

    INT_type i;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    if( OriginalSequence == NULL_SEQUENCE ){
	goto LEAVE;
    }

    if( OriginalSequence->sequence_tag != SEQUENCE_PERSONAL_NAME_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopySequencePersonalNames]: The orginal does not have SEQUENCE_PERSONAL_NAME_tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( NewSequence->sequence_tag != SEQUENCE_PERSONAL_NAME_tag ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_CopySequencePersonalNames]: The new does not have SEQUENCE_PERSONAL_NAME_tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < OriginalSequence->length; i++ ){

	SlaveRval = ATTR_CopyPersonalName(
		    /* original */
		    &OriginalSequence->sequence_value.personal_names[i],
		    /* new */
		    &NewSequence->sequence_value.personal_names[i] );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_CopySequencePersonalNames]: Can not copy personal name " );
	    dummy = TKERR_FormatInt("%d", i );
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




POINTER_type SEQUENCE_REVISION_HISTORY_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    SEQUENCE_REVISION_HISTORY_type OriginalSequence;
    SEQUENCE_REVISION_HISTORY_type NewSequence;

    INT_type i;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    OriginalSequence = (SEQUENCE_REVISION_HISTORY_type) value;
    
    if( OriginalSequence->sequence_tag != SEQUENCE_REVISION_HISTORY_tag ){
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_REVISION_HISTORY_Alloc]: value does not have SEQUENCE_REVISION_HISTORY_tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    NewSequence = MakeSequence( SEQUENCE_REVISION_HISTORY_tag,
		    OriginalSequence->length );

    if( NewSequence == ERROR_SEQUENCE ){
	rval = ERROR_POINTER;
	goto LEAVE;
    }

    rval = (POINTER_type) NewSequence;

    for( i = (INT_type) 0; i < OriginalSequence->length; i++ ){

	SlaveRval = ATTR_AllocRevisionHistory( 
		    /* original */ 
		    &OriginalSequence->sequence_value.revision_histories[i],
		    /* new */ 
		    &NewSequence->sequence_value.revision_histories[i] );
	if( SlaveRval == ERROR_INT ){
	    /* CORE LEAK HERE */
	    rval = ERROR_POINTER;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SEQUENCE_REVISION_HISTORY_Alloc]: The above error occurred in allocating sequence element " );
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



INT_type ATTR_AllocRevisionHistory( original, new )
    IN REVISION_HISTORY_type *original;
    OUT REVISION_HISTORY_type *new;
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

    ptr = STRING_Alloc( (POINTER_type) original->revision_date_and_time );
    if( ptr == ERROR_POINTER ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocRevisionHistory]: The above error occurred in allocating the revision date and time" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    new->revision_date_and_time = (STRING_type) ptr;

    ptr = STRING_Alloc( (POINTER_type) original->version_number );
    if( ptr == ERROR_POINTER ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocRevisionHistory]: The above error occurred in allocating the version number" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    new->version_number = (STRING_type) ptr;

    ptr = ATTR_AllocSequenceRevisers( (POINTER_type) original->revisers );
    if( ptr == ERROR_POINTER ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocRevisionHistory]:tThe above error occurred in allocating the revisers");
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    new->revisers = (SEQUENCE_REVISER_type) ptr;

    SlaveRval = ATTR_Alloc_String_or_id_value( &original->version_reference,
		    &new->version_reference );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocRevisionHistory]: can not allocate the verson reference" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;	
    }

    ptr = SEQUENCE_BYTE_Alloc( (POINTER_type) original->user_comments );
    if( ptr == ERROR_POINTER ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocRevisionHistory]: The above error occurred in allocating the user comments" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    new->user_comments = (SEQUENCE_REVISER_type) ptr;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



POINTER_type ATTR_AllocSequenceRevisers( original )
    IN POINTER_type original;
{

    POINTER_type rval;

    SEQUENCE_REVISER_type OriginalSequence;
    SEQUENCE_REVISER_type NewSequence;

    INT_type i;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    OriginalSequence = (SEQUENCE_REVISER_type) original;

    if( OriginalSequence == NULL_SEQUENCE ){
	rval = (POINTER_type) NULL_SEQUENCE;
	goto LEAVE;
    }

    if( OriginalSequence->sequence_tag != SEQUENCE_REVISER_tag ){
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocSequenceRevisers]: The original does not have SEQUENCE_REVISERS_tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    NewSequence = MakeSequence( SEQUENCE_REVISER_tag,
		    OriginalSequence->length );
    if( NewSequence == ERROR_SEQUENCE ){
	rval = ERROR_POINTER;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocSequenceRevisers]: The above error occurred in attempt to make the sequence of revisers" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    rval = (POINTER_type) NewSequence;

    for( i = (INT_type) 0; i < OriginalSequence->length; i++ ){

	SlaveRval = ATTR_AllocReviser(
		    /* original */
		    &OriginalSequence->sequence_value.revisers[i],
		    /* new */
		    &NewSequence->sequence_value.revisers[i] );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_POINTER;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocSequenceRevisers]: The above error occurred in allocating reviser " );
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



INT_type ATTR_AllocReviser( original, new )
    IN REVISER_type *original;
    OUT REVISER_type *new;
{
    INT_type rval;

    POINTER_type ptr;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    ptr = ATTR_AllocSequencePersonalNames( (POINTER_type) original->names );
    if( ptr == ERROR_POINTER ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocReviser]: The above error occurred in allocating the names" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    new->names = (SEQUENCE_PERSONAL_NAME_type) ptr;
    
    ptr = SEQUENCE_BYTE_Alloc( (POINTER_type) original->position );
    if( ptr == ERROR_POINTER ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocReviser]: The above error occurred in allocating the position" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    new->position = (SEQUENCE_BYTE_type) ptr;

    ptr = SEQUENCE_BYTE_Alloc( (POINTER_type) original->organization );
    if( ptr == ERROR_POINTER ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocReviser]: The above error occurred in allocating the organization" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }
    new->organization = (SEQUENCE_BYTE_type) ptr;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



POINTER_type  ATTR_AllocSequencePersonalNames( original )
    IN POINTER_type original;
{

    POINTER_type rval;

    SEQUENCE_PERSONAL_NAME_type OriginalSequence;
    SEQUENCE_PERSONAL_NAME_type NewSequence;

    INT_type SlaveRval;

    INT_type i;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    OriginalSequence = (SEQUENCE_PERSONAL_NAME_type) original;

    if( OriginalSequence == NULL_SEQUENCE ){
	rval = (POINTER_type) NULL_SEQUENCE;
	goto LEAVE;
    }

    if( OriginalSequence->sequence_tag != SEQUENCE_PERSONAL_NAME_tag ){
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocSequencePersonalNames]: The orginal does not have SEQUENCE_PERSONAL_NAME_tag" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    NewSequence = MakeSequence( SEQUENCE_PERSONAL_NAME_tag,
		    OriginalSequence->length );
    if( NewSequence == ERROR_SEQUENCE ){
	rval = ERROR_POINTER;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_AllocSequencePersonalNames]: The above error occurred during MakeSequence for the personal names" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    rval = (POINTER_type) NewSequence;

    for( i = (INT_type) 0; i < OriginalSequence->length; i++ ){

	SlaveRval = ATTR_AllocPersonalName(
		    /* original */
		    &OriginalSequence->sequence_value.personal_names[i],
		    /* new */
		    &NewSequence->sequence_value.personal_names[i] );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_POINTER;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_AllocSequencePersonalNames]: Can not allocate personal name " );
	    dummy = TKERR_FormatInt("%d", i );
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



INT_type SEQUENCE_REVISION_HISTORY_Free(storage)
	POINTER_type storage;
{

    INT_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = DeleteSequenceDeeply((SEQUENCE_type) storage);

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type ATTR_FreeRevisionHistory( TheRevisionHistory )
    IN REVISION_HISTORY_type *TheRevisionHistory;
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

    if( TheRevisionHistory->revision_date_and_time != NULL_STRING ){
	SlaveRval = STRING_Free((POINTER_type) TheRevisionHistory->revision_date_and_time);
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
    	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeRevisionHistory]:  can not free the revision date and time" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    if( TheRevisionHistory->version_number != NULL_STRING ){
	SlaveRval = STRING_Free((POINTER_type)  TheRevisionHistory->version_number);
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeRevisionHistory]:  can not free the version number" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    if( TheRevisionHistory->revisers != NULL_SEQUENCE ){
	SlaveRval = DeleteSequenceDeeply((SEQUENCE_type) TheRevisionHistory->revisers);
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeRevisionHistory]:  can not free the revisers" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    switch( TheRevisionHistory->version_reference.string_or_id_tag ){

    case STRING_tag:
	SlaveRval = STRING_Free(
    (POINTER_type) TheRevisionHistory->version_reference.string_or_id_value.string);
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeRevsionHistory]:  can not free the version reference string" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    case ID_tag:
	SlaveRval = SEQUENCE_INT_Free(
    (POINTER_type) TheRevisionHistory->version_reference.string_or_id_value.id);
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeRevisionHistory]:  can not free the version reference id" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	break;

    default:
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_FreeRevisionHistory]: bad string or id tag seen in the version reference - got " );
	dummy = TKERR_FormatInt( "%d", (INT_type) 
	    TheRevisionHistory->version_reference.string_or_id_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;

    }

    if( TheRevisionHistory->user_comments != NULL_SEQUENCE ){
	SlaveRval =
	    SEQUENCE_BYTE_Free((POINTER_type) TheRevisionHistory->user_comments);
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy =
    TKERR_Format("[ATTR_FreeRevisionHistory]: can not free the user comments" );
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
    


INT_type ATTR_FreeReviser( TheReviser )
    IN REVISER_type *TheReviser;
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

    if( TheReviser->names != NULL_SEQUENCE ){
	SlaveRval = DeleteSequenceDeeply((SEQUENCE_type) TheReviser->names);
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeReviser]:  can not free the names" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    if( TheReviser->position != NULL_SEQUENCE ){
	SlaveRval = DeleteSequence( TheReviser->position );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeReviser]:  can not free the position" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    if( TheReviser->organization != NULL_SEQUENCE ){
	SlaveRval = DeleteSequence( TheReviser->organization );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeReviser]: can not free the organization " );
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



INT_type SEQUENCE_REVISION_HISTORY_Set(AttrStructPtr, TheStorage)
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

    AttrStructPtr->attribute_value.SEQUENCE_REVISION_HISTORY_field =
	(SEQUENCE_REVISION_HISTORY_type) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type SEQUENCE_REVISION_HISTORY_Get(AttrStructPtr)
	ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_SEQUENCE_REVISION_HISTORY( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_REVISION_HISTORY_CkVal(constituent, attribute, value, mask)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN POINTER_type value;
	IN PARM_MASK_type mask;
{ 

    /*
	Attributes with SEQUENCE_REVISION_HISTORY:
	
	    dp_REVISION_HISTORY
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

    case dp_REVISION_HISTORY:
	if( !( CONST_IsDocProf( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SEQUENCE_REVISION_HISTORY_CkVal]: Attribute ");
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
	dummy = TKERR_Format("[SEQUENCE_REVISION_HISTORY_CkVal]: Atribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" does not have type SEQUENCE_REVISION_HISTORY_type");
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



INT_type SEQUENCE_REVISION_HISTORY_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
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
    dummy = TKERR_Format("[SEQUENCE_REVISION_HISTORY_Deflt]: there is no iso 8613 default for sequences of revision history" );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type at_SAME_LAYOUT_OBJECT_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type destination;
{

    INT_type rval;

    at_SAME_LAYOUT_OBJECT_type *Source;
    at_SAME_LAYOUT_OBJECT_type *Destination;

    INT_type SlaveRval;

    PARM_MASK_type dummy_mask;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    Source = (at_SAME_LAYOUT_OBJECT_type *) source;
    Destination = (at_SAME_LAYOUT_OBJECT_type *) destination;

    Destination->parm1 = Source->parm1;

    dummy_mask = (PARM_MASK_type) 0;

    SlaveRval = LAYOUT_OBJECT_Copy( 
		    /* source */ (POINTER_type) &Source->parm2,
		    /* source mask */ ALL_PARMS,
    		    /* wanted mask */ ALL_PARMS,
		    /* dest mask */ &dummy_mask,
		    /* destination */ (POINTER_type) &Destination->parm2 );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[at_SAME_LAYOUT_OBJECT_Copy]: can not copy the parm2 layout object type" );
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



POINTER_type at_SAME_LAYOUT_OBJECT_Alloc(value)
	IN POINTER_type value;
{

    POINTER_type rval;

    POINTER_type StrPtr;

    at_SAME_LAYOUT_OBJECT_type *OriginalSLO;
    at_SAME_LAYOUT_OBJECT_type *NewSLO;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = MM_Malloc( (INT_type) sizeof( at_SAME_LAYOUT_OBJECT_type ) );
    if( rval == ERROR_POINTER ){
	rval = ERROR_POINTER;
	TKError = NOMEM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[at_SAME_LAYOUT_OBJECT_Alloc]: Can not get memory for the at_SAME_LAYOUT_OBJECT_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    OriginalSLO = (at_SAME_LAYOUT_OBJECT_type *) value;
    NewSLO = (at_SAME_LAYOUT_OBJECT_type *) rval;

    switch( OriginalSLO->parm2.layout_object_tag ){

    case LAYOUT_OBJ_DESC_tag:
	break;

    case LAYOUT_CATEGORY_tag:
	StrPtr = STRING_Alloc( (POINTER_type) 
	    OriginalSLO->parm2.layout_object_value.layout_category );
	if( StrPtr == ERROR_POINTER ){
	    rval = ERROR_POINTER;
	    dummy = TKERR_StartErrMsg();   
	    dummy = TKERR_Format("[at_SAME_LAYOUT_OBJECT_Alloc]: Can not get space for the parm2 layout category string" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	NewSLO->parm2.layout_object_value.layout_category = 
	    (STRING_type) StrPtr;
	break;

    case OBJECT_TYPE_PAGE_tag:
	break;

    case NULL_tag:
	break;

    default:
	rval = ERROR_POINTER;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[at_SAME_LAYOUT_OBJECT_Alloc]: bad layout object tag of " );
	dummy = TKERR_FormatInt("%d", (INT_type) OriginalSLO->parm2.layout_object_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }    
    
    NewSLO->parm2.layout_object_tag = OriginalSLO->parm2.layout_object_tag;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type at_SAME_LAYOUT_OBJECT_Free(storage)
	IN POINTER_type storage;
{

    INT_type rval;

    at_SAME_LAYOUT_OBJECT_type *TheSLO;

    INT_type SlaveRval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheSLO = (at_SAME_LAYOUT_OBJECT_type *) storage;

    switch( TheSLO->parm2.layout_object_tag ){

    case LAYOUT_OBJ_DESC_tag:
	break;

    case LAYOUT_CATEGORY_tag:
	SlaveRval = STRING_Free( (POINTER_type) 
	    TheSLO->parm2.layout_object_value.layout_category );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();   
	    dummy = TKERR_Format("[at_SAME_LAYOUT_OBJECT_Free]: Can not free the parm2 layout category string" );
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
	dummy = TKERR_Format("[at_SAME_LAYOUT_OBJECT_Free]: bad layout object tag of " );
	dummy = TKERR_FormatInt("%d", (INT_type) TheSLO->parm2.layout_object_tag );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }    

    dummy = MM_Free( storage );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type at_SAME_LAYOUT_OBJECT_Set(AttrStructPtr, TheStorage)
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

    AttrStructPtr->attribute_value.at_SAME_LAYOUT_OBJECT_field =
	(at_SAME_LAYOUT_OBJECT_type *) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type at_SAME_LAYOUT_OBJECT_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_at_SAME_LAYOUT_OBJECT( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type at_SAME_LAYOUT_OBJECT_CkVal(constituent, attribute, value, mask)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN POINTER_type value;
	IN PARM_MASK_type mask;
{

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    at_SAME_LAYOUT_OBJECT_type *TheSLO;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){

    case at_SAME_LAYOUT_OBJECT:
	if( !( CONST_IsLayoutStyle( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[at_SAME_LAYOUT_OBJECT_CkVal]: Attribute" );
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
	dummy = TKERR_Format("[at_SAME_LAYOUT_OBJECT_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " does not have type at_SAME_LAYOUT_OBJECT_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheSLO = (at_SAME_LAYOUT_OBJECT_type *) value;

    if( TheSLO->parm2.layout_object_tag == NULL_tag ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[at_SAME_LAYOUT_OBJECT_Alloc]: The parm2 layout object tag must NOT be NULL_tag" );
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



INT_type at_SAME_LAYOUT_OBJECT_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type *destination;
{

    /*
	Attributes with type at_SAME_LAYOUT_OBJECT_type:

	    at_SAME_LAYOUT_OBJECT
    */

    INT_type rval;

    INT_type dummy;

    at_SAME_LAYOUT_OBJECT_type *TheSLO;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    switch( attribute ){

    case at_SAME_LAYOUT_OBJECT:
	if( *destination == (POINTER_type) 0){
	    *destination = MM_Malloc((INT_type) sizeof(at_SAME_LAYOUT_OBJECT_type));
	    if( *destination == ERROR_POINTER ){
		rval = ERROR_INT;
		TKError = NOMEM_err;
		dummy = TKERR_StartErrMsg();
		dummy = TKERR_Format( 
		    "[at_SAME_LAYOUT_OBJECT_Deftl]: Can't get memory for an at_SAME_LAYOUT_OBJECT_type" );
		dummy = TKERR_EndErrMsg();
		goto LEAVE;
	    }
	}
	else {
	    rval = ERROR_INT;
	    TKError = INTERNAL_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[at_SAME_LAYOUT_OBJECT_Deflt}: Already have memory and shouldn't" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
	TheSLO = (at_SAME_LAYOUT_OBJECT_type *) *destination;
	TheSLO->parm1 = NULL_CONSTITUENT;
	*destination_mask = wanted_mask;
	break;


    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[INT_Deflt]: " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " does not have at_SAME_LAYOUT_OBJECT_type" );
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
