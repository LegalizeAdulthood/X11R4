
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr21.c,v 1.3 89/10/31 21:31:53 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr21.c,v $

(c) Copyright 1988 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	attr21.c,v $
 * Revision 1.3  89/10/31  21:31:53  mss
 * fixed placement of casting in STRING_free
 * 
 * Revision 1.2  89/10/31  13:38:17  jr
 * Casting problems.
 * 
 * Revision 1.1  89/09/21  06:20:13  mss
 * Initial revision
 * 
 * Revision 1.10  89/09/12  09:07:03  mss
 * Removed #
 * 
 * Revision 1.9  89/08/02  17:23:45  annm
 * personal name now uses SEQUENCE_BYTE canonicals NOT STRING canonical procedures
 * 
 * Revision 1.8  89/04/21  18:15:29  annm
 * chnages for deleteseq/deletesequencedeeply
 * 
 * Revision 1.7  89/03/16  15:27:41  annm
 * changes for SEQUENCE_REVISION_HISTORY_type and at_SAME_LAYOUT_OBJECT_type
 * 
 * Revision 1.6  89/03/09  15:51:00  annm
 * no change
 * 
 * Revision 1.5  89/02/09  15:07:20  annm
 * minor tweek
 * minor tweek
 * 
 * Revision 1.4  89/02/02  15:08:56  annm
 * tweek to comment
 * 
 * Revision 1.3  89/02/01  18:01:33  annm
 * added canonical procedures for SEQUENCE_RESOURCE_MAP_type
 * 
 * Revision 1.2  89/01/31  19:04:59  annm
 * added the canonical procedures for SEQUENCE_COPYRIGHT_INFO
 * 
 * Revision 1.1  89/01/30  13:14:06  annm
 * Initial revision
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr21.c,v $";
#endif



#define TK_IMPL_ATTR
#define TK_GVARS_ATTR21
#define TK_TKI_TKERR
#define TK_TKI_SEQ
#define TK_TKI_STR
#define TK_IMPL_CONST
#define TK_TKI_MM

#include <rnames.h>
#include <acd.h>
#include <tkerr.h>
#include <seq.h>
#include <str.h>
#include <const.h>
#include <mm.h>
#include <parms.h>

#include <attr0.h>

#undef TK_IMPL_ATTR
#undef TK_GVARS_ATTR21
#undef TK_TKI_TKERR
#undef TK_TKI_SEQ
#undef TK_TKI_STR
#undef TK_IMPL_CONST
#undef TK_TKI_MM

/*
    This file contains the canonical aunion.h operations for the following 
	types:

	SEQUENCE_NAME_ORG_type
	SEQUENCE_COPYRIGHT_INFO_type
	SEQUENCE_RESOURCE_MAP_type

*/



INT_type SEQUENCE_NAME_ORG_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type destination;
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
	    doing a CopySequence: in this case the top level
	    copy sequence code will alloc the top level sequence
	    ( the sequence of name org's),
	    so the copy sequence elements loop must alloc the
	    contained sequence of char's

	If this routine merely called copy sequence elements, then the
	    subordinate sequence of chars would be allocated for a second
	    time as set/get attr has already called 
	    SEQUENCE_NAME_ORG_Alloc which does a deep alloc, 
	    meaning that the contained sequence's of char's have already
	    been alloc'ed.

	So... after the obvious (redundant) check for sequence type, the
	routine loops copying each contained sequence of chars. The
	copy of these contained sequences of chars can use the canonical
	MUMBLE_Copy.

	Whew !!! 
    */

    INT_type rval;

    SEQUENCE_type SourceSequence;
    SEQUENCE_type DestinationSequence;

    NAME_ORG_type *SourceNameOrg;
    NAME_ORG_type *DestinationNameOrg;

    INT_type i;

    INT_type dummy;

    INT_type SlaveRval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    SourceSequence = (SEQUENCE_type) source;

    if( SourceSequence->sequence_tag != SEQUENCE_NAME_ORG_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_NAME_ORG_Copy]: source not a sequence of name org's" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    DestinationSequence = (SEQUENCE_type) destination;

    if( DestinationSequence->sequence_tag != SEQUENCE_NAME_ORG_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_NAME_ORG_Copy]: destination not a sequence of name org 's" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < SourceSequence->length; i++ ){

	SourceNameOrg = (NAME_ORG_type *) 
	    &SourceSequence->sequence_value.name_orgs[i];
	DestinationNameOrg = (NAME_ORG_type *)
	    &DestinationSequence->sequence_value.name_orgs[i];

	SlaveRval = ATTR_CopyNameOrg( SourceNameOrg, DestinationNameOrg );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    goto LEAVE;
	}

    } /* closes for */
    
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



INT_type ATTR_CopyNameOrg( SourceNameOrg, DestinationNameOrg )
    IN NAME_ORG_type *SourceNameOrg;
    OUT NAME_ORG_type *DestinationNameOrg;
{

    INT_type rval;

    INT_type SlaveRval;

    PARM_MASK_type dummy_mask;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    if( SourceNameOrg->name_is_present == BOOL_true ){

	SlaveRval = ATTR_CopyPersonalName( /* source */
		    &SourceNameOrg->name,
		    /* destination */ &DestinationNameOrg->name );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    goto LEAVE;
	}

	DestinationNameOrg->name_is_present = BOOL_true;

    } /* closes if name_is_present */
    else {
	DestinationNameOrg->name_is_present = BOOL_false;
	DestinationNameOrg->name.surname = NULL_SEQUENCE;
	DestinationNameOrg->name.givenname = NULL_SEQUENCE;
	DestinationNameOrg->name.initials = NULL_SEQUENCE;
	DestinationNameOrg->name.title = NULL_SEQUENCE;
    }

    SlaveRval = STRING_Copy( 
		    (POINTER_type )SourceNameOrg->organization,
		    /* source mask */ ALL_PARMS, 
		    /* wanted mask */ ALL_PARMS,
		    /* dest mask */ &dummy_mask,
		    (POINTER_type) DestinationNameOrg->organization );
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



INT_type ATTR_CopyPersonalName( source, destination)
    IN PERSONAL_NAME_type *source;
    OUT PERSONAL_NAME_type *destination;
{

    INT_type rval;

    INT_type SlaveRval;

    PARM_MASK_type dummy_mask;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    dummy_mask = (PARM_MASK_type) 0;

    SlaveRval = SEQUENCE_BYTE_Copy( 
		(POINTER_type) source->surname,
		/* source mask */ ALL_PARMS,
		/* wanted mask */ ALL_PARMS,
		/* dest mask */ &dummy_mask,
		(POINTER_type) destination->surname );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	goto LEAVE;
    }

    SlaveRval = SEQUENCE_BYTE_Copy( 
		(POINTER_type) source->givenname,
		/* source mask */ ALL_PARMS,
		/* wanted mask */ ALL_PARMS,
		/* dest mask */ &dummy_mask,
		(POINTER_type) destination->givenname );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	goto LEAVE;
    }

    SlaveRval = SEQUENCE_BYTE_Copy( 
		(POINTER_type) source->initials,
		/* source mask */ ALL_PARMS,
		/* wanted mask */ ALL_PARMS,
		/* dest mask */ &dummy_mask,
		(POINTER_type) destination->initials );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	goto LEAVE; 
    }

    SlaveRval = SEQUENCE_BYTE_Copy( 
		(POINTER_type) source->title,
		/* source mask */ ALL_PARMS,
		/* wanted mask */ ALL_PARMS,
		/* dest mask */ &dummy_mask,
		(POINTER_type) destination->title );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	goto LEAVE;
    }

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
	    Leaveing();
	}
#endif

    return( rval );

}




POINTER_type SEQUENCE_NAME_ORG_Alloc(value)
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

    if( TheSource->sequence_tag != SEQUENCE_NAME_ORG_tag ){
	rval = ERROR_POINTER;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[SEQUENCE_NAME_ORG_Alloc]: value isn't SEQUENCE_NAME_ORG_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheSequence = MakeSequence( SEQUENCE_NAME_ORG_tag,
		    TheSource->length );
    if( TheSequence == ERROR_SEQUENCE ){
	rval = ERROR_POINTER;
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < TheSource->length; i++ ){

	SlaveRval = ATTR_AllocNameOrg( 
	    (NAME_ORG_type *)
	    &TheSource->sequence_value.name_orgs[i],

	    (NAME_ORG_type *)
	    &TheSequence->sequence_value.name_orgs[i] 
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



INT_type ATTR_AllocNameOrg( Source, Destination )
    IN NAME_ORG_type *Source;
    OUT NAME_ORG_type *Destination;
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

    if( Source->name_is_present == BOOL_true ){

	SlaveRval = ATTR_AllocPersonalName( &Source->name,
			&Destination->name );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    goto LEAVE;
	}

	Destination->name_is_present = BOOL_true;

    }
    else {
	Destination->name_is_present = BOOL_false;
    }

    Destination->organization = (STRING_type )
		STRING_Alloc( (POINTER_type) Source->organization );
    if( (POINTER_type) Destination->organization == ERROR_POINTER ){
	rval = ERROR_INT;
	if( Destination->name_is_present == BOOL_true ){
	    Destination->name_is_present = BOOL_false;
	    dummy = SEQUENCE_BYTE_Free( (POINTER_type) Destination->name.surname );
	    dummy = SEQUENCE_BYTE_Free( (POINTER_type) Destination->name.givenname );
	    dummy = SEQUENCE_BYTE_Free( (POINTER_type) Destination->name.initials );
	    dummy = SEQUENCE_BYTE_Free( (POINTER_type) Destination->name.title );
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



INT_type ATTR_AllocPersonalName( source, destination )
    IN PERSONAL_NAME_type *source;
    OUT PERSONAL_NAME_type *destination;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    destination->surname = 
	(SEQUENCE_BYTE_type) SEQUENCE_BYTE_Alloc( (POINTER_type) source->surname );
   if( (POINTER_type) destination->surname == ERROR_POINTER ){
	rval = ERROR_INT;
	goto LEAVE;
    }

    destination->givenname = 
	(SEQUENCE_BYTE_type) SEQUENCE_BYTE_Alloc( (POINTER_type) source->givenname );
    if( (POINTER_type) destination->givenname == ERROR_POINTER ){
        rval = ERROR_INT;
	dummy = SEQUENCE_BYTE_Free( (POINTER_type) destination->surname );
	goto LEAVE;
    }

    destination->initials = 
	(SEQUENCE_BYTE_type) SEQUENCE_BYTE_Alloc( (POINTER_type) source->initials );
    if( (POINTER_type) destination->initials == ERROR_POINTER ){
	rval = ERROR_INT;
	dummy = SEQUENCE_BYTE_Free( (POINTER_type) destination->surname );
	dummy = SEQUENCE_BYTE_Free( (POINTER_type) destination->givenname );
	goto LEAVE;
    }

    destination->title = 
	(SEQUENCE_BYTE_type) SEQUENCE_BYTE_Alloc( (POINTER_type) source->title );
    if( (POINTER_type) destination->title == ERROR_POINTER ){
	rval = ERROR_INT;
	dummy = SEQUENCE_BYTE_Free( (POINTER_type) destination->surname );
	dummy = SEQUENCE_BYTE_Free( (POINTER_type) destination->givenname );
	dummy = SEQUENCE_BYTE_Free( (POINTER_type) destination->initials );
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



INT_type SEQUENCE_NAME_ORG_Free(storage)
	IN POINTER_type storage;
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



INT_type ATTR_FreeNameOrg( TheNameOrg )
    IN NAME_ORG_type *TheNameOrg;
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

    if( TheNameOrg->name_is_present == BOOL_true ){
	SlaveRval = ATTR_FreePersonalName( &TheNameOrg->name );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeNameOrg]: can not free the name" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    SlaveRval = STRING_Free((POINTER_type) TheNameOrg->organization);
    if (SlaveRval == ERROR_INT) {
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_FreeNameOrg]: can not free the organization" );
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



INT_type ATTR_FreePersonalName( ThePersonalName )
    IN PERSONAL_NAME_type *ThePersonalName;
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

    SlaveRval = DeleteSequence( ThePersonalName->surname );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_FreePersonalName]: can not free the surname" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( ThePersonalName->givenname != NULL_SEQUENCE ){
	SlaveRval = DeleteSequence( ThePersonalName->givenname );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreePersonalName]:  can not free the given name" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }	

    if( ThePersonalName->initials != NULL_SEQUENCE ){
	SlaveRval = DeleteSequence( ThePersonalName->initials );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreePersonalName]:  can not free the initials" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    if( ThePersonalName->title != NULL_SEQUENCE ){
	SlaveRval = DeleteSequence( ThePersonalName->title );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreePersonalName]: can not free the title " );
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



INT_type SEQUENCE_NAME_ORG_Set(AttrStructPtr, TheStorage)
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

    AttrStructPtr->attribute_value.SEQUENCE_NAME_ORG_field =
	(SEQUENCE_NAME_ORG_type) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type SEQUENCE_NAME_ORG_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_SEQUENCE_NAME_ORG( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_NAME_ORG_CkVal(constituent, attribute, value, mask)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN POINTER_type value;
	IN PARM_MASK_type mask;
{

    /*
	Attributes with type SEQUENCE_NAME_ORG_type:
	    dp_PREPARERS
	    dp_OWNERS
	    dp_AUTHORS
	    dp_DISTRIBUTION_LIST
    */


    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    SEQUENCE_type TheSequence;

    INT_type dummy;

    INT_type i;

    NAME_ORG_type *TheNameOrg;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;
    
    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ) {
    
    case dp_PREPARERS:
    case dp_OWNERS:
    case dp_AUTHORS:
    case dp_DISTRIBUTION_LIST:
	if( !( CONST_IsDocProf( TheConstituent ) ) ) {
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format( 
		"[SEQUENCE_NAME_ORG_CkVal]: ");
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
	dummy = TKERR_Format( "[SEQUENCE_NAME_ORG_CkVal]: Attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " doesn't have SEQUENCE_NAME_ORG_type" );
	dummy = TKERR_EndErrMsg();	
	goto LEAVE;
    }

    TheSequence = (SEQUENCE_type) value;

    if( TheSequence->sequence_tag != SEQUENCE_NAME_ORG_tag ) {
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_NAME_ORG_CkVal]: " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " value must be a sequence of NAME_ORG_type's" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }   

    for( i = (INT_type) 0; i < TheSequence->length; i++ ){

	TheNameOrg = &TheSequence->sequence_value.name_orgs[i];

	/* make sure that if the name is present then so is the surname */
	if( TheNameOrg->name_is_present == BOOL_true &&
	    ( TheNameOrg->name.surname == NULL_SEQUENCE ||
	    TheNameOrg->name.surname == ERROR_SEQUENCE ||
	    TheNameOrg->name.surname == (SEQUENCE_BYTE_type) 0 )
	){
	    rval = ERROR_INT;
	    TKError = BADPARM_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SEQUENCE_NAME_ORG_CkVal]: " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " the surname must be specified when the name is present" );
	    dummy = TKERR_FormatInt(" - this error occurred in element %d", i );
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



INT_type SEQUENCE_NAME_ORG_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN PARM_MASK_type wanted_mask;
	IN PARM_MASK_type *destination_mask;
	IN POINTER_type *destination;
{

    /*
	Attributes with type SEQUENCE_NAME_ORG_type:
	    dp_PREPARERS
	    dp_OWNERS
	    dp_AUTHORS
	    dp_DISTRIBUTION_LIST
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
    dummy = TKERR_Format("[SEQUENCE_NAME_ORG_Deflt]: can not obtain iso 8613 default" );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_COPYRIGHT_INFO_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type destination;
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
	    ( the sequence of copyright info's),
	    so the copy sequence elements loop must alloc the
	    contained sequence of strings

	If this routine merely called copy sequence elements, then the
	    subordinate sequence of strings would be allocated for a second
	    time as set/get attr has already called 
	    SEQUENCE_COPYRIGHT_INFO_Alloc which does a deep alloc, 
	    meaning that the contained sequence's of strings have already
	    been alloc'ed.

	So... after the obvious (redundant) check for sequence type, the
	routine loops copying each contained sequence of strings. The
	copy of these contained sequences of strings can use canonical
	copy for sequence of string.

	Whew !!! 
    */


    INT_type rval;

    SEQUENCE_type SourceSequence;
    SEQUENCE_type DestinationSequence;
    
    INT_type i;

    INT_type dummy;

    INT_type SlaveRval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    SourceSequence = (SEQUENCE_type) source;

    if( SourceSequence == NULL_SEQUENCE ){
	*destination_mask = source_mask;
	goto LEAVE;
    }

    if( SourceSequence->sequence_tag != SEQUENCE_COPYRIGHT_INFO_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_COPYRIGHT_INFO_Copy]: source not a sequence of copyright info's" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    DestinationSequence = (SEQUENCE_type) destination;

    if( DestinationSequence->sequence_tag != SEQUENCE_COPYRIGHT_INFO_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_NAME_ORG_Copy]: destination not a sequence of copyright info 's" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( SourceSequence->length != DestinationSequence->length ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_COPYRIGHT_INFO_Copy]: source and destination lengths don't match" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < SourceSequence->length; i++ ){

	SlaveRval = ATTR_CopyCopyRight( 
	    &SourceSequence->sequence_value.copyright_infos[i],
	    &DestinationSequence->sequence_value.copyright_infos[i] );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    goto LEAVE;
	}

    } /* closes for */
    
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



INT_type ATTR_CopyCopyRight( Source, Destination )
    IN COPYRIGHT_INFO_type *Source;
    OUT COPYRIGHT_INFO_type *Destination;
{

    INT_type rval;

    SEQUENCE_type SourceCopyright;
    SEQUENCE_type DestinationCopyright;

    INT_type SlaveRval;

    PARM_MASK_type dummy_mask;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    SourceCopyright = Source->copyright_information;

    DestinationCopyright = Destination->copyright_information;

    SlaveRval = SEQUENCE_STRING_Copy( 
		    (POINTER_type) SourceCopyright,
		    /* source mask */ (PARM_MASK_type) 0,
		    /* wanted mask */ (PARM_MASK_type) 0,
		    /* dest mask */ &dummy_mask,
		    (POINTER_type) DestinationCopyright );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	goto LEAVE;
    }

    SourceCopyright = Source->copyright_dates;

    DestinationCopyright = Destination->copyright_dates;

    SlaveRval = SEQUENCE_STRING_Copy( 
		    (POINTER_type) SourceCopyright,
		    /* source mask */ (PARM_MASK_type) 0,
		    /* wanted mask */ (PARM_MASK_type) 0,
		    /* dest mask */ &dummy_mask,
		    (POINTER_type) DestinationCopyright );
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



POINTER_type SEQUENCE_COPYRIGHT_INFO_Alloc(value)
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

    if( TheSource == NULL_SEQUENCE ){
	rval = value;
	goto LEAVE;
    }

    if( TheSource->sequence_tag != SEQUENCE_COPYRIGHT_INFO_tag ){
	rval = ERROR_POINTER;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[SEQUENCE_COPYRIGHT_INFO_Alloc]: value isn't SEQUENCE_COPYRIGHT_INFO_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheSequence = MakeSequence( SEQUENCE_COPYRIGHT_INFO_tag,
		    TheSource->length );
    if( TheSequence == ERROR_SEQUENCE ){
	rval = ERROR_POINTER;
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < TheSource->length; i++ ){

	SlaveRval = ATTR_AllocCopyright( 
	    (COPYRIGHT_INFO_type *)
	    &TheSource->sequence_value.copyright_infos[i],

	    (COPYRIGHT_INFO_type *)
	    &TheSequence->sequence_value.copyright_infos[i] 
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



INT_type ATTR_AllocCopyright( source, destination )
    IN COPYRIGHT_INFO_type *source;
    OUT COPYRIGHT_INFO_type *destination;
{

    INT_type rval;

    POINTER_type Ptr;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    Ptr = SEQUENCE_STRING_Alloc( 
	    (POINTER_type) source->copyright_information );
    if( Ptr == ERROR_POINTER ){
	rval = ERROR_INT;
	goto LEAVE;
    }
    destination->copyright_information = (SEQUENCE_type) Ptr;

    Ptr = SEQUENCE_STRING_Alloc( 
	    (POINTER_type) source->copyright_dates );
    if( Ptr == ERROR_POINTER ){
	rval = ERROR_INT;
	goto LEAVE;
    }
    destination->copyright_dates = (SEQUENCE_type) Ptr;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



INT_type SEQUENCE_COPYRIGHT_INFO_Free(storage)
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




INT_type ATTR_FreeCopyrightInfo( storage )
    IN COPYRIGHT_INFO_type *storage;
{

    INT_type rval;

    INT_type dummy;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    if( storage->copyright_information != NULL_SEQUENCE ){
	rval = SEQUENCE_STRING_Free((POINTER_type) storage->copyright_information);
	if( rval == ERROR_INT ){
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[ATTR_FreeCopyrightInfo]: can not free the copyright information" );
	    dummy = TKERR_EndErrMsg();
	    goto LEAVE;
	}
    }

    if (storage->copyright_dates != NULL_SEQUENCE) {
	rval = SEQUENCE_STRING_Free((POINTER_type) storage->copyright_dates);
	if( rval == ERROR_INT ){
	    dummy = TKERR_StartErrMsg();
	    dummy =
    TKERR_Format("[ATTR_FreeCopyrightInfo]: can not free the copyright dates" );
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



INT_type SEQUENCE_COPYRIGHT_INFO_Set(AttrStructPtr, TheStorage)
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

    AttrStructPtr->attribute_value.SEQUENCE_COPYRIGHT_INFO_field =
	(SEQUENCE_COPYRIGHT_INFO_type) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type SEQUENCE_COPYRIGHT_INFO_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_SEQUENCE_COPYRIGHT_INFO( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_COPYRIGHT_INFO_CkVal(constituent, attribute, value, mask)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN POINTER_type value;
	IN PARM_MASK_type mask;
{

    /* 
	Attributes with type SEQUENCE_COPYRIGHT_INFO_type:
	    dp_COPYRIGHT
    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    INT_type dummy;

    SEQUENCE_type TheSequence;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){

    case dp_COPYRIGHT:
	if( !( CONST_IsDocProf( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SEQUENCE_COPYRIGHT_INFO_type]: attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on the document profile" );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_COPYRIGHT_INFO_CkVal]: attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" does not have type SEQUENCE_COPYRIGHT_INFO_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheSequence = (SEQUENCE_type) value;

    if( TheSequence->sequence_tag != SEQUENCE_COPYRIGHT_INFO_tag ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[SEQUENCE_COPYRIGHT_INFO_CkVal]: attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " must have a value that is a sequence of copyright info's" );
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



INT_type SEQUENCE_COPYRIGHT_INFO_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type *destination;
{

    /* 
	Attributes with type SEQUENCE_COPYRIGHT_INFO_type:
	    dp_COPYRIGHT
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
    dummy = TKERR_Format("[SEQUENCE_COPYRIGHT_INFO_Deflt]: can not get iso 8613 default" );
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_RESOURCE_MAP_Copy(source, source_mask, wanted_mask,
    destination_mask, destination )
	IN POINTER_type source;
	IN PARM_MASK_type source_mask;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type destination;
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
	    ( the sequence of resource maps),
	    so the copy sequence elements loop must alloc the
	    contained sequence of char's

	If this routine merely called copy sequence elements, then the
	    subordinate sequence of chars would be allocated for a second
	    time as set/get attr has already called 
	    SEQUENCE_RESOURCE_MAP_Alloc which does a deep alloc, 
	    meaning that the contained sequence of char's have already
	    been alloc'ed.

	So... after the obvious (redundant) check for sequence type, the
	routine loops copying each contained sequence of chars. The
	copy of these contained sequences of chars uses the canonical
	STRING_Copy.

	Whew !!! 
    */


    INT_type rval;

    SEQUENCE_type SourceSequence;
    SEQUENCE_type DestinationSequence;
    
    INT_type i;

    INT_type dummy;

    INT_type SlaveRval;


#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    SourceSequence = (SEQUENCE_type) source;

    if( SourceSequence == NULL_SEQUENCE ){
	*destination_mask = source_mask;
	goto LEAVE;
    }

    if( SourceSequence->sequence_tag != SEQUENCE_RESOURCE_MAP_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_RESOURCE_MAP_Copy]: source not a sequence of resource maps" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    DestinationSequence = (SEQUENCE_type) destination;

    if( DestinationSequence->sequence_tag != SEQUENCE_RESOURCE_MAP_tag ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_RESOURCE_MAP_Copy]: destination not a sequence of resource maps" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    if( SourceSequence->length != DestinationSequence->length ){
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_RESOURCE_MAP_Copy]: source and destination lengths don't match" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < SourceSequence->length; i++ ){

	SlaveRval = ATTR_CopyResourceMap( 
	    &SourceSequence->sequence_value.resource_maps[i],
	    &DestinationSequence->sequence_value.resource_maps[i] );
	if( SlaveRval == ERROR_INT ){
	    rval = ERROR_INT;
	    goto LEAVE;
	}

    } /* closes for */
    
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



INT_type ATTR_CopyResourceMap( Source, Destination )
    IN RESOURCE_MAP_type *Source;
    OUT RESOURCE_MAP_type *Destination;
{

    INT_type rval;

    INT_type SlaveRval;

    PARM_MASK_type dummy_mask;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    SlaveRval = STRING_Copy( (POINTER_type) Source->name,
		    /* source mask */ (PARM_MASK_type) 0,
		    /* wanted mask */ (PARM_MASK_type) 0,
		    /* dest mask */ &dummy_mask,
		    (POINTER_type) Destination->name );
    if( SlaveRval == ERROR_INT ){
	rval = ERROR_INT;
	goto LEAVE;
    }

    Destination->id = Source->id;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );

}



POINTER_type SEQUENCE_RESOURCE_MAP_Alloc(value)
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

    if( TheSource == NULL_SEQUENCE ){
	rval = value;
	goto LEAVE;
    }

    if( TheSource->sequence_tag != SEQUENCE_RESOURCE_MAP_tag ){
	rval = ERROR_POINTER;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[SEQUENCE_RESOURCE_MAP_Alloc]: value isn't SEQUENCE_RESOURCE_MAP_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheSequence = MakeSequence( SEQUENCE_RESOURCE_MAP_tag,
		    TheSource->length );
    if( TheSequence == ERROR_SEQUENCE ){
	rval = ERROR_POINTER;
	goto LEAVE;
    }

    for( i = (INT_type) 0; i < TheSource->length; i++ ){

	SlaveRval = ATTR_AllocResourceMap( 
	    (RESOURCE_MAP_type *)
	    &TheSource->sequence_value.resource_maps[i],

	    (RESOURCE_MAP_type *)
	    &TheSequence->sequence_value.resource_maps[i]
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



INT_type ATTR_AllocResourceMap( Source, Destination )
    IN RESOURCE_MAP_type *Source;
    OUT RESOURCE_MAP_type *Destination;
{

    INT_type rval;

    POINTER_type Ptr;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    Ptr = STRING_Alloc( 
	    (POINTER_type) Source->name );
    if( Ptr == ERROR_POINTER ){
	rval = ERROR_INT;
	goto LEAVE;
    }
    Destination->name = (STRING_type) Ptr;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_RESOURCE_MAP_Free(storage)
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



INT_type ATTR_FreeResourceMap( TheMap )
    IN RESOURCE_MAP_type *TheMap;
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

    SlaveRval = STRING_Free((POINTER_type)(TheMap->name));
    if (SlaveRval == ERROR_INT) {
	rval = ERROR_INT;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[ATTR_FreeResourceMap]: can not free the name" );
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



INT_type SEQUENCE_RESOURCE_MAP_Set(AttrStructPtr, TheStorage)
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
    AttrStructPtr->attribute_value.SEQUENCE_RESOURCE_MAP_field =
	(SEQUENCE_RESOURCE_MAP_type) TheStorage;

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



POINTER_type SEQUENCE_RESOURCE_MAP_Get(AttrStructPtr)
	IN ATTR_attribute_t AttrStructPtr;
{

    POINTER_type rval;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = (POINTER_type) GET_SEQUENCE_RESOURCE_MAP( AttrStructPtr );

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}



INT_type SEQUENCE_RESOURCE_MAP_CkVal(constituent, attribute, value, mask)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN POINTER_type value;
	IN PARM_MASK_type mask;
{

    /*
	Attriubtes with type SEQUENCE_RESOURCE_MAP_type:
	    dp_RESOURCES
    */

    INT_type rval;

    INTERNAL_CONSTITUENT TheConstituent;

    INT_type dummy;

    SEQUENCE_type TheSequence;

#ifdef DEBUG
	if( TraceRoutine() ){
		Entering();
	}
#endif

    rval = SUCCESS_INT;

    TheConstituent = (INTERNAL_CONSTITUENT) constituent;

    switch( attribute ){

    case dp_RESOURCES:
	if( !( CONST_IsDocProf( TheConstituent ) ) ){
	    rval = ERROR_INT;
	    TKError = BADATTR_err;
	    dummy = TKERR_StartErrMsg();
	    dummy = TKERR_Format("[SEQUENCE_RESOURCE_MAP_type]: attribute " );
	    dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	    dummy = TKERR_Format( " must go on the document profile" );
	    dummy = TKERR_EndErrMsg();
	}
	break;

    default:
	rval = ERROR_INT;
	TKError = INTERNAL_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format("[SEQUENCE_RESOURCE_MAP_CkVal]: attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format(" does not have type SEQUENCE_RESOURCE_MAP_type" );
	dummy = TKERR_EndErrMsg();
	goto LEAVE;
    }

    TheSequence = (SEQUENCE_type) value;

    if( TheSequence->sequence_tag != SEQUENCE_RESOURCE_MAP_tag ){
	rval = ERROR_INT;
	TKError = BADPARM_err;
	dummy = TKERR_StartErrMsg();
	dummy = TKERR_Format( "[SEQUENCE_RESOURCE_MAP_CkVal]: attribute " );
	dummy = TKERR_Format( ATTR_AttrNameTbl[attribute].attrid_name );
	dummy = TKERR_Format( " must have a value that is a sequence of resourcemaps" );
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



INT_type SEQUENCE_RESOURCE_MAP_Deflt(constituent, attribute, wanted_mask, destination_mask, destination)
	IN CONSTITUENT constituent;
	IN INT_type attribute;
	IN PARM_MASK_type wanted_mask;
	IN OUT PARM_MASK_type *destination_mask;
	OUT POINTER_type *destination;
{

    /*
	Attriubtes with type SEQUENCE_RESOURCE_MAP_type:
	    dp_RESOURCES
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
    dummy = TKERR_Format("[SEQUENCE_RESOURCE_MAP_Deflt]: can not get iso 8613 default");
    dummy = TKERR_EndErrMsg();

LEAVE:
#ifdef DEBUG
	if( TraceRoutine() ){
		Leaving();
	}
#endif

    return( rval );
}
